// Improved Phase 6 Demo - Configurable Integration Test
//
// This demo provides better control over Phase 6 integration testing
// with proper reward calculation and configurable parameters.

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>

using namespace nlm;

// Configuration structure for Phase 6 demo
struct Phase6DemoConfig {
    uint64_t neuronCount = 500;
    uint64_t maxSteps = 5000;
    uint64_t regionCount = 4;
    float connectionProbability = 0.08f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    float noveltyThreshold = 0.15f;
    float curiosityRate = 0.05f;
    std::string checkpointPath = "/tmp/nlm_phase6_demo.ckpt";
    std::string configFile = "configs/phase6_default.cfg";
};

// Parse command line arguments for configuration
Phase6DemoConfig parseArguments(int argc, char* argv[]) {
    Phase6DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 9) == "--neurons=") {
            config.neuronCount = std::stoull(arg.substr(9));
        } else if (arg.substr(0, 8) == "--steps=") {
            config.maxSteps = std::stoull(arg.substr(8));
        } else if (arg.substr(0, 10) == "--regions=") {
            config.regionCount = std::stoull(arg.substr(10));
        } else if (arg.substr(0, 17) == "--connection=") {
            config.connectionProbability = std::stof(arg.substr(17));
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg.substr(0, 19) == "--checkpoint=") {
            config.checkpointPath = arg.substr(19);
        } else if (arg.substr(0, 19) == "--config-file=") {
            config.configFile = arg.substr(19);
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        }
    }
    
    return config;
}

void printUsage() {
    std::cout << "NLM Phase 6 Demo - Usage:" << std::endl;
    std::cout << "  nlm_phase6_demo [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons=<N>            Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=<N>              Number of simulation steps (default: 5000)" << std::endl;
    std::cout << "  --regions=<N>            Number of brain regions (default: 4)" << std::endl;
    std::cout << "  --connection=<P>         Connection probability (default: 0.08)" << std::endl;
    std::cout << "  --no-checkpoint          Disable checkpointing" << std::endl;
    std::cout << "  --no-replay              Disable replay" << std::endl;
    std::cout << "  --no-development         Disable development" << std::endl;
    std::cout << "  --checkpoint=<path>      Checkpoint file path (default: /tmp/nlm_phase6_demo.ckpt)" << std::endl;
    std::cout << "  --config-file=<path>     Configuration file (default: configs/phase6_default.cfg)" << std::endl;
    std::cout << "  --help, -h               Show this help message" << std::endl;
}

void printDemoHeader(const Phase6DemoConfig& config) {
    std::cout << "=== NLM Phase 6 Enhanced Integration Demo ===" << std::endl;
    std::cout << "Testing the fully integrated artificial brain system..." << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Connection Probability: " << config.connectionProbability << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
}

void printPerformanceSummary(const Phase6IntegrationResult& result) {
    std::cout << "=== Performance Summary ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "Memory Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "Wall Clock Time: " << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << std::endl;
}

void printIntegrationStatus(const Phase6IntegrationResult& result) {
    std::cout << "=== Integration Status ===" << std::endl;
    
    auto printStatus = [](const std::string& name, bool status, const std::string& indicator) {
        std::cout << "  " << name << ": " << indicator << (status ? " CONNECTED" : " DISCONNECTED") << std::endl;
    };
    
    printStatus("Working Memory", result.memoryWorkingMemoryIntegrated, "[+]");
    printStatus("Episodic Memory", result.memoryEpisodicMemoryIntegrated, "[+]");
    printStatus("Neuromodulation", result.neuromodulationIntegrated, "[+]");
    printStatus("Prediction System", result.predictionIntegrated, "[+]");
    printStatus("Development System", result.developmentIntegrated, "[+]");
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "[+] WORKING" : "[-] FAILED") << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6DemoConfig config = parseArguments(argc, argv);
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    printDemoHeader(config);
    
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
    
    // Run full experiment with user's configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config experimentConfig;
    experimentConfig.neuronCount = config.neuronCount;
    experimentConfig.maxSteps = config.maxSteps;
    experimentConfig.regionCount = config.regionCount;
    experimentConfig.connectionProbability = config.connectionProbability;
    experimentConfig.enableCheckpointing = config.enableCheckpointing;
    experimentConfig.enableReplay = config.enableReplay;
    experimentConfig.enableDevelopment = config.enableDevelopment;
    experimentConfig.checkpointPath = config.checkpointPath;
    
    auto result = experiment.run(experimentConfig);
    
    // Print results
    printPerformanceSummary(result);
    printIntegrationStatus(result);
    
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Phase 6 integration test completed successfully!" << std::endl;
    
    // Determine if the integration is successful
    bool overallSuccess = result.totalReward > 0.0f &&
                         result.memoryWorkingMemoryIntegrated &&
                         result.memoryEpisodicMemoryIntegrated &&
                         result.neuromodulationIntegrated &&
                         result.checkpointingWorks;
    
    if (overallSuccess) {
        std::cout << "✓ ALL SYSTEMS INTEGRATED SUCCESSFULLY" << std::endl;
        std::cout << "✓ The artificial brain is functioning as a unified system" << std::endl;
        return 0;
    } else {
        std::cout << "✗ INTEGRATION ISSUES DETECTED" << std::endl;
        std::cout << "  Note: Some systems may need additional work" << std::endl;
        return 1;
    }
}