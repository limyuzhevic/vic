#include "Brain.hpp"
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
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Neuromodulator statistics collection
struct NeuromodulatorStatistics {
    float totalDopamine = 0.0f;
    float totalACh = 0.0f;
    float totalNE = 0.0f;
    float total5HT = 0.0f;
    float averagePlasticity = 0.0f;
    size_t updateCount = 0;
};

// Global neuromodulation statistics
static NeuromodulatorStatistics gStats;

// Helper function to update statistics
static void UpdateNeuromodulatorStats(const Dopamine& da, const Acetylcholine& ach, 
                                       const Norepinephrine& ne, const Serotonin& ht) {
    gStats.totalDopamine += da.getLevel();
    gStats.totalACh += ach.getLevel();
    gStats.totalNE += ne.getLevel();
    gStats.total5HT += ht.getLevel();
    
    float avgPlasticity = (da.getPlasticityFactor() + ach.getPlasticityFactor() + 
                          ne.getPlasticityFactor() + ht.getPlasticityFactor()) / 4.0f;
    gStats.averagePlasticity += avgPlasticity;
    gStats.updateCount++;
}

} // namespace nlm

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

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
    
    // Validate configuration pointer
    if (!pImpl || !pImpl->config) {
        NLM_LOG_ERROR("Brain initialization failed: invalid configuration");
        return false;
    }
    
    // Get configuration values with validation
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Validate configuration parameters
    if (neuronCount == 0) {
        NLM_LOG_ERROR("Invalid configuration: neuron_count must be greater than 0");
        return false;
    }
    if (regionCount == 0) {
        NLM_LOG_ERROR("Invalid configuration: region_count must be greater than 0");
        return false;
    }
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid configuration: connection_probability must be between 0.0 and 1.0");
        return false;
    }
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        if (!addRegion("Region_" + std::to_string(i + 1))) {
            NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
            return false;
        }
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1) + " during neuron creation");
            return false;
        }
        
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
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (!region) {
            NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1) + " during connectivity initialization");
            return false;
        }
        // Initialize random connectivity and synapse weights
        region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    if (!pImpl->workingMemory) {
        NLM_LOG_ERROR("Failed to initialize working memory - memory system not available");
        return false;
    }
    if (!pImpl->workingMemory->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize working memory");
        return false;
    }
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    if (!pImpl->episodicMemory) {
        NLM_LOG_ERROR("Failed to initialize episodic memory - memory system not available");
        return false;
    }
    if (!pImpl->episodicMemory->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize episodic memory");
        return false;
    }
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    if (!pImpl->associativeMemory) {
        NLM_LOG_ERROR("Failed to initialize associative memory - memory system not available");
        return false;
    }
    if (!pImpl->associativeMemory->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize associative memory");
        return false;
    }
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    if (!pImpl->planner) {
        NLM_LOG_ERROR("Failed to initialize neural planner - planner not available");
        return false;
    }
    if (!pImpl->planner->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize neural planner");
        return false;
    }
    pImpl->planner->setPlanningDepth(5);
    
    if (!pImpl->conceptFormation) {
        NLM_LOG_ERROR("Failed to initialize concept formation - concept system not available");
        return false;
    }
    if (!pImpl->conceptFormation->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize concept formation");
        return false;
    }
    
    if (!pImpl->attention) {
        NLM_LOG_ERROR("Failed to initialize attentional selection - attention system not available");
        return false;
    }
    if (!pImpl->attention->initialize(this)) {
        NLM_LOG_ERROR("Failed to initialize attentional selection");
        return false;
    }
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    acetylcholine = std::make_unique<Acetylcholine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
    norepinephrine = std::make_unique<Norepinephrine>();
    serotonin = std::make_unique<Serotonin>();
    
    // Configure neuromodulators with brain reference
    dopamine->initialize(this);
    acetylcholine->initialize(this);
    curiosity->initialize(this);
    predictionError->initialize(this);
    novelty->initialize(this);
    norepinephrine->initialize(this);
    serotonin->initialize(this);
    
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
    if (!pImpl->checkpointManager) {
        NLM_LOG_ERROR("Failed to initialize checkpoint manager - performance system not available");
        return false;
    }
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

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
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    acetylcholine = std::make_unique<Acetylcholine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
    norepinephrine = std::make_unique<Norepinephrine>();
    serotonin = std::make_unique<Serotonin>();
    
    // Configure neuromodulators with brain reference
    dopamine->initialize(this);
    acetylcholine->initialize(this);
    curiosity->initialize(this);
    predictionError->initialize(this);
    novelty->initialize(this);
    norepinephrine->initialize(this);
    serotonin->initialize(this);
    
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
     * 6. Apply plasticity rules (STDP, Hebbian)
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
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

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
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

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

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons from brain regions
            for (auto& region : pImpl->regions) {
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
            
            // STORE WORKING MEMORY IN EPISODIC MEMORY
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                // Add working memory neurons to episode
                for (const NeuronId& wmNeuron : pImpl->workingMemory->getMemoryNeurons()) {
                    if (std::find(episode.activeNeurons.begin(), episode.activeNeurons.end(), wmNeuron) == episode.activeNeurons.end()) {
                        episode.activeNeurons.push_back(wmNeuron);
                        episode.neuronActivations.push_back(pImpl->workingMemory->getNeuronActivation(wmNeuron));
                    }
                }
                
                // Add working memory content as sensory state for storage
                std::vector<float> wmContent = pImpl->workingMemory->retrieve();
                if (!wmContent.empty()) {
                    episode.sensoryState = wmContent;
                }
            }
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // ========== STEP 7: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // Store the most recent sensory observation for prediction
        if (pImpl->workingMemory && !pImpl->workingMemory->retrieve().empty()) {
            std::vector<float> currentObservation = pImpl->workingMemory->retrieve();
            
            // Convert observation to SensoryInput (using InternalSignals)
            auto sensoryInput = std::make_unique<InternalSignals>();
            sensoryInput->clearSignals();
            for (float value : currentObservation) {
                sensoryInput->addSignal(value);
            }
            
            // Make prediction based on current state
            std::unique_ptr<SensoryInput> prediction = pImpl->predictionSystem->predictNextState(*sensoryInput);
            
            // Train the system on current observation
            pImpl->predictionSystem->train(*sensoryInput);
            
            // If we have both prediction and actual, compare them
            if (prediction) {
                // Use current observation as "actual" for comparison
                pImpl->predictionSystem->updatePredictions(*prediction, *sensoryInput);
                
                // Compute prediction error for neuromodulation
                float predError = pImpl->predictionSystem->getPredictionError();
                
                // Store prediction error in prediction error system
                if (pImpl->predictionError) {
                    pImpl->predictionError->computeError(prediction->getData().empty() ? 0.0f : prediction->getData()[0],
                        sensoryInput->getData().empty() ? 0.0f : sensoryInput->getData()[0]);
                }
            }
        }
    }
    
    // Apply neuromodulation effects from prediction error
    if (pImpl->predictionError) {
        float predErrorMagnitude = pImpl->predictionError->getMagnitude();
        float predErrorValue = pImpl->predictionError->getError();
        
        // Feed prediction error into dopamine system (reward prediction error)
        if (pImpl->dopamine) {
            // Positive prediction error (better than expected) increases dopamine
            // Negative prediction error (worse than expected) decreases dopamine
            pImpl->dopamine->signalRewardPredictionError(predErrorValue);
        }
        
        // Feed prediction error into curiosity system
        if (pImpl->curiosity) {
            pImpl->curiosity->update(predErrorMagnitude, predErrorValue, pImpl->timestep);
        }
    }
    
    // ========== STEP 8: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 9: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Get working memory traces as input to concept formation
        std::vector<float> wmTraces = {};
        if (pImpl->workingMemory) {
            wmTraces = pImpl->workingMemory->retrieve();
        }
        
        // Get prediction error to drive error-driven learning
        float predictionError = 0.0f;
        if (pImpl->predictionError) {
            predictionError = pImpl->predictionError->getError();
        }
        
        // Get dopamine level to modulate concept learning
        float dopamineLevel = 0.0f;
        if (pImpl->dopamine) {
            dopamineLevel = pImpl->dopamine->getLevel();
        }
        
        // Process working memory patterns to form/update concepts
        // Concepts are formed from neural activity patterns in working memory
        for (size_t i = 0; i < wmTraces.size(); i += 10) { // Process in chunks
            // Extract a feature vector from working memory
            std::vector<float> featureVector;
            size_t chunkSize = std::min(static_cast<size_t>(10), wmTraces.size() - i);
            for (size_t j = 0; j < chunkSize; ++j) {
                featureVector.push_back(wmTraces[i + j]);
            }
            
            // Present experience to concept formation system
            size_t conceptId = pImpl->conceptFormation->presentExperience(
                featureVector, featureVector, dopamineLevel, currentStep);
            
            // Store concept in episodic memory if concept was formed/updated
            if (conceptId > 0) {
                // Get concept details
                const DiscoveredConcept* concept = pImpl->conceptFormation->getConcept(conceptId);
                if (concept) {
                    // Store concept formation in episodic memory
                    EpisodicMemoryItem conceptEpisode;
                    conceptEpisode.timestamp = currentStep;
                    conceptEpisode.sensoryState = featureVector;
                    conceptEpisode.reward = dopamineLevel;
                    conceptEpisode.novelty = predictionError;
                    
                    // Add concept information
                    conceptEpisode.activeNeurons.push_back(static_cast<NeuronId>(conceptId));
                    conceptEpisode.neuronActivations.push_back(
                        pImpl->conceptFormation->getConceptStability(conceptId));
                    
                    pImpl->episodicMemory->storeEpisode(conceptEpisode);
                }
            }
            
            // Update concept based on prediction error (error-driven learning)
            if (std::abs(predictionError) > 0.1f) {
                // High prediction error drives concept refinement
                if (conceptId > 0) {
                    pImpl->conceptFormation->updateConcept(
                        conceptId, featureVector, featureVector, predictionError);
                }
            }
        }
        
        // Apply attentional modulation to concept formation
        if (pImpl->attention && !pImpl->attention->getWinners().empty()) {
            // Attention winners influence which concepts are prioritized
            // This is a simplified attention mechanism - in practice this would be more sophisticated
            for (NeuronId winner : pImpl->attention->getWinners()) {
                // We could boost concepts associated with attended neurons
                // For now, this is a placeholder for attentional selection in concept formation
            }
        }
    }
    }
    
    // ========== STEP 10: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 11: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 12: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
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
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Store input pattern for working memory encoding
    std::vector<float> sensoryPattern;
    sensoryPattern.reserve(values.size());
    
    // Distribute input across sensory neurons and store in working memory
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
            // Store normalized value for working memory encoding
            sensoryPattern.push_back(std::max(0.0f, normalizedValue) / 10.0f);
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory if input is significant
        if (pImpl->workingMemory && std::abs(normalizedValue) > 1.0f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(),
                std::abs(normalizedValue) / 20.0f);
        }
    }
    
    // Encode the complete sensory pattern in working memory
    if (pImpl->workingMemory && !sensoryPattern.empty()) {
        pImpl->workingMemory->store(sensoryPattern, 0.5f);
    }
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
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
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
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
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
    
    pImpl->spikeSystem->reset();
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
        
        // Collect neuron data from all regions
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
                    neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                    neuronData.regionId.push_back(region->getId().index());
                    // Find population ID
                    for (size_t popIdx = 0; popIdx < region->getPopulationCount(); ++popIdx) {
                        auto* popPtr = region->getPopulation(PopulationId(popIdx));
                        if (popPtr && std::find(popPtr->getNeurons().begin(), popPtr->getNeurons().end(), neuron) != popPtr->getNeurons().end()) {
                            neuronData.populationId.push_back(popIdx);
                            break;
                        }
                    }
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
                synapseData.plasticityFlags.push_back(syn->getPlasticityFlags().value);
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
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
        
        // Apply synapse states with matching
        for (size_t i = 0; i < synapseData.sourceNeuron.size(); ++i) {
            // Match source and destination neurons by index
            NeuronId srcId(synapseData.sourceNeuron[i]);
            NeuronId dstId(synapseData.destinationNeuron[i]);
            
            // Find matching source neuron in the brain regions
            for (auto& region : pImpl->regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == srcId) {
                        // Found source neuron
                        // Find matching destination neuron
                        for (auto& region2 : pImpl->regions) {
                            auto neurons2 = region2->getAllNeurons();
                            for (auto* neuron2 : neurons2) {
                                if (neuron2->getId() == dstId) {
                                    // Found matching neurons - need to find synapse or create if missing
                                    auto synapsesFrom = region->getSynapsesFrom(srcId);
                                    bool synapseFound = false;
                                    for (Synapse* syn : synapsesFrom) {
                                        if (syn->getDestinationNeuron() == dstId) {
                                            // Update existing synapse state
                                            syn->setWeight(synapseData.weight[i]);
                                            syn->setDelay(synapseData.delay[i]);
                                            syn->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
                                            syn->setEligibilityTrace(synapseData.eligibilityTrace[i]);
                                            syn->setEfficacy(synapseData.efficacy[i]);
                                            syn->setShortTermDepression(synapseData.shortTermDepression[i]);
                                            syn->setShortTermFacilitation(synapseData.shortTermFacilitation[i]);
                                            // Note: plasticityFlags not available in Synapse API - would need to be added
                                            synapseFound = true;
                                            break;
                                        }
                                    }
                                    
                                    if (!synapseFound) {
                                        // Create new synapse if missing (preserving original structure)
                                        // Skip if regions are different (inter-region connections handled separately)
                                        if (region->getId() == region2->getId()) {
                                            Synapse* newSyn = region->addSynapse(srcId, dstId, 
                                                                               synapseData.weight[i], 
                                                                               synapseData.delay[i]);
                                            if (newSyn) {
                                                newSyn->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
                                                newSyn->setEligibilityTrace(synapseData.eligibilityTrace[i]);
                                                newSyn->setEfficacy(synapseData.efficacy[i]);
                                                newSyn->setShortTermDepression(synapseData.shortTermDepression[i]);
                                                newSyn->setShortTermFacilitation(synapseData.shortTermFacilitation[i]);
                                                // Note: plasticityFlags not available in Synapse API
                                            }
                                        }
                                    }
                                    
                                    break; // Found destination neuron, exit inner loop
                                }
                            }
                        }
                        
                        break; // Found source neuron, exit outer loop
                    }
                }
            }
        }
        
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

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

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
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
        NLM_LOG_INFO("Working memory capacity: " + std::to_string(pImpl->workingMemory->getCapacity()) + 
                    " / " + std::to_string(pImpl->workingMemory->getMemoryNeurons().size()));
        NLM_LOG_INFO("Working memory activity: " + std::to_string(pImpl->workingMemory->getMemoryActivity()));
        NLM_LOG_INFO("Working memory winners: " + std::to_string(pImpl->workingMemory->getWinners().size()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Concept formation status
    if (pImpl->conceptFormation) {
        NLM_LOG_INFO("Concept formation concepts: " + std::to_string(pImpl->conceptFormation->getConceptCount()));
        NLM_LOG_INFO("Concept formation stable concepts: " + std::to_string(
            count_if(pImpl->conceptFormation->getConcepts().begin(), 
                    pImpl->conceptFormation->getConcepts().end(),
                    [](const DiscoveredConcept& c) { 
                        return pImpl->conceptFormation->getConceptStability(c.id) >= 0.7f; 
                    })));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Prediction system status
    if (pImpl->predictionSystem) {
        NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionSystem->getPredictionError()));
        NLM_LOG_INFO("Prediction confidence: " + std::to_string(pImpl->predictionSystem->getConfidence()));
        NLM_LOG_INFO("Error history size: " + std::to_string(pImpl->predictionSystem->getErrorHistory().size()));
    }
    if (pImpl->predictionError) {
        NLM_LOG_INFO("Prediction error signal: " + std::to_string(pImpl->predictionError->getError()));
        NLM_LOG_INFO("Prediction error magnitude: " + std::to_string(pImpl->predictionError->getMagnitude()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    // Checkpoint status
    if (pImpl->checkpointManager) {
        auto infoList = pImpl->checkpointManager->listCheckpoints();
        NLM_LOG_INFO("Checkpoint count: " + std::to_string(infoList.size()));
        
        if (!infoList.empty()) {
            NLM_LOG_INFO("Latest checkpoint: " + infoList.back().name + 
                        " (step: " + std::to_string(infoList.back().step) + ")");
        }
        
        NLM_LOG_INFO("Checkpoint directory: " + pImpl->checkpointManager->getCheckpointDir());
    }
}

} // namespace nlm
