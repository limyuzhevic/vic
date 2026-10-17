#pragma once

#include "NeuromodulatorMemoryIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct NeuromodulatorMemoryIntegration::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr), consolidationModulation(1.0f), replayModulation(1.0f),
             dopamineWeight(0.5f), acetylcholineWeight(0.3f),
             norepinephrineWeight(0.2f), serotoninWeight(0.0f) {}
    
    float consolidationModulation;
    float replayModulation;
    float dopamineWeight;
    float acetylcholineWeight;
    float norepinephrineWeight;
    float serotoninWeight;
};

NeuromodulatorMemoryIntegration::NeuromodulatorMemoryIntegration()
    : pImpl(new Impl)
    , brain_(nullptr)
    , consolidationModulation_(1.0f)
    , replayModulation_(1.0f)
    , dopamineWeight_(0.5f)
    , acetylcholineWeight_(0.3f)
    , norepinephrineWeight_(0.2f)
    , serotoninWeight_(0.0f)
{
}

NeuromodulatorMemoryIntegration::~NeuromodulatorMemoryIntegration() = default;

void NeuromodulatorMemoryIntegration::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuromodulatorMemoryIntegration initialized");
}

void NeuromodulatorMemoryIntegration::update(float dt) {
    if (!brain_) return;
    
    // Calculate neuromodulator effects on memory
    float totalConsolidationEffect = 0.0f;
    float totalReplayEffect = 0.0f;
    
    // Dopamine effect (reward-based modulation)
    if (brain_->getDopamine()) {
        float dopamineLevel = brain_->getDopamine()->getLevel();
        totalConsolidationEffect += dopamineLevel * dopamineWeight_;
        totalReplayEffect += dopamineLevel * dopamineWeight_ * 0.5f;  // Less effect on replay
    }
    
    // Acetylcholine effect (attention-based modulation)
    if (brain_->getAcetylcholine()) {
        float acetylcholineLevel = brain_->getAcetylcholine()->getLevel();
        totalConsolidationEffect += acetylcholineLevel * acetylcholineWeight_ * 1.5f;  // Strong effect on consolidation
        totalReplayEffect += acetylcholineLevel * acetylcholineWeight_ * 0.8f;
    }
    
    // Norepinephrine effect (arousal-based modulation)
    if (brain_->getNorepinephrine()) {
        float norepinephrineLevel = brain_->getNorepinephrine()->getLevel();
        totalConsolidationEffect += norepinephrineLevel * norepinephrineWeight_;
        totalReplayEffect += norepinephrineLevel * norepinephrineWeight_;
    }
    
    // Serotonin effect (mood-based modulation)
    if (brain_->getSerotonin()) {
        float serotoninLevel = brain_->getSerotonin()->getLevel();
        // Serotonin has complex effects - generally stabilizes memory
        totalConsolidationEffect += serotoninLevel * serotoninWeight_ * 0.5f;
        totalReplayEffect += serotoninLevel * serotoninWeight_ * 0.3f;
    }
    
    // Apply neuromodulator cross-talk effects
    if (brain_->getDopamine() && brain_->getAcetylcholine()) {
        float daACh = brain_->getDopamine()->getLevel() * brain_->getAcetylcholine()->getLevel();
        totalConsolidationEffect += daACh * 0.3f;  // Strong synergy for combined attention and reward
        totalReplayEffect += daACh * 0.2f;
    }
    
    // Store modulation values
    pImpl->consolidationModulation = totalConsolidationEffect;
    pImpl->replayModulation = totalReplayEffect;
    
    // Apply modulation to memory systems
    applyModulationToMemorySystems(totalConsolidationEffect, totalReplayEffect);
}

void NeuromodulatorMemoryIntegration::applyModulationToMemorySystems(float consolidationMod, float replayMod) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    auto* em = brain_->getEpisodicMemory();
    
    // Modulate consolidation threshold based on neuromodulators
    float originalThreshold = 0.5f;
    float modulatedThreshold = originalThreshold * (1.0f - consolidationMod * 0.3f);
    modulatedThreshold = std::clamp(modulatedThreshold, 0.1f, 0.9f);
    
    // Modulate replay strength
    float replayStrengthMultiplier = 1.0f + replayMod * 0.5f;
    replayStrengthMultiplier = std::clamp(replayStrengthMultiplier, 0.5f, 2.0f);
    
    // Update sleep consolidation system if available
    // In a full implementation, these systems would have references to each other
    
    NLM_LOG_INFO("Neuromodulator modulation applied: consolidation=" + 
                std::to_string(consolidationMod) + ", replay=" + 
                std::to_string(replayMod));
}

} // namespace nlm