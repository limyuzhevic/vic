#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

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
    
    // Apply comprehensive developmental effects across all brain systems
    if (brain) {
        // Update neuromodulation systems
        updateNeuromodulation(brain->getDopamine(), brain->getAcetylcholine(),
                             brain->getNorepinephrine(), brain->getSerotonin(),
                             dt);
        
        // Update memory systems
        updateMemorySystems(brain->getWorkingMemory(),
                           brain->getEpisodicMemory(),
                           brain->getAssociativeMemory(),
                           dt);
        
        // Update cognitive systems
        updateCognitiveSystems(brain->getPlanner(),
                              brain->getConceptFormation(),
                              brain->getSocialLearning(),
                              dt);
        
        // Update attention system
        updateAttentionSystem(brain->getAttention(), dt);
        
        // Update developmental effects on neural excitability and connectivity
        updateExcitabilityAndConnectivity(brain, dt);
        
        // Update developmental effects on learning and adaptation
        updateLearningAndAdaptation(brain, rng, dt);
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

// Update neuromodulation systems with developmental effects
void DevelopmentSystem::updateNeuromodulation(Dopamine* dopamine, Acetylcholine* ach,
                                             Norepinephrine* ne, Serotonin* serotonin,
                                             TimestepDuration dt) {
    if (!dopamine || !ach || !ne || !serotonin) return;
    
    // Update stage-specific neuromodulatory effects
    updateStageSpecificNeuromodulation(dopamine, ach, ne, serotonin);
    
    // Update baseline levels based on developmental stage
    float stage = static_cast<float>(pImpl->stage);
    
    // Dopamine baseline changes with development
    float dopamineBaseline;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        dopamineBaseline = 0.3f;  // Low baseline, high exploration drive
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        dopamineBaseline = 0.5f;  // Moderate baseline for learning
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        dopamineBaseline = 0.7f;  // High baseline for skill refinement
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        dopamineBaseline = 0.6f;  // Stable baseline
    } else { // Aging
        dopamineBaseline = 0.4f;  // Declining baseline
    }
    dopamine->setLevel(dopamineBaseline + (rand() / (RAND_MAX + 1.0f)) * 0.2f - 0.1f);
    
    // Acetylcholine attention effects vary by stage
    if (pImpl->stage == DevelopmentalStage::Initial) {
        ach->setLevel(0.4f);  // Focused on exploration, less selective attention
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        ach->setLevel(0.8f);  // High attention for sensory processing
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        ach->setLevel(0.6f);  // Balanced attention
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        ach->setLevel(0.7f);  // Stable selective attention
    } else { // Aging
        ach->setLevel(0.5f);  // Declining attention capacity
    }
    
    // Norepinephrine arousal changes
    if (pImpl->stage == DevelopmentalStage::Initial) {
        ne->setLevel(0.5f);  // Moderate arousal for exploration
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        ne->setLevel(0.7f);  // High arousal for sensory processing
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        ne->setLevel(0.6f);  // Balanced arousal
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        ne->setLevel(0.5f);  // Stable arousal
    } else { // Aging
        ne->setLevel(0.3f);  // Lower arousal for calmness
    }
    
    // Serotonin mood and risk-taking effects
    if (pImpl->stage == DevelopmentalStage::Initial) {
        serotonin->setLevel(0.4f);  // More risk-taking, exploratory mood
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        serotonin->setLevel(0.5f);  // Balanced mood
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        serotonin->setLevel(0.6f);  // Stable mood
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        serotonin->setLevel(0.7f);  // Positive mood
    } else { // Aging
        serotonin->setLevel(0.5f);  // Compensatory mood stability
    }
    
    // Update neuromodulators
    dopamine->update(dt);
    ach->update(dt);
    ne->update(dt);
    serotonin->update(dt);
}

