// MetricsCollector.hpp - Comprehensive metrics collection for NLM test suite
#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <numeric>
#include "TestFramework.hpp"
#include "TestUtils.hpp"

namespace nlm {

// Performance metrics for system components
struct PerformanceMetrics {
    // Time metrics
    double setupTime;      // Time to set up the test
    double executionTime;  // Time to run the test
    double teardownTime;   // Time to clean up the test
    double totalTime;      // Total time for the test
    
    // Resource metrics
    size_t memoryUsage;    // Peak memory usage (if available)
    size_t cpuTime;        // CPU time used (if available)
    
    // Statistical metrics
    double throughput;     // Operations per second
    double efficiency;     // Performance relative to baseline
    
    PerformanceMetrics()
        : setupTime(0.0), executionTime(0.0), teardownTime(0.0), totalTime(0.0),
          memoryUsage(0), cpuTime(0), throughput(0.0), efficiency(0.0) {}
};

// Behavioral metrics for learning and memory tests
struct BehavioralMetrics {
    // Memory metrics
    double memoryRetention;      // How well memory is retained over time
    double memoryStability;      // Stability of memory performance
    double memoryCapacity;       // Maximum memory capacity achieved
    double memoryDecayRate;       // Rate of memory decay
    
    // Learning metrics
    double learningRate;         // Speed of learning
    double learningCurve;        // Learning progress over time
    double plateauReached;       // Whether learning plateaued
    double transferAbility;      // Ability to transfer learning to new tasks
    
    // Prediction metrics
    double predictionAccuracy;   // Accuracy of predictions
    double predictionConsistency; // Consistency of predictions over time
    double predictionError;      // Average prediction error
    double predictionConfidence;  // Confidence in predictions
    
    // Neuromodulation metrics
    double neuromodulationEfficiency; // Efficiency of neuromodulatory signaling
    double rewardSensitivity;         // Sensitivity to reward signals
    double noveltyResponse;          // Response to novelty
    
    // Development metrics
    double developmentalProgress;    // Progress through developmental stages
    double criticalPeriod;            // Effectiveness of critical period learning
    double maturationRate;            // Rate of maturation
    
    // Social metrics (for multi-agent tests)
    double socialLearning;           // Effectiveness of social learning
    double imitationAccuracy;        // Accuracy of imitation
    double coordination;             // Coordination with other agents
    
    BehavioralMetrics()
        : memoryRetention(0.0), memoryStability(0.0), memoryCapacity(0.0), memoryDecayRate(0.0),
          learningRate(0.0), learningCurve(0.0), plateauReached(0.0), transferAbility(0.0),
          predictionAccuracy(0.0), predictionConsistency(0.0), predictionError(0.0), predictionConfidence(0.0),
          neuromodulationEfficiency(0.0), rewardSensitivity(0.0), noveltyResponse(0.0),
          developmentalProgress(0.0), criticalPeriod(0.0), maturationRate(0.0),
          socialLearning(0.0), imitationAccuracy(0.0), coordination(0.0) {}
};

// System metrics for integration tests
struct SystemMetrics {
    // Brain system metrics
    double brainPerformance;       // Overall brain performance
    double neuralEfficiency;       // Efficiency of neural processing
    double networkStability;       // Stability of neural network
    double integrationQuality;     // Quality of system integration
    
    // Component interaction metrics
    double feedbackLoop;           // Effectiveness of feedback loops
    double modularity;             // Modularity of systems
    double adaptability;           // Ability to adapt to changes
    double robustness;             // Robustness to perturbations
    
    // Scalability metrics
    double scalability;            // Performance scaling with system size
    double resourceUtilization;    // Efficiency of resource usage
    
    SystemMetrics()
        : brainPerformance(0.0), neuralEfficiency(0.0), networkStability(0.0), integrationQuality(0.0),
          feedbackLoop(0.0), modularity(0.0), adaptability(0.0), robustness(0.0),
          scalability(0.0), resourceUtilization(0.0) {}
};

// Comprehensive metrics collection for a single test run
class MetricsCollector {
private:
    struct TestMetrics {
        std::string testName;
        TestMetrics(const std::string& name) : testName(name) {}
    };
    
