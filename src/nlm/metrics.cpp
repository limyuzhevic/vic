// nlm/metrics.cpp - Implementation of metrics collection system
#include "metrics.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <numeric>
#include <cmath>
#include <set>

namespace nlm {

// Metric implementation
void Metric::update(float newValue) {
    value = newValue;
    minValue = std::min(minValue, newValue);
    maxValue = std::max(maxValue, newValue);
    timestamp = std::chrono::steady_clock::now();
}

double Metric::getAgeSeconds() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
    return static_cast<double>(duration.count());
}

bool Metric::isStale(double maxAgeSeconds) const {
    return getAgeSeconds() > maxAgeSeconds;
}

// TimeSeries implementation
void TimeSeries::addPoint(double timestamp, float value) {
    dataPoints.emplace_back(timestamp, value);
    
    // Trim if exceeding max points
    if (dataPoints.size() > maxPoints) {
        dataPoints.pop_front();
    }
}

float TimeSeries::getLatestValue() const {
    if (dataPoints.empty()) return 0.0f;
    return dataPoints.back().second;
}

std::vector<std::pair<double, float>> TimeSeries::getDataPoints(double fromTime) const {
    std::vector<std::pair<double, float>> result;
    for (const auto& point : dataPoints) {
        if (point.first >= fromTime) {
            result.push_back(point);
        }
    }
    return result;
}

double TimeSeries::getDuration() const {
    if (dataPoints.size() < 2) return 0.0;
    return dataPoints.back().first - dataPoints.front().first;
}

float TimeSeries::getAverage() const {
    if (dataPoints.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& point : dataPoints) {
        sum += point.second;
    }
    return sum / static_cast<float>(dataPoints.size());
}

float TimeSeries::getMax() const {
    if (dataPoints.empty()) return 0.0f;
    return std::max_element(dataPoints.begin(), dataPoints.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; })->second;
}

float TimeSeries::getMin() const {
    if (dataPoints.empty()) return 0.0f;
    return std::min_element(dataPoints.begin(), dataPoints.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; })->second;
}

