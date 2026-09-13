#include "nlm/CLI.hpp"
#include "nlm/Exception.hpp"
#include "nlm/AdvancedLogger.hpp"
#include "nlm/Experiment.hpp"
#include "nlm/ExperimentRunner.hpp"
#include "nlm/ExperimentPresets.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <filesystem>
#include <iomanip>
#include <sstream>

using namespace nlm;

/**
 * Global error handler for NLM applications
 * Provides centralized error handling and recovery
 */
class GlobalErrorHandler {
public:
    enum class ErrorLevel { Warning, Error, Critical };
    
    using ErrorHandler = std::function<void(ErrorLevel, const std::string&, const std::string&)>;
    
    GlobalErrorHandler() {
        std::set_terminate([]() {
            std::cerr << "FATAL ERROR: Program terminated unexpectedly" << std::endl;
            std::exit(1);
        });
        
        std::set_new_handler([]() {
            std::cerr << "FATAL ERROR: Memory allocation failed" << std::endl;
            std::exit(1);
        });
    }
    
    void addHandler(ErrorHandler handler) {
        errorHandlers.push_back(handler);
    }
    
    void reportError(ErrorLevel level, const std::string& message, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::string formattedMessage = formatErrorMessage(level, message, context);
        
        // Log the error
        switch (level) {
            case ErrorLevel::Warning:
                NLM_WARNING("Error: " + formattedMessage);
                break;
            case ErrorLevel::Error:
                NLM_ERROR("Error: " + formattedMessage);
                break;
            case ErrorLevel::Critical:
                NLM_CRITICAL("Error: " + formattedMessage);
                break;
        }
        
        // Call registered handlers
        for (const auto& handler : errorHandlers) {
            try {
                handler(level, formattedMessage, context);
            } catch (const std::exception& e) {
                std::cerr << "Error in error handler: " << e.what() << std::endl;
            }
        }
    }
    
private:
    std::vector<ErrorHandler> errorHandlers;
    std::mutex mutex;
    
