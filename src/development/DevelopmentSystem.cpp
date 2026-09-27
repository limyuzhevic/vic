#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"


namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Enhanced development parameters
    double baselineExcitability;
    double criticalPeriodIntensity;
    double maturationRefinement;
    double neuromodulationBaseline;
    double attentionMaturation;
    double memoryConsolidation;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
        , baselineExcitability(1.0)
        , criticalPeriodIntensity(1.0)
        , maturationRefinement(0.0)
        , neuromodulationBaseline(0.5)
        , attentionMaturation(0.0)
        , memoryConsolidation(0.0) {}
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
            pImpl->criticalPeriodIntensity = 1.0f;  // Peak critical period plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            pImpl->criticalPeriodIntensity = 0.0f;  // End critical period
            pImpl->maturationRefinement = 1.0f;    // Start maturation
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            pImpl->maturationRefinement = 0.0f;    // End maturation
            pImpl->attentionMaturation = 0.8f;     // Significant attention development
            pImpl->memoryConsolidation = 0.7f;     // Enhanced memory consolidation
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            pImpl->attentionMaturation = 0.0f;    // End attention development
            pImpl->memoryConsolidation = 0.0f;    // End memory consolidation enhancement
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
    
    // Apply stage-specific effects
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodEffects(brain, rng);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturationEffects(brain);
            break;
        case DevelopmentalStage::Adult:
            applyAdultMaintenanceEffects(brain);
            break;
        case DevelopmentalStage::Aging:
            applyAgingEffects(brain);
            break;
    }
    
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
        case DevelopmentalStage::CriticalPeriod: return 0.8f * pImpl->criticalPeriodIntensity;
        case DevelopmentalStage::Maturation: return 0.5f - pImpl->maturationRefinement * 0.3f;
        case DevelopmentalStage::Adult: return 0.2f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->criticalPeriodIntensity > 0.5f;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

// ===== ENHANCED DEVELOPMENT EFFECTS =====

void DevelopmentSystem::updateNeuralExcitability(Brain* brain, float timeConstant) {
    if (!brain) return;
    
    float excitabilityMultiplier = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: rapid increase in neural excitability
            excitabilityMultiplier = 1.0f + 0.5f * (1.0f - pImpl->stageAge / 300.0f);
            break;
        case DevelopmentalStage::Maturation:
            // Maturation: stabilization with minor refinements
            excitabilityMultiplier = 1.0f + 0.2f * (1.0f - pImpl->maturationRefinement);
            break;
        case DevelopmentalStage::Adult:
            // Adult: baseline maintenance with occasional tuning
            excitabilityMultiplier = 1.0f + 0.1f * std::sin(static_cast<float>(age_) * 0.01f);
            break;
        case DevelopmentalStage::Aging:
            // Aging: gradual decline in excitability
            excitabilityMultiplier = 1.0f - 0.3f * (static_cast<float>(age_) / 3600.0f);
            break;
        default:
            break;
    }
    
    pImpl->baselineExcitability = std::clamp(excitabilityMultiplier, 0.5f, 2.0f);
    
    // Apply effects to all neurons
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Modify threshold potential based on excitability
                float thresholdMod = (pImpl->baselineExcitability - 1.0f) * 0.5f;
                neuron->setThreshold(neuron->getState().threshold + thresholdMod);
                
                // Modify membrane time constant
                float tauMod = 1.0f + (pImpl->baselineExcitability - 1.0f) * 0.3f;
                neuron->setMembraneTimeConstant(neuron->getMembraneTimeConstant() * tauMod);
                
                // Adjust spontaneous firing rate
                float spontaneousRate = neuron->getSpontaneousFiringRate() * pImpl->baselineExcitability;
                neuron->setSpontaneousFiringRate(spontaneousRate);
            }
        }
    }
}

