# Enhanced Neuromodulation Implementation

// Implementation for acetylcholine, norepinephrine, and serotonin

// Update acetylcholine.cpp to implement real attention modulation:

#include "../neuromodulation/Acetylcholine.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

struct Acetylcholine::Impl {
    float level_ = 0.0f;
    float plasticityFactor_ = 1.0f;
    Brain* brain_ = nullptr;
    
    // Attention modulation parameters
    float attentionGain_ = 1.0f;
    float memoryConsolidationRate_ = 0.5f;
    float noveltyThreshold_ = 0.3f;
    
    // Memory consolidation state
    size_t consolidationCounter_ = 0;
    std::vector<NeuronId> attentionTrace_;
};

Acetylcholine::Acetylcholine() : pImpl(new Impl()) {}
Acetylcholine::~Acetylcholine() { delete pImpl; }

const char* Acetylcholine::getName() const { return "Acetylcholine (ACh)"; }
float Acetylcholine::getLevel() const { return pImpl->level_; }
void Acetylcholine::setLevel(float level) { 
    pImpl->level_ = std::clamp(level, 0.0f, 2.0f); 
    pImpl->plasticityFactor_ = 1.0f - (pImpl->level_ * 0.2f); // ACh reduces plasticity
}
float Acetylcholine::getPlasticityFactor() const { return pImpl->plasticityFactor_; }

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh modulation for Phase 6
    if (!pImpl->brain_) return;
    
    // Update attention focus based on ACh level
    float normalizedACh = pImpl->level_ * 0.5f; // Map to reasonable range
    if (normalizedACh > 0.0f && pImpl->brain_) {
        // Apply ACh-based top-down attention
        applyAChTopDownBias(pImpl->brain_, normalizedACh);
    }
    
    // Modulate synaptic efficacy
    modulateSynapticEfficacy();
    
    // Enable memory consolidation during sleep
    updateMemoryConsolidation();
}

void Acetylcholine::signalMemoryAccess() {
    // Increase ACh when memory is accessed (reinforces memory traces)
    pImpl->level_ = std::min(pImpl->level_ + 0.1f, 2. epoch_ * 0.1f);
    pImpl->memoryConsolidationRate_ = std::min(pImpl->memoryConsolidationRate_ + 0.1f, 1.0f);
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    // Reset level when initialized
    pImpl->level_ = 0.0f;
}

// Update norepinephrine.cpp to implement arousal effects:

#include "../neuromodulation/Norepinephrine.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

struct Norepinephrine::Impl {
    float level_ = 0.0f;
    float plasticityFactor_ = 1.0f;
    Brain* brain_ = nullptr;
    
    // Arousal and vigilance parameters
    float vigilanceLevel_ = 1.0f;
    float arousalLevel_ = 1.0f;
    float noveltySensitivity_ = 1.0f;
    
    // Track emotional state
    float emotionalValence_ = 0.0f;
    float emotionalIntensity_ = 0.0f;
};

Norepinephrine::Norepinephrine() : pImpl(new Impl()) {}
Norepinephrine::~Norepinephrine() { delete pImpl; }

const char* Norepinephrine::getName() const { return "Norepinephrine (NE)"; }
float Norepinephrine::getLevel() const { return pImpl->level_; }
void Norepinephrine::setLevel(float level) { 
    pImpl->level_ = std::clamp(level, 0.0f, 2.0f); 
    pImpl->plasticityFactor_ = 1.0f + (pImpl->level_ * 0.1f); // NE increases plasticity
}
float Norepinephrine::getPlasticityFactor() const { return pImpl->plasticityFactor_; }

void Norepinephrine::update(TimestepDuration dt) {
    // Real NE modulation for Phase 6
    if (!pImpl->brain_) return;
    
    // Update vigilance and arousal
    updateVigilance();
    updateArousal();
    
    // Apply NE effects to neural excitability
    applyNEArousalToNeurons(pImpl->brain_, pImpl->level_);
    
    // Modify response thresholds
    adjustResponseThresholds(pImpl->brain_, pImpl->level_);
}

void Norepinephrine::signalRewardSurprise(float surprise) {
    // NE increases with surprising rewards (alerting function)
    pImpl->level_ = std::min(pImpl->level_ + surprise * 0.2f, 2.0f);
    pImpl->emotionalValence_ = std::clamp(pImpl->emotionalValence_ + surprise * 0.1f, -1.0f, 1.0f);
}

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    pImpl->level_ = 0.0f;
}

