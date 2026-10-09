// MetricsCollector implementation for NLM
#include "utils/MetricsCollector.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <thread>
#include <atomic>
#include <filesystem>
#include <limits>

namespace nlm {

struct MetricsCollector::Impl {
    MetricsCollectorConfig config;
    std::atomic<bool> collecting{false};
    std::atomic<bool> paused{false};
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastCollectionTime;
    
    // Data storage
    std::unordered_map<std::string, std::vector<MetricSample>> metricHistory;
    std::unordered_map<std::string, MetricAggregation> currentAggregations;
    std::vector<MetricsAlert> alerts;
    
    // Statistics
    std::atomic<size_t> totalRecords{0};
    size_t successfulRecords{0};
    size_t failedRecords{0};
    
    // Collection thread
    std::thread collectionThread;
    std::mutex metricsMutex;
    std::mutex aggregationsMutex;
    std::mutex alertsMutex;
    
    // Alert thresholds and messages
    std::unordered_map<std::string, double> alertThresholds;
    std::unordered_map<std::string, std::string> alertMessages;
};

MetricsCollector::MetricsCollector() : collecting(false), paused(false) {
    pImpl = std::make_unique<Impl>();
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastCollectionTime = pImpl->startTime;
}

MetricsCollector::~MetricsCollector() {
    stopCollecting();
}

MetricsCollector::MetricsCollector(MetricsCollector&& other) noexcept
    : pImpl(std::move(other.pImpl)), config(std::move(other.config)),
      collecting(other.collecting.load()), paused(other.paused.load()) {
    other.collecting.store(false);
    other.paused.store(false);
}

MetricsCollector& MetricsCollector::operator=(MetricsCollector&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        config = std::move(other.config);
        collecting = other.collecting.load();
        paused = other.paused.load();
        other.collecting.store(false);
        other.paused.store(false);
    }
    return *this;
}

bool MetricsCollector::configure(const MetricsCollectorConfig& cfg) {
    if (!cfg.validate()) {
        std::cerr << "Error: Invalid metrics collector configuration" << std::endl;
        return false;
    }
    
    pImpl->config = cfg;
    pImpl->config.startTime = std::chrono::steady_clock::now();
    pImpl->alertThresholds = cfg.alertThresholds;
    pImpl->alertMessages = cfg.alertMessages;
    
    return true;
}

bool MetricsCollector::startCollecting() {
    if (pImpl->collecting) {
        std::cout << "Metrics collection is already running" << std::endl;
        return true;
    }
    
    if (!pImpl->config.enabled) {
        std::cout << "Metrics collection is disabled" << std::endl;
        return false;
    }
    
    pImpl->collecting = true;
    pImpl->paused = false;
    
    std::thread collectorThread(&MetricsCollector::collectionWorker, this);
    pImpl->collectionThread = std::move(collectorThread);
    
    std::cout << "Metrics collection started" << std::endl;
    return true;
}

bool MetricsCollector::stopCollecting() {
    if (!pImpl->collecting) {
        std::cout << "Metrics collection is not running" << std::endl;
        return true;
    }
    
    pImpl->collecting = false;
    
    if (pImpl->collectionThread.joinable()) {
        pImpl->collectionThread.join();
    }
    
    std::cout << "Metrics collection stopped" << std::endl;
    return true;
}

bool MetricsCollector::pauseCollecting() {
    if (!pImpl->collecting) {
        std::cout << "Metrics collection is not running" << std::endl;
        return true;
    }
    
    pImpl->paused = true;
    std::cout << "Metrics collection paused" << std::endl;
    return true;
}

bool MetricsCollector::resumeCollecting() {
    if (!pImpl->collecting) {
        std::cout << "Metrics collection is not running" << std::endl;
        return true;
    }
    
    pImpl->paused = false;
    std::cout << "Metrics collection resumed" << std::endl;
    return true;
}

