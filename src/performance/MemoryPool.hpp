// Memory pool system for NLM
// Manages memory allocation for neural simulation objects

#pragma once

#include <memory>
#include <vector>
#include <cstddef>

namespace nlm {

/**
 * Memory pool system for neural simulation
 * Efficiently allocates and deallocates neural simulation objects
 */
class MemoryPool {
public:
    MemoryPool();
    ~MemoryPool();
    
    // Pool configuration
    void setPoolSize(size_t size);
    void setAlignment(size_t alignment);
    size_t getPoolSize() const;
    size_t getAlignment() const;
    
    // Memory allocation
    void* allocate(size_t size);
    template<typename T, typename... Args>
    T* allocateObject(Args&&... args);
    void deallocate(void* ptr, size_t size);
    
    // Object pooling
    template<typename T>
    class PoolAllocator {
    public:
        typedef T value_type;
        
        PoolAllocator(MemoryPool& pool) : pool_(pool) {}
        
        template<typename U>
        PoolAllocator(const PoolAllocator<U>& other) : pool_(other.pool_) {}
        
        T* allocate(std::size_t n);
        void deallocate(T* p, std::size_t n);
        
        template<typename U, typename... Args>
        void construct(U* p, Args&&... args);
        template<typename U>
        void destroy(U* p);
        
        bool operator==(const PoolAllocator& other) const;
        bool operator!=(const PoolAllocator& other) const;
        
    private:
        MemoryPool& pool_;
    };
    
    // Statistics
    size_t getAllocatedMemory() const;
    size_t getPeakMemory() const;
    size_t getAllocationCount() const;
    size_t getDeallocationCount() const;
    
    // Cleanup
    void clear();
    void shrinkToFit();
    
    // Debug
    void enableDebugMode(bool enable);
    bool isDebugMode() const;
    std::string getMemoryReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
