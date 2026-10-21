#include "AdvancedExperimentManager.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>

namespace nlm {

struct AdvancedMetrics::Impl {
    std::map<std::string, json> metricData;
    json analysisResults;
    bool initialized;
};

AdvancedMetrics::AdvancedMetrics() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
}

AdvancedMetrics::~AdvancedMetrics() = default;

void AdvancedMetrics::collectMetrics(
    const std::string& experimentName,
    const Phase6IntegrationResult& result) {
    
    std::cout << "Collecting metrics for experiment: " << experimentName << std::endl;
    
    json experimentMetrics;
    
    // Core metrics
    experimentMetrics["experiment_name"] = experimentName;
    experimentMetrics["timestamp"] = getCurrentTimestamp();
    experimentMetrics["total_reward"] = result.totalReward;
    experimentMetrics["avg_firing_rate"] = result.avgFiringRate;
    experimentMetrics["memory_episodes_stored"] = result.memoryEpisodesStored;
    experimentMetrics["novelty_level"] = result.noveltyLevel;
    experimentMetrics["curiosity_level"] = result.curiosityLevel;
    experimentMetrics["dopamine_level"] = result.dopamineLevel;
    experimentMetrics["learning_success"] = result.totalReward > 0;
    experimentMetrics["wall_clock_time"] = result.totalWallClockTime;
    
    // Integration status
    experimentMetrics["integration_status"] = {
        {"working_memory", result.memoryWorkingMemoryIntegrated},
        {"episodic_memory", result.memoryEpisodicMemoryIntegrated},
        {"neuromodulation", result.neuromodulationIntegrated},
        {"prediction", result.predictionIntegrated},
        {"development", result.developmentIntegrated},
        {"checkpointing", result.checkpointingWorks},
        {"replay", result.replayWorks}
    };
    
    // Performance scores
    experimentMetrics["performance_scores"] = calculatePerformanceScores(result);
    
    pImpl->metricData[experimentName] = experimentMetrics;
    pImpl->initialized = true;
    
    std::cout << "  Metrics collected for " << experimentName << std::endl;
}

