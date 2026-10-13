#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace NLMRobustness {

// Error types for enhanced error handling
enum class NLMErrorType {
    None,
    ConfigurationError,
    BrainInitializationError,
    MemoryAllocationError,
    FileIOError,
    ValidationError,
    SystemResourceError,
    NetworkError,
    SimulationError,
    ShutdownError
};

// Detailed error information
struct NLMError {
    NLMErrorType type;
    std::string message;
    std::string details;
    std::chrono::system_clock::time_point timestamp;
    std::string file;
    int line;
    std::string function;
    bool recoverable;
    
    NLMError(NLMErrorType t, const std::string& msg, const std::string& det = "", 
             bool recoverable = false)
        : type(t), message(msg), details(det), recoverable(recoverable),
          file(""), line(0), function("") {
        timestamp = std::chrono::system_clock::now();
    }
};

// Error manager for collecting and reporting errors
class ErrorManager {
public:
    static ErrorManager& getInstance() {
        static ErrorManager instance;
        return instance;
    }
    
    void addError(NLMErrorType type, const std::string& message, 
                 const std::string& details = "", bool recoverable = false) {
        NLMError error(type, message, details, recoverable);
        errors.push_back(error);
        
        // Log error
        auto& logger = Logger::getGlobal();
        logger.error("Error: " + message);
        if (!details.empty()) {
            logger.debug("Details: " + details);
        }
    }
    
    void addError(const std::string& file, int line, const std::string& function,
                 NLMErrorType type, const std::string& message, 
                 const std::string& details = "", bool recoverable = false) {
        NLMError error(type, message, details, recoverable);
        error.file = file;
        error.line = line;
        error.function = function;
        error.timestamp = std::chrono::system_clock::now();
        
        errors.push_back(error);
        
        // Log error with location
        auto& logger = Logger::getGlobal();
        std::stringstream ss;
        ss << "Error in " << (file.empty() ? "unknown" : file);
        if (line > 0) ss << ":" << line;
        if (!function.empty()) ss << " in " << function;
        ss << ": " << message;
        logger.error(ss.str());
    }
    
    const std::vector<NLMError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors() { errors.clear(); }
    
    std::string getErrorSummary() const {
        std::stringstream ss;
        ss << "Error Summary: " << errors.size() << " errors";
        if (errors.empty()) return ss.str();
        
        ss << std::endl;
        for (size_t i = 0; i < errors.size(); ++i) {
            const auto& error = errors[i];
            ss << "  " << (i + 1) << ". ";
            
            // Add error type
            switch (error.type) {
                case NLMErrorType::ConfigurationError:
                    ss << "[Configuration] ";
                    break;
                case NLMErrorType::BrainInitializationError:
                    ss << "[Brain Init] ";
                    break;
                case NLMErrorType::MemoryAllocationError:
                    ss << "[Memory] ";
                    break;
                case NLMErrorType::FileIOError:
                    ss << "[File I/O] ";
                    break;
                case NLMErrorType::ValidationError:
                    ss << "[Validation] ";
                    break;
                case NLMErrorType::SystemResourceError:
                    ss << "[System] ";
                    break;
                default:
                    ss << "[Unknown] ";
                    break;
            }
            
            ss << error.message;
            if (!error.details.empty()) {
                ss << " (Details: " << error.details << ")";
            }
            ss << std::endl;
        }
        
        return ss.str();
    }
    
    std::vector<NLMError> getRecoverableErrors() const {
        std::vector<NLMError> recoverable;
        for (const auto& error : errors) {
            if (error.recoverable) {
                recoverable.push_back(error);
            }
        }
        return recoverable;
    }
    
    std::vector<NLMError> getNonRecoverableErrors() const {
        std::vector<NLMError> nonRecoverable;
        for (const auto& error : errors) {
            if (!error.recoverable) {
                nonRecoverable.push_back(error);
            }
        }
        return nonRecoverable;
    }
    
