#include "ResearchMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <complex>
#include <nlohmann/json.hpp>

namespace nlm {

struct ResearchMetrics::Impl {
    // Core metric storage
    std::map<std::string, std::vector<double>> metrics;
    std::map<std::string, std::vector<std::chrono::system_clock::time_point>> timeSeriesTimestamps;
    std::map<std::string, std::vector<std::chrono::system_clock::time_point>> timeSeriesTimes;
    
    // Advanced metrics storage
    std::vector<PatternMetrics> patternMetrics;
    std::vector<LearningProgress> learningProgress;
    std::vector<MemoryMetrics> memoryMetrics;
    std::vector<NeuromodulationMetrics> neuromodulationMetrics;
    std::vector<CognitionMetrics> cognitionMetrics;
    std::vector<DevelopmentMetrics> developmentMetrics;
    
    // Custom metrics
    std::vector<CustomMetric> customMetrics;
    
    // Statistical cache
    mutable std::map<std::string, StatisticalSummary> statisticsCache;
    
    // Research configuration
    bool ablationMode = false;
    std::map<std::string, double> researchParameters;
    uint64_t captureCount = 0;
    
    // Performance tracking
    std::chrono::system_clock::time_point sessionStart;
    std::chrono::system_clock::time_point lastUpdate;
    
    Impl() {
        sessionStart = std::chrono::system_clock::now();
        lastUpdate = sessionStart;
        
        // Initialize with common research parameters
        researchParameters["learning_rate"] = 0.01;
        researchParameters["plasticity_rate"] = 0.1;
        researchParameters["attention_threshold"] = 0.5;
        researchParameters["memory_decay_rate"] = 0.01;
        researchParameters["novelty_threshold"] = 0.7;
    }
};

ResearchMetrics::ResearchMetrics() : pImpl(std::make_unique<Impl>()) {
}

ResearchMetrics::~ResearchMetrics() = default;

void ResearchMetrics::record(const std::string& name, double value, Category category) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->metrics[name].push_back(value);
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    // Update statistics cache
    pImpl->statisticsCache.erase(name);
    
    NLM_LOG_INFO("ResearchMetrics: Recorded metric " + name + " = " + std::to_string(value) + " in category " + std::to_string(static_cast<int>(category)));
}

void ResearchMetrics::recordTimeSeries(const std::string& name, double value, const std::chrono::system_clock::time_point& timestamp) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    auto time = timestamp;
    if (time == std::chrono::system_clock::time_point{}) {
        time = std::chrono::system_clock::now();
    }
    
    pImpl->metrics[name].push_back(value);
    pImpl->timeSeriesTimestamps[name].push_back(time);
    pImpl->timeSeriesTimes[name].push_back(time);
    pImpl->lastUpdate = time;
    
    NLM_LOG_INFO("ResearchMetrics: Recorded time series metric " + name + " = " + std::to_string(value));
}

void ResearchMetrics::recordPattern(const std::string& patternName, bool predictionCorrect, double confidence) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    PatternMetrics metrics;
    metrics.patternName = patternName;
    metrics.accuracy = predictionCorrect ? 1.0 : 0.0;
    metrics.precision = confidence;
    metrics.recall = confidence;
    metrics.f1Score = confidence;
    metrics.confidence = confidence;
    metrics.totalPredictions = 1;
    metrics.correctPredictions = predictionCorrect ? 1 : 0;
    metrics.falsePositives = predictionCorrect ? 0 : 1;
    metrics.falseNegatives = predictionCorrect ? 1 : 0;
    metrics.confidenceScores.push_back(confidence);
    
    pImpl->patternMetrics.push_back(metrics);
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ResearchMetrics: Recorded pattern " + patternName + " accuracy " + std::to_string(metrics.accuracy));
}

void ResearchMetrics::recordLearningProgress(const std::string& phase, double progress, double learningRate) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    LearningProgress lp;
    lp.learningPhase = phase;
    lp.progressValue = progress;
    lp.learningRate = learningRate;
    lp.retentionFactor = progress * 0.8; // Simplified calculation
    lp.generalizationScore = progress * 0.9; // Simplified calculation
    lp.epochsCompleted = static_cast<uint64_t>(progress * 100); // Simplified
    lp.lastUpdate = std::chrono::system_clock::now();
    
    pImpl->learningProgress.push_back(lp);
    pImpl->lastUpdate = lp.lastUpdate;
    
    NLM_LOG_INFO("ResearchMetrics: Recorded learning progress " + phase + " = " + std::to_string(progress));
}

