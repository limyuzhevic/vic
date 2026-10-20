#include "AbstractMemory.hpp"

namespace nlm {

// Default implementations for AbstractMemory
// Can be overridden by concrete implementations

const char* AbstractMemory::getName() const {
    return "AbstractMemory";
}

std::string AbstractMemory::getStatus() const {
    return "Abstract memory system (not implemented)";
}

void AbstractMemory::enable(bool enable) {
    // Default implementation: do nothing
}

bool AbstractMemory::isEnabled() const {
    return true;  // Default: enabled
}

void AbstractMemory::reset() {
    // Default implementation: do nothing
}

} // namespace nlm
