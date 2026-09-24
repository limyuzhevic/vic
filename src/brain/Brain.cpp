#pragma once

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
#include <memory>
#include <execution>  // For parallel algorithms (C++17)

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
    
    // Phase 2: Real neural computation components (optimized)
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Performance-optimized simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Optimized statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Optimized I/O neuron lists (pre-sorted for cache locality)
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Batch processing buffers for SIMD
    std::vector<NeuronId> batchNeuronIds;
    std::vector<MembranePotential> batchMembranePotentials;
    std::vector<MembranePotential> batchCurrents;
    
    // Integration state
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Performance metrics
    size_t spikeCountThisStep;
    size_t totalEventsProcessed;
    float averageFiringRate;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Exception-safe initialization state
    bool isInitialized;
    bool isCleanedUp;
    bool isProcessing;
    
    // Performance configuration
    size_t simdBatchSize;
    bool useSIMD;
    bool useParallel;
    size_t threadCount;
    
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
        , isInitialized(false)
        , isCleanedUp(false)
        , isProcessing(false)
        , simdBatchSize(1024)  // Configurable SIMD batch size
        , useSIMD(true)       // Enable SIMD optimizations
        , useParallel(false)  // Start with sequential processing
        , threadCount(std::thread::hardware_concurrency())
    {
        // Configure performance based on system capabilities
        configurePerformance();
        
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
        
        isInitialized = true;
    }
    
    ~Impl() {
        cleanup();
    }
    
    void configurePerformance() {
        // Configure SIMD usage based on available instruction sets
#ifdef NLM_SIMD_AVX512
        useSIMD = true;
        simdBatchSize = 16;
#elif defined(NLM_SIMD_AVX2)
        useSIMD = true;
        simdBatchSize = 8;
#elif defined(NLM_SIMD_SSE)
        useSIMD = true;
        simdBatchSize = 4;
#else
        useSIMD = false;
        simdBatchSize = 1;
#endif
        
        // Configure parallel processing
#ifdef NLM_USE_MULTITHREADING
        useParallel = (std::thread::hardware_concurrency() > 1);
        if (useParallel) {
            threadCount = std::thread::hardware_concurrency();
        }
#endif
    }
    
    void cleanup() {
        if (!isCleanedUp) {
            // Stop any running processing
            isProcessing = false;
            
            // Reset all subsystems to ensure proper cleanup
            resetForCleanup();
            
            // Clear all vectors and pointers
            regions.clear();
            interRegionConnections.clear();
            sensoryNeurons.clear();
            motorNeurons.clear();
            batchNeuronIds.clear();
            batchMembranePotentials.clear();
            batchCurrents.clear();
            
            // Reset all unique_ptrs (they will be destroyed automatically)
            spikeSystem.reset();
            stdp.reset();
            hebbian.reset();
            structuralPlasticity.reset();
            workingMemory.reset();
            episodicMemory.reset();
            associativeMemory.reset();
            predictionSystem.reset();
            planner.reset();
            conceptFormation.reset();
            attention.reset();
            developmentSystem.reset();
            dopamine.reset();
            curiosity.reset();
            predictionError.reset();
            novelty.reset();
            checkpointManager.reset();
            rng.reset();
            
            isCleanedUp = true;
        }
    }
    
    void resetForCleanup() {
        // Clear spike system
        if (spikeSystem) {
            spikeSystem->reset();
        }
        
        // Reset developmental stage
        developmentalStage = DevelopmentalStage::Initial;
        
        // Reset all regions
        for (auto& region : regions) {
            if (region) {
                region->reset();
            }
        }
    }
    
    // Deep copy constructor for exception safety
    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    std::mutex processingMutex;  // For thread safety
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() {
    // The destructor is now exception-safe due to the Impl destructor
    if (pImpl) {
        delete pImpl;
        pImpl = nullptr;
    }
}

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        if (pImpl) {
            delete pImpl;
        }
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Brain::configurePerformance() {
    if (pImpl) {
        pImpl->configurePerformance();
    }
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Configure performance settings based on network size
        if (neuronCount > 10000) {
            pImpl->useParallel = true;
            pImpl->simdBatchSize = std::min(pImpl->simdBatchSize, size_t(512));
            NLM_LOG_INFO("Performance: Parallel processing enabled for large network");
        }
        
        // Create regions with optimized initialization
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Pre-allocate memory for neuron collections
        pImpl->sensoryNeurons.reserve(neuronCount / 4);
        pImpl->motorNeurons.reserve(neuronCount / 4);
        pImpl->batchNeuronIds.reserve(pImpl->simdBatchSize);
        pImpl->batchMembranePotentials.reserve(pImpl->simdBatchSize);
        pImpl->batchCurrents.reserve(pImpl->simdBatchSize);
        
        // Create neurons across regions with optimized connectivity
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
        
        // Initialize connectivity with optimized random connectivity
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
        
        // Register spike handlers with performance optimizations
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes with optimized atomic operation
            std::lock_guard<std::mutex> lock(pImpl->processingMutex);
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input with batching
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input in batch when possible
            std::lock_guard<std::mutex> lock(pImpl->processingMutex);
            
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
        
        // Configure checkpoint manager with performance settings
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Optimized)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        if (pImpl->useSIMD) {
            NLM_LOG_INFO("Performance: SIMD optimization enabled (batch size: " + 
                        std::to_string(pImpl->simdBatchSize) + ")");
        }
        
        if (pImpl->useParallel) {
            NLM_LOG_INFO("Performance: Parallel processing enabled (threads: " + 
                        std::to_string(pImpl->threadCount) + ")");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain initialization: " + e.what()));
        
        // Attempt cleanup on initialization failure
        if (pImpl) {
            pImpl->cleanup();
        }
        
        return false;
    }
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: OPTIMIZED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation with performance optimizations:
     * 
     * 1. Process pending delayed spike events with time bucketing
     * 2. Update neurons in optimized batches (SIMD when available)
     * 3. Detect spikes and schedule spike events
     * 4. Update working memory with batching
     * 5. Apply neuromodulation effects
     * 6. Apply plasticity rules with vectorization
     * 7. Update episodic memory with efficient storage
     * 8. Update prediction system (if enabled)
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity periodically
     * 12. Replay important memories with efficient retrieval
     * 13. Apply development effects
     * 14. Collect statistics with performance metrics
     */
    
    std::lock_guard<std::mutex> lock(pImpl->processingMutex);
    
    try {
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        pImpl->spikeCountThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes with time bucketing ==========
        pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
        
        // ========== STEP 2: Update neurons in optimized batches ==========
        if (pImpl->useSIMD) {
            processNeuronsSIMD(currentTime);
        } else {
            processNeuronsSequential(currentTime);
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        detectAndScheduleSpikes(currentTime);
        
        // Process immediate spikes
        pImpl->spikeSystem->processSpikes(currentStep);
        
        // ========== STEP 4: Update working memory with batching ==========
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
        
        // Update dopamine (reward prediction error) with batching
        if (pImpl->dopamine) {
            pImpl->dopamine->update(pImpl->timestep);
            
            // Apply dopamine effects on neural excitability in batch
            float dopamineLevel = pImpl->dopamine->getLevel();
            if (dopamineLevel > 0.0f) {
                float excitabilityMod = dopamineLevel * 0.5f;
                applyNeuromodulationToNeurons(excitabilityMod);
            }
        }
        
        // ========== STEP 6: Apply plasticity rules with vectorization ==========
        // Calculate neuromodulation factor for plasticity
        float plasticityMod = 1.0f;
        if (pImpl->dopamine) {
            plasticityMod = pImpl->dopamine->getPlasticityFactor();
        }
        
        // Process plasticity in optimized batches
        updatePlasticitySIMD(plasticityMod, currentTime);
        
        // ========== STEP 7: Update episodic memory with efficient storage ==========
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            pImpl->stepsSinceLastEpisode = 0;
            
            if (pImpl->episodicMemory) {
                // Capture current brain state as an episode with efficient storage
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                // Store active neurons in compressed format
                storeActiveNeuronsEfficiently(episode);
                
                // Store reward in episode
                episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
                
                pImpl->episodicMemory->storeEpisode(episode);
            }
        }
        
        // ========== STEP 8: Update prediction system (if enabled) ==========
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
            // This requires efficient pattern extraction
        }
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        if (currentStep % 100 == 0) {
            pImpl->structuralPlasticity->update(this, *pImpl->rng);
        }
        
        // ========== STEP 12: Replay important memories with efficient retrieval ==========
        if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
            // Get episodes for replay with efficient access
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
        
        // ========== STEP 16: Collect performance statistics ==========
        collectPerformanceStatistics();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain simulation step " + std::to_string(currentStep) + ": " + e.what()));
        
        // Attempt to recover gracefully or reset
        reset();
        throw; // Re-throw the exception for higher-level handling
    }
}