void MetricsCollector::recordMetric(const MetricSample& sample) {
    if (!pImpl->config.enabled || !sample.isValid()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->metricHistory[sample.name].push_back(sample);
    pImpl->totalRecords++;
    pImpl->successfulRecords++;
    pImpl->lastCollectionTime = std::chrono::steady_clock::now();
    
    // Update aggregation
    updateAggregation(sample.name, sample);
    
    // Check for alerts
    checkForAlerts();
}

void MetricsCollector::recordMetric(const std::string& name, MetricType type, const std::variant<size_t, double, std::string, bool>& value) {
    MetricSample sample;
    sample.name = name;
    sample.type = type;
    sample.value = value;
    sample.timestamp = std::chrono::steady_clock::now();
    sample.sampleId = pImpl->totalRecords;
    
    recordMetric(sample);
}

void MetricsCollector::recordMetric(const std::string& name, MetricType type, const std::variant<size_t, double, std::string, bool>& value,
                                     const std::unordered_map<std::string, std::string>& tags) {
    MetricSample sample;
    sample.name = name;
    sample.type = type;
    sample.value = value;
    sample.timestamp = std::chrono::steady_clock::now();
    sample.sampleId = pImpl->totalRecords;
    sample.tags = tags;
    
    recordMetric(sample);
}

void MetricsCollector::recordTimer(const std::string& name, std::chrono::duration<double> duration,
                                    const std::unordered_map<std::string, std::string>& tags) {
    double value = duration.count();
    recordMetric(name, MetricType::Timer, value, tags);
}

void MetricsCollector::recordCounter(const std::string& name, size_t count,
                                      const std::unordered_map<std::string, std::string>& tags) {
    recordMetric(name, MetricType::Counter, count, tags);
}

void MetricsCollector::recordGauge(const std::string& name, double value,
                                    const std::unordered_map<std::string, std::string>& tags) {
    recordMetric(name, MetricType::Gauge, value, tags);
}

void MetricsCollector::recordHistogram(const std::string& name, double value,
                                        const std::unordered_map<std::string, std::string>& tags) {
    // Check if histogram aggregation exists, create if not
    auto aggIt = pImpl->currentAggregations.find(name);
    if (aggIt == pImpl->currentAggregations.end() || aggIt->second.type != MetricType::Histogram) {
        createHistogramAggregation(name, pImpl->config.histogramRanges[name].first, 
                                   pImpl->config.histogramRanges[name].second, 10);
    }
    
    recordMetric(name, MetricType::Histogram, value, tags);
}

std::vector<MetricAggregation> MetricsCollector::getAggregations(const std::string& name, 
                                                                  const std::chrono::steady_clock::time_point& startTime,
                                                                  const std::chrono::steady_clock::time_point& endTime) {
    std::lock_guard<std::mutex> lock(pImpl->aggregationsMutex);
    std::vector<MetricAggregation> result;
    
    if (name.empty()) {
        // Return all aggregations
        for (auto& pair : pImpl->currentAggregations) {
            result.push_back(pair.second);
        }
    } else {
        auto it = pImpl->currentAggregations.find(name);
        if (it != pImpl->currentAggregations.end()) {
            result.push_back(it->second);
        }
    }
    
    return result;
}

MetricAggregation MetricsCollector::getCurrentAggregation(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->aggregationsMutex);
    
    auto it = pImpl->currentAggregations.find(name);
    if (it != pImpl->currentAggregations.end()) {
        return it->second;
    }
    
    MetricAggregation empty;
    empty.name = name;
    return empty;
}

std::vector<MetricSample> MetricsCollector::getMetricHistory(const std::string& name, size_t limit) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<MetricSample> result;
    
    if (name.empty()) {
        for (auto& pair : pImpl->metricHistory) {
            auto& samples = pair.second;
            if (limit == 0 || samples.size() <= limit) {
                result.insert(result.end(), samples.begin(), samples.end());
            } else {
                result.insert(result.end(), samples.end() - limit, samples.end());
            }
        }
    } else {
        auto it = pImpl->metricHistory.find(name);
        if (it != pImpl->metricHistory.end()) {
            auto& samples = it->second;
            if (limit == 0 || samples.size() <= limit) {
                result = samples;
            } else {
                result.assign(samples.end() - limit, samples.end());
            }
        }
    }
    
    // Sort by timestamp
    std::sort(result.begin(), result.end(),
              [](const MetricSample& a, const MetricSample& b) {
                  return a.timestamp < b.timestamp;
              });
    
    return result;
}

