/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <map>
#include <string>
#include <algorithm>

// Forward declarations for argument parsing
namespace nlm {
    class ArgumentParser {
    public:
        static void printHelp();
        static bool printVersion();
        static bool parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs);
        static std::string getVersion();
    };
}

namespace nlm {

void ArgumentParser::printHelp() {
    std::cout << "Usage: nlm_phase6_demo [options]" << std::endl;
    std::cout << "Neural learning machine Phase 6 demonstration - Integration test" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version, -v                 Show version information" << std::endl;
    std::cout << "  --neuron-count <count>        Number of neurons (default: 500)" << std::endl;
    std::cout << "  --max-steps <steps>            Maximum simulation steps (default: 2000)" << std::endl;
    std::cout << "  --verbose, -V                 Enable verbose output" << std::endl;
    std::cout << "  --output <file>               Save results to file" << std::endl;
    std::cout << "  --checkpoint-dir <dir>         Directory for checkpoint files" << std::endl;
    std::cout << "  --seed <seed>                 Random seed for reproducible results" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase6_demo --help" << std::endl;
    std::cout << "  nlm_phase6_demo --version" << std::endl;
    std::cout << "  nlm_phase6_demo --neuron-count 1000 --max-steps 5000" << std::endl;
    std::cout << "  nlm_phase6_demo --checkpoint-dir checkpoints" << std::endl;
}

bool ArgumentParser::printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << std::endl;
    std::cout << "Integration test verifies:" << std::endl;
    std::cout << "  - Memory systems connectivity" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
    std::cout << "  - Prediction system integration" << std::endl;
    std::cout << "  - Development system integration" << std::endl;
    std::cout << "  - Checkpoint save/load functionality" << std::endl;
    std::cout << "  - Replay and consolidation" << std::endl;
    std::cout << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  C++ Standard: C++20" << std::endl;
    std::cout << "  Author: NLM Project Contributors" << std::endl;
    std::cout << "  License: MIT" << std::endl;
    return true;
}

std::string ArgumentParser::getVersion() {
    return "NLM 0.1.0 (Phase 6: Final Integration)";
}

bool ArgumentParser::parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key value or --key=value patterns
        if (arg.find('--') == 0) {
            std::string key;
            std::string value;
            size_t equalsPos = arg.find('=');
            
            if (equalsPos != std::string::npos) {
                key = arg.substr(2, equalsPos - 2);
                value = arg.substr(equalsPos + 1);
            } else {
                key = arg.substr(2);
                // Check if next argument exists and doesn't start with --
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                } else {
                    value = "true"; // Flag argument
                }
            }
            
            // Convert key to canonical form (remove dashes)
            std::replace(key.begin(), key.end(), '-', '_');
            parsedArgs[key] = value;
        } else if (arg.find('-') == 0 && arg.size() > 1 && !std::isalnum(arg[1])) {
            // Handle short options like -h, -v
            std::string key = arg.substr(1, 1);
            std::string value = "true";
            
            // Check for combined short options like -hV
            if (arg.size() > 2) {
                for (size_t j = 1; j < arg.size(); ++j) {
                    std::string singleKey(1, arg[j]);
                    parsedArgs[singleKey] = "true";
                }
                continue;
            }
            
            parsedArgs[key] = value;
        }
    }
    return true;
}

} // namespace nlm

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::map<std::string, std::string> parsedArgs;
    if (!nlm::ArgumentParser::parseArgs(argc, argv, parsedArgs)) {
        std::cerr << "ERROR: Failed to parse arguments." << std::endl;
        return 1;
    }
    
    // Handle help and version flags
    if (parsedArgs.find("h") != parsedArgs.end() || parsedArgs.find("help") != parsedArgs.end()) {
        nlm::ArgumentParser::printHelp();
        return 0;
    }
    
    if (parsedArgs.find("v") != parsedArgs.end() || parsedArgs.find("version") != parsedArgs.end()) {
        nlm::ArgumentParser::printVersion();
        return 0;
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse and validate arguments
    int neuronCount = 500;
    int maxSteps = 2000;
    std::string checkpointDir = "";
    bool verbose = false;
    
    // Parse neuron count
    if (parsedArgs.find("neuron_count") != parsedArgs.end()) {
        try {
            neuronCount = std::stoi(parsedArgs["neuron_count"]);
            if (neuronCount <= 0) {
                std::cerr << "ERROR: Neuron count must be positive. Got: " << neuronCount << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid neuron count value: " << parsedArgs["neuron_count"] << std::endl;
            return 1;
        }
    }
    
    // Parse max steps
    if (parsedArgs.find("max_steps") != parsedArgs.end()) {
        try {
            maxSteps = std::stoi(parsedArgs["max_steps"]);
            if (maxSteps <= 0) {
                std::cerr << "ERROR: Max steps must be positive. Got: " << maxSteps << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid max steps value: " << parsedArgs["max_steps"] << std::endl;
            return 1;
        }
    }
    
    // Parse checkpoint directory
    if (parsedArgs.find("checkpoint_dir") != parsedArgs.end()) {
        checkpointDir = parsedArgs["checkpoint_dir"];
    }
    
    // Parse verbose flag
    if (parsedArgs.find("verbose") != parsedArgs.end() || parsedArgs.find("V") != parsedArgs.end()) {
        verbose = true;
    }
    
    // Validate parameters
    if (verbose) {
        std::cout << "Running with parameters:\n";
        std::cout << "  Neuron count: " << neuronCount << "\n";
        std::cout << "  Max steps: " << maxSteps << "\n";
        std::cout << "  Checkpoint dir: " << (checkpointDir.empty() ? "Default" : checkpointDir) << "\n";
        std::cout << "  Verbose mode: Enabled\n\n";
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
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = static_cast<size_t>(neuronCount);
    config.maxSteps = static_cast<size_t>(maxSteps);
    config.enableCheckpointing = !checkpointDir.empty();
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointDir = checkpointDir;
    
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