#include "BrainError.hpp"
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
#include <vector>
#include <unordered_map>
#include <mutex>

namespace nlm {

struct Brain::Impl {
private:
    mutable std::mutex initializationMutex_;
    mutable std::mutex stepMutex_;
    mutable std::mutex memoryMutex_;
    
    // Internal validation helpers
    void validateConfiguration() const {
        if (!config) {
            throw BrainException(BrainError::INVALID_CONFIGURATION, "Configuration is null");
        }
    }
    
    void validateSpikeSystem() const {
        if (!spikeSystem) {
            throw BrainException(BrainError::INITIALIZATION_FAILED, "Spike system not initialized");
        }
    }
    
    void validateMemorySystems() const {
        if (!workingMemory || !episodicMemory || !associativeMemory) {
            throw BrainException(BrainError::INITIALIZATION_FAILED, "Memory systems not initialized");
        }
    }
    
    void validateNeuromodulationSystems() const {
        if (!dopamine || !curiosity || !novelty) {
            throw BrainException(BrainError::INITIALIZATION_FAILED, "Neuromodulation systems not initialized");
        }
    }
    
    void validateCognitionSystems() const {
        if (!planner || !conceptFormation || !attention) {
            throw BrainException(BrainError::INITIALIZATION_FAILED, "Cognition systems not initialized");
        }
    }
    
    void validateDevelopmentSystem() const {
        if (!developmentSystem) {
            throw BrainException(BrainError::INITIALIZATION_FAILED, "Development system not initialized");
        }
    }
    
