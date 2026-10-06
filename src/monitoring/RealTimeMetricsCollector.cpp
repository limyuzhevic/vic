#include "RealTimeMetricsCollector.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace nlm {

RealTimeMetricsCollector::RealTimeMetricsCollector(std::shared_ptr<MetricRegistry> registry,
                                                   const MetricsCollectionConfig& config)
    : registry_(registry), config_(config), running_(false),
      lastCollectionTime_(std::chrono::steady_clock::now()),
      totalCollections_(0) {}

RealTimeMetricsCollector::~RealTimeMetricsCollector() {
    stop();
}

bool RealTimeMetricsCollector::start() {
    if (!registry_) {
        NLM_LOG_ERROR("Cannot start collection: MetricRegistry not provided");
        return false;
    }
    
    if (running_.load()) {
        NLM_LOG_WARNING("Metrics collection is already running");
        return false;
    }
    
    running_.store(true);
    
    // Start collection thread
    collectionThread_ = std::thread(&RealTimeMetricsCollector::collectionWorker, this);
    
    // Start storage thread
    storageThread_ = std::thread(&RealTimeMetricsCollector::storageWorker, this);
    
    // Start alert thread
    alertThread_ = std::thread(&RealTimeMetricsCollector::alertWorker, this);
    
    NLM_LOG_INFO("RealTimeMetricsCollector started successfully");
    return true;
}

void RealTimeMetricsCollector::stop() {
    if (!running_.exchange(false)) {
        return; // Already stopped
    }
    
    cv_.notify_all();
    
    if (collectionThread_.joinable()) {
        collectionThread_.join();
    }
    if (storageThread_.joinable()) {
        storageThread_.join();
    }
    if (alertThread_.joinable()) {
        alertThread_.join();
    }
    
    NLM_LOG_INFO("RealTimeMetricsCollector stopped successfully");
}

void RealTimeMetricsCollector::collectMetrics() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!registry_ || !running_.load()) {
        return;
    }
    
    // Collect all registered metrics
    std::vector<MetricData> metrics = registry_->collectAllMetrics();
    
    // Convert to samples and add to buffer
    for (const auto& metric : metrics) {
        MetricSample sample(metric.name, metric.value, metric.tags);
        addSample(sample);
    }
    
    // Update statistics
    {
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_.metricsCollected = metrics.size();
        stats_.bufferSize = metricsBuffer_.size();
        stats_.lastCollectionTime = std::chrono::system_clock::now();
        stats_.collectionRate = calculateCollectionRate();
        totalCollections_++;
    }
    
    // Check for alerts if alert system is configured
    if (alertSystem_) {
        checkForAlerts(metrics);
    }
    
    // Store to historical store if configured
    if (historicalStore_) {
        historicalStore_->storeMetrics(metrics);
    }
    
    // Analyze with performance analyzer if configured
    if (performanceAnalyzer_) {
        performanceAnalyzer_->analyze(metrics);
    }
}

std::vector<MetricData> RealTimeMetricsCollector::getCurrentMetrics() const {
    if (!registry_) {
        return std::vector<MetricData>();
    }
    
    return registry_->collectAllMetrics();
}

std::vector<MetricSample> RealTimeMetricsCollector::getRecentSamples(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MetricSample> result;
    result.reserve(std::min(count, metricsBuffer_.size()));
    
    if (count >= metricsBuffer_.size()) {
        result.assign(metricsBuffer_.rbegin(), metricsBuffer_.rend());
    } else {
        auto start = metricsBuffer_.rbegin();
        std::advance(start, metricsBuffer_.size() - count);
        result.assign(start, metricsBuffer_.rend());
    }
    
    return result;
}

bool RealTimeMetricsCollector::addCustomMetric(const std::string& name,
                                               const std::string& category,
                                               const std::string& description,
                                               MetricCollector collector,
                                               const std::unordered_map<std::string, std::string>& tags) {
    if (!registry_) {
        return false;
    }
    
    return registry_->registerCustomMetric(name, category, description, collector, tags);
}

