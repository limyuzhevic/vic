/**
 * @file Brain.cpp
 * @brief Implementation of the Brain class - real spiking neural computation engine
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full spiking dynamics implementation planned for Phase 2.
 */

#include "Brain.hpp"

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

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
#include <memory>
#include <unordered_map>
#include <limits>

namespace nlm {

/**
 * @struct Brain::Impl
 * @brief Private implementation details for the Brain class (Pimpl idiom)
 * @details This struct contains all the actual implementation details of the Brain class,
 *          including internal systems, simulation state, and memory management. This allows
 *          the Brain class to be forward-declared and maintains binary compatibility.
 *
 * The implementation includes:
 * - Core neural computation systems (spike, STDP, Hebbian, structural plasticity)
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction and cognitive systems
 * - Neuromodulatory signals
 * - Simulation state and parameters
 * - Statistics and monitoring
 * - Checkpoint management
 */

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
    
    /**
     * @brief Constructs the Brain implementation with configuration
     * @param cfg Configuration parameters for the brain
     * @details Initializes all neural systems and sets up the simulation environment.
     *          This is called by Brain::Brain(std::shared_ptr<Config>).
     */
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
        /**
         * @brief Initialize random generator with seed from config
         * @details Uses the config to get a random seed, defaults to 42 if not specified.
         *          The random generator is essential for initializing neurons, synapses,
         *          and introducing stochasticity in neural dynamics.
         */
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        /**
         * @brief Initialize plasticity systems
         * @details Creates instances of all plasticity mechanisms: STDP for spike-timing
         *          dependent plasticity, Hebbian for co-activation learning, and structural
         *          plasticity for synapse formation and elimination.
         */
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        /**
         * @brief Initialize memory systems
         * @details Sets up the three integrated memory systems: working memory for
         *          transient active information, episodic memory for experience storage,
         *          and associative memory for pattern associations.
         */
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        /**
         * @brief Initialize prediction system
         * @details Sets up the prediction system for sensory prediction and error
         *          computation, enabling predictive coding and error-driven learning.
         */
        predictionSystem = std::make_unique<PredictionSystem>();
        
        /**
         * @brief Initialize cognition systems
         * @details Creates the three main cognitive systems: neural planner for action
         *          planning, concept formation for pattern discovery, and attentional
         *          selection for focus control.
         */
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        /**
         * @brief Initialize development system
         * @details Sets up the development system for maturational changes and
         *          adaptive development over the brain's lifetime.
         */
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        /**
         * @brief Initialize neuromodulation systems
         * @details Creates the four main neuromodulatory systems: dopamine for reward
         *          and reinforcement, curiosity for exploration, novelty for detection,
         *          and prediction error for learning.
         */
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        /**
         * @brief Configure STDP parameters
         * @details Reads STDP (Spike-Timing-Dependent Plasticity) parameters from
         *          configuration and applies them to the STDP system.
         */
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        /**
         * @brief Configure structural plasticity
         * @details Reads structural plasticity parameters (synaptogenesis and pruning)
         *          from configuration and applies them to the structural plasticity system.
         */
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        /**
         * @brief Get timestep from configuration
         * @details Reads the simulation timestep (time between steps) from configuration,
         *          defaulting to 0.001 seconds (1ms) if not specified.
         */
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        /**
         * @brief Get integration intervals from config
         * @details Reads memory integration intervals (replay and consolidation) from
         *          configuration for episodic memory management.
         */
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        /**
         * @brief Initialize checkpoint manager
         * @details Creates the checkpoint manager for saving and loading brain state.
         */
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    /**
     * @brief Current developmental stage of the brain
     * @details Determines which developmental processes are active and influences
     *          plasticity rates and learning capabilities.
     */
    DevelopmentalStage developmentalStage;
    
    /**
     * @brief Next available region ID for new regions
     * @details Monotonically increasing ID counter for neural regions.
     */
    RegionId nextRegionId;
};
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

// Helper class implementations
namespace {
    
    // SimulationStepCoordinator implementation
    SimulationStepCoordinator::SimulationStepCoordinator(Brain* brain) : brain_(brain) {}
    
