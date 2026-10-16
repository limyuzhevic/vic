#pragma once

/**
 * Performance Analysis System for NLM Framework
 * 
 * This system provides comprehensive performance analysis:
 * - Performance analysis and anomaly detection
 * - Regression detection and comparison
 * - Performance scoring and grading
 * - Bottleneck identification and hot path analysis
 * - Optimization recommendations
 * - Performance impact assessment
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#include <optional>
#include <functional>
#include <map>
#include "../core/Config/Config.hpp"
#include "PerformanceMonitor.hpp"
#include "PerformanceMetrics.hpp"

namespace nlm {

/**
 * Performance anomaly information
 */
struct PerformanceAnomaly {
    std::string id;
    PerformanceCategory category;
    std::string description;
    double currentValue;
    double expectedValue;
    double deviation;  // Percentage deviation
    std::chrono::microseconds timestamp;
    AlertType severity;
    std::string recommendation;
};

/**
 * Performance regression information
 */
struct PerformanceRegression {
    std::string id;
    PerformanceCategory category;
    std::string metricName;
    double baselineValue;
    double currentValue;
    double regressionPercent;
    std::chrono::microseconds baselineTimestamp;
    std::chrono::microseconds currentTimestamp;
    std::string regressionDescription;
    std::string suggestedFix;
    double impactScore;  // 0.0 to 1.0
};

/**
 * Optimization recommendation
 */
struct OptimizationRecommendation {
    std::string id;
    PerformanceCategory category;
    std::string problem;
    std::string currentState;
    std::string proposedSolution;
    double expectedImprovement;  // Percentage improvement
    std::chrono::microseconds estimatedImplementationTime;
    std::string implementationNotes;
    std::string riskLevel; // Low, Medium, High
};

/**
 * Performance scoring information
 */
struct PerformanceScore {
    double overallScore;  // 0.0 to 100.0
    std::unordered_map<PerformanceCategory, double> categoryScores;
    std::string overallGrade; // A+, A, B+, B, C+, C, D, F
    std::chrono::microseconds timestamp;
    std::string calculationDetails;
};

/**
 * Performance analysis system
 */
class PerformanceAnalysis {
public:
    PerformanceAnalysis(std::shared_ptr<PerformanceMetrics> metrics);
    ~PerformanceAnalysis();
    
    // Performance analysis
    std::vector<std::string> analyzePerformance() const;
    std::vector<PerformanceAnomaly> detectAnomalies() const;
    std::vector<PerformanceRegression> detectRegression(const std::string& baselineName) const;
    std::vector<OptimizationRecommendation> getOptimizationRecommendations() const;
    
    // Bottleneck analysis
    std::vector<std::string> identifyBottlenecks() const;
    std::vector<std::string> identifyHotPaths() const;
    std::string getBottleneckDetails() const;
    std::string getHotPathDetails() const;
    
    // Performance scoring
    PerformanceScore calculatePerformanceScore() const;
    std::unordered_map<PerformanceCategory, double> getCategoryScores() const;
    std::string getOverallPerformanceGrade() const;
    
    // Comparison analysis
    bool establishBaseline(const std::string& baselineName);
    bool compareToBaseline(const std::string& baselineName) const;
    std::string getComparisonReport() const;
    
    // Recommendations
    std::vector<OptimizationRecommendation> generateRecommendations(const std::string& problem) const;
    std::string getRecommendationDetails(const std::string& recommendationId) const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
    // Baseline management
    void setBaselineName(const std::string& baselineName);
    std::string getBaselineName() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor
    PerformanceAnalysis();
};

} // namespace nlm

// Global performance analysis instance for convenience
nlm::PerformanceAnalysis& getGlobalPerformanceAnalysis();