#pragma once

#include "d3d11.h"
#include <string>

// Creating Buffer Resources(Direct3D 10)
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205130(v=vs.85).aspx
// 
HRESULT CreateVertexBuffer(ID3D11Device* pDevice, void* data, int ByteWidth, ID3D11Buffer** pVertexBuffer);
HRESULT CreateIndexBuffer(ID3D11Device* pDevice, void* data, int ByteWidth, ID3D11Buffer** pIndexBuffer);
HRESULT CreateConstantBuffer(ID3D11Device* pDevice, int ByteWidth, ID3D11Buffer** pConstantBuffer);

template <typename T>
HRESULT UpdateConstantBuffer(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pConstantBuffer, const T& data);

#include "DxUtils.inc"


HRESULT CreateVertexShader(
    ID3D11Device* pDevice,
    const std::string& filename,
    const std::string& entry,
    const std::string& sm,
    D3D11_INPUT_ELEMENT_DESC* layout,
    UINT                    numElements,
    ID3D11InputLayout** ppInputLayout,
    ID3D11VertexShader** ppVertexShader
);

HRESULT CreatePixelShader(
    ID3D11Device* pDevice,
    const std::string& filename,
    const std::string& entry,
    const std::string& sm,
    ID3D11PixelShader** ppPixelShader
);


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
    BOOL                        bGenerateSRV = false
);


HRESULT CreateSampler(ID3D11Device* pDevice, ID3D11SamplerState** ppSamplerState);