
#include "pch.h"

#include "GraphicContext/GraphicContext.h"
#include "Utilities/DXHelper.h"

#include <dxgi1_6.h>

GraphicContext::GraphicContext() :
    commandQueue(nullptr),
    device(nullptr),
    fence(nullptr),
    handle(CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS)),
    cnt(0)
{
    HRESULT hr = Initialize();
}

GraphicContext::~GraphicContext()
{
    SAFE_RELEASE(fence);
    SAFE_RELEASE(commandQueue);
    SAFE_RELEASE(device);
}

void GraphicContext::Wait()
{
    // Fence value update.
    ++cnt;

    // Change fence value.
    auto hr = commandQueue->Signal(fence, cnt);
    if (FAILED(hr))
    {

        return;
    }

    // Wait for completion (polling).
    while (fence->GetCompletedValue() != cnt)
    {
        // Set fence events
        hr = fence->SetEventOnCompletion(cnt, handle);
        if (FAILED(hr))
        {

            return;
        }

        // Wait for fence event.
        WaitForSingleObject(handle, INFINITE);
    }
}

HRESULT GraphicContext::Initialize()
{
    printf("Setting up graphic context...\n");

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
            printf("[OK] Enabled debug layer\n");
        }
        else
        {
            printf("[WARNING] Debug layer not successfully enabled!\n");
        }
    }
#endif

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    // Check hardware adapter support.
    Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter);

    // Create the device.
    ThrowIfFailed(D3D12CreateDevice(
        hardwareAdapter.Get(),
        D3D_FEATURE_LEVEL_12_1,
        IID_PPV_ARGS(&device)
    ));

    printf("[OK] Created device\n");

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

    printf("[OK] Created command queue\n");

    // Create fence.
    ThrowIfFailed(device->CreateFence(cnt, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    printf("[OK] Created fence\n");

    return S_OK;
}

void GraphicContext::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** pAdapter, bool requestHighPerformanceAdapter)
{
    *pAdapter = nullptr;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

    Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (UINT adapterIndex = 0;
            DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter));
            ++adapterIndex)
        {
            // Check to see whether the adapter supports Direct3D 12, but don't create
             // the actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }

    }
    else
    {
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Check to see whether the adapter supports Direct3D 12, but don't create
            // the actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }
}