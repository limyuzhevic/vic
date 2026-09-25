#include "brain/Brain.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include <chrono>
#include <ctime>
#include <unordered_set>
#include <map>
#include <deque>
#include <bitset>
#include <atomic>
#include <thread>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <filesystem>

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
        
        try {
            rng = std::make_unique<RandomGenerator>(seed);
            NLM_LOG_INFO("Random generator initialized with seed: " + std::to_string(seed));
        } catch (const std::exception& e) {
            NLM_THROW(NLMErrors::MemoryAllocation, "Failed to initialize random generator: " + std::string(e.what()));
        }
        
        // Initialize plasticity systems
        try {
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
            NLM_LOG_INFO("Plasticity systems initialized");
        } catch (const std::exception& e) {
            NLM_THROW(NLMErrors::PlasticityFailed, "Failed to initialize plasticity systems: " + std::string(e.what()));
        }
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        try {
            // Initialize memory systems
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            NLM_LOG_INFO("Memory systems initialized");
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to initialize all memory systems: " + std::string(e.what()));
        }
        
        try {
            // Initialize prediction system
            predictionSystem = std::make_unique<PredictionSystem>();
            NLM_LOG_INFO("Prediction system initialized");
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to initialize prediction system: " + std::string(e.what()));
        }
        
        try {
            // Initialize cognition systems
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
            NLM_LOG_INFO("Cognition systems initialized");
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to initialize cognition systems: " + std::string(e.what()));
        }
        
        try {
            // Initialize development system
            developmentSystem = std::make_unique<DevelopmentSystem>();
            NLM_LOG_INFO("Development system initialized");
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to initialize development system: " + std::string(e.what()));
        }
        
        try {
            // Initialize neuromodulation systems
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
            NLM_LOG_INFO("Neuromodulation systems initialized");
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to initialize neuromodulation systems: " + std::string(e.what()));
        }
        
        // Configure STDP parameters
        try {
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            NLM_VALIDATE(ltpWeight, ltpWeight >= 0.0f && ltpWeight <= 1.0f, NLMErrors::InvalidParameter, "LTP weight out of valid range");
            NLM_VALIDATE(ltdWeight, ltdWeight >= 0.0f && ltdWeight <= 1.0f, NLMErrors::InvalidParameter, "LTD weight out of valid range");
            NLM_VALIDATE(tau, tau > 0.0f, NLMErrors::InvalidParameter, "STDP tau must be positive");
            
            stdp->configure(ltpWeight, ltdWeight, tau);
            NLM_LOG_INFO("STDP configured: LTP weight = " + std::to_string(ltpWeight) + ", LTD weight = " + std::to_string(ltdWeight) + ", tau = " + std::to_string(tau));
        } catch (const std::exception& e) {
            NLM_THROW(NLMErrors::PlasticityFailed, "Failed to configure STDP parameters: " + std::string(e.what()));
        }
        
        // Configure structural plasticity
        try {
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            NLM_VALIDATE(synaptogenesisRate, synaptogenesisRate >= 0.0f && synaptogenesisRate <= 1.0f, NLMErrors::InvalidParameter, "Synaptogenesis rate out of valid range");
            NLM_VALIDATE(pruningRate, pruningRate >= 0.0f && pruningRate <= 1.0f, NLMErrors::InvalidParameter, "Pruning rate out of valid range");
            
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
            NLM_LOG_INFO("Structural plasticity configured: synaptogenesis rate = " + std::to_string(synaptogenesisRate) + ", pruning rate = " + std::to_string(pruningRate));
        } catch (const std::exception& e) {
            NLM_THROW(NLMErrors::PlasticityFailed, "Failed to configure structural plasticity: " + std::string(e.what()));
        }
        
        // Get timestep
        try {
            timestep = config->getOr<double>("simulation_timestep", 0.001);
            NLM_VALIDATE(timestep, timestep > 0.0 && timestep < 1.0, NLMErrors::InvalidParameter, "Simulation timestep out of valid range");
        } catch (const std::exception& e) {
            NLM_THROW(NLMErrors::ConfigParsing, "Failed to get simulation timestep from config: " + std::string(e.what()));
        }
        
        // Get integration intervals from config
        try {
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
            NLM_VALIDATE(replayInterval, replayInterval > 0, NLMErrors::InvalidParameter, "Replay interval must be positive");
            NLM_VALIDATE(consolidationInterval, consolidationInterval > 0, NLMErrors::InvalidParameter, "Consolidation interval must be positive");
        } catch (const std::exception& e) {
            NLM_THROW(NLMErrors::ConfigParsing, "Failed to get integration intervals from config: " + std::string(e.what()));
        }
        
        // Initialize checkpoint manager
        try {
            checkpointManager = std::make_unique<CheckpointManager>();
            NLM_LOG_INFO("Checkpoint manager initialized");
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to initialize checkpoint manager: " + std::string(e.what()));
        }
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(*config)) {}

