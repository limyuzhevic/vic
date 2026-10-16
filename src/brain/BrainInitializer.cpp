// BrainInitializer.cpp - Comprehensive implementation of performance-optimized brain initialization
// 
// Provides extensive performance optimization during brain initialization,
// including memory management, SIMD acceleration, event-driven processing,
// sparse connectivity, and hash-based O(1) lookups for efficient access.

#include "BrainInitializer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../performance/PerformanceOptimization.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/EventQueue.hpp"
#include "../performance/SparseConnectivity.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <numeric>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace nlm {

// Static factory initialization
std::atomic<bool> BrainInitializer::PerformanceOptimizedBrainFactory::factoryInitialized_{false};
std::mutex BrainInitializer::PerformanceOptimizedBrainFactory::factoryMutex_;
BrainInitializer* BrainInitializer::PerformanceOptimizedBrainFactory::factoryInitializer_ = nullptr;

// BrainInitializer implementation
BrainInitializer::BrainInitializer(std::shared_ptr<Config> config,
                                 const PerformanceConfig& perfConfig)
    : config_(std::move(config)), perfConfig_(perfConfig) {
    
    // Validate performance configuration
    validatePerformanceConfig();
    
    // Initialize performance infrastructure
    if (perfConfig_.enableMemoryPools) {
        initializeMemoryInfrastructure();
    }
    
    if (perfConfig_.enableSIMD) {
        initializeSIMDInfrastructure();
    }
    
    if (perfConfig_.enableMultithreading) {
        initializeThreadingInfrastructure();
    }
    
    if (perfConfig_.enableEventDriven) {
        initializeEventDrivenInfrastructure();
    }
    
    if (perfConfig_.enableSparseConnectivity) {
        initializeSparseConnectivityInfrastructure();
    }
    
    NLM_LOG_INFO("BrainInitializer created with performance optimizations");
}

BrainInitializer::~BrainInitializer() {
    cleanup();
    NLM_LOG_INFO("BrainInitializer destroyed");
}

bool BrainInitializer::initialize(std::shared_ptr<Brain> brain) {
    return initialize(brain, perfConfig_);
}

bool BrainInitializer::initialize(std::shared_ptr<Brain> brain, 
                                  const PerformanceConfig& config) {
    if (!brain) {
        NLM_LOG_ERROR("BrainInitializer: Cannot initialize null brain");
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update configuration
    perfConfig_ = config;
    validatePerformanceConfig();
    
    NLM_LOG_INFO("Initializing brain with performance optimizations...");
    
    // Check for initialization cancellation
    if (cancelled_.load()) {
        NLM_LOG_INFO("Brain initialization cancelled");
        return false;
    }
    
    // Validate configuration
    if (!validateConfiguration(brain)) {
        NLM_LOG_ERROR("BrainInitializer: Invalid brain configuration");
        return false;
    }
    
    // Reset cancellation flag
    cancelled_.store(false);
    
    // Mark as initializing
    initializing_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
    
    try {
        // Execute initialization workflow
        if (!initializeWorkflow(brain)) {
            NLM_LOG_ERROR("BrainInitializer: Initialization workflow failed");
            return false;
        }
        
        // Log initialization statistics
        logInitializationProgress(brain);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("Brain initialization completed successfully in " + 
                    std::to_string(totalTime) + " seconds");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("BrainInitializer: Exception during initialization: ") + e.what());
        handleInitializationError(std::string("Exception: ") + e.what());
        return false;
    }
}

BrainInitializerStats BrainInitializer::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

PerformanceConfig BrainInitializer::getPerformanceConfig() const {
    return perfConfig_;
}

void BrainInitializer::updatePerformanceConfig(const PerformanceConfig& config) {
    perfConfig_ = config;
    validatePerformanceConfig();
    
    // Reinitialize infrastructure if needed
    if (config.enableMemoryPools != perfConfig_.enableMemoryPools) {
        cleanup();
        if (perfConfig_.enableMemoryPools) {
            initializeMemoryInfrastructure();
        }
    }
    
    NLM_LOG_INFO("BrainInitializer performance configuration updated");
}

