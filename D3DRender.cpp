#include "D3DRender.h"
#include "D3D11Shader.h"

#include <vector>
#include <cassert>
#include <sstream>
#include <fstream>

#include <xnamath.h>
#include <algorithm>

#include "SimpleCamera.h"


#include "tinyobj/include/tiny_obj_loader.h"
#include "DxUtils.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "DxObjModel.h"
#include "VoxelDebug.h"


D3DRender::D3DRender()
{
    
}


D3DRender::~D3DRender()
{
    swapChain_->Release();
    device_->Release();
    context_->Release();
}

HRESULT D3DRender::Init(const std::string& filename)
{
    HRESULT hr = E_FAIL;
    dxObjModel_ = new DxObjModel();
    hr = dxObjModel_->Init(filename, device_);
    // hr = dxObjModel_->Init("models/susanne.obj", device_);
    
    assert(!FAILED(hr));

    
    _CalcVoxelInfo(voxelScale_, voxelBias_);

    voxelDebug_ = new VoxelDebug();
    hr = voxelDebug_->Init(device_, dim_, voxelScale_, voxelBias_);
    assert(!FAILED(hr));

    hr = CreateConstantBuffer(device_, sizeof(CommonBuffer), &commonBuffer);
    assert(!FAILED(hr));

    cpuCommonBuffer.light.vLightColor = glm::vec4(1.0, 1.0, 1.0,1.0);
    cpuCommonBuffer.light.vLightDir = glm::vec4(glm::normalize(glm::vec3(0.4, 0.8,0.0)), 1.0);
    cpuCommonBuffer.dim = dim_;

    return hr;
}

void D3DRender::Unit()
{
    commonBuffer->Release();
    releaseBuffers();
    if (dxObjModel_)
    {
        dxObjModel_->Unit();
        delete dxObjModel_;
        dxObjModel_ = nullptr;
    }
}

void D3DRender::setSimpleCamera(const SimpleCamera* simpleCamera)
{
    simpleCameraPtr_ = (SimpleCamera*)simpleCamera;
} 

void D3DRender::resize(int width, int height)
{
    releaseBuffers();
    swapChain_->ResizeBuffers(1, width, height, swapChainDesc_.BufferDesc.Format, 0);
    swapChain_->GetDesc(&swapChainDesc_);
    viewport_.Width = width;
    viewport_.Height = height;
    width_ = width;
    height_ = height;
    createBuffers();
}

void D3DRender::createDevice(int width, int height, HWND hWnd)
{
    HRESULT hr;

    width_ = width;
    height_ = height;

    ZeroMemory(&swapChainDesc_, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc_.BufferCount = 1;
    swapChainDesc_.BufferDesc.Width = width;
    swapChainDesc_.BufferDesc.Height = height;
    swapChainDesc_.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc_.SampleDesc.Count = 1;
    swapChainDesc_.SampleDesc.Quality = 0;
    swapChainDesc_.Windowed = true;
    swapChainDesc_.OutputWindow = hWnd;
    swapChainDesc_.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc_.BufferDesc.RefreshRate.Denominator = 1;

    hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG,
        NULL, NULL, D3D11_SDK_VERSION,
        &swapChainDesc_, &swapChain_, &device_,
        NULL, &context_);
    assert(!FAILED(hr));

    createBuffers();

    hr = CreateSampler(device_, &shadowSampler_);
    assert(!FAILED(hr));

    hr = CreateDefaultSampler(device_, &voxelSampler_);

    hr = CreateDepthAsset(device_, 512, 512, &shadowTex_, &shadowDSV_, &shadowRSV_, true);
    assert(!FAILED(hr));

    for (int i = 0; i < 6; ++i)
    {
        hr = CreateTexture3D(device_, dim_, dim_, dim_, &voxelTexs_[i], &voxelUAVs_[i], &voxelSRVs_[i]);
        assert(!FAILED(hr));
    }
    
}

int indexCount = 0;

void D3DRender::createBuffers() {
    HRESULT hr;

    hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
        (LPVOID*)&backBuffer_);
    assert(!FAILED(hr));

    hr = device_->CreateRenderTargetView(backBuffer_, NULL, &rtView_);
    assert(!FAILED(hr));

    hr = CreateDepthAsset(device_, width_, height_, &mainDepthTex_, &rtDSV_, nullptr, false);
    assert(!FAILED(hr));

    ZeroMemory(&viewport_, sizeof(D3D11_VIEWPORT));
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.Width = width_;
    viewport_.Height = height_;
}

void D3DRender::releaseBuffers() {
    rtView_->Release();
    backBuffer_->Release();

    
}

static float currTime = 0.0f;

void D3DRender::render() 
{
    HRESULT hr;

    renderDepth();
    renderVoxel();

    renderColor();
    swapChain_->Present(0, 0);
}

