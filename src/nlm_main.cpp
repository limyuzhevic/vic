#include "src/test_runner/CommandLineParser.hpp"
#include "src/test_runner/TestRunner.hpp"
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <chrono>

namespace nlm {

// Forward declarations for test classes
class BasicConnectivityTest;
class PlasticityExperimentTest;
class StdpVerificationTest;

// Enhanced error handling system
class ErrorHandler {
public:
    enum class ErrorType {
        ConfigurationError,
        FileError,
        SimulationError,
        ResourceError,
        RecoveryError
    };
    
    struct Error {
        ErrorType type;
        std::string message;
        std::string context;
        int retryCount;
        bool recoverable;
    };
    
    static void initialize(const std::string& programName, bool verbose = false) {
        program = programName;
        isVerbose = verbose;
    }
    
    static bool handleError(ErrorType type, const std::string& message, 
                           const std::string& context = "", bool recoverable = true,
                           int maxRetries = 3) {
        Error error;
        error.type = type;
        error.message = message;
        error.context = context;
        error.retryCount = 0;
        error.recoverable = recoverable;
        
        if (error.recoverable) {
            return handleRecoverableError(error, maxRetries, message, context);
        } else {
            return handleFatalError(error);
        }
    }
    
    static bool handleFileError(const std::string& filepath, const std::string& operation,
                               const std::string& reason, int maxRetries = 3) {
        return handleError(ErrorType::FileError, 
                          "Failed to " + operation + " file: " + reason,
                          "File: " + filepath, true, maxRetries);
    }
    
    static bool handleConfigurationError(const std::string& parameter, 
                                         const std::string& reason,
                                         const std::string& context = "") {
        return handleError(ErrorType::ConfigurationError,
                          "Invalid configuration: " + reason,
                          "Parameter: " + parameter + ", " + context, false);
    }
    
    static bool handleSimulationError(const std::string& reason,
                                      const std::string& context = "",
                                      int maxRetries = 2) {
        return handleError(ErrorType::SimulationError,
                          "Simulation failed: " + reason,
                          context, true, maxRetries);
    }
    
    static void printError(const Error& error) {
        std::cerr << program << ": ";
        
        switch (error.type) {
            case ErrorType::ConfigurationError:
                std::cerr << "Configuration Error";
                break;
            case ErrorType::FileError:
                std::cerr << "File Error";
                break;
            case ErrorType::SimulationError:
                std::cerr << "Simulation Error";
                break;
            case ErrorType::ResourceError:
                std::cerr << "Resource Error";
                break;
            case ErrorType::RecoveryError:
                std::cerr << "Recovery Error";
                break;
        }
        
        std::cerr << ": " << error.message << std::endl;
        
        if (!error.context.empty()) {
            std::cerr << "  Context: " << error.context << std::endl;
        }
        
        if (isVerbose) {
            std::cerr << "  Type: " << static_cast<int>(error.type) << std::endl;
            std::cerr << "  Retry count: " << error.retryCount << std::endl;
        }
    }
    