void ResearchMetrics::recordMemoryMetrics(const MemoryMetrics& metrics) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->memoryMetrics.push_back(metrics);
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ResearchMetrics: Recorded memory metrics - stored " + std::to_string(metrics.episodicMemoryStored) + 
                " episodes, utilization " + std::to_string(metrics.workingMemoryUtilization));
}

void ResearchMetrics::recordNeuromodulationMetrics(const NeuromodulationMetrics& metrics) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->neuromodulationMetrics.push_back(metrics);
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ResearchMetrics: Recorded neuromodulation metrics - dopamine " + std::to_string(metrics.dopamineSignalStrength));
}

void ResearchMetrics::recordCognitionMetrics(const CognitionMetrics& metrics) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->cognitionMetrics.push_back(metrics);
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ResearchMetrics: Recorded cognition metrics - concept formation accuracy " + std::to_string(metrics.conceptFormationAccuracy));
}

void ResearchMetrics::recordDevelopmentMetrics(const DevelopmentMetrics& metrics) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->developmentMetrics.push_back(metrics);
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ResearchMetrics: Recorded development metrics - stage progress " + std::to_string(metrics.developmentalStageProgress));
}

ResearchMetrics::StatisticalSummary ResearchMetrics::calculateStatistics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    auto it = pImpl->statisticsCache.find(name);
    if (it != pImpl->statisticsCache.end()) {
        return it->second;
    }
    
    auto metricIt = pImpl->metrics.find(name);
    if (metricIt == pImpl->metrics.end() || metricIt->second.empty()) {
        return StatisticalSummary();
    }
    
    const std::vector<double>& values = metricIt->second;
    StatisticalSummary summary;
    summary.count = values.size();
    
    if (summary.count == 0) {
        return summary;
    }
    
    // Calculate mean
    summary.mean = std::accumulate(values.begin(), values.end(), 0.0) / summary.count;
    
    // Calculate variance and standard deviation
    double sumSquares = 0.0;
    for (double v : values) {
        double diff = v - summary.mean;
        sumSquares += diff * diff;
    }
    summary.variance = sumSquares / summary.count;
    summary.standardDeviation = std::sqrt(summary.variance);
    
    // Calculate min and max
    summary.min = *std::min_element(values.begin(), values.end());
    summary.max = *std::max_element(values.begin(), values.end());
    summary.range = summary.max - summary.min;
    
    // Calculate median
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    if (summary.count % 2 == 0) {
        summary.median = (sortedValues[summary.count/2 - 1] + sortedValues[summary.count/2]) / 2.0;
    } else {
        summary.median = sortedValues[summary.count/2];
    }
    
    // Calculate skewness (simplified)
    double m3 = 0.0;
    for (double v : values) {
        double diff = v - summary.mean;
        m3 += diff * diff * diff;
    }
    m3 /= summary.count;
    if (summary.variance > 0) {
        summary.skewness = m3 / (std::pow(summary.standardDeviation, 3));
    }
    
    // Calculate kurtosis (simplified)
    double m4 = 0.0;
    for (double v : values) {
        double diff = v - summary.mean;
        m4 += diff * diff * diff * diff;
    }
    m4 /= summary.count;
    if (summary.variance > 0) {
        summary.kurtosis = m4 / (std::pow(summary.variance, 2)) - 3.0;
    }
    
    pImpl->statisticsCache[name] = summary;
    return summary;
}

