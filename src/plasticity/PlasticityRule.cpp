#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Note: The actual comprehensive Hebbian implementation is in Hebbian.cpp
// This file is kept for backward compatibility with the original placeholder

} // namespace nlm
