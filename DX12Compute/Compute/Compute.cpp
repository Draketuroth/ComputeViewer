
#include "pch.h"

#include "Compute/Compute.h"
#include "Utilities/DXHelper.h"
#include "Utilities/d3dx12.h"

#include <d3dcompiler.h>

#include <iostream>
#include <sstream>
#include <algorithm>


Compute::Compute() :
    texturePixelSize(0),
    textureWidth(0),
    textureHeight(0),
    dispatchX(0),
    dispatchY(0),
    dispatchZ(0),
    imageData(),
    shaderPath()
{
    graphicContext = new GraphicContext();
}

Compute::~Compute()
{
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
}

void Compute::SetDispatchSize(int x, int y, int z)
{
    dispatchX = x;
    dispatchY = y;
    dispatchZ = z;
}

void  Compute::Dispatch()
{
    HRESULT hr = S_OK;
    ID3D12Device* device = graphicContext->getDevicePtr();;
    ID3D12CommandQueue* queue = graphicContext->getCommandQueuePtr();

    // The compute shader expects 2 floats, the source texture and the destination texture.
    CD3DX12_DESCRIPTOR_RANGE srvCbvRanges[2];
    srvCbvRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
    srvCbvRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0);

    CD3DX12_ROOT_PARAMETER rootParameters[3];
    rootParameters[0].InitAsConstants(2, 0);
    rootParameters[1].InitAsDescriptorTable(1, &srvCbvRanges[0]);
    rootParameters[2].InitAsDescriptorTable(1, &srvCbvRanges[1]);

    // Static sampler used to get the linearly interpolated color for the texture.
    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
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

    // Create the root signature for the computer shader from the parameters and sampler above.
    ID3DBlob* signature = nullptr;
    ID3DBlob* error = nullptr;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(rootParameters), rootParameters, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
    ID3D12RootSignature* rootSignature;
    device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    // Create the descriptor heap with layout: source texture - destination texture
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 2;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ID3D12DescriptorHeap* descriptorHeap;
    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap));
    UINT descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    ID3DBlob* shaderBlob;
    hr = D3DCompileFromFile(
        shaderPath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main", "cs_5_1",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &shaderBlob,
        nullptr);

    // Create the pipeline state object for the compute shader using the root signature.
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature;
    psoDesc.CS.pShaderBytecode = shaderBlob->GetBufferPointer();
    psoDesc.CS.BytecodeLength = shaderBlob->GetBufferSize();
    ID3D12PipelineState* pipelineState;
    device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

    // Prepare the shader resource view description for the source texture.
    D3D12_SHADER_RESOURCE_VIEW_DESC srcTextureSRVDesc = {};
    srcTextureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srcTextureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

    // Prepare the unordered access view description for the destination texture.
    D3D12_UNORDERED_ACCESS_VIEW_DESC destTextureUAVDesc = {};
    destTextureUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

    // Get a new empty command list in recording state
    ID3D12CommandAllocator* commandAllocator;
    hr = device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&commandAllocator));

    ID3D12GraphicsCommandList* graphicsCommandList;
    hr = device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        commandAllocator,
        nullptr,
        IID_PPV_ARGS(&graphicsCommandList));

    // Set root signature, pso and descriptor heap
    graphicsCommandList->SetComputeRootSignature(rootSignature);
    graphicsCommandList->SetPipelineState(pipelineState);
    graphicsCommandList->SetDescriptorHeaps(1, &descriptorHeap);

    ID3D12Resource* textureUploadHeap;

    // Texture data
    const unsigned int textureSize = textureWidth * textureHeight * texturePixelSize;
    std::vector<char> colors;
    colors.resize(textureSize);
    for (unsigned int i = 0; i < textureSize; i++)
        colors[i] = 0;

    DXGI_FORMAT imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    D3D12_RESOURCE_DESC textureResourceDescription = {};
    textureResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureResourceDescription.Width = textureWidth;
    textureResourceDescription.Height = textureHeight;
    textureResourceDescription.DepthOrArraySize = 1;
    textureResourceDescription.MipLevels = 1;
    textureResourceDescription.Format = imageFormat;
    textureResourceDescription.SampleDesc.Count = 1;
    textureResourceDescription.SampleDesc.Quality = 0;
    textureResourceDescription.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_HEAP_PROPERTIES textureResourceHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

    ID3D12Resource* textureResource;
    ThrowIfFailed(device->CreateCommittedResource(
        &textureResourceHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &textureResourceDescription,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&textureResource)));

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource, 0, 1);

    CD3DX12_HEAP_PROPERTIES uploadBufferHeapProperties(D3D12_HEAP_TYPE_UPLOAD);

    CD3DX12_RESOURCE_DESC uploadBufferResourceDescription = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    // Create the GPU upload buffer.
    ThrowIfFailed(device->CreateCommittedResource(
        &uploadBufferHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uploadBufferResourceDescription,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUploadHeap)));

    // Copy data to the intermediate upload heap and then schedule a copy 
    // from the upload heap to the Texture2D.

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = colors.data();
    textureData.RowPitch = textureWidth * texturePixelSize;
    textureData.SlicePitch = textureData.RowPitch * textureHeight;

    UpdateSubresources(graphicsCommandList, textureResource, textureUploadHeap, 0, 0, 1, &textureData);

    // CPU handle for the first descriptor on the descriptor heap, used to fill the heap
    CD3DX12_CPU_DESCRIPTOR_HANDLE currentCPUHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 0, descriptorSize);

    // GPU handle for the first descriptor on the descriptor heap, used to initialize the descriptor tables.
    CD3DX12_GPU_DESCRIPTOR_HANDLE currentGPUHandle(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, descriptorSize);

    //Transition from pixel shader resource to unordered access
    CD3DX12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(textureResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    graphicsCommandList->ResourceBarrier(1, &transitionBarrier);

    // Create shader resource view for the source texture in the descriptor heap
    srcTextureSRVDesc.Format = imageFormat;
    srcTextureSRVDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(textureResource, &srcTextureSRVDesc, currentCPUHandle);
    currentCPUHandle.Offset(1, descriptorSize);

    // Create unordered access view for the destination texture in the descriptor heap
    destTextureUAVDesc.Format = imageFormat;
    device->CreateUnorderedAccessView(textureResource, nullptr, &destTextureUAVDesc, currentCPUHandle);
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
    transitionBarrier = CD3DX12_RESOURCE_BARRIER::UAV(textureResource);
    graphicsCommandList->ResourceBarrier(1, &transitionBarrier);

    UINT64 sizeInBytes;
    UINT64 rowSizeInBytes;
    UINT numRows;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;

    device->GetCopyableFootprints(&textureResourceDescription, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &sizeInBytes);

    auto readbackResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeInBytes);
    auto readbackHeapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

    ID3D12Resource* pReadbackResource;
    device->CreateCommittedResource(&readbackHeapDesc, D3D12_HEAP_FLAG_NONE, &readbackResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pReadbackResource));

    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(textureResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
    graphicsCommandList->ResourceBarrier(1, &barrier);

    D3D12_TEXTURE_COPY_LOCATION dst = CD3DX12_TEXTURE_COPY_LOCATION(pReadbackResource, footprint);
    D3D12_TEXTURE_COPY_LOCATION src = CD3DX12_TEXTURE_COPY_LOCATION(textureResource, 0);
    graphicsCommandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    graphicsCommandList->Close();

    ID3D12CommandList* com[] = { graphicsCommandList };

    queue->ExecuteCommandLists(1, com);

    graphicContext->Wait();

    void* readBackImageData;
    pReadbackResource->Map(0, nullptr, &readBackImageData);

    imageData.resize(textureSize);
    std::copy((unsigned char*)readBackImageData, (unsigned char*)readBackImageData + sizeInBytes, imageData.begin());
}
