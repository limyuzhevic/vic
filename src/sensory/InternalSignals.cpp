#include "InternalSignals.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct InternalSignals::Impl {
    std::vector<float> signals;
    
    Impl() = default;
};

InternalSignals::InternalSignals() : pImpl(new Impl) {
    timestamp_ = 0.0;
}

InternalSignals::~InternalSignals() = default;

const char* InternalSignals::getType() const {
    return "InternalSignals";
}

const std::vector<float>& InternalSignals::getData() const {
    return pImpl->signals;
}

size_t InternalSignals::getDimensions() const {
    return pImpl->signals.size();
}

double InternalSignals::getTimestamp() const {
    return timestamp_;
}

void InternalSignals::setTimestamp(double timestamp) {
    timestamp_ = timestamp;
}

std::unique_ptr<SensoryInput> InternalSignals::clone() const {
    auto cloned = std::make_unique<InternalSignals>();
    cloned->pImpl->signals = pImpl->signals;
    cloned->timestamp_ = timestamp_;
    return cloned;
}

void InternalSignals::addSignal(float value) {
    pImpl->signals.push_back(value);
}

void InternalSignals::clearSignals() {
    pImpl->signals.clear();
}

size_t InternalSignals::getSignalCount() const {
    return pImpl->signals.size();
}

float InternalSignals::getSignal(size_t index) const {
    if (index < pImpl->signals.size()) {
        return pImpl->signals[index];
    }
    return 0.0f;
}

} // namespace nlm
