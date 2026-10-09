// Performance Monitoring Implementation
// Implements comprehensive performance tracking for neural computation optimizations

#include "PerformanceMonitoring.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace nlm {

PerformanceBenchmark::PerformanceBenchmark() : session_start_time_(std::chrono::steady_clock::now()) {}
PerformanceBenchmark::~PerformanceBenchmark() = default;

PerformanceBenchmark::BenchmarkResult PerformanceBenchmark::runBenchmark(const std::string& benchmarkName,
                                                                           size_t neurons_count,
                                                                           size_t steps) {
    BenchmarkResult result(benchmarkName, 0.0, neurons_count, 0, 0, 0.0);
    
    auto start_time = std::chrono::steady_clock::now();
    
    // Simulate benchmark execution
    // In a real implementation, this would run actual neural simulation
    for (size_t step = 0; step < steps; ++step) {
        auto step_start = std::chrono::steady_clock::now();
        
        // Simulate processing neurons
        for (size_t i = 0; i < neurons_count; ++i) {
            // Simulate neuron processing
            if (i % 100 == 0) {  // Simulate some spikes
                result.spikes_processed++;
            }
        }
        
        auto step_end = std::chrono::steady_clock::now();
        auto step_duration = std::chrono::duration<double>(step_end - step_start).count();
        
        // Update peak time if needed
        if (step_duration > result.execution_time) {
            result.execution_time = step_duration;
        }
        
        // Simulate memory usage
        result.memory_usage += static_cast<size_t>(neurons_count * 100);  // Approx memory per neuron
    }
    
    auto end_time = std::chrono::steady_clock::now();
    result.execution_time = std::chrono::duration<double>(end_time - start_time).count();
    
    // Calculate efficiency score (neurons processed per second)
    double total_operations = neurons_count * steps;
    if (result.execution_time > 0.0) {
        result.efficiency_score = total_operations / result.execution_time;
    }
    
    {
        std::lock_guard<std::mutex> lock(history_mutex_);
        history_.push_back(result);
    }
    
    return result;
}

double PerformanceBenchmark::getAverageExecutionTime() const {
    if (history_.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : history_) {
        total += result.execution_time;
    }
    return total / history_.size();
}

