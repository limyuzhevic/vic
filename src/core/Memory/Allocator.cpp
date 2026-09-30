// Enhanced NLM Custom Allocator Implementation
// File: src/core/Memory/Allocator.cpp

#include "Allocator.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace nlm {

// MemoryPoolAllocator implementation

MemoryPoolAllocator::MemoryPoolAllocator(const MemoryPoolConfig& config)
    : config_(config)
    , current_block_size_(config.block_size)
    , free_list_head_(nullptr)
    , total_blocks_(0)
    , tracking_enabled_(true)
    , stats_dirty_(true) {
    initialize_allocator();
}

MemoryPoolAllocator::~MemoryPoolAllocator() {
    cleanup();
}

void* MemoryPoolAllocator::allocate(size_t size, Alignment alignment) {
    size_t aligned_size = get_aligned_size(size, alignment);
    
    if (aligned_size <= current_block_size_) {
        // Allocate from current block
        return allocate_from_block(aligned_size);
    }
    
    // Allocate from new block
    return allocate_new_block(aligned_size, alignment);
}

void MemoryPoolAllocator::deallocate(void* ptr, size_t size) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find and free the block containing this pointer
    FreeBlock* current = free_list_head_.load(std::memory_order_acquire);
    while (current) {
        if (is_pointer_in_block(ptr, current)) {
            free_block(current, ptr);
            return;
        }
        current = current->next;
    }
    
    // Pointer not found in any free block, check if it's from a used block
    free_from_used_block(ptr);
}

bool MemoryPoolAllocator::owns_pointer(void* ptr) const {
    return find_block_containing_pointer(ptr) != nullptr;
}

size_t MemoryPoolAllocator::get_allocated_size(void* ptr) const {
    MemoryBlock* block = find_block_containing_pointer(ptr);
    if (!block) return 0;
    return block->size;
}

void* MemoryPoolAllocator::get_pointer(void* raw_ptr) const {
    MemoryBlock* block = find_block_containing_pointer(raw_ptr);
    if (!block) return nullptr;
    return block->ptr;
}

void* MemoryPoolAllocator::release_pointer(void* smart_ptr) {
    // For memory pool allocator, this is a no-op as we don't have separate smart pointers
    return smart_ptr;
}

AllocationStats MemoryPoolAllocator::get_stats() const {
    if (stats_dirty_) {
        calculate_stats();
    }
    return stats_;
}

void MemoryPoolAllocator::reset_stats() {
    stats_ = AllocationStats();
    stats_dirty_ = false;
}

void MemoryPoolAllocator::enable_tracking(bool enable) {
    tracking_enabled_ = enable;
    stats_dirty_ = true;
}

bool MemoryPoolAllocator::is_tracking_enabled() const {
    return tracking_enabled_;
}

void MemoryPoolAllocator::initialize_allocator() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    total_blocks_ = 0;
    free_list_head_ = nullptr;
    
    // Create initial blocks
    for (size_t i = 0; i < config_.initial_blocks; ++i) {
        MemoryBlock* block = new MemoryBlock(current_block_size_, config_.alignment);
        add_block_to_list(block);
        
        // Add all memory in the block to free list
        void* block_ptr = block->ptr;
        for (size_t offset = 0; offset < block->size; offset += current_block_size_) {
            FreeBlock* free_block = new FreeBlock(
                static_cast<char*>(block_ptr) + offset,
                current_block_size_,
                config_.alignment
            );
            free_block->next = block->free_list;
            block->free_list = free_block;
        }
        
        total_blocks_++;
    }
    
    if (config_.enable_statistics) {
        stats_dirty_ = true;
    }
}

void MemoryPoolAllocator::cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryBlock* current = memory_blocks_.front();
    while (current) {
        MemoryBlock* next = current->next;
        
        // Clean up free blocks
        FreeBlock* free_block = current->free_list;
        while (free_block) {
            FreeBlock* next_free = free_block->next;
            delete free_block;
            free_block = next_free;
        }
        
        delete current;
        current = next;
    }
    
    memory_blocks_.clear();
    free_list_head_ = nullptr;
    total_blocks_ = 0;
}

