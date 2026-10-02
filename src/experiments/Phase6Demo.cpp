#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

using namespace nlm;

void printPhase6DemoUsage() {
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --neurons COUNT               Set number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps COUNT                 Set number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --quick                       Skip individual system tests, run only integration test" << std::endl;
    std::cout << "  --enable-dev                  Enable development system" << std::endl;
    std::cout << "  --disable-dev                 Disable development system" << std::endl;
    std::cout << "  --enable-checkpoint           Enable checkpoint save/load" << std::endl;
    std::cout << "  --disable-checkpoint          Disable checkpoint save/load" << std::endl;
    std::cout << "  --enable-replay               Enable replay system" << std::endl;
    std::cout << "  --disable-replay              Disable replay system" << std::endl;
    std::cout << "  --debug                       Enable debug logging" << std::endl;
    std::cout << "  --quiet                       Minimal output (errors only)" << std::endl;
    std::cout << "  --validate-only              Run only integration verification, no simulation" << std::endl;
    std::cout << "  --checkpoint-path PATH        Set checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  ./nlm_phase6_demo --quick --debug" << std::endl;
    std::cout << "  ./nlm_phase6_demo --validate-only" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6Config config;
    bool quickMode = false;
    bool validateOnly = false;
    bool debugMode = false;
    bool quietMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
            std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
            printPhase6DemoUsage();
            return 0;
        }
        else if (arg == "--quick") {
            quickMode = true;
        }
        else if (arg == "--validate-only") {
            validateOnly = true;
        }
        else if (arg == "--debug") {
            debugMode = true;
        }
        else if (arg == "--quiet") {
            quietMode = true;
        }
        else if (arg.substr(0, 9) == "--neurons") {
            size_t neurons = 500;
            if (arg.find('=') != std::string::npos) {
                neurons = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neurons = std::stoul(argv[++i]);
            }
            config.neuronCount = neurons;
        }
        else if (arg.substr(0, 7) == "--steps") {
            size_t steps = 2000;
            if (arg.find('=') != std::string::npos) {
                steps = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
            config.maxSteps = steps;
        }
        else if (arg == "--enable-dev") {
            config.enableDevelopment = true;
        }
        else if (arg == "--disable-dev") {
            config.enableDevelopment = false;
        }
        else if (arg == "--enable-checkpoint") {
            config.enableCheckpointing = true;
        }
        else if (arg == "--disable-checkpoint") {
            config.enableCheckpointing = false;
        }
        else if (arg == "--enable-replay") {
            config.enableReplay = true;
        }
        else if (arg == "--disable-replay") {
            config.enableReplay = false;
        }
        else if (arg.substr(0, 17) == "--checkpoint-path") {
            std::string path = "./checkpoint_test.bin";
            if (arg.find('=') != std::string::npos) {
                path = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                path = argv[++i];
            }
            config.checkpointPath = path;
        }
    }
    
    try {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging based on mode
        Logger::getInstance().setLevel(debugMode ? Logger::Level::Debug : Logger::Level::Info);
        
        if (quietMode) {
            // Remove all log levels except errors
            auto& logger = Logger::getInstance();
            logger.clear();  // Clear all loggers
            auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
            logger.addLogger(consoleLogger);
        }
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        
        if (!validateOnly) {
            bool integrationOK = experiment.verifyIntegration();
            
            std::cout << std::endl;
            
            if (!integrationOK) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return 1;
            }
            
            std::cout << "Integration verification passed!" << std::endl << std::endl;
        }
        
        if (!quickMode && !validateOnly) {
            // Test individual systems only if not in quick or validate-only mode
            std::cout << "--- Memory Integration Test ---" << std::endl;
            experiment.testMemoryIntegration();
            std::cout << std::endl;
            
            std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
            experiment.testNeuromodulationIntegration();
            std::cout << std::endl;
            
            std::cout << "--- Checkpoint Test ---" << std::endl;
            experiment.testCheckpointing();
            std::cout << std::endl;
            
            std::cout << "--- Replay Test ---" << std::endl;
            experiment.testReplay();
            std::cout << std::endl;
        }
        
        // Run full experiment with configured settings
        if (!validateOnly) {
            std::cout << "--- Full Integration Experiment ---" << std::endl;
            
            // Validate configuration
            if (config.neuronCount < 10 || config.neuronCount > 10000) {
                throw std::runtime_error("Invalid neuron count: " + std::to_string(config.neuronCount) +
                                        ". Must be between 10 and 10000.");
            }
            
            if (config.maxSteps < 10 || config.maxSteps > 100000) {
                throw std::runtime_error("Invalid max steps: " + std::to_string(config.maxSteps) +
                                        ". Must be between 10 and 100000.");
            }
            
            auto result = experiment.run(config);
            
            std::cout << std::endl;
            std::cout << "=== FINAL RESULTS ===" << std::endl;
            std::cout << "Total reward: " << result.totalReward << std::endl;
            std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
            std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
            std::cout << std::endl;
            
            std::cout << "=== INTEGRATION STATUS ===" << std::endl;
            std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << std::endl;
            
            std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        }
        
        if (validateOnly) {
            std::cout << std::endl;
            std::cout << "=== VALIDATION COMPLETE ===" << std::endl;
            std::cout << "All systems verified and ready for integration." << std::endl;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