std::string PerformanceBenchmark::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "=== Performance Benchmark Report ===" << std::endl;
    ss << std::endl;
    ss << "Total Benchmarks: " << history_.size() << std::endl;
    ss << "Average Execution Time: " << getAverageExecutionTime() << " seconds" << std::endl;
    
    if (!history_.empty()) {
        double min_time = history_.front().execution_time;
        double max_time = history_.front().execution_time;
        
        for (const auto& result : history_) {
            if (result.execution_time < min_time) min_time = result.execution_time;
            if (result.execution_time > max_time) max_time = result.execution_time;
        }
        
        ss << "Min Execution Time: " << min_time << " seconds" << std::endl;
        ss << "Max Execution Time: " << max_time << " seconds" << std::endl;
    }
    
    ss << std::endl;
    ss << "=== Benchmark History ===" << std::endl;
    for (const auto& result : history_) {
        ss << "Benchmark: " << result.name << std::endl;
        ss << "  Execution Time: " << result.execution_time << " seconds" << std::endl;
        ss << "  Neurons Processed: " << result.neurons_processed << std::endl;
        ss << "  Spikes Processed: " << result.spikes_processed << std::endl;
        ss << "  Memory Usage: " << formatMemory(result.memory_usage) << std::endl;
        ss << "  Efficiency Score: " << result.efficiency_score << " ops/sec" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

bool PerformanceBenchmark::exportToCSV(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "BenchmarkName,ExecutionTime,NeuronsProcessed,SpikesProcessed,MemoryUsage,EfficiencyScore" << std::endl;
    
    for (const auto& result : history_) {
        file << result.name << "," << result.execution_time << "," << result.neurons_processed << 
            "," << result.spikes_processed << "," << result.memory_usage << "," << result.efficiency_score << std::endl;
    }
    
    file.close();
    return true;
}

void PerformanceBenchmark::clearHistory() {
    std::lock_guard<std::mutex> lock(history_mutex_);
    history_.clear();
}

PerformanceBenchmark::BenchmarkStats PerformanceBenchmark::getStatistics() const {
    BenchmarkStats stats;
    
    if (history_.empty()) {
        return stats;
    }
    
    stats.total_benchmarks = history_.size();
    
    double total_time = 0.0;
    double min_time = history_.front().execution_time;
    double max_time = history_.front().execution_time;
    double total_neurons = 0.0;
    double total_spikes = 0.0;
    double total_efficiency = 0.0;
    
    for (const auto& result : history_) {
        total_time += result.execution_time;
        if (result.execution_time < min_time) min_time = result.execution_time;
        if (result.execution_time > max_time) max_time = result.execution_time;
        total_neurons += result.neurons_processed;
        total_spikes += result.spikes_processed;
        total_efficiency += result.efficiency_score;
    }
    
    stats.average_time = total_time / stats.total_benchmarks;
    stats.min_time = min_time;
    stats.max_time = max_time;
    stats.total_neurons_processed = total_neurons;
    stats.total_spikes_processed = total_spikes;
    stats.average_efficiency = total_efficiency / stats.total_benchmarks;
    
    return stats;
}

PerformanceMonitor::PerformanceMonitor() : monitoring_(false) {}
PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::startMonitoring() {
    monitoring_ = true;
    monitoring_start_time_ = std::chrono::steady_clock::now();
    session_start_time_ = monitoring_start_time_;
}

void PerformanceMonitor::stopMonitoring() {
    monitoring_ = false;
}

void PerformanceMonitor::recordStep(const PerformanceMetrics& metrics, size_t step) {
    if (!monitoring_) return;
    
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    updateMetrics(metrics);
    metrics_history_.push_back(metrics);
    
    // Keep only recent history to prevent unbounded growth
    if (metrics_history_.size() > 10000) {
        metrics_history_.erase(metrics_history_.begin());
    }
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    return current_metrics_;
}

PerformanceMonitor::PerformanceStats PerformanceMonitor::getPerformanceStats() const {
    PerformanceStats stats;
    
    if (metrics_history_.empty()) {
        return stats;
    }
    
    for (const auto& metrics : metrics_history_) {
        stats.total_steps++;
        stats.max_active_neurons = std::max(stats.max_active_neurons, metrics.active_neurons.load());
        stats.total_spikes += metrics.total_spikes.load();
        stats.average_firing_rate += metrics.getAverageFiringRate();
        stats.average_spike_rate += metrics.getAverageSpikeRate();
        stats.memory_usage_mb += metrics.total_memory_usage.load() / (1024 * 1024);
    }
    
    size_t count = metrics_history_.size();
    if (count > 0) {
        stats.average_firing_rate /= count;
        stats.average_spike_rate /= count;
        stats.memory_usage_mb /= count;
        
        // Calculate simulation speed (steps per second)
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(end_time - monitoring_start_time_).count();
        if (duration > 0.0) {
            stats.simulation_speed = stats.total_steps / duration;
        }
        
        stats.uptime = std::chrono::steady_clock::now() - session_start_time_;
    }
    
    return stats;
}

std::string PerformanceMonitor::generateReport() const {
    std::stringstream ss;
    ss << "=== Performance Monitor Report ===" << std::endl;
    ss << std::endl;
    
    PerformanceStats stats = getPerformanceStats();
    
    ss << "Monitoring Status: " << (isMonitoring() ? "Active" : "Inactive") << std::endl;
    ss << "Total Steps Recorded: " << stats.total_steps << std::endl;
    ss << "Maximum Active Neurons: " << stats.max_active_neurons << std::endl;
    ss << "Total Spikes: " << stats.total_spikes << std::endl;
    ss << "Average Firing Rate: " << stats.average_firing_rate << " Hz" << std::endl;
    ss << "Average Spike Rate: " << stats.average_spike_rate << " Hz" << std::endl;
    ss << "Average Memory Usage: " << stats.memory_usage_mb << " MB" << std::endl;
    ss << "Simulation Speed: " << stats.simulation_speed << " steps/sec" << std::endl;
    ss << "Session Duration: " << formatTime(getSessionDuration().count()) << std::endl;
    
    ss << std::endl;
    ss << "=== Performance Metrics ===" << std::endl;
    ss << "Neurons: " << current_metrics_.total_neurons.load() << std::endl;
    ss << "Synapses: " << current_metrics_.total_synapses.load() << std::endl;
    ss << "Active Neurons: " << current_metrics_.active_neurons.load() << std::endl;
    ss << "Firing Neurons: " << current_metrics_.firing_neurons.load() << std::endl;
    ss << "Total Spikes: " << current_metrics_.total_spikes.load() << std::endl;
    ss << "Pending Spike Events: " << current_metrics_.pending_spike_events.load() << std::endl;
    ss << "Memory Allocations: " << current_metrics_.memory_allocations.load() << std::endl;
    ss << "Memory Deallocations: " << current_metrics_.memory_deallocations.load() << std::endl;
    ss << "Current Memory Usage: " << formatMemory(current_metrics_.total_memory_usage.load()) << std::endl;
    ss << "Peak Memory Usage: " << formatMemory(current_metrics_.peak_memory_usage.load()) << std::endl;
    
    return ss.str();
}

double PerformanceMonitor::getSessionDuration() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(now - session_start_time_).count();
    return duration;
}

