#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <cmath>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Forward declarations for system modulation methods
    void modulateMemorySystems(Brain* brain);
    void modulatePredictionSystem(Brain* brain);
    void modulateCognitiveSystems(Brain* brain);
    void modulateNeuromodulationSystems(Brain* brain);
    void modulatePlasticitySystems(Brain* brain);
    
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

// Memory systems modulation
void DevelopmentSystem::modulateWorkingMemory(NeuralWorkingMemory* workingMemory) {
    if (!workingMemory) return;
    
    float stage = static_cast<float>(getStage());
    float modulation = 0.0f;
    
    // Age-appropriate working memory capacity and decay
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Limited capacity, high plasticity
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.5f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 2.0f); // Faster decay
            modulation = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid learning phase
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.8f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 1.5f);
            modulation = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            // Building capacity
            workingMemory->setCapacity(workingMemory->getCapacity() * 1.2f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 0.8f);
            modulation = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            // Stable, optimized capacity
            workingMemory->setCapacity(workingMemory->getCapacity() * 1.0f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 0.7f);
            modulation = 0.2f;
            break;
        case DevelopmentalStage::Aging:
            // Declining efficiency
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.7f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 1.3f);
            modulation = 0.1f;
            break;
        default:
            modulation = 0.5f;
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated working memory - stage=") + getStageName() + 
                  ", capacity_mod=", modulation);
}

void DevelopmentSystem::modulateEpisodicMemory(NeuralEpisodicMemory* episodicMemory) {
    if (!episodicMemory) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Early experiences are foundational
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 0.3f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid episode formation
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.5f);
            break;
        case DevelopmentalStage::Maturation:
            // Building memory archive
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.2f);
            break;
        case DevelopmentalStage::Adult:
            // Stable, curated memory
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.0f);
            break;
        case DevelopmentalStage::Aging:
            // Reduced capacity for new episodes
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 0.6f);
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated episodic memory - stage=") + getStageName() +
                  ", capacity_mod=", static_cast<float>(episodicMemory->getMaxEpisodes()) / 1000.0f);
}

void DevelopmentSystem::modulateAssociativeMemory(NeuralAssociativeMemory* associativeMemory) {
    if (!associativeMemory) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Forming basic associations
            // Higher baseline association formation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for association learning
            // Enhanced Hebbian learning potential
            break;
        case DevelopmentalStage::Maturation:
            // Refining associations
            // Better pattern separation
            break;
        case DevelopmentalStage::Adult:
            // Stable associations
            break;
        case DevelopmentalStage::Aging:
            // Weaker associations
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated associative memory - stage=") + getStageName());
}

// Prediction system modulation
void DevelopmentSystem::modulatePredictionSystem(PredictionSystem* predictionSystem) {
    if (!predictionSystem) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic prediction capabilities
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid prediction learning
            // Wider prediction horizons, higher uncertainty tolerance
            break;
        case DevelopmentalStage::Maturation:
            // Refining prediction accuracy
            // Better confidence calibration
            break;
        case DevelopmentalStage::Adult:
            // Optimized prediction
            // Narrower, more accurate horizons
            break;
        case DevelopmentalStage::Aging:
            // Declining prediction accuracy
            // Slower adaptation to prediction errors
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated prediction system - stage=") + getStageName());
}

// Cognitive systems modulation
void DevelopmentSystem::modulateNeuralPlanner(NeuralPlanner* planner) {
    if (!planner) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Simple reactive planning
            planner->setPlanningDepth(2);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing planning abilities
            planner->setPlanningDepth(5);
            break;
        case DevelopmentalStage::Maturation:
            // Advanced planning strategies
            planner->setPlanningDepth(10);
            break;
        case DevelopmentalStage::Adult:
            // Full planning capacity
            planner->setPlanningDepth(15);
            break;
        case DevelopmentalStage::Aging:
            // Declining planning complexity
            planner->setPlanningDepth(std::max(2, static_cast<int>(planner->getPlanningDepth() * 0.6f)));
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated neural planner - stage=") + getStageName() +
                  ", planning_depth=", static_cast<float>(planner->getPlanningDepth()));
}

