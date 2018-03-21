#pragma once


#include "d3d11.h"
#include "glm/glm.hpp"
#include <string>
#include <vector>

struct MaterialParam
{
    glm::vec4 ka;
    glm::vec4 kd;
    glm::vec4 ks;
};

struct MeshRender
{
    ID3D11Buffer*   vbuffer;
    ID3D11Buffer*   ibuffer;

    MaterialParam   materialParam;
    UINT            indexCount;
};

struct BoundingBox
{
    glm::vec3 vMin;
    glm::vec3 vMax;
};

class DxObjModel
{
public:
    DxObjModel();
    ~DxObjModel();

    HRESULT Init(const std::string& filename, ID3D11Device* pDevice);
    void    Unit();

    void Render(ID3D11DeviceContext* pContext);
    void RenderDepth(ID3D11DeviceContext* pContext);

    const BoundingBox& GetBoundingBox();
private:

    
    
    std::vector<MeshRender>               meshRenders_;
    BoundingBox                           boundingBox_;

    ID3D11Buffer*                   cbPerObject_;
    ID3D11InputLayout*              layout_;

    ID3D11VertexShader*             vshader_;
    ID3D11PixelShader*              pshader_;

    ID3D11RasterizerState*          state;
    ID3D11DepthStencilState*        dsState;
    ID3D11BlendState*               blendState;

    ID3D11VertexShader*             smvshader_;
};