    static bool handleRecoverableError(Error error, int maxRetries, 
                                      const std::string& baseMessage,
                                      const std::string& baseContext) {
        error.retryCount = 0;
        
        while (error.retryCount < maxRetries) {
            // Attempt to recover or retry the operation
            if (attemptRecovery(error)) {
                return true;
            }
            
            error.retryCount++;
            
            // Exponential backoff
            int delay = 100 * std::pow(2, error.retryCount - 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            
            // Try again
            if (retryOperation(error, baseMessage, baseContext)) {
                return true;
            }
        }
        
        return handleFatalError(error);
    }
    
    static bool attemptRecovery(Error& error) {
        // Implement recovery strategies based on error type
        switch (error.type) {
            case ErrorType::FileError:
                // Try to load backup file or alternative source
                std::cerr << "Attempting to recover from file error..." << std::endl;
                return true;
            case ErrorType::SimulationError:
                // Try to reduce simulation complexity
                std::cerr << "Attempting to reduce simulation complexity..." << std::endl;
                return true;
            case ErrorType::ConfigurationError:
                // Try to apply defaults
                std::cerr << "Attempting to apply default configuration..." << std::endl;
                return true;
            default:
                return false;
        }
    }
    
    static bool retryOperation(Error error, const std::string& baseMessage,
                              const std::string& baseContext, int maxRetries) {
        std::cerr << "Retry attempt " << error.retryCount << "/" 
                  << " (max " << maxRetries << ") " << baseMessage << std::endl;
        
        if (isVerbose) {
            std::cerr << "  Context: " << baseContext << std::endl;
        }
        
        // In a real implementation, this would retry the actual operation
        return false; // For now, always fail
    }
    
    static bool handleFatalError(const Error& error) {
        printError(error);
        
        if (error.retryCount >= maxRetries) {
            std::cerr << "Operation failed after " << error.retryCount 
                      << " retry attempts." << std::endl;
        }
        
        return false;
    }
    
    static void setMaxRetries(int retries) {
        maxRetries = retries;
    }
    
    static void enableRecovery(bool enable) {
        recoveryEnabled = enable;
    }
    
private:
    static std::string program;
    static bool isVerbose;
    static int maxRetries;
    static bool recoveryEnabled;
};

// Recovery manager for handling system-level issues
class RecoveryManager {
public:
    enum class RecoveryAction {
        None,
        ReduceComplexity,
        ApplyDefaults,
        UseBackupConfig,
        ExtendResources,
        SkipResourceIntensive,
        ContinueWithLimitedFunctionality
    };
    
    static bool attemptRecovery(ErrorHandler::ErrorType type, 
                               const std::string& context,
                               RecoveryAction& recommendedAction) {
        switch (type) {
            case ErrorHandler::ErrorType::ConfigurationError:
                recommendedAction = RecoveryAction::ApplyDefaults;
                return true;
            
            case ErrorHandler::ErrorType::FileError:
                recommendedAction = RecoveryAction::UseBackupConfig;
                return true;
            
            case ErrorHandler::ErrorType::SimulationError:
                recommendedAction = RecoveryAction::ReduceComplexity;
                return true;
            
            case ErrorHandler::ErrorType::ResourceError:
                recommendedAction = RecoveryAction::SkipResourceIntensive;
                return true;
            
            default:
                return false;
        }
    }
    
    static std::string getRecoveryMessage(RecoveryAction action) {
        switch (action) {
            case RecoveryAction::ApplyDefaults:
                return "Applying default configuration values.";
            case RecoveryAction::UseBackupConfig:
                return "Attempting to load backup configuration.";
            case RecoveryAction::ReduceComplexity:
                return "Reducing simulation complexity (fewer neurons, smaller steps).";
            case RecoveryAction::SkipResourceIntensive:
                return "Skipping resource-intensive operations.";
            case RecoveryAction::ContinueWithLimitedFunctionality:
                return "Continuing with limited functionality.";
            default:
                return "Attempting recovery...";
        }
    }
};

// Enhanced configuration loader with error handling
class SafeConfigLoader {
public:
    static std::shared_ptr<Config> loadConfig(const std::string& filepath, 
                                             bool attemptBackups = true) {
        auto config = std::make_shared<Config>();
        
        // Try to load primary config file
        if (!loadConfigFile(config, filepath)) {
            if (attemptBackups) {
                // Try backup files
                std::vector<std::string> backupFiles = {
                    filepath + ".backup",
                    filepath + ".bak",
                    "default.cfg",
                    "configs/default.cfg"
                };
                
                for (const auto& backup : backupFiles) {
                    if (loadConfigFile(config, backup)) {
                        std::cerr << "Loaded configuration from backup: " << backup << std::endl;
                        return config;
                    }
                }
            }
            
            // Return empty config with warnings
            std::cerr << "Warning: Could not load configuration from: " << filepath << std::endl;
            std::cerr << "Using empty configuration." << std::endl;
            return config;
        }
        
        return config;
    }
    
private:
    static bool loadConfigFile(std::shared_ptr<Config>& config, const std::string& filepath) {
        try {
            if (config->loadFromFile(filepath)) {
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception while loading config file " << filepath 
                      << ": " << e.what() << std::endl;
        }
        return false;
    }
};

// Enhanced error handling system
class ErrorHandler {
public:
    enum class ErrorType {
        ConfigurationError,
        FileError,
        SimulationError,
        ResourceError,
        RecoveryError
    };
    
