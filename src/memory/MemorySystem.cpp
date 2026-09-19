#include "MemorySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// MemorySystem implementation
// Base class with common functionality for all memory systems

void MemorySystem::initialize(Brain* brain) {
    if (!brain) {
        ErrorHandler::reportError(getNullPointerError("Brain", "MemorySystem::initialize"));
        return;
    }
    brain_ = brain;
    NLM_LOG_INFO("MemorySystem initialized");
}

void MemorySystem::clear() {
    // Default implementation - derived classes override
    NLM_LOG_INFO("MemorySystem cleared");
}

void MemorySystem::updateStats() {
    // Default implementation - derived classes override
}

} // namespace nlm
