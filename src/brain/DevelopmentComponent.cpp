// DevelopmentComponent.cpp - Implementation of development component
#include "DevelopmentComponent.h"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentComponent::Impl {
    Brain* brain_ = nullptr;
    double developmentalAge_ = 0.0;
    bool stageAdvanced_ = false;
};

DevelopmentComponent::DevelopmentComponent() : pImpl(std::make_unique<Impl>()) {}

DevelopmentComponent::~DevelopmentComponent() = default;

bool DevelopmentComponent::initialize(Brain* brain) {
    if (!brain) return false;
    
    pImpl->brain_ = brain;
    pImpl->developmentalAge_ = 0.0;
    pImpl->stageAdvanced_ = false;
    
    // Initialize development system
    developmentSystem_ = std::make_unique<DevelopmentSystem>();
    
    initialized_ = true;
    NLM_LOG_INFO("DevelopmentComponent initialized");
    return true;
}

void DevelopmentComponent::update(const TimestepDuration& dt) {
    if (!initialized_ || !pImpl->brain_) return;
    
    pImpl->developmentalAge_ += dt;
    
    // Update development system periodically
    static size_t updateCount = 0;
    updateCount++;
    if (updateCount >= 100) {  // Update every 100 steps
        developmentSystem_->update(pImpl->brain_, pImpl->brain_->getRandomGenerator(), dt * 100);
        updateCount = 0;
        
        // Check if stage should advance based on developmental age
        if (pImpl->developmentalAge_ >= 1000.0) {  // 1000 timestep units = 1 developmental time unit
            pImpl->stageAdvanced_ = true;
        }
    }
    
    NLM_LOG_TRACE("DevelopmentComponent updated, age: " + std::to_string(pImpl->developmentalAge_));
}

void DevelopmentComponent::reset() {
    if (developmentSystem_) {
        developmentSystem_->setStage(DevelopmentalStage::Initial);
        developmentSystem_->setDevelopmentalAge(0.0);
    }
    
    pImpl->developmentalAge_ = 0.0;
    pImpl->stageAdvanced_ = false;
    
    initialized_ = false;
    NLM_LOG_INFO("DevelopmentComponent reset");
}

void DevelopmentComponent::logStatus() const {
    NLM_LOG_INFO("=== Development Component Status ===");
    NLM_LOG_INFO("Developmental stage: " + std::to_string(getDevelopmentalStage()));
    NLM_LOG_INFO("Developmental age: " + std::to_string(getDevelopmentalAge()));
    NLM_LOG_INFO("Plasticity modifier: " + std::to_string(getPlasticityModifier()));
    NLM_LOG_INFO("Critical period: " + std::to_string(isCriticalPeriod()) ? "Yes" : "No");
}

DevelopmentalStage DevelopmentComponent::getDevelopmentalStage() const {
    return developmentSystem_ ? developmentSystem_->getStage() : DevelopmentalStage::Initial;
}

void DevelopmentComponent::setDevelopmentalStage(DevelopmentalStage stage) {
    if (developmentSystem_) {
        developmentSystem_->setStage(stage);
    }
}

float DevelopmentComponent::getPlasticityModifier() const {
    return developmentSystem_ ? developmentSystem_->getPlasticityModifier() : 1.0f;
}

bool DevelopmentComponent::isCriticalPeriod() const {
    return developmentSystem_ ? developmentSystem_->isCriticalPeriod() : false;
}

float DevelopmentComponent::getCriticalPeriodProgress() const {
    return developmentSystem_ ? developmentSystem_->getCriticalPeriodProgress() : 0.0f;
}

void DevelopmentComponent::advanceStage() {
    if (developmentSystem_) {
        developmentSystem_->advanceStage();
    }
}

} // namespace nlm
