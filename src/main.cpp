// Main unified NLM entry point
// Uses NLMCLI for command-line parsing and configuration
// Supports multiple experiment types via a unified interface

#include "NLMCLI.h"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace nlm;

class NLMApplication {
public:
    NLMApplication() : cli() {
        // Initialize global error handling
        if (!initializeErrorHandling()) {
            throw std::runtime_error("Failed to initialize error handling system");
        }
    }
    
    bool initialize(int argc, char* argv[]) {
        // Parse command line arguments using enhanced CLI
        if (!cli.parseArguments(argc, argv)) {
            return false;
        }
        
        // Display version if requested
        if (shouldShowVersion(argc, argv)) {
            cli.displayVersion();
            return false;
        }
        
        // Display help if requested or if no valid experiment type specified
        if (shouldShowHelp(argc, argv)) {
            cli.displayHelp();
            return false;
        }
        
        // Validate options
        if (!cli.validateOptions()) {
            return false;
        }
        
        // Process options
        if (!cli.processOptions()) {
            return false;
        }
        
        // Validate configuration if requested
        if (cli.getOptions().validateConfig) {
            if (!cli.validateConfiguration()) {
                std::cerr << "ERROR: Configuration validation failed" << std::endl;
                return false;
            }
        }
        
        // Initialize logging
        initializeLogging();
        
        // Log startup information
        logStartupInfo();
        
        return true;
    }
    
    void run() {
        NLM_LOG_INFO("Starting NLM unified application...");
        
        // Start performance monitoring if requested
        if (cli.getOptions().profilePerformance) {
            cli.startPerformanceMonitoring();
        }
        
        try {
            // Create and configure brain based on CLI options
            auto brain = createBrainFromConfig();
            
            if (!brain->initialize()) {
                throw NLMError("Failed to initialize brain", 
                             ErrorCategory::SimulationError,
                             "Check system resources and configuration");
            }
            
            brain->logStatus();
            
            // Run selected experiment
            switch (cli.getOptions().experimentType) {
                case 1: // Phase 3
                    runPhase3Experiment(brain);
                    break;
                case 2: // Phase 4
                    runPhase4Experiment(brain);
                    break;
                case 3: // Phase 6
                    runPhase6Experiment(brain);
                    break;
                default: // Basic simulation
                    runBasicSimulation(brain);
                    break;
            }
            
            // Export results if requested
            exportAllResults(brain);
            
        } catch (const NLMError& e) {
            NLM_LOG_ERROR("NLM Error: " + std::string(e.what()));
            NLM_LOG_INFO("Suggestion: " + e.getSuggestion());
            if (e.isRecoverable()) {
                NLM_LOG_INFO("This error is recoverable. Check configuration and logs for details.");
            }
            throw; // Re-throw to be caught by main
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Standard exception: " + std::string(e.what()));
            throw;
        }
        
        // Stop performance monitoring
        if (cli.getOptions().profilePerformance) {
            cli.stopPerformanceMonitoring();
        }
        
        // Print performance report
        if (cli.getOptions().profilePerformance) {
            cli.printPerformanceReport();
        }
        
        // Export all results
        if (!cli.getOptions().outputFile.empty()) {
            cli.exportResults();
        }
        
        NLM_LOG_INFO("NLM unified application completed successfully");
    }
    
private:
    NLMCLI cli;
    
    bool initializeErrorHandling() {
        // Initialize global error handling system
        try {
            // This will initialize the global instances
            NLM_LOG_INFO("Enhanced error handling system initialized");
            return true;
        } catch (...) {
            NLM_LOG_ERROR("Failed to initialize error handling system");
            return false;
        }
    }
    
    void initializeLogging() {
        auto logger = std::make_shared<Logger>();
        std::shared_ptr<Logger> consoleLogger;
        
        std::string logLevel = cli.getOptions().logLevel;
        if (logLevel == "debug") {
            consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        } else if (logLevel == "info") {
            consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        } else if (logLevel == "warning") {
            consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        } else if (logLevel == "error") {
            consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        }
        
        if (consoleLogger) {
            logger->addLogger(consoleLogger);
        }
        Logger::setGlobal(logger);
    }
    