Brain::~Brain() = default;

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain...");
    
    try {
        // Get neuron and region counts from config
        int64_t neuronCount = pImpl->config->getOr<int64_t>("neuron_count", 500);
        int64_t regionCount = pImpl->config->getOr<int64_t>("region_count", 1);
        int64_t connectionProbability = pImpl->config->getOr<int64_t>("connection_probability", 15);
        
        NLM_VALIDATE(neuronCount, neuronCount > 0, NLMErrors::InvalidParameter, "Neuron count must be positive");
        NLM_VALIDATE(regionCount, regionCount > 0, NLMErrors::InvalidParameter, "Region count must be positive");
        NLM_VALIDATE(connectionProbability, connectionProbability >= 0 && connectionProbability <= 100, NLMErrors::InvalidParameter, "Connection probability must be between 0 and 100");
        
        // Create regions
        NLM_LOG_INFO("Creating " + std::to_string(regionCount) + " brain regions...");
        for (int64_t i = 1; i <= regionCount; ++i) {
            auto region = std::make_unique<NeuralRegion>(
                RegionId(i),
                static_cast<size_t>(neuronCount / regionCount),
                pImpl->rng.get(),
                pImpl->config.get()
            );
            
            if (!region->initialize()) {
                NLM_THROW(NLMErrors::SystemNotInitialized, "Failed to initialize region " + std::to_string(i));
            }
            
            NLM_LOG_INFO("Region " + std::to_string(i) + " initialized with " + 
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
            
            pImpl->regions.push_back(std::move(region));
        }
        
        NLM_LOG_INFO("Brain initialization completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain initialization failed: " + std::string(e.what()));
        return false;
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting brain to initial state");
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset plasticity systems
    if (spikeSystem) spikeSystem->reset();
    if (stdp) stdp->reset();
    if (hebbian) hebbian->reset();
    if (structuralPlasticity) structuralPlasticity->reset();
    
    // Reset integrated systems
    if (workingMemory) workingMemory->reset();
    if (episodicMemory) episodicMemory->reset();
    if (predictionSystem) predictionSystem->reset();
    if (planner) planner->reset();
    if (conceptFormation) conceptFormation->reset();
    if (attention) attention->reset();
    if (developmentSystem) developmentSystem->reset();
    if (dopamine) dopamine->reset();
    if (curiosity) curiosity->reset();
    if (predictionError) predictionError->reset();
    if (novelty) novelty->reset();
    
    NLM_LOG_INFO("Brain reset completed");
}

void Brain::step(SimulationStep step, Timestamp time) {
    try {
        // Validate inputs
        NLM_VALIDATE(step, step > 0, NLMErrors::InvalidParameter, "Simulation step must be positive");
        NLM_VALIDATE(time, time >= 0.0, NLMErrors::InvalidParameter, "Time must be non-negative");
        
        // Update step and time
        pImpl->currentStep = step;
        pImpl->currentTime = time;
        
        // Check for resting phase
        if (pImpl->isResting) {
            NLM_LOG_DEBUG("Brain is resting, skipping simulation step " + std::to_string(step));
            return;
        }
        
        // Check for memory replay
        if (pImpl->stepsSinceLastEpisode >= pImpl->replayInterval) {
            NLM_LOG_INFO("Triggering memory replay at step " + std::to_string(step));
            performMemoryReplay();
            pImpl->stepsSinceLastEpisode = 0;
        }
        
        // Check for consolidation
        if (pImpl->stepsSinceLastEpisode >= pImpl->consolidationInterval) {
            NLM_LOG_INFO("Triggering memory consolidation at step " + std::to_string(step));
            performMemoryConsolidation();
            pImpl->stepsSinceLastEpisode = 0;
        }
        
        // Get sensory input from integrated systems
        performSensoryIntegration();
        
        // Get attentional focus
        performAttentionalSelection();
        
        // Get motor plans from cognition systems
        performMotorPlanning();
        
        // Get neuromodulation signals
        performNeuromodulation();
        
        // Execute plasticity computations
        performPlasticityComputations();
        
        // Update neuron states
        updateNeurons();
        
        // Propagate spikes
        propagateSpikes();
        
        // Update memory systems
        updateMemorySystems();
        
        // Update prediction system
        updatePredictionSystem();
        
        // Update cognition systems
        updateCognitionSystems();
        
        // Update development system
        updateDevelopmentSystem();
        
        // Update neuromodulation systems
        updateNeuromodulationSystems();
        
        // Increment counters
        pImpl->stepsSinceLastEpisode++;
        pImpl->totalSpikesTotal += pImpl->totalSpikesThisStep;
        
        NLM_LOG_DEBUG("Simulation step " + std::to_string(step) + " completed. " + 
                     std::to_string(pImpl->totalSpikesThisStep) + " spikes this step");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during simulation step " + std::to_string(step) + ": " + std::string(e.what()));
        throw;
    }
}

void Brain::propagateSpikes() {
    try {
        // Clear spike counts
        pImpl->totalSpikesThisStep = 0;
        
        // Get all neurons from all regions
        std::vector<Neuron*> allNeurons;
        for (auto& region : pImpl->regions) {
            auto regionNeurons = region->getAllNeurons();
            allNeurons.insert(allNeurons.end(), regionNeurons.begin(), regionNeurons.end());
        }
        
        // Add sensory and motor neurons
        allNeurons.insert(allNeurons.end(), pImpl->sensoryNeurons.begin(), pImpl->sensoryNeurons.end());
        allNeurons.insert(allNeurons.end(), pImpl->motorNeurons.begin(), pImpl->motorNeurons.end());
        
        // Propagate spikes event-driven
        std::vector<std::pair<Neuron*, float>> spikeEvents;
        for (Neuron* neuron : allNeurons) {
            if (neuron->hasSpiked()) {
                spikeEvents.push_back(std::make_pair(neuron, neuron->getMembranePotential()));
                pImpl->totalSpikesThisStep++;
            }
        }
        
        // Propagate spikes to connected neurons
        for (const auto& spikeEvent : spikeEvents) {
            Neuron* presynaptic = spikeEvent.first;
            float voltage = spikeEvent.second;
            
            // Get synapses from the presynaptic neuron
            auto presynapticRegion = findRegionByNeuron(presynaptic);
            if (!presynapticRegion) {
                continue;
            }
            
            // Find the synapse
            auto& synapses = presynapticRegion->getSynapses();
            for (auto& synapse : synapses) {
                if (synapse->getPresynapticNeuron() == presynaptic) {
                    // Apply synaptic transmission with plasticity
                    float synapticWeight = synapse->getWeight();
                    float transmissionDelay = synapse->getDelay();
                    
                    // Get postsynaptic neuron
                    Neuron* postsynaptic = synapse->getPostsynapticNeuron();
                    if (!postsynaptic) {
                        continue;
                    }
                    
                    // Apply plasticity
                    if (spikeSystem) {
                        spikeSystem->applyPlasticity(*synapse, presynaptic, postsynaptic, voltage);
                    }
                    
                    // Inject current into postsynaptic neuron
                    float injectedCurrent = synapticWeight * voltage;
                    postsynaptic->injectCurrent(injectedCurrent);
                }
            }
        }
        
        // Also propagate through inter-region connections
        for (const auto& conn : pImpl->interRegionConnections) {
            auto sourceRegion = findRegion(conn.sourceRegion);
            auto targetRegion = findRegion(conn.targetRegion);
            
            if (sourceRegion && targetRegion) {
                // Simplified inter-region spike propagation
                NLM_LOG_DEBUG("Inter-region connection: " + std::to_string(conn.sourceRegion) + 
                             " -> " + std::to_string(conn.targetRegion) + 
                             " (weight: " + std::to_string(conn.weight) + ")");
            }
        }
        
        NLM_LOG_DEBUG("Spike propagation completed. " + std::to_string(pImpl->totalSpikesThisStep) + " spikes propagated");
        
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::IntegrationFailed, "Error during spike propagation: " + std::string(e.what()));
    }
}

