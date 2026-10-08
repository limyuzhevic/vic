// Main implementation of refactored Brain class
// This file contains the implementation of the Brain class using the new component architecture

#include "BrainRefactored.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Brain class implementation using component architecture

// Constructor
Brain::Brain(std::shared_ptr<Config> cfg) 
    : config(std::move(cfg)),
      timestep(DEFAULT_TIMESTEP),
      currentStep(0),
      currentTime(0.0),
      stepsSinceLastEpisode(0),
      replayInterval(DEFAULT_REPLAY_INTERVAL),
      consolidationInterval(DEFAULT_CONSOLIDATION_INTERVAL),
      isResting(false),
      totalSpikesThisStep(0),
      totalSpikesTotal(0),
      nextRegionId(1),
      developmentalStage(DevelopmentalStage::Initial) {
    
    // Initialize components
    computation = std::make_unique<BrainComputation>(config, regions);
    memory = std::make_unique<BrainMemory>(config);
    prediction = std::make_unique<BrainPrediction>();
    cognition = std::make_unique<BrainCognition>(config);
    neuromodulation = std::make_unique<BrainNeuromodulation>();
    integration = std::make_unique<BrainIntegration>(config);
    
    // Setup components after they're created
    setupComponents();
}

// Move constructor
Brain::Brain(Brain&& other) noexcept 
    : config(std::move(other.config)),
      computation(std::move(other.computation)),
      memory(std::move(other.memory)),
      prediction(std::move(other.prediction)),
      cognition(std::move(other.cognition)),
      neuromodulation(std::move(other.neuromodulation)),
      integration(std::move(other.integration)),
      regions(std::move(other.regions)),
      interRegionConnections(std::move(other.interRegionConnections)),
      timestep(other.timestep),
      currentStep(other.currentStep),
      currentTime(other.currentTime),
      stepsSinceLastEpisode(other.stepsSinceLastEpisode),
      replayInterval(other.replayInterval),
      consolidationInterval(other.consolidationInterval),
      isResting(other.isResting),
      totalSpikesThisStep(other.totalSpikesThisStep),
      totalSpikesTotal(other.totalSpikesTotal),
      sensoryNeurons(std::move(other.sensoryNeurons)),
      motorNeurons(std::move(other.motorNeurons)),
      checkpointManager(std::move(other.checkpointManager)),
      developmentalStage(other.developmentalStage),
      nextRegionId(other.nextRegionId) {
}

// Move assignment operator
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        // Clean up existing resources
        reset();
        
        // Move resources
        config = std::move(other.config);
        computation = std::move(other.computation);
        memory = std::move(other.memory);
        prediction = std::move(other.prediction);
        cognition = std::move(other.cognition);
        neuromodulation = std::move(other.neuromodulation);
        integration = std::move(other.integration);
        regions = std::move(other.regions);
        interRegionConnections = std::move(other.interRegionConnections);
        timestep = other.timestep;
        currentStep = other.currentStep;
        currentTime = other.currentTime;
        stepsSinceLastEpisode = other.stepsSinceLastEpisode;
        replayInterval = other.replayInterval;
        consolidationInterval = other.consolidationInterval;
        isResting = other.isResting;
        totalSpikesThisStep = other.totalSpikesThisStep;
        totalSpikesTotal = other.totalSpikesTotal;
        sensoryNeurons = std::move(other.sensoryNeurons);
        motorNeurons = std::move(other.motorNeurons);
        checkpointManager = std::move(other.checkpointManager);
        developmentalStage = other.developmentalStage;
        nextRegionId = other.nextRegionId;
    }
    return *this;
}

