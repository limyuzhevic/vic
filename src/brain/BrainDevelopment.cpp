#pragma once

#include "BrainDevelopment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "NeuralRegion.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../development/Maturation.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Pruning.hpp"
#include <algorithm>

namespace nlm {

struct BrainDevelopment::Impl {
    std::shared_ptr<Config> config;
    
    // Development systems
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Maturation> maturation;
    std::unique_ptr<Synaptogenesis> synaptogenesis;
    std::unique_ptr<Pruning> pruning;
    
    // Development state
    DevelopmentalStage currentStage;
    float developmentProgress;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , currentStage(DevelopmentalStage::Initial)
        , developmentProgress(0.0f)
    {}
};

BrainDevelopment::BrainDevelopment(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainDevelopment::~BrainDevelopment() = default;

bool BrainDevelopment::initialize() {
    NLM_LOG_INFO("Initializing Brain Development system...");
    
    // Initialize development systems
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    pImpl->maturation = std::make_unique<Maturation>();
    pImpl->synaptogenesis = std::make_unique<Synaptogenesis>();
    pImpl->pruning = std::make_unique<Pruning>();
    
    // Initialize all systems
    pImpl->developmentSystem->initialize(nullptr);
    
    NLM_LOG_INFO("Brain Development system initialization complete");
    return true;
}

void BrainDevelopment::update(float timestep, Timestamp time, SimulationStep step) {
    // Update development effects
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(nullptr, *pImpl->config, timestep);
    }
    
    // Update development progress
    pImpl->developmentProgress += timestep;
    
    // Update developmental stage based on progress
    if (pImpl->developmentProgress > 1000.0f) {  // Example thresholds
        pImpl->currentStage = DevelopmentalStage::CriticalPeriod;
    } else if (pImpl->developmentProgress > 2000.0f) {
        pImpl->currentStage = DevelopmentalStage::Maturation;
    } else if (pImpl->developmentProgress > 3000.0f) {
        pImpl->currentStage = DevelopmentalStage::Adult;
    }
}

void BrainDevelopment::applyToPlasticity(float& plasticityMod) {
    if (!pImpl->developmentSystem) return;
    
    switch (pImpl->currentStage) {
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
}

void BrainDevelopment::applyToNeurons(NeuralRegion* region) {
    if (!region || !pImpl->maturation) return;
    
    // Apply maturation effects to neurons
    pImpl->maturation->applyToNeurons(region, *pImpl->config);
    
    // Apply synaptogenesis
    pImpl->synaptogenesis->applyToRegion(region, *pImpl->config);
    
    // Apply pruning
    pImpl->pruning->applyToRegion(region, *pImpl->config);
}

void BrainDevelopment::triggerCriticalPeriod() {
    pImpl->currentStage = DevelopmentalStage::CriticalPeriod;
    NLM_LOG_INFO("Critical period triggered");
}

void BrainDevelopment::triggerMaturation() {
    pImpl->currentStage = DevelopmentalStage::Maturation;
    NLM_LOG_INFO("Maturation triggered");
}

DevelopmentSystem* BrainDevelopment::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

Maturation* BrainDevelopment::getMaturation() {
    return pImpl->maturation.get();
}

Synaptogenesis* BrainDevelopment::getSynaptogenesis() {
    return pImpl->synaptogenesis.get();
}

Pruning* BrainDevelopment::getPruning() {
    return pImpl->pruning.get();
}

DevelopmentalStage BrainDevelopment::getCurrentStage() const {
    return pImpl->currentStage;
}

float BrainDevelopment::getDevelopmentProgress() const {
    return pImpl->developmentProgress;
}

void BrainDevelopment::logStatus() const {
    NLM_LOG_INFO("=== Brain Development Status ===");
    NLM_LOG_INFO("Current developmental stage: " + std::to_string(static_cast<int>(pImpl->currentStage)));
    NLM_LOG_INFO("Development progress: " + std::to_string(pImpl->developmentProgress));
    
    if (pImpl->developmentSystem) {
        NLM_LOG_INFO("Development system active: yes");
    }
    if (pImpl->maturation) {
        NLM_LOG_INFO("Maturation active: yes");
    }
    if (pImpl->synaptogenesis) {
        NLM_LOG_INFO("Synaptogenesis active: yes");
    }
    if (pImpl->pruning) {
        NLM_LOG_INFO("Pruning active: yes");
    }
}

BrainDevelopment::BrainDevelopment(BrainDevelopment&& other) noexcept : pImpl(std::move(other.pImpl)) {}

BrainDevelopment& BrainDevelopment::operator=(BrainDevelopment&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

} // namespace nlm