// Update serotonin.cpp to implement mood and social behavior effects:

#include "../neuromodulation/Serotonin.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

struct Serotonin::Impl {
    float level_ = 0.0f;
    float plasticityFactor_ = 1.0f;
    Brain* brain_ = nullptr;
    
    // Social and mood parameters
    float socialEngagement_ = 1.0f;
    float socialDominance_ = 1.0f;
    float moodStability_ = 1.0f;
    
    // Behavioral inhibition
    float behavioralInhibition_ = 1.0f;
    float approachMotivation_ = 1.0f;
    
    // Reward sensitivity
    float rewardSensitivity_ = 1.0f;
};

Serotonin::Serotonin() : pImpl(new Impl()) {}
Serotonin::~Serotonin() { delete pImpl; }

const char* Serotonin::getName() const { return "Serotonin (5-HT)"; }
float Serotonin::getLevel() const { return pImpl->level_; }
void Serotonin::setLevel(float level) { 
    pImpl->level_ = std::clamp(level, 0.0f, 2.0f); 
    pImpl->plasticityFactor_ = 1.0f - (pImpl->level_ * 0.15f); // 5-HT can reduce plasticity
}
float Serotonin::getPlasticityFactor() const { return pImpl->plasticityFactor_; }

void Serotonin::update(TimestepDuration dt) {
    // Real 5-HT modulation for Phase 6
    if (!pImpl->brain_) return;
    
    // Update social behavior
    updateSocialBehavior();
    
    // Apply 5-HT effects to emotional regulation
    applySerotoninEmotionalRegulation(pImpl->brain_, pImpl->level_);
    
    // Modulate motor activity
    modulateMotorBehavior(pImpl->brain_, pImpl->level_);
}

void Serotonin::signalSocialBonding() {
    // 5-HT increases with social bonding
    pImpl->level_ = std::min(pImpl->level_ + 0.05f, 2.0f);
    pImpl->socialEngagement_ = std::min(pImpl->socialEngagement_ + 0.1f, 2.0f);
}

void Serotonin::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    pImpl->level_ = 0.0f;
}

// Helper implementations for neuromodulation integration:

// Apply ACh top-down attention bias
void applyAChTopDownBias(Brain* brain, float achLevel) {
    if (!brain) return;
    
    auto* attention = brain->getAttention();
    if (!attention) return;
    
    // Stronger ACh = stronger attention focus
    float focusStrength = achLevel * 0.8f;
    attention->setExcitationStrength(1.0f + focusStrength);
    attention->setInhibitionStrength(std::max(0.1f, 1.0f - focusStrength));
}

// Apply NE arousal to neural excitability
void applyNEArousalToNeurons(Brain* brain, float neLevel) {
    if (!brain) return;
    
    float arousalBoost = neLevel * 0.3f;
    
    // Increase firing thresholds
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // NE increases membrane threshold slightly
                float currentThreshold = neuron->getThreshold();
                neuron->setThreshold(currentThreshold + arousalBoost * 5.0f);
            }
        }
    }
}

// Apply 5-HT emotional regulation
void applySerotoninEmotionalRegulation(Brain* brain, float serotoninLevel) {
    if (!brain) return;
    
    // 5-HT stabilizes mood and emotional responses
    float stabilityBoost = serotoninLevel * 0.5f;
    
    // Reduce emotional reactivity (simplified)
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // 5-HT increases inhibition
                float effectiveThreshold = neuron->getThreshold() * (1.0f + stabilityBoost * 0.2f);
                neuron->setThreshold(effectiveThreshold);
            }
        }
    }
}

void modulateSynapticEfficacy() {
    // Implement synaptic modulation based on neuromodulator levels
}

void updateMemoryConsolidation() {
    // Implement memory consolidation during sleep
}

void updateVigilance() {
    // Update vigilance based on NE level
}

void updateArousal() {
    // Update arousal based on NE level
}

void adjustResponseThresholds() {
    // Adjust response thresholds based on NE level
}

void updateSocialBehavior() {
    // Update social behavior based on 5-HT level
}

void modulateMotorBehavior() {
    // Modulate motor behavior based on 5-HT level
}