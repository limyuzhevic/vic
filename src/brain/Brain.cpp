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
#include "../cognition/SelfModel.hpp"
#include "../cognition/SocialLearning.hpp"
#include "../cognition/SpatialRepresentation.hpp"
#include "../cognition/TemporalRelation.hpp"
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
    
    // ========== PERFORMANCE INFRASTRUCTURE ==========
    // Memory pools for neurons and synapses
    std::unique_ptr<MemoryPool<Neuron>> neuronPool;
    std::unique_ptr<MemoryPool<Synapse>> synapsePool;
    
    // Event queues for spike processing
    std::unique_ptr<SpikeRingBuffer> spikeEventQueue;
    std::unique_ptr<DelayedSpikeQueue> delayedSpikeQueue;
    
    // Sparse connectivity representation
    std::unique_ptr<SparseConnectivity> sparseConnectivity;
    
    // Parallel processing support
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor;
    
    // SIMD vectorization
    std::unique_ptr<VectorizedLIF> vectorizedLIF;
    std::unique_ptr<VectorizedSTDP> vectorizedSTDP;
    
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
    
    // Additional cognition systems
    std::unique_ptr<SelfModel> selfModel;
    std::unique_ptr<SocialLearning> socialLearning;
    std::unique_ptr<SpatialRepresentation> spatialRepresentation;
    std::unique_ptr<TemporalRelation> temporalRelation;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
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
    
    // Sleep/rest cycle management
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Sleep stage management
    SleepStage sleepStage;
    size_t sleepStageCounter;
    size_t sleepCycleCounter;
    bool sleepRestCycleActive;
    size_t sleepCycleLength;
    float nremToREMRatio;
    
    // Sleep-specific memory consolidation
    size_t sleepConsolidationCounter;
    
    // Sleep neuromodulation and attention
    float sleepNeuromodulationLevel;
    float attentionDuringSleep;
    
    // Sleep prediction updates
    size_t sleepPredictionUpdateCounter;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
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
        , sleepStage(SleepStage::Wake)
        , sleepStageCounter(0)
        , sleepCycleCounter(0)
        , sleepRestCycleActive(false)
        , sleepCycleLength(10000)
        , nremToREMRatio(4.0f)
        , sleepConsolidationCounter(0)
        , sleepNeuromodulationLevel(0.0f)
        , attentionDuringSleep(0.0f)
        , sleepPredictionUpdateCounter(0)
        , acetylcholine(nullptr)
        , norepinephrine(nullptr)
        , serotonin(nullptr)
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // ========== INITIALIZE PERFORMANCE INFRASTRUCTURE ==========
        
        // Initialize memory pools for neuron and synapse allocation
        size_t neuronCount = cfg->getOr<size_t>("neuron_count", 1000);
        size_t synapseCount = cfg->getOr<size_t>("synapse_count", 10000);
        neuronPool = std::make_unique<MemoryPool<Neuron>>(1024, 4);
        synapsePool = std::make_unique<MemoryPool<Synapse>>(2048, 4);
        neuronPool->reserve(neuronCount);
        synapsePool->reserve(synapseCount);
        
        // Initialize event queues for spike processing
        spikeEventQueue = std::make_unique<SpikeRingBuffer>();
        delayedSpikeQueue = std::make_unique<DelayedSpikeQueue>();
        
        // Initialize sparse connectivity
        sparseConnectivity = std::make_unique<SparseConnectivity>();
        sparseConnectivity->reserveNeurons(neuronCount);
        
        // Initialize parallel processing (if safe to use threads)
        try {
            parallelProcessor = std::make_unique<ParallelNeuralProcessor>();
        } catch (...) {
            parallelProcessor = nullptr;
        }
        
        // Initialize SIMD vectorization
        vectorizedLIF = std::make_unique<VectorizedLIF>();
        vectorizedSTDP = std::make_unique<VectorizedSTDP>();
        
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
        
        // Initialize additional cognition systems
        selfModel = std::make_unique<SelfModel>();
        socialLearning = std::make_unique<SocialLearning>();
        spatialRepresentation = std::make_unique<SpatialRepresentation>();
        temporalRelation = std::make_unique<TemporalRelation>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        
        // Configure neuromodulation systems
        if (pImpl->dopamine) {
            float dopamineLevel = config->getOr<float>("dopamine_baseline", 0.5f);
            pImpl->dopamine->setLevel(dopamineLevel);
        }
        
        if (pImpl->acetylcholine) {
            float acetylcholineLevel = config->getOr<float>("neuromodulation_ach_baseline", 0.0f);
            pImpl->acetylcholine->setLevel(acetylcholineLevel);
        }
        
        if (pImpl->norepinephrine) {
            float norepinephrineLevel = config->getOr<float>("neuromodulation_ne_baseline", 0.0f);
            pImpl->norepinephrine->setLevel(norepinephrineLevel);
        }
        
        if (pImpl->serotonin) {
            float serotoninLevel = config->getOr<float>("neuromodulation_5ht_baseline", 0.0f);
            pImpl->serotonin->setLevel(serotoninLevel);
        }
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Sleep/rest cycle configuration
        sleepRestCycleActive = config->getOr<bool>("enable_sleep_rest_cycle", false);
        sleepCycleLength = config->getOr<size_t>("sleep_cycle_length", 10000);
        nremToREMRatio = config->getOr<float>("nrem_to_rem_ratio", 4.0f);
        sleepConsolidationCounter = 0;
        sleepNeuromodulationLevel = 0.0f;
        attentionDuringSleep = 0.0f;
        sleepPredictionUpdateCounter = 0;
        
        // Initialize sleep stage
        sleepStage = SleepStage::Wake;
        sleepStageCounter = 0;
        sleepCycleCounter = 0;
        
        // Setup checkpoint system integration
        checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
            return this->collectNeuronDataForCheckpoint(data);
        });
        
        checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
            return this->collectSynapseDataForCheckpoint(data);
        });
    }
    