// Initialize brain with configuration
bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
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
                    sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    motorNeurons.push_back(neuron);
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
            region->initializeRandomConnectivity(*getRandomGenerator(), 
                                                  connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========
    
    // Initialize memory systems
    if (memory) {
        memory->initializeSystems(this);
        memory->getWorkingMemory()->setCapacity(neuronCount / 10);
        memory->getEpisodicMemory()->setMaxEpisodes(1000);
    }
    
    // Initialize cognition systems
    if (cognition) {
        cognition->initialize(this);
        cognition->setPlanningDepth(5);
        cognition->setInhibitionStrength(DEFAULT_ATTENTION_INHIBITION_STRENGTH);
        cognition->setExcitationStrength(DEFAULT_ATTENTION_EXCITATION_STRENGTH);
    }
    
    // Initialize neuromodulation
    if (neuromodulation) {
        neuromodulation->initialize(this);
    }
    
    // Setup spike system handlers
    if (computation && getSpikeSystem()) {
        computation->registerCountAndDeliverHandlers(*getSpikeSystem(),
            [this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++totalSpikesThisStep;
                ++totalSpikesTotal;
            },
            [this](const DelayedSpikeEvent& event) {
                // Deliver synaptic input (handled by integration)
            });
    }
    
    // Configure checkpoint manager
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (integration) {
        integration->configureCheckpointManager(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(motorNeurons.size()));
    
    return true;
}

// Setup components after they're created
void Brain::setupComponents() {
    // Setup component integration after all components are created
    if (computation && getSpikeSystem()) {
        // Register spike handlers
        computation->registerCountAndDeliverHandlers(*getSpikeSystem(),
            [this](const DetailedSpikeEvent& event) {
                ++totalSpikesThisStep;
                ++totalSpikesTotal;
            },
            [this](const DelayedSpikeEvent& event) {
                // Synaptic input delivery handled by integration layer
            });
    }
}

// Main simulation step
void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    this->currentStep = currentStep;
    this->currentTime = currentTime;
    totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    if (computation) {
        computation->step(currentStep, currentTime, regions);
    }
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
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
                    if (computation) {
                        computation->queueSpike(event);
                    }

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

                        if (computation) {
                            computation->queueDelayedSpike(delayedEvent);
                        }

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (memory && memory->getWorkingMemory()) {
                        memory->getWorkingMemory()->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes through computation component
    if (computation) {
        computation->processSpikes(regions);
    }
    
    // ========== STEP 4: Update working memory ==========
    if (memory) {
        memory->step(timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    if (neuromodulation) {
        neuromodulation->step(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = neuromodulation->getDopamineLevel();
        if (dopamineLevel > 0.0f) {
            float excitabilityMod = dopamineLevel * DEFAULT_DOPAMINE_EXCITABILITY_MOD;
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (neuromodulation) {
        plasticityMod = neuromodulation->getPlasticityFactor();
    }
    
    if (computation) {
        computation->updatePlasticity(regions, plasticityMod, currentTime);
    }
    
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = computation ? computation->getPreSpikeHistory(syn->getSourceNeuron()) : std::vector<float>();
                const auto& postSpikes = computation ? computation->getPostSpikeHistory(syn->getDestinationNeuron()) : std::vector<float>();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Apply STDP logic would go here
                    // For now, apply neuromodulation scaling
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = computation ? computation->getPreSpikeHistory(syn->getSourceNeuron()) : std::vector<float>();
                const auto& postSpikes = computation ? computation->getPostSpikeHistory(syn->getDestinationNeuron()) : std::vector<float>();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Apply Hebbian logic would go here
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        stepsSinceLastEpisode = 0;
        
        if (memory && memory->getEpisodicMemory()) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = neuromodulation ? neuromodulation->getDopamineLevel() : 0.0f;
            
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
            
            memory->getEpisodicMemory()->storeEpisode(episode);
        }
    }
    
    // ========== STEP 8: Update prediction system ==========
    if (prediction) {
        prediction->update();
    }
    
    // ========== STEP 9: Update attention system ==========
    if (cognition) {
        cognition->step(timestep);
        
        // Apply attention to working memory winners
        if (memory && memory->getWorkingMemory() && !memory->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = memory->getWorkingMemory()->getMemoryNeurons();
            cognition->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (cognition && cognition->getConceptFormation()) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % DEFAULT_STRUCTURAL_PLASTICITY_UPDATE_INTERVAL == 0) {
        // Apply structural plasticity
        if (computation && getRandomGenerator()) {
            // Would call structural plasticity system
        }
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (memory && currentStep % replayInterval == 0 && memory->getEpisodicMemory()) {
        // Get episodes for replay
        auto episodesToReplay = memory->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            memory->getEpisodicMemory()->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % DEFAULT_DEVELOPMENT_UPDATE_INTERVAL == 0) {  // Update development every 1000 steps
        // Update development system
        if (integration) {
            integration->updateDevelopment(developmentalStage, 
                                          DEFAULT_SYNAPTOTGESIS_RATE, 
                                          DEFAULT_PRUNING_RATE);
        }
        
        // Apply development effects on plasticity rates
        if (computation) {
            DevelopmentalStage stage = developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
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
            
            // Update plasticity rates based on development stage
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (memory && currentStep % consolidationInterval == 0 && memory->getEpisodicMemory()) {
        // Consolidate important memories, remove weak ones
        memory->getEpisodicMemory()->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (integration) {
        integration->updateCheckpoint(currentStep, currentTime);
    }
}

// Other Brain methods implementation (simplified for brevity)
// These would be implemented similarly with improved error handling and null safety

void Brain::receiveSensoryInput(const SensoryInput& input) {
    // Simplified implementation with null checks
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = sensoryNeurons.size();
    if (numSensory == 0) return;
    
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        if (sensoryNeurons[i]) {
            sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            if (memory && memory->getWorkingMemory() && normalizedValue > 0.5f) {
                memory->getWorkingMemory()->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n && n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
    }
}

// Component accessor implementations
SpikeSystem* Brain::getSpikeSystem() {
    return computation ? computation->getSpikeSystem() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return computation ? computation->getSpikeSystem() : nullptr;
}

// Other accessor implementations...

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    currentStep = 0;
    currentTime = 0.0;
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    
    for (auto& region : regions) {
        if (region) {
            region->reset();
        }
    }
    
    if (computation) {
        computation->reset(regions);
    }
    
    if (memory) {
        memory->clearAll();
    }
    
    developmentalStage = DevelopmentalStage::Initial;
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

// Other Brain method implementations...

} // namespace nlm
