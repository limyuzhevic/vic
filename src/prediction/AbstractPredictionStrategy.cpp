#include "AbstractPredictionStrategy.hpp"

namespace nlm {

// Default implementations for AbstractPredictionStrategy
// Can be overridden by concrete implementations

const char* AbstractPredictionStrategy::getName() const {
    return "AbstractPredictionStrategy";
}

std::string AbstractPredictionStrategy::getStatus() const {
    return "Abstract prediction strategy (not implemented)";
}

void AbstractPredictionStrategy::enable(bool enable) {
    // Default implementation: do nothing
}

bool AbstractPredictionStrategy::isEnabled() const {
    return true;  // Default: enabled
}

void AbstractPredictionStrategy::reset() {
    // Default implementation: do nothing
}

} // namespace nlm
