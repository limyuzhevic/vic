/**
 * Phase 6 Enhanced Demo - Advanced Integration Test
 * 
 * This demo showcases the improved Phase 6 integration with:
 * - Enhanced STDP variants (RewardModulatedSTDP, TripletSTDP)
 * - Advanced reward computation with novelty detection
 * - Improved neuromodulation and development systems
 * - Better memory systems with concept formation
 * - Performance optimizations and scalability
 */

#include "experiments/Phase6EnhancedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <chrono>

using namespace nlm;

void printEnhancedBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然 Enhanced                                        ║
    ║     Neural Learning Machine - Phase 6                          ║
    ║                                                               ║
    ║     Enhanced Integration Demo                                  ║
    ║     Advanced STDP, Memory, Neuromodulation                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char* argv[]) {
    printEnhancedBanner();
    std::cout << "Testing the enhanced integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize enhanced logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6EnhancedExperiment experiment;
    
    // First, run the enhanced integration verification
    std::cout << "--- Enhanced Integration Verification ---" << std::auto;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Enhanced integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Enhanced integration verification passed!" << std::endl << std::endl;
    
    // Test enhanced systems
    std::cout << "--- Advanced Neuromodulation Integration ---" << std::endl;
    experiment.testAdvancedNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Advanced Memory Integration ---" << std::endl;
    experiment.testAdvancedMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Advanced Plasticity Integration ---" << std::endl;
    experiment.testAdvancedPlasticityIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Advanced Prediction Integration ---" << std::endl;
    experiment.testAdvancedPredictionIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Advanced Checkpoint Test ---" << std::endl;
    experiment.testAdvancedCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Enhanced Replay Test ---" << std::endl;
    experiment.testEnhancedReplay();
    std::cout << std::endl;
    
    // Run full enhanced experiment with optimized settings
    std::cout << "--- Full Enhanced Integration Experiment ---" << std::endl;
    Phase6EnhancedConfig config;
    config.neuronCount = 2000;              // Larger brain
    config.maxSteps = 5000;                 // Longer simulation
    config.enableCheckpointing = true;     // Advanced checkpointing
    config.enableReplay = true;             // Enhanced replay
    config.enableDevelopment = true;       // Advanced development
    config.enableAdvancedNeuromodulation = true; // Enhanced neuromodulation
    config.enableAdvancedMemory = true;    // Advanced memory systems
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << std::endl;
    std::cout << "=== ENHANCED FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Avg synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Learning rate: " << result.learningRate << std::endl;
    std::cout << "Working memory usage: " << result.workingMemoryUsage << std::endl;
    std::cout << "Episodic memory usage: " << result.episodicMemoryUsage << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== ENHANCED INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Advanced Neuromodulation: " << (result.neuromodulationIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "ENHANCED WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "ENHANCED WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Social Learning: " << (result.socialLearningIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Concept Formation: " << (result.conceptFormationIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Advanced Plasticity: " << (result.advancedPlasticityIntegrated ? "ENHANCED CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << duration.count() << "s" << std::endl;
    std::cout << "Performance: " << (result.neuronsPerSecond / 1000.0f) << "K neurons/second" << std::endl;
    
    return 0;
}

} // namespace nlm