// Helper methods
NeuralRegion* Brain::findRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::findRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

NeuralRegion* Brain::findRegionByNeuron(Neuron* neuron) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        if (std::find(neurons.begin(), neurons.end(), neuron) != neurons.end()) {
            return region.get();
        }
    }
    return nullptr;
}

// Integration system methods
void Brain::performSensoryIntegration() {
    NLM_LOG_DEBUG("Performing sensory integration");
    
    // Get sensory input from integrated systems
    if (predictionSystem) {
        auto sensorySignals = predictionSystem->getSensorySignals();
        for (const auto& signal : sensorySignals) {
            // Inject into sensory neurons
            injectCurrentToNeurons(NeuronType::Sensory, signal);
        }
    }
    
    if (attention) {
        auto focusedRegionId = attention->getFocusedRegion();
        if (focusedRegionId) {
            NLM_LOG_DEBUG("Attention focused on region " + std::to_string(focusedRegionId->value()));
        }
    }
}

void Brain::performAttentionalSelection() {
    NLM_LOG_DEBUG("Performing attentional selection");
    
    if (attention) {
        auto selectedNeuron = attention->selectNeuron(pImpl->regions);
        if (selectedNeuron) {
            NLM_LOG_DEBUG("Attention selected neuron " + std::to_string(selectedNeuron->getId().value()));
        }
    }
}

