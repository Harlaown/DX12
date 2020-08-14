#pragma once

#include "d3dUtil.h"
#include "FrameResource.h"
#include "GraphicPSO.h"


class Ssao
{
public:

	Ssao(ID3D12Device* device,
	     std::shared_ptr<GCommandList> cmdList,
	     UINT width, UINT height);
	Ssao(const Ssao& rhs) = delete;
	Ssao& operator=(const Ssao& rhs) = delete;
	~Ssao() = default;

	static const DXGI_FORMAT AmbientMapFormat = DXGI_FORMAT_R16_UNORM;
	static const DXGI_FORMAT NormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

	static const int MaxBlurRadius = 5;

	UINT SsaoMapWidth() const;
	UINT SsaoMapHeight() const;

	void GetOffsetVectors(Vector4 offsets[]);
	std::vector<float> CalcGaussWeights(float sigma);


	Texture& NormalMap();
	Texture& AmbientMap();

	GMemory* NormalMapRtv()
	{
		return &normalMapRtvMemory;
	};
	GMemory* NormalMapSrv()
	{
		return &normalMapSrvMemory;
	};
	GMemory* AmbientMapSrv()
	{
		return &ambientMapMapSrvMemory;
	};

	void BuildDescriptors(
		Texture& depthStencilBuffer);

	void RebuildDescriptors(Texture& depthStencilBuffer);

	void SetPSOs(GraphicPSO& ssaoPso, GraphicPSO& ssaoBlurPso);

	void OnResize(UINT newWidth, UINT newHeight);

	
	void ComputeSsao(
		std::shared_ptr<GCommandList> cmdList,
		FrameResource* currFrame,
		int blurCount);
	void ClearAmbiantMap(std::shared_ptr<GCommandList> cmdList);


private:
		
	void BlurAmbientMap(std::shared_ptr<GCommandList> cmdList, FrameResource* currFrame, int blurCount);
	void BlurAmbientMap(std::shared_ptr<GCommandList> cmdList, bool horzBlur);
	Texture CreateNormalMap();
	Texture CreateAmbientMap();

	void BuildResources();
	void BuildRandomVectorTexture(std::shared_ptr<GCommandList> cmdList);

	void BuildOffsetVectors();


private:
	ID3D12Device* md3dDevice;

	GraphicPSO mSsaoPso;
	GraphicPSO mBlurPso;

	Texture mRandomVectorMap;
	Texture mNormalMap;
	Texture mAmbientMap0;
	Texture mAmbientMap1;

	GMemory normalMapSrvMemory = DXAllocator::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1);
	GMemory normalMapRtvMemory = DXAllocator::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
	
	GMemory depthMapSrvMemory = DXAllocator::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
	
	GMemory randomVectorSrvMemory = DXAllocator::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
	
	GMemory ambientMapMapSrvMemory = DXAllocator::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);
	GMemory ambientMapRtvMemory = DXAllocator::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2);
	

	UINT mRenderTargetWidth;
	UINT mRenderTargetHeight;

	Vector4 mOffsets[14];

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;
};
