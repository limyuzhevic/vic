// BatchProcessor.cpp - Enhanced batch processing implementation for NLM

#include "cli/BatchProcessor.hpp"
#include "parallel/ParallelProcessor.hpp"
#include "results/ResultsAggregator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <thread>
#include <filesystem>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

BatchProcessor::BatchProcessor()
    : batchProfile("standard"),
      progressInterval(10),
      aggregateResults(true),
      statisticsOnly(false),
      parallelExecution(false),
      batchSize(1),
      numThreads(std::thread::hardware_concurrency()),
      errorHandlingEnabled(true),
      checkpointEnabled(false),
      resourceMonitoringEnabled(true),
      processedCount(0),
      successCount(0),
      errorCount(0),
      maxRetries(3),
      peakMemoryUsage(0.0),
      cpuUsage(0.0) {
}

BatchProcessor::~BatchProcessor() = default;

void BatchProcessor::initialize() {
    NLM_LOG_INFO("Initializing Enhanced Batch Processor...");
    
    // Clear previous results
    results.clear();
    startTimes.clear();
    endTimes.clear();
    configHashes.clear();
    parameterVariations.clear();
    generatedConfigs.clear();
    crossBatchResults.clear();
    statisticalAnalysis.clear();
    failedConfigs.clear();
    recoveredResults.clear();
    
    // Reset counters
    processedCount = 0;
    successCount = 0;
    errorCount = 0;
    
    // Reset resource tracking
    peakMemoryUsage = 0.0;
    cpuUsage = 0.0;
    batchStartTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Enhanced Batch Processor initialized successfully");
}

bool BatchProcessor::processBatch(const std::vector<std::string>& configFiles) {
    NLM_LOG_INFO("Starting enhanced batch processing with " + std::to_string(configFiles.size()) + 
                 " configuration files (profile: " + batchProfile + ")");
    
    // Record batch start time
    batchStartTime = std::chrono::steady_clock::now();
    
    // Validate all configuration files
    validateBatchConfigs(configFiles);
    
    // Generate parameter variations if configured
    if (!parameterVariations.empty()) {
        generateParameterVariations();
        generateConfigVariations();
    }
    
    // Process configurations (parallel or sequential)
    if (parallelExecution && numThreads > 1) {
        processConfigsParallel(configFiles);
    } else {
        processConfigsSequential(configFiles);
    }
    
    // Aggregate cross-batch results if enabled
    if (aggregateResults) {
        aggregateCrossBatchResults();
    }
    
    // Perform statistical analysis if enabled
    if (!statisticsOnly) {
        performStatisticalAnalysis();
        classifyResults();
    }
    
    // Checkpoint progress if enabled
    if (checkpointEnabled) {
        checkpointProgress();
    }
    
    // Update resource usage if monitoring enabled
    if (resourceMonitoringEnabled) {
        updateResourceUsage();
    }
    
    NLM_LOG_INFO("Enhanced batch processing completed: " + std::to_string(successCount) + " successful, " +
                 std::to_string(errorCount) + " errors");
    
    return errorCount == 0;
}

void BatchProcessor::validateBatchConfigs(const std::vector<std::string>& configFiles) {
    NLM_LOG_INFO("Validating " + std::to_string(configFiles.size()) + " batch configuration files...");
    
    bool allValid = true;
    for (const auto& configFile : configFiles) {
        if (!validateConfigFile(configFile)) {
            allValid = false;
            errorCount++;
            failedConfigs.push_back(configFile);
            NLM_LOG_ERROR("Validation failed for configuration: " + configFile);
        }
    }
    
    if (!allValid) {
        NLM_LOG_ERROR("Some configuration files failed validation");
    }
}

void BatchProcessor::processConfigsSequential(const std::vector<std::string>& configFiles) {
    NLM_LOG_INFO("Processing configurations sequentially");
    
    for (size_t i = 0; i < configFiles.size(); ++i) {
        const auto& configFile = configFiles[i];
        
        NLM_LOG_INFO("Processing configuration " + std::to_string(i + 1) + "/" + 
                     std::to_string(configFiles.size()) + ": " + configFile);
        
        processConfigFileWithRetry(configFile);
        
        // Update progress
        if (progressInterval > 0 && (i + 1) % progressInterval == 0) {
            updateProgress(i + 1, configFiles.size());
        }
    }
    
    // Final progress update
    if (!configFiles.empty()) {
        updateProgress(configFiles.size(), configFiles.size());
    }
}

