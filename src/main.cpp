// Updated NLM main.cpp with comprehensive error handling and recovery

// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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
#include "experiments/ExperimentRunner.hpp"
#include "core/Errors/Errors.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>

using namespace nlm;

// ============================================================================
// GLOBAL ERROR HANDLING SETUP
// ============================================================================

// Global error recovery context
struct GlobalErrorContext {
    std::vector<std::string> errorLog;
    bool enableGracefulShutdown = true;
    bool enableBackupRecovery = true;
    size_t maxRecoveryAttempts = 3;
    std::chrono::milliseconds recoveryDelay = std::chrono::milliseconds(100);
    
    GlobalErrorContext() {
        // Initialize error handler
        ErrorReportingUtils::setErrorHandler(
            [this](const ErrorReportingUtils::ErrorReport& report) {
                errorLog.push_back(report.errorType + ": " + report.message);
                if (errorLog.size() > 100) {
                    errorLog.erase(errorLog.begin());
                }
            }
        );
    }
    
    void logSystemStatus() const {
        std::stringstream ss;
        ss << "=== SYSTEM STATUS ===\n";
        ss << "Errors recorded: " << errorLog.size() << "\n";
        ss << "Graceful shutdown: " << (enableGracefulShutdown ? "enabled" : "disabled") << "\n";
        ss << "Backup recovery: " << (enableBackupRecovery ? "enabled" : "disabled") << "\n";
        ss << "Max recovery attempts: " << maxRecoveryAttempts << "\n";
        ss << "Recovery delay: " << recoveryDelay.count() << "ms\n";
        
        NLM_LOG_INFO(ss.str());
    }
    
    void attemptRecovery(const std::string& operation, std::function<bool()> func) {
        if (!enableBackupRecovery) {
            return;
        }
        
        size_t attempts = 0;
        while (attempts < maxRecoveryAttempts) {
            try {
                if (func()) {
                    NLM_LOG_INFO(std::string("Recovery successful for ") + operation);
                    return;
                }
            } catch (const NLMException& e) {
                NLM_LOG_WARNING(std::string("Recovery attempt ") + std::to_string(attempts + 1) + 
                               std::string(" for ") + operation + std::string(": ") + e.what());
                attempts++;
                if (attempts < maxRecoveryAttempts) {
                    std::this_thread::sleep_for(recoveryDelay);
                }
            }
        }
        
        NLM_LOG_ERROR(std::string("All recovery attempts failed for ") + operation);
    }
};

GlobalErrorContext globalErrorContext;

// ============================================================================
// GLOBAL ERROR HANDLING SETUP
// ============================================================================

void initializeErrorHandling() {
    try {
        // Initialize error handling system
        GlobalErrorHandler::initialize();
        
        // Enable structured logging
        GlobalErrorHandler::enableStructuredLogging(true);
        
        // Enable error recovery
        GlobalErrorHandler::enableErrorRecovery(true);
        
        // Enable guard checking
        GlobalErrorHandler::enableGuardChecking(true);
        
        // Add basic system guards
        SafeGuardUtils::addGuard(std::make_unique<SafeGuardUtils::MemoryGuard>(10 * 1024 * 1024)); // 10MB minimum
        
        NLM_LOG_INFO("Error handling system initialized successfully");
        
    } catch (const NLMException& e) {
        NLM_LOG_CRITICAL(std::string("Failed to initialize error handling: ") + e.what());
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL(std::string("Unexpected error during error handling initialization: ") + e.what());
        NLM_THROW(NLMOperationException, std::string("Error handling initialization failed: ") + e.what());
    }
}