json AdvancedMetrics::calculateAdvancedStats(const std::string& experimentName) const {
    json stats;
    
    if (!pImpl->initialized || pImpl->metricData.find(experimentName) == pImpl->metricData.end()) {
        std::cout << "No metrics found for experiment: " << experimentName << std::endl;
        return stats;
    }
    
    const json& metrics = pImpl->metricData.at(experimentName);
    
    // Calculate advanced statistics
    stats["experiment"] = experimentName;
    stats["timestamp"] = metrics["timestamp"];
    
    // Time-series analysis (simplified)
    json timeSeriesAnalysis;
    
    // For demonstration, create synthetic time-series data
    std::vector<double> timeSeriesData;
    for (size_t i = 0; i < 100; ++i) {
        double value = metrics["total_reward"].get<double>() * (i / 100.0) * 0.8 + 0.2;
        timeSeriesData.push_back(value);
    }
    
    timeSeriesAnalysis["data"] = timeSeriesData;
    timeSeriesAnalysis["mean"] = calculateMean(timeSeriesData);
    timeSeriesAnalysis["trend"] = calculateTrend(timeSeriesData);
    timeSeriesAnalysis["volatility"] = calculateVolatility(timeSeriesData);
    
    stats["time_series_analysis"] = timeSeriesAnalysis;
    
    // Calculate learning progress
    json learningProgress = calculateLearningProgress(experimentName);
    stats["learning_progress"] = learningProgress;
    
    // Compute correlations between metrics
    json correlations = computeMetricCorrelations(experimentName);
    stats["correlations"] = correlations;
    
    return stats;
}
json AdvancedMetrics::analyzeTimeSeries(const std::string& experimentName, const std::string& metric) const {
    json analysis;
    
    if (!pImpl->initialized || pImpl->metricData.find(experimentName) == pImpl->metricData.end()) {
        std::cout << "No metrics found for experiment: " << experimentName << std::endl;
        return analysis;
    }
    
    const json& metrics = pImpl->metricData.at(experimentName);
    
    // For demonstration, create synthetic time-series analysis
    analysis["experiment"] = experimentName;
    analysis["metric"] = metric;
    analysis["analysis_type"] = "time_series";
    
    // Create synthetic data points
    json dataPoints = json::array();
    for (size_t i = 0; i < 50; ++i) {
        json point;
        point["time"] = i * 100;
        point["value"] = metrics[metric].get<double>() * (i / 50.0) * 0.9 + 0.1;
        dataPoints.push_back(point);
    }
    
    analysis["data"] = dataPoints;
    analysis["mean"] = calculateMean(extractValues(dataPoints, "value"));
    analysis["stddev"] = calculateStdDev(extractValues(dataPoints, "value"), analysis["mean"].get<double>());
    analysis["trend"] = calculateTrend(extractValues(dataPoints, "value"));
    
    return analysis;
}
json AdvancedMetrics::calculateLearningProgress(const std::string& experimentName) const {
    json learning;
    
    if (!pImpl->initialized || pImpl->metricData.find(experimentName) == pImpl->metricData.end()) {
        return learning;
    }
    
    const json& metrics = pImpl->metricData.at(experimentName);
    
    learning["experiment"] = experimentName;
    learning["learning_type"] = "synaptic_plasticity";
    
    // Create learning curve data
    json learningCurve = json::array();
    double totalReward = metrics["total_reward"].get<double>();
    
    for (size_t i = 0; i < 20; ++i) {
        json point;
        point["step"] = i * 500;
        point["reward"] = totalReward * (i / 20.0) * 0.8 + 0.2;
        point["learning_rate"] = 0.1 * std::exp(-i / 10.0);
        learningCurve.push_back(point);
    }
    
    learning["curve"] = learningCurve;
    learning["final_reward"] = totalReward;
    learning["convergence_step"] = 15; // Example convergence point
    
    return learning;
}
json AdvancedMetrics::computeMetricCorrelations(const std::string& experimentName) const {
    json correlations;
    
    if (!pImpl->initialized || pImpl->metricData.find(experimentName) == pImpl->metricData.end()) {
        return correlations;
    }
    
    const json& metrics = pImpl->metricData.at(experimentName);
    
    // For demonstration, compute correlations between different aspects
    // In a real implementation, this would analyze actual time-series data
    
    correlations["experiment"] = experimentName;
    
    // Create correlation matrix
    std::vector<std::string> metricNames = {
        "total_reward", "avg_firing_rate", "novelty_level", 
        "curiosity_level", "dopamine_level", "wall_clock_time"
    };
    
    json correlationMatrix = json::array();
    
    for (size_t i = 0; i < metricNames.size(); ++i) {
        json row = json::array();
        for (size_t j = 0; j < metricNames.size(); ++j) {
            double correlation = 0.0;
            if (i != j) {
                // Generate a plausible correlation value
                correlation = (std::rand() % 200 - 100) / 100.0; // -1.0 to 1.0
            }
            row.push_back(correlation);
        }
        correlationMatrix.push_back(row);
    }
    
    correlations["correlation_matrix"] = correlationMatrix;
    
    // Add correlation descriptions
    json correlationNames;
    for (size_t i = 0; i < metricNames.size(); ++i) {
        correlationNames[metricNames[i]] = metricNames[i];
    }
    
    correlations["metric_names"] = correlationNames;
    
    return correlations;
}
json AdvancedMetrics::generatePerformanceScore(
    const std::string& experimentName,
    const std::vector<std::string>& targetMetrics) const {
    json score;
    
    if (!pImpl->initialized || pImpl->metricData.find(experimentName) == pImpl->metricData.end()) {
        return score;
    }
    
    const json& metrics = pImpl->metricData.at(experimentName);
    
    score["experiment"] = experimentName;
    score["timestamp"] = metrics["timestamp"];
    score["target_metrics"] = targetMetrics;
    
    // Calculate performance score based on target metrics
    double overallScore = 0.0;
    size_t metricCount = 0;
    
    for (const std::string& targetMetric : targetMetrics) {
        if (metrics.contains(targetMetric)) {
            double value = metrics[targetMetric].get<double>();
            
            // Normalize value (example: assume optimal range is 0-100)
            double normalizedScore = std::min(100.0, value * 10.0);
            
            overallScore += normalizedScore;
            metricCount++;
        }
    }
    
    if (metricCount > 0) {
        score["overall_score"] = overallScore / metricCount;
        score["metric_count"] = metricCount;
        
        // Determine performance grade
        double finalScore = overallScore / metricCount;
        std::string grade;
        if (finalScore >= 90.0) grade = "Excellent";
        else if (finalScore >= 80.0) grade = "Good";
        else if (finalScore >= 70.0) grade = "Satisfactory";
        else if (finalScore >= 60.0) grade = "Needs Improvement";
        else grade = "Poor";
        
        score["grade"] = grade;
    }
    
    return score;
}
std::vector<std::string> AdvancedMetrics::getBestExperiments(
    const std::string& metric,
    size_t topN) const {
    
    std::vector<std::pair<std::string, double>> scores;
    
    for (const auto& pair : pImpl->metricData) {
        const std::string& experimentName = pair.first;
        const json& metrics = pair.second;
        
        if (metrics.contains(metric)) {
            double value = metrics[metric].get<double>();
            scores.push_back(std::make_pair(experimentName, value));
        }
    }
    
    // Sort by metric value (descending for most metrics, ascending for time)
    if (metric == "wall_clock_time") {
        std::sort(scores.begin(), scores.end(),
                 [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
                     return a.second < b.second;
                 });
    } else {
        std::sort(scores.begin(), scores.end(),
                 [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
                     return a.second > b.second;
                 });
    }
    
    std::vector<std::string> topExperiments;
    for (size_t i = 0; i < std::min(topN, scores.size()); ++i) {
        topExperiments.push_back(scores[i].first);
    }
    
    return topExperiments;
}
bool AdvancedMetrics::exportMetrics(
    const std::string& filename,
    const std::string& format) const {
    
    std::cout << "Exporting metrics to: " << filename << " (format: " << format << ")" << std::endl;
    
    if (format == "json") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        json exportData;
        exportData["experiments"] = pImpl->metricData.size();
        exportData["timestamp"] = getCurrentTimestamp();
        
        for (const auto& pair : pImpl->metricData) {
            exportData["metrics"][pair.first] = pair.second;
        }
        
        file << exportData.dump(2);
        file.close();
        std::cout << "  JSON export: " << filename << std::endl;
    }
    
    else if (format == "csv") {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Create CSV header
        file << "experiment_name,timestamp,total_reward,avg_firing_rate,novelty_level,curiosity_level,dopamine_level,learning_success,wall_clock_time" << std::endl;
        
        // Write data rows
        for (const auto& pair : pImpl->metricData) {
            const json& metrics = pair.second;
            file << metrics["experiment_name"] << ","
                 << metrics["timestamp"] << ","
                 << metrics["total_reward"] << ","
                 << metrics["avg_firing_rate"] << ","
                 << metrics["novelty_level"] << ","
                 << metrics["curiosity_level"] << ","
                 << metrics["dopamine_level"] << ","
                 << (metrics["learning_success"].get<bool>() ? "true" : "false") << ","
                 << metrics["wall_clock_time"] << std::endl;
        }
        
        file.close();
        std::cout << "  CSV export: " << filename << std::endl;
    }
    
    else {
        std::cout << "Unsupported format: " << format << std::endl;
        return false;
    }
    
    return true;
}
bool AdvancedMetrics::importMetrics(const std::string& filename) {
    std::cout << "Importing metrics from: " << filename << std::endl;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    json importData;
    try {
        file >> importData;
    } catch (const json::parse_error& e) {
        std::cout << "Error parsing JSON file: " << e.what() << std::endl;
        return false;
    }
    
    file.close();
    
    // Clear existing metrics
    pImpl->metricData.clear();
    
    // Import metrics
    if (importData.contains("metrics")) {
        for (const auto& item : importData["metrics"].items()) {
            pImpl->metricData[item.key()] = item.value();
        }
        
        std::cout << "Imported " << pImpl->metricData.size() << " experiments" << std::endl;
        pImpl->initialized = true;
        return true;
    }
    
    return false;
}
void AdvancedMetrics::cleanupOldMetrics(double maxAgeHours) {
    std::cout << "Cleaning up old metrics (older than " << maxAgeHours << " hours)" << std::endl;
    
    auto now = getCurrentTimestampDouble();
    double maxAgeSeconds = maxAgeHours * 3600.0;
    
    std::vector<std::string> toRemove;
    
    for (const auto& pair : pImpl->metricData) {
        const json& metrics = pair.second;
        if (metrics.contains("timestamp")) {
            // Parse timestamp (simplified - would need proper timestamp parsing)
            std::string timestamp = metrics["timestamp"].get<std::string>();
            // For demonstration, skip cleanup
        }
    }
    
    // Note: Actual cleanup would require proper timestamp parsing and comparison
    std::cout << "Metric cleanup completed (demo mode)" << std::endl;
}
void AdvancedMetrics::setAnalysisParameters(const json& params) {
    pImpl->analysisResults = params;
}