std::vector<std::string> MetricsCollector::getMetricNames() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    std::vector<std::string> names;
    names.reserve(pImpl->metricHistory.size());
    
    for (const auto& pair : pImpl->metricHistory) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<MetricsAlert> MetricsCollector::getAlerts() {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    return pImpl->alerts;
}

void MetricsCollector::acknowledgeAlert(size_t index) {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    if (index < pImpl->alerts.size()) {
        pImpl->alerts[index].acknowledged = true;
    }
}

void MetricsCollector::clearAlerts() {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    pImpl->alerts.clear();
}

std::string MetricsCollector::generateReport(const std::string& name) {
    if (pImpl->config.format == "json") {
        return generateJSONReport(name);
    } else if (pImpl->config.format == "csv") {
        return generateCSVReport(name);
    } else if (pImpl->config.format == "html") {
        return generateHTMLReport(name);
    } else {
        return generateTextReport(name);
    }
}

bool MetricsCollector::exportMetrics(const std::string& filename, const std::string& format, 
                                      const std::string& name) {
    if (!ensureOutputDirectory(filename)) {
        return false;
    }
    
    std::string content;
    if (format == "json") {
        content = generateJSONReport(name);
    } else if (format == "csv") {
        content = generateCSVReport(name);
    } else if (format == "html") {
        content = generateHTMLReport(name);
    } else {
        content = generateTextReport(name);
    }
    
    return writeToFile(filename, content);
}

std::unordered_map<std::string, double> MetricsCollector::getAnalysis(const std::string& name, const std::string& analysisType) {
    std::unordered_map<std::string, double> analysis;
    
    auto history = getMetricHistory(name);
    if (history.empty()) {
        return analysis;
    }
    
    std::vector<double> values;
    for (const auto& sample : history) {
        if (std::holds_alternative<size_t>(sample.value)) {
            values.push_back(static_cast<double>(std::get<size_t>(sample.value)));
        } else if (std::holds_alternative<double>(sample.value)) {
            values.push_back(std::get<double>(sample.value));
        }
    }
    
    if (values.empty()) {
        return analysis;
    }
    
    if (analysisType == "statistics") {
        analysis["count"] = static_cast<double>(values.size());
        analysis["mean"] = calculateMean(values);
        analysis["variance"] = calculateVariance(values);
        analysis["stddev"] = calculateStdDev(values);
        analysis["min"] = *std::min_element(values.begin(), values.end());
        analysis["max"] = *std::max_element(values.begin(), values.end());
    } else if (analysisType == "percentiles") {
        std::vector<double> percentiles = pImpl->config.defaultPercentiles;
        std::vector<double> results = calculatePercentiles(values, percentiles);
        for (size_t i = 0; i < percentiles.size(); ++i) {
            analysis[std::to_string(percentiles[i]) + "th"] = results[i];
        }
    } else if (analysisType == "trend") {
        if (values.size() >= 2) {
            double first = values.front();
            double last = values.back();
            double change = ((last - first) / first) * 100.0;
            analysis["change_percent"] = change;
            analysis["trend_direction"] = (change > 0.0) ? 1.0 : -1.0;
        }
    }
    
    return analysis;
}

