#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include "../core/Config/Config.hpp"

namespace nlm {

// Batch run configuration
struct BatchRunConfig {
    std::string id;
    std::string description;
    std::map<std::string, std::string> parameters;
    std::string outputDirectory;
    bool parallel;
    size_t maxParallel;
    double timeout;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    BatchRunConfig() : parallel(false), maxParallel(4), timeout(0.0) {}
};

// Experiment result aggregation
struct ResultAggregation {
    std::string metricName;
    std::vector<double> values;
    double mean;
    double stddev;
    double min;
    double max;
    double median;
    
    ResultAggregation() : mean(0.0), stddev(0.0), min(0.0), max(0.0), median(0.0) {}
};

class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    bool createBatch(const std::string& batchConfigFile, const std::string& outputDir);
    bool executeBatch(const std::string& batchId, const std::map<std::string, std::string>& options = {});
    bool monitorBatch(const std::string& batchId, const std::string& outputFile);
    
    bool loadBatchConfiguration(const std::string& filepath);
    bool saveBatchConfiguration(const std::string& filepath) const;
    
    std::vector<std::string> getBatchIds() const;
    std::map<std::string, std::string> getBatchStatus(const std::string& batchId) const;
    
    // Result processing
    bool processBatchResults(const std::string& batchId, const std::string& outputFile);
    bool aggregateBatchResults(const std::string& batchId, const std::string& outputFile);
    
    // Monitoring
    bool monitorBatchProgress(const std::string& batchId, std::ofstream& outputFile);
    bool generateBatchReport(const std::string& batchId, const std::string& outputFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool executeSingleExperiment(const std::string& experimentConfig);
    bool executeParameterSweep(const BatchRunConfig& batchConfig);
    bool executeFixedParameterSet(const BatchRunConfig& batchConfig, const std::map<std::string, std::string>& params);
    
    std::vector<std::map<std::string, std::string>> generateParameterCombinations(
        const std::map<std::string, std::vector<std::string>>& paramRanges);
    
    std::vector<std::map<std::string, std::string>> generateParameterCombinationsRecursive(
        const std::map<std::string, std::vector<std::string>>& params,
        const std::vector<std::string>& keys,
        size_t index,
        std::map<std::string, std::string> current);
    
    bool runExperimentWithParameters(
        const std::map<std::string, std::string>& parameters,
        const std::string& outputDirectory);
    
    bool loadExperimentTemplate(const std::string& templateFile);
    std::map<std::string, std::string> applyParametersToTemplate(
        const std::map<std::string, std::string>& templateParams,
        const std::map<std::string, std::string>& overrideParams);
    
    bool saveResults(const std::string& resultsFile, const std::map<std::string, std::string>& results);
    
    bool aggregateResults(const std::vector<std::string>& resultFiles,
                         std::map<std::string, ResultAggregation>& aggregations);
    
    void updateBatchStatus(const std::string& batchId, const std::string& status,
                          const std::map<std::string, std::string>& progress);
};

} // namespace nlm
