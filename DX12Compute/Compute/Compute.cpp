
#include "pch.h"

#include "Compute/Compute.h"
#include "Utilities/PrintHelper.h"
#include "Utilities/d3dx12.h"

#include <d3dcompiler.h>

#include <windows.h>
#include <iostream>
#include <sstream>
#include <algorithm>

#define BUF_SIZE 8294400 // 1920 * 1080 * 4 (Current image limit)

TCHAR mappingObjectName[] = TEXT("Global\\ImageMappingObject");

Compute::Compute() :
    textureSize(0),
    texturePixelSize(0),
    textureWidth(0),
    textureHeight(0),
    dispatchX(0),
    dispatchY(0),
    dispatchZ(0),
    descriptorHeap(nullptr),
    descriptorSize(0),
    textureResource(nullptr),
    textureUploadHeap(nullptr),
    graphicsCommandList(nullptr),
    commandAllocator(nullptr),
    pipelineState(nullptr),
    rootSignature(nullptr),
    imageFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
    samplerDesc(),
    srcTextureSRVDesc(),
    destTextureUAVDesc(),
    textureResourceDescription(),
    device(nullptr),
    queue(nullptr),
    inputColors(),
    shaderPath()
{
    mapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                NULL,
                                PAGE_READWRITE,
                                0,
                                BUF_SIZE,
                                mappingObjectName);

    if (mapFile == NULL)
    {
        PrintStatus("FAIL", "Shared file mapping was NULL!\n");
        exit(0);
    }
    sharedMemoryBuffer = (LPCSTR)MapViewOfFile(mapFile,
                                               FILE_MAP_ALL_ACCESS,
                                               0,
                                               0,
                                               BUF_SIZE);

    if (sharedMemoryBuffer == NULL)
    {
        PrintStatus("FAIL", "Shared memory buffer was NULL!\n");

        CloseHandle(mapFile);

        exit(0);
    }

    graphicContext = new GraphicContext();

    device = graphicContext->GetDevicePtr();
    queue = graphicContext->GetCommandQueuePtr();

    ResetSamplerDesc();
    ResetUAVSRVDescriptions();
    ResetSourceTextureDescription();

    CreateRootSignature();
    CreateDescriptorHeap();

    CreateCommandAllocator();
    CreateGraphicsList();
}

Compute::~Compute()
{
    UnmapViewOfFile(sharedMemoryBuffer);

    CloseHandle(mapFile);

    delete graphicContext;
}

void Compute::SetShaderPath(const wchar_t* path)
{
    shaderPath = path;
}

void Compute::SetTextureSize(int width, int height, int pixelSize)
{
    texturePixelSize = pixelSize;
    textureWidth = width;
    textureHeight = height;

    textureSize = textureWidth * textureHeight * texturePixelSize;

    inputColors.resize(textureSize);
}

void Compute::SetDispatchSize(int x, int y, int z)
{
    dispatchX = x;
    dispatchY = y;
    dispatchZ = z;
}

bool Compute::CreateDescriptorHeap()
{
    // Create the descriptor heap with layout: source texture - destination texture
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 2;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (!ReportStatus(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap)), "Create descriptor heap"))
        return false;

    descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    return true;
}

bool Compute::Dispatch()
{
    HRESULT hr = S_OK;

    if (!ResetPipelineState("main", "cs_5_1"))
        return false;

    // Don't forget to reset closed graphics command list...
    graphicsCommandList->Reset(commandAllocator.Get(), nullptr);

    // Set root signature, pso and descriptor heap.
    graphicsCommandList->SetComputeRootSignature(rootSignature.Get());
    graphicsCommandList->SetPipelineState(pipelineState.Get());
    graphicsCommandList->SetDescriptorHeaps(1, descriptorHeap.GetAddressOf());

    if (!CreateSourceTextureResource())
        return false;

    if (!CreateTextureUploadHeap())
        return false;

    UpdateSubResource();

    if (!ExecuteComputeJob())
        return false;

    return true;
}