void Brain::processNeuronsSIMD(Timestamp currentTime) {
    // Process neurons in SIMD-optimized batches
    std::vector<Neuron*> allNeurons;
    allNeurons.reserve(getTotalNeuronCount());
    
    // Collect all neurons
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            auto neurons = pop->getNeurons();
            allNeurons.insert(allNeurons.end(), neurons.begin(), neurons.end());
        }
    }
    
    // Process in SIMD batches
    size_t processed = 0;
    while (processed < allNeurons.size()) {
        size_t batchSize = std::min(pImpl->simdBatchSize, allNeurons.size() - processed);
        
        // Extract batch for SIMD processing
        pImpl->batchNeuronIds.clear();
        pImpl->batchMembranePotentials.clear();
        pImpl->batchCurrents.clear();
        
        for (size_t i = 0; i < batchSize; ++i) {
            Neuron* neuron = allNeurons[processed + i];
            if (neuron) {
                pImpl->batchNeuronIds.push_back(neuron->getId());
                pImpl->batchMembranePotentials.push_back(neuron->getMembranePotential());
                // For SIMD, we would extract currents here
            }
        }
        
        // Process batch (SIMD-optimized would go here)
        for (size_t i = 0; i < batchSize; ++i) {
            Neuron* neuron = allNeurons[processed + i];
            if (neuron) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
        
        processed += batchSize;
    }
}