void DevelopmentSystem::updatePlasticityRules(Brain* brain, RandomGenerator& rng, float timeConstant) {
    if (!brain || !brain->pImpl->stdp) return;
    
    float stagePlasticity = 1.0f;
    float eligibilityDecayRate = 0.9f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced STDP during critical period
            stagePlasticity = 1.0f + 0.5f * (1.0f - getCriticalPeriodProgress());
            eligibilityDecayRate = 0.7f;  // Slower decay, longer eligibility windows
            break;
        case DevelopmentalStage::Maturation:
            // Refined plasticity rules
            stagePlasticity = 0.8f;
            eligibilityDecayRate = 0.85f;
            break;
        case DevelopmentalStage::Adult:
            // Stable plasticity
            stagePlasticity = 0.6f;
            eligibilityDecayRate = 0.9f;
            break;
        case DevelopmentalStage::Aging:
            // Reduced plasticity and faster eligibility decay
            stagePlasticity = 0.3f;
            eligibilityDecayRate = 0.95f;
            break;
        default:
            break;
    }
    
    // Modulate STDP parameters
    STDP* stdp = brain->pImpl->stdp.get();
    if (stdp) {
        // Scale LTP and LTD weights based on developmental stage
        float ltpWeight = stdp->getLTPWeight() * stagePlasticity;
        float ltdWeight = stdp->getLTDWeight() * stagePlasticity;
        float tau = stdp->getTimeConstant() * (1.0f + (1.0f - stagePlasticity) * 0.5f);
        
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Enhance Hebbian learning for complex pattern formation
        if (brain->pImpl->hebbian) {
            float hebbianLearningRate = brain->pImpl->hebbian->getLearningRate() * stagePlasticity;
            brain->pImpl->hebbian->setLearningRate(hebbianLearningRate);
        }
    }
    
    // Update eligibility trace dynamics in synapses
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Modify eligibility trace decay rate based on development
            float currentEligibility = syn->getEligibilityTrace();
            currentEligibility *= eligibilityDecayRate;
            syn->setEligibilityTrace(currentEligibility);
            
            // Add developmental bias to eligibility
            if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
                // Enhanced eligibility during critical period
                currentEligibility = std::min(1.0f, currentEligibility * 1.1f);
                syn->setEligibilityTrace(currentEligibility);
            }
        }
    }
}

void DevelopmentSystem::updateAttentionSystem(Brain* brain, float timeConstant) {
    if (!brain) return;
    
    float attentionEnhancement = 1.0f;
    float signalToNoiseRatio = 1.0f;
    float distractibilityReduction = 0.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::CriticalPeriod:
            // Early development: basic attentional mechanisms
            attentionEnhancement = 1.0f + 0.3f * (1.0f - getCriticalPeriodProgress());
            signalToNoiseRatio = 1.0f + 0.2f * (1.0f - getCriticalPeriodProgress());
            break;
        case DevelopmentalStage::Maturation:
            // Maturation: refined attention and filtering
            attentionEnhancement = 1.0f + pImpl->attentionMaturation * 0.7f;
            signalToNoiseRatio = 1.0f + pImpl->attentionMaturation * 0.5f;
            distractibilityReduction = pImpl->attentionMaturation * 0.3f;
            break;
        case DevelopmentalStage::Adult:
            // Adult: optimized attention
            attentionEnhancement = 1.0f + 0.2f * std::sin(static_cast<float>(age_) * 0.005f);
            signalToNoiseRatio = 1.0f + 0.15f * std::cos(static_cast<float>(age_) * 0.003f);
            break;
        case DevelopmentalStage::Aging:
            // Aging: decline in attentional capacity
            attentionEnhancement = 1.0f - 0.4f * (static_cast<float>(age_) / 3600.0f);
            signalToNoiseRatio = 1.0f - 0.3f * (static_cast<float>(age_) / 3600.0f);
            break;
        default:
            break;
    }
    
    // Apply attention effects to attentional selection system
    if (brain->pImpl->attention) {
        brain->pImpl->attention->setInhibitionStrength(
            brain->pImpl->attention->getInhibitionStrength() * attentionEnhancement
        );
        brain->pImpl->attention->setExcitationStrength(
            brain->pImpl->attention->getExcitationStrength() * signalToNoiseRatio
        );
        
        // Reduce distractibility in later stages
        if (distractibilityReduction > 0.0f) {
            // This would require adding a distractibility parameter to AttentionalSelection
            // For now, we enhance the inhibitory focus
            float currentInhibition = brain->pImpl->attention->getInhibitionStrength();
            brain->pImpl->attention->setInhibitionStrength(
                currentInhibition * (1.0f + distractibilityReduction)
            );
        }
    }
    
    // Apply signal-to-noise ratio improvements to neural populations
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            // Enhance focused processing in adult and aging stages
            if (pImpl->stage >= DevelopmentalStage::Maturation) {
                for (auto* neuron : pop->getNeurons()) {
                    // Improve signal fidelity
                    neuron->setSignalToNoiseRatio(
                        neuron->getSignalToNoiseRatio() * signalToNoiseRatio
                    );
                }
            }
        }
    }
}

