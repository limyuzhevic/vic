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

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0), development_(new Development()) {}

DevelopmentSystem::~DevelopmentSystem() {
    delete pImpl;
    delete development_;
}

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    development_->setStage(stage);
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
    development_->setStage(pImpl->stage);
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

void DevelopmentSystem::updateEnhanced(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Use the enhanced Development class for comprehensive developmental effects
    development_->initialize(brain);
    development_->update(brain, rng, dt);
}

void DevelopmentSystem::applyExcitabilityEffects(Brain* brain) {
    development_->applyExcitabilityEffects(brain);
}

void DevelopmentSystem::applyAttentionEffects(Brain* brain) {
    development_->applyAttentionEffects(brain);
}

void DevelopmentSystem::applyMemoryConsolidationEffects(Brain* brain) {
    development_->applyMemoryConsolidationEffects(brain);
}

void DevelopmentSystem::applyNeuromodulationEffects(Brain* brain) {
    development_->applyNeuromodulationEffects(brain);
}

void DevelopmentSystem::applyPlasticityRuleEffects(Brain* brain) {
    development_->applyPlasticityRuleEffects(brain);
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Delegate to development class for enhanced plasticity modifier
    return development_->getPlasticityRuleModifier(PlasticityType::STDP);
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return development_->isCriticalPeriod();
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    return development_->getCriticalPeriodProgress();
}

DevelopmentSystem::DevelopmentMetrics DevelopmentSystem::getMetrics() const {
    DevelopmentMetrics metrics;
    metrics.age = age_;
    metrics.stage = pImpl->stage;
    metrics.neuralExcitability = development_->getMetrics().neuralExcitability;
    metrics.attentionCapacity = development_->getMetrics().attentionCapacity;
    metrics.memoryConsolidation = development_->getMetrics().memoryConsolidation;
    metrics.dopamineRelease = development_->getMetrics().dopamineRelease;
    metrics.acetylcholineRelease = development_->getMetrics().acetylcholineRelease;
    metrics.norepinephrineRelease = development_->getMetrics().norepinephrineRelease;
    metrics.serotoninRelease = development_->getMetrics().serotoninRelease;
    metrics.plasticityStrength = development_->getMetrics().plasticityStrength;
    metrics.learningRate = development_->getMetrics().learningRate;
    metrics.explorationBias = development_->getMetrics().explorationBias;
    metrics.memoryRetention = development_->getMetrics().memoryRetention;
    
    return metrics;
}

} // namespace nlm

