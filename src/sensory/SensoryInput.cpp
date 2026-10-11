// SensoryInput.cpp - Implementation
#include "SensoryInput.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

SensoryInput::SensoryInput(std::shared_ptr<Environment> environment)
    : environment_(environment), lastObservation_(nullptr) {}

SensoryInput::~SensoryInput() {}

void SensoryInput::update() {
    if (environment_) {
        lastObservation_ = environment_->getLastObservation();
    }
}

Observation* SensoryInput::getLastObservation() const {
    return lastObservation_.get();
}

void SensoryInput::setEnvironment(std::shared_ptr<Environment> env) {
    environment_ = env;
}

std::vector<float> SensoryInput::getSensoryFeatures() const {
    std::vector<float> features;
    if (lastObservation_) {
        features = lastObservation_->getFeatures();
    }
    return features;
}

float SensoryInput::getFeature(size_t index) const {
    if (lastObservation_ && index < lastObservation_->getFeatureCount()) {
        return lastObservation_->getFeature(index);
    }
    return 0.0f;
}

void SensoryInput::clear() {
    lastObservation_ = nullptr;
}

} // namespace nlm