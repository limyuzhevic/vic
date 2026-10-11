// SensoryPercept.cpp - Implementation
#include "SensoryPercept.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

SensoryPercept::SensoryPercept() : timestamp(0.0), reliability(1.0f) {}

SensoryPercept::SensoryPercept(std::shared_ptr<Observation> observation)
    : observation_(observation), timestamp(0.0), reliability(1.0f) {}

SensoryPercept::~SensoryPercept() {}

void SensoryPercept::setObservation(std::shared_ptr<Observation> obs) {
    observation_ = obs;
}

Observation* SensoryPercept::getObservation() const {
    return observation_.get();
}

std::vector<float> SensoryPercept::getFeatures() const {
    std::vector<float> features;
    if (observation_) {
        features = observation_->getFeatures();
    }
    return features;
}

float SensoryPercept::getFeature(size_t index) const {
    if (observation_ && index < observation_->getFeatureCount()) {
        return observation_->getFeature(index);
    }
    return 0.0f;
}

float SensoryPercept::getFeatureByName(const std::string& name) const {
    if (observation_) {
        return observation_->getFeatureByName(name);
    }
    return 0.0f;
}

void SensoryPercept::updateReliability(float amount) {
    reliability = std::max(0.0f, std::min(1.0f, reliability + amount));
}

} // namespace nlm