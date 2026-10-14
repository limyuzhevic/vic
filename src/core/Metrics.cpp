#include "core/Metrics.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <functional>

namespace nlm {

// Metric implementation
class CounterMetric : public Metric {
private:
    std::string name_;
    std::string unit_;
    double value_;
    std::string timestamp_;
    mutable std::mutex mutex_;
    
public:
    CounterMetric(const std::string& name, const std::string& unit, double initial = 0.0)
        : name_(name), unit_(unit), value_(initial) {
        updateTimestamp();
    }
    
    std::string getName() const override { return name_; }
    std::string getUnit() const override { return unit_; }
    double getValue() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }
    std::string getTimestamp() const override { return timestamp_; }
    MetricType getType() const override { return MetricType::Counter; }
    
    void increment(double amount = 1.0) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ += amount;
        updateTimestamp();
    }
    
    void set(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = value;
        updateTimestamp();
    }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << name_ << ": " << value_ << " " << unit_;
        return ss.str();
    }
    
    nlohmann::json toJSON() const override {
        return nlohmann::json{
            {"name", name_},
            {"unit", unit_},
            {"value", value_},
            {"timestamp", timestamp_},
            {"type", "counter"}
        };
    }
    
private:
    void updateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        timestamp_ = ss.str();
    }
};

class GaugeMetric : public Metric {
private:
    std::string name_;
    std::string unit_;
    double value_;
    std::string timestamp_;
    mutable std::mutex mutex_;
    
public:
    GaugeMetric(const std::string& name, const std::string& unit, double initial = 0.0)
        : name_(name), unit_(unit), value_(initial) {
        updateTimestamp();
    }
    
    std::string getName() const override { return name_; }
    std::string getUnit() const override { return unit_; }
    double getValue() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }
    std::string getTimestamp() const override { return timestamp_; }
    MetricType getType() const override { return MetricType::Gauge; }
    
    void set(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = value;
        updateTimestamp();
    }
    
    void increment(double amount = 1.0) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ += amount;
        updateTimestamp();
    }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << name_ << ": " << value_ << " " << unit_;
        return ss.str();
    }
    
    nlohmann::json toJSON() const override {
        return nlohmann::json{
            {"name", name_},
            {"unit", unit_},
            {"value", value_},
            {"timestamp", timestamp_},
            {"type", "gauge"}
        };
    }
    
private:
    void updateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        timestamp_ = ss.str();
    }
};

class RateMetric : public Metric {
private:
    std::string name_;
    std::string unit_;
    double value_;
    std::string timestamp_;
    mutable std::mutex mutex_;
    double duration_;
    
public:
    RateMetric(const std::string& name, const std::string& unit, double initial = 0.0, double duration = 1.0)
        : name_(name), unit_(unit), value_(initial), duration_(duration) {
        updateTimestamp();
    }
    
    std::string getName() const override { return name_; }
    std::string getUnit() const override { return unit_ + "/" + std::to_string(duration_) + "s"; }
    double getValue() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }
    std::string getTimestamp() const override { return timestamp_; }
    MetricType getType() const override { return MetricType::Rate; }
    
    void set(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = value;
        updateTimestamp();
    }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << name_ << ": " << value_ << " " << getUnit();
        return ss.str();
    }
    
    nlohmann::json toJSON() const override {
        return nlohmann::json{
            {"name", name_},
            {"unit", getUnit()},
            {"value", value_},
            {"timestamp", timestamp_},
            {"type", "rate"},
            {"duration", duration_}
        };
    }
    
private:
    void updateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        timestamp_ = ss.str();
    }
};

// Metrics collector implementation
class DefaultMetricsCollector : public MetricsCollector {
private:
    std::vector<std::shared_ptr<Metric>> metrics_;
    double collectionInterval_;
    std::chrono::steady_clock::time_point lastCollection_;
    bool collecting_;
    std::mutex mutex_;
    
public:
    DefaultMetricsCollector() : collectionInterval_(1.0), collecting_(false) {}
    
    void registerMetric(const std::string& name, std::shared_ptr<Metric> metric) override {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.push_back(metric);
    }
    
    void unregisterMetric(const std::string& name) override {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.erase(
            std::remove_if(metrics_.begin(), metrics_.end(),
                          [&name](const std::shared_ptr<Metric>& metric) {
                              return metric->getName() == name;
                          }),
            metrics_.end()
        );
    }
    
    void startCollecting() override {
        std::lock_guard<std::mutex> lock(mutex_);
        collecting_ = true;
        lastCollection_ = std::chrono::steady_clock::now();
    }
    
    void stopCollecting() override {
        std::lock_guard<std::mutex> lock(mutex_);
        collecting_ = false;
    }
    
