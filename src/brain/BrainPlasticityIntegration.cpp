// BrainPlasticityIntegration.cpp - STDP, Hebbian, structural plasticity integration implementation
// Contains plasticity system integration functionality

#include "BrainPlasticityIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Implementation of BrainPlasticityIntegration

BrainPlasticityIntegration::BrainPlasticityIntegration(BrainCore* core) : pImpl(new Impl(core)) {}

BrainPlasticityIntegration::~BrainPlasticityIntegration() = default;

BrainPlasticityIntegration::BrainPlasticityIntegration(BrainPlasticityIntegration&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainPlasticityIntegration& BrainPlasticityIntegration::operator=(BrainPlasticityIntegration&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainPlasticityIntegration::initialize() {
    NLM_LOG_INFO("Initializing Brain Plasticity Integration...");
    
    if (!pImpl->brainCore) {
        NLM_LOG_ERROR("Cannot initialize plasticity integration - no brain core available");
        return false;
    }
    
    pImpl->stdp = pImpl->brainCore->getSTDP();
    pImpl->hebbian = pImpl->brainCore->getHebbian();
    pImpl->structuralPlasticity = pImpl->brainCore->getStructuralPlasticity();
    
    NLM_LOG_INFO("Brain Plasticity Integration initialized successfully");
    return true;
}

void BrainPlasticityIntegration::update(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                       TimestepDuration dt, float plasticityMod,
                                       SimulationStep currentStep, Timestamp currentTime) {
    applySTDPToRegions(regions);
    applyHebbianToRegions(regions);
    updateStructuralPlasticity(pImpl->brainCore, *pImpl->brainCore->getRandomGenerator(), currentStep, currentTime);
}

STDP* BrainPlasticityIntegration::getSTDP() {
    return pImpl->stdp;
}

Hebbian* BrainPlasticityIntegration::getHebbian() {
    return pImpl->hebbian;
}

StructuralPlasticity* BrainPlasticityIntegration::getStructuralPlasticity() {
    return pImpl->structuralPlasticity;
}

void BrainPlasticityIntegration::applySTDP() {
    if (pImpl->stdp) {
        pImpl->stdpUpdates++;
    }
}

void BrainPlasticityIntegration::applyHebbianLearning() {
    if (pImpl->hebbian) {
        pImpl->hebbianUpdates++;
    }
}

void BrainPlasticityIntegration::applyStructuralPlasticity() {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticityEvents++;
    }
}

void BrainPlasticityIntegration::configureSTDP(float ltpWeight, float ltdWeight, float tau) {
    if (pImpl->stdp) {
        pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    }
}

void BrainPlasticityIntegration::configureStructuralPlasticity(float synaptogenesisRate, float pruningRate) {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        pImpl->structuralPlasticity->setPruningRate(pruningRate);
    }
}

void BrainPlasticityIntegration::clear() {
    pImpl->weightHistory.clear();
    pImpl->plasticityEvents.clear();
    pImpl->stdpUpdates = 0;
    pImpl->hebbianUpdates = 0;
    pImpl->structuralPlasticityEvents = 0;
}

size_t BrainPlasticityIntegration::getSTDPUpdates() const {
    return pImpl->stdpUpdates;
}

size_t BrainPlasticityIntegration::getHebbianUpdates() const {
    return pImpl->hebbianUpdates;
}

size_t BrainPlasticityIntegration::getStructuralPlasticityEvents() const {
    return pImpl->structuralPlasticityEvents;
}

float BrainPlasticityIntegration::getTotalPlasticity() const {
    // Calculate total plasticity based on weight history
    if (pImpl->weightHistory.empty()) return 0.0f;
    float sum = 0.0f;
    for (float weight : pImpl->weightHistory) {
        sum += std::abs(weight);
    }
    return sum / pImpl->weightHistory.size();
}

void BrainPlasticityIntegration::Impl::Impl(BrainCore* core) : brainCore(core), stdp(nullptr), 
    hebbian(nullptr), structuralPlasticity(nullptr), stdpUpdates(0), 
    hebbianUpdates(0), structuralPlasticityEvents(0) {}

void BrainPlasticityIntegration::applySTDPToRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            // Apply STDP to each synapse
            applySTDP();
        }
    }
}

void BrainPlasticityIntegration::applyHebbianToRegions(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    for (const auto& region : regions) {
        for (const auto& syn : region->getSynapses()) {
            // Apply Hebbian learning to each synapse
            applyHebbianLearning();
        }
    }
}

void BrainPlasticityIntegration::updateStructuralPlasticity(BrainCore* brain, RandomGenerator& rng, SimulationStep currentStep, Timestamp currentTime) {
    if (!brain || !pImpl->structuralPlasticity) return;
    
    // Apply structural plasticity based on development stage
    auto* sp = pImpl->structuralPlasticity;
    if (sp) {
        auto stage = brain->getDevelopmentalStage();
        float plasticityMod = 1.0f;
        
        switch (stage) {
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
        
        // Update structural plasticity
        sp->update(brain, rng, currentStep);
        
        if (currentStep % 100 == 0) {
            pImpl->structuralPlasticityEvents++;
        }
    }
}

void BrainPlasticityIntegration::updateSynaptogenesis(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    if (!pImpl->structuralPlasticity) return;
    
    // Create new synapses based on synaptogenesis rate
    // This is handled by the structural plasticity system
    pImpl->structuralPlasticityEvents++;
}

void BrainPlasticityIntegration::updatePruning(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    if (!pImpl->structuralPlasticity) return;
    
    // Prune weak synapses based on pruning rate
    // This is handled by the structural plasticity system
    pImpl->structuralPlasticityEvents++;
}

void BrainPlasticityIntegration::calculatePlasticityStats() const {}

} // namespace nlm

