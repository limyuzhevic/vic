#include "development/DevelopmentSystemWrapper.hpp"
#include <algorithm>

namespace nlm {

struct DevelopmentSystemWrapper::Impl {
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    Brain* brain;
    
    // Configuration
    float developmentSpeed;
    float maturationRate;
    size_t criticalPeriodDuration;
    std::array<size_t, 4> developmentalStageDurations;  // Initial, CriticalPeriod, Maturation, Adult
    
    // State tracking
    SimulationStep stageStartStep;
    SimulationStep currentStageStartStep;
    
    Impl()
        : developmentSystem(nullptr)
        , brain(nullptr)
        , developmentSpeed(1.0f)
        , maturationRate(0.5f)
        , criticalPeriodDuration(1000)
        , stageStartStep(0)
        , currentStageStartStep(0) {
        // Initialize developmental stage durations
        developmentalStageDurations[static_cast<size_t>(DevelopmentalStage::Initial)] = 0;
        developmentalStageDurations[static_cast<size_t>(DevelopmentalStage::CriticalPeriod)] = 1000;
        developmentalStageDurations[static_cast<size_t>(DevelopmentalStage::Maturation)] = 5000;
        developmentalStageDurations[static_cast<size_t>(DevelopmentalStage::Adult)] = std::numeric_limits<size_t>::max();
    }
};

DevelopmentSystemWrapper::DevelopmentSystemWrapper() : pImpl(std::make_unique<Impl>()) {}
DevelopmentSystemWrapper::~DevelopmentSystemWrapper() = default;

DevelopmentSystemWrapper::DevelopmentSystemWrapper(DevelopmentSystemWrapper&& other) noexcept = default;
DevelopmentSystemWrapper& DevelopmentSystemWrapper::operator=(DevelopmentSystemWrapper&& other) noexcept = default;

void DevelopmentSystemWrapper::initialize(Brain* b) {
    brain = b;
    
    // Create development system
    developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Initialize with brain reference
    developmentSystem->initialize(b);
    
    // Apply configuration
    setDevelopmentSpeed(pImpl->developmentSpeed);
    setMaturationRate(pImpl->maturationRate);
    setCriticalPeriodDuration(pImpl->criticalPeriodDuration);
    for (int i = 0; i < 4; ++i) {
        setDevelopmentalStageDuration(static_cast<DevelopmentalStage>(i), pImpl->developmentalStageDurations[i]);
    }
}

DevelopmentSystem* DevelopmentSystemWrapper::getDevelopmentSystem() {
    return developmentSystem.get();
}

const DevelopmentSystem* DevelopmentSystemWrapper::getDevelopmentSystem() const {
    return developmentSystem.get();
}

void DevelopmentSystemWrapper::updateDevelopment(SimulationStep currentStep, float timestep, const class RandomGenerator& rng) {
    if (!developmentSystem) return;
    
    // Update development system
    developmentSystem->update(brain, rng, timestep * 1000);
    
    // Update developmental stage if needed
    if (currentStep - pImpl->stageStartStep >= pImpl->developmentalStageDurations[static_cast<size_t>(pImpl->developmentSystem->getDevelopmentalStage())]) {
        // Move to next developmental stage
        DevelopmentalStage currentStage = pImpl->developmentSystem->getDevelopmentalStage();
        DevelopmentalStage nextStage;
        
        switch (currentStage) {
            case DevelopmentalStage::Initial:
                nextStage = DevelopmentalStage::CriticalPeriod;
                break;
            case DevelopmentalStage::CriticalPeriod:
                nextStage = DevelopmentalStage::Maturation;
                break;
            case DevelopmentalStage::Maturation:
                nextStage = DevelopmentalStage::Adult;
                break;
            case DevelopmentalStage::Adult:
                nextStage = DevelopmentalStage::Adult;
                break;
        }
        
        pImpl->developmentSystem->setDevelopmentalStage(nextStage);
        pImpl->stageStartStep = currentStep;
    }
}

DevelopmentalStage DevelopmentSystemWrapper::getDevelopmentalStage() const {
    return developmentSystem ? developmentSystem->getDevelopmentalStage() : DevelopmentalStage::Initial;
}

void DevelopmentSystemWrapper::setDevelopmentalStage(DevelopmentalStage stage) {
    if (developmentSystem) {
        developmentSystem->setDevelopmentalStage(stage);
        pImpl->stageStartStep = 0; // Reset stage start for timing
    }
}

void DevelopmentSystemWrapper::applyDevelopmentalEffects(Brain* brain) {
    if (!developmentSystem) return;
    
    DevelopmentalStage stage = developmentSystem->getDevelopmentalStage();
    float plasticityMod = 1.0f;
    
    // Calculate plasticity modulation based on developmental stage
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
    
    // Apply effects to brain systems
    if (developmentSystem) {
        developmentSystem->applyEffectsToPlasticity(plasticityMod);
        developmentSystem->applyEffectsToExcitability(plasticityMod);
        developmentSystem->applyEffectsToConnectivity(plasticityMod);
    }
}

void DevelopmentSystemWrapper::updatePlasticityBasedOnDevelopment(Brain* brain) {
    if (!developmentSystem || !brain) return;
    
    DevelopmentalStage stage = developmentSystem->getDevelopmentalStage();
    float plasticityMod = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;
            break;
    }
    