    std::vector<std::shared_ptr<Metric>> getMetrics() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return metrics_;
    }
    
    std::shared_ptr<Metric> getMetric(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& metric : metrics_) {
            if (metric->getName() == name) {
                return metric;
            }
        }
        return nullptr;
    }
    
    void setCollectionInterval(double interval) override {
        std::lock_guard<std::mutex> lock(mutex_);
        collectionInterval_ = interval;
    }
    
    void setRetentionPeriod(double period) override {
        // Implementation would depend on storage system
    }
    
    bool collect() override {
        if (!collecting_) {
            return false;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastCollection_).count();
        
        if (elapsed < collectionInterval_) {
            return false; // Not time to collect yet
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        lastCollection_ = now;
        
        // Collect metrics from all registered sources
        for (const auto& metric : metrics_) {
            // In a real implementation, this would call actual collection functions
            // For now, we just update timestamps
            updateMetricTimestamp(metric);
        }
        
        return true;
    }
    
    bool saveToFile(const std::string& filepath) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "NLM Metrics Data" << std::endl;
        file << "===================" << std::endl;
        file << std::endl;
        
        for (const auto& metric : metrics_) {
            file << metric->toString() << std::endl;
        }
        
        return true;
    }
    
    bool saveToDatabase(const std::string& connectionString) override {
        // Implementation would depend on database system
        return false;
    }
    
    std::string generateReport() const override {
        std::stringstream ss;
        ss << "NLM Metrics Report" << std::endl;
        ss << "===================" << std::endl;
        ss << std::endl;
        ss << "Total metrics collected: " << metrics_.size() << std::endl;
        ss << std::endl;
        
        for (const auto& metric : metrics_) {
            ss << metric->getName() << ": " << metric->getValue() << " " << metric->getUnit()
               << " (last updated: " << metric->getTimestamp() << ")" << std::endl;
        }
        
        return ss.str();
    }
    
private:
    void updateMetricTimestamp(std::shared_ptr<Metric> metric) {
        // This is a simplified implementation
        // In a real implementation, this would update the metric's timestamp
        // based on actual collection timing
    }
};

// Metrics processor implementation
class DefaultMetricsProcessor : public MetricsProcessor {
public:
    double calculateMean(const std::vector<double>& values) const override {
        if (values.empty()) return 0.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    double calculateStandardDeviation(const std::vector<double>& values) const override {
        if (values.size() <= 1) return 0.0;
        double mean = calculateMean(values);
        double sum = std::accumulate(values.begin(), values.end(), 0.0,
                                    [mean](double acc, double val) {
                                        return acc + (val - mean) * (val - mean);
                                    });
        return std::sqrt(sum / (values.size() - 1));
    }
    
    double calculateMedian(const std::vector<double>& values) const override {
        if (values.empty()) return 0.0;
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        size_t size = sorted.size();
        if (size % 2 == 0) {
            return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
        } else {
            return sorted[size / 2];
        }
    }
    
    std::vector<double> calculatePercentiles(const std::vector<double>& values,
                                              const std::vector<double>& percentiles) const override {
        std::vector<double> result;
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        
        for (double percentile : percentiles) {
            double index = (percentile / 100.0) * (sorted.size() - 1);
            size_t lower = static_cast<size_t>(std::floor(index));
            size_t upper = static_cast<size_t>(std::ceil(index));
            
            if (lower == upper) {
                result.push_back(sorted[lower]);
            } else {
                double weight = index - lower;
                result.push_back(sorted[lower] * (1.0 - weight) + sorted[upper] * weight);
            }
        }
        
        return result;
    }
    
    std::pair<double, double> calculateMinMax(const std::vector<double>& values) const override {
        if (values.empty()) return std::make_pair(0.0, 0.0);
        double min = *std::min_element(values.begin(), values.end());
        double max = *std::max_element(values.begin(), values.end());
        return std::make_pair(min, max);
    }
    
    std::string generateReport(const std::vector<std::shared_ptr<Metric>>& metrics,
                               const std::string& format = "text") const override {
        if (format == "text") {
            return generateTextReport(metrics);
        } else if (format == "json") {
            return generateJSONReport(metrics);
        } else if (format == "csv") {
            return generateCSVReport(metrics);
        } else {
            return generateTextReport(metrics);
        }
    }
    
    std::string generateTrendAnalysis(const std::vector<std::shared_ptr<Metric>>& metrics) const override {
        std::stringstream ss;
        ss << "Trend Analysis" << std::endl;
        ss << "===============" << std::endl;
        ss << std::endl;
        
        for (const auto& metric : metrics) {
            ss << metric->getName() << ": " << getTrendDescription(metric) << std::endl;
        }
        
        return ss.str();
    }
    
private:
    std::string generateTextReport(const std::vector<std::shared_ptr<Metric>>& metrics) const {
        std::stringstream ss;
        ss << "NLM Metrics Summary" << std::endl;
        ss << "====================" << std::endl;
        ss << std::endl;
        
        for (const auto& metric : metrics) {
            ss << metric->getName() << ": " << metric->getValue() << " " << metric->getUnit()
               << " (" << metricTypeToString(metric->getType()) << ")" << std::endl;
        }
        
        return ss.str();
    }
    
    std::string generateJSONReport(const std::vector<std::shared_ptr<Metric>>& metrics) const {
        nlohmann::json jsonData = metrics;
        return jsonData.dump(2);
    }
    
    std::string generateCSVReport(const std::vector<std::shared_ptr<Metric>>& metrics) const {
        std::stringstream ss;
        ss << "Metric,Value,Unit,Timestamp,Type" << std::endl;
        
        for (const auto& metric : metrics) {
            ss << metric->getName() << ","
               << metric->getValue() << ","
               << metric->getUnit() << ","
               << metric->getTimestamp() << ","
               << metricTypeToString(metric->getType()) << std::endl;
        }
        
        return ss.str();
    }
    
    std::string metricTypeToString(MetricType type) const {
        switch (type) {
            case MetricType::Counter: return "counter";
            case MetricType::Gauge: return "gauge";
            case MetricType::Rate: return "rate";
            case MetricType::Histogram: return "histogram";
            case MetricType::Summary: return "summary";
            default: return "unknown";
        }
    }
    
    std::string getTrendDescription(std::shared_ptr<Metric> metric) const {
        // This is a simplified implementation
        // In a real implementation, this would analyze historical data to determine trends
        return "stable";
    }
};

// Metrics storage implementation
class FileMetricsStorage : public MetricsStorage {
private:
    std::string filepath_;
    size_t maxSize_;
    double retentionPeriod_;
    
public:
    FileMetricsStorage(const std::string& filepath, size_t maxSize = 1000, double retentionPeriod = 30.0)
        : filepath_(filepath), maxSize_(maxSize), retentionPeriod_(retentionPeriod) {}
    
    bool store(const std::vector<std::shared_ptr<Metric>>& metrics) override {
        std::ofstream file(filepath_, std::ios::app);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Metric Storage: " << metrics.size() << " metrics" << std::endl;
        file << "Timestamp: " << getCurrentTimestamp() << std::endl;
        file << std::endl;
        
        for (const auto& metric : metrics) {
            file << metric->toString() << std::endl;
        }
        
        return true;
    }
    
    std::vector<std::shared_ptr<Metric>> load(const std::string& query) override {
        std::vector<std::shared_ptr<Metric>> metrics;
        
        std::ifstream file(filepath_);
        if (!file.is_open()) {
            return metrics;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("Metric:") != std::string::npos) {
                // Parse metric from file (simplified implementation)
                // In a real implementation, this would parse stored metrics
            }
        }
        
        return metrics;
    }
    
    bool clear() override {
        std::ofstream file(filepath_);
        if (!file.is_open()) {
            return false;
        }
        file.close();
        return true;
    }
    
    size_t size() const override {
        // Implementation would depend on storage system
        return 0;
    }
    
    std::string getStorageType() const override {
        return "file";
    }
    
    std::string getStorageInfo() const override {
        std::stringstream ss;
        ss << "File: " << filepath_ << std::endl;
        ss << "Max size: " << maxSize_ << std::endl;
        ss << "Retention period: " << retentionPeriod_ << " days";
        return ss.str();
    }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Metrics visualization implementation
class ConsoleMetricsVisualizer : public MetricsVisualizer {
public:
    void createTimeSeriesChart(const std::string& title,
                               const std::vector<std<std::shared_ptr<Metric>>>& metrics,
                               const std::string& outputFile) override {
        std::cout << "Time Series Chart: " << title << std::endl;
        std::cout << "====================" << std::endl;
        
        for (const auto& metric : metrics) {
            std::cout << metric->getName() << ": " << metric->getValue() << " " << metric->getUnit()
                     << " (timestamp: " << metric->getTimestamp() << ")" << std::endl;
        }
        
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "Time series data would be saved here" << std::endl;
            }
        }
    }
    
    void createHistogram(const std::string& title,
                         const std::vector<double>& values,
                         const std::string& outputFile) override {
        std::cout << "Histogram: " << title << std::endl;
        std::cout << "================" << std::endl;
        
        // Simple histogram printing
        if (!values.empty()) {
            double min = *std::min_element(values.begin(), values.end());
            double max = *std::max_element(values.begin(), values.end());
            double range = max - min;
            
            if (range > 0) {
                std::vector<size_t> buckets(10, 0);
                for (double value : values) {
                    size_t bucket = static_cast<size_t>(((value - min) / range) * 9);
                    if (bucket >= buckets.size()) bucket = buckets.size() - 1;
                    buckets[bucket]++;
                }
                
                for (size_t i = 0; i < buckets.size(); ++i) {
                    double bucketMin = min + (i * range / buckets.size());
                    double bucketMax = min + ((i + 1) * range / buckets.size());
                    std::cout << std::fixed << std::setprecision(2)
                             << "[" << bucketMin << ", " << bucketMax << "): "
                             << buckets[i] << " items" << std::endl;
                }
            }
        }
        
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "Histogram data would be saved here" << std::endl;
            }
        }
    }
    
    void createComparisonChart(const std::string& title,
                               const std::vector<std::vector<std::shared_ptr<Metric>>>& datasets,
                               const std::vector<std::string>& labels,
                               const std::string& outputFile) override {
        std::cout << "Comparison Chart: " << title << std::endl;
        std::cout << "===================" << std::endl;
        
        if (datasets.size() == labels.size() && !datasets.empty()) {
            for (size_t i = 0; i < datasets.size(); ++i) {
                std::cout << labels[i] << ":" << std::endl;
                for (const auto& metric : datasets[i]) {
                    std::cout << "  " << metric->getName() << ": " << metric->getValue()
                             << " " << metric->getUnit() << std::endl;
                }
            }
        }
        
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "Comparison data would be saved here" << std::endl;
            }
        }
    }
    
    void createDashboard(const std::string& title,
                         const std::vector<std::shared_ptr<Metric>>& metrics,
                         const std::string& outputFile) override {
        std::cout << "Dashboard: " << title << std::endl;
        std::cout << "=============" << std::endl;
        
        double totalMetrics = static_cast<double>(metrics.size());
        std::cout << "Total metrics: " << totalMetrics << std::endl;
        
        if (!metrics.empty()) {
            double sum = std::accumulate(metrics.begin(), metrics.end(), 0.0,
                                        [](double acc, const std::shared_ptr<Metric>& metric) {
                                            return acc + metric->getValue();
                                        });
            double average = sum / totalMetrics;
            std::cout << "Average value: " << average << std::endl;
        }
        
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "Dashboard would be saved here" << std::endl;
            }
        }
    }
};

