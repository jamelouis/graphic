#pragma once
#include "glm/glm.hpp"
#include <string>
#include <dxgi.h>
#include <d3d11.h>
#include <D3Dcompiler.h>

class SimpleCamera;
class DxObjModel;
class VoxelDebug;

struct Light
{
    glm::vec4 vLightDir;
    glm::vec4 vLightColor;
};

struct CommonBuffer
{
    glm::mat4x4 viewProj;
    Light       light;
    glm::mat4x4 smviewProj;
    glm::mat4x4 storageTransorm;
    float       voxelScale;
    UINT        dim;
    glm::vec2      padding;
};

class D3DRender
{
public:
    D3DRender();
    ~D3DRender();

public:
    HRESULT Init(const std::string& filename);
    void    Unit();

    void setSimpleCamera(const SimpleCamera* simpleCamera);
    void resize(int width, int height);
    void createDevice(int width, int height, HWND hWnd);
    void createBuffers();
    void releaseBuffers();
    void render();
    void renderColor();
    void renderDepth();
    void renderVoxel();
    

private:
    void _UpdateCamera();
    void _UpdateDepthCamera();
    void _UpdateVoxelCamera(int face);

    void _CalcVoxelInfo(float& voxelScale, glm::vec3& voxelBias);
private:

    ID3D11Device* device_;
    ID3D11DeviceContext* context_;
    IDXGISwapChain* swapChain_;
    ID3D11RenderTargetView* rtView_;

    ID3D11Texture2D* mainDepthTex_;
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
    VoxelDebug*    voxelDebug_;

public:
    CommonBuffer   cpuCommonBuffer;


    ID3D11Texture2D*            shadowTex_;
    ID3D11DepthStencilView*     shadowDSV_;
    ID3D11ShaderResourceView*   shadowRSV_;
    ID3D11SamplerState*         shadowSampler_;

    D3D11_VIEWPORT              shadowViewport_;

    ID3D11Texture3D*            voxelTexs_[6];
    ID3D11UnorderedAccessView*  voxelUAVs_[6];
    ID3D11ShaderResourceView*   voxelSRVs_[6];
    ID3D11SamplerState*         voxelSampler_;

    D3D11_VIEWPORT              boxelViewport_;

    static const int            dim_ = 16;
    float                       voxelScale_;
    glm::vec3                   voxelBias_;
};

