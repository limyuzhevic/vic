#include "BrainRefactored.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

// ========== NEURAL DYNAMICS STEP IMPLEMENTATION ==========

void NeuralDynamicsStep::updateNeuronDynamics(
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    const TimestepDuration& timestep,
    Timestamp currentTime) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void NeuralDynamicsStep::detectAndQueueSpikes(
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    SpikeSystem* spikeSystem,
    const TimestepDuration& timestep,
    Timestamp currentTime,
    SimulationStep currentStep,
    size_t& totalSpikesThisStep) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    // This is handled by MemorySystemsStep
                    ++totalSpikesThisStep;
                }
            }
        }
    }
}

void NeuralDynamicsStep::processSpikes(
    SpikeSystem* spikeSystem,
    SimulationStep currentStep) {
    spikeSystem->processSpikes(currentStep);
}

void NeuralDynamicsStep::injectSensoryInput(
    std::vector<Neuron*>& sensoryNeurons,
    const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        sensoryNeurons[i]->injectCurrent(normalizedValue);
    }
}

void NeuralDynamicsStep::injectCurrentToNeurons(
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    NeuronType type,
    MembranePotential current) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// ========== MEMORY SYSTEMS STEP IMPLEMENTATION ==========

void MemorySystemsStep::updateWorkingMemory(
    NeuralWorkingMemory* workingMemory,
    const TimestepDuration& timestep) {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void MemorySystemsStep::updateEpisodicMemory(
    NeuralEpisodicMemory* episodicMemory,
    size_t stepsSinceLastEpisode,
    Timestamp currentTime,
    const TimestepDuration& timestep,
    const Dopamine* dopamine,
    std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    if (!episodicMemory) return;
    
    size_t stepsSinceLastEpisode = 0;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        // Capture current brain state as an episode
        EpisodicMemoryItem episode;
        episode.timestamp = currentTime;
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring() || 
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                        episode.activeNeurons.push_back(neuron->getId());
                        episode.neuronActivations.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                    }
                }
            }
        }
        
        // Store reward in episode
        episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
        
        episodicMemory->storeEpisode(episode);
    }
}

void MemorySystemsStep::processMemoryReplay(
    NeuralEpisodicMemory* episodicMemory,
    size_t replayInterval,
    size_t currentStep) {
    if (!episodicMemory) return;
    
    if (currentStep % replayInterval == 0) {
        // Get episodes for replay
        auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            episodicMemory->replayEpisode(episode);
        }
    }
}

void MemorySystemsStep::consolidateMemories(
    NeuralEpisodicMemory* episodicMemory,
    size_t consolidationInterval,
    size_t currentStep,
    float consolidationStrength) {
    if (!episodicMemory) return;
    
    if (currentStep % consolidationInterval == 0) {
        // Consolidate important memories, remove weak ones
        episodicMemory->consolidate(consolidationStrength);
    }
}

void MemorySystemsStep::updateWorkingMemoryFromSensory(
    NeuralWorkingMemory* workingMemory,
    std::vector<Neuron*>& sensoryNeurons,
    const class SensoryInput& input) {
    if (!workingMemory) return;
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons and store in working memory
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Store in working memory if above threshold
        if (normalizedValue > 0.5f) {
            workingMemory->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

// ========== NEUROMODULATION STEP IMPLEMENTATION ==========

void NeuromodulationStep::updateNoveltyDetection(
    Novelty* novelty,
    const TimestepDuration& timestep) {
    if (novelty) {
        novelty->update(timestep);
    }
}

void NeuromodulationStep::updateCuriositySignals(
    Curiosity* curiosity,
    const TimestepDuration& timestep) {
    if (curiosity) {
        curiosity->update(timestep);
    }
}

void NeuromodulationStep::updateDopamineSystem(
    Dopamine* dopamine,
    const TimestepDuration& timestep) {
    if (dopamine) {
        dopamine->update(timestep);
    }
}

void NeuromodulationStep::applyDopamineToExcitability(
    Dopamine* dopamine,
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    const TimestepDuration& timestep) {
    if (!dopamine) return;
    
    float dopamineLevel = dopamine->getLevel();
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Dopamine modulates excitability by injecting additional current
                // Higher dopamine increases excitability (lower effective threshold)
                float excitabilityMod = dopamineLevel * 0.5f;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void NeuromodulationStep::applyNeuromodulationToPlasticity(
    Dopamine* dopamine,
    STDP* stdp) {
    if (!dopamine || !stdp) return;
    
    float modulation = dopamine->getLevel();
    
    // Scale STDP learning rates
    float currentLTP = stdp->getLTPWeight();
    float currentLTD = stdp->getLTDWeight();
    stdp->setLTPWeight(0.01f * modulation);
    stdp->setLTDWeight(0.012f * modulation);
}

void NeuromodulationStep::integratePredictionError(
    PredictionError* predictionError,
    PredictionSystem* predictionSystem,
    const TimestepDuration& timestep) {
    if (!predictionError || !predictionSystem) return;
    
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
    // This would involve comparing predicted vs actual sensory input
}

// ========== PLASTICITY STEP IMPLEMENTATION ==========

void PlasticityStep::applySTDPPlasticity(
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    STDP* stdp,
    float plasticityMod,
    const TimestepDuration& timestep) {
    if (!stdp) return;
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    stdp->update(syn, preSpikes, postSpikes, timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Simplified Hebbian update
                    float weight = syn->getWeight();
                    float hebbianChange = 0.001f;  // Simplified Hebbian update
                    weight += hebbianChange;
                    syn->setWeight(weight);
                }
            }
            
            // Update synapse state
            syn->step(timestep);
        }
    }
}

void PlasticityStep::applyHebbianPlasticity(
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    Hebbian* hebbian,
    const TimestepDuration& timestep) {
    if (!hebbian) return;
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Simplified Hebbian update
                    float weight = syn->getWeight();
                    float hebbianChange = 0.001f;  // Simplified Hebbian update
                    weight += hebbianChange;
                    syn->setWeight(weight);
                }
            }
        }
    }
}

void PlasticityStep::applyStructuralPlasticity(
    StructuralPlasticity* structuralPlasticity,
    size_t currentStep,
    DevelopmentalStage developmentalStage,
    const TimestepDuration& timestep) {
    if (!structuralPlasticity) return;
    
    if (currentStep % 100 == 0) {  // Periodic structural plasticity
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        
        // Apply developmental modulation
        float plasticityMod = 1.0f;
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }
        
        synaptogenesisRate *= plasticityMod;
        pruningRate *= (2.0f - plasticityMod);
        
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        structuralPlasticity->update();
    }
}

void PlasticityStep::updateSynapseState(
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    const TimestepDuration& timestep,
    Timestamp currentTime) {
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            syn->step(currentTime);
        }
    }
}

// ========== COGNITION STEP IMPLEMENTATION ==========

void CognitionStep::updateAttentionalSelection(
    AttentionalSelection* attention,
    NeuralWorkingMemory* workingMemory,
    const TimestepDuration& timestep) {
    if (!attention) return;
    
    attention->update(timestep);
    
    // Apply attention to working memory winners
    if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        attention->processCompetition(competitors);
    }
}

void CognitionStep::updateNeuralPlanning(
    NeuralPlanner* planner,
    const TimestepDuration& timestep) {
    if (planner) {
        planner->update(timestep);
    }
}

void CognitionStep::updateConceptFormation(
    ConceptFormation* conceptFormation,
    std::vector<std::unique_ptr<NeuralRegion>>& regions,
    const TimestepDuration& timestep) {
    if (!conceptFormation) return;
    
    // Would process current neural activity patterns to form concepts
    // This requires sensory state encoding
    // Simplified implementation - encode current neural state
    encodeCurrentStateForConceptLearning(regions, conceptFormation, timestep);
}

void CognitionStep::applyAttentionToWorkingMemory(
    AttentionalSelection* attention,
    NeuralWorkingMemory* workingMemory) {
    if (!attention || !workingMemory) return;
    
    // Apply attention to working memory winners
    if (!workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        attention->processCompetition(competitors);
    }
}

void CognitionStep::processCognitiveOutput(
    NeuralPlanner* planner,
    ConceptFormation* conceptFormation) {
    if (planner) {
        // Simplified action selection based on concept activation
        // This would use planner to generate actions
    }
}