    std::vector<TestMetrics> testMetrics;
    std::map<std::string, PerformanceMetrics> performanceData;
    std::map<std::string, BehavioralMetrics> behavioralData;
    std::map<std::string, SystemMetrics> systemData;
    std::vector<std::pair<std::string, std::vector<double>>> timeseriesData;
    
    bool verbose;
    
public:
    MetricsCollector(bool verboseOutput = false) : verbose(verboseOutput) {}
    
    // Start collecting metrics for a test
    void startTest(const std::string& testName) {
        testMetrics.emplace_back(testName);
    }
    
    // Stop collecting metrics for a test
    void stopTest() {
        if (!testMetrics.empty()) {
            testMetrics.pop_back();
        }
    }
    
    // Record performance metrics
    void recordPerformance(const std::string& testName, const PerformanceMetrics& metrics) {
        performanceData[testName] = metrics;
    }
    
    // Record behavioral metrics
    void recordBehavioral(const std::string& testName, const BehavioralMetrics& metrics) {
        behavioralData[testName] = metrics;
    }
    
    // Record system metrics
    void recordSystem(const std::string& testName, const SystemMetrics& metrics) {
        systemData[testName] = metrics;
    }
    
    // Record timeseries data (e.g., learning curves)
    void recordTimeseries(const std::string& name, const std::vector<double>& data) {
        timeseriesData.emplace_back(name, data);
    }
    
    // Get performance metrics for a test
    PerformanceMetrics getPerformance(const std::string& testName) const {
        auto it = performanceData.find(testName);
        if (it != performanceData.end()) {
            return it->second;
        }
        return PerformanceMetrics();
    }
    
    // Get behavioral metrics for a test
    BehavioralMetrics getBehavioral(const std::string& testName) const {
        auto it = behavioralData.find(testName);
        if (it != behavioralData.end()) {
            return it->second;
        }
        return BehavioralMetrics();
    }
    
    // Get system metrics for a test
    SystemMetrics getSystem(const std::string& testName) const {
        auto it = systemData.find(testName);
        if (it != systemData.end()) {
            return it->second;
        }
        return SystemMetrics();
    }
    
