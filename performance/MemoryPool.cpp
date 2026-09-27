#include "MemoryPool.hpp"

namespace nlm {

// Example usage and test
template<typename T>
void testMemoryPool() {
    // Test basic functionality
    MemoryPool<T> pool(sizeof(T) * 100, 10);
    
    // Allocate and deallocate
    T* ptr1 = pool.allocate();
    T* ptr2 = pool.allocate();
    
    *ptr1 = T{1};
    *ptr2 = T{2};
    
    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
    
    // Check stats
    auto stats = pool.getStats();
    // Use stats for monitoring
}

} // namespace nlm
