// NLM Phase 6 Demo - Final Integration
// 
// This is the main executable for Phase 6: Final Integration Demo
// Demonstrates the complete integrated artificial brain with all systems working together.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>

using namespace nlm;

// Helper function to print section headers
void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

// Helper function to print results
void printResult(const std::string& name, float value, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

// Parse command line arguments
void parseArgs(int argc, char* argv[], bool& enableCheckpointing, std::string& checkpointPath) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--no-checkpointing") {
            enableCheckpointing = false;
        } else if (arg.substr(0, 17) == "--checkpoint-path") {
            if (arg.find('=') != std::string::npos) {
                checkpointPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                checkpointPath = argv[++i];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6: Final Integration Demo ===" << std::endl;
    std::cout << "Testing the complete integrated artificial brain..." << std::endl;
    std::cout << "This demonstrates all Phase 6 systems working together." << std::endl << std::endl;

    // Parse command line arguments
    bool enableCheckpointing = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    parseArgs(argc, argv, enableCheckpointing, checkpointPath);

    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    printSection("Phase 6 Configuration");
    std::cout << "  Neuron count: 500" << std::endl;
    std::cout << "  Region count: 1" << std::endl;
    std::cout << "  Connection probability: 0.15" << std::endl;
    std::cout << "  Max simulation steps: 2000" << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    if (!checkpointPath.empty()) {
        std::cout << "  Checkpoint path: " << checkpointPath << std::endl;
    }
    std::cout << "  Development: Enabled" << std::endl;
    std::cout << "  Replay: Enabled" << std::endl;

    // Create and configure experiment
    printSection("Creating Phase 6 Integrated Experiment");
    Phase6IntegratedExperiment experiment;

    // Run integration verification
    printSection("Integration Verification");
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << "Integration verification: " << (integrationOK ? "PASSED" : "FAILED") << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        std::cerr << "This indicates that brain systems are not properly connected." << std::endl;
        return 1;
    }

    // Test individual system integrations
    printSection("Testing Individual System Integrations");
    
    std::cout << "Testing memory integration...";
    experiment.testMemoryIntegration();
    std::cout << " COMPLETED" << std::endl;
    
    std::cout << "Testing neuromodulation integration...";
    experiment.testNeuromodulationIntegration();
    std::cout << " COMPLETED" << std::endl;
    
    std::cout << "Testing checkpoint system...";
    experiment.testCheckpointing();
    std::cout << " COMPLETED" << std::endl;
    
    std::cout << "Testing replay system...";
    experiment.testReplay();
    std::cout << " COMPLETED" << std::endl;

    // Create configuration for main experiment
    printSection("Phase 6 Full Integration Experiment");
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = checkpointPath;

    std::cout << "Running full integration experiment..." << std::endl;
    std::cout << "This simulates a complete brain lifetime with learning and development." << std::endl << std::endl;

    auto result = experiment.run(config);

    // Display final results
    printSection("FINAL RESULTS");
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "Average synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << result.totalWallClockTime << " seconds" << std::endl;

    // Display integration status
    printSection("INTEGRATION STATUS");
    std::cout << "Working Memory:            " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory:           " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation:           " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Prediction System:         " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Development System:        " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Checkpointing:              " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "Replay System:             " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;

    // Determine overall success
    bool success = integrationOK && 
                   result.memoryWorkingMemoryIntegrated &&
                   result.memoryEpisodicMemoryIntegrated &&
                   result.neuromodulationIntegrated &&
                   result.predictionIntegrated &&
                   result.developmentIntegrated &&
                   result.checkpointingWorks &&
                   result.replayWorks;

    std::cout << std::endl;
    if (success) {
        std::cout << "=== SUCCESS ===" << std::endl;
        std::cout << "All Phase 6 systems are properly integrated!" << std::endl;
        std::cout << "The NLM brain functions as a complete artificial neural substrate" << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
    } else {
        std::cout << "=== INTEGRATION INCOMPLETE ===" << std::endl;
        std::cout << "Some systems are not properly integrated." << std::endl;
        std::cout << "Review the integration status above for details." << std::endl;
    }

    return success ? 0 : 1;
}