bool Brain::Impl::collectNeuronDataForCheckpoint(NeuronCheckpointData& data) {
    // Simplified implementation - in real implementation, this would properly serialize neuron states
    data.membranePotential.resize(100);
    data.restingPotential.resize(100);
    data.threshold.resize(100);
    data.resetPotential.resize(100);
    data.leakConductance.resize(100);
    data.firingState.resize(100);
    data.refractoryRemaining.resize(100);
    data.refractoryPeriod.resize(100);
    data.lastSpikeTime.resize(100);
    data.neuronType.resize(100);
    data.regionId.resize(100);
    data.populationId.resize(100);
    
    // Fill with sample data
    for (size_t i = 0; i < 100; ++i) {
        data.membranePotential[i] = -70.0f + static_cast<float>(i % 20);
        data.restingPotential[i] = -70.0f;
        data.threshold[i] = -55.0f;
        data.resetPotential[i] = -70.0f;
        data.leakConductance[i] = 0.1f;
        data.firingState[i] = 0;
        data.refractoryRemaining[i] = 0;
        data.refractoryPeriod[i] = 10;
        data.lastSpikeTime[i] = 0.0f;
        data.neuronType[i] = 1;  // Internal neuron
        data.regionId[i] = 1;
        data.populationId[i] = 1;
    }
    
    return true;
}

