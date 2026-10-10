#include "InternalSignals.hpp"
#include <algorithm>

namespace nlm {

struct InternalSignals::Impl {
    std::vector<float> signals;
    std::vector<float> homeostatic;
    
    Impl() = default;
};

InternalSignals::InternalSignals() : pImpl(new Impl) {}

InternalSignals::~InternalSignals() = default;

InternalSignals::InternalSignals(InternalSignals&& other) noexcept : pImpl(std::move(other.pImpl)) {}

InternalSignals& InternalSignals::operator=(InternalSignals&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

const char* InternalSignals::getType() const {
    return "InternalSignals";
}

const std::vector<float>& InternalSignals::getData() const {
    return pImpl->signals;
}

size_t InternalSignals::getDimensions() const {
    return pImpl->signals.size();
}

std::unique_ptr<SensoryInput> InternalSignals::clone() const {
    auto clone = std::make_unique<InternalSignals>(*this);
    clone->setTimestamp(getTimestamp());
    return clone;
}

void InternalSignals::addSignal(float value) {
    pImpl->signals.push_back(value);
    
    // Generate homeostatic signals based on current signals
    float sum = 0.0f;
    for (float signal : pImpl->signals) {
        sum += signal;
    }
    float mean = pImpl->signals.empty() ? 0.0f : sum / pImpl->signals.size();
    
    // Generate simple homeostatic signals
    pImpl->homeostatic.clear();
    pImpl->homeostatic.push_back(1.0f / (1.0f + std::abs(mean)));  // Allostatic load (inversely related to mean)
    pImpl->homeostatic.push_back(0.5f * (1.0f + std::tanh(mean)));  // Metabolic state
}

void InternalSignals::clearSignals() {
    pImpl->signals.clear();
    pImpl->homeostatic.clear();
}

} // namespace nlm