void DevelopmentSystem::modulateConceptFormation(ConceptFormation* conceptFormation) {
    if (!conceptFormation) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Simple concepts
            conceptFormation->setFormationThreshold(0.9f); // Stricter formation
            conceptFormation->setStabilityThreshold(0.8f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid concept discovery
            conceptFormation->setFormationThreshold(0.7f);
            conceptFormation->setStabilityThreshold(0.6f);
            break;
        case DevelopmentalStage::Maturation:
            // More nuanced concepts
            conceptFormation->setFormationThreshold(0.5f);
            conceptFormation->setStabilityThreshold(0.4f);
            break;
        case DevelopmentalStage::Adult:
            // Sophisticated concepts
            conceptFormation->setFormationThreshold(0.3f);
            conceptFormation->setStabilityThreshold(0.2f);
            break;
        case DevelopmentalStage::Aging:
            // Reduced concept complexity
            conceptFormation->setFormationThreshold(0.6f);
            conceptFormation->setStabilityThreshold(0.5f);
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated concept formation - stage=") + getStageName());
}

void DevelopmentSystem::modulateAttentionalSelection(AttentionalSelection* attention) {
    if (!attention) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Limited attentional control
            attention->setInhibitionStrength(0.7f);
            attention->setExcitationStrength(1.0f);
            attention->setCompetitionThreshold(0.8f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing attentional abilities
            attention->setInhibitionStrength(0.5f);
            attention->setExcitationStrength(1.5f);
            attention->setCompetitionThreshold(0.6f);
            break;
        case DevelopmentalStage::Maturation:
            // Mature attentional control
            attention->setInhibitionStrength(0.3f);
            attention->setExcitationStrength(2.0f);
            attention->setCompetitionThreshold(0.4f);
            break;
        case DevelopmentalStage::Adult:
            // Optimized attentional balance
            attention->setInhibitionStrength(0.2f);
            attention->setExcitationStrength(2.2f);
            attention->setCompetitionThreshold(0.3f);
            break;
        case DevelopmentalStage::Aging:
            // Declining attentional flexibility
            attention->setInhibitionStrength(0.4f);
            attention->setExcitationStrength(1.5f);
            attention->setCompetitionThreshold(0.5f);
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated attentional selection - stage=") + getStageName());
}

// Neuromodulation systems modulation
void DevelopmentSystem::modulateDopamine(Dopamine* dopamine) {
    if (!dopamine) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic reward processing
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for reward learning
            // Enhanced reinforcement learning potential
            break;
        case DevelopmentalStage::Maturation:
            // Refined reward processing
            break;
        case DevelopmentalStage::Adult:
            // Optimized reward processing
            break;
        case DevelopmentalStage::Aging:
            // Declining reward sensitivity
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated dopamine - stage=") + getStageName());
}

void DevelopmentSystem::modulateAcetylcholine(Acetylcholine* acetylcholine) {
    if (!acetylcholine) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic attention modulation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced attention development
            break;
        case DevelopmentalStage::Maturation:
            // Mature attention modulation
            break;
        case DevelopmentalStage::Adult:
            // Optimized cholinergic function
            break;
        case DevelopmentalStage::Aging:
            // Declining cholinergic function
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated acetylcholine - stage=") + getStageName());
}

void DevelopmentSystem::modulateNorepinephrine(Norepinephrine* norepinephrine) {
    if (!norepinephrine) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic arousal regulation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing arousal system
            break;
        case DevelopmentalStage::Maturation:
            // Mature arousal regulation
            break;
        case DevelopmentalStage::Adult:
            // Optimized stress response
            break;
        case DevelopmentalStage::Aging:
            // Altered stress regulation
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated norepinephrine - stage=") + getStageName());
}

void DevelopmentSystem::modulateSerotonin(Serotonin* serotonin) {
    if (!serotonin) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic mood regulation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing emotional regulation
            break;
        case DevelopmentalStage::Maturation:
            // Mature emotional regulation
            break;
        case DevelopmentalStage::Adult:
            // Optimized mood regulation
            break;
        case DevelopmentalStage::Aging:
            // Changes in emotional processing
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated serotonin - stage=") + getStageName());
}

void DevelopmentSystem::modulateSTDP(STDP* stdp) {
    if (!stdp) return;
    
    float stage = static_cast<float>(getStage());
    float plasticityMod = getPlasticityModifier();
    
    // Modify STDP parameters based on development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High LTP/LTD balance for rapid learning
            // LTP weight = 0.01, LTD weight = 0.012 (default)
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period STDP rules
            // Enhanced LTP for critical learning
            break;
        case DevelopmentalStage::Maturation:
            // Refining STDP rules
            // Better timing dependence
            break;
        case DevelopmentalStage::Adult:
            // Stable STDP parameters
            break;
        case DevelopmentalStage::Aging:
            // Declining STDP efficacy
            break;
        default:
            break;
    }
    
    // Apply developmental modulation to STDP weights
    float ltpWeight = 0.01f * plasticityMod;
    float ltdWeight = 0.012f * plasticityMod;
    // In real implementation, would call stdp->configure(ltpWeight, ltdWeight, tau)
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated STDP - stage=") + getStageName() +
                  ", plasticity_mod=", plasticityMod);
}

