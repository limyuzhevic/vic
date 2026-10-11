// NLM (熙然) - Neural Learning Machine
// Enhanced Phase 2: Real Neural Computation with Batch Processing
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Comprehensive batch processing capabilities
//
// This enhanced version includes:
// - Multiple configuration file processing
// - Different execution profiles (quick, standard, thorough)
// - Configurable batch sizes and parallelization
// - Progress tracking and reporting
// - Error handling and recovery for batches
// - Parameter variation generation
// - Cross-batch result aggregation and comparison
// - Statistical analysis of batch results
// - Success/failure classification
// - Result filtering and visualization preparation
// - Multi-threading for parallel batch execution
// - Load balancing across CPU cores
// - Resource management for batch runs
// - Thread-safe result aggregation

#include "cli/CLIEngine.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Enhanced Edition with Batch Processing                    ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║     - Enhanced batch processing capabilities                  ║
    ║     - Parameter sweep and variations                          ║
    ║     - Parallel execution support                              ║
    ║     - Results aggregation and analysis                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Create and use the enhanced CLI Engine with batch processing
    auto cliEngine = std::make_shared<CLIEngine>();
    
    if (!cliEngine->parseArguments(argc, argv)) {
        // Help or version displayed, or error occurred
        return 0;
    }
    
    // Check if batch mode is explicitly requested
    bool batchModeRequested = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--batch" || arg == "--batch-mode" || 
            (arg.find("--batch") == 0 && arg.find("=") != std::string::npos)) {
            batchModeRequested = true;
            break;
        }
    }
    
    // For backward compatibility, automatically enable batch mode
    // if batch config files are provided
    if (!cliEngine->getBatchConfigFiles().empty() && !batchModeRequested) {
        NLM_LOG_INFO("Batch configuration files detected, enabling batch mode");
        cliEngine->setBatchMode(true);
        batchModeRequested = true;
    }
    
    // Execute main application
    int result = cliEngine->execute();
    
    // Generate final summary if batch mode was used
    if (batchModeRequested) {
        NLM_LOG_INFO("=== Batch Processing Summary ===");
        NLM_LOG_INFO("Batch processing completed with enhanced features");
        NLM_LOG_INFO("Results are available in the output file");
        NLM_LOG_INFO("For detailed analysis, check the results aggregator output");
    }
    
    return result;
}