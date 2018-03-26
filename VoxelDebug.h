#pragma once
#include <d3d11.h>
#include "glm/glm.hpp"

class VoxelDebug
{
public:
    VoxelDebug();
    ~VoxelDebug();

    HRESULT Init(ID3D11Device* pDevice, int dim, float voxelScale, glm::vec3 voxelBias);
    void    Unit();

    void    Render(ID3D11DeviceContext* pContext);

private:

    ID3D11InputLayout*  layout_;

    ID3D11Buffer*       vertexBuffer_;

    ID3D11VertexShader*     vshader_;
    ID3D11GeometryShader*   gshader_;
    ID3D11PixelShader*      pshader_;

    int                     vertexCount_;

};

