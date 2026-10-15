// InternalSignals implementation for NLM - InternalSignals.cpp
// This implements the InternalSignals class defined in SensoryInput.hpp

#include "InternalSignals.hpp"

namespace nlm {

struct InternalSignals::Impl {
    std::vector<float> signals_;
    
    Impl() {}
};

InternalSignals::InternalSignals() : pImpl(new Impl) {}

InternalSignals::~InternalSignals() = default;

const char* InternalSignals::getType() const {
    return "InternalSignals";
}

const std::vector<float>& InternalSignals::getData() const {
    return pImpl->signals_;
}

size_t InternalSignals::getDimensions() const {
    return pImpl->signals_.size();
}

std::unique_ptr<SensoryInput> InternalSignals::clone() const {
    auto clone = std::make_unique<InternalSignals>();
    clone->pImpl->signals_ = pImpl->signals_;
    clone->setTimestamp(timestamp_);
    return clone;
}

void InternalSignals::addSignal(float value) {
    pImpl->signals_.push_back(value);
}

void InternalSignals::clearSignals() {
    pImpl->signals_.clear();
}

} // namespace nlm