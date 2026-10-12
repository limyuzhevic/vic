// ExperimentResult - Standardized result structure
#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

struct ExperimentResult {
    // Metadata
    std::string experimentName;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    double duration;
    float progress;
    
    // Status
    bool success;
    std::string statusMessage;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    // Results by category
    std::map<std::string, double> metricResults;
    std::map<std::string, std::string> parameterResults;
    std::vector<std::pair<std::string, double>> timeseries;
    
    // Performance metrics
    double totalReward;
    float averageFiringRate;
    size_t totalSpikes;
    size_t totalSynapses;
    size_t totalNeurons;
    
    // Statistical results
    double meanValue;
    double variance;
    double stdDev;
    double minValue;
    double maxValue;
    
    // Experimental comparisons
    std::map<std::string, double> comparativeResults;
    
    // Checkpoint support
    std::string checkpointFilePath;
    bool checkpointExists;
    
    ExperimentResult()
        : duration(0.0), progress(0.0), success(false), totalReward(0.0),
          averageFiringRate(0.0), totalSpikes(0), totalSynapses(0), 
          totalNeurons(0), meanValue(0.0), variance(0.0), stdDev(0.0),
          minValue(0.0), maxValue(0.0), checkpointExists(false) {}
    
    // Helper methods
    void updateStatistics() {
        if (!timeseries.empty()) {
            std::vector<double> values;
            for (const auto& pair : timeseries) {
                values.push_back(pair.second);
            }
            
            meanValue = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            
            double sqSum = 0.0;
            for (double val : values) {
                sqSum += (val - meanValue) * (val - meanValue);
            }
            variance = sqSum / values.size();
            stdDev = std::sqrt(variance);
            
            minValue = *std::min_element(values.begin(), values.end());
            maxValue = *std::max_element(values.begin(), values.end());
        }
    }
    
    std::string toCSV() const {
        return experimentName + "," + std::to_string(success) + 
               "," + std::to_string(totalReward) + 
               "," + std::to_string(meanValue) + 
               "," + std::to_string(stdDev);
    }
    
    std::string toJSON() const {
        // Simplified JSON generation
        return "{\"experiment\": \"" + experimentName + "\", \"success\": " + 
               std::to_string(success) + ", \"reward\": " + std::to_string(totalReward) + "}";
    }
};

} // namespace nlm