void D3DRender::renderColor()
{
    HRESULT hr = E_FAIL;
    float clearColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };

    _UpdateCamera();
    //_UpdateVoxelCamera();

    hr = UpdateConstantBuffer(context_, commonBuffer, cpuCommonBuffer);
    assert(!FAILED(hr));

    context_->OMSetRenderTargets(1, &rtView_, rtDSV_);
    context_->RSSetViewports(1, &viewport_);
    context_->ClearRenderTargetView(rtView_, clearColor);
    context_->ClearDepthStencilView(rtDSV_, D3D11_CLEAR_DEPTH, 1.0f, 0);

    context_->VSSetConstantBuffers(0, 1, &commonBuffer);
    context_->PSSetConstantBuffers(0, 1, &commonBuffer);

    context_->PSSetShaderResources(0, 1, &shadowRSV_);
    context_->PSSetSamplers(0, 1, &shadowSampler_);

    dxObjModel_->Render(context_);

    context_->GSSetConstantBuffers(0, 1, &commonBuffer);
    context_->PSSetShaderResources(1, 6, &voxelSRVs_[0]);
    context_->PSSetSamplers(1, 1, &voxelSampler_);

    voxelDebug_->Render(context_);

    ID3D11ShaderResourceView* pnullSRV[7] = { NULL };
    context_->PSSetShaderResources(0, 7, &pnullSRV[0]);

}

void D3DRender::_UpdateCamera()
{
    glm::vec3 vPos, vTarget, vUp;
    vPos.x = std::sin(simpleCameraPtr_->cameraAngleY_)*simpleCameraPtr_->cameraDistance_;
    vPos.y = std::sin(simpleCameraPtr_->cameraAngleX_)*simpleCameraPtr_->cameraDistance_;
    vPos.z = std::cos(simpleCameraPtr_->cameraAngleY_)*simpleCameraPtr_->cameraDistance_;
    vTarget.x = 0.0f, vTarget.y = 0.0f, vTarget.z = 0.0f;
    vUp.x = 0.0f, vUp.y = 1.0f, vUp.z = 0.0f;
    glm::mat4x4 View = glm::lookAt(vPos, vTarget, vUp);
    glm::mat4x4 Proj = glm::perspective(glm::degrees(0.523598775f), (float)width_ / (float)height_,
        1.0f, 10000.0f);

    auto boundingBox = dxObjModel_->GetBoundingBox();
    glm::mat4x4 world = glm::translate(glm::mat4(1.f), -(boundingBox.vMax + boundingBox.vMin) / 2.0f);

    glm::mat4x4 ViewProj = Proj * View * world;

    cpuCommonBuffer.viewProj = glm::transpose(ViewProj);
    cpuCommonBuffer.voxelScale = 1.0f / voxelScale_ / dim_;
}

void D3DRender::renderDepth()
{
    HRESULT hr;

    _UpdateDepthCamera();

    hr = UpdateConstantBuffer(context_, commonBuffer, cpuCommonBuffer);
    assert(!FAILED(hr));

    ID3D11RenderTargetView* pnullView = NULL;
    context_->OMSetRenderTargets(1, &pnullView, shadowDSV_);
    context_->RSSetViewports(1, &shadowViewport_);
    context_->ClearDepthStencilView(shadowDSV_, D3D11_CLEAR_DEPTH, 1.0f, 0);

    context_->VSSetConstantBuffers(0, 1, &commonBuffer);

    dxObjModel_->RenderDepth(context_);

    ID3D11DepthStencilView* pnullDSV = NULL;
    context_->OMSetRenderTargets(1, &pnullView, pnullDSV);
   // swapChain_->Present(0, 0);
}

void D3DRender::_UpdateDepthCamera()
{
    auto boundingBox = dxObjModel_->GetBoundingBox();

    float fDist = std::abs(glm::distance(boundingBox.vMax, boundingBox.vMin))/2.0;

    glm::vec3 vTarget = glm::vec3(0.0f);
    glm::vec3 vPos = vTarget + fDist * glm::vec3(cpuCommonBuffer.light.vLightDir);
    glm::vec3 vUpd = glm::vec3(0.0, 1.0, 0.0);

    glm::mat4 view = glm::lookAt(vPos, vTarget, vUpd);
    glm::mat4 proj = glm::ortho(-fDist, fDist, -fDist, fDist, -2.0f * fDist, 3.0f * fDist);

    glm::mat4x4 world = glm::translate(glm::mat4(1.f), -(boundingBox.vMax + boundingBox.vMin) / 2.0f);

    glm::mat4 worldViewProj = proj * view *world;
    cpuCommonBuffer.viewProj = glm::transpose(worldViewProj);
    cpuCommonBuffer.smviewProj = cpuCommonBuffer.viewProj;

    ZeroMemory(&shadowViewport_, sizeof(D3D11_VIEWPORT));
    shadowViewport_.MinDepth = 0.0f;
    shadowViewport_.MaxDepth = 1.0f;
    shadowViewport_.TopLeftX = 0;
    shadowViewport_.TopLeftY = 0;
    shadowViewport_.Width    = 512;
    shadowViewport_.Height   = 512;
}