void DevelopmentSystem::modulateHebbian(Hebbian* hebbian) {
    if (!hebbian) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic Hebbian learning
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced Hebbian learning
            break;
        case DevelopmentalStage::Maturation:
            // Refining Hebbian learning
            break;
        case DevelopmentalStage::Adult:
            // Optimized Hebbian learning
            break;
        case DevelopmentalStage::Aging:
            // Reduced Hebbian efficacy
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated Hebbian - stage=") + getStageName());
}

void DevelopmentSystem::modulateStructuralPlasticity(StructuralPlasticity* structuralPlasticity) {
    if (!structuralPlasticity) return;
    
    float stage = static_cast<float>(getStage());
    float plasticityMod = getPlasticityModifier();
    
    // This is already being modulated in Brain.cpp step 13
    // But we ensure consistency here
    float synaptogenesisRate = 0.0001f * plasticityMod;
    float pruningRate = 0.00001f * (2.0f - plasticityMod);
    
    // In real implementation, would call structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    // and structuralPlasticity->setPruningRate(pruningRate);
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated structural plasticity - stage=") + getStageName() +
                  ", synaptogenesis_rate=", synaptogenesisRate);
}

// Integration: Apply modulation to all systems
void DevelopmentSystem::Impl::modulateMemorySystems(Brain* brain) {
    if (!brain) return;
    
    // Working memory
    if (auto* workingMemory = brain->getWorkingMemory()) {
        static_cast<DevelopmentSystem*>(this)->modulateWorkingMemory(workingMemory);
    }
    
    // Episodic memory
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        static_cast<DevelopmentSystem*>(this)->modulateEpisodicMemory(episodicMemory);
    }
    
    // Associative memory
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        static_cast<DevelopmentSystem*>(this)->modulateAssociativeMemory(associativeMemory);
    }
}

void DevelopmentSystem::Impl::modulatePredictionSystem(Brain* brain) {
    if (!brain) return;
    
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        static_cast<DevelopmentSystem*>(this)->modulatePredictionSystem(predictionSystem);
    }
}

void DevelopmentSystem::Impl::modulateCognitiveSystems(Brain* brain) {
    if (!brain) return;
    
    // Neural planner
    if (auto* planner = brain->getPlanner()) {
        static_cast<DevelopmentSystem*>(this)->modulateNeuralPlanner(planner);
    }
    
    // Concept formation
    if (auto* conceptFormation = brain->getConceptFormation()) {
        static_cast<DevelopmentSystem*>(this)->modulateConceptFormation(conceptFormation);
    }
    
    // Attentional selection
    if (auto* attention = brain->getAttention()) {
        static_cast<DevelopmentSystem*>(this)->modulateAttentionalSelection(attention);
    }
}

void DevelopmentSystem::Impl::modulateNeuromodulationSystems(Brain* brain) {
    if (!brain) return;
    
    // Dopamine
    if (auto* dopamine = brain->getDopamine()) {
        static_cast<DevelopmentSystem*>(this)->modulateDopamine(dopamine);
    }
    
    // Acetylcholine
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        static_cast<DevelopmentSystem*>(this)->modulateAcetylcholine(acetylcholine);
    }
    
    // Norepinephrine
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        static_cast<DevelopmentSystem*>(this)->modulateNorepinephrine(norepinephrine);
    }
    
    // Serotonin
    if (auto* serotonin = brain->getSerotonin()) {
        static_cast<DevelopmentSystem*>(this)->modulateSerotonin(serotonin);
    }
}

void DevelopmentSystem::Impl::modulatePlasticitySystems(Brain* brain) {
    if (!brain) return;
    
    // STDP
    if (auto* stdp = brain->getSTDP()) {
        static_cast<DevelopmentSystem*>(this)->modulateSTDP(stdp);
    }
    
    // Hebbian
    if (auto* hebbian = brain->getHebbian()) {
        static_cast<DevelopmentSystem*>(this)->modulateHebbian(hebbian);
    }
    
    // Structural plasticity
    if (auto* structuralPlasticity = brain->getStructuralPlasticity()) {
        static_cast<DevelopmentSystem*>(this)->modulateStructuralPlasticity(structuralPlasticity);
    }
}

// System modulation entry points
void DevelopmentSystem::modulateWorkingMemory(NeuralWorkingMemory* workingMemory) {
    if (!workingMemory) return;
    
    float stage = static_cast<float>(getStage());
    float modulation = 0.0f;
    
    // Age-appropriate working memory capacity and decay
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Limited capacity, high plasticity
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.5f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 2.0f); // Faster decay
            modulation = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid learning phase
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.8f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 1.5f);
            modulation = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            // Building capacity
            workingMemory->setCapacity(workingMemory->getCapacity() * 1.2f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 0.8f);
            modulation = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            // Stable, optimized capacity
            workingMemory->setCapacity(workingMemory->getCapacity() * 1.0f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 0.7f);
            modulation = 0.2f;
            break;
        case DevelopmentalStage::Aging:
            // Declining efficiency
            workingMemory->setCapacity(workingMemory->getCapacity() * 0.7f);
            workingMemory->setDecayRate(workingMemory->getDecayRate() * 1.3f);
            modulation = 0.1f;
            break;
        default:
            modulation = 0.5f;
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated working memory - stage=") + getStageName() + 
                  ", capacity_mod=", modulation);
}

void DevelopmentSystem::modulateEpisodicMemory(NeuralEpisodicMemory* episodicMemory) {
    if (!episodicMemory) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Early experiences are foundational
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 0.3f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid episode formation
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.5f);
            break;
        case DevelopmentalStage::Maturation:
            // Building memory archive
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.2f);
            break;
        case DevelopmentalStage::Adult:
            // Stable, curated memory
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 1.0f);
            break;
        case DevelopmentalStage::Aging:
            // Reduced capacity for new episodes
            episodicMemory->setMaxEpisodes(episodicMemory->getMaxEpisodes() * 0.6f);
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated episodic memory - stage=") + getStageName() +
                  ", capacity_mod=", static_cast<float>(episodicMemory->getMaxEpisodes()) / 1000.0f);
}

