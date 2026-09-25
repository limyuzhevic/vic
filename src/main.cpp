// Updated main.cpp for NLM Phase 3 with comprehensive factory pattern
// Modern C++ implementation with factory-based object creation, dependency injection, and testability

#include "core/Config/Config.hpp"
#include "core/Config/ConfigParser.hpp"
#include "core/Factory/FactoryManager.hpp"
#include "core/Random/Random.hpp"
#include "core/Random/RandomGeneratorFactory.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/LoggerFactory.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/BrainFactory.hpp"
#include "world/World.hpp"
#include "world/WorldFactory.hpp"
#include "experiments/Experiment.hpp"
#include "experiments/ExperimentFactory.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "Error/NLMError.hpp"
#include "Error/ErrorCategory.hpp"

#include <chrono>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <variant>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <cmath>
#include <random>
#include <map>

using namespace std;
using namespace nlm;

// Simple LearningExperiment class for plasticity testing
class LearningExperiment {
public:
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed) 
        : brain_(brain), seed_(seed) {
        recordInitialState();
    }
    
    ~LearningExperiment() {}
    
    void recordInitialState() {
        if (brain_) {
            // Record initial brain state
            initialPlasticity_ = 0.0f;
            if (brain_->getNeurons()) {
                initialPlasticity_ = brain_->calculatePlasticity();
            }
        }
    }
    
    void recordFinalState() {
        if (brain_) {
            // Record final brain state
            finalPlasticity_ = brain_->calculatePlasticity();
        }
    }
    
    float calculatePlasticity() const {
        if (finalPlasticity_ >= 0 && initialPlasticity_ >= 0) {
            return finalPlasticity_ - initialPlasticity_;
        }
        return 0.0f;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    uint64_t seed_;
    float initialPlasticity_ = 0.0f;
    float finalPlasticity_ = 0.0f;
};

// Global error handling callback
NLMErrorHandler errorHandler = nullptr;

void setErrorHandler(NLMErrorHandler handler) {
    errorHandler = handler;
}

void reportError(ErrorCategory category, const std::string& message, ErrorHandlingResult result = ErrorHandlingResult::Terminate) {
    if (errorHandler) {
        errorHandler(category, message, result);
    } else {
        std::cerr << "[NLM " << errorCategoryToString(category) << "] " << message << std::endl;
    }
}

// Convert ErrorCategory to string for error reporting
static std::string errorCategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::FileSystem: return "FILESYSTEM";
        case ErrorCategory::Validation: return "VALIDATION";
        case ErrorCategory::Memory: return "MEMORY";
        case ErrorCategory::Threading: return "THREADING";
        case ErrorCategory::Configuration: return "CONFIGURATION";
        case ErrorCategory::Random: return "RANDOM";
        case ErrorCategory::Simulation: return "SIMULATION";
        case ErrorCategory::Generic: return "GENERIC";
        default: return "UNKNOWN";
    }
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 3: Factory Pattern Implementation                   ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Comprehensive factory pattern                           ║
    ║     - Dependency injection                                    ║
    ║     - Object pooling for performance                          ║
    ║     - Thread-safe factory operations                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Error types for better error categorization
enum class ErrorType {
    None,
    ArgumentError,
    ConfigError,
    InitializationError,
    FactoryError,
    RuntimeError
};

// Application configuration structure
struct AppConfig {
    // Command line arguments
    string configFile = "configs/default.cfg";
    bool helpRequested = false;
    bool versionRequested = false;
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    int neuronCount = 500;
    int regionCount = 1;
    float connectionProbability = 0.15f;
    bool runTests = true;
    bool verboseLogging = false;
    
    // Factory configuration
    std::string configFactoryId = "";
    std::string randomFactoryId = "";
    std::string loggerFactoryId = "";
    std::string brainFactoryId = "";
    std::string worldFactoryId = "";
    std::string experimentFactoryId = "";
    
    // Parsed config
    std::unique_ptr<Config> config;
    
    // Validate configuration
    bool validate() const {
        if (randomSeed == 0) return false;
        if (simulationTimestep <= 0.0) return false;
        if (neuronCount <= 0) return false;
        if (regionCount <= 0) return false;
        if (connectionProbability <= 0.0f || connectionProbability > 1.0f) return false;
        return true;
    }
    
