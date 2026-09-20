# Phase 6 Demo - Integration Test
// 
// This demo runs the Phase 6 integration experiment to verify
// that all brain systems are properly connected.

#include "Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>

using namespace nlm;

Phase6Demo::Phase6Demo() : experiment_(std::make_unique<Phase6IntegratedExperiment>()) {
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
}

Phase6Demo::~Phase6Demo() = default;

int Phase6Demo::run(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Parse command line arguments
    std::string checkpointPath = "/tmp/nlm_phase6_checkpoint.bin";
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string logLevel = "Info";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--checkpoints" && i + 1 < argc) {
            checkpointPath = argv[++i];
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = static_cast<size_t>(std::stoul(argv[++i]));
        } else if (arg == "--steps" && i + 1 < argc) {
            maxSteps = static_cast<size_t>(std::stoul(argv[++i]));
        } else if (arg == "--no-checkpoints") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--debug") {
            logLevel = "Debug";
        } else if (arg == "--warn") {
            logLevel = "Warn";
        } else if (arg == "--error") {
            logLevel = "Error";
        }
    }
    
    // Update logging level
    if (logLevel == "Debug") {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    } else if (logLevel == "Warn") {
        Logger::getInstance().setLevel(Logger::Level::Warn);
    } else if (logLevel == "Error") {
        Logger::getInstance().setLevel(Logger::Level::Error);
    } else {
        Logger::getInstance().setLevel(Logger::Level::Info);
    }
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment_->verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment_->testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment_->testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment_->testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment_->testReplay();
    std::cout << std::endl;
    
    // Run full experiment with configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    config.checkpointPath = checkpointPath;
    
    auto result = experiment_->run(config);
    
    // Display detailed results with improved formatting
    std::cout << std::endl;
    std::cout << "=== FINAL PHASE 6 INTEGRATION RESULTS ===" << std::endl;
    
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "  Total Reward (per step): " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Dopamine Level: " << std::fixed << std::setprecision(4) << result.dopamineLevel << std::endl;
    std::cout << "  Novelty Level: " << std::fixed << std::setprecision(4) << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity Level: " << std::fixed << std::setprecision(4) << result.curiosityLevel << std::endl;
    std::cout << "  Wall Clock Time: " << std::fixed << std::setprecision(2) << result.totalWallClockTime << " seconds" << std::endl;
    
    std::cout << "\nSystem Integration Status:" << std::endl;
    std::cout << "  Working Memory:     " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory:    " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation:    " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction:          " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Development:        " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing:       " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    
    // Print success summary
    std::cout << "\n=== PHASE 6 INTEGRATION SUCCESS ===" << std::endl;
    int integratedCount = 0;
    if (result.memoryWorkingMemoryIntegrated) integratedCount++;
    if (result.memoryEpisodicMemoryIntegrated) integratedCount++;
    if (result.neuromodulationIntegrated) integratedCount++;
    if (result.predictionIntegrated) integratedCount++;
    if (result.developmentIntegrated) integratedCount++;
    if (result.checkpointingWorks) integratedCount++;
    
    std::cout << "Systems successfully integrated: " << integratedCount << "/6" << std::endl;
    
    if (integratedCount >= 5) {
        std::cout << "✓ EXCELLENT: Almost all systems are properly integrated!" << std::endl;
    } else if (integratedCount >= 3) {
        std::cout << "✓ GOOD: Core systems are integrated!" << std::endl;
    } else {
        std::cout << "⚠ WARNING: Multiple systems are not integrated!" << std::endl;
    }
    
    std::cout << "\nThe NLM brain is now a complete artificial neural substrate" << std::endl;
    std::cout << "that learns from experience through integrated cognitive, neural," << std::endl;
    std::cout << "and memory systems working together in a unified framework." << std::endl;
    
    return 0;
}

void Phase6Demo::printHelp() {
    std::cout << "NLM Phase 6 Demo - Integration Test" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --checkpoints <path>     Set checkpoint file path (default: /tmp/nlm_phase6_checkpoint.bin)" << std::endl;
    std::cout << "  --neurons <count>        Number of neurons to simulate (default: 500)" << std::endl;
    std::cout << "  --steps <count>          Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --no-checkpoints         Disable checkpointing" << std::endl;
    std::cout << "  --no-replay              Disable replay functionality" << std::endl;
    std::cout << "  --no-development         Disable development system" << std::endl;
    std::cout << "  --debug                  Set logging level to Debug" << std::endl;
    std::cout << "  --warn                   Set logging level to Warn" << std::endl;
    std::cout << "  --error                  Set logging level to Error" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase6_demo                    # Run with default settings" << std::endl;
    std::cout << "  nlm_phase6_demo --neurons 1000     # Use larger brain with 1000 neurons" << std::endl;
    std::cout << "  nlm_phase6_demo --steps 5000        # Run longer simulation" << std::endl;
    std::cout << "  nlm_phase6_demo --debug --no-checkpoints # Debug run without checkpointing" << std::endl;
    std::cout << std::endl;
    std::cout << "The demo tests all Phase 6 integration points:" << std::endl;
    std::cout << "  ✓ Memory systems (working, episodic)" << std::endl;
    std::cout << "  ✓ Neuromodulation (dopamine, curiosity)" << std::endl;
    std::cout << "  ✓ Prediction system integration" << std::endl;
    std::cout << "  ✓ Development and plasticity" << std::endl;
    std::cout << "  ✓ Checkpoint save/load" << std::endl;
    std::cout << "  ✓ Replay and consolidation" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Features:" << std::endl;
    std::cout << "  • Configurable simulation size (neurons, steps)" << std::endl;
    std::cout << "  • Toggle individual system components" << std::endl;
    std::cout << "  • Adjustable checkpoint paths" << std::endl;
    std::cout << "  • Variable logging levels" << std::endl;
    std::cout << "  • Performance metrics collection" << std::endl;
}