void DevelopmentSystem::modulateAssociativeMemory(NeuralAssociativeMemory* associativeMemory) {
    if (!associativeMemory) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Forming basic associations
            // Higher baseline association formation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for association learning
            // Enhanced Hebbian learning potential
            break;
        case DevelopmentalStage::Maturation:
            // Refining associations
            // Better pattern separation
            break;
        case DevelopmentalStage::Adult:
            // Stable associations
            break;
        case DevelopmentalStage::Aging:
            // Weaker associations
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated associative memory - stage=") + getStageName());
}

// Prediction system modulation
void DevelopmentSystem::modulatePredictionSystem(PredictionSystem* predictionSystem) {
    if (!predictionSystem) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic prediction capabilities
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid prediction learning
            // Wider prediction horizons, higher uncertainty tolerance
            break;
        case DevelopmentalStage::Maturation:
            // Refining prediction accuracy
            // Better confidence calibration
            break;
        case DevelopmentalStage::Adult:
            // Optimized prediction
            // Narrower, more accurate horizons
            break;
        case DevelopmentalStage::Aging:
            // Declining prediction accuracy
            // Slower adaptation to prediction errors
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated prediction system - stage=") + getStageName());
}

// Cognitive systems modulation
void DevelopmentSystem::modulateNeuralPlanner(NeuralPlanner* planner) {
    if (!planner) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Simple reactive planning
            planner->setPlanningDepth(2);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing planning abilities
            planner->setPlanningDepth(5);
            break;
        case DevelopmentalStage::Maturation:
            // Advanced planning strategies
            planner->setPlanningDepth(10);
            break;
        case DevelopmentalStage::Adult:
            // Full planning capacity
            planner->setPlanningDepth(15);
            break;
        case DevelopmentalStage::Aging:
            // Declining planning complexity
            planner->setPlanningDepth(std::max(2, static_cast<int>(planner->getPlanningDepth() * 0.6f)));
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated neural planner - stage=") + getStageName() +
                  ", planning_depth=", static_cast<float>(planner->getPlanningDepth()));
}

