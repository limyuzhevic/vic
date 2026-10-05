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
    
    // Apply comprehensive developmental effects based on current stage
    applyDevelopmentalEffects(brain, rng, dt);
}

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    float plasticityMod = getPlasticityModifier();
    float developmentalInfluence = calculateDevelopmentalInfluence();
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            applyInitialDevelopmentalEffects(brain, rng, dt, plasticityMod, developmentalInfluence);
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodEffects(brain, rng, dt, plasticityMod, developmentalInfluence);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturationEffects(brain, rng, dt, plasticityMod, developmentalInfluence);
            break;
        case DevelopmentalStage::Adult:
            applyAdultEffects(brain, rng, dt, plasticityMod, developmentalInfluence);
            break;
        case DevelopmentalStage::Aging:
            applyAgingEffects(brain, rng, dt, plasticityMod, developmentalInfluence);
            break;
    }
}

void DevelopmentSystem::applyInitialDevelopmentalEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt, float plasticityMod, float developmentalInfluence) {
    NLM_LOG_INFO("Applying Initial stage developmental effects");
    
    // Initial stage: high plasticity, rapid synaptogenesis
    if (auto* sp = brain->getStructuralPlasticity()) {
        // Increase synaptogenesis rate dramatically
        float currentRate = sp->getSynaptogenesisRate();
        sp->setSynaptogenesisRate(currentRate * (1.0f + developmentalInfluence * 2.0f));
        
        // Reduce pruning significantly
        float currentPruning = sp->getPruningRate();
        sp->setPruningRate(currentPruning * (1.0f - developmentalInfluence * 0.5f));
    }
    
    // High neuromodulation for exploration and development
    if (auto* novelty = brain->getNovelty()) {
        float noveltyLevel = novelty->getLevel();
        novelty->setLevel(std::min(1.0f, noveltyLevel + 0.1f));
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        curiosity->setLevel(std::min(1.0f, curiosityLevel + 0.15f));
    }
    
    // High dopamine for reward sensitivity during initial learning
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        dopamine->setLevel(std::min(1.0f, dopamineLevel + 0.2f));
    }
    
    // Rapid neural connectivity establishment
    for (auto* region : brain->getRegions()) {
        size_t neuronCount = region->getTotalNeuronCount();
        if (neuronCount > 10) {
            // Increase connectivity probability during development
            float connectionProb = 0.3f + developmentalInfluence * 0.3f;
            // In a full implementation, this would call region->initializeRandomConnectivity
            // with the updated connection probability
        }
    }
    
    // Develop strong working memory capacity
    if (auto* workingMemory = brain->getWorkingMemory()) {
        size_t currentCapacity = workingMemory->getCapacity();
        workingMemory->setCapacity(static_cast<size_t>(currentCapacity * 1.5f));
    }
}

void DevelopmentSystem::applyCriticalPeriodEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt, float plasticityMod, float developmentalInfluence) {
    NLM_LOG_INFO("Applying CriticalPeriod stage developmental effects");
    
    // Critical period: enhanced sensory processing and refinement
    if (auto* attention = brain->getAttention()) {
        // Fine-tune attention parameters for critical learning
        attention->setInhibitionStrength(0.6f);  // Slightly stronger inhibition for refinement
        attention->setExcitationStrength(1.3f);  // Slightly stronger excitation
    }
    
    // Enhanced neuromodulation for learning precision
    if (auto* novelty = brain->getNovelty()) {
        float noveltyLevel = novelty->getLevel();
        novelty->setLevel(std::max(0.2f, noveltyLevel - 0.05f));  // Stabilize from peak
    }
    
    // Develop prediction systems for efficient processing
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        // pImpl->predictionSystem->enablePredictionLearning(true);
    }
    
    // Enhance working memory efficiency
    if (auto* workingMemory = brain->getWorkingMemory()) {
        // Optimize working memory for pattern storage
        float decayRate = workingMemory->getDecayRate();
        workingMemory->setDecayRate(decayRate * 0.8f);  // Slower decay for learning
        
        // Strengthen memory traces
        workingMemory->strengthenMemory(1.2f);
    }
    
    // Develop basic concept formation
    if (auto* conceptFormation = brain->getConceptFormation()) {
        // pImpl->conceptFormation->enableBasicConcepts(true);
    }
}

