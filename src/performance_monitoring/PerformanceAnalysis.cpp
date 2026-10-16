#include "PerformanceAnalysis.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <cmath>
#include <map>

namespace nlm {

struct PerformanceAnalysis::Impl {
    std::shared_ptr<PerformanceMetrics> metrics;
    std::shared_ptr<Config> config;
    PerformanceConfig perfConfig;
    
    std::string baselineName;
    std::unordered_map<std::string, PerformanceProfile> baselines;
    
    mutable std::mutex analysisMutex;
    
    bool initialized{false};
};

PerformanceAnalysis::PerformanceAnalysis() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
}

PerformanceAnalysis::PerformanceAnalysis(std::shared_ptr<PerformanceMetrics> metrics) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->metrics = metrics;
    pImpl->config = nullptr; // Would need to get from PerformanceMonitor
    pImpl->initialized = true;
}

PerformanceAnalysis::~PerformanceAnalysis() = default;

std::vector<std::string> PerformanceAnalysis::analyzePerformance() const {
    std::vector<std::string> analysis;
    
    if (!pImpl->metrics) {
        analysis.push_back("No metrics available for analysis");
        return analysis;
    }
    
    // Get all metrics
    auto allMetrics = pImpl->metrics->getAllMetrics();
    
    if (allMetrics.empty()) {
        analysis.push_back("No performance metrics collected");
        return analysis;
    }
    
    // Analyze by category
    std::unordered_map<PerformanceCategory, std::vector<PerformanceMetric>> categorizedMetrics;
    for (const auto& metric : allMetrics) {
        categorizedMetrics[metric.category].push_back(metric);
    }
    
    // Neural computation analysis
    if (categorizedMetrics.find(PerformanceCategory::NeuralComputation) != categorizedMetrics.end()) {
        auto& neuralMetrics = categorizedMetrics[PerformanceCategory::NeuralComputation];
        if (!neuralMetrics.empty()) {
            analysis.push_back("Neural computation metrics analyzed:");
            for (const auto& metric : neuralMetrics) {
                analysis.push_back("  - " + metric.name + ": " + std::to_string(metric.value) + " " + metric.unit);
            }
        }
    }
    
    // Memory management analysis
    if (categorizedMetrics.find(PerformanceCategory::MemoryManagement) != categorizedMetrics.end()) {
        auto& memoryMetrics = categorizedMetrics[PerformanceCategory::MemoryManagement];
        if (!memoryMetrics.empty()) {
            analysis.push_back("Memory management metrics analyzed:");
            for (const auto& metric : memoryMetrics) {
                analysis.push_back("  - " + metric.name + ": " + std::to_string(metric.value) + " " + metric.unit);
            }
        }
    }
    
    // Connectivity analysis
    if (categorizedMetrics.find(PerformanceCategory::Connectivity) != categorizedMetrics.end()) {
        auto& connectivityMetrics = categorizedMetrics[PerformanceCategory::Connectivity];
        if (!connectivityMetrics.empty()) {
            analysis.push_back("Connectivity metrics analyzed:");
            for (const auto& metric : connectivityMetrics) {
                analysis.push_back("  - " + metric.name + ": " + std::to_string(metric.value) + " " + metric.unit);
            }
        }
    }
    
    // Integration analysis
    if (categorizedMetrics.find(PerformanceCategory::Integration) != categorizedMetrics.end()) {
        auto& integrationMetrics = categorizedMetrics[PerformanceCategory::Integration];
        if (!integrationMetrics.empty()) {
            analysis.push_back("Integration metrics analyzed:");
            for (const auto& metric : integrationMetrics) {
                analysis.push_back("  - " + metric.name + ": " + std::to_string(metric.value) + " " + metric.unit);
            }
        }
    }
    
    // Development analysis
    if (categorizedMetrics.find(PerformanceCategory::Development) != categorizedMetrics.end()) {
        auto& developmentMetrics = categorizedMetrics[PerformanceCategory::Development];
        if (!developmentMetrics.empty()) {
            analysis.push_back("Development metrics analyzed:");
            for (const auto& metric : developmentMetrics) {
                analysis.push_back("  - " + metric.name + ": " + std::to_string(metric.value) + " " + metric.unit);
            }
        }
    }
    
    // Overall analysis
    if (categorizedMetrics.find(PerformanceCategory::Overall) != categorizedMetrics.end()) {
        auto& overallMetrics = categorizedMetrics[PerformanceCategory::Overall];
        if (!overallMetrics.empty()) {
            analysis.push_back("Overall metrics analyzed:");
            for (const auto& metric : overallMetrics) {
                analysis.push_back("  - " + metric.name + ": " + std::to_string(metric.value) + " " + metric.unit);
            }
        }
    }
    
    return analysis;
}