void Brain::performMotorPlanning() {
    NLM_LOG_DEBUG("Performing motor planning");
    
    if (planner) {
        auto motorCommands = planner->generateMotorCommands(*predictionSystem, *attention);
        for (const auto& cmd : motorCommands) {
            executeMotorCommand(cmd);
        }
    }
}

void Brain::performNeuromodulation() {
    NLM_LOG_DEBUG("Performing neuromodulation");
    
    if (dopamine) {
        dopamine->update(pImpl->currentTime, pImpl->totalSpikesThisStep);
    }
    
    if (curiosity) {
        curiosity->update(pImpl->currentTime, pImpl->totalSpikesThisStep);
    }
    
    if (predictionError) {
        predictionError->update(pImpl->currentTime, pImpl->totalSpikesThisStep);
    }
    
    if (novelty) {
        novelty->update(pImpl->currentTime, pImpl->totalSpikesThisStep);
    }
}

void Brain::performPlasticityComputations() {
    NLM_LOG_DEBUG("Performing plasticity computations");
    
    if (stdp) {
        stdp->compute(pImpl->regions);
    }
    
    if (hebbian) {
        hebbian->compute(pImpl->regions);
    }
    
    if (structuralPlasticity) {
        structuralPlasticity->compute(pImpl->regions);
    }
}

void Brain::updateNeurons() {
    NLM_LOG_DEBUG("Updating neuron states");
    
    for (auto& region : pImpl->regions) {
        region->update(pImpl->timestep);
    }
}

void Brain::updateMemorySystems() {
    NLM_LOG_DEBUG("Updating memory systems");
    
    if (workingMemory) workingMemory->update(pImpl->currentTime, pImpl->currentStep);
    if (episodicMemory) episodicMemory->update(pImpl->currentTime, pImpl->currentStep);
    if (associativeMemory) associativeMemory->update(pImpl->currentTime, pImpl->currentStep);
}

void Brain::updatePredictionSystem() {
    NLM_LOG_DEBUG("Updating prediction system");
    
    if (predictionSystem) predictionSystem->update(pImpl->currentTime, pImpl->currentStep);
}

void Brain::updateCognitionSystems() {
    NLM_LOG_DEBUG("Updating cognition systems");
    
    if (planner) planner->update(pImpl->currentTime, pImpl->currentStep);
    if (conceptFormation) conceptFormation->update(pImpl->currentTime, pImpl->currentStep);
    if (attention) attention->update(pImpl->currentTime, pImpl->currentStep);
}

void Brain::updateDevelopmentSystem() {
    NLM_LOG_DEBUG("Updating development system");
    
    if (developmentSystem) developmentSystem->update(pImpl->currentTime, pImpl->currentStep);
}

void Brain::updateNeuromodulationSystems() {
    NLM_LOG_DEBUG("Updating neuromodulation systems");
    
    if (dopamine) dopamine->update(pImpl->currentTime, pImpl->currentStep);
    if (curiosity) curiosity->update(pImpl->currentTime, pImpl->currentStep);
    if (predictionError) predictionError->update(pImpl->currentTime, pImpl->currentStep);
    if (novelty) novelty->update(pImpl->currentTime, pImpl->currentStep);
}

void Brain::performMemoryReplay() {
    NLM_LOG_INFO("Performing memory replay");
    
    if (episodicMemory) {
        episodicMemory->replay(pImpl->currentStep, pImpl->currentTime);
    }
    
    if (workingMemory) {
        workingMemory->replay(pImpl->currentStep, pImpl->currentTime);
    }
}

void Brain::performMemoryConsolidation() {
    NLM_LOG_INFO("Performing memory consolidation");
    
    if (episodicMemory) {
        episodicMemory->consolidate(pImpl->currentStep, pImpl->currentTime);
    }
    
    if (associativeMemory) {
        associativeMemory->consolidate(pImpl->currentStep, pImpl->currentTime);
    }
}

