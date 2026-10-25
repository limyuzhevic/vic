Phase6Demo implementation

This file provides the implementation for the Phase6Demo class.
It handles the main integration test demo for NLM Phase 6.

The demo runs comprehensive tests to verify that all brain systems
are properly integrated and connected.
*/

#include "Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

Phase6Demo::Phase6Demo() : experiment_(std::make_unique<Phase6IntegratedExperiment>()) {}

Phase6Demo::~Phase6Demo() = default;

void Phase6Demo::run(const std::string& configPath) {
    configPath_ = configPath;
    run(Phase6Config{});
}

void Phase6Demo::run(const Phase6Config& config) {
    printBanner();
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config demoConfig = config;
    demoConfig.neuronCount = 500;
    demoConfig.maxSteps = 2000;
    demoConfig.enableCheckpointing = true;
    demoConfig.enableReplay = true;
    demoConfig.enableDevelopment = true;
    
    results_ = experiment_->run(demoConfig);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << results_.totalReward << std::endl;
    std::cout << "Avg firing rate: " << results_.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << results_.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << results_.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (results_.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (results_.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (results_.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (results_.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (results_.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (results_.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << results_.totalWallClockTime << "s" << std::endl;
}

bool Phase6Demo::verifyIntegration() {
    return experiment_->verifyIntegration();
}

bool Phase6Demo::testMemoryIntegration() {
    return experiment_->testMemoryIntegration();
}

bool Phase6Demo::testNeuromodulationIntegration() {
    return experiment_->testNeuromodulationIntegration();
}

bool Phase6Demo::testCheckpointing() {
    return experiment_->testCheckpointing();
}

bool Phase6Demo::testReplay() {
    return experiment_->testReplay();
}

void Phase6Demo::printBanner() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
}

void Phase6Demo::printUsage() {
    std::cout << "Usage: nlm_phase6_demo [config_file]" << std::endl;
    std::cout << "  config_file: Path to configuration file (default: configs/default.cfg)" << std::endl;
}

// Main entry point for Phase 6 demo
int main(int argc, char* argv[]) {
    nlm::Phase6Demo demo;
    
    if (argc > 1) {
        demo.run(argv[1]);
    } else {
        demo.run();
    }
    
    return 0;
}