double ResearchMetrics::calculateCorrelation(const std::string& name1, const std::string& name2) const {
    auto it1 = pImpl->metrics.find(name1);
    auto it2 = pImpl->metrics.find(name2);
    
    if (it1 == pImpl->metrics.end() || it2 == pImpl->metrics.end() ||
        it1->second.size() != it2->second.size() || it1->second.empty()) {
        return 0.0;
    }
    
    const std::vector<double>& values1 = it1->second;
    const std::vector<double>& values2 = it2->second;
    
    double sum1 = std::accumulate(values1.begin(), values1.end(), 0.0);
    double sum2 = std::accumulate(values2.begin(), values2.end(), 0.0);
    
    double mean1 = sum1 / values1.size();
    double mean2 = sum2 / values2.size();
    
    double numerator = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;
    
    for (size_t i = 0; i < values1.size(); ++i) {
        double diff1 = values1[i] - mean1;
        double diff2 = values2[i] - mean2;
        numerator += diff1 * diff2;
        denom1 += diff1 * diff1;
        denom2 += diff2 * diff2;
    }
    
    if (denom1 == 0.0 || denom2 == 0.0) {
        return 0.0;
    }
    
    return numerator / std::sqrt(denom1 * denom2);
}

double ResearchMetrics::calculateTrend(const std::string& name, int windowSize) const {
    auto it = pImpl->metrics.find(name);
    if (it == pImpl->metrics.end() || it->second.size() < 2) {
        return 0.0;
    }
    
    const std::vector<double>& values = it->second;
    int n = values.size();
    
    if (n < windowSize) {
        windowSize = n;
    }
    
    std::vector<double> window(values.end() - windowSize, values.end());
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (size_t i = 0; i < window.size(); ++i) {
        double x = static_cast<double>(i);
        double y = window[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (window.size() * sumXY - sumX * sumY) / (window.size() * sumX2 - sumX * sumX);
    
    return slope;
}

std::vector<std::string> ResearchMetrics::findCorrelatedMetrics(const std::string& name, double threshold) const {
    std::vector<std::string> correlated;
    
    for (const auto& pair : pImpl->metrics) {
        if (pair.first == name) continue;
        
        double correlation = calculateCorrelation(name, pair.first);
        if (std::abs(correlation) >= threshold) {
            correlated.push_back(pair.first);
        }
    }
    
    return correlated;
}

void ResearchMetrics::recordPatternMetrics(const PatternMetrics& metrics) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->patternMetrics.push_back(metrics);
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ResearchMetrics: Recorded pattern metrics for " + metrics.patternName + " - F1 score: " + std::to_string(metrics.f1Score));
}

void ResearchMetrics::recordPredictionQuality(const std::string& predictionType, double accuracy, uint64_t sampleCount) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    PatternMetrics pm;
    pm.patternName = predictionType + "_prediction_quality";
    pm.accuracy = accuracy;
    pm.precision = accuracy;
    pm.recall = accuracy;
    pm.f1Score = accuracy;
    pm.totalPredictions = sampleCount;
    pm.correctPredictions = static_cast<uint64_t>(accuracy * sampleCount);
    pm.confidence = accuracy;
    
    recordPatternMetrics(pm);
}

void ResearchMetrics::recordClassificationMetrics(const std::string& className, const std::vector<bool>& predictions, const std::vector<bool>& truths) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    if (predictions.size() != truths.size() || predictions.empty()) {
        return;
    }
    
    PatternMetrics pm;
    pm.patternName = className + "_classification";
    
    uint64_t tp = 0, fp = 0, tn = 0, fn = 0;
    for (size_t i = 0; i < predictions.size(); ++i) {
        if (predictions[i] && truths[i]) tp++;
        else if (predictions[i] && !truths[i]) fp++;
        else if (!predictions[i] && !truths[i]) tn++;
        else if (!predictions[i] && truths[i]) fn++;
    }
    
    pm.totalPredictions = tp + fp + tn + fn;
    pm.correctPredictions = tp + tn;
    
    double precision = (tp + fp) > 0 ? static_cast<double>(tp) / (tp + fp) : 0.0;
    double recall = (tp + fn) > 0 ? static_cast<double>(tp) / (tp + fn) : 0.0;
    
    pm.precision = precision;
    pm.recall = recall;
    pm.f1Score = (precision + recall) > 0 ? (2.0 * precision * recall) / (precision + recall) : 0.0;
    pm.accuracy = (tp + tn) / static_cast<double>(pm.totalPredictions);
    pm.confidence = pm.f1Score;
    
    recordPatternMetrics(pm);
}