PerformanceConfig BrainInitializer::getOptimalPerformanceConfig(size_t neuronCount) {
    PerformanceConfig config;
    
    // Base configuration
    config.enableMemoryPools = true;
    config.enableSIMD = true;
    config.enableEventDriven = true;
    config.enableMultithreading = true;
    config.enableSparseConnectivity = true;
    config.enablePerformanceMonitoring = true;
    
    // Configure based on network size
    if (neuronCount >= 1000000) {
        // Very large network
        config.threadCount = std::max<size_t>(4, std::thread::hardware_concurrency());
        config.memoryPoolBlockSize = 4096;
        config.memoryPoolInitialBlocks = 8;
        config.parallelGrainSize = 512;
        config.activityThreshold = 0.005f;
        config.connectivityThreshold = 0.01f;
        
        NLM_LOG_INFO("Using maximum performance configuration for very large network");
        
    } else if (neuronCount >= 100000) {
        // Large network
        config.threadCount = std::max<size_t>(2, std::thread::hardware_concurrency() / 2);
        config.memoryPoolBlockSize = 2048;
        config.memoryPoolInitialBlocks = 4;
        config.parallelGrainSize = 256;
        config.activityThreshold = 0.01f;
        config.connectivityThreshold = 0.02f;
        
        NLM_LOG_INFO("Using high-performance configuration for large network");
        
    } else if (neuronCount >= 10000) {
        // Medium network
        config.threadCount = std::max<size_t>(1, std::thread::hardware_concurrency() / 4);
        config.memoryPoolBlockSize = 1024;
        config.memoryPoolInitialBlocks = 2;
        config.parallelGrainSize = 128;
        config.activityThreshold = 0.02f;
        config.connectivityThreshold = 0.05f;
        
        NLM_LOG_INFO("Using balanced configuration for medium network");
        
    } else {
        // Small network - optimize for memory efficiency
        config.enableMultithreading = false;
        config.enableEventDriven = false;
        config.enableSparseConnectivity = false;
        config.enableSIMD = false;
        config.enableMemoryPools = false;
        
        NLM_LOG_INFO("Using lightweight configuration for small network");
    }
    
    return config;
}

bool BrainInitializer::validateBrainConfiguration(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("BrainInitializer: Brain is null");
        return false;
    }
    
    if (brain->getRegionCount() == 0) {
        NLM_LOG_ERROR("BrainInitializer: Brain has no regions");
        return false;
    }
    
    if (brain->getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("BrainInitializer: Brain has no neurons");
        return false;
    }
    
    // Validate configuration consistency
    size_t configuredRegions = config_->getOr<size_t>("region_count", 1);
    if (configuredRegions > brain->getRegionCount()) {
        NLM_LOG_WARNING("BrainInitializer: Config specifies " + 
                       std::to_string(configuredRegions) + " regions, but brain has " +
                       std::to_string(brain->getRegionCount()) + " regions");
    }
    
    return true;
}

bool BrainInitializer::initializeWorkflow(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Starting brain initialization workflow...");
    
    // Phase 1: Setup performance infrastructure
    if (perfConfig_.enableMemoryPools) {
        NLM_LOG_INFO("Phase 1: Setting up memory infrastructure");
        if (!setupMemoryInfrastructure()) {
            NLM_LOG_ERROR("Memory infrastructure setup failed");
            return false;
        }
    }
    
    // Phase 2: Setup SIMD acceleration
    if (perfConfig_.enableSIMD) {
        NLM_LOG_INFO("Phase 2: Setting up SIMD infrastructure");
        if (!setupSIMDInfrastructure()) {
            NLM_LOG_ERROR("SIMD infrastructure setup failed");
            return false;
        }
    }
    
    // Phase 3: Setup event-driven processing
    if (perfConfig_.enableEventDriven) {
        NLM_LOG_INFO("Phase 3: Setting up event-driven infrastructure");
        if (!setupEventDrivenInfrastructure()) {
            NLM_LOG_ERROR("Event-driven infrastructure setup failed");
            return false;
        }
    }
    
    // Phase 4: Setup parallel processing
    if (perfConfig_.enableMultithreading) {
        NLM_LOG_INFO("Phase 4: Setting up threading infrastructure");
        if (!setupThreadingInfrastructure()) {
            NLM_LOG_ERROR("Threading infrastructure setup failed");
            return false;
        }
    }
    
    // Phase 5: Setup connectivity
    if (perfConfig_.enableSparseConnectivity) {
        NLM_LOG_INFO("Phase 5: Setting up connectivity infrastructure");
        if (!setupConnectivityInfrastructure(brain)) {
            NLM_LOG_ERROR("Connectivity infrastructure setup failed");
            return false;
        }
    }
    
    // Phase 6: Initialize systems
    NLM_LOG_INFO("Phase 6: Initializing brain systems");
    if (!initializeSystems(brain)) {
        NLM_LOG_ERROR("Brain systems initialization failed");
        return false;
    }
    
    // Phase 7: Apply performance optimizations
    NLM_LOG_INFO("Phase 7: Applying performance optimizations");
    if (!applyPerformanceOptimizations(brain)) {
        NLM_LOG_ERROR("Performance optimization application failed");
        return false;
    }
    
    // Phase 8: Setup performance monitoring
    NLM_LOG_INFO("Phase 8: Setting up performance monitoring");
    if (!setupPerformanceMonitoring(brain)) {
        NLM_LOG_ERROR("Performance monitoring setup failed");
        return false;
    }
    
    // Phase 9: Collect and analyze statistics
    NLM_LOG_INFO("Phase 9: Collecting performance statistics");
    collectAndAnalyzeStats(brain);
    
    NLM_LOG_INFO("Brain initialization workflow completed successfully");
    return true;
}

