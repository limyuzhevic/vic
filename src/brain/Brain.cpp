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

// Add checkpoint data structures
struct WorkingMemoryCheckpointData {
    std::vector<float> activations;
    std::vector<NeuronId> neurons;
    std::vector<SimulationStep> timestamps;
    float decayRate;
    size_t capacity;
};

struct NeuromodulationCheckpointData {
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionError;
};

struct DevelopmentCheckpointData {
    DevelopmentalStage stage;
    float maturityLevel;
    bool hasPruned;
    bool hasSynaptogenesis;
};

struct PredictionCheckpointData {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<float> predictionBuffers;
};

struct CognitiveCheckpointData {
    std::vector<float> actionQuality;
    std::vector<float> currentGoal;
    size_t planningDepth;
    float planningConfidence;
    std::vector<bool> recentPlanSuccess;
};

struct RandomGeneratorCheckpointData {
    uint64_t currentState;
    uint64_t seed;
};

struct SimulationCheckpointData {
    uint64_t currentStep;
    Timestamp currentTime;
    uint64_t totalSpikesTotal;
    SimulationStep developmentalStage;
    size_t stepsSinceLastEpisode;
};

struct BrainCheckpointData {
    // Neural data (already handled by CheckpointSystem)
    std::vector<float> neuronMembranePotentials;
    std::vector<float> neuronRestingPotentials;
    std::vector<float> neuronThresholds;
    std::vector<float> neuronResetPotentials;
    std::vector<float> neuronLeakConductances;
    std::vector<uint8_t> neuronFiringStates;
    std::vector<uint32_t> neuronRefractoryRemainings;
    std::vector<uint32_t> neuronRefractoryPeriods;
    std::vector<float> neuronLastSpikeTimes;
    
    // Synapse data (already handled by CheckpointSystem)
    std::vector<uint64_t> synapseSourceNeurons;
    std::vector<uint64_t> synapseDestinationNeurons;
    std::vector<float> synapseWeights;
    std::vector<uint32_t> synapseDelays;
    std::vector<uint8_t> synapseTypes;
    std::vector<uint8_t> synapsePlasticityFlags;
    std::vector<float> synapseEligibilityTraces;
    
    // System data
    WorkingMemoryCheckpointData workingMemory;
    NeuromodulationCheckpointData neuromodulation;
    DevelopmentCheckpointData development;
    PredictionCheckpointData prediction;
    CognitiveCheckpointData cognition;
    RandomGeneratorCheckpointData randomGenerator;
    SimulationCheckpointData simulation;
    InterRegionConnection interRegionConnections;
};

// Forward declaration for CheckpointSystem namespace
namespace nlm {
    struct Brain::Impl {
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
        
    // Get spike system (const version)
    const SpikeSystem* getSpikeSystem() const {
        return pImpl->spikeSystem.get();
    }
    
    // Get STDP (const version)
    const STDP* getSTDP() const {
        return pImpl->stdp.get();
    }
    
    // Get Hebbian (const version)
    const Hebbian* getHebbian() const {
        return pImpl->hebbian.get();
    }
    
    // Get structural plasticity (const version)
    const StructuralPlasticity* getStructuralPlasticity() const {
        return pImpl->structuralPlasticity.get();
    }
    
    // Get random generator (const version)
    const RandomGenerator* getRandomGenerator() const {
        return pImpl->rng.get();
    }
    
    // Get configuration (const version)
    const Config* getConfig() const {
        return pImpl->config.get();
    }
    
    // Get current timestep (const version)
    double getTimestep() const {
        return pImpl->timestep;
    }
    
    // Get current simulation step (const version)
    uint64_t getCurrentStep() const {
        return pImpl->currentStep;
    }
    
    // Get current simulation time (const version)
    double getCurrentTime() const {
        return pImpl->currentTime;
    }
    
    // Get total spike count (const version)
    uint64_t getTotalSpikesTotal() const {
        return pImpl->totalSpikesTotal;
    }
    
    // Get steps since last episode (const version)
    size_t getStepsSinceLastEpisode() const {
        return pImpl->stepsSinceLastEpisode;
    }
    
    // Get replay interval (const version)
    size_t getReplayInterval() const {
        return pImpl->replayInterval;
    }
    
    // Get consolidation interval (const version)
    size_t getConsolidationInterval() const {
        return pImpl->consolidationInterval;
    }
    
    // Get is resting (const version)
    bool getIsResting() const {
        return pImpl->isResting;
    }
    
