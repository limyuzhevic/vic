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
    
    // Parse command-line arguments
    int neuronCount = 500;
    int maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    double developmentSpeed = 1.0;
    bool showDetailedStats = false;
    std::string checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--neurons") == 0 && i + 1 < argc) {
            neuronCount = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "--steps") == 0 && i + 1 < argc) {
            maxSteps = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "--no-checkpoint") == 0) {
            enableCheckpointing = false;
        } else if (strcmp(argv[i], "--no-replay") == 0) {
            enableReplay = false;
        } else if (strcmp(argv[i], "--no-dev") == 0) {
            enableDevelopment = false;
        } else if (strcmp(argv[i], "--dev-speed") == 0 && i + 1 < argc) {
            developmentSpeed = std::atof(argv[++i]);
        } else if (strcmp(argv[i], "--stats") == 0) {
            showDetailedStats = true;
        } else if (strcmp(argv[i], "--checkpoint") == 0 && i + 1 < argc) {
            checkpointPath = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            std::cout << "Usage: nlm_phase6_demo [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --neurons N          Number of neurons (default: 500)\n";
            std::cout << "  --steps N            Maximum simulation steps (default: 2000)\n";
            std::cout << "  --no-checkpoint       Disable checkpointing\n";
            std::cout << "  --no-replay           Disable replay system\n";
            std::cout << "  --no-dev              Disable developmental system\n";
            std::cout << "  --dev-speed N        Development speed multiplier (default: 1.0)\n";
            std::cout << "  --stats               Show detailed statistics during simulation\n";
            std::cout << "  --checkpoint PATH     Checkpoint file path (default: /tmp/nlm_checkpoint_demo.bin)\n";
            std::cout << "  -h, --help           Show this help message\n";
            return 0;
        }
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
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    config.developmentSpeed = developmentSpeed;
    config.checkpointPath = checkpointPath;
    config.showDetailedStats = showDetailedStats;
    
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