void shutdownErrorHandling() {
    try {
        // Log final error summary
        NLM_LOG_INFO(GlobalErrorHandler::getErrorSummary());
        
        // Disable all guards
        SafeGuardUtils::clearAllGuards();
        
        // Shutdown error handling system
        GlobalErrorHandler::shutdown();
        
        NLM_LOG_INFO("Error handling system shut down successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL(std::string("Error during error handling shutdown: ") + e.what());
    }
}

// ============================================================================
// CLI Mode enumeration
// ============================================================================

enum class CLIMode {
    Research,
    Education,
    Demonstration,
    Test
};

// Verbosity levels
enum class VerbosityLevel {
    Quiet = 0,
    Normal = 1,
    Detailed = 2
};

// Test filtering options
enum class TestFilter {
    None,
    BasicConnectivity,
    Plasticity,
    Stdp,
    All
};

// Command-line options structure
struct CLIOptions {
    CLIMode mode = CLIMode::Demonstration;
    VerbosityLevel verbosity = VerbosityLevel::Normal;
    TestFilter testFilter = TestFilter::All;
    std::string configFile = "configs/default.cfg";
    bool showHelp = false;
    bool showVersion = false;
    bool showBanner = true;
    bool runProgress = false;
    bool validateOnly = false;
    size_t maxSteps = 0;
    std::string outputFile;
};

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Parse command line arguments
bool parseCommandLine(int argc, char* argv[], CLIOptions& options) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help") {
                options.showHelp = true;
            } else if (arg == "--version") {
                options.showVersion = true;
            } else if (arg == "--no-banner") {
                options.showBanner = false;
            } else if (arg == "--validate-only") {
                options.validateOnly = true;
            } else if (arg == "--progress") {
                options.runProgress = true;
            } else if (arg == "--filter") {
                if (i + 1 < argc) {
                    options.testFilter = CLIUtils::stringToTestFilter(argv[++i]);
                }
            } else if (arg == "--mode") {
                if (i + 1 < argc) {
                    options.mode = CLIUtils::stringToMode(argv[++i]);
                }
            } else if (arg == "--verbosity") {
                if (i + 1 < argc) {
                    std::string level = argv[++i];
                    if (level == "quiet") options.verbosity = VerbosityLevel::Quiet;
                    else if (level == "detailed") options.verbosity = VerbosityLevel::Detailed;
                    else options.verbosity = VerbosityLevel::Normal;
                }
            } else if (arg == "--config") {
                if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            } else if (arg == "--output") {
                if (i + 1 < argc) {
                    options.outputFile = argv[++i];
                }
            } else if (arg == "--max-steps") {
                if (i + 1 < argc) {
                    options.maxSteps = std::stoul(argv[++i]);
                }
            } else if (arg.substr(0, 2) == "--") {
                NLM_THROW(NLMValueError, std::string("Unknown option: ") + arg);
            }
        }
        
        return true;
    } catch (const NLMException& e) {
        NLM_LOG_ERROR(std::string("Command line parsing error: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error parsing command line: ") + e.what());
        return false;
    }
}

