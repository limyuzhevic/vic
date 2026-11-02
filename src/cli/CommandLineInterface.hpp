#pragma once

/**
 * @file CommandLineInterface.hpp
 * @brief Unified command-line interface for NLM
 * 
 * This file defines the CommandLineInterface class, providing a comprehensive
 * command-line interface for the NLM brain simulation. It supports advanced
 * configuration options, visualization control, performance monitoring,
 * and debugging capabilities for both basic and advanced users.
 * 
 * @author NLM Research Team
 * @date Phase 6 Enhancement
 * @version 2.0
 * 
 * @features
 * - Unified configuration management (files, command line, runtime)
 * - Advanced visualization control and options
 * - Performance monitoring and profiling
 * - Debugging and diagnostic tools
 * - Help system with comprehensive documentation
 * - Configuration export/import capabilities
 * - Scriptable interface for automation
 * - Comprehensive error handling and reporting
 * 
 * @cli_overview The NLM command-line interface provides:
 * 1. Configuration management for brain simulation parameters
 * 2. Visualization control for real-time monitoring
 * 3. Performance monitoring and optimization
 * 4. Debugging and diagnostic tools
 * 5. Help and documentation system
 * 
 * @usage_examples Basic usage:
 *   nlm --brain.neuron-count=1000 --plasticity.learning-rate=0.01
 *   nlm --config=config.json --visualize --visualize-mode=network
 *   nlm --benchmark --benchmark-duration=60
 *   nlm --help  # Show comprehensive help
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include "core/Config/Config.hpp"
#include "visualization/VisualizationManager.hpp"
#include "performance/Performance.hpp"

namespace nlm {

/**
 * @enum CommandLineOption
 * @brief Enumeration of command-line options
 * 
 * @enumclass CommandLineOption
 * 
 * @var CommandLineOption::CONFIG_FILE Load configuration from file
 * @var CommandLineOption::CONFIG_KEY Set configuration key=value
 * @var CommandLineOption::VISUALIZE Enable visualization
 * @var CommandLineOption::VISUALIZE_MODE Set visualization mode
 * @var CommandLineOption::VISUALIZE_BACKEND Set visualization backend
 * @var CommandLineOption::VISUALIZE_EXPORT Export visualization to file
 * @var CommandLineOption::PERFORMANCE_ENABLE Enable performance monitoring
 * @var CommandLineOption::PERFORMANCE_STATS Display performance statistics
 * @var CommandLineOption::BENCHMARK Run benchmark test
 * @var CommandLineOption::BENCHMARK_DURATION Benchmark duration in seconds
 * @var CommandLineOption::LOG_LEVEL Set log level
 * @var CommandLineOption::LOG_TO_FILE Enable file logging
 * @var CommandLineOption::LOG_FILE Set log file path
 * @var CommandLineOption::RANDOM_SEED Set random seed
 * @var CommandLineOption::ENVIRONMENT Configure environment
 * @var CommandLineOption::VERSION Show version information
 * @var CommandLineOption::HELP Show help information
 * @var CommandLineOption::EXAMPLE Run example configuration
 * @var CommandLineOption::DEMO Run demo mode
 * @var CommandLineOption::TEST Run test suite
 * @var CommandLineOption::VALIDATE Validate configuration
 * @var CommandLineOption::EXPORT_CONFIG Export configuration to file
 * @var CommandLineOption::IMPORT_CONFIG Import configuration from file
 * @var CommandLineOption::LIST_CONFIG List configuration options
 * @var CommandLineOption::GET_CONFIG Get configuration value
 * @var CommandLineOption::SET_CONFIG Set configuration value
 * @var CommandLineOption::RESET_CONFIG Reset configuration to defaults
 * @var CommandLineOption::STATISTICS Show configuration statistics
 * @var CommandLineOption::VALIDATION Rules Show validation rules
 */
