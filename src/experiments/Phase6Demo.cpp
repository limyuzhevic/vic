/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * This executable builds upon all previous phases to demonstrate:
 * - Complete neural network integration
 * - Memory systems integration (working + episodic memory)
 * - Neuromodulation integration (dopamine, curiosity, novelty)
 * - Prediction systems integration
 * - Cognitive systems integration (attention, planning, concept formation)
 * - Development systems integration (synaptogenesis, pruning, maturation)
 * - Checkpoint save/load functionality
 * - Replay and memory consolidation
 */

#include "experiments/Phase6Demo.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

Phase6Demo::Phase6Demo()
    : totalReward(0.0f)
    , avgFiringRate(0.0f)
    , memoryEpisodesStored(0.0f)
    , dopamineLevel(0.0f)
    , curiosityLevel(0.0f)
    , noveltyLevel(0.0f)
    , neuronCount(500)
    , maxSteps(2000)
    , enableCheckpointing(true)
    , enableReplay(true)
    , enableDevelopment(true) {
}

Phase6Demo::~Phase6Demo() {
}

void Phase6Demo::run(
    size_t n,
    size_t m,
    bool chk,
    bool rep,
    bool dev
) {
    neuronCount = n;
    maxSteps = m;
    enableCheckpointing = chk;
    enableReplay = rep;
    enableDevelopment = dev;
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << "Configuration: " << neuronCount << " neurons, " << maxSteps << " steps" << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
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
    
    // Run full experiment
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Store results
    totalReward = result.totalReward;
    avgFiringRate = result.avgFiringRate;
    memoryEpisodesStored = result.memoryEpisodesStored;
    dopamineLevel = result.dopamineLevel;
    curiosityLevel = result.curiosityLevel;
    noveltyLevel = result.noveltyLevel;
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << " Hz" << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << wallClockTime << "s" << std::endl;
    std::cout << "Total wall clock time stored in result: " << result.totalWallClockTime << "s" << std::endl;
}

bool Phase6Demo::verifyIntegration() {
    Phase6IntegratedExperiment experiment;
    return experiment.verifyIntegration();
}

bool Phase6Demo::testMemoryIntegration() {
    Phase6IntegratedExperiment experiment;
    return experiment.testMemoryIntegration();
}

bool Phase6Demo::testNeuromodulationIntegration() {
    Phase6IntegratedExperiment experiment;
    return experiment.testNeuromodulationIntegration();
}

bool Phase6Demo::testCheckpointing() {
    Phase6IntegratedExperiment experiment;
    return experiment.testCheckpointing();
}

bool Phase6Demo::testReplay() {
    Phase6IntegratedExperiment experiment;
    return experiment.testReplay();
}

int main(int argc, char* argv[]) {
    Phase6Demo demo;
    
    // Parse command line arguments
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    // Simple argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 6 Demo Usage:" << std::endl;
            std::cout << "  --neurons <count>    Number of neurons (default: 500)" << std::endl;
            std::cout << "  --steps <steps>       Max simulation steps (default: 2000)" << std::endl;
            std::cout << "  --no-checkpoint       Disable checkpoint testing" << std::endl;
            std::cout << "  --no-replay           Disable replay testing" << std::endl;
            std::cout << "  --no-development      Disable development testing" << std::endl;
            std::cout << "  --help, -h           Show this help" << std::endl;
            return 0;
        }
    }
    
    demo.run(neuronCount, maxSteps, enableCheckpointing, enableReplay, enableDevelopment);
    
    std::cout << std::endl;
    std::cout << "=== DEMO SUMMARY ===" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << neuronCount << std::endl;
    std::cout << "  Max Steps: " << maxSteps << std::endl;
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Replay: " << (enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    std::cout << "Results:" << std::endl;
    std::cout << "  Total Reward: " << demo.getTotalReward() << std::endl;
    std::cout << "  Average Firing Rate: " << demo.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Episodes Stored: " << demo.getMemoryEpisodesStored() << std::endl;
    std::cout << "  Neuromodulation Level: " << demo.getDopamineLevel() << std::endl;
    
    return 0;
}