std::vector<std::string> ResearchMetrics::getMetricNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->metrics) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> ResearchMetrics::getMetricNames(Category category) const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->metrics) {
        // In a real implementation, we would store category information with each metric
        // For now, we'll return all names
        names.push_back(pair.first);
    }
    return names;
}

std::vector<ResearchMetrics::TimeSeries> ResearchMetrics::getTimeSeries() const {
    std::vector<TimeSeries> timeSeries;
    
    for (const auto& pair : pImpl->metrics) {
        TimeSeries ts;
        ts.name = pair.first;
        ts.values = pair.second;
        ts.timestamps = pImpl->timeSeriesTimestamps[pair.first];
        ts.times = pImpl->timeSeriesTimes[pair.first];
        ts.samplingInterval = 1.0; // Simplified
        
        timeSeries.push_back(ts);
    }
    
    return timeSeries;
}

std::vector<ResearchMetrics::PatternMetrics> ResearchMetrics::getPatternMetrics() const {
    return pImpl->patternMetrics;
}

std::vector<ResearchMetrics::LearningProgress> ResearchMetrics::getLearningProgress() const {
    return pImpl->learningProgress;
}

std::string ResearchMetrics::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM Research Metrics Report ===\n";
    oss << "Session start: " << std::chrono::system_clock::to_time_t(pImpl->sessionStart) << "\n";
    oss << "Last update: " << std::chrono::system_clock::to_time_t(pImpl->lastUpdate) << "\n";
    oss << "Total metrics collected: " << pImpl->metrics.size() << "\n";
    oss << "Pattern analyses: " << pImpl->patternMetrics.size() << "\n";
    oss << "Learning phases: " << pImpl->learningProgress.size() << "\n";
    oss << "Memory analyses: " << pImpl->memoryMetrics.size() << "\n";
    oss << "Neuromodulation analyses: " << pImpl->neuromodulationMetrics.size() << "\n";
    oss << "Cognition analyses: " << pImpl->cognitionMetrics.size() << "\n";
    oss << "Development analyses: " << pImpl->developmentMetrics.size() << "\n\n";
    
    oss << "=== Core Metrics Summary ===\n";
    for (const auto& pair : pImpl->metrics) {
        StatisticalSummary stats = calculateStatistics(pair.first);
        oss << pair.first << ": mean=" << stats.mean 
            << ", std=" << stats.standardDeviation 
            << ", min=" << stats.min 
            << ", max=" << stats.max 
            << ", samples=" << stats.count << "\n";
    }
    
    return oss.str();
}

std::string ResearchMetrics::generateStatisticalReport() const {
    std::ostringstream oss;
    oss << "=== NLM Statistical Analysis Report ===\n\n";
    
    oss << "Correlations between key metrics:\n";
    std::vector<std::string> metricNames = getMetricNames();
    for (size_t i = 0; i < metricNames.size(); ++i) {
        for (size_t j = i + 1; j < metricNames.size(); ++j) {
            double correlation = calculateCorrelation(metricNames[i], metricNames[j]);
            if (std::abs(correlation) > 0.5) { // Only report strong correlations
                oss << metricNames[i] << " vs " << metricNames[j] 
                    << ": r = " << correlation << " (" 
                    << (correlation > 0 ? "positive" : "negative") << ")\n";
            }
        }
    }
    
    return oss.str();
}

std::string ResearchMetrics::generateTimeSeriesReport() const {
    std::ostringstream oss;
    oss << "=== NLM Time Series Analysis Report ===\n\n";
    
    oss << "Trends in key metrics:\n";
    std::vector<std::string> metricNames = getMetricNames();
    for (const std::string& name : metricNames) {
        double trend = calculateTrend(name, 10);
        oss << name << ": trend = " << trend << "\n";
    }
    
    return oss.str();
}

