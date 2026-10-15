#include "ExperimentLifecycleManager.hpp"

namespace nlm {

ExperimentLifecycleManager& ExperimentLifecycleManager::getInstance() {
    static ExperimentLifecycleManager instance;
    return instance;
}

// Base implementation stubs for ExperimentLifecycleManager
// These would be fully implemented with proper state management,
// resource handling, and lifecycle control logic

} // namespace nlm