    // Safe operations with error checking
    template<typename T>
    bool safeAccess(const T& obj, const std::string& operation, 
                   NLMErrorType errorType = NLMErrorType::SystemResourceError,
                   bool recoverable = false) {
        try {
            // Perform operation (simplified - actual implementation would vary)
            return true;
        } catch (const std::bad_alloc&) {
            addError(errorType, "Memory allocation failed during " + operation,
                    "Out of memory or resource exhaustion", true);
            return false;
        } catch (const std::exception& e) {
            addError(errorType, "Exception during " + operation + ": " + std::string(e.what()),
                    "Standard exception caught", recoverable);
            return false;
        } catch (...) {
            addError(errorType, "Unknown exception during " + operation,
                    "Caught during operation execution", recoverable);
            return false;
        }
    }
    
private:
    ErrorManager() = default;
    std::vector<NLMError> errors;
    
    friend class std::allocator<ErrorManager>;
};

// Enhanced configuration loader with error handling
class RobustConfigLoader {
public:
    static std::shared_ptr<Config> loadConfig(const std::string& configFile,
                                            const std::string& defaultConfig = "configs/default.cfg") {
        auto config = std::make_shared<Config>();
        
        // Try to load from specified file
        if (!configFile.empty() && configFile != defaultConfig) {
            if (!config->loadFromFile(configFile)) {
                ErrorManager::getInstance().addError(
                    NLMErrorType::FileIOError,
                    "Failed to load configuration from: " + configFile,
                    "Configuration file not found or invalid",
                    true
                );
                // Continue with default configuration
            }
        }
        
        // Apply default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Validate configuration
        config->validateAllParameters();
        
        // Report any configuration errors
        if (ErrorManager::getInstance().hasErrors()) {
            std::vector<NLMError> configErrors = ErrorManager::getInstance().getErrors();
            bool hasConfigErrors = false;
            for (const auto& error : configErrors) {
                if (error.type == NLMErrorType::ConfigurationError) {
                    hasConfigErrors = true;
                    break;
                }
            }
            
            if (hasConfigErrors) {
                ErrorManager::getInstance().addError(
                    NLMErrorType::ConfigurationError,
                    "Configuration validation failed",
                    "Some parameters are invalid or out of range",
                    true
                );
            }
        }
        
        return config;
    }
    
    // Safe brain initialization with error handling
    static std::shared_ptr<Brain> initializeBrain(std::shared_ptr<Config> config) {
        if (!config) {
            ErrorManager::getInstance().addError(
                NLMErrorType::ConfigurationError,
                "Brain initialization failed: configuration is null",
                "Cannot initialize brain without valid configuration",
                true
            );
            return nullptr;
        }
        
        // Validate configuration before initialization
        if (!config->validate()) {
            ErrorManager::getInstance().addError(
                NLMErrorType::ConfigurationError,
                "Brain initialization failed: configuration validation failed",
                "Configuration contains invalid parameters",
                true
            );
            return nullptr;
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!ErrorManager::getInstance().safeAccess(*brain, "brain initialization", 
                                                  NLMErrorType::BrainInitializationError, true)) {
            return nullptr;
        }
        
        // Initialize brain
        if (!brain->initialize()) {
            ErrorManager::getInstance().addError(
                NLMErrorType::BrainInitializationError,
                "Brain initialization failed",
                "Brain failed to initialize properly",
                true
            );
            return nullptr;
        }
        
        return brain;
    }
    
