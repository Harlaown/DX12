#include "pch.h"
#include "GDataUploader.h"
#include "d3dApp.h"
#include "d3dUtil.h"

GDataUploader::GDataUploader(size_t pageSize)
    : PageSize(pageSize)
{}

GDataUploader::~GDataUploader()
{
	for (auto && page : pages)
	{
        page.reset();
	}	

    pages.clear();
}

GDataUploader::UploadAllocation GDataUploader::Allocate(size_t sizeInBytes, size_t alignment = 8)
{
	for (auto && page : pages)
    {
	    if(page->HasSpace(sizeInBytes, alignment))
	    {
            return page->Allocate(sizeInBytes, alignment);
	    }
    }	

    std::shared_ptr<UploadMemoryPage> page;
	
    if (PageSize >= sizeInBytes)
    {
        page = CreatePage(PageSize);       
    }
    else
    {
	    const uint32_t newSize = Math::NextHighestPow2((uint32_t)sizeInBytes);    	
	    page = CreatePage(Math::IsAligned(newSize, alignment) ? sizeInBytes : Math::AlignUp(newSize, alignment));    	
    }
    pages.push_back(std::move(page));
    return pages.back()->Allocate(sizeInBytes, alignment);
}

std::shared_ptr<GDataUploader::UploadMemoryPage> GDataUploader::CreatePage(uint32_t pageSize) const
{
    return  std::make_shared<UploadMemoryPage>(pageSize);
}

void GDataUploader::Reset()
{
    for (auto&& page : pages)
    {
        page->Reset();
    }
}

GDataUploader::UploadMemoryPage::UploadMemoryPage(size_t sizeInBytes)
    : CPUPtr(nullptr)
    , GPUPtr(D3D12_GPU_VIRTUAL_ADDRESS(0))
    , PageSize(sizeInBytes)
    , Offset(0)
{
    auto& device = DXLib::D3DApp::GetApp().GetDevice();

    ThrowIfFailed(device.CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(PageSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&d3d12Resource)
    ));
	
    d3d12Resource->SetName(L"Upload Buffer (Page)");

    GPUPtr = d3d12Resource->GetGPUVirtualAddress();
    d3d12Resource->Map(0, nullptr, &CPUPtr);
}

GDataUploader::UploadMemoryPage::~UploadMemoryPage()
{
    d3d12Resource->Unmap(0, nullptr);
    CPUPtr = nullptr;
    GPUPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
    d3d12Resource->Release();
}

bool GDataUploader::UploadMemoryPage::HasSpace(size_t sizeInBytes, size_t alignment) const
{
	const size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
	const size_t alignedOffset = Math::AlignUp(Offset, alignment);

    return alignedOffset + alignedSize <= PageSize;
}

GDataUploader::UploadAllocation GDataUploader::UploadMemoryPage::Allocate(size_t sizeInBytes, size_t alignment)
{   
    const size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
    Offset = Math::AlignUp(Offset, alignment);

    const UploadAllocation allocation
    {
     static_cast<uint8_t*>(CPUPtr) + Offset,
     GPUPtr + Offset
    };

    Offset += alignedSize;

    return allocation;
}

void GDataUploader::UploadMemoryPage::Reset()
{
    Offset = 0;
}

size_t GDataUploader::GetPageSize() const
{
	return PageSize;
}