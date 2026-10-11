// Maturation.cpp - Implementation
#include "Maturation.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

Maturation::Maturation() : enabled_(true), thresholdAge_(10.0f), 
                           refractoryPeriod_(1.0f), currentCooldown_(0.0f) {}

Maturation::~Maturation() {}

void Maturation::update(float dt, Brain* brain) {
    if (!enabled_ || !brain) return;
    
    currentCooldown_ -= dt;
    
    if (currentCooldown_ <= 0.0f) {
        performMaturation(brain, dt);
    }
}

void Maturation::performMaturation(Brain* brain, float dt) {
    if (!brain) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    
    for (auto* neuron : neurons) {
        if (neuron->getAge() >= thresholdAge_) {
            // Perform maturation on neuron
            matureNeuron(neuron, dt);
        }
    }
    
    currentCooldown_ = refractoryPeriod_;
}

void Maturation::matureNeuron(Neuron* neuron, float dt) {
    if (!neuron) return;
    
    // Increase neuron properties based on maturation
    // Example: increase maximum firing rate, decrease threshold, etc.
    
    float currentRate = neuron->getFiringRate();
    float newRate = currentRate * 1.1f; // 10% increase
    neuron->setFiringRate(newRate);
    
    // Decrease threshold slightly (easier to fire)
    float currentThreshold = neuron->getThreshold();
    float newThreshold = currentThreshold * 0.95f; // 5% decrease
    neuron->setThreshold(newThreshold);
}

void Maturation::setEnabled(bool enabled) {
    enabled_ = enabled;
}

bool Maturation::isEnabled() const {
    return enabled_;
}

void Maturation::setThresholdAge(float age) {
    thresholdAge_ = age;
}

float Maturation::getThresholdAge() const {
    return thresholdAge_;
}

void Maturation::setRefractoryPeriod(float period) {
    refractoryPeriod_ = period;
}

float Maturation::getRefractoryPeriod() const {
    return refractoryPeriod_;
}

} // namespace nlm