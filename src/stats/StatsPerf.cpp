#include "StatsPerf.hpp"
#include <numeric>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <sys/resource.h>
#include <unistd.h>
#include <cstring>

namespace nlm {
namespace stats {

// Histogram implementation
Histogram::Histogram(size_t bucketCount, double minValue, double maxValue)
    : minValue_(minValue), maxValue_(maxValue), totalCount_(0), totalSum_(0.0) {
    if (bucketCount == 0) {
        throw std::invalid_argument("Bucket count must be greater than 0");
    }
    
    if (minValue >= maxValue) {
        throw std::invalid_argument("Min value must be less than max value");
    }
    
    double bucketSize = (maxValue - minValue) / bucketCount;
    buckets_.reserve(bucketCount);
    
    for (size_t i = 0; i < bucketCount; ++i) {
        double lower = minValue + i * bucketSize;
        double upper = (i == bucketCount - 1) ? maxValue : lower + bucketSize;
        buckets_.emplace_back(lower, upper);
    }
}

void Histogram::addValue(double value) {
    if (value < minValue_ || value > maxValue_) {
        return; // Skip values outside range
    }
    
    size_t bucketIndex = findBucketIndex(value);
    if (bucketIndex < buckets_.size()) {
        Bucket& bucket = buckets_[bucketIndex];
        bucket.count++;
        bucket.sum += value;
        bucket.sumOfSquares += value * value;
    }
    
    totalCount_++;
    totalSum_ += value;
    
    if (value < minValue_) minValue_ = value;
    if (value > maxValue_) maxValue_ = value;
}

void Histogram::setBucketCount(size_t count) {
    if (count == 0) {
        throw std::invalid_argument("Bucket count must be greater than 0");
    }
    
    // Recalculate buckets with new count
    double bucketSize = (maxValue_ - minValue_) / count;
    buckets_.clear();
    buckets_.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        double lower = minValue_ + i * bucketSize;
        double upper = (i == count - 1) ? maxValue_ : lower + bucketSize;
        buckets_.emplace_back(lower, upper);
    }
}

void Histogram::setRange(double minValue, double maxValue) {
    if (minValue >= maxValue) {
        throw std::invalid_argument("Min value must be less than max value");
    }
    
    minValue_ = minValue;
    maxValue_ = maxValue;
    
    // Recalculate buckets with new range
    double bucketSize = (maxValue_ - minValue_) / buckets_.size();
    for (size_t i = 0; i < buckets_.size(); ++i) {
        double lower = minValue_ + i * bucketSize;
        double upper = (i == buckets_.size() - 1) ? maxValue_ : lower + bucketSize;
        buckets_[i] = Bucket(lower, upper);
    }
}

double Histogram::getMean() const {
    if (totalCount_ == 0) return 0.0;
    return totalSum_ / totalCount_;
}

double Histogram::getVariance() const {
    if (totalCount_ <= 1) return 0.0;
    return (totalSumOfSquares_ - (totalSum_ * totalSum_) / totalCount_) / (totalCount_ - 1);
}

double Histogram::getStandardDeviation() const {
    return std::sqrt(getVariance());
}

double Histogram::getPercentile(double percentile) const {
    if (percentile < 0.0 || percentile > 100.0 || totalCount_ == 0) {
        return 0.0;
    }
    
    double targetCount = (percentile / 100.0) * totalCount_;
    size_t currentCount = 0;
    
    for (const auto& bucket : buckets_) {
        currentCount += bucket.count;
        if (currentCount >= targetCount) {
            return bucket.sum / bucket.count;
        }
    }
    
    return buckets_.back().sum / buckets_.back().count;
}

size_t Histogram::findBucketIndex(double value) const {
    if (value <= minValue_) return 0;
    if (value >= maxValue_) return buckets_.size() - 1;
    
    double bucketSize = (maxValue_ - minValue_) / buckets_.size();
    size_t index = static_cast<size_t>((value - minValue_) / bucketSize);
    
    // Ensure index is within bounds
    return std::min(index, buckets_.size() - 1);
}

// StatisticsCollector implementation
void StatisticsCollector::recordCounter(const std::string& name, int64_t value,
                                       const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    MetricData* metric = getOrCreateMetric(name, StatisticsType::Counter);
    if (metric) {
        StatisticsDataPoint dataPoint;
        dataPoint.name = name;
        dataPoint.type = StatisticsType::Counter;
        dataPoint.value = static_cast<double>(value);
        dataPoint.tags = tags;
        dataPoint.unit = "count";
        dataPoint.description = "Counter metric";
        
        metric->dataPoints.push_back(dataPoint);
    }
}

void StatisticsCollector::recordGauge(const std::string& name, double value,
                                      const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    MetricData* metric = getOrCreateMetric(name, StatisticsType::Gauge);
    if (metric) {
        StatisticsDataPoint dataPoint;
        dataPoint.name = name;
        dataPoint.type = StatisticsType::Gauge;
        dataPoint.value = value;
        dataPoint.tags = tags;
        dataPoint.unit = "value";
        dataPoint.description = "Gauge metric";
        
        metric->dataPoints.push_back(dataPoint);
    }
}

void StatisticsCollector::recordHistogram(const std::string& name, double value,
                                          const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    MetricData* metric = getOrCreateMetric(name, StatisticsType::Histogram);
    if (metric) {
        StatisticsDataPoint dataPoint;
        dataPoint.name = name;
        dataPoint.type = StatisticsType::Histogram;
        dataPoint.value = value;
        dataPoint.tags = tags;
        dataPoint.unit = "value";
        dataPoint.description = "Histogram metric";
        
        metric->dataPoints.push_back(dataPoint);
    }
}

void StatisticsCollector::recordTimer(const std::string& name, std::chrono::duration<double> duration,
                                      const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    MetricData* metric = getOrCreateMetric(name, StatisticsType::Timer);
    if (metric) {
        StatisticsDataPoint dataPoint;
        dataPoint.name = name;
        dataPoint.type = StatisticsType::Timer;
        dataPoint.value = duration.count();
        dataPoint.tags = tags;
        dataPoint.unit = "seconds";
        dataPoint.description = "Timer metric";
        
        metric->dataPoints.push_back(dataPoint);
    }
}

void StatisticsCollector::recordDistribution(const std::string& name, const std::vector<double>& values,
                                             const std::map<std::string, std::string>& tags) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    MetricData* metric = getOrCreateMetric(name, StatisticsType::Distribution);
    if (metric) {
        StatisticsDataPoint dataPoint;
        dataPoint.name = name;
        dataPoint.type = StatisticsType::Distribution;
        dataPoint.value = values.size(); // Store count for now
        dataPoint.tags = tags;
        dataPoint.unit = "count";
        dataPoint.description = "Distribution metric";
        
        metric->dataPoints.push_back(dataPoint);
        
        // Store actual values in a separate field (would need extension)
    }
}

