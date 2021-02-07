#ifndef GRAPHICCONTEXT_H
#define GRAPHICCONTEXT_H

#include <d3d12.h>
#include <dxgi.h>

class GraphicContext
{
public:
    GraphicContext();
    ~GraphicContext();

    bool Wait();

    ID3D12CommandQueue* getCommandQueuePtr() const { return commandQueue.Get(); };
    ID3D12Device* getDevicePtr() const { return device.Get(); }
    ID3D12Fence* getFencePtr() const { return fence.Get(); }

private:
    bool Initialize();

    void GetHardwareAdapter(
        IDXGIFactory1* pFactory,
        IDXGIAdapter1** pAdapter,
        bool requestHighPerformanceAdapter = false);

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
    Microsoft::WRL::ComPtr<ID3D12Device6> device;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    void* handle;

    long long counter;
};

#endif