    // Get error message for validation failures
    string getValidationError() const {
        if (randomSeed == 0) return "Random seed must be positive";
        if (simulationTimestep <= 0.0) return "Simulation timestep must be positive";
        if (neuronCount <= 0) return "Neuron count must be positive";
        if (regionCount <= 0) return "Region count must be positive";
        if (connectionProbability <= 0.0f || connectionProbability > 1.0f) return "Connection probability must be between 0 and 1";
        return "Unknown validation error";
    }
};

// Exception class for application-specific errors
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorType type, const std::string& message)
        : std::runtime_error(message), errorType(type) {}
    
    ErrorType getErrorType() const { return errorType; }
    
    static NLMException fromErrorType(ErrorType type, const std::string& message) {
        return NLMException(type, message);
    }
    
private:
    ErrorType errorType;
};

// Enhanced argument parsing with comprehensive error handling
namespace arg_utils {
    std::string toLower(std::string str) {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
    
    std::string extractValue(const std::string& arg, const std::string& prefix) {
        size_t pos = arg.find(prefix);
        if (pos != std::string::npos) {
            pos += prefix.size();
            if (pos < arg.size() && arg[pos] == '=') {
                pos++;
            }
            return arg.substr(pos);
        }
        return "";
    }
    
    bool isFlag(const std::string& arg) {
        return startsWith(arg, "--");
    }
    
    bool isShortFlag(const std::string& arg) {
        return startsWith(arg, "-");
    }
}

// Enhanced argument parser with validation and error handling
class ArgumentParser {
public:
    static AppConfig parseArgs(int argc, char* argv[]) {
        AppConfig config;
        
        // Validate input arguments
        if (argc <= 0) {
            throw std::invalid_argument("Invalid argument count: must be at least 1");
        }
        
        // Check for help/version requests early with validation
        for (int i = 1; i < argc; ++i) {
            if (argv[i] == nullptr) {
                throw std::invalid_argument("Null argument encountered at position " + std::to_string(i));
            }
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                config.helpRequested = true;
            } else if (arg == "-v" || arg == "--version") {
                config.versionRequested = true;
            } else if (arg == "-c" || arg == "--config") {
                if (i + 1 < argc) {
                    config.configFile = argv[++i];
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Config file path required after " + arg);
                }
            } else if (arg == "-s" || arg == "--seed") {
                if (i + 1 < argc) {
                    try {
                        config.randomSeed = std::stoull(argv[++i]);
                    } catch (const std::exception& e) {
                        throw NLMException(ErrorType::ArgumentError,
                                          "Invalid random seed: " + std::string(e.what()));
                    }
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Random seed value required after " + arg);
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    try {
                        config.simulationTimestep = std::stod(argv[++i]);
                    } catch (const std::exception& e) {
                        throw NLMException(ErrorType::ArgumentError,
                                          "Invalid timestep value: " + std::string(e.what()));
                    }
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Timestep value required after " + arg);
                }
            } else if (arg == "--neurons") {
                if (i + 1 < argc) {
                    try {
                        config.neuronCount = std::stoi(argv[++i]);
                    } catch (const std::exception& e) {
                        throw NLMException(ErrorType::ArgumentError,
                                          "Invalid neuron count: " + std::string(e.what()));
                    }
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Neuron count value required after " + arg);
                }
            } else if (arg == "--regions") {
                if (i + 1 < argc) {
                    try {
                        config.regionCount = std::stoi(argv[++i]);
                    } catch (const std::exception& e) {
                        throw NLMException(ErrorType::ArgumentError,
                                          "Invalid region count: " + std::string(e.what()));
                    }
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Region count value required after " + arg);
                }
            } else if (arg == "--connection-prob") {
                if (i + 1 < argc) {
                    try {
                        config.connectionProbability = std::stof(argv[++i]);
                    } catch (const std::exception& e) {
                        throw NLMException(ErrorType::ArgumentError,
                                          "Invalid connection probability: " + std::string(e.what()));
                    }
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Connection probability value required after " + arg);
                }
            } else if (arg == "--no-tests") {
                config.runTests = false;
            } else if (arg == "--verbose") {
                config.verboseLogging = true;
            } else if (arg == "--config-factory") {
                if (i + 1 < argc) {
                    config.configFactoryId = argv[++i];
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Config factory ID required after " + arg);
                }
            } else if (arg == "--random-factory") {
                if (i + 1 < argc) {
                    config.randomFactoryId = argv[++i];
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Random factory ID required after " + arg);
                }
            } else if (arg == "--logger-factory") {
                if (i + 1 < argc) {
                    config.loggerFactoryId = argv[++i];
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Logger factory ID required after " + arg);
                }
            } else if (arg == "--brain-factory") {
                if (i + 1 < argc) {
                    config.brainFactoryId = argv[++i];
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Brain factory ID required after " + arg);
                }
            } else if (arg == "--world-factory") {
                if (i + 1 < argc) {
                    config.worldFactoryId = argv[++i];
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "World factory ID required after " + arg);
                }
            } else if (arg == "--experiment-factory") {
                if (i + 1 < argc) {
                    config.experimentFactoryId = argv[++i];
                } else {
                    throw NLMException(ErrorType::ArgumentError,
                                      "Experiment factory ID required after " + arg);
                }
            } else {
                // Unknown argument with validation
                throw NLMException(ErrorType::ArgumentError,
                                  "Unknown argument: " + arg);
            }
        }
        