std::vector<PerformanceAnomaly> PerformanceAnalysis::detectAnomalies() const {
    std::vector<PerformanceAnomaly> anomalies;
    
    if (!pImpl->metrics) {
        return anomalies;
    }
    
    auto allMetrics = pImpl->metrics->getAllMetrics();
    
    // Simple anomaly detection - would be more sophisticated in production
    for (const auto& metric : allMetrics) {
        // Check for extreme values
        if (std::isnan(metric.value) || std::isinf(metric.value)) {
            PerformanceAnomaly anomaly;
            anomaly.id = "nan_inf_" + metric.name;
            anomaly.category = metric.category;
            anomaly.description = "Invalid value detected: " + std::to_string(metric.value);
            anomaly.currentValue = metric.value;
            anomaly.expectedValue = 0.0;
            anomaly.deviation = 100.0;
            anomaly.timestamp = metric.timestamp;
            anomaly.severity = AlertType::Critical;
            anomaly.recommendation = "Check measurement system and data sources";
            anomalies.push_back(anomaly);
        }
        else if (metric.value < 0.0 && metric.name.find("count") == std::string::npos) {
            PerformanceAnomaly anomaly;
            anomaly.id = "negative_value_" + metric.name;
            anomaly.category = metric.category;
            anomaly.description = "Negative value detected for metric: " + metric.name;
            anomaly.currentValue = metric.value;
            anomaly.expectedValue = 0.0;
            anomaly.deviation = 100.0;
            anomaly.timestamp = metric.timestamp;
            anomaly.severity = AlertType::Warning;
            anomaly.recommendation = "Check if negative values are expected for this metric";
            anomalies.push_back(anomaly);
        }
    }
    
    return anomalies;
}

std::vector<PerformanceRegression> PerformanceAnalysis::detectRegression(const std::string& baselineName) const {
    std::vector<PerformanceRegression> regressions;
    
    // Simple regression detection - would need proper baseline comparison
    // For now, return empty vector as it would require historical data
    regressions.push_back(PerformanceRegression{
        "regression_placeholder",
        PerformanceCategory::Overall,
        "placeholder_metric",
        100.0,
        120.0,
        20.0,
        std::chrono::microseconds(0),
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()),
        "Regression detection not fully implemented",
        "Implement proper baseline comparison",
        0.5
    });
    
    return regressions;
}

std::vector<OptimizationRecommendation> PerformanceAnalysis::getOptimizationRecommendations() const {
    std::vector<OptimizationRecommendation> recommendations;
    
    if (!pImpl->metrics) {
        return recommendations;
    }
    
    auto allMetrics = pImpl->metrics->getAllMetrics();
    
    // Generate recommendations based on metrics
    for (const auto& metric : allMetrics) {
        if (metric.name == "cpu.time" && metric.value > 1000.0) {
            OptimizationRecommendation rec;
            rec.id = "reduce_cpu_usage_" + std::to_string(metric.timestamp.count());
            rec.category = PerformanceCategory::Overall;
            rec.problem = "High CPU usage detected";
            rec.currentState = std::to_string(metric.value) + " seconds";
            rec.proposedSolution = "Optimize CPU-intensive operations and consider parallelization";
            rec.expectedImprovement = 30.0;
            rec.estimatedImplementationTime = std::chrono::minutes(30);
            rec.implementationNotes = "May require architectural changes";
            rec.riskLevel = "Medium";
            recommendations.push_back(rec);
        }
        
        if (metric.name == "memory.usage" && metric.value > 1024.0 * 1024.0 * 1024.0) {
            OptimizationRecommendation rec;
            rec.id = "reduce_memory_usage_" + std::to_string(metric.timestamp.count());
            rec.category = PerformanceCategory::MemoryManagement;
            rec.problem = "High memory usage detected";
            rec.currentState = std::to_string(metric.value / 1024.0 / 1024.0) + " MB";
            rec.proposedSolution = "Implement memory pooling and reduce memory allocations";
            rec.expectedImprovement = 50.0;
            rec.estimatedImplementationTime = std::chrono::hours(2);
            rec.implementationNotes = "Use pre-allocated pools for frequently allocated objects";
            rec.riskLevel = "Low";
            recommendations.push_back(rec);
        }
    }
    
    return recommendations;
}