    // Get all test names
    std::vector<std::string> getTestNames() const {
        std::vector<std::string> names;
        for (const auto& pair : performanceData) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Generate comprehensive report
    std::string generateReport() const {
        std::stringstream ss;
        
        ss << "=== NLM Test Metrics Report ===" << std::endl << std::endl;
        
        // Performance metrics summary
        ss << "Performance Metrics:" << std::endl;
        for (const auto& pair : performanceData) {
            const auto& metrics = pair.second;
            ss << "  " << pair.first << ":" << std::endl;
            ss << "    Setup: " << TimeUtils::formatTime(metrics.setupTime) << std::endl;
            ss << "    Execution: " << TimeUtils::formatTime(metrics.executionTime) << std::endl;
            ss << "    Teardown: " << TimeUtils::formatTime(metrics.teardownTime) << std::endl;
            ss << "    Total: " << TimeUtils::formatTime(metrics.totalTime) << std::endl;
            ss << "    Throughput: " << metrics.throughput << " ops/sec" << std::endl;
            ss << "    Efficiency: " << metrics.efficiency << std::endl;
        }
        
        ss << std::endl;
        
        // Behavioral metrics summary
        ss << "Behavioral Metrics:" << std::endl;
        for (const auto& pair : behavioralData) {
            const auto& metrics = pair.second;
            ss << "  " << pair.first << ":" << std::endl;
            ss << "    Memory Retention: " << metrics.memoryRetention << std::endl;
            ss << "    Memory Stability: " << metrics.memoryStability << std::endl;
            ss << "    Learning Rate: " << metrics.learningRate << std::endl;
            ss << "    Learning Curve: " << metrics.learningCurve << std::endl;
            ss << "    Prediction Accuracy: " << metrics.predictionAccuracy << std::endl;
            ss << "    Neuromodulation Efficiency: " << metrics.neuromodulationEfficiency << std::endl;
            ss << "    Social Learning: " << metrics.socialLearning << std::endl;
        }
        
        ss << std::endl;
        
        // System metrics summary
        ss << "System Metrics:" << std::endl;
        for (const auto& pair : systemData) {
            const auto& metrics = pair.second;
            ss << "  " << pair.first << ":" << std::endl;
            ss << "    Brain Performance: " << metrics.brainPerformance << std::endl;
            ss << "    Neural Efficiency: " << metrics.neuralEfficiency << std::endl;
            ss << "    Network Stability: " << metrics.networkStability << std::endl;
            ss << "    Integration Quality: " << metrics.integrationQuality << std::endl;
            ss << "    Adaptability: " << metrics.adaptability << std::endl;
            ss << "    Scalability: " << metrics.scalability << std::endl;
        }
        
        ss << std::endl;
        
        // Timeseries data summary
        ss << "Timeseries Data:" << std::endl;
        for (const auto& pair : timeseriesData) {
            ss << "  " << pair.first << ": " << pair.second.size() << " points" << std::endl;
            if (!pair.second.empty()) {
                ss << "    Range: [" << Statistics::min(pair.second) << ", " << Statistics::max(pair.second) << "]" << std::endl;
                ss << "    Mean: " << Statistics::mean(pair.second) << std::endl;
                ss << "    StdDev: " << Statistics::stddev(pair.second) << std::endl;
            }
        }
        
        // Generate statistical summaries
        ss << std::endl << "=== Statistical Analysis ===" << std::endl;
        for (const auto& pair : timeseriesData) {
            const auto& data = pair.second;
            if (data.size() >= 2) {
                ss << pair.first << ":" << std::endl;
                ss << "  Mean: " << Statistics::mean(data) << std::endl;
                ss << "  StdDev: " << Statistics::stddev(data) << std::endl;
                ss << "  Min: " << Statistics::min(data) << std::endl;
                ss << "  Max: " << Statistics::max(data) << std::endl;
                ss << "  Median: " << Statistics::median(data) << std::endl;
                
                // Calculate basic trends
                if (data.size() >= 3) {
                    double firstThird = Statistics::mean(std::vector<double>(data.begin(), data.begin() + data.size()/3));
                    double lastThird = Statistics::mean(std::vector<double>(data.end() - data.size()/3, data.end()));
                    ss << "  Trend: First third=" << firstThird << ", Last third=" << lastThird << std::endl;
                    
                    if (lastThird > firstThird * 1.1) {
                        ss << "    Trend: Increasing" << std::endl;
                    } else if (lastThird < firstThird * 0.9) {
                        ss << "    Trend: Decreasing" << std::endl;
                    } else {
                        ss << "    Trend: Stable" << std::endl;
                    }
                }
            }
        }
        
        return ss.str();
    }
    
    // Save metrics to file
    bool saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport();
        file.close();
        return true;
    }
    
    // Clear all metrics
    void clear() {
        performanceData.clear();
        behavioralData.clear();
        systemData.clear();
        timeseriesData.clear();
        testMetrics.clear();
    }
    
    // Calculate average performance across tests
    PerformanceMetrics getAveragePerformance() const {
        PerformanceMetrics avg;
        if (performanceData.empty()) return avg;
        
        int count = 0;
        for (const auto& pair : performanceData) {
            avg.setupTime += pair.second.setupTime;
            avg.executionTime += pair.second.executionTime;
            avg.teardownTime += pair.second.teardownTime;
            avg.totalTime += pair.second.totalTime;
            avg.throughput += pair.second.throughput;
            avg.efficiency += pair.second.efficiency;
            count++;
        }
        
        if (count > 0) {
            avg.setupTime /= count;
            avg.executionTime /= count;
            avg.teardownTime /= count;
            avg.totalTime /= count;
            avg.throughput /= count;
            avg.efficiency /= count;
        }
        
        return avg;
    }
    
