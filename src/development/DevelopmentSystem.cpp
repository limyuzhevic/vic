#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Developmental parameter tuning
    float baseExcitability;
    float attentionInhibition;
    float attentionExcitation;
    float attentionBroadness;
    float consolidationRate;
    float workingMemoryCapacity;
    float episodicStorageCapacity;
    float dopamineLevel;
    float acetylcholineLevel;
    float norepinephrineLevel;
    float serotoninLevel;
    
    Brain* brain;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
        , baseExcitability(0.0f), attentionInhibition(0.0f), attentionExcitation(0.0f), attentionBroadness(0.0f)
        , consolidationRate(0.0f), workingMemoryCapacity(0.0f), episodicStorageCapacity(0.0f)
        , dopamineLevel(0.0f), acetylcholineLevel(0.0f), norepinephrineLevel(0.0f), serotoninLevel(0.0f)
        , brain(nullptr) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0)
    , baseExcitability_(0.0f), attentionInhibition_(0.0f), attentionExcitation_(0.0f), attentionBroadness_(0.0f)
    , consolidationRate_(0.0f), workingMemoryCapacity_(0.0f), episodicStorageCapacity_(0.0f)
    , dopamineLevel_(0.0f), acetylcholineLevel_(0.0f), norepinephrineLevel_(0.0f), serotoninLevel_(0.0f)
    , brain_(nullptr) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    tuneForDevelopmentStage();
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
    tuneForDevelopmentStage();
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
    pImpl->brain = brain;
    
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
    pImpl->brain = brain;
    
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

// NEW: Neural excitability effects
float DevelopmentSystem::getNeuralExcitability() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            return 0.3f;
        case DevelopmentalStage::CriticalPeriod:
            return 0.6f;
        case DevelopmentalStage::Maturation:
            return 0.8f;
        case DevelopmentalStage::Adult:
            return 1.0f;
        case DevelopmentalStage::Aging:
            return 0.7f;
        default:
            return 0.5f;
    }
}

float DevelopmentSystem::getAttentionBalance() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            return 0.5f;  // Broad attention
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;  // Flexible attention
        case DevelopmentalStage::Maturation:
            return 0.9f;  // Focused attention
        case DevelopmentalStage::Adult:
            return 1.0f;  // Stable attention
        case DevelopmentalStage::Aging:
            return 0.7f;  // Reduced filtering
        default:
            return 0.5f;
    }
}

float DevelopmentSystem::getMemoryConsolidationStrength() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            return 1.0f;
        case DevelopmentalStage::CriticalPeriod:
            return 1.5f;
        case DevelopmentalStage::Maturation:
            return 1.0f;
        case DevelopmentalStage::Adult:
            return 0.8f;
        case DevelopmentalStage::Aging:
            return 0.6f;
        default:
            return 0.8f;
    }
}

float DevelopmentSystem::getNeuromodulationSensitivity() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            return 0.7f;
        case DevelopmentalStage::CriticalPeriod:
            return 1.0f;
        case DevelopmentalStage::Maturation:
            return 0.9f;
        case DevelopmentalStage::Adult:
            return 0.8f;
        case DevelopmentalStage::Aging:
            return 0.6f;
        default:
            return 0.8f;
    }
}

// NEW: Developmental effects on specific systems
void DevelopmentSystem::applyEffectsToNeurons() {
    float baseExcitability = getNeuralExcitability();
    float ageFactor = 1.0f;
    
    if (pImpl->stage == DevelopmentalStage::Aging) {
        ageFactor = 0.8f;
    }
    
    // Apply to all neurons through brain reference
    if (pImpl->brain) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Update neuron excitability
                    neuron->setExcitability(baseExcitability * ageFactor);
                }
            }
        }
    }
    
    baseExcitability_ = baseExcitability;
}

