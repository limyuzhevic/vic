#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double startAge;  // Age when entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0), startAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->startAge = age_;
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
    pImpl->startAge = age_;
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
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
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

float DevelopmentSystem::getMemoryConsolidationFactor() const {
    // Memory consolidation improves with development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.1f;
        case DevelopmentalStage::CriticalPeriod: return 0.3f;
        case DevelopmentalStage::Maturation: return 0.7f;
        case DevelopmentalStage::Adult: return 0.9f;
        case DevelopmentalStage::Aging: return 0.8f;
        default: return 0.5f;
    }
}

float DevelopmentSystem::getNeuralExcitabilityModifier() const {
    // Neural excitability follows a bell curve: high in development, optimal in adulthood
    float ageFactor = static_cast<float>(age_) / 1200.0f;  // Normalize to 0-1
    if (ageFactor < 0.3f) {
        return 0.8f + 0.2f * (ageFactor / 0.3f);  // Rising from Initial
    } else if (ageFactor < 0.8f) {
        return 1.0f;  // Peak in adulthood
    } else {
        return 1.0f - 0.5f * ((ageFactor - 0.8f) / 0.2f);  // Declining in aging
    }
}

float DevelopmentSystem::getAttentionModulationFactor() const {
    // Attention improves with development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.3f;
        case DevelopmentalStage::CriticalPeriod: return 0.6f;
        case DevelopmentalStage::Maturation: return 0.8f;
        case DevelopmentalStage::Adult: return 1.0f;
        case DevelopmentalStage::Aging: return 0.9f;
        default: return 0.5f;
    }
}

float DevelopmentSystem::getRewardSensitivityFactor() const {
    // Reward sensitivity increases with development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.2f;
        case DevelopmentalStage::CriticalPeriod: return 0.5f;
        case DevelopmentalStage::Maturation: return 0.8f;
        case DevelopmentalStage::Adult: return 1.0f;
        case DevelopmentalStage::Aging: return 0.7f;
        default: return 0.5f;
    }
}

bool DevelopmentSystem::enablesComplexBehavior() const {
    // Enable complex behavior in maturation and adulthood
    return pImpl->stage >= DevelopmentalStage::Maturation;
}

bool DevelopmentSystem::enablesLongTermMemory() const {
    // Enable long-term memory in maturation and adulthood
    return pImpl->stage >= DevelopmentalStage::Maturation;
}

bool DevelopmentSystem::enablesSocialLearning() const {
    // Enable social learning in adulthood
    return pImpl->stage == DevelopmentalStage::Adult || pImpl->stage == DevelopmentalStage::Aging;
}

SimulationStep DevelopmentSystem::getStageDuration(DevelopmentalStage stage) const {
    // Return estimated duration in steps for each stage
    switch (stage) {
        case DevelopmentalStage::Initial: return 60;
        case DevelopmentalStage::CriticalPeriod: return 300;
        case DevelopmentalStage::Maturation: return 600;
        case DevelopmentalStage::Adult: return 2400;
        case DevelopmentalStage::Aging: return 1200;
        default: return 60;
    }
}

} // namespace nlm
