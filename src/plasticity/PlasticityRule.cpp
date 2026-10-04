#include "PlasticityRule.hpp"
#include <algorithm>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

} // namespace nlm