void DevelopmentSystem::applyEffectsToAttention() {
    float inhibition = 0.5f;
    float excitation = 1.0f;
    float broadness = 0.5f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            inhibition = 0.5f;
            excitation = 1.0f;
            broadness = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            inhibition = 0.8f;
            excitation = 1.2f;
            broadness = 0.7f;
            break;
        case DevelopmentalStage::Maturation:
            inhibition = 0.9f;
            excitation = 1.1f;
            broadness = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            inhibition = 1.0f;
            excitation = 1.0f;
            broadness = 0.3f;
            break;
        case DevelopmentalStage::Aging:
            inhibition = 0.8f;
            excitation = 0.9f;
            broadness = 0.4f;
            break;
    }
    
    attentionInhibition_ = inhibition;
    attentionExcitation_ = excitation;
    attentionBroadness_ = broadness;
}

void DevelopmentSystem::applyEffectsToMemory() {
    float consolidation = getMemoryConsolidationStrength();
    float workingMemCap = 1.0f;
    float episodicCap = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            workingMemCap = 1.0f;
            episodicCap = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            workingMemCap = 0.8f;
            episodicCap = 1.2f;
            break;
        case DevelopmentalStage::Maturation:
            workingMemCap = 0.7f;
            episodicCap = 0.9f;
            break;
        case DevelopmentalStage::Adult:
            workingMemCap = 0.6f;
            episodicCap = 0.8f;
            break;
        case DevelopmentalStage::Aging:
            workingMemCap = 0.5f;
            episodicCap = 0.6f;
            break;
    }
    
    consolidationRate_ = consolidation;
    workingMemoryCapacity_ = workingMemCap;
    episodicStorageCapacity_ = episodicCap;
}

void DevelopmentSystem::applyEffectsToNeuromodulation() {
    dopamineLevel_ = 0.8f;
    acetylcholineLevel_ = 0.7f;
    norepinephrineLevel_ = 0.6f;
    serotoninLevel_ = 0.5f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            dopamineLevel_ = 0.8f;
            acetylcholineLevel_ = 0.7f;
            norepinephrineLevel_ = 0.6f;
            serotoninLevel_ = 0.5f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            dopamineLevel_ = 1.0f;
            acetylcholineLevel_ = 0.9f;
            norepinephrineLevel_ = 0.8f;
            serotoninLevel_ = 0.7f;
            break;
        case DevelopmentalStage::Maturation:
            dopamineLevel_ = 0.9f;
            acetylcholineLevel_ = 0.8f;
            norepinephrineLevel_ = 0.7f;
            serotoninLevel_ = 0.6f;
            break;
        case DevelopmentalStage::Adult:
            dopamineLevel_ = 0.8f;
            acetylcholineLevel_ = 0.7f;
            norepinephrineLevel_ = 0.6f;
            serotoninLevel_ = 0.5f;
            break;
        case DevelopmentalStage::Aging:
            dopamineLevel_ = 0.6f;
            acetylcholineLevel_ = 0.5f;
            norepinephrineLevel_ = 0.4f;
            serotoninLevel_ = 0.4f;
            break;
    }
}

// NEW: Developmental parameter tuning
void DevelopmentSystem::tuneForDevelopmentStage() {
    applyEffectsToNeurons();
    applyEffectsToAttention();
    applyEffectsToMemory();
    applyEffectsToNeuromodulation();
}

bool DevelopmentSystem::isOptimalStageForLearning() const {
    // Critical period and maturation are optimal for learning
    return pImpl->stage == DevelopmentalStage::CriticalPeriod ||
           pImpl->stage == DevelopmentalStage::Maturation;
}

float DevelopmentSystem::getLearningPotential() const {
    float potential = getPlasticityModifier() * getNeuromodulationSensitivity();
    
    // Critical period has highest learning potential
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        potential *= 1.5f;
    }
    
    return potential;
}

void DevelopmentSystem::initialize(Brain* brain) {
    brain_ = brain;
    pImpl->brain = brain;
    tuneForDevelopmentStage();
}

} // namespace nlm
