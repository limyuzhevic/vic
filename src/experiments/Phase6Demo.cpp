/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/CLI/CLI.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::string programName = "nlm_phase6_demo";
    std::string version = "0.1.0";
    std::string description = "NLM Phase 6 Integration Demo - Full brain system integration test";
    
    nlm::ArgumentParser parser(programName, version, description);
    
    // Add Phase 6 specific arguments
    parser.addArgument('', 'neurons', "Number of neurons (100-10000)", "500",
                      true, "positive-int");
    parser.addArgument('', 'steps', "Maximum simulation steps (100-100000)", "2000",
                      true, "positive-int");
    parser.addArgument('', 'enable-checkpoint', "Enable checkpointing", "",
                      false, "");
    parser.addArgument('', 'enable-replay', "Enable replay capabilities", "",
                      false, "");
    parser.addArgument('', 'enable-development', "Enable developmental processes", "",
                      false, "");
    
    parser.parse(argc, argv);
    
    // Check for help/version
    if (parser.hasArgument("help")) {
        parser.printHelp();
        return 0;
    }
    if (parser.hasArgument("version")) {
        parser.printVersion();
        return 0;
    }
    
    // Use logger controller if log-level is set
    std::string logLevel = parser.getValue("log-level");
    std::string logFile = parser.getValue("log-file");
    std::string logColors = !parser.hasArgument("no-colors") ? "true" : "false";
    
    if (!logLevel.empty()) {
        bool enableColors = (logColors == "true");
        nlm::LoggerController loggerController;
        loggerController.configure(logLevel, logFile, enableColors);
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment with CLI-configured settings
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
    
    // Run full experiment with CLI-configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = parser.getInt("neurons");
    config.maxSteps = parser.getInt("steps");
    config.enableCheckpointing = parser.hasArgument("enable-checkpoint");
    config.enableReplay = parser.hasArgument("enable-replay");
    config.enableDevelopment = parser.hasArgument("enable-development");
    
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
