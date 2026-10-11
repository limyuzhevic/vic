// BatchProcessor.hpp - Batch processing capabilities for NLM
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <unordered_map>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * BatchProcessor class - Batch processing capabilities for NLM
 * 
 * @brief Provides comprehensive batch processing functionality including:
 *        - Multiple configuration files processing
 *        - Batch mode with different execution profiles
 *        - Output aggregation for multiple runs
 *        - Statistical analysis aggregation
 *        - Error handling and recovery
 *        - Progress tracking for batch operations
 * 
 * Features:
 * - Process multiple configuration files in batch mode
 * - Support for different execution profiles (quick, standard, thorough)
 * - Aggregate results from multiple runs
 * - Statistical analysis and metrics collection
 * - Progress reporting for long batch operations
 * - Error handling with recovery options
 * - Checkpoint and restart capabilities
 * - Resource monitoring and management
 * 
 * @note Integrates with existing Config system for configuration management
 * @note Uses Enhanced logging for batch operation tracking
 * @see CLIEngine for batch mode integration
 * @see Profiler for performance monitoring
 */
class BatchProcessor {
public:
    /**
     * @brief BatchProcessor constructor
     * @details Initializes batch processor with default settings
     * @post Processor ready for batch operations
     */
    BatchProcessor();
    
    /**
     * @brief BatchProcessor destructor
     * @details Cleans up batch processor resources
     * @post All resources released
     */
    ~BatchProcessor();
    
    /**
     * @brief Initialize batch processor
     * @post Processor initialized and ready for operations
     */
    void initialize();
    
    /**
     * @brief Process configuration files in batch mode
     * @param configFiles Vector of configuration file paths
     * @return true if processing successful, false if error
     * @post All configurations processed and results aggregated
     * 
     * @details Processes multiple configuration files:
     * 1. Validates all configuration files
     * 2. Loads and processes each configuration
     * 3. Runs experiments for each configuration
     * 4. Collects metrics and statistics
     * 5. Aggregates results
     * 
     * @warning Files must be readable and in valid configuration format
     * @see processConfig() for single configuration processing
     * @see aggregateResults() for result aggregation
     */
    bool processBatch(const std::vector<std::string>& configFiles);
    
    /**
     * @brief Process single configuration file
     * @param configFile Configuration file path
     * @return true if processing successful, false if error
     * @post Configuration processed and results collected
     * 
     * @details Processes a single configuration file:
     * 1. Loads configuration from file
     * 2. Runs experiments
     * 3. Collects metrics
     * 4. Stores results for aggregation
     */
    bool processConfig(const std::string& configFile);
    
    /**
     * @brief Get batch statistics
     * @return Batch statistics as string
     * @post Statistics formatted for output
     * 
     * @details Provides comprehensive statistics including:
     * - Total number of configurations processed
     * - Number of successful and failed runs
     * - Average and maximum execution times
     * - Average and maximum neuron counts
     * - Average and maximum spike counts
     * - Learning detection statistics
     * - Error statistics
     */
    std::string getStatistics() const;
    
    /**
     * @brief Get all batch results
     * @return Map of configuration file to results
     * @post All results accessible
     */
    const std::map<std::string, std::string>& getResults() const { return results; }
    
    /**
     * @brief Set batch profile
     * @param profile Profile name (quick, standard, thorough)
     * @post Profile set for batch processing
     */
    void setProfile(const std::string& profile) { batchProfile = profile; }
    
    /**
     * @brief Set progress reporting interval
     * @param interval Progress reporting interval
     * @post Progress interval set
     */
    void setProgressInterval(size_t interval) { progressInterval = interval; }
    
    /**
     * @brief Set aggregation options
     * @param aggregate Aggregate results flag
     * @post Aggregation option set
     */
    void setAggregateResults(bool aggregate) { aggregateResults = aggregate; }
    
    /**
     * @brief Set statistics-only mode
     * @param statsOnly Statistics only flag
     * @post Statistics-only mode set
     */
    void setStatisticsOnly(bool statsOnly) { statisticsOnly = statsOnly; }
    
    /**
     * @brief Check if aggregate results
     * @return true if results will be aggregated, false otherwise
     * @post Aggregation status accessible
     */
    bool isAggregateResults() const { return aggregateResults; }
    
    /**
     * @brief Check if statistics-only mode
     * @return true if statistics-only mode enabled, false otherwise
     * @post Statistics-only mode status accessible
     */
    bool isStatisticsOnly() const { return statisticsOnly; }
    
    /**
     * @brief Enable parameter variation generation
     * @param variations Parameter variations configuration
     * @post Parameter variation generation enabled
     */
    void setParameterVariations(const std::vector<std::map<std::string, std::vector<double>>>& variations) { parameterVariations = variations; }
    
