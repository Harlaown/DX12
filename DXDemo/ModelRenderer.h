#pragma once
#include "Model.h"
#include "Renderer.h"
#include "DXAllocator.h"

class Transform;
class GCommandList;

class ModelRenderer : public Renderer
{
	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	
	

	ObjectConstants constantData{};
	
	std::unique_ptr<ConstantBuffer<ObjectConstants>> modelDataBuffer = nullptr;
	void SendDataToConstantBuffer(UINT index, ObjectConstants data) const;
	D3D12_GPU_VIRTUAL_ADDRESS GetMeshConstantData(UINT index = 0) const;
	
	custom_vector<std::shared_ptr<Material>> meshesMaterials = DXAllocator::CreateVector< std::shared_ptr<Material>>();
	
	
protected:
		
	void Draw(std::shared_ptr<GCommandList> cmdList) override;

	void Update() override;

public:

	std::shared_ptr<Model> model = nullptr;

	void SetModel(std::shared_ptr<Model> asset);

	UINT GetMeshesCount() const;

	void SetMeshMaterial(UINT index, const std::shared_ptr<Material> material);
};