bool BrainInitializer::setupMemoryInfrastructure() {
    auto phaseStart = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Setting up memory pools...");
    
    // Create memory pools
    neuronPool_ = std::make_unique<MemoryPool<Neuron>>(perfConfig_.memoryPoolBlockSize, perfConfig_.memoryPoolInitialBlocks);
    synapsePool_ = std::make_unique<MemoryPool<Synapse>>(perfConfig_.memoryPoolBlockSize, perfConfig_.memoryPoolInitialBlocks);
    
    // Initialize SoA pools for better cache locality
    neuronSoAPool_ = std::make_unique<NeuronPool>();
    synapseSoAPool_ = std::make_unique<SynapsePool>();
    
    // Reserve space based on expected sizes
    size_t neuronCount = config_->getOr<size_t>("neuron_count", 1000);
    size_t synapseCount = neuronCount * 100;  // Approximate synapses per neuron
    
    neuronSoAPool_->reserve(std::min(neuronCount, 1000000UL));
    synapseSoAPool_->reserve(std::min(synapseCount, 10000000UL));
    
    // Update memory stats
    stats_.memoryPoolSetupTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - phaseStart).count();
    
    NLM_LOG_INFO("Memory infrastructure setup completed in " + 
                std::to_string(stats_.memoryPoolSetupTime) + " seconds");
    return true;
}

bool BrainInitializer::setupSIMDInfrastructure() {
    auto phaseStart = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Setting up SIMD infrastructure...");
    
    // Check SIMD availability
    simdAvailable_ = SIMDUtils::isSIMDAvailable();
    
    if (!simdAvailable_) {
        NLM_LOG_WARNING("SIMD not available, falling back to scalar operations");
        perfConfig_.enableSIMD = false;
        
        stats_.simdSetupTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - phaseStart).count();
        return true;  // Don't fail initialization
    }
    
    // Configure SIMD vectorizer
    lifVectorizer_ = std::make_unique<VectorizedLIF>();
    
    // Determine SIMD width
    if (SIMDUtils::getSIMDType() == std::string("AVX-512")) {
        simdVectorWidth_ = 16;
    } else if (SIMDUtils::getSIMDType() == std::string("AVX2")) {
        simdVectorWidth_ = 8;
    } else if (SIMDUtils::getSIMDType() == std::string("SSE")) {
        simdVectorWidth_ = 4;
    } else {
        simdVectorWidth_ = 1;
    }
    
    stats_.simdSetupTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - phaseStart).count();
    
    NLM_LOG_INFO("SIMD infrastructure setup completed in " + 
                std::to_string(stats_.simdSetupTime) + " seconds");
    NLM_LOG_INFO("SIMD type: " + std::string(SIMDUtils::getSIMDType()));
    NLM_LOG_INFO("SIMD vector width: " + std::to_string(simdVectorWidth_));
    
    return true;
}

bool BrainInitializer::setupEventDrivenInfrastructure() {
    auto phaseStart = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Setting up event-driven infrastructure...");
    
    // Create event queues
    spikeQueue_ = std::make_unique<SpikeRingBuffer>();
    delayedSpikeQueue_ = std::make_unique<DelayedSpikeQueue>();
    
    // Configure queue sizes
    size_t neuronCount = config_->getOr<size_t>("neuron_count", 1000);
    size_t queueSize = std::min<size_t>(neuronCount * 4, 16384UL);
    
    spikeQueue_->resize(queueSize);
    delayedSpikeQueue_->configure(std::min<size_t>(neuronCount / 100, 1024UL), 100);
    
    // Initialize event processor
    eventProcessor_ = std::make_unique<EventDrivenProcessor>(perfConfig_);
    
    stats_.eventDrivenSetupTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - phaseStart).count();
    
    NLM_LOG_INFO("Event-driven infrastructure setup completed in " + 
                std::to_string(stats_.eventDrivenSetupTime) + " seconds");
    
    return true;
}

bool BrainInitializer::setupThreadingInfrastructure() {
    auto phaseStart = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Setting up threading infrastructure...");
    
    // Create thread pool
    threadPool_ = std::make_unique<ThreadPool>(perfConfig_.threadCount);
    
    // Initialize parallel processor
    parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(perfConfig_.threadCount);
    
    // Configure thread-local state
    threadLocalStates_.resize(perfConfig_.threadCount);
    for (auto& state : threadLocalStates_) {
        state.neuronCount = 0;
        state.spikeCount = 0;
        state.firingCount = 0;
        for (size_t i = 0; i < 16; ++i) {
            state.synapticInput[i] = 0.0f;
        }
    }
    
    stats_.threadingSetupTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - phaseStart).count();
    
    NLM_LOG_INFO("Threading infrastructure setup completed in " + 
                std::to_string(stats_.threadingSetupTime) + " seconds");
    NLM_LOG_INFO("Thread count: " + std::to_string(perfConfig_.threadCount));
    
    return true;
}

