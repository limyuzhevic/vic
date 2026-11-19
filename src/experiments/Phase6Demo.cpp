/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getGlobal().setLevel(LogLevel::Info);
    
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
    
    // Test neuromodulation integration including new neuromodulators
    std::cout << "--- Neuromodulation Integration Test (including ACh, NE, 5-HT) ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    // Test individual neuromodulators
    std::cout << "--- Individual Neuromodulator Tests ---" << std::endl;
    experiment.testDopamine();
    experiment.testAcetylcholine();
    experiment.testNorepinephrine();
    experiment.testSerotonin();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << std::endl;
    
    // Neuromodulator levels
    std::cout << "=== NEUROMODULATOR LEVELS ===" << std::endl;
    std::cout << "Dopamine (DA): " << result.dopamineLevel << std::endl;
    std::cout << "Acetylcholine (ACh): " << result.acetylcholineLevel << std::endl;
    std::cout << "Norepinephrine (NE): " << result.norepinephrineLevel << std::endl;
    std::cout << "Serotonin (5-HT): " << result.serotoninLevel << std::endl;
    std::cout << "Novelty: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity: " << result.curiosityLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation (DA): " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Acetylcholine (ACh): " << (result.acetylcholineIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Norepinephrine (NE): " << (result.norepinephrineIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Serotonin (5-HT): " << (result.serotoninIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
