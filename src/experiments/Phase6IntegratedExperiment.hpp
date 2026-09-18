#pragma once

/**
 * Phase6IntegratedExperiment - Phase 6 final integration test
 * 
 * This experiment demonstrates that the integrated brain systems work together:
 * - Memory systems are connected to neural processing
 * - Neuromodulation affects plasticity and neural dynamics
 * - Development affects plasticity rates
 * - Prediction system is integrated
 * - Replay and consolidation are functional
 * - Checkpoint save/load works
 * 
 * The experiment runs a complete lifetime simulation and verifies
 * that all systems interact properly.
 */

#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * Phase 6 integration test result
 */
struct Phase6IntegrationResult {
    // System integration status
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    bool replayWorks;
    
    // Metrics
    float totalReward;
    float avgFiringRate;
    float avgSynapticWeight;
    float memoryEpisodesStored;
    float noveltyLevel;
    float curiosityLevel;
    float dopamineLevel;
    
    // Timestamps
    time_t startTime;
    time_t endTime;
    double totalWallClockTime;
    
    Phase6IntegrationResult()
        : memoryWorkingMemoryIntegrated(false)
        , memoryEpisodicMemoryIntegrated(false)
        , neuromodulationIntegrated(false)
        , predictionIntegrated(false)
        , developmentIntegrated(false)
        , checkpointingWorks(false)
        , replayWorks(false)
        , totalReward(0.0f)
        , avgFiringRate(0.0f)
        , avgSynapticWeight(0.0f)
        , memoryEpisodesStored(0.0f)
        , noveltyLevel(0.0f)
        , curiosityLevel(0.0f)
        , dopamineLevel(0.0f)
        , startTime(0)
        , endTime(0)
        , totalWallClockTime(0.0) {}
};

/**
 * Configuration for Phase 6 integration experiment
 */
struct Phase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string checkpointPath;
    std::string logLevel;
    bool enableProfiling;
    bool enableDetailedLogging;
    
    Phase6Config()
        : maxSteps(10000)
        , neuronCount(1000)
        , regionCount(1)
        , connectionProbability(0.1f)
        , enableCheckpointing(true)
        , enableReplay(true)
        , enableDevelopment(true)
        , checkpointPath("./checkpoint_test.bin")
        , logLevel("INFO")
        , enableProfiling(false)
        , enableDetailedLogging(false) {}
    
    /**
     * Parse command line arguments
     */
    static Phase6Config parseCommandLine(int argc, char* argv[]) {
        Phase6Config config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--neurons" && i + 1 < argc) {
                config.neuronCount = std::stoul(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                config.maxSteps = std::stoul(argv[++i]);
            } else if (arg == "--regions" && i + 1 < argc) {
                config.regionCount = std::stoul(argv[++i]);
            } else if (arg == "--connection-prob" && i + 1 < argc) {
                config.connectionProbability = std::stof(argv[++i]);
            } else if (arg == "--no-checkpoint") {
                config.enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                config.enableReplay = false;
            } else if (arg == "--no-development") {
                config.enableDevelopment = false;
            } else if (arg == "--checkpoint" && i + 1 < argc) {
                config.checkpointPath = argv[++i];
            } else if (arg == "--log-level" && i + 1 < argc) {
                config.logLevel = argv[++i];
            } else if (arg == "--profile") {
                config.enableProfiling = true;
            } else if (arg == "--detailed-log") {
                config.enableDetailedLogging = true;
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            }
        }
        
        return config;
    }
    
    static void printHelp() {
        std::cout << "NLM Phase 6 Integration Demo - Command Line Options" << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << "  --neurons N           Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --steps S             Number of simulation steps (default: 10000)" << std::endl;
        std::cout << "  --regions R           Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --connection-prob P   Connection probability (default: 0.1)" << std::endl;
        std::cout << "  --no-checkpoint       Disable checkpointing" << std::endl;
        std::cout << "  --no-replay           Disable replay system" << std::endl;
        std::cout << "  --no-development      Disable development system" << std::endl;
        std::cout << "  --checkpoint PATH     Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
        std::cout << "  --log-level L         Log level (DEBUG, INFO, WARN, ERROR) (default: INFO)" << std::endl;
        std::cout << "  --profile             Enable performance profiling" << std::endl;
        std::cout << "  --detailed-log        Enable detailed logging" << std::endl;
        std::cout << "  -h, --help            Show this help message" << std::endl;
        std::cout << "=====================================================" << std::endl;
    }
};

/**
 * Phase 6 integrated experiment runner
 */
class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    /**
     * Run the complete integration test
     */
    Phase6IntegrationResult run(const Phase6Config& config);
    
    /**
     * Run a simple integration verification
     */
    bool verifyIntegration();
    
    /**
     * Test memory integration
     */
    bool testMemoryIntegration();
    
    /**
     * Test neuromodulation integration
     */
    bool testNeuromodulationIntegration();
    
    /**
     * Test checkpoint save/load
     */
    bool testCheckpointing();
    
    /**
     * Test replay system
     */
    bool testReplay();
    
    /**
     * Get current experiment status for monitoring
     */
    std::string getStatusString() const;
    
    /**
     * Cleanup all resources and reset state
     */
    void cleanup();
    
private:
    // State tracking for experiment runs
    bool verificationCompleted;
    bool memoryIntegrationTested;
    bool neuromodulationIntegrationTested;
    bool checkpointingTested;
    bool replayTested;
    double lastProgressUpdateTime;
    std::unordered_map<std::string, std::vector<double>> metricHistory;
    
    // Resources management
    std::shared_ptr<Config> defaultConfig;
};

} // namespace nlm