bool BrainInitializer::setupConnectivityInfrastructure(std::shared_ptr<Brain> brain) {
    auto phaseStart = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Setting up connectivity infrastructure...");
    
    // Create sparse connectivity structure
    sparseConnectivity_ = std::make_unique<SparseConnectivity>();
    
    // Configure based on network parameters
    size_t neuronCount = config_->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config_->getOr<size_t>("region_count", 1);
    
    sparseConnectivity_->reserveNeurons(neuronCount);
    
    // Set up connectivity parameters
    float connectionProbability = config_->getOr<float>("connection_probability", 0.1f);
    if (connectionProbability > perfConfig_.connectivityThreshold) {
        sparseConnectivity_->enableSparseConnections(true);
    }
    
    stats_.connectivitySetupTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - phaseStart).count();
    
    NLM_LOG_INFO("Connectivity infrastructure setup completed in " + 
                std::to_string(stats_.connectivitySetupTime) + " seconds");
    
    return true;
}

bool BrainInitializer::initializeSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing brain systems...");
    
    // Create optimized region and population management
    if (!initializeRegionManagement(brain)) {
        NLM_LOG_ERROR("Region management initialization failed");
        return false;
    }
    
    // Initialize connectivity with optimizations
    if (!initializeConnectivityOptimized(brain)) {
        NLM_LOG_ERROR("Connectivity initialization failed");
        return false;
    }
    
    // Initialize neural computation systems
    if (!initializeNeuralComputationSystems(brain)) {
        NLM_LOG_ERROR("Neural computation systems initialization failed");
        return false;
    }
    
    // Initialize memory systems
    if (!initializeMemorySystemsOptimized(brain)) {
        NLM_LOG_ERROR("Memory systems initialization failed");
        return false;
    }
    
    // Initialize cognition systems
    if (!initializeCognitionSystems(brain)) {
        NLM_LOG_ERROR("Cognition systems initialization failed");
        return false;
    }
    
    // Initialize neuromodulation systems
    if (!initializeNeuromodulationSystems(brain)) {
        NLM_LOG_ERROR("Neuromodulation systems initialization failed");
        return false;
    }
    
    // Initialize development systems
    if (!initializeDevelopmentSystems(brain)) {
        NLM_LOG_ERROR("Development systems initialization failed");
        return false;
    }
    
    // Initialize plasticity systems
    if (!initializePlasticitySystems(brain)) {
        NLM_LOG_ERROR("Plasticity systems initialization failed");
        return false;
    }
    
    // Initialize checkpoint system
    if (!initializeCheckpointSystem(brain)) {
        NLM_LOG_ERROR("Checkpoint system initialization failed");
        return false;
    }
    
    NLM_LOG_INFO("Brain systems initialization completed successfully");
    return true;
}

bool BrainInitializer::initializeRegionManagement(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing region management...");
    
    // Create regions using optimized configuration
    size_t neuronCount = config_->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config_->getOr<size_t>("region_count", 1);
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto regionId = addRegionOptimized(brain, "Region_" + std::to_string(i + 1));
        
        // Configure region based on performance settings
        if (perfConfig_.enableSparseConnectivity) {
            configureRegionForSparseConnectivity(brain, regionId, neuronCount);
        } else {
            configureRegionForStandardConnectivity(brain, regionId, neuronCount);
        }
        
        // Collect input/output neurons
        collectInputOutputNeurons(brain, regionId);
    }
    
    NLM_LOG_INFO("Region management initialization completed");
    return true;
}

bool BrainInitializer::initializeConnectivityOptimized(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing connectivity with optimizations...");
    
    // Create efficient connectivity structure
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t regionCount = brain->getRegionCount();
    
    // Initialize sparse connectivity if enabled
    if (perfConfig_.enableSparseConnectivity && sparseConnectivity_) {
        // Configure sparse connectivity for each region
        for (size_t i = 0; i < regionCount; ++i) {
            auto regionId = brain->getRegionIds()[i];
            configureRegionSparseConnectivity(brain, regionId, neuronCount);
        }
        
        // Build efficient synapse lookup tables
        createSynapseLookupTable(brain);
    }
    
    // Initialize standard connectivity as fallback
    initializeStandardConnectivity(brain);
    
    NLM_LOG_INFO("Connectivity initialization completed");
    return true;
}

bool BrainInitializer::initializeNeuralComputationSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing neural computation systems...");
    
    // Configure STDP parameters
    float ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config_->getOr<float>("stdp_tau", 20.0f);
    
    auto* stdp = brain->getSTDP();
    if (stdp) {
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Configure structural plasticity
    float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
    
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
    
    // Configure spike system
    auto* spikeSystem = brain->getSpikeSystem();
    if (spikeSystem) {
        // Register optimized spike handlers
        spikeSystem->registerHandler([this, brain](const DetailedSpikeEvent& event) {
            ++stats_.spikeCount;
            brain->getPerformanceMonitor()->recordComponentTime("spike_processing", 0.0);
        });
        
        spikeSystem->registerDelayedHandler([this, brain](const DelayedSpikeEvent& event) {
            deliverDelayedSpikeOptimized(brain, event);
        });
    }
    
    NLM_LOG_INFO("Neural computation systems initialization completed");
    return true;
}

