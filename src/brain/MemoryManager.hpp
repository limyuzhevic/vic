// Memory management with improved error handling
#pragma once

#include <memory>
#include <string>

namespace nlm {

class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Allocate memory with validation
    void* allocate(size_t size, const std::string& context = "");
    
    // Deallocate memory with validation
    void deallocate(void* ptr, const std::string& context = "");
    
    // Get available memory
    size_t getAvailableMemory() const;
    
    // Get used memory
    size_t getUsedMemory() const;
    
    // Validate memory state
    bool validate() const;
    
    // Reset memory manager
    void reset();
    
private:
    void* memoryPool;
    size_t poolSize;
    size_t usedMemory;
    bool initialized;
};

} // namespace nlm
