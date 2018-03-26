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

HRESULT CreateGeometryShader(
    ID3D11Device* pDevice,
    const std::string& filename,
    const std::string& entry,
    const std::string& sm,
    ID3D11GeometryShader** ppGeometryShader 
)
{
    HRESULT hr = E_FAIL;

    ID3DBlob* pBlob;
    hr = CompileShader(filename, entry, sm, &pBlob);
    assert(!FAILED(hr));

    hr = pDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppGeometryShader);
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

// Implementing ID3DInclude
// http://nikvoss.com/2013/03/implementing-id3dinclude/
// 
class D3DInclude : public ID3DInclude
{
    HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
    {
        std::string filePath = std::string("shader\\") + pFileName;
        std::ifstream file(filePath.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

        //OutputDebugStringA(filePath.c_str());
        if (file.is_open())
        {
            long long fileSize = file.tellg();
            char* buf = new char[fileSize];
            file.seekg(0, std::ios::beg);
            file.read(buf, fileSize);
            file.close();
            *ppData = buf;
            *pBytes = fileSize;
            //OutputDebugStringA((char*)*ppData);
        }

        //OutputDebugStringA("End of Open");
        
        

        return S_OK;
    };

    HRESULT Close(LPCVOID pData)
    {
        char* buf = (char*)pData;
        delete[] buf;
        return S_OK;
    };
};

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
    D3DInclude Include;
    std::ifstream stream(filename.c_str());
    std::string source(std::istreambuf_iterator<char>(stream), (std::istreambuf_iterator<char>()));

    hr = D3DCompile(source.c_str(), source.size(), filename.c_str(), NULL, &Include, entry.c_str(), sm.c_str(), flags, 0, ppBlob, &errBlob);
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
HRESULT CreateDepthAsset(
    ID3D11Device* pDevice,
    int width,
    int height,
    ID3D11Texture2D**           ppTexture,
    ID3D11DepthStencilView**    ppDSV,
    ID3D11ShaderResourceView**  ppSRV,
    BOOL                        bGenerateSRV
    )
{
    HRESULT hr = E_FAIL;

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    hr = pDevice->CreateTexture2D(&desc, NULL, ppTexture);
    assert(!FAILED(hr));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
    dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsDesc.Flags = 0;
    dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;
    
    hr = pDevice->CreateDepthStencilView(*ppTexture, &dsDesc, ppDSV);
    assert(!FAILED(hr));

    if (bGenerateSRV)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {
            DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
            D3D11_SRV_DIMENSION_TEXTURE2D,
            0,
            0
        };
        desc.Texture2D.MipLevels = 1;
        hr = pDevice->CreateShaderResourceView(*ppTexture, &desc, ppSRV);
        assert(!FAILED(hr));
    }

    return hr;
}


//
//
//
HRESULT CreateSampler(ID3D11Device* pDevice, ID3D11SamplerState** ppSamplerState)
{
    HRESULT hr = E_FAIL;

    D3D11_SAMPLER_DESC desc = 
    {
        D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
        D3D11_TEXTURE_ADDRESS_BORDER,
        D3D11_TEXTURE_ADDRESS_BORDER,
        D3D11_TEXTURE_ADDRESS_BORDER,
        0,
        0,
        D3D11_COMPARISON_LESS,
        0,0,0,0,
        0,
        0
    };

    hr = pDevice->CreateSamplerState(&desc, ppSamplerState);
    assert(!FAILED(hr));

    return hr;
}

HRESULT CreateDefaultSampler(ID3D11Device* pDevice, ID3D11SamplerState** ppSamplerState)
{
    HRESULT hr = E_FAIL;

    D3D11_SAMPLER_DESC desc =
    {
        D3D11_FILTER_MIN_MAG_MIP_POINT,
        D3D11_TEXTURE_ADDRESS_BORDER,
        D3D11_TEXTURE_ADDRESS_BORDER,
        D3D11_TEXTURE_ADDRESS_BORDER,
        0,
        0,
        D3D11_COMPARISON_NEVER,
        0,0,0,0,
        0,
        0
    };

    hr = pDevice->CreateSamplerState(&desc, ppSamplerState);
    assert(!FAILED(hr));

    return hr;
}

HRESULT CreateTexture3D(
    ID3D11Device* pDevice,
    int width,
    int height,
    int depth,
    ID3D11Texture3D** ppTex3D,
    ID3D11UnorderedAccessView** ppUAV,
    ID3D11ShaderResourceView** ppSRV
)
{
    HRESULT hr = E_FAIL;

    D3D11_TEXTURE3D_DESC desc =
    {
        width, height, depth,
        0,
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE,
        0,
        0
    };

    hr = pDevice->CreateTexture3D(&desc, NULL, ppTex3D);
    assert(!FAILED(hr));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = 
    {
        DXGI_FORMAT_R16G16B16A16_FLOAT,
        D3D11_UAV_DIMENSION_TEXTURE3D,
    };

    uavDesc.Texture3D.MipSlice = 0;
    uavDesc.Texture3D.WSize = depth;

    hr = pDevice->CreateUnorderedAccessView(*ppTex3D, &uavDesc, ppUAV);
    assert(!FAILED(hr));

    hr = pDevice->CreateShaderResourceView(*ppTex3D, NULL, ppSRV);
    assert(!FAILED(hr));

    return hr;
    
}