// Injection and execution methods
void Brain::injectCurrentToNeurons(NeuronType type, float current) {
    NLM_VALIDATE(current, current >= 0.0f, NLMErrors::InvalidParameter, "Current must be non-negative");
    
    std::vector<Neuron*>& neurons = (type == NeuronType::Sensory) ? pImpl->sensoryNeurons : pImpl->motorNeurons;
    
    for (Neuron* neuron : neurons) {
        neuron->injectCurrent(current);
    }
}

void Brain::executeMotorCommand(const MotorCommand& command) {
    NLM_LOG_DEBUG("Executing motor command: " + std::to_string(static_cast<int>(command)));
    
    switch (command) {
        case MotorCommand::MoveForward:
            if (auto action = std::make_unique<Action>(ActionType::MoveForward)) {
                action->execute();
            }
            break;
        case MotorCommand::MoveBackward:
            if (auto action = std::make_unique<Action>(ActionType::MoveBackward)) {
                action->execute();
            }
            break;
        case MotorCommand::TurnLeft:
            if (auto action = std::make_unique<Action>(ActionType::TurnLeft)) {
                action->execute();
            }
            break;
        case MotorCommand::TurnRight:
            if (auto action = std::make_unique<Action>(ActionType::TurnRight)) {
                action->execute();
            }
            break;
        default:
            NLM_LOG_WARNING("Unsupported motor command: " + std::to_string(static_cast<int>(command)));
            break;
    }
}

// Information and status methods
void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("  Simulation step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("  Simulation time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("  Total spikes so far: " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("  Spikes this step: " + std::to_string(pImpl->totalSpikesThisStep));
    NLM_LOG_INFO("  Neuron count: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("  Synapse count: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("  Active neuron count: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("  Firing neuron count: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  Developmental stage: " + developmentalStageToString(pImpl->developmentalStage));
}

std::string Brain::developmentalStageToString(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void Brain::saveCheckpoint(const std::string& filepath) {
    NLM_LOG_INFO("Saving brain checkpoint to: " + filepath);
    
    if (!checkpointManager) {
        NLM_THROW(NLMErrors::SystemNotInitialized, "Checkpoint manager not initialized");
    }
    
    try {
        checkpointManager->save(pImpl->regions, pImpl->interRegionConnections, filepath);
        NLM_LOG_INFO("Checkpoint saved successfully");
    } catch (const std::exception& e) {
        NLM_THROW(NLMErrors::FileIO, "Failed to save checkpoint: " + std::string(e.what()));
    }
}

bool Brain::loadCheckpoint(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain checkpoint from: " + filepath);
    
    if (!checkpointManager) {
        NLM_THROW(NLMErrors::SystemNotInitialized, "Checkpoint manager not initialized");
    }
    
    try {
        if (checkpointManager->load(pImpl->regions, pImpl->interRegionConnections, filepath)) {
            NLM_LOG_INFO("Checkpoint loaded successfully");
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading checkpoint: " + std::string(e.what()));
        return false;
    }
}

// Accessor methods
Brain* Brain::getBrain() { return this; }

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
    NLM_VALIDATE(source, source.value > 0, NLMErrors::InvalidParameter, "Source region ID must be positive");
    NLM_VALIDATE(target, target.value > 0, NLMErrors::InvalidParameter, "Target region ID must be positive");
    NLM_VALIDATE(weight, weight >= 0.0f && weight <= 1.0f, NLMErrors::InvalidParameter, "Weight must be between 0 and 1");
    NLM_VALIDATE(delay, delay >= 0.0f, NLMErrors::InvalidParameter, "Delay must be non-negative");
    
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
    if (!pImpl->episodicMemory) {
        NLM_LOG_WARNING("Episodic memory not available - returning null pointer");
    }
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

bool Brain::hasSensoryInput() const {
    return !pImpl->sensoryNeurons.empty();
}

bool Brain::hasPredictionActivity() const {
    return pImpl->predictionSystem && pImpl->predictionSystem->hasActivity();
}

bool Brain::hasAttentionActivity() const {
    return pImpl->attention && pImpl->attention->hasActivity();
}

bool Brain::hasConceptActivity() const {
    return pImpl->conceptFormation && pImpl->conceptFormation->hasActivity();
}

bool Brain::hasNoveltyActivity() const {
    return pImpl->novelty && pImpl->novelty->hasActivity();
}

bool Brain::hasCuriosityActivity() const {
    return pImpl->curiosity && pImpl->curiosity->hasInterest();
}

bool Brain::hasDopamineActivity() const {
    return pImpl->dopamine && pImpl->dopamine->getLevel() > 0.0f;
}

bool Brain::hasWorkingMemoryActivity() const {
    return pImpl->workingMemory && pImpl->workingMemory->hasActiveTraces();
}

} // namespace nlm
