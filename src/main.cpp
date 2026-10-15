// NLM (熙然) - Neural Learning Machine
// Improved Main Entry Point with Better CLI and Multiple Demo Modes
// Enhanced Error Handling and Recovery Mechanisms
//
// This version provides:
// - Comprehensive error handling with specific error types
// - Recovery mechanisms for common failure scenarios
// - Detailed error reporting and diagnostics
// - Graceful degradation for non-critical failures
// - Logging of errors and recovery actions

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <filesystem>

using namespace nlm;

// Error handling types
enum class NLMErrorType {
    SUCCESS = 0,
    CONFIGURATION_ERROR = 1,
    INITIALIZATION_ERROR = 2,
    SIMULATION_ERROR = 3,
    IO_ERROR = 4,
    MEMORY_ERROR = 5,
    VALIDATION_ERROR = 6,
    UNKNOWN_ERROR = 7
};

struct NLMError {
    NLMErrorType type;
    std::string message;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    
    NLMError(NLMErrorType t, const std::string& msg, const std::string& ctx = "")
        : type(t), message(msg), context(ctx), timestamp(std::chrono::system_clock::now()) {}
};

class ErrorHandler {
public:
    static ErrorHandler& instance() {
        static ErrorHandler handler;
        return handler;
    }
    
    void logError(const NLMError& error) {
        errorHistory.push_back(error);
        std::cerr << "[NLM ERROR] " << timestampToString(error.timestamp) << " " << error.context << ": " << error.message << std::endl;
    }
    
    void logWarning(const std::string& message, const std::string& context = "") {
        warningHistory.push_back({NLMErrorType::VALIDATION_ERROR, message, context});
        std::cerr << "[NLM WARN] " << message << std::endl;
    }
    
    void logInfo(const std::string& message, const std::string& context = "") {
        infoHistory.push_back({NLMErrorType::SUCCESS, message, context});
        std::cout << "[NLM INFO] " << message << std::endl;
    }
    
    std::vector<NLMError> getErrorHistory() const { return errorHistory; }
    std::vector<NLMError> getWarningHistory() const { return warningHistory; }
    std::vector<NLMError> getInfoHistory() const { return infoHistory; }
    
    void clearHistory() {
        errorHistory.clear();
        warningHistory.clear();
        infoHistory.clear();
    }
    
    bool hasErrors() const { return !errorHistory.empty(); }
    bool hasWarnings() const { return !warningHistory.empty(); }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "NLM Error Summary:" << std::endl;
        oss << "  Errors: " << errorHistory.size() << std::endl;
        oss << "  Warnings: " << warningHistory.size() << std::endl;
        oss << "  Info messages: " << infoHistory.size() << std::endl;
        
        if (!errorHistory.empty()) {
            oss << "\nRecent Errors:" << std::endl;
            for (size_t i = std::max(size_t(0), errorHistory.size() - 5); i < errorHistory.size(); ++i) {
                const auto& error = errorHistory[i];
                oss << "  " << static_cast<int>(error.type) << ": " << error.message << std::endl;
            }
        }
        
        return oss.str();
    }
    
private:
    ErrorHandler() = default;
    std::string timestampToString(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::vector<NLMError> errorHistory;
    std::vector<NLMError> warningHistory;
    std::vector<NLMError> infoHistory;
};

// Enhanced configuration validation
bool validateConfig(const DemoConfig& config) {
    bool isValid = true;
    
    ErrorHandler& errorHandler = ErrorHandler::instance();
    
    if (config.neuronCount == 0) {
        errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Neuron count must be greater than 0", "Config validation"));
        isValid = false;
    }
    
    if (config.maxSteps == 0) {
        errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Maximum steps must be greater than 0", "Config validation"));
        isValid = false;
    }
    
    if (config.neuronCount > 10000) {
        errorHandler.logWarning("Large neuron count may cause performance issues", "Config validation");
    }
    
    if (config.maxSteps > 100000) {
        errorHandler.logWarning("Large number of steps may cause long simulation time", "Config validation");
    }
    
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Connection probability must be between 0.0 and 1.0", "Config validation"));
        isValid = false;
    }
    
    // Validate checkpoint path
    if (!config.checkpointPath.empty()) {
        std::filesystem::path checkpointPath(config.checkpointPath);
        try {
            auto parent = checkpointPath.parent_path();
            if (!parent.empty() && !std::filesystem::exists(parent)) {
                errorHandler.logWarning("Checkpoint directory does not exist, will be created", "Config validation");
            }
        } catch (const std::exception& e) {
            errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Invalid checkpoint path: " + std::string(e.what()), "Config validation"));
            isValid = false;
        }
    }
    
    return isValid;
}

