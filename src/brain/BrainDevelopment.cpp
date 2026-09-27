// BrainDevelopment.cpp - Development system integration implementation
#include "BrainDevelopment.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations for BrainDevelopment implementation
struct BrainDevelopment::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

BrainDevelopment::BrainDevelopment() : pImpl(new Impl()) {}

BrainDevelopment::~BrainDevelopment() = default;

void BrainDevelopment::updateDevelopment(double timestep) {
    if (!pImpl || !pImpl->brain) {
        NLM_LOG_WARNING("BrainDevelopment: Cannot update - brain reference not set");
        return;
    }
    
    // Get the development system from brain
    auto* developmentSystem = pImpl->brain->getDevelopmentSystem();
    if (!developmentSystem) {
        NLM_LOG_WARNING("BrainDevelopment: Development system not available");
        return;
    }
    
    // Update development system
    developmentSystem->update(pImpl->brain, *pImpl->brain->getRandomGenerator(), timestep);
    
    NLM_LOG_DEBUG("BrainDevelopment: Development system updated with timestep " + 
               std::to_string(timestep));
}

DevelopmentalStage BrainDevelopment::getDevelopmentalStage() const {
    if (!pImpl || !pImpl->brain) return DevelopmentalStage::Initial;
    return pImpl->brain->getDevelopmentalStage();
}

void BrainDevelopment::setDevelopmentalStage(DevelopmentalStage stage) {
    if (!pImpl || !pImpl->brain) return;
    pImpl->brain->setDevelopmentalStage(stage);
}

double BrainDevelopment::getDevelopmentalAge() const {
    if (!pImpl || !pImpl->brain) return 0.0;
    // Development system doesn't expose age directly, so return 0 for now
    return 0.0;
}

float BrainDevelopment::getPlasticityModifier() const {
    if (!pImpl || !pImpl->brain) return 1.0f;
    // Get developmental stage to determine plasticity modifier
    DevelopmentalStage stage = pImpl->brain->getDevelopmentalStage();
    
    // Simple developmental plasticity modifiers
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
            return 1.0f;
    }
}

DevelopmentSystem* BrainDevelopment::getDevelopmentSystem() {
    if (!pImpl || !pImpl->brain) return nullptr;
    return pImpl->brain->getDevelopmentSystem();
}

} // namespace nlm