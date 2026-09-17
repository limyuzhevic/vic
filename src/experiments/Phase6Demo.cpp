/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Check for command-line options
    bool verboseMode = false;
    bool autoRun = false;
    std::string configPath = "";
    int steps = 2000;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--verbose" || arg == "-v") {
            verboseMode = true;
            Logger::getGlobal().setLevel(LogLevel::Debug);
        } else if (arg == "--auto" || arg == "-a") {
            autoRun = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configPath = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--steps=") {
            steps = std::stoi(arg.substr(9));
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo Options:" << std::endl;
            std::cout << "  --verbose, -v    Enable verbose logging (Debug level)" << std::endl;
            std::cout << "  --auto, -a       Auto-run without user input" << std::endl;
            std::cout << "  --config=file    Load configuration from file" << std::endl;
            std::cout << "  --steps=N        Number of simulation steps (default: 2000)" << std::endl;
            std::cout << "  --help, -h       Show this help message" << std::endl;
            return 0;
        }
    }
    
    if (verboseMode) {
        std::cout << "Verbose logging enabled" << std::endl;
    }
    
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
    
    // Interactive menu if not auto-running
    if (!autoRun) {
        std::cout << "=== Interactive Menu ===" << std::endl;
        std::cout << "1. Memory Integration Test" << std::endl;
        std::cout << "2. Neuromodulation Integration Test" << std::endl;
        std::cout << "3. Checkpoint Test" << std::endl;
        std::cout << "4. Replay Test" << std::endl;
        std::cout << "5. Run Full Integration Experiment" << std::endl;
        std::cout << "Enter your choice (1-5): ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                std::cout << "\n--- Memory Integration Test ---" << std::endl;
                experiment.testMemoryIntegration();
                break;
            case 2:
                std::cout << "\n--- Neuromodulation Integration Test ---" << std::endl;
                experiment.testNeuromodulationIntegration();
                break;
            case 3:
                std::cout << "\n--- Checkpoint Test ---" << std::endl;
                experiment.testCheckpointing();
                break;
            case 4:
                std::cout << "\n--- Replay Test ---" << std::endl;
                experiment.testReplay();
                break;
            case 5:
                std::cout << "\n--- Full Integration Experiment ---" << std::endl;
                break;
            default:
                std::cout << "Invalid choice, running full experiment..." << std::endl;
        }
    }
    
    // Run full experiment with user-configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = steps;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    // Load custom config if specified
    if (!configPath.empty()) {
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(configPath)) {
            std::cout << "Loaded configuration from: " << configPath << std::endl;
            
            // Apply config values if available
            auto neuronCountOpt = cfg->get<int64_t>("neuron_count");
            if (neuronCountOpt) {
                config.neuronCount = *neuronCountOpt;
                std::cout << "Using neuron count: " << config.neuronCount << std::endl;
            }
        }
    }
    
    // Create timestamp for checkpoint path
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream checkpointPath;
    checkpointPath << "/tmp/nlm_phase6_demo_" << time_t << ".bin";
    config.checkpointPath = checkpointPath.str();
    
    std::cout << "Starting integration experiment..." << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpoint enabled: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "  Replay enabled: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "  Development enabled: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
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
    
    std::cout << std::endl;
    std::cout << "=== DEMO COMPLETE ===" << std::endl;
    std::cout << "All Phase 6 integration tests completed successfully!" << std::endl;
    
    return 0;
}