void D3DRender::renderVoxel()
{
    HRESULT hr = E_FAIL;

    int i = 0;
    float uav[4] = { 0.0 };
    float clearColor[4] = { 0.3,0.3,0.3,1.0 };

    for (int i = 0; i < 6; ++i)
    {
        _UpdateVoxelCamera(i);

        hr = UpdateConstantBuffer(context_, commonBuffer, cpuCommonBuffer);
        assert(!FAILED(hr));

        context_->OMSetRenderTargetsAndUnorderedAccessViews(1, &rtView_, rtDSV_, 1, 1, &voxelUAVs_[i], NULL);
        context_->ClearRenderTargetView(rtView_, clearColor);
        context_->ClearDepthStencilView(rtDSV_, D3D11_CLEAR_DEPTH, 1.0f, 0);

        context_->ClearUnorderedAccessViewFloat(voxelUAVs_[i], uav);
        context_->RSSetViewports(1, &boxelViewport_);

        context_->VSSetConstantBuffers(0, 1, &commonBuffer);
        context_->PSSetConstantBuffers(0, 1, &commonBuffer);

        context_->PSSetShaderResources(0, 1, &shadowRSV_);
        context_->PSSetSamplers(0, 1, &shadowSampler_);

        dxObjModel_->RenderVoxel(context_);

        ID3D11ShaderResourceView* pnullSRV = NULL;
        context_->PSSetShaderResources(0, 1, &pnullSRV);

        ID3D11UnorderedAccessView* pnullUAV = NULL;
        context_->OMSetRenderTargetsAndUnorderedAccessViews(0, NULL, NULL, 0, 1, &pnullUAV, NULL);
    }
    
}

void D3DRender::_UpdateVoxelCamera(int face)
{
    auto boundingBox = dxObjModel_->GetBoundingBox();

    float fDist = std::abs(glm::distance(boundingBox.vMax, boundingBox.vMin)) / 2.0;
    glm::vec3 t = boundingBox.vMax - boundingBox.vMin;
    float fScale = 1.0f/std::max(t.x, std::max(t.y, t.z));

    glm::vec3 vTarget = glm::vec3(0.0f,-1.0,0.0);
    glm::vec3 vPos = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 vUp = glm::vec3(1.0, 0.0, 0.0);

    switch (face)
    {
    case 0: // +X
        vTarget = glm::vec3(1.0, 0.0, 0.0);
        vUp     = glm::vec3(0.0, 1.0, 0.0);
        break;
    case 1: // -X
        vTarget = glm::vec3(-1.0, 0.0, 0.0);
        vUp     = glm::vec3(0.0, 1.0, 0.0);
        break;
    case 2: // +Y
        vTarget = glm::vec3(0.0, 1.0, 0.0);
        vUp     = glm::vec3(0.0, 0.0, -1.0);
        break;
    case 3: // -Y
        vTarget = glm::vec3(0.0, -1.0, 0.0);
        vUp     = glm::vec3(0.0, 0.0, 1.0);
        break;
    case 4: // +Z
        vTarget = glm::vec3(0.0, 0.0, 1.0);
        vUp     = glm::vec3(0.0, 1.0, 0.0);
        break;
    case 5: // -Z
        vTarget = glm::vec3(0.0, 0.0, -1.0);
        vUp     = glm::vec3(0.0, 1.0, 0.0);
        break;
    default:
        assert(0 != 1);

    }

    glm::mat4 view = glm::lookAt(vPos, vTarget, vUp);
    glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -3.0f, 1.01f);

    glm::mat4x4 world = glm::translate(glm::mat4(1.0), glm::vec3(-1.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(2.0))
        * glm::scale(glm::mat4(1.0), glm::vec3(fScale)) 
        * glm::translate(glm::mat4(1.f), -boundingBox.vMin);
    
    glm::mat4 worldViewProj = proj * view *world;

    cpuCommonBuffer.viewProj = glm::transpose(worldViewProj);
    cpuCommonBuffer.storageTransorm = glm::transpose(world);
    

    ZeroMemory(&boxelViewport_, sizeof(D3D11_VIEWPORT));
    boxelViewport_.MinDepth = 0.0f;
    boxelViewport_.MaxDepth = 1.0f;
    boxelViewport_.TopLeftX = 0;
    boxelViewport_.TopLeftY = 0;
    boxelViewport_.Width = dim_;
    boxelViewport_.Height = dim_;
}

void D3DRender::_CalcVoxelInfo(float& voxelScale, glm::vec3& voxelBias)
{
    auto boundingBox = dxObjModel_->GetBoundingBox();

    float fDist = std::abs(glm::distance(boundingBox.vMax, boundingBox.vMin)) / 2.0;
    glm::vec3 t = boundingBox.vMax - boundingBox.vMin;
    voxelScale = 1.0f / std::max(t.x, std::max(t.y, t.z));

    voxelBias = -boundingBox.vMin;
}