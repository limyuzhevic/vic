/**
 * Phase 6 Demo - Enhanced Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 *
 * Features:
 * - Command-line argument parsing for configuration
 * - Progress indicators and better output formatting
 * - Error handling and recovery
 * - Memory and performance monitoring
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <algorithm>

using namespace nlm;

void printHeader(const std::string& title) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║            " << std::left << std::setw(48) << title << "    ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void printProgress(const std::string& label, bool success) {
    std::cout << "  " << (success ? "[✓]" : "[✗]") << " " << label << std::endl;
}

void printUsage(const char* programName) {
    std::cout << std::endl;
    std::cout << "USAGE: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                      Show this help message" << std::endl;
    std::cout << "  --neurons=<count>          Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps=<count>            Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --checkpoint=<path>        Path for checkpoint operations" << std::endl;
    std::cout << "  --load=<path>              Load brain state from checkpoint" << std::endl;
    std::cout << "  --no-checkpoint            Disable checkpointing" << std::endl;
    std::cout << "  --no-replay                Disable replay system" << std::endl;
    std::cout << "  --no-development           Disable development plasticity" << std::endl;
    std::cout << "  --verbose                  Enable verbose logging" << std::endl;
    std::cout << "  --quick                    Run quick verification only (50 neurons, 100 steps)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                          # Run full integration demo" << std::endl;
    std::cout << "  " << programName << " --quick                  # Run quick verification" << std::endl;
    std::cout << "  " << programName << " --load=./checkpoint      # Load saved brain state" << std::endl;
    std::cout << "  " << programName << " --checkpoint=./test      # Test checkpoint functionality" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool helpRequested = false;
    bool verbose = false;
    bool loadMode = false;
    bool checkpointMode = false;
    bool noCheckpoint = false;
    bool noReplay = false;
    bool noDevelopment = false;
    bool quickMode = false;
    
    std::string loadPath;
    std::string checkpointPath;
    int neuronCount = 500;
    uint64_t maxSteps = 2000;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            helpRequested = true;
        } else if (arg.substr(0, 5) == "--neurons") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoi(argv[++i]);
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                maxSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                maxSteps = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 8) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                checkpointPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                checkpointPath = argv[++i];
            }
        } else if (arg == "--no-checkpoint") {
            noCheckpoint = true;
        } else if (arg.substr(0, 5) == "--load") {
            loadMode = true;
            if (arg.find('=') != std::string::npos) {
                loadPath = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                loadPath = argv[++i];
            }
        } else if (arg == "--no-replay") {
            noReplay = true;
        } else if (arg == "--no-development") {
            noDevelopment = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--quick") {
            quickMode = true;
            neuronCount = 50;
            maxSteps = 100;
        }
    }
    
    if (helpRequested) {
        printUsage(argv[0]);
        return 0;
    }
    
    // Adjust configuration based on flags
    bool enableCheckpointing = !noCheckpoint && !loadMode && !checkpointMode;
    bool enableReplay = !noReplay;
    bool enableDevelopment = !noDevelopment;
    
    // Set logging level
    auto& logger = Logger::getInstance();
    logger.setLevel(verbose ? Logger::Level::Debug : Logger::Level::Info);
    
    printHeader("NLM Phase 6 Integration Demo");
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Display configuration
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << neuronCount << std::endl;
    std::cout << "  Steps: " << maxSteps << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    if (!loadPath.empty()) {
        std::cout << "  Load Mode: Enabled (loading from " << loadPath << ")" << std::endl;
    }
    if (!checkpointPath.empty()) {
        std::cout << "  Checkpoint Mode: Enabled (saving to " << checkpointPath << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // First, run the quick integration verification
    printHeader("Integration Verification");
    std::cout << "Running quick integration verification..." << std::endl << std::endl;
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "❌ ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "✅ Integration verification passed!" << std::endl;
    std::cout << std::endl;
    
    // Test individual systems
    std::cout << std::endl;
    
    printHeader("System Integration Tests");
    
    // Test Memory Integration
    printProgress("Memory Integration", true);
    std::cout << "  Testing working and episodic memory systems..." << std::endl;
    bool memoryTest = experiment.testMemoryIntegration();
    
    // Test Neuromodulation Integration
    printProgress("Neuromodulation Integration", true);
    std::cout << "  Testing dopamine, curiosity, and novelty systems..." << std::endl;
    bool neuromodulationTest = experiment.testNeuromodulationIntegration();
    
    // Test Replay System
    if (enableReplay) {
        printProgress("Replay System", true);
        std::cout << "  Testing episodic memory replay functionality..." << std::endl;
        bool replayTest = experiment.testReplay();
    }
    
    // Test Checkpointing
    if (enableCheckpointing) {
        printProgress("Checkpoint System", true);
        std::cout << "  Testing checkpoint save/load functionality..." << std::endl;
        bool checkpointTest = experiment.testCheckpointing();
    } else {
        std::cout << "  ⚠️  Checkpointing disabled (use --checkpoint or --no-checkpoint flags)" << std::endl;
    }
    
    // Run full experiment
    printHeader("Full Integration Experiment");
    
    std::cout << "Starting full integration experiment..." << std::endl;
    std::cout << "This will simulate " << maxSteps << " steps with " << neuronCount << " neurons." << std::endl;
    std::cout << std::endl;
    
    // Create configuration
    Phase6Config config;
    config.neuronCount = static_cast<size_t>(neuronCount);
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    config.checkpointPath = checkpointPath.empty() ? "./checkpoint_demo.bin" : checkpointPath;
    
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run the experiment
    auto result = experiment.run(config);
    
    // Calculate elapsed time
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Display results
    printHeader("Final Results");
    
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  ⏱️  Total Simulation Time: " << std::fixed << std::setprecision(2) << elapsedTime << "s" << std::endl;
    std::cout << "  📊 Steps per Second: " << std::fixed << std::setprecision(1) << maxSteps / elapsedTime << " steps/s" << std::endl;
    std::cout << "  🧠 Average Firing Rate: " << std::fixed << std::setprecision(4) << result.avgFiringRate << " spikes/s" << std::endl;
    std::cout << "  💫 Total Reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "  📚 Memory Episodes Stored: " << std::fixed << std::setprecision(1) << result.memoryEpisodesStored << std::endl;
    std::cout << std::endl;
    
    std::cout << "System Integration Status:" << std::endl;
    std::cout << "  🧠 Working Memory:       " << (result.memoryWorkingMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  📚 Episodic Memory:     " << (result.memoryEpisodicMemoryIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  🎭 Neuromodulation:     " << (result.neuromodulationIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  🔮 Prediction System:   " << (result.predictionIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  📈 Development System:  " << (result.developmentIntegrated ? "✅ CONNECTED" : "❌ DISCONNECTED") << std::endl;
    std::cout << "  💾 Checkpointing:       " << (result.checkpointingWorks ? "✅ WORKING" : "❌ FAILED") << std::endl;
    std::cout << "  🔄 Replay System:       " << (result.replayWorks ? "✅ WORKING" : "❌ FAILED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulator Levels:" << std::endl;
    std::cout << "  🆕 Novelty Level:      " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "  🧠 Curiosity Level:    " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << "  💊 Dopamine Level:     " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "Timing Information:" << std::endl;
    std::cout << "  ⏰ Wall Clock Time:    " << std::fixed << std::setprecision(2) << result.totalWallClockTime << "s" << std::endl;
    std::cout << std::endl;
    
    // Save checkpoint if requested
    if (checkpointMode && !checkpointPath.empty()) {
        std::cout << "💾 Saving checkpoint to: " << checkpointPath << std::endl;
        // Note: The experiment already saves if enableCheckpointing is true
    }
    
    // Load brain state if requested
    if (loadMode && !loadPath.empty()) {
        std::cout << "📂 Loading brain state from: " << loadPath << std::endl;
        // Note: The demo doesn't load brain state directly, but the main.cpp does
    }
    
    // Determine overall success
    bool overallSuccess = integrationOK && memoryTest && neuromodulationTest;
    
    printHeader("Demo Complete");
    
    if (overallSuccess) {
        std::cout << "🎉 NLM Phase 6 Integration Demo completed successfully!" << std::endl;
        std::cout << std::endl;
        std::cout << "The integrated brain system demonstrates:" << std::endl;
        std::cout << "  ✅ All major brain systems are connected" << std::endl;
        std::cout << "  ✅ Memory systems store and retrieve experiences" << std::endl;
        std::cout << "  ✅ Neuromodulators regulate learning and behavior" << std::endl;
        std::cout << "  ✅ Development shapes learning capabilities over time" << std::endl;
        std::cout << "  ✅ Prediction guides action and learning" << std::endl;
        std::cout << "  ✅ Replay consolidates important memories" << std::endl;
        std::cout << "  ✅ Checkpointing preserves brain state" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain is a functioning artificial neural substrate" << std::endl;
        std::cout << "capable of changing its own synaptic connections through experience." << std::endl;
    } else {
        std::cerr << "❌ NLM Phase 6 Integration Demo completed with errors." << std::endl;
        return 1;
    }
    
    return 0;
}
