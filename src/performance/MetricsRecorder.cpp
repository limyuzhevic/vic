#include "MetricsRecorder.hpp"
#include "../brain/Brain.hpp"
#include "../performance/OptimizedBrain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <thread>
#include <limits>

namespace nlm {

struct MetricsRecorder::Impl {
    std::unordered_map<std::string, MetricRecord> metrics;
    std::mutex metricsMutex;
    std::chrono::high_resolution_clock::time_point lastRecordTime;
};

MetricsRecorder::MetricsRecorder() 
    : enabled_(true), recordingInterval_(100) { // 100ms default recording interval
    pImpl = std::make_unique<Impl>();
    pImpl->lastRecordTime = std::chrono::high_resolution_clock::now();
}

MetricsRecorder::~MetricsRecorder() = default;

void MetricsRecorder::record(const std::string& name, double value, const std::string& label) {
    if (!enabled_) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Check if enough time has passed for recording
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - pImpl->lastRecordTime);
    if (elapsed < recordingInterval_) {
        return; // Skip this recording
    }
    
    pImpl->lastRecordTime = now;
    
    auto it = pImpl->metrics.find(name);
    if (it == pImpl->metrics.end()) {
        MetricRecord record;
        record.name = name;
        record.addDataPoint(value, label);
        pImpl->metrics[name] = std::move(record);
    } else {
        it->second.addDataPoint(value, label);
    }
}

void MetricsRecorder::recordNeuralMetrics(const class Brain& brain, Timestamp currentTime, SimulationStep currentStep) {
    if (!enabled_) return;
    
    // Basic neural metrics
    record("total_neurons", static_cast<double>(brain.getTotalNeuronCount()), "Total neurons");
    record("total_synapses", static_cast<double>(brain.getTotalSynapseCount()), "Total synapses");
    record("active_neurons", static_cast<double>(brain.getActiveNeuronCount()), "Active neurons");
    record("firing_neurons", static_cast<double>(brain.getFiringNeuronCount()), "Firing neurons (this step)");
    record("total_spikes", static_cast<double>(brain.getTotalSpikeCount()), "Total spikes");
    record("avg_firing_rate", brain.getAverageFiringRate(), "Average firing rate (Hz)");
    record("e_i_ratio", brain.getExcitationInhibitionRatio(), "E/I ratio");
    
    // Time and step metrics
    record("current_step", static_cast<double>(currentStep), "Current simulation step");
    record("current_time", currentTime, "Current simulation time (s)");
    
    // Region-specific metrics
    size_t regionCount = brain.getRegionCount();
    if (regionCount > 0) {
        record("region_count", static_cast<double>(regionCount), "Number of regions");
    }
}

void MetricsRecorder::recordPerformanceMetrics(const class OptimizedBrain* optimizedBrain) {
    if (!enabled_) return;
    
    // Basic performance metrics
    record("system_cpu_usage", getCurrentCPUUsage(), "CPU utilization (%)");
    record("system_memory_usage", getCurrentMemoryUsage(), "Memory usage (MB)");
    
    // If optimized brain is available, record its performance stats
    if (optimizedBrain) {
        auto perfStats = optimizedBrain->getPerformanceStats();
        record("perf_neurons_per_second", perfStats.neuronsPerSecond, "Neurons processed per second");
        record("perf_spikes_per_second", perfStats.spikesPerSecond, "Spikes processed per second");
        record("perf_synaptic_events_per_second", perfStats.synapticEventsPerSecond, "Synaptic events per second");
        record("perf_real_time_factor", perfStats.realTimeFactor, "Real-time simulation factor");
        record("perf_active_neurons", static_cast<double>(perfStats.activeNeurons), "Currently active neurons");
        record("perf_firing_neurons", static_cast<double>(perfStats.firingNeurons), "Currently firing neurons");
        record("perf_pending_spikes", static_cast<double>(perfStats.pendingSpikes), "Pending spikes");
        record("perf_pending_delayed_spikes", static_cast<double>(perfStats.pendingDelayedSpikes), "Pending delayed spikes");
        record("perf_avg_firing_rate", perfStats.avgFiringRate, "Average firing rate (Hz)");
        record("perf_memory_usage_mb", perfStats.memoryUsageMB, "Memory usage (MB)");
        record("perf_step_time_ms", perfStats.stepTimeMs, "Average step time (ms)");
    }
}