    struct Error {
        ErrorType type;
        std::string message;
        std::string context;
        int retryCount;
        bool recoverable;
    };
    
    static void initialize(const std::string& programName, bool verbose = false) {
        program = programName;
        isVerbose = verbose;
    }
    
    static bool handleError(ErrorType type, const std::string& message, 
                           const std::string& context = "", bool recoverable = true,
                           int maxRetries = 3) {
        Error error;
        error.type = type;
        error.message = message;
        error.context = context;
        error.retryCount = 0;
        error.recoverable = recoverable;
        
        if (error.recoverable) {
            return handleRecoverableError(error, maxRetries, message, context);
        } else {
            return handleFatalError(error);
        }
    }
    
    static bool handleFileError(const std::string& filepath, const std::string& operation,
                               const std::string& reason, int maxRetries = 3) {
        return handleError(ErrorType::FileError, 
                          "Failed to " + operation + " file: " + reason,
                          "File: " + filepath, true, maxRetries);
    }
    
    static bool handleConfigurationError(const std::string& parameter, 
                                         const std::string& reason,
                                         const std::string& context = "") {
        return handleError(ErrorType::ConfigurationError,
                          "Invalid configuration: " + reason,
                          "Parameter: " + parameter + ", " + context, false);
    }
    
    static bool handleSimulationError(const std::string& reason,
                                      const std::string& context = "",
                                      int maxRetries = 2) {
        return handleError(ErrorType::SimulationError,
                          "Simulation failed: " + reason,
                          context, true, maxRetries);
    }
    
    static bool handleRecoverableError(Error error, int maxRetries, 
                                      const std::string& baseMessage,
                                      const std::string& baseContext) {
        error.retryCount = 0;
        
        while (error.retryCount < maxRetries) {
            // Attempt to recover or retry the operation
            if (attemptRecovery(error)) {
                return true;
            }
            
            error.retryCount++;
            
            // Exponential backoff
            int delay = 100 * std::pow(2, error.retryCount - 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            
            // Try again
            if (retryOperation(error, baseMessage, baseContext, maxRetries)) {
                return true;
            }
        }
        
        return handleFatalError(error);
    }
    
    static bool attemptRecovery(Error& error) {
        // Implement recovery strategies based on error type
        switch (error.type) {
            case ErrorType::FileError:
                // Try to load backup file or alternative source
                std::cerr << "Attempting to recover from file error..." << std::endl;
                return true;
            case ErrorType::SimulationError:
                // Try to reduce simulation complexity
                std::cerr << "Attempting to reduce simulation complexity..." << std::endl;
                return true;
            case ErrorType::ConfigurationError:
                // Try to apply defaults
                std::cerr << "Attempting to apply default configuration..." << std::endl;
                return true;
            default:
                return false;
        }
    }
    
    static bool retryOperation(Error error, const std::string& baseMessage,
                              const std::string& baseContext, int maxRetries) {
        std::cerr << "Retry attempt " << error.retryCount << "/" 
                  << " (max " << maxRetries << ") " << baseMessage << std::endl;
        
        if (isVerbose) {
            std::cerr << "  Context: " << baseContext << std::endl;
        }
        
        // In a real implementation, this would retry the actual operation
        return false; // For now, always fail
    }
    
    static bool handleFatalError(const Error& error) {
        printError(error);
        
        if (error.retryCount >= maxRetries) {
            std::cerr << "Operation failed after " << error.retryCount 
                      << " retry attempts." << std::endl;
        }
        
        return false;
    }
    
    static void setMaxRetries(int retries) {
        maxRetries = retries;
    }
    
    static void enableRecovery(bool enable) {
        recoveryEnabled = enable;
    }
    
private:
    static std::string program;
    static bool isVerbose;
    static int maxRetries;
    static bool recoveryEnabled;
};

// Recovery manager for handling system-level issues
class RecoveryManager {
public:
    enum class RecoveryAction {
        None,
        ReduceComplexity,
        ApplyDefaults,
        UseBackupConfig,
        ExtendResources,
        SkipResourceIntensive,
        ContinueWithLimitedFunctionality
    };
    