    void logStartupInfo() {
        const auto& options = cli.getOptions();
        
        NLM_LOG_INFO("=== NLM Unified Application ===");
        NLM_LOG_INFO("Version: NLM v0.1.0");
        NLM_LOG_INFO("Build: Enhanced unified interface with CLI framework");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:")
        NLM_LOG_INFO("  File: " + options.configFile);
        NLM_LOG_INFO("  Log level: " + options.logLevel);
        NLM_LOG_INFO("  Experiment type: " + std::to_string(options.experimentType));
        NLM_LOG_INFO("  Steps: " + std::to_string(options.numSteps));
        NLM_LOG_INFO("  Timestep: " + std::to_string(options.timestep) + "s");
        NLM_LOG_INFO("  Random seed: " + std::to_string(options.randomSeed));
        NLM_LOG_INFO("  Threads: " + std::to_string(options.threads));
        NLM_LOG_INFO("  Memory limit: " + std::to_string(options.maxMemoryMB) + " MB");
        NLM_LOG_INFO("");
        
        if (options.enableCheckpointing) {
            NLM_LOG_INFO("Features enabled: Checkpointing");
        }
        if (options.enableReplay) {
            NLM_LOG_INFO("Features enabled: Memory replay");
        }
        if (options.enableDevelopment) {
            NLM_LOG_INFO("Features enabled: Development processes");
        }
        if (options.benchmark) {
            NLM_LOG_INFO("Benchmark mode: " + options.benchmarkMode);
        }
        if (options.validateConfig) {
            NLM_LOG_INFO("Configuration validation: Enabled");
        }
        if (options.exportNeuralData) {
            NLM_LOG_INFO("Data export: Neural state data");
        }
        if (options.exportMemoryData) {
            NLM_LOG_INFO("Data export: Memory state data");
        }
        if (options.exportConfiguration) {
            NLM_LOG_INFO("Data export: Configuration data");
        }
    }
    
    std::shared_ptr<Brain> createBrainFromConfig() {
        auto config = std::make_shared<Config>();
        
        // Load configuration from file if specified
        if (!cli.getOptions().configFile.empty() && 
            cli.getOptions().configFile != "configs/default.cfg") {
            
            if (config->loadFromFile(cli.getOptions().configFile)) {
                NLM_LOG_INFO("Loaded configuration from: " + cli.getOptions().configFile);
            } else {
                NLM_LOG_WARNING("Using default configuration (failed to load file)");
            }
        }
        
        // Apply command-line overrides
        if (cli.getOptions().randomSeed != 42) {
            config->set("random_seed", static_cast<int64_t>(cli.getOptions().randomSeed), 
                       ConfigSource::CommandLine);
        }
        
        if (cli.getOptions().timestep != 0.001) {
            config->set("simulation_timestep", cli.getOptions().timestep, 
                       ConfigSource::CommandLine);
        }
        
        // Create brain with enhanced error handling
        auto brain = std::make_shared<Brain>(config);
        
        // Apply brain-specific configuration based on experiment type
        configureBrainForExperiment(brain, config);
        
        return brain;
    }
    
    void configureBrainForExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
        const auto& options = cli.getOptions();
        
