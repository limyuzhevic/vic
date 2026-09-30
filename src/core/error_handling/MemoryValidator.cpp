#include "MemoryValidator.hpp"
#include "exceptions/BrainException.hpp"
#include "core/Types/Types.hpp"
#include "core/Logger/Logger.hpp"
#include <cstring>
#include <stdexcept>

namespace nlm {

void MemoryValidator::validateMemoryState(const Brain* brain) {
    if (!brain) {
        throw BrainInitializationException("Brain object is null", "MemoryValidator");
    }
    
    // Validate memory system states
    if (brain->getWorkingMemory() && brain->getWorkingMemory()->getActiveTraces() > 
        brain->getWorkingMemory()->getCapacity()) {
        throw MemoryException(
            "Working memory active traces exceed capacity",
            {{ "active_traces", std::to_string(brain->getWorkingMemory()->getActiveTraces()) },
             { "capacity", std::to_string(brain->getWorkingMemory()->getCapacity()) }},
            "MemoryValidator"
        );
    }
}

void MemoryValidator::validateMemoryAccess(const void* ptr, size_t size) {
    if (size > 0 && ptr == nullptr) {
        throw MemoryException(
            "Attempt to access null pointer with size > 0",
            {{ "size", std::to_string(size) }},
            "MemoryValidator"
        );
    }
    
    // Check for buffer overflow
    if (size > 1024 * 1024 * 1024) {  // 1GB limit
        throw MemoryException(
            "Memory access size exceeds maximum allowed size",
            {{ "size", std::to_string(size) },
             { "max_size", "1073741824" }},
            "MemoryValidator"
        );
    }
}

void MemoryValidator::validateMemoryCapacity(size_t required, size_t available) {
    if (required > available) {
        std::map<std::string, std::string> context;
        context["required"] = std::to_string(required);
        context["available"] = std::to_string(available);
        context["deficit"] = std::to_string(required - available);
        
        throw MemoryException(
            "Insufficient memory capacity",
            context,
            "MemoryValidator"
        );
    }
}

void MemoryValidator::validateMemoryAllocation(size_t size, const std::string& purpose) {
    if (size == 0) {
        throw MemoryException(
            "Cannot allocate zero bytes",
            {{ "purpose", purpose }},
            "MemoryValidator"
        );
    }
    
    // Check for potential overflow
    if (size > std::numeric_limits<size_t>::max() / 2) {
        throw MemoryException(
            "Memory allocation size would cause overflow",
            {{ "size", std::to_string(size) },
             { "purpose", purpose }},
            "MemoryValidator"
        );
    }
}

void MemoryValidator::handleMemoryError(const std::exception& e) {
    try {
        throw;
    } catch (const BrainException& be) {
        LOG_ERROR("Memory system error: " + std::string(be.whatWithDetails()));
        throw;
    } catch (const std::bad_alloc& e) {
        LOG_CRITICAL("Memory allocation failed: " + std::string(e.what()));
        throw MemoryException(
            "System out of memory - cannot allocate requested memory",
            {{ "error", std::string(e.what()) }},
            "MemoryValidator"
        );
    } catch (const std::exception& e) {
        LOG_ERROR("Unexpected memory system error: " + std::string(e.what()));
        throw MemoryException(
            "Unexpected memory system error",
            {{ "error", std::string(e.what()) }},
            "MemoryValidator"
        );
    }
}

} // namespace nlm