enum class CommandLineOption {
    CONFIG_FILE,
    CONFIG_KEY,
    VISUALIZE,
    VISUALIZE_MODE,
    VISUALIZE_BACKEND,
    VISUALIZE_EXPORT,
    PERFORMANCE_ENABLE,
    PERFORMANCE_STATS,
    BENCHMARK,
    BENCHMARK_DURATION,
    LOG_LEVEL,
    LOG_TO_FILE,
    LOG_FILE,
    RANDOM_SEED,
    ENVIRONMENT,
    VERSION,
    HELP,
    EXAMPLE,
    DEMO,
    TEST,
    VALIDATE,
    EXPORT_CONFIG,
    IMPORT_CONFIG,
    LIST_CONFIG,
    GET_CONFIG,
    SET_CONFIG,
    RESET_CONFIG,
    STATISTICS,
    VALIDATION_RULES
};

/**
 * @struct CommandLineArgs
 * @brief Parsed command-line arguments
 * 
 * This structure holds parsed command-line arguments and their values.
 * It provides a convenient interface for accessing command-line options
 * and their parameters.
 * 
 * @details The CommandLineArgs structure includes:
 * - Option flags and values
 * - Configuration options
 * - Visualization options
 * - Performance monitoring options
 * - Debugging and diagnostic options
 * - Help and version information
 * - Error and warning information
 * 
 * @note This structure is designed for efficient access and memory usage.
 */
struct CommandLineArgs {
    // Configuration options
    std::string configFile;                    ///< Configuration file path
    std::map<std::string, std::string> configKeys;  ///< Configuration key-value pairs
    
    // Visualization options
    bool enableVisualization = false;          ///< Enable visualization
    std::string visualizeMode;                  ///< Visualization mode
    std::string visualizeBackend;               ///< Visualization backend
    std::string visualizeExport;                ///< Visualization export file
    
    // Performance monitoring options
    bool enablePerformance = false;            ///< Enable performance monitoring
    bool showPerformanceStats = false;          ///< Show performance statistics
    
    // Benchmark options
    bool enableBenchmark = false;              ///< Enable benchmark mode
    double benchmarkDuration = 0.0;             ///< Benchmark duration in seconds
    
    // Logging options
    std::string logLevel;                      ///< Log level
    bool logToFile = false;                    ///< Enable file logging
    std::string logFile;                       ///< Log file path
    
    // Random seed options
    int64_t randomSeed = 0;                     ///< Random seed
    
    // Environment options
    std::string environmentConfig;              ///< Environment configuration
    
    // General options
    bool showVersion = false;                   ///< Show version information
    bool showHelp = false;                      ///< Show help information
    bool runExample = false;                    ///< Run example configuration
    bool runDemo = false;                       ///< Run demo mode
    bool runTests = false;                      ///< Run test suite
    bool validateConfig = false;                ///< Validate configuration
    std::string exportConfig;                   ///< Export configuration file
    std::string importConfig;                   ///< Import configuration file
    std::string getConfigKey;                   ///< Get configuration key
    std::string setConfigKey;                   ///< Set configuration key
    std::string setConfigValue;                 ///< Set configuration value
    bool resetConfig = false;                   ///< Reset configuration
    bool showStatistics = false;                ///< Show statistics
    bool showValidationRules = false;           ///< Show validation rules
    
    // Advanced options
    bool enableDebug = false;                   ///< Enable debug mode
    bool enableProfile = false;                 ///< Enable profiling
    std::string profileOutput;                  ///< Profile output file
    std::string outputDir;                      ///< Output directory
    std::string checkpointFile;                 ///< Checkpoint file path
    
    // Internal options
    std::vector<std::string> rawArgs;           ///< Raw command-line arguments
    std::map<std::string, std::string> parsedArgs;  ///< Parsed arguments
    
    // Statistics
    std::chrono::system_clock::time_point parseTime;   ///< Argument parsing time
    size_t totalArgs = 0;                        ///< Total arguments processed
    size_t validArgs = 0;                       ///< Valid arguments processed
    size_t invalidArgs = 0;                     ///< Invalid arguments processed
};

