#pragma once

#include <cfloat>
#include <algorithm>

namespace nlm {

// Constants for Brain operations
namespace BrainConstants {
    // Development timing
    constexpr double INITIAL_DEVELOPMENT_INTERVAL = 1000.0;  // Update development every 1000 steps
    constexpr double REPLAY_INTERVAL = 100;                  // Replay every 100 steps
    constexpr double CONSOLIDATION_INTERVAL = 1000;          // Consolidate every 1000 steps
    
    // Plasticity modulation
    constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
    constexpr float CRITICAL_PERIOD_PLASTICITY_MODIFIER = 0.8f;
    constexpr float MATURATION_PLASTICITY_MODIFIER = 0.5f;
    constexpr float ADULT_PLASTICITY_MODIFIER = 0.2f;
    constexpr float PLASTICITY_MODULATION_FACTOR = 1.0f;
    
    // Dopamine effects
    constexpr float DOPAMINE_EXCITABILITY_FACTOR = 0.5f;
    
    // Memory system thresholds
    constexpr size_t EPISODE_STORAGE_INTERVAL = 10;          // Store episode every 10 steps
    constexpr float CONSOLIDATION_THRESHOLD = 0.3f;
    constexpr float MEMORY_NEIGHBOR_THRESHOLD = 5.0f;
    
    // Attention system
    constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;
    constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;
    
    // Other constants
    constexpr float MOVEMENT_COST = 0.01f;
    constexpr float WALL_COLLISION_COST = 0.05f;
    constexpr float TURN_COST = 0.005f;
}

// Forward declarations for forward pass processing
class BrainStepProcessor;
class MemorySystemManager;
class NeuromodulationSystem;
class DevelopmentSystem;
class PredictionSystem;
class CognitionSystem;
class PlasticitySystem;
class CheckpointSystem;

// Brain step processor - handles the core neural dynamics and spike propagation
class BrainStepProcessor {
public:
    static void ProcessDelayedSpikes(SpikeSystem* spikeSystem, SimulationStep currentStep, Timestamp currentTime) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    static void UpdateNeuralDynamics(
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        SpikeSystem* spikeSystem,
        double timestep,
        SimulationStep currentStep,
        Timestamp currentTime) {
        
        // Update all neurons (LIF dynamics)
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
        
        // Detect spikes and schedule spike events
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                    if (justFired) {
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        spikeSystem->queueSpike(event);

                        // Record post-synaptic spike for incoming synapses (plasticity)
                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            syn->recordPostSpike(currentTime);
                        }
                    }
                }
            }
        }
        
        // Process immediate spikes
        spikeSystem->processSpikes(currentStep);
    }
};

// Memory system manager - coordinates all memory operations
class MemorySystemManager {
public:
    static void UpdateEpisodicMemory(
        std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        double timestep,
        SimulationStep currentStep,
        size_t& stepsSinceLastEpisode,
        size_t episodeStorageInterval,
        Dopamine* dopamine = nullptr) {
        
        stepsSinceLastEpisode++;
        if (stepsSinceLastEpisode >= episodeStorageInterval) {
            stepsSinceLastEpisode = 0;
            
            if (episodicMemory) {
                // Capture current brain state as an episode
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                
                // Store active neurons
                for (auto& region : regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > BrainConstants::MEMORY_NEIGHBOR_THRESHOLD) {
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
    }
    
    static void UpdateWorkingMemory(
        std::unique_ptr<NeuralWorkingMemory>& workingMemory,
        double timestep) {
        if (workingMemory) {
            workingMemory->update(timestep);
        }
    }
    
    static void UpdateReplaySystem(
        std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
        SimulationStep currentStep,
        size_t replayInterval) {
        if (currentStep % replayInterval == 0 && episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    static void UpdateConsolidationSystem(
        std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
        SimulationStep currentStep,
        size_t consolidationInterval) {
        if (currentStep % consolidationInterval == 0 && episodicMemory) {
            // Consolidate important memories, remove weak ones
            episodicMemory->consolidate(BrainConstants::CONSOLIDATION_THRESHOLD);
        }
    }
};

// Neuromodulation system - handles all neuromodulator effects
class NeuromodulationSystem {
public:
    static void UpdateNeuromodulation(
        Dopamine* dopamine,
        Curiosity* curiosity,
        Novelty* novelty,
        PredictionError* predictionError,
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        double timestep) {
        
        // Update novelty detection
        if (novelty) {
            novelty->update(timestep);
        }
        
        // Update curiosity
        if (curiosity) {
            curiosity->update(timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (dopamine) {
            dopamine->update(timestep);
            
            // Apply dopamine effects on neural excitability
            // Dopamine modulates neural excitability by adjusting effective current injection
            // Higher dopamine increases excitability (lower effective threshold)
            float dopamineLevel = dopamine->getLevel();
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        // Positive dopamine adds excitatory bias
                        float excitabilityMod = dopamineLevel * BrainConstants::DOPAMINE_EXCITABILITY_FACTOR;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    static void UpdatePlasticityModulation(
        Dopamine* dopamine,
        float& plasticityMod,
        STDP* stdp) {
        
        // Calculate neuromodulation factor for plasticity
        if (dopamine) {
            plasticityMod = dopamine->getPlasticityFactor();
        }
        
        if (stdp) {
            float weightMod = 1.0f + (plasticityMod - 1.0f) * 0.01f;
            stdp->setLTPWeight(0.01f * weightMod);
            stdp->setLTDWeight(0.012f * weightMod);
        }
    }
};

// Development system - handles developmental processes
class DevelopmentSystem {
public:
    static void UpdateDevelopment(
        DevelopmentSystem* devSystem,
        std::shared_ptr<Brain> brain,
        RandomGenerator& rng,
        double timestep,
        SimulationStep currentStep,
        size_t developmentInterval,
        float& plasticityModifier,
        DevelopmentalStage& developmentalStage,
        StructuralPlasticity* structuralPlasticity = nullptr) {
        
        if (currentStep % developmentInterval == 0) {
            if (devSystem) {
                devSystem->update(brain.get(), rng, timestep * 1000);
            }
            
            // Development affects plasticity rates
            if (structuralPlasticity) {
                float plasticityMod = 1.0f;
                
                switch (developmentalStage) {
                    case DevelopmentalStage::Initial:
                        plasticityMod = BrainConstants::INITIAL_PLASTICITY_MODIFIER;
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        plasticityMod = BrainConstants::CRITICAL_PERIOD_PLASTICITY_MODIFIER;
                        break;
                    case DevelopmentalStage::Maturation:
                        plasticityMod = BrainConstants::MATURATION_PLASTICITY_MODIFIER;
                        break;
                    case DevelopmentalStage::Adult:
                        plasticityMod = BrainConstants::ADULT_PLASTICITY_MODIFIER;
                        break;
                }
                
                structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
                structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }
};

// Cognition system - handles cognitive processes
class CognitionSystem {
public:
    static void UpdateCognition(
        NeuralPlanner* planner,
        ConceptFormation* conceptFormation,
        AttentionalSelection* attention,
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        std::unique_ptr<NeuralWorkingMemory>& workingMemory,
        double timestep) {
        
        // Update neural planner
        if (planner) {
            planner->initialize(nullptr);  // Note: This would need proper brain reference
            planner->setPlanningDepth(5);
        }
        
        // Update concept formation
        if (conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
        
        // Update attention system
        if (attention) {
            attention->update(timestep);
            attention->setInhibitionStrength(BrainConstants::ATTENTION_INHIBITION_STRENGTH);
            attention->setExcitationStrength(BrainConstants::ATTENTION_EXCITATION_STRENGTH);
            
            // Apply attention to working memory winners
            if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                attention->processCompetition(competitors);
            }
        }
    }
};

// Plasticity system - handles synaptic plasticity
class PlasticitySystem {
public:
    static void UpdatePlasticity(
        std::unique_ptr<STDP>& stdp,
        std::unique_ptr<Hebbian>& hebbian,
        const std::vector<std::unique_ptr<NeuralRegion>>& regions,
        double timestep,
        float plasticityMod,
        Dopamine* dopamine = nullptr) {
        
        for (auto& region : regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        if (stdp) {
                            stdp->update(syn, preSpikes, postSpikes, timestep);
                            float weight = syn->getWeight();
                            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                            syn->setWeight(weight);
                        }
                    }
                }
                
                // Apply Hebbian learning
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        if (hebbian) {
                            hebbian->update(syn, preSpikes, postSpikes, timestep);
                        }
                    }
                }
                
                // Update synapse state
                syn->step(static_cast<Timestamp>(0.0));  // Note: This needs proper time parameter
            }
        }
    }
    
    static void UpdateStructuralPlasticity(
        StructuralPlasticity* structuralPlasticity,
        std::shared_ptr<Brain> brain,
        RandomGenerator& rng,
        SimulationStep currentStep,
        size_t structuralPlasticityInterval) {
        
        if (currentStep % structuralPlasticityInterval == 0 && structuralPlasticity) {
            structuralPlasticity->update(brain.get(), rng);
        }
    }
};

// Checkpoint system - handles persistence
class CheckpointSystem {
public:
    static void UpdateCheckpoints(
        CheckpointManager* checkpointManager,
        SimulationStep currentStep,
        Timestamp currentTime) {
        if (checkpointManager) {
            checkpointManager->update(currentStep, currentTime);
        }
    }
};

// Brain implementation - simplified with extracted helper classes
Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

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
    
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(BrainConstants::ATTENTION_INHIBITION_STRENGTH);
    pImpl->attention->setExcitationStrength(BrainConstants::ATTENTION_EXCITATION_STRENGTH);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
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
    // ===== EXTRACTED BRAIN STEPS =====
    
    // Step 1: Process pending delayed spikes (deliver synaptic input)
    BrainStepProcessor::ProcessDelayedSpikes(pImpl->spikeSystem, currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    BrainStepProcessor::UpdateNeuralDynamics(
        pImpl->regions, pImpl->spikeSystem, pImpl->timestep, currentStep, currentTime);
    
    // Step 3: Update working memory
    MemorySystemManager::UpdateWorkingMemory(pImpl->workingMemory, pImpl->timestep);
    
    // Step 4: Apply neuromodulation effects
    NeuromodulationSystem::UpdateNeuromodulation(
        pImpl->dopamine.get(), pImpl->curiosity.get(), pImpl->novelty.get(),
        pImpl->predictionError.get(), pImpl->regions, pImpl->timestep);
    
    // Step 5: Update plasticity rules (STDP and Hebbian)
    PlasticitySystem::UpdatePlasticity(
        pImpl->stdp, pImpl->hebbian, pImpl->regions, pImpl->timestep,
        BrainConstants::PLASTICITY_MODULATION_FACTOR, pImpl->dopamine.get());
    
    // Step 6: Update episodic memory
    MemorySystemManager::UpdateEpisodicMemory(
        pImpl->episodicMemory, pImpl->regions, pImpl->timestep,
        pImpl->stepsSinceLastEpisode, BrainConstants::EPISODE_STORAGE_INTERVAL,
        pImpl->dopamine.get());
    
    // Step 7: Update cognition systems
    CognitionSystem::UpdateCognition(
        pImpl->planner.get(), pImpl->conceptFormation.get(), pImpl->attention.get(),
        pImpl->regions, pImpl->workingMemory, pImpl->timestep);
    
    // Step 8: Apply structural plasticity periodically
    PlasticitySystem::UpdateStructuralPlasticity(
        pImpl->structuralPlasticity.get(), this, *pImpl->rng, currentStep, 100);
    
    // Step 9: Replay important memories
    MemorySystemManager::UpdateReplaySystem(
        pImpl->episodicMemory, currentStep, pImpl->replayInterval);
    
    // Step 10: Apply development effects
    DevelopmentSystem::UpdateDevelopment(
        pImpl->developmentSystem.get(), this, *pImpl->rng, pImpl->timestep,
        currentStep, BrainConstants::INITIAL_DEVELOPMENT_INTERVAL,
        pImpl->developmentalStage, pImpl->structuralPlasticity.get());
    
    // Step 11: Periodic memory consolidation
    MemorySystemManager::UpdateConsolidationSystem(
        pImpl->episodicMemory, currentStep, pImpl->consolidationInterval);
    
    // Step 12: Checkpoint management
    CheckpointSystem::UpdateCheckpoints(
        pImpl->checkpointManager.get(), currentStep, currentTime);
}

// The rest of the Brain implementation continues... (omitted for brevity)
// The remaining methods (receiveSensoryInput, injectCurrent, etc.) would be included

} // namespace nlm