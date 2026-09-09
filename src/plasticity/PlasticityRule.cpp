#include "PlasticityRule.hpp"

namespace nlm {
    // Base PlasticityRule implementation
    bool PlasticityRule::isEnabled() const {
        return enabled_;
    }

    void PlasticityRule::setEnabled(bool enabled) {
        enabled_ = enabled;
    }
}
