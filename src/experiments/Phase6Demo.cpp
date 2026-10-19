/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>

using namespace nlm;

void printHelp(const std::string& programName) {
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Integration test verifying all brain systems are connected:" << std::endl;
    std::cout << "  - Working Memory: Maintaining and recalling information" << std::endl;
    std::cout << "  - Episodic Memory: Storing full experiences" << std::endl;
    std::cout << "  - Neuromodulation: Dopamine-based learning" << std::endl;
    std::cout << "  - Prediction: Forward and inverse models" << std::endl;
    std::cout << "  - Development: Adaptive plasticity" << std::endl;
    std::cout << "  - Checkpointing: State saving and replay" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --neuron-count=N              Set number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps=N                 Set maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --enable-checkpointing        Enable checkpointing (default: true)" << std::endl;
    std::cout << "  --disable-checkpointing       Disable checkpointing" << std::endl;
    std::cout << "  --enable-replay               Enable replay functionality (default: true)" << std::endl;
    std::cout << "  --disable-replay              Disable replay functionality" << std::endl;
    std::cout << "  --enable-development          Enable development mechanisms (default: true)" << std::endl;
    std::cout << "  --disable-development         Disable development mechanisms" << std::endl;
    std::cout << "  --seed=N                      Set random seed for reproducibility" << std::endl;
    std::cout << "  --config=FILE                 Load configuration from FILE" << std::endl;
    std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                          Run with default settings" << std::endl;
    std::cout << "  " << programName << " --neuron-count=1000 --max-steps=5000" << std::endl;
    std::cout << "  " << programName << " --disable-checkpointing --disable-replay" << std::endl;
    std::cout << "  " << programName << " --help" << std::endl;
    std::cout << std::endl;
    std::cout << "The integration test verifies that all brain subsystems work together" << std::endl;
    std::cout << "and can recover from failures through checkpointing and replay." << std::endl;
}

void trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    Phase6Config config;
    bool verbose = false;
    bool showHelp = false;
    std::string configFile = "";
    
    // Check for help flag first
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        }
    }
    
    // Show help and exit if requested
    if (showHelp) {
        printHelp(argv[0]);
        return 0;
    }
    
    // Parse remaining arguments with validation
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            // Already handled above
            continue;
        }
        
        // Parse --neuron-count=N with validation
        if (arg.rfind("--neuron-count=", 0) == 0) {
            try {
                size_t pos;
                int value = std::stoi(arg.substr(16), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --neuron-count: '" << arg.substr(16) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                if (value < 100 || value > 10000) {
                    std::cerr << "Error: --neuron-count must be between 100 and 10000" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                config.neuronCount = static_cast<size_t>(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --neuron-count: '" << arg.substr(16) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --neuron-count out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        // Parse --max-steps=N with validation
        else if (arg.rfind("--max-steps=", 0) == 0) {
            try {
                size_t pos;
                int value = std::stoi(arg.substr(12), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --max-steps: '" << arg.substr(12) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                if (value < 100 || value > 100000) {
                    std::cerr << "Error: --max-steps must be between 100 and 100000" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                config.maxSteps = static_cast<size_t>(value);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --max-steps: '" << arg.substr(12) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --max-steps out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        // Parse --enable-checkpointing / --disable-checkpointing
        else if (arg == "--enable-checkpointing") {
            config.enableCheckpointing = true;
        } else if (arg == "--disable-checkpointing") {
            config.enableCheckpointing = false;
        }
        // Parse --enable-replay / --disable-replay
        else if (arg == "--enable-replay") {
            config.enableReplay = true;
        } else if (arg == "--disable-replay") {
            config.enableReplay = false;
        }
        // Parse --enable-development / --disable-development
        else if (arg == "--enable-development") {
            config.enableDevelopment = true;
        } else if (arg == "--disable-development") {
            config.enableDevelopment = false;
        }
        // Parse --seed=N
        else if (arg.rfind("--seed=", 0) == 0) {
            try {
                size_t pos;
                unsigned long value = std::stoul(arg.substr(7), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --seed: '" << arg.substr(7) << "'" << std::endl;
                    printHelp(argv[0]);
                    return 1;
                }
                // Note: Random seed setting would require access to Random class
                // For now, we'll just log it
                std::cout << "Note: Random seed set to " << value << " (not yet implemented in random system)" << std::endl;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --seed: '" << arg.substr(7) << "'" << std::endl;
                printHelp(argv[0]);
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --seed out of range" << std::endl;
                printHelp(argv[0]);
                return 1;
            }
        }
        // Parse --config=FILE
        else if (arg.rfind("--config=", 0) == 0) {
            configFile = arg.substr(9);
        }
        // Parse --config FILE
        else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        }
        // Parse --verbose, -v
        else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
        else {
            // Unknown argument - show help
            std::cerr << "Error: Unknown argument: " << arg << std::endl;
            printHelp(argv[0]);
            return 1;
        }
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(verbose ? Logger::Level::Debug : Logger::Level::Info);
    
    // Display configuration summary
    std::cout << "=== Configuration ===" << std::endl;
    std::cout << "Neuron count: " << config.neuronCount << std::endl;
    std::cout << "Max steps: " << config.maxSteps << std::endl;
    std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
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
    
    // Run full experiment with settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
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
}