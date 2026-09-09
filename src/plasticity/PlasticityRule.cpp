#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PlasticityRule::Impl {
    bool enabled;
    
    Impl() : enabled(true) {}
};

PlasticityRule::PlasticityRule() : pImpl(new Impl) {}

PlasticityRule::~PlasticityRule() = default;

bool PlasticityRule::isEnabled() const {
    return pImpl->enabled;
}

void PlasticityRule::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
}

} // namespace nlm