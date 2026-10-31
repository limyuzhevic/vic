// DevelopmentManager implementation
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

DevelopmentManager::DevelopmentManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config)
    : brain_(brain), config_(config), currentStage_(DevelopmentalStage::Initial), 
      plasticityModulation_(1.0f), developmentCounter_(0) {
    developmentSystem_ = std::make_unique<DevelopmentSystem>();
}

bool DevelopmentManager::initialize() {
    NLM_LOG_INFO("Initializing DevelopmentManager...");
    
    developmentSystem_->initialize(brain_.get());
    
    NLM_LOG_INFO("DevelopmentManager initialized successfully");
    return true;
}

void DevelopmentManager::reset() {
    NLM_LOG_INFO("Resetting DevelopmentManager...");
    developmentSystem_->reset();
    currentStage_ = DevelopmentalStage::Initial;
    plasticityModulation_ = 1.0f;
    developmentCounter_ = 0;
    NLM_LOG_INFO("DevelopmentManager reset complete");
}

void DevelopmentManager::update() {
    // Development updates are handled during simulation steps
}

void DevelopmentManager::logStatus() const {
    NLM_LOG_INFO("=== DevelopmentManager Status ===");
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(currentStage_)));
    NLM_LOG_INFO("Plasticity modulation: " + std::to_string(plasticityModulation_));
    NLM_LOG_INFO("Development counter: " + std::to_string(developmentCounter_));
}

void DevelopmentManager::applyDevelopment(SimulationStep step, Timestamp time) {
    developmentCounter_++;
    
    if (developmentCounter_ >= config_.developmentInterval) {
        developmentSystem_->update(brain_.get(), *brain_->getRandomGenerator(), time * 1000);
        developmentCounter_ = 0;
        
        // Update plasticity modulation based on developmental stage
        updatePlasticityModulation(step, time);
        
        NLM_LOG_INFO("Development system updated - stage: " + std::to_string(static_cast<int>(currentStage_)));
    }
}

void DevelopmentManager::updatePlasticityModulation(SimulationStep step, Timestamp time) {
    float plasticityMod = 1.0f;
    
    switch (currentStage_) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = config_.criticalPeriodFactor;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = config_.initialPlasticity;  // Stable
            break;
    }
    
    plasticityModulation_ = plasticityMod;
    
    // Apply development effects on plasticity systems
    if (brain_->getStructuralPlasticity()) {
        brain_->getStructuralPlasticity()->setSynaptogenesisRate(config_.synaptogenesisRate * plasticityMod);
        brain_->getStructuralPlasticity()->setPruningRate(config_.pruningRate * (2.0f - plasticityMod));
    }
}

DevelopmentalStage DevelopmentManager::getDevelopmentalStage() const {
    return currentStage_;
}

void DevelopmentManager::setDevelopmentalStage(DevelopmentalStage stage) {
    currentStage_ = stage;
    NLM_LOG_INFO("Developmental stage set to: " + std::to_string(static_cast<int>(stage)));
}

float DevelopmentManager::getPlasticityModulation() const {
    return plasticityModulation_;
}

} // namespace nlm