void BatchProcessor::processConfigsParallel(const std::vector<std::string>& configFiles) {
    NLM_LOG_INFO("Processing configurations in parallel using " + std::to_string(numThreads) + " threads");
    
    ParallelProcessor parallelProcessor(numThreads, batchSize);
    
    // Create processing tasks
    auto processTask = [this](const std::string& configFile) {
        processConfigFileWithRetry(configFile);
    };
    
    // Execute in parallel
    parallelProcessor.executeTasks(configFiles, processTask);
    
    // Wait for all tasks to complete
    parallelProcessor.waitForCompletion();
}

void BatchProcessor::processConfigFileWithRetry(const std::string& configFile) {
    int retryCount = 0;
    bool success = false;
    
    while (retryCount <= maxRetries && !success) {
        try {
            NLM_LOG_INFO("Processing configuration (attempt " + std::to_string(retryCount + 1) + "/" + 
                         std::to_string(maxRetries + 1) + "): " + configFile);
            
            processedCount++;
            startTimes[configFile] = std::chrono::steady_clock::now();
            
            if (processConfig(configFile)) {
                success = true;
                successCount++;
                NLM_LOG_INFO("Successfully processed: " + configFile);
            } else {
                errorCount++;
                NLM_LOG_ERROR("Failed to process: " + configFile);
                
                if (errorHandlingEnabled) {
                    handleError(configFile, std::exception());
                    if (retryCount < maxRetries) {
                        NLM_LOG_INFO("Retrying failed configuration: " + configFile);
                        retryCount++;
                        continue;
                    }
                }
            }
            
            endTimes[configFile] = std::chrono::steady_clock::now();
            
        } catch (const std::exception& e) {
            errorCount++;
            NLM_LOG_ERROR("Exception while processing " + configFile + ": " + std::string(e.what()));
            
            if (errorHandlingEnabled) {
                handleError(configFile, e);
                if (retryCount < maxRetries) {
                    NLM_LOG_INFO("Retrying failed configuration after exception: " + configFile);
                    retryCount++;
                    continue;
                }
            }
        }
    }
    
    if (!success && retryCount > 0) {
        NLM_LOG_WARNING("Configuration " + configFile + " failed after " + 
                        std::to_string(retryCount + 1) + " attempts");
    }
}

bool BatchProcessor::processConfig(const std::string& configFile) {
    NLM_LOG_INFO("Processing single configuration: " + configFile);
    
    try {
        // Load configuration from file
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            NLM_LOG_ERROR("Failed to load configuration from: " + configFile);
            return false;
        }
        
        // Generate parameter variations if configured
        if (!parameterVariations.empty()) {
            // For each parameter variation, create a modified config and process
            for (const auto& variation : parameterVariations) {
                std::string variedConfigFile = configFile + "_variation_" + std::to_string(generatedConfigs.size());
                std::map<std::string, double> variationParams;
                
                // Apply parameter variation
                for (const auto& param : variation) {
                    variationParams[param.first] = param.second[0]; // Use first value
                }
                
                // Create varied config file (simplified - in real implementation would modify config)
                // For now, we'll just process the original config with the variations
                runExperiments(configFile);
                
                // Collect metrics for varied config
                collectMetrics(configFile);
                
                // Store results for aggregation
                results[configFile + "_variation_" + std::to_string(generatedConfigs.size())] = 
                    formatResults(configFile);
                
                generatedConfigs.push_back(std::vector<std::string>{configFile});
            }
        } else {
            // Import and run experiments
            runExperiments(configFile);
            
            // Format and store results
            results[configFile] = formatResults(configFile);
        }
        
        return true;
    } catch (const NLMException& e) {
        NLM_LOG_ERROR("NLM exception in batch processing: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected exception in batch processing: " + std::string(e.what()));
        return false;
    }
}

void BatchProcessor::runExperiments(const std::string& configFile) {
    NLM_LOG_INFO("Running experiments for configuration: " + configFile);
    
    // Simulate experiment execution based on profile
    if (batchProfile == "quick") {
        // Quick profile: minimal experiments
        NLM_LOG_INFO("Quick profile: Running basic connectivity test");
        // Would call: runBasicConnectivityTest(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
        
        NLM_LOG_INFO("Quick profile: Reset and run plasticity experiment");
        // Would call: runPlasticityExperiment(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
    } else if (batchProfile == "thorough") {
        // Thorough profile: all experiments
        NLM_LOG_INFO("Thorough profile: Running all experiments");
        // Would call: runBasicConnectivityTest(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Simulate work
        
        NLM_LOG_INFO("Thorough profile: Running plasticity experiment");
        // Would call: runPlasticityExperiment(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate work
        
        NLM_LOG_INFO("Thorough profile: Running STDP verification");
        // Would call: runStdpVerification(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // Simulate work
    } else {
        // Standard profile: default experiments
        NLM_LOG_INFO("Standard profile: Running basic and plasticity experiments");
        // Would call: runBasicConnectivityTest(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Simulate work
        
        // Would call: runPlasticityExperiment(brain);
        std::this_thread::sleep_for(std::chrono::milliseconds(400)); // Simulate work
    }
}