std::vector<std::string> PerformanceAnalysis::identifyBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    if (!pImpl->metrics) {
        bottlenecks.push_back("No metrics available for bottleneck analysis");
        return bottlenecks;
    }
    
    auto allMetrics = pImpl->metrics->getAllMetrics();
    
    // Simple bottleneck identification
    for (const auto& metric : allMetrics) {
        if (metric.name == "cpu.time" && metric.value > 1000.0) {
            bottlenecks.push_back("CPU-intensive operations (" + std::to_string(metric.value) + " seconds)");
        }
        
        if (metric.name == "memory.usage" && metric.value > 512.0 * 1024.0 * 1024.0) {
            bottlenecks.push_back("Memory usage (" + std::to_string(metric.value / 1024.0 / 1024.0) + " MB)");
        }
        
        if (metric.name == "event.count" && metric.value > 100000.0) {
            bottlenecks.push_back("High event count (" + std::to_string(metric.value) + " events)");
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceAnalysis::identifyHotPaths() const {
    std::vector<std::string> hotPaths;
    
    // Simple hot path identification based on metric names
    if (!pImpl->metrics) {
        hotPaths.push_back("No metrics available for hot path analysis");
        return hotPaths;
    }
    
    auto allMetrics = pImpl->metrics->getAllMetrics();
    
    for (const auto& metric : allMetrics) {
        if (metric.name.find("step") != std::string::npos ||
            metric.name.find("neuron") != std::string::npos ||
            metric.name.find("spike") != std::string::npos) {
            hotPaths.push_back(metric.name + " (" + std::to_string(metric.value) + " " + metric.unit + ")");
        }
    }
    
    return hotPaths;
}

std::string PerformanceAnalysis::getBottleneckDetails() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    auto bottlenecks = identifyBottlenecks();
    if (bottlenecks.empty()) {
        ss << "No bottlenecks detected";
        return ss.str();
    }
    
    ss << "Identified bottlenecks:\n";
    for (size_t i = 0; i < bottlenecks.size(); ++i) {
        ss << i + 1 << ". " << bottlenecks[i] << "\n";
    }
    
    return ss.str();
}

std::string PerformanceAnalysis::getHotPathDetails() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    auto hotPaths = identifyHotPaths();
    if (hotPaths.empty()) {
        ss << "No hot paths detected";
        return ss.str();
    }
    
    ss << "Hot paths:\n";
    for (size_t i = 0; i < hotPaths.size(); ++i) {
        ss << i + 1 << ". " << hotPaths[i] << "\n";
    }
    
    return ss.str();
}

PerformanceScore PerformanceAnalysis::calculatePerformanceScore() const {
    PerformanceScore score;
    
    if (!pImpl->metrics) {
        score.overallScore = 0.0;
        score.overallGrade = "F";
        score.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch());
        score.calculationDetails = "No metrics available";
        return score;
    }
    
    auto allMetrics = pImpl->metrics->getAllMetrics();
    
    // Simple scoring based on metrics
    double baseScore = 100.0;
    
    for (const auto& metric : allMetrics) {
        if (metric.name == "cpu.time" && metric.value > 1000.0) {
            baseScore -= std::min(50.0, (metric.value - 1000.0) / 20.0);
        }
        
        if (metric.name == "memory.usage" && metric.value > 512.0 * 1024.0 * 1024.0) {
            baseScore -= std::min(30.0, (metric.value - 512.0 * 1024.0 * 1024.0) / (64.0 * 1024.0 * 1024.0));
        }
        
        if (metric.name == "event.count" && metric.value > 100000.0) {
            baseScore -= std::min(20.0, (metric.value - 100000.0) / 50000.0);
        }
    }
    
    score.overallScore = std::max(0.0, baseScore);
    
    // Assign grade based on score
    if (score.overallScore >= 90.0) score.overallGrade = "A+";
    else if (score.overallScore >= 85.0) score.overallGrade = "A";
    else if (score.overallScore >= 80.0) score.overallGrade = "B+";
    else if (score.overallScore >= 70.0) score.overallGrade = "B";
    else if (score.overallScore >= 60.0) score.overallGrade = "C+";
    else if (score.overallScore >= 50.0) score.overallGrade = "C";
    else if (score.overallScore >= 40.0) score.overallGrade = "D";
    else score.overallGrade = "F";
    
    score.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
    
    std::stringstream details;
    details << std::fixed << std::setprecision(1);
    details << "Score calculated based on " << allMetrics.size() << " metrics. "
            << "Base score: 100.0, deductions for CPU time, memory usage, and event count.";
    score.calculationDetails = details.str();
    
    return score;
}