void DevelopmentSystem::updateMemorySystems(Brain* brain, float timeConstant) {
    if (!brain) return;
    
    float consolidationEnhancement = 1.0f;
    float replayFrequency = 1.0f;
    float stabilityIncrease = 0.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: initial memory formation
            consolidationEnhancement = 1.0f + 0.3f * (1.0f - getCriticalPeriodProgress());
            replayFrequency = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            // Maturation: enhanced consolidation and replay
            consolidationEnhancement = 1.0f + pImpl->memoryConsolidation * 0.8f;
            replayFrequency = 1.2f;
            stabilityIncrease = pImpl->memoryConsolidation * 0.5f;
            break;
        case DevelopmentalStage::Adult:
            // Adult: efficient consolidation and replay
            consolidationEnhancement = 1.0f;
            replayFrequency = 1.0f;
            stabilityIncrease = 0.1f * std::sin(static_cast<float>(age_) * 0.002f);
            break;
        case DevelopmentalStage::Aging:
            // Aging: decline in memory consolidation
            consolidationEnhancement = 1.0f - 0.5f * (static_cast<float>(age_) / 3600.0f);
            replayFrequency = 0.6f;
            stabilityIncrease = -0.2f * (static_cast<float>(age_) / 3600.0f);
            break;
        default:
            break;
    }
    
    // Apply consolidation effects to episodic memory
    if (brain->pImpl->episodicMemory) {
        // Increase consolidation efficiency
        brain->pImpl->episodicMemory->setConsolidationEfficiency(
            brain->pImpl->episodicMemory->getConsolidationEfficiency() * consolidationEnhancement
        );
        
        // Modify replay frequency
        brain->pImpl->replayInterval = 
            static_cast<size_t>(brain->pImpl->replayInterval / replayFrequency);
    }
    
    // Apply effects to working memory
    if (brain->pImpl->workingMemory) {
        // Enhance memory stability
        size_t currentCapacity = brain->pImpl->workingMemory->getCapacity();
        size_t newCapacity = static_cast<size_t>(currentCapacity * (1.0f + stabilityIncrease));
        brain->pImpl->workingMemory->setCapacity(std::max(currentCapacity, newCapacity));
        
        // Improve memory trace stability
        if (brain->pImpl->workingMemory->getMemoryNeurons().size() > 0) {
            float stability = brain->pImpl->workingMemory->getMemoryNeurons().size() > 0 ? 
                (stabilityIncrease + 0.5f) : 0.5f;
            brain->pImpl->workingMemory->setMemoryStability(stability);
        }
    }
}

void DevelopmentSystem::updateNeuromodulation(Brain* brain, float timeConstant) {
    if (!brain) return;
    
    float baselineModulation = 1.0f;
    float sensitivityFactor = 1.0f;
    float responseDynamics = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: high neuromodulator sensitivity
            baselineModulation = 1.2f;
            sensitivityFactor = 1.5f;
            responseDynamics = 0.8f;  // Faster response dynamics
            break;
        case DevelopmentalStage::Maturation:
            // Maturation: balanced neuromodulation
            baselineModulation = 1.0f + pImpl->maturationRefinement * 0.2f;
            sensitivityFactor = 1.2f - pImpl->maturationRefinement * 0.1f;
            responseDynamics = 0.9f;
            break;
        case DevelopmentalStage::Adult:
            // Adult: stable neuromodulation
            baselineModulation = 1.0f;
            sensitivityFactor = 1.0f;
            responseDynamics = 1.0f;
            break;
        case DevelopmentalStage::Aging:
            // Aging: reduced neuromodulation
            baselineModulation = 0.8f - 0.3f * (static_cast<float>(age_) / 3600.0f);
            sensitivityFactor = 0.7f - 0.2f * (static_cast<float>(age_) / 3600.0f);
            responseDynamics = 1.2f;  // Slower dynamics
            break;
        default:
            break;
    }
    
    pImpl->neuromodulationBaseline = baselineModulation;
    
    // Apply neuromodulation effects to neuromodulator systems
    if (brain->pImpl->dopamine) {
        float currentDopamine = brain->pImpl->dopamine->getLevel();
        brain->pImpl->dopamine->setLevel(currentDopamine * baselineModulation);
        brain->pImpl->dopamine->setSensitivity(baselineModulation * sensitivityFactor);
    }
    
    if (brain->pImpl->acetylcholine) {
        float currentACh = brain->pImpl->acetylcholine->getLevel();
        brain->pImpl->acetylcholine->setLevel(currentACh * baselineModulation);
        brain->pImpl->acetylcholine->setSensitivity(baselineModulation * sensitivityFactor);
    }
    
    if (brain->pImpl->norepinephrine) {
        float currentNE = brain->pImpl->norepinephrine->getLevel();
        brain->pImpl->norepinephrine->setLevel(currentNE * baselineModulation);
        brain->pImpl->norepinephrine->setSensitivity(baselineModulation * sensitivityFactor);
    }
    
    // Apply response dynamics to plasticity factors
    if (brain->pImpl->dopamine) {
        float currentPlasticityFactor = brain->pImpl->dopamine->getPlasticityFactor();
        brain->pImpl->dopamine->setPlasticityFactor(
            currentPlasticityFactor * responseDynamics
        );
    }
}

