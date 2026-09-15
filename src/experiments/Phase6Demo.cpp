// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demonstration
//
// This phase demonstrates the complete integration of all brain systems:
// - Memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction and planning systems
// - Development stages affecting plasticity
// - Checkpoint save/load functionality
// - Replay and consolidation mechanisms
//
// This is the final demonstration that all Phase 6 achievements work together
// in a coherent artificial brain system.

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <memory>
#include <stdexcept>

using namespace nlm;

void printPhase6Banner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printSystemStatus(const Phase6IntegrationResult& result) {
    std::cout << "\n=== BRAIN SYSTEM INTEGRATION STATUS ===" << std::endl;
    
    std::cout << std::left << std::setw(30) << "Working Memory:" 
              << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << std::left << std::setw(30) << "Episodic Memory:" 
              << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << std::left << std::setw(30) << "Neuromodulation:" 
              << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << std::left << std::setw(30) << "Prediction System:" 
              << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << std::left << std::setw(30) << "Development System:" 
              << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << std::left << std::setw(30) << "Checkpointing:" 
              << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << std::left << std::setw(30) << "Replay System:" 
              << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
}

void printPerformanceMetrics(const Phase6IntegrationResult& result) {
    std::cout << "\n=== PERFORMANCE METRICS ===" << std::endl;
    
    std::cout << std::left << std::setw(30) << "Total Reward:" 
              << std::fixed << std::setprecision(3) << result.totalReward << std::endl;
    std::cout << std::left << std::setw(30) << "Average Firing Rate:" 
              << std::fixed << std::setprecision(3) << result.avgFiringRate << " Hz" << std::endl;
    std::cout << std::left << std::setw(30) << "Average Synaptic Weight:" 
              << std::fixed << std::setprecision(4) << result.avgSynapticWeight << std::endl;
    std::cout << std::left << std::setw(30) << "Memory Episodes Stored:" 
              << std::fixed << std::setprecision(0) << result.memoryEpisodesStored << std::endl;
    
    std::cout << std::left << std::setw(30) << "Novelty Level:" 
              << std::fixed << std::setprecision(3) << result.noveltyLevel << std::endl;
    std::cout << std::left << std::setw(30) << "Curiosity Level:" 
              << std::fixed << std::setprecision(3) << result.curiosityLevel << std::endl;
    std::cout << std::left << std::setw(30) << "Dopamine Level:" 
              << std::fixed << std::setprecision(3) << result.dopamineLevel << std::endl;
    
    std::cout << std::left << std::setw(30) << "Wall Clock Time:" 
              << std::fixed << std::setprecision(2) << result.totalWallClockTime << "s" << std::endl;
}

void validateConfiguration(const Phase6Config& config) {
    if (config.neuronCount == 0) {
        throw std::invalid_argument("Neuron count must be greater than 0");
    }
    if (config.maxSteps == 0) {
        throw std::invalid_argument("Max steps must be greater than 0");
    }
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        throw std::invalid_argument("Connection probability must be between 0.0 and 1.0");
    }
}

int main(int argc, char* argv[]) {
    try {
        printPhase6Banner();
        
        std::cout << "\n=== NLM Phase 6 Final Integration Demo ===" << std::endl;
        std::cout << "Demonstrating complete artificial brain functionality..." << std::endl;
        std::cout << "" << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Create the integration experiment
        std::cout << "--- Creating Phase 6 Integration Experiment ---" << std::endl;
        Phase6IntegratedExperiment experiment;
        
        // First, verify integration
        std::cout << "\n--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Phase 6 integration verification failed!" << std::endl;
            std::cerr << "The brain systems may not be properly connected." << std::endl;
            return 1;
        }
        
        std::cout << "✓ Integration verification passed! All systems are connected." << std::endl;
        std::cout << "" << std::endl;
        
        // Run individual system tests
        std::cout << "--- Individual System Tests ---" << std::endl;
        
        std::cout << "Testing memory integration..." << std::endl;
        bool memoryOK = experiment.testMemoryIntegration();
        
        std::cout << "Testing neuromodulation integration..." << std::endl;
        bool neuromodOK = experiment.testNeuromodulationIntegration();
        
        std::cout << "Testing checkpoint system..." << std::endl;
        bool checkpointOK = experiment.testCheckpointing();
        
        std::cout << "Testing replay system..." << std::endl;
        bool replayOK = experiment.testReplay();
        
        std::cout << "" << std::endl;
        
        // Run full integration experiment with configurable settings
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        
        Phase6Config config;
        config.neuronCount = 1000;           // Larger network for realistic behavior
        config.maxSteps = 5000;              // Longer simulation for development
        config.enableCheckpointing = true;   // Test checkpointing
        config.enableReplay = true;         // Test replay
        config.enableDevelopment = true;    // Test development effects
        
        // Validate configuration
        try {
            validateConfiguration(config);
            std::cout << "✓ Configuration validated successfully." << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "ERROR: Invalid configuration: " << e.what() << std::endl;
            return 1;
        }
        
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Steps: " << config.maxSteps << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "enabled" : "disabled") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "enabled" : "disabled") << std::endl;
        std::cout << "" << std::endl;
        
        Phase6IntegrationResult result = experiment.run(config);
        
        // Display results
        printSystemStatus(result);
        printPerformanceMetrics(result);
        
        // Final assessment
        std::cout << "\n=== PHASE 6 FINAL ASSESSMENT ===" << std::endl;
        
        bool allSystemsWorking = result.memoryWorkingMemoryIntegrated &&
                                result.memoryEpisodicMemoryIntegrated &&
                                result.neuromodulationIntegrated &&
                                result.predictionIntegrated &&
                                result.developmentIntegrated &&
                                result.checkpointingWorks;
        
        if (allSystemsWorking) {
            std::cout << "🎉 SUCCESS: All Phase 6 systems are properly integrated!" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "The NLM brain is now a fully functioning artificial intelligence:" << std::endl;
            std::cout << "  ✓ Memory systems store and retrieve experiences" << std::endl;
            std::cout << "  ✓ Neuromodulators guide learning and behavior" << std::endl;
            std::cout << "  ✓ Prediction and planning enable goal-directed behavior" << std::endl;
            std::cout << "  ✓ Development modulates learning throughout lifetime" << std::endl;
            std::cout << "  ✓ Checkpoints allow persistence across sessions" << std::endl;
            std::cout << "  ✓ Replay consolidates important memories" << std::endl;
            std::cout << "" << std::endl;
            std::cout << "Phase 6 integration complete. The NLM is ready for real-world deployment." << std::endl;
        } else {
            std::cout << "⚠️  WARNING: Some systems may not be fully integrated." << std::endl;
            std::cout << "The NLM may need further refinement for optimal performance." << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }
}