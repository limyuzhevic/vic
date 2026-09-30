// Development processes implementation for Brain.cpp
// Contains: DevelopmentSystem, developmental stages, plasticity adaptation

#pragma once

#include "Development.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void DevelopmentSystem::initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    config_ = config;
    brain_ = brain;
    
    // Initialize development system
    developmentStage_ = DevelopmentalStage::Initial;
    
    NLM_LOG_INFO("DevelopmentSystem initialized");
}

void DevelopmentSystem::processStep(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    if (!brain_ || !brain_->getDevelopmentSystem()) return;
    
    // Update developmental stage based on simulation steps
    updateDevelopmentalStage(currentStep);
    
    // Apply developmental effects on plasticity
    applyDevelopmentEffects(currentStep, currentTime, timestep);
}

void DevelopmentSystem::updateDevelopmentalStage(SimulationStep currentStep) {
    // Determine developmental stage based on simulation progress
    if (currentStep < 1000) {
        developmentStage_ = DevelopmentalStage::Initial;
    } else if (currentStep < 5000) {
        developmentStage_ = DevelopmentalStage::CriticalPeriod;
    } else if (currentStep < 10000) {
        developmentStage_ = DevelopmentalStage::Maturation;
    } else {
        developmentStage_ = DevelopmentalStage::Adult;
    }
    
    NLM_LOG_INFO("Developmental stage updated to: " + std::to_string(static_cast<int>(developmentStage_)));
}

void DevelopmentSystem::applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    if (!brain_ || !brain_->getStructuralPlasticity()) return;
    
    auto* sp = brain_->getStructuralPlasticity();
    if (!sp) return;
    
    float plasticityMod = 1.0f;
    
    switch (developmentStage_) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;  // Stable
            break;
    }
    
    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    
    // Apply development effects on other systems
    applyDevelopmentToMemorySystems();
    applyDevelopmentToCognitiveSystems();
}

void DevelopmentSystem::applyDevelopmentToMemorySystems() {
    if (!brain_) return;
    
    // Development affects memory consolidation and retention
    if (brain_->getEpisodicMemory()) {
        float consolidationRate = (developmentStage_ == DevelopmentalStage::Adult) ? 0.5f : 0.3f;
        brain_->getEpisodicMemory()->consolidate(consolidationRate);
    }
}

void DevelopmentSystem::applyDevelopmentToCognitiveSystems() {
    if (!brain_ || !brain_->getAttention()) return;
    
    // Development affects attentional control and planning
    float attentionMod = (developmentStage_ == DevelopmentalStage::Adult) ? 1.0f : 0.7f;
    
    // Adjust attentional parameters based on developmental stage
    if (developmentStage_ == DevelopmentalStage::Adult) {
        // Mature attentional control
        brain_->getAttention()->setInhibitionStrength(0.5f);
        brain_->getAttention()->setExcitationStrength(1.5f);
    } else {
        // Developing attentional control
        brain_->getAttention()->setInhibitionStrength(0.3f);
        brain_->getAttention()->setExcitationStrength(1.2f);
    }
}

DevelopmentalStage DevelopmentSystem::getDevelopmentalStage() const {
    return developmentStage_;
}

void DevelopmentSystem::setDevelopmentalStage(DevelopmentalStage stage) {
    developmentStage_ = stage;
}

} // namespace nlm