void MemoryPoolAllocator::add_block_to_list(MemoryBlock* block) {
    if (memory_blocks_.empty()) {
        memory_blocks_.push_back(block);
    } else {
        MemoryBlock* last = memory_blocks_.back();
        last->next = block;
        memory_blocks_.push_back(block);
    }
}

void* MemoryPoolAllocator::allocate_from_block(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryBlock* current = memory_blocks_.front();
    while (current) {
        // Try to find a suitable free block
        FreeBlock* prev = nullptr;
        FreeBlock* free_block = current->free_list;
        
        while (free_block) {
            if (free_block->size >= size) {
                // Found suitable block
                if (prev) {
                    prev->next = free_block->next;
                } else {
                    current->free_list = free_block->next;
                }
                
                void* ptr = free_block->ptr;
                
                // Split block if necessary
                if (free_block->size > size) {
                    FreeBlock* split_block = new FreeBlock(
                        static_cast<char*>(ptr) + size,
                        free_block->size - size,
                        free_block->alignment
                    );
                    split_block->next = free_block->next;
                    free_block->next = split_block;
                }
                
                delete free_block;
                current->in_use = true;
                
                if (tracking_enabled_) {
                    track_allocation(ptr, size, current);
                }
                
                return ptr;
            }
            
            prev = free_block;
            free_block = free_block->next;
        }
        
        current = current->next;
    }
    
    return nullptr; // No suitable block found
}

void* MemoryPoolAllocator::allocate_new_block(size_t size, Alignment alignment) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (total_blocks_ >= config_.max_blocks) {
        throw std::runtime_error("Memory pool exhausted");
    }
    
    // Create new block
    MemoryBlock* block = new MemoryBlock(current_block_size_ * 2, alignment);
    add_block_to_list(block);
    total_blocks_++;
    
    // Add all memory in the block to free list
    void* block_ptr = block->ptr;
    for (size_t offset = 0; offset < block->size; offset += current_block_size_) {
        FreeBlock* free_block = new FreeBlock(
            static_cast<char*>(block_ptr) + offset,
            current_block_size_,
            alignment
        );
        free_block->next = block->free_list;
        block->free_list = free_block;
    }
    
    // Try to allocate from the new block
    return allocate_from_block(size);
}

bool MemoryPoolAllocator::is_pointer_in_block(void* ptr, FreeBlock* block) const {
    return ptr >= block->ptr && ptr < static_cast<char*>(block->ptr) + block->size;
}

bool MemoryPoolAllocator::is_pointer_in_block(void* ptr, MemoryBlock* block) const {
    return ptr >= block->ptr && ptr < static_cast<char*>(block->ptr) + block->size;
}

MemoryBlock* MemoryPoolAllocator::find_block_containing_pointer(void* ptr) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check free blocks
    FreeBlock* current = free_list_head_.load(std::memory_order_acquire);
    while (current) {
        if (is_pointer_in_block(ptr, current)) {
            return find_memory_block_for_pointer(ptr);
        }
        current = current->next;
    }
    
    // Check used blocks
    MemoryBlock* mem_block = memory_blocks_.front();
    while (mem_block) {
        if (is_pointer_in_block(ptr, mem_block)) {
            return mem_block;
        }
        mem_block = mem_block->next;
    }
    
    return nullptr;
}