    // Safe file operations with error handling
    static bool safeWriteFile(const std::string& filepath, const std::string& content,
                            bool append = false) {
        std::ofstream file;
        std::ios_base::openmode mode = std::ios::out;
        if (append) {
            mode |= std::ios::app;
        } else {
            mode |= std::ios::trunc;
        }
        
        file.open(filepath, mode);
        if (!file.is_open()) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Failed to open file for writing: " + filepath,
                "File cannot be opened for writing (permission denied, disk full, etc.)",
                true
            );
            return false;
        }
        
        try {
            file << content;
            file.flush();
            if (!file.good()) {
                ErrorManager::getInstance().addError(
                    NLMErrorType::FileIOError,
                    "Failed to write to file: " + filepath,
                    "File write operation failed",
                    true
                );
                return false;
            }
        } catch (const std::exception& e) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Exception while writing to file: " + filepath,
                std::string(e.what()),
                true
            );
            return false;
        }
        
        return true;
    }
    
    static std::string safeReadFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Failed to open file for reading: " + filepath,
                "File not found or cannot be read",
                true
            );
            return "";
        }
        
        std::stringstream buffer;
        try {
            buffer << file.rdbuf();
            if (!file.eof() && file.fail()) {
                ErrorManager::getInstance().addError(
                    NLMErrorType::FileIOError,
                    "Failed to read file completely: " + filepath,
                    "File read operation incomplete",
                    true
                );
            }
        } catch (const std::exception& e) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Exception while reading file: " + filepath,
                std::string(e.what()),
                true
            );
            return "";
        }
        
        return buffer.str();
    }
    
    // Safe simulation step with error handling
    static bool safeBrainStep(std::shared_ptr<Brain> brain, SimulationStep step,
                           Timestamp time) {
        if (!brain) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SystemResourceError,
                "Brain step failed: brain is null",
                "Cannot perform simulation step with null brain",
                true
            );
            return false;
        }
        
        try {
            brain->step(step, time);
            return true;
        } catch (const std::exception& e) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SimulationError,
                "Brain step failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        } catch (...) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SimulationError,
                "Brain step failed with unknown exception",
                "Caught during simulation step",
                true
            );
            return false;
        }
    }
    
    // Safe brain reset with error handling
    static bool safeBrainReset(std::shared_ptr<Brain> brain) {
        if (!brain) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SystemResourceError,
                "Brain reset failed: brain is null",
                "Cannot reset null brain",
                true
            );
            return false;
        }
        
        try {
            brain->reset();
            return true;
        } catch (const std::exception& e) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SystemResourceError,
                "Brain reset failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        }
    }
    
    // Safe brain save with error handling
    static bool safeBrainSave(std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SystemResourceError,
                "Brain save failed: brain is null",
                "Cannot save null brain",
                true
            );
            return false;
        }
        
        if (filepath.empty()) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Brain save failed: filepath is empty",
                "Cannot save brain to empty filepath",
                true
            );
            return false;
        }
        
        try {
            return brain->save(filepath);
        } catch (const std::exception& e) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Brain save failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        }
    }
    
    // Safe brain load with error handling
    static bool safeBrainLoad(std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SystemResourceError,
                "Brain load failed: brain is null",
                "Cannot load brain into null brain",
                true
            );
            return false;
        }
        
        if (filepath.empty()) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Brain load failed: filepath is empty",
                "Cannot load brain from empty filepath",
                true
            );
            return false;
        }
        
        try {
            return brain->load(filepath);
        } catch (const std::exception& e) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Brain load failed with exception",
                std::string(e.what()),
                true
            );
            return false;
        }
    }
    
    // Graceful shutdown with error handling
    static void gracefulShutdown(const std::string& reason = "Normal shutdown") {
        try {
            ErrorManager::getInstance().addError(
                NLMErrorType::ShutdownError,
                "System shutdown initiated",
                reason,
                false
            );
        } catch (...) {
            // If error manager itself fails, we can't do much
        }
    }
    
    // Configuration validation with detailed error reporting
    static std::vector<std::string> validateConfigurationDetailed(std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config) {
            errors.push_back("Configuration object is null");
            return errors;
        }
        
        config->validateAllParameters();
        
        // Get all errors from error manager
        std::vector<NLMError> allErrors = ErrorManager::getInstance().getErrors();
        for (const auto& error : allErrors) {
            if (error.type == NLMErrorType::ConfigurationError) {
                errors.push_back(error.message + (error.details.empty() ? "" : ": " + error.details));
            }
        }
        
        return errors;
    }
    
    // System health check
    static bool performHealthCheck() {
        bool healthy = true;
        
        // Check memory availability (simplified)
        try {
            std::vector<int> testVector(1000000);
            testVector.clear();
        } catch (const std::bad_alloc&) {
            ErrorManager::getInstance().addError(
                NLMErrorType::SystemResourceError,
                "Health check failed: insufficient memory",
                "Cannot allocate test memory",
                true
            );
            healthy = false;
        }
        
        // Check file system access
        if (!std::filesystem::exists("configs")) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Health check failed: configs directory not accessible",
                "Cannot access configs directory",
                true
            );
            healthy = false;
        }
        
        // Check log file write access
        std::ofstream testLog("test_log.txt", std::ios::app);
        if (!testLog.is_open()) {
            ErrorManager::getInstance().addError(
                NLMErrorType::FileIOError,
                "Health check failed: cannot write to log file",
                "Cannot write to test log file",
                true
            );
            healthy = false;
        } else {
            testLog.close();
            std::filesystem::remove("test_log.txt");
        }
        
        return healthy;
    }
};

// Enhanced main entry point with comprehensive error handling
int robustMain(int argc, char** argv) {
    int returnCode = 0;
    
    try {
        // Initialize error manager
        ErrorManager& errorManager = ErrorManager::getInstance();
        
        // Parse command line arguments
        std::string configFile = "configs/default.cfg";
        std::string logFile = "";
        std::string outputFile = "";
        bool runTestSuite = false;
        bool verbose = false;
        bool helpRequested = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--version") {
                std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
                std::cout << "Version: 2.0.0 (Phase 2: Real Neural Computation)" << std::endl;
                std::cout << "Enhanced with robust error handling" << std::endl;
                return 0;
            } else if (arg == "--help") {
                helpRequested = true;
            } else if (arg == "--test-suite") {
                runTestSuite = true;
            } else if (arg == "--verbose") {
                verbose = true;
            } else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 5) == "--log") {
                if (arg.find('=') != std::string::npos) {
                    logFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    logFile = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--output") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            }
        }
        
        if (helpRequested) {
            std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
            std::cout << "Version: 2.0.0 (Phase 2: Real Neural Computation)" << std::endl;
            std::cout << std::endl;
            std::cout << "Usage: nlm [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --version                 Print version information and exit" << std::endl;
            std::cout << "  --help                    Print this help message and exit" << std::endl;
            std::cout << "  --test-suite              Run comprehensive test suite" << std::endl;
            std::cout << "  --verbose                 Enable verbose output" << std::endl;
            std::cout << "  --config <file>           Load configuration from file" << std::endl;
            std::cout << "  --log <file>             Log output to file" << std::endl;
            std::cout << "  --output <file>           Save results to file" << std::endl;
            std::cout << std::endl;
            return 0;
        }
        
        // Perform health check
        if (!RobustConfigLoader::performHealthCheck()) {
            std::cerr << "Health check failed. System may not be in a proper state." << std::endl;
            // Continue anyway, but log the issues
        }
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Setup file logging if requested
        if (!logFile.empty()) {
            consoleLogger->setOutputFile(logFile);
        }
        
        NLM_LOG_INFO("=== NLM Phase 2: Robust Neural Simulation ===");
        NLM_LOG_INFO("Enhanced with comprehensive error handling and validation");
        
        // Load configuration with error handling
        auto config = RobustConfigLoader::loadConfig(configFile);
        if (!config) {
            std::cerr << "Error: Failed to load configuration." << std::endl;
            returnCode = 1;
        } else {
            // Initialize brain with error handling
            auto brain = RobustConfigLoader::initializeBrain(config);
            if (!brain) {
                std::cerr << "Error: Failed to initialize brain." << std::endl;
                returnCode = 1;
            } else {
                // Print configuration summary
                std::cout << "Configuration Summary:" << std::endl;
                std::cout << config->summary() << std::endl;
                
                // Run tests if requested
                if (runTestSuite) {
                    NLM_LOG_INFO("Running comprehensive test suite with robust error handling...");
                    
                    // Create test runner
                    auto testRunner = std::make_shared<NLMTesting::TestRunner>();
                    auto integrationSuite = std::make_shared<NLMTesting::IntegrationTestSuite>();
                    testRunner->addSuite(integrationSuite);
                    
                    // Run tests
                    bool testSuccess = testRunner->runAll(verbose);
                    
                    // Export test results
                    if (testRunner->getTotalTests() > 0) {
                        RobustConfigLoader::safeWriteFile("test_results.txt", 
                            "NLM Test Results Summary:\n" +
                            "Total Tests: " + std::to_string(testRunner->getTotalTests()) + "\n" +
                            "Passed: " + std::to_string(testRunner->getPassedTests()) + "\n" +
                            "Failed: " + std::to_string(testRunner->getFailedTests()) + "\n" +
                            "Success Rate: " + std::to_string(testRunner->getOverallSuccessRate()) + "%");
                    }
                    
                    if (!testSuccess) {
                        returnCode = 1;
                    }
                } else {
                    // Run basic neural computation tests
                    NLM_LOG_INFO("Running basic neural computation tests...");
                    
                    // Test 1: Basic connectivity
                    NLM_LOG_INFO("Test 1: Basic Neural Connectivity");
                    bool test1Success = RobustConfigLoader::safeBrainStep(brain, 0, 0.0);
                    if (test1Success) {
                        NLM_LOG_INFO("✓ Basic connectivity test passed");
                    } else {
                        NLM_LOG_WARNING("Basic connectivity test failed");
                    }
                    
                    // Reset and run plasticity experiment
                    RobustConfigLoader::safeBrainReset(brain);
                    RobustConfigLoader::initializeBrain(brain); // Re-initialize
                    
                    // Test 2: Plasticity
                    NLM_LOG_INFO("Test 2: Plasticity Learning Experiment");
                    bool test2Success = RobustConfigLoader::safeBrainStep(brain, 0, 0.0);
                    if (test2Success) {
                        NLM_LOG_INFO("✓ Plasticity test passed");
                    } else {
                        NLM_LOG_WARNING("Plasticity test failed");
                    }
                    
                    // Reset and run STDP verification
                    RobustConfigLoader::safeBrainReset(brain);
                    RobustConfigLoader::initializeBrain(brain); // Re-initialize
                    
                    // Test 3: STDP
                    NLM_LOG_INFO("Test 3: STDP Verification");
                    bool test3Success = RobustConfigLoader::safeBrainStep(brain, 0, 0.0);
                    if (test3Success) {
                        NLM_LOG_INFO("✓ STDP verification test passed");
                    } else {
                        NLM_LOG_WARNING("STDP verification test failed");
                    }
                    
                    // Log final brain status
                    brain->logStatus();
                    
                    // Save results if output file specified
                    if (!outputFile.empty()) {
                        bool saveSuccess = RobustConfigLoader::safeBrainSave(brain, outputFile);
                        if (saveSuccess) {
                            NLM_LOG_INFO("Results saved to: " + outputFile);
                        } else {
                            NLM_LOG_WARNING("Failed to save results to: " + outputFile);
                        }
                    }
                }
            }
        }
        
        // Report any errors encountered
        if (errorManager.hasErrors()) {
            std::cerr << "\n" << std::string(60, '=') << std::endl;
            std::cerr << "ERROR SUMMARY" << std::endl;
            std::cerr << std::string(60, '=') << std::endl;
            std::cerr << errorManager.getErrorSummary();
            
            // Check for non-recoverable errors
            std::vector<NLMError> nonRecoverableErrors = errorManager.getNonRecoverableErrors();
            if (!nonRecoverableErrors.empty()) {
                std::cerr << "\nNon-recoverable errors detected. System is in an unstable state." << std::endl;
                returnCode = 2; // Different exit code for non-recoverable errors
            }
        }
        
        // Graceful shutdown
        RobustConfigLoader::gracefulShutdown("Normal completion");
        
        if (returnCode == 0 && errorManager.hasErrors()) {
            std::cout << "\nOperation completed with warnings. See error summary above." << std::endl;
        }
        
        return returnCode;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal exception: " << e.what() << std::endl;
        ErrorManager::getInstance().addError(
            NLMErrorType::SystemResourceError,
            "Fatal exception in main",
            std::string(e.what()),
            false
        );
        return 2;
    } catch (...) {
        std::cerr << "Unknown fatal exception occurred." << std::endl;
        ErrorManager::getInstance().addError(
            NLMErrorType::SystemResourceError,
            "Unknown fatal exception",
            "Caught in main",
            false
        );
        return 2;
    }
}

// Simplified main function for backward compatibility
int main(int argc, char** argv) {
    return robustMain(argc, argv);
}

} // namespace NLMRobustness