#pragma once

template <typename T>
HRESULT UpdateConstantBuffer(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* pConstantBuffer, const T& data)
{
    HRESULT hr = E_FAIL;
    D3D11_MAPPED_SUBRESOURCE mappedRes;

    hr = pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes);
    if (FAILED(hr))
    {
        return hr;
    }
    
    T* pData = (T*)mappedRes.pData;
    *pData = data;
    
    pDeviceContext->Unmap(pConstantBuffer, 0);

    return hr;
}