    std::string formatErrorMessage(ErrorLevel level, const std::string& message, const std::string& context) {
        std::stringstream ss;
        ss << "[" << getCurrentTimestamp() << "]";
        
        if (!context.empty()) {
            ss << " Context: " << context;
        }
        
        ss << " Message: " << message;
        return ss.str();
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

/**
 * Application initializer
 * Handles all startup initialization and validation
 */
class ApplicationInitializer {
public:
    ApplicationInitializer() {
        initializeLogging();
        initializeErrorHandling();
        initializeConfiguration();
    }
    
    ~ApplicationInitializer() {
        cleanup();
    }
    
    bool isInitialized() const { return initialized; }
    
    const Config& getConfiguration() const { return config; }
    
    const CLI::Options& getOptions() const { return options; }
    
    GlobalErrorHandler& getErrorHandler() { return errorHandler; }
    
    bool initialize(const std::vector<std::string>& args);
    
    void cleanup() {
        NLM_INFO("Cleaning up application resources");
        
        // Cleanup is handled by destructors
        NLM_INFO("Cleanup completed");
    }

private:
    bool initialized = false;
    
    Config config;
    CLI::Options options;
    GlobalErrorHandler errorHandler;
    
    void initializeLogging() {
        // Initialize advanced logging
        globalLogger = AdvancedLogger::create();
        globalLogger->setLevel(AdvancedLogger::Level::Info);
        
        // Add file logger if configured
        std::string logFile = "nlm_phase2.log";
        globalLogger->addFileLogger(logFile);
        
        NLM_INFO("Logging system initialized");
        NLM_INFO("Log file: " + logFile);
    }
    
    void initializeErrorHandling() {
        // Initialize global error handler
        errorHandler.addHandler([](GlobalErrorHandler::ErrorLevel level, 
                                  const std::string& message, 
                                  const std::string& context) {
            // Can add additional error handling here
        });
        
        NLM_INFO("Error handling system initialized");
    }
    
    void initializeConfiguration() {
        NLM_INFO("Initializing configuration system");
        
        // Apply default configuration values
        config.set("random_seed", 42);
        config.set("simulation_timestep", 0.001);
        config.set("neuron_count", 500);
        config.set("region_count", 1);
        config.set("connection_probability", 0.15);
        
        // Set defaults for experiment parameters
        config.set("enable_hebbian", true);
        config.set("enable_stdp", true);
        config.set("enable_reward_modulation", true);
        config.set("enable_development", false);
        config.set("enable_neuromodulation", true);
        config.set("enable_memory", true);
        
        NLM_INFO("Default configuration applied");
    }
    
    bool parseCommandLine(int argc, char** argv);
    bool loadConfigurationFromArgs(const std::vector<std::string>& args);
    bool validateConfiguration() const;
};

// Global application instance
ApplicationInitializer application;

/**
 * Print help information for NLM Phase 2
 */
void printHelp() {
    std::cout << "=========================================================" << std::endl;
    std::cout << "NLM Phase 2 - Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "=========================================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "An experimental artificial developmental brain system implementing" << std::endl;
    std::cout << "real spiking neural computation with LIF neurons, STDP plasticity," << std::endl;
    std::cout << "and integrated cognitive systems." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm_phase2 [OPTIONS]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "BASIC OPTIONS:" << std::endl;
    std::cout << "  -h, --help                    Show this help message" << std::endl;
    std::cout << "  -v, --version                 Show version information" << std::endl;
    std::cout << "  -l, --log-level LEVEL         Set log level (trace, debug, info, warning, error, critical)" << std::endl;
    std::cout << "  -c, --config FILE             Load configuration from file" << std::endl;
    std::cout << "  -o, --output-dir DIR          Set output directory for results" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "EXPERIMENT OPTIONS:" << std::endl;
    std::cout << "  --experiment TYPE             Run specific experiment (connectivity, plasticity, stdp, development, memory, neuromodulation, advanced, all)" << std::endl;
    std::cout << "  --preset PRESET               Use predefined experiment preset (quick_test, comprehensive, etc.)" << std::endl;
    std::cout << "  --list-experiments            List all available experiments" << std::endl;
    std::cout << "  --list-presets                List all available presets" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "EXPERIMENT-SPECIFIC OPTIONS:" << std::endl;
    std::cout << "  --steps N                     Number of simulation steps" << std::endl;
    std::cout << "  --neurons N                   Number of neurons" << std::endl;
    std::cout << "  --regions N                   Number of neural regions" << std::endl;
    std::cout << "  --input-current F             External input current (nA)" << std::endl;
    std::cout << "  --stdp-ltp F                  STDP LTP learning rate" << std::endl;
    std::cout << "  --stdp-ltd F                  STDP LTD learning rate" << std::endl;
    std::cout << "  --enable-learning             Enable plasticity learning" << std::endl;
    std::cout << "  --enable-development          Enable developmental processes" << std::endl;
    std::cout << "  --enable-neuromodulation       Enable neuromodulation systems" << std::endl;
    std::cout << "  --enable-memory              Enable memory systems" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "OUTPUT OPTIONS:" << std::endl;
    std::cout << "  --save-results                Save experiment results to files" << std::endl;
    std::cout << "  --result-format FORMAT        Result file format (json, csv, text)" << std::endl;
    std::cout << "  --generate-report             Generate comprehensive report" << std::endl;
    std::cout << "  --verbose                     Enable verbose output" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm_phase2 --help                    Show help" << std::endl;
    std::cout << "  nlm_phase2 --version                 Show version" << std::endl;
    std::cout << "  nlm_phase2 --list-experiments        List experiments" << std::endl;
    std::cout << "  nlm_phase2 --list-presets            List presets" << std::endl;
    std::cout << "  nlm_phase2 --preset quick_test       Run quick test suite" << std::endl;
    std::cout << "  nlm_phase2 --preset comprehensive     Run comprehensive test suite" << std::endl;
    std::cout << "  nlm_phase2 --experiment connectivity --steps 1000" << std::endl;
    std::cout << "  nlm_phase2 --experiment plasticity --steps 5000 --input-current 50.0" << std::endl;
    std::cout << "  nlm_phase2 --experiment stdp --steps 2000 --stdp-ltp 0.01 --stdp-ltd 0.008" << std::endl;
    std::cout << "  nlm_phase2 --experiment advanced --steps 10000 --enable-development --enable-memory" << std::endl;
    std::cout << "  nlm_phase2 --experiment all --enable-neuromodulation --save-results" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "CONFIGURATION FILE FORMAT:" << std::endl;
    std::cout << "  Configuration can be specified via --config file.json/file.yaml/file.ini" << std::endl;
    std::cout << "  Configuration can also be set via command line arguments" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "AVAILABLE PRESETS:" << std::endl;
    auto presets = ExperimentPresets::getPresetNames();
    for (const auto& preset : presets) {
        std::cout << "  - " << preset << ": " << ExperimentPresets::getPresetDescription(preset) << std::endl;
    }
    std::cout << "" << std::endl;
    std::cout << "=========================================================" << std::endl;
}

/**
 * Get list of available experiments
 */
std::vector<std::string> getAvailableExperiments() {
    auto registry = ExperimentRegistry::getInstance();
    return registry.getExperimentNames();
}

/**
 * Run experiments based on configuration
 */
std::vector<ExperimentRunner::RunResult> runExperiments() {
    ExperimentRunner::GlobalConfig globalConfig;
    globalConfig.programName = "nlm_phase2";
    globalConfig.version = "0.1.0";
    globalConfig.description = "Neural Learning Machine - Advanced Neural Computation Framework";
    
    ExperimentRunner::ExecutionConfig executionConfig;
    
    // Configure based on command line options
    if (application.getOptions().find("--experiment") != application.getOptions().end()) {
        std::string experimentType = application.getOptions()["--experiment"];
        if (experimentType == "all") {
            executionConfig.runAll = true;
            executionConfig.experimentType = "all";
        } else {
            executionConfig.experimentNames = {experimentType};
            executionConfig.experimentType = experimentType;
        }
    }
    
    // Apply preset if specified
    if (application.getOptions().find("--preset") != application.getOptions().end()) {
        std::string preset = application.getOptions()["--preset"];
        ExperimentPresets::applyPreset(executionConfig, preset);
    }
    
    // Set experiment-specific parameters from command line
    if (application.getOptions().find("--steps") != application.getOptions().end()) {
        executionConfig.experimentParams["general"]["steps"] = application.getOptions()["--steps"];
    }
    
    if (application.getOptions().find("--neurons") != application.getOptions().end()) {
        executionConfig.experimentParams["general"]["neuron_count"] = application.getOptions()["--neurons"];
    }
    
    if (application.getOptions().find("--regions") != application.getOptions().end()) {
        executionConfig.experimentParams["general"]["region_count"] = application.getOptions()["--regions"];
    }
    
    if (application.getOptions().find("--input-current") != application.getOptions().end()) {
        executionConfig.experimentParams["general"]["input_current"] = application.getOptions()["--input-current"];
    }
    
    // Set output configuration
    if (application.getOptions().find("--output-dir") != application.getOptions().end()) {
        executionConfig.outputDirectory = application.getOptions()["--output-dir"];
    }
    
    // Configure result handling
    executionConfig.saveResults = application.getOptions().find("--save-results") != application.getOptions().end();
    executionConfig.generateReport = application.getOptions().find("--generate-report") != application.getOptions().end();
    
    if (application.getOptions().find("--result-format") != application.getOptions().end()) {
        executionConfig.resultFormat = application.getOptions()["--result-format"];
    }
    
    // Configure learning flags
    if (application.getOptions().find("--enable-learning") != application.getOptions().end()) {
        application.getConfiguration().set("enable_hebbian", true);
        application.getConfiguration().set("enable_stdp", true);
    }
    
    if (application.getOptions().find("--enable-development") != application.getOptions().end()) {
        application.getConfiguration().set("enable_development", true);
    }
    
    if (application.getOptions().find("--enable-neuromodulation") != application.getOptions().end()) {
        application.getConfiguration().set("enable_neuromodulation", true);
    }
    
    if (application.getOptions().find("--enable-memory") != application.getOptions().end()) {
        application.getConfiguration().set("enable_memory", true);
    }
    
    // Create and run experiment runner
    ExperimentRunner runner(globalConfig);
    runner.setExecutionConfig(executionConfig);
    
    return runner.runExperiments();
}

/**
 * Main application entry point
 */
int main(int argc, char** argv) {
    try {
        NLM_INFO("=== NLM Phase 2 Neural Computation ===");
        NLM_INFO("Initializing application");
        
        // Initialize application
        if (!application.initialize({argv, argv + argc})) {
            NLM_ERROR("Failed to initialize application");
            return 1;
        }
        
        if (!application.isInitialized()) {
            NLM_ERROR("Application not properly initialized");
            return 1;
        }
        
        const auto& config = application.getConfiguration();
        const auto& options = application.getOptions();
        
        // Print application banner
        NLM_INFO("");
        NLM_INFO("NLM (熙然) - Neural Learning Machine v0.1.0");
        NLM_INFO("Phase 2: Real Neural Computation");
        NLM_INFO("");
        
        // Display configuration summary
        NLM_INFO("Configuration Summary:");
        NLM_INFO("  Random seed: " + config.getOr("random_seed", 42));
        NLM_INFO("  Simulation timestep: " + std::to_string(config.getOr("simulation_timestep", 0.001)) + "s");
        NLM_INFO("  Neuron count: " + std::to_string(config.getOr("neuron_count", 500)));
        NLM_INFO("  Region count: " + std::to_string(config.getOr("region_count", 1)));
        NLM_INFO("  Connection probability: " + std::to_string(config.getOr("connection_probability", 0.15f)));
        
        // Check if help is requested
        if (options.find("--help") != options.end() || 
            options.find("-h") != options.end()) {
            printHelp();
            return 0;
        }
        
        // Check if version is requested
        if (options.find("--version") != options.end() || 
            options.find("-v") != options.end()) {
            NLM_INFO("NLM Phase 2 v0.1.0");
            NLM_INFO("Neural Learning Machine - Phase 2 Real Neural Computation");
            NLM_INFO("An experimental artificial developmental brain system");
            return 0;
        }
        
        // Display available experiments
        NLM_INFO("");
        NLM_INFO("Available experiments:");
        auto experiments = getAvailableExperiments();
        for (const auto& exp : experiments) {
            NLM_INFO("  - " + exp);
        }
        
        // Validate configuration
        NLM_INFO("");
        NLM_INFO("Validating configuration...");
        if (!application.validateConfiguration()) {
            NLM_ERROR("Configuration validation failed");
            return 1;
        }
        
        NLM_INFO("Configuration validated successfully");
        
        // Run experiments
        NLM_INFO("");
        NLM_INFO("=== Running Experiments ===");
        
        auto results = runExperiments();
        
        // Generate and display summary
        NLM_INFO("");
        NLM_INFO("=== Experiment Summary ===");
        
        int successful = 0;
        int failed = 0;
        float totalExecutionTime = 0.0f;
        
        for (const auto& result : results) {
            if (result.success) {
                successful++;
                totalExecutionTime += result.executionTime;
                NLM_INFO("[SUCCESS] " + result.experimentName + ": " + 
                         std::to_string(result.executionTime) + "s, " +
                         std::to_string(result.averageFiringRate) + " avg firing");
            } else {
                failed++;
                NLM_ERROR("[FAILED] " + result.experimentName + ": " + result.errorMessage);
            }
        }
        
        NLM_INFO("");
        NLM_INFO("=== Overall Results ===");
        NLM_INFO("Total experiments: " + std::to_string(results.size()));
        NLM_INFO("Successful: " + std::to_string(successful));
        NLM_INFO("Failed: " + std::to_string(failed));
        NLM_INFO("Success rate: " + std::to_string(successful * 100.0f / std::max(1, results.size())) + "%");
        
        if (totalExecutionTime > 0.0f) {
            NLM_INFO("Average execution time: " + std::to_string(totalExecutionTime / std::max(1, results.size())) + "s");
        }
        
        NLM_INFO("");
        NLM_INFO("=== Application Complete ===");
        NLM_INFO("Neural computation experiments completed successfully");
        
        return failed > 0 ? 1 : 0;
        
    } catch (const NLMException& e) {
        NLM_ERROR("NLM Error: " + std::string(e.what()));
        NLM_ERROR("Error code: " + NLMException::errorCodeToString(e.getErrorCode()));
        NLM_ERROR("Context: " + e.getContext());
        NLM_ERROR("Suggestion: " + e.getSuggestion());
        return 1;
    } catch (const std::exception& e) {
        NLM_ERROR("Unexpected error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_ERROR("Unknown error occurred");
        return 1;
    }
}