// ===== STAGE-SPECIFIC BEHAVIORS =====

void DevelopmentSystem::applyCriticalPeriodEffects(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Enhance spontaneous activity patterns
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Increase spontaneous firing during critical period
                float spontaneousRate = neuron->getSpontaneousFiringRate();
                spontaneousRate *= 1.2f;  // 20% increase
                neuron->setSpontaneousFiringRate(spontaneousRate);
            }
        }
    }
    
    // Modify synaptic competition to favor strong connections
    if (brain->pImpl->structuralPlasticity) {
        brain->pImpl->structuralPlasticity->setSynaptogenesisRate(
            brain->pImpl->structuralPlasticity->getSynaptogenesisRate() * 1.5f
        );
        brain->pImpl->structuralPlasticity->setPruningRate(
            brain->pImpl->structuralPlasticity->getPruningRate() * 1.2f
        );
    }
    
    // Enhance Hebbian learning for pattern formation
    if (brain->pImpl->hebbian) {
        float learningRate = brain->pImpl->hebbian->getLearningRate();
        brain->pImpl->hebbian->setLearningRate(learningRate * 1.3f);
    }
}

void DevelopmentSystem::applyMaturationEffects(Brain* brain) {
    if (!brain) return;
    
    // Refine synaptic weights for optimal function
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            // Push weights toward optimal range (0.2-0.8)
            if (weight > 0.8f) {
                weight *= 0.95f;
            } else if (weight < 0.2f) {
                weight *= 1.05f;
            }
            syn->setWeight(weight);
        }
    }
    
    // Stabilize neural excitability
    float targetExcitability = 1.2f;  // Slightly above baseline for optimal function
    float currentExcitability = pImpl->baselineExcitability;
    pImpl->baselineExcitability += (targetExcitability - currentExcitability) * 0.1f;
    
    // Enhance attention mechanisms
    if (brain->pImpl->attention) {
        // Optimize attentional focus
        float currentInhibition = brain->pImpl->attention->getInhibitionStrength();
        brain->pImpl->attention->setInhibitionStrength(currentInhibition * 0.98f);
    }
    
    // Improve memory replay fidelity
    if (brain->pImpl->episodicMemory) {
        brain->pImpl->episodicMemory->setReplayFidelity(
            brain->pImpl->episodicMemory->getReplayFidelity() * 1.1f
        );
    }
}

void DevelopmentSystem::applyAdultMaintenanceEffects(Brain* brain) {
    if (!brain) return;
    
    // Maintain stable synaptic weights
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Small stochastic adjustments to maintain homeostasis
            float weight = syn->getWeight();
            if (rng) {
                float noise = rng->getUniform() * 0.02f - 0.01f;  // ±1% noise
                weight += noise;
                weight = std::clamp(weight, 0.0f, 1.0f);
                syn->setWeight(weight);
            }
        }
    }
    
    // Stabilize neuromodulator levels
    pImpl->baselineExcitability = std::clamp(pImpl->baselineExcitability, 0.9f, 1.1f);
    
    // Optimize attention for efficiency
    if (brain->pImpl->attention) {
        // Fine-tune attentional parameters for optimal performance
        float currentInhibition = brain->pImpl->attention->getInhibitionStrength();
        brain->pImpl->attention->setInhibitionStrength(
            currentInhibition * (0.95f + 0.1f * std::sin(static_cast<float>(age_) * 0.01f))
        );
    }
}

void DevelopmentSystem::applyAgingEffects(Brain* brain) {
    if (!brain) return;
    
    // Reduce neural excitability
    pImpl->baselineExcitability *= 0.98f;  // ~2% reduction per time unit
    
    // Increase synaptic noise
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Add noise to synaptic weights
            if (rng) {
                float noise = (rng->getUniform() - 0.5f) * 0.05f;  // ±5% noise
                float weight = syn->getWeight() + noise;
                weight = std::clamp(weight, 0.0f, 1.0f);
                syn->setWeight(weight);
            }
            
            // Reduce eligibility trace decay
            float eligibility = syn->getEligibilityTrace();
            eligibility *= 0.99f;  // Slightly slower decay
            syn->setEligibilityTrace(eligibility);
        }
    }
    
    // Reduce attentional capacity
    if (brain->pImpl->attention) {
        float currentInhibition = brain->pImpl->attention->getInhibitionStrength();
        brain->pImpl->attention->setInhibitionStrength(currentInhibition * 0.95f);
    }
    
    // Reduce memory consolidation efficiency
    if (brain->pImpl->episodicMemory) {
        brain->pImpl->episodicMemory->setConsolidationEfficiency(
            brain->pImpl->episodicMemory->getConsolidationEfficiency() * 0.9f
        );
    }
}

} // namespace nlm
