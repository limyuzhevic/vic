/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Command line usage examples:
 *   ./nlm_phase6_demo              # Run default integration test
 *   ./nlm_phase6_demo --help        # Show help
 *   ./nlm_phase6_demo --version     # Show version
 *   ./nlm_phase6_demo --neurons 1000     # Set neuron count
 *   ./nlm_phase6_demo --steps 5000        # Set max steps
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help              Display this help message and exit\n";
    std::cout << "  --version           Display version information and exit\n";
    std::cout << "  --neurons N         Set number of neurons (default: 500)\n";
    std::cout << "  --steps N           Set max simulation steps (default: 2000)\n";
    std::cout << "  --world W           Set world size (default: 16x16)\n";
    std::cout << "  --checkpoint P      Set checkpoint path (default: ./checkpoint.bin)\n";
}

void printVersion() {
    std::cout << "NLM Phase 6 Demo - Integration Test v0.1.0\n";
    std::cout << "Final integration test for all brain systems.\n";
}

void validateNeuronCount(size_t count) {
    if (count == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    if (count > 1000000) {
        throw std::invalid_argument("Neuron count too large (max: 1,000,000)");
    }
}

void validateStepCount(size_t count) {
    if (count == 0) {
        throw std::invalid_argument("Step count must be greater than 0");
    }
    if (count > 1000000) {
        throw std::invalid_argument("Step count too large (max: 1,000,000)");
    }
}

void validateNeuronCount(size_t count) {
    if (count == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    if (count > 1000000) {
        throw std::invalid_argument("Neuron count too large (max: 1,000,000)");
    }
}

void validateStepCount(size_t count) {
    if (count == 0) {
        throw std::invalid_argument("Step count must be greater than 0");
    }
    if (count > 1000000) {
        throw std::invalid_argument("Step count too large (max: 1,000,000)");
    }
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6Config config;
    bool showHelp = false;
    bool showVersion = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            showHelp = true;
        } else if (arg == "--version") {
            showVersion = true;
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoull(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--world" && i + 1 < argc) {
            // Parse world size like "16" or "16x16"
            std::string worldArg = argv[++i];
            size_t pos = worldArg.find('x');
            if (pos != std::string::npos) {
                config.regionCount = std::stoull(worldArg.substr(0, pos));
                config.regionCount = std::stoull(worldArg.substr(pos + 1));
            } else {
                size_t worldSize = std::stoull(worldArg);
                config.neuronCount = worldSize * 10; // Heuristic relationship
            }
        } else if (arg.find("--checkpoint") != std::string::npos) {
            if (arg.find("=") != std::string::npos) {
                config.checkpointPath = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                config.checkpointPath = argv[++i];
            }
        } else {
            std::cerr << "ERROR: Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    try {
        // Validate inputs
        validateNeuronCount(config.neuronCount);
        validateStepCount(config.maxSteps);
        
        // Check help/version flags before running
        if (showHelp) {
            std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
            std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
            printUsage(argv[0]);
            return 0;
        }
        
        if (showVersion) {
            std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
            std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
            printVersion();
            return 0;
        }
        
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
        
        // Run full experiment with smaller settings for demo
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config fullConfig = config; // Use parsed config
        fullConfig.maxSteps = std::min(fullConfig.maxSteps, static_cast<size_t>(2000)); // Cap at 2000 for demo
        
        auto result = experiment.run(fullConfig);
        
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
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 1;
    }
}
    
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
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
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
