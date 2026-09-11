// BrainDevelopmentIntegration.cpp - Developmental system integration implementation
// Contains developmental system integration functionality

#include "BrainDevelopmentIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Implementation of BrainDevelopmentIntegration

BrainDevelopmentIntegration::BrainDevelopmentIntegration(BrainCore* core) : pImpl(new Impl(core)) {}

BrainDevelopmentIntegration::~BrainDevelopmentIntegration() = default;

BrainDevelopmentIntegration::BrainDevelopmentIntegration(BrainDevelopmentIntegration&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainDevelopmentIntegration& BrainDevelopmentIntegration::operator=(BrainDevelopmentIntegration&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainDevelopmentIntegration::initialize() {
    NLM_LOG_INFO("Initializing Brain Development Integration...");
    
    if (!pImpl->brainCore) {
        NLM_LOG_ERROR("Cannot initialize development integration - no brain core available");
        return false;
    }
    
    pImpl->developmentSystem = pImpl->brainCore->getDevelopmentSystem();
    pImpl->currentStage = DevelopmentalStage::Initial;
    
    NLM_LOG_INFO("Brain Development Integration initialized successfully");
    return true;
}

void BrainDevelopmentIntegration::update(BrainCore* brain, RandomGenerator& rng, float timeDelta) {
    if (!brain || !pImpl->developmentSystem) return;
    
    // Update development system
    pImpl->developmentSystem->update(brain, rng, timeDelta);
    
    // Update developmental stage based on development progress
    updateDevelopmentStage();
    
    // Update brain with development effects
    applyDevelopmentEffects();
    
    // Increment development cycles
    pImpl->developmentCycles++;
}

DevelopmentSystem* BrainDevelopmentIntegration::getDevelopmentSystem() {
    return pImpl->developmentSystem;
}

DevelopmentalStage BrainDevelopmentIntegration::getDevelopmentalStage() const {
    return pImpl->currentStage;
}

void BrainDevelopmentIntegration::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->currentStage = stage;
}

float BrainDevelopmentIntegration::calculatePlasticityModulation(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 1.0f;  // High plasticity
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;
        case DevelopmentalStage::Maturation:
            return 0.5f;
        case DevelopmentalStage::Adult:
            return 0.2f;  // Stable
        default:
            return 0.5f;
    }
}

void BrainDevelopmentIntegration::clear() {
    pImpl->currentStage = DevelopmentalStage::Initial;
    pImpl->developmentCycles = 0;
    pImpl->developmentalProgress = 0.0f;
}

size_t BrainDevelopmentIntegration::getDevelopmentCycles() const {
    return pImpl->developmentCycles;
}

float BrainDevelopmentIntegration::getDevelopmentalProgress() const {
    return pImpl->developmentalProgress;
}

void BrainDevelopmentIntegration::Impl::Impl(BrainCore* core) : brainCore(core), developmentSystem(nullptr),
    currentStage(DevelopmentalStage::Initial), developmentCycles(0), developmentalProgress(0.0f) {}

void BrainDevelopmentIntegration::updateDevelopmentStage() {
    // Update developmental stage based on development progress
    // This would typically be driven by the development system
    // For now, we just keep the stage as is
}

void BrainDevelopmentIntegration::updateSynaptogenesis() {
    // Synaptogenesis is handled by the development system
    if (pImpl->developmentSystem) {
        // Trigger synaptogenesis
    }
}

void BrainDevelopmentIntegration::updatePruning() {
    // Pruning is handled by the development system
    if (pImpl->developmentSystem) {
        // Trigger pruning
    }
}

void BrainDevelopmentIntegration::updateMaturation() {
    // Maturation is handled by the development system
    if (pImpl->developmentSystem) {
        // Trigger maturation
    }
}

void BrainDevelopmentIntegration::applyDevelopmentEffects() {
    // Apply development effects on brain properties
    if (pImpl->brainCore) {
        // Development affects structural plasticity rates
        auto* sp = pImpl->brainCore->getStructuralPlasticity();
        if (sp) {
            float plasticityMod = calculatePlasticityModulation(pImpl->currentStage);
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void BrainDevelopmentIntegration::calculateDevelopmentStats() const {}

} // namespace nlm

