#pragma once

#include "NeuralException.hpp"

namespace nlm {

// Memory pool exceptions
class MemoryPoolError : public MemoryError {
public:
    MemoryPoolError(const std::string& message, MemoryError::ErrorCode code = MemoryError::ErrorCode::MEMORY_ALLOCATION_FAILED)
        : MemoryError(message, code) {}
    
    MemoryPoolError(const char* message, MemoryError::ErrorCode code = MemoryError::ErrorCode::MEMORY_ALLOCATION_FAILED)
        : MemoryError(message, code) {}
};

// Memory management exceptions
class MemoryManagementError : public MemoryError {
public:
    MemoryManagementError(const std::string& message, MemoryError::ErrorCode code = MemoryError::ErrorCode::MEMORY_ERROR)
        : MemoryError(message, code) {}
    
    MemoryManagementError(const char* message, MemoryError::ErrorCode code = MemoryError::ErrorCode::MEMORY_ERROR)
        : MemoryError(message, code) {}
};

} // namespace nlm