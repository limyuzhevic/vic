// Phase 6 Demo with improved error handling and argument parsing
// Demonstrates integration test improvements and unified configuration system

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "config/NLMConfig.hpp"
#include <iostream>
#include <memory>
#include <cstring>
#include <functional>

using namespace nlm;

// Command-line argument parsing
struct Phase6DemoArgs {
    std::string checkpointPath = "./checkpoint_test.bin";
    size_t neuronCount = 1000;
    size_t maxSteps = 10000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool verbose = false;
    bool runQuickTest = false;
    std::string configFile;
};

// Parse command-line arguments
Phase6DemoArgs parseArgs(int argc, char* argv[]) {
    Phase6DemoArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quick" || arg == "-q") {
            args.runQuickTest = true;
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            args.checkpointPath = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            args.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            args.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            args.regionCount = std::stoul(argv[++i]);
        } else if (arg == "--connection" && i + 1 < argc) {
            args.connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        }
    }
    
    return args;
}

// Print help information
void printHelp() {
    std::cout << "=== NLM Phase 6 Demo Help ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
    std::cout << "  --quick, -q             Run quick integration test" << std::endl;
    std::cout << "  --checkpoint <path>     Set checkpoint file path" << std::endl;
    std::cout << "  --neurons <count>       Set number of neurons" << std::endl;
    std::cout << "  --steps <count>         Set maximum simulation steps" << std::endl;
    std::cout << "  --regions <count>       Set number of brain regions" << std::endl;
    std::cout << "  --connection <prob>     Set connection probability" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --quick --verbose" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 500 --steps 2000 --checkpoint my_checkpoint.bin" << std::endl;
}

// Create configuration from command-line arguments
Phase6Config createConfig(const Phase6DemoArgs& args) {
    Phase6Config config;
    
    // Set configuration values from command-line arguments
    config.neuronCount = static_cast<uint64_t>(args.neuronCount);
    config.maxSteps = static_cast<uint64_t>(args.maxSteps);
    config.regionCount = static_cast<size_t>(args.regionCount);
    config.connectionProbability = args.connectionProbability;
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;
    config.checkpointPath = args.checkpointPath;
    
    // Log configuration
    if (args.verbose) {
        std::cout << "=== Phase 6 Configuration ===" << std::endl;
        std::cout << "Neurons: " << config.neuronCount << std::endl;
        std::cout << "Max Steps: " << config.maxSteps << std::endl;
        std::cout << "Regions: " << config.regionCount << std::endl;
        std::cout << "Connection Probability: " << config.connectionProbability << std::endl;
        std::cout << "Checkpoint Path: " << config.checkpointPath << std::endl;
        std::cout << "Enable Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
        std::cout << "Enable Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
        std::cout << "Enable Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    }
    
    return config;
}

// Safe function execution wrapper
bool safeExecute(const std::string& operation, std::function<bool()> func) {
    try {
        return func();
    } catch (const std::exception& e) {
        std::cerr << "ERROR during " << operation << ": " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "ERROR during " << operation ": Unknown exception" << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        Phase6DemoArgs args = parseArgs(argc, argv);
        
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl;
        std::cout << std::endl;
        
        // Initialize logging
        auto& logger = Logger::getInstance();
        logger.setLevel(Logger::Level::Info);
        
        if (args.verbose) {
            std::cout << "Logging level: Info" << std::endl;
        }
        
        // Create experiment
        std::cout << "--- Creating Phase 6 Experiment ---" << std::endl;
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        if (args.runQuickTest) {
            std::cout << "--- Quick Integration Verification ---" << std::endl;
            bool integrationOK = safeExecute("integration verification", [&]() {
                return experiment.verifyIntegration();
            });
            
            std::cout << std::endl;
            
            if (!integrationOK) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return 1;
            }
            
            std::cout << "Integration verification passed!" << std::endl << std::endl;
        }
        
        // Test individual systems safely
        if (args.runQuickTest || args.verbose) {
            std::cout << "--- Memory Integration Test ---" << std::endl;
            bool memoryOK = safeExecute("memory integration test", [&]() {
                return experiment.testMemoryIntegration();
            });
            
            if (args.verbose) {
                std::cout << "Memory integration: " << (memoryOK ? "PASSED" : "FAILED") << std::endl;
            }
        }
        
        std::cout << std::endl;
        
        if (args.runQuickTest || args.verbose) {
            std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
            bool neuromodulationOK = safeExecute("neuromodulation integration test", [&]() {
                return experiment.testNeuromodulationIntegration();
            });
            
            if (args.verbose) {
                std::cout << "Neuromodulation integration: " << (neuromodulationOK ? "PASSED" : "FAILED") << std::endl;
            }
        }
        
        std::cout << std::endl;
        
        if (args.runQuickTest || args.verbose) {
            std::cout << "--- Checkpoint Test ---" << std::endl;
            bool checkpointOK = safeExecute("checkpoint test", [&]() {
                return experiment.testCheckpointing();
            });
            
            if (args.verbose) {
                std::cout << "Checkpoint test: " << (checkpointOK ? "PASSED" : "FAILED") << std::endl;
            }
        }
        
        std::cout << std::endl;
        
        if (args.runQuickTest || args.verbose) {
            std::cout << "--- Replay Test ---" << std::endl;
            bool replayOK = safeExecute("replay test", [&]() {
                return experiment.testReplay();
            });
            
            if (args.verbose) {
                std::cout << "Replay test: " << (replayOK ? "PASSED" : "FAILED") << std::endl;
            }
        }
        
        std::cout << std::endl;
        
        // Run full experiment with error handling
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        
        Phase6Config config = createConfig(args);
        
        Phase6IntegrationResult result = safeExecute("full integration experiment", [&]() {
            return experiment.run(config);
        });
        
        if (!result.memoryWorkingMemoryIntegrated || !result.memoryEpisodicMemoryIntegrated) {
            std::cerr << "ERROR: Memory systems not properly integrated!" << std::endl;
            return 1;
        }
        
        if (!result.neuromodulationIntegrated || !result.predictionIntegrated) {
            std::cerr << "ERROR: Neuromodulation or prediction systems not integrated!" << std::endl;
            return 1;
        }
        
        // Display results
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Average synaptic weight: " << result.avgSynapticWeight << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
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
        
        // Success message
        if (result.memoryWorkingMemoryIntegrated && 
            result.memoryEpisodicMemoryIntegrated &&
            result.neuromodulationIntegrated &&
            result.predictionIntegrated) {
            std::cout << std::endl;
            std::cout << "=== SUCCESS ===" << std::endl;
            std::cout << "All critical systems are properly integrated!" << std::endl;
            std::cout << "The NLM brain is functioning as an integrated artificial neural system." << std::endl;
        } else {
            std::cerr << std::endl;
            std::cerr << "=== FAILURE ===" << std::endl;
            std::cerr << "Some systems are not properly integrated." << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}