    // Get sensory neurons (const version)
    const std::vector<Neuron*>& getSensoryNeurons() const {
        return pImpl->sensoryNeurons;
    }
    
    // Get motor neurons (const version)
    const std::vector<Neuron*>& getMotorNeurons() const {
        return pImpl->motorNeurons;
    }
    
    // Helper functions for checkpoint manager
    bool saveNeurons(NeuronCheckpointData& data) {
        data.membranePotential.clear();
        data.restingPotential.clear();
        data.threshold.clear();
        data.resetPotential.clear();
        data.leakConductance.clear();
        data.firingState.clear();
        data.refractoryRemaining.clear();
        data.refractoryPeriod.clear();
        data.lastSpikeTime.clear();
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    data.membranePotential.push_back(state.membranePotential);
                    data.restingPotential.push_back(state.restingPotential);
                    data.threshold.push_back(state.threshold);
                    data.resetPotential.push_back(state.resetPotential);
                    data.leakConductance.push_back(state.leakConductance);
                    data.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    data.refractoryRemaining.push_back(state.refractoryRemaining);
                    data.refractoryPeriod.push_back(state.refractoryPeriod);
                    data.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        return !data.membranePotential.empty();
    }
    
    bool saveSynapses(SynapseCheckpointData& data) {
        data.sourceNeuron.clear();
        data.destinationNeuron.clear();
        data.weight.clear();
        data.delay.clear();
        data.synapseType.clear();
        data.plasticityFlags.clear();
        data.eligibilityTrace.clear();
        data.efficacy.clear();
        data.shortTermDepression.clear();
        data.shortTermFacilitation.clear();
        
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                data.weight.push_back(syn->getWeight());
                data.delay.push_back(syn->getDelay());
                data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                data.plasticityFlags.push_back(syn->getPlasticityFlags());
                data.eligibilityTrace.push_back(syn->getEligibilityTrace());
                data.efficacy.push_back(1.0f); // Placeholder
                data.shortTermDepression.push_back(0.0f); // Placeholder
                data.shortTermFacilitation.push_back(0.0f); // Placeholder
            }
        }
        return !data.sourceNeuron.empty();
    }
    
    // Set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage) {
        pImpl->developmentalStage = stage;
    }
    
    // Get all regions (const version)
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const {
        return pImpl->regions;
    }
    
    // Get inter-region connections
    const std::vector<InterRegionConnection>& getInterRegionConnections() const {
        return pImpl->interRegionConnections;
    }

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
     * 8. Update prediction system with sensory input
     * 9. Update attention system based on working memory
     * 10. Update concept formation from sensory patterns
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Update neuromodulation levels
     * 15. Collect statistics
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
                
                // Store working memory for firing neurons - not just sensory neurons
                if (pImpl->workingMemory) {
                    const auto& state = neuron->getState();
                    if (neuron->isFiring() || 
                        std::abs(state.membranePotential - state.restingPotential) > 2.0f) {
                        float activation = std::abs(state.membranePotential - state.restingPotential) / 10.0f;
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), activation);
                    }
                }
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
    
    // Update working memory (maintenance and competition)
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }

    // Apply neuromodulation effects on neural excitability
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
            
            // Store active neurons
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
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // Store sensory input patterns to working memory
    if (pImpl->workingMemory) {
        // Store current sensory input patterns to working memory for pattern learning
        // Use attention-weighted values for better encoding
        std::vector<NeuronId> activeNeurons;
        std::vector<float> activations;
        
        for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
            auto* neuron = pImpl->sensoryNeurons[i];
            if (neuron->isFiring()) {
                float activation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f;
                pImpl->workingMemory->storeToNeuron(neuron->getId(), activation);
            }
        }
    }
    
    // Update prediction system with current state
    if (pImpl->predictionSystem) {
        // Get current sensory input from agent's percept if available
        // The prediction system needs real sensory input to learn
        // For now, we use a simple approach - check if AgentBrain has stored recent percept
        // This is a placeholder for actual integration with AgentBrain
        
        // Simple training with current brain state
        std::vector<float> currentState;
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    currentState.push_back(
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 50.0f);
                }
            }
        }
        
        if (!currentState.empty()) {
            class SensoryInput input;
            // TODO: Properly create SensoryInput from neural state
            // For now create placeholder
            pImpl->predictionSystem->train(input);
        }
    }

    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners for neural competition
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            std::vector<NeuronId> attended = pImpl->attention->processCompetition(competitors);
            
            // Apply attended regions to brain regions for focused processing
            for (const auto& winner : attended) {
                // Extract region ID from neuron ID (assuming regionId = neuronId / 1000)
                RegionId regionId = static_cast<RegionId>(winner.getId() / 1000);
                pImpl->attention->focusOnRegion(regionId);
            }
        }
    }

    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Process current neural activity patterns to form concepts
        // Get current brain state for concept formation
        std::vector<float> currentPattern;
        
        // Sample neural activity for concept formation
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                // Use average neuron activity in each population
                float totalActivity = 0.0f;
                size_t count = 0;
                for (auto* neuron : pop->getNeurons()) {
                    totalActivity += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                    ++count;
                }
                if (count > 0) {
                    currentPattern.push_back(totalActivity / count);
                }
            }
        }
        
        if (!currentPattern.empty()) {
            // Present pattern to concept formation system
            pImpl->conceptFormation->presentExperience(
                currentPattern,  // neural pattern
                currentPattern,  // features (same for now)
                pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f,  // reward from dopamine
                pImpl->currentStep
            );
        }
    }

    // ========== STEP 11: Apply development effects ==========
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

    // ========== STEP 16: Update neuromodulation ==========
    // Update neuromodulation systems
    if (pImpl->novelty) pImpl->novelty->update(pImpl->timestep);
    if (pImpl->curiosity) pImpl->curiosity->update(pImpl->timestep);
    if (pImpl->dopamine) pImpl->dopamine->update(pImpl->timestep);
    if (pImpl->predictionError) pImpl->predictionError->update(pImpl->timestep);

    // ========== STEP 17: Collect statistics ==========
    // Collect brain statistics for monitoring
    // (Stats collection could go here)
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory for all sensory input (not just strong inputs)
        if (pImpl->workingMemory) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
    
    // Train prediction system with sensory input
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->train(input);
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
        
        // Write inter-region connections
        std::vector<InterRegionConnection> connections;
        connections.reserve(pImpl->interRegionConnections.size());
        for (const auto& conn : pImpl->interRegionConnections) {
            connections.push_back(conn);
        }
        if (!connections.empty()) {
            writer.writeSection(CheckpointSection::Connectivity, 
                               &connections[0], 
                               connections.size() * sizeof(InterRegionConnection));
        }
        
        // Write plasticity state (STDP, Hebbian, StructuralPlasticity)
        // This would be implemented based on the actual plasticity system structures
        
        // Write development system state
        {
            DevelopmentCheckpointData devData;
            devData.stage = pImpl->developmentalStage;
            // Add development system specific state here
            writer.writeSection(CheckpointSection::Development, 
                               &devData, sizeof(DevelopmentCheckpointData));
        }
        
        // Write neuromodulation systems state
        {
            NeuromodulationCheckpointData neuromodData;
            neuromodData.dopamineLevel = 0.5f; // Placeholder - get from dopamine system
            neuromodData.curiosityLevel = 0.5f; // Placeholder
            neuromodData.noveltyLevel = 0.5f; // Placeholder
            neuromodData.predictionError = 0.5f; // Placeholder
            writer.writeSection(CheckpointSection::Neuromodulation, 
                               &neuromodData, sizeof(NeuromodulationCheckpointData));
        }
        
        // Write memory systems state
        {
            WorkingMemoryCheckpointData memData;
            // TODO: Get actual memory system state
            writer.writeSection(CheckpointSection::Memory, 
                               &memData, sizeof(WorkingMemoryCheckpointData));
        }
        
        // Write prediction system state
        {
            PredictionCheckpointData predData;
            // TODO: Get actual prediction system state
            writer.writeSection(CheckpointSection::Prediction, 
                               &predData, sizeof(PredictionCheckpointData));
        }
        
        // Write development system state
        {
            DevelopmentCheckpointData devData;
            devData.stage = pImpl->developmentalStage;
            devData.maturityLevel = 0.5f; // Placeholder - get from development system
            devData.hasPruned = false; // Placeholder - get from development system
            devData.hasSynaptogenesis = false; // Placeholder - get from development system
            writer.writeSection(CheckpointSection::Development, 
                               &devData, sizeof(DevelopmentCheckpointData));
        }
        
        // Write neuromodulation systems state
        {
            NeuromodulationCheckpointData neuromodData;
            neuromodData.dopamineLevel = 0.5f; // Placeholder - get from dopamine system
            neuromodData.curiosityLevel = 0.5f; // Placeholder
            neuromodData.noveltyLevel = 0.5f; // Placeholder
            neuromodData.predictionError = 0.5f; // Placeholder
            writer.writeSection(CheckpointSection::Neuromodulation, 
                               &neuromodData, sizeof(NeuromodulationCheckpointData));
        }
        
        // Write memory systems state
        {
            WorkingMemoryCheckpointData memData;
            memData.decayRate = 0.01f; // Placeholder - get from working memory
            memData.capacity = 100; // Placeholder - get from working memory
            // Get actual memory system state
            if (pImpl->workingMemory) {
                auto ret = pImpl->workingMemory->retrieve();
                memData.activations = ret;
                memData.neurons = pImpl->workingMemory->getMemoryNeurons();
                memData.timestamps.resize(memData.neurons.size(), pImpl->currentStep);
            }
            writer.writeSection(CheckpointSection::Memory, 
                               &memData, sizeof(WorkingMemoryCheckpointData));
        }
        
        // Write prediction system state
        {
            PredictionCheckpointData predData;
            // Get actual prediction system state
            if (pImpl->predictionSystem) {
                predData.predictionError = pImpl->predictionSystem->getPredictionError();
                predData.confidence = pImpl->predictionSystem->getConfidence();
                predData.errorHistory = pImpl->predictionSystem->getErrorHistory();
                // predictionBuffers would need to be added to PredictionSystem
            }
            writer.writeSection(CheckpointSection::Prediction, 
                               &predData, sizeof(PredictionCheckpointData));
        }
        
        // Write cognitive systems state
        {
            CognitiveCheckpointData cogData;
            cogData.planningDepth = 3; // Placeholder - get from planner
            cogData.planningConfidence = 0.5f; // Placeholder - get from planner
            // Get actual cognitive system state
            if (pImpl->planner) {
                cogData.actionQuality = std::vector<float>(10, 0.5f); // Placeholder
                cogData.currentGoal = std::vector<float>(5, 0.5f); // Placeholder
                cogData.recentPlanSuccess = std::vector<bool>(10, true); // Placeholder
            }
            writer.writeSection(CheckpointSection::Environment, 
                               &cogData, sizeof(CognitiveCheckpointData));
        }
        
        // Write random generator state
        {
            RandomGeneratorCheckpointData rngData;
            // Get actual random generator state
            if (pImpl->rng) {
                rngData.currentState = pImpl->rng->getState(); // Assuming this method exists
                rngData.seed = pImpl->rng->getSeed(); // Assuming this method exists
            }
            writer.writeSection(CheckpointSection::RandomState, 
                               &rngData, sizeof(RandomGeneratorCheckpointData));
        }
        
        // Write simulation state
        {
            SimulationCheckpointData simData;
            simData.currentStep = pImpl->currentStep;
            simData.currentTime = pImpl->currentTime;
            simData.totalSpikesTotal = pImpl->totalSpikesTotal;
            simData.developmentalStage = pImpl->developmentalStage;
            simData.stepsSinceLastEpisode = pImpl->stepsSinceLastEpisode;
            writer.writeSection(CheckpointSection::SimulationState, 
                               &simData, sizeof(SimulationCheckpointData));
        }
        
        // Write spike history
        {
            // TODO: Get actual spike history data
            // For now, create a minimal spike history entry
            struct SpikeHistoryEntry {
                uint64_t neuronId;
                Timestamp lastSpikeTime;
                uint32_t spikeCount;
            };
            std::vector<SpikeHistoryEntry> spikeHistory;
            writer.writeSection(CheckpointSection::SpikeHistory, 
                               &spikeHistory[0], 
                               spikeHistory.size() * sizeof(SpikeHistoryEntry));
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
        
        // Load development system state
        {
            DevelopmentCheckpointData devData;
            auto devSection = reader.readSection(CheckpointSection::Development);
            if (!devSection.empty() && devSection.size() >= sizeof(DevelopmentCheckpointData)) {
                std::memcpy(&devData, devSection.data(), sizeof(DevelopmentCheckpointData));
                pImpl->developmentalStage = devData.stage;
                // Restore development system specific state
            }
        }
        
        // Load neuromodulation systems state
        {
            NeuromodulationCheckpointData neuromodData;
            auto neuromodSection = reader.readSection(CheckpointSection::Neuromodulation);
            if (!neuromodSection.empty() && neuromodSection.size() >= sizeof(NeuromodulationCheckpointData)) {
                std::memcpy(&neuromodData, neuromodSection.data(), sizeof(NeuromodulationCheckpointData));
                // Restore neuromodulation systems state (dopamine, curiosity, novelty, prediction error)
                // These would be restored from the actual neuromodulation systems
            }
        }
        
        // Load memory systems state
        {
            WorkingMemoryCheckpointData memData;
            auto memSection = reader.readSection(CheckpointSection::Memory);
            if (!memSection.empty() && memSection.size() >= sizeof(WorkingMemoryCheckpointData)) {
                std::memcpy(&memData, memSection.data(), sizeof(WorkingMemoryCheckpointData));
                // Restore working memory state
                if (pImpl->workingMemory) {
                    pImpl->workingMemory->clear();
                    for (size_t i = 0; i < memData.neurons.size() && i < memData.activations.size(); ++i) {
                        pImpl->workingMemory->storeToNeuron(memData.neurons[i], memData.activations[i]);
                    }
                    pImpl->workingMemory->setDecayRate(memData.decayRate);
                    pImpl->workingMemory->setCapacity(memData.capacity);
                }
            }
        }
        
        // Load prediction system state
        {
            PredictionCheckpointData predData;
            auto predSection = reader.readSection(CheckpointSection::Prediction);
            if (!predSection.empty() && predSection.size() >= sizeof(PredictionCheckpointData)) {
                std::memcpy(&predData, predSection.data(), sizeof(PredictionCheckpointData));
                // Restore prediction system state
                if (pImpl->predictionSystem) {
                    // Restore prediction system state (predictionError, confidence, errorHistory)
                    // These would be restored based on the actual PredictionSystem interface
                }
            }
        }
        
        // Load cognitive systems state
        {
            CognitiveCheckpointData cogData;
            auto cogSection = reader.readSection(CheckpointSection::Environment);
            if (!cogSection.empty() && cogSection.size() >= sizeof(CognitiveCheckpointData)) {
                std::memcpy(&cogData, cogSection.data(), sizeof(CognitiveCheckpointData));
                // Restore cognitive systems state (planner, concept formation, attention)
                if (pImpl->planner) {
                    // Restore planner state
                    pImpl->planner->setPlanningDepth(cogData.planningDepth);
                    pImpl->planner->setPlanningConfidence(cogData.planningConfidence);
                }
                // Restore other cognitive systems
            }
        }
        
        // Load random generator state
        {
            RandomGeneratorCheckpointData rngData;
            auto rngSection = reader.readSection(CheckpointSection::RandomState);
            if (!rngSection.empty() && rngSection.size() >= sizeof(RandomGeneratorCheckpointData)) {
                std::memcpy(&rngData, rngSection.data(), sizeof(RandomGeneratorCheckpointData));
                // Restore random generator state
                if (pImpl->rng) {
                    // Restore random generator state (currentState, seed)
                }
            }
        }
        
        // Load simulation state
        {
            SimulationCheckpointData simData;
            auto simSection = reader.readSection(CheckpointSection::SimulationState);
            if (!simSection.empty() && simSection.size() >= sizeof(SimulationCheckpointData)) {
                std::memcpy(&simData, simSection.data(), sizeof(SimulationCheckpointData));
                pImpl->currentStep = simData.currentStep;
                pImpl->currentTime = simData.currentTime;
                pImpl->totalSpikesTotal = simData.totalSpikesTotal;
                pImpl->developmentalStage = simData.developmentalStage;
                pImpl->stepsSinceLastEpisode = simData.stepsSinceLastEpisode;
            }
        }
        
        // Load spike history
        {
            auto spikeSection = reader.readSection(CheckpointSection::SpikeHistory);
            if (!spikeSection.empty()) {
                // Restore spike history data
                // Parse and restore spike history for neurons
            }
        }
        
        // Load inter-region connections
        {
            auto connSection = reader.readSection(CheckpointSection::Connectivity);
            if (!connSection.empty()) {
                // Parse and restore inter-region connections
                const InterRegionConnection* connections = reinterpret_cast<const InterRegionConnection*>(connSection.data());
                size_t connCount = connSection.size() / sizeof(InterRegionConnection);
                pImpl->interRegionConnections.clear();
                pImpl->interRegionConnections.reserve(connCount);
                for (size_t i = 0; i < connCount; ++i) {
                    pImpl->interRegionConnections.push_back(connections[i]);
                }
            }
        }
        
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
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