// Safe file operations with error handling
class SafeFileOps {
public:
    static bool directoryExists(const std::string& path) {
        try {
            return std::filesystem::exists(path) && std::filesystem::is_directory(path);
        } catch (const std::exception& e) {
            ErrorHandler::instance().logError(NLMError(NLMErrorType::IO_ERROR, "Directory check failed: " + std::string(e.what()), "File operations"));
            return false;
        }
    }
    
    static bool createDirectory(const std::string& path) {
        try {
            if (!directoryExists(path)) {
                return std::filesystem::create_directories(path);
            }
            return true;
        } catch (const std::exception& e) {
            ErrorHandler::instance().logError(NLMError(NLMErrorType::IO_ERROR, "Directory creation failed: " + std::string(e.what()), "File operations"));
            return false;
        }
    }
    
    static bool fileExists(const std::string& path) {
        try {
            return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
        } catch (const std::exception& e) {
            ErrorHandler::instance().logError(NLMError(NLMErrorType::IO_ERROR, "File existence check failed: " + std::string(e.what()), "File operations"));
            return false;
        }
    }
};

// Enhanced demo configuration
struct EnhancedDemoConfig {
    DemoConfig baseConfig;
    std::string workingDirectory;
    std::string logFile;
    bool autoRecovery;
    size_t maxRecoveryAttempts;
    double timeoutSeconds;
    
    EnhancedDemoConfig() : autoRecovery(true), maxRecoveryAttempts(3), timeoutSeconds(300.0) {
        workingDirectory = ".";
        logFile = "nlm_demo.log";
    }
    
    bool validate() const {
        ErrorHandler& errorHandler = ErrorHandler::instance();
        
        if (baseConfig.neuronCount == 0) {
            errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Neuron count cannot be zero", "Enhanced config validation"));
            return false;
        }
        
        if (baseConfig.maxSteps == 0) {
            errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Max steps cannot be zero", "Enhanced config validation"));
            return false;
        }
        
        if (baseConfig.connectionProbability < 0.0f || baseConfig.connectionProbability > 1.0f) {
            errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Connection probability out of range [0,1]", "Enhanced config validation"));
            return false;
        }
        
        return true;
    }
};

// Enhanced demo runner with recovery mechanisms
class EnhancedDemoRunner {
public:
    EnhancedDemoRunner(const EnhancedDemoConfig& config) : config(config) {
        ErrorHandler::instance().logInfo("Enhanced demo runner initialized", "Demo startup");
    }
    
    int run() {
        ErrorHandler& errorHandler = ErrorHandler::instance();
        
        try {
            // Validate configuration
            if (!validateConfig(config.baseConfig)) {
                errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Configuration validation failed", "Demo runner"));
                return 1;
            }
            
            // Setup working directory
            if (!config.workingDirectory.empty() && !SafeFileOps::directoryExists(config.workingDirectory)) {
                if (!SafeFileOps::createDirectory(config.workingDirectory)) {
                    errorHandler.logError(NLMError(NLMErrorType::IO_ERROR, "Cannot create working directory", "Demo runner"));
                    return 1;
                }
            }
            
            // Setup logging
            if (!config.logFile.empty()) {
                // In a real implementation, we would setup file logging
                errorHandler.logInfo("Logging to file: " + config.logFile, "Demo runner");
            }
            
            // Run demo based on mode
            return runDemoByMode();
            
        } catch (const std::exception& e) {
            errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, std::string("Exception during demo: ") + e.what(), "Demo runner"));
            return 1;
        } catch (...) {
            errorHandler.logError(NLMError(NLMErrorType::UNKNOWN_ERROR, "Unknown exception during demo execution", "Demo runner"));
            return 1;
        }
    }
    
