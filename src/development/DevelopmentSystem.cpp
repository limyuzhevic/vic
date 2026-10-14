#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Neuromodulator effects on development
    float dopamineInfluence;
    float acetylcholineInfluence;
    float norepinephrineInfluence;
    float serotoninInfluence;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0),
             dopamineInfluence(0.0f), acetylcholineInfluence(0.0f), norepinephrineInfluence(0.0f), serotoninInfluence(0.0f) {}
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
    
    // Neuromodulator effects on development
    if (brain) {
        // Update neuromodulator influence based on current brain state
        if (brain->getDopamine()) {
            pImpl->dopamineInfluence = brain->getDopamine()->getLevel();
        }
        if (brain->getAcetylcholine()) {
            pImpl->acetylcholineInfluence = brain->getAcetylcholine()->getLevel();
        }
        if (brain->getNorepinephrine()) {
            pImpl->norepinephrineInfluence = brain->getNorepinephrine()->getLevel();
        }
        if (brain->getSerotonin()) {
            pImpl->serotoninInfluence = brain->getSerotonin()->getLevel();
        }
    }
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        // Neuromodulators can delay critical period closure
        float criticalPeriodDelay = 1.0f + pImpl->dopamineInfluence * 0.5f + pImpl->acetylcholineInfluence * 0.3f;
        if (pImpl->stageAge > 300.0 * criticalPeriodDelay) {
            advanceStage();
        }
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        // Neuromodulators can accelerate or delay maturation
        float maturationSpeed = 1.0f - pImpl->serotoninInfluence * 0.3f + pImpl->norepinephrineInfluence * 0.2f;
        if (pImpl->stageAge > 600.0 * maturationSpeed) {
            advanceStage();
        }
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Neuromodulator effects on development
    if (brain) {
        // Update neuromodulator influence based on current brain state
        if (brain->getDopamine()) {
            pImpl->dopamineInfluence = brain->getDopamine()->getLevel();
        }
        if (brain->getAcetylcholine()) {
            pImpl->acetylcholineInfluence = brain->getAcetylcholine()->getLevel();
        }
        if (brain->getNorepinephrine()) {
            pImpl->norepinephrineInfluence = brain->getNorepinephrine()->getLevel();
        }
        if (brain->getSerotonin()) {
            pImpl->serotoninInfluence = brain->getSerotonin()->getLevel();
        }
    }
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        // Neuromodulators can delay critical period closure
        float criticalPeriodDelay = 1.0f + pImpl->dopamineInfluence * 0.5f + pImpl->acetylcholineInfluence * 0.3f;
        if (age_ > 360.0 * criticalPeriodDelay) {
            advanceStage();
        }
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        // Neuromodulators can accelerate or delay maturation
        float maturationSpeed = 1.0f - pImpl->serotoninInfluence * 0.3f + pImpl->norepinephrineInfluence * 0.2f;
        if (age_ > 960.0 * maturationSpeed) {
            advanceStage();
        }
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    float basePlasticity = 0.5f;
    
    // Neuromodulators modulate plasticity
    basePlasticity += pImpl->dopamineInfluence * 0.3f;  // Dopamine enhances plasticity
    basePlasticity += pImpl->acetylcholineInfluence * 0.2f;  // ACh enhances learning
    basePlasticity += pImpl->norepinephrineInfluence * 0.1f;  // NE modulates plasticity
    basePlasticity -= pImpl->serotoninInfluence * 0.1f;  // 5-HT reduces plasticity (stability)
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return std::min(1.0f, basePlasticity * 1.5f);
        case DevelopmentalStage::CriticalPeriod: return std::min(1.0f, basePlasticity * 1.2f);
        case DevelopmentalStage::Maturation: return std::max(0.1f, basePlasticity * 0.7f);
        case DevelopmentalStage::Adult: return std::max(0.1f, basePlasticity * 0.5f);
        case DevelopmentalStage::Aging: return std::max(0.1f, basePlasticity * 0.3f);
        default: return basePlasticity;
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
    // Neuromodulators influence critical period plasticity
    float baseProgress = std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
    
    // Enhanced plasticity if neuromodulators are high
    float neuromodulatorBoost = pImpl->dopamineInfluence * 0.2f + pImpl->acetylcholineInfluence * 0.3f;
    return std::min(1.0f, baseProgress * (1.0f + neuromodulatorBoost));
}

} // namespace nlm