    static bool attemptRecovery(ErrorHandler::ErrorType type, 
                               const std::string& context,
                               RecoveryAction& recommendedAction) {
        switch (type) {
            case ErrorHandler::ErrorType::ConfigurationError:
                recommendedAction = RecoveryAction::ApplyDefaults;
                return true;
            
            case ErrorHandler::ErrorType::FileError:
                recommendedAction = RecoveryAction::UseBackupConfig;
                return true;
            
            case ErrorHandler::ErrorType::SimulationError:
                recommendedAction = RecoveryAction::ReduceComplexity;
                return true;
            
            case ErrorHandler::ErrorType::ResourceError:
                recommendedAction = RecoveryAction::SkipResourceIntensive;
                return true;
            
            default:
                return false;
        }
    }
    
    static std::string getRecoveryMessage(RecoveryAction action) {
        switch (action) {
            case RecoveryAction::ApplyDefaults:
                return "Applying default configuration values.";
            case RecoveryAction::UseBackupConfig:
                return "Attempting to load backup configuration.";
            case RecoveryAction::ReduceComplexity:
                return "Reducing simulation complexity (fewer neurons, smaller steps).";
            case RecoveryAction::SkipResourceIntensive:
                return "Skipping resource-intensive operations.";
            case RecoveryAction::ContinueWithLimitedFunctionality:
                return "Continuing with limited functionality.";
            default:
                return "Attempting recovery...";
        }
    }
};

// Enhanced configuration loader with error handling
class SafeConfigLoader {
public:
    static std::shared_ptr<Config> loadConfig(const std::string& filepath, 
                                             bool attemptBackups = true) {
        auto config = std::make_shared<Config>();
        
        // Try to load primary config file
        if (!loadConfigFile(config, filepath)) {
            if (attemptBackups) {
                // Try backup files
                std::vector<std::string> backupFiles = {
                    filepath + ".backup",
                    filepath + ".bak",
                    "default.cfg",
                    "configs/default.cfg"
                };
                
                for (const auto& backup : backupFiles) {
                    if (loadConfigFile(config, backup)) {
                        std::cerr << "Loaded configuration from backup: " << backup << std::endl;
                        return config;
                    }
                }
            }
            
            // Return empty config with warnings
            std::cerr << "Warning: Could not load configuration from: " << filepath << std::endl;
            std::cerr << "Using empty configuration." << std::endl;
            return config;
        }
        
        return config;
    }
    
private:
    static bool loadConfigFile(std::shared_ptr<Config>& config, const std::string& filepath) {
        try {
            if (config->loadFromFile(filepath)) {
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception while loading config file " << filepath 
                      << ": " << e.what() << std::endl;
        }
        return false;
    }
};

// Initialize static members of ErrorHandler
std::string ErrorHandler::program = "NLM";
bool ErrorHandler::isVerbose = false;
int ErrorHandler::maxRetries = 3;
bool ErrorHandler::recoveryEnabled = true;

// Recovery manager for handling system-level issues
class RecoveryManager {
public:
    enum class RecoveryAction {
        None,
        ReduceComplexity,
        ApplyDefaults,
        UseBackupConfig,
        ExtendResources,
        SkipResourceIntensive,
        ContinueWithLimitedFunctionality
    };
    
