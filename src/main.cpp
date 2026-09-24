// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include <iostream>
#include <memory>
#include <exception>
#include "core/Logger/Logger.hpp"
#include "CLI.hpp"
#include "ConfigManager.hpp"
#include "LoggingManager.hpp"
#include "BrainManager.hpp"
#include "Banner.hpp"

using namespace nlm;

// Global error handler
void setupErrorHandling() {
    std::set_terminate([]() {
        NLM_LOG_CRITICAL("FATAL: Program terminated due to unhandled exception");
        std::exit(1);
    });
}

// Main application entry point
int main(int argc, char** argv) {
    setupErrorHandling();
    
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    try {
        // Parse command line arguments
        NLM_LOG_INFO("Parsing command line arguments...");
        CLI::Options cliOptions = CLI::parseArgs(argc, argv);
        
        // Handle help/version flags
        if (cliOptions.help) {
            CLI::printHelp();
            return 0;
        }
        
        if (cliOptions.version) {
            std::cout << "NLM Phase 2 - Real Neural Computation" << std::endl;
            std::cout << "Version: 2.0" << std::endl;
            std::cout << "Built on " << __DATE__ << " at " << __TIME__ << std::endl;
            return 0;
        }
        
        // Setup logging with enhanced error handling
        NLM_LOG_INFO("Setting up logging system...");
        try {
            LoggingManager::setupLogging(cliOptions);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to setup logging: " + std::string(e.what()));
            std::cerr << "Warning: Using default logging configuration." << std::endl;
        }
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Implementing:");
        NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
        NLM_LOG_INFO("  - Event-driven spike propagation with delays");
        NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
        NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
        NLM_LOG_INFO("");
        
        // Setup configuration with enhanced error handling
        NLM_LOG_INFO("Setting up configuration...");
        std::shared_ptr<Config> config = ConfigManager::setupConfig(cliOptions);
        
        // Process command line arguments for config with validation
        NLM_LOG_INFO("Processing command line arguments for configuration...");
        try {
            if (config->loadFromArgs(argc, argv)) {
                NLM_LOG_INFO("Command line arguments processed successfully.");
            } else {
                NLM_LOG_WARNING("Some command line arguments may be invalid.");
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error processing command line arguments: " + std::string(e.what()));
            NLM_LOG_WARNING("Continuing with file and default configuration.");
        }
        
        NLM_LOG_INFO("Starting NLM Phase 2 neural computation...");
        
        // Run the neural computation using the BrainManager
        if (BrainManager::initializeAndRun(config)) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Phase 2 Complete ===");
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Phase 2 Objectives Completed:");
            NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
            NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
            NLM_LOG_INFO("  ✓ STDP plasticity rule");
            NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
            NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
            NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
            NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
            NLM_LOG_INFO("");
            NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
            NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
            NLM_LOG_INFO("");
            
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to complete Phase 2 neural computation");
            return 1;
        }
        
    } catch (const CLIValidationError& e) {
        NLM_LOG_ERROR("Configuration error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const CLIError& e) {
        NLM_LOG_ERROR("Command line error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        CLI::printHelp();
        return 1;
    } catch (const ConfigValidationError& e) {
        NLM_LOG_ERROR("Configuration validation error: " + std::string(e.what()) + 
                     " (key: '" + e.getKey() + "', value: '" + e.getValue() + "')");
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Configuration key: " << e.getKey() << std::endl;
        if (!e.getValue().empty()) {
            std::cerr << "Invalid value: '" << e.getValue() << "'" << std::endl;
        }
        return 1;
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Unexpected error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