bool RealTimeMetricsCollector::removeMetric(const std::string& name) {
    if (!registry_) {
        return false;
    }
    
    return registry_->unregisterMetric(name);
}

bool RealTimeMetricsCollector::hasMetric(const std::string& name) const {
    if (!registry_) {
        return false;
    }
    
    return registry_->hasMetric(name);
}

std::string RealTimeMetricsCollector::exportMetrics(const std::string& format) const {
    if (!registry_) {
        return "";
    }
    
    if (format == "json" || format == "auto") {
        return registry_->exportToJSON();
    } else if (format == "csv") {
        // TODO: Implement CSV export
        return "CSV export not yet implemented";
    } else {
        return "Unsupported format: " + format;
    }
}

bool RealTimeMetricsCollector::importMetrics(const std::string& data, const std::string& format) {
    if (!registry_) {
        return false;
    }
    
    if (format == "json" || format == "auto") {
        return registry_->importFromJSON(data);
    } else if (format == "csv") {
        // TODO: Implement CSV import
        return false;
    }
    
    return false;
}

RealTimeMetricsCollector::CollectionStats RealTimeMetricsCollector::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

void RealTimeMetricsCollector::collectionWorker() {
    while (running_.load()) {
        auto startTime = std::chrono::steady_clock::now();
        
        collectMetrics();
        
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(endTime - startTime).count();
        
        // Sleep for the remainder of the collection interval
        double sleepTime = std::max(0.0, config_.collectionInterval - elapsed);
        if (sleepTime > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }
}

void RealTimeMetricsCollector::storageWorker() {
    while (running_.load()) {
        // Storage is handled inline in collectMetrics() for simplicity
        // In a more complex implementation, this could handle batch storage
        std::this_thread::sleep_for(std::chrono::duration<double>(config_.storageInterval));
    }
}

void RealTimeMetricsCollector::alertWorker() {
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::duration<double>(0.1)); // Check every 100ms
        
        if (alertSystem_) {
            auto metrics = getCurrentMetrics();
            if (!metrics.empty()) {
                checkForAlerts(metrics);
            }
        }
    }
}

void RealTimeMetricsCollector::addSample(const MetricSample& sample) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    metricsBuffer_.push_back(sample);
    stats_.samplesStored++;
    
    // Compress if buffer is too large
    if (metricsBuffer_.size() > config_.maxBufferSize) {
        compressIfNeeded();
    }
}

void RealTimeMetricsCollector::checkForAlerts(const std::vector<MetricData>& metrics) {
    // Simple alert checking - check for anomalous values
    for (const auto& metric : metrics) {
        // Check if value is NaN or infinite
        if (std::isnan(metric.value) || std::isinf(metric.value)) {
            NLM_LOG_WARNING(std::string("Alert: Metric ") + metric.name + 
                          " has invalid value: " + std::to_string(metric.value));
        }
        
        // Check if value is zero for critical metrics
        if (metric.value == 0.0 && 
            (metric.category == "NEURAL_PERFORMANCE" || 
             metric.category == "MEMORY_PERFORMANCE")) {
            NLM_LOG_WARNING(std::string("Alert: Metric ") + metric.name + 
                          " has zero value");
        }
    }
}

void RealTimeMetricsCollector::compressIfNeeded() {
    if (metricsBuffer_.size() <= config_.maxBufferSize * 0.8) {
        return; // Buffer is within limits
    }
    
    // Remove oldest 20% of samples
    size_t removeCount = metricsBuffer_.size() * 0.2;
    for (size_t i = 0; i < removeCount; ++i) {
        metricsBuffer_.pop_front();
    }
    
    NLM_LOG_INFO(std::string("Compressed metrics buffer, removed ") + 
                 std::to_string(removeCount) + " samples");
}

double RealTimeMetricsCollector::calculateCollectionRate() const {
    if (totalCollections_ < 2) {
        return 0.0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - lastCollectionTime_).count();
    
    if (elapsed <= 0.0) {
        return 0.0;
    }
    
    return static_cast<double>(totalCollections_) / elapsed;
}

} // namespace nlm