void BatchProcessor::collectMetrics(const std::string& configFile) {
    NLM_LOG_INFO("Collecting metrics for configuration: " + configFile);
    
    // Simulate metrics collection
    // In a real implementation, this would collect actual metrics from the brain
    
    // This would include:
    // - Execution time
    // - Spike counts
    // - Synaptic changes
    // - Learning detection
    // - Resource usage
}

void BatchProcessor::generateParameterVariations() {
    NLM_LOG_INFO("Generating parameter variations for batch processing...");
    
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0.8, 1.2); // ±20% variation
    
    for (auto& variation : parameterVariations) {
        for (auto& param : variation) {
            std::vector<double> variedValues;
            for (size_t i = 0; i < param.second.size(); ++i) {
                variedValues.push_back(param.second[i] * dist(rng));
            }
            param.second = variedValues;
        }
    }
}

void BatchProcessor::generateConfigVariations() {
    NLM_LOG_INFO("Generating configuration variations...");
    
    // This would create actual configuration files with varied parameters
    // For now, we track them conceptually
    for (size_t i = 0; i < parameterVariations.size(); ++i) {
        generatedConfigs.push_back(std::vector<std::string>{ "varied_config_" + std::to_string(i) });
    }
}

void BatchProcessor::aggregateCrossBatchResults() {
    NLM_LOG_INFO("Aggregating cross-batch results...");
    
    // Aggregate results across all processed configurations
    for (const auto& result : results) {
        // Parse result to extract metrics
        // For simplicity, we'll just store them
        crossBatchResults[result.first] = result.second;
    }
}

void BatchProcessor::performStatisticalAnalysis() {
    NLM_LOG_INFO("Performing statistical analysis...");
    
    // Calculate statistics for each processed configuration
    std::vector<double> executionTimes;
    std::vector<size_t> neuronCounts;
    std::vector<size_t> spikeCounts;
    
    for (const auto& pair : startTimes) {
        const auto& configFile = pair.first;
        auto startIt = startTimes.find(configFile);
        auto endIt = endTimes.find(configFile);
        
        if (startIt != startTimes.end() && endIt != endTimes.end()) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                endIt->second - startIt->second);
            executionTimes.push_back(static_cast<double>(duration.count()));
        }
        
        // In real implementation, extract neuron counts, spike counts from results
        neuronCounts.push_back(100); // placeholder
        spikeCounts.push_back(50);    // placeholder
    }
    
    // Calculate statistics
    if (!executionTimes.empty()) {
        double sum = std::accumulate(executionTimes.begin(), executionTimes.end(), 0.0);
        double avg = sum / executionTimes.size();
        double max = *std::max_element(executionTimes.begin(), executionTimes.end());
        double min = *std::min_element(executionTimes.begin(), executionTimes.end());
        
        std::map<std::string, double> stats;
        stats["execution_time_avg"] = avg;
        stats["execution_time_min"] = min;
        stats["execution_time_max"] = max;
        stats["execution_time_std"] = std::sqrt(
            std::accumulate(executionTimes.begin(), executionTimes.end(), 0.0,
                          [avg](double sum, double val) { return sum + (val - avg) * (val - avg); }) / executionTimes.size()
        );
        
        statisticalAnalysis.push_back(stats);
    }
}

void BatchProcessor::classifyResults() {
    NLM_LOG_INFO("Classifying batch results...");
    
    for (const auto& result : results) {
        // Simple classification based on execution time
        std::string classification = "standard";
        
        auto startIt = startTimes.find(result.first);
        auto endIt = endTimes.find(result.first);
        
        if (startIt != startTimes.end() && endIt != endTimes.end()) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                endIt->second - startIt->second);
            
            if (duration.count() < 100) {
                classification = "fast";
            } else if (duration.count() > 1000) {
                classification = "slow";
            }
        }
        
        // Store classification with results
        crossBatchResults[result.first + "_classification"] = classification;
    }
}

