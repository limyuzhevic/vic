/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "\nPhase 6 Integration Demo Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --neurons N             Set number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S               Set simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint PATH       Set checkpoint file path (default: ./checkpoint.bin)" << std::endl;
    std::cout << "  --quick, -q             Use quick demo settings (100 neurons, 500 steps)" << std::endl;
    std::cout << "  --extended, -e          Use extended demo settings (1000 neurons, 10000 steps)" << std::endl;
    std::cout << "  --verbose, -v           Enable verbose logging" << std::endl;
}

bool parseArguments(int argc, char* argv[], Phase6Config& config, bool& verbose) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return false; // Exit after showing help
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = static_cast<size_t>(std::atoi(argv[++i]));
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = static_cast<size_t>(std::atoi(argv[++i]));
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg == "--quick" || arg == "-q") {
            config.neuronCount = 100;
            config.maxSteps = 500;
            config.checkpointPath = "./checkpoint_quick.bin";
        } else if (arg == "--extended" || arg == "-e") {
            config.neuronCount = 1000;
            config.maxSteps = 10000;
            config.checkpointPath = "./checkpoint_extended.bin";
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else {
            std::cerr << "ERROR: Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Default configuration
    Phase6Config config;
    bool verbose = false;
    
    // Parse command line arguments
    bool continueDemo = parseArguments(argc, argv, config, verbose);
    if (!continueDemo) {
        return 0; // Help was shown
    }
    
    if (verbose) {
        std::cout << "=== VERBOSE MODE ===" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Max Steps: " << config.maxSteps << std::endl;
        std::cout << "  Checkpoint Path: " << config.checkpointPath << std::endl;
        std::cout << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "This indicates that brain systems are not properly integrated." << std::endl;
        std::cerr << "Please check the logs above for detailed error information." << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems with error handling
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryTest = experiment.testMemoryIntegration();
    if (!memoryTest) {
        std::cerr << "WARNING: Memory integration test encountered issues." << std::endl;
        // Continue with other tests
    }
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodTest = experiment.testNeuromodulationIntegration();
    if (!neuromodTest) {
        std::cerr << "WARNING: Neuromodulation integration test encountered issues." << std::endl;
        // Continue with other tests
    }
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    bool checkpointTest = experiment.testCheckpointing();
    if (!checkpointTest) {
        std::cerr << "WARNING: Checkpoint test encountered issues." << std::endl;
        // Continue with other tests
    }
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    bool replayTest = experiment.testReplay();
    if (!replayTest) {
        std::cerr << "WARNING: Replay test encountered issues." << std::endl;
        // Continue with other tests
    }
    std::cout << std::endl;
    
    // Run full experiment with specified settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Starting experiment with:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpoint: " << config.checkpointPath << std::endl;
    std::cout << "  Enable Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "  Enable Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << "  Enable Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    // Calculate overall integration score
    int connectedSystems = 0;
    if (result.memoryWorkingMemoryIntegrated) connectedSystems++;
    if (result.memoryEpisodicMemoryIntegrated) connectedSystems++;
    if (result.neuromodulationIntegrated) connectedSystems++;
    if (result.predictionIntegrated) connectedSystems++;
    if (result.developmentIntegrated) connectedSystems++;
    
    int totalSystems = 7;
    float integrationScore = (static_cast<float>(connectedSystems) / totalSystems) * 100.0f;
    
    std::cout << "=== INTEGRATION SUMMARY ===" << std::endl;
    std::cout << "Systems Connected: " << connectedSystems << " out of " << totalSystems << std::endl;
    std::cout << "Integration Score: " << std::fixed << std::setprecision(1) << integrationScore << "%" << std::endl;
    
    if (integrationScore >= 85.0f) {
        std::cout << "\n✅ SUCCESS: Excellent integration!" << std::endl;
        std::cout << "The artificial brain demonstrates robust system integration." << std::endl;
    } else if (integrationScore >= 70.0f) {
        std::cout << "\n⚠️  GOOD: Integration successful with minor issues." << std::endl;
        std::cout << "Review the disconnected systems above for improvement opportunities." << std::endl;
    } else {
        std::cout << "\n❌ ISSUES: Integration incomplete." << std::endl;
        std::cout << "Multiple systems are not properly integrated. See detailed output above." << std::endl;
    }
    
    return 0;
}