bool Compute::CreateRootSignature()
{
    HRESULT hr = S_OK;

    // The compute shader expects 2 floats, the source texture and the destination texture.
    CD3DX12_DESCRIPTOR_RANGE srvCbvRanges[2];
    srvCbvRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
    srvCbvRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0);

    CD3DX12_ROOT_PARAMETER rootParameters[3];
    rootParameters[0].InitAsConstants(2, 0);
    rootParameters[1].InitAsDescriptorTable(1, &srvCbvRanges[0]);
    rootParameters[2].InitAsDescriptorTable(1, &srvCbvRanges[1]);

    // Create the root signature for the computer shader from the parameters and sampler above.
    Microsoft::WRL::ComPtr<ID3DBlob> signature = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> error = nullptr;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(rootParameters), rootParameters, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    if(!ReportStatus(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error), "Create serialized versioned root signature"))
        return false;

    if (!ReportStatus(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)), "Create root signature"))
        return false;

    return true;
}

bool Compute::CreateCommandAllocator()
{
    // Get a new empty command list in recording state
    if (!ReportStatus(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandAllocator)), "Creating command allocator"))
            return false;

    return true;
}

bool Compute::CreateGraphicsList()
{
    if (!ReportStatus(device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&graphicsCommandList)), "Creating graphics command list"))
            return false;

    return true;
}

bool Compute::CreateSourceTextureResource()
{
    // Texture data
    // Provide base image data from GUI...
    CopyMemory(inputColors.data(), (PVOID)sharedMemoryBuffer, textureSize);

    textureResourceDescription.Width = textureWidth;
    textureResourceDescription.Height = textureHeight;

    CD3DX12_HEAP_PROPERTIES textureResourceHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

    if (!ReportStatus(device->CreateCommittedResource(
        &textureResourceHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDescription,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&textureResource)), "Create source texture resource"))
            return false;

    return true;
}

bool Compute::CreateTextureUploadHeap()
{
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource.Get(), 0, 1);
    CD3DX12_HEAP_PROPERTIES uploadBufferHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC uploadBufferResourceDescription = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    // Create the GPU upload buffer.
    if (!ReportStatus(device->CreateCommittedResource(
        &uploadBufferHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uploadBufferResourceDescription,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUploadHeap)), "Create texture upload heap"))
            return false;

    return true;
}

void Compute::UpdateSubResource()
{
    PrintStatus("INFO", "Updating subresource data");

    // Copy data to the intermediate upload heap and then schedule a copy 
    // from the upload heap to the Texture2D.

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = inputColors.data();
    textureData.RowPitch = textureWidth * texturePixelSize;
    textureData.SlicePitch = textureData.RowPitch * textureHeight;

    UpdateSubresources(graphicsCommandList.Get(), textureResource.Get(), textureUploadHeap.Get(), 0, 0, 1, &textureData);
}

bool Compute::ResetPipelineState(const char* shaderEntryPoint, const char* shaderVersion)
{
    Microsoft::WRL::ComPtr <ID3DBlob> shaderBlob;
    if (!ReportStatus(D3DCompileFromFile(
        shaderPath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        shaderEntryPoint, shaderVersion,
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &shaderBlob,
        nullptr), "Compile shader")) 
    {
        //OutputDebugString((WCHAR*)shaderBlob->GetBufferPointer());
        return false;
    }

    // Create the pipeline state object for the compute shader using the root signature.
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.CS.pShaderBytecode = shaderBlob->GetBufferPointer();
    psoDesc.CS.BytecodeLength = shaderBlob->GetBufferSize();

    if (!ReportStatus(device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)), "Create compute pipeline state"))
        return false;

    return true;
}

void Compute::ResetSamplerDesc()
{
    PrintStatus("INFO", "Setting up static sampler description");

    // Static sampler used to get the linearly interpolated color for the texture.

    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    samplerDesc.ShaderRegister = 0;
    samplerDesc.RegisterSpace = 0;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
}

void Compute::ResetUAVSRVDescriptions()
{
    PrintStatus("INFO", "Setting up UAV and SRV descriptions");

    // Prepare the shader resource view description for the source texture.
    srcTextureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srcTextureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srcTextureSRVDesc.Texture2D.MipLevels = 1;
    srcTextureSRVDesc.Format = imageFormat;

    // Prepare the unordered access view description for the destination texture.
    destTextureUAVDesc = {};
    destTextureUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    destTextureUAVDesc.Format = imageFormat;
}