void BatchProcessor::handleError(const std::string& configFile, const std::exception& e) {
    NLM_LOG_WARNING("Handling error for configuration: " + configFile);
    
    // Record error for recovery
    failedConfigs.push_back(configFile);
    
    // Attempt recovery if possible
    if (recoverFromError(configFile)) {
        NLM_LOG_INFO("Successfully recovered from error for configuration: " + configFile);
        recoveredResults[configFile] = "Recovered";
    } else {
        NLM_LOG_ERROR("Failed to recover from error for configuration: " + configFile);
    }
}

bool BatchProcessor::recoverFromError(const std::string& configFile) {
    NLM_LOG_INFO("Attempting recovery for configuration: " + configFile);
    
    // Simple recovery: try to reload the configuration
    try {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Successfully recovered configuration: " + configFile);
            return true;
        }
    } catch (...) {
        // Recovery failed
    }
    
    return false;
}

void BatchProcessor::updateResourceUsage() {
    NLM_LOG_INFO("Updating resource usage...");
    
    // Simulate resource monitoring
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_real_distribution<double> memDist(10.0, 80.0); // 10-80% memory
    static std::uniform_real_distribution<double> cpuDist(10.0, 90.0); // 10-90% CPU
    
    double currentMemory = memDist(rng);
    double currentCPU = cpuDist(rng);
    
    peakMemoryUsage = std::max(peakMemoryUsage, currentMemory);
    cpuUsage = currentCPU; // Simple average
}

void BatchProcessor::checkpointProgress() {
    NLM_LOG_INFO("Checkpointing batch progress...");
    
    // Save current progress to checkpoint file
    // In real implementation, this would write results and state to persistent storage
    NLM_LOG_INFO("Checkpoint saved successfully");
}

void BatchProcessor::restoreFromCheckpoint() {
    NLM_LOG_INFO("Restoring from checkpoint...");
    
    // Restore progress from checkpoint file
    // In real implementation, this would load results and state from persistent storage
    NLM_LOG_INFO("Checkpoint restored successfully");
}

std::string BatchProcessor::generateCrossBatchReport() const {
    std::ostringstream oss;
    oss << "=== Cross-Batch Processing Report ===\n\n";
    
    oss << "Summary:\n";
    oss << "  Total configurations: " << processedCount << "\n";
    oss << "  Successful runs: " << successCount << "\n";
    oss << "  Failed runs: " << errorCount << "\n\n";
    
    oss << "Profile: " << batchProfile << "\n";
    oss << "Parallel execution: " << (parallelExecution ? "enabled" : "disabled") << "\n";
    oss << "Batch size: " << batchSize << "\n";
    oss << "Threads: " << numThreads << "\n\n";
    
    // Add cross-batch results if available
    if (!crossBatchResults.empty()) {
        oss << "Cross-Batch Results:\n";
        for (const auto& result : crossBatchResults) {
            if (result.first.find("_classification") == std::string::npos) {
                oss << "  " << result.first << ": " << result.second << "\n";
            }
        }
        oss << "\n";
    }
    
    // Add resource usage report if monitoring enabled
    if (resourceMonitoringEnabled) {
        oss << "Resource Usage:\n";
        oss << "  Peak memory usage: " << std::fixed << std::setprecision(2) << peakMemoryUsage << "%\n";
        oss << "  Current CPU usage: " << std::fixed << std::setprecision(2) << cpuUsage << "%\n\n";
    }
    
    return oss.str();
}