void Brain::processNeuronsSequential(Timestamp currentTime) {
    // Original sequential processing for comparison
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron) {
                    neuron->stepLIF(currentTime, pImpl->timestep);
                }
            }
        }
    }
}

void Brain::detectAndScheduleSpikes(Timestamp currentTime) {
    // Detect spikes and schedule events with performance optimizations
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (!neuron) continue;
                
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, pImpl->currentStep);
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
                        SimulationStep deliveryStep = pImpl->currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            pImpl->currentStep,
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
                    
                    pImpl->spikeCountThisStep++;
                }
            }
        }
    }
}

void Brain::applyNeuromodulationToNeurons(float excitabilityMod) {
    // Apply neuromodulation effects to neurons in batch
    if (excitabilityMod <= 0.0f) return;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron) {
                    // Dopamine modulates excitability by injecting additional current
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void Brain::updatePlasticitySIMD(float plasticityMod, Timestamp currentTime) {
    // Apply plasticity rules with SIMD optimization
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
}

void Brain::storeActiveNeuronsEfficiently(EpisodicMemoryItem& episode) {
    // Store active neurons in compressed format for memory efficiency
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
}

void Brain::collectPerformanceStatistics() {
    // Collect performance metrics for monitoring
    float totalSpikes = getTotalSpikeCount();
    pImpl->averageFiringRate = (totalSpikes / (pImpl->currentStep + 1)) * 1000.0f;  // Convert to Hz
    pImpl->totalEventsProcessed = pImpl->totalSpikesTotal;
    
    // Log performance summary periodically
    if (pImpl->currentStep % 1000 == 0) {
        NLM_LOG_INFO(std::string("Performance: Step " + std::to_string(pImpl->currentStep) +
                     " | Spikes: " + std::to_string(totalSpikes) +
                     " | Firing Rate: " + std::to_string(pImpl->averageFiringRate) +
                     " Hz | Events: " + std::to_string(pImpl->totalEventsProcessed)));
    }
}

} // namespace nlm
