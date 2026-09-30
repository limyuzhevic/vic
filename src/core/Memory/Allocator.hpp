// Enhanced NLM Custom Allocator System
// File: src/core/Memory/Allocator.hpp

#pragma once

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <vector>
#include <memory>
#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <functional>

namespace nlm {

// Memory allocation strategies
enum class AllocationStrategy {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT,
    NEXT_FIT,
    SLAB_ALLOCATION,
    FREELIST_ALLOCATION
};

// Memory alignment options
enum class Alignment {
    BYTE_1 = 1,
    BYTE_2 = 2,
    BYTE_4 = 4,
    BYTE_8 = 8,
    BYTE_16 = 16,
    BYTE_32 = 32,
    BYTE_64 = 64,
    BYTE_128 = 128,
    BYTE_256 = 256,
    BYTE_512 = 512,
    BYTE_1024 = 1024
};

// Memory pool configuration
struct MemoryPoolConfig {
    size_t block_size;
    size_t initial_blocks;
    size_t max_blocks;
    Alignment alignment;
    AllocationStrategy allocation_strategy;
    bool enable_thread_safety;
    bool enable_statistics;
    size_t cache_line_size = 64;  // Standard cache line size
};

// Allocation statistics
struct AllocationStats {
    uint64_t total_allocations = 0;
    uint64_t total_deallocations = 0;
    uint64_t current_allocations = 0;
    uint64_t peak_allocations = 0;
    uint64_t total_bytes_allocated = 0;
    uint64_t total_bytes_deallocated = 0;
    uint64_t peak_memory_usage = 0;
    uint64_t current_memory_usage = 0;
    double fragmentation_ratio = 0.0;
    std::chrono::steady_clock::time_point last_update;
};

// Memory block metadata
struct MemoryBlock {
    void* ptr = nullptr;
    size_t size = 0;
    size_t alignment = 0;
    std::chrono::steady_clock::time_point allocated_time;
    std::chrono::steady_clock::time_point last_access_time;
    bool in_use = false;
    size_t thread_id = 0;
    std::string allocation_site;
    uint64_t allocation_sequence = 0;
};

// Smart pointer base class for custom memory management
class CustomSmartPointer {
public:
    virtual ~CustomSmartPointer() = default;
    
    virtual void* allocate(size_t size, Alignment alignment = Alignment::BYTE_8) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual bool owns_pointer(void* ptr) const = 0;
    virtual size_t get_allocated_size(void* ptr) const = 0;
    virtual void* get_pointer(void* raw_ptr) const = 0;
    virtual void* release_pointer(void* smart_ptr) = 0;
    virtual AllocationStats get_stats() const = 0;
    virtual void reset_stats() = 0;
    virtual void enable_tracking(bool enable) = 0;
    virtual bool is_tracking_enabled() const = 0;
};

// Custom allocator base class
class CustomAllocator : public CustomSmartPointer {
public:
    virtual ~CustomAllocator() = default;
    
    // Core allocation methods
    void* allocate(size_t size) override {
        return allocate(size, Alignment::BYTE_8);
    }
    
    void deallocate(void* ptr) override {
        deallocate(ptr, 0);
    }
    
    // Virtual allocation methods (to be implemented by subclasses)
    virtual void* allocate(size_t size, Alignment alignment) = 0;
    virtual void deallocate(void* ptr, size_t size) = 0;
    