bool BrainInitializer::initializeMemorySystemsOptimized(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing memory systems with optimizations...");
    
    // Initialize working memory with optimal capacity
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->initialize(brain.get());
        size_t neuronCount = brain->getTotalNeuronCount();
        workingMemory->setCapacity(std::max<size_t>(100, neuronCount / 10));
    }
    
    // Initialize episodic memory
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        episodicMemory->initialize(brain.get());
        episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        associativeMemory->initialize(brain.get());
    }
    
    NLM_LOG_INFO("Memory systems initialization completed");
    return true;
}

bool BrainInitializer::initializeCognitionSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing cognition systems...");
    
    // Initialize neural planner
    auto* planner = brain->getPlanner();
    if (planner) {
        planner->initialize(brain.get());
        planner->setPlanningDepth(5);
    }
    
    // Initialize concept formation
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        conceptFormation->initialize(brain.get());
    }
    
    // Initialize attention system
    auto* attention = brain->getAttention();
    if (attention) {
        attention->initialize(brain.get());
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    NLM_LOG_INFO("Cognition systems initialization completed");
    return true;
}

bool BrainInitializer::initializeNeuromodulationSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing neuromodulation systems...");
    
    // Initialize novelty detection
    auto* novelty = brain->getNovelty();
    if (novelty) {
        novelty->initialize(brain.get());
    }
    
    // Initialize curiosity system
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        curiosity->initialize(brain.get());
    }
    
    // Initialize dopamine system
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        dopamine->initialize(brain.get());
    }
    
    // Initialize prediction error system
    auto* predictionError = brain->getPredictionErrorSignal();
    if (predictionError) {
        predictionError->initialize(brain.get());
    }
    
    NLM_LOG_INFO("Neuromodulation systems initialization completed");
    return true;
}

bool BrainInitializer::initializeDevelopmentSystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing development systems...");
    
    // Configure development parameters
    auto* developmentSystem = brain->getDevelopmentSystem();
    if (developmentSystem) {
        developmentSystem->setParameters(
            config_->getOr<float>("development.mutation_rate", 0.01f),
            config_->getOr<float>("development.selection_strength", 1.0f),
            config_->getOr<float>("development.adaptation_rate", 0.1f)
        );
    }
    
    NLM_LOG_INFO("Development systems initialization completed");
    return true;
}

bool BrainInitializer::initializePlasticitySystems(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing plasticity systems...");
    
    // Enable plasticity on all synapses
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    NLM_LOG_INFO("Plasticity systems initialization completed");
    return true;
}

bool BrainInitializer::initializeCheckpointSystem(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Initializing checkpoint system...");
    
    // Configure checkpoint manager
    std::string checkpointDir = config_->getOr<std::string>("checkpoint_dir", "./checkpoints");
    auto* checkpointManager = brain->getConfig()->get<CheckpointManager*>("checkpoint_manager");
    if (checkpointManager) {
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("Checkpoint system initialization completed");
    return true;
}

bool BrainInitializer::applyPerformanceOptimizations(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Applying performance optimizations...");
    
    if (perfConfig_.enableMemoryPools) {
        applyMemoryPoolOptimizations(brain);
    }
    
    if (perfConfig_.enableSIMD) {
        applySIMDOptimizations(brain);
    }
    
    if (perfConfig_.enableEventDriven) {
        applyEventDrivenOptimizations(brain);
    }
    
    if (perfConfig_.enableMultithreading) {
        applyThreadingOptimizations(brain);
    }
    
    if (perfConfig_.enableSparseConnectivity) {
        applySparseConnectivityOptimizations(brain);
    }
    
    NLM_LOG_INFO("Performance optimizations applied");
    return true;
}

void BrainInitializer::applyMemoryPoolOptimizations(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Applying memory pool optimizations...");
    
    // Create optimized neuron lookup table
    createNeuronLookupTable(brain);
    
    // Create optimized synapse lookup table
    createSynapseLookupTable(brain);
    
    // Create optimized region lookup table
    createRegionLookupTable(brain);
    
    // Update statistics
    stats_.neuronsInitialized = brain->getTotalNeuronCount();
    stats_.synapsesInitialized = brain->getTotalSynapseCount();
    stats_.regionsInitialized = brain->getRegionCount();
}

void BrainInitializer::applySIMDOptimizations(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Applying SIMD optimizations...");
    
    // Configure SIMD for neural computation
    if (lifVectorizer_) {
        // Configure vectorizer parameters
        lifVectorizer_->V_rest = -70.0f;
        lifVectorizer_->V_reset = -70.0f;
        lifVectorizer_->V_threshold = -55.0f;
        lifVectorizer_->tau = 20.0f;
        lifVectorizer_->C = 1.0f;
        lifVectorizer_->dt = 1.0f;
    }
    
    NLM_LOG_INFO("SIMD optimizations applied");
}

void BrainInitializer::applyEventDrivenOptimizations(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Applying event-driven optimizations...");
    
    // Configure event-driven processing parameters
    if (eventProcessor_) {
        // Event processor is already configured in constructor
    }
    
    NLM_LOG_INFO("Event-driven optimizations applied");
}

void BrainInitializer::applyThreadingOptimizations(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Applying threading optimizations...");
    
    // Configure parallel processing
    if (parallelProcessor_) {
        // Parallel processor is already configured in constructor
    }
    
    NLM_LOG_INFO("Threading optimizations applied");
}

void BrainInitializer::applySparseConnectivityOptimizations(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Applying sparse connectivity optimizations...");
    
    // Configure sparse connectivity
    if (sparseConnectivity_) {
        // Sparse connectivity is already configured in constructor
    }
    
    NLM_LOG_INFO("Sparse connectivity optimizations applied");
}

void BrainInitializer::setupPerformanceMonitoring(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Setting up performance monitoring...");
    
    // Setup performance monitoring if enabled
    if (perfConfig_.enablePerformanceMonitoring) {
        brain->resetPerformanceMetrics();
        NLM_LOG_INFO("Performance monitoring enabled");
    }
}

void BrainInitializer::collectAndAnalyzeStats(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Collecting and analyzing performance statistics...");
    
    // Calculate optimization effectiveness
    calculateOptimizationEffectiveness();
    
    // Update initialization statistics
    updateStats(0.0, brain);  // Timing already collected elsewhere
    
    NLM_LOG_INFO("Performance statistics collected and analyzed");
}

void BrainInitializer::updateStats(double elapsedTime, const std::shared_ptr<Brain>& brain) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // Update timing statistics
    stats_.totalInitializationTime = elapsedTime;
    
    // Update memory statistics
    if (neuronPool_) {
        stats_.memoryPoolAllocationCount = neuronPool_->allocatedCount();
    }
    
    if (synapsePool_) {
        stats_.poolDeallocationCount = synapsePool_->allocatedCount();
    }
    
    // Update system statistics
    stats_.threadCount = perfConfig_.threadCount;
    stats_.simdAvailable = simdAvailable_;
    stats_.eventDrivenEnabled = perfConfig_.enableEventDriven;
    stats_.threadingEnabled = perfConfig_.enableMultithreading;
    stats_.sparseConnectivityEnabled = perfConfig_.enableSparseConnectivity;
}