        return config;
    }
    
    static void printHelp() {
        cout << "Usage: " << PROGRAM_NAME << " [options]" << endl;
        cout << endl;
        cout << PROGRAM_DESCRIPTION << endl;
        cout << endl;
        cout << "Options:" << endl;
        cout << "  -h, --help                  Show this help message and exit" << endl;
        cout << "  -v, --version               Show version information and exit" << endl;
        cout << "  -c, --config FILE           Path to configuration file (JSON, YAML, or simple format)" << endl;
        cout << "  -s, --seed SEED             Random seed for reproducible experiments" << endl;
        cout << "      --timestep VALUE         Simulation timestep for numerical integration" << endl;
        cout << "      --neurons VALUE          Number of neurons in the network" << endl;
        cout << "      --regions VALUE          Number of neural regions" << endl;
        cout << "      --connection-prob VALUE  Probability of synaptic connections (0.0-1.0)" << endl;
        cout << "      --no-tests               Disable running test experiments" << endl;
        cout << "      --verbose                Enable verbose logging" << endl;
        cout << "      --config-factory ID       Config factory ID for dependency injection" << endl;
        cout << "      --random-factory ID       Random generator factory ID for dependency injection" << endl;
        cout << "      --logger-factory ID       Logger factory ID for dependency injection" << endl;
        cout << "      --brain-factory ID        Brain factory ID for dependency injection" << endl;
        cout << "      --world-factory ID        World factory ID for dependency injection" << endl;
        cout << "      --experiment-factory ID   Experiment factory ID for dependency injection" << endl;
        cout << endl;
        cout << "Examples:" << endl;
        cout << "  " << PROGRAM_NAME << " --config my_config.json" << endl;
        cout << "  " << PROGRAM_NAME << " --seed 123 --timestep 0.0001" << endl;
        cout << "  " << PROGRAM_NAME << " --config-factory default --random-factory secure" << endl;
        cout << "  " << PROGRAM_NAME << " --no-tests --verbose" << endl;
        cout << endl;
        cout << "Configuration files can be in JSON, YAML, or simple key=value format." << endl;
    }
    
    static void printVersion() {
        cout << PROGRAM_NAME << " version " << PROGRAM_VERSION << endl;
        cout << PROGRAM_DESCRIPTION << endl;
    }
};