void MetricsRecorder::recordSpikeMetrics(const std::vector<class DetailedSpikeEvent>& spikes) {
    if (!enabled_) return;
    
    if (!spikes.empty()) {
        record("spike_count_current_step", static_cast<double>(spikes.size()), "Spikes in current step");
        record("spike_rate", static_cast<double>(spikes.size()) / 0.001, "Spike rate (Hz)"); // Assuming 1ms timestep
    }
}

const MetricRecord* MetricsRecorder::getMetric(const std::string& name) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> MetricsRecorder::getMetricNames() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->metricsMutex);
    std::vector<std::string> names;
    names.reserve(pImpl->metrics.size());
    for (const auto& pair : pImpl->metrics) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<DataPoint> MetricsRecorder::getHistoricalData(const std::string& name, size_t limit) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->metrics.find(name);
    if (it == pImpl->metrics.end()) {
        return {};
    }
    
    const auto& record = it->second;
    std::vector<DataPoint> result = record.data;
    
    if (limit > 0 && limit < result.size()) {
        auto start = result.end() - limit;
        result = std::vector<DataPoint>(start, result.end());
    }
    
    return result;
}

std::optional<double> MetricsRecorder::getCurrentValue(const std::string& name) const {
    const MetricRecord* record = getMetric(name);
    if (record) {
        return record->getLatestValue();
    }
    return std::optional<double>();
}

MetricsRecorder::MetricStats MetricsRecorder::getStats(const std::string& name) const {
    MetricStats stats;
    const MetricRecord* record = getMetric(name);
    if (!record) {
        return stats;
    }
    
    const auto& data = record->data;
    if (data.empty()) {
        return stats;
    }
    
    // Calculate statistics
    std::vector<double> values;
    values.reserve(data.size());
    for (const auto& point : data) {
        values.push_back(point.value);
    }
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    stats.count = values.size();
    stats.mean = sum / stats.count;
    stats.min = record->minValue;
    stats.max = record->maxValue;
    stats.lastValue = record->lastValue;
    
    // Calculate standard deviation
    if (stats.count > 1) {
        double sumSquares = 0.0;
        for (double v : values) {
            double diff = v - stats.mean;
            sumSquares += diff * diff;
        }
        stats.stdDev = std::sqrt(sumSquares / (stats.count - 1));
    }
    
    return stats;
}

void MetricsRecorder::clear() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metrics.clear();
}

void MetricsRecorder::clearOldData(const std::chrono::milliseconds& age) {
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoff = now - age;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    for (auto& pair : pImpl->metrics) {
        auto& record = pair.second;
        auto newEnd = std::remove_if(record.data.begin(), record.data.end(),
            [cutoff](const DataPoint& point) {
                return point.timestamp < cutoff;
            });
        record.data.erase(newEnd, record.data.end());
        
        // Update stats
        record.minValue = std::numeric_limits<double>::max();
        record.maxValue = std::numeric_limits<double>::lowest();
        record.lastValue = 0.0;
        record.dataPointCount = record.data.size();
        
        for (const auto& point : record.data) {
            record.minValue = std::min(record.minValue, point.value);
            record.maxValue = std::max(record.maxValue, point.value);
            record.lastValue = point.value;
        }
    }
}

std::string MetricsRecorder::summary() const {
    std::ostringstream oss;
    
    std::shared_lock<std::shared_mutex> lock(pImpl->metricsMutex);
    oss << "=== MetricsRecorder Summary ===\n";
    oss << "Enabled: " << (enabled_ ? "Yes" : "No") << "\n";
    oss << "Recording interval: " << recordingInterval_.count() << " ms\n";
    oss << "Recorded metrics: " << pImpl->metrics.size() << "\n\n";
    
    for (const auto& pair : pImpl->metrics) {
        const auto& record = pair.second;
        oss << "Metric: " << record.name << "\n";
        oss << "  Data points: " << record.getDataCount() << "\n";
        
        MetricStats stats = getStats(record.name);
        oss << "  " << stats.toString() << "\n";
    }
    
    return oss.str();
}

void MetricsRecorder::recordGlobal(const std::string& name, double value, const std::string& label) {
    getInstance().record(name, value, label);
}

// Helper functions for system metrics

namespace {
    double getCurrentCPUUsage() {
        // Placeholder - would implement with platform-specific code
        return 45.0; // 45% CPU usage
    }
    
    double getCurrentMemoryUsage() {
        // Placeholder - would implement with platform-specific code
        return 512.0; // 512 MB memory usage
    }
}

} // namespace nlm