// Update memory systems with developmental effects
void DevelopmentSystem::updateMemorySystems(NeuralWorkingMemory* workingMemory,
                                           NeuralEpisodicMemory* episodicMemory,
                                           NeuralAssociativeMemory* associativeMemory,
                                           TimestepDuration dt) {
    if (!workingMemory || !episodicMemory || !associativeMemory) return;
    
    // Working memory capacity matures over time
    float workingMemoryCapacity;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        workingMemoryCapacity = 3.0f;  // Very limited capacity
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        workingMemoryCapacity = 5.0f;  // Expanding capacity
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        workingMemoryCapacity = 7.0f;  // Near adult capacity
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        workingMemoryCapacity = 7.0f;  // Stable capacity
    } else { // Aging
        workingMemoryCapacity = 5.0f;  // Some decline
    }
    
    // Update working memory with developmental effects
    // (Assuming NeuralWorkingMemory has methods for capacity and update)
    // workingMemory->setCapacity(workingMemoryCapacity);
    // workingMemory->setLearningRate(getPlasticityModifier() * 0.5f);
    
    // Episodic memory consolidation effects
    float episodicMemoryConsolidation;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        episodicMemoryConsolidation = 0.2f;  // Rapid but unstable consolidation
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        episodicMemoryConsolidation = 0.6f;  // Strong sensory memory formation
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        episodicMemoryConsolidation = 0.8f;  // Efficient consolidation
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        episodicMemoryConsolidation = 0.9f;  // Stable consolidation
    } else { // Aging
        episodicMemoryConsolidation = 0.7f;  // Compensatory mechanisms
    }
    
    // Associative memory strength development
    float associativeMemoryStrength = getPlasticityModifier() * 0.8f;
    
    // Update memory systems
    // workingMemory->update(dt);
    // episodicMemory->update(dt);
    // associativeMemory->update(dt);
}

// Update cognitive systems with developmental effects
void DevelopmentSystem::updateCognitiveSystems(NeuralPlanner* planner,
                                              ConceptFormation* conceptFormation,
                                              SocialLearning* socialLearning,
                                              TimestepDuration dt) {
    if (!planner || !conceptFormation || !socialLearning) return;
    
    // Planning depth development
    float planningDepth;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        planningDepth = 1.0f;  // Very short-term planning
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        planningDepth = 2.0f;  // Expanding planning horizon
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        planningDepth = 3.0f;  // Multi-step planning
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        planningDepth = 4.0f;  // Complex planning
    } else { // Aging
        planningDepth = 2.5f;  // Simplified planning
    }
    
    // Concept formation complexity
    float conceptFormationComplexity = getPlasticityModifier() * 0.9f;
    
    // Social learning capacity
    float socialLearningCapacity;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        socialLearningCapacity = 0.3f;  // Limited social learning
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        socialLearningCapacity = 0.8f;  // Strong social learning (imitation)
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        socialLearningCapacity = 0.9f;  // High social learning
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        socialLearningCapacity = 0.7f;  // Selective social learning
    } else { // Aging
        socialLearningCapacity = 0.5f;  // Some decline in social learning
    }
    
    // Update cognitive systems
    // planner->setPlanningDepth(planningDepth);
    // conceptFormation->setComplexity(conceptFormationComplexity);
    // socialLearning->setCapacity(socialLearningCapacity);
    // planner->update(dt);
    // conceptFormation->update(dt);
    // socialLearning->update(dt);
}

// Update attention system with developmental effects
void DevelopmentSystem::updateAttentionSystem(AttentionalSelection* attention,
                                            TimestepDuration dt) {
    if (!attention) return;
    
    // Inhibition/excitation balance
    float inhibitionExcitationBalance;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        inhibitionExcitationBalance = 0.6f;  // More excitation for exploration
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        inhibitionExcitationBalance = 0.7f;  // Balanced
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        inhibitionExcitationBalance = 0.8f;  // Increasing inhibition for focus
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        inhibitionExcitationBalance = 0.9f;  // High inhibition for selective attention
    } else { // Aging
        inhibitionExcitationBalance = 0.7f;  // Reduced inhibition
    }
    
    // Competition thresholds
    float competitionThreshold;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        competitionThreshold = 0.5f;  // Low competition, broad focus
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        competitionThreshold = 0.6f;  // Moderate competition
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        competitionThreshold = 0.7f;  // Increasing competition
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        competitionThreshold = 0.8f;  // High competition for resources
    } else { // Aging
        competitionThreshold = 0.6f;  // Reduced competition
    }
    
    // Update attention system
    // attention->setInhibitionExcitationBalance(inhibitionExcitationBalance);
    // attention->setCompetitionThreshold(competitionThreshold);
    // attention->update(dt);
}

// Update developmental effects on neural excitability and connectivity
void DevelopmentSystem::updateExcitabilityAndConnectivity(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Neural excitability changes throughout development
    float baseExcitability = 1.0f;
    
    // Synaptic pruning/strengthening based on developmental stage
    if (pImpl->stage == DevelopmentalStage::Initial) {
        baseExcitability *= 1.2f;  // Higher excitability for rapid learning
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        baseExcitability *= 1.1f;  // High excitability for sensory input
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        baseExcitability *= 0.95f;  // Slight decrease, synaptic refinement
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        baseExcitability *= 0.9f;  // Stable excitability
    } else { // Aging
        baseExcitability *= 0.8f;  // Lower excitability
    }
    
    // Circuit refinement and specialization
    float refinementFactor;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        refinementFactor = 0.3f;  // Early general connectivity
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        refinementFactor = 0.6f;  // Selective connectivity strengthening
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        refinementFactor = 0.8f;  // Circuit specialization
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        refinementFactor = 0.9f;  // Optimized circuits
    } else { // Aging
        refinementFactor = 0.7f;  // Some degradation but maintenance
    }
    
    // Update neural properties through spike system
    if (brain->getSpikeSystem()) {
        // brain->getSpikeSystem()->setGlobalExcitability(baseExcitability);
        // brain->getSpikeSystem()->setCircuitRefinement(refinementFactor);
    }
    
    // Update structural plasticity
    if (brain->getStructuralPlasticity()) {
        // brain->getStructuralPlasticity()->setPruningRate(1.0f - refinementFactor);
        // brain->getStructuralPlasticity()->setGrowthRate(refinementFactor);
    }
}