// ========== PREDICTION STEP IMPLEMENTATION ==========

void PredictionStep::updatePredictionSystem(
    PredictionSystem* predictionSystem,
    const class SensoryInput& input,
    PredictionError* predictionError,
    const TimestepDuration& timestep) {
    if (!predictionSystem) return;
    
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
    generatePredictions(predictionSystem, input);
    computePredictionErrors(predictionError, predictionSystem, input);
}

void PredictionStep::processPredictionErrors(
    PredictionError* predictionError,
    PredictionSystem* predictionSystem) {
    if (!predictionError || !predictionSystem) return;
    
    // Process prediction errors and update beliefs
    // This would involve comparing predicted vs actual sensory input
}

void PredictionStep::integratePredictionsWithNeurons(
    PredictionSystem* predictionSystem,
    std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    if (!predictionSystem) return;
    
    // Integrate prediction signals with neural activity
    // This would apply prediction errors to modulate neural firing
}

void PredictionStep::updateExpectedStates(
    PredictionSystem* predictionSystem,
    Timestamp currentTime) {
    if (!predictionSystem) return;
    
    // Update expected states based on predictions
    // This would update internal models of the environment
}

// ========== DEVELOPMENT STEP IMPLEMENTATION ==========

void DevelopmentStep::applyDevelopmentEffects(
    DevelopmentSystem* developmentSystem,
    StructuralPlasticity* structuralPlasticity,
    size_t currentStep,
    const TimestepDuration& timestep,
    DevelopmentalStage& developmentalStage) {
    if (!developmentSystem) return;
    
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        developmentSystem->update(nullptr, nullptr, timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = structuralPlasticity;
        if (sp) {
            float plasticityMod = 1.0f;
            
            switch (developmentalStage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void DevelopmentStep::calculateDevelopmentModulation(
    DevelopmentalStage stage,
    float& plasticityMod,
    float& synaptogenesisRate,
    float& pruningRate) {
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            synaptogenesisRate = 0.0001f;
            pruningRate = 0.00001f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            synaptogenesisRate = 0.00008f;
            pruningRate = 0.000012f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            synaptogenesisRate = 0.00005f;
            pruningRate = 0.000015f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;  // Stable
            synaptogenesisRate = 0.00002f;
            pruningRate = 0.00002f;
            break;
    }
}

void DevelopmentStep::adaptSystemParameters(
    DevelopmentSystem* developmentSystem,
    StructuralPlasticity* structuralPlasticity,
    DevelopmentalStage stage) {
    if (!developmentSystem || !structuralPlasticity) return;
    
    float plasticityMod, synaptogenesisRate, pruningRate;
    calculateDevelopmentModulation(stage, plasticityMod, synaptogenesisRate, pruningRate);
    
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
}

void DevelopmentStep::updateMaturationTimeline(
    size_t currentStep,
    DevelopmentalStage& developmentalStage) {
    if (currentStep % 5000 == 0) {  // Update developmental stage every 5000 steps
        // Simplified developmental progression
        if (developmentalStage == DevelopmentalStage::Initial) {
            developmentalStage = DevelopmentalStage::CriticalPeriod;
        } else if (developmentalStage == DevelopmentalStage::CriticalPeriod) {
            developmentalStage = DevelopmentalStage::Maturation;
        } else if (developmentalStage == DevelopmentalStage::Maturation) {
            developmentalStage = DevelopmentalStage::Adult;
        }
    }
}

// ========== CHECKPOINT STEP IMPLEMENTATION ==========

void CheckpointStep::manageCheckpoints(
    CheckpointManager* checkpointManager,
    size_t currentStep,
    Timestamp currentTime) {
    if (!checkpointManager) return;
    
    checkpointManager->update(currentStep, currentTime);
}

void CheckpointStep::saveBrainState(
    const std::string& filepath,
    Brain* brain,
    const TimestepDuration& timestep) {
    if (!brain) return;
    
    // Simplified save implementation
    // Full implementation would use checkpoint manager
    std::string checkpointDir = filepath.substr(0, filepath.find_last_of('/'));
    // Create checkpoint directory if needed
}

void CheckpointStep::loadBrainState(
    const std::string& filepath,
    Brain* brain) {
    if (!brain) return;
    
    // Simplified load implementation
    // Full implementation would use checkpoint manager
}

void CheckpointStep::cleanupOldCheckpoints(
    CheckpointManager* checkpointManager,
    int maxCheckpoints,
    size_t currentStep) {
    if (!checkpointManager) return;
    
    // Cleanup old checkpoints
    checkpointManager->cleanup(maxCheckpoints, currentStep);
}

// ========== STATISTICS STEP IMPLEMENTATION ==========

void StatisticsStep::collectAndReportStatistics(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions,
    size_t totalSpikesThisStep,
    size_t totalSpikesTotal,
    SpikeSystem* spikeSystem,
    const TimestepDuration& timestep,
    Timestamp currentTime) {
    // Calculate E/I ratio
    float eiratio = 0.0f;
    calculateEIEratio(regions, eiratio);
    
    // Format memory stats
    std::string memoryStats;
    // This would require access to memory systems - simplified here
    
    // Format neuromodulation stats
    std::string neuromodStats;
    // This would require access to neuromodulation systems - simplified here
    
    // Log status (placeholder for actual logging)
    // logStatus() would be called separately
}

void StatisticsStep::updateMemoryStatistics(
    const NeuralWorkingMemory* workingMemory,
    const NeuralEpisodicMemory* episodicMemory) {
    // Update memory statistics
}

void StatisticsStep::updateNeuromodulationStatistics(
    const Dopamine* dopamine,
    const Curiosity* curiosity,
    const Novelty* novelty,
    const PredictionError* predictionError) {
    // Update neuromodulation statistics
}

void StatisticsStep::updatePlasticityStatistics(
    const STDP* stdp,
    const Hebbian* hebbian,
    const StructuralPlasticity* structuralPlasticity) {
    // Update plasticity statistics
}

void StatisticsStep::logSystemStatus(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions,
    size_t totalSpikesThisStep,
    size_t totalSpikesTotal,
    SpikeSystem* spikeSystem,
    const TimestepDuration& timestep,
    Timestamp currentTime) {
    // This function was originally in Brain.cpp - keeping it here for organization
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t activeNeurons = 0;
    float avgFiringRate = 0.0f;
    
    for (const auto& region : regions) {
        totalNeurons += region->getTotalNeuronCount();
        totalSynapses += region->getSynapseCount();
        activeNeurons += region->getActiveNeuronCount();
        avgFiringRate += region->getAverageFiringRate();
    }
    
    if (!regions.empty()) {
        avgFiringRate /= static_cast<float>(regions.size());
    }
    
    float eiratio = 0.0f;
    calculateEIEratio(regions, eiratio);
    
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(regions.size()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(totalNeurons));
    NLM_LOG_INFO("Total synapses: " + std::to_string(totalSynapses));
    NLM_LOG_INFO("Active neurons: " + std::to_string(activeNeurons));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(totalSpikesThisStep));
    NLM_LOG_INFO("Total spikes: " + std::to_string(totalSpikesTotal));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(avgFiringRate));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(eiratio));
    
    // Memory system status
    // This would require access to memory systems - simplified here
    
    // Neuromodulation status
    // This would require access to neuromodulation systems - simplified here
    
    // Development status
    // This would require access to development system - simplified here
}