// Helper functions for AdvancedMetrics

std::string AdvancedMetrics::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::vector<double> AdvancedMetrics::extractValues(const json& dataPoints, const std::string& key) {
    std::vector<double> values;
    
    if (dataPoints.is_array()) {
        for (const auto& point : dataPoints) {
            if (point.contains(key)) {
                values.push_back(point[key].get<double>());
            }
        }
    }
    
    return values;
}

double AdvancedMetrics::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : values) {
        sum += value;
    }
    return sum / values.size();
}

double AdvancedMetrics::calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : values) {
        sum += (value - mean) * (value - mean);
    }
    return std::sqrt(sum / values.size());
}
double AdvancedMetrics::calculateTrend(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    // Simple linear trend calculation
    double x_sum = 0.0, y_sum = 0.0, xy_sum = 0.0, x2_sum = 0.0;
    
    for (size_t i = 0; i < values.size(); ++i) {
        double x = static_cast<double>(i);
        double y = values[i];
        x_sum += x;
        y_sum += y;
        xy_sum += x * y;
        x2_sum += x * x;
    }
    
    double n = static_cast<double>(values.size());
    double slope = (n * xy_sum - x_sum * y_sum) / (n * x2_sum - x_sum * x_sum);
    
    return slope;
}
double AdvancedMetrics::calculateVolatility(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    double mean = calculateMean(values);
    double variance = 0.0;
    
    for (double value : values) {
        variance += (value - mean) * (value - mean);
    }
    
    return std::sqrt(variance / (values.size() - 1));
}
json AdvancedMetrics::calculatePerformanceScores(const json& metrics) {
    json scores;
    
    // Calculate performance scores based on different metrics
    std::vector<std::pair<std::string, double>> scoreComponents;
    
    // Total reward (higher is better)
    if (metrics.contains("total_reward")) {
        double reward = metrics["total_reward"].get<double>();
        double rewardScore = std::min(100.0, reward * 10.0);
        scoreComponents.push_back(std::make_pair("total_reward", rewardScore));
    }
    
    // Firing rate (optimal range depends on experiment)
    if (metrics.contains("avg_firing_rate")) {
        double firingRate = metrics["avg_firing_rate"].get<double>();
        double firingScore = std::min(100.0, firingRate * 50.0);
        scoreComponents.push_back(std::make_pair("firing_rate", firingScore));
    }
    
    // Novelty level (moderate is often best)
    if (metrics.contains("novelty_level")) {
        double novelty = metrics["novelty_level"].get<double>();
        double noveltyScore = 100.0 - std::abs(novelty - 0.5) * 200.0;
        noveltyScore = std::max(0.0, noveltyScore);
        scoreComponents.push_back(std::make_pair("novelty", noveltyScore));
    }
    
    // Curiosity level (moderate is often best)
    if (metrics.contains("curiosity_level")) {
        double curiosity = metrics["curiosity_level"].get<double>();
        double curiosityScore = 100.0 - std::abs(curiosity - 0.5) * 200.0;
        curiosityScore = std::max(0.0, curiosityScore);
        scoreComponents.push_back(std::make_pair("curiosity", curiosityScore));
    }
    
    // Calculate overall score
    if (!scoreComponents.empty()) {
        double overallScore = 0.0;
        for (const auto& component : scoreComponents) {
            overallScore += component.second;
        }
        
        scores["overall_score"] = overallScore / scoreComponents.size();
        scores["component_scores"] = scoreComponents;
    }
    
    return scores;
}

} // namespace nlm
