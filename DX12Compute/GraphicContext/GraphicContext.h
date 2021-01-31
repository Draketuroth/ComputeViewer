#ifndef GRAPHICCONTEXT_H
#define GRAPHICCONTEXT_H

#include <d3d12.h>
#include <dxgi.h>

class GraphicContext
{
public:
    GraphicContext();
    ~GraphicContext();

    void Wait();

    ID3D12CommandQueue* getCommandQueuePtr() const { return commandQueue; };
    ID3D12Device* getDevicePtr() const { return device; }
    ID3D12Fence* getFencePtr() const { return fence; }

private:
    HRESULT Initialize();

    void GetHardwareAdapter(
        IDXGIFactory1* pFactory,
        IDXGIAdapter1** pAdapter,
        bool requestHighPerformanceAdapter = false);

    ID3D12CommandQueue* commandQueue;
    ID3D12Device6* device;
    ID3D12Fence* fence;
    void* handle;

    long long cnt;
};

#endif