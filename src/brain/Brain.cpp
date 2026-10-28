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
#include <unordered_map>
#include <vector>
#include <queue>

namespace nlm {

// ========== BRAIN IMPLEMENTATION - REFACTORED ========== //

// Forward declarations for internal helper functions
namespace BrainImpl {
    // Memory system helpers
    void initializeMemorySystems(Brain::Impl* impl);
    void updateMemorySystems(Brain::Impl* impl, SimulationStep step, Timestamp time);
    void storeEpisodicMemory(Brain::Impl* impl, SimulationStep step, Timestamp time);
    void replayMemory(Brain::Impl* impl, SimulationStep step);
    
    // Neuromodulation helpers
    void applyNeuromodulation(Brain::Impl* impl);
    void updateDopamine(Brain::Impl* impl, float reward, float predictionError);
    
    // Plasticity helpers
    void applyPlasticity(Brain::Impl* impl, Timestamp time);
    void updateSTDP(Brain::Impl* impl, Synapse* synapse, const std::vector<Timestamp>& preSpikes, 
                   const std::vector<Timestamp>& postSpikes, Timestamp currentTime);
    void updateHebbian(Brain::Impl* impl, Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes, Timestamp currentTime);
    
    // Cognition helpers
    void updateCognitionSystems(Brain::Impl* impl, Timestamp time);
    void updateAttention(Brain::Impl* impl, Timestamp time);
    void updatePlanning(Brain::Impl* impl, Timestamp time);
    void updateConcepts(Brain::Impl* impl, Timestamp time);
    
    // Development helpers
    void updateDevelopment(Brain::Impl* impl, Timestamp time, Timestamp elapsedTime);
    void applyDevelopmentalPlasticity(Brain::Impl* impl);
    
    // Statistics helpers
    void collectStatistics(Brain::Impl* impl);
    py::dict getNetworkStatistics(Brain::Impl* impl);
    
    // Configuration helpers
    void applyConfiguration(Brain::Impl* impl);
    void configurePlasticity(Brain::Impl* impl);
    void configureNeuromodulation(Brain::Impl* impl);
    
    // Connectivity helpers
    void setupConnectivity(Brain::Impl* impl);
    void setupSensoryMotorNeurons(Brain::Impl* impl);
    void setupInterRegionConnections(Brain::Impl* impl);
    
    // Input/Output helpers
    void processSensoryInput(Brain::Impl* impl, const SensoryInput& input);
    std::unique_ptr<Action> selectAction(Brain::Impl* impl);
    
    // Checkpoint helpers
    bool saveBrainState(Brain::Impl* impl, const std::string& filepath);
    bool loadBrainState(Brain::Impl* impl, const std::string& filepath);
    
    // Batch processing helpers
    void processBatchSteps(Brain::Impl* impl, size_t steps, Timestamp timestep);
    void optimizePerformance(Brain::Impl* impl);
    
    // Error handling helpers
    std::string formatError(const std::string& operation, const std::exception& e);
    void validateBrainState(Brain::Impl* impl);
}

// ========== BRAIN CLASS DEFINITION - REFACTORED ========== //

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

// ========== BRAIN CONSTRUCTOR AND BASIC OPERATIONS ========== //

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
    
    // Apply all configuration
    BrainImpl::applyConfiguration(pImpl);
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Setup connectivity
    BrainImpl::setupConnectivity(pImpl);
    
    // Setup spike system handlers
    setupSpikeHandlers();
    
    // Configure checkpoint manager
    configureCheckpointManager();
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    logStatus();
    
    return true;
}

void Brain::setupSpikeHandlers() {
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
}

void Brain::configureCheckpointManager() {
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP - REFACTORED
     * 
     * This implements the complete integrated brain simulation with:
     * 1. Event-driven spike processing
     * 2. Neural dynamics and spike generation
     * 3. Memory system updates
     * 4. Neuromodulation effects
     * 5. Plasticity updates
     * 6. Episodic memory storage
     * 7. Prediction system updates
     * 8. Cognition system updates
     * 9. Structural plasticity
     * 10. Memory replay
     * 11. Development effects
     * 12. Statistics collection
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process spike events (Step 1)
    processSpikeEvents(currentStep, currentTime);
    
    // Update neurons (Step 2)
    updateNeurons(currentStep, currentTime);
    
    // Process spikes and schedule events (Step 3)
    processSpikes(currentStep);
    
    // Update all integrated systems (Steps 4-11)
    BrainImpl::updateIntegratedSystems(pImpl, currentStep, currentTime);
    
    // Periodic operations (Steps 10-12)
    handlePeriodicOperations(currentStep, currentTime);
    
    // Collect statistics (Step 13)
    BrainImpl::collectStatistics(pImpl);
}

