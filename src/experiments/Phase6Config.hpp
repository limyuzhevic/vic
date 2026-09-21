// Phase 6 Configuration - Enhanced Settings for Integration Testing
// 
// This file contains configuration options specifically for Phase 6
// integration testing, building upon the settings from previous phases.
//
// Phase 6 focuses on integrating all existing systems into a coherent
// artificial brain. This configuration provides optimized settings
// for testing the complete integrated brain.

/**
 * Integration Configuration
 */
struct Phase6IntegrationConfig {
    // System Integration Settings
    bool enableAllSystems;                    // Enable all integrated systems
    bool memoryIntegration;                   // Integrate memory systems
    bool neuromodulation;                      // Enable neuromodulation effects
    bool development;                          // Enable developmental stages
    bool prediction;                           // Enable prediction system
    bool curiosity;                            // Enable curiosity-driven learning
    bool replay;                               // Enable episodic memory replay
    bool checkpointing;                        // Enable checkpoint save/load
    bool structuralPlasticity;                 // Enable structural plasticity
    
    // Performance Settings
    size_t neuronCount;                        // Number of neurons for testing
    size_t regionCount;                        // Number of brain regions
    float connectionProbability;               // Synaptic connection probability
    float initialWeightMean;                   // Mean initial synaptic weight
    float initialWeightStd;                    // Standard deviation of initial weights
    
    // Plasticity Settings
    float stdpLtpWeight;                       // STDP LTP learning rate
    float stdpLtdWeight;                       // STDP LTD learning rate
    float stdpTau;                             // STDP time constant
    float hebbianLearningRate;                 // Hebbian learning rate
    float synaptogenesisRate;                   // Structural synaptogenesis rate
    float pruningRate;                         // Structural pruning rate
    
    // Neuromodulation Settings
    float dopamineScale;                       // Dopamine modulation scale
    float curiosityStrength;                   // Curiosity exploration strength
    float noveltyThreshold;                    // Novelty detection threshold
    
    // Memory Settings
    size_t workingMemoryCapacity;              // Working memory capacity
    size_t maxEpisodicEpisodes;                // Maximum episodic memory episodes
    float memoryReplayInterval;                // Memory replay interval
    float consolidationThreshold;              // Memory consolidation threshold
    
    // Development Settings
    DevelopmentalStage initialDevelopmentalStage;  // Starting developmental stage
    float developmentSpeed;                     // Development progression speed
    float plasticityMaturationRate;             // Plasticity maturation rate
    
    // Performance Monitoring
    bool performanceMonitoring;                // Enable performance monitoring
    double monitoringInterval;                 // Performance monitoring interval
    std::string statsOutputFile;               // Performance statistics output file
    bool debugMode;                            // Enable debug mode
    
    // Testing Settings
    uint64_t testDuration;                     // Test duration in steps
    uint64_t warmupSteps;                      // Warmup steps before testing
    bool verboseLogging;                       // Enable verbose logging
    bool logToFile;                            // Log to file instead of stdout
    
    Phase6IntegrationConfig()
        : enableAllSystems(true)
        , memoryIntegration(true)
        , neuromodulation(true)
        , development(true)
        , prediction(true)
        , curiosity(true)
        , replay(true)
        , checkpointing(true)
        , structuralPlasticity(true)
        
        // Performance Settings
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , initialWeightMean(0.5f)
        , initialWeightStd(0.1f)
        
        // Plasticity Settings
        , stdpLtpWeight(0.01f)
        , stdpLtdWeight(0.012f)
        , stdpTau(20.0f)
        , hebbianLearningRate(0.005f)
        , synaptogenesisRate(0.0001f)
        , pruningRate(0.00001f)
        
        // Neuromodulation Settings
        , dopamineScale(1.0f)
        , curiosityStrength(0.5f)
        , noveltyThreshold(0.1f)
        
        // Memory Settings
        , workingMemoryCapacity(100)
        , maxEpisodicEpisodes(1000)
        , memoryReplayInterval(100.0f)
        , consolidationThreshold(0.3f)
        
        // Development Settings
        , initialDevelopmentalStage(DevelopmentalStage::Adult)
        , developmentSpeed(1.0f)
        , plasticityMaturationRate(0.5f)
        
        // Performance Monitoring
        , performanceMonitoring(true)
        , monitoringInterval(1.0)
        , statsOutputFile("phase6_stats.csv")
        , debugMode(false)
        
        // Testing Settings
        , testDuration(10000)
        , warmupSteps(1000)
        , verboseLogging(true)
        , logToFile(true) {}
};

/**
 * Helper function to create default Phase 6 configuration
 */
