// AdvancedArgs.hpp - Advanced argument parsing for NLM
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <chrono>

#include "core/Config/Config.hpp"

namespace nlm {

/**
 * AdvancedArgs class - Advanced command-line argument parsing for NLM
 * 
 * @brief Provides comprehensive advanced argument parsing including:
 *        - --help and --version flags
 *        - Enhanced configuration file options
 *        - Verbose/debug mode options
 *        - Output file options
 *        - Progress reporting options
 *        - Extended configuration options
 *        - Batch processing options
 *        - Performance profiling options
 * 
 * Features:
 * - Support for --help and --version flags
 * - Enhanced configuration file options with multiple file support
 * - Verbose/debug mode options with different levels
 * - Output file options with format specification
 * - Progress reporting options with customizable intervals
 * - Extended configuration options for plasticity, memory, neuromodulation
 * - Batch processing options with different profiles
 * - Performance profiling options with timing and memory tracking
 * - Error injection and recovery testing options
 * - Validation and verification commands
 * 
 * @note Integrates with existing Config system for configuration management
 * @see CLIEngine for main CLI interface
 * @see ExtendedConfig for extended configuration options
 */
class AdvancedArgs {
public:
    /**
     * @brief AdvancedArgs constructor
     * @details Initializes advanced argument parser with default options
     * @post Parser ready for argument parsing
     */
    AdvancedArgs();
    
    /**
     * @brief AdvancedArgs destructor
     * @details Cleans up argument parser resources
     * @post All resources released
     */
    ~AdvancedArgs();
    
    /**
     * @brief Parse command-line arguments
     * @param argc Argument count
     * @param argv Argument vector
     * @param config Configuration to update
     * @return true if parsing successful, false if error
     * @post Configuration updated with parsed arguments
     * 
     * @details Parses all advanced command-line arguments:
     * - Basic flags (--help, --version)
     * - Configuration options (--config, --batch-config)
     * - Verbosity and debug options (--verbose, --debug)
     * - Output options (--output, --format)
     * - Progress options (--progress, --progress-interval)
     * - Profiling options (--profile, --profile-file)
     * - Execution options (--batch, --validate)
     * - Extended options (plasticity, memory, neuromodulation)
     * - Error testing options (--error-inject, --recovery-test)
     * - Validation options (--validate, --verify)
     */
    bool parse(int argc, char** argv, Config& config);
    
    /**
     * @brief Check if help is requested
     * @return true if help requested, false otherwise
     * @post Help status accessible
     */
    bool isHelpRequested() const { return helpRequested; }
    
    /**
     * @brief Check if version is requested
     * @return true if version requested, false otherwise
     * @post Version status accessible
     */
    bool isVersionRequested() const { return versionRequested; }
    
    /**
     * @brief Check if verbose mode is enabled
     * @return true if verbose mode enabled, false otherwise
     * @post Verbosity status accessible
     */
    bool isVerbose() const { return verbose; }
    
    /**
     * @brief Check if debug mode is enabled
     * @return true if debug mode enabled, false otherwise
     * @post Debug mode accessible
     */
    bool isDebug() const { return debug; }
    
    /**
     * @brief Get debug level
     * @return Debug level (0=none, 1=basic, 2=detailed, 3=extreme)
     * @post Debug level accessible
     */
    int getDebugLevel() const { return debugLevel; }
    
    /**
     * @brief Check if batch processing is enabled
     * @return true if batch processing enabled, false otherwise
     * @post Batch mode accessible
     */
    bool isBatchMode() const { return batchMode; }
    
    /**
     * @brief Check if profiling is enabled
     * @return true if profiling enabled, false otherwise
     * @post Profiling status accessible
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
     * @post Validation status accessible
     */
    bool isValidationEnabled() const { return validationEnabled; }
    
    /**
     * @brief Check if error injection is enabled
     * @return true if error injection enabled, false otherwise
     * @post Error injection status accessible
     */
    bool isErrorInjectionEnabled() const { return errorInjectionEnabled; }
    
    /**
     * @brief Check if recovery testing is enabled
     * @return true if recovery testing enabled, false otherwise
     * @post Recovery testing status accessible
     */
    bool isRecoveryTestingEnabled() const { return recoveryTestingEnabled; }
    
    /**
     * @brief Check if statistics collection is enabled
     * @return true if statistics collection enabled, false otherwise
     * @post Statistics collection status accessible
     */
    bool isStatisticsEnabled() const { return statisticsEnabled; }
    
    /**
     * @brief Get output file path
     * @return Output file path
     * @post Output file path accessible
     */
    const std::string& getOutputFile() const { return outputFile; }
    
    /**
     * @brief Get output format
     * @return Output format (text, json, csv)
     * @post Output format accessible
     */
    const std::string& getOutputFormat() const { return outputFormat; }
    
    /**
     * @brief Get progress reporting interval
     * @return Progress reporting interval in steps
     * @post Progress interval accessible
     */
    size_t getProgressInterval() const { return progressInterval; }
    
    /**
     * @brief Get configuration file path
     * @return Configuration file path
     * @post Config file path accessible
     */
    const std::string& getConfigFile() const { return configFile; }
    
