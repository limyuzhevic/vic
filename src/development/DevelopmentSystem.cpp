// DevelopmentSystem.cpp - Implementation
#include "DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

DevelopmentSystem::DevelopmentSystem() : enabled_(true), structuralPlasticityEnabled_(true),
                                        age_(0.0), plasticityDecayRate_(0.001f) {}

DevelopmentSystem::~DevelopmentSystem() {}

void DevelopmentSystem::update(float dt) {
    if (!enabled_) return;
    
    age_ += dt;
    
    // Update developmental plasticity
    currentPlasticity_ = initialPlasticity_ * std::pow(1.0f - plasticityDecayRate_, age_);
    
    // Age-dependent maturation
    if (age_ >= maturationThreshold_) {
        onMaturation();
    }
}

void DevelopmentSystem::onMaturation() {
    if (!maturationCallback_) return;
    
    maturationCallback_(age_);
}

void DevelopmentSystem::setMaturationCallback(std::function<void(float)> callback) {
    maturationCallback_ = callback;
}

void DevelopmentSystem::setStructuralPlasticityEnabled(bool enabled) {
    structuralPlasticityEnabled_ = enabled;
}

void DevelopmentSystem::setDevelopmentEnabled(bool enabled) {
    enabled_ = enabled;
}

float DevelopmentSystem::getAge() const {
    return age_;
}

float DevelopmentSystem::getPlasticity() const {
    return currentPlasticity_;
}

bool DevelopmentSystem::isStructuralPlasticityEnabled() const {
    return structuralPlasticityEnabled_;
}

bool DevelopmentSystem::isDevelopmentEnabled() const {
    return enabled_;
}

} // namespace nlm