/**
 * @class CommandLineInterface
 * @brief Unified command-line interface for NLM
 * 
 * This class provides a comprehensive command-line interface for the NLM
 * brain simulation. It supports advanced configuration options, visualization
 * control, performance monitoring, and debugging capabilities for both
 * basic and advanced users.
 * 
 * @details The CommandLineInterface uses a sophisticated approach to
 * command-line parsing:
 * 
 * 1. **Argument Parsing**: Parses command-line arguments using modern C++
 *    techniques with comprehensive error handling.
 * 
 * 2. **Configuration Management**: Integrates with the Config class to
 *    provide configuration management through command-line arguments.
 * 
 * 3. **Visualization Control**: Provides control over visualization
 *    options including mode, backend, and export capabilities.
 * 
 * 4. **Performance Monitoring**: Enables performance monitoring and
 *    profiling with detailed statistics.
 * 
 * 5. **Help System**: Comprehensive help system with examples and
 *    detailed documentation.
 * 
 * 6. **Error Handling**: Robust error handling with detailed error
 *    messages and recovery mechanisms.
 * 
 * @thread_safety This class is thread-safe for read operations.
 * Write operations should be protected by appropriate locking.
 * 
 * @exception_safety All operations provide strong exception safety.
 * If an exception is thrown, the interface remains in a valid state.
 */
class CommandLineInterface {
public:
    /**
     * @brief Construct command-line interface
     * 
     * Creates a CommandLineInterface object with default settings.
     * The interface is ready to parse command-line arguments.
     */
    CommandLineInterface();
    
    /**
     * @brief Destructor for cleanup
     * 
     * Properly cleans up all resources including configuration,
     * visualization, and performance monitoring components.
     */
    ~CommandLineInterface();
    
    /**
     * @brief Disable copying to prevent resource conflicts
     * 
     * @param other CommandLineInterface to copy from
     * @throws std::runtime_error if copying is attempted
     */
    CommandLineInterface(const CommandLineInterface&) = delete;
    CommandLineInterface& operator=(const CommandLineInterface&) = delete;
    
    /**
     * @brief Enable move construction for efficient transfer
     * 
     * @param other CommandLineInterface to move from
     */
    CommandLineInterface(CommandLineInterface&&) noexcept;
    
    /**
     * @brief Enable move assignment for efficient transfer
     * 
     * @param other CommandLineInterface to move from
     * @return Reference to this CommandLineInterface after assignment
     */
    CommandLineInterface& operator=(CommandLineInterface&&) noexcept;
    
    /**
     * @brief Parse command-line arguments
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array
     * @return Parsed command-line arguments
     * @throws std::runtime_error if argument parsing fails
     * 
     * @details This method parses command-line arguments using modern C++
     * techniques. It supports both short and long options, as well as
     * key-value pairs. The parsing includes comprehensive validation
     * and error handling.
     */
    static CommandLineArgs parseArgs(int argc, char** argv);
    
    /**
     * @brief Process parsed command-line arguments
     * 
     * @param args Parsed command-line arguments
     * @return true if processing succeeded, false otherwise
     * @throws std::runtime_error if argument processing fails
     * 
     * @details This method processes parsed command-line arguments,
     * configuring the NLM system accordingly. It includes configuration
     * loading, visualization setup, performance monitoring, and
     * debugging configuration.
     */
    bool processArgs(const CommandLineArgs& args);
    
    /**
     * @brief Get help information
     * 
     * @param topic Help topic (empty string for general help)
     * @return Help information string
     * 
     * @details This method provides comprehensive help information
     * for the NLM command-line interface. It includes:
     * - General help overview
     * - Configuration options
     * - Visualization options
     * - Performance monitoring options
     * - Debugging options
     * - Examples and usage patterns
     */
    static std::string getHelp(const std::string& topic = "");
    