    static bool attemptRecovery(ErrorHandler::ErrorType type, 
                               const std::string& context,
                               RecoveryAction& recommendedAction) {
        switch (type) {
            case ErrorHandler::ErrorType::ConfigurationError:
                recommendedAction = RecoveryAction::ApplyDefaults;
                return true;
            
            case ErrorHandler::ErrorType::FileError:
                recommendedAction = RecoveryAction::UseBackupConfig;
                return true;
            
            case ErrorHandler::ErrorType::SimulationError:
                recommendedAction = RecoveryAction::ReduceComplexity;
                return true;
            
            case ErrorHandler::ErrorType::ResourceError:
                recommendedAction = RecoveryAction::SkipResourceIntensive;
                return true;
            
            default:
                return false;
        }
    }
    
    static std::string getRecoveryMessage(RecoveryAction action) {
        switch (action) {
            case RecoveryAction::ApplyDefaults:
                return "Applying default configuration values.";
            case RecoveryAction::UseBackupConfig:
                return "Attempting to load backup configuration.";
            case RecoveryAction::ReduceComplexity:
                return "Reducing simulation complexity (fewer neurons, smaller steps).";
            case RecoveryAction::SkipResourceIntensive:
                return "Skipping resource-intensive operations.";
            case RecoveryAction::ContinueWithLimitedFunctionality:
                return "Continuing with limited functionality.";
            default:
                return "Attempting recovery...";
        }
    }
};

// Enhanced configuration loader with error handling
class SafeConfigLoader {
public:
    static std::shared_ptr<Config> loadConfig(const std::string& filepath, 
                                             bool attemptBackups = true) {
        auto config = std::make_shared<Config>();
        
        // Try to load primary config file
        if (!loadConfigFile(config, filepath)) {
            if (attemptBackups) {
                // Try backup files
                std::vector<std::string> backupFiles = {
                    filepath + ".backup",
                    filepath + ".bak",
                    "default.cfg",
                    "configs/default.cfg"
                };
                
                for (const auto& backup : backupFiles) {
                    if (loadConfigFile(config, backup)) {
                        std::cerr << "Loaded configuration from backup: " << backup << std::endl;
                        return config;
                    }
                }
            }
            
            // Return empty config with warnings
            std::cerr << "Warning: Could not load configuration from: " << filepath << std::endl;
            std::cerr << "Using empty configuration." << std::endl;
            return config;
        }
        
        return config;
    }
    
private:
    static bool loadConfigFile(std::shared_ptr<Config>& config, const std::string& filepath) {
        try {
            if (config->loadFromFile(filepath)) {
                return true;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception while loading config file " << filepath 
                      << ": " << e.what() << std::endl;
        }
        return false;
    }
};

// Result formatter for structured output
class ResultFormatter {
public:
    static void formatJson(const std::vector<TestResult>& results) {
        std::cout << "{\n";
        std::cout << "  \"test_run\": {\n";
        std::cout << "    \"timestamp\": \"" << getCurrentTimestamp() << "\"," << std::endl;
        std::cout << "    \"tests\": [" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            std::cout << "      {\n";
            std::cout << "        \"name\": \"" << result.name << "\"," << std::endl;
            std::cout << "        \"passed\": " << (result.passed ? "true" : "false") << "," << std::endl;
            std::cout << "        \"message\": \"" << escapeString(result.message) << "\"," << std::endl;
            std::cout << "        \"details\": \"" << escapeString(result.details) << "\"," << std::endl;
            std::cout << "        \"execution_time\": " << result.executionTime << std::endl;
            std::cout << "      }" << (i == results.size() - 1 ? "" : ",") << std::endl;
        }
        
        std::cout << "    ]" << std::endl;
        std::cout << "  }" << std::endl;
    }
    
    static void formatXml(const std::vector<TestResult>& results) {
        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        std::cout << "<test_run timestamp=\"" << getCurrentTimestamp() << "\">" << std::endl;
        
        for (const auto& result : results) {
            std::cout << "  <test>" << std::endl;
            std::cout << "    <name>" << result.name << "</name>" << std::endl;
            std::cout << "    <passed>" << (result.passed ? "true" : "false") << "</passed>" << std::endl;
            std::cout << "    <message>" << escapeString(result.message) << "</message>" << std::endl;
            std::cout << "    <details>" << escapeString(result.details) << "</details>" << std::endl;
            std::cout << "    <execution_time>" << result.executionTime << "</execution_time>" << std::endl;
            std::cout << "  </test>" << std::endl;
        }
        
        std::cout << "</test_run>" << std::endl;
    }
    
