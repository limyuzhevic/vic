// ValidationUtils.cpp - Implementation of validation utilities
#include "ValidationUtils.hpp"
#include <cstdlib>
#include <stdexcept>

namespace nlm {

// Memory safety utility implementations
void* safeAllocate(size_t size, const std::string& context) {
    if (size == 0) {
        return nullptr;
    }
    
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw MemoryError(MemoryError::Type::AllocationError,
                         "Failed to allocate " + std::to_string(size) + " bytes in context: " + context);
    }
    return ptr;
}

void safeDeallocate(void* ptr, const std::string& context) {
    if (!ptr) {
        return; // Already null
    }
    
    std::free(ptr);
}

// Error handling utility implementations
void logAndRethrow(const std::exception& e, const std::string& context) {
    NLM_LOG_ERROR("Exception in " + context + ": " + e.what());
    throw; // Re-throw the exception
}

bool tryCatchRethrow(const std::function<void()>& operation,
                   const std::string& context) {
    try {
        operation();
        return true;
    } catch (const std::exception& e) {
        logAndRethrow(e, context);
        return false;
    }
}

// StateRestorer method implementations
void StateRestorer::clear() {
    savedStates.clear();
    NLM_LOG_INFO("Cleared all saved states");
}

} // namespace nlm