std::unordered_map<PerformanceCategory, double> PerformanceAnalysis::getCategoryScores() const {
    std::unordered_map<PerformanceCategory, double> categoryScores;
    
    if (!pImpl->metrics) {
        return categoryScores;
    }
    
    auto allMetrics = pImpl->metrics->getAllMetrics();
    
    // Simple category scoring
    for (const auto& metric : allMetrics) {
        double score = 100.0;
        
        if (metric.name == "cpu.time" && metric.value > 1000.0) {
            score -= std::min(50.0, (metric.value - 1000.0) / 20.0);
        }
        
        if (metric.name == "memory.usage" && metric.value > 512.0 * 1024.0 * 1024.0) {
            score -= std::min(30.0, (metric.value - 512.0 * 1024.0 * 1024.0) / (64.0 * 1024.0 * 1024.0));
        }
        
        categoryScores[metric.category] = std::max(0.0, score);
    }
    
    return categoryScores;
}

std::string PerformanceAnalysis::getOverallPerformanceGrade() const {
    auto score = calculatePerformanceScore();
    return score.overallGrade;
}

bool PerformanceAnalysis::establishBaseline(const std::string& baselineName) {
    pImpl->baselineName = baselineName;
    
    // Store baseline in config
    if (pImpl->config) {
        pImpl->config->set("performance.baseline_name", baselineName, ConfigSource::Runtime);
    }
    
    return true;
}

bool PerformanceAnalysis::compareToBaseline(const std::string& baselineName) const {
    // Simple baseline comparison
    return baselineName == pImpl->baselineName;
}

std::string PerformanceAnalysis::getComparisonReport() const {
    std::stringstream ss;
    ss << "Baseline comparison report for baseline: " << pImpl->baselineName << "\n";
    ss << "Note: Baseline comparison requires historical data storage";
    return ss.str();
}

std::vector<OptimizationRecommendation> PerformanceAnalysis::generateRecommendations(const std::string& problem) const {
    std::vector<OptimizationRecommendation> recommendations;
    
    // Generate recommendations based on problem
    if (problem.find("cpu") != std::string::npos) {
        OptimizationRecommendation rec;
        rec.id = "cpu_optimization_" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        rec.category = PerformanceCategory::Overall;
        rec.problem = problem;
        rec.currentState = "High CPU usage detected";
        rec.proposedSolution = "Optimize CPU-intensive operations";
        rec.expectedImprovement = 25.0;
        rec.estimatedImplementationTime = std::chrono::minutes(60);
        rec.implementationNotes = "Consider parallelization and algorithm optimization";
        rec.riskLevel = "Medium";
        recommendations.push_back(rec);
    }
    else if (problem.find("memory") != std::string::npos) {
        OptimizationRecommendation rec;
        rec.id = "memory_optimization_" + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        rec.category = PerformanceCategory::MemoryManagement;
        rec.problem = problem;
        rec.currentState = "High memory usage detected";
        rec.proposedSolution = "Implement memory pooling and reduce allocations";
        rec.expectedImprovement = 40.0;
        rec.estimatedImplementationTime = std::chrono::hours(4);
        rec.implementationNotes = "Use pre-allocated pools for frequently used objects";
        rec.riskLevel = "Low";
        recommendations.push_back(rec);
    }
    
    return recommendations;
}

std::string PerformanceAnalysis::getRecommendationDetails(const std::string& recommendationId) const {
    std::stringstream ss;
    ss << "Recommendation details for ID: " << recommendationId << "\n";
    ss << "This recommendation requires implementation details.";
    return ss.str();
}

void PerformanceAnalysis::setConfig(const PerformanceConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    pImpl->perfConfig = config;
}

PerformanceConfig PerformanceAnalysis::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->analysisMutex);
    return pImpl->perfConfig;
}

void PerformanceAnalysis::setBaselineName(const std::string& baselineName) {
    pImpl->baselineName = baselineName;
}

std::string PerformanceAnalysis::getBaselineName() const {
    return pImpl->baselineName;
}

// Singleton implementation
namespace {
    std::shared_ptr<PerformanceAnalysis> g_analysisInstance;
    std::mutex g_analysisMutex;
}

PerformanceAnalysis& PerformanceAnalysis::getInstance() {
    std::lock_guard<std::mutex> lock(g_analysisMutex);
    if (!g_analysisInstance) {
        g_analysisInstance = std::shared_ptr<PerformanceAnalysis>(new PerformanceAnalysis());
    }
    return *g_analysisInstance;
}

void PerformanceAnalysis::setInstance(std::shared_ptr<PerformanceAnalysis> analysis) {
    std::lock_guard<std::mutex> lock(g_analysisMutex);
    g_analysisInstance = analysis;
}

bool PerformanceAnalysis::isInitialized() {
    std::lock_guard<std::mutex> lock(g_analysisMutex);
    return g_analysisInstance != nullptr && g_analysisInstance->pImpl->initialized;
}

} // namespace nlm

nlm::PerformanceAnalysis& getGlobalPerformanceAnalysis() {
    return nlm::PerformanceAnalysis::getInstance();
}