inline Phase6IntegrationConfig createDefaultPhase6Config() {
    Phase6IntegrationConfig config;
    
    // Optimize for integration testing
    config.neuronCount = 1000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    
    // Enhanced plasticity for integration
    config.stdpLtpWeight = 0.01f;
    config.stdpLtdWeight = 0.012f;
    config.stdpTau = 20.0f;
    config.hebbianLearningRate = 0.005f;
    
    // Memory integration
    config.workingMemoryCapacity = 100;
    config.maxEpisodicEpisodes = 1000;
    config.memoryReplayInterval = 100.0f;
    
    // Neuromodulation for integration
    config.dopamineScale = 1.0f;
    config.curiosityStrength = 0.5f;
    config.noveltyThreshold = 0.1f;
    
    // Development for adult brain
    config.initialDevelopmentalStage = DevelopmentalStage::Adult;
    config.developmentSpeed = 1.0f;
    config.plasticityMaturationRate = 0.5f;
    
    // Performance monitoring
    config.performanceMonitoring = true;
    config.monitoringInterval = 1.0;
    config.statsOutputFile = "phase6_stats.csv";
    
    // Testing settings
    config.testDuration = 10000;
    config.warmupSteps = 1000;
    config.verboseLogging = true;
    
    return config;
}

/**
 * Helper function to create lightweight Phase 6 configuration
 * For quick testing and validation
 */
inline Phase6IntegrationConfig createLightweightPhase6Config() {
    Phase6IntegrationConfig config = createDefaultPhase6Config();
    
    // Reduce resource usage for lightweight testing
    config.neuronCount = 500;
    config.testDuration = 2000;
    config.warmupSteps = 200;
    config.workingMemoryCapacity = 50;
    config.maxEpisodicEpisodes = 500;
    config.memoryReplayInterval = 50.0f;
    
    return config;
}

/**
 * Helper function to create intensive Phase 6 configuration
 * For thorough integration testing
 */
inline Phase6IntegrationConfig createIntensivePhase6Config() {
    Phase6IntegrationConfig config = createDefaultPhase6Config();
    
    // Increase resource usage for intensive testing
    config.neuronCount = 2000;
    config.testDuration = 20000;
    config.warmupSteps = 2000;
    config.workingMemoryCapacity = 200;
    config.maxEpisodicEpisodes = 5000;
    config.memoryReplayInterval = 200.0f;
    config.dopamineScale = 2.0f;
    config.curiosityStrength = 1.0f;
    
    return config;
}

/**
 * Command-line argument parser for Phase 6 configuration
 */
inline void parsePhase6Args(int argc, char** argv, Phase6IntegrationConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.find("--phase6.neuron-count") == 0) {
            if (arg.find('=') != std::string::npos) {
                config.neuronCount = static_cast<size_t>(std::stoi(
                    arg.substr(arg.find('=') + 1)));
            } else if (i + 1 < argc) {
                config.neuronCount = static_cast<size_t>(std::stoi(argv[++i]));
            }
        } else if (arg.find("--phase6.enable-all-systems") == 0) {
            config.enableAllSystems = true;
        } else if (arg.find("--phase6.disable-performance-monitoring") == 0) {
            config.performanceMonitoring = false;
        } else if (arg.find("--phase6.debug") == 0) {
            config.debugMode = true;
        } else if (arg.find("--phase6.test-duration") == 0) {
            if (arg.find('=') != std::string::npos) {
                config.testDuration = static_cast<uint64_t>(std::stoi(
                    arg.substr(arg.find('=') + 1)));
            } else if (i + 1 < argc) {
                config.testDuration = static_cast<uint64_t>(std::stoi(argv[++i]));
            }
        } else if (arg.find("--phase6.warmup-steps") == 0) {
            if (arg.find('=') != std::string::npos) {
                config.warmupSteps = static_cast<uint64_t>(std::stoi(
                    arg.substr(arg.find('=') + 1)));
            } else if (i + 1 < argc) {
                config.warmupSteps = static_cast<uint64_t>(std::stoi(argv[++i]));
            }
        } else if (arg.find("--phase6.config-file") == 0) {
            if (arg.find('=') != std::string::npos) {
                std::string configFile = arg.substr(arg.find('=') + 1);
                // TODO: Load configuration from file
                // This would update config with values from the file
            } else if (i + 1 < argc) {
                std::string configFile = argv[++i];
                // TODO: Load configuration from file
                // This would update config with values from the file
            }
        }
    }
}

/**
 * Validate Phase 6 configuration
 */
