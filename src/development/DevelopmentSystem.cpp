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

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

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

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Developmental effects
    float neuralExcitability;
    float attentionModulator;
    float memoryConsolidationRate;
    float learningRate;
    float attentionSpan;
    float memoryCapacity;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
        , neuralExcitability(1.0f), attentionModulator(1.0f), memoryConsolidationRate(0.5f)
        , learningRate(1.0f), attentionSpan(1.0f), memoryCapacity(1.0f) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

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

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Apply developmental effects
    applyDevelopmentalEffects(brain, currentStep);
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Apply developmental effects
    applyDevelopmentalEffects(brain, static_cast<SimulationStep>(age_ / dt));
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
}

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain, SimulationStep currentStep) {
    // Set developmental stage-specific effects based on current stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->neuralExcitability = 1.5f;
            pImpl->attentionModulator = 1.2f;
            pImpl->memoryConsolidationRate = 0.6f;
            pImpl->learningRate = 1.5f;
            pImpl->attentionSpan = 0.8f;
            pImpl->memoryCapacity = 0.8f;
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            pImpl->neuralExcitability = 1.2f;
            pImpl->attentionModulator = 1.0f;
            pImpl->memoryConsolidationRate = 0.8f;
            pImpl->learningRate = 1.2f;
            pImpl->attentionSpan = 1.0f;
            pImpl->memoryCapacity = 1.0f;
            break;
            
        case DevelopmentalStage::Maturation:
            pImpl->neuralExcitability = 0.8f;
            pImpl->attentionModulator = 0.9f;
            pImpl->memoryConsolidationRate = 1.0f;
            pImpl->learningRate = 1.0f;
            pImpl->attentionSpan = 1.2f;
            pImpl->memoryCapacity = 1.2f;
            break;
            
        case DevelopmentalStage::Adult:
            pImpl->neuralExcitability = 0.5f;
            pImpl->attentionModulator = 0.8f;
            pImpl->memoryConsolidationRate = 1.2f;
            pImpl->learningRate = 0.7f;
            pImpl->attentionSpan = 1.5f;
            pImpl->memoryCapacity = 1.5f;
            break;
            
        case DevelopmentalStage::Aging:
            pImpl->neuralExcitability = 0.3f;
            pImpl->attentionModulator = 0.6f;
            pImpl->memoryConsolidationRate = 0.7f;
            pImpl->learningRate = 0.4f;
            pImpl->attentionSpan = 1.8f;
            pImpl->memoryCapacity = 2.0f;
            break;
    }
    
    // Log developmental effects
    NLM_LOG_DEBUG("Development effects: excitability=" + std::to_string(pImpl->neuralExcitability) +
                 ", attention_mod=" + std::to_string(pImpl->attentionModulator) +
                 ", memory_consolid=" + std::to_string(pImpl->memoryConsolidationRate) +
                 ", learning_rate=" + std::to_string(pImpl->learningRate));
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

float DevelopmentSystem::getNeuralExcitabilityModifier() const {
    return pImpl->neuralExcitability;
}

float DevelopmentSystem::getAttentionModulator() const {
    return pImpl->attentionModulator;
}

float DevelopmentSystem::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

void DevelopmentSystem::applyDevelopmentalPlasticityRules(Brain* brain) {
    // Apply stage-specific plasticity rules beyond just rate modifiers
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High synaptic strengthening, low pruning
            if (brain && brain->getSTDP()) {
                brain->getSTDP()->setLTPWeight(0.02f);
                brain->getSTDP()->setLTDWeight(0.012f);
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced Hebbian learning
            if (brain && brain->getHebbian()) {
                // Could modify Hebbian parameters
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Balanced plasticity
            if (brain && brain->getSTDP()) {
                brain->getSTDP()->setLTPWeight(0.01f);
                brain->getSTDP()->setLTDWeight(0.012f);
            }
            break;
            
        case DevelopmentalStage::Adult:
            // Low plasticity, high stability
            if (brain && brain->getSTDP()) {
                brain->getSTDP()->setLTPWeight(0.005f);
                brain->getSTDP()->setLTDWeight(0.012f);
            }
            break;
            
        case DevelopmentalStage::Aging:
            // Reduced strengthening, increased maintenance
            if (brain && brain->getSTDP()) {
                brain->getSTDP()->setLTPWeight(0.002f);
                brain->getSTDP()->setLTDWeight(0.015f);
            }
            break;
    }
}

bool DevelopmentSystem::isOptimalLearningStage() const {
    // Critical period and early maturation are optimal for learning
    return (pImpl->stage == DevelopmentalStage::CriticalPeriod ||
            pImpl->stage == DevelopmentalStage::Maturation);
}

float DevelopmentSystem::getStageSpecificLearningRate() const {
    return pImpl->learningRate;
}

float DevelopmentSystem::getStageSpecificAttentionSpan() const {
    return pImpl->attentionSpan;
}

float DevelopmentSystem::getStageSpecificMemoryCapacity() const {
    return pImpl->memoryCapacity;
}

} // namespace nlm
