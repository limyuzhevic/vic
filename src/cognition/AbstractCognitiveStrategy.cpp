#include "AbstractCognitiveStrategy.hpp"

namespace nlm {

// Default implementations for AbstractCognitiveStrategy
// Can be overridden by concrete implementations

const char* AbstractCognitiveStrategy::getName() const {
    return "AbstractCognitiveStrategy";
}

std::string AbstractCognitiveStrategy::getStatus() const {
    return "Abstract cognitive strategy (not implemented)";
}

void AbstractCognitiveStrategy::enable(bool enable) {
    // Default implementation: do nothing
}

bool AbstractCognitiveStrategy::isEnabled() const {
    return true;  // Default: enabled
}

void AbstractCognitiveStrategy::reset() {
    // Default implementation: do nothing
}

} // namespace nlm
