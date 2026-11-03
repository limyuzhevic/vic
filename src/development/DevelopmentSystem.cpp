#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

DevelopmentSystem::~DevelopmentSystem() {
    delete pImpl;
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    // Call the other update method with default timestep
    TimestepDuration dt = 0.001;
    update(brain, *pImpl->brain->getConfig()->getOr<RandomGenerator*>("rng"), dt);
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Enhanced developmental stage progression with configurable timing
    // Default: Initial (60s), CriticalPeriod (360s), Maturation (960s)
    // Phase 5 fix: separate age_ and stageAge for different update styles
    
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
    
    // Enhanced stage-specific behaviors
    updateStageSpecificFeatures(brain, rng, dt);
}

void DevelopmentSystem::updateStageSpecificFeatures(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Implement stage-specific developmental processes
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Initialize neural connections, set up basic circuitry
            initializeBasicCircuitry(brain, rng, dt);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Enhance sensory processing, fine-tune connections
            enhanceSensoryProcessing(brain, rng, dt);
            break;
        case DevelopmentalStage::Maturation:
            // Develop higher-order functions, integrate systems
            developHigherOrderFunctions(brain, rng, dt);
            break;
        case DevelopmentalStage::Adult:
            // Stabilize, prune unnecessary connections
            stabilizeConnections(brain, rng, dt);
            break;
        case DevelopmentalStage::Aging:
            // Gradual decline, adapt to limitations
            adaptToAging(brain, rng, dt);
            break;
    }
}

void DevelopmentSystem::initializeBasicCircuitry(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Phase 5 improvement: more sophisticated initialization
    const float synaptogenesisRate = pImpl->stage == DevelopmentalStage::Initial ? 0.001f : 0.0001f;
    
    // Gradually increase synaptic connections
    if (rng.getReal(0.0, 1.0) < synaptogenesisRate * dt) {
        // Create new synaptic connections
        for (auto* region : brain->getRegions()) {
            // Implement synaptogenesis
        }
    }
}

void DevelopmentSystem::enhanceSensoryProcessing(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Phase 5 improvement: enhanced sensory tuning during critical period
    float learningRate = 0.5f; // Decrease with age
    learningRate *= (1.0f - std::min(1.0f, static_cast<float>(age_) / 360.0f));
    
    // Enhance sensory neuron connections
    if (rng.getReal(0.0, 1.0) < learningRate * dt) {
        // Implement sensory enhancement
    }
}

void DevelopmentSystem::developHigherOrderFunctions(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Phase 5 improvement: develop higher cognitive functions
    float integrationRate = 0.2f * (1.0f - std::min(1.0f, static_cast<float>(age_) / 960.0f));
    
    // Develop prefrontal-like connections
    if (rng.getReal(0.0, 1.0) < integrationRate * dt) {
        // Implement higher-order function development
    }
}

void DevelopmentSystem::stabilizeConnections(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Phase 5 improvement: connection stabilization in adult stage
    float pruningRate = 0.01f; // Stable pruning rate
    
    // Prune weak connections
    if (rng.getReal(0.0, 1.0) < pruningRate * dt) {
        // Implement connection pruning
    }
}

void DevelopmentSystem::adaptToAging(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Phase 5 improvement: aging adaptations
    float adaptationRate = 0.05f;
    
    // Adapt to age-related decline
    if (rng.getReal(0.0, 1.0) < adaptationRate * dt) {
        // Implement aging adaptations
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

} // namespace nlm