PerformanceMonitor::TrendAnalysis PerformanceMonitor::getTrendAnalysis(size_t window_size) const {
    TrendAnalysis analysis;
    
    if (metrics_history_.size() < 2) {
        return analysis;
    }
    
    size_t history_size = metrics_history_.size();
    size_t start_index = (history_size > window_size) ? 
        history_size - window_size : 0;
    
    std::vector<double> spike_rates, firing_rates, memory_usage, performance;
    
    for (size_t i = start_index; i < history_size; ++i) {
        const auto& metrics = metrics_history_[i];
        spike_rates.push_back(metrics.getAverageSpikeRate());
        firing_rates.push_back(metrics.getAverageFiringRate());
        memory_usage.push_back(metrics.total_memory_usage.load() / (1024 * 1024)); // MB
        performance.push_back(metrics.simulation_speed);
    }
    
    analysis.spike_rate_trend = calculateTrend(spike_rates, 10);
    analysis.firing_rate_trend = calculateTrend(firing_rates, 10);
    analysis.memory_usage_trend = calculateTrend(memory_usage, 10);
    analysis.performance_trend = calculateTrend(performance, 10);
    
    return analysis;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    current_metrics_ = PerformanceMetrics();
    metrics_history_.clear();
    monitoring_start_time_ = std::chrono::steady_clock::now();
}

std::string PerformanceMonitor::formatTime(double seconds) const {
    std::stringstream ss;
    if (seconds < 60.0) {
        ss << seconds << " seconds";
    } else if (seconds < 3600.0) {
        ss << (seconds / 60.0) << " minutes";
    } else {
        ss << (seconds / 3600.0) << " hours";
    }
    return ss.str();
}

std::string PerformanceMonitor::formatMemory(size_t bytes) const {
    std::stringstream ss;
    if (bytes < 1024) {
        ss << bytes << " bytes";
    } else if (bytes < (1024 * 1024)) {
        ss << (bytes / 1024) << " KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        ss << (bytes / (1024 * 1024)) << " MB";
    } else {
        ss << (bytes / (1024 * 1024 * 1024)) << " GB";
    }
    return ss.str();
}

OptimizationEffectivenessTracker::OptimizationEffectivenessTracker() {}
OptimizationEffectivenessTracker::~OptimizationEffectivenessTracker() = default;

void OptimizationEffectivenessTracker::recordOptimization(const std::string& optimization_type, 
                                                         double execution_time, 
                                                         size_t objects_processed) {
    OptimizationRecord record;
    record.type = optimization_type;
    record.execution_time = execution_time;
    record.objects_processed = objects_processed;
    record.timestamp = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(records_mutex_);
    records_.push_back(record);
}

OptimizationEffectivenessTracker::OptimizationStats OptimizationEffectivenessTracker::getOptimizationStats() const {
    OptimizationStats stats;
    
    if (records_.empty()) {
        return stats;
    }
    
    std::lock_guard<std::mutex> lock(records_mutex_);
    
    for (const auto& record : records_) {
        stats.total_operations++;
        stats.total_execution_time += record.execution_time;
        stats.total_objects_processed += record.objects_processed;
        stats.effectiveness_by_type[record.type] += 
            (record.objects_processed / record.execution_time);
    }
    
    if (stats.total_execution_time > 0.0) {
        stats.average_efficiency = stats.total_objects_processed / stats.total_execution_time;
    }
    
    return stats;
}

std::string OptimizationEffectivenessTracker::getEffectivenessReport(const std::string& optimization_type) const {
    std::stringstream ss;
    ss << "=== Optimization Effectiveness Report ===" << std::endl;
    ss << std::endl;
    ss << "Optimization Type: " << optimization_type << std::endl;
    
    double effectiveness = 0.0;
    size_t count = 0;
    double total_time = 0.0;
    
    {
        std::lock_guard<std::mutex> lock(records_mutex_);
        for (const auto& record : records_) {
            if (record.type == optimization_type) {
                double current_effectiveness = record.objects_processed / record.execution_time;
                effectiveness += current_effectiveness;
                total_time += record.execution_time;
                count++;
            }
        }
    }
    
    if (count > 0) {
        effectiveness /= count;
        ss << "Total Records: " << count << std::endl;
        ss << "Average Effectiveness: " << effectiveness << " objects/sec" << std::endl;
        ss << "Total Execution Time: " << total_time << " seconds" << std::endl;
    }
    
    return ss.str();
}

