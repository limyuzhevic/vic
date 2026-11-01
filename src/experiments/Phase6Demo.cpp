/**
 * Phase 6 Demo - Integration Test with CLI
 *
 * This demo runs the Phase 6 integration experiment with enhanced
 * command-line options and better user experience.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <fstream>

using namespace nlm;

void printHelp() {
    std::cout << R"(
NLM Phase 6 Integration Demo

Usage: nlm_phase6_demo [OPTIONS]

Options:
  --help, -h                    Show this help message
  --quick, -q                   Run only integration verification (fast check)
  --full, -f                    Run full integration test with all subsystems
  --memory-only                Test only memory integration
  --neuromod-only              Test only neuromodulation integration
  --checkpoint-only            Test only checkpointing
  --replay-only                Test only replay functionality
  --neurons <count>            Override neuron count (default: 500)
  --steps <count>              Override simulation steps (default: 2000)
  --seed <seed>                Set random seed (default: 42)
  --verbose, -v                 Enable verbose logging
  --quiet, -q                   Suppress informational messages
  --output <file>              Save results to file (JSON format)
  --no-checkpoint              Disable checkpointing
  --no-replay                   Disable replay testing
  --no-development              Disable development system

Examples:
  nlm_phase6_demo                        # Run full integration test
  nlm_phase6_demo --quick                 # Quick integration check only
  nlm_phase6_demo --neurons 1000         # Larger network
  nlm_phase6_demo --memory-only           # Test only memory systems
";) << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    bool quickMode = false;
    bool fullMode = false;
    bool memoryOnly = false;
    bool neuromodOnly = false;
    bool checkpointOnly = false;
    bool replayOnly = false;
    int64_t neuronCount = 500;
    size_t maxSteps = 2000;
    int64_t randomSeed = 42;
    bool verbose = false;
    bool quiet = false;
    std::string outputFile;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--quick" || arg == "-q") {
            quickMode = true;
        } else if (arg == "--full" || arg == "-f") {
            fullMode = true;
        } else if (arg == "--memory-only") {
            memoryOnly = true;
        } else if (arg == "--neuromod-only") {
            neuromodOnly = true;
        } else if (arg == "--checkpoint-only") {
            checkpointOnly = true;
        } else if (arg == "--replay-only") {
            replayOnly = true;
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoll(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            maxSteps = static_cast<size_t>(std::stoul(argv[++i]));
        } else if (arg == "--seed" && i + 1 < argc) {
            randomSeed = std::stoll(argv[++i]);
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            quiet = true;
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            enableReplay = false;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        }
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << std::endl;
    
    // Configure logging
    Logger::getGlobal().setLevel(verbose ? LogLevel::Info : LogLevel::Warning);
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Run based on mode selection
    if (quickMode) {
        std::cout << "--- Integration Verification ---" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
        return 0;
    }
    
    // Test individual systems based on mode selection
    if (memoryOnly) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
    }
    
    if (neuromodOnly) {
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
    }
    
    if (checkpointOnly) {
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
    }
    
    if (replayOnly) {
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    }
    
    if (!memoryOnly && !neuromodOnly && !checkpointOnly && !replayOnly) {
        // Run all integration tests
        if (!quiet) {
            std::cout << "--- Integration Verification ---" << std::endl;
        }
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        if (!quiet) {
            std::cout << "Integration verification passed!" << std::endl << std::endl;
        }
        
        // Test individual systems if requested
        if (!memoryOnly) {
            std::cout << "--- Memory Integration Test ---" << std::endl;
            experiment.testMemoryIntegration();
            std::cout << std::endl;
        }
        
        if (!neuromodOnly) {
            std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
            experiment.testNeuromodulationIntegration();
            std::cout << std::endl;
        }
        
        if (!checkpointOnly) {
            std::cout << "--- Checkpoint Test ---" << std::endl;
            experiment.testCheckpointing();
            std::cout << std::endl;
        }
        
        if (!replayOnly) {
            std::cout << "--- Replay Test ---" << std::endl;
            experiment.testReplay();
            std::cout << std::endl;
        }
    }
    
    // Run full experiment with custom configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = neuronCount;
    config.maxSteps = maxSteps;
    config.enableCheckpointing = enableCheckpointing;
    config.enableReplay = enableReplay;
    config.enableDevelopment = enableDevelopment;
    
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
    
    // Save results if requested
    if (!outputFile.empty()) {
        std::cout << "Saving results to: " << outputFile << std::endl;
        if (savePhase6ResultsToFile(result, outputFile)) {
            std::cout << "Results saved successfully!" << std::endl;
        } else {
            std::cerr << "ERROR: Failed to save results!" << std::endl;
        }
    }
    
    return 0;
}

// Additional result structure and save function for Phase 6
struct Phase6IntegrationResult {
    double totalReward;
    double avgFiringRate;
    size_t memoryEpisodesStored;
    float dopamineLevel;
    bool memoryWorkingMemoryIntegrated;
    bool memoryEpisodicMemoryIntegrated;
    bool neuromodulationIntegrated;
    bool predictionIntegrated;
    bool developmentIntegrated;
    bool checkpointingWorks;
    double totalWallClockTime;
};

bool savePhase6ResultsToFile(const Phase6IntegrationResult& result, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{\n";
        file << "  \"totalReward\": " << result.totalReward << ",\n";
        file << "  \"avgFiringRate\": " << result.avgFiringRate << ",\n";
        file << "  \"memoryEpisodesStored\": " << result.memoryEpisodesStored << ",\n";
        file << "  \"dopamineLevel\": " << result.dopamineLevel << ",\n";
        file << "  \"memoryWorkingMemoryIntegrated\": " << (result.memoryWorkingMemoryIntegrated ? "true" : "false") << ",\n";
        file << "  \"memoryEpisodicMemoryIntegrated\": " << (result.memoryEpisodicMemoryIntegrated ? "true" : "false") << ",\n";
        file << "  \"neuromodulationIntegrated\": " << (result.neuromodulationIntegrated ? "true" : "false") << ",\n";
        file << "  \"predictionIntegrated\": " << (result.predictionIntegrated ? "true" : "false") << ",\n";
        file << "  \"developmentIntegrated\": " << (result.developmentIntegrated ? "true" : "false") << ",\n";
        file << "  \"checkpointingWorks\": " << (result.checkpointingWorks ? "true" : "false") << ",\n";
        file << "  \"totalWallClockTime\": " << result.totalWallClockTime << ",\n";
        file << "  \"phase\": \"Phase 6\"\n";
        file << "}\n";
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving results: " << e.what() << std::endl;
        return false;
    }
}