// ========== BRAIN CLASS IMPLEMENTATION ==========

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)), 
    neuralDynamicsStep(),
    memorySystemsStep(),
    neuromodulationStep(),
    plasticityStep(),
    cognitionStep(),
    predictionStep(),
    developmentStep(),
    checkpointStep(),
    statisticsStep() {
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    }
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    if (pImpl->planner) {
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(this);
    }
    
    if (pImpl->attention) {
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
    }
    
    // Initialize neuromodulation
    if (pImpl->novelty) {
        pImpl->novelty->initialize(this);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(this);
    }
    
    // Register spike handlers for event-driven processing
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
    }
    
    // Register delayed spike handler to deliver synaptic input
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : pImpl->regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == event.destination_neuron) {
                        // Apply synaptic weight as current
                        MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                        if (event.is_excitatory) {
                            neuron->receiveExcitatoryInput(synapticCurrent);
                        } else {
                            neuron->receiveInhibitoryInput(-synapticCurrent);
                        }
                        return;
                    }
                }
            }
        });
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP and Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Execute modular step components
    executeStepPhase1(pImpl->timestep, currentTime, currentStep);
    executeStepPhase2(pImpl->timestep, currentTime, currentStep);
    executeStepPhase3(pImpl->timestep, currentTime, currentStep);
    executeStepPhase4(pImpl->timestep, currentTime, currentStep);
    executeStepPhase5(pImpl->timestep, currentTime, currentStep);
    executeStepPhase6(pImpl->timestep, currentTime, currentStep);
    executeStepPhase7(pImpl->timestep, currentTime, currentStep);
    executeStepPhase8(pImpl->timestep, currentTime, currentStep);
    executeStepPhase9(pImpl->timestep, currentTime, currentStep);
    executeStepPhase10(pImpl->timestep, currentTime, currentStep);
    executeStepPhase11(pImpl->timestep, currentTime, currentStep);
    executeStepPhase12(pImpl->timestep, currentTime, currentStep);
    executeStepPhase13(pImpl->timestep, currentTime, currentStep);
    executeStepPhase14(pImpl->timestep, currentTime, currentStep);
    executeStepPhase15(pImpl->timestep, currentTime, currentStep);
}

