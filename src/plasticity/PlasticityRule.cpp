#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

} // namespace nlm