// Enhanced config loader with retry logic and validation
class ConfigLoader {
public:
    static std::unique_ptr<Config> loadConfig(const AppConfig& appConfig) {
        std::unique_ptr<Config> config;
        
        // Validate config file path
        std::filesystem::path configPath(appConfig.configFile);
        
        if (!std::filesystem::exists(configPath)) {
            throw NLMException(ErrorType::ConfigError, 
                             "Configuration file not found: " + appConfig.configFile);
        }
        
        // Check file size before attempting to load
        try {
            auto fileSize = std::filesystem::file_size(configPath);
            if (fileSize > 100 * 1024 * 1024) { // 100MB limit
                throw NLMException(ErrorType::ConfigError,
                                 "Configuration file too large: " + appConfig.configFile + 
                                 " (" + std::to_string(fileSize) + " bytes)");
            }
        } catch (const std::filesystem::filesystem_error& e) {
            throw NLMException(ErrorType::ConfigError,
                             "Cannot access configuration file " + appConfig.configFile + ": " + 
                             std::string(e.what()));
        }
        
        try {
            // Detect file format with error handling
            std::string format = ConfigParser::detectFormat(configPath.string());
            
            // Load based on format with retry logic
            const int maxRetries = 3;
            for (int attempt = 0; attempt < maxRetries; ++attempt) {
                try {
                    if (format == "json") {
                        config = ConfigParser::parseFile(appConfig.configFile);
                    } else if (format == "yaml" || format == "yml") {
                        config = ConfigParser::parseFile(appConfig.configFile);
                    } else {
                        // Try simple format first
                        std::ifstream file(appConfig.configFile);
                        if (file.good()) {
                            std::stringstream buffer;
                            buffer << file.rdbuf();
                            std::string content = buffer.str();
                            
                            if (ConfigParser::isSimpleFormat(content)) {
                                config = ConfigParser::parseString(content, "simple");
                            } else {
                                // Auto-detect for complex formats
                                config = ConfigParser::parseFile(appConfig.configFile);
                            }
                        } else {
                            throw NLMException(ErrorType::ConfigError,
                                             "Cannot read configuration file: " + appConfig.configFile);
                        }
                    }
                    break; // Success, exit retry loop
                } catch (const std::exception& e) {
                    if (attempt == maxRetries - 1) {
                        throw NLMException(ErrorType::ConfigError,
                                         "Failed to load configuration after " + std::to_string(maxRetries) +
                                         " attempts: " + std::string(e.what()));
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (attempt + 1)));
                }
            }
            
        } catch (const std::exception& e) {
            throw NLMException(ErrorType::ConfigError,
                             "Error loading configuration: " + std::string(e.what()));
        }
        
        // Apply command line overrides
        if (appConfig.randomSeed != 42) {
            config->set("random_seed", appConfig.randomSeed, nlm::ConfigSource::Runtime);
        }
        
        if (appConfig.simulationTimestep != 0.001) {
            config->set("simulation_timestep", appConfig.simulationTimestep, nlm::ConfigSource::Runtime);
        }
        
        if (appConfig.neuronCount != 500) {
            config->set("neuron_count", appConfig.neuronCount, nlm::ConfigSource::Runtime);
        }
        
        if (appConfig.regionCount != 1) {
            config->set("region_count", appConfig.regionCount, nlm::ConfigSource::Runtime);
        }
        
        if (appConfig.connectionProbability != 0.15f) {
            config->set("connection_probability", appConfig.connectionProbability, nlm::ConfigSource::Runtime);
        }
        
        return config;
    }
};