int64_t StatisticsCollector::getCounter(const std::string& name,
                                        const std::map<std::string, std::string>& filters) const {
    MetricData* metric = getMetric(name, StatisticsType::Counter);
    if (!metric) return 0;
    
    return static_cast<int64_t>(getFilteredDataPoints(*metric, filters).size());
}

double StatisticsCollector::getGauge(const std::string& name,
                                     const std::map<std::string, std::string>& filters) const {
    MetricData* metric = getMetric(name, StatisticsType::Gauge);
    if (!metric) return 0.0;
    
    auto filteredPoints = getFilteredDataPoints(*metric, filters);
    if (filteredPoints.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto* point : filteredPoints) {
        sum += point->value;
    }
    
    return sum / filteredPoints.size();
}

Histogram StatisticsCollector::getHistogram(const std::string& name,
                                            const std::map<std::string, std::string>& filters) const {
    MetricData* metric = getMetric(name, StatisticsType::Histogram);
    if (!metric) return Histogram();
    
    auto filteredPoints = getFilteredDataPoints(*metric, filters);
    Histogram histogram;
    
    for (const auto* point : filteredPoints) {
        histogram.addValue(point->value);
    }
    
    return histogram;
}

double StatisticsCollector::getTimerAverage(const std::string& name,
                                            const std::map<std::string, std::string>& filters) const {
    MetricData* metric = getMetric(name, StatisticsType::Timer);
    if (!metric) return 0.0;
    
    auto filteredPoints = getFilteredDataPoints(*metric, filters);
    if (filteredPoints.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto* point : filteredPoints) {
        sum += point->value;
    }
    
    return sum / filteredPoints.size();
}

std::vector<double> StatisticsCollector::getDistribution(const std::string& name,
                                                        const std::map<std::string, std::string>& filters) const {
    MetricData* metric = getMetric(name, StatisticsType::Distribution);
    if (!metric) return {};
    
    // This would need additional storage for distribution data
    return {};
}

void StatisticsCollector::clear() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    metrics_.clear();
}

std::vector<std::string> StatisticsCollector::getMetricNames() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<std::string> names;
    names.reserve(metrics_.size());
    
    for (const auto& pair : metrics_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::map<std::string, std::string>> StatisticsCollector::getTagCombinations(const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<std::map<std::string, std::string>> tagCombinations;
    
    auto metricIt = metrics_.find(metricName);
    if (metricIt == metrics_.end()) return tagCombinations;
    
    const MetricData& metric = metricIt->second;
    
    for (const auto& dataPoint : metric.dataPoints) {
        // Check if this tag combination is already in the list
        if (std::find(tagCombinations.begin(), tagCombinations.end(), dataPoint.tags) == tagCombinations.end()) {
            tagCombinations.push_back(dataPoint.tags);
        }
    }
    
    return tagCombinations;
}

std::map<std::string, double> StatisticsCollector::calculateAggregates(const std::string& metricName) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::map<std::string, double> aggregates;
    
    auto metricIt = metrics_.find(metricName);
    if (metricIt == metrics_.end()) return aggregates;
    
    const MetricData& metric = metricIt->second;
    
    if (metric.dataPoints.empty()) return aggregates;
    
    double sum = 0.0;
    double sumOfSquares = 0.0;
    size_t count = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    
    for (const auto& dataPoint : metric.dataPoints) {
        double value = dataPoint.value;
        sum += value;
        sumOfSquares += value * value;
        count++;
        
        if (value < min) min = value;
        if (value > max) max = value;
    }
    
    if (count > 0) {
        aggregates["mean"] = sum / count;
        aggregates["min"] = min;
        aggregates["max"] = max;
        
        if (count > 1) {
            double variance = (sumOfSquares - (sum * sum) / count) / (count - 1);
            aggregates["variance"] = variance;
            aggregates["stddev"] = std::sqrt(variance);
        }
    }
    
    return aggregates;
}