float TimeSeries::getStandardDeviation() const {
    if (dataPoints.size() < 2) return 0.0f;
    
    float mean = getAverage();
    float sumSquaredDiff = 0.0f;
    for (const auto& point : dataPoints) {
        float diff = point.second - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return std::sqrt(sumSquaredDiff / static_cast<float>(dataPoints.size() - 1));
}

std::vector<float> TimeSeries::getRecentValues(size_t count) const {
    std::vector<float> result;
    size_t start = std::max(size_t(0), static_cast<size_t>(dataPoints.size()) - count);
    
    for (size_t i = start; i < dataPoints.size(); ++i) {
        result.push_back(dataPoints[i].second);
    }
    
    return result;
}

// ExperimentResult implementation
void ExperimentResult::recordMetric(const std::string& name, float value) {
    metrics[name] = value;
}

// LearningCurve implementation
void LearningCurve::addDataPoint(double time, float performance, float reward) {
    timePoints.push_back(time);
    performanceValues.push_back(performance);
    rewardValues.push_back(reward);
}

float LearningCurve::computeSlope(double startTime, double endTime) const {
    std::vector<std::pair<double, float>> relevantPoints;
    
    for (size_t i = 0; i < timePoints.size(); ++i) {
        if (timePoints[i] >= startTime && timePoints[i] <= endTime) {
            relevantPoints.emplace_back(timePoints[i], performanceValues[i]);
        }
    }
    
    if (relevantPoints.size() < 2) return 0.0f;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    for (const auto& point : relevantPoints) {
        sumX += point.first;
        sumY += point.second;
        sumXY += point.first * point.second;
        sumX2 += point.first * point.first;
    }
    
    double n = static_cast<double>(relevantPoints.size());
    return static_cast<float>((n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX));
}

float LearningCurve::computeAreaUnderCurve() const {
    if (timePoints.size() < 2) return 0.0f;
    
    float area = 0.0f;
    for (size_t i = 1; i < timePoints.size(); ++i) {
        double width = timePoints[i] - timePoints[i-1];
        float height = (performanceValues[i] + performanceValues[i-1]) / 2.0f;
        area += static_cast<float>(width * height);
    }
    
    return area;
}

double LearningCurve::findInflectionPoint() const {
    if (timePoints.size() < 3) return -1.0;
    
    // Find point where derivative changes sign (simple inflection detection)
    std::vector<double> derivatives;
    for (size_t i = 1; i < performanceValues.size(); ++i) {
        double deriv = performanceValues[i] - performanceValues[i-1];
        derivatives.push_back(deriv);
    }
    
    for (size_t i = 1; i < derivatives.size(); ++i) {
        if (derivatives[i] > 0 && derivatives[i-1] < 0) {
            return timePoints[i];
        }
    }
    
    return -1.0;
}

float LearningCurve::getMaximumPerformance() const {
    if (performanceValues.empty()) return 0.0f;
    return *std::max_element(performanceValues.begin(), performanceValues.end());
}

double LearningCurve::getTimeToConvergence(float threshold) const {
    float target = getMaximumPerformance() * threshold;
    
    for (size_t i = 0; i < performanceValues.size(); ++i) {
        if (performanceValues[i] >= target) {
            return timePoints[i];
        }
    }
    
    return -1.0;
}

float LearningCurve::computeStability() const {
    if (performanceValues.size() < 2) return 0.0f;
    
    float mean = getAverage();
    float variance = 0.0f;
    for (float value : performanceValues) {
        float diff = value - mean;
        variance += diff * diff;
    }
    variance /= static_cast<float>(performanceValues.size());
    
    return std::sqrt(variance); // Lower variance means more stable
}

std::string LearningCurve::getSummary() const {
    std::ostringstream oss;
    oss << "Learning Curve Summary:" << std::endl;
    oss << "  Data points: " << timePoints.size() << std::endl;
    oss << "  Max performance: " << getMaximumPerformance() << std::endl;
    oss << "  Final performance: " << (timePoints.empty() ? 0.0f : performanceValues.back()) << std::endl;
    oss << "  Slope: " << computeSlope(0.0, timePoints.empty() ? 0.0 : timePoints.back()) << std::endl;
    oss << "  Area under curve: " << computeAreaUnderCurve() << std::endl;
    oss << "  Stability: " << computeStability() << std::endl;
    oss << "  Time to 95% convergence: " << getTimeToConvergence(0.95f) << std::endl;
    
    return oss.str();
}

// AnomalyDetector implementation
bool AnomalyDetector::isAnomaly(const std::vector<float>& dataPoints, size_t index) const {
    if (index >= dataPoints.size()) return false;
    
    switch (method) {
        case DetectionMethod::STATIC_THRESHOLD:
            return std::abs(dataPoints[index] - *std::max_element(dataPoints.begin(), dataPoints.end())) > threshold;
        
        case DetectionMethod::MOVING_AVERAGE:
            if (index < windowSize) return false;
            float movingAvg = 0.0f;
            for (size_t i = index - windowSize + 1; i <= index; ++i) {
                movingAvg += dataPoints[i];
            }
            movingAvg /= windowSize;
            return std::abs(dataPoints[index] - movingAvg) > threshold;
        
        case DetectionMethod::STANDARD_DEVIATION:
            if (dataPoints.size() < 2) return false;
            float mean = 0.0f, variance = 0.0f;
            for (float value : dataPoints) {
                mean += value;
            }
            mean /= static_cast<float>(dataPoints.size());
            
            for (float value : dataPoints) {
                float diff = value - mean;
                variance += diff * diff;
            }
            variance /= static_cast<float>(dataPoints.size());
            float stdDev = std::sqrt(variance);
            
            return (dataPoints[index] - mean) / stdDev > zScoreThreshold;
        
        default:
            return false; // Other methods not implemented yet
    }
}

std::vector<bool> AnomalyDetector::detectAnomalies(const std::vector<float>& dataPoints) const {
    std::vector<bool> anomalies(dataPoints.size(), false);
    for (size_t i = 0; i < dataPoints.size(); ++i) {
        anomalies[i] = isAnomaly(dataPoints, i);
    }
    return anomalies;
}

float AnomalyDetector::computeZScore(const std::vector<float>& dataPoints, size_t index) const {
    if (index >= dataPoints.size() || dataPoints.size() < 2) return 0.0f;
    
    float mean = 0.0f, stdDev = 0.0f;
    for (float value : dataPoints) {
        mean += value;
    }
    mean /= static_cast<float>(dataPoints.size());
    
    for (float value : dataPoints) {
        float diff = value - mean;
        stdDev += diff * diff;
    }
    stdDev /= static_cast<float>(dataPoints.size());
    stdDev = std::sqrt(stdDev);
    
    if (stdDev == 0.0f) return 0.0f;
    return (dataPoints[index] - mean) / stdDev;
}

std::vector<size_t> AnomalyDetector::getAnomalyIndices(const std::vector<float>& dataPoints) const {
    std::vector<size_t> anomalies;
    for (size_t i = 0; i < dataPoints.size(); ++i) {
        if (isAnomaly(dataPoints, i)) {
            anomalies.push_back(i);
        }
    }
    return anomalies;
}

// MetricsCollector implementation
struct MetricsCollector::Impl {
    std::map<std::string, std::map<std::string, Metric>> metrics;
    std::map<std::string, std::map<std::string, TimeSeries>> timeSeries;
    std::map<std::string, ExperimentResult> experimentResults;
    std::map<std::string, LearningCurve> learningCurves;
    std::set<std::string> collectingControllers;
    std::map<std::string, float> samplingRates;
    std::map<std::string, double> retentionTimes;
    std::map<std::string, size_t> maxDataPoints;
    std::map<std::string, bool> anomalyDetectionEnabled;
    AnomalyDetector anomalyDetector;
    bool globalPaused = false;
    std::chrono::steady_clock::time_point lastCleanup;
};

MetricsCollector::MetricsCollector() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastCleanup = std::chrono::steady_clock::now();
    NLM_LOG_INFO("MetricsCollector initialized");
}