std::vector<OptimizationEffectivenessTracker::OptimizationComparison> 
OptimizationEffectivenessTracker::compareOptimizations(const std::map<std::string, double>& baseline) const {
    std::vector<OptimizationComparison> comparisons;
    
    std::lock_guard<std::mutex> lock(records_mutex_);
    
    for (const auto& record : records_) {
        OptimizationComparison comparison;
        comparison.optimization_type = record.type;
        
        if (baseline.find(record.type) != baseline.end()) {
            comparison.before_value = baseline.at(record.type);
        }
        
        double current_effectiveness = record.objects_processed / record.execution_time;
        comparison.after_value = current_effectiveness;
        
        if (comparison.before_value > 0.0) {
            comparison.improvement_factor = comparison.after_value / comparison.before_value;
        }
        
        comparison.impact_level = classifyImpactLevel(comparison.improvement_factor);
        comparisons.push_back(comparison);
    }
    
    return comparisons;
}

OptimizationEffectivenessTracker::OverallImpact 
OptimizationEffectivenessTracker::getOverallImpact() const {
    OverallImpact impact;
    
    if (records_.empty()) {
        return impact;
    }
    
    std::lock_guard<std::mutex> lock(records_mutex_);
    
    // Calculate overall improvements by comparing against baseline
    double total_before = 0.0;
    double total_after = 0.0;
    
    for (const auto& record : records_) {
        double before_effectiveness = 0.0; // Would need baseline
        double after_effectiveness = record.objects_processed / record.execution_time;
        
        // This is a simplified calculation - in practice, would need more sophisticated comparison
        total_before += before_effectiveness;
        total_after += after_effectiveness;
    }
    
    if (total_before > 0.0) {
        impact.total_performance_improvement = (total_after - total_before) / total_before * 100.0;
    }
    
    // Find most and least effective optimizations
    std::map<std::string, double> effectiveness_map;
    for (const auto& record : records_) {
        double effectiveness = record.objects_processed / record.execution_time;
        effectiveness_map[record.type] += effectiveness;
    }
    
    if (!effectiveness_map.empty()) {
        auto it = std::max_element(effectiveness_map.begin(), effectiveness_map.end(),
                                 [](const auto& a, const auto& b) { return a.second < b.second; });
        impact.most_effective_optimizations.push_back(it->first);
        
        auto it_least = std::min_element(effectiveness_map.begin(), effectiveness_map.end(),
                                       [](const auto& a, const auto& b) { return a.second < b.second; });
        impact.least_effective_optimizations.push_back(it_least->first);
    }
    
    return impact;
}

std::string OptimizationEffectivenessTracker::generateEffectivenessAnalysis() const {
    std::stringstream ss;
    ss << "=== Optimization Effectiveness Analysis ===" << std::endl;
    ss << std::endl;
    
    OverallImpact impact = getOverallImpact();
    
    ss << "Overall Performance Improvement: " << impact.total_performance_improvement << "%" << std::endl;
    ss << "Most Effective Optimizations: ";
    for (size_t i = 0; i < impact.most_effective_optimizations.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << impact.most_effective_optimizations[i];
    }
    ss << std::endl;
    
    ss << "Least Effective Optimizations: ";
    for (size_t i = 0; i < impact.least_effective_optimizations.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << impact.least_effective_optimizations[i];
    }
    ss << std::endl;
    
    return ss.str();
}

std::string OptimizationEffectivenessTracker::classifyImpactLevel(double improvement_factor) const {
    if (improvement_factor >= 2.0) {
        return "major";
    } else if (improvement_factor >= 1.5) {
        return "significant";
    } else if (improvement_factor >= 1.1) {
        return "moderate";
    } else {
        return "minimal";
    }
}

double OptimizationEffectivenessTracker::calculateEffectiveness(const std::string& optimization_type) const {
    double total_effectiveness = 0.0;
    size_t count = 0;
    
    std::lock_guard<std::mutex> lock(records_mutex_);
    
    for (const auto& record : records_) {
        if (record.type == optimization_type) {
            total_effectiveness += record.objects_processed / record.execution_time;
            count++;
        }
    }
    
    if (count > 0) {
        return total_effectiveness / count;
    }
    
    return 0.0;
}

PerformanceReporter::PerformanceReporter() {}
PerformanceReporter::~PerformanceReporter() = default;

