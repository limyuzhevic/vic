#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

struct SystemState {
    PerformanceMetrics performance; // System performance metrics
    LearningCurve learningCurve; // Learning progress over time
    NetworkActivityPattern activityPattern; // Network activity patterns
    std::map<std::string, float> crossSystemCorrelations; // Correlations between systems
    std::vector<std::pair<Timestamp, std::string>> checkpointData; // Checkpoint information
    std::vector<std::pair<Timestamp, double>> resourceUtilization; // Resource utilization over time
    Timestamp timestamp; // Time of measurement
    std::string sessionId; // Session identifier
};

struct SystemData {
    std::vector<SystemState> states; // Time series of system states
    std::vector<Timestamp> timestamps; // Corresponding timestamps
    std::string sessionId; // Session identifier
};

class SystemAnalyzer {
public:
    SystemAnalyzer();
    ~SystemAnalyzer() = default;
    
    // Add system data
    void addData(const SystemData& data);
    
    // Add system state
    void addState(const SystemState& state);
    
    // Analyze system integration
    std::string analyzeSystemIntegration(const SystemData& data) const;
    
    // Calculate system performance
    PerformanceMetrics calculateSystemPerformance(const SystemData& data) const;
    
    // Calculate system stability
    double calculateSystemStability(const SystemData& data) const;
    
    // Analyze cross-system correlations
    std::map<std::string, double> analyzeCrossSystemCorrelations(const SystemData& data) const;
    
    // Analyze checkpoint performance
    double analyzeCheckpointPerformance(const SystemData& data) const;
    
    // Analyze resource utilization
    double analyzeResourceUtilization(const SystemData& data) const;
    
    // Predict system behavior
    SystemData predictSystemBehavior(const SystemData& data, double timeHorizon) const;
    
    // Compare system performance
    double compareSystemPerformance(const SystemData& data1, const SystemData& data2) const;
    
    // Get system data
    const SystemData& getData() const;
    
    // Clear system data
    void clearData();
    
    // Export analysis results
    bool exportResults(const std::string& filename, const std::string& format = "json") const;
    
    // Import analysis results
    bool importResults(const std::string& filename, const std::string& format = "json");
    
    // Generate system analysis report
    std::string generateReport() const;
    
    // Check if analyzer has data
    bool hasData() const;
    
    // Get analysis timestamp
    Timestamp getTimestamp() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm