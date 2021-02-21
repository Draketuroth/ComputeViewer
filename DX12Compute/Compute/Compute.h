#ifndef COMPUTE_H
#define COMPUTE_H

#include <d3d12.h>
#include <vector>
#include <string>

#include "GraphicContext/GraphicContext.h"

class Compute
{
public:
    Compute();
    ~Compute();

    void SetShaderPath(const wchar_t* path);
    void SetTextureSize(int width, int height, int pixelSize);
    void SetDispatchSize(int x, int y, int z);

    bool Dispatch();

private:
    bool CreateDescriptorHeap();
    bool CreateRootSignature();
    bool CreateCommandAllocator();
    bool CreateGraphicsList();
    bool CreateSourceTextureResource();
    bool CreateTextureUploadHeap();

    void UpdateSubResource();

    bool ResetPipelineState(const char* shaderEntryPoint, const char* shaderVersion);
    void ResetSamplerDesc();
    void ResetUAVSRVDescriptions();
    void ResetSourceTextureDescription();

    bool ExecuteComputeJob();

    unsigned int textureSize;
    int texturePixelSize;
    int textureWidth;
    int textureHeight;

    int dispatchX;
    int dispatchY;
    int dispatchZ;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    UINT descriptorSize;

    Microsoft::WRL::ComPtr <ID3D12Resource> textureResource;
    Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadHeap;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    DXGI_FORMAT imageFormat;

    D3D12_STATIC_SAMPLER_DESC samplerDesc;
    D3D12_SHADER_RESOURCE_VIEW_DESC srcTextureSRVDesc;
    D3D12_UNORDERED_ACCESS_VIEW_DESC destTextureUAVDesc;
    D3D12_RESOURCE_DESC textureResourceDescription;

    ID3D12Device* device;
    ID3D12CommandQueue* queue;

    HANDLE mapFile;
    LPCSTR sharedMemoryBuffer;


    std::vector<unsigned char> inputColors;

    std::wstring shaderPath;
    GraphicContext* graphicContext;
};

#endif