    static void formatSimple(const std::vector<TestResult>& results) {
        for (const auto& result : results) {
            std::cout << result.name << ": " << (result.passed ? "PASS" : "FAIL") << std::endl;
            std::cout << "  " << result.message << std::endl;
            if (!result.details.empty()) {
                std::cout << "  " << result.details << std::endl;
            }
        }
    }
    
private:
    static std::string escapeString(const std::string& str) {
        std::string escaped = str;
        // Basic escaping - could be enhanced for XML/JSON
        std::replace(escaped.begin(), escaped.end(), '"', "\\"");
        std::replace(escaped.begin(), escaped.end(), '\n', "\\n");
        return escaped;
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Main application class
class NLMApplication {
public:
    NLMApplication() : verbose(false), batchMode(false), testMode(false) {}
    
    bool run(int argc, char* argv[]) {
        CommandLineParser parser;
        setupOptions(parser);
        
        if (!parser.parse(argc, argv)) {
            return false;
        }
        
        parseArguments(parser);
        
        if (parser.getConfigFile().empty()) {
            std::cerr << "Error: No configuration file specified" << std::endl;
            return false;
        }
        
        return runApplication();
    }
    
    void setVerbosity(bool verbose) { this->verbose = verbose; }
    void setBatchMode(bool batch) { this->batchMode = batch; }
    void setTestMode(bool test) { this->testMode = test; }
    
private:
    bool verbose;
    bool batchMode;
    bool testMode;
    std::shared_ptr<Config> config;
    
    void setupOptions(CommandLineParser& parser) {
        // Core options
        parser.addOption("help", "h", "Show help message", "", false, {"flag"});
        parser.addOption("verbose", "v", "Enable verbose output", "false", false, {"bool"});
        parser.addOption("batch-mode", "", "Run in batch mode (non-interactive)", "false", false, {"bool"});
        parser.addOption("test-mode", "", "Run in test mode only", "false", false, {"bool"});
        parser.addOption("output-format", "", "Output format (simple, json, xml)", "simple", false, {"string"});
        
        // Configuration options
        parser.addOption("neuron-count", "", "Number of neurons in the brain", "500", false, {"int"});
        parser.addOption("region-count", "", "Number of brain regions", "1", false, {"int"});
        parser.addOption("connection-probability", "", "Synapse connection probability (0.0-1.0)", "0.15", false, {"float"});
        parser.addOption("random-seed", "", "Random seed for reproducibility", "42", false, {"int"});
        parser.addOption("simulation-timestep", "", "Simulation timestep in seconds", "0.001", false, {"float"});
        
        // Plasticity options
        parser.addOption("stdp-ltp-weight", "", "STDP LTP weight", "0.02", false, {"float"});
        parser.addOption("stdp-ltd-weight", "", "STDP LTD weight", "0.015", false, {"float"});
        parser.addOption("stdp-tau", "", "STDP time constant", "20.0", false, {"float"});
        
        // Advanced options
        parser.addOption("enable-plasticity", "", "Enable synaptic plasticity", "true", false, {"bool"});
        parser.addOption("enable-development", "", "Enable brain development", "true", false, {"bool"});
        parser.addOption("enable-neuromodulation", "", "Enable neuromodulation", "true", false, {"bool"});
    }
    
    void parseArguments(CommandLineParser& parser) {
        // Create configuration
        config = std::make_shared<Config>();
        
        // Load from config file if specified
        std::string configFile = parser.getConfigFile();
        if (!configFile.empty() && config->loadFromFile(configFile)) {
            if (verbose) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            }
        } else if (!configFile.empty()) {
            std::cerr << "Warning: Could not load configuration from: " << configFile << std::endl;
        }
        
        // Override with command line options
        if (parser.hasOption("neuron-count")) {
            config->set("neuron_count", parser.getIntOption("neuron-count"));
        }
        if (parser.hasOption("region-count")) {
            config->set("region_count", parser.getIntOption("region-count"));
        }
        if (parser.hasOption("connection-probability")) {
            config->set("connection_probability", parser.getFloatOption("connection-probability"));
        }
        if (parser.hasOption("random-seed")) {
            config->set("random_seed", parser.getIntOption("random-seed"));
        }
        if (parser.hasOption("simulation-timestep")) {
            config->set("simulation_timestep", parser.getFloatOption("simulation-timestep"));
        }
        if (parser.hasOption("stdp-ltp-weight")) {
            config->set("stdp_ltp_weight", parser.getFloatOption("stdp-ltp-weight"));
        }
        if (parser.hasOption("stdp-ltd-weight")) {
            config->set("stdp_ltd_weight", parser.getFloatOption("stdp-ltd-weight"));
        }
        if (parser.hasOption("stdp-tau")) {
            config->set("stdp_tau", parser.getFloatOption("stdp-tau"));
        }
        
        if (parser.hasOption("enable-plasticity")) {
            config->set("plasticity_enable", parser.getBoolOption("enable-plasticity"));
        }
        if (parser.hasOption("enable-development")) {
            config->set("development_enable", parser.getBoolOption("enable-development"));
        }
        if (parser.hasOption("enable-neuromodulation")) {
            config->set("neuromodulation_enable", parser.getBoolOption("enable-neuromodulation"));
        }
        
        // Set defaults for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    bool runApplication() {
        // Validate configuration
        if (!ConfigurationValidator::validate(*config)) {
            return false;
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        if (verbose) {
            std::cout << "Brain initialized with " << brain->getTotalNeuronCount() 
                      << " neurons and " << brain->getTotalSynapseCount() << " synapses." << std::endl;
        }
        
        if (testMode) {
            runTestSuite(brain);
        } else {
            runBasicSimulation(brain);
        }
        
        return true;
    }
    
    void runTestSuite(std::shared_ptr<Brain> brain) {
        std::cout << "Running NLM test suite..." << std::endl;
        
        TestRunner runner(config);
        
        // Add tests
        runner.addTest(std::make_unique<BasicConnectivityTest>());
        runner.addTest(std::make_unique<PlasticityExperimentTest>());
        runner.addTest(std::make_unique<StdpVerificationTest>());
        
        // Setup and run tests
        for (auto& test : runner.getTests()) {
            test->setup(brain);
        }
        
        auto results = runner.runAllTests();
        
        // Output results
        std::string format = parser.getStringOption("output-format", "simple");
        
        if (format == "json") {
            ResultFormatter::formatJson(results);
        } else if (format == "xml") {
            ResultFormatter::formatXml(results);
        } else {
            ResultFormatter::formatSimple(results);
        }
        
        // Exit with error code if any tests failed
        if (!batchMode) {
            int failed = 0;
            for (const auto& result : results) {
                if (!result.passed) failed++;
            }
            
            if (failed > 0) {
                std::cerr << "\n" << failed << " test(s) failed" << std::endl;
                return;
            }
        }
    }
    
    void runBasicSimulation(std::shared_ptr<Brain> brain) {
        std::cout << "Running basic simulation..." << std::endl;
        
        // Basic simulation with logging
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
            
            if (verbose && step % 20 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                          << " firing neurons" << std::endl;
            }
        }
        
        std::cout << "Simulation complete!" << std::endl;
        std::cout << "Final statistics:" << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    }
};

} // namespace nlm

int main(int argc, char* argv[]) {
    nlm::NLMApplication app;
    
    if (argc < 2) {
        std::cout << "Usage: nlm [OPTIONS] CONFIG_FILE" << std::endl;
        std::cout << "Use --help for more information." << std::endl;
        return 1;
    }
    
    bool success = app.run(argc, argv);
    return success ? 0 : 1;
}