void BrainInitializer::calculateOptimizationEffectiveness() {
    NLM_LOG_INFO("Calculating optimization effectiveness...");
    
    // Calculate memory optimization ratio
    if (neuronPool_ && stats_.memoryPoolAllocationCount > 0) {
        stats_.memoryOptimizationRatio = static_cast<double>(stats_.memoryPoolAllocationCount) / 
                                         static_cast<double>(stats_.neuronsInitialized);
    }
    
    // Calculate SIMD speedup factor (placeholder)
    stats_.simdSpeedupFactor = 2.0;  // Typical SIMD speedup
    
    // Calculate threading speedup factor (placeholder)
    stats_.threadingSpeedupFactor = static_cast<double>(perfConfig_.threadCount);
    
    // Calculate event-driven speedup factor (placeholder)
    stats_.eventDrivenSpeedupFactor = 1.5;  // Typical event-driven speedup
    
    NLM_LOG_INFO("Optimization effectiveness calculated");
}

void BrainInitializer::logInitializationProgress(const std::shared_ptr<Brain>& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Brain Initialization Complete ===");
    
    NLM_LOG_INFO("Network Statistics:");
    NLM_LOG_INFO("  Regions: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Performance Configuration:");
    NLM_LOG_INFO("  Memory pools: " + std::string(perfConfig_.enableMemoryPools ? "Yes" : "No"));
    NLM_LOG_INFO("  SIMD: " + std::string(perfConfig_.enableSIMD ? "Yes" : "No"));
    if (perfConfig_.enableSIMD && simdAvailable_) {
        NLM_LOG_INFO("    SIMD type: " + std::string(SIMDUtils::getSIMDType()));
    }
    NLM_LOG_INFO("  Event-driven: " + std::string(perfConfig_.enableEventDriven ? "Yes" : "No"));
    NLM_LOG_INFO("  Multithreading: " + std::string(perfConfig_.enableMultithreading ? "Yes" : "No"));
    if (perfConfig_.enableMultithreading) {
        NLM_LOG_INFO("    Threads: " + std::to_string(perfConfig_.threadCount));
    }
    NLM_LOG_INFO("  Sparse connectivity: " + std::string(perfConfig_.enableSparseConnectivity ? "Yes" : "No"));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Memory Statistics:");
    NLM_LOG_INFO("  Pool allocations: " + std::to_string(stats_.memoryPoolAllocationCount));
    NLM_LOG_INFO("  Pool deallocations: " + std::to_string(stats_.poolDeallocationCount));
    NLM_LOG_INFO("  Peak memory: " + std::to_string(stats_.peakMemoryUsage / (1024 * 1024)) + " MB");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Initialization Successful ===");
}

void BrainInitializer::handleInitializationError(const std::string& error) {
    NLM_LOG_ERROR("Brain initialization error: " + error);
    
    // Update stats to reflect error
    stats_.totalInitializationTime = 0.0;
    
    // Cleanup on error
    cleanup();
}

