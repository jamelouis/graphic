#include "VoxelDebug.h"
#include "DxUtils.h"
#include "glm/glm.hpp"
#include <vector>

struct VertexData
{
    glm::vec4 worldPos;
    glm::vec4 texPos;
};

VoxelDebug::VoxelDebug()
{
}


VoxelDebug::~VoxelDebug()
{
}

HRESULT VoxelDebug::Init(ID3D11Device* pDevice, int dim, float voxelScale, glm::vec3 voxelBias)
{
    HRESULT hr = E_FAIL;
    
    float step = 1.0 / dim;
    float invVoxelScale =  1.0 / voxelScale;

    std::vector<VertexData> vertices;
    for (float z = .5f * step; z < 1.0f; z += step)
    {
        for (float y = .5f*step; y < 1.0f; y += step)
        {
            for (float x = .5f*step; x < 1.0f; x += step)
            {
                glm::vec3 texPos(x, y, z);
                glm::vec3 worldPos = texPos * invVoxelScale - voxelBias;

                VertexData vd;
                vd.worldPos = glm::vec4(texPos * invVoxelScale - voxelBias,1.0);
                vd.texPos = glm::vec4(texPos,1.0);
                vertices.push_back(vd);
            }
        }
    }

    vertexCount_ = vertices.size();

    CreateVertexBuffer(pDevice, &vertices[0], sizeof(vertices[0])*vertexCount_, &vertexBuffer_);

    D3D11_INPUT_ELEMENT_DESC layout[2] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    CreateVertexShader(pDevice, "shader/voxeldebug.fx", "vsmain", "vs_5_0", layout, 2, &layout_, &vshader_);
    CreateGeometryShader(pDevice, "shader/voxeldebug.fx", "gsmain", "gs_5_0", &gshader_);
    CreatePixelShader(pDevice, "shader/voxeldebug.fx", "psmain", "ps_5_0", &pshader_);

    return S_OK;
    
}

void    VoxelDebug::Unit()
{
    layout_->Release();
    vertexBuffer_->Release();
    vshader_->Release();
    gshader_->Release();
    pshader_->Release();
}

void    VoxelDebug::Render(ID3D11DeviceContext* pContext)
{
    UINT stride = sizeof(float) * 8;
    UINT offset = 0;

    pContext->IASetInputLayout(layout_);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    pContext->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);

    pContext->VSSetShader(vshader_, NULL, NULL);
    pContext->GSSetShader(gshader_, NULL, NULL);
    pContext->PSSetShader(pshader_, NULL, NULL);

    pContext->Draw(vertexCount_, 0);

    pContext->VSSetShader(NULL, NULL, NULL);
    pContext->GSSetShader(NULL, NULL, NULL);
    pContext->PSSetShader(NULL, NULL, NULL);

}