std::vector<std::string> MetricsCollector::getTrendAnalysis(const std::string& name, size_t windowSize) {
    std::vector<std::string> trends;
    
    auto history = getMetricHistory(name, windowSize + 1);
    if (history.size() < 2) {
        return trends;
    }
    
    // Get values in chronological order
    std::vector<double> values;
    for (size_t i = history.size() - windowSize - 1; i < history.size() - 1; ++i) {
        const auto& sample = history[i];
        if (std::holds_alternative<size_t>(sample.value)) {
            values.push_back(static_cast<double>(std::get<size_t>(sample.value)));
        } else if (std::holds_alternative<double>(sample.value)) {
            values.push_back(std::get<double>(sample.value));
        }
    }
    
    if (values.size() < 2) {
        return trends;
    }
    
    // Calculate simple moving average
    std::vector<double> sma;
    for (size_t i = 1; i < values.size(); ++i) {
        double sum = 0.0;
        for (size_t j = std::max(0, static_cast<long>(i - windowSize + 1)); j <= i; ++j) {
            sum += values[j];
        }
        sma.push_back(sum / windowSize);
    }
    
    // Detect trend
    if (sma.size() >= 2) {
        double first = sma.front();
        double last = sma.back();
        double slope = (last - first) / (sma.size() - 1);
        
        if (slope > 0.01) {
            trends.push_back("Increasing trend detected");
        } else if (slope < -0.01) {
            trends.push_back("Decreasing trend detected");
        } else {
            trends.push_back("Stable trend detected");
        }
    }
    
    return trends;
}

void MetricsCollector::clearMetrics(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (name.empty()) {
        pImpl->metricHistory.clear();
    } else {
        pImpl->metricHistory.erase(name);
    }
}

void MetricsCollector::reset() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metricHistory.clear();
    pImpl->currentAggregations.clear();
    clearAlerts();
    pImpl->totalRecords = 0;
    pImpl->successfulRecords = 0;
    pImpl->failedRecords = 0;
    pImpl->lastCollectionTime = std::chrono::steady_clock::now();
}

void MetricsCollector::createHistogramAggregation(const std::string& name, double bucketMin, double bucketMax, size_t bucketCount) {
    MetricAggregation agg;
    agg.name = name;
    agg.type = MetricType::Histogram;
    agg.bucketMin = bucketMin;
    agg.bucketMax = bucketMax;
    agg.bucketCount = bucketCount;
    agg.histogramBuckets.resize(bucketCount, 0);
    agg.startTime = std::chrono::steady_clock::now();
    
    pImpl->currentAggregations[name] = agg;
}

void MetricsCollector::updateAggregation(const std::string& name, const MetricSample& sample) {
    auto aggIt = pImpl->currentAggregations.find(name);
    if (aggIt == pImpl->currentAggregations.end()) {
        return;
    }
    
    MetricAggregation& agg = aggIt->second;
    agg.update(sample);
    agg.endTime = std::chrono::steady_clock::now();
}

void MetricsCollector::collectionWorker() {
    while (pImpl->collecting) {
        if (pImpl->paused) {
            std::this_thread::sleep_for(pImpl->config.collectionInterval);
            continue;
        }
        
        // Cleanup old data
        cleanupOldMetrics();
        cleanupOldAggregations();
        
        // Check for alerts
        checkForAlerts();
        
        std::this_thread::sleep_for(pImpl->config.collectionInterval);
    }
}

void MetricsCollector::cleanupOldMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto now = std::chrono::steady_clock::now();
    auto maxAge = std::chrono::milliseconds(pImpl->config.maxMetricHistory);
    
    for (auto& pair : pImpl->metricHistory) {
        auto& samples = pair.second;
        auto it = std::remove_if(samples.begin(), samples.end(),
                                [now, maxAge](const MetricSample& sample) {
                                    auto age = std::chrono::duration_cast<std::chrono::milliseconds>(now - sample.timestamp);
                                    return age > maxAge;
                                });
        samples.erase(it, samples.end());
    }
}

void MetricsCollector::cleanupOldAggregations() {
    std::lock_guard<std::mutex> lock(pImpl->aggregationsMutex);
    
    auto now = std::chrono::steady_clock::now();
    auto maxAge = std::chrono::minutes(10);  // Aggregation cleanup every 10 minutes
    
    for (auto it = pImpl->currentAggregations.begin(); it != pImpl->currentAggregations.end(); ) {
        auto& agg = it->second;
        if (now - agg.endTime > maxAge) {
            it = pImpl->currentAggregations.erase(it);
        } else {
            ++it;
        }
    }
}

