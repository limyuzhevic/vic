#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <fstream>
#include <map>
#include <set>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "cli/ProfilerIntegration.hpp"

namespace nlm {

// Forward declarations
class AdvancedArgs;
class BatchProcessor;
class Profiler;
class ExtendedConfig;
class Brain; // Add forward declaration for Brain

/**
 * CLIEngine class - Main command-line interface engine for NLM
 * 
 * @brief Provides comprehensive CLI functionality for NLM including:
 *        - Enhanced argument parsing
 *        - Verbose/debug mode options
 *        - Output file options
 *        - Progress reporting options
 *        - Configuration validation and verification
 *        - Help and version information
 *        - Batch processing capabilities
 *        - Enhanced performance profiling integration
 *        - Advanced argument parsing
 * 
 * Features:
 * - Support for --help and --version flags
 * - Enhanced configuration file options
 * - Verbose/debug mode options
 * - Output file options
 * - Progress reporting options
 * - Multiple execution profiles
 * - Statistics aggregation
 * - Enhanced performance profiling with profiler integration
 * 
 * @note Integrates with existing Config system for configuration management
 * @note Uses Enhanced logging for verbose output
 * @see AdvancedArgs for advanced argument parsing
 * @see BatchProcessor for batch processing capabilities
 * @see ProfilerIntegration for enhanced performance profiling
 */
 */
class CLIEngine {
public:
    /**
     * @brief CLIEngine constructor
     * @details Initializes CLI engine with default settings
     * @post Engine ready for argument parsing and command execution
     */
    CLIEngine();
    
    /**
     * @brief CLIEngine destructor
     * @details Cleans up CLI engine resources
     * @post All resources released
     */
    ~CLIEngine();
    
    /**
     * @brief Parse command-line arguments
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if parsing successful, false if error
     * @post Configuration and settings parsed
     * 
     * @details Parses all supported command-line arguments including:
     * - --help, -h: Display help information
     * - --version, -v: Display version information
     * - --config=<file>: Load configuration from file
     * - --verbose, -V: Enable verbose output
     * - --debug, -d: Enable debug mode
     * - --output=<file>: Specify output file
     * - --batch: Enable batch processing mode
     * - --profile: Enable performance profiling
     * - --progress: Enable progress reporting
     * - --validate: Validate configuration
     * - And many more advanced options
     */
    bool parseArguments(int argc, char** argv);
    
    /**
     * @brief Get parsed configuration
     * @return Shared pointer to configuration
     * @post Configuration accessible for brain initialization
     * 
     * @details Returns the configuration after processing all sources:
     * 1. Default configuration
     * 2. Configuration file (if specified)
     * 3. Command-line arguments (highest priority)
     */
    std::shared_ptr<Config> getConfig() const { return config; }
    
    /**
     * @brief Get output file path
     * @return Output file path
     * @post Output file accessible for writing
     */
    const std::string& getOutputFile() const { return outputFile; }
    
    /**
     * @brief Check if verbose mode is enabled
     * @return true if verbose mode enabled, false otherwise
     * @post Verbosity level accessible
     */
    bool isVerbose() const { return verbose; }
    
    /**
     * @brief Check if debug mode is enabled
     * @return true if debug mode enabled, false otherwise
     * @post Debug mode accessible
     */
    bool isDebug() const { return debug; }
    
    /**
     * @brief Check if batch processing is enabled
     * @return true if batch processing enabled, false otherwise
     * @post Batch mode accessible
     */
    bool isBatchMode() const { return batchMode; }
    
    /**
     * @brief Check if profiling is enabled
     * @return true if profiling enabled, false otherwise
     * @post Profiling mode accessible
     */
    bool isProfilingEnabled() const { return profilingEnabled; }
    
    /**
     * @brief Check if progress reporting is enabled
     * @return true if progress reporting enabled, false otherwise
     * @post Progress reporting accessible
     */
    bool isProgressReportingEnabled() const { return progressReporting; }
    
    /**
     * @brief Check if validation is enabled
     * @return true if validation enabled, false otherwise
     * @post Validation mode accessible
     */
    bool isValidationEnabled() const { return validationEnabled; }
    
    /**
     * @brief Get progress reporting interval
     * @return Progress reporting interval in steps
     * @post Progress interval accessible
     */
    size_t getProgressInterval() const { return progressInterval; }
    
    /**
     * @brief Execute the main application logic
     * @return Exit code (0 for success, non-zero for failure)
     * @post Main application logic executed
     * 
     * @details Executes the main brain simulation logic:
     * - Loads configuration
     * - Initializes brain
     * - Runs experiments
     * - Collects metrics
     * - Writes output if specified
     * 
     * @see parseArguments() for argument parsing
     * @see getConfig() for configuration access
     */
    int execute();
    
    /**
     * @brief Display help information
     * @post Help information displayed to stdout
     * 
     * @details Displays comprehensive help including:
     * - Usage information
     * - All supported command-line options
     * - Configuration file format
     * - Examples
     * - Version information
     */
    void displayHelp() const;
    
    /**
     * @brief Display version information
     * @post Version information displayed to stdout
     * 
     * @details Displays NLM version and build information
     */
    void displayVersion() const;
    
    /**
     * @brief Write output to file
     * @param data Data to write
     * @post Data written to output file
     * 
     * @warning File permissions must allow writing
     * @see getOutputFile() for output file path
     */
    void writeOutput(const std::string& data) const;
    
    /**
     * @brief Get performance metrics
     * @return Performance metrics as string
     * @post Metrics collected and formatted
     */
    std::string getPerformanceMetrics() const;
    
    /**
     * @brief Get batch processing statistics
     * @return Batch processing statistics
     * @post Batch statistics collected
     */
    std::string getBatchStatistics() const;

    /**
     * @brief Get performance metrics
     * @return Performance metrics as string
     * @post Metrics collected and formatted
     */
    std::string getPerformanceMetrics() const;
    
    /**
     * @brief Get batch processing statistics
     * @return Batch processing statistics
     * @post Batch statistics collected
     */
    std::string getBatchStatistics() const;

private:
    std::shared_ptr<Config> config;
    std::shared_ptr<ExtendedConfig> extendedConfig;
    std::shared_ptr<AdvancedArgs> advancedArgs;
    std::shared_ptr<BatchProcessor> batchProcessor;
    std::shared_ptr<Profiler> profiler;
    std::shared_ptr<Brain> brain; // Add brain member variable
    
    std::string outputFile;
    bool verbose;
    bool debug;
    bool batchMode;
    bool profilingEnabled;
    bool progressReporting;
    bool validationEnabled;
    bool helpRequested;
    bool versionRequested;
    
    size_t progressInterval;
    std::string configFile;
    std::vector<std::string> batchConfigFiles;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    void initializeComponents();
    void loadExtendedConfiguration();
    void runExperiments();
    void collectMetrics();
    bool validateConfiguration() const;
    void logConfigurationSummary() const;
    
    // Suppress warnings about unused private members
    CLIEngine(const CLIEngine&) = delete;
    CLIEngine& operator=(const CLIEngine&) = delete;
};

} // namespace nlm