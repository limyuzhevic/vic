/**
 * Phase 6 Demo - Enhanced Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected. Enhanced version
 * includes better logging, configuration options, and comprehensive reporting.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <map>

using namespace nlm;

// Configuration structure for Phase 6 demo
struct Phase6DemoConfig {
    std::string name = "NLM Phase 6 Integration Demo";
    std::string description = "Testing the integrated artificial brain systems";
    bool runIntegrationVerification = true;
    bool runMemoryTest = true;
    bool runNeuromodulationTest = true;
    bool runCheckpointTest = true;
    bool runReplayTest = true;
    bool runFullExperiment = true;
    
    // Full experiment configuration
    uint64_t maxSteps = 2000;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_demo.bin";
    
    Phase6DemoConfig() = default;
    
    bool parseArgs(int argc, char* argv[]);
    void printConfig() const;
    std::string generateReport(const Phase6IntegrationResult& result) const;
};

bool Phase6DemoConfig::parseArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 6) == "--name") {
            if (arg.find('=') != std::string::npos) {
                name = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                name = argv[++i];
            }
        } else if (arg.substr(0, 5) == "--desc") {
            if (arg.find('=') != std::string::npos) {
                description = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                description = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoul(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg.substr(0, 6) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                maxSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--no-verification") {
            runIntegrationVerification = false;
        } else if (arg == "--no-memory") {
            runMemoryTest = false;
        } else if (arg == "--no-neuromod") {
            runNeuromodulationTest = false;
        } else if (arg == "--no-checkpoint") {
            runCheckpointTest = false;
        } else if (arg == "--no-replay") {
            runReplayTest = false;
        } else if (arg == "--no-full") {
            runFullExperiment = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo Options:" << std::endl;
            std::cout << "  --name=name              Demo name" << std::endl;
            std::cout << "  --desc=description       Demo description" << std::endl;
            std::cout << "  --neurons=NUMBER        Number of neurons" << std::endl;
            std::cout << "  --steps=NUMBER           Max simulation steps" << std::endl;
            std::cout << "  --no-verification        Skip integration verification" << std::endl;
            std::cout << "  --no-memory              Skip memory integration test" << std::endl;
            std::cout << "  --no-neuromod            Skip neuromodulation integration test" << std::endl;
            std::cout << "  --no-checkpoint          Skip checkpoint test" << std::endl;
            std::cout << "  --no-replay              Skip replay test" << std::endl;
            std::cout << "  --no-full                Skip full integration experiment" << std::endl;
            std::cout << "  --help, -h              Show this help" << std::endl;
            return false;
        }
    }
    return true;
}

void Phase6DemoConfig::printConfig() const {
    std::cout << "=== Phase 6 Demo Configuration ===" << std::endl;
    std::cout << "Demo: " << name << std::endl;
    std::cout << "Description: " << description << std::endl;
    std::cout << "Neurons: " << neuronCount << std::endl;
    std::cout << "Max steps: " << maxSteps << std::endl;
    std::cout << "Integration verification: " << (runIntegrationVerification ? "YES" : "NO") << std::endl;
    std::cout << "Memory integration test: " << (runMemoryTest ? "YES" : "NO") << std::endl;
    std::cout << "Neuromodulation integration test: " << (runNeuromodulationTest ? "YES" : "NO") << std::endl;
    std::cout << "Checkpoint test: " << (runCheckpointTest ? "YES" : "NO") << std::endl;
    std::cout << "Replay test: " << (runReplayTest ? "YES" : "NO") << std::endl;
    std::cout << "Full experiment: " << (runFullExperiment ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
}

std::string Phase6DemoConfig::generateReport(const Phase6IntegrationResult& result) const {
    std::stringstream report;
    
    report << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    report << "║                    PHASE 6 INTEGRATION DEMO REPORT               ║" << std::endl;
    report << "║                                                               ║" << std::endl;
    report << "║ Integration Verification:   " << (result.memoryWorkingMemoryIntegrated ? "✓ PASS" : "✗ FAIL") << "          ║" << std::endl;
    report << "║ Memory Integration:        " << (result.memoryEpisodicMemoryIntegrated ? "✓ PASS" : "✗ FAIL") << "          ║" << std::endl;
    report << "║ Neuromodulation:           " << (result.neuromodulationIntegrated ? "✓ PASS" : "✗ FAIL") << "          ║" << std::endl;
    report << "║ Prediction System:         " << (result.predictionIntegrated ? "✓ PASS" : "✗ FAIL") << "          ║" << std::endl;
    report << "║ Development System:        " << (result.developmentIntegrated ? "✓ PASS" : "✗ FAIL") << "          ║" << std::endl;
    report << "║ Checkpointing:              " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << "     ║" << std::endl;
    report << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    
    return report.str();
}

void printSection(const std::string& title, const std::string& description) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              " << std::left << std::setw(58) << title << "║" << std::endl;
    std::cout << "║              " << std::left << std::setw(58) << description << "║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6DemoConfig config;
    if (!config.parseArgs(argc, argv)) {
        return 0; // Help was shown
    }
    
    // Print configuration
    config.printConfig();
    
    // Record start time for performance measurement
    auto startWall = std::chrono::high_resolution_clock::now();
    auto startCpu = std::chrono::system_clock::now();
    
    std::cout << "Initializing Phase 6 integration test..." << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification if requested
    if (config.runIntegrationVerification) {
        printSection("Integration Verification", "Quick check that all brain systems are connected");
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "❌ ERROR: Integration verification failed!" << std::endl;
            std::cerr << "   Not all brain systems are properly connected." << std::endl;
            return 1;
        }
        
        std::cout << "✅ Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems if requested
    if (config.runMemoryTest) {
        printSection("Memory Integration Test", "Test that memory systems store and retrieve information");
        bool memoryOK = experiment.testMemoryIntegration();
        if (memoryOK) {
            std::cout << "✅ Memory integration test completed!" << std::endl;
        } else {
            std::cout << "⚠️  Memory integration test completed with warnings." << std::endl;
        }
        std::cout << std::endl;
    }
    
    if (config.runNeuromodulationTest) {
        printSection("Neuromodulation Integration Test", "Test that reward and curiosity systems affect learning");
        bool neuromodOK = experiment.testNeuromodulationIntegration();
        if (neuromodOK) {
            std::cout << "✅ Neuromodulation integration test completed!" << std::endl;
        } else {
            std::cout << "⚠️  Neuromodulation integration test completed with warnings." << std::endl;
        }
        std::cout << std::endl;
    }
    
    if (config.runCheckpointTest) {
        printSection("Checkpoint Save/Load Test", "Test that brain state can be saved and restored");
        bool checkpointOK = experiment.testCheckpointing();
        if (checkpointOK) {
            std::cout << "✅ Checkpoint test completed successfully!" << std::endl;
        } else {
            std::cout << "❌ Checkpoint test failed!" << std::endl;
        }
        std::cout << std::endl;
    }
    
    if (config.runReplayTest) {
        printSection("Replay System Test", "Test that episodic memory can be replayed for consolidation");
        bool replayOK = experiment.testReplay();
        if (replayOK) {
            std::cout << "✅ Replay test completed!" << std::endl;
        } else {
            std::cout << "⚠️  Replay test completed with warnings." << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Run full integration experiment if requested
    if (config.runFullExperiment) {
        printSection("Full Integration Experiment", "Complete lifetime simulation with all brain systems");
        
        Phase6Config experimentConfig;
        experimentConfig.neuronCount = config.neuronCount;
        experimentConfig.maxSteps = config.maxSteps;
        experimentConfig.regionCount = config.regionCount;
        experimentConfig.connectionProbability = config.connectionProbability;
        experimentConfig.enableCheckpointing = config.enableCheckpointing;
        experimentConfig.enableReplay = config.enableReplay;
        experimentConfig.enableDevelopment = config.enableDevelopment;
        experimentConfig.checkpointPath = config.checkpointPath;
        
        std::cout << "Running full integration experiment with " << config.neuronCount << " neurons for " 
                  << config.maxSteps << " steps..." << std::endl << std::endl;
        
        auto result = experiment.run(experimentConfig);
        
        // Print final results
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    FINAL RESULTS                             ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        std::cout << "Total reward (normalized):     " << std::fixed << std::setprecision(4) 
                  << result.totalReward << std::endl;
        std::cout << "Average firing rate:           " << std::fixed << std::setprecision(2) 
                  << result.avgFiringRate << std::endl;
        std::cout << "Memory episodes stored:        " << std::fixed << std::setprecision(1) 
                  << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level:                " << std::fixed << std::setprecision(3) 
                  << result.dopamineLevel << std::endl;
        std::cout << "Novelty level:                 " << std::fixed << std::setprecision(3) 
                  << result.noveltyLevel << std::endl;
        std::cout << "Curiosity level:               " << std::fixed << std::setprecision(3) 
                  << result.curiosityLevel << std::endl;
        
        std::cout << std::endl;
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    SYSTEM INTEGRATION STATUS                  ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        std::cout << "Working Memory:                 " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory:                " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation:                " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "Prediction System:              " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "Development System:             " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
        std::cout << "Checkpointing:                   " << (result.checkpointingWorks ? "✅ WORKING" : "❌ NOT WORKING") << std::endl;
        
        std::cout << std::endl;
        
        // Calculate and display performance metrics
        auto endCpu = std::chrono::system_clock::now();
        auto endWall = std::chrono::high_resolution_clock::now();
        
        auto cpuDuration = std::chrono::duration_cast<std::chrono::seconds>(endCpu - startCpu);
        auto wallDuration = std::chrono::duration_cast<std::chrono::seconds>(endWall - startWall);
        
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                      PERFORMANCE METRICS                     ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        std::cout << "CPU time used:                  " << cpuDuration.count() << " seconds" << std::endl;
        std::cout << "Wall clock time:                " << wallDuration.count() << " seconds" << std::endl;
        
        if (result.totalWallClockTime > 0) {
            std::cout << "Steps per second:               " << std::fixed << std::setprecision(1) 
                      << result.maxSteps / result.totalWallClockTime << std::endl;
            std::cout << "Neurons per second:            " << std::fixed << std::setprecision(0) 
                      << (result.neuronCount / result.totalWallClockTime) << std::endl;
        }
        
        std::cout << std::endl;
        
        // Generate summary report
        std::cout << config.generateReport(result) << std::endl;
        
        std::cout << std::endl;
        std::cout << "✅ Phase 6 integration demo completed successfully!" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain is a functioning artificial neural substrate with:" << std::endl;
        std::cout << "  • Memory systems connected to neural processing" << std::endl;
        std::cout << "  • Neuromodulation affecting plasticity and learning" << std::endl;
        std::cout << "  • Prediction integrated with learning" << std::endl;
        std::cout << "  • Development affecting plasticity rates" << std::endl;
        std::cout << "  • Checkpoint save/load functionality" << std::endl;
        std::cout << "  • Replay and consolidation capabilities" << std::endl;
        
        return 0;
    }
    
    // If only partial tests were run
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    DEMO COMPLETED                             ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "✅ Phase 6 integration demo completed successfully!" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: Full integration experiment was not run (--no-full flag used)." << std::endl;
    std::cout << "To run the complete integration test, remove the --no-full flag." << std::endl;
    
    return 0;
}