void DevelopmentSystem::applyMaturationEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt, float plasticityMod, float developmentalInfluence) {
    NLM_LOG_INFO("Applying Maturation stage developmental effects");
    
    // Maturation: balance between stability and continued learning
    if (auto* sp = brain->getStructuralPlasticity()) {
        // Moderate structural changes
        float currentRate = sp->getSynaptogenesisRate();
        sp->setSynaptogenesisRate(currentRate * 0.7f);
        
        float currentPruning = sp->getPruningRate();
        sp->setPruningRate(currentPruning * 1.2f);
    }
    
    // Develop complex cognitive functions
    if (auto* conceptFormation = brain->getConceptFormation()) {
        // pImpl->conceptFormation->enableAdvancedConcepts(true);
    }
    
    // Refine attention mechanisms
    if (auto* attention = brain->getAttention()) {
        // pImpl->attention->optimizeForComplexTasks(true);
        attention->setInhibitionStrength(0.5f);  // Optimized balance
        attention->setExcitationStrength(1.5f);
    }
    
    // Develop planning capabilities
    if (auto* planner = brain->getPlanner()) {
        // pImpl->planner->enableComplexPlanning(true);
        planner->setPlanningDepth(8);  // More sophisticated planning
    }
    
    // Stabilize neuromodulation levels
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        dopamine->setLevel(std::max(0.3f, dopamineLevel - 0.1f));  // Stabilize
    }
}

void DevelopmentSystem::applyAdultEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt, float plasticityMod, float developmentalInfluence) {
    NLM_LOG_INFO("Applying Adult stage developmental effects");
    
    // Adult: stable, efficient operation with maintenance
    if (auto* sp = brain->getStructuralPlasticity()) {
        // Stabilize structural plasticity
        float currentRate = sp->getSynaptogenesisRate();
        sp->setSynaptogenesisRate(currentRate * 0.3f);
        
        float currentPruning = sp->getPruningRate();
        sp->setPruningRate(currentPruning * 1.5f);
    }
    
    // Focus on efficiency and expertise
    if (auto* conceptFormation = brain->getConceptFormation()) {
        // pImpl->conceptFormation->enableExpertiseLearning(true);
    }
    
    // Optimize attention for focused processing
    if (auto* attention = brain->getAttention()) {
        attention->setInhibitionStrength(0.4f);  // Weaker inhibition for expertise
        attention->setExcitationStrength(1.8f);  // Stronger excitation for expertise
    }
    
    // Maintain planning capabilities but reduce complexity
    if (auto* planner = brain->getPlanner()) {
        // pImpl->planner->enableEfficiencyOptimization(true);
        planner->setPlanningDepth(5);  // Manageable planning depth
    }
    
    // Stabilize all neuromodulation systems
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        dopamine->setLevel(std::max(0.2f, dopamineLevel - 0.15f));  // Stable low level
    }
    if (auto* curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        curiosity->setLevel(std::max(0.1f, curiosityLevel - 0.2f));  // Reduced exploration
    }
}

void DevelopmentSystem::applyAgingEffects(Brain* brain, RandomGenerator& rng, TimestepDuration dt, float plasticityMod, float developmentalInfluence) {
    NLM_LOG_INFO("Applying Aging stage developmental effects");
    
    // Aging: gradual decline with wisdom accumulation
    if (auto* sp = brain->getStructuralPlasticity()) {
        // Increase pruning for efficiency
        float currentPruning = sp->getPruningRate();
        sp->setPruningRate(currentPruning * 2.0f);
        
        float currentRate = sp->getSynaptogenesisRate();
        sp->setSynaptogenesisRate(currentRate * 0.1f);
    }
    
    // Wisdom-based decision making (enhanced by accumulated episodic memory)
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        // Use accumulated wisdom for better decisions
        size_t episodeCount = episodicMemory->getEpisodeCount();
        if (episodeCount > 100) {
            // High wisdom level - use episodic memory more effectively
            // pImpl->wisdomLevel = std::min(1.0f, static_cast<float>(episodeCount) / 1000.0f);
        }
    }
    
    // Slower processing but more deliberate
    if (auto* attention = brain->getAttention()) {
        attention->setInhibitionStrength(0.7f);  // Stronger inhibition for deliberate processing
        attention->setExcitationStrength(1.2f);  // Moderate excitation
    }
    
    // Simplified but effective planning
    if (auto* planner = brain->getPlanner()) {
        // pImpl->planner->enableDeliberatePlanning(true);
        planner->setPlanningDepth(3);  // Simpler but more deliberate
    }
    
    // Low neuromodulation levels for stability
    if (auto* dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        dopamine->setLevel(std::max(0.05f, dopamineLevel - 0.25f));  // Very stable low level
    }
    if (auto* curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        curiosity->setLevel(std::max(0.02f, curiosityLevel - 0.3f));  // Minimal exploration
    }
}

float DevelopmentSystem::calculateDevelopmentalInfluence() const {
    // Calculate overall developmental influence factor (0-1)
    if (pImpl->stage == DevelopmentalStage::Initial) {
        return 1.0f;
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        return 0.7f;
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        return 0.4f;
    } else if (pImpl->stage == DevelopmentalStage::Adult) {
        return 0.2f;
    } else {
        return 0.05f;
    }
}

} // namespace nlm
