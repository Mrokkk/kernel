#pragma once

#include <spinlock.hpp>
#include <kernel_list.hpp>
#include <lazy_instance.hpp>

#include "VirtualMemoryManager.hpp"

namespace memory
{

template <class BackendAllocator, size_t MemoryBlockSize>
class Allocator final
{
    class MemoryBlock final
    {
        struct Data
        {
            uint8_t data[MemoryBlockSize];
            uint8_t blockPtr[0];
        } __attribute__((packed));

        template <typename Ptr>
        Ptr pointerOffset(Ptr ptr, int off)
        {
            return reinterpret_cast<Ptr>(reinterpret_cast<char*>(ptr) + off);
        }

    public:
        constexpr explicit MemoryBlock(const size_t size)
            : size(size)
            , free(false)
        {
        }

        void divide(const size_t pivot)
        {
            auto oldSize = size;
            size = pivot;
            free = false;
            auto newBlock = reinterpret_cast<MemoryBlock*>(pointerOffset(data(), pivot));
            newBlock->size = oldSize - MemoryBlockSize - size;
            newBlock->free = true;
            list_.insert(&newBlock->list_);
        }

        void tryToDivide(const size_t pivot)
        {
            if (size <= pivot + 2 * MemoryBlockSize)
            {
                free = false;
            }
            else
            {
                divide(pivot);
            }
        }

        void* data()
        {
            return reinterpret_cast<Data*>(this)->blockPtr;
        }

        constexpr void* operator new(size_t, void* address)
        {
            return address;
        }

        typename yacppl::kernel_list<MemoryBlock>::node list_;
        uint32_t size;
        bool free;
    };

public:
    explicit Allocator(BackendAllocator* backendAllocator)
        : blocks_(&MemoryBlock::list_)
        , backendAllocator_(backendAllocator)
    {
        growHeap();
    }

    void* allocate(size_t size)
    {
        auto lock = yacppl::make_scoped_lock(spinlock_);

        adaptSize(size);
        for (auto& temp : blocks_)
        {
            if (temp.free and temp.size >= size)
            {
                temp.tryToDivide(size);
                return temp.data();
            }
        }

        auto newBlock = createMemoryBlock(size);
        if (not newBlock)
        {
            return nullptr;
        }

        blocks_.push_back(*newBlock);
        return newBlock->data();
    }

    bool free(void* address)
    {
        auto lock = yacppl::make_scoped_lock(spinlock_);

        for (auto& temp : blocks_)
        {
            if (temp.data() == address)
            {
                temp.free = true;
                return true;
            }

            auto next = temp.list_.next()->entry();
            if (not next)
            {
                break;
            }
            //if (next->free && temp.free)
            //{
                //temp.size = temp.size + next->size + MemoryBlockSize;
                //blocks_.erase(*next);
            //}
        }
        return false;
    }

private:
    void adaptSize(size_t& size) const
    {
        if (size % MemoryBlockSize)
        {
            size = (size / MemoryBlockSize) * MemoryBlockSize + MemoryBlockSize;
        }
    }

    void growHeap()
    {
        heap_ = reinterpret_cast<uint32_t>(backendAllocator_->allocate());
        end_ = heap_ + PAGE_SIZE;
    }

    MemoryBlock* createMemoryBlock(size_t size)
    {
        if (heap_ + MemoryBlockSize + size >= end_)
        {
            growHeap();
        }
        auto block = new(reinterpret_cast<void*>(heap_)) MemoryBlock(size);
        heap_ += MemoryBlockSize + size;
        return block;
    }

    yacppl::kernel_list<MemoryBlock> blocks_;
    BackendAllocator* backendAllocator_;
    static yacppl::spinlock spinlock_;
    uint32_t heap_ = 0;
    uint32_t end_ = 0;
};

template <class BackendAllocator, size_t MemoryBlockSize>
yacppl::spinlock Allocator<BackendAllocator, MemoryBlockSize>::spinlock_;

constexpr const size_t memoryBlockSize = 32;
extern yacppl::lazy_instance<Allocator<VirtualMemoryManager, memoryBlockSize>> heapAllocator;

} // namespace memory