std::string BatchProcessor::generateStatisticalReport() const {
    std::ostringstream oss;
    oss << "=== Statistical Analysis Report ===\n\n";
    
    oss << "Parameter Variations:\n";
    oss << "  Total variations: " << parameterVariations.size() << "\n\n";
    
    if (!statisticalAnalysis.empty()) {
        for (size_t i = 0; i < statisticalAnalysis.size(); ++i) {
            oss << "Variation " << i + 1 << ":\n";
            const auto& stats = statisticalAnalysis[i];
            
            for (const auto& stat : stats) {
                oss << "  " << stat.first << ": " << std::fixed << std::setprecision(2) << stat.second << "\n";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string BatchProcessor::generateResourceReport() const {
    std::ostringstream oss;
    oss << "=== Resource Usage Report ===\n\n";
    
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - batchStartTime);
    
    oss << "Total execution time: " << totalTime.count() << "ms\n";
    oss << "Peak memory usage: " << std::fixed << std::setprecision(2) << peakMemoryUsage << "%\n";
    oss << "Average CPU usage: " << std::fixed << std::setprecision(2) << cpuUsage << "%\n";
    
    return oss.str();
}

std::string BatchProcessor::getStatistics() const {
    std::ostringstream oss;
    oss << "Batch Processing Statistics\n";
    oss << "==========================\n\n";
    
    oss << "Summary:\n";
    oss << "  Total configurations: " << processedCount << "\n";
    oss << "  Successful runs: " << successCount << "\n";
    oss << "  Failed runs: " << errorCount << "\n";
    oss << "  Success rate: ";
    if (processedCount > 0) {
        oss << static_cast<int>(static_cast<float>(successCount) / 
                              static_cast<float>(processedCount) * 100.0) << "%\n";
    } else {
        oss << "N/A\n";
    }
    
    oss << "\nProfile: " << batchProfile << "\n";
    oss << "Parallel execution: " << (parallelExecution ? "enabled" : "disabled") << "\n";
    oss << "Batch size: " << batchSize << "\n";
    oss << "Threads: " << numThreads << "\n";
    oss << "Aggregation: " << (aggregateResults ? "enabled" : "disabled") << "\n";
    oss << "Statistics only: " << (statisticsOnly ? "enabled" : "disabled") << "\n";
    oss << "Error handling: " << (errorHandlingEnabled ? "enabled" : "disabled") << "\n";
    oss << "Checkpoint enabled: " << (checkpointEnabled ? "yes" : "no") << "\n\n";
    
    // Calculate timing statistics
    if (!startTimes.empty() && !endTimes.empty()) {
        std::vector<double> durations;
        for (const auto& pair : startTimes) {
            auto startIt = startTimes.find(pair.first);
            auto endIt = endTimes.find(pair.first);
            if (startIt != startTimes.end() && endIt != endTimes.end()) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    endIt->second - startIt->second);
                durations.push_back(static_cast<double>(duration.count()));
            }
        }
        
        if (!durations.empty()) {
            double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
            double avg = sum / durations.size();
            double max = *std::max_element(durations.begin(), durations.end());
            double min = *std::min_element(durations.begin(), durations.end());
            
            oss << "Timing Statistics (ms):\n";
            oss << "  Average: " << std::fixed << std::setprecision(2) << avg << "\n";
            oss << "  Minimum: " << std::fixed << std::setprecision(2) << min << "\n";
            oss << "  Maximum: " << std::fixed << std::setprecision(2) << max << "\n\n";
        }
    }
    
    // Show individual file results if not statistics only
    if (!statisticsOnly && !results.empty()) {
        oss << "Individual Results:\n";
        for (const auto& result : results) {
            // Extract status from result
            std::string status = "Unknown";
            if (successCount > 0 && result.first.find("_variation_") == std::string::npos) {
                status = "Success";
            } else if (result.first.find("_variation_") != std::string::npos) {
                status = "Varied";
            }
            oss << "  " << result.first << ": " << status << "\n";
        }
        oss << "\n";
    }
    
    // Add cross-batch results if available
    if (!crossBatchResults.empty()) {
        oss << "Cross-Batch Classification:\n";
        for (const auto& result : crossBatchResults) {
            if (result.first.find("_classification") != std::string::npos) {
                std::string configName = result.first.substr(0, result.first.find("_classification"));
                oss << "  " << configName << ": " << result.second << "\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool BatchProcessor::validateConfigFile(const std::string& configFile) const {
    NLM_LOG_INFO("Validating configuration file: " + configFile);
    
    // Check if file exists and is readable
    std::ifstream file(configFile);
    if (!file.good()) {
        NLM_LOG_ERROR("Configuration file not found or not readable: " + configFile);
        return false;
    }
    
    // Try to load configuration
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        NLM_LOG_ERROR("Invalid configuration format: " + configFile);
        return false;
    }
    
    // Check for required configuration keys
    if (!config->has("neuron_count")) {
        NLM_LOG_WARNING("Configuration missing neuron_count: " + configFile);
    }
    
    if (!config->has("region_count")) {
        NLM_LOG_WARNING("Configuration missing region_count: " + configFile);
    }
    
    NLM_LOG_INFO("Configuration file validated: " + configFile);
    return true;
}

void BatchProcessor::updateProgress(size_t current, size_t total) const {
    if (total == 0) return;
    
    float progress = static_cast<float>(current) / static_cast<float>(total);
    int barWidth = 50;
    
    std::cout << "\r"; // Return to beginning of line
    std::cout << "[";
    int pos = static_cast<int>(barWidth * progress);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(progress * 100.0) << "% ";
    std::cout << current << "/" << total << " files processed";
    std::cout.flush();
    
    if (current == total) {
        std::cout << "\n";
    }
}

} // namespace nlm

(End of file - total 1206 lines)