    void validatePredictionSystem() const {
        if (!predictionSystem) {
            throw BrainException(BrainError::INITIALIZATION_FAILED, "Prediction system not initialized");
        }
    }

public:
    // Thread-safe initialization
    void initializeWithValidation() {
        std::lock_guard<std::mutex> lock(initializationMutex_);
        
        validateConfiguration();
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems with error checking
        try {
            spikeSystem = std::make_unique<SpikeSystem>();
            validateSpikeSystem();
            
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
        } catch (const std::bad_alloc&) {
            throw BrainException(BrainError::MEMORY_ALLOCATION_FAILED, "Failed to allocate plasticity systems");
        }
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        try {
            // Initialize memory systems
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            
            validateMemorySystems();
            
            // Initialize prediction system
            predictionSystem = std::make_unique<PredictionSystem>();
            validatePredictionSystem();
            
            // Initialize cognition systems
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
            
            validateCognitionSystems();
            
            // Initialize development system
            developmentSystem = std::make_unique<DevelopmentSystem>();
            validateDevelopmentSystem();
            
            // Initialize neuromodulation systems
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
            
            validateNeuromodulationSystems();
        } catch (const std::bad_alloc&) {
            throw BrainException(BrainError::MEMORY_ALLOCATION_FAILED, "Failed to allocate integrated systems");
        }
        
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
    
    // Thread-safe step execution with comprehensive validation
    void executeStepWithValidation(SimulationStep currentStep, Timestamp currentTime) {
        std::lock_guard<std::mutex> lock(stepMutex_);
        
        // Validate inputs
        if (currentStep < 0) {
            throw BrainException(BrainError::STEP_EXECUTION_FAILED, "Invalid step number: negative value");
        }
        
        // Validate systems
        validateSpikeSystem();
        validateMemorySystems();
        validateCognitionSystems();
        validateDevelopmentSystem();
        
        // Update state
        currentStep = currentStep;
        currentTime = currentTime;
        totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes ==========
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
        
        // ========== STEP 2: Update all neurons ==========
        for (auto& region : regions) {
            if (!validateRegion(region.get())) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!validatePopulation(pop.get())) continue;
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!validateNeuron(neuron)) continue;
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
        
        // ========== STEP 3: Detect and process spikes ==========
        for (auto& region : regions) {
            if (!validateRegion(region.get())) continue;
            
            for (auto& pop : region->getPopulations()) {
                if (!validatePopulation(pop.get())) continue;
                
                for (auto* neuron : pop->getNeurons()) {
                    if (!validateNeuron(neuron)) continue;
                    
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
                            if (!validateSynapse(syn)) continue;
                            syn->recordPostSpike(currentTime);
                        }

                        // Get outgoing synapses and schedule delayed spike events
                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            if (!validateSynapse(syn)) continue;
                            
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
                        
                        // Store to working memory
                        if (workingMemory) {
                            workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
        
        // Process immediate spikes
        spikeSystem->processSpikes(currentStep);
        
        // ========== STEP 4: Update working memory ==========
        if (workingMemory) {
            std::lock_guard<std::mutex> memoryLock(memoryMutex_);
            workingMemory->update(timestep);
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
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
            float dopamineLevel = dopamine->getLevel();
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (!validateNeuron(neuron)) continue;
                        
                        // Dopamine modulates excitability by injecting additional current
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
        
        // ========== STEP 6: Apply plasticity rules ==========
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (dopamine) {
            plasticityMod = dopamine->getPlasticityFactor();
        }
        
        for (auto& region : regions) {
            for (auto& syn : region->getSynapses()) {
                if (!validateSynapse(&syn)) continue;
                
                // Apply STDP with neuromodulation
                if (syn.getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn.getPreSpikeHistory();
                    const auto& postSpikes = syn.getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        stdp->update(&syn, preSpikes, postSpikes, timesteptep);
                        float weight = syn.getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn.setWeight(weight);
                    }
                }
                
                // Apply Hebbian learning
                if (syn.getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn.getPreSpikeHistory();
                    const auto& postSpikes = syn.getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        hebbian->update(&syn, preSpikes, postSpikes, timesteptep);
                    }
                }
                
                // Update synapse state
                syn.step(currentTime);
            }
        }
        
        // ========== STEP 7: Update episodic memory ==========
        stepsSinceLastEpisode++;
        if (stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
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
                            if (!validateNeuron(neuron)) continue;
                            
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
        
        // ========== STEP 8: Update prediction system ==========
        if (predictionSystem) {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        }
        
        // ========== STEP 9: Update attention system ==========
        if (attention) {
            attention->update(timestep);
            
            // Apply attention to working memory winners
            if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                attention->processCompetition(competitors);
            }
        }
        
        // ========== STEP 10: Update concept formation ==========
        if (conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
        
        // ========== STEP 11: Apply structural plasticity ==========
        if (currentStep % 100 == 0) {
            structuralPlasticity->update(this, *rng);
        }
        
        // ========== STEP 12: Replay important memories ==========
        if (currentStep % replayInterval == 0 && episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
        }
        
        // ========== STEP 13: Apply development effects ==========
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            developmentSystem->update(this, *rng, timesteptep * 1000);
            
            // Development affects plasticity rates
            auto* sp = structuralPlasticity.get();
            if (sp) {
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
                
                sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
        
        // ========== STEP 14: Periodic memory consolidation ==========
        if (currentStep % consolidationInterval == 0 && episodicMemory) {
            // Consolidate important memories, remove weak ones
            episodicMemory->consolidate(0.3f);
        }
        
        // ========== STEP 15: Checkpoint management ==========
        if (checkpointManager) {
            checkpointManager->update(currentStep, currentTime);
        }
    }
    
    // Helper method to get neural safe wrapper for neurons
    NeuralSafeWrapper<Neuron> safeNeuron(Neuron* neuron) {
        return NeuralSafeWrapper<Neuron>(neuron, __FILE__, __LINE__, "neuron access");
    }
    
    NeuralSafeWrapper<Synapse> safeSynapse(Synapse* synapse) {
        return NeuralSafeWrapper<Synapse>(synapse, __FILE__, __LINE__, "synapse access");
    }
    
    // Configuration for methods
    bool initialize() {
        try {
            initializeWithValidation();
            NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
            NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
            NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
            NLM_LOG_INFO("Sensory neurons: " + std::to_string(sensoryNeurons.size()));
            NLM_LOG_INFO("Motor neurons: " + std::to_string(motorNeurons.size()));
            return true;
        } catch (const BrainException& e) {
            NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
            return false;
        } catch (const std::bad_alloc&) {
            NLM_LOG_ERROR("Brain initialization failed: Memory allocation error");
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
            return false;
        }
    }
    
    void receiveSensoryInput(const class SensoryInput& input) {
        try {
            if (!validateBrain(this)) return;
            
            const auto& values = input.getData();
            if (values.empty()) return;
            
            size_t numSensory = sensoryNeurons.size();
            if (numSensory == 0) return;
            
            // Distribute input across sensory neurons with error checking
            for (size_t i = 0; i < numSensory; ++i) {
                // Normalize input value to range [-10, 10] mV
                float normalizedValue = 0.0f;
                if (i < values.size()) {
                    normalizedValue = static_cast<float>(values[i]) * 10.0f;
                }
                
                // Use safe wrapper for neuron access
                auto neuronWrapper = safeNeuron(sensoryNeurons[i]);
                neuronWrapper->injectCurrent(normalizedValue);
                
                // Also store in working memory
                if (workingMemory && normalizedValue > 0.5f) {
                    workingMemory->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
                }
            }
        } catch (const BrainException& e) {
            NLM_LOG_ERROR(std::string("Sensory input failed: ") + e.what());
        }
    }
    
    // ... other methods would follow with similar improvements
    
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
    
    // Thread safety
    std::mutex initializationMutex_;
    std::mutex stepMutex_;
    std::mutex memoryMutex_;
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    initializeBrainErrorLogging();
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

// Improved initialize with comprehensive error handling
bool Brain::initialize() {
    try {
        NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
        
        // Validate input
        if (!pImpl) {
            throw BrainException(BrainError::INITIALIZATION_FAILED, "Brain implementation is null");
        }
        
        // Get configuration values with validation
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration
        if (neuronCount == 0) {
            throw BrainException(BrainError::INVALID_CONFIGURATION, "Neuron count cannot be zero");
        }
        if (regionCount == 0) {
            throw BrainException(BrainError::INVALID_CONFIGURATION, "Region count cannot be zero");
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            throw BrainException(BrainError::INVALID_CONFIGURATION, "Connection probability must be between 0 and 1");
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions with error handling
        for (size_t i = 0; i < regionCount; ++i) {
            try {
                addRegion("Region_" + std::to_string(i + 1));
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to create region " ) + std::to_string(i + 1) + ": " + e.what());
                return false;
            }
        }
        
        // Create neurons across regions with validation
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!validateRegion(region)) {
                continue;
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
                        if (validateNeuron(neuron)) {
                            pImpl->sensoryNeurons.push_back(neuron);
                        }
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        if (validateNeuron(neuron)) {
                            pImpl->motorNeurons.push_back(neuron);
                        }
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                            ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                            std::to_string(region->getTotalNeuronCount()) + " neurons");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to create neurons in region " ) + std::to_string(i + 1) + ": " + e.what());
                continue;
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!validateRegion(region)) continue;
            
            try {
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Failed to initialize connectivity for region " ) + std::to_string(i + 1) + ": " + e.what());
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
            
            // Register delayed spike handler to deliver synaptic input
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
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
        
    } catch (const BrainException& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    } catch (const std::bad_alloc&) {
        NLM_LOG_ERROR("Brain initialization failed: Memory allocation error");
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain initialization failed: ") + e.what());
        return false;
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    try {
        // Use the improved implementation with validation
        pImpl->executeStepWithValidation(currentStep, currentTime);
    } catch (const BrainException& e) {
        NLM_LOG_ERROR(std::string("Brain step execution failed: ") + e.what());
        // Don't throw from step to maintain simulation continuity
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain step: ") + e.what());
    }
}

// ... other methods would follow with similar improvements

} // namespace nlm