    /**
     * @brief Get version information
     * 
     * @return Version information string
     */
    static std::string getVersion();
    
    /**
     * @brief Get example configurations
     * 
     * @return Map of example configuration names and descriptions
     */
    static std::map<std::string, std::string> getExamples();
    
    /**
     * @brief Get demo configurations
     * 
     * @return Map of demo configuration names and descriptions
     */
    static std::map<std::string, std::string> getDemos();
    
    /**
     * @brief Get configuration options
     * 
     * @return Map of configuration option names and descriptions
     */
    static std::map<std::string, std::string> getConfigOptions();
    
    /**
     * @brief Get visualization options
     * 
     * @return Map of visualization option names and descriptions
     */
    static std::map<std::string, std::string> getVisualizationOptions();
    
    /**
     * @brief Get performance monitoring options
     * 
     * @return Map of performance monitoring option names and descriptions
     */
    static std::map<std::string, std::string> getPerformanceOptions();
    
    /**
     * @brief Get debugging options
     * 
     * @return Map of debugging option names and descriptions
     */
    static std::map<std::string, std::string> getDebugOptions();
    
    /**
     * @brief Get log levels
     * 
     * @return Vector of available log level names
     */
    static std::vector<std::string> getLogLevels();
    
    /**
     * @brief Get visualization backends
     * 
     * @return Vector of available visualization backend names
     */
    static std::vector<std::string> getVisualizationBackends();
    
    /**
     * @brief Get visualization modes
     * 
     * @return Vector of available visualization mode names
     */
    static std::vector<std::string> getVisualizationModes();
    
    /**
     * @brief Get configuration file formats
     * 
     * @return Vector of supported configuration file format names
     */
    static std::vector<std::string> getConfigFileFormats();
    
    /**
     * @brief Get available examples
     * 
     * @return Vector of available example names
     */
    static std::vector<std::string> getAvailableExamples();
    
    /**
     * @brief Get available demos
     * 
     * @return Vector of available demo names
     */
    static std::vector<std::string> getAvailableDemos();
    
    /**
     * @brief Validate command-line arguments
     * 
     * @param args Command-line arguments to validate
     * @return Validation result (true if valid, false otherwise)
     */
    static bool validateArgs(const CommandLineArgs& args);
    
    /**
     * @brief Show version and build information
     */
    static void showVersionInfo();
    
    /**
     * @brief Show help information
     * 
     * @param topic Help topic (optional)
     */
    static void showHelp(const std::string& topic = "");
    
    /**
     * @brief Show example configurations
     */
    static void showExamples();
    
    /**
     * @brief Show demo configurations
     */
    static void showDemos();
    
    /**
     * @brief Show configuration options
     */
    static void showConfigOptions();
    
    /**
     * @brief Show visualization options
     */
    static void showVisualizationOptions();
    
    /**
     * @brief Show performance monitoring options
     */
    static void showPerformanceOptions();
    
    /**
     * @brief Show debugging options
     */
    static void showDebugOptions();
    
    /**
     * @brief Show log level options
     */
    static void showLogLevelOptions();
    
    /**
     * @brief Show visualization backend options
     */
    static void showVisualizationBackendOptions();
    
    /**
     * @brief Show visualization mode options
     */
    static void showVisualizationModeOptions();
    
    /**
     * @brief Show configuration file format options
     */
    static void showConfigFileFormatOptions();
    
    /**
     * @brief Show available examples
     */
    static void showAvailableExamples();
    
    /**
     * @brief Show available demos
     */
    static void showAvailableDemos();
    
    /**
     * @brief Run example configuration
     * 
     * @param exampleName Name of example to run
     * @return true if example ran successfully
     */
    static bool runExample(const std::string& exampleName);
    
    /**
     * @brief Run demo configuration
     * 
     * @param demoName Name of demo to run
     * @return true if demo ran successfully
     */
    static bool runDemo(const std::string& demoName);
    
    /**
     * @brief Run test suite
     * 
     * @return true if test suite ran successfully
     */
    static bool runTestSuite();
    