    /**
     * @brief Get batch configuration files
     * @return Vector of batch configuration file paths
     * @post Batch config files accessible
     */
    const std::vector<std::string>& getBatchConfigFiles() const { return batchConfigFiles; }
    
    /**
     * @brief Get profiling file path
     * @return Profiling file path
     * @post Profiling file accessible
     */
    const std::string& getProfilingFile() const { return profilingFile; }
    
    /**
     * @brief Get batch profile name
     * @return Batch profile name
     * @post Batch profile accessible
     */
    const std::string& getBatchProfile() const { return batchProfile; }
    
    /**
     * @brief Get error injection type
     * @return Error injection type (none, neuron, synapse, etc.)
     * @post Error injection type accessible
     */
    const std::string& getErrorInjectionType() const { return errorInjectionType; }
    
    /**
     * @brief Get error injection probability
     * @return Error injection probability (0.0-1.0)
     * @post Error injection probability accessible
     */
    float getErrorInjectionProbability() const { return errorInjectionProbability; }
    
    /**
     * @brief Get number of neurons
     * @return Number of neurons
     * @post Neuron count accessible
     */
    int64_t getNeuronCount() const { return neuronCount; }
    
    /**
     * @brief Get number of regions
     * @return Number of regions
     * @post Region count accessible
     */
    int64_t getRegionCount() const { return regionCount; }
    
    /**
     * @brief Get simulation timestep
     * @return Simulation timestep
     * @post Timestep accessible
     */
    double getTimestep() const { return timestep; }
    
    /**
     * @brief Get random seed
     * @return Random seed
     * @post Random seed accessible
     */
    int64_t getRandomSeed() const { return randomSeed; }
    
    /**
     * @brief Get connection probability
     * @return Connection probability
     * @post Connection probability accessible
     */
    float getConnectionProbability() const { return connectionProbability; }
    
    /**
     * @brief Get STDP LTP weight
     * @return STDP LTP weight
     * @post STDP LTP weight accessible
     */
    float getSTDP_LTPWeight() const { return stdpLTPWeight; }
    
    /**
     * @brief Get STDP LTD weight
     * @return STDP LTD weight
     * @post STDP LTD weight accessible
     */
    float getSTDP_LTDWeight() const { return stdpLTDWeight; }
    
    /**
     * @brief Get plasticity mode
     * @return Plasticity mode (stdp, hebbian, structural, mixed)
     * @post Plasticity mode accessible
     */
    const std::string& getPlasticityMode() const { return plasticityMode; }
    
    /**
     * @brief Get memory capacity
     * @return Memory capacity
     * @post Memory capacity accessible
     */
    int64_t getMemoryCapacity() const { return memoryCapacity; }
    
    /**
     * @brief Check if neuromodulation is enabled
     * @return true if neuromodulation enabled, false otherwise
     * @post Neuromodulation status accessible
     */
    bool isNeuromodulationEnabled() const { return neuromodulationEnabled; }
    
    /**
     * @brief Get development stage
     * @return Development stage (1-6)
     * @post Development stage accessible
     */
    int64_t getDevelopmentStage() const { return developmentStage; }
    
    /**
     * @brief Get checkpoint interval
     * @return Checkpoint interval
     * @post Checkpoint interval accessible
     */
    int64_t getCheckpointInterval() const { return checkpointInterval; }
    
    /**
     * @brief Check if auto-save is enabled
     * @return true if auto-save enabled, false otherwise
     * @post Auto-save status accessible
     */
    bool isAutoSaveEnabled() const { return autoSaveEnabled; }
    
    /**
     * @brief Initialize default options
     * @post Default options set
     * 
     * @details Sets default values for all options when no command-line
     * arguments specify them
     */
    void initializeDefaultOptions();

private:
    bool helpRequested;
    bool versionRequested;
    bool verbose;
    bool debug;
    int debugLevel;
    bool batchMode;
    bool profilingEnabled;
    bool progressReporting;
    bool validationEnabled;
    bool errorInjectionEnabled;
    bool recoveryTestingEnabled;
    bool statisticsEnabled;
    
    std::string outputFile;
    std::string outputFormat;
    size_t progressInterval;
    std::string configFile;
    std::vector<std::string> batchConfigFiles;
    std::string profilingFile;
    std::string batchProfile;
    std::string errorInjectionType;
    float errorInjectionProbability;
    
    int64_t neuronCount;
    int64_t regionCount;
    double timestep;
    int64_t randomSeed;
    float connectionProbability;
    float stdpLTPWeight;
    float stdpLTDWeight;
    std::string plasticityMode;
    int64_t memoryCapacity;
    bool neuromodulationEnabled;
    int64_t developmentStage;
    int64_t checkpointInterval;
    bool autoSaveEnabled;
    
    void parseBasicArgs(int argc, char** argv, Config& config);
    void parseAdvancedArgs(int argc, char** argv, Config& config);
    void applyConfigOverrides(Config& config) const;
    bool validateArgs() const;
    
    // Suppress warnings about unused private members
    AdvancedArgs(const AdvancedArgs&) = delete;
    AdvancedArgs& operator=(const AdvancedArgs&) = delete;
};

} // namespace nlm