    // Utility allocation methods
    template<typename T, typename... Args>
    T* allocate_t(Args&&... args) {
        void* ptr = allocate(sizeof(T), Alignment::BYTE_8);
        return new (ptr) T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    void deallocate_t(T* ptr) {
        if (ptr) {
            ptr->~T();
            deallocate(ptr, sizeof(T));
        }
    }
    
    // Type-safe allocation wrapper
    template<typename T>
    T* allocate_array(size_t count) {
        size_t total_size = sizeof(T) * count;
        void* ptr = allocate(total_size, Alignment::BYTE_8);
        return reinterpret_cast<T*>(ptr);
    }
    
    template<typename T>
    void deallocate_array(T* ptr, size_t count) {
        if (ptr) {
            deallocate(ptr, sizeof(T) * count);
        }
    }
    
    // Alignment helpers
    static size_t get_aligned_size(size_t size, Alignment alignment) {
        size_t alignment_value = static_cast<size_t>(alignment);
        return ((size + alignment_value - 1) / alignment_value) * alignment_value;
    }
    
    static void* get_aligned_pointer(void* ptr, Alignment alignment) {
        size_t address = reinterpret_cast<size_t>(ptr);
        size_t alignment_value = static_cast<size_t>(alignment);
        size_t aligned_address = (address + alignment_value - 1) & ~(alignment_value - 1);
        return reinterpret_cast<void*>(aligned_address);
    }
};

// Thread-safe allocator wrapper
class ThreadSafeAllocator : public CustomAllocator {
public:
    ThreadSafeAllocator(std::unique_ptr<CustomAllocator> allocator)
        : allocator_(std::move(allocator)), stats_mutex_(), stats_dirty_(true) {
    }
    
    void* allocate(size_t size, Alignment alignment) override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        void* ptr = allocator_->allocate(size, alignment);
        if (ptr) {
            stats_.total_allocations++;
            stats_.current_allocations++;
            stats_.peak_allocations = std::max(stats_.peak_allocations, stats_.current_allocations);
            stats_.total_bytes_allocated += size;
            stats_.current_memory_usage += size;
            stats_.peak_memory_usage = std::max(stats_.peak_memory_usage, stats_.current_memory_usage);
            stats_.last_update = std::chrono::steady_clock::now();
            stats_dirty_ = true;
        }
        return ptr;
    }
    
    void deallocate(void* ptr, size_t size) override {
        if (!ptr) return;
        
        std::lock_guard<std::mutex> lock(stats_mutex_);
        allocator_->deallocate(ptr, size);
        stats_.total_deallocations++;
        stats_.current_allocations = std::max(0ULL, stats_.current_allocations - 1);
        stats_.total_bytes_deallocated += size;
        stats_.current_memory_usage = std::max(0ULL, stats_.current_memory_usage - size);
        stats_.last_update = std::chrono::steady_clock::now();
        stats_dirty_ = true;
    }
    
    bool owns_pointer(void* ptr) const override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        return allocator_->owns_pointer(ptr);
    }
    
    size_t get_allocated_size(void* ptr) const override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        return allocator_->get_allocated_size(ptr);
    }
    
    void* get_pointer(void* raw_ptr) const override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        return allocator_->get_pointer(raw_ptr);
    }
    
    void* release_pointer(void* smart_ptr) override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        return allocator_->release_pointer(smart_ptr);
    }
    
    AllocationStats get_stats() const override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        if (stats_dirty_) {
            // Recalculate statistics from underlying allocator
            auto underlying_stats = allocator_->get_stats();
            stats_ = underlying_stats;
            stats_dirty_ = false;
        }
        return stats_;
    }
    
    void reset_stats() override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        stats_ = AllocationStats();
        stats_dirty_ = false;
        allocator_->reset_stats();
    }
    
    void enable_tracking(bool enable) override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        allocator_->enable_tracking(enable);
        stats_.last_update = std::chrono::steady_clock::now();
        stats_dirty_ = true;
    }
    
    bool is_tracking_enabled() const override {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        return allocator_->is_tracking_enabled();
    }

protected:
    std::unique_ptr<CustomAllocator> allocator_;
    mutable std::mutex stats_mutex_;
    AllocationStats stats_;
    mutable bool stats_dirty_;
};

// Memory pool allocator
class MemoryPoolAllocator : public CustomAllocator {
public:
    MemoryPoolAllocator(const MemoryPoolConfig& config)
        : config_(config), current_block_size_(config.block_size),
          free_list_head_(nullptr), total_blocks_(0), stats_dirty_(true) {
        initialize_allocator();
    }
    
    ~MemoryPoolAllocator() {
        cleanup();
    }
    