    /**
     * @brief Validate configuration
     * 
     * @return true if configuration is valid
     */
    static bool validateConfiguration();
    
    /**
     * @brief Export configuration
     * 
     * @param filename Output file name
     * @param format File format
     * @return true if configuration was exported successfully
     */
    static bool exportConfiguration(const std::string& filename, const std::string& format = "json");
    
    /**
     * @brief Import configuration
     * 
     * @param filename Input file name
     * @return true if configuration was imported successfully
     */
    static bool importConfiguration(const std::string& filename);
    
    /**
     * @brief Get configuration value
     * 
     * @param key Configuration key
     * @return Configuration value
     */
    static std::string getConfiguration(const std::string& key);
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return true if configuration was set successfully
     */
    static bool setConfiguration(const std::string& key, const std::string& value);
    
    /**
     * @brief Reset configuration to defaults
     */
    static void resetConfiguration();
    
    /**
     * @brief Show configuration statistics
     */
    static void showConfigurationStatistics();
    
    /**
     * @brief Show validation rules
     */
    static void showValidationRules();
    
    /**
     * @brief Initialize CLI with configuration
     * 
     * @param config Configuration object to initialize with
     */
    void initialize(const Config& config);
    
    /**
     * @brief Initialize CLI with visualization
     * 
     * @param visualization Visualization object to initialize with
     */
    void initialize(const VisualizationManager& visualization);
    
    /**
     * @brief Initialize CLI with performance monitoring
     * 
     * @param performance Performance monitoring object to initialize with
     */
    void initialize(const PerformanceMonitor& performance);
    
    /**
     * @brief Get parsed arguments
     * 
     * @return Parsed command-line arguments
     */
    const CommandLineArgs& getArgs() const;
    
    /**
     * @brief Check if CLI is initialized
     * 
     * @return true if CLI is initialized
     */
    bool isInitialized() const;
    
    /**
     * @brief Reset CLI to initial state
     */
    void reset();
    
    /**
     * @brief Set callback for CLI events
     * 
     * @param callback Function to call for CLI events
     */
    void setEventCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief Set callback for CLI errors
     * 
     * @param callback Function to call for CLI errors
     */
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief Get CLI statistics
     * 
     * @return CLI statistics string
     */
    std::string getStatistics() const;
    
    /**
     * @brief Check if CLI has errors
     * 
     * @return true if CLI has errors
     */
    bool hasErrors() const;
    
    /**
     * @brief Get error count
     * 
     * @return Number of errors
     */
    size_t getErrorCount() const;
    
    /**
     * @brief Get warning count
     * 
     * @return Number of warnings
     */
    size_t getWarningCount() const;
    
    /**
     * @brief Get success count
     * 
     * @return Number of successful operations
     */
    size_t getSuccessCount() const;
    
    /**
     * @brief Get failure count
     * 
     * @return Number of failed operations
     */
    size_t getFailureCount() const;
    
    /**
     * @brief Get parse time
     * 
     * @return Parse time in seconds
     */
    double getParseTime() const;
    
    /**
     * @brief Get total arguments processed
     * 
     * @return Total arguments processed
     */
    size_t getTotalArgs() const;
    
    /**
     * @brief Get valid arguments processed
     * 
     * @return Valid arguments processed
     */
    size_t getValidArgs() const;
    
    /**
     * @brief Get invalid arguments processed
     * 
     * @return Invalid arguments processed
     */
    size_t getInvalidArgs() const;
    
private:
    /**
     * @struct Impl
     * @brief Private implementation details for CommandLineInterface
     */
    struct Impl;
    
    std::unique_ptr<Impl> pImpl; ///< Private implementation pointer
    
