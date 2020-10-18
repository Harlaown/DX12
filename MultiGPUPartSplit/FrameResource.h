#pragma once

#include "GDescriptor.h"
#include "ShaderBuffersData.h"
#include "GTexture.h"

using namespace  PEPEngine;
using namespace Graphics;


struct FrameResource
{
public:

	FrameResource(std::shared_ptr<GDevice> primeDevices, std::shared_ptr<GDevice> secondDevice,  UINT passCount, UINT materialCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();
			

	GDescriptor BackBufferRTVMemory;
	
	std::shared_ptr<ConstantBuffer<PassConstants>> PrimePassConstantBuffer;
	std::shared_ptr<ConstantBuffer<PassConstants>> ShadowPassConstantBuffer;
	std::shared_ptr<ConstantBuffer<SsaoConstants>> SsaoConstantBuffer;

	custom_vector<std::shared_ptr<UploadBuffer<MaterialConstants>>> MaterialBuffers = MemoryAllocator::CreateVector < std::shared_ptr<UploadBuffer<MaterialConstants>>>();
	
	

	UINT64 FenceValue = 0;
	UINT64 SecondFenceValue = 0;
};
