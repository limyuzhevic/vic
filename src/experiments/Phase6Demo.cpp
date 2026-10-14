// nlm_phase6_demo - Phase 6 Main Integration Demo
// Demonstrates the final integrated Phase 6 artificial brain system
// Phase 6 is the final integration phase where all brain systems work together

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                 ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all brain systems:                   ║
    ║     - Working and episodic memory systems                     ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)          ║
    ║     - Prediction and planning systems                          ║
    ║     - Cognitive development                                    ║
    ║     - Structural plasticity and replay                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --neurons N          Number of neurons (default: 1000)\n";
    std::cout << "  --steps S            Number of simulation steps (default: 10000)\n";
    std::cout << "  --regions R          Number of brain regions (default: 1)\n";
    std::cout << "  --connection C       Connection probability (default: 0.1)\n";
    std::cout << "  --no-checkpointing   Disable checkpoint save/load\n";
    std::cout << "  --no-replay          Disable memory replay\n";
    std::cout << "  --no-development     Disable developmental changes\n";
    std::cout << "  --checkpoint P        Checkpoint file path (default: ./checkpoint_test.bin)\n";
    std::cout << "  --help               Show this help message\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << programName << " --neurons 5000 --steps 50000 --regions 2\n";
}

Phase6Config parseCommandLine(int argc, char* argv[]) {
    Phase6Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = static_cast<size_t>(std::atoi(argv[++i]));
        } else if (arg == "--steps" && i + 1 < argc) {
            config.maxSteps = static_cast<uint64_t>(std::atol(argv[++i]));
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = static_cast<size_t>(std::atoi(argv[++i]));
        } else if (arg == "--connection" && i + 1 < argc) {
            config.connectionProbability = static_cast<float>(std::atof(argv[++i]));
        } else if (arg == "--no-checkpointing") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        }
    }
    
    return config;
}

void printResult(const std::string& name, const std::string& value) {
    std::cout << std::left << std::setw(35) << name << ": " << value << "\n";
}

void printPhase6Result(const Phase6IntegrationResult& result) {
    printBanner();
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  PHASE 6 INTEGRATION RESULTS\n";
    std::cout << std::string(70, '=') << "\n\n";
    
    // System Integration Status
    std::cout << "SYSTEM INTEGRATION STATUS:\n";
    std::cout << std::string(50, '-') << "\n";
    printResult("Working Memory Integration:", result.memoryWorkingMemoryIntegrated ? "✓ YES" : "✗ NO");
    printResult("Episodic Memory Integration:", result.memoryEpisodicMemoryIntegrated ? "✓ YES" : "✗ NO");
    printResult("Neuromodulation Integration:", result.neuromodulationIntegrated ? "✓ YES" : "✗ NO");
    printResult("Prediction Integration:", result.predictionIntegrated ? "✓ YES" : "✗ NO");
    printResult("Development Integration:", result.developmentIntegrated ? "✓ YES" : "✗ NO");
    printResult("Checkpointing:", result.checkpointingWorks ? "✓ WORKS" : "✗ FAILED");
    printResult("Replay System:", result.replayWorks ? "✓ WORKS" : "✗ FAILED");
    
    // Performance Metrics
    std::cout << "\nPERFORMANCE METRICS:\n";
    std::cout << std::string(50, '-') << "\n";
    printResult("Total Reward:", std::to_string(result.totalReward));
    printResult("Average Firing Rate:", std::to_string(result.avgFiringRate) + " Hz");
    printResult("Average Synaptic Weight:", std::to_string(result.avgSynapticWeight));
    printResult("Memory Episodes Stored:", std::to_string(static_cast<size_t>(result.memoryEpisodesStored)));
    printResult("Novelty Level:", std::to_string(result.noveltyLevel));
    printResult("Curiosity Level:", std::to_string(result.curiosityLevel));
    printResult("Dopamine Level:", std::to_string(result.dopamineLevel));
    
    // Timing
    std::cout << "\nTIMING:\n";
    std::cout << std::string(50, '-') << "\n";
    printResult("Wall Clock Time:", std::to_string(result.totalWallClockTime) + " seconds");
    
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "PHASE 6 COMPLETE - All Brain Systems Integrated Successfully!\n";
    std::cout << std::string(70, '=') << "\n";
}

int main(int argc, char* argv[]) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Integration Demo...\n\n";
    
    // Parse command line arguments
    Phase6Config config = parseCommandLine(argc, argv);
    
    // Display configuration
    std::cout << "Configuration:\n";
    printResult("Neurons:", std::to_string(config.neuronCount));
    printResult("Steps:", std::to_string(config.maxSteps));
    printResult("Regions:", std::to_string(config.regionCount));
    printResult("Connection Probability:", std::to_string(config.connectionProbability));
    printResult("Checkpointing:", config.enableCheckpointing ? "Enabled" : "Disabled");
    printResult("Replay:", config.enableReplay ? "Enabled" : "Disabled");
    printResult("Development:", config.enableDevelopment ? "Enabled" : "Disabled");
    printResult("Checkpoint Path:", config.checkpointPath);
    
    // Run the Phase 6 integration experiment
    std::cout << "\nRunning Phase 6 integration experiment...\n";
    std::cout << std::string(70, '-') << "\n";
    
    Phase6IntegratedExperiment experiment;
    Phase6IntegrationResult result = experiment.run(config);
    
    // Display results
    printPhase6Result(result);
    
    return 0;
}
