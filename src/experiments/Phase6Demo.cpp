/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Usage:
 *   ./nlm_phase6_demo              # Run standard demo
 *   ./nlm_phase6_demo --quick      # Skip individual system tests
 *   ./nlm_phase6_demo --steps N    # Set number of simulation steps
 *   ./nlm_phase6_demo --neurons N  # Set neuron count
 *   ./nlm_phase6_demo --help       # Show this help message
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace nlm;

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Usage: ./nlm_phase6_demo [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "  --quick                 Skip individual system tests (fast verification)" << std::endl;
    std::cout << "  --steps N               Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --neurons N             Neuron count (default: 500)" << std::endl;
    std::cout << "  --regions N             Region count (default: 1)" << std::endl;
    std::cout << "  --checkpoint PATH        Checkpoint file path (default: ./checkpoint_test.bin)" << std::endl;
    std::cout << "  --verbose               Enable verbose logging" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Example: ./nlm_phase6_demo --steps 3000 --neurons 1000 --verbose" << std::endl;
}

bool parseArgs(int argc, char* argv[], Phase6Config& config, bool& skipIndividualTests, bool& verbose) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return false;  // Exit after showing help
        } else if (arg == "--quick") {
            skipIndividualTests = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = std::stoul(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::stoul(argv[++i]);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else {
            std::cerr << "ERROR: Unknown option: " << arg << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase6Config config;
    bool skipIndividualTests = false;
    bool verbose = false;
    
    if (!parseArgs(argc, argv, config, skipIndividualTests, verbose)) {
        return 0;  // Help was shown
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << "Configuration: " << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Regions: " << config.regionCount << std::endl;
    std::cout << "  Max Steps: " << config.maxSteps << std::endl;
    std::cout << "  Checkpoint: " << config.checkpointPath << std::endl;
    std::cout << "  Individual tests: " << (skipIndividualTests ? "Skipped" : "Enabled") << std::endl;
    std::cout << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(verbose ? Logger::Level::Debug : Logger::Level::Info);
    
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
    
    // Test individual systems (unless skipped)
    if (!skipIndividualTests) {
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
    }
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running " << config.maxSteps << " simulation steps..." << std::endl;
    
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
    
    std::cout << std::endl;
    std::cout << "=== Demo Complete ===" << std::endl;
    std::cout << "The NLM brain successfully integrated all systems and completed the experiment." << std::endl;
    
    return 0;
}