MemoryBlock* MemoryPoolAllocator::find_memory_block_for_pointer(void* ptr) const {
    MemoryBlock* current = memory_blocks_.front();
    while (current) {
        if (is_pointer_in_block(ptr, current)) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void MemoryPoolAllocator::free_block(FreeBlock* block, void* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the corresponding memory block
    MemoryBlock* mem_block = find_memory_block_for_pointer(ptr);
    if (!mem_block) {
        return; // Block not found, maybe already freed
    }
    
    // Add to free list of memory block
    FreeBlock* new_free_block = new FreeBlock(block->ptr, block->size, block->alignment);
    new_free_block->next = mem_block->free_list;
    mem_block->free_list = new_free_block;
    
    mem_block->in_use = false;
    
    if (tracking_enabled_) {
        track_deallocation(ptr, block->size, mem_block);
    }
}

void MemoryPoolAllocator::free_from_used_block(void* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryBlock* current = memory_blocks_.front();
    while (current) {
        if (is_pointer_in_block(ptr, current)) {
            // Mark as free in the memory block's free list
            FreeBlock* new_free_block = new FreeBlock(ptr, current->size, current->alignment);
            new_free_block->next = current->free_list;
            current->free_list = new_free_block;
            current->in_use = false;
            
            if (tracking_enabled_) {
                track_deallocation(ptr, current->size, current);
            }
            
            return;
        }
        current = current->next;
    }
}

void MemoryPoolAllocator::track_allocation(void* ptr, size_t size, MemoryBlock* block) {
    // Track allocation for debugging and leak detection
    // Implementation details depend on tracking requirements
}

void MemoryPoolAllocator::track_deallocation(void* ptr, size_t size, MemoryBlock* block) {
    // Track deallocation for debugging and leak detection
    // Implementation details depend on tracking requirements
}

void MemoryPoolAllocator::calculate_stats() {
    stats_.total_allocations = 0;
    stats_.total_deallocations = 0;
    stats_.current_allocations = 0;
    stats_.peak_allocations = 0;
    stats_.total_bytes_allocated = 0;
    stats_.total_bytes_deallocated = 0;
    stats_.peak_memory_usage = 0;
    stats_.current_memory_usage = 0;
    
    // Calculate statistics from all memory blocks
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryBlock* current = memory_blocks_.front();
    while (current) {
        // Count allocations in this block
        size_t block_allocations = 0;
        FreeBlock* free_block = current->free_list;
        while (free_block) {
            block_allocations++;
            free_block = free_block->next;
        }
        
        stats_.total_allocations += block_allocations;
        stats_.current_allocations += block_allocations;
        stats_.peak_allocations = std::max(stats_.peak_allocations, stats_.current_allocations);
        stats_.total_bytes_allocated += block_allocations * current_block_size_;
        stats_.current_memory_usage += block_allocations * current_block_size_;
        
        current = current->next;
    }
    
    stats_.peak_memory_usage = stats_.current_memory_usage;
    stats_.last_update = std::chrono::steady_clock::now();
    stats_dirty_ = false;
}

void MemoryPoolAllocator::rebuild_allocator() {
    // Rebuild allocator with new configuration
    cleanup();
    initialize_allocator();
}

// MemoryPoolAllocator helper methods implementation

void* MemoryPoolAllocator::allocate_aligned_memory(size_t size, size_t alignment) {
    void* ptr = std::malloc(size + alignment);
    if (!ptr) return nullptr;
    
    size_t addr = reinterpret_cast<size_t>(ptr);
    size_t aligned_addr = (addr + alignment - 1) & ~(alignment - 1);
    void* aligned_ptr = reinterpret_cast<void*>(aligned_addr);
    
    // Store the original pointer
    void** original_ptr = reinterpret_cast<void**>(aligned_addr - alignment);
    *original_ptr = ptr;
    
    return aligned_ptr;
}

void MemoryPoolAllocator::free_aligned_memory(void* ptr, size_t size, size_t alignment) {
    if (!ptr) return;
    
    void** original_ptr = reinterpret_cast<void**>(
        reinterpret_cast<size_t>(ptr) - alignment
    );
    void* original = *original_ptr;
    
    std::free(original);
}

} // namespace nlm