void MetricsCollector::checkForAlerts() {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    
    for (const auto& pair : pImpl->alertThresholds) {
        const std::string& name = pair.first;
        double threshold = pair.second;
        
        auto history = getMetricHistory(name, 1);  // Get latest sample
        if (history.empty()) continue;
        
        const auto& sample = history.back();
        double value = 0.0;
        
        if (std::holds_alternative<size_t>(sample.value)) {
            value = static_cast<double>(std::get<size_t>(sample.value));
        } else if (std::holds_alternative<double>(sample.value)) {
            value = std::get<double>(sample.value);
        }
        
        // Check threshold
        if (value > threshold) {
            auto alertIt = pImpl->alertMessages.find(name);
            std::string message = (alertIt != pImpl->alertMessages.end()) ? alertIt->second : 
                                 ("Metric " + name + " exceeded threshold: " + std::to_string(value) + " > " + std::to_string(threshold));
            
            MetricsAlert alert;
            alert.metricName = name;
            alert.currentValue = value;
            alert.threshold = threshold;
            alert.message = message;
            alert.severity = (value > threshold * 2.0) ? "ERROR" : "WARNING";
            alert.timestamp = std::chrono::steady_clock::now();
            alert.acknowledged = false;
            alert.tags = sample.tags;
            
            pImpl->alerts.push_back(alert);
        }
    }
}