inline bool validatePhase6Config(const Phase6IntegrationConfig& config) {
    // Validate configuration values
    if (config.neuronCount == 0) {
        std::cerr << "ERROR: neuronCount must be > 0" << std::endl;
        return false;
    }
    
    if (config.regionCount == 0) {
        std::cerr << "ERROR: regionCount must be > 0" << std::endl;
        return false;
    }
    
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        std::cerr << "ERROR: connectionProbability must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    if (config.testDuration == 0) {
        std::cerr << "ERROR: testDuration must be > 0" << std::endl;
        return false;
    }
    
    if (config.warmupSteps >= config.testDuration) {
        std::cerr << "ERROR: warmupSteps must be < testDuration" << std::endl;
        return false;
    }
    
    if (config.workingMemoryCapacity == 0) {
        std::cerr << "ERROR: workingMemoryCapacity must be > 0" << std::endl;
        return false;
    }
    
    if (config.maxEpisodicEpisodes == 0) {
        std::cerr << "ERROR: maxEpisodicEpisodes must be > 0" << std::endl;
        return false;
    }
    
    if (config.memoryReplayInterval <= 0.0f) {
        std::cerr << "ERROR: memoryReplayInterval must be > 0" << std::endl;
        return false;
    }
    
    if (config.consolidationThreshold <= 0.0f || config.consolidationThreshold > 1.0f) {
        std::cerr << "ERROR: consolidationThreshold must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    if (config.stdpLtpWeight < 0.0f) {
        std::cerr << "ERROR: stdpLtpWeight must be >= 0.0" << std::endl;
        return false;
    }
    
    if (config.stdpLtdWeight < 0.0f) {
        std::cerr << "ERROR: stdpLtdWeight must be >= 0.0" << std::endl;
        return false;
    }
    
    if (config.stdpTau <= 0.0f) {
        std::cerr << "ERROR: stdpTau must be > 0.0" << std::endl;
        return false;
    }
    
    return true;
}

/**
 * Print Phase 6 configuration summary
 */
inline void printPhase6ConfigSummary(const Phase6IntegrationConfig& config) {
    std::cout << "=== Phase 6 Configuration Summary ===" << std::endl;
    std::cout << "System Integration:" << std::endl;
    std::cout << "  - All systems enabled: " << (config.enableAllSystems ? "YES" : "NO") << std::endl;
    std::cout << "  - Memory integration: " << (config.memoryIntegration ? "YES" : "NO") << std::endl;
    std::cout << "  - Neuromodulation: " << (config.neuromodulation ? "YES" : "NO") << std::endl;
    std::cout << "  - Development: " << (config.development ? "YES" : "NO") << std::endl;
    std::cout << "  - Prediction system: " << (config.prediction ? "YES" : "NO") << std::endl;
    std::cout << "  - Curiosity: " << (config.curiosity ? "YES" : "NO") << std::endl;
    std::cout << "  - Replay system: " << (config.replay ? "YES" : "NO") << std::endl;
    std::cout << "  - Checkpointing: " << (config.checkpointing ? "YES" : "NO") << std::endl;
    std::cout << "  - Structural plasticity: " << (config.structuralPlasticity ? "YES" : "NO") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Settings:" << std::endl;
    std::cout << "  - Neuron count: " << config.neuronCount << std::endl;
    std::cout << "  - Region count: " << config.regionCount << std::endl;
    std::cout << "  - Connection probability: " << config.connectionProbability << std::endl;
    std::cout << "  - Initial weight mean: " << config.initialWeightMean << std::endl;
    std::cout << "  - Initial weight std: " << config.initialWeightStd << std::endl;
    
    std::cout << std::endl;
    std::cout << "Plasticity Settings:" << std::endl;
    std::cout << "  - STDP LTP weight: " << config.stdpLtpWeight << std::endl;
    std::cout << "  - STDP LTD weight: " << config.stdpLtdWeight << std::endl;
    std::cout << "  - STDP tau: " << config.stdpTau << std::endl;
    std::cout << "  - Hebbian learning rate: " << config.hebbianLearningRate << std::endl;
    std::cout << "  - Synaptogenesis rate: " << config.synaptogenesisRate << std::endl;
    std::cout << "  - Pruning rate: " << config.pruningRate << std::endl;
    
    std::cout << std::endl;
    std::cout << "Memory Settings:" << std::endl;
    std::cout << "  - Working memory capacity: " << config.workingMemoryCapacity << std::endl;
    std::cout << "  - Max episodic episodes: " << config.maxEpisodicEpisodes << std::endl;
    std::cout << "  - Memory replay interval: " << config.memoryReplayInterval << std::endl;
    std::cout << "  - Consolidation threshold: " << config.consolidationThreshold << std::endl;
    
    std::cout << std::endl;
    std::cout << "Development Settings:" << std::endl;
    std::cout << "  - Initial developmental stage: " << static_cast<int>(config.initialDevelopmentalStage) << std::endl;
    std::cout << "  - Development speed: " << config.developmentSpeed << std::endl;
    std::cout << "  - Plasticity maturation rate: " << config.plasticityMaturationRate << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Monitoring:" << std::endl;
    std::cout << "  - Enabled: " << (config.performanceMonitoring ? "YES" : "NO") << std::endl;
    std::cout << "  - Monitoring interval: " << config.monitoringInterval << std::endl;
    std::cout << "  - Stats output file: " << config.statsOutputFile << std::endl;
    std::cout << "  - Debug mode: " << (config.debugMode ? "YES" : "NO") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Testing Settings:" << std::endl;
    std::cout << "  - Test duration: " << config.testDuration << " steps" << std::endl;
    std::cout << "  - Warmup steps: " << config.warmupSteps << std::endl;
    std::cout << "  - Verbose logging: " << (config.verboseLogging ? "YES" : "NO") << std::endl;
    std::cout << "  - Log to file: " << (config.logToFile ? "YES" : "NO") << std::endl;
    
    std::cout << std::endl;
}