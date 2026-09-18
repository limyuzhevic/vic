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
    
    // Parse command line options
    Phase6Config config = Phase6Config::parseCommandLine(argc, argv);
    
    // Validate configuration
    if (!Phase6IntegratedExperiment::validateConfig(config)) {
        std::cerr << "ERROR: Invalid configuration parameters!" << std::endl;
        std::cerr << "Neuron count: " << config.neuronCount << std::endl;
        std::cerr << "Max steps: " << config.maxSteps << std::endl;
        std::cerr << "Connection probability: " << config.connectionProbability << std::endl;
        std::cerr << "Valid ranges: 10-100000 neurons, 1-1000000 steps, 0.0-1.0 probability" << std::endl;
        return 1;
    }
    
    // Initialize logging with detailed configuration
    std::cout << "Initializing logging..." << std::endl;
    Logger::getInstance().setLevel(Logger::Level::Info);
    NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
    NLM_LOG_INFO("Configuration: " + std::to_string(config.neuronCount) + " neurons, " +
                 std::to_string(config.maxSteps) + " steps");
    NLM_LOG_INFO("Checkpointing: " + std::string(config.enableCheckpointing ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("Replay: " + std::string(config.enableReplay ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("Development: " + std::string(config.enableDevelopment ? "ENABLED" : "DISABLED"));
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    try {
        // First, run the quick integration verification
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            std::cerr << "Check logs for detailed error information." << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        
        // Test individual systems
        std::cout << "--- Memory Integration Test ---" << std::endl;
        bool memoryOK = experiment.testMemoryIntegration();
        if (!memoryOK) {
            std::cerr << "WARNING: Memory integration test had issues" << std::endl;
        }
        std::cout << std::endl;
        
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        bool neuromodulationOK = experiment.testNeuromodulationIntegration();
        if (!neuromodulationOK) {
            std::cerr << "WARNING: Neuromodulation integration test had issues" << std::endl;
        }
        std::cout << std::endl;
        
        // Skip checkpoint and replay tests if disabled for performance
        if (config.enableCheckpointing) {
            std::cout << "--- Checkpoint Test ---" << std::endl;
            bool checkpointOK = experiment.testCheckpointing();
            if (!checkpointOK) {
                std::cerr << "WARNING: Checkpoint test had issues" << std::endl;
            }
            std::cout << std::endl;
        }
        
        if (config.enableReplay) {
            std::cout << "--- Replay Test ---" << std::endl;
            bool replayOK = experiment.testReplay();
            if (!replayOK) {
                std::cerr << "WARNING: Replay test had issues" << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Run full experiment with user's configuration
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        std::cout << "Running " << config.maxSteps << " simulation steps with " << 
            config.neuronCount << " neurons..." << std::endl;
        
        auto result = experiment.run(config);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward per step: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== SYSTEM INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction System: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development System: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay System: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== PERFORMANCE ===" << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "Steps per second: " << (result.maxSteps / result.totalWallClockTime) << std::endl;
        
        // Overall assessment
        bool allSystemsWorking = result.memoryWorkingMemoryIntegrated && 
                                result.memoryEpisodicMemoryIntegrated && 
                                result.neuromodulationIntegrated && 
                                result.predictionIntegrated && 
                                result.developmentIntegrated && 
                                result.checkpointingWorks && 
                                result.replayWorks;
        
        std::cout << std::endl;
        if (allSystemsWorking) {
            std::cout << "✓ ALL SYSTEMS SUCCESSFULLY INTEGRATED!" << std::endl;
            std::cout << "The NLM brain is a fully functional artificial neural system" << std::endl;
            std::cout << "capable of learning, memory, motivation, and development." << std::endl;
        } else {
            std::cout << "⚠ SOME SYSTEMS HAVE ISSUES (see above status)" << std::endl;
            std::cout << "The brain may still be functional but may have integration problems." << std::endl;
        }
        
        return allSystemsWorking ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        std::cerr << "The experiment encountered a fatal error." << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "FATAL ERROR: Unknown exception occurred" << std::endl;
        std::cerr << "The experiment encountered an unknown fatal error." << std::endl;
        return 1;
    }
}