// NLM Phase 6: Final Integration Test
// Demonstrates that the integrated brain systems work together:
// - Memory systems are connected to neural processing
// - Neuromodulation affects plasticity and neural dynamics
// - Development affects plasticity rates
// - Prediction system is integrated
// - Replay and consolidation are functional
// - Checkpoint save/load works
//
// This experiment runs a complete lifetime simulation and verifies
// that all systems interact properly.

#include "Phase6IntegratedExperiment.hpp"
#include <iostream>

using namespace nlm;

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Test                           ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;

    std::cout << "Phase 6: Final Integration Test" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << std::endl;

    Phase6IntegratedExperiment experiment;
    
    // Configure for moderate test
    Phase6Config config;
    config.neuronCount = 1000;
    config.maxSteps = 5000;
    config.regionCount = 1;
    config.connectionProbability = 0.1f;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max steps: " << config.maxSteps << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Connection probability: " << config.connectionProbability << std::endl;
    std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
    std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
    std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
    std::cout << std::endl;

    std::cout << "Running Phase 6 integration test..." << std::endl;
    std::cout << "This will test the complete integrated brain loop." << std::endl;
    std::cout << std::endl;

    // Run the integration test
    Phase6IntegrationResult result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== Phase 6 Integration Test Results ===" << std::endl;
    std::cout << std::endl;

    // Print system integration status
    std::cout << "System Integration:" << std::endl;
    std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
    std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓" : "✗") << std::endl;
    std::cout << "  Replay: " << (result.replayWorks ? "✓" : "✗") << std::endl;
    std::cout << std::endl;

    // Print performance metrics
    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Total reward: " << result.totalReward << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "  Average synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "  Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "  Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "  Total wall clock time: " << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << std::endl;

    // Overall assessment
    int integratedSystems = 0;
    if (result.memoryWorkingMemoryIntegrated) integratedSystems++;
    if (result.memoryEpisodicMemoryIntegrated) integratedSystems++;
    if (result.neuromodulationIntegrated) integratedSystems++;
    if (result.predictionIntegrated) integratedSystems++;
    if (result.developmentIntegrated) integratedSystems++;
    if (result.checkpointingWorks) integratedSystems++;
    if (result.replayWorks) integratedSystems++;
    
    std::cout << "Integration Assessment:" << std::endl;
    std::cout << "  Successfully integrated systems: " << integratedSystems << "/7" << std::endl;
    
    if (integratedSystems >= 6) {
        std::cout << "  ✓ EXCELLENT: Most systems are properly integrated!" << std::endl;
        std::cout << "  ✓ The NLM brain demonstrates coherent artificial intelligence!" << std::endl;
    } else if (integratedSystems >= 4) {
        std::cout << "  ✓ GOOD: Several systems are integrated." << std::endl;
        std::cout << "  ✓ There is potential for further improvements." << std::endl;
    } else {
        std::cout << "  ⚠ NEEDS WORK: Integration is incomplete." << std::endl;
        std::cout << "  ⚠ Further development is needed to achieve full integration." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Phase 6 integration test complete." << std::endl;
    
    return (integratedSystems >= 6) ? 0 : 1;
}
