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

    unsigned char* GetImageData() { return imageData.data(); }

    void Dispatch();

private:
    int texturePixelSize;
    int textureWidth;
    int textureHeight;

    int dispatchX;
    int dispatchY;
    int dispatchZ;

    std::vector<unsigned char> imageData;
    std::wstring shaderPath;
    GraphicContext* graphicContext;
};

#endif