    void* allocate(size_t size, Alignment alignment) override {
        size_t aligned_size = get_aligned_size(size, alignment);
        
        if (aligned_size <= current_block_size_) {
            // Allocate from current block
            return allocate_from_block(aligned_size);
        }
        
        // Allocate from new block
        return allocate_new_block(aligned_size, alignment);
    }
    
    void deallocate(void* ptr, size_t size) override {
        if (!ptr) return;
        
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
    
    bool owns_pointer(void* ptr) const override {
        return find_block_containing_pointer(ptr) != nullptr;
    }
    
    size_t get_allocated_size(void* ptr) const override {
        MemoryBlock* block = find_block_containing_pointer(ptr);
        if (!block) return 0;
        return block->size;
    }
    
    void* get_pointer(void* raw_ptr) const override {
        MemoryBlock* block = find_block_containing_pointer(raw_ptr);
        if (!block) return nullptr;
        return block->ptr;
    }
    
    void* release_pointer(void* smart_ptr) override {
        // For memory pool allocator, this is a no-op as we don't have separate smart pointers
        return smart_ptr;
    }
    
    AllocationStats get_stats() const override {
        if (stats_dirty_) {
            calculate_stats();
        }
        return stats_;
    }
    
    void reset_stats() override {
        stats_ = AllocationStats();
        stats_dirty_ = false;
    }
    
    void enable_tracking(bool enable) override {
        tracking_enabled_ = enable;
        stats_dirty_ = true;
    }
    
    bool is_tracking_enabled() const override {
        return tracking_enabled_;
    }
    
    // Configuration methods
    void set_block_size(size_t size) {
        current_block_size_ = size;
        // Reallocate blocks if necessary
        std::lock_guard<std::mutex> lock(mutex_);
        rebuild_allocator();
    }
    
    void set_alignment(Alignment alignment) {
        config_.alignment = alignment;
    }
    
    void set_allocation_strategy(AllocationStrategy strategy) {
        config_.allocation_strategy = strategy;
    }

private:
    struct FreeBlock {
        void* ptr;
        size_t size;
        Alignment alignment;
        FreeBlock* next;
        
        FreeBlock(void* p, size_t s, Alignment a) 
            : ptr(p), size(s), alignment(a), next(nullptr) {}
    };
    
    struct MemoryBlock {
        void* ptr;
        size_t size;
        size_t alignment;
        std::chrono::steady_clock::time_point allocated_time;
        bool in_use;
        size_t thread_id;
        std::string allocation_site;
        FreeBlock* free_list;
        MemoryBlock* next;
        
        MemoryBlock(size_t block_size, Alignment align)
            : ptr(nullptr), size(block_size), alignment(static_cast<size_t>(align)),
              allocated_time(std::chrono::steady_clock::now()), in_use(false),
              thread_id(0), allocation_site(""), free_list(nullptr), next(nullptr) {
            // Allocate memory
            ptr = allocate_aligned_memory(block_size, align);
        }
        
        ~MemoryBlock() {
            if (ptr) {
                free_aligned_memory(ptr, size, alignment);
            }
        }
    };
    
    void initialize_allocator() {
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
    
    void cleanup() {
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
    
    void* allocate_aligned_memory(size_t size, Alignment alignment) {
        size_t alignment_value = static_cast<size_t>(alignment);
        size_t misaligned_addr = reinterpret_cast<size_t>(std::malloc(size + alignment_value));
        if (!misaligned_addr) return nullptr;
        
        size_t aligned_addr = (misaligned_addr + alignment_value - 1) & ~(alignment_value - 1);
        void* aligned_ptr = reinterpret_cast<void*>(aligned_addr);
        
        // Store the original pointer for deallocation
        void** original_ptr = reinterpret_cast<void**>(aligned_addr - alignment_value);
        *original_ptr = reinterpret_cast<void*>(misaligned_addr);
        
        return aligned_ptr;
    }
    
    void free_aligned_memory(void* ptr, size_t size, Alignment alignment) {
        if (!ptr) return;
        
        size_t alignment_value = static_cast<size_t>(alignment);
        void** original_ptr = reinterpret_cast<void**>(
            reinterpret_cast<size_t>(ptr) - alignment_value
        );
        void* original = *original_ptr;
        
        std::free(original);
    }
    
    void add_block_to_list(MemoryBlock* block) {
        if (memory_blocks_.empty()) {
            memory_blocks_.push_back(block);
        } else {
            MemoryBlock* last = memory_blocks_.back();
            last->next = block;
            memory_blocks_.push_back(block);
        }
    }
    
    void* allocate_from_block(size_t size) {
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
    
    void* allocate_new_block(size_t size, Alignment alignment) {
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
    
    bool is_pointer_in_block(void* ptr, FreeBlock* block) const {
        return ptr >= block->ptr && ptr < static_cast<char*>(block->ptr) + block->size;
    }
    
    bool is_pointer_in_block(void* ptr, MemoryBlock* block) const {
        return ptr >= block->ptr && ptr < static_cast<char*>(block->ptr) + block->size;
    }
    
    MemoryBlock* find_block_containing_pointer(void* ptr) const {
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
    
    MemoryBlock* find_memory_block_for_pointer(void* ptr) const {
        MemoryBlock* current = memory_blocks_.front();
        while (current) {
            if (is_pointer_in_block(ptr, current)) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
    
    void free_block(FreeBlock* block, void* ptr) {
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
    
    void free_from_used_block(void* ptr) {
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
    
    void track_allocation(void* ptr, size_t size, MemoryBlock* block) {
        // Track allocation for debugging and leak detection
        // Implementation details depend on tracking requirements
    }
    
    void track_deallocation(void* ptr, size_t size, MemoryBlock* block) {
        // Track deallocation for debugging and leak detection
        // Implementation details depend on tracking requirements
    }
    
    void calculate_stats() {
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
    
    void rebuild_allocator() {
        // Rebuild allocator with new configuration
        cleanup();
        initialize_allocator();
    }
    
    // Helper methods
    void* allocate_aligned_memory(size_t size, size_t alignment) {
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
    
    void free_aligned_memory(void* ptr, size_t size, size_t alignment) {
        if (!ptr) return;
        
        void** original_ptr = reinterpret_cast<void**>(
            reinterpret_cast<size_t>(ptr) - alignment
        );
        void* original = *original_ptr;
        
        std::free(original);
    }
    
    std::vector<MemoryBlock*> memory_blocks_;
    std::atomic<FreeBlock*> free_list_head_;
    MemoryPoolConfig config_;
    size_t current_block_size_;
    size_t total_blocks_;
    bool tracking_enabled_;
    mutable std::mutex mutex_;
    AllocationStats stats_;
    mutable bool stats_dirty_;
};

// Factory functions for creating allocators
inline std::unique_ptr<CustomAllocator> create_memory_pool_allocator(
    size_t block_size,
    size_t initial_blocks = 4,
    size_t max_blocks = 1024,
    Alignment alignment = Alignment::BYTE_64,
    AllocationStrategy strategy = AllocationStrategy::FIRST_FIT,
    bool thread_safe = true) {
    
    MemoryPoolConfig config;
    config.block_size = block_size;
    config.initial_blocks = initial_blocks;
    config.max_blocks = max_blocks;
    config.alignment = alignment;
    config.allocation_strategy = strategy;
    config.enable_thread_safety = thread_safe;
    config.enable_statistics = true;
    
    std::unique_ptr<CustomAllocator> allocator = std::make_unique<MemoryPoolAllocator>(config);
    
    if (thread_safe) {
        allocator = std::make_unique<ThreadSafeAllocator>(std::move(allocator));
    }
    
    return allocator;
}

inline std::unique_ptr<CustomAllocator> create_custom_allocator(
    AllocationStrategy strategy = AllocationStrategy::BEST_FIT,
    Alignment alignment = Alignment::BYTE_64,
    bool thread_safe = true) {
    
    // For now, return a memory pool allocator as the default implementation
    return create_memory_pool_allocator(
        4096,  // Default block size: 4KB
        16,    // Initial blocks
        1024,  // Max blocks
        alignment,
        strategy,
        thread_safe
    );
}

} // namespace nlm
