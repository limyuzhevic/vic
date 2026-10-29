// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration with Advanced CLI
//
// This phase implements the complete integrated artificial brain with:
// - All cognitive systems integrated
// - Full neuromodulation integration
// - Memory systems connected
// - Prediction system integrated
// - Advanced command-line interface for power users

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "cli/AdvancedCLI.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <csignal>
#include <algorithm>

using namespace nlm;

// Global variables for signal handling
volatile bool g_running = true;

// Signal handler for graceful shutdown
void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down gracefully..." << std::endl;
    g_running = false;
}

void printBanner() {
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║     NLM — 熙然                                              ║
    ║     Neural Learning Machine                                   ║
    ║                                                              ║
    ║     Phase 6: Final Integration                                ║
    ║                                                              ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates all cognitive systems into a       ║
    ║     coherent brain loop.                                      ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "NLM Phase 6 Advanced Command Line Interface" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Advanced usage options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version                     Show version information" << std::endl;
    std::cout << "  --phase6-demo, --p6-demo      Run Phase 6 integration demo" << std::endl;
    std::cout << "  --phase6, --p6                Run Phase 6 final integration experiment" << std::endl;
    std::cout << "  --neurons N                   Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S                     Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --regions R                   Number of brain regions (default: 1)" << std::endl;
    std::cout << "  --conn-prob P                 Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --save-checkpoint             Save checkpoint at end of run" << std::endl;
    std::cout << "  --load-checkpoint FILE        Load checkpoint from file" << std::endl;
    std::cout << "  --verbose                     Enable verbose output" << std::endl;
    std::cout << "  --quiet                       Suppress most output" << std::endl;
    std::cout << "  --validate                    Validate configuration before execution" << std::endl;
    std::cout << "  --benchmark                   Run benchmark tests" << std::endl;
    std::cout << "  --profile                     Profile performance" << std::endl;
    std::cout << "  --dry-run                     Parse arguments but don't execute" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --phase6-demo --neurons 1000 --steps 5000 --save-checkpoint" << std::endl;
    std::cout << "  nlm --phase6 --conn-prob 0.05 --log-level Info --log-file nlm.log" << std::endl;
    std::cout << "  nlm --validate --profile --benchmark=memory" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine" << std::endl;
    std::cout << "Version: Phase 6.0 - Final Integration" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Author: NLM Research Team" << std::endl;
    std::cout << "License: MIT" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    // Set up signal handlers for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    printBanner();
    
    // Parse advanced command line arguments
    AdvancedCLIOptions options = g_cli.parseArgs(argc, argv);
    
    if (options.dryRun) {
        std::cout << "Dry run mode - configuration validated, execution skipped" << std::endl;
        g_cli.printConfiguration(options);
        return 0;
    }
    
    if (options.validate) {
        if (!g_cli.validateConfiguration(options)) {
            std::cerr << "Configuration validation failed" << std::endl;
            return 1;
        }
        std::cout << "Configuration validation passed" << std::endl;
    }
    
    if (options.benchmark) {
        g_cli.runBenchmark(options);
        return 0;
    }
    
    if (options.profile) {
        g_cli.profilePerformance(options);
        return 0;
    }
    
    // Setup logging
    g_cli.setupLogging(options);
    
    // Execute based on phase selection
    int result = 0;
    
    if (options.runPhase6Demo) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        result = g_cli.executePhase6Demo(options);
    }
    else if (options.runPhase6Experiment) {
        std::cout << "=== NLM Phase 6 Final Integration Experiment ===" << std::endl;
        result = g_cli.executePhase6Experiment(options);
    }
    else {
        // Default to Phase 6 demo
        std::cout << "No phase specified. Defaulting to Phase 6 Demo." << std::endl;
        std::cout << "Use --help for usage information." << std::endl;
        return 1;
    }
    
    // Handle checkpoint management
    if (options.saveCheckpoint || !options.checkpointPath.empty()) {
        g_cli.manageCheckpoints(options);
    }
    
    // Clean up checkpoints if requested
    if (options.cleanCheckpoints) {
        g_cli.cleanCheckpointDirectory(".");
    }
    
    return result;
}

} // namespace nlm