MetricsCollector::~MetricsCollector() {
    NLM_LOG_INFO("MetricsCollector shutdown");
}

void MetricsCollector::startCollecting(const std::string& controllerId) {
    pImpl->collectingControllers.insert(controllerId);
    pImpl->samplingRates[controllerId] = 10.0f; // Default 10 Hz
    pImpl->retentionTimes[controllerId] = 3600.0; // Default 1 hour
    pImpl->maxDataPoints[controllerId] = 1000;
    pImpl->anomalyDetectionEnabled[controllerId] = false;
    
    NLM_LOG_INFO("Started metrics collection for controller: " + controllerId);
}

void MetricsCollector::stopCollecting(const std::string& controllerId) {
    pImpl->collectingControllers.erase(controllerId);
    pImpl->metrics.erase(controllerId);
    pImpl->timeSeries.erase(controllerId);
    pImpl->samplingRates.erase(controllerId);
    pImpl->retentionTimes.erase(controllerId);
    pImpl->maxDataPoints.erase(controllerId);
    pImpl->anomalyDetectionEnabled.erase(controllerId);
    
    NLM_LOG_INFO("Stopped metrics collection for controller: " + controllerId);
}

bool MetricsCollector::isCollecting(const std::string& controllerId) const {
    return pImpl->collectingControllers.find(controllerId) != pImpl->collectingControllers.end();
}

void MetricsCollector::recordMetric(const std::string& controllerId, const Metric& metric) {
    if (!isCollecting(controllerId) || pImpl->globalPaused) {
        return;
    }
    
    pImpl->metrics[controllerId][metric.name] = metric;
    
    // Add to time series if applicable
    if (pImpl->timeSeries.find(controllerId) != pImpl->timeSeries.end() &&
        pImpl->timeSeries[controllerId].find(metric.name) != pImpl->timeSeries[controllerId].end()) {
        double timestamp = metric.getAgeSeconds();
        pImpl->timeSeries[controllerId][metric.name].addPoint(timestamp, metric.value);
    }
}

void MetricsCollector::recordCounter(const std::string& controllerId, const std::string& name, 
                                      float value, const std::string& context) {
    Metric metric(name, Metric::Type::COUNTER, value);
    metric.setContext(context);
    recordMetric(controllerId, metric);
}