std::string MetricsCollector::generateTextReport(const std::string& name) const {
    std::ostringstream oss;
    oss << "=== Metrics Collector Report ===" << std::endl;
    oss << "Generated on: " << getCurrentTimestamp() << std::endl;
    oss << "Collecting: " << (pImpl->collecting ? "Yes" : "No") << std::endl;
    oss << "Paused: " << (pImpl->paused ? "Yes" : "No") << std::endl;
    oss << std::endl;
    
    if (!name.empty()) {
        oss << "=== Metrics for " << name << " ===" << std::endl;
        auto analysis = getAnalysis(name, "statistics");
        
        if (!analysis.empty()) {
            for (const auto& pair : analysis) {
                oss << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        auto trends = getTrendAnalysis(name, 5);
        if (!trends.empty()) {
            oss << std::endl << "Trends:" << std::endl;
            for (const auto& trend : trends) {
                oss << "  - " << trend << std::endl;
            }
        }
    } else {
        oss << "=== All Metrics ===" << std::endl;
        auto names = getMetricNames();
        
        for (const auto& metricName : names) {
            auto analysis = getAnalysis(metricName, "statistics");
            if (!analysis.empty()) {
                oss << metricName << ": " << analysis["count"] << " samples, ";
                oss << "mean=" << analysis["mean"] << ", stddev=" << analysis["stddev"] << std::endl;
            }
        }
    }
    
    oss << std::endl << "=== Collection Statistics ===" << std::endl;
    oss << "Total Records: " << pImpl->totalRecords << std::endl;
    oss << "Successful Records: " << pImpl->successfulRecords << std::endl;
    oss << "Failed Records: " << pImpl->failedRecords << std::endl;
    oss << "Collection Efficiency: " << (pImpl->totalRecords > 0 ? (pImpl->successfulRecords * 100.0 / pImpl->totalRecords) : 0.0) << "%" << std::endl;
    oss << "Last Collection: " << getFormattedTime(pImpl->lastCollectionTime) << std::endl;
    
    auto alerts = getAlerts();
    auto activeAlerts = getActiveAlerts();
    if (!activeAlerts.empty()) {
        oss << std::endl << "=== Active Alerts ===" << std::endl;
        for (size_t i = 0; i < activeAlerts.size(); ++i) {
            const auto& alert = activeAlerts[i];
            oss << "[" << alert.severity << "] " << alert.message << " (" << alert.metricName << " = " << alert.currentValue 
                << " > " << alert.threshold << ")" << std::endl;
        }
    }
    
    return oss.str();
}

std::string MetricsCollector::generateJSONReport(const std::string& name) const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"report_type\": \"metrics_collector\"," << std::endl;
    oss << "  \"timestamp\": \"" << getCurrentTimestamp() << "\"," << std::endl;
    oss << "  \"collecting\": " << (pImpl->collecting ? "true" : "false") << "," << std::endl;
    oss << "  \"paused\": " << (pImpl->paused ? "true" : "false") << "," << std::endl;
    
    if (!name.empty()) {
        oss << "  \"metric_name\": \"" << name << "\"," << std::endl;
        auto analysis = getAnalysis(name, "statistics");
        
        oss << "  \"statistics\": {" << std::endl;
        bool first = true;
        for (const auto& pair : analysis) {
            if (!first) oss << ",\n";
            oss << "    \"" << pair.first << "\": " << pair.second;
            first = false;
        }
        oss << "}" << std::endl;
        
        auto trends = getTrendAnalysis(name, 5);
        if (!trends.empty()) {
            oss << "  \"trends\": [" << std::endl;
            for (size_t i = 0; i < trends.size(); ++i) {
                oss << "    \"" << trends[i] << "\"";
                if (i < trends.size() - 1) oss << ",";
                oss << std::endl;
            }
            oss << "  ]" << std::endl;
        }
    } else {
        auto names = getMetricNames();
        
        oss << "  \"metrics_summary\": [" << std::endl;
        for (size_t i = 0; i < names.size(); ++i) {
            auto analysis = getAnalysis(names[i], "statistics");
            if (!analysis.empty()) {
                oss << "    {" << std::endl;
                oss << "      \"name\": \"" << names[i] << "\"," << std::endl;
                oss << "      \"count\": " << analysis["count"] << "," << std::endl;
                oss << "      \"mean\": " << analysis["mean"] << "," << std::endl;
                oss << "      \"stddev\": " << analysis["stddev"] << std::endl;
                if (i < names.size() - 1) oss << "    }," << std::endl;
                else oss << "    }" << std::endl;
            }
        }
        oss << "  ]" << std::endl;
    }
    
    oss << "}" << std::endl;
    return oss.str();
}

std::string MetricsCollector::generateCSVReport(const std::string& name) const {
    std::ostringstream oss;
    oss << "Metric,Value,Unit,Timestamp" << std::endl;
    
    if (!name.empty()) {
        auto analysis = getAnalysis(name, "statistics");
        
        for (const auto& pair : analysis) {
            oss << name << "," << pair.first << "," << getMetricUnit(name) << ","
                << getCurrentTimestamp() << std::endl;
        }
    } else {
        auto names = getMetricNames();
        
        for (const auto& metricName : names) {
            auto analysis = getAnalysis(metricName, "statistics");
            
            for (const auto& pair : analysis) {
                oss << metricName << "," << pair.first << "," << getMetricUnit(metricName) << ","
                    << getCurrentTimestamp() << std::endl;
            }
        }
    }
    
    return oss.str();
}

std::string MetricsCollector::generateHTMLReport(const std::string& name) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "<title>Metrics Collector Report</title>" << std::endl;
    oss << "<style>" << std::endl;
    oss << "table { border-collapse: collapse; width: 100%; }" << std::endl;
    oss << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "th { background-color: #f2f2f2; }" << std::endl;
    oss << "tr:nth-child(even) { background-color: #f9f9f9; }" << std::endl;
    oss << "</style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << "<h1>Metrics Collector Report</h1>" << std::endl;
    oss << "<p>Generated on: " << getCurrentTimestamp() << "</p>" << std::endl;
    
    if (!name.empty()) {
        oss << "<h2>Metrics for " << name << "</h2>" << std::endl;
        auto analysis = getAnalysis(name, "statistics");
        
        oss << "<table>" << std::endl;
        oss << "<tr><th>Metric</th><th>Value</th><th>Unit</th></tr>" << std::endl;
        
        for (const auto& pair : analysis) {
            oss << "<tr><td>" << pair.first << "</td><td>" << pair.second << "</td><td>" << getMetricUnit(name) << "</td></tr>" << std::endl;
        }
        oss << "</table>" << std::endl;
    }
    
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    return oss.str();
}