    // Helper functions for argument parsing
    static bool parseShortOption(const std::string& arg, CommandLineArgs& args);
    static bool parseLongOption(const std::string& arg, CommandLineArgs& args);
    static bool parseKeyValuePair(const std::string& arg, CommandLineArgs& args);
    static bool parseConfigOption(const std::string& key, const std::string& value, CommandLineArgs& args);
    static bool parseVisualizationOption(const std::string& key, const std::string& value, CommandLineArgs& args);
    static bool parsePerformanceOption(const std::string& key, const std::string& value, CommandLineArgs& args);
    static bool parseDebugOption(const std::string& key, const std::string& value, CommandLineArgs& args);
    static bool parseLogOption(const std::string& key, const std::string& value, CommandLineArgs& args);
    static bool parseBenchmarkOption(const std::string& key, const std::string& value, CommandLineArgs& args);
    static bool parseEnvironmentOption(const std::string& key, const std::string& value, CommandLineArgs& args);
    
    // Helper functions for validation
    static bool isValidOption(const std::string& option) const;
    static bool isValidConfigKey(const std::string& key) const;
    static bool isValidVisualizationMode(const std::string& mode) const;
    static bool isValidVisualizationBackend(const std::string& backend) const;
    static bool isValidLogLevel(const std::string& level) const;
    static bool isValidEnvironment(const std::string& env) const;
    
    // Helper functions for error handling
    static void throwArgError(const std::string& message);
    static void throwValidationError(const std::string& message);
    static void throwConfigError(const std::string& message);
    
    // Helper functions for help and documentation
    static std::string buildHelpString(const std::string& topic) const;
    static std::string buildExampleString() const;
    static std::string buildDemoString() const;
    static std::string buildConfigOptionsString() const;
    static std::string buildVisualizationOptionsString() const;
    static std::string buildPerformanceOptionsString() const;
    static std::string buildDebugOptionsString() const;
    static std::string buildLogLevelOptionsString() const;
    static std::string buildVisualizationBackendOptionsString() const;
    static std::string buildVisualizationModeOptionsString() const;
    static std::string buildConfigFileFormatOptionsString() const;
    
    // Helper functions for statistics
    static std::string buildStatisticsString() const;
    
    // Statistics
    struct Statistics {
        size_t totalArgs = 0;
        size_t validArgs = 0;
        size_t invalidArgs = 0;
        size_t errorCount = 0;
        size_t warningCount = 0;
        size_t successCount = 0;
        size_t failureCount = 0;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        
        Statistics() {
            startTime = std::chrono::system_clock::now();
        }
        
        void incrementTotalArgs() { totalArgs++; }
        void incrementValidArgs() { validArgs++; }
        void incrementInvalidArgs() { invalidArgs++; }
        void incrementErrorCount() { errorCount++; }
        void incrementWarningCount() { warningCount++; }
        void incrementSuccessCount() { successCount++; }
        void incrementFailureCount() { failureCount++; }
        
        void endTiming() { endTime = std::chrono::system_clock::now(); }
        
        double getParseTime() const {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
            return static_cast<double>(duration.count());
        }
    };
    
    // Constants
    static constexpr const char* VERSION = "2.0.0";
    static constexpr const char* PROGRAM_NAME = "nlm";
    static constexpr const char* AUTHOR = "NLM Research Team";
    static constexpr const char* COPYRIGHT = "Copyright (c) 2026 NLM Research Team";
    static constexpr const char* LICENSE = "MIT License";
    static constexpr const char* DESCRIPTION = "Neural Learning Machine - Advanced Spiking Neural Network Simulator";
    
    // Option strings
    static const std::vector<std::string> SHORT_OPTIONS;
    static const std::vector<std::string> LONG_OPTIONS;
    
    // Validation rules
    static const std::unordered_set<std::string> VALID_SHORT_OPTIONS;
    static const std::unordered_set<std::string> VALID_LONG_OPTIONS;
    
    // Callback functions
    std::function<void(const std::string&)> eventCallback_;
    std::function<void(const std::string&)> errorCallback_;
}; // namespace nlm

// Template implementations
#include "CommandLineInterface.tpp"

} // namespace nlm