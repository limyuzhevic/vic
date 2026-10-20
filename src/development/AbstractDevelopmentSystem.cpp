#include "AbstractDevelopmentSystem.hpp"

namespace nlm {

// Default implementations for AbstractDevelopmentSystem
// Can be overridden by concrete implementations

const char* AbstractDevelopmentSystem::getName() const {
    return "AbstractDevelopmentSystem";
}

std::string AbstractDevelopmentSystem::getStatus() const {
    return "Abstract development system (not implemented)";
}

void AbstractDevelopmentSystem::enable(bool enable) {
    // Default implementation: do nothing
}

bool AbstractDevelopmentSystem::isEnabled() const {
    return true;  // Default: enabled
}

void AbstractDevelopmentSystem::reset() {
    // Default implementation: do nothing
}

} // namespace nlm
