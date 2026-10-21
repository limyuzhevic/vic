#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    // Phase 6: FINAL INTEGRATION - The ultimate integration test
    // This verifies that all previous phases (1-5) are properly integrated
    // into a coherent artificial brain system
    
    try {
        std::cout << "=== NLM Phase 6: Final Integration Demo ===" << std::endl;
        std::cout << "Running complete integration verification..." << std::endl << std::endl;
        
        // Create and run the integration experiment
        nlm::Phase6IntegratedExperiment experiment;
        
        // Optional: Allow command-line arguments to customize test
        nlm::Phase6Config config;
        
        // Parse command line arguments
        if (argc > 1) {
            // Parse neurons and steps from arguments
            int argIndex = 1;
            if (std::string(argv[argIndex]) == "--neurons" && argc > argIndex + 1) {
                config.neuronCount = std::stoull(argv[++argIndex]);
                if (argc > ++argIndex && std::string(argv[argIndex]) == "--steps") {
                    config.maxSteps = std::stoull(argv[++argIndex]);
                }
            } else {
                // Simple mode: use defaults
                config.neuronCount = std::stoull(argv[argIndex]);
            }
        }
        
        // Run the full integration experiment
        auto result = experiment.run(config);
        
        // Display comprehensive results
        std::cout << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << "               PHASE 6 INTEGRATION RESULTS              " << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  • Total Reward:           " << std::fixed << std::setprecision(3) 
                  << result.totalReward << std::endl;
        std::cout << "  • Average Firing Rate:    " << std::fixed << std::setprecision(3) 
                  << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  • Average Synaptic Weight:" << std::fixed << std::setprecision(3) 
                  << result.avgSynapticWeight << std::endl;
        std::cout << "  • Episodes Stored:        " << result.memoryEpisodesStored << std::endl;
        std::cout << "  • Wall Clock Time:        " << std::fixed << std::setprecision(1) 
                  << result.totalWallClockTime << " seconds" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Neuromodulation Levels:" << std::endl;
        std::cout << "  • Novelty Level:          " << std::fixed << std::setprecision(3) 
                  << result.noveltyLevel << std::endl;
        std::cout << "  • Curiosity Level:        " << std::fixed << std::setprecision(3) 
                  << result.curiosityLevel << std::endl;
        std::cout << "  • Dopamine Level:         " << std::fixed << std::setprecision(3) 
                  << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "System Integration Status:" << std::endl;
        std::cout << "  • Working Memory:         " 
                  << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  • Episodic Memory:        " 
                  << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  • Neuromodulation:        " 
                  << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  • Prediction System:      " 
                  << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  • Development System:     " 
                  << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "  • Checkpointing:           " 
                  << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
        std::cout << "  • Replay System:          " 
                  << (result.replayWorks ? "✓ FUNCTIONAL" : "✗ NOT FUNCTIONAL") << std::endl;
        std::cout << std::endl;
        
        // Success indicator
        bool allSystemsIntegrated = result.memoryWorkingMemoryIntegrated && 
                                   result.memoryEpisodicMemoryIntegrated && 
                                   result.neuromodulationIntegrated && 
                                   result.predictionIntegrated && 
                                   result.developmentIntegrated && 
                                   result.checkpointingWorks;
        
        if (allSystemsIntegrated) {
            std::cout << "🎉 SUCCESS: All brain systems are properly integrated!" << std::endl;
            std::cout << "   The NLM artificial brain is functioning as a unified cognitive architecture." << std::endl;
        } else {
            std::cout << "⚠️  WARNING: Some systems are not properly integrated." << std::endl;
            std::cout << "   The brain architecture may require additional work to achieve full integration." << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "=====================================================" << std::endl;
        std::cout << "                 PHASE 6 COMPLETE                    " << std::endl;
        std::cout << "=====================================================" << std::endl;
        
        return allSystemsIntegrated ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ ERROR: Phase 6 integration failed: " << e.what() << std::endl;
        return 1;
    }
}
