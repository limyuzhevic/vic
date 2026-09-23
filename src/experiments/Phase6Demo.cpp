void showHelp() {
    std::cout << "NLM Phase 6 Demo - Integration Test" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --verbose, -V           Enable verbose logging" << std::endl;
    std::cout << "  --batch, -b             Run in batch mode (no interactive output)" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --schema <name>         Set configuration schema (phase1,phase2,phase3,phase4,phase5,phase6)" << std::endl;
    std::cout << "  --key=value             Set configuration key to value" << std::endl;
    std::cout << "  -key value              Set configuration key to value (alternative format)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_phase6_demo --schema phase6 --config config.json --neuron_count 500" << std::endl;
    std::cout << "  ./nlm_phase6_demo --batch --verbose --schema phase6" << std::endl;
    std::cout << "  ./nlm_phase6_demo --help" << std::endl;
}

void showVersion() {
    std::cout << "NLM Phase 6 Integration Demo v1.0.0" << std::endl;
    std::cout << "Phase 6: Final Integration Test" << std::endl;
}

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Enhanced command line argument parsing
    std::string configFile = "configs/default.cfg";
    std::string schemaName = "phase6";
    bool verboseMode = false;
    bool batchMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help flag
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return 0;
        }
        
        // Handle version flag
        if (arg == "--version" || arg == "-v") {
            showVersion();
            return 0;
        }
        
        // Handle verbose flag
        if (arg == "--verbose" || arg == "-V") {
            verboseMode = true;
            Logger::getInstance().setLevel(Logger::Level::Debug);
            continue;
        }
        
        // Handle batch mode flag
        if (arg == "--batch" || arg == "-b") {
            batchMode = true;
            continue;
        }
        
        // Handle config file flag
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
            continue;
        }
        
        // Handle schema flag
        if (arg == "--schema" && i + 1 < argc) {
            schemaName = argv[++i];
            continue;
        }
        
        // Handle key=value pairs
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                // Set configuration (would need access to config object)
                NLM_LOG_INFO("Setting config " + key + " = " + value);
            }
        }
    }
    
    // Show demo information
    std::cout << "--- Demo Configuration ---" << std::endl;
    std::cout << "Config file: " << configFile << std::endl;
    std::cout << "Schema: " << schemaName << std::endl;
    std::cout << "Verbose mode: " << (verboseMode ? "enabled" : "disabled") << std::endl;
    std::cout << "Batch mode: " << (batchMode ? "enabled" : "disabled") << std::endl;
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
    
    // Test individual systems with enhanced diagnostics
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryTestOK = experiment.testMemoryIntegration();
    std::cout << (memoryTestOK ? "Memory integration: PASSED" : "Memory integration: FAILED") << std::endl << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodulationTestOK = experiment.testNeuromodulationIntegration();
    std::cout << (neuromodulationTestOK ? "Neuromodulation integration: PASSED" : "Neuromodulation integration: FAILED") << std::endl << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointTestOK = experiment.testCheckpointing();
    std::cout << (checkpointTestOK ? "Checkpoint test: PASSED" : "Checkpoint test: FAILED") << std::endl << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayTestOK = experiment.testReplay();
    std::cout << (replayTestOK ? "Replay test: PASSED" : "Replay test: FAILED") << std::endl << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    
    auto result = experiment.run(config);
    
    // Enhanced results reporting
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay System: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    // Success summary
    std::cout << std::endl;
    std::cout << "=== DEMO SUMMARY ===" << std::endl;
    int passedTests = 0;
    int totalTests = 4;
    
    if (integrationOK) passedTests++;
    std::cout << "Integration verification: " << (integrationOK ? "PASS" : "FAIL") << std::endl;
    
    if (memoryTestOK) passedTests++;
    std::cout << "Memory integration test: " << (memoryTestOK ? "PASS" : "FAIL") << std::endl;
    
    if (neuromodulationTestOK) passedTests++;
    std::cout << "Neuromodulation integration test: " << (neuromodulationTestOK ? "PASS" : "FAIL") << std::endl;
    
    if (checkpointTestOK) passedTests++;
    std::cout << "Checkpoint test: " << (checkpointTestOK ? "PASS" : "FAIL") << std::endl;
    
    if (replayTestOK) passedTests++;
    std::cout << "Replay test: " << (replayTestOK ? "PASS" : "FAIL") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Tests passed: " << passedTests << "/" << totalTests << std::endl;
    std::cout << (passedTests == totalTests ? "SUCCESS: All integration tests passed!" : "FAILURE: Some integration tests failed.") << std::endl;
    
    return (passedTests == totalTests) ? 0 : 1;
}