std::string MetricsCollector::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string MetricsCollector::getFormattedTime(const std::chrono::steady_clock::time_point& timestamp) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
    return std::to_string(elapsed) + "s";
}

std::string MetricsCollector::getMetricUnit(const std::string& name) const {
    // Determine unit based on metric name
    if (name.find("cpu") != std::string::npos) return "%";
    if (name.find("memory") != std::string::npos) return "MB";
    if (name.find("time") != std::string::npos || name.find("duration") != std::string::npos) return "seconds";
    if (name.find("rate") != std::string::npos) return "per second";
    if (name.find("count") != std::string::npos || name.find("total") != std::string::npos) return "count";
    if (name.find("percent") != std::string::npos) return "%";
    return "";
}

double MetricsCollector::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double MetricsCollector::calculateVariance(const std::vector<double>& values) const {
    if (values.size() < 2) return 0.0;
    
    double mean = calculateMean(values);
    double sum = 0.0;
    for (double value : values) {
        sum += (value - mean) * (value - mean);
    }
    return sum / (values.size() - 1);
}

double MetricsCollector::calculateStdDev(const std::vector<double>& values) const {
    double variance = calculateVariance(values);
    return std::sqrt(variance);
}

std::vector<double> MetricsCollector::calculatePercentiles(const std::vector<double>& values, const std::vector<double>& percentiles) const {
    if (values.empty()) return {};
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    std::vector<double> result;
    for (double p : percentiles) {
        double index = (p / 100.0) * (sorted.size() - 1);
        size_t lower = static_cast<size_t>(std::floor(index));
        size_t upper = static_cast<size_t>(std::ceil(index));
        
        if (lower == upper) {
            result.push_back(sorted[lower]);
        } else {
            double weight = index - lower;
            double percentileValue = sorted[lower] * (1.0 - weight) + sorted[upper] * weight;
            result.push_back(percentileValue);
        }
    }
    
    return result;
}

std::vector<MetricsAlert> MetricsCollector::getActiveAlerts() {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    std::vector<MetricsAlert> activeAlerts;
    for (const auto& alert : pImpl->alerts) {
        if (!alert.acknowledged) {
            activeAlerts.push_back(alert);
        }
    }
    return activeAlerts;
}

size_t MetricsCollector::getTotalMetricCount() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    size_t total = 0;
    for (const auto& pair : pImpl->metricHistory) {
        total += pair.second.size();
    }
    return total;
}

double MetricsCollector::getCollectionEfficiency() const {
    if (pImpl->totalRecords == 0) return 0.0;
    return static_cast<double>(pImpl->successfulRecords) * 100.0 / pImpl->totalRecords;
}

bool MetricsCollector::isHealthy() const {
    // Check if metrics collection is healthy
    if (!pImpl->collecting) {
        return false;
    }
    
    if (pImpl->metricHistory.empty()) {
        return false;
    }
    
    // Check if there are any failed records
    if (pImpl->failedRecords > pImpl->successfulRecords * 10) {
        return false;
    }
    
    return true;
}

std::string MetricsCollector::getHealthStatus() const {
    if (!pImpl->collecting) {
        return "Not collecting metrics";
    }
    
    if (pImpl->metricHistory.empty()) {
        return "No metrics collected";
    }
    
    if (pImpl->failedRecords > 0) {
        return "Metrics collection has failures";
    }
    
    return "Healthy";
}

// File I/O
bool MetricsCollector::writeToFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

bool MetricsCollector::ensureOutputDirectory(const std::string& filename) const {
    size_t pos = filename.find_last_of('/');
    if (pos != std::string::npos) {
        std::string directory = filename.substr(0, pos);
        if (!std::filesystem::exists(directory)) {
            return std::filesystem::create_directories(directory);
        }
    }
    return true;
}

} // namespace nlm