void MetricsCollector::recordGauge(const std::string& controllerId, const std::string& name, 
                                   float value, const std::string& context) {
    Metric metric(name, Metric::Type::GAUGE, value);
    metric.setContext(context);
    recordMetric(controllerId, metric);
}

void MetricsCollector::recordHistogram(const std::string& controllerId, const std::string& name,
                                        float value, float min, float max,
                                        const std::string& context) {
    Metric metric(name, Metric::Type::HISTOGRAM, value, min, max);
    metric.setContext(context);
    recordMetric(controllerId, metric);
}

void MetricsCollector::recordTimer(const std::string& controllerId, const std::string& name,
                                    float duration, const std::string& context) {
    Metric metric(name, Metric::Type::TIMER, duration);
    metric.setContext(context);
    recordMetric(controllerId, metric);
}

void MetricsCollector::recordPercentage(const std::string& controllerId, const std::string& name,
                                        float percentage, const std::string& context) {
    Metric metric(name, Metric::Type::PERCENTAGE, percentage);
    metric.setContext(context);
    recordMetric(controllerId, metric);
}

void MetricsCollector::recordTimeSeries(const std::string& controllerId, const TimeSeries& series) {
    if (!isCollecting(controllerId)) {
        return;
    }
    
    pImpl->timeSeries[controllerId][series.name] = series;
}

void MetricsCollector::updateTimeSeries(const std::string& controllerId, const std::string& name,
                                        double timestamp, float value) {
    if (pImpl->timeSeries.find(controllerId) == pImpl->timeSeries.end() ||
        pImpl->timeSeries[controllerId].find(name) == pImpl->timeSeries[controllerId].end()) {
        return;
    }
    
    pImpl->timeSeries[controllerId][name].addPoint(timestamp, value);
}

void MetricsCollector::recordBrainControllerMetrics(const std::string& controllerId, 
                                                    const BrainController& controller) {
    if (!isCollecting(controllerId)) {
        return;
    }
    
    auto metrics = controller.getPerformanceMetrics();
    for (const auto& pair : metrics) {
        recordGauge(controllerId, pair.first, pair.second, "brain_controller");
    }
}

void MetricsCollector::recordExperimentStart(const std::string& controllerId, 
                                             const std::string& experimentId) {
    if (!isCollecting(controllerId)) {
        return;
    }
    
    ExperimentResult result(experimentId, controllerId);
    result.startTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - pImpl->lastCleanup).count();
    result.status = "running";
    
    pImpl->experimentResults[experimentId] = result;
}

void MetricsCollector::recordExperimentEnd(const std::string& controllerId,
                                           const std::string& experimentId,
                                           const std::string& status, float accuracy) {
    auto it = pImpl->experimentResults.find(experimentId);
    if (it != pImpl->experimentResults.end()) {
        it->second.endTime = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - pImpl->lastCleanup).count();
        it->second.status = status;
        it->second.accuracy = accuracy;
        
        // Add to learning curve
        LearningCurve curve(experimentId);
        // TODO: Extract learning data from experiment results
        pImpl->learningCurves[experimentId] = curve;
        
        NLM_LOG_INFO("Recorded experiment result: " + experimentId + " - " + status + " - accuracy: " + std::to_string(accuracy));
    }
}

Metric MetricsCollector::getLatestMetric(const std::string& controllerId, const std::string& name) const {
    auto it = pImpl->metrics.find(controllerId);
    if (it == pImpl->metrics.end() || it->second.find(name) == it->second.end()) {
        return Metric(name, Metric::Type::GAUGE, 0.0f);
    }
    return it->second[name];
}