// Metrics report exporter implementation
class ConsoleReportExporter : public ReportExporter {
public:
    bool exportToText(const std::vector<std::shared_ptr<Metric>>& metrics,
                     const std::string& outputFile) override {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << "NLM Metrics Report" << std::endl;
        file << "====================" << std::endl;
        file << std::endl;
        
        for (const auto& metric : metrics) {
            file << metric->toString() << std::endl;
        }
        
        return true;
    }
    
    bool exportToJSON(const std::vector<std::shared_ptr<Metric>>& metrics,
                     const std::string& outputFile) override {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        jsonData["metrics"] = metrics;
        file << jsonData.dump(2);
        
        return true;
    }
    
    bool exportToCSV(const std::vector<std::shared_ptr<Metric>>& metrics,
                    const std::string& outputFile) override {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Metric,Value,Unit,Timestamp,Type" << std::endl;
        
        for (const auto& metric : metrics) {
            file << metric->getName() << ","
                 << metric->getValue() << ","
                 << metric->getUnit() << ","
                 << metric->getTimestamp() << ","
                 << static_cast<int>(metric->getType()) << std::endl;
        }
        
        return true;
    }
    
    bool exportToXML(const std::vector<std::shared_ptr<Metric>>& metrics,
                    const std::string& outputFile) override {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            return false;
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        file << "<metrics>" << std::endl;
        
        for (const auto& metric : metrics) {
            file << "  <metric>" << std::endl;
            file << "    <name>" << metric->getName() << "</name>" << std::endl;
            file << "    <value>" << metric->getValue() << "</value>" << std::endl;
            file << "    <unit>" << metric->getUnit() << "</unit>" << std::endl;
            file << "    <timestamp>" << metric->getTimestamp() << "</timestamp>" << std::endl;
            file << "    <type>" << static_cast<int>(metric->getType()) << "</type>" << std::endl;
            file << "  </metric>" << std::endl;
        }
        
        file << "</metrics>" << std::endl;
        
        return true;
    }
};

// Global factory functions
std::shared_ptr<MetricsCollector> createMetricsCollector() {
    return std::make_shared<DefaultMetricsCollector>();
}

std::shared_ptr<MetricsStorage> createMetricsStorage(const std::string& filepath) {
    return std::make_shared<FileMetricsStorage>(filepath);
}

std::shared_ptr<MetricsProcessor> createMetricsProcessor() {
    return std::make_shared<DefaultMetricsProcessor>();
}

std::shared_ptr<MetricsVisualizer> createMetricsVisualizer() {
    return std::make_shared<ConsoleMetricsVisualizer>();
}

std::shared_ptr<ReportExporter> createReportExporter() {
    return std::make_shared<ConsoleReportExporter>();
}

} // namespace nlm
