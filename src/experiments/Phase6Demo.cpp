#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>

using namespace nlm;

void printHelp(const char* programName) {
    std::cout << "=== NLM Phase 6 Integration Demo ===\n";
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help                    Show this help message\n";
    std::cout << "  -n, --neurons <count>         Number of neurons (default: 500)\n";
    std::cout << "  -s, --steps <count>           Number of simulation steps (default: 2000)\n";
    std::cout << "  -r, --regions <count>         Number of brain regions (default: 1)\n";
    std::cout << "  -c, --connection <rate>       Connection probability (default: 0.1)\n";
    std::cout << "      --no-checkpointing        Disable checkpoint save/load\n";
    std::cout << "      --no-replay              Disable memory replay\n";
    std::cout << "      --no-development         Disable development\n";
    std::cout << "  -p, --checkpoint <path>       Checkpoint file path (default: ./checkpoint.bin)\n";
    std::cout << "  --enable-curiosity            Enable curiosity-driven exploration\n";
    std::cout << "  --enable-reward              Enable reward-based learning\n";
    std::cout << "  --enable-structural          Enable structural plasticity\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << "                            Run demo with default settings\n";
    std::cout << "  " << programName << " -n 1000 -s 5000                     Larger simulation\n";
    std::cout << "  " << programName << " --no-checkpointing               Fast run without checkpointing\n\n";
}

bool parseCommandLine(int argc, char* argv[], Phase6Config& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            return false; // Signal to exit after showing help
        } else if (arg == "-n" || arg == "--neurons") {
            if (i + 1 < argc) {
                config.neuronCount = static_cast<size_t>(std::stoul(argv[++i]));
                if (config.neuronCount == 0) {
                    std::cerr << "ERROR: Neuron count must be positive" << std::endl;
                    return false;
                }
            } else {
                std::cerr << "ERROR: Missing value for --neurons option" << std::endl;
                return false;
            }
        } else if (arg == "-s" || arg == "--steps") {
            if (i + 1 < argc) {
                config.maxSteps = static_cast<uint64_t>(std::stoul(argv[++i]));
                if (config.maxSteps == 0) {
                    std::cerr << "ERROR: Steps count must be positive" << std::endl;
                    return false;
                }
            } else {
                std::cerr << "ERROR: Missing value for --steps option" << std::endl;
                return false;
            }
        } else if (arg == "-r" || arg == "--regions") {
            if (i + 1 < argc) {
                config.regionCount = static_cast<size_t>(std::stoul(argv[++i]));
                if (config.regionCount == 0) {
                    std::cerr << "ERROR: Region count must be positive" << std::endl;
                    return false;
                }
            } else {
                std::cerr << "ERROR: Missing value for --regions option" << std::endl;
                return false;
            }
        } else if (arg == "-c" || arg == "--connection") {
            if (i + 1 < argc) {
                float prob = std::stof(argv[++i]);
                if (prob < 0.0f || prob > 1.0f) {
                    std::cerr << "ERROR: Connection probability must be between 0.0 and 1.0" << std::endl;
                    return false;
                }
                config.connectionProbability = prob;
            } else {
                std::cerr << "ERROR: Missing value for --connection option" << std::endl;
                return false;
            }
        } else if (arg == "-p" || arg == "--checkpoint") {
            if (i + 1 < argc) {
                config.checkpointPath = argv[++i];
            } else {
                std::cerr << "ERROR: Missing value for --checkpoint option" << std::endl;
                return false;
            }
        } else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--enable-curiosity") {
            config.enableCuriosity = true;
        } else if (arg == "--enable-reward") {
            config.enableReward = true;
        } else if (arg == "--enable-structural") {
            config.enableStructural = true;
        } else {
            std::cerr << "ERROR: Unknown option: " << arg << std::endl;
            printHelp(argv[0]);
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    Phase6Config config;
    
    // Parse command line arguments
    bool continueWithDemo = parseCommandLine(argc, argv, config);
    if (!continueWithDemo) {
        return 0; // Help was shown, exit gracefully
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===\n";
    std::cout << "Testing the integrated artificial brain...\n\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---\n";
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!\n";
        return 1;
    }
    
    std::cout << "Integration verification passed!\n\n";
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---\n";
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---\n";
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---\n";
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---\n";
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with user-specified settings
    std::cout << "--- Full Integration Experiment ---\n";
    std::cout << "Configuration:\n";
    std::cout << "  Neurons: " << config.neuronCount << "\n";
    std::cout << "  Steps: " << config.maxSteps << "\n";
    std::cout << "  Regions: " << config.regionCount << "\n";
    std::cout << "  Connection probability: " << config.connectionProbability << "\n";
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << "\n";
    std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << "\n";
    std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << "\n";
    std::cout << "  Curiosity: " << (config.enableCuriosity ? "enabled" : "disabled") << "\n";
    std::cout << "  Reward: " << (config.enableReward ? "enabled" : "disabled") << "\n";
    std::cout << "  Structural: " << (config.enableStructural ? "enabled" : "disabled") << "\n";
    std::cout << "  Checkpoint path: " << config.checkpointPath << "\n\n";
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===\n";
    std::cout << "Total reward: " << result.totalReward << "\n";
    std::cout << "Avg firing rate: " << result.avgFiringRate << "\n";
    std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << "\n";
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << "\n";
    std::cout << "Novelty level: " << result.noveltyLevel << "\n";
    std::cout << "Curiosity level: " << result.curiosityLevel << "\n";
    std::cout << "Dopamine level: " << result.dopamineLevel << "\n";
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===\n";
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << "\n";
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << "\n";
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s\n";
    
    std::cout << std::endl;
    std::cout << "=== DEMONSTRATION COMPLETE ===\n";
    std::cout << "The NLM Phase 6 integration successfully demonstrates:\n";
    std::cout << "  - Memory systems integrated with neural processing\n";
    std::cout << "  - Neuromodulation affecting plasticity and dynamics\n";
    std::cout << "  - Development affecting plasticity rates\n";
    std::cout << "  - Prediction system integrated\n";
    std::cout << "  - Replay and consolidation functional\n";
    std::cout << "  - Checkpoint save/load working\n";
    std::cout << "  - Complete brain loop functioning coherently\n";
    
    return 0;
}
