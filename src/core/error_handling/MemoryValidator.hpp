// Include the core exception hierarchy and necessary headers
#pragma once

#include "exceptions/BrainException.hpp"
#include <map>
#include <vector>
#include <string>

namespace nlm {

/**
 * Memory system validation and error handling
 */
class MemoryValidator {
public:
    static void validateMemoryState(const Brain* brain);
    static void validateMemoryAccess(const void* ptr, size_t size);
    static void validateMemoryCapacity(size_t required, size_t available);
    static void validateMemoryAllocation(size_t size, const std::string& purpose);
    static void handleMemoryError(const std::exception& e);
};

} // namespace nlm