std::string StatisticsCollector::generateReport() const {
    std::ostringstream oss;
    oss << "=== Statistics Report ===" << std::endl;
    oss << "Generated at: " << utils::generateTimestamp() << std::endl;
    oss << std::endl;
    
    oss << "Metrics: " << metrics_.size() << std::endl;
    for (const auto& pair : metrics_) {
        const std::string& name = pair.first;
        const MetricData& metric = pair.second;
        
        oss << "  - " << name << " (" << static_cast<int>(metric.type) << ")";
        
        if (!metric.dataPoints.empty()) {
            oss << ": " << metric.dataPoints.size() << " data points";
        }
        
        oss << std::endl;
    }
    
    oss << std::endl;
    oss << "Top metrics by data point count:" << std::endl;
    
    std::vector<std::pair<std::string, size_t>> sortedMetrics;
    for (const auto& pair : metrics_) {
        sortedMetrics.emplace_back(pair.first, pair.second.dataPoints.size());
    }
    
    std::sort(sortedMetrics.begin(), sortedMetrics.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min<size_t>(5, sortedMetrics.size()); ++i) {
        oss << "  " << (i + 1) << ". " << sortedMetrics[i].first << ": "
            << sortedMetrics[i].second << " points" << std::endl;
    }
    
    return oss.str();
}

std::string StatisticsCollector::generateJSONReport() const {
    std::ostringstream oss;
    oss << "{\"statistics\": [" << std::endl;
    
    bool first = true;
    for (const auto& pair : metrics_) {
        const std::string& name = pair.first;
        const MetricData& metric = pair.second;
        
        if (!first) oss << ", " << std::endl;
        first = false;
        
        oss << "  {\"name\": \"" << name << "\", \"type\": " << static_cast<int>(metric.type)
            << ", \"dataPoints\": " << metric.dataPoints.size() << "}";
    }
    
    oss << std::endl << "]" << std::endl;
    return oss.str();
}

std::string StatisticsCollector::generateCSVReport() const {
    std::ostringstream oss;
    oss << "Metric,Type,DataPoints,Average,Min,Max" << std::endl;
    
    for (const auto& pair : metrics_) {
        const std::string& name = pair.first;
        const MetricData& metric = pair.second;
        
        if (metric.dataPoints.empty()) continue;
        
        double sum = 0.0;
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::lowest();
        
        for (const auto& dataPoint : metric.dataPoints) {
            sum += dataPoint.value;
            if (dataPoint.value < min) min = dataPoint.value;
            if (dataPoint.value > max) max = dataPoint.value;
        }
        
        double average = sum / metric.dataPoints.size();
        
        oss << "\"" << name << "\"," << static_cast<int>(metric.type) << "," 
            << metric.dataPoints.size() << "," << average << "," << min << "," << max << std::endl;
    }
    
    return oss.str();
}

StatisticsCollector::MetricData* StatisticsCollector::getOrCreateMetric(const std::string& name, StatisticsType type) {
    auto it = metrics_.find(name);
    if (it != metrics_.end()) {
        return &it->second;
    }
    
    return &metrics_[name];
}

StatisticsCollector::MetricData* StatisticsCollector::getMetric(const std::string& name, StatisticsType type) const {
    auto it = metrics_.find(name);
    if (it == metrics_.end()) return nullptr;
    
    const MetricData& metric = it->second;
    if (metric.type != type) return nullptr;
    
    return const_cast<MetricData*>(&metric);
}

std::vector<const StatisticsCollector::StatisticsDataPoint*> StatisticsCollector::getFilteredDataPoints(
    const MetricData& metric, const std::map<std::string, std::string>& filters) const {
    std::vector<const StatisticsDataPoint*> filteredPoints;
    
    for (const auto& dataPoint : metric.dataPoints) {
        bool matches = true;
        
        for (const auto& filterPair : filters) {
            const std::string& key = filterPair.first;
            const std::string& expectedValue = filterPair.second;
            
            auto tagIt = dataPoint.tags.find(key);
            if (tagIt == dataPoint.tags.end() || tagIt->second != expectedValue) {
                matches = false;
                break;
            }
        }
        
        if (matches) {
            filteredPoints.push_back(&dataPoint);
        }
    }
    
    return filteredPoints;
}

} // namespace stats
} // namespace nlm