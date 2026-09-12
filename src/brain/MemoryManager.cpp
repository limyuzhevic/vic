// Memory manager implementation with improved error handling
#include "MemoryManager.hpp"
#include <stdexcept>
#include <cstring>

namespace nlm {

MemoryManager::MemoryManager() : memoryPool(nullptr), poolSize(0), usedMemory(0), initialized(false) {
    // Initialize memory pool
    poolSize = 1024 * 1024 * 1024;  // 1GB pool
    memoryPool = std::malloc(poolSize);
    if (!memoryPool) {
        throw std::runtime_error("Failed to allocate memory pool");
    }
    initialized = true;
}

MemoryManager::~MemoryManager() {
    reset();
}

void* MemoryManager::allocate(size_t size, const std::string& context) {
    if (!initialized) {
        throw std::runtime_error("Memory manager not initialized");
    }
    
    if (size == 0) {
        return nullptr;
    }
    
    if (size > poolSize - usedMemory) {
        throw std::runtime_error("Not enough memory available in pool");
    }
    
    void* ptr = static_cast<char*>(memoryPool) + usedMemory;
    usedMemory += size;
    
    // Initialize allocated memory to zero
    std::memset(ptr, 0, size);
    
    return ptr;
}

void MemoryManager::deallocate(void* ptr, const std::string& context) {
    if (!ptr) {
        return;
    }
    
    if (!initialized) {
        throw std::runtime_error("Memory manager not initialized");
    }
    
    // Check if pointer is within memory pool
    if (ptr < memoryPool || ptr >= static_cast<char*>(memoryPool) + poolSize) {
        throw std::runtime_error("Pointer not in memory pool");
    }
    
    // Mark memory as free (not actually freeing to allow reuse)
    size_t ptrOffset = static_cast<char*>(ptr) - static_cast<char*>(memoryPool);
    
    // Don't actually free - just track used memory
    if (ptrOffset < usedMemory) {
        // Find next used memory boundary
        while (ptrOffset > 0 && static_cast<char*>(memoryPool)[ptrOffset - 1] == 0) {
            ptrOffset--;
        }
        usedMemory = ptrOffset;
    }
}

size_t MemoryManager::getAvailableMemory() const {
    return poolSize - usedMemory;
}

size_t MemoryManager::getUsedMemory() const {
    return usedMemory;
}

bool MemoryManager::validate() const {
    if (!initialized) {
        return false;
    }
    
    if (!memoryPool) {
        return false;
    }
    
    if (usedMemory > poolSize) {
        return false;
    }
    
    return true;
}

void MemoryManager::reset() {
    if (memoryPool) {
        std::free(memoryPool);
        memoryPool = nullptr;
    }
    poolSize = 0;
    usedMemory = 0;
    initialized = false;
}

} // namespace nlm
