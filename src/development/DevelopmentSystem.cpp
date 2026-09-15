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

void DevelopmentSystem::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    NLM_LOG_INFO("Applying developmental effects for stage: " + std::string(getStageName()));
    
    // Calculate modifiers based on developmental stage
    float plasticityMod = getPlasticityModifier();
    float excitabilityMod = 1.0f + (1.0f - plasticityMod) * 0.5f;  // More excitability with lower plasticity
    float membraneMod = 1.0f - (1.0f - plasticityMod) * 0.3f;      // Membrane properties stabilize
    float neuromodMod = plasticityMod;                              // Neuromodulators follow plasticity
    float attentionMod = 1.0f - (1.0f - plasticityMod) * 0.2f;    // Attention stabilizes
    float learningMod = plasticityMod;                              // Learning follows plasticity
    
    // Apply all developmental effects to brain
    modifyExcitabilityThresholds(brain, excitabilityMod);
    modifyMembraneProperties(brain, membraneMod);
    modifyNeuromodulatorLevels(brain, neuromodMod);
    modifyAttentionSystem(brain, attentionMod);
    modifyLearningRates(brain, learningMod);
}

void DevelopmentSystem::modifyExcitabilityThresholds(Brain* brain, float excitabilityModifier) {
    if (!brain) return;
    
    NLM_LOG_INFO("Development modifying neural excitability thresholds with modifier: " + std::to_string(excitabilityModifier));
    
    // Modify thresholds for all neurons based on developmental stage
    // Excitability increases during critical periods for learning
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float threshold = neuron->getThreshold();
                
                // Modify threshold based on developmental stage
                if (pImpl->stage == DevelopmentalStage::Initial || pImpl->stage == DevelopmentalStage::CriticalPeriod) {
                    // Lower threshold during early development for easier firing
                    float newThreshold = threshold * (1.0f - (excitabilityModifier - 1.0f) * 0.3f);
                    neuron->setThreshold(newThreshold);
                } else {
                    // Higher threshold in mature brain for stability
                    float newThreshold = threshold * excitabilityModifier;
                    neuron->setThreshold(newThreshold);
                }
            }
        }
    }
}

void DevelopmentSystem::modifyMembraneProperties(Brain* brain, float membraneModifier) {
    if (!brain) return;
    
    NLM_LOG_INFO("Development modifying membrane properties with modifier: " + std::to_string(membraneModifier));
    
    // Modify membrane properties based on developmental stage
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                float leakConductance = neuron->getLeakConductance();
                float newConductance = leakConductance * membraneModifier;
                neuron->setLeakConductance(newConductance);
                
                // Also modify adaptation variable to affect spike-frequency adaptation
                auto& state = neuron->getState();
                float adaptation = state.adaptationVariable;
                state.adaptationVariable = adaptation * membraneModifier;
            }
        }
    }
}

void DevelopmentSystem::modifyNeuromodulatorLevels(Brain* brain, float neuromodulatorModifier) {
    if (!brain) return;
    
    NLM_LOG_INFO("Development modifying neuromodulator levels with modifier: " + std::to_string(neuromodulatorModifier));
    
    // Modify neuromodulator concentrations based on developmental stage
    if (brain->getDopamine()) {
        float currentDA = brain->getDopamine()->getLevel();
        brain->getDopamine()->setLevel(currentDA * neuromodulatorModifier);
    }
    
    if (brain->getAcetylcholine()) {
        float currentACh = brain->getAcetylcholine()->getLevel();
        brain->getAcetylcholine()->setLevel(currentACh * neuromodulatorModifier);
    }
    
    if (brain->getNorepinephrine()) {
        float currentNE = brain->getNorepinephrine()->getLevel();
        brain->getNorepinephrine()->setLevel(currentNE * neuromodulatorModifier);
    }
    
    if (brain->getSerotonin()) {
        float current5HT = brain->getSerotonin()->getLevel();
        brain->getSerotonin()->setLevel(current5HT * neuromodulatorModifier);
    }
}

void DevelopmentSystem::modifyAttentionSystem(Brain* brain, float attentionModifier) {
    if (!brain) return;
    
    NLM_LOG_INFO("Development modifying attention system with modifier: " + std::to_string(attentionModifier));
    
    // Modify attention system parameters
    if (brain->getAttention()) {
        // Adjust inhibition and excitation strengths based on developmental stage
        float currentInhibition = brain->getAttention()->getInhibitionStrength();
        float currentExcitation = brain->getAttention()->getExcitationStrength();
        
        brain->getAttention()->setInhibitionStrength(currentInhibition * attentionModifier);
        brain->getAttention()->setExcitationStrength(currentExcitation * attentionModifier);
    }
    
    // Also affect working memory parameters
    if (brain->getWorkingMemory()) {
        // Working memory capacity scales with developmental maturity
        size_t currentCapacity = brain->getWorkingMemory()->getCapacity();
        size_t newCapacity = static_cast<size_t>(currentCapacity * attentionModifier);
        brain->getWorkingMemory()->setCapacity(newCapacity);
    }
}

void DevelopmentSystem::modifyLearningRates(Brain* brain, float learningModifier) {
    if (!brain) return;
    
    NLM_LOG_INFO("Development modifying learning rates with modifier: " + std::to_string(learningModifier));
    
    // Modify learning-related parameters
    if (brain->getSTDP()) {
        // Adjust STDP learning rates
        float currentLTP = brain->getSTDP()->getLTPWeight();
        float currentLTD = brain->getSTDP()->getLTDWeight();
        
        brain->getSTDP()->setLTPWeight(currentLTP * learningModifier);
        brain->getSTDP()->setLTDWeight(currentLTD * learningModifier);
    }
    
    if (brain->getPredictionSystem()) {
        // Prediction system learning is modified
        // Note: This depends on the PredictionSystem having learning rate methods
        // For now we log the modification
        NLM_LOG_INFO("Development modifying prediction system learning rate");
    }
}

} // namespace nlm
