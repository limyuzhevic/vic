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
    
    // Parse command line arguments for configuration
    Phase6Config config;
    bool demoMode = true;
    std::string checkpointFile = "./checkpoint.bin";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--quick" || arg == "-q") {
            demoMode = true;
            config.neuronCount = 100;
            config.maxSteps = 500;
            config.enableCheckpointing = false;
            std::cout << "Quick demo mode enabled" << std::endl;
        } else if (arg == "--full" || arg == "-f") {
            demoMode = false;
            config.neuronCount = 1000;
            config.maxSteps = 5000;
            config.enableCheckpointing = true;
            std::cout << "Full demo mode enabled" << std::endl;
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointFile = argv[++i];
            config.checkpointPath = checkpointFile;
            config.enableCheckpointing = true;
            std::cout << "Checkpoint file: " << checkpointFile << std::endl;
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoi(argv[++i]);
            std::cout << "Neuron count: " << config.neuronCount << std::endl;
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoi(argv[++i]);
            std::cout << "Max steps: " + std::to_string(config.maxSteps) << std::endl;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo - Usage:" << std::endl;
            std::cout << "  nlm_phase6_demo [--quick|-q] [--full|-f] [--checkpoint <file>] [--neurons <count>] [--steps <count>] [--help|-h]" << std::endl;
            std::cout << "  --quick or -q: Run quick demo with 100 neurons for 500 steps (default)" << std::endl;
            std::cout << "  --full or -f: Run full demo with 1000 neurons for 5000 steps with checkpointing" << std::endl;
            std::cout << "  --checkpoint <file>: Enable checkpointing and specify checkpoint file" << std::endl;
            std::cout << "  --neurons <count>: Set custom neuron count" << std::endl;
            std::cout << "  --steps <count>: Set custom step count" << std::endl;
            std::cout << "  --help or -h: Show this help message" << std::endl;
            return 0;
        }
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
    
    // Run full experiment with user-configurable settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    
    if (demoMode) {
        // Use demo settings for quick testing
        config.neuronCount = config.neuronCount == 1000 ? 500 : config.neuronCount;
        config.maxSteps = config.maxSteps == 10000 ? 2000 : config.maxSteps;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = checkpointFile;
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
    
    // Show usage summary
    std::cout << std::endl;
    std::cout << "=== USAGE SUMMARY ===" << std::endl;
    std::cout << "Run './nlm_phase6_demo --help' for more configuration options" << std::endl;
    std::cout << "Available modes: --quick (default), --full, or custom configuration" << std::endl;
    
    return 0;
}
