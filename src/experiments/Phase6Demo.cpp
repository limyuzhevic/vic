/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Usage:
 *   ./nlm_phase6_demo                    # Run with default settings
 *   ./nlm_phase6_demo --help             # Show help
 *   ./nlm_phase6_demo --neurons=1000     # Set neuron count
 *   ./nlm_phase6_demo --steps=5000       # Set step count
 *   ./nlm_phase6_demo --checkpoint=./my_checkpoint.bin  # Set checkpoint path
 *   ./nlm_phase6_demo --no-checkpoint    # Disable checkpointing
 *   ./nlm_phase6_demo --no-replay        # Disable replay
 *   ./nlm_phase6_demo --no-development   # Disable development
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>

using namespace nlm;

void printUsage() {
    std::cout << "=== NLM Phase 6 Integration Demo Usage ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --neurons=<count>             Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps=<count>               Number of simulation steps (default: 10000)" << std::endl;
    std::cout << "  --checkpoint=<path>           Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --no-checkpoint               Disable checkpointing" << std::endl;
    std::cout << "  --no-replay                   Disable replay system" << std::endl;
    std::cout << "  --no-development              Disable development system" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --neurons=500 --steps=2000 --no-replay" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems
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
        
            // Parse command line arguments for configuration
        Phase6Config config;
        
        // Allow command-line overrides
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                return 0;
            }
            else if (arg.find("--neurons") != std::string::npos) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    config.neuronCount = std::stoi(arg.substr(pos + 1));
                } else if (i + 1 < argc) {
                    config.neuronCount = std::stoi(argv[++i]);
                }
            }
            else if (arg.find("--steps") != std::string::npos) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    config.maxSteps = std::stoull(arg.substr(pos + 1));
                } else if (i + 1 < argc) {
                    config.maxSteps = std::stoull(argv[++i]);
                }
            }
            else if (arg.find("--checkpoint") != std::string::npos) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    config.checkpointPath = arg.substr(pos + 1);
                } else if (i + 1 < argc) {
                    config.checkpointPath = argv[++i];
                }
            }
            else if (arg == "--no-checkpoint") {
                config.enableCheckpointing = false;
            }
            else if (arg == "--no-replay") {
                config.enableReplay = false;
            }
            else if (arg == "--no-development") {
                config.enableDevelopment = false;
            }
            else if (arg == "--verbose" || arg == "-v") {
                Logger::getInstance().setLevel(Logger::Level::Debug);
            }
            else if (arg == "--quiet" || arg == "-q") {
                Logger::getInstance().setLevel(Logger::Level::Error);
            }
            else if (arg == "--export-results" || arg == "--export") {
                config.exportResults = true;
            }
            else if (arg.find("--export-file") != std::string::npos) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    config.resultsFile = arg.substr(pos + 1);
                }
            }
            else if (arg == "--benchmark") {
                config.benchmarkMode = true;
            }
            else if (arg == "--progress") {
                config.showProgress = true;
            }
            else if (arg == "--simulation-speed") {
                if (i + 1 < argc) {
                    config.simulationSpeed = std::stod(argv[++i]);
                }
            }
            else if (arg.find("--region-count") != std::string::npos) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    config.regionCount = std::stoi(arg.substr(pos + 1));
                }
            }
            else if (arg.find("--connection-prob") != std::string::npos) {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    config.connectionProbability = std::stof(arg.substr(pos + 1));
                }
            }
        }
        
        // Run full experiment with configuration
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        
        // Update logging level based on mode
        if (config.verboseLogging) {
            Logger::getInstance().setLevel(Logger::Level::Debug);
        } else if (config.quietMode) {
            Logger::getInstance().setLevel(Logger::Level::Error);
        } else {
            Logger::getInstance().setLevel(Logger::Level::Info);
        }
        
        // Validate configuration
        if (config.neuronCount == 0) {
            std::cerr << "ERROR: neuronCount must be greater than 0" << std::endl;
            return 1;
        }
        if (config.maxSteps == 0) {
            std::cerr << "ERROR: maxSteps must be greater than 0" << std::endl;
            return 1;
        }
        if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
            std::cerr << "ERROR: connectionProbability must be between 0.0 and 1.0" << std::endl;
            return 1;
        }
        if (config.regionCount == 0) {
            std::cerr << "ERROR: regionCount must be greater than 0" << std::endl;
            return 1;
        }
        
        if (config.benchmarkMode) {
            std::cout << "=== BENCHMARK MODE ===" << std::endl;
            std::cout << "Running experiment to collect performance metrics..." << std::endl;
        } else if (config.exportResults) {
            std::cout << "=== RESULTS EXPORT MODE ===" << std::endl;
            std::cout << "Results will be exported to: " << config.resultsFile << std::endl;
        }
        
        std::cout << "Configuration: " << config.neuronCount << " neurons, " 
                  << config.maxSteps << " steps, " << config.regionCount << " regions" << std::endl;
        std::cout << "Connection probability: " << config.connectionProbability << std::endl;
        if (config.enableCheckpointing) {
            std::cout << "Checkpoint: " << config.checkpointPath << std::endl;
        }
        std::cout << "Logging level: ";
        if (config.verboseLogging) {
            std::cout << "DEBUG" << std::endl;
        } else if (config.quietMode) {
            std::cout << "ERROR ONLY" << std::endl;
        } else {
            std::cout << "INFO" << std::endl;
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
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}