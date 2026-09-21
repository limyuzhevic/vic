#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>

using namespace nlm;

void printDemoBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM Phase 6 Integration Demo                              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage() {
    std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Integration test options:" << std::endl;
    std::cout << "  --quick                    Run only integration verification (fast)" << std::endl;
    std::cout << "  --neurons N               Set neuron count (default: 1000)" << std::endl;
    std::cout << "  --steps S                  Set max simulation steps (default: 10000)" << std::endl;
    std::cout << "  --checkpoint-path P        Set checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --no-checkpoint            Disable checkpointing" << std::endl;
    std::cout << "  --no-replay                Disable replay system" << std::endl;
    std::cout << "  --no-development           Disable development system" << std::endl;
    std::cout << "  --connection-prob P        Set connection probability (default: 0.1)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Logging options:" << std::endl;
    std::cout << "  --verbose                  Enable verbose logging" << std::endl;
    std::cout << "  --quiet                    Reduce log level to warnings only" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Advanced options:" << std::endl;
    std::cout << "  --ultra-fast               Run with minimal configuration for speed" << std::endl;
    std::cout << "  --compare                  Run comparison experiment across runs" << std::endl;
    std::cout << "  --benchmark                Measure and report performance metrics" << std::endl;
}

int main(int argc, char* argv[]) {
    printDemoBanner();
    std::cout << "Running Phase 6 integration demo..." << std::endl << std::endl;
    
    // Initialize config with defaults
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Parse command line arguments
    std::string checkpointPath = "./checkpoint_test.bin";
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    float connectionProbability = 0.1f;
    bool verboseLogging = false;
    bool quietLogging = false;
    bool ultraFastMode = false;
    bool compareMultipleRuns = false;
    bool benchmarkMode = false;
    bool quickMode = false;
    
    uint64_t neuronCount = 1000;
    uint64_t maxSteps = 10000;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--quick") {
            quickMode = true;
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                maxSteps = std::stoul(argv[++i]);
            }
        } else if (arg == "--checkpoint-path") {
            if (i + 1 < argc) {
                checkpointPath = argv[++i];
            }
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--connection-prob") {
            if (i + 1 < argc) {
                connectionProbability = std::stof(argv[++i]);
            }
        } else if (arg == "--verbose") {
            verboseLogging = true;
        } else if (arg == "--quiet") {
            quietLogging = true;
        } else if (arg == "--ultra-fast") {
            ultraFastMode = true;
        } else if (arg == "--compare") {
            compareMultipleRuns = true;
        } else if (arg == "--benchmark") {
            benchmarkMode = true;
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        }
    }
    
    // Apply ultra-fast mode overrides
    if (ultraFastMode) {
        neuronCount = 100;
        maxSteps = 500;
        enableDevelopment = false;
        enableReplay = false;
        enableCheckpointing = false;
        verboseLogging = true;
    }
    
    // Configure logging level
    auto logger = Logger::getInstance();
    if (quietLogging) {
        logger.setLevel(Logger::Level::Warning);
    } else if (verboseLogging) {
        logger.setLevel(Logger::Level::Debug);
    } else {
        logger.setLevel(Logger::Level::Info);
    }
    
    // Print configuration
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << neuronCount << std::endl;
    std::cout << "  Max steps: " << maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "enabled" : "disabled") << std::endl;
    if (enableCheckpointing) {
        std::cout << "  Checkpoint path: " << checkpointPath << std::endl;
    }
    std::cout << "  Replay: " << (enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "enabled" : "disabled") << std::endl;
    std::cout << "  Connection probability: " << connectionProbability << std::endl;
    std::cout << "" << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Quick mode: only run integration verification
    if (quickMode) {
        std::cout << "=== Quick Integration Verification ===" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "✓ Integration verification passed!" << std::endl;
        return 0;
    }
    
    // Run full integration experiment
    std::cout << "=== Full Integration Experiment ===" << std::endl;
    
    Phase6Config demoConfig;
    demoConfig.neuronCount = neuronCount;
    demoConfig.maxSteps = maxSteps;
    demoConfig.regionCount = 1;
    demoConfig.connectionProbability = connectionProbability;
    demoConfig.enableCheckpointing = enableCheckpointing;
    demoConfig.enableReplay = enableReplay;
    demoConfig.enableDevelopment = enableDevelopment;
    demoConfig.checkpointPath = checkpointPath;
    
    // Enable additional features if requested
    if (compareMultipleRuns) {
        std::cout << "Note: Comparison experiment requires Phase 2 main.cpp" << std::endl;
    }
    
    if (benchmarkMode) {
        std::cout << "Benchmark mode: Performance metrics will be collected" << std::endl;
    }
    
    auto result = experiment.run(demoConfig);
    
    // Print results
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Average synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== DEMO COMPLETE ===" << std::endl;
    
    return 0;
}