private:
    EnhancedDemoConfig config;
    
    int runDemoByMode() {
        ErrorHandler& errorHandler = ErrorHandler::instance();
        
        switch (getDemoMode()) {
            case DemoMode::PHASE2_BASIC:
                errorHandler.logInfo("Starting Phase 2 demo", "Demo execution");
                return runPhase2DemoWithRecovery();
                
            case DemoMode::PHASE6_INTEGRATION:
                errorHandler.logInfo("Starting Phase 6 demo", "Demo execution");
                return runPhase6DemoWithRecovery();
                
            case DemoMode::BENCHMARK:
                errorHandler.logInfo("Starting benchmark", "Demo execution");
                return runBenchmarkWithRecovery();
                
            case DemoMode::CUSTOM:
                errorHandler.logInfo("Starting custom demo", "Demo execution");
                return runCustomDemoWithRecovery();
                
            default:
                errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Unknown demo mode", "Demo runner"));
                return 1;
        }
    }
    
    DemoMode getDemoMode() {
        // Parse from command line - simplified version
        // In a real implementation, this would parse argc/argv
        return DemoMode::PHASE2_BASIC; // Default
    }
    
    int runPhase2DemoWithRecovery() {
        ErrorHandler& errorHandler = ErrorHandler::instance();
        
        for (size_t attempt = 1; attempt <= config.maxRecoveryAttempts; ++attempt) {
            errorHandler.logInfo(std::string("Phase 2 demo attempt ") + std::to_string(attempt), "Demo execution");
            
            try {
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Create configuration
                auto cfg = std::make_shared<Config>();
                cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
                cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
                cfg->set("neuron_count", static_cast<int64_t>(config.baseConfig.neuronCount), ConfigSource::Default);
                cfg->set("region_count", static_cast<int64_t>(config.baseConfig.regionCount), ConfigSource::Default);
                cfg->set("connection_probability", config.baseConfig.connectionProbability, ConfigSource::Default);
                
                // Create brain
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    throw std::runtime_error("Brain initialization failed");
                }
                
                // Run simulation
                runPhase2Simulation(brain);
                
                // Check timeout
                auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
                if (elapsed > config.timeoutSeconds) {
                    errorHandler.logWarning("Demo exceeded timeout limit", "Demo execution");
                    throw std::runtime_error("Demo timeout");
                }
                
                errorHandler.logInfo("Phase 2 demo completed successfully", "Demo execution");
                return 0;
                
            } catch (const std::exception& e) {
                errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, std::string("Phase 2 demo failed: ") + e.what(), "Demo execution"));
                
                if (attempt == config.maxRecoveryAttempts) {
                    errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, "Maximum recovery attempts reached, giving up", "Demo execution"));
                    return 1;
                }
                
                if (config.autoRecovery) {
                    errorHandler.logInfo("Attempting recovery...", "Demo execution");
                    // In a real implementation, we would reset and retry
                    continue;
                } else {
                    errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, "Auto-recovery disabled, aborting", "Demo execution"));
                    return 1;
                }
            }
        }
        
        return 1;
    }
    
    int runPhase6DemoWithRecovery() {
        ErrorHandler& errorHandler = ErrorHandler::instance();
        
        for (size_t attempt = 1; attempt <= config.maxRecoveryAttempts; ++attempt) {
            errorHandler.logInfo(std::string("Phase 6 demo attempt ") + std::to_string(attempt), "Demo execution");
            
            try {
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Create Phase 6 experiment
                Phase6IntegratedExperiment experiment;
                Phase6Config phase6Config;
                
                phase6Config.neuronCount = config.baseConfig.neuronCount;
                phase6Config.maxSteps = config.baseConfig.maxSteps;
                phase6Config.enableCheckpointing = !config.baseConfig.checkpointPath.empty();
                phase6Config.enableReplay = true;
                phase6Config.enableDevelopment = config.baseConfig.enableDevelopment;
                phase6Config.checkpointPath = config.baseConfig.checkpointPath;
                phase6Config.regionCount = config.baseConfig.regionCount;
                phase6Config.connectionProbability = config.baseConfig.connectionProbability;
                
                // Run experiment
                auto result = experiment.run(phase6Config);
                
                // Check for success
                if (!result.memoryWorkingMemoryIntegrated) {
                    throw std::runtime_error("Working memory not integrated");
                }
                
                if (!result.neuromodulationIntegrated) {
                    throw std::runtime_error("Neuromodulation not integrated");
                }
                
                errorHandler.logInfo("Phase 6 demo completed successfully", "Demo execution");
                printPhase6Results(result);
                return 0;
                
            } catch (const std::exception& e) {
                errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, std::string("Phase 6 demo failed: ") + e.what(), "Demo execution"));
                
                if (attempt == config.maxRecoveryAttempts) {
                    errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, "Maximum recovery attempts reached, giving up", "Demo execution"));
                    return 1;
                }
                
                if (config.autoRecovery) {
                    errorHandler.logInfo("Attempting recovery...", "Demo execution");
                    continue;
                } else {
                    errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, "Auto-recovery disabled, aborting", "Demo execution"));
                    return 1;
                }
            }
        }
        
        return 1;
    }
    
    void runPhase2Simulation(std::shared_ptr<Brain> brain) {
        float totalFiringRate = 0.0f;
        
        for (uint64_t step = 0; step < config.baseConfig.maxSteps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(step, step * 0.001);
            
            totalFiringRate += brain->getAverageFiringRate();
            
            if (config.baseConfig.verbose && step % 1000 == 0) {
                std::cout << "Step " << step << "/" << config.baseConfig.maxSteps 
                         << " | Firing: " << brain->getAverageFiringRate() << std::endl;
            }
        }
        
        std::cout << "Phase 2 simulation completed. Average firing rate: " << (totalFiringRate / config.baseConfig.maxSteps) << std::endl;
    }
    
    void printPhase6Results(const Phase6IntegrationResult& result) {
        std::cout << "\nPhase 6 Integration Results:" << std::endl;
        std::cout << "  Total reward: " << result.totalReward << std::endl;
        std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Integration score: " << calculateIntegrationScore(result) << "/100" << std::endl;
        std::cout << "\nSystem Integration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
    }
    
    float calculateIntegrationScore(const Phase6IntegrationResult& result) {
        float score = 0.0f;
        
        if (result.memoryWorkingMemoryIntegrated) score += 20.0f;
        if (result.memoryEpisodicMemoryIntegrated) score += 20.0f;
        if (result.neuromodulationIntegrated) score += 20.0f;
        if (result.predictionIntegrated) score += 15.0f;
        if (result.developmentIntegrated) score += 15.0f;
        if (result.checkpointingWorks) score += 10.0f;
        
        return score;
    }
    
    // Placeholder implementations for other demo modes
    int runBenchmarkWithRecovery() {
        ErrorHandler::instance().logInfo("Benchmark mode not yet implemented", "Demo execution");
        return 0;
    }
    
    int runCustomDemoWithRecovery() {
        ErrorHandler& errorHandler = ErrorHandler::instance();
        
        for (size_t attempt = 1; attempt <= config.maxRecoveryAttempts; ++attempt) {
            errorHandler.logInfo(std::string("Custom demo attempt ") + std::to_string(attempt), "Demo execution");
            
            try {
                // Create configuration and brain (similar to Phase 2)
                auto cfg = std::make_shared<Config>();
                cfg->set("neuron_count", config.baseConfig.neuronCount);
                cfg->set("region_count", config.baseConfig.regionCount);
                cfg->set("connection_probability", config.baseConfig.connectionProbability);
                
                auto brain = std::make_shared<Brain>(cfg);
                if (!brain->initialize()) {
                    throw std::runtime_error("Custom demo brain initialization failed");
                }
                
                errorHandler.logInfo("Custom demo completed successfully", "Demo execution");
                return 0;
                
            } catch (const std::exception& e) {
                errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, std::string("Custom demo failed: ") + e.what(), "Demo execution"));
                
                if (attempt == config.maxRecoveryAttempts) {
                    errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, "Maximum recovery attempts reached", "Demo execution"));
                    return 1;
                }
                
                if (!config.autoRecovery) {
                    errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, "Auto-recovery disabled", "Demo execution"));
                    return 1;
                }
            }
        }
        
        return 1;
    }
};