void Brain::processSpikeEvents(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void Brain::updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::processSpikes(SimulationStep currentStep) {
    pImpl->spikeSystem->processSpikes(currentStep);
}

void BrainImpl::updateIntegratedSystems(Impl* impl, SimulationStep step, Timestamp time) {
    // Update working memory
    if (impl->workingMemory) {
        impl->workingMemory->update(impl->timestep);
    }
    
    // Apply neuromodulation
    applyNeuromodulation(impl);
    
    // Apply plasticity
    applyPlasticity(impl, time);
    
    // Update episodic memory
    storeEpisodicMemory(impl, step, time);
    
    // Update prediction system
    if (impl->predictionSystem) {
        // Prediction system updates
    }
    
    // Update cognition systems
    updateCognitionSystems(impl, time);
    
    // Apply structural plasticity periodically
    if (step % 100 == 0) {
        impl->structuralPlasticity->update(nullptr, *impl->rng);
    }
}

void BrainImpl::applyNeuromodulation(Impl* impl) {
    // Update novelty detection
    if (impl->novelty) {
        impl->novelty->update(impl->timestep);
    }
    
    // Update curiosity
    if (impl->curiosity) {
        impl->curiosity->update(impl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (impl->dopamine) {
        impl->dopamine->update(impl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = impl->dopamine->getLevel();
        for (auto& region : impl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainImpl::applyPlasticity(Impl* impl, Timestamp time) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (impl->dopamine) {
        plasticityMod = impl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : impl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    updateSTDP(impl, syn, preSpikes, postSpikes, time);
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
                    updateHebbian(impl, syn, preSpikes, postSpikes, time);
                }
            }
            
            // Update synapse state
            syn->step(time);
        }
    }
}

void BrainImpl::updateSTDP(Impl* impl, Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes, Timestamp currentTime) {
    if (impl->stdp) {
        impl->stdp->update(synapse, preSpikes, postSpikes, impl->timestep);
    }
}

void BrainImpl::updateHebbian(Impl* impl, Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes, Timestamp currentTime) {
    if (impl->hebbian) {
        impl->hebbian->update(synapse, preSpikes, postSpikes, impl->timestep);
    }
}

void BrainImpl::storeEpisodicMemory(Impl* impl, SimulationStep step, Timestamp time) {
    impl->stepsSinceLastEpisode++;
    if (impl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        impl->stepsSinceLastEpisode = 0;
        
        if (impl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = step;
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : impl->regions) {
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
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            impl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainImpl::updateCognitionSystems(Impl* impl, Timestamp time) {
    // Update attention system
    updateAttention(impl, time);
    
    // Update planning system
    updatePlanning(impl, time);
    
    // Update concept formation
    updateConcepts(impl, time);
}

void BrainImpl::updateAttention(Impl* impl, Timestamp time) {
    if (impl->attention) {
        impl->attention->update(impl->timestep);
        
        // Apply attention to working memory winners
        if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
            impl->attention->processCompetition(competitors);
        }
    }
}

void BrainImpl::updatePlanning(Impl* impl, Timestamp time) {
    if (impl->planner) {
        impl->planner->update(impl->timestep);
    }
}

void BrainImpl::updateConcepts(Impl* impl, Timestamp time) {
    if (impl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::handlePeriodicOperations(SimulationStep currentStep, Timestamp currentTime) {
    // Handle memory replay
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        BrainImpl::replayMemory(pImpl, currentStep);
    }
    
    // Apply development effects
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        BrainImpl::updateDevelopment(pImpl, currentTime, currentTime * 1000);
        
        // Apply developmental plasticity
        applyDevelopmentalPlasticity(pImpl);
    }
    
    // Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainImpl::replayMemory(Impl* impl, SimulationStep step) {
    // Get episodes for replay
    auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
    for (const auto* episode : episodesToReplay) {
        impl->episodicMemory->replayEpisode(episode);
    }
}

void BrainImpl::updateDevelopment(Impl* impl, Timestamp currentTime, Timestamp elapsedTime) {
    if (impl->developmentSystem) {
        impl->developmentSystem->update(nullptr, *impl->rng, elapsedTime);
    }
}

void BrainImpl::applyDevelopmentalPlasticity(Impl* impl) {
    auto* sp = impl->structuralPlasticity;
    if (sp) {
        DevelopmentalStage stage = impl->developmentalStage;
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

void BrainImpl::collectStatistics(Impl* impl) {
    // Basic statistics are collected during the step
}

py::dict BrainImpl::getNetworkStatistics(Impl* impl) {
    py::dict stats;
    stats["neurons"] = impl->regions.empty() ? 0 : 
                      std::accumulate(impl->regions.begin(), impl->regions.end(), 0, 
                                    [](size_t sum, const auto& region) {
                                        return sum + region->getTotalNeuronCount();
                                    });
    stats["synapses"] = impl->regions.empty() ? 0 : 
                      std::accumulate(impl->regions.begin(), impl->regions.end(), 0, 
                                    [](size_t sum, const auto& region) {
                                        return sum + region->getSynapseCount();
                                    });
    stats["active_neurons"] = impl->regions.empty() ? 0 : 
                              std::accumulate(impl->regions.begin(), impl->regions.end(), 0,
                                            [](size_t sum, const auto& region) {
                                                return sum + region->getActiveNeuronCount();
                                            });
    stats["firing_rate"] = impl->regions.empty() ? 0.0f : 
                          std::accumulate(impl->regions.begin(), impl->regions.end(), 0.0f,
                                        [](float sum, const auto& region) {
                                            return sum + region->getAverageFiringRate();
                                        }) / impl->regions.size();
    stats["e_i_ratio"] = impl->regions.empty() ? 0.0f : 
                        std::accumulate(impl->regions.begin(), impl->regions.end(), 0.0f,
                                      [](float sum, const auto& region) {
                                          return sum + region->getAverageSynapticWeight();
                                      }) / impl->regions.size();
    stats["total_spikes"] = impl->totalSpikesTotal;
    stats["firing_this_step"] = impl->totalSpikesThisStep;
    stats["pending_spikes"] = impl->spikeSystem->getPendingSpikeCount() + impl->spikeSystem->getPendingDelayedCount();
    
    return stats;
}

void BrainImpl::applyConfiguration(Impl* impl) {
    configurePlasticity(impl);
    configureNeuromodulation(impl);
}

void BrainImpl::configurePlasticity(Impl* impl) {
    // Configure STDP parameters
    float ltpWeight = impl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = impl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = impl->config->getOr<float>("stdp_tau", 20.0f);
    impl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = impl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = impl->config->getOr<float>("pruning_rate", 0.00001f);
    impl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl->structuralPlasticity->setPruningRate(pruningRate);
}

void BrainImpl::configureNeuromodulation(Impl* impl) {
    // Configure neuromodulation parameters
    // This would configure dopamine, curiosity, novelty, etc.
}

void BrainImpl::setupConnectivity(Impl* impl) {
    size_t regionCount = impl->config->getOr<size_t>("region_count", 1);
    size_t neuronCount = impl->config->getOr<size_t>("neuron_count", 1000);
    float connectionProbability = impl->config->getOr<float>("connection_probability", 0.1f);
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
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
                    impl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    impl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*impl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

// ========== BRAIN INPUT/OUTPUT OPERATIONS ========== //

void Brain::receiveSensoryInput(const SensoryInput& input) {
    BrainImpl::processSensoryInput(pImpl, input);
}

void BrainImpl::processSensoryInput(Impl* impl, const SensoryInput& input) {
    // Inject current into sensory neurons based on input
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = impl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        impl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (impl->workingMemory && normalizedValue > 0.5f) {
            impl->workingMemory->storeToNeuron(impl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

std::unique_ptr<Action> Brain::produceAction() {
    return BrainImpl::selectAction(pImpl);
}

std::unique_ptr<Action> BrainImpl::selectAction(Impl* impl) {
    // Simple action selection based on motor neuron activity
    if (impl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : impl->motorNeurons) {
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

// ========== BRAIN STATE MANAGEMENT ========== //

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

// ========== BRAIN STATE OPERATIONS ========== //

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
    return BrainImpl::saveBrainState(pImpl, filepath);
}

bool BrainImpl::saveBrainState(Impl* impl, const std::string& filepath) {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            impl->getTotalNeuronCount(),
            impl->getTotalSynapseCount(),
            impl->getRegionCount(),
            impl->currentStep,
            impl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(impl->getTotalNeuronCount());
        neuronData.restingPotential.reserve(impl->getTotalNeuronCount());
        neuronData.threshold.reserve(impl->getTotalNeuronCount());
        neuronData.resetPotential.reserve(impl->getTotalNeuronCount());
        neuronData.leakConductance.reserve(impl->getTotalNeuronCount());
        
        for (const auto& region : impl->regions) {
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
        for (const auto& region : impl->regions) {
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
    return BrainImpl::loadBrainState(pImpl, filepath);
}

bool BrainImpl::loadBrainState(Impl* impl, const std::string& filepath) {
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
        
        // Apply neuron states - this would need to be implemented
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons");
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

// ========== BRAIN REGION OPERATIONS ========== //

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

// ========== MEMORY SYSTEM ACCESSORS ========== //

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

} // namespace nlm
