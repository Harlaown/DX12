#include "GraphicDescriptorAllocator.h"
#include "GraphicDescriptorAllocatorPage.h"


GraphicDescriptorAllocator::GraphicDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerPage)
    : allocatorType(type)
    , numDescriptorsPerPage(numDescriptorsPerPage)
{
}

GraphicDescriptorAllocator::~GraphicDescriptorAllocator()
{
    availablePages.clear();	
    pages.clear();
}

std::shared_ptr<GraphicDescriptorAllocatorPage> GraphicDescriptorAllocator::CreateAllocatorPage()
{
    auto newPage = std::make_shared<GraphicDescriptorAllocatorPage>(allocatorType, numDescriptorsPerPage);

    pages.emplace_back(newPage);
    availablePages.insert(pages.size() - 1);

    return newPage;
}

GraphicDescriptorAllocation GraphicDescriptorAllocator::Allocate(uint32_t numDescriptors)
{
    std::lock_guard<std::mutex> lock(allocationMutex);

    GraphicDescriptorAllocation allocation;

    auto iterator = availablePages.begin();
    while (iterator != availablePages.end())
    {
        auto page = pages[*iterator];

        allocation = page->Allocate(numDescriptors);
                
        if (allocation.IsNull())
        {
	        ++iterator;
	        continue;
        }
            	
        if (page->FreeHandlerCount() == 0)
        {
	        iterator = availablePages.erase(iterator);
        }
        	
        return allocation;
    }
    
    numDescriptorsPerPage = std::max(numDescriptorsPerPage, numDescriptors);

    auto newPage = CreateAllocatorPage();
    allocation = newPage->Allocate(numDescriptors);

    return allocation;
}

void GraphicDescriptorAllocator::ReleaseStaleDescriptors(uint64_t frameNumber)
{
    std::lock_guard<std::mutex> lock(allocationMutex);

    for (size_t i = 0; i < pages.size(); ++i)
    {
        auto page = pages[i];

        page->ReleaseStaleDescriptors(frameNumber);

        if (page->FreeHandlerCount() > 0)
        {
            availablePages.insert(i);
        }
    }
}

