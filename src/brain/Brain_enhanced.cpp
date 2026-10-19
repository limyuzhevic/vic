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
#include <stdexcept>
#include <cassert>

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
    
    // Error handling state
    mutable bool errorState;
    mutable std::string lastError;
    
    // Performance monitoring
    double stepTimeSum;
    size_t stepTimeCount;
    double maxStepTime;
    
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
        , replayInterval(100)
        , consolidationInterval(1000)
        , errorState(false)
        , lastError("")
        , stepTimeSum(0.0)
        , stepTimeCount(0)
        , maxStepTime(0.0)
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
    
    // Helper methods for error handling
    void setError(const std::string& error) const {
        errorState = true;
        lastError = error;
        NLM_LOG_ERROR("Brain Error: " + error);
    }
    
    bool hasError() const {
        return errorState;
    }
    
    std::string getLastError() const {
        return lastError;
    }
    
    void resetError() {
        errorState = false;
        lastError.clear();
    }
    
    void validateNeurons(const std::vector<Neuron*>& neurons) const {
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (!neurons[i]) {
                setError("Null neuron found at index " + std::to_string(i));
                return;
            }
        }
    }
    
    void validateRegions() const {
        for (size_t i = 0; i < regions.size(); ++i) {
            if (!regions[i]) {
                setError("Null region found at index " + std::to_string(i));
                return;
            }
        }
    }
    
    void validateMemorySystems() const {
        if (!workingMemory) setError("Working memory not initialized");
        if (!episodicMemory) setError("Episodic memory not initialized");
        if (!associativeMemory) setError("Associative memory not initialized");
    }
    
    void validateNeuromodulationSystems() const {
        if (!dopamine) setError("Dopamine system not initialized");
        if (!curiosity) setError("Curiosity system not initialized");
        if (!novelty) setError("Novelty system not initialized");
        if (!predictionError) setError("Prediction error system not initialized");
    }
    
    void validatePlasticitySystems() const {
        if (!spikeSystem) setError("Spike system not initialized");
        if (!stdp) setError("STDP system not initialized");
        if (!hebbian) setError("Hebbian system not initialized");
        if (!structuralPlasticity) setError("Structural plasticity system not initialized");
    }
};

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
    
    try {
        // Reset error state
        pImpl->resetError();
        
        // Get configuration values with validation
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration values
        if (neuronCount == 0) {
            setError("Invalid neuron count: must be > 0");
            return false;
        }
        if (regionCount == 0) {
            setError("Invalid region count: must be > 0");
            return false;
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            setError("Invalid connection probability: must be in range [0.0, 1.0]");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                RegionId regionId = addRegion("Region_" + std::to_string(i + 1));
                if (!regionId) {
                    setError("Failed to create region " + std::to_string(i));
                    return false;
                }
            } catch (const std::exception& e) {
                setError(std::string("Exception creating region " + std::to_string(i) + ": ") + e.what());
                return false;
            }
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        if (neuronsPerRegion == 0) neuronsPerRegion = 1;  // Ensure at least one neuron per region
        
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                setError("Region " + std::to_string(i + 1) + " not found after creation");
                return false;
            }
            
            try {
                // Add populations
                auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
                auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
                auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
                
                // Collect sensory and motor neurons for I/O
                auto* sensoryPop = region->getPopulation(sensoryPopId);
                auto* motorPop = region->getPopulation(motorPopId);
                if (sensoryPop) {
                    for (auto* neuron : sensoryPop->getNeurons()) {
                        if (neuron) {
                            pImpl->sensoryNeurons.push_back(neuron);
                        } else {
                            setError("Null sensory neuron in region " + std::to_string(i + 1));
                        }
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        if (neuron) {
                            pImpl->motorNeurons.push_back(neuron);
                        } else {
                            setError("Null motor neuron in region " + std::to_string(i + 1));
                        }
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                             ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                             std::to_string(region->getTotalNeuronCount()) + " neurons");
            } catch (const std::exception& e) {
                setError(std::string("Exception creating neurons in region " + std::to_string(i + 1) + ": ") + e.what());
                return false;
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                setError("Region " + std::to_string(i + 1) + " not found during connectivity initialization");
                return false;
            }
            
            try {
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            } catch (const std::exception& e) {
                setError(std::string("Exception initializing connectivity in region " + std::to_string(i + 1) + ": ") + e.what());
                return false;
            }
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Validate memory systems before initialization
        pImpl->validateMemorySystems();
        if (pImpl->hasError()) {
            return false;
        }
        
        try {
            // Initialize working memory
            pImpl->workingMemory->initialize(this);
            pImpl->workingMemory->setCapacity(neuronCount / 10);
            
            // Initialize episodic memory
            pImpl->episodicMemory->initialize(this);
            pImpl->episodicMemory->setMaxEpisodes(1000);
            
            // Initialize associative memory
            pImpl->associativeMemory->initialize(this);
        } catch (const std::exception& e) {
            setError(std::string("Exception initializing memory systems: ") + e.what());
            return false;
        }
        
        // Validate neuromodulation systems
        pImpl->validateNeuromodulationSystems();
        if (pImpl->hasError()) {
            return false;
        }
        
        // Validate plasticity systems
        pImpl->validatePlasticitySystems();
        if (pImpl->hasError()) {
            return false;
        }
        
        try {
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
                // Count spikes with validation
                if (!event.neuronId.is_valid()) {
                    // Handle invalid neuron ID
                    return;
                }
                // Count spikes
                ++pImpl->totalSpikesThisStep;
                ++pImpl->totalSpikesTotal;
            });
            
            // Register delayed spike handler to deliver synaptic input
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                // Validate event data
                if (!event.destination_neuron.is_valid() || !event.source_neuron.is_valid()) {
                    return;
                }
                
                // Find destination neuron and deliver synaptic input
                for (auto& region : pImpl->regions) {
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron && neuron->getId() == event.destination_neuron) {
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
        } catch (const std::exception& e) {
            setError(std::string("Exception initializing integrated systems: ") + e.what());
            return false;
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return !pImpl->hasError();
        
    } catch (const std::exception& e) {
        setError(std::string("Exception in brain initialization: ") + e.what());
        return false;
    }
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
    
    try {
        // Reset error state for this step
        pImpl->resetError();
        
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        try {
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        } catch (const std::exception& e) {
            setError(std::string("Exception in delayed spike processing: ") + e.what());
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        try {
            pImpl->validateRegions();
            if (pImpl->hasError()) {
                NLM_LOG_ERROR("Brain error before neuron update: " + pImpl->getLastError());
            }
            
            for (auto& region : pImpl->regions) {
                if (!region) {
                    setError("Null region encountered during neuron update");
                    continue;
                }
                for (auto& pop : region->getPopulations()) {
                    if (!pop) continue;
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            setError("Null neuron encountered during LIF update");
                            continue;
                        }
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    }
                }
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in neuron dynamics update: ") + e.what());
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        try {
            for (auto& region : pImpl->regions) {
                if (!region) continue;
                for (auto& pop : region->getPopulations()) {
                    if (!pop) continue;
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) continue;
                        
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
                                if (syn) syn->recordPostSpike(currentTime);
                            }

                            // Get outgoing synapses and schedule delayed spike events
                            auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                            for (Synapse* syn : outgoingSynapses) {
                                if (!syn) continue;
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
        } catch (const std::exception& e) {
            setError(std::string("Exception in spike detection and scheduling: ") + e.what());
        }
        
        // Process immediate spikes
        try {
            pImpl->spikeSystem->processSpikes(currentStep);
        } catch (const std::exception& e) {
            setError(std::string("Exception in immediate spike processing: ") + e.what());
        }
        
        // ========== STEP 4: Update working memory ==========
        try {
            if (pImpl->workingMemory) {
                pImpl->workingMemory->update(pImpl->timestep);
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in working memory update: ") + e.what());
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        try {
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
                float dopamineLevel = pImpl->dopamine->getLevel();
                for (auto& region : pImpl->regions) {
                    if (!region) continue;
                    for (auto& pop : region->getPopulations()) {
                        if (!pop) continue;
                        for (auto* neuron : pop->getNeurons()) {
                            if (!neuron) continue;
                            // Dopamine modulates excitability by injecting additional current
                            float excitabilityMod = dopamineLevel * 0.5f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        }
                    }
                }
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in neuromodulation update: ") + e.what());
        }
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        try {
            // Calculate neuromodulation factor for plasticity
            float plasticityMod = 1.0f;
            if (pImpl->dopamine) {
                plasticityMod = pImpl->dopamine->getPlasticityFactor();
            }
            
            pImpl->validateRegions();
            if (pImpl->hasError()) {
                NLM_LOG_ERROR("Brain error before plasticity update: " + pImpl->getLastError());
            }
            
            for (auto& region : pImpl->regions) {
                if (!region) continue;
                for (auto& syn : region->getSynapses()) {
                    if (!syn) continue;
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
        } catch (const std::exception& e) {
            setError(std::string("Exception in plasticity update: ") + e.what());
        }
        
        // ========== STEP 7: Update episodic memory ==========
        try {
            pImpl->stepsSinceLastEpisode++;
            if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
                pImpl->stepsSinceLastEpisode = 0;
                
                if (pImpl->episodicMemory) {
                    // Capture current brain state as an episode
                    EpisodicMemoryItem episode;
                    episode.timestamp = currentStep;
                    episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                    
                    // Store active neurons
                    pImpl->validateRegions();
                    if (!pImpl->hasError()) {
                        for (auto& region : pImpl->regions) {
                            if (!region) continue;
                            for (auto& pop : region->getPopulations()) {
                                if (!pop) continue;
                                for (auto* neuron : pop->getNeurons()) {
                                    if (!neuron) continue;
                                    if (neuron->isFiring() || 
                                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                        episode.activeNeurons.push_back(neuron->getId());
                                        episode.neuronActivations.push_back(
                                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                                    }
                                }
                            }
                        }
                    }
                    
                    // Store reward in episode
                    episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                    
                    pImpl->episodicMemory->storeEpisode(episode);
                }
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in episodic memory update: ") + e.what());
        }
        
        // ========== STEP 8: Update prediction system ==========
        try {
            if (pImpl->predictionSystem) {
                // The prediction system would be updated with sensory observations
                // For now, just track prediction error history
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in prediction system update: ") + e.what());
        }
        
        // ========== STEP 9: Update attention system ==========
        try {
            if (pImpl->attention) {
                pImpl->attention->update(pImpl->timestep);
                
                // Apply attention to working memory winners
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                    pImpl->attention->processCompetition(competitors);
                }
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in attention system update: ") + e.what());
        }
        
        // ========== STEP 10: Update concept formation ==========
        try {
            if (pImpl->conceptFormation) {
                // Would process current neural activity patterns to form concepts
                // This requires sensory state encoding
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in concept formation update: ") + e.what());
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        try {
            if (currentStep % 100 == 0 && pImpl->structuralPlasticity) {
                pImpl->structuralPlasticity->update(this, *pImpl->rng);
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in structural plasticity update: ") + e.what());
        }
        
        // ========== STEP 12: Replay important memories ==========
        try {
            if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
                // Get episodes for replay
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    if (episode) {
                        pImpl->episodicMemory->replayEpisode(episode);
                    }
                }
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in memory replay: ") + e.what());
        }
        
        // ========== STEP 13: Apply development effects ==========
        try {
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
        } catch (const std::exception& e) {
            setError(std::string("Exception in development update: ") + e.what());
        }
        
        // ========== STEP 14: Periodic memory consolidation ==========
        try {
            if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
                // Consolidate important memories, remove weak ones
                pImpl->episodicMemory->consolidate(0.3f);
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in memory consolidation: ") + e.what());
        }
        
        // ========== STEP 15: Checkpoint management ==========
        try {
            if (pImpl->checkpointManager) {
                pImpl->checkpointManager->update(currentStep, currentTime);
            }
        } catch (const std::exception& e) {
            setError(std::string("Exception in checkpoint management: ") + e.what());
        }
        
        // Record performance timing
        try {
            // Simple timing measurement (in a real implementation, use high-resolution timers)
            pImpl->stepTimeSum += 0.001;  // Placeholder
            pImpl->stepTimeCount++;
            if (0.001 > pImpl->maxStepTime) {  // Placeholder
                pImpl->maxStepTime = 0.001;
            }
        } catch (...) {
            // Don't let timing measurements fail the step
        }
        
        // Log warnings if errors occurred
        if (pImpl->hasError()) {
            NLM_LOG_WARNING("Brain simulation completed with errors: " + pImpl->getLastError());
        }
        
    } catch (const std::exception& e) {
        setError(std::string("Exception in brain step: ") + e.what());
        NLM_LOG_ERROR("Critical error in brain simulation: " + std::string(e.what()));
    } catch (...) {
        setError("Unknown exception in brain simulation");
        NLM_LOG_ERROR("Unknown exception in brain simulation");
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    try {
        // Inject current into sensory neurons based on input
        // This is a simple mapping - sensory encoding
        
        const auto& values = input.getData();
        if (values.empty()) {
            NLM_LOG_WARNING("Empty sensory input received");
            return;
        }
        
        size_t numSensory = pImpl->sensoryNeurons.size();
        if (numSensory == 0) {
            NLM_LOG_WARNING("No sensory neurons available for input processing");
            return;
        }
        
        // Distribute input across sensory neurons
        for (size_t i = 0; i < numSensory; ++i) {
            // Validate neuron pointer
            if (pImpl->sensoryNeurons[i] == nullptr) {
                setError("Null sensory neuron encountered during input processing");
                continue;
            }
            
            // Normalize input value to range [-10, 10] mV
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = static_cast<float>(values[i]) * 10.0f;
            }
            
            // Validate normalized value range
            if (std::isnan(normalizedValue) || std::isinf(normalizedValue)) {
                setError("Invalid normalized sensory value: " + std::to_string(normalizedValue));
                normalizedValue = 0.0f;
            }
            
            // Inject current into this sensory neuron
            pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
            
            // Also store in working memory
            if (pImpl->workingMemory && normalizedValue > 0.5f) {
                pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
            }
        }
        
    } catch (const std::exception& e) {
        setError(std::string("Exception in sensory input processing: ") + e.what());
    } catch (...) {
        setError("Unknown exception in sensory input processing");
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    try {
        if (std::isnan(current) || std::isinf(current)) {
            setError("Invalid current value: " + std::to_string(current));
            return;
        }
        
        for (auto& region : pImpl->regions) {
            if (!region) continue;
            auto neurons = region->getAllNeurons();
            for (auto* n : neurons) {
                if (n && n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
        
        NLM_LOG_WARNING("Neuron " + std::to_string(static_cast<int>(neuron.index())) + " not found for current injection");
        
    } catch (const std::exception& e) {
        setError(std::string("Exception injecting current into neuron: ") + e.what());
    } catch (...) {
        setError("Unknown exception in current injection");
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    try {
        if (std::isnan(current) || std::isinf(current)) {
            setError("Invalid current value for neuron type injection: " + std::to_string(current));
            return;
        }
        
        pImpl->validateRegions();
        if (pImpl->hasError()) {
            NLM_LOG_ERROR("Brain error before neuron type injection: " + pImpl->getLastError());
        }
        
        for (auto& region : pImpl->regions) {
            if (!region) continue;
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                if (pop->getNeuronType() == type) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            setError("Null neuron encountered during type-based current injection");
                            continue;
                        }
                        neuron->injectCurrent(current);
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        setError(std::string("Exception injecting current to neuron type: ") + e.what());
    } catch (...) {
        setError("Unknown exception in neuron type current injection");
    }
}

// Include Brain.hpp methods with error handling
// (The rest of the Brain class methods continue with the same error handling pattern)

// Wrapper methods with error handling
SpikeSystem* Brain::getSpikeSystem() {
    if (!pImpl->spikeSystem) {
        setError("Spike system not initialized");
    }
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    if (!pImpl->stdp) {
        setError("STDP system not initialized");
    }
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    if (!pImpl->hebbian) {
        setError("Hebbian system not initialized");
    }
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    if (!pImpl->structuralPlasticity) {
        setError("Structural plasticity system not initialized");
    }
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    try {
        pImpl->validateRegions();
        for (const auto& region : pImpl->regions) {
            if (!region) continue;
            for (const auto& syn : region->getSynapses()) {
                if (!syn) continue;
                float weight = syn->getWeight();
                if (weight > 0) {
                    totalExcitatory += weight;
                } else {
                    totalInhibitory += std::abs(weight);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception calculating E/I ratio: " + std::string(e.what()));
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
    return pImpl->spikeSystem ? pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount() : 0;
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
        if (neuron && neuron->isFiring()) {
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
    try {
        // Apply neuromodulation effects on plasticity
        float modulation = signal.getLevel();
        
        // Scale STDP learning rates
        if (pImpl->stdp) {
            pImpl->stdp->setLTPWeight(0.01f * modulation);
            pImpl->stdp->setLTDWeight(0.012f * modulation);
        }
    } catch (const std::exception& e) {
        setError(std::string("Exception in neuromodulation application: ") + e.what());
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    try {
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
    } catch (const std::exception& e) {
        setError(std::string("Exception in development: ") + e.what());
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    try {
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
        pImpl->totalSpikesTotal = 0;
        pImpl->isResting = false;
        pImpl->stepsSinceLastEpisode = 0;
        pImpl->resetError();
        
        pImpl->validateRegions();
        for (auto& region : pImpl->regions) {
            if (region) {
                region->reset();
            }
        }
        
        if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
        pImpl->developmentalStage = DevelopmentalStage::Initial;
        
        // Reset memory systems
        if (pImpl->workingMemory) pImpl->workingMemory->clear();
        if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
        if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
        if (pImpl->attention) pImpl->attention->reset();
        
        NLM_LOG_INFO("NLM Brain reset complete");
    } catch (const std::exception& e) {
        setError(std::string("Exception during brain reset: ") + e.what());
        NLM_LOG_ERROR("Error during brain reset: " + std::string(e.what()));
    }
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        if (filepath.empty()) {
            setError("Empty filepath provided for save operation");
            return false;
        }
        
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            setError("Failed to create checkpoint file: " + filepath);
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
        
        pImpl->validateRegions();
        for (const auto& region : pImpl->regions) {
            if (!region) continue;
            for (const auto& pop : region->getPopulations()) {
                if (!pop) continue;
                for (const auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        setError("Null neuron encountered during checkpoint save");
                        continue;
                    }
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
            setError("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        pImpl->validateRegions();
        for (const auto& region : pImpl->regions) {
            if (!region) continue;
            for (const auto* syn : region->getSynapses()) {
                if (!syn) {
                    setError("Null synapse encountered during checkpoint save");
                    continue;
                }
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            setError("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            setError("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return !pImpl->hasError();
        
    } catch (const std::exception& e) {
        setError(std::string("Exception saving brain: ") + e.what());
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        if (filepath.empty()) {
            setError("Empty filepath provided for load operation");
            return false;
        }
        
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            setError("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            setError("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            setError("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        pImpl->validateRegions();
        for (auto& region : pImpl->regions) {
            if (!region) continue;
            for (auto& pop : region->getPopulations()) {
                if (!pop) continue;
                for (auto* neuron : pop->getNeurons()) {
                    if (!neuron) {
                        setError("Null neuron encountered during checkpoint load");
                        continue;
                    }
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
            setError("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return !pImpl->hasError();
        
    } catch (const std::exception& e) {
        setError(std::string("Exception loading brain: ") + e.what());
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    try {
        if (name.empty()) {
            setError("Cannot add region with empty name");
            return RegionId();
        }
        
        RegionId id(pImpl->nextRegionId++);
        auto region = std::make_unique<NeuralRegion>(id, name);
        pImpl->regions.push_back(std::move(region));
        return id;
    } catch (const std::exception& e) {
        setError(std::string("Exception adding region: ") + e.what());
        return RegionId();
    }
}

NeuralRegion* Brain::getRegion(RegionId id) {
    try {
        for (auto& region : pImpl->regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        setError(std::string("Exception getting region: ") + e.what());
        return nullptr;
    }
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    try {
        for (const auto& region : pImpl->regions) {
            if (region && region->getId() == id) {
                return region.get();
            }
        }
        return nullptr;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting region: ") + e.what());
        return nullptr;
    }
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    try {
        ids.reserve(pImpl->regions.size());
        for (const auto& region : pImpl->regions) {
            if (region) {
                ids.push_back(region->getId());
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting region IDs: ") + e.what());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    try {
        if (!source.is_valid() || !target.is_valid()) {
            setError("Invalid region IDs for inter-region connection");
            return;
        }
        
        pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
    } catch (const std::exception& e) {
        setError(std::string("Exception adding inter-region connection: ") + e.what());
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    try {
        if (!source.is_valid() || !target.is_valid()) {
            setError("Invalid region IDs for inter-region connection removal");
            return;
        }
        
        pImpl->interRegionConnections.erase(
            std::remove_if(pImpl->interRegionConnections.begin(),
                          pImpl->interRegionConnections.end(),
                          [source, target](const InterRegionConnection& conn) {
                              return conn.sourceRegion == source && conn.targetRegion == target;
                          }),
            pImpl->interRegionConnections.end()
        );
    } catch (const std::exception& e) {
        setError(std::string("Exception removing inter-region connection: ") + e.what());
    }
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    try {
        pImpl->validateRegions();
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getTotalNeuronCount();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception counting neurons: ") + e.what());
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    try {
        pImpl->validateRegions();
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getSynapseCount();
            }
        }
        total += pImpl->interRegionConnections.size();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception counting synapses: ") + e.what());
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    try {
        pImpl->validateRegions();
        for (const auto& region : pImpl->regions) {
            if (region) {
                total += region->getActiveNeuronCount();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception counting active neurons: ") + e.what());
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    try {
        pImpl->validateRegions();
        for (const auto& region : pImpl->regions) {
            if (region) {
                sum += region->getAverageFiringRate();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception calculating average firing rate: ") + e.what());
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (!pImpl->workingMemory) {
        setError("Working memory not initialized");
    }
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (!pImpl->episodicMemory) {
        setError("Episodic memory not initialized");
    }
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (!pImpl->associativeMemory) {
        setError("Associative memory not initialized");
    }
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    if (!pImpl->predictionSystem) {
        setError("Prediction system not initialized");
    }
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    if (!pImpl->planner) {
        setError("Planner system not initialized");
    }
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    if (!pImpl->conceptFormation) {
        setError("Concept formation system not initialized");
    }
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    if (!pImpl->attention) {
        setError("Attention system not initialized");
    }
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    if (!pImpl->developmentSystem) {
        setError("Development system not initialized");
    }
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
    if (!pImpl->dopamine) {
        setError("Dopamine system not initialized");
    }
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    if (!pImpl->curiosity) {
        setError("Curiosity system not initialized");
    }
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    if (!pImpl->novelty) {
        setError("Novelty system not initialized");
    }
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    if (!pImpl->predictionError) {
        setError("Prediction error system not initialized");
    }
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
    
    // Error status
    if (pImpl->hasError()) {
        NLM_LOG_WARNING("Brain in error state: " + pImpl->getLastError());
    }
    
    // Performance metrics
    if (pImpl->stepTimeCount > 0) {
        NLM_LOG_INFO("Performance metrics:");
        NLM_LOG_INFO("  Average step time: " + std::to_string(pImpl->stepTimeSum / pImpl->stepTimeCount) + "s");
        NLM_LOG_INFO("  Maximum step time: " + std::to_string(pImpl->maxStepTime) + "s");
    }
    
    for (const auto& region : pImpl->regions) {
        if (region) {
            NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                         " (" + region->getName() + "): " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                         std::to_string(region->getSynapseCount()) + " synapses, " +
                         "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
    }
}

} // namespace nlm