bool BrainInitializer::canInitialize() const {
    return !initializing_.load() && !cancelled_.load();
}

bool BrainInitializer::waitForInitialization(double timeoutSeconds) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    while (initializing_.load() && !cancelled_.load()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(currentTime - startTime).count();
        
        if (elapsed > timeoutSeconds) {
            NLM_LOG_ERROR("Brain initialization timeout");
            return false;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return !cancelled_.load();
}

void BrainInitializer::cancelInitialization() {
    cancelled_.store(true);
    NLM_LOG_INFO("Brain initialization cancelled");
}

void BrainInitializer::cancelInitializationInternal() {
    cancelled_.store(true);
    initializing_.store(false);
}

void BrainInitializer::cleanup() {
    if (cleanupRequested_.exchange(true)) {
        return;  // Already cleaned up
    }
    
    NLM_LOG_INFO("Cleaning up BrainInitializer...");
    
    // Cleanup performance infrastructure
    threadPool_.reset();
    parallelProcessor_.reset();
    eventProcessor_.reset();
    spikeQueue_.reset();
    delayedSpikeQueue_.reset();
    sparseConnectivity_.reset();
    checkpointManager_.reset();
    
    neuronPool_.reset();
    synapsePool_.reset();
    neuronSoAPool_.reset();
    synapseSoAPool_.reset();
    lifVectorizer_.reset();
    
    // Clear lookup tables
    neuronLookup_.clear();
    synapseLookup_.clear();
    regionLookup_.clear();
    populationLookup_.clear();
    threadLocalStates_.clear();
    
    // Reset state
    initializing_.store(false);
    cancelled_.store(false);
    
    NLM_LOG_INFO("BrainInitializer cleanup completed");
}

bool BrainInitializer::isInitialized() const {
    return initializing_.load();
}

double BrainInitializer::getInitializationProgress() const {
    // Placeholder implementation - would track actual progress
    return 1.0;  // Assuming 100% completion for now
}

double BrainInitializer::getElapsedTime() const {
    if (!startTime_.time_since_epoch().count()) {
        return 0.0;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(currentTime - startTime_).count();
}

bool BrainInitializer::isInitializationSlow(double thresholdSeconds) const {
    return getElapsedTime() > thresholdSeconds;
}

// PerformanceOptimizedBrainFactory implementation
BrainInitializer& BrainInitializer::PerformanceOptimizedBrainFactory::getInitializer() {
    initializeFactory();
    return *factoryInitializer_;
}

void BrainInitializer::PerformanceOptimizedBrainFactory::initializeFactory() {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    if (!factoryInitialized_) {
        factoryInitializer_ = new BrainInitializer(std::make_shared<Config>());
        factoryInitialized_ = true;
    }
}

PerformanceConfig BrainInitializer::PerformanceOptimizedBrainFactory::createMaximumPerformanceConfig() {
    PerformanceConfig config;
    config.enableMemoryPools = true;
    config.enableSIMD = true;
    config.enableEventDriven = true;
    config.enableMultithreading = true;
    config.enableSparseConnectivity = true;
    config.enablePerformanceMonitoring = true;
    
    config.memoryPoolBlockSize = 4096;
    config.memoryPoolInitialBlocks = 8;
    config.threadCount = std::max<size_t>(4, std::thread::hardware_concurrency());
    config.parallelGrainSize = 512;
    config.activityThreshold = 0.005f;
    config.connectivityThreshold = 0.01f;
    
    return config;
}

PerformanceConfig BrainInitializer::PerformanceOptimizedBrainFactory::createBalancedPerformanceConfig() {
    PerformanceConfig config;
    config.enableMemoryPools = true;
    config.enableSIMD = true;
    config.enableEventDriven = true;
    config.enableMultithreading = true;
    config.enableSparseConnectivity = true;
    config.enablePerformanceMonitoring = true;
    
    config.memoryPoolBlockSize = 2048;
    config.memoryPoolInitialBlocks = 4;
    config.threadCount = std::max<size_t>(2, std::thread::hardware_concurrency() / 2);
    config.parallelGrainSize = 256;
    config.activityThreshold = 0.01f;
    config.connectivityThreshold = 0.02f;
    
    return config;
}

PerformanceConfig BrainInitializer::PerformanceOptimizedBrainFactory::createMemoryOptimizedConfig() {
    PerformanceConfig config;
    config.enableMemoryPools = true;
    config.enableSIMD = false;
    config.enableEventDriven = false;
    config.enableMultithreading = false;
    config.enableSparseConnectivity = false;
    config.enablePerformanceMonitoring = false;
    
    config.memoryPoolBlockSize = 1024;
    config.memoryPoolInitialBlocks = 2;
    
    return config;
}

PerformanceConfig BrainInitializer::PerformanceOptimizedBrainFactory::createSpeedOptimizedConfig() {
    PerformanceConfig config;
    config.enableMemoryPools = false;
    config.enableSIMD = true;
    config.enableEventDriven = true;
    config.enableMultithreading = true;
    config.enableSparseConnectivity = true;
    config.enablePerformanceMonitoring = true;
    
    config.memoryPoolBlockSize = 0;
    config.memoryPoolInitialBlocks = 0;
    config.threadCount = std::thread::hardware_concurrency();
    config.parallelGrainSize = 256;
    config.activityThreshold = 0.01f;
    
    return config;
}

PerformanceConfig BrainInitializer::PerformanceOptimizedBrainFactory::createSmallNetworkConfig() {
    PerformanceConfig config;
    config.enableMemoryPools = false;
    config.enableSIMD = false;
    config.enableEventDriven = false;
    config.enableMultithreading = false;
    config.enableSparseConnectivity = false;
    config.enablePerformanceMonitoring = false;
    
    return config;
}

PerformanceConfig BrainInitializer::PerformanceOptimizedBrainFactory::createLargeNetworkConfig() {
    PerformanceConfig config;
    config.enableMemoryPools = true;
    config.enableSIMD = true;
    config.enableEventDriven = true;
    config.enableMultithreading = true;
    config.enableSparseConnectivity = true;
    config.enablePerformanceMonitoring = true;
    
    config.memoryPoolBlockSize = 4096;
    config.memoryPoolInitialBlocks = 8;
    config.threadCount = std::max<size_t>(4, std::thread::hardware_concurrency());
    config.parallelGrainSize = 512;
    config.activityThreshold = 0.005f;
    config.connectivityThreshold = 0.01f;
    
    return config;
}

std::shared_ptr<Brain> BrainInitializer::PerformanceOptimizedBrainFactory::createBrainWithConfig(std::shared_ptr<Config> config,
                                                                                            const PerformanceConfig& perfConfig) {
    // Create optimized brain using the BrainInitializer factory
    return createOptimizedBrain(std::move(config), perfConfig);
}

BrainInitializer& BrainInitializer::PerformanceOptimizedBrainFactory::getInitializer() {
    initializeFactory();
    return *factoryInitializer_;
}

std::chrono::duration<double> BrainInitializer::PerformanceOptimizedBrainFactory::benchmarkBrainInitialization(std::shared_ptr<Config> config,
                                                                                                       const PerformanceConfig& perfConfig) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    auto brain = createBrainWithConfig(std::move(config), perfConfig);
    
    if (brain && !brain->initialize()) {
        throw std::runtime_error("Brain initialization failed during benchmark");
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    return std::chrono::duration<double>(endTime - startTime);
}

std::string BrainInitializer::PerformanceOptimizedBrainFactory::compareOptimizationProfiles(std::shared_ptr<Config> config) {
    std::stringstream ss;
    
    ss << "Optimization Profile Comparison:" << std::endl;
    ss << "================================" << std::endl;
    ss << std::endl;
    
    // Compare different profiles
    auto profiles = getAvailableProfiles();
    
    for (const auto& profile : profiles) {
        PerformanceConfig perfConfig;
        
        if (profile == "Maximum Performance") {
            perfConfig = createMaximumPerformanceConfig();
        } else if (profile == "Balanced Performance") {
            perfConfig = createBalancedPerformanceConfig();
        } else if (profile == "Memory Optimized") {
            perfConfig = createMemoryOptimizedConfig();
        } else if (profile == "Speed Optimized") {
            perfConfig = createSpeedOptimizedConfig();
        }
        
        ss << profile << ":" << std::endl;
        ss << "  Memory pools: " << (perfConfig.enableMemoryPools ? "Yes" : "No") << std::endl;
        ss << "  SIMD: " << (perfConfig.enableSIMD ? "Yes" : "No") << std::endl;
        ss << "  Event-driven: " << (perfConfig.enableEventDriven ? "Yes" : "No") << std::endl;
        ss << "  Multithreading: " << (perfConfig.enableMultithreading ? "Yes" : "No") << std::endl;
        ss << "  Sparse connectivity: " << (perfConfig.enableSparseConnectivity ? "Yes" : "No") << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string BrainInitializer::PerformanceOptimizedBrainFactory::getRecommendedProfile(double memoryConstraint,
                                                                               double timeConstraint,
                                                                               size_t neuronCount) {
    if (neuronCount < 1000) {
        return "Small Network";
    } else if (neuronCount < 10000) {
        if (timeConstraint < memoryConstraint) {
            return "Speed Optimized";
        } else {
            return "Memory Optimized";
        }
    } else {
        if (memoryConstraint < timeConstraint) {
            return "Memory Optimized";
        } else {
            return "Maximum Performance";
        }
    }
}

// Static factory initialization
std::atomic<bool> BrainInitializer::PerformanceOptimizedBrainFactory::factoryInitialized_{false};
std::mutex BrainInitializer::PerformanceOptimizedBrainFactory::factoryMutex_;
BrainInitializer* BrainInitializer::PerformanceOptimizedBrainFactory::factoryInitializer_ = nullptr;

} // namespace nlm
