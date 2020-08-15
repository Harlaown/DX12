#pragma once
#include "Renderer.h"
#include "GraphicBuffer.h"
#include "STLCustomAllocator.h"
#include "assimp/scene.h"

class GCommandList;

class ModelMesh
{
public:
	ModelMesh(std::shared_ptr<GCommandList> cmdList, std::string name, custom_vector<Vertex>& vertices,
	          custom_vector<DWORD>& indices, D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void Update(Transform* transform);

	void Draw(std::shared_ptr<GCommandList> cmdList) const;
	void static CalculateTangent(UINT i1, UINT i2, UINT i3, custom_vector<Vertex>& vertex);

	Material* material{};

private:
	ObjectConstants bufferConstant{};
	std::unique_ptr<ConstantBuffer<ObjectConstants>> objectConstantBuffer = nullptr;

	std::string name;


	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType;
	std::unique_ptr<VertexBuffer> vertexBuffer = nullptr;
	std::unique_ptr<IndexBuffer> indexBuffer = nullptr;
};

class ModelRenderer : public Renderer
{
	custom_vector<ModelMesh> meshes = DXAllocator::CreateVector<ModelMesh>();

	void ProcessNode(aiNode* node, const aiScene* scene, std::shared_ptr<GCommandList>);

	static ModelMesh ProcessMesh(aiMesh* mesh, const aiScene* scene,
		std::shared_ptr<GCommandList> cmdList);

	void Draw(std::shared_ptr<GCommandList> cmdList) override;

	void Update() override;

public:

	bool AddModel(std::shared_ptr<GCommandList> cmdList, const std::string& filePath);

	UINT GetMeshesCount()
	{
		return meshes.size();
	}

	void SetMeshMaterial(UINT index, Material* material)
	{
		meshes[index].material = material;
	}
};