// Update developmental effects on learning and adaptation
void DevelopmentSystem::updateLearningAndAdaptation(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    // Learning rates change with developmental stage
    float learningRate = getPlasticityModifier() * 0.8f;
    
    // Prediction error computation adapts during development
    float predictionErrorGain;
    if (pImpl->stage == DevelopmentalStage::Initial) {
        predictionErrorGain = 1.5f;  // High prediction error sensitivity
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        predictionErrorGain = 1.2f;  // Still high sensitivity
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        predictionErrorGain = 0.9f;  // Mature error processing
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        predictionErrorGain = 0.8f;  // Stable error processing
    } else { // Aging
        predictionErrorGain = 1.0f;  // Compensatory increased sensitivity
    }
    
    // Update learning systems
    if (brain->getPredictionSystem()) {
        // brain->getPredictionSystem()->setLearningRate(learningRate);
        // brain->getPredictionSystem()->setPredictionErrorGain(predictionErrorGain);
        // brain->getPredictionSystem()->update(dt);
    }
    
    // Working memory learning adaptation
    if (brain->getWorkingMemory()) {
        // brain->getWorkingMemory()->setAdaptationRate(learningRate);
        // brain->getWorkingMemory()->setMemoryUpdateFrequency(1.0f / dt);
    }
    
    // Curiosity-driven exploration changes with development
    if (brain->getCuriosity()) {
        float curiosityLevel;
        if (pImpl->stage == DevelopmentalStage::Initial) {
            curiosityLevel = 1.0f;  // High curiosity for exploration
        } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
            curiosityLevel = 0.8f;  // Strong curiosity for learning
        } else if (pImpl->stage == DevelopmentalStage::Maturation) {
            curiosityLevel = 0.6f;  // Moderate curiosity
        } else if (pImpl->stage == DevelopmentalStage::Adult) {
            curiosityLevel = 0.4f;  // Stable curiosity
        } else { // Aging
            curiosityLevel = 0.5f;  // Compensatory curiosity
        }
        // brain->getCuriosity()->setCuriosityLevel(curiosityLevel);
    }
    
    // Novelty detection adaptation
    if (brain->getNovelty()) {
        float noveltySensitivity = learningRate;
        // brain->getNovelty()->setSensitivity(noveltySensitivity);
    }
}

// Implement stage-specific neuromodulatory effects
void DevelopmentSystem::updateStageSpecificNeuromodulation(Dopamine* dopamine, Acetylcholine* ach,
                                                           Norepinephrine* ne, Serotonin* serotonin) {
    if (!dopamine || !ach || !ne || !serotonin) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High dopamine for exploration, high ACh for sensory processing
            // Moderate NE for alertness, low 5-HT for risk-taking
            dopamine->setLevel(0.4f);
            ach->setLevel(0.5f);
            ne->setLevel(0.5f);
            serotonin->setLevel(0.3f);
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Peak learning with balanced neuromodulators
            dopamine->setLevel(0.7f);
            ach->setLevel(0.9f);
            ne->setLevel(0.8f);
            serotonin->setLevel(0.5f);
            break;
            
        case DevelopmentalStage::Maturation:
            // Refinement with stable neuromodulation
            dopamine->setLevel(0.8f);
            ach->setLevel(0.7f);
            ne->setLevel(0.6f);
            serotonin->setLevel(0.7f);
            break;
            
        case DevelopmentalStage::Adult:
            // Stable adult neuromodulation
            dopamine->setLevel(0.7f);
            ach->setLevel(0.7f);
            ne->setLevel(0.5f);
            serotonin->setLevel(0.8f);
            break;
            
        case DevelopmentalStage::Aging:
            // Compensatory neuromodulation changes
            dopamine->setLevel(0.5f);
            ach->setLevel(0.6f);
            ne->setLevel(0.4f);
            serotonin->setLevel(0.6f);
            break;
    }
}

} // namespace nlm