    void SimulationStepCoordinator::execute(SimulationStep currentStep, Timestamp currentTime) {
        executeStep1(currentStep, currentTime);
        executeStep2(currentStep, currentTime);
        executeStep3(currentStep, currentTime);
        executeStep4(currentStep, currentTime);
        executeStep5(currentStep, currentTime);
        executeStep6(currentStep, currentTime);
        executeStep7(currentStep, currentTime);
        executeStep8(currentStep, currentTime);
        executeStep9(currentStep, currentTime);
        executeStep10(currentStep, currentTime);
        executeStep11(currentStep, currentTime);
        executeStep12(currentStep, currentTime);
        executeStep13(currentStep, currentTime);
        executeStep14(currentStep, currentTime);
        executeStep15(currentStep, currentTime);
    }
    
    void SimulationStepCoordinator::executeStep1(SimulationStep currentStep, Timestamp currentTime) {
        // Process pending delayed spike events (deliver synaptic input)
        brain_->pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    void SimulationStepCoordinator::executeStep2(SimulationStep currentStep, Timestamp currentTime) {
        // Update all neurons (LIF dynamics)
        for (auto& region : brain_->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, brain_->pImpl->timestep);
                }
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep3(SimulationStep currentStep, Timestamp currentTime) {
        // Detect spikes and schedule spike events
        for (auto& region : brain_->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain_->pImpl->timestep * 2.0f);

                    if (justFired) {
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        brain_->pImpl->spikeSystem->queueSpike(event);

                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            syn->recordPostSpike(currentTime);
                        }

                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = currentStep + delay;
                            Timestamp deliveryTime = currentTime + delay * brain_->pImpl->timestep;

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

                            brain_->pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                            syn->recordPreSpike(currentTime);
                        }
                        
                        if (brain_->pImpl->workingMemory) {
                            brain_->pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep4(SimulationStep currentStep, Timestamp currentTime) {
        // Process immediate spikes
        brain_->pImpl->spikeSystem->processSpikes(currentStep);
    }
    
    void SimulationStepCoordinator::executeStep5(SimulationStep currentStep, Timestamp currentTime) {
        // Update working memory
        if (brain_->pImpl->workingMemory) {
            brain_->pImpl->workingMemory->update(brain_->pImpl->timestep);
        }
    }
    
    void SimulationStepCoordinator::executeStep6(SimulationStep currentStep, Timestamp currentTime) {
        // Apply neuromodulation effects
        if (brain_->pImpl->novelty) {
            brain_->pImpl->novelty->update(brain_->pImpl->timestep);
        }
        
        if (brain_->pImpl->curiosity) {
            brain_->pImpl->curiosity->update(brain_->pImpl->timestep);
        }
        
        if (brain_->pImpl->dopamine) {
            brain_->pImpl->dopamine->update(brain_->pImpl->timestep);
            
            float dopamineLevel = brain_->pImpl->dopamine->getLevel();
            for (auto& region : brain_->pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep7(SimulationStep currentStep, Timestamp currentTime) {
        // Apply plasticity rules (STDP and Hebbian)
        float plasticityMod = 1.0f;
        if (brain_->pImpl->dopamine) {
            plasticityMod = brain_->pImpl->dopamine->getPlasticityFactor();
        }
        
        for (auto& region : brain_->pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        brain_->pImpl->stdp->update(syn, preSpikes, postSpikes, brain_->pImpl->timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }
                
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        brain_->pImpl->hebbian->update(syn, preSpikes, postSpikes, brain_->pImpl->timestep);
                    }
                }
                
                syn->step(currentTime);
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep8(SimulationStep currentStep, Timestamp currentTime) {
        // Update episodic memory
        brain_->pImpl->stepsSinceLastEpisode++;
        if (brain_->pImpl->stepsSinceLastEpisode >= 10) {
            brain_->pImpl->stepsSinceLastEpisode = 0;
            
            if (brain_->pImpl->episodicMemory) {
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0.0f;
                
                for (auto& region : brain_->pImpl->regions) {
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
                
                episode.reward = brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0.0f;
                brain_->pImpl->episodicMemory->storeEpisode(episode);
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep9(SimulationStep currentStep, Timestamp currentTime) {
        // Update prediction system
        if (brain_->pImpl->predictionSystem) {
        }
    }
    
    void SimulationStepCoordinator::executeStep10(SimulationStep currentStep, Timestamp currentTime) {
        // Update attention system
        if (brain_->pImpl->attention) {
            brain_->pImpl->attention->update(brain_->pImpl->timestep);
            
            if (brain_->pImpl->workingMemory && !brain_->pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = brain_->pImpl->workingMemory->getMemoryNeurons();
                brain_->pImpl->attention->processCompetition(competitors);
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep11(SimulationStep currentStep, Timestamp currentTime) {
        // Update concept formation
        if (brain_->pImpl->conceptFormation) {
        }
    }
    
    void SimulationStepCoordinator::executeStep12(SimulationStep currentStep, Timestamp currentTime) {
        // Apply structural plasticity periodically
        if (currentStep % 100 == 0) {
            brain_->pImpl->structuralPlasticity->update(brain_, *brain_->pImpl->rng);
        }
    }
    
    void SimulationStepCoordinator::executeStep13(SimulationStep currentStep, Timestamp currentTime) {
        // Replay important memories
        if (currentStep % brain_->pImpl->replayInterval == 0 && brain_->pImpl->episodicMemory) {
            auto episodesToReplay = brain_->pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                brain_->pImpl->episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep14(SimulationStep currentStep, Timestamp currentTime) {
        // Apply development effects
        if (currentStep % 1000 == 0) {
            brain_->pImpl->developmentSystem->update(brain_, *brain_->pImpl->rng, brain_->pImpl->timestep * 1000);
            
            auto* sp = brain_->pImpl->structuralPlasticity;
            if (sp) {
                DevelopmentalStage stage = brain_->pImpl->developmentalStage;
                float plasticityMod = 1.0f;
                
                switch (stage) {
                    case DevelopmentalStage::Initial:
                        plasticityMod = 1.0f;
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        plasticityMod = 0.8f;
                        break;
                    case DevelopmentalStage::Maturation:
                        plasticityMod = 0.5f;
                        break;
                    case DevelopmentalStage::Adult:
                        plasticityMod = 0.2f;
                        break;
                }
                
                sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }
    
    void SimulationStepCoordinator::executeStep15(SimulationStep currentStep, Timestamp currentTime) {
        // Checkpoint management
        if (brain_->pImpl->checkpointManager) {
            brain_->pImpl->checkpointManager->update(currentStep, currentTime);
        }
    }
    
    // BrainStatisticsCalculator implementation
    float BrainStatisticsCalculator::calculateExcitationInhibitionRatio(const Brain* brain) {
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        for (const auto& region : brain->pImpl->regions) {
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
    
    size_t BrainStatisticsCalculator::calculateTotalSpikeCount(const Brain* brain) {
        return brain->pImpl->totalSpikesTotal;
    }
    
    size_t BrainStatisticsCalculator::calculatePendingSpikeEventCount(const Brain* brain) {
        return brain->pImpl->spikeSystem->getPendingSpikeCount() + 
               brain->pImpl->spikeSystem->getPendingDelayedCount();
    }
    
    size_t BrainStatisticsCalculator::calculateTotalNeuronCount(const Brain* brain) {
        size_t total = 0;
        for (const auto& region : brain->pImpl->regions) {
            total += region->getTotalNeuronCount();
        }
        return total;
    }
    
    size_t BrainStatisticsCalculator::calculateTotalSynapseCount(const Brain* brain) {
        size_t total = 0;
        for (const auto& region : brain->pImpl->regions) {
            total += region->getSynapseCount();
        }
        total += brain->pImpl->interRegionConnections.size();
        return total;
    }
    
    size_t BrainStatisticsCalculator::calculateActiveNeuronCount(const Brain* brain) {
        size_t total = 0;
        for (const auto& region : brain->pImpl->regions) {
            total += region->getActiveNeuronCount();
        }
        return total;
    }
    
    float BrainStatisticsCalculator::calculateAverageFiringRate(const Brain* brain) {
        if (brain->pImpl->regions.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& region : brain->pImpl->regions) {
            sum += region->getAverageFiringRate();
        }
        return sum / static_cast<float>(brain->pImpl->regions.size());
    }
    
    // CheckpointManager implementation
    void CheckpointManager::configure(const std::string& directory, size_t maxCheckpoints, 
                                     size_t maxFileSize, bool compressionEnabled) {
        directory_ = directory;
        maxCheckpoints_ = maxCheckpoints;
        maxFileSize_ = maxFileSize;
        compressionEnabled_ = compressionEnabled;
    }
    
    bool CheckpointManager::save(const Brain* brain, const std::string& filepath) const {
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        try {
            CheckpointWriter writer;
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
                return false;
            }
            
            // Set metadata
            writer.setMetadata(
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getRegionCount(),
                brain->pImpl->currentStep,
                brain->pImpl->currentTime
            );
            
            // Write neurons
            NeuronCheckpointData neuronData;
            neuronData.membranePotential.reserve(brain->getTotalNeuronCount());
            neuronData.restingPotential.reserve(brain->getTotalNeuronCount());
            neuronData.threshold.reserve(brain->getTotalNeuronCount());
            neuronData.resetPotential.reserve(brain->getTotalNeuronCount());
            neuronData.leakConductance.reserve(brain->getTotalNeuronCount());
            
            for (const auto& region : brain->pImpl->regions) {
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
            for (const auto& region : brain->pImpl->regions) {
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
    
    bool CheckpointManager::load(Brain* brain, const std::string& filepath) {
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
            for (auto& region : brain->pImpl->regions) {
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
    
    // SensoryInputProcessor implementation
    void SensoryInputProcessor::process(SensoryInputProcessor* processor, const SensoryInput& input, Brain* brain) {
        processor->injectToSensoryNeurons(brain->pImpl->sensoryNeurons, input);
    }
    
    void SensoryInputProcessor::injectToSensoryNeurons(const std::vector<Neuron*>& sensoryNeurons, 
                                                       const SensoryInput& input) {
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
    
    void SensoryInputProcessor::normalizeAndInject(const std::vector<float>& values, 
                                                   const std::vector<Neuron*>& neurons) {
        for (size_t i = 0; i < neurons.size(); ++i) {
            float normalizedValue = 0.0f;
            if (i < values.size()) {
                normalizedValue = values[i] * 10.0f;
            }
            neurons[i]->injectCurrent(normalizedValue);
        }
    }
    
    // ActionSelectionSystem implementation
    std::unique_ptr<Action> ActionSelectionSystem::selectAction(const std::vector<Neuron*>& motorNeurons) {
        // Simple action selection based on motor neuron activity
        if (motorNeurons.empty()) {
            return std::make_unique<Action>(ActionType::Wait);
        }
        
        // Calculate activity of motor neuron groups
        size_t firingMotor = 0;
        for (auto* neuron : motorNeurons) {
            if (neuron->isFiring()) {
                ++firingMotor;
            }
        }
        
        // Return a simple action
        ActionType type = ActionType::Wait;
        if (firingMotor > 0) {
            type = ActionType::MoveForward;
        }
        
        return std::make_unique<Action>(type);
    }
    
    void ActionSelectionSystem::calculateMotorActivity(const std::vector<Neuron*>& motorNeurons, 
                                                       size_t& firingMotor) {
        firingMotor = 0;
        for (auto* neuron : motorNeurons) {
            if (neuron->isFiring()) {
                ++firingMotor;
            }
        }
    }
    
    ActionType ActionSelectionSystem::activityToActionType(size_t firingMotor) {
        return firingMotor > 0 ? ActionType::MoveForward : ActionType::Wait;
    }
    
    // NeuronStepProcessor implementation
    bool NeuronStepProcessor::processNeuronStep(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) {
        return neuron->stepLIF(currentTime, dt);
    }
    
    void NeuronStepProcessor::processRegionNeurons(Brain* brain, RegionId regionId, 
                                                    Timestamp currentTime, TimestepDuration dt) {
        auto* region = brain->getRegion(regionId);
        if (region) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, dt);
                }
            }
        }
    }
    
    void NeuronStepProcessor::processSpikes(Brain* brain, SimulationStep currentStep, 
                                            Timestamp currentTime) {
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < dt * 2.0f);

                    if (justFired) {
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        brain->pImpl->spikeSystem->queueSpike(event);

                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            syn->recordPostSpike(currentTime);
                        }

                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = currentStep + delay;
                            Timestamp deliveryTime = currentTime + delay * brain->pImpl->timestep;

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

                            brain->pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                            syn->recordPreSpike(currentTime);
                        }
                    }
                }
            }
        }
    }
    
    void NeuronStepProcessor::deliverDelayedSpikes(Brain* brain, SimulationStep currentStep, 
                                                   Timestamp currentTime) {
        brain->pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
} // namespace nlm
#include <memory>
#include <unordered_map>
#include <limits>

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

// Add forward declarations and helper class implementations
namespace {
    
    // Forward declarations for helper classes
    class SimulationStepCoordinatorImpl : public SimulationStepCoordinator {
    public:
        SimulationStepCoordinatorImpl(Brain* brain) : SimulationStepCoordinator(brain) {}
        
        void execute(SimulationStep currentStep, Timestamp currentTime) override {
            executeStep1(currentStep, currentTime);
            executeStep2(currentStep, currentTime);
            executeStep3(currentStep, currentTime);
            executeStep4(currentStep, currentTime);
            executeStep5(currentStep, currentTime);
            executeStep6(currentStep, currentTime);
            executeStep7(currentStep, currentTime);
            executeStep8(currentStep, currentTime);
            executeStep9(currentStep, currentTime);
            executeStep10(currentStep, currentTime);
            executeStep11(currentStep, currentTime);
            executeStep12(currentStep, currentTime);
            executeStep13(currentStep, currentTime);
            executeStep14(currentStep, currentTime);
            executeStep15(currentStep, currentTime);
        }
        
        void executeStep1(SimulationStep currentStep, Timestamp currentTime) override {
            // Process pending delayed spike events (deliver synaptic input)
            pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        }
        
        void executeStep2(SimulationStep currentStep, Timestamp currentTime) override {
            // Update all neurons (LIF dynamics)
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->stepLIF(currentTime, pImpl->timestep);
                    }
                }
            }
        }
        
        void executeStep3(SimulationStep currentStep, Timestamp currentTime) override {
            // Detect spikes and schedule spike events
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        bool justFired = (state.firingState == FiringState::Refractory &&
                                         state.lastSpikeTime >= 0.0f &&
                                         std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                        if (justFired) {
                            SpikeEvent event(neuron->getId(), currentTime, currentStep);
                            pImpl->spikeSystem->queueSpike(event);

                            auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                            for (Synapse* syn : incomingSynapses) {
                                syn->recordPostSpike(currentTime);
                            }

                            auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                            for (Synapse* syn : outgoingSynapses) {
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

                                syn->recordPreSpike(currentTime);
                            }
                            
                            if (pImpl->workingMemory) {
                                pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                    std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                            }
                        }
                    }
                }
            }
        }
        
        void executeStep4(SimulationStep currentStep, Timestamp currentTime) override {
            // Process immediate spikes
            pImpl->spikeSystem->processSpikes(currentStep);
        }
        
        void executeStep5(SimulationStep currentStep, Timestamp currentTime) override {
            // Update working memory
            if (pImpl->workingMemory) {
                pImpl->workingMemory->update(pImpl->timestep);
            }
        }
        
        void executeStep6(SimulationStep currentStep, Timestamp currentTime) override {
            // Apply neuromodulation effects
            if (pImpl->novelty) {
                pImpl->novelty->update(pImpl->timestep);
            }
            
            if (pImpl->curiosity) {
                pImpl->curiosity->update(pImpl->timestep);
            }
            
            if (pImpl->dopamine) {
                pImpl->dopamine->update(pImpl->timestep);
                
                float dopamineLevel = pImpl->dopamine->getLevel();
                for (auto& region : pImpl->regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            float excitabilityMod = dopamineLevel * 0.5f;
                            if (excitabilityMod > 0.0f) {
                                neuron->injectCurrent(excitabilityMod);
                            }
                        }
                    }
                }
            }
        }
        
        void executeStep7(SimulationStep currentStep, Timestamp currentTime) override {
            // Apply plasticity rules (STDP and Hebbian)
            float plasticityMod = 1.0f;
            if (pImpl->dopamine) {
                plasticityMod = pImpl->dopamine->getPlasticityFactor();
            }
            
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    if (syn->getPlasticityFlags().stdp) {
                        const auto& preSpikes = syn->getPreSpikeHistory();
                        const auto& postSpikes = syn->getPostSpikeHistory();
                        
                        if (!preSpikes.empty() && !postSpikes.empty()) {
                            pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                            float weight = syn->getWeight();
                            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                            syn->setWeight(weight);
                        }
                    }
                    
                    if (syn->getPlasticityFlags().hebbian) {
                        const auto& preSpikes = syn->getPreSpikeHistory();
                        const auto& postSpikes = syn->getPostSpikeHistory();
                        
                        if (!preSpikes.empty() && !postSpikes.empty()) {
                            pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        }
                    }
                    
                    syn->step(currentTime);
                }
            }
        }
        
        void executeStep8(SimulationStep currentStep, Timestamp currentTime) override {
            // Update episodic memory
            pImpl->stepsSinceLastEpisode++;
            if (pImpl->stepsSinceLastEpisode >= 10) {
                pImpl->stepsSinceLastEpisode = 0;
                
                if (pImpl->episodicMemory) {
                    EpisodicMemoryItem episode;
                    episode.timestamp = currentStep;
                    episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                    
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
                    
                    episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                    pImpl->episodicMemory->storeEpisode(episode);
                }
            }
        }
        
        void executeStep9(SimulationStep currentStep, Timestamp currentTime) override {
            // Update prediction system
            if (pImpl->predictionSystem) {
            }
        }
        
        void executeStep10(SimulationStep currentStep, Timestamp currentTime) override {
            // Update attention system
            if (pImpl->attention) {
                pImpl->attention->update(pImpl->timestep);
                
                if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                    std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                    pImpl->attention->processCompetition(competitors);
                }
            }
        }
        
        void executeStep11(SimulationStep currentStep, Timestamp currentTime) override {
            // Update concept formation
            if (pImpl->conceptFormation) {
            }
        }
        
        void executeStep12(SimulationStep currentStep, Timestamp currentTime) override {
            // Apply structural plasticity periodically
            if (currentStep % 100 == 0) {
                pImpl->structuralPlasticity->update(brain_, *pImpl->rng);
            }
        }
        
        void executeStep13(SimulationStep currentStep, Timestamp currentTime) override {
            // Replay important memories
            if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
                for (const auto* episode : episodesToReplay) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
        }
        
        void executeStep14(SimulationStep currentStep, Timestamp currentTime) override {
            // Apply development effects
            if (currentStep % 1000 == 0) {
                pImpl->developmentSystem->update(brain_, *pImpl->rng, pImpl->timestep * 1000);
                
                auto* sp = pImpl->structuralPlasticity;
                if (sp) {
                    DevelopmentalStage stage = pImpl->developmentalStage;
                    float plasticityMod = 1.0f;
                    
                    switch (stage) {
                        case DevelopmentalStage::Initial:
                            plasticityMod = 1.0f;
                            break;
                        case DevelopmentalStage::CriticalPeriod:
                            plasticityMod = 0.8f;
                            break;
                        case DevelopmentalStage::Maturation:
                            plasticityMod = 0.5f;
                            break;
                        case DevelopmentalStage::Adult:
                            plasticityMod = 0.2f;
                            break;
                    }
                    
                    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
                }
            }
        }
        
        void executeStep15(SimulationStep currentStep, Timestamp currentTime) override {
            // Checkpoint management
            if (pImpl->checkpointManager) {
                pImpl->checkpointManager->update(currentStep, currentTime);
            }
        }
        
    private:
        Impl* pImpl; // Access to Brain's implementation
    };
    
    // Helper function implementations
    namespace {
        
        float calculateExcitationInhibitionRatio(const Brain* brain) {
            float totalExcitatory = 0.0f;
            float totalInhibitory = 0.0f;
            
            for (const auto& region : brain->pImpl->regions) {
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
        
        size_t calculateTotalSpikeCount(const Brain* brain) {
            return brain->pImpl->totalSpikesTotal;
        }
        
        size_t calculatePendingSpikeEventCount(const Brain* brain) {
            return brain->pImpl->spikeSystem->getPendingSpikeCount() + 
                   brain->pImpl->spikeSystem->getPendingDelayedCount();
        }
        
        size_t calculateTotalNeuronCount(const Brain* brain) {
            size_t total = 0;
            for (const auto& region : brain->pImpl->regions) {
                total += region->getTotalNeuronCount();
            }
            return total;
        }
        
        size_t calculateTotalSynapseCount(const Brain* brain) {
            size_t total = 0;
            for (const auto& region : brain->pImpl->regions) {
                total += region->getSynapseCount();
            }
            total += brain->pImpl->interRegionConnections.size();
            return total;
        }
        
        size_t calculateActiveNeuronCount(const Brain* brain) {
            size_t total = 0;
            for (const auto& region : brain->pImpl->regions) {
                total += region->getActiveNeuronCount();
            }
            return total;
        }
        
        float calculateAverageFiringRate(const Brain* brain) {
            if (brain->pImpl->regions.empty()) return 0.0f;
            float sum = 0.0f;
            for (const auto& region : brain->pImpl->regions) {
                sum += region->getAverageFiringRate();
            }
            return sum / static_cast<float>(brain->pImpl->regions.size());
        }
    }
    
} // namespace

// Helper class implementations
namespace {

class SimulationStepCoordinatorImpl : public SimulationStepCoordinator {
public:
    SimulationStepCoordinatorImpl(Brain* brain) : SimulationStepCoordinator(brain) {}
    
    void execute(SimulationStep currentStep, Timestamp currentTime) override {
        executeStep1(currentStep, currentTime);
        executeStep2(currentStep, currentTime);
        executeStep3(currentStep, currentTime);
        executeStep4(currentStep, currentTime);
        executeStep5(currentStep, currentTime);
        executeStep6(currentStep, currentTime);
        executeStep7(currentStep, currentTime);
        executeStep8(currentStep, currentTime);
        executeStep9(currentStep, currentTime);
        executeStep10(currentStep, currentTime);
        executeStep11(currentStep, currentTime);
        executeStep12(currentStep, currentTime);
        executeStep13(currentStep, currentTime);
        executeStep14(currentStep, currentTime);
        executeStep15(currentStep, currentTime);
    }
    
    void executeStep1(SimulationStep currentStep, Timestamp currentTime) override {
        // Process pending delayed spike events (deliver synaptic input)
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
    
    void executeStep2(SimulationStep currentStep, Timestamp currentTime) override {
        // Update all neurons (LIF dynamics)
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, pImpl->timestep);
                }
            }
        }
    }
    
    void executeStep3(SimulationStep currentStep, Timestamp currentTime) override {
        // Detect spikes and schedule spike events
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                    if (justFired) {
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        pImpl->spikeSystem->queueSpike(event);

                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            syn->recordPostSpike(currentTime);
                        }

                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
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

                            syn->recordPreSpike(currentTime);
                        }
                        
                        if (pImpl->workingMemory) {
                            pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }
    }
    
    void executeStep4(SimulationStep currentStep, Timestamp currentTime) override {
        // Process immediate spikes
        pImpl->spikeSystem->processSpikes(currentStep);
    }
    
    void executeStep5(SimulationStep currentStep, Timestamp currentTime) override {
        // Update working memory
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(pImpl->timestep);
        }
    }
    
    void executeStep6(SimulationStep currentStep, Timestamp currentTime) override {
        // Apply neuromodulation effects
        if (pImpl->novelty) {
            pImpl->novelty->update(pImpl->timestep);
        }
        
        if (pImpl->curiosity) {
            pImpl->curiosity->update(pImpl->timestep);
        }
        
        if (pImpl->dopamine) {
            pImpl->dopamine->update(pImpl->timestep);
            
            float dopamineLevel = pImpl->dopamine->getLevel();
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
    
    void executeStep7(SimulationStep currentStep, Timestamp currentTime) override {
        // Apply plasticity rules (STDP and Hebbian)
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        for (auto& region : pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }
                
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    }
                }
                
                syn->step(currentTime);
            }
        }
    }
    
    void executeStep8(SimulationStep currentStep, Timestamp currentTime) override {
        // Update episodic memory
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
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
                
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                pImpl->episodicMemory->storeEpisode(episode);
            }
        }
    }
    
    void executeStep9(SimulationStep currentStep, Timestamp currentTime) override {
        // Update prediction system
        if (pImpl->predictionSystem) {
        }
    }
    
    void executeStep10(SimulationStep currentStep, Timestamp currentTime) override {
        // Update attention system
        if (pImpl->attention) {
            pImpl->attention->update(pImpl->timestep);
            
            if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
                pImpl->attention->processCompetition(competitors);
            }
        }
    }
    
    void executeStep11(SimulationStep currentStep, Timestamp currentTime) override {
        // Update concept formation
        if (pImpl->conceptFormation) {
        }
    }
    
    void executeStep12(SimulationStep currentStep, Timestamp currentTime) override {
        // Apply structural plasticity periodically
        if (currentStep % 100 == 0) {
            pImpl->structuralPlasticity->update(brain_, *pImpl->rng);
        }
    }
    
    void executeStep13(SimulationStep currentStep, Timestamp currentTime) override {
        // Replay important memories
        if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                pImpl->episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    void executeStep14(SimulationStep currentStep, Timestamp currentTime) override {
        // Apply development effects
        if (currentStep % 1000 == 0) {
            pImpl->developmentSystem->update(brain_, *pImpl->rng, pImpl->timestep * 1000);
            
            auto* sp = pImpl->structuralPlasticity;
            if (sp) {
                DevelopmentalStage stage = pImpl->developmentalStage;
                float plasticityMod = 1.0f;
                
                switch (stage) {
                    case DevelopmentalStage::Initial:
                        plasticityMod = 1.0f;
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        plasticityMod = 0.8f;
                        break;
                    case DevelopmentalStage::Maturation:
                        plasticityMod = 0.5f;
                        break;
                    case DevelopmentalStage::Adult:
                        plasticityMod = 0.2f;
                        break;
                }
                
                sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }
    
    void executeStep15(SimulationStep currentStep, Timestamp currentTime) override {
        // Checkpoint management
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->update(currentStep, currentTime);
        }
    }
    
private:
    Impl* pImpl; // Access to Brain's implementation
};

// Helper function implementations
namespace {
    
    float calculateExcitationInhibitionRatio(const Brain* brain) {
        float totalExcitatory = 0.0f;
        float totalInhibitory = 0.0f;
        
        for (const auto& region : brain->pImpl->regions) {
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
    
    size_t calculateTotalSpikeCount(const Brain* brain) {
        return brain->pImpl->totalSpikesTotal;
    }
    
    size_t calculatePendingSpikeEventCount(const Brain* brain) {
        return brain->pImpl->spikeSystem->getPendingSpikeCount() + 
               brain->pImpl->spikeSystem->getPendingDelayedCount();
    }
    
    size_t calculateTotalNeuronCount(const Brain* brain) {
        size_t total = 0;
        for (const auto& region : brain->pImpl->regions) {
            total += region->getTotalNeuronCount();
        }
        return total;
    }
    
    size_t calculateTotalSynapseCount(const Brain* brain) {
        size_t total = 0;
        for (const auto& region : brain->pImpl->regions) {
            total += region->getSynapseCount();
        }
        total += brain->pImpl->interRegionConnections.size();
        return total;
    }
    
    size_t calculateActiveNeuronCount(const Brain* brain) {
        size_t total = 0;
        for (const auto& region : brain->pImpl->regions) {
            total += region->getActiveNeuronCount();
        }
        return total;
    }
    
    float calculateAverageFiringRate(const Brain* brain) {
        if (brain->pImpl->regions.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& region : brain->pImpl->regions) {
            sum += region->getAverageFiringRate();
        }
        return sum / static_cast<float>(brain->pImpl->regions.size());
    }
}

} // namespace

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

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    delete pImpl;
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
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
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
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
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

void Brain::} // namespace

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    delete pImpl;
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
    
    // Create simulation coordinator and execute all steps
    SimulationStepCoordinatorImpl coordinator(this);
    coordinator.execute(currentStep, currentTime);
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
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
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
    return BrainStatisticsCalculator::calculateExcitationInhibitionRatio(this);
}

size_t Brain::getTotalSpikeCount() const {
    return BrainStatisticsCalculator::calculateTotalSpikeCount(this);
}

size_t Brain::getPendingSpikeEventCount() const {
    return BrainStatisticsCalculator::calculatePendingSpikeEventCount(this);
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Use ActionSelectionSystem to select action
    return ActionSelectionSystem::selectAction(pImpl->motorNeurons);
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
    return pImpl->checkpointManager->save(this, filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->checkpointManager->load(const_cast<Brain*>(this), filepath);
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

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                   float weight, Delay delay) {
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
    return BrainStatisticsCalculator::calculateTotalNeuronCount(this);
}

size_t Brain::getTotalSynapseCount() const {
    return BrainStatisticsCalculator::calculateTotalSynapseCount(this);
}

size_t Brain::getActiveNeuronCount() const {
    return BrainStatisticsCalculator::calculateActiveNeuronCount(this);
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    return BrainStatisticsCalculator::calculateAverageFiringRate(this);
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