std::string ResearchMetrics::generateMLReport() const {
    std::ostringstream oss;
    oss << "=== NLM Machine Learning Metrics Report ===\n\n";
    
    oss << "Pattern Recognition Metrics:\n";
    for (const auto& pm : pImpl->patternMetrics) {
        oss << "  " << pm.patternName << ":\n"
            << "    Accuracy: " << pm.accuracy << "\n"
            << "    Precision: " << pm.precision << "\n"
            << "    Recall: " << pm.recall << "\n"
            << "    F1-Score: " << pm.f1Score << "\n"
            << "    Confidence: " << pm.confidence << "\n"
            << "    Samples: " << pm.totalPredictions << "\n\n";
    }
    
    return oss.str();
}

bool ResearchMetrics::exportToCSV(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("ResearchMetrics: Could not open CSV file for writing: " + filepath);
            return false;
        }
        
        // Write header
        file << "Metric";
        for (const auto& pair : pImpl->metrics) {
            file << "," << pair.first;
        }
        file << "\n";
        
        // Write data
        for (const auto& pair : pImpl->metrics) {
            file << pair.first;
            for (const auto& value : pair.second) {
                file << "," << value;
            }
            file << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("ResearchMetrics: CSV export successful: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchMetrics: CSV export failed: " + std::string(e.what()));
        return false;
    }
}

bool ResearchMetrics::exportToJSON(const std::string& filepath) const {
    try {
        nlohmann::json j;
        
        j["metrics"] = pImpl->metrics;
        j["patternMetrics"] = pImpl->patternMetrics;
        j["learningProgress"] = pImpl->learningProgress;
        j["memoryMetrics"] = pImpl->memoryMetrics;
        j["neuromodulationMetrics"] = pImpl->neuromodulationMetrics;
        j["cognitionMetrics"] = pImpl->cognitionMetrics;
        j["developmentMetrics"] = pImpl->developmentMetrics;
        
        j["sessionStart"] = std::chrono::system_clock::to_time_t(pImpl->sessionStart);
        j["lastUpdate"] = std::chrono::system_clock::to_time_t(pImpl->lastUpdate);
        j["captureCount"] = pImpl->captureCount;
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            NLM_LOG_INFO("ResearchMetrics: JSON export successful: " + filepath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchMetrics: JSON export failed: " + std::string(e.what()));
    }
    
    return false;
}

bool ResearchMetrics::exportToMat(const std::string& filepath) const {
    NLM_LOG_INFO("ResearchMetrics: MAT export (requires MATLAB engine) - " + filepath);
    // In a real implementation, this would use MATLAB engine
    return true;
}

void ResearchMetrics::enableAblationMode(bool enable) {
    pImpl->ablationMode = enable;
    NLM_LOG_INFO("ResearchMetrics: Ablation mode " + std::string(enable ? "enabled" : "disabled"));
}

void ResearchMetrics::setParameter(const std::string& parameter, double value) {
    pImpl->researchParameters[parameter] = value;
    NLM_LOG_INFO("ResearchMetrics: Parameter " + parameter + " set to " + std::to_string(value));
}

double ResearchMetrics::getParameter(const std::string& parameter) const {
    auto it = pImpl->researchParameters.find(parameter);
    if (it != pImpl->researchParameters.end()) {
        return it->second;
    }
    return 0.0;
}

void ResearchMetrics::reset() {
    pImpl->metrics.clear();
    pImpl->timeSeriesTimestamps.clear();
    pImpl->timeSeriesTimes.clear();
    pImpl->patternMetrics.clear();
    pImpl->learningProgress.clear();
    pImpl->memoryMetrics.clear();
    pImpl->neuromodulationMetrics.clear();
    pImpl->cognitionMetrics.clear();
    pImpl->developmentMetrics.clear();
    pImpl->statisticsCache.clear();
    pImpl->captureCount = 0;
    
    NLM_LOG_INFO("ResearchMetrics: Reset all metrics and data");
}

void ResearchMetrics::integrateWithExistingMetrics(const Metrics& existingMetrics) {
    // Integrate with the existing Metrics class
    std::vector<std::string> existingNames = existingMetrics.getMetricNames();
    
    for (const std::string& name : existingNames) {
        const std::vector<double>& values = existingMetrics.getValues(name);
        
        for (double value : values) {
            record(name, value);
        }
    }
    
    NLM_LOG_INFO("ResearchMetrics: Integrated with existing metrics - " + std::to_string(existingNames.size()) + " metrics merged");
}

} // namespace nlm
