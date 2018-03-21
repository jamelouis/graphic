#pragma once
#include "glm/glm.hpp"
#include <string>
#include <dxgi.h>
#include <d3d11.h>
#include <D3Dcompiler.h>

class SimpleCamera;
class DxObjModel;

struct Light
{
    glm::vec4 vLightDir;
    glm::vec4 vLightColor;
};

struct CommonBuffer
{
    glm::mat4x4 viewProj;
    Light       light;
};

class D3DRender
{
public:
    D3DRender();
    ~D3DRender();

public:
    HRESULT Init();
    void    Unit();

    void setSimpleCamera(const SimpleCamera* simpleCamera);
    void resize(int width, int height);
    void createDevice(int width, int height, HWND hWnd);
    void createBuffers();
    void releaseBuffers();
    void render();

    ID3D11Device* device_;
    ID3D11DeviceContext* context_;
    IDXGISwapChain* swapChain_;
    ID3D11RenderTargetView* rtView_;
    ID3D11DepthStencilView* rtDSV_;
    DXGI_SWAP_CHAIN_DESC swapChainDesc_;
    ID3D11Texture2D* backBuffer_;
    D3D11_VIEWPORT viewport_;
    IDXGIAdapter* adapter_;

    ID3D11Buffer*    commonBuffer;

    int width_;
    int height_;

    SimpleCamera* simpleCameraPtr_;

    DxObjModel*    dxObjModel_;

    CommonBuffer   cpuCommonBuffer;
};