std::vector<Metric> MetricsCollector::getMetricsHistory(const std::string& controllerId, 
                                                        const std::string& name,
                                                        double fromTime) const {
    std::vector<Metric> result;
    auto it = pImpl->metrics.find(controllerId);
    if (it == pImpl->metrics.end() || it->second.find(name) == it->second.end()) {
        return result;
    }
    
    for (const auto& pair : it->second) {
        if (pair.first == name && pair.second.getAgeSeconds() >= fromTime) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

TimeSeries MetricsCollector::getTimeSeries(const std::string& controllerId, const std::string& name) const {
    auto it = pImpl->timeSeries.find(controllerId);
    if (it == pImpl->timeSeries.end() || it->second.find(name) == it->second.end()) {
        return TimeSeries(name);
    }
    return it->second[name];
}

std::vector<TimeSeries> MetricsCollector::getTimeSeriesList(const std::string& controllerId) const {
    std::vector<TimeSeries> result;
    auto it = pImpl->timeSeries.find(controllerId);
    if (it != pImpl->timeSeries.end()) {
        for (const auto& pair : it->second) {
            result.push_back(pair.second);
        }
    }
    return result;
}

ExperimentResult MetricsCollector::getExperimentResult(const std::string& experimentId) const {
    auto it = pImpl->experimentResults.find(experimentId);
    if (it == pImpl->experimentResults.end()) {
        return ExperimentResult(experimentId, "unknown");
    }
    return it->second;
}

std::vector<ExperimentResult> MetricsCollector::getExperimentResults(const std::string& controllerId) const {
    std::vector<ExperimentResult> result;
    for (const auto& pair : pImpl->experimentResults) {
        if (pair.second.controllerId == controllerId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

LearningCurve MetricsCollector::getLearningCurve(const std::string& experimentId) const {
    auto it = pImpl->learningCurves.find(experimentId);
    if (it == pImpl->learningCurves.end()) {
        return LearningCurve(experimentId);
    }
    return it->second;
}

std::vector<LearningCurve> MetricsCollector::getLearningCurves(const std::string& controllerId) const {
    std::vector<LearningCurve> result;
    for (const auto& pair : pImpl->learningCurves) {
        // TODO: Check if experiment belongs to controller
        result.push_back(pair.second);
    }
    return result;
}

float MetricsCollector::computeAverage(const std::string& controllerId, const std::string& metricName) const {
    auto metrics = getMetricsHistory(controllerId, metricName);
    if (metrics.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& metric : metrics) {
        sum += metric.value;
    }
    
    return sum / static_cast<float>(metrics.size());
}

float MetricsCollector::computeMax(const std::string& controllerId, const std::string& metricName) const {
    auto metrics = getMetricsHistory(controllerId, metricName);
    if (metrics.empty()) return 0.0f;
    
    return std::max_element(metrics.begin(), metrics.end(),
        [](const Metric& a, const Metric& b) { return a.value < b.value; })->value;
}

float MetricsCollector::computeMin(const std::string& controllerId, const std::string& metricName) const {
    auto metrics = getMetricsHistory(controllerId, metricName);
    if (metrics.empty()) return 0.0f;
    
    return std::min_element(metrics.begin(), metrics.end(),
        [](const Metric& a, const Metric& b) { return a.value < b.value; })->value;
}

float MetricsCollector::computeStandardDeviation(const std::string& controllerId, const std::string& metricName) const {
    auto metrics = getMetricsHistory(controllerId, metricName);
    if (metrics.size() < 2) return 0.0f;
    
    float mean = computeAverage(controllerId, metricName);
    float sumSquaredDiff = 0.0f;
    for (const auto& metric : metrics) {
        float diff = metric.value - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return std::sqrt(sumSquaredDiff / static_cast<float>(metrics.size() - 1));
}

float MetricsCollector::computeMedian(const std::string& controllerId, const std::string& metricName) const {
    auto metrics = getMetricsHistory(controllerId, metricName);
    if (metrics.empty()) return 0.0f;
    
    std::vector<float> values;
    for (const auto& metric : metrics) {
        values.push_back(metric.value);
    }
    
    std::sort(values.begin(), values.end());
    size_t middle = values.size() / 2;
    
    if (values.size() % 2 == 0) {
        return (values[middle - 1] + values[middle]) / 2.0f;
    } else {
        return values[middle];
    }
}

float MetricsCollector::compareControllers(const std::string& controllerId1, const std::string& controllerId2,
                                          const std::string& metricName) const {
    float avg1 = computeAverage(controllerId1, metricName);
    float avg2 = computeAverage(controllerId2, metricName);
    
    if (avg1 == avg2) return 0.0f;
    
    float diff = std::abs(avg1 - avg2);
    float maxVal = std::max(std::abs(avg1), std::abs(avg2));
    if (maxVal == 0.0f) return 0.0f;
    
    return diff / maxVal; // Relative difference
}

std::map<std::string, float> MetricsCollector::compareExperiments(const std::string& controllerId,
                                                          const std::vector<std::string>& experimentIds) const {
    std::map<std::string, float> results;
    
    for (const auto& expId : experimentIds) {
        auto result = getExperimentResult(expId);
        if (result.isCompleted()) {
            results[expId] = result.accuracy;
        }
    }
    
    return results;
}

std::string MetricsCollector::generateMetricsReport(const std::string& controllerId) const {
    std::ostringstream oss;
    oss << "=== Metrics Report for Controller: " << controllerId << " ===" << std::endl;
    
    auto metrics = getMetricsHistory(controllerId, "");
    if (metrics.empty()) {
        oss << "No metrics recorded." << std::endl;
        return oss.str();
    }
    
    // Group metrics by name
    std::map<std::string, std::vector<Metric>> groupedMetrics;
    for (const auto& metric : metrics) {
        groupedMetrics[metric.name].push_back(metric);
    }
    
    for (const auto& pair : groupedMetrics) {
        const std::string& name = pair.first;
        const std::vector<Metric>& metricList = pair.second;
        
        oss << std::endl << "Metric: " << name << std::endl;
        oss << "  Count: " << metricList.size() << std::endl;
        oss << "  Average: " << computeAverage(controllerId, name) << std::endl;
        oss << "  Min: " << computeMin(controllerId, name) << std::endl;
        oss << "  Max: " << computeMax(controllerId, name) << std::endl;
        oss << "  Std Dev: " << computeStandardDeviation(controllerId, name) << std::endl;
        oss << "  Latest: " << getLatestMetric(controllerId, name).value << std::endl;
    }
    
    oss << std::endl << "============================================" << std::endl;
    return oss.str();
}

bool MetricsCollector::exportMetricsToCSV(const std::string& filepath, const std::string& controllerId) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for CSV export: " + filepath);
        return false;
    }
    
    file << "timestamp,controller,metric_name,value,context" << std::endl;
    
    for (const auto& pair : pImpl->metrics) {
        const std::string& cid = pair.first;
        if (!controllerId.empty() && cid != controllerId) continue;
        
        for (const auto& metricPair : pair.second) {
            const Metric& metric = metricPair.second;
            double age = metric.getAgeSeconds();
            file << age << "," << cid << "," << metric.name << "," 
                 << metric.value << "," << metric.context << std::endl;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Metrics exported to CSV: " + filepath);
    return true;
}

bool MetricsCollector::exportMetricsToJSON(const std::string& filepath, const std::string& controllerId) const {
    // TODO: Implement JSON export
    NLM_LOG_WARNING("JSON export not yet implemented");
    return false;
}

bool MetricsCollector::exportLearningCurvesToJSON(const std::string& filepath,
                                                  const std::vector<std::string>& experimentIds) const {
    // TODO: Implement learning curve JSON export
    NLM_LOG_WARNING("Learning curve JSON export not yet implemented");
    return false;
}

void MetricsCollector::setSamplingRate(const std::string& controllerId, float samplesPerSecond) {
    pImpl->samplingRates[controllerId] = samplesPerSecond;
}

void MetricsCollector::setMetricRetentionTime(const std::string& controllerId, double retentionTime) {
    pImpl->retentionTimes[controllerId] = retentionTime;
}

void MetricsCollector::setMaxDataPoints(const std::string& controllerId, size_t maxPoints) {
    pImpl->maxDataPoints[controllerId] = maxPoints;
}

void MetricsCollector::enableAnomalyDetection(const std::string& controllerId, bool enable) {
    pImpl->anomalyDetectionEnabled[controllerId] = enable;
}

void MetricsCollector::setAnomalyDetector(const AnomalyDetector& detector) {
    pImpl->anomalyDetector = detector;
}

void MetricsCollector::clearControllerData(const std::string& controllerId) {
    pImpl->metrics.erase(controllerId);
    pImpl->timeSeries.erase(controllerId);
    pImpl->experimentResults.erase(controllerId);
    pImpl->collectingControllers.erase(controllerId);
    pImpl->samplingRates.erase(controllerId);
    pImpl->retentionTimes.erase(controllerId);
    pImpl->maxDataPoints.erase(controllerId);
    pImpl->anomalyDetectionEnabled.erase(controllerId);
}

void MetricsCollector::clearAllData() {
    pImpl->metrics.clear();
    pImpl->timeSeries.clear();
    pImpl->experimentResults.clear();
    pImpl->learningCurves.clear();
    pImpl->collectingControllers.clear();
    pImpl->samplingRates.clear();
    pImpl->retentionTimes.clear();
    pImpl->maxDataPoints.clear();
    pImpl->anomalyDetectionEnabled.clear();
    
    NLM_LOG_INFO("All metrics data cleared");
}

void MetricsCollector::pauseCollecting(const std::string& controllerId) {
    pImpl->globalPaused = true;
    NLM_LOG_INFO("Metrics collection paused for controller: " + controllerId);
}

void MetricsCollector::resumeCollecting(const std::string& controllerId) {
    pImpl->globalPaused = false;
    NLM_LOG_INFO("Metrics collection resumed for controller: " + controllerId);
}

std::vector<std::string> MetricsCollector::getCollectingControllerIds() const {
    return std::vector<std::string>(pImpl->collectingControllers.begin(), 
                                    pImpl->collectingControllers.end());
}

size_t MetricsCollector::getMetricCount(const std::string& controllerId, const std::string& metricName) const {
    auto it = pImpl->metrics.find(controllerId);
    if (it == pImpl->metrics.end() || it->second.find(metricName) == it->second.end()) {
        return 0;
    }
    return it->second.at(metricName).getAgeSeconds() > 0 ? 1 : 0; // Simplified
}

double MetricsCollector::getControllerDataAge(const std::string& controllerId) const {
    if (!isCollecting(controllerId)) return 0.0;
    
    double maxAge = 0.0;
    for (const auto& pair : pImpl->metrics) {
        if (pair.first == controllerId) {
            for (const auto& metricPair : pair.second) {
                maxAge = std::max(maxAge, metricPair.second.getAgeSeconds());
            }
        }
    }
    
    return maxAge;
}

size_t MetricsCollector::getTotalMetricCount() const {
    size_t total = 0;
    for (const auto& pair : pImpl->metrics) {
        total += pair.second.size();
    }
    return total;
}

size_t MetricsCollector::getTotalAnomalyCount() const {
    // TODO: Implement anomaly counting
    return 0;
}

void MetricsCollector::printMetricsSummary(const std::string& controllerId) const {
    std::cout << generateMetricsReport(controllerId);
}

void MetricsCollector::printAnomalyReport(const std::string& controllerId) const {
    std::cout << "=== Anomaly Report for Controller: " << controllerId << " ===" << std::endl;
    std::cout << "Anomaly detection enabled: " << (pImpl->anomalyDetectionEnabled.count(controllerId) ? 
        (pImpl->anomalyDetectionEnabled.at(controllerId) ? "yes" : "no") : "no") << std::endl;
    std::cout << "Total anomalies: " << getTotalAnomalyCount() << std::endl;
    std::cout << "===================================" << std::endl;
}

void MetricsCollector::generateSystemReport() const {
    std::cout << "=== System Metrics Report ===" << std::endl;
    std::cout << "Total controllers: " << getCollectingControllerIds().size() << std::endl;
    std::cout << "Total metrics: " << getTotalMetricCount() << std::endl;
    std::cout << "Total anomalies: " << getTotalAnomalyCount() << std::endl;
    
    std::cout << std::endl << "Controllers:" << std::endl;
    for (const auto& controllerId : getCollectingControllerIds()) {
        std::cout << "  - " << controllerId << ": " << getMetricCount(controllerId, "") 
                  << " metrics, age: " << getControllerDataAge(controllerId) << "s" << std::endl;
    }
    
    std::cout << std::endl << "====================================" << std::endl;
}

// Metrics convenience functions
namespace metrics {
    MetricsCollector& getCollector() {
        static MetricsCollector collector;
        return collector;
    }
    
    void record(const std::string& name, float value) {
        getCollector().recordGauge("global", name, value);
    }
    
    void record(const std::string& controllerId, const std::string& name, float value) {
        getCollector().recordGauge(controllerId, name, value);
    }
    
    void recordCounter(const std::string& name, float value) {
        getCollector().recordCounter("global", name, value);
    }
    
    void recordGauge(const std::string& name, float value) {
        getCollector().recordGauge("global", name, value);
    }
    
    void recordTimer(const std::string& name, float duration) {
        getCollector().recordTimer("global", name, duration);
    }
    
    void recordBrainControllerMetrics(const std::string& controllerId, const BrainController& controller) {
        getCollector().recordBrainControllerMetrics(controllerId, controller);
    }
    
    void recordExperimentMetrics(const std::string& experimentId, const std::map<std::string, float>& metrics) {
        for (const auto& pair : metrics) {
            getCollector().recordGauge("experiments", pair.first, pair.second);
        }
    }
    
    std::string getReport(const std::string& controllerId) {
        return getCollector().generateMetricsReport(controllerId);
    }
    
    bool exportToCSV(const std::string& filepath, const std::string& controllerId) {
        return getCollector().exportMetricsToCSV(filepath, controllerId);
    }
    
    bool exportToJSON(const std::string& filepath, const std::string& controllerId) {
        return getCollector().exportMetricsToJSON(filepath, controllerId);
    }
    
    void setSamplingRate(float samplesPerSecond) {
        getCollector().setSamplingRate("global", samplesPerSecond);
    }
    
    void setRetentionTime(double retentionTime) {
        getCollector().setMetricRetentionTime("global", retentionTime);
    }
    
    void clearAll() {
        getCollector().clearAllData();
    }
    
    bool registerMetricsPlugin(std::unique_ptr<plugins::IntegrationPlugin> plugin) {
        // TODO: Implement plugin registration
        NLM_LOG_WARNING("Plugin registration not yet implemented");
        return false;
    }
    
    bool unregisterMetricsPlugin(const std::string& name) {
        // TODO: Implement plugin unregistration
        NLM_LOG_WARNING("Plugin unregistration not yet implemented");
        return false;
    }
}

// Global metrics instance
namespace {
    MetricsCollector globalMetricsCollector;
}

MetricsCollector& getGlobalMetricsCollector() {
    return globalMetricsCollector;
}

// Utility functions
std::string generateMetricName(const std::string& prefix, const std::string& suffix) {
    if (suffix.empty()) {
        return prefix;
    }
    return prefix + "_" + suffix;
}

std::string formatMetricValue(float value, const std::string& unit) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << value;
    if (!unit.empty()) {
        oss << " " << unit;
    }
    return oss.str();
}

double calculateMean(const std::vector<float>& values) {
    if (values.empty()) return 0.0;
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return static_cast<double>(sum) / values.size();
}

float calculateStdDev(const std::vector<float>& values, float mean) {
    if (values.size() < 2) return 0.0f;
    
    float sumSquaredDiff = 0.0f;
    for (float value : values) {
        float diff = value - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return std::sqrt(sumSquaredDiff / static_cast<float>(values.size() - 1));
}

std::vector<size_t> findOutliers(const std::vector<float>& values, float threshold) {
    std::vector<size_t> outliers;
    if (values.size() < 2) return outliers;
    
    float mean = calculateMean(values);
    float stdDev = calculateStdDev(values, mean);
    
    for (size_t i = 0; i < values.size(); ++i) {
        float zScore = std::abs(values[i] - mean) / stdDev;
        if (zScore > threshold) {
            outliers.push_back(i);
        }
    }
    
    return outliers;
}

std::string getTimeString(double timestamp) {
    auto timePoint = std::chrono::steady_clock::now() - std::chrono::duration<double>(timestamp);
    auto time = std::chrono::system_clock::to_time_t(timePoint);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
