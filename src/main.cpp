// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This demo demonstrates the complete integrated artificial brain:
// - Memory systems connected to neural processing
// - Neuromodulation affecting plasticity and neural dynamics
// - Development affecting plasticity rates
// - Prediction system integrated
// - Replay and consolidation functional
// - Checkpoint save/load working
//
// Users can select different phases via command line:
//   --phase 2  : Basic neural computation (Phase 2 demo)
//   --phase 4  : Advanced brain systems (Phase 4 demo)
//   --phase 5  : Performance optimized (Phase 5 demo)
//   --phase 6  : Full integration (Phase 6 demo - default)
//   --help     : Show help information
//
// Example usage:
//   ./nlm                    // Run Phase 6 demo (default)
//   ./nlm --phase 2          // Run Phase 2 demo
//   ./nlm --phase 6 --config myconfig.cfg

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>

using namespace nlm;

// Phase 2 demo - Basic neural computation
void runPhase2Demo() {
    std::cout << "=== NLM Phase 2: Basic Neural Computation ===" << std::endl;
    std::cout << "Implementing:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neuron dynamics" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << std::endl;
    
    // Create Phase 2 demo using the original implementation
    // This would include the learning experiments and plasticity demonstrations
    std::cout << "Note: Phase 2 demo would be implemented here" << std::endl;
    std::cout << "Current main.cpp contains the Phase 2 implementation." << std::endl;
}

// Phase 4 demo - Advanced brain systems
void runPhase4Demo() {
    std::cout << "=== NLM Phase 4: Advanced Brain Systems ===" << std::endl;
    std::cout << "Implementing:" << std::endl;
    std::cout << "  - Neural prediction system" << std::endl;
    std::cout << "  - Working memory" << std::endl;
    std::cout << "  - Episodic-like memory" << std::endl;
    std::cout << "  - Concept formation" << std::endl;
    std::cout << "  - Neural attention (NOT Transformer)" << std::endl;
    std::cout << "  - Predictive planning" << std::endl;
    std::cout << "  - Self-model" << std::endl;
    std::cout << "  - Social learning" << std::endl;
    std::cout << "  - Continual learning" << std::endl;
    std::cout << std::endl;
    
    // Create Phase 4 demo
    std::cout << "Note: Phase 4 demo would be implemented here" << std::endl;
    std::cout << "Current Phase 4 experiments are in src/experiments/Phase4Demo.cpp" << std::endl;
}

// Phase 5 demo - Performance optimized
void runPhase5Demo() {
    std::cout << "=== NLM Phase 5: Performance Optimized ===" << std::endl;
    std::cout << "Implementing:" << std::endl;
    std::cout << "  - Memory pools and event queues" << std::endl;
    std::cout << "  - SIMD vectorization" << std::endl;
    std::cout << "  - Parallel processing" << std::endl;
    std::cout << "  - Checkpoint system" << std::endl;
    std::cout << std::endl;
    
    // Create Phase 5 demo
    std::cout << "Note: Phase 5 demo would be implemented here" << std::endl;
    std::cout << "Current Phase 5 experiments are in src/experiments/Phase5Experiment.cpp" << std::endl;
}

// Phase 6 demo - Full integration
void runPhase6Demo(const std::string& configFile = "configs/default.cfg") {
    std::cout << "=== NLM Phase 6: Final Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
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
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
}

void printHelp() {
    std::cout << "=== NLM (熙然) - Neural Learning Machine ===" << std::endl;
    std::cout << "Phase-based artificial developmental brain simulation" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --phase 2    Run Phase 2: Basic neural computation" << std::endl;
    std::cout << "  --phase 4    Run Phase 4: Advanced brain systems" << std::endl;
    std::cout << "  --phase 5    Run Phase 5: Performance optimized" << std::endl;
    std::cout << "  --phase 6    Run Phase 6: Full integration (default)" << std::endl;
    std::cout << "  --config FILE   Load configuration from FILE" << std::endl;
    std::cout << "  --help        Show this help message" << std::endl;
    std::cout << "  --version     Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm                    # Run Phase 6 demo (default)" << std::endl;
    std::cout << "  ./nlm --phase 2          # Run Phase 2 demo" << std::endl;
    std::cout << "  ./nlm --phase 6 --config myconfig.cfg" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version 0.1.0 (Phase 6.0)" << std::endl;
    std::cout << "Built with C++20, featuring:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neuron dynamics" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  - Full memory system integration" << std::endl;
    std::cout << "  - Neuromodulation systems" << std::endl;
    std::cout << "  - Prediction and cognition" << std::endl;
    std::cout << "  - Development system" << std::endl;
    std::cout << "  - Checkpoint and replay" << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string phase = "6"; // Default phase
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--phase" && i + 1 < argc) {
            phase = argv[++i];
        } else if (arg == "--phase2") {
            phase = "2";
        } else if (arg == "--phase4") {
            phase = "4";
        } else if (arg == "--phase5") {
            phase = "5";
        } else if (arg == "--phase6") {
            phase = "6";
        }
    }
    
    try {
        // Run selected phase
        if (phase == "2") {
            runPhase2Demo();
        } else if (phase == "4") {
            runPhase4Demo();
        } else if (phase == "5") {
            runPhase5Demo();
        } else {
            // Default to Phase 6
            runPhase6Demo(configFile);
        }
        
        std::cout << std::endl;
        std::cout << "=== DEMO COMPLETE ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}