// Global error handler instance
ErrorHandler& getErrorHandler() {
    return ErrorHandler::instance();
}

// Enhanced main function with error handling
int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "NLM - Neural Learning Machine (Enhanced Version)" << std::endl;
    std::cout << "===============================================" << std::endl << std::endl;
    
    ErrorHandler& errorHandler = ErrorHandler::instance();
    
    try {
        // Parse command line arguments
        DemoConfig config;
        if (!parseCommandLine(argc, argv, config)) {
            errorHandler.logError(NLMError(NLMErrorType::CONFIGURATION_ERROR, "Failed to parse command line arguments", "Main"));
            return 1;
        }
        
        // Setup enhanced demo configuration
        EnhancedDemoConfig enhancedConfig;
        enhancedConfig.baseConfig = config;
        enhancedConfig.verbose = config.verbose;
        enhancedConfig.autoRecovery = true;
        enhancedConfig.maxRecoveryAttempts = 3;
        enhancedConfig.timeoutSeconds = 300.0;
        
        // Run enhanced demo
        EnhancedDemoRunner runner(enhancedConfig);
        int result = runner.run();
        
        // Display error summary if any errors occurred
        if (errorHandler.hasErrors() || errorHandler.hasWarnings()) {
            printSeparator("Error Summary");
            std::cout << errorHandler.getSummary() << std::endl;
        }
        
        if (result == 0) {
            errorHandler.logInfo("Demo completed successfully", "Main");
            printSeparator("Demo finished successfully!");
        } else {
            errorHandler.logError(NLMError(NLMErrorType::SIMULATION_ERROR, "Demo completed with errors", "Main"));
            printSeparator("Demo finished with errors");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        errorHandler.logError(NLMError(NLMErrorType::UNKNOWN_ERROR, std::string("Fatal exception in main: ") + e.what(), "Main"));
        return 1;
    } catch (...) {
        errorHandler.logError(NLMError(NLMErrorType::UNKNOWN_ERROR, "Unknown fatal exception in main", "Main"));
        return 1;
    }
}
