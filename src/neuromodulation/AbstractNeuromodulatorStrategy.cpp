#include "AbstractNeuromodulatorStrategy.hpp"

namespace nlm {

// Default implementations for AbstractNeuromodulatorStrategy
// Can be overridden by concrete implementations

void AbstractNeuromodulatorStrategy::initialize(Brain* brain) {
    // Default implementation: do nothing
}

const char* AbstractNeuromodulatorStrategy::getName() const {
    return "AbstractNeuromodulatorStrategy";
}

std::string AbstractNeuromodulatorStrategy::getStatus() const {
    return "Abstract neuromodulator strategy (not implemented)";
}

void AbstractNeuromodulatorStrategy::enable(bool enable) {
    // Default implementation: do nothing
}

bool AbstractNeuromodulatorStrategy::isEnabled() const {
    return true;  // Default: enabled
}

void AbstractNeuromodulatorStrategy::reset() {
    // Default implementation: do nothing
}

const char* AbstractNeuromodulatorStrategy::getModulationSource() const {
    return "Generic";
}

const char* AbstractNeuromodulatorStrategy::getModulationTarget() const {
    return "Generic";
}

} // namespace nlm