        switch (options.experimentType) {
            case 1: // Phase 3
                config->set("neuron_count", 500, ConfigSource::Default);
                config->set("region_count", 2, ConfigSource::Default);
                config->set("connection_probability", 0.1f, ConfigSource::Default);
                break;
                
            case 2: // Phase 4
                config->set("neuron_count", 1000, ConfigSource::Default);
                config->set("region_count", 3, ConfigSource::Default);
                config->set("connection_probability", 0.05f, ConfigSource::Default);
                break;
                
            case 3: // Phase 6
                config->set("neuron_count", 2000, ConfigSource::Default);
                config->set("region_count", 4, ConfigSource::Default);
                config->set("connection_probability", 0.02f, ConfigSource::Default);
                break;
                
            default: // Basic
                config->set("neuron_count", 500, ConfigSource::Default);
                config->set("region_count", 1, ConfigSource::Default);
                config->set("connection_probability", 0.15f, ConfigSource::Default);
                break;
        }
    }
    
    void runBasicSimulation(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running basic simulation...");
        
        // Reset brain to ensure clean state
        brain->reset();
        
        // Run basic simulation steps
        for (int step = 0; step < cli.getOptions().numSteps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            try {
                brain->step(step, step * cli.getOptions().timestep);
                
                auto stepEnd = std::chrono::high_resolution_clock::now();
                auto stepDuration = std::chrono::duration_cast<std::chrono::duration<double>>(stepEnd - stepStart);
                
                cli.recordStepTime(stepDuration.count());
                
                // Print progress periodically
                if (step % 100 == 0) {
                    cli.printProgress();
                    NLM_LOG_INFO("Step " + std::to_string(step) + ": " + 
                        std::to_string(brain->getFiringNeuronCount()) + " firing neurons");
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error at step " + std::to_string(step) + ": " + std::string(e.what()));
                throw NLMError("Simulation failed at step " + std::to_string(step),
                             ErrorCategory::SimulationError,
                             "Check system resources and configuration");
            }
        }
        
        // Print final statistics
        printSimulationResults(brain);
    }
    
    void runPhase3Experiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running Phase 3 Experiment...");
        
        // Create Phase 3 demo
        Phase3Demo phase3Demo;
        
        // Parse experiment arguments from CLI
        int numEpisodes = parseExperimentArgument<int>("episodes", 5);
        int stepsPerEpisode = parseExperimentArgument<int>("steps", 200);
        
        NLM_LOG_INFO("Phase 3 Configuration: Episodes=" + std::to_string(numEpisodes) + 
            ", Steps per episode=" + std::to_string(stepsPerEpisode));
        
        phase3Demo.run(numEpisodes, stepsPerEpisode);
    }
    
    void runPhase4Experiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running Phase 4 Experiment...");
        
        // Create Phase 4 demo
        Phase4Demo phase4Demo;
        
        // Parse experiment arguments from CLI
        int numTrials = parseExperimentArgument<int>("trials", 10);
        
        NLM_LOG_INFO("Phase 4 Configuration: Trials=" + std::to_string(numTrials));
        
        phase4Demo.run(numTrials);
    }
    
    void runPhase6Experiment(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("Running Phase 6 Integration Test...");
        
        // Create Phase 6 demo
        Phase6Demo phase6Demo;
        
        // Phase 6 demo has its own internal configuration
        // but we can influence it through our configuration
        
        NLM_LOG_INFO("Phase 6 Configuration: Integration test with " + 
            std::to_string(cli.getOptions().numSteps) + " steps");
        
        phase6Demo.run();
    }
    
    void printSimulationResults(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Basic Simulation Results ===");
        NLM_LOG_INFO("Total steps: " + std::to_string(cli.getOptions().numSteps));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("Firing neuron count: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Total neuron count: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapse count: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        
        // Calculate some derived metrics
        if (cli.getOptions().numSteps > 0) {
            double stepsPerSecond = cli.getOptions().numSteps / 
                (cli.metrics.totalWallClockTime > 0 ? cli.metrics.totalWallClockTime : 1.0);
            NLM_LOG_INFO("Performance: " + std::to_string(stepsPerSecond) + " steps/second");
        }
    }
    
    template<typename T>
    T parseExperimentArgument(const std::string& argName, T defaultValue) {
        auto it = std::find_if(cli.getOptions().experimentArgs.begin(),
                               cli.getOptions().experimentArgs.end(),
                               [&argName](const std::string& arg) {
                                   return arg.find("--" + argName) == 0;
                               });
        
        if (it != cli.getOptions().experimentArgs.end() && 
            std::next(it) != cli.getOptions().experimentArgs.end()) {
            try {
                std::string value = *(std::next(it));
                if (std::is_same<T, int>::value) {
                    return std::stoi(value);
                } else if (std::is_same<T, double>::value) {
                    return std::stod(value);
                } else if (std::is_same<T, std::string>::value) {
                    return value;
                }
            } catch (...) {
                // Ignore parsing errors and use default
            }
        }
        return defaultValue;
    }
    
    void exportAllResults(std::shared_ptr<Brain> brain) {
        const auto& options = cli.getOptions();
        
        if (options.exportNeuralData) {
            NLM_LOG_INFO("Exporting neural state data...");
            // Implementation for exporting neural state
            cli.exportNeuralState();
        }
        
        if (options.exportMemoryData) {
            NLM_LOG_INFO("Exporting memory state data...");
            // Implementation for exporting memory state
            cli.exportMemoryState();
        }
        
        if (options.exportConfiguration) {
            NLM_LOG_INFO("Exporting configuration data...");
            // Implementation for exporting configuration
            cli.exportConfigurationState();
        }
        
        if (!options.outputFile.empty()) {
            NLM_LOG_INFO("Exporting all results to: " + options.outputFile);
            cli.exportResults();
        }
    }
    
    bool shouldShowVersion(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--version" || arg == "-v") {
                return true;
            }
        }
        return false;
    }
    
    bool shouldShowHelp(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                return true;
            }
        }
        return false;
    }
};

int main(int argc, char* argv[]) {
    try {
        NLMApplication app;
        
        if (!app.initialize(argc, argv)) {
            return 1;
        }
        
        app.run();
        
        return 0;
    } catch (const NLMError& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        if (!e.isRecoverable()) {
            std::cerr << "This error is not recoverable. Please check your configuration and system resources." << std::endl;
        }
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception: " << e.what() << std::endl;
        std::cerr << "This is likely a bug. Please report this issue." << std::endl;
        return 2;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred" << std::endl;
        return 3;
    }
}