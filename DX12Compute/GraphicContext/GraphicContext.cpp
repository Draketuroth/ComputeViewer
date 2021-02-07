
#include "pch.h"

#include "GraphicContext/GraphicContext.h"
#include "Utilities/PrintHelper.h"

#include <dxgi1_6.h>

GraphicContext::GraphicContext() :
    commandQueue(nullptr),
    device(nullptr),
    fence(nullptr),
    handle(CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS)),
    counter(0)
{
    Initialize();
}

GraphicContext::~GraphicContext()
{

}

bool GraphicContext::Wait()
{
    // Fence value update.
    ++counter;

    // Change fence value.
    if (!ReportStatus(commandQueue->Signal(fence.Get(), counter), "Signaling command queue..."))
        return false;

    DWORD timestamp = GetTickCount64();

    // Wait for completion (polling).
    while (fence->GetCompletedValue() != counter)
    {
        // Set fence events
        if (FAILED(fence->SetEventOnCompletion(counter, handle)))
            return false;

        if ((GetTickCount64() - timestamp) > 10000)
        {
            PrintStatus("ERROR", "Time-out for fence event, dispatch was not successful");
            return false;
        }

        // Wait for fence event.
        WaitForSingleObject(handle, INFINITE);
    }
    return true;
}

bool GraphicContext::Initialize()
{
    PrintStatus("INFO", "Setting up graphic context...");

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (!ReportStatus(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)), "Enable debug interface"))
            return false;

        debugController->EnableDebugLayer();

        // Enable additional debug layers.
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    if (!ReportStatus(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)), "Create DXGI factory"))
        return false;
    
    // Check hardware adapter support.
    Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(factory.Get(), &hardwareAdapter);

    // Create the device.
    if (!ReportStatus(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)), "Create Device"))
        return false;

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (!ReportStatus(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)), "Create command queue"))
        return false;

    if (ReportStatus(device->CreateFence(counter, D3D12_FENCE_FLAGS::D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "Create fence"))
        return false;

    return true;
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