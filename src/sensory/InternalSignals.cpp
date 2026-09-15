#include "InternalSignals.hpp"
#include <algorithm>

namespace nlm {

struct InternalSignals::Impl {
    std::vector<float> signals;
    
    // Homeostatic system signals
    std::vector<float> hungerLevels;
    std::vector<float> thirstLevels;
    std::vector<float> fatigueLevels;
    std::vector<float> metabolicRates;
    std::vector<float> stressLevels;
    
    // Predictive coding
    std::vector<float> predictionErrors;
    
    // State
    float homeostaticSetpoint;
    float interoceptiveNoise;
    
    Impl() : homeostaticSetpoint(0.0f), interoceptiveNoise(0.1f) {}
};

InternalSignals::InternalSignals() : pImpl(new Impl) {}

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

std::unique_ptr<SensoryInput> InternalSignals::clone() const {
    auto clone = std::make_unique<InternalSignals>();
    for (float sig : pImpl->signals) {
        clone->addSignal(sig);
    }
    clone->setHomeostaticSetpoint(pImpl->homeostaticSetpoint);
    clone->updateInteroceptiveNoise(pImpl->interoceptiveNoise);
    return clone;
}

void InternalSignals::addSignal(float value) {
    pImpl->signals.push_back(value);
}

void InternalSignals::clearSignals() {
    pImpl->signals.clear();
}

const std::vector<float>& InternalSignals::getHomeostaticSignals() const {
    return pImpl->signals;
}

const std::vector<float>& InternalSignals::getHungerLevels() const {
    return pImpl->hungerLevels;
}

const std::vector<float>& InternalSignals::getThirstLevels() const {
    return pImpl->thirstLevels;
}

const std::vector<float>& InternalSignals::getFatigueLevels() const {
    return pImpl->fatigueLevels;
}

const std::vector<float>& InternalSignals::getMetabolicRates() const {
    return pImpl->metabolicRates;
}

const std::vector<float>& InternalSignals::getStressLevels() const {
    return pImpl->stressLevels;
}

const std::vector<float>& InternalSignals::getPredictionErrors() const {
    return pImpl->predictionErrors;
}

void InternalSignals::setHomeostaticSetpoint(float setpoint) {
    pImpl->homeostaticSetpoint = setpoint;
}

void InternalSignals::updateInteroceptiveNoise(float noise) {
    pImpl->interoceptiveNoise = std::clamp(noise, 0.0f, 1.0f);
}

} // namespace nlm