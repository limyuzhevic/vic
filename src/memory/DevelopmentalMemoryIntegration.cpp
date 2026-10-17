#pragma once

#include "DevelopmentalMemoryIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct DevelopmentalMemoryIntegration::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr), plasticityFactor(1.0f), stabilityFactor(0.5f),
             isCriticalPeriod(false), memoryCapacityMultiplier(1.0f),
             criticalPeriodStepsRemaining(0) {}
    
    float plasticityFactor;
    float stabilityFactor;
    bool isCriticalPeriod;
    float memoryCapacityMultiplier;
    size_t criticalPeriodStepsRemaining;
};

DevelopmentalMemoryIntegration::DevelopmentalMemoryIntegration()
    : pImpl(new Impl)
    , brain_(nullptr)
    , stage_(DevelopmentalStage::Adult)
    , plasticityFactor_(1.0f)
    , stabilityFactor_(0.5f)
    , isCriticalPeriod_(false)
    , memoryCapacityMultiplier_(1.0f)
    , criticalPeriodStepsRemaining_(0)
{
}

DevelopmentalMemoryIntegration::~DevelopmentalMemoryIntegration() = default;

void DevelopmentalMemoryIntegration::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("DevelopmentalMemoryIntegration initialized");
}

void DevelopmentalMemoryIntegration::update(float dt) {
    if (!brain_) return;
    
    // Update developmental effects based on stage
    updateDevelopmentalParameters();
    
    // Apply critical period effects if active
    if (isCriticalPeriod_) {
        applyCriticalPeriodEffects();
    }
    
    // Adjust memory system parameters based on development
    adjustMemorySystemParameters();
    
    NLM_LOG_INFO("DevelopmentalMemoryIntegration updated. Stage: " + 
                std::to_string(static_cast<int>(stage_)) + 
                ", Plasticity: " + std::to_string(plasticityFactor_) + 
                ", Stability: " + std::to_string(stabilityFactor_));
}

void DevelopmentalMemoryIntegration::updateDevelopmentalParameters() {
    // Update plasticity and stability factors based on developmental stage
    switch (stage_) {
        case DevelopmentalStage::Embryonic:
            plasticityFactor_ = 1.5f;
            stabilityFactor_ = 0.3f;
            memoryCapacityMultiplier_ = 0.5f;
            isCriticalPeriod_ = false;
            break;
            
        case DevelopmentalStage::Infant:
            plasticityFactor_ = 1.2f;
            stabilityFactor_ = 0.4f;
            memoryCapacityMultiplier_ = 0.7f;
            isCriticalPeriod_ = false;
            break;
            
        case DevelopmentalStage::Juvenile:
            plasticityFactor_ = 1.0f;
            stabilityFactor_ = 0.6f;
            memoryCapacityMultiplier_ = 0.9f;
            isCriticalPeriod_ = false;
            break;
            
        case DevelopmentalStage::Adolescence:
            plasticityFactor_ = 0.8f;
            stabilityFactor_ = 0.8f;
            memoryCapacityMultiplier_ = 1.0f;
            isCriticalPeriod_ = true;
            criticalPeriodStepsRemaining_ = 1000;
            break;
            
        case DevelopmentalStage::Adult:
            plasticityFactor_ = 0.6f;
            stabilityFactor_ = 1.0f;
            memoryCapacityMultiplier_ = 1.0f;
            isCriticalPeriod_ = false;
            break;
    }
}

void DevelopmentalMemoryIntegration::applyCriticalPeriodEffects() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    auto* em = brain_->getEpisodicMemory();
    
    // During critical period, enhance learning from new experiences
    if (criticalPeriodStepsRemaining_ > 0) {
        --criticalPeriodStepsRemaining_;
    }
    
    // Enhanced memory integration during critical period
    for (size_t i = 0; i < em->getEpisodeCount(); ++i) {
        const auto* episode = em->getEpisode(i);
        
        // Increase relevance of episodes during critical period
        if (episode->reward > 0.0f) {
            em->updateRelevance(i, 0.05f);
        }
    }
}

void DevelopmentalMemoryIntegration::adjustMemorySystemParameters() {
    if (!brain_) return;
    
    // Adjust working memory capacity based on development
    if (brain_->getWorkingMemory()) {
        size_t baseCapacity = 100;
        size_t newCapacity = static_cast<size_t>(baseCapacity * memoryCapacityMultiplier_);
        brain_->getWorkingMemory()->setCapacity(newCapacity);
        brain_->getWorkingMemory()->setDecayRate(0.01f / plasticityFactor_);
    }
    
    // Adjust episodic memory consolidation based on development
    if (brain_->getEpisodicMemory()) {
        float consolidationThreshold = 0.5f * stabilityFactor_;
        // In full implementation, would pass threshold to consolidation
    }
    
    // Adjust neuromodulation sensitivity based on development
    if (brain_->getDopamine()) {
        // Dopamine sensitivity increases with development
        float sensitivity = 0.5f + plasticityFactor_ * 0.1f;
        // In full implementation, would adjust dopamine sensitivity
    }
}

} // namespace nlm