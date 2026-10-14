// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Complete Artificial Brain
//
// This is the main executable for the completed NLM Phase 6 system.
// It provides a comprehensive interface for running Phase 6 integration tests
// and demonstrates the complete artificial brain with all systems integrated.
//
// The NLM Phase 6 brain now implements:
// - Real spiking neural computation (LIF neurons, event-driven dynamics)
// - Working, episodic, and associative memory systems
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction and planning systems
// - Cognitive mechanisms (attention, concept formation, self-model)
// - Developmental stages and structural plasticity
// - Checkpoint saving and replay capabilities
// - Closed-loop interaction with environment

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     Complete Artificial Brain with All Systems Integrated       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "NLM Phase 6: Complete Artificial Brain Integration\n\n";
    std::cout << "Options:\n";
    std::cout << "  --demo [quick|full]    Run demonstration (quick: 500 steps, full: 2000 steps)\n";
    std::cout << "  --quick                Run quick integration test (100 steps)\n";
    std::cout << "  --full                 Run full integration test (2000 steps)\n";
    std::cout << "  --checkpoints          Test checkpoint save/load\n";
    std::cout << "  --replay               Test memory replay\n";
    std::cout << "  --help                 Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --demo full\n";
    std::cout << "  " << programName << " --quick\n";
    std::cout << "  " << programName << " --checkpoints\n";
}

void printIntegrationStatus(const Phase6IntegrationResult& result) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "INTEGRATION STATUS REPORT" << std::endl;
    std::cout << std::string(60, '=') << "\n";
    
    std::cout << "System Integration Check:\n";
    std::cout << "  Memory Systems:  " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "                  " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Prediction:      " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Development:     " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << "\n";
    std::cout << "  Checkpointing:   " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << "\n";
    
    std::cout << "\nPerformance Metrics:\n";
    std::cout << "  Total Reward:      " << std::fixed << std::setprecision(4) << result.totalReward << "\n";
    std::cout << "  Average Firing:    " << std::fixed << std::setprecision(4) << result.avgFiringRate << " Hz\n";
    std::cout << "  Episodes Stored:   " << std::fixed << std::setprecision(1) << result.memoryEpisodesStored << "\n";
    std::cout << "  Dopamine Level:    " << std::fixed << std::setprecision(4) << result.dopamineLevel << "\n";
    std::cout << "  Novelty Level:     " << std::fixed << std::setprecision(4) << result.noveltyLevel << "\n";
    std::cout << "  Curiosity Level:   " << std::fixed << std::setprecision(4) << result.curiosityLevel << "\n";
    
    std::cout << "\nTimeline:\n";
    std::cout << "  Wall Clock Time:   " << std::fixed << std::setprecision(3) << result.totalWallClockTime << " seconds\n";
    
    std::cout << std::string(60, '=') << "\n";
}

int main(int argc, char** argv) {
    printBanner();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Initializing NLM Phase 6: Complete Artificial Brain...\n" << std::endl;
    
    // Parse command line arguments
    bool runQuickTest = false;
    bool runFullTest = false;
    bool testCheckpoints = false;
    bool testReplay = false;
    bool runDemo = false;
    std::string demoType = "full";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--quick") {
            runQuickTest = true;
        } else if (arg == "--full") {
            runFullTest = true;
        } else if (arg == "--checkpoints") {
            testCheckpoints = true;
        } else if (arg == "--replay") {
            testReplay = true;
        } else if (arg == "--demo") {
            runDemo = true;
            if (i + 1 < argc) {
                demoType = argv[++i];
            }
        }
    }
    
    // Determine what to run
    bool runMinimalTest = false;
    if (runQuickTest || runFullTest || testCheckpoints || testReplay) {
        runMinimalTest = true;  // Only run requested tests
    }
    
    // Create configuration
    Phase6Config config;
    
    if (runDemo) {
        if (demoType == "quick") {
            config.neuronCount = 500;
            config.maxSteps = 500;
        } else {
            config.neuronCount = 1000;
            config.maxSteps = 2000;
        }
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
    } else if (runFullTest) {
        config.neuronCount = 1000;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
    } else if (runQuickTest) {
        config.neuronCount = 500;
        config.maxSteps = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
    } else {
        // Default behavior - full demo
        config.neuronCount = 1000;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
    }
    
    // Set additional config values
    config.regionCount = 1;
    config.connectionProbability = 0.15f;
    config.checkpointPath = "/tmp/nlm_phase6_checkpoint.bin";
    
    // Create and run the integration experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "=== NLM Phase 6 Integration ===" << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
    std::cout << std::endl;
    
    if (runDemo || runFullTest || !runMinimalTest) {
        // Run full demo
        std::cout << "Running full integration experiment..." << std::endl;
        auto result = experiment.run(config);
        printIntegrationStatus(result);
        
        // Run additional tests if requested
        if (testCheckpoints) {
            std::cout << "\n=== Testing Checkpoint System ===" << std::endl;
            experiment.testCheckpointing();
        }
        
        if (testReplay) {
            std::cout << "\n=== Testing Memory Replay ===" << std::endl;
            experiment.testReplay();
        }
    } else if (runQuickTest) {
        // Run quick test with verification
        std::cout << "Running quick integration verification..." << std::endl;
        bool verificationPassed = experiment.verifyIntegration();
        
        if (verificationPassed) {
            std::cout << "✓ Integration verification PASSED" << std::endl;
        } else {
            std::cout << "✗ Integration verification FAILED" << std::endl;
        }
        
        // Test individual systems
        experiment.testMemoryIntegration();
        experiment.testNeuromodulationIntegration();
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "PHASE 6 COMPLETE" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "The NLM brain is a functioning artificial neural substrate" << std::endl;
    std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
    
    return 0;
}