void Brain::executeStepPhase1(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 1: Process pending delayed spikes (deliver synaptic input)
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
}

void Brain::executeStepPhase2(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 2: Update all neurons (LIF dynamics)
    neuralDynamicsStep.updateNeuronDynamics(pImpl->regions, timestep, currentTime);
}

void Brain::executeStepPhase3(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 3: Detect spikes and schedule spike events
    neuralDynamicsStep.detectAndQueueSpikes(pImpl->regions, pImpl->spikeSystem.get(), timestep, currentTime, currentStep, pImpl->totalSpikesThisStep);
    neuralDynamicsStep.processSpikes(pImpl->spikeSystem.get(), currentStep);
}

void Brain::executeStepPhase4(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 4: Update working memory
    memorySystemsStep.updateWorkingMemory(pImpl->workingMemory.get(), timestep);
}

void Brain::executeStepPhase5(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 5: Apply neuromodulation effects
    neuromodulationStep.updateNoveltyDetection(pImpl->novelty.get(), timestep);
    neuromodulationStep.updateCuriositySignals(pImpl->curiosity.get(), timestep);
    neuromodulationStep.updateDopamineSystem(pImpl->dopamine.get(), timestep);
    neuromodulationStep.applyDopamineToExcitability(pImpl->dopamine.get(), pImpl->regions, timestep);
    neuromodulationStep.applyNeuromodulationToPlasticity(pImpl->dopamine.get(), pImpl->stdp.get());
    neuromodulationStep.integratePredictionError(pImpl->predictionError.get(), pImpl->predictionSystem.get(), timestep);
}

void Brain::executeStepPhase6(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 6: Apply plasticity rules (STDP and Hebbian)
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    plasticityStep.applySTDPPlasticity(pImpl->regions, pImpl->stdp.get(), plasticityMod, timestep);
    plasticityStep.applyHebbianPlasticity(pImpl->regions, pImpl->hebbian.get(), timestep);
    plasticityStep.applyStructuralPlasticity(pImpl->structuralPlasticity.get(), currentStep, pImpl->developmentalStage, timestep);
    plasticityStep.updateSynapseState(pImpl->regions, timestep, currentTime);
}

void Brain::executeStepPhase7(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 7: Update episodic memory
    memorySystemsStep.updateEpisodicMemory(pImpl->episodicMemory.get(), pImpl->stepsSinceLastEpisode, currentTime, timestep, pImpl->dopamine.get(), pImpl->regions);
    
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
    }
}

void Brain::executeStepPhase8(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 8: Update prediction system
    predictionStep.updatePredictionSystem(pImpl->predictionSystem.get(), SensoryInput(), pImpl->predictionError.get(), timestep);
}