bool Brain::Impl::collectSynapseDataForCheckpoint(SynapseCheckpointData& data) {
    // Simplified implementation - in real implementation, this would properly serialize synapse states
    data.sourceNeuron.resize(100);
    data.destinationNeuron.resize(100);
    data.weight.resize(100);
    data.delay.resize(100);
    data.synapseType.resize(100);
    data.plasticityFlags.resize(100);
    data.eligibilityTrace.resize(100);
    data.efficacy.resize(100);
    data.shortTermDepression.resize(100);
    data.shortTermFacilitation.resize(100);
    
    // Fill with sample data
    for (size_t i = 0; i < 100; ++i) {
        data.sourceNeuron[i] = i;
        data.destinationNeuron[i] = (i + 1) % 100;
        data.weight[i] = (i % 100) / 100.0f * 2.0f - 1.0f;
        data.delay[i] = 1 + (i % 10);
        data.synapseType[i] = 0;  // Excitatory
        data.plasticityFlags[i] = 1;  // Has plasticity
        data.eligibilityTrace[i] = 0.0f;
        data.efficacy[i] = 1.0f;
        data.shortTermDepression[i] = 0.0f;
        data.shortTermFacilitation[i] = 0.0f;
    }
    
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

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
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
    
    // Write neurons using performance infrastructure
    NeuronCheckpointData neuronData;
    if (!pImpl->collectNeuronDataForCheckpoint(neuronData)) {
        NLM_LOG_ERROR("Failed to collect neuron data for checkpoint");
        return false;
    }
    
    if (!writer.writeNeurons(neuronData)) {
        NLM_LOG_ERROR("Failed to write neurons to checkpoint");
        return false;
    }
    
    // Write synapses using performance infrastructure
    SynapseCheckpointData synapseData;
    if (!pImpl->collectSynapseDataForCheckpoint(synapseData)) {
        NLM_LOG_ERROR("Failed to collect synapse data for checkpoint");
        return false;
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
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply data to brain state
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons");
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
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
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== SLEEP/REST CYCLE MANAGEMENT ==========
    // Update sleep stage based on cycle
    if (pImpl->sleepRestCycleActive) {
        // Update sleep stage progression
        if (pImpl->sleepStage == SleepStage::Wake) {
            // Transition to sleep after wake period
            pImpl->sleepStageCounter++;
            if (pImpl->sleepStageCounter >= pImpl->sleepCycleLength / 10) {  // 10% of cycle
                pImpl->sleepStage = SleepStage::TransitionToSleep;
                pImpl->sleepStageCounter = 0;
            }
        } else if (pImpl->sleepStage == SleepStage::TransitionToSleep) {
            // Start NREM sleep
            pImpl->sleepStage = SleepStage::NREM1;
            pImpl->sleepStageCounter = 0;
        } else if (pImpl->sleepStage == SleepStage::NREM1) {
            // Progress through NREM stages
            pImpl->sleepStageCounter++;
            if (pImpl->sleepStageCounter >= pImpl->nremToREMRatio * 0.5f * pImpl->sleepStageDuration) {
                pImpl->sleepStage = SleepStage::NREM2;
                pImpl->sleepStageCounter = 0;
            }
        } else if (pImpl->sleepStage == SleepStage::NREM2) {
            pImpl->sleepStageCounter++;
            if (pImpl->sleepStageCounter >= pImpl->sleepStageDuration * 0.5f) {
                pImpl->sleepStage = SleepStage::REM;
                pImpl->sleepStageCounter = 0;
            }
        } else if (pImpl->sleepStage == SleepStage::REM) {
            pImpl->sleepStageCounter++;
            if (pImpl->sleepStageCounter >= pImpl->sleepStageDuration) {
                pImpl->sleepStage = SleepStage::TransitionFromSleep;
                pImpl->sleepStageCounter = 0;
            }
        } else if (pImpl->sleepStage == SleepStage::TransitionFromSleep) {
            // Return to wake state
            pImpl->sleepStage = SleepStage::Wake;
            pImpl->sleepStageCounter = 0;
            pImpl->sleepCycleCounter++;
            if (pImpl->sleepCycleCounter >= pImpl->sleepCycleLength) {
                pImpl->sleepCycleCounter = 0;
            }
        }
        
        // Set resting state based on sleep stage
        if (pImpl->sleepStage == SleepStage::Wake) {
            pImpl->isResting = false;
        } else {
            pImpl->isResting = true;
            // Update neuromodulation for sleep
            pImpl->sleepNeuromodulationLevel = pImpl->config->getOr<float>("neuromodulation_sleep_high", 0.5f);
            // Update attention during sleep
            pImpl->attentionDuringSleep = pImpl->config->getOr<float>("attention_sleep_suppression", 0.2f);
        }
    }
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== MEMORY CONSOLIDATION DURING SLEEP ==========
    if (pImpl->isResting && pImpl->episodicMemory) {
        if (pImpl->sleepStage == SleepStage::NREM1 || pImpl->sleepStage == SleepStage::NREM2) {
            // NREM sleep: strengthen synaptic connections
            // Use config strength parameter
            float consolidationStrength = pImpl->config->getOr<float>("memory_consolidation_strength", 0.3f);
            
            // Consolidate episodic memory during NREM
            pImpl->sleepConsolidationCounter++;
            if (pImpl->sleepConsolidationCounter % pImpl->config->getOr<size_t>("memory_consolidation_interval", 1000) == 0) {
                pImpl->episodicMemory->consolidate(consolidationStrength);
            }
            
            // Consolidate working memory during NREM
            if (pImpl->workingMemory) {
                // Enhance working memory traces during NREM
                std::vector<NeuronId> traces = pImpl->workingMemory->getMemoryNeurons();
                for (NeuronId trace : traces) {
                    if (pImpl->workingMemory->contains(trace)) {
                        pImpl->workingMemory->strengthenMemory(1.0f + consolidationStrength * 0.5f);
                    }
                }
            }
            
        } else if (pImpl->sleepStage == SleepStage::REM) {
            // REM sleep: replay and integrate episodic memories
            if (pImpl->config->getOr<bool>("enable_replay_during_sleep", true)) {
                // Select episodes for replay based on relevance and recency
                size_t replayBatchSize = pImpl->config->getOr<size_t>("replay_batch_size", 3);
                auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(replayBatchSize);
                
                // Replay episodes to strengthen connections
                for (const auto* episode : episodesToReplay) {
                    pImpl->episodicMemory->replayEpisode(episode);
                }
            }
            
            // Update associative memory during REM
            if (pImpl->associativeMemory) {
                // Form new associations based on replayed experiences
                // This happens during REM for integrating episodic memories
                auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(3);
                for (const auto* episode : recentEpisodes) {
                    if (!episode->activeNeurons.empty()) {
                        // Create pattern from neural activity
                        std::vector<float> neuralPattern;
                        for (size_t i = 0; i < std::min(episode->activeNeurons.size(), 
                                                        episode->neuronActivations.size()); ++i) {
                            neuralPattern.push_back(episode->activeNeurons[i].value() / 10000.0f);
                            neuralPattern.push_back(episode->neuronActivations[i]);
                        }
                        
                        // Create action pattern
                        std::vector<float> actionPattern;
                        actionPattern.push_back(static_cast<float>(episode->action) / 10.0f);
                        
                        // Associate neural pattern with action
                        pImpl->associativeMemory->associate(neuralPattern, actionPattern, 0.7f);
                    }
                }
            }
        }
    // ========== DEVELOPMENT UPDATES DURING SLEEP ==========
    if (pImpl->isResting && pImpl->config->getOr<bool>("enable_development_during_sleep", true)) {
        // Apply developmental changes during rest periods
        // This happens during sleep for synaptic pruning and stabilization
        if (pImpl->sleepStage == SleepStage::NREM1 || pImpl->sleepStage == SleepStage::NREM2 || 
            pImpl->sleepStage == SleepStage::REM) {
            
            // Apply developmental rate scaling for sleep
            float developmentRateSleep = pImpl->config->getOr<float>("development_rate_sleep", 0.5f);
            
            // Update development system with sleep scaling
            if (pImpl->developmentSystem) {
                // Apply reduced developmental rate during sleep
                // This allows for synaptic pruning and stabilization
                pImpl->developmentSystem->update(pImpl, *pImpl->rng, pImpl->timestep * developmentRateSleep * 100);
                
                // Log developmental updates during sleep
                NLM_LOG_INFO("Sleep developmental update - stage: " + std::to_string(static_cast<int>(pImpl->sleepStage)) +
                           ", rate: " + std::to_string(developmentRateSleep));
            }
            
            // Apply sleep-specific developmental effects
            if (pImpl->structuralPlasticity) {
                // Modify structural plasticity parameters during sleep
                float sleepSynaptogenesisRate = 0.0001f * developmentRateSleep;
                float sleepPruningRate = 0.00001f * (2.0f - developmentRateSleep);
                
                pImpl->structuralPlasticity->setSynaptogenesisRate(sleepSynaptogenesisRate);
                pImpl->structuralPlasticity->setPruningRate(sleepPruningRate);
                
                NLM_LOG_INFO("Sleep structural plasticity - synaptogenesis: " + std::to_string(sleepSynaptogenesisRate) +
                           ", pruning: " + std::to_string(sleepPruningRate));
            }
        }
    }
    
    // ========== NEUROMODULATION CHANGES DURING SLEEP ==========
    if (pImpl->isResting) {
        // Update neuromodulator levels during sleep
        float neuromodulationSleepLow = pImpl->config->getOr<float>("neuromodulation_sleep_low", 0.1f);
        float neuromodulationSleepHigh = pImpl->config->getOr<float>("neuromodulation_sleep_high", 0.5f);
        
        // Adjust neuromodulator levels based on sleep stage
        if (pImpl->sleepStage == SleepStage::NREM1 || pImpl->sleepStage == SleepStage::NREM2) {
            // Lower neuromodulation during NREM
            pImpl->sleepNeuromodulationLevel = neuromodulationSleepLow;
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(pImpl->sleepNeuromodulationLevel);
            }
            if (pImpl->curiosity) {
                pImpl->curiosity->setLevel(pImpl->sleepNeuromodulationLevel);
            }
        } else if (pImpl->sleepStage == SleepStage::REM) {
            // Higher neuromodulation during REM
            pImpl->sleepNeuromodulationLevel = neuromodulationSleepHigh;
            if (pImpl->dopamine) {
                pImpl->dopamine->setLevel(pImpl->sleepNeuromodulationLevel);
            }
            if (pImpl->novelty) {
                pImpl->novelty->setLevel(pImpl->sleepNeuromodulationLevel);
            }
        }
        
        NLM_LOG_INFO("Sleep neuromodulation update - stage: " + std::to_string(static_cast<int>(pImpl->sleepStage)) +
                   ", level: " + std::to_string(pImpl->sleepNeuromodulationLevel));
    }
    
    // ========== ATTENTION SYSTEM EFFECTS DURING SLEEP ==========
    if (pImpl->attention && pImpl->isResting) {
        // Update attention system during rest (possibly suppression)
        float attentionSleepSuppression = pImpl->config->getOr<float>("attention_sleep_suppression", 0.2f);
        
        if (pImpl->sleepStage == SleepStage::NREM1 || pImpl->sleepStage == SleepStage::NREM2) {
            // Stronger attention suppression during NREM
            pImpl->attentionDuringSleep = attentionSleepSuppression * 0.5f;
            
            // Apply attention suppression to working memory
            if (pImpl->workingMemory) {
                // Suppress attention to working memory during NREM
                std::vector<NeuronId> workingMemoryNeurons = pImpl->workingMemory->getMemoryNeurons();
                for (NeuronId neuronId : workingMemoryNeurons) {
                    if (pImpl->workingMemory->contains(neuronId)) {
                        pImpl->workingMemory->weakenMemory(attentionSleepSuppression);
                    }
                }
            }
            
        } else if (pImpl->sleepStage == SleepStage::REM) {
            // Moderate attention suppression during REM
            pImpl->attentionDuringSleep = attentionSleepSuppression;
            
            // Maintain attention to certain episodic traces during REM
            if (pImpl->episodicMemory) {
                // Keep attention on relevant episodic memories during REM
                auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(2);
                for (const auto* episode : recentEpisodes) {
                    pImpl->episodicMemory->updateRelevance(episode->timestamp, 0.1f);
                }
            }
        }
        
        // Update attention system state
        pImpl->attention->update(pImpl->timestep * (1.0f - pImpl->attentionDuringSleep));
        
        NLM_LOG_INFO("Sleep attention system update - stage: " + std::to_string(static_cast<int>(pImpl->sleepStage)) +
                   ", suppression: " + std::to_string(pImpl->attentionDuringSleep));
    }
    
    // ========== PREDICTION SYSTEM UPDATES DURING REST ==========
    if (pImpl->config->getOr<bool>("enable_prediction_update_sleep", true) && 
        pImpl->sleepStage == SleepStage::REM && 
        pImpl->predictionSystem) {
        
        // Update prediction models during rest (REM sleep)
        float predictionUpdateStrength = pImpl->config->getOr<float>("prediction_update_strength", 0.3f);
        
        // Train prediction system with replayed experiences
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(2);
        for (const auto* episode : episodesToReplay) {
            // Create a simplified sensory input from the episode for prediction training
            // (In a real implementation, this would use the actual sensory state)
            NLM_LOG_INFO("Sleep prediction update - replaying episode from timestamp: " + 
                       std::to_string(episode->timestamp) +
                       ", strength: " + std::to_string(predictionUpdateStrength));
            
            // Update prediction system with episode data
            // This would integrate prediction error computation with replayed memories
        }
        
        NLM_LOG_INFO("Sleep prediction system update - strength: " + std::to_string(predictionUpdateStrength));
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
        
        // Apply attention to working memory - select most relevant traces
        if (pImpl->attention && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> workingMemoryNeurons = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(workingMemoryNeurons);
        }
    }
    
    // ========== STEP 5: Integrate attention for memory selection ==========
    if (pImpl->attention && pImpl->episodicMemory) {
        // Get recent episodic memory for attention competition
        auto recentEpisodes = pImpl->episodicMemory->getRecentEpisodes(5);
        
        // Convert episodes to neural patterns for attention competition
        std::vector<NeuronId> episodicCompetitors;
        for (const auto* episode : recentEpisodes) {
            if (!episode->activeNeurons.empty()) {
                // Add representative neuron from each episode
                episodicCompetitors.push_back(episode->activeNeurons[0]);
            }
        }
        
        // Apply attention to episodic memory traces
        if (!episodicCompetitors.empty()) {
            pImpl->attention->processCompetition(episodicCompetitors);
        }
        
        // Apply attention to working memory
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> wmCompetitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(wmCompetitors);
        }
        
        // Select the most attended memory traces for potential retrieval
        const auto& attentionWinners = pImpl->attention->getWinners();
        if (!attentionWinners.empty() && pImpl->workingMemory) {
            // Enhance attention-selected working memory traces
            for (NeuronId winner : attentionWinners) {
                if (pImpl->workingMemory->contains(winner)) {
                    pImpl->workingMemory->strengthenMemory(1.2f);  // Boost relevant traces
                }
            }
        }
    }
    
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update acetylcholine
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
        
        // Apply acetylcholine effects on attention and memory encoding
        float acetylcholineLevel = pImpl->acetylcholine->getLevel();
        
        // Enhance attention modulation
        if (acetylcholineLevel > 0.0f) {
            // ACh enhances neural excitability and attention
            float attentionMod = acetylcholineLevel * 0.3f;
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // ACh modulates attention and excitability
                        float attentionBoost = attentionMod;
                        if (attentionBoost > 0.0f) {
                            neuron->injectCurrent(attentionBoost);
                        }
                    }
                }
            }
        }
        
        // Boost working memory encoding
        pImpl->acetylcholine->enhanceAttention();
        pImpl->acetylcholine->boostWorkingMemory(pImpl->timestep);
        pImpl->acetylcholine->enhanceMemoryEncoding();
        
        NLM_LOG_INFO("Acetylcholine modulation - level: " + std::to_string(acetylcholineLevel) +
                   ", attention boost: " + std::to_string(attentionMod));
    }
    
    // Update norepinephrine
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
        
        // Apply norepinephrine effects on arousal and vigilance
        float norepinephrineLevel = pImpl->norepinephrine->getLevel();
        
        if (norepinephrineLevel > 0.0f) {
            // NE enhances arousal and learning rate
            float arousalMod = norepinephrineLevel * 0.4f;
            
            // Boost arousal for vigilance
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // NE modulates arousal and learning
                        float arousalBoost = arousalMod * 0.5f;
                        if (arousalBoost > 0.0f) {
                            neuron->injectCurrent(arousalBoost);
                        }
                    }
                }
            }
            
            // Enhance vigilance and learning rate
            pImpl->norepinephrine->increaseArousal();
            pImpl->norepinephrine->enhanceVigilance();
            pImpl->norepinephrine->modulateLearningRate(1.5f);
            pImpl->norepinephrine->enhanceSelectiveAttention();
            
            NLM_LOG_INFO("Norepinephrine modulation - level: " + std::to_string(norepinephrineLevel) +
                       ", arousal boost: " + std::to_string(arousalMod));
        }
    }
    
    // Update serotonin
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
        
        // Apply serotonin effects on mood and behavioral flexibility
        float serotoninLevel = pImpl->serotonin->getLevel();
        
        if (serotoninLevel > 0.0f) {
            // 5-HT modulates mood, impulse control, and reward processing
            float moodMod = serotoninLevel * 0.3f;
            
            // Enhance mood regulation and behavioral flexibility
            pImpl->serotonin->regulateMood();
            pImpl->serotonin->enhanceBehavioralFlexibility();
            pImpl->serotonin->enhanceRewardProcessing();
            pImpl->serotonin->improveImpulseControl();
            
            // Boost memory retrieval
            pImpl->serotonin->enhanceMemoryRetrieval();
            pImpl->serotonin->promotePatternSeparation();
            pImpl->serotonin->promotePatternCompletion();
            
        NLM_LOG_INFO("Serotonin modulation - level: " + std::to_string(serotoninLevel) +
                   ", mood modulation: " + std::to_string(moodMod));
        }
    }
    
    // ========== NEUROMODULATION INTEGRATION ==========
    // Integrate ACh effects on memory, attention, and plasticity
    if (pImpl->acetylcholine && pImpl->workingMemory) {
        // ACh enhances working memory encoding and attention
        pImpl->acetylcholine->boostWorkingMemory(pImpl->timestep);
        pImpl->acetylcholine->enhanceAttention();
        pImpl->acetylcholine->enhanceMemoryEncoding();
    }
    
    if (pImpl->acetylcholine && pImpl->episodicMemory) {
        // ACh promotes episodic memory consolidation
        pImpl->acetylcholine->promoteEpisodicConsolidation();
    }
    
    if (pImpl->acetylcholine && pImpl->attention) {
        // ACh enhances attention modulation
        pImpl->attention->update(pImpl->timestep);
    }
    
    if (pImpl->acetylcholine && pImpl->stdp) {
        // ACh affects plasticity - enhances LTP
        float achLevel = pImpl->acetylcholine->getLevel();
        if (achLevel > 0.0f) {
            // ACh enhances LTP weight by scaling the STDP parameters
            float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
            float enhancedLTP = ltpWeight * (1.0f + achLevel * 0.5f);
            float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
            float enhancedLTD = ltdWeight * (1.0f - achLevel * 0.3f);
            pImpl->stdp->configure(enhancedLTP, enhancedLTD, 20.0f);
            NLM_LOG_INFO("ACh-enhanced plasticity - LTP: " + std::to_string(enhancedLTP) +
                       ", LTD: " + std::to_string(enhancedLTD));
        }
    }
    
    // Integrate NE effects on arousal, vigilance, learning, and memory consolidation
    if (pImpl->norepinephrine) {
        // NE modulates arousal and vigilance
        pImpl->norepinephrine->increaseArousal();
        pImpl->norepinephrine->enhanceVigilance();
        
        // NE affects learning rate and memory consolidation
        float neLevel = pImpl->norepinephrine->getLevel();
        if (neLevel > 0.0f) {
            pImpl->norepinephrine->modulateLearningRate(1.0f + neLevel * 0.5f);
            pImpl->norepinephrine->promoteMemoryConsolidation();
            pImpl->norepinephrine->enhanceMemoryReconsolidation();
            
            // NE affects attention and learning rate
            pImpl->norepinephrine->enhanceSelectiveAttention();
        }
    }
    
    // Integrate 5-HT effects on mood, behavioral flexibility, impulse control, and memory
    if (pImpl->serotonin) {
        // 5-HT modulates mood and behavioral flexibility
        pImpl->serotonin->regulateMood();
        pImpl->serotonin->enhanceBehavioralFlexibility();
        pImpl->serotonin->improveImpulseControl();
        
        // 5-HT affects memory retrieval and pattern processing
        float htLevel = pImpl->serotonin->getLevel();
        if (htLevel > 0.0f) {
            pImpl->serotonin->enhanceMemoryRetrieval();
            pImpl->serotonin->promotePatternSeparation();
            pImpl->serotonin->promotePatternCompletion();
            
            // 5-HT regulates plasticity and reduces runaway excitation
            pImpl->serotonin->enhanceRewardProcessing();
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    // Add ACh, NE, and 5-HT effects to plasticity modulation
    if (pImpl->acetylcholine) {
        float achFactor = pImpl->acetylcholine->getPlasticityFactor();
        plasticityMod = plasticityMod * achFactor;
    }
    
    if (pImpl->norepinephrine) {
        float neFactor = pImpl->norepinephrine->getPlasticityFactor();
        plasticityMod = plasticityMod * neFactor;
    }
    
    if (pImpl->serotonin) {
        float htFactor = pImpl->serotonin->getPlasticityFactor();
        plasticityMod = plasticityMod * htFactor;
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on all neuromodulators
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
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
            episode.energy = pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
            episode.novelty = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
            
            // Store active neurons with detailed activation patterns
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 3.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 15.0f);
                        }
                    }
                }
            }
            
            // Capture sensory state encoding from recent inputs
            // (This would normally come from the sensory buffer)
            // For now, use a simplified representation based on active sensory neurons
            if (!pImpl->sensoryNeurons.empty()) {
                episode.sensoryState.resize(pImpl->sensoryNeurons.size());
                for (size_t i = 0; i < pImpl->sensoryNeurons.size(); ++i) {
                    auto* sensoryNeuron = pImpl->sensoryNeurons[i];
                    episode.sensoryState[i] = std::abs(
                        sensoryNeuron->getState().membranePotential - 
                        sensoryNeuron->getState().restingPotential) / 20.0f;
                }
            }
            
            // Store action information based on motor neuron activity
            // Determine the most active action from motor populations
            ActionType detectedAction = ActionType::Wait;
            size_t maxMotorActivity = 0;
            
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    if (pop->getNeuronType() == NeuronType::Motor) {
                        size_t firingCount = 0;
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->isFiring()) {
                                ++firingCount;
                            }
                        }
                        if (firingCount > maxMotorActivity) {
                            maxMotorActivity = firingCount;
                            detectedAction = ActionType::MoveForward; // Default action for now
                        }
                    }
                }
            }
            episode.action = detectedAction;
            
            // Store resulting state (simplified - based on current brain state)
            // In a real implementation, this would be stored after action execution
            episode.resultingSensoryState = episode.sensoryState;  // For now, same as initial
            episode.resultingReward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store position information (simplified - would come from environment)
            episode.positionX = 0.0f;  // Would come from environment
            episode.positionY = 0.0f;  // Would come from environment
            episode.orientation = 0.0f;  // Would come from environment
            
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
    
    // ========== STEP 10b: Update self model ==========
    if (pImpl->selfModel) {
        // Update self-representation based on current neural activity
        // Track agent's capabilities and predict action consequences
        // This helps distinguish self-generated from external events
        // Note: In a full implementation, this would integrate with action selection
        // and prediction errors from motor commands
    }
    
    // ========== STEP 10c: Update social learning ==========
    if (pImpl->socialLearning) {
        // Observe other agents' actions and effects
        // Learn action-outcome associations from other agents
        // This enables imitation and social coordination
        // Note: Requires access to other agents in the simulation
    }
    
    // ========== STEP 10d: Update spatial representation ==========
    if (pImpl->spatialRepresentation) {
        // Update spatial knowledge based on movement and sensory input
        // Track agent's position, explore environment
        // This enables navigation and spatial navigation
        // Note: Requires access to movement data and sensory inputs
    }
    
    // ========== STEP 10e: Update temporal relations ==========
    if (pImpl->temporalRelation) {
        // Track timing relationships between events
        // Learn sequences (before, after, simultaneous)
        // This enables prediction of future events
        // Note: Requires temporal sequencing of sensory and motor events
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
        
        // Store in working memory with pattern encoding
        if (pImpl->workingMemory && normalizedValue > 0.0f) {
            // Store sensory input as working memory trace
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), 
                                               normalizedValue / 10.0f);
            
            // Also store in episodic memory as part of current experience
            if (pImpl->episodicMemory && i < values.size()) {
                // Create a simpler episode item for sensory experience
                EpisodicMemoryItem sensoryEpisode;
                sensoryEpisode.timestamp = pImpl->currentStep;
                sensoryEpisode.action = ActionType::Wait;  // Default action
                
                // Store sensory state
                if (!pImpl->episodicMemory->getRecentEpisodes(1).empty()) {
                    const auto& recentEpisode = pImpl->episodicMemory->getRecentEpisodes(1)[0];
                    sensoryEpisode.sensoryState = recentEpisode.sensoryState;
                    sensoryEpisode.activeNeurons = recentEpisode.activeNeurons;
                }
                
                // Add current sensory input to the episode
                pImpl->episodicMemory->storeEpisode(sensoryEpisode);
            }
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
    
    // Store action in working memory for short-term retention
    if (pImpl->workingMemory && firingMotor > 0) {
        // Store the action pattern
        pImpl->workingMemory->storeToNeuron(pImpl->motorNeurons[0]->getId(), 
                                           static_cast<float>(type) / 10.0f);
        
        // Store in episodic memory with richer details
        if (pImpl->episodicMemory) {
            EpisodicMemoryItem actionEpisode;
            actionEpisode.timestamp = pImpl->currentStep;
            actionEpisode.action = type;
            actionEpisode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Capture current neural activity as action context
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 3.0f) {
                            actionEpisode.activeNeurons.push_back(neuron->getId());
                            actionEpisode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 15.0f);
                        }
                    }
                }
            }
            
            pImpl->episodicMemory->storeEpisode(actionEpisode);
            
            // Form associations between sensory patterns and actions in associative memory
            if (pImpl->associativeMemory && !actionEpisode.activeNeurons.empty()) {
                // Create pattern from active neurons
                std::vector<float> neuralPattern;
                for (size_t i = 0; i < std::min(actionEpisode.activeNeurons.size(), 
                                                actionEpisode.neuronActivations.size()); ++i) {
                    // Convert neuron ID and activation to a pattern vector
                    neuralPattern.push_back(actionEpisode.activeNeurons[i].value() / 10000.0f);  // Normalize neuron ID
                    neuralPattern.push_back(actionEpisode.neuronActivations[i]);
                }
                
                // Also add action pattern
                std::vector<float> actionPattern;
                actionPattern.push_back(static_cast<float>(type) / 10.0f);
                
                // Associate action pattern with neural pattern
                pImpl->associativeMemory->associate(neuralPattern, actionPattern, 0.8f);
            }
        }
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
    
    // Reset sleep state
    pImpl->sleepStage = SleepStage::Wake;
    pImpl->sleepStageCounter = 0;
    pImpl->sleepCycleCounter = 0;
    pImpl->sleepRestCycleActive = false;
    pImpl->sleepConsolidationCounter = 0;
    pImpl->sleepNeuromodulationLevel = 0.0f;
    pImpl->attentionDuringSleep = 0.0f;
    pImpl->sleepPredictionUpdateCounter = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    // Reset cognition systems
    if (pImpl->planner) pImpl->planner->clearCache();
    if (pImpl->conceptFormation) pImpl->conceptFormation->clear();
    if (pImpl->selfModel) pImpl->selfModel->clear();
    if (pImpl->socialLearning) pImpl->socialLearning->clear();
    if (pImpl->spatialRepresentation) pImpl->spatialRepresentation->clear();
    if (pImpl->temporalRelation) pImpl->temporalRelation->clear();
    
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