    // Update plasticity systems
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
        structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void DevelopmentSystemWrapper::updateNeuralExcitability(Brain* brain) {
    if (!developmentSystem || !brain) return;
    
    // Update neural excitability based on developmental stage
    DevelopmentalStage stage = developmentSystem->getDevelopmentalStage();
    float excitabilityMod = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            excitabilityMod = 1.2f;  // Higher excitability
            break;
        case DevelopmentalStage::CriticalPeriod:
            excitabilityMod = 1.1f;
            break;
        case DevelopmentalStage::Maturation:
            excitabilityMod = 1.0f;
            break;
        case DevelopmentalStage::Adult:
            excitabilityMod = 0.8f;  // Lower excitability
            break;
    }
    
    // Apply excitability modulation to neurons
    developmentSystem->applyExcitabilityModulation(excitabilityMod);
}

void DevelopmentSystemWrapper::updateSynapticConnectivity(Brain* brain) {
    if (!developmentSystem || !brain) return;
    
    // Update synaptic connectivity based on developmental stage
    DevelopmentalStage stage = developmentSystem->getDevelopmentalStage();
    float connectivityMod = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            connectivityMod = 1.5f;  // More connections
            break;
        case DevelopmentalStage::CriticalPeriod:
            connectivityMod = 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            connectivityMod = 1.0f;
            break;
        case DevelopmentalStage::Adult:
            connectivityMod = 0.8f;  // Fewer connections
            break;
    }
    
    // Apply connectivity modulation
    developmentSystem->applyConnectivityModulation(connectivityMod);
}

void DevelopmentSystemWrapper::setDevelopmentSpeed(float speed) {
    pImpl->developmentSpeed = speed;
    if (developmentSystem) {
        developmentSystem->setDevelopmentSpeed(speed);
    }
}

void DevelopmentSystemWrapper::setMaturationRate(float rate) {
    pImpl->maturationRate = rate;
    if (developmentSystem) {
        developmentSystem->setMaturationRate(rate);
    }
}

void DevelopmentSystemWrapper::setCriticalPeriodDuration(size_t duration) {
    pImpl->criticalPeriodDuration = duration;
    if (developmentSystem) {
        developmentSystem->setCriticalPeriodDuration(duration);
    }
}

void DevelopmentSystemWrapper::setDevelopmentalStageDuration(DevelopmentalStage stage, size_t duration) {
    pImpl->developmentalStageDurations[static_cast<size_t>(stage)] = duration;
    if (developmentSystem) {
        developmentSystem->setStageDuration(stage, duration);
    }
}

} // namespace nlm