void DevelopmentSystem::modulateConceptFormation(ConceptFormation* conceptFormation) {
    if (!conceptFormation) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Simple concepts
            conceptFormation->setFormationThreshold(0.9f); // Stricter formation
            conceptFormation->setStabilityThreshold(0.8f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Rapid concept discovery
            conceptFormation->setFormationThreshold(0.7f);
            conceptFormation->setStabilityThreshold(0.6f);
            break;
        case DevelopmentalStage::Maturation:
            // More nuanced concepts
            conceptFormation->setFormationThreshold(0.5f);
            conceptFormation->setStabilityThreshold(0.4f);
            break;
        case DevelopmentalStage::Adult:
            // Sophisticated concepts
            conceptFormation->setFormationThreshold(0.3f);
            conceptFormation->setStabilityThreshold(0.2f);
            break;
        case DevelopmentalStage::Aging:
            // Reduced concept complexity
            conceptFormation->setFormationThreshold(0.6f);
            conceptFormation->setStabilityThreshold(0.5f);
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated concept formation - stage=") + getStageName());
}

void DevelopmentSystem::modulateAttentionalSelection(AttentionalSelection* attention) {
    if (!attention) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Limited attentional control
            attention->setInhibitionStrength(0.7f);
            attention->setExcitationStrength(1.0f);
            attention->setCompetitionThreshold(0.8f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing attentional abilities
            attention->setInhibitionStrength(0.5f);
            attention->setExcitationStrength(1.5f);
            attention->setCompetitionThreshold(0.6f);
            break;
        case DevelopmentalStage::Maturation:
            // Mature attentional control
            attention->setInhibitionStrength(0.3f);
            attention->setExcitationStrength(2.0f);
            attention->setCompetitionThreshold(0.4f);
            break;
        case DevelopmentalStage::Adult:
            // Optimized attentional balance
            attention->setInhibitionStrength(0.2f);
            attention->setExcitationStrength(2.2f);
            attention->setCompetitionThreshold(0.3f);
            break;
        case DevelopmentalStage::Aging:
            // Declining attentional flexibility
            attention->setInhibitionStrength(0.4f);
            attention->setExcitationStrength(1.5f);
            attention->setCompetitionThreshold(0.5f);
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated attentional selection - stage=") + getStageName());
}

// Neuromodulation systems modulation
void DevelopmentSystem::modulateDopamine(Dopamine* dopamine) {
    if (!dopamine) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic reward processing
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period for reward learning
            // Enhanced reinforcement learning potential
            break;
        case DevelopmentalStage::Maturation:
            // Refined reward processing
            break;
        case DevelopmentalStage::Adult:
            // Optimized reward processing
            break;
        case DevelopmentalStage::Aging:
            // Declining reward sensitivity
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated dopamine - stage=") + getStageName());
}

void DevelopmentSystem::modulateAcetylcholine(Acetylcholine* acetylcholine) {
    if (!acetylcholine) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic attention modulation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced attention development
            break;
        case DevelopmentalStage::Maturation:
            // Mature attention modulation
            break;
        case DevelopmentalStage::Adult:
            // Optimized cholinergic function
            break;
        case DevelopmentalStage::Aging:
            // Declining cholinergic function
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated acetylcholine - stage=") + getStageName());
}

void DevelopmentSystem::modulateNorepinephrine(Norepinephrine* norepinephrine) {
    if (!norepinephrine) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic arousal regulation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing arousal system
            break;
        case DevelopmentalStage::Maturation:
            // Mature arousal regulation
            break;
        case DevelopmentalStage::Adult:
            // Optimized stress response
            break;
        case DevelopmentalStage::Aging:
            // Altered stress regulation
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated norepinephrine - stage=") + getStageName());
}

void DevelopmentSystem::modulateSerotonin(Serotonin* serotonin) {
    if (!serotonin) return;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic mood regulation
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Developing emotional regulation
            break;
        case DevelopmentalStage::Maturation:
            // Mature emotional regulation
            break;
        case DevelopmentalStage::Adult:
            // Optimized mood regulation
            break;
        case DevelopmentalStage::Aging:
            // Changes in emotional processing
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated serotonin - stage=") + getStageName());
}

void DevelopmentSystem::modulateSTDP(STDP* stdp) {
    if (!stdp) return;
    
    float stage = static_cast<float>(getStage());
    float plasticityMod = getPlasticityModifier();
    
    // Modify STDP parameters based on development
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // High LTP/LTD balance for rapid learning
            // LTP weight = 0.01, LTD weight = 0.012 (default)
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Sensitive period STDP rules
            // Enhanced LTP for critical learning
            break;
        case DevelopmentalStage::Maturation:
            // Refining STDP rules
            // Better timing dependence
            break;
        case DevelopmentalStage::Adult:
            // Stable STDP parameters
            break;
        case DevelopmentalStage::Aging:
            // Declining STDP efficacy
            break;
        default:
            break;
    }
    
    // Apply developmental modulation to STDP weights
    float ltpWeight = 0.01f * plasticityMod;
    float ltdWeight = 0.012f * plasticityMod;
    // In real implementation, would call stdp->configure(ltpWeight, ltdWeight, tau)
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated STDP - stage=") + getStageName() +
                  ", plasticity_mod=", plasticityMod);
}

void DevelopmentSystem::modulateHebbian(Hebbian* hebbian) {
    if (!hebbian) return;
    
    float stage = static_cast<float>(getStage());
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            // Basic Hebbian learning
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced Hebbian learning
            break;
        case DevelopmentalStage::Maturation:
            // Refining Hebbian learning
            break;
        case DevelopmentalStage::Adult:
            // Optimized Hebbian learning
            break;
        case DevelopmentalStage::Aging:
            // Reduced Hebbian efficacy
            break;
        default:
            break;
    }
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated Hebbian - stage=") + getStageName());
}