// Load and validate configuration
std::shared_ptr<Config> loadConfiguration(const std::string& configFile) {
    NLM_TRY_CATCH(
        // Validate configuration file exists
        if (!ValidationUtils::fileExists(configFile)) {
            NLM_THROW(NLMConfigurationException, std::string("Configuration file not found: ") + configFile);
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration
        if (!config->loadFromFile(configFile)) {
            NLM_THROW(NLMConfigurationException, std::string("Failed to load configuration from: ") + configFile);
        }
        
        // Validate configuration
        globalErrorContext.attemptRecovery("configuration validation", [config]() {
            // Validate essential keys
            if (!config->has("neuron_count") || 
                config->get("neuron_count") <= 0) {
                return false;
            }
            
            if (!config->has("region_count") || 
                static_cast<size_t>(config->get("region_count")) == 0) {
                return false;
            }
            
            return true;
        });
        
        // Generate configuration summary
        NLM_LOG_INFO(std::string("Configuration loaded from: ") + configFile);
        NLM_LOG_INFO(config->summary());
        
        return config;
    ,
        std::string("Load configuration from ") + configFile
    )
}

// Validate configuration parameters
bool validateConfiguration(const std::shared_ptr<Config>& config) {
    NLM_TRY_CATCH(
        if (!config) {
            NLM_THROW(NLMConfigurationException, "Cannot validate: Configuration is null");
        }
        
        // Validate essential parameters
        if (!config->has("neuron_count")) {
            NLM_LOG_WARNING("Configuration missing: neuron_count");
            return false;
        }
        
        if (!config->has("region_count")) {
            NLM_LOG_WARNING("Configuration missing: region_count");
            return false;
        }
        
        // Validate parameter ranges
        size_t neuronCount = static_cast<size_t>(config->get("neuron_count"));
        if (neuronCount == 0 || neuronCount > 1000000) {
            NLM_LOG_ERROR(std::string("Invalid neuron_count: ") + std::to_string(neuronCount));
            return false;
        }
        
        size_t regionCount = static_cast<size_t>(config->get("region_count"));
        if (regionCount == 0 || regionCount > 1000) {
            NLM_LOG_ERROR(std::string("Invalid region_count: ") + std::to_string(regionCount));
            return false;
        }
        
        // Validate other parameters if present
        if (config->has("simulation_timestep")) {
            double timestep = config->get("simulation_timestep");
            if (timestep <= 0 || timestep > 1.0) {
                NLM_LOG_ERROR(std::string("Invalid simulation_timestep: ") + std::to_string(timestep));
                return false;
            }
        }
        
        return true;
    ,
        "Configuration validation"
    )
}

// ============================================================================
// CLI UTILITIES
// ============================================================================

namespace CLIUtils {
    // Existing utility functions...
    
    // Enhanced error reporting
    void printError(const std::string& message) {
        NLM_LOG_ERROR(std::string("Error: ") + message);
    }
    
    void printSuccess(const std::string& message) {
        NLM_LOG_INFO(std::string("Success: ") + message);
    }
    
    void printWarning(const std::string& message) {
        NLM_LOG_WARNING(std::string("Warning: ") + message);
    }
}

// ============================================================================
// MAIN ENTRY POINT WITH COMPREHENSIVE ERROR HANDLING
// ============================================================================

int main(int argc, char* argv[]) {
    try {
        // Initialize error handling system
        initializeErrorHandling();
        
        // Parse command line arguments
        CLIOptions options;
        if (!parseCommandLine(argc, argv, options)) {
            CLIUtils::printHelp("nlm");
            return 1;
        }
        
        // Handle help and version
        if (options.showHelp) {
            CLIUtils::printHelp("nlm");
            return 0;
        }
        
        if (options.showVersion) {
            CLIUtils::printVersion();
            return 0;
        }
        
        // Print banner if enabled
        if (options.showBanner) {
            printBanner();
        }
        
        // Initialize logging
        Logger::getGlobal().setLevel(LogLevel::Info);
        NLM_LOG_INFO(std::string("NLM starting with mode: ") + modeToString(options.mode));
        
        // Load and validate configuration
        auto config = loadConfiguration(options.configFile);
        if (!validateConfiguration(config)) {
            NLM_THROW(NLMConfigurationException, "Configuration validation failed");
        }
        
        // Validate only mode
        if (options.validateOnly) {
            NLM_LOG_INFO("Configuration validation completed successfully");
            globalErrorContext.logSystemStatus();
            return 0;
        }
        
        // Create brain with error recovery
        std::cout << "Creating brain with configuration..." << std::flush;
        globalErrorContext.attemptRecovery("brain creation", [&]() {
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                return false;
            }
            
            std::cout << "Done!" << std::endl;
            
            // Run based on mode
            switch (options.mode) {
                case CLIMode::Research:
                    ModeHandlers::runResearchMode(brain, options.verbosity, options.testFilter, 
                                                 options.maxSteps, options.runProgress);
                    break;
                case CLIMode::Education:
                    ModeHandlers::runEducationMode(brain, options.verbosity, options.testFilter,
                                                 options.maxSteps, options.runProgress);
                    break;
                case CLIMode::Demonstration:
                    ModeHandlers::runDemonstrationMode(brain, options.verbosity, options.testFilter,
                                                     options.maxSteps, options.runProgress);
                    break;
                case CLIMode::Test:
                    ModeHandlers::runTestMode(brain, options.verbosity, options.testFilter,
                                             options.maxSteps, options.runProgress);
                    break;
            }
            
            // Log final status
            brain->logStatus();
            
            return true;
        });
        
        // If brain creation failed, attempt fallback
        std::cerr << "ERROR: Failed to create and run brain after all recovery attempts." << std::endl;
        globalErrorContext.logSystemStatus();
        return 1;
        
    } catch (const NLMConfigurationException& e) {
        NLM_LOG_CRITICAL(std::string("Configuration error: ") + e.what());
        std::cerr << "ERROR: Configuration error: " << e.what() << std::endl;
        return 1;
    } catch (const NLMValueError& e) {
        NLM_LOG_CRITICAL(std::string("Value error: ") + e.what());
        std::cerr << "ERROR: Value error: " << e.what() << std::endl;
        return 1;
    } catch (const NLMStateException& e) {
        NLM_LOG_CRITICAL(std::string("State error: ") + e.what());
        std::cerr << "ERROR: State error: " << e.what() << std::endl;
        return 1;
    } catch (const NLMMemoryException& e) {
        NLM_LOG_CRITICAL(std::string("Memory error: ") + e.what());
        std::cerr << "ERROR: Memory error: " << e.what() << std::endl;
        return 1;
    } catch (const NLMRessourceException& e) {
        NLM_LOG_CRITICAL(std::string("Resource error: ") + e.what());
        std::cerr << "ERROR: Resource error: " << e.what() << std::endl;
        return 1;
    } catch (const NLMOperationException& e) {
        NLM_LOG_CRITICAL(std::string("Operation error: ") + e.what());
        std::cerr << "ERROR: Operation error: " << e.what() << std::endl;
        return 1;
    } catch (const NLMException& e) {
        NLM_LOG_CRITICAL(std::string("NLM error: ") + e.what());
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL(std::string("Unexpected error: ") + e.what());
        std::cerr << "ERROR: Unexpected error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        NLM_LOG_CRITICAL("Unknown error occurred");
        std::cerr << "ERROR: Unknown error occurred" << std::endl;
        return 1;
    }
}