    // Calculate average behavioral metrics across tests
    BehavioralMetrics getAverageBehavioral() const {
        BehavioralMetrics avg;
        if (behavioralData.empty()) return avg;
        
        int count = 0;
        for (const auto& pair : behavioralData) {
            avg.memoryRetention += pair.second.memoryRetention;
            avg.memoryStability += pair.second.memoryStability;
            avg.memoryCapacity += pair.second.memoryCapacity;
            avg.memoryDecayRate += pair.second.memoryDecayRate;
            avg.learningRate += pair.second.learningRate;
            avg.learningCurve += pair.second.learningCurve;
            avg.plateauReached += pair.second.plateauReached;
            avg.transferAbility += pair.second.transferAbility;
            avg.predictionAccuracy += pair.second.predictionAccuracy;
            avg.predictionConsistency += pair.second.predictionConsistency;
            avg.predictionError += pair.second.predictionError;
            avg.predictionConfidence += pair.second.predictionConfidence;
            avg.neuromodulationEfficiency += pair.second.neuromodulationEfficiency;
            avg.rewardSensitivity += pair.second.rewardSensitivity;
            avg.noveltyResponse += pair.second.noveltyResponse;
            avg.developmentalProgress += pair.second.developmentalProgress;
            avg.criticalPeriod += pair.second.criticalPeriod;
            avg.maturationRate += pair.second.maturationRate;
            avg.socialLearning += pair.second.socialLearning;
            avg.imitationAccuracy += pair.second.imitationAccuracy;
            avg.coordination += pair.second.coordination;
            count++;
        }
        
        if (count > 0) {
            avg.memoryRetention /= count;
            avg.memoryStability /= count;
            avg.memoryCapacity /= count;
            avg.memoryDecayRate /= count;
            avg.learningRate /= count;
            avg.learningCurve /= count;
            avg.plateauReached /= count;
            avg.transferAbility /= count;
            avg.predictionAccuracy /= count;
            avg.predictionConsistency /= count;
            avg.predictionError /= count;
            avg.predictionConfidence /= count;
            avg.neuromodulationEfficiency /= count;
            avg.rewardSensitivity /= count;
            avg.noveltyResponse /= count;
            avg.developmentalProgress /= count;
            avg.criticalPeriod /= count;
            avg.maturationRate /= count;
            avg.socialLearning /= count;
            avg.imitationAccuracy /= count;
            avg.coordination /= count;
        }
        
        return avg;
    }
    
    // Calculate average system metrics across tests
    SystemMetrics getAverageSystem() const {
        SystemMetrics avg;
        if (systemData.empty()) return avg;
        
        int count = 0;
        for (const auto& pair : systemData) {
            avg.brainPerformance += pair.second.brainPerformance;
            avg.neuralEfficiency += pair.second.neuralEfficiency;
            avg.networkStability += pair.second.networkStability;
            avg.integrationQuality += pair.second.integrationQuality;
            avg.feedbackLoop += pair.second.feedbackLoop;
            avg.modularity += pair.second.modularity;
            avg.adaptability += pair.second.adaptability;
            avg.robustness += pair.second.robustness;
            avg.scalability += pair.second.scalability;
            avg.resourceUtilization += pair.second.resourceUtilization;
            count++;
        }
        
        if (count > 0) {
            avg.brainPerformance /= count;
            avg.neuralEfficiency /= count;
            avg.networkStability /= count;
            avg.integrationQuality /= count;
            avg.feedbackLoop /= count;
            avg.modularity /= count;
            avg.adaptability /= count;
            avg.robustness /= count;
            avg.scalability /= count;
            avg.resourceUtilization /= count;
        }
        
        return avg;
    }
};}

#endif // METRICSCOLLECTOR_HPP
