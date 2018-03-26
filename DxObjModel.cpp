#include "DxObjModel.h"
#include "tinyobj/include/tiny_obj_loader.h"
#include "DxUtils.h"
#include <unordered_map>

#define ObjNormal

struct VertexData
{
    glm::vec4 vPosition;
#ifdef ObjNormal
    glm::vec4 vNormal;
#endif
};

struct Mesh
{
    std::vector<VertexData> vertexDatas;
    std::vector<DWORD>       indices;

    MaterialParam           materialParam;
};

bool LoadObj(
    const std::string filename,
    std::vector<Mesh>& meshs,
    BoundingBox&       boundingBox
)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    const std::string basepath = "models/";
    const bool triangulate = true;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), basepath.c_str(), triangulate);

    float minx = attrib.vertices[0];
    float miny = attrib.vertices[1];
    float minz = attrib.vertices[2];
    float maxx = minx;
    float maxy = miny;
    float maxz = minz;

    for (int i = 0; i < attrib.vertices.size() / 3; i++)
    {
        minx = std::min(minx, attrib.vertices[i * 3]);
        maxx = std::max(maxx, attrib.vertices[i * 3]);
        miny = std::min(miny, attrib.vertices[i * 3 + 1]);
        maxy = std::max(maxy, attrib.vertices[i * 3 + 1]);
        minz = std::min(minz, attrib.vertices[i * 3 + 2]);
        maxz = std::max(maxz, attrib.vertices[i * 3 + 2]);
    }

    boundingBox.vMin = glm::vec3(minx, miny, minz);
    boundingBox.vMax = glm::vec3(maxx, maxy, maxz);

    for (auto shape : shapes)
    {
        Mesh mesh;
        for (int i = 0; i < shape.mesh.indices.size(); ++i)
        {
            VertexData vertexData;
            int index_vertices = shape.mesh.indices[i].vertex_index;
            
            int index_normal = shape.mesh.indices[i].normal_index;
            vertexData.vPosition = glm::vec4(
                attrib.vertices[index_vertices * 3],
                attrib.vertices[index_vertices * 3 + 1],
                attrib.vertices[index_vertices * 3 + 2],
                1.0f
            );
#ifdef ObjNormal
            vertexData.vNormal = glm::vec4(
                attrib.normals[index_normal * 3],
                attrib.normals[index_normal * 3 + 1],
                attrib.normals[index_normal * 3 + 2],
                1.0f
            );
#endif
            mesh.indices.push_back(mesh.vertexDatas.size());
            mesh.vertexDatas.push_back(vertexData);
            
        }

        if (materials.size() > 0)
        {
            mesh.materialParam.ka = glm::vec4(
                materials[shape.mesh.material_ids[0]].ambient[0],
                materials[shape.mesh.material_ids[0]].ambient[1],
                materials[shape.mesh.material_ids[0]].ambient[2],
                1.0f
            );
            mesh.materialParam.kd = glm::vec4(
                materials[shape.mesh.material_ids[0]].diffuse[0],
                materials[shape.mesh.material_ids[0]].diffuse[1],
                materials[shape.mesh.material_ids[0]].diffuse[2],
                1.0f
            );
            mesh.materialParam.ks = glm::vec4(
                materials[shape.mesh.material_ids[0]].specular[0],
                materials[shape.mesh.material_ids[0]].specular[1],
                materials[shape.mesh.material_ids[0]].specular[2],
                1.0f
            );
        }
        else
        {
            mesh.materialParam.ka = glm::vec4(0.0f);
            mesh.materialParam.kd = glm::vec4(1.0f);
            mesh.materialParam.ks = glm::vec4(1.0f);
        }
        meshs.push_back(mesh);
    }
    return ret;
}



DxObjModel::DxObjModel()
{
}


DxObjModel::~DxObjModel()
{
}

HRESULT DxObjModel::Init(const std::string& filename, ID3D11Device* pDevice)
{
    HRESULT hr = E_FAIL;

    std::vector<Mesh> meshs;

    BOOL bLoadObj = LoadObj(filename, meshs, boundingBox_);
    assert(bLoadObj);

    for (auto mesh : meshs)
    {
        MeshRender meshRender;
        hr = CreateVertexBuffer(pDevice, &mesh.vertexDatas[0], sizeof(VertexData)*mesh.vertexDatas.size(), &meshRender.vbuffer);
        assert(!FAILED(hr));

        hr = CreateIndexBuffer(pDevice, (void*)&mesh.indices[0], sizeof(mesh.indices[0])*mesh.indices.size(), &meshRender.ibuffer);
        assert(!FAILED(hr));

        meshRender.materialParam = mesh.materialParam;
        meshRender.indexCount = mesh.indices.size();

        meshRenders_.push_back(meshRender);
    }

    hr = CreateConstantBuffer(pDevice, sizeof(MaterialParam), &cbPerObject_);
    assert(!FAILED(hr));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = 2;

    hr = CreateVertexShader(pDevice, "shader/test1.fx", "VSMain", "vs_4_0", layout, numElements, &layout_, &vshader_);
    assert(!FAILED(hr));

    hr = CreatePixelShader(pDevice, "shader/test1.fx", "PSMain", "ps_4_0", &pshader_);
    assert(!FAILED(hr));

    hr = CreateVertexShader(pDevice, "shader/shadow.fx", "VSMain", "vs_4_0", layout, numElements, &layout_, &smvshader_);
    assert(!FAILED(hr));

    hr = CreateVertexShader(pDevice, "shader/voxel.fx", "VSMain", "vs_4_0", layout, numElements, &layout_, &voxelVsShader_);
    assert(!FAILED(hr));

    hr = CreatePixelShader(pDevice, "shader/voxel.fx", "PSMain", "ps_5_0", &voxelPsShader_);
    assert(!FAILED(hr));

    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
    desc.FillMode = D3D11_FILL_SOLID;
    //desc.FillMode = D3D11_FILL_WIREFRAME;
    desc.CullMode = D3D11_CULL_FRONT;
    desc.DepthClipEnable = TRUE;
    hr = pDevice->CreateRasterizerState(&desc, &state);
    assert(!FAILED(hr));

    D3D11_DEPTH_STENCIL_DESC dsDesc = 
    {
        TRUE,//DepthEnable
        D3D11_DEPTH_WRITE_MASK_ALL,//DepthWriteMask
        D3D11_COMPARISON_LESS,//DepthFunc
        FALSE,//StencialEnable
        D3D11_DEFAULT_STENCIL_READ_MASK,//StencilReadMask
        D3D11_DEFAULT_STENCIL_WRITE_MASK,//StencilWriteMask
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_COMPARISON_ALWAYS,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_STENCIL_OP_KEEP,
        D3D11_COMPARISON_ALWAYS
    };

    hr = pDevice->CreateDepthStencilState(&dsDesc, &dsState);
    assert(!FAILED(hr));

    D3D11_BLEND_DESC blendDesc = 
    {
        FALSE,
        FALSE,
        FALSE,
        D3D11_BLEND_ONE,
        D3D11_BLEND_ZERO,
        D3D11_BLEND_OP_ADD,
        D3D11_BLEND_ONE,
        D3D11_BLEND_ZERO,
        D3D11_BLEND_OP_ADD,
        D3D11_COLOR_WRITE_ENABLE_ALL
    };

    hr = pDevice->CreateBlendState(&blendDesc, &blendState);
    assert(!FAILED(hr));

    return hr;
}

void    DxObjModel::Unit()
{
    state->Release();
    pshader_->Release();
    vshader_->Release();
    layout_->Release();
    cbPerObject_->Release();
    for (auto meshRender : meshRenders_)
    {
        meshRender.vbuffer->Release();
        meshRender.ibuffer->Release();
    }
    
}

void DxObjModel::Render(ID3D11DeviceContext* pContext)
{
    _Render(pContext, vshader_, pshader_);
}

void DxObjModel::RenderDepth(ID3D11DeviceContext* pContext)
{
    HRESULT hr = E_FAIL;
    UINT stride = sizeof(float) * 8;
    UINT offset = 0;

    pContext->IASetInputLayout(layout_);
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->VSSetShader(vshader_, NULL, 0);
    pContext->PSSetShader(NULL, NULL, 0);

    pContext->RSSetState(state);
    pContext->OMSetDepthStencilState(dsState, 0);
    float BlendFactor[4] = { 0, 0, 0, 0 };
    pContext->OMSetBlendState(blendState, BlendFactor, 0xffffffff);

    for (auto meshRender : meshRenders_)
    {
        pContext->IASetVertexBuffers(0, 1, &meshRender.vbuffer, &stride, &offset);

        pContext->IASetIndexBuffer(meshRender.ibuffer, DXGI_FORMAT_R32_UINT, 0);
        pContext->DrawIndexed(meshRender.indexCount, 0, 0);
    }
}

void DxObjModel::RenderVoxel(ID3D11DeviceContext* pContext)
{
    _Render(pContext, voxelVsShader_, voxelPsShader_);
}

const BoundingBox& DxObjModel::GetBoundingBox()
{
    return boundingBox_;
}

void DxObjModel::_Render(
    ID3D11DeviceContext* pContext,
    ID3D11VertexShader*             vshader,
    ID3D11PixelShader*              pshader
)
{
    HRESULT hr = E_FAIL;
    UINT stride = sizeof(float) * 8;
    UINT offset = 0;

    pContext->IASetInputLayout(layout_);


    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->VSSetShader(vshader, NULL, 0);
    pContext->PSSetShader(pshader, NULL, 0);

    pContext->RSSetState(state);
    pContext->OMSetDepthStencilState(dsState, 0);
    float BlendFactor[4] = { 0, 0, 0, 0 };
    pContext->OMSetBlendState(blendState, BlendFactor, 0xffffffff);

    for (auto meshRender : meshRenders_)
    {
        pContext->IASetVertexBuffers(0, 1, &meshRender.vbuffer, &stride, &offset);

        hr = UpdateConstantBuffer(pContext, cbPerObject_, meshRender.materialParam);
        pContext->PSSetConstantBuffers(1, 1, &cbPerObject_);

        pContext->IASetIndexBuffer(meshRender.ibuffer, DXGI_FORMAT_R32_UINT, 0);
        pContext->DrawIndexed(meshRender.indexCount, 0, 0);
    }
}