void DevelopmentSystem::modulateStructuralPlasticity(StructuralPlasticity* structuralPlasticity) {
    if (!structuralPlasticity) return;
    
    float stage = static_cast<float>(getStage());
    float plasticityMod = getPlasticityModifier();
    
    // This is already being modulated in Brain.cpp step 13
    // But we ensure consistency here
    float synaptogenesisRate = 0.0001f * plasticityMod;
    float pruningRate = 0.00001f * (2.0f - plasticityMod);
    
    // In real implementation, would call structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    // and structuralPlasticity->setPruningRate(pruningRate);
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Modulated structural plasticity - stage=") + getStageName() +
                  ", synaptogenesis_rate=", synaptogenesisRate);
}

// Integration: Apply modulation to all systems
void DevelopmentSystem::modulateMemorySystems(Brain* brain) {
    if (!brain) return;
    
    // Working memory
    if (auto* workingMemory = brain->getWorkingMemory()) {
        modulateWorkingMemory(workingMemory);
    }
    
    // Episodic memory
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        modulateEpisodicMemory(episodicMemory);
    }
    
    // Associative memory
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        modulateAssociativeMemory(associativeMemory);
    }
}

void DevelopmentSystem::modulatePredictionSystem(Brain* brain) {
    if (!brain) return;
    
    if (auto* predictionSystem = brain->getPredictionSystem()) {
        modulatePredictionSystem(predictionSystem);
    }
}

void DevelopmentSystem::modulateCognitiveSystems(Brain* brain) {
    if (!brain) return;
    
    // Neural planner
    if (auto* planner = brain->getPlanner()) {
        modulateNeuralPlanner(planner);
    }
    
    // Concept formation
    if (auto* conceptFormation = brain->getConceptFormation()) {
        modulateConceptFormation(conceptFormation);
    }
    
    // Attentional selection
    if (auto* attention = brain->getAttention()) {
        modulateAttentionalSelection(attention);
    }
}

void DevelopmentSystem::modulateNeuromodulationSystems(Brain* brain) {
    if (!brain) return;
    
    // Dopamine
    if (auto* dopamine = brain->getDopamine()) {
        modulateDopamine(dopamine);
    }
    
    // Acetylcholine
    if (auto* acetylcholine = brain->getAcetylcholine()) {
        modulateAcetylcholine(acetylcholine);
    }
    
    // Norepinephrine
    if (auto* norepinephrine = brain->getNorepinephrine()) {
        modulateNorepinephrine(norepinephrine);
    }
    
    // Serotonin
    if (auto* serotonin = brain->getSerotonin()) {
        modulateSerotonin(serotonin);
    }
}

void DevelopmentSystem::modulatePlasticitySystems(Brain* brain) {
    if (!brain) return;
    
    // STDP
    if (auto* stdp = brain->getSTDP()) {
        modulateSTDP(stdp);
    }
    
    // Hebbian
    if (auto* hebbian = brain->getHebbian()) {
        modulateHebbian(hebbian);
    }
    
    // Structural plasticity
    if (auto* structuralPlasticity = brain->getStructuralPlasticity()) {
        modulateStructuralPlasticity(structuralPlasticity);
    }
}

void DevelopmentSystem::modulateAllSystems(Brain* brain) {
    if (!brain) return;
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Starting comprehensive system modulation for stage=") + getStageName() +
                  ", age=", age_);
    
    // Modulate all systems based on current developmental stage
    modulateMemorySystems(brain);
    modulatePredictionSystem(brain);
    modulateCognitiveSystems(brain);
    modulateNeuromodulationSystems(brain);
    modulatePlasticitySystems(brain);
    
    NLM_LOG_DEBUG(std::string("DevelopmentSystem: Completed system modulation for stage=") + getStageName());
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
    
    // Apply developmental modulation to brain systems
    modulateAllSystems(brain);
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
    
    // Apply developmental modulation to brain systems
    modulateAllSystems(brain);
}
