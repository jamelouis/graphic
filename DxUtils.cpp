#include "DxUtils.h"
#include "d3d11Shader.h"
#include <D3Dcompiler.h>
#include <fstream>
#include <cassert>


HRESULT CreateBuffer(ID3D11Device* pDevice, void* data, int ByteWidth, UINT BindFlags, ID3D11Buffer** pVertexBuffer);


HRESULT CreateVertexBuffer(ID3D11Device* pDevice, void* data, int ByteWidth, ID3D11Buffer** pVertexBuffer)
{
    HRESULT hr = E_FAIL;

    hr = CreateBuffer(pDevice, data, ByteWidth, D3D11_BIND_VERTEX_BUFFER, pVertexBuffer);

    return hr;
}

HRESULT CreateIndexBuffer(ID3D11Device* pDevice, void* data, int ByteWidth, ID3D11Buffer** pIndexBuffer)
{
    HRESULT hr = E_FAIL;

    hr = CreateBuffer(pDevice, data, ByteWidth, D3D11_BIND_INDEX_BUFFER, pIndexBuffer);

    return hr;
}

HRESULT CreateConstantBuffer(ID3D11Device* pDevice, int ByteWidth, ID3D11Buffer** pConstantBuffer)
{
    HRESULT hr = E_FAIL;

    D3D11_BUFFER_DESC desc;

    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.Usage          = D3D11_USAGE_DYNAMIC;
    desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags      = 0;
    desc.ByteWidth      = ByteWidth;

    hr                  = pDevice->CreateBuffer(&desc, NULL, pConstantBuffer);

    return hr;
}
HRESULT CreateBuffer(ID3D11Device* pDevice, void* data, int ByteWidth, UINT BindFlags, ID3D11Buffer** pBuffer)
{
    HRESULT hr = E_FAIL;

    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DEFAULT;
    Desc.ByteWidth = ByteWidth;
    Desc.BindFlags = BindFlags;
    Desc.CPUAccessFlags = 0;
    Desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA Data;
    Data.pSysMem = data;

    hr = pDevice->CreateBuffer(&Desc, &Data, pBuffer);
    return hr;
}

HRESULT CompileShader(
    const std::string&  filename,
    const std::string&  entry,
    const std::string&  sm,
    ID3DBlob**          ppBlob
);

HRESULT CreateVertexShader(
    ID3D11Device* pDevice,
    const std::string& filename,
    const std::string& entry,
    const std::string& sm,
    D3D11_INPUT_ELEMENT_DESC* layout,
    UINT                    numElements,
    ID3D11InputLayout** ppInputLayout,
    ID3D11VertexShader** ppVertexShader
)
{
    HRESULT hr = E_FAIL;

    ID3DBlob* pBlob;
    hr = CompileShader(filename, entry, sm, &pBlob);
    assert(!FAILED(hr));

    hr = pDevice->CreateInputLayout(layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), ppInputLayout);
    assert(!FAILED(hr));

    hr = pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppVertexShader);
    assert(!FAILED(hr));

    return hr;
}

HRESULT CreatePixelShader(
    ID3D11Device* pDevice,
    const std::string& filename,
    const std::string& entry,
    const std::string& sm,
    ID3D11PixelShader** ppPixelShader
)
{
    HRESULT hr = E_FAIL;

    ID3DBlob* pBlob;
    hr = CompileShader(filename, entry, sm, &pBlob);
    assert(!FAILED(hr));

    hr = pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppPixelShader);
    assert(!FAILED(hr));

    return hr;
}


HRESULT CompileShader(
    const std::string&  filename,
    const std::string&  entry,
    const std::string&  sm,
    ID3DBlob**          ppBlob
)
{
    HRESULT hr = E_FAIL;
    DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
    ID3DBlob* errBlob;

    std::ifstream stream(filename.c_str());
    std::string source(std::istreambuf_iterator<char>(stream), (std::istreambuf_iterator<char>()));

    hr = D3DCompile(source.c_str(), source.size(), filename.c_str(), NULL, NULL, entry.c_str(), sm.c_str(), flags, 0, ppBlob, &errBlob);
    if (FAILED(hr))
    {
        if (errBlob != NULL)
        {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            errBlob->Release();
        }
        assert(0 && "Fail");
    }
    if (errBlob != NULL)
    {
        errBlob->Release();
    }
    return hr;
}


//
//
//
HRESULT CreateDSV(
    ID3D11Device* pDevice,
    int width,
    int height,
    ID3D11DepthStencilView** ppDSV
    )
{
    HRESULT hr = E_FAIL;

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;


    ID3D11Texture2D *pTexture;
    hr = pDevice->CreateTexture2D(&desc, NULL, &pTexture);
    assert(!FAILED(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
    dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsDesc.Flags = 0;
    dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;
    
    hr = pDevice->CreateDepthStencilView(pTexture, &dsDesc, ppDSV);
    assert(!FAILED(hr));

    return hr;
}