std::string PerformanceReporter::generateReport(const PerformanceMonitor& monitor,
                                               const PerformanceBenchmark& benchmark,
                                               const OptimizationEffectivenessTracker& tracker) const {
    std::stringstream ss;
    ss << "=== NLM Performance Report ===" << std::endl;
    ss << std::endl;
    ss << generateExecutiveSummary(monitor, benchmark, tracker);
    ss << std::endl;
    ss << generatePerformanceDetails(monitor);
    ss << std::endl;
    ss << generateBenchmarkDetails(benchmark);
    ss << std::endl;
    ss << generateOptimizationDetails(tracker);
    ss << std::endl;
    ss << generateRecommendations(monitor, tracker);
    return ss.str();
}

bool PerformanceReporter::generateHTMLReport(const std::string& filepath,
                                            const PerformanceMonitor& monitor,
                                            const PerformanceBenchmark& benchmark,
                                            const OptimizationEffectivenessTracker& tracker) const {
    // This would generate a complete HTML report with charts and graphs
    // For now, we'll create a simple text-based report
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head><title>NLM Performance Report</title></head>" << std::endl;
    file << "<body>" << std::endl;
    file << "<h1>NLM Performance Report</h1>" << std::endl;
    file << "<pre>" << std::endl;
    file << generateReport(monitor, benchmark, tracker) << std::endl;
    file << "</pre>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

bool PerformanceReporter::generateJSONReport(const std::string& filepath,
                                           const PerformanceMonitor& monitor,
                                           const PerformanceBenchmark& benchmark,
                                           const OptimizationEffectivenessTracker& tracker) const {
    // JSON report generation would go here
    // For now, return false as it's complex to implement fully
    return false;
}

void PerformanceReporter::generateTextReport(std::ostream& output,
                                           const PerformanceMonitor& monitor,
                                           const PerformanceBenchmark& benchmark,
                                           const OptimizationEffectivenessTracker& tracker) const {
    output << generateReport(monitor, benchmark, tracker);
}

std::string PerformanceReporter::generatePerformanceSummary(const PerformanceMonitor& monitor) const {
    PerformanceStats stats = monitor.getPerformanceStats();
    std::stringstream ss;
    ss << "Performance Summary:" << std::endl;
    ss << "  Total Steps: " << stats.total_steps << std::endl;
    ss << "  Max Active Neurons: " << stats.max_active_neurons << std::endl;
    ss << "  Total Spikes: " << stats.total_spikes << std::endl;
    ss << "  Average Firing Rate: " << stats.average_firing_rate << " Hz" << std::endl;
    ss << "  Average Spike Rate: " << stats.average_spike_rate << " Hz" << std::endl;
    ss << "  Average Memory Usage: " << stats.memory_usage_mb << " MB" << std::endl;
    ss << "  Simulation Speed: " << stats.simulation_speed << " steps/sec" << std::endl;
    return ss.str();
}

std::string PerformanceReporter::generateBenchmarkSummary(const PerformanceBenchmark& benchmark) const {
    PerformanceBenchmark::BenchmarkStats stats = benchmark.getStatistics();
    std::stringstream ss;
    ss << "Benchmark Summary:" << std::endl;
    ss << "  Total Benchmarks: " << stats.total_benchmarks << std::endl;
    ss << "  Average Execution Time: " << stats.average_time << " seconds" << std::endl;
    ss << "  Min Execution Time: " << stats.min_time << " seconds" << std::endl;
    ss << "  Max Execution Time: " << stats.max_time << " seconds" << std::endl;
    ss << "  Average Neurons Processed: " << stats.total_neurons_processed / stats.total_benchmarks << std::endl;
    ss << "  Average Efficiency: " << stats.average_efficiency << " ops/sec" << std::endl;
    return ss.str();
}

std::string PerformanceReporter::generateOptimizationSummary(const OptimizationEffectivenessTracker& tracker) const {
    OptimizationEffectivenessTracker::OptimizationStats stats = tracker.getOptimizationStats();
    std::stringstream ss;
    ss << "Optimization Summary:" << std::endl;
    ss << "  Total Operations: " << stats.total_operations << std::endl;
    ss << "  Total Execution Time: " << stats.total_execution_time << " seconds" << std::endl;
    ss << "  Average Efficiency: " << stats.average_efficiency << " ops/sec" << std::endl;
    ss << "  Effectiveness by Type:" << std::endl;
    for (const auto& pair : stats.effectiveness_by_type) {
        ss << "    " << pair.first << ": " << pair.second << " ops/sec" << std::endl;
    }
    return ss.str();
}

PerformanceMonitor& GlobalPerformanceMonitor::getInstance() {
    static GlobalPerformanceMonitor instance;
    return instance;
}

} // namespace nlm