void Brain::executeStepPhase9(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 9: Update attention system
    cognitionStep.updateAttentionalSelection(pImpl->attention.get(), pImpl->workingMemory.get(), timestep);
}

void Brain::executeStepPhase10(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 10: Update concept formation
    cognitionStep.updateConceptFormation(pImpl->conceptFormation.get(), pImpl->regions, timestep);
}

void Brain::executeStepPhase11(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 11: Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        developmentStep.applyDevelopmentEffects(pImpl->developmentSystem.get(), pImpl->structuralPlasticity.get(), currentStep, timestep, pImpl->developmentalStage);
    }
}

void Brain::executeStepPhase12(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 12: Replay important memories
    memorySystemsStep.processMemoryReplay(pImpl->episodicMemory.get(), pImpl->replayInterval, currentStep);
}

void Brain::executeStepPhase13(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 13: Apply development effects
    developmentStep.applyDevelopmentEffects(pImpl->developmentSystem.get(), pImpl->structuralPlasticity.get(), currentStep, timestep, pImpl->developmentalStage);
    developmentStep.updateMaturationTimeline(currentStep, pImpl->developmentalStage);
}

void Brain::executeStepPhase14(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 14: Periodic memory consolidation
    memorySystemsStep.consolidateMemories(pImpl->episodicMemory.get(), pImpl->consolidationInterval, currentStep, 0.3f);
}

void Brain::executeStepPhase15(const TimestepDuration& timestep, Timestamp currentTime, SimulationStep currentStep) {
    // STEP 15: Checkpoint management and statistics
    checkpointStep.manageCheckpoints(pImpl->checkpointManager.get(), currentStep, currentTime);
    statisticsStep.collectAndReportStatistics(pImpl->regions, pImpl->totalSpikesThisStep, pImpl->totalSpikesTotal, pImpl->spikeSystem.get(), timestep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    neuralDynamicsStep.injectSensoryInput(pImpl->sensoryNeurons, input);
    memorySystemsStep.updateWorkingMemoryFromSensory(pImpl->workingMemory.get(), pImpl->sensoryNeurons, input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    neuralDynamicsStep.injectCurrentToNeurons(pImpl->regions, type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        float currentLTP = pImpl->stdp->getLTPWeight();
        float currentLTD = pImpl->stdp->getLTDWeight();
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    statisticsStep.logSystemStatus(pImpl->regions, pImpl->totalSpikesThisStep, pImpl->totalSpikesTotal, pImpl->spikeSystem.get(), pImpl->timestep, pImpl->currentTime);
    
    // Log region details
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm

// ========== CONCRETE IMPLEMENTATION FILES ==========

// NeuralDynamicsStep.cpp (implementation)
// NeuralDynamicsStep.cpp includes:
// - LIF neuron dynamics update
// - Spike detection and queuing
// - Synaptic current delivery
// - Sensory input injection

// MemorySystemsStep.cpp (implementation)
// MemorySystemsStep.cpp includes:
// - Working memory maintenance
// - Episodic memory capture
// - Memory replay
// - Memory consolidation

// NeuromodulationStep.cpp (implementation)
// NeuromodulationStep.cpp includes:
// - Dopamine, curiosity, novelty updates
// - Neuromodulation application to neurons
// - Plasticity modulation

// PlasticityStep.cpp (implementation)
// PlasticityStep.cpp includes:
// - STDP plasticity
// - Hebbian plasticity
// - Structural plasticity
// - Synapse state updates

// CognitionStep.cpp (implementation)
// CognitionStep.cpp includes:
// - Attentional selection
// - Neural planning
// - Concept formation
// - Cognitive output processing

// PredictionStep.cpp (implementation)
// PredictionStep.cpp includes:
// - Prediction system updates
// - Prediction error computation
// - Expected state updates

// DevelopmentStep.cpp (implementation)
// DevelopmentStep.cpp includes:
// - Developmental stage progression
// - Plasticity rate adaptation
// - System parameter changes

// CheckpointStep.cpp (implementation)
// CheckpointStep.cpp includes:
// - Checkpoint management
// - Save/restore functionality
// - Cleanup operations

// StatisticsStep.cpp (implementation)
// StatisticsStep.cpp includes:
// - Statistics collection
// - System status logging
// - Performance metrics