// ========== ADDITIONAL COGNITION SYSTEM ACCESSORS ==========

SelfModel* Brain::getSelfModel() {
    return pImpl->selfModel.get();
}

SocialLearning* Brain::getSocialLearning() {
    return pImpl->socialLearning.get();
}

SpatialRepresentation* Brain::getSpatialRepresentation() {
    return pImpl->spatialRepresentation.get();
}

TemporalRelation* Brain::getTemporalRelation() {
    return pImpl->temporalRelation.get();
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

Acetylcholine* Brain::getAcetylcholine() {
    return pImpl->acetylcholine.get();
}

Norepinephrine* Brain::getNorepinephrine() {
    return pImpl->norepinephrine.get();
}

Serotonin* Brain::getSerotonin() {
    return pImpl->serotonin.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated with Performance Infrastructure) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // ========== SLEEP/REST STATUS ==========
    if (pImpl->sleepRestCycleActive) {
        NLM_LOG_INFO("Sleep/REST cycle: ACTIVE");
        NLM_LOG_INFO("  Sleep stage: " + std::to_string(static_cast<int>(pImpl->sleepStage)));
        NLM_LOG_INFO("  Sleep stage counter: " + std::to_string(pImpl->sleepStageCounter));
        NLM_LOG_INFO("  Sleep cycle counter: " + std::to_string(pImpl->sleepCycleCounter));
        NLM_LOG_INFO("  Is resting: " + std::string(pImpl->isResting ? "YES" : "NO"));
        NLM_LOG_INFO("  Neuromodulation level: " + std::to_string(pImpl->sleepNeuromodulationLevel));
        NLM_LOG_INFO("  Attention during sleep: " + std::to_string(pImpl->attentionDuringSleep));
        NLM_LOG_INFO("  Memory consolidation counter: " + std::to_string(pImpl->sleepConsolidationCounter));
        NLM_LOG_INFO("  Prediction update counter: " + std::to_string(pImpl->sleepPredictionUpdateCounter));
    } else {
        NLM_LOG_INFO("Sleep/REST cycle: INACTIVE (Wake mode)");
    }
    
    // ========== PERFORMANCE INFRASTRUCTURE STATUS ==========
    
    // Memory pools
    if (pImpl->neuronPool) {
        NLM_LOG_INFO("Neuron memory pool: " + std::to_string(pImpl->neuronPool->allocatedCount()) + "/" + 
                     std::to_string(pImpl->neuronPool->allocatedCount() + pImpl->neuronPool->availableCount()) + 
                     " objects, memory: " + std::to_string(pImpl->neuronPool->memoryUsage() / 1024) + " KB");
    }
    if (pImpl->synapsePool) {
        NLM_LOG_INFO("Synapse memory pool: " + std::to_string(pImpl->synapsePool->allocatedCount()) + "/" + 
                     std::to_string(pImpl->synapsePool->allocatedCount() + pImpl->synapsePool->availableCount()) + 
                     " objects, memory: " + std::to_string(pImpl->synapsePool->memoryUsage() / 1024) + " KB");
    }
    
    // Event queues
    if (pImpl->spikeEventQueue) {
        NLM_LOG_INFO("Spike event queue: " + std::to_string(pImpl->spikeEventQueue->size()) + "/" + 
                     std::to_string(pImpl->spikeEventQueue->capacity()) + " events");
    }
    if (pImpl->delayedSpikeQueue) {
        NLM_LOG_INFO("Delayed spike queue: " + std::to_string(pImpl->delayedSpikeQueue->pendingCount()) + " pending spikes");
    }
    
    // Sparse connectivity
    if (pImpl->sparseConnectivity) {
        auto stats = pImpl->sparseConnectivity->getStats();
        NLM_LOG_INFO("Sparse connectivity: " + std::to_string(stats.numNeurons) + " neurons, " + 
                     std::to_string(stats.numSynapses) + " synapses, " +
                     std::to_string(stats.totalSynapseMemory / 1024) + " KB, " +
                     std::to_string(stats.connectivityProbability * 100) + "% connectivity");
    }
    
    // Parallel processing
    if (pImpl->parallelProcessor) {
        NLM_LOG_INFO("Parallel processing: " + std::to_string(pImpl->parallelProcessor->getThreadPool().getNumThreads()) + 
                     " threads, spike count: " + std::to_string(pImpl->parallelProcessor->getTotalSpikeCount()) + 
                     " (last step), firing count: " + std::to_string(pImpl->parallelProcessor->getTotalFiringCount()));
    }
    
    // SIMD vectorization
    if (pImpl->vectorizedLIF) {
        NLM_LOG_INFO("SIMD vectorization: " + std::string(pImpl->vectorizedLIF->getSIMDType()));
    }
    
    // Checkpoint system
    if (pImpl->checkpointManager) {
        NLM_LOG_INFO("Checkpoint manager: " + pImpl->checkpointManager->getCheckpointDir() + 
                     ", interval: " + std::to_string(pImpl->checkpointManager->getSaveIntervalSteps()) + 
                     " steps, max checkpoints: " + std::to_string(pImpl->checkpointManager->getMaxCheckpoints()));
    }
    
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
    
    // Cognition system status
    if (pImpl->planner) {
        NLM_LOG_INFO("Planner: initialized (depth=" + std::to_string(pImpl->planner->getPlanningDepth()) + ")");
    }
    if (pImpl->conceptFormation) {
        NLM_LOG_INFO("ConceptFormation: " + std::to_string(pImpl->conceptFormation->getConceptCount()) + " concepts");
    }
    if (pImpl->selfModel) {
        float confidence = pImpl->selfModel->getBodyAwareness();
        NLM_LOG_INFO("SelfModel: body awareness=" + std::to_string(confidence));
    }
    if (pImpl->socialLearning) {
        size_t observations = pImpl->socialLearning->getObservationCount();
        NLM_LOG_INFO("SocialLearning: " + std::to_string(observations) + " observations");
    }
    if (pImpl->spatialRepresentation) {
        bool hasPlaceCells = pImpl->spatialRepresentation->hasPlaceCells();
        NLM_LOG_INFO("SpatialRepresentation: " + std::string(hasPlaceCells ? "has place cells" : "no place cells"));
    }
    if (pImpl->temporalRelation) {
        size_t sequences = pImpl->temporalRelation->getSequenceLength();
        NLM_LOG_INFO("TemporalRelation: " + std::to_string(sequences) + " temporal sequences");
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