    /**
     * @brief Enable parallel execution
     * @param enable Enable parallel execution flag
     * @post Parallel execution option set
     */
    void setParallelExecution(bool enable) { parallelExecution = enable; }
    
    /**
     * @brief Set batch size for processing
     * @param size Batch size
     * @post Batch size set
     */
    void setBatchSize(size_t size) { batchSize = size; }
    
    /**
     * @brief Set number of threads for parallel processing
     * @param threads Number of threads
     * @post Thread count set
     */
    void setNumThreads(size_t threads) { numThreads = threads; }
    
    /**
     * @brief Get number of processed configurations
     * @return Number of processed configurations
     * @post Count accessible
     */
    size_t getProcessedCount() const { return processedCount; }
    
    /**
     * @brief Get number of successful runs
     * @return Number of successful runs
     * @post Count accessible
     */
    size_t getSuccessCount() const { return successCount; }
    
    /**
     * @brief Get number of failed runs
     * @return Number of failed runs
     * @post Count accessible
     */
    size_t getErrorCount() const { return errorCount; }
    
    /**
     * @brief Set error handling and recovery enabled
     * @param enable Enable error handling and recovery flag
     * @post Error handling and recovery option set
     */
    void setErrorHandling(bool enable) { errorHandlingEnabled = enable; }
    
    /**
     * @brief Set checkpoint and restart enabled
     * @param enable Enable checkpoint and restart flag
     * @post Checkpoint and restart option set
     */
    void setCheckpointEnabled(bool enable) { checkpointEnabled = enable; }
    
    /**
     * @brief Set resource monitoring enabled
     * @param enable Enable resource monitoring flag
     * @post Resource monitoring option set
     */
    void setResourceMonitoring(bool enable) { resourceMonitoringEnabled = enable; }

private:
    // Core batch processing state
    std::map<std::string, std::string> results;
    std::map<std::string, std::chrono::steady_clock::time_point> startTimes;
    std::map<std::string, std::chrono::steady_clock::time_point> endTimes;
    std::map<std::string, std::string> configHashes;
    
    // Configuration and processing settings
    std::string batchProfile;
    size_t progressInterval;
    bool aggregateResults;
    bool statisticsOnly;
    bool parallelExecution;
    size_t batchSize;
    size_t numThreads;
    bool errorHandlingEnabled;
    bool checkpointEnabled;
    bool resourceMonitoringEnabled;
    size_t processedCount;
    size_t successCount;
    size_t errorCount;
    
    // Advanced features
    std::vector<std::map<std::string, std::vector<double>>> parameterVariations;
    std::vector<std::vector<std::string>> generatedConfigs;
    std::map<std::string, std::string> crossBatchResults;
    std::vector<std::map<std::string, double>> statisticalAnalysis;
    
    // Error handling and recovery
    std::vector<std::string> failedConfigs;
    std::map<std::string, std::string> recoveredResults;
    size_t maxRetries;
    
    // Resource management
    double peakMemoryUsage;
    double cpuUsage;
    std::chrono::steady_clock::time_point batchStartTime;
    
    void runExperiments(const std::string& configFile);
    void collectMetrics(const std::string& configFile);
    std::string formatResults(const std::string& configFile) const;
    bool validateConfigFile(const std::string& configFile) const;
    void updateProgress(size_t current, size_t total) const;
    
    // Core processing methods
    void validateBatchConfigs(const std::vector<std::string>& configFiles);
    void processConfigFiles(const std::vector<std::string>& configFiles);
    void processConfigFileWithRetry(const std::string& configFile);
    
    // Advanced features methods
    void generateParameterVariations();
    void generateConfigVariations();
    void aggregateCrossBatchResults();
    void performStatisticalAnalysis();
    void classifyResults();
    
    // Error handling methods
    void handleError(const std::string& configFile, const std::exception& e);
    bool recoverFromError(const std::string& configFile);
    
    // Parallel processing methods
    void processConfigsParallel(const std::vector<std::string>& configFiles);
    void processConfigsSequential(const std::vector<std::string>& configFiles);
    
    // Resource management methods
    void updateResourceUsage();
    void checkpointProgress();
    void restoreFromCheckpoint();
    
    // Output and reporting methods
    std::string generateCrossBatchReport() const;
    std::string generateStatisticalReport() const;
    std::string generateResourceReport() const;
    
    // Suppress warnings about unused private members
    BatchProcessor(const BatchProcessor&) = delete;
    BatchProcessor& operator=(const BatchProcessor&) = delete;
};

} // namespace nlm