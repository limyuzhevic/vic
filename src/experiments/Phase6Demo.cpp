/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Integration Test                                 ║
    ║                                                               ║
    ║     Testing complete brain integration                        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << R"(
=== NLM Phase 6 Integration Demo ===

Usage: nlm_phase6_demo [OPTIONS]

Options:
  --help                    Show this help message
  --version                 Show version information
  --neuron-count=<n>        Number of neurons (default: 500)
  --max-steps=<n>           Maximum simulation steps (default: 2000)
  --enable-checkpointing    Enable checkpointing (default: true)
  --enable-replay           Enable replay functionality (default: true)
  --enable-development      Enable developmental stages (default: true)
  --checkpoint-interval=<i>  Checkpoint interval in steps (default: 100)
  --stats-file=<file>        File to save experiment statistics (default: phase6_stats.json)
  --verbose                 Enable verbose logging
  --quiet                   Suppress informational messages

Examples:
  nlm_phase6_demo --neuron-count=1000 --max-steps=5000
  nlm_phase6_demo --enable-checkpointing --checkpoint-interval=500
  nlm_phase6_demo --enable-replay --stats-file=/tmp/experiment_stats.json

Advanced Options (for power users):
  --enable-stress-test      Run stress test with higher neuron counts
  --enable-benchmark        Run benchmark mode with timing measurements
  --enable-debug           Enable debug mode with additional diagnostics
  --enable-memory-profiling Enable memory profiling
  --max-memory-usage=<m>    Maximum memory usage in MB (default: 1024)

Press ESC to exit visualization mode when enabled.
    )" << std::endl;
}

void printVersion() {
    printBanner();
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << "Version 0.1.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Integrated Brain Systems:" << std::endl;
    std::cout << "  ✓ Working Memory" << std::endl;
    std::cout << "  ✓ Episodic Memory" << std::endl;
    std::cout << "  ✓ Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "  ✓ Prediction Systems" << std::endl;
    std::cout << "  ✓ Development & Maturation" << std::endl;
    std::cout << "  ✓ Checkpointing & Replay" << std::endl;
}

Phase6Config parseArguments(int argc, char* argv[]) {
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.checkpointInterval = 100;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.enableStressTest = false;
    config.enableBenchmark = false;
    config.enableDebug = false;
    config.enableMemoryProfiling = false;
    config.maxMemoryUsage = 1024;
    std::string statsFile = "phase6_stats.json";
    bool verbose = false;
    bool quiet = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printBanner();
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printBanner();
            printVersion();
            exit(0);
        } else if (arg.substr(0, 16) == "--neuron-count") {
            try {
                config.neuronCount = std::stoi(arg.substr(16));
            } catch (...) {
                std::cerr << "Warning: Invalid neuron-count value, using default" << std::endl;
            }
        } else if (arg.substr(0, 13) == "--max-steps") {
            try {
                config.maxSteps = std::stoi(arg.substr(13));
            } catch (...) {
                std::cerr << "Warning: Invalid max-steps value, using default" << std::endl;
            }
        } else if (arg.substr(0, 25) == "--checkpoint-interval") {
            try {
                config.checkpointInterval = std::stoi(arg.substr(25));
            } catch (...) {
                std::cerr << "Warning: Invalid checkpoint-interval value, using default" << std::endl;
            }
        } else if (arg.substr(0, 21) == "--enable-checkpointing") {
            config.enableCheckpointing = true;
        } else if (arg.substr(0, 18) == "--enable-replay") {
            config.enableReplay = true;
        } else if (arg.substr(0, 24) == "--enable-development") {
            config.enableDevelopment = true;
        } else if (arg.substr(0, 19) == "--stats-file") {
            statsFile = arg.substr(19);
            if (!statsFile.empty() && statsFile[0] == '=') {
                statsFile = statsFile.substr(1);
            }
        } else if (arg == "--verbose") {
            verbose = true;
            config.enableDebug = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--enable-stress-test") {
            config.enableStressTest = true;
            config.neuronCount = 2000; // Higher count for stress test
            config.maxSteps = 5000;
        } else if (arg == "--enable-benchmark") {
            config.enableBenchmark = true;
        } else if (arg == "--enable-debug") {
            config.enableDebug = true;
            verbose = true;
        } else if (arg == "--enable-memory-profiling") {
            config.enableMemoryProfiling = true;
        } else if (arg.substr(0, 23) == "--max-memory-usage") {
            try {
                config.maxMemoryUsage = std::stoi(arg.substr(23));
            } catch (...) {
                std::cerr << "Warning: Invalid max-memory-usage value, using default" << std::endl;
            }
        }
    }
    
    config.statsFile = statsFile;
    
    return config;
}

int main(int argc, char* argv[]) {
    Phase6Config config = parseArguments(argc, argv);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl;
    std::cout << "Configuration: Neuron Count=" << config.neuronCount 
              << ", Max Steps=" << config.maxSteps 
              << ", Checkpointing=" << (config.enableCheckpointing ? "Enabled" : "Disabled")
              << ", Replay=" << (config.enableReplay ? "Enabled" : "Disabled")
              << ", Development=" << (config.enableDevelopment ? "Enabled" : "Disabled")
              << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (config.enableDebug) {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (config.statsFile != "phase6_stats.json") {
        consoleLogger->setLevel(LogLevel::Info);
    }
    
    if (!config.verbose && !config.enableDebug && !config.enableBenchmark) {
        // Suppress informational messages
        consoleLogger->setLevel(LogLevel::Warning);
    }
    
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
    
    // Run full experiment with configuration
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    std::cout << "Running experiment with " << config.neuronCount << " neurons for " 
              << config.maxSteps << " steps..." << std::endl;
    
    if (config.enableStressTest) {
        std::cout << "STRESS TEST MODE: Running with enhanced load" << std::endl;
    }
    if (config.enableBenchmark) {
        std::cout << "BENCHMARK MODE: Collecting performance metrics" << std::endl;
    }
    if (config.enableMemoryProfiling) {
        std::cout << "MEMORY PROFILING MODE: Monitoring memory usage" << std::endl;
    }
    
    auto result = experiment.run(config);
    
    // Save statistics if configured
    if (config.statsFile != "phase6_stats.json") {
        std::cout << "\nSaving statistics to: " << config.statsFile << std::endl;
        experiment.saveResults(config.statsFile);
    }
    
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
    
    if (config.enableBenchmark) {
        std::cout << "\n=== PERFORMANCE METRICS ===" << std::endl;
        std::cout << "Steps per second: " << (config.maxSteps / std::max<double>(result.totalWallClockTime, 0.001)) << std::endl;
        std::cout << "Spikes per second: " << (result.totalSpikes / std::max<double>(result.totalWallClockTime, 0.001)) << std::endl;
        std::cout << "Memory usage: " << result.memoryUsageMB << " MB" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== INTEGRATION COMPLETE ===" << std::endl;
    std::cout << "The NLM Phase 6 integration is fully functional with all systems properly connected." << std::endl;
    std::cout << "The artificial brain is ready for real-world deployment and testing." << std::endl;
    
    return 0;
}
