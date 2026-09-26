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

struct LearningMetrics {
    double performance; // Performance metric (0-1)
    double exploration; // Exploration level (0-1)
    double exploitation; // Exploitation level (0-1)
    double curiosity; // Curiosity level (0-1)
    double learningRate; // Learning rate
    double convergenceRate; // Convergence rate
    double stability; // Stability measure
    Timestamp timestamp; // Time of measurement
    std::string policy; // Decision-making policy
    std::map<std::string, double> additionalMetrics; // Additional metrics
};

struct LearningCurve {
    std::vector<Timestamp> timePoints;
    std::vector<float> performanceValues; // Performance metric values
    std::vector<float> explorationValues; // Exploration levels
    std::vector<float> exploitationValues; // Exploitation levels
    std::vector<float> curiosityValues; // Curiosity levels
    std::string metricName; // Name of the metric being tracked
};

struct LearningData {
    std::vector<LearningMetrics> metrics; // Time series of learning metrics
    std::vector<Timestamp> timestamps; // Corresponding timestamps
    std::string sessionId; // Session identifier
    std::string taskId; // Task identifier
};

class LearningAnalyzer {
public:
    LearningAnalyzer();
    ~LearningAnalyzer() = default;
    
    // Add learning data
    void addData(const LearningData& data);
    
    // Add learning metrics
    void addMetrics(const LearningMetrics& metrics);
    
    // Calculate learning curve
    LearningCurve calculateLearningCurve(const LearningData& data) const;
    
    // Calculate convergence rate
    double calculateConvergenceRate(const LearningData& data) const;
    
    // Calculate stability
    double calculateStability(const LearningData& data) const;
    
    // Analyze exploration vs. exploitation
    double analyzeExplorationExploitation(const LearningData& data) const;
    
    // Analyze curiosity dynamics
    double analyzeCuriosityDynamics(const LearningData& data) const;
    
    // Analyze policy performance
    double analyzePolicyPerformance(const LearningData& data, const std::string& policy) const;
    
    // Predict learning progress
    LearningCurve predictLearningProgress(const LearningData& data, double timeHorizon) const;
    
    // Identify learning phases
    std::vector<std::pair<std::string, std::pair<Timestamp, Timestamp>>> identifyLearningPhases(const LearningData& data) const;
    
    // Compare learning performance
    double compareLearningPerformance(const LearningData& data1, const LearningData& data2) const;
    
    // Get learning data
    const LearningData& getData() const;
    
    // Clear learning data
    void clearData();
    
    // Export analysis results
    bool exportResults(const std::string& filename, const std::string& format = "json") const;
    
    // Import analysis results
    bool importResults(const std::string& filename, const std::string& format = "json");
    
    // Generate learning report
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