int main(int argc, char* argv[]) {
    try {
        // Initialize error handling
        NLMErrorHandler originalHandler = errorHandler;
        
        // Print banner
        printBanner();
        
        // Parse command line arguments
        AppConfig appConfig = ArgumentParser::parseArgs(argc, argv);
        
        // Validate configuration
        if (!appConfig.validate()) {
            std::cerr << "Configuration validation failed: " << appConfig.getValidationError() << std::endl;
            if (!appConfig.helpRequested && !appConfig.versionRequested) {
                ArgumentParser::printHelp();
                return 1;
            }
        }
        
        // Show help or version and exit
        if (appConfig.helpRequested) {
            ArgumentParser::printHelp();
            return 0;
        }
        
        if (appConfig.versionRequested) {
            ArgumentParser::printVersion();
            return 0;
        }
        
        // Initialize factory system
        FactoryManager& factoryManager = FactoryManager::getInstance();
        factoryManager.discoverFactories();
        factoryManager.initializeAllFactories();
        
        // Configure factories based on command line arguments
        std::unordered_map<std::string, std::any> configFactoryConfig;
        configFactoryConfig["id"] = "default_config";
        configFactoryConfig["type"] = "file";
        factoryManager.registerConfig("config_factory_default", configFactoryConfig);
        
        std::unordered_map<std::string, std::any> randomFactoryConfig;
        randomFactoryConfig["id"] = "secure_random";
        randomFactoryConfig["type"] = "secure";
        factoryManager.registerConfig("random_factory_secure", randomFactoryConfig);
        
        std::unordered_map<std::string, std::any> loggerFactoryConfig;
        loggerFactoryConfig["id"] = "console_logger";
        loggerFactoryConfig["type"] = "console";
        loggerFactoryConfig["level"] = "info";
        factoryManager.registerConfig("logger_factory_console", loggerFactoryConfig);
        
        std::unordered_map<std::string, std::any> brainFactoryConfig;
        brainFactoryConfig["id"] = "adaptive_brain";
        brainFactoryConfig["type"] = "adaptive";
        factoryManager.registerConfig("brain_factory_adaptive", brainFactoryConfig);
        
        std::unordered_map<std::string, std::any> worldFactoryConfig;
        worldFactoryConfig["id"] = "dynamic_world";
        worldFactoryConfig["type"] = "dynamic";
        factoryManager.registerConfig("world_factory_dynamic", worldFactoryConfig);
        
        std::unordered_map<std::string, std::any> experimentFactoryConfig;
        experimentFactoryConfig["id"] = "phase_experiment";
        experimentFactoryConfig["type"] = "phase";
        factoryManager.registerConfig("experiment_factory_phase", experimentFactoryConfig);
        
        // Load configuration using factory
        std::unique_ptr<Config> config = factoryManager.createConfigFromConfig("config_factory_default");
        if (!config) {
            throw NLMException(ErrorType::FactoryError, "Failed to create config using factory");
        }
        
        appConfig.config = std::move(config);
        
        // Log configuration summary
        const Config* configPtr = appConfig.config.get();
        std::string value;
        struct ConfigSummary {
            std::string key;
            std::string value;
        } configSummary[] = {
            {"random_seed", std::to_string(configPtr->getOr<int64_t>("random_seed", appConfig.randomSeed))},
            {"simulation_timestep", std::to_string(configPtr->getOr<double>("simulation_timestep", appConfig.simulationTimestep)) + "s"},
            {"neuron_count", std::to_string(configPtr->getOr<int64_t>("neuron_count", appConfig.neuronCount))},
            {"region_count", std::to_string(configPtr->getOr<int64_t>("region_count", appConfig.regionCount))},
            {"connection_probability", std::to_string(configPtr->getOr<float>("connection_probability", appConfig.connectionProbability))}
        };
        
        for (const auto& item : configSummary) {
            NLM_LOG_INFO("  " + item.key + ": " + item.value);
        }
        
        // Log configuration source summary
        NLM_LOG_INFO("Configuration sources: " + configPtr->getSourceSummary());
        
        // Initialize simulation clock using factory
        double timestep = configPtr->getOr<double>("simulation_timestep", appConfig.simulationTimestep);
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing simulation clock with timestep: " + std::to_string(timestep) + "s");
        
        SimulationClock clock(timestep);
        
        // Create random generator using factory
        ObjectParams randomParams;
        randomParams.setParam("seed", configPtr->getOr<int64_t>("random_seed", appConfig.randomSeed));
        randomParams.setParam("type", "deterministic");
        
        auto randomGenerator = factoryManager.createRandomGenerator(appConfig.randomFactoryId, randomParams);
        if (!randomGenerator) {
            throw NLMException(ErrorType::FactoryError, "Failed to create random generator using factory");
        }
        
        // Create logger using factory
        ObjectParams loggerParams;
        loggerParams.setParam("level", static_cast<int>(LogLevel::Info));
        loggerParams.setParam("type", "console");
        loggerParams.setParam("useColors", appConfig.verboseLogging);
        
        auto logger = factoryManager.createLogger(appConfig.loggerFactoryId, loggerParams);
        if (!logger) {
            throw NLMException(ErrorType::FactoryError, "Failed to create logger using factory");
        }
        
        // Initialize brain using factory
        ObjectParams brainParams;
        brainParams.setParam("regions", std::vector<std::string>{"default"});
        brainParams.setParam("plasticityType", "stdp");
        brainParams.setParam("adaptationStrategy", "none");
        
        auto brain = factoryManager.createBrain(appConfig.brainFactoryId, brainParams);
        if (!brain) {
            throw NLMException(ErrorType::FactoryError, "Failed to create brain using factory");
        }
        
        if (!brain->initialize()) {
            throw NLMException(ErrorType::InitializationError, "Failed to initialize brain!");
        }
        
        brain->logStatus();
        
        // Run tests if enabled
        if (appConfig.runTests) {
            // Run Test 1: Basic connectivity
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 1: Basic Connectivity ===");
            brain->runBasicConnectivityTest();
            
            // Reset brain for plasticity experiment
            brain->reset();
            brain->initialize();
            
            // Run Test 2: Plasticity learning experiment
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
            LearningExperiment experiment(brain.get(), configPtr->getOr<int64_t>("random_seed", appConfig.randomSeed));
            experiment.recordInitialState();
            brain->runPlasticityExperiment();
            experiment.recordFinalState();
            
            NLM_LOG_INFO("Plasticity change: " + std::to_string(experiment.calculatePlasticity() * 100.0f) + "%");
            
            // Reset and run Test 3: STDP verification
            brain->reset();
            brain->initialize();
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Test 3: STDP Verification ===");
            brain->runStdpVerification();
        }
        
        // Create world using factory
        ObjectParams worldParams;
        worldParams.setParam("type", "simple");
        
        auto world = factoryManager.createWorld(appConfig.worldFactoryId, worldParams);
        if (!world) {
            throw NLMException(ErrorType::FactoryError, "Failed to create world using factory");
        }
        
        // Create experiment using factory
        ObjectParams experimentParams;
        experimentParams.setParam("type", "phase");
        experimentParams.setParam("phase", 2);
        
        auto experiment = factoryManager.createExperiment(appConfig.experimentFactoryId, experimentParams);
        if (!experiment) {
            throw NLMException(ErrorType::FactoryError, "Failed to create experiment using factory");
        }
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 3 Complete ===");
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Phase 3 Objectives Completed:");
        NLM_LOG_INFO("  ✓ Comprehensive factory pattern implemented");
        NLM_LOG_INFO("  ✓ Dependency injection support added");
        NLM_LOG_INFO("  ✓ Object pooling for performance");
        NLM_LOG_INFO("  ✓ Thread-safe factory operations");
        NLM_LOG_INFO("  ✓ Factory lifecycle management");
        NLM_LOG_INFO("  ✓ Factory discovery mechanism");
        
        // Save configuration snapshot for debugging
        std::string snapshotFile = "nlm_phase3_config_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
        if (configPtr->saveSnapshot(snapshotFile)) {
            NLM_LOG_INFO("Configuration snapshot saved to: " + snapshotFile);
        } else {
            NLM_LOG_WARNING("Failed to save configuration snapshot");
        }
        
        // Display factory statistics
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Factory System Statistics ===");
        auto factoryStats = factoryManager.getAllFactories();
        NLM_LOG_INFO("Registered factories: " + std::to_string(factoryStats.size()));
        
        for (const auto& entry : factoryStats) {
            NLM_LOG_INFO("  - " + entry.factoryId + " (type: " + entry.typeName + ")");
        }
        
        std::vector<std::string> testResults = factoryManager.getFactoryTestsResults();
        if (!testResults.empty()) {
            NLM_LOG_INFO("Factory test results:");
            for (const auto& result : testResults) {
                NLM_LOG_INFO("  - " + result);
            }
        }
        
        NLM_LOG_INFO("NLM Phase 3 execution completed successfully!");
        
        // Cleanup
        factoryManager.shutdownAllFactories();
        
    } catch (const NLMException& e) {
        NLM_LOG_ERROR("NLM Error: " + std::string(e.what()));
        return 1;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown error occurred");
        return 1;
    }
    
    return 0;
}
