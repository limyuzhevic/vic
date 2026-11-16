#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "=== NLM Phase 6 Integration Demo ===\n\n";
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help                    Show this help message\n";
    std::cout << "  --neuron-count <N>       Number of neurons (default: 500)\n";
    std::cout << "  --max-steps <N>          Maximum simulation steps (default: 2000)\n";
    std::cout << "  --checkpoint-path <path> Checkpoint file path (default: ./checkpoint_test.bin)\n";
    std::cout << "  --no-checkpointing       Disable checkpointing\n";
    std::cout << "  --no-replay              Disable replay\n";
    std::cout << "  --no-development         Disable development system\n";
    std::cout << "  --config <file>          Load configuration from file\n";
    std::cout << "  --performance            Run in performance mode (smaller network)\n";
    std::cout << "  --debug                  Enable debug logging\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --neuron-count 100 --max-steps 500\n";
    std::cout << "  " << programName << " --checkpoint-path /tmp/my_checkpoint.bin\n";
    std::cout << "  " << programName << " --no-checkpointing --no-replay\n\n";
    std::cout << "Phase 6 Integration Demo verifies that all brain systems are properly connected:\n\n";
    std::cout << "  Memory Systems (Working Memory + Episodic Memory)\n";
    std::cout << "  Neuromodulation (Dopamine, Curiosity, Novelty)\n";
    std::cout << "  Prediction System\n";
    std::cout << "  Development System\n";
    std::cout << "  Checkpoint System\n";
    std::cout << "  Replay System\n\n";
    std::cout << "The integration test ensures that all these systems work together as a coherent artificial brain.\n\n";
}

Phase6Config parseArgs(int argc, char* argv[]) {
    Phase6Config config;
    
    // Set defaults
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "./checkpoint_test.bin";
    config.connectionProbability = 0.1f;
    config.regionCount = 1;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        } else if (arg.substr(0, 17) == "--neuron-count=") {
            config.neuronCount = static_cast<size_t>(std::stoi(arg.substr(17)));
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            config.neuronCount = static_cast<size_t>(std::stoi(argv[++i]));
        } else if (arg.substr(0, 14) == "--max-steps=") {
            config.maxSteps = static_cast<size_t>(std::stoi(arg.substr(14)));
        } else if (arg == "--max-steps" && i + 1 < argc) {
            config.maxSteps = static_cast<size_t>(std::stoi(argv[++i]));
        } else if (arg.substr(0, 21) == "--checkpoint-path=") {
            config.checkpointPath = arg.substr(21);
        } else if (arg == "--checkpoint-path" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg.substr(0, 9) == "--config=") {
            // Load config from file (placeholder for full implementation)
            std::cout << "Config file loading not yet implemented: " << arg.substr(9) << std::endl;
        } else if (arg == "--config" && i + 1 < argc) {
            std::cout << "Config file loading not yet implemented: " << argv[++i] << std::endl;
        } else if (arg == "--performance") {
            // Performance mode: smaller network for faster execution
            config.neuronCount = 100;
            config.maxSteps = 500;
            config.enableCheckpointing = false;
        } else if (arg == "--debug") {
            Logger::getGlobal().setLevel(LogLevel::Debug);
        }
    }
    
    return config;
}
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
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
    
    // Additional integration diagnostics
    std::cout << "\n=== Integration Diagnostics ===" << std::endl;
    if (result.totalReward > 0) {
        std::cout << "✓ Reward system is working (agent got positive rewards)\n";
    } else {
        std::cout << "Note: Reward system may need tuning - adjust perception/action mapping\n";
    }
    
    if (result.avgFiringRate > 10 && result.avgFiringRate < 100) {
        std::cout << "✓ Neural firing rate is in healthy range\n";
    } else {
        std::cout << "Note: Firing rate may need adjustment (current: " << result.avgFiringRate << ")\n";
    }
    
    if (result.memoryEpisodesStored > 0) {
        std::cout << "✓ Memory systems are capturing experiences\n";
    } else {
        std::cout << "Note: Memory systems may need stimulation to store experiences\n";
    }
    
    if (result.checkpointingWorks) {
        std::cout << "✓ Checkpointing is working correctly\n";
    } else {
        std::cout << "✗ Checkpointing needs implementation fix\n";
    }
    
    return 0;
}