void Compute::ResetSourceTextureDescription()
{
    PrintStatus("INFO", "Setting up source texture description");

    textureResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDescription.Width = 0;
    textureResourceDescription.Height = 0;
    textureResourceDescription.DepthOrArraySize = 1;
    textureResourceDescription.MipLevels = 1;
    textureResourceDescription.Format = imageFormat;
    textureResourceDescription.SampleDesc.Count = 1;
    textureResourceDescription.SampleDesc.Quality = 0;
    textureResourceDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
}

bool Compute::ExecuteComputeJob()
{
    // CPU handle for the first descriptor on the descriptor heap, used to fill the heap
    CD3DX12_CPU_DESCRIPTOR_HANDLE currentCPUHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 0, descriptorSize);

    // GPU handle for the first descriptor on the descriptor heap, used to initialize the descriptor tables.
    CD3DX12_GPU_DESCRIPTOR_HANDLE currentGPUHandle(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, descriptorSize);

    //Transition from pixel shader resource to unordered access
    CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(textureResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    graphicsCommandList->ResourceBarrier(1, &transitionBarrier);

    // Create shader resource view for the source texture in the descriptor heap
    device->CreateShaderResourceView(textureResource.Get(), &srcTextureSRVDesc, currentCPUHandle);
    currentCPUHandle.Offset(1, descriptorSize);

    // Create unordered access view for the destination texture in the descriptor heap
    device->CreateUnorderedAccessView(textureResource.Get(), nullptr, &destTextureUAVDesc, currentCPUHandle);
    currentCPUHandle.Offset(1, descriptorSize);

    //Pass the source and destination texture views to the shader via descriptor tables
    graphicsCommandList->SetComputeRootDescriptorTable(1, currentGPUHandle);
    currentGPUHandle.Offset(1, descriptorSize);
    graphicsCommandList->SetComputeRootDescriptorTable(2, currentGPUHandle);
    currentGPUHandle.Offset(1, descriptorSize);

    //Dispatch the compute shader with one thread per 8x8 pixels
    UINT dispatchGroupsX = (textureWidth + (dispatchX - 1)) / dispatchX;
    UINT dispatchGroupsY = (textureHeight + (dispatchY - 1)) / dispatchY;
    UINT dispatchGroupsZ = 1;
    graphicsCommandList->Dispatch(dispatchGroupsX, dispatchGroupsY, dispatchGroupsZ);

    //Wait for all accesses to the destination texture UAV to be finished.
    transitionBarrier = CD3DX12_RESOURCE_BARRIER::UAV(textureResource.Get());
    graphicsCommandList->ResourceBarrier(1, &transitionBarrier);

    UINT64 sizeInBytes = 0;
    UINT64 rowSizeInBytes = 0;
    UINT numRows = 0;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;

    device->GetCopyableFootprints(&textureResourceDescription, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &sizeInBytes);

    auto readbackResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);
    auto readbackHeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

    Microsoft::WRL::ComPtr<ID3D12Resource> pReadbackResource;
    device->CreateCommittedResource(&readbackHeapDesc, D3D12_HEAP_FLAG_NONE, &readbackResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pReadbackResource));

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(textureResource.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
    graphicsCommandList->ResourceBarrier(1, &barrier);

    D3D12_TEXTURE_COPY_LOCATION dst = CD3DX12_TEXTURE_COPY_LOCATION(pReadbackResource.Get(), footprint);
    D3D12_TEXTURE_COPY_LOCATION src = CD3DX12_TEXTURE_COPY_LOCATION(textureResource.Get(), 0);
    graphicsCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    CD3DX12_RESOURCE_BARRIER::Transition(textureResource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

    graphicsCommandList->Close();

    ID3D12CommandList* com[] = { graphicsCommandList.Get() };

    queue->ExecuteCommandLists(1, com);

    PrintStatus("INFO", "Executing command list...");

    if (!graphicContext->Wait())
        return false;

    PrintStatus("INFO", "Command list finished!");

    void* readBackImageData = nullptr;
    pReadbackResource->Map(0, nullptr, &readBackImageData);

    CopyMemory((PVOID)sharedMemoryBuffer, readBackImageData, textureSize);

    readBackImageData = nullptr;

    return true;
}
