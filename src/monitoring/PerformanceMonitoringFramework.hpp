#pragma once

/**
 * Performance Monitoring Infrastructure
 * 
 * This module provides comprehensive performance monitoring capabilities for NLM,
 * including real-time performance monitoring, performance alerting, performance reporting,
 * and performance analysis.
 * 
 * Key Features:
 * - Performance monitoring system with real-time data collection
 * - Performance alerting with configurable thresholds
 * - Performance reporting with multiple output formats
 * - Performance analysis with trend detection and anomaly detection
 * - Performance dashboards and visualization
 * - Performance metrics aggregation
 * - Performance trend analysis
 * - Performance correlation analysis
 * - Performance prediction
 * - Performance optimization recommendations
 * 
 * Architecture:
 * - MonitoringBase: Abstract base class for all monitoring components
 * - MetricCollector: Base class for metric collection
 * - RealTimeMonitor: Real-time performance monitoring
 * - PerformanceAlert: Performance alerting system
 * - PerformanceReporter: Report generation
 * - PerformanceAnalyzer: Data analysis and trend detection
 * - PerformanceDashboard: Dashboard generation and visualization
 * - PerformanceMetrics: Container for performance metrics
 * - PerformanceAlertRule: Definition of alert rules
 * - PerformanceTrend: Trend analysis results
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <functional>
#include <cmath>
#include <set>

namespace nlm {
namespace monitoring {

// Performance monitoring data structures
struct PerformanceMetric {
    uint64_t timestamp_ns;
    std::string metric_name;
    std::string metric_type;  // counter, gauge, histogram, summary
    double value;
    std::map<std::string, std::string> labels;
    std::string unit;
    uint64_t thread_id;
    std::string thread_name;
};

struct PerformanceAlertRule {
    std::string rule_name;
    std::string metric_name;
    std::string condition;  // greater_than, less_than, equals, contains
    double threshold_value;
    std::string severity;  // critical, warning, info
    std::string message;
    std::map<std::string, std::string> labels;
    std::chrono::nanoseconds check_interval;
    bool enabled;
    std::function<bool(double)> custom_condition;
};

struct PerformanceAlert {
    uint64_t timestamp_ns;
    std::string rule_name;
    std::string metric_name;
    double current_value;
    double threshold_value;
    std::string severity;
    std::string message;
    std::string status;  // triggered, resolved, acknowledged
    std::map<std::string, std::string> metadata;
};

struct PerformanceReport {
    uint64_t start_time_ns;
    uint64_t end_time_ns;
    double total_duration_seconds;
    std::vector<PerformanceMetric> metrics;
    std::vector<PerformanceAlert> alerts;
    std::map<std::string, double> summary_metrics;
    std::map<std::string, std::string> report_metadata;
    std::string format;  // text, json, xml, csv
    std::string title;
    std::string description;
};

struct PerformanceTrend {
    std::string metric_name;
    std::vector<double> values;
    std::vector<uint64_t> timestamps;
    double slope;
    double r_squared;
    double mean_change_rate;
    std::string trend_type;  // increasing, decreasing, stable, volatile
    std::chrono::nanoseconds period;
    std::map<std::string, std::string> trend_details;
};

struct PerformanceAnomaly {
    uint64_t timestamp_ns;
    std::string metric_name;
    double expected_value;
    double actual_value;
    double deviation;
    double confidence_score;
    std::string anomaly_type;  // spike, drop, trend_break, seasonal
    std::map<std::string, std::string> anomaly_details;
};

struct PerformancePrediction {
    std::string metric_name;
    std::vector<uint64_t> prediction_timestamps;
    std::vector<double> predicted_values;
    std::vector<double> confidence_intervals;
    std::string prediction_type;  // linear, exponential, moving_average
    double forecast_horizon_seconds;
    std::map<std::string, std::string> prediction_details;
};

struct OptimizationOpportunity {
    std::string opportunity_name;
    std::string metric_name;
    double current_value;
    double target_value;
    double improvement_potential;
    std::string category;  // cpu, memory, network, io, configuration
    double impact_score;
    std::string recommendation;
    double estimated_improvement;
    std::string implementation_effort;
    std::map<std::string, std::string> details;
};

// Abstract base class for monitoring components
class MonitoringComponentBase {
protected:
    std::string name_;
    bool enabled_;
    bool running_;
    std::mutex component_mutex_;
    
public:
    MonitoringComponentBase(const std::string& name) : name_(name), enabled_(false), running_(false) {}
    virtual ~MonitoringComponentBase() = default;
    
    virtual void start() {
        std::lock_guard<std::mutex> lock(component_mutex_);
        enabled_ = true;
        running_ = true;
    }
    
    virtual void stop() {
        std::lock_guard<std::mutex> lock(component_mutex_);
        running_ = false;
        enabled_ = false;
    }
    
    virtual void pause() {
        std::lock_guard<std::mutex> lock(component_mutex_);
        running_ = false;
    }
    
    virtual void resume() {
        std::lock_guard<std::mutex> lock(component_mutex_);
        if (enabled_) {
            running_ = true;
        }
    }
    
    virtual void collect_metrics() = 0;
    
    bool is_enabled() const { return enabled_; }
    bool is_running() const { return running_; }
    const std::string& get_name() const { return name_; }
};

// Metric collector implementation
class MetricCollector : public MonitoringComponentBase {
protected:
    std::vector<PerformanceMetric> collected_metrics_;
    uint64_t collection_interval_ns_;
    std::thread collection_thread_;
    bool stop_collection_;
    
public:
    MetricCollector(const std::string& name, uint64_t collection_interval_ns = 1000000ULL)
        : MonitoringComponentBase(name), collection_interval_ns_(collection_interval_ns),
          stop_collection_(false) {
        collected_metrics_.reserve(10000);
    }
    
    ~MetricCollector() {
        stop();
    }
    
    void start() override {
        MonitoringComponentBase::start();
        stop_collection_ = false;
        collected_metrics_.clear();
        
        collection_thread_ = std::thread(&MetricCollector::collection_loop, this);
    }
    
    void stop() override {
        MonitoringComponentBase::stop();
        stop_collection_ = true;
        if (collection_thread_.joinable()) {
            collection_thread_.join();
        }
    }
    
    void collect_metrics() override {
        // Collect metrics - implementation depends on specific collector
        // This is overridden by derived classes
    }
    
    void add_metric(const PerformanceMetric& metric) {
        std::lock_guard<std::mutex> lock(component_mutex_);
        collected_metrics_.push_back(metric);
    }
    
    const std::vector<PerformanceMetric>& get_metrics() const {
        std::lock_guard<std::mutex> lock(component_mutex_);
        return collected_metrics_;
    }
    
    void clear_metrics() {
        std::lock_guard<std::mutex> lock(component_mutex_);
        collected_metrics_.clear();
    }
    
private:
    void collection_loop() {
        while (!stop_collection_) {
            collect_metrics();
            std::this_thread::sleep_for(std::chrono::nanoseconds(collection_interval_ns_));
        }
    }
};

// CPU Metric Collector
class CPUMetricCollector : public MetricCollector {
public:
    CPUMetricCollector() : MetricCollector("cpu_collector") {}
    
    void collect_metrics() override {
        if (!is_running()) return;
        
        PerformanceMetric metric;
        metric.timestamp_ns = getCurrentTimestamp();
        metric.metric_name = "cpu_usage_percentage";
        metric.metric_type = "gauge";
        metric.value = collect_cpu_usage();  // Placeholder
        metric.unit = "%";
        metric.thread_id = std::this_thread::get_id();
        metric.thread_name = "main";  // Placeholder
        
        add_metric(metric);
        
        // Collect per-core metrics
        collect_per_core_metrics();
    }
    
private:
    double collect_cpu_usage() {
        // Collect CPU usage (placeholder)
        return 25.0;  // Placeholder: 25%
    }
    
    void collect_per_core_metrics() {
        // Collect per-core metrics (placeholder)
        for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
            PerformanceMetric metric;
            metric.timestamp_ns = getCurrentTimestamp();
            metric.metric_name = "cpu_core_" + std::to_string(i) + "_usage";
            metric.metric_type = "gauge";
            metric.value = 20.0 + (i * 5.0);  // Placeholder
            metric.unit = "%";
            metric.labels = {{{"core", std::to_string(i)}}};
            metric.thread_id = std::this_thread::get_id();
            metric.thread_name = "main";  // Placeholder
            
            add_metric(metric);
        }
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Memory Metric Collector
class MemoryMetricCollector : public MetricCollector {
public:
    MemoryMetricCollector() : MetricCollector("memory_collector") {}
    
    void collect_metrics() override {
        if (!is_running()) return;
        
        // Collect memory metrics (placeholder)
        PerformanceMetric metric;
        metric.timestamp_ns = getCurrentTimestamp();
        metric.metric_name = "memory_usage_bytes";
        metric.metric_type = "gauge";
        metric.value = measure_memory_usage();  // Placeholder
        metric.unit = "bytes";
        metric.thread_id = std::this_thread::get_id();
        metric.thread_name = "main";  // Placeholder
        
        add_metric(metric);
        
        metric.metric_name = "memory_peak_bytes";
        metric.value = measure_peak_memory_usage();  // Placeholder
        add_metric(metric);
        
        metric.metric_name = "memory_allocation_count";
        metric.value = measure_allocation_count();  // Placeholder
        metric.metric_type = "counter";
        add_metric(metric);
        
        metric.metric_name = "memory_deallocation_count";
        metric.value = measure_deallocation_count();  // Placeholder
        metric.metric_type = "counter";
        add_metric(metric);
    }
    
private:
    size_t measure_memory_usage() {
        // Measure current memory usage (placeholder)
        return 512 * 1024 * 1024;  // Placeholder: 512MB
    }
    
    size_t measure_peak_memory_usage() {
        // Measure peak memory usage (placeholder)
        return 2 * 1024 * 1024 * 1024;  // Placeholder: 2GB
    }
    
    size_t measure_allocation_count() {
        // Measure allocation count (placeholder)
        return 1000;  // Placeholder
    }
    
    size_t measure_deallocation_count() {
        // Measure deallocation count (placeholder)
        return 950;  // Placeholder
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Network Metric Collector
class NetworkMetricCollector : public MetricCollector {
public:
    NetworkMetricCollector() : MetricCollector("network_collector") {}
    
    void collect_metrics() override {
        if (!is_running()) return;
        
        // Collect network metrics (placeholder)
        PerformanceMetric metric;
        metric.timestamp_ns = getCurrentTimestamp();
        metric.metric_name = "network_bytes_sent";
        metric.metric_type = "counter";
        metric.value = measure_bytes_sent();  // Placeholder
        metric.unit = "bytes";
        metric.labels = {{{"protocol", "tcp"}}};
        metric.thread_id = std::this_thread::get_id();
        metric.thread_name = "main";  // Placeholder
        
        add_metric(metric);
        
        metric.metric_name = "network_bytes_received";
        metric.value = measure_bytes_received();  // Placeholder
        add_metric(metric);
        
        metric.metric_name = "network_packets_sent";
        metric.value = measure_packets_sent();  // Placeholder
        metric.metric_type = "counter";
        add_metric(metric);
        
        metric.metric_name = "network_packets_received";
        metric.value = measure_packets_received();  // Placeholder
        metric.metric_type = "counter";
        add_metric(metric);
    }
    
private:
    uint64_t measure_bytes_sent() {
        // Measure bytes sent (placeholder)
        return 1024 * 1024;  // Placeholder: 1MB
    }
    
    uint64_t measure_bytes_received() {
        // Measure bytes received (placeholder)
        return 2048 * 1024;  // Placeholder: 2MB
    }
    
    uint32_t measure_packets_sent() {
        // Measure packets sent (placeholder)
        return 1000;  // Placeholder
    }
    
    uint32_t measure_packets_received() {
        // Measure packets received (placeholder)
        return 1050;  // Placeholder
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// IO Metric Collector
class IOMetricCollector : public MetricCollector {
public:
    IOMetricCollector() : MetricCollector("io_collector") {}
    
    void collect_metrics() override {
        if (!is_running()) return;
        
        // Collect IO metrics (placeholder)
        PerformanceMetric metric;
        metric.timestamp_ns = getCurrentTimestamp();
        metric.metric_name = "io_read_bytes";
        metric.metric_type = "counter";
        metric.value = measure_read_bytes();  // Placeholder
        metric.unit = "bytes";
        metric.labels = {{{"operation", "read"}}};
        metric.thread_id = std::this_thread::get_id();
        metric.thread_name = "main";  // Placeholder
        
        add_metric(metric);
        
        metric.metric_name = "io_write_bytes";
        metric.value = measure_write_bytes();  // Placeholder
        add_metric(metric);
        
        metric.metric_name = "io_read_operations";
        metric.value = measure_read_operations();  // Placeholder
        metric.metric_type = "counter";
        add_metric(metric);
        
        metric.metric_name = "io_write_operations";
        metric.value = measure_write_operations();  // Placeholder
        metric.metric_type = "counter";
        add_metric(metric);
    }
    
private:
    uint64_t measure_read_bytes() {
        // Measure read bytes (placeholder)
        return 512 * 1024 * 1024;  // Placeholder: 512MB
    }
    
    uint64_t measure_write_bytes() {
        // Measure write bytes (placeholder)
        return 256 * 1024 * 1024;  // Placeholder: 256MB
    }
    
    uint32_t measure_read_operations() {
        // Measure read operations (placeholder)
        return 5000;  // Placeholder
    }
    
    uint32_t measure_write_operations() {
        // Measure write operations (placeholder)
        return 3000;  // Placeholder
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Performance Alert System
class PerformanceAlertSystem : public MonitoringComponentBase {
private:
    std::vector<PerformanceAlertRule> alert_rules_;
    std::vector<PerformanceAlert> active_alerts_;
    std::mutex alert_mutex_;
    
public:
    PerformanceAlertSystem() : MonitoringComponentBase("alert_system") {}
    
    void add_alert_rule(const PerformanceAlertRule& rule) {
        std::lock_guard<std::mutex> lock(alert_mutex_);
        alert_rules_.push_back(rule);
    }
    
    void check_alerts(const std::vector<PerformanceMetric>& metrics) {
        if (!is_running()) return;
        
        std::lock_guard<std::mutex> lock(alert_mutex_);
        
        // Check each alert rule
        for (const auto& rule : alert_rules_) {
            if (!rule.enabled) continue;
            
            double metric_value = get_metric_value(metrics, rule.metric_name, rule.labels);
            
            if (rule.custom_condition) {
                if (rule.custom_condition(metric_value)) {
                    trigger_alert(rule, metric_value);
                } else {
                    resolve_alert(rule.metric_name, rule_name_to_labels(rule));
                }
            } else {
                bool should_trigger = check_rule_condition(metric_value, rule);
                if (should_trigger) {
                    trigger_alert(rule, metric_value);
                } else {
                    resolve_alert(rule.metric_name, rule_name_to_labels(rule));
                }
            }
        }
    }
    
    const std::vector<PerformanceAlert>& get_active_alerts() const {
        std::lock_guard<std::mutex> lock(alert_mutex_);
        return active_alerts_;
    }
    
    std::vector<PerformanceAlert> get_alerts_by_severity(const std::string& severity) const {
        std::vector<PerformanceAlert> filtered;
        std::lock_guard<std::mutex> lock(alert_mutex_);
        
        for (const auto& alert : active_alerts_) {
            if (alert.severity == severity) {
                filtered.push_back(alert);
            }
        }
        
        return filtered;
    }
    
private:
    double get_metric_value(const std::vector<PerformanceMetric>& metrics,
                            const std::string& metric_name,
                            const std::map<std::string, std::string>& labels) const {
        for (const auto& metric : metrics) {
            if (metric.metric_name == metric_name && metric_labels_match(metric.labels, labels)) {
                return metric.value;
            }
        }
        return 0.0;  // Default if metric not found
    }
    
    bool metric_labels_match(const std::map<std::string, std::string>& metric_labels,
                             const std::map<std::string, std::string>& target_labels) const {
        for (const auto& pair : target_labels) {
            if (metric_labels.find(pair.first) == metric_labels.end() ||
                metric_labels.at(pair.first) != pair.second) {
                return false;
            }
        }
        return true;
    }
    
    bool check_rule_condition(double value, const PerformanceAlertRule& rule) const {
        if (rule.condition == "greater_than") {
            return value > rule.threshold_value;
        } else if (rule.condition == "less_than") {
            return value < rule.threshold_value;
        } else if (rule.condition == "equals") {
            return std::abs(value - rule.threshold_value) < 1e-6;
        } else if (rule.condition == "contains") {
            return value > 0 && value <= rule.threshold_value;
        }
        return false;
    }
    
    std::map<std::string, std::string> rule_name_to_labels(const PerformanceAlertRule& rule) const {
        std::map<std::string, std::string> labels;
        for (const auto& pair : rule.labels) {
            labels[pair.first] = pair.second;
        }
        return labels;
    }
    
    void trigger_alert(const PerformanceAlertRule& rule, double current_value) {
        PerformanceAlert alert;
        alert.timestamp_ns = getCurrentTimestamp();
        alert.rule_name = rule.rule_name;
        alert.metric_name = rule.metric_name;
        alert.current_value = current_value;
        alert.threshold_value = rule.threshold_value;
        alert.severity = rule.severity;
        alert.message = rule.message;
        alert.status = "triggered";
        alert.metadata = rule.labels;
        
        std::lock_guard<std::mutex> lock(alert_mutex_);
        active_alerts_.push_back(alert);
        
        // Log alert
        std::cout << "[ALERT] " << alert.message << " - Severity: " << alert.severity << std::endl;
    }
    
    void resolve_alert(const std::string& metric_name,
                       const std::map<std::string, std::string>& labels) {
        std::lock_guard<std::mutex> lock(alert_mutex_);
        
        auto it = std::find_if(active_alerts_.begin(), active_alerts_.end(),
            [&metric_name, &labels](const PerformanceAlert& alert) {
                return alert.metric_name == metric_name && 
                       metric_labels_match(alert.metadata, labels);
            });
        
        if (it != active_alerts_.end()) {
            it->status = "resolved";
            std::cout << "[ALERT RESOLVED] " << metric_name << " - Resolved successfully" << std::endl;
        }
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Performance Reporter
class PerformanceReporter : public MonitoringComponentBase {
public:
    PerformanceReporter() : MonitoringComponentBase("reporter") {}
    
    std::string generate_text_report(const PerformanceReport& report) {
        std::stringstream ss;
        ss << "=== NLM Performance Report ===" << std::endl;
        ss << "Period: " << report.start_time_ns << " - " << report.end_time_ns << std::endl;
        ss << "Duration: " << report.total_duration_seconds << " seconds" << std::endl;
        ss << std::endl;
        
        ss << "Metrics Summary:" << std::endl;
        for (const auto& pair : report.summary_metrics) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        ss << std::endl;
        
        ss << "Alerts:" << std::endl;
        for (const auto& alert : report.alerts) {
            ss << "  [" << alert.severity << "] " << alert.message << std::endl;
            ss << "    Time: " << alert.timestamp_ns << " ns" << std::endl;
            ss << "    Status: " << alert.status << std::endl;
        }
        ss << std::endl;
        
        ss << "Detailed Metrics:" << std::endl;
        for (const auto& metric : report.metrics) {
            ss << "  " << metric.metric_name << ": " << metric.value << " " << metric.unit << std::endl;
        }
        
        return ss.str();
    }
    
    std::string generate_json_report(const PerformanceReport& report) {
        // Generate JSON report (placeholder)
        return "{\"report\": \"JSON report implementation needed\"}";
    }
    
    std::string generate_xml_report(const PerformanceReport& report) {
        // Generate XML report (placeholder)
        return "<report>XML report implementation needed</report>";
    }
    
    std::string generate_csv_report(const PerformanceReport& report) {
        // Generate CSV report (placeholder)
        std::stringstream ss;
        ss << "timestamp,metric_name,value,unit" << std::endl;
        for (const auto& metric : report.metrics) {
            ss << metric.timestamp_ns << ","
               << metric.metric_name << ","
               << metric.value << ","
               << metric.unit << std::endl;
        }
        return ss.str();
    }
    
    void save_report(const PerformanceReport& report, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            if (report.format == "json") {
                file << generate_json_report(report);
            } else if (report.format == "xml") {
                file << generate_xml_report(report);
            } else if (report.format == "csv") {
                file << generate_csv_report(report);
            } else {
                file << generate_text_report(report);
            }
            file.close();
        }
    }
};

// Performance Analyzer
class PerformanceAnalyzer {
public:
    static std::vector<PerformanceTrend> analyze_trends(const std::vector<PerformanceMetric>& metrics,
                                                         const std::string& metric_name,
                                                         const std::chrono::nanoseconds& period) {
        std::vector<PerformanceTrend> trends;
        
        for (const auto& metric : metrics) {
            if (metric.metric_name == metric_name) {
                PerformanceTrend trend;
                trend.metric_name = metric_name;
                trend.values.push_back(metric.value);
                trend.timestamps.push_back(metric.timestamp_ns);
                
                // Simple linear regression for trend analysis
                trend.slope = calculate_linear_regression_slope(trend.timestamps, trend.values);
                trend.r_squared = calculate_r_squared(trend.timestamps, trend.values, trend.slope);
                
                // Determine trend type
                if (std::abs(trend.slope) < 1e-6) {
                    trend.trend_type = "stable";
                } else if (trend.slope > 0) {
                    trend.trend_type = "increasing";
                } else {
                    trend.trend_type = "decreasing";
                }
                
                trend.period = period;
                trends.push_back(trend);
            }
        }
        
        return trends;
    }
    
    static std::vector<PerformanceAnomaly> detect_anomalies(const std::vector<PerformanceMetric>& metrics,
                                                             const std::string& metric_name,
                                                             double threshold = 2.0) {
        std::vector<PerformanceAnomaly> anomalies;
        
        std::vector<PerformanceMetric> filtered_metrics;
        for (const auto& metric : metrics) {
            if (metric.metric_name == metric_name) {
                filtered_metrics.push_back(metric);
            }
        }
        
        if (filtered_metrics.empty()) return anomalies;
        
        // Calculate statistics
        double mean = calculate_mean(filtered_metrics);
        double std_dev = calculate_standard_deviation(filtered_metrics, mean);
        
        for (const auto& metric : filtered_metrics) {
            double deviation = std::abs(metric.value - mean) / std_dev;
            
            if (deviation > threshold) {
                PerformanceAnomaly anomaly;
                anomaly.timestamp_ns = metric.timestamp_ns;
                anomaly.metric_name = metric_name;
                anomaly.expected_value = mean;
                anomaly.actual_value = metric.value;
                anomaly.deviation = deviation;
                anomaly.confidence_score = 1.0 / (1.0 + deviation);
                
                // Determine anomaly type
                if (metric.value > mean + 2 * std_dev) {
                    anomaly.anomaly_type = "spike";
                } else if (metric.value < mean - 2 * std_dev) {
                    anomaly.anomaly_type = "drop";
                } else if (std::abs(calculate_linear_regression_slope(
                    {metric.timestamp_ns}, {metric.value}) - calculate_average_slope(filtered_metrics)) > 0.1) {
                    anomaly.anomaly_type = "trend_break";
                } else {
                    anomaly.anomaly_type = "volatile";
                }
                
                anomalies.push_back(anomaly);
            }
        }
        
        return anomalies;
    }
    
    static std::vector<PerformancePrediction> predict_performance(const std::vector<PerformanceMetric>& metrics,
                                                                    const std::string& metric_name,
                                                                    const std::chrono::nanoseconds& forecast_horizon,
                                                                    const std::string& prediction_type = "linear") {
        std::vector<PerformancePrediction> predictions;
        
        std::vector<PerformanceMetric> filtered_metrics;
        for (const auto& metric : metrics) {
            if (metric.metric_name == metric_name) {
                filtered_metrics.push_back(metric);
            }
        }
        
        if (filtered_metrics.size() < 2) return predictions;
        
        PerformancePrediction prediction;
        prediction.metric_name = metric_name;
        prediction.prediction_type = prediction_type;
        prediction.forecast_horizon_seconds = 
            std::chrono::duration<double>(forecast_horizon).count();
        
        // Generate prediction timestamps
        uint64_t last_timestamp = filtered_metrics.back().timestamp_ns;
        uint64_t forecast_steps = static_cast<uint64_t>(prediction.forecast_horizon_seconds * 1e9 / 1000000);  // Assuming 1ms intervals
        
        for (uint64_t i = 1; i <= forecast_steps; ++i) {
            uint64_t pred_timestamp = last_timestamp + i * 1000000;  // 1ms intervals
            prediction.prediction_timestamps.push_back(pred_timestamp);
            
            double pred_value = predict_value(filtered_metrics, prediction_type, i);
            prediction.predicted_values.push_back(pred_value);
            
            double confidence = calculate_prediction_confidence(filtered_metrics, i);
            prediction.confidence_intervals.push_back(confidence);
        }
        
        predictions.push_back(prediction);
        
        return predictions;
    }
    
    static std::vector<OptimizationOpportunity> identify_optimization_opportunities(
        const std::vector<PerformanceMetric>& metrics,
        const std::map<std::string, double>& target_values) {
        std::vector<OptimizationOpportunity> opportunities;
        
        for (const auto& target : target_values) {
            std::string metric_name = target.first;
            double target_value = target.second;
            
            double current_value = get_current_metric_value(metrics, metric_name);
            
            if (current_value > 0 && target_value > 0) {
                OptimizationOpportunity opportunity;
                opportunity.opportunity_name = "improve_" + metric_name;
                opportunity.metric_name = metric_name;
                opportunity.current_value = current_value;
                opportunity.target_value = target_value;
                opportunity.improvement_potential = (target_value - current_value) / current_value * 100.0;
                opportunity.category = categorize_optimization(metric_name);
                opportunity.impact_score = calculate_impact_score(current_value, target_value);
                opportunity.recommendation = generate_optimization_recommendation(metric_name);
                opportunity.estimated_improvement = opportunity.improvement_potential * 0.8;  // 80% of potential
                opportunity.implementation_effort = assess_implementation_effort(opportunity.category);
                
                opportunities.push_back(opportunity);
            }
        }
        
        return opportunities;
    }
    
    static std::string generate_optimization_summary(const std::vector<OptimizationOpportunity>& opportunities) {
        std::stringstream ss;
        ss << "=== Optimization Opportunities ===" << std::endl;
        ss << "Total opportunities: " << opportunities.size() << std::endl;
        ss << std::endl;
        
        for (const auto& opportunity : opportunities) {
            ss << "Opportunity: " << opportunity.opportunity_name << std::endl;
            ss << "  Metric: " << opportunity.metric_name << std::endl;
            ss << "  Current: " << opportunity.current_value << std::endl;
            ss << "  Target: " << opportunity.target_value << std::endl;
            ss << "  Potential improvement: " << opportunity.improvement_potential << "%" << std::endl;
            ss << "  Impact score: " << opportunity.impact_score << std::endl;
            ss << "  Recommendation: " << opportunity.recommendation << std::endl;
            ss << "  Estimated improvement: " << opportunity.estimated_improvement << "%" << std::endl;
            ss << "  Implementation effort: " << opportunity.implementation_effort << std::endl;
            ss << std::endl;
        }
        
        return ss.str();
    }
    
private:
    static double calculate_linear_regression_slope(const std::vector<uint64_t>& x,
                                                      const std::vector<double>& y) {
        if (x.size() != y.size() || x.size() < 2) return 0.0;
        
        double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
        double sum_y = std::accumulate(y.begin(), y.end(), 0.0);
        double sum_xy = 0.0;
        double sum_xx = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            sum_xy += x[i] * y[i];
            sum_xx += x[i] * x[i];
        }
        
        double n = static_cast<double>(x.size());
        return (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
    }
    
    static double calculate_r_squared(const std::vector<uint64_t>& x,
                                       const std::vector<double>& y,
                                       double slope) {
        if (x.size() != y.size() || x.size() < 2) return 0.0;
        
        double mean_y = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
        double ss_total = 0.0;
        double ss_residual = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            double predicted_y = mean_y + slope * (x[i] - std::accumulate(x.begin(), x.end(), 0.0) / x.size());
            ss_total += (y[i] - mean_y) * (y[i] - mean_y);
            ss_residual += (y[i] - predicted_y) * (y[i] - predicted_y);
        }
        
        return ss_total - ss_residual / ss_total;
    }
    
    static double calculate_mean(const std::vector<PerformanceMetric>& metrics) {
        double sum = 0.0;
        for (const auto& metric : metrics) {
            sum += metric.value;
        }
        return sum / metrics.size();
    }
    
    static double calculate_standard_deviation(const std::vector<PerformanceMetric>& metrics,
                                                 double mean) {
        double sum_sq = 0.0;
        for (const auto& metric : metrics) {
            double diff = metric.value - mean;
            sum_sq += diff * diff;
        }
        return std::sqrt(sum_sq / metrics.size());
    }
    
    static double calculate_linear_regression_slope(const std::vector<uint64_t>& timestamps,
                                                      const std::vector<double>& values) {
        return calculate_linear_regression_slope(timestamps, values);
    }
    
    static double calculate_average_slope(const std::vector<PerformanceMetric>& metrics) {
        if (metrics.size() < 2) return 0.0;
        
        double total_slope = 0.0;
        for (size_t i = 1; i < metrics.size(); ++i) {
            double slope = (metrics[i].value - metrics[i-1].value) /
                          static_cast<double>(metrics[i].timestamp_ns - metrics[i-1].timestamp_ns);
            total_slope += slope;
        }
        
        return total_slope / (metrics.size() - 1);
    }
    
    static double predict_value(const std::vector<PerformanceMetric>& metrics,
                                 const std::string& prediction_type,
                                 size_t steps_ahead) {
        if (prediction_type == "linear") {
            if (metrics.size() < 2) return 0.0;
            
            double last_value = metrics.back().value;
            double avg_change = calculate_average_change(metrics);
            
            return last_value + avg_change * static_cast<double>(steps_ahead);
        }
        
        // Default to linear prediction
        return 0.0;
    }
    
    static double calculate_average_change(const std::vector<PerformanceMetric>& metrics) {
        if (metrics.size() < 2) return 0.0;
        
        double total_change = 0.0;
        for (size_t i = 1; i < metrics.size(); ++i) {
            double change = metrics[i].value - metrics[i-1].value;
            total_change += change;
        }
        
        return total_change / (metrics.size() - 1);
    }
    
    static double calculate_prediction_confidence(const std::vector<PerformanceMetric>& metrics,
                                                   size_t steps_ahead) {
        if (metrics.size() < 3) return 0.5;  // Default confidence
        
        // Simple confidence calculation based on data variance
        double mean = calculate_mean(metrics);
        double variance = 0.0;
        for (const auto& metric : metrics) {
            double diff = metric.value - mean;
            variance += diff * diff;
        }
        variance /= metrics.size();
        
        // Confidence decreases with steps ahead and increases with data stability
        double confidence = 1.0 / (1.0 + variance * static_cast<double>(steps_ahead));
        
        return std::min(0.95, std::max(0.1, confidence));
    }
    
    static double get_current_metric_value(const std::vector<PerformanceMetric>& metrics,
                                            const std::string& metric_name) {
        for (const auto& metric : metrics) {
            if (metric.metric_name == metric_name) {
                return metric.value;
            }
        }
        return 0.0;
    }
    
    static std::string categorize_optimization(const std::string& metric_name) {
        if (metric_name.find("cpu") != std::string::npos ||
            metric_name.find("CPU") != std::string::npos) {
            return "cpu";
        } else if (metric_name.find("memory") != std::string::npos ||
                   metric_name.find("Memory") != std::string::npos) {
            return "memory";
        } else if (metric_name.find("network") != std::string::npos ||
                   metric_name.find("Network") != std::string::npos) {
            return "network";
        } else if (metric_name.find("io") != std::string::npos ||
                   metric_name.find("IO") != std::string::npos) {
            return "io";
        } else {
            return "configuration";
        }
    }
    
    static double calculate_impact_score(double current_value, double target_value) {
        if (current_value == 0) return 0.0;
        
        double improvement = (target_value - current_value) / current_value;
        return std::min(1.0, std::max(0.0, improvement * 10.0));  // Scale impact score
    }
    
    static std::string generate_optimization_recommendation(const std::string& metric_name) {
        if (metric_name.find("cpu") != std::string::npos ||
            metric_name.find("CPU") != std::string::npos) {
            return "Optimize CPU-intensive operations and reduce context switching";
        } else if (metric_name.find("memory") != std::string::npos ||
                   metric_name.find("Memory") != std::string::npos) {
            return "Reduce memory footprint and improve cache locality";
        } else if (metric_name.find("network") != std::string::npos ||
                   metric_name.find("Network") != std::string::npos) {
            return "Optimize network protocols and reduce latency";
        } else if (metric_name.find("io") != std::string::npos ||
                   metric_name.find("IO") != std::string::npos) {
            return "Reduce I/O operations and improve data locality";
        } else {
            return "Configure parameters for better performance";
        }
    }
    
    static std::string assess_implementation_effort(const std::string& category) {
        if (category == "cpu") {
            return "high";
        } else if (category == "memory") {
            return "high";
        } else if (category == "network") {
            return "medium";
        } else if (category == "io") {
            return "medium";
        } else {
            return "low";
        }
    }
};

// Main Performance Monitoring Framework Class
class PerformanceMonitoringFramework {
private:
    std::vector<std::unique_ptr<MetricCollector>> collectors_;
    std::unique_ptr<PerformanceAlertSystem> alert_system_;
    std::unique_ptr<PerformanceReporter> reporter_;
    std::mutex monitoring_mutex_;
    bool enabled_;
    uint64_t collection_interval_ns_;
    
public:
    PerformanceMonitoringFramework(uint64_t collection_interval_ns = 1000000ULL)
        : collection_interval_ns_(collection_interval_ns), enabled_(false) {
        
        // Create default collectors
        collectors_.push_back(std::make_unique<CPUMetricCollector>());
        collectors_.push_back(std::make_unique<MemoryMetricCollector>());
        collectors_.push_back(std::make_unique<NetworkMetricCollector>());
        collectors_.push_back(std::make_unique<IOMetricCollector>());
        
        alert_system_ = std::make_unique<PerformanceAlertSystem>();
        reporter_ = std::make_unique<PerformanceReporter>();
    }
    
    void add_collector(std::unique_ptr<MetricCollector> collector) {
        collectors_.push_back(std::move(collector));
    }
    
    void add_alert_rule(const PerformanceAlertRule& rule) {
        alert_system_->add_alert_rule(rule);
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(monitoring_mutex_);
        enabled_ = true;
        
        for (auto& collector : collectors_) {
            collector->start();
        }
        
        alert_system_->start();
        reporter_->start();
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(monitoring_mutex_);
        
        reporter_->stop();
        alert_system_->stop();
        
        for (auto& collector : collectors_) {
            collector->stop();
        }
        
        enabled_ = false;
    }
    
    PerformanceReport collect_and_analyze() {
        std::lock_guard<std::mutex> lock(monitoring_mutex_);
        
        if (!enabled_) return PerformanceReport{};
        
        PerformanceReport report;
        report.start_time_ns = getCurrentTimestamp();
        
        // Collect metrics from all collectors
        std::vector<PerformanceMetric> all_metrics;
        for (auto& collector : collectors_) {
            const auto& metrics = collector->get_metrics();
            all_metrics.insert(all_metrics.end(), metrics.begin(), metrics.end());
        }
        
        report.metrics = all_metrics;
        
        // Check alerts
        alert_system_->check_alerts(all_metrics);
        
        // Copy active alerts
        report.alerts = alert_system_->get_active_alerts();
        
        // Generate summary metrics
        report.summary_metrics = calculate_summary_metrics(all_metrics);
        
        // Set end time and duration
        report.end_time_ns = getCurrentTimestamp();
        report.total_duration_seconds = 
            (report.end_time_ns - report.start_time_ns) / 1e9;
        
        // Perform analysis
        report = analyze_performance_report(report);
        
        return report;
    }
    
    std::vector<PerformanceTrend> analyze_trends(const std::string& metric_name,
                                                 const std::chrono::nanoseconds& period) {
        return PerformanceAnalyzer::analyze_trends(
            get_current_metrics(), metric_name, period);
    }
    
    std::vector<PerformanceAnomaly> detect_anomalies(const std::string& metric_name,
                                                      double threshold) {
        return PerformanceAnalyzer::detect_anomalies(
            get_current_metrics(), metric_name, threshold);
    }
    
    std::vector<PerformancePrediction> predict_performance(
        const std::string& metric_name,
        const std::chrono::nanoseconds& forecast_horizon,
        const std::string& prediction_type = "linear") {
        return PerformanceAnalyzer::predict_performance(
            get_current_metrics(), metric_name, forecast_horizon, prediction_type);
    }
    
    std::vector<OptimizationOpportunity> identify_optimization_opportunities(
        const std::map<std::string, double>& target_values) {
        return PerformanceAnalyzer::identify_optimization_opportunities(
            get_current_metrics(), target_values);
    }
    
    void generate_report(const PerformanceReport& report, const std::string& filename) {
        reporter_->save_report(report, filename);
    }
    
    void print_report(const PerformanceReport& report) {
        std::cout << reporter_->generate_text_report(report);
    }
    
    const std::vector<PerformanceMetric>& get_current_metrics() {
        std::lock_guard<std::mutex> lock(monitoring_mutex_);
        
        // Collect current metrics from all collectors
        std::vector<PerformanceMetric> current_metrics;
        for (auto& collector : collectors_) {
            const auto& metrics = collector->get_metrics();
            current_metrics.insert(current_metrics.end(), metrics.begin(), metrics.end());
        }
        
        // Clear metrics to avoid duplicates on next collection
        for (auto& collector : collectors_) {
            collector->clear_metrics();
        }
        
        return current_metrics;
    }
    
private:
    std::map<std::string, double> calculate_summary_metrics(const std::vector<PerformanceMetric>& metrics) {
        std::map<std::string, double> summary;
        
        // Calculate basic statistics for each metric
        std::map<std::string, std::vector<double>> metric_values;
        for (const auto& metric : metrics) {
            metric_values[metric.metric_name].push_back(metric.value);
        }
        
        for (const auto& pair : metric_values) {
            const auto& values = pair.second;
            if (values.empty()) continue;
            
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            double mean = sum / values.size();
            
            double sum_sq = 0.0;
            for (double value : values) {
                double diff = value - mean;
                sum_sq += diff * diff;
            }
            double std_dev = std::sqrt(sum_sq / values.size());
            
            summary[pair.first + "_mean"] = mean;
            summary[pair.first + "_stddev"] = std_dev;
            summary[pair.first + "_min"] = *std::min_element(values.begin(), values.end());
            summary[pair.first + "_max"] = *std::max_element(values.begin(), values.end());
        }
        
        return summary;
    }
    
    PerformanceReport analyze_performance_report(PerformanceReport report) {
        // Analyze the report for trends, anomalies, and optimization opportunities
        // This is a simplified implementation
        
        // Add trend analysis for key metrics
        std::vector<std::string> key_metrics = {
            "cpu_usage_percentage",
            "memory_usage_bytes",
            "network_bytes_sent",
            "io_read_bytes"
        };
        
        for (const auto& metric_name : key_metrics) {
            std::vector<PerformanceTrend> trends = 
                PerformanceAnalyzer::analyze_trends(report.metrics, metric_name, 
                                                   std::chrono::nanoseconds(10000000));  // 10 seconds
            // In a real implementation, we would store these trends in the report
        }
        
        // Add anomaly detection
        for (const auto& metric_name : key_metrics) {
            std::vector<PerformanceAnomaly> anomalies = 
                PerformanceAnalyzer::detect_anomalies(report.metrics, metric_name);
            // In a real implementation, we would store these anomalies in the report
        }
        
        return report;
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Global performance monitoring framework instance
static std::unique_ptr<PerformanceMonitoringFramework> global_monitoring_framework;

// Convenience functions
namespace nlm {
namespace monitoring {

inline PerformanceMonitoringFramework& getGlobalMonitoringFramework() {
    if (!global_monitoring_framework) {
        global_monitoring_framework = std::make_unique<PerformanceMonitoringFramework>();
    }
    return *global_monitoring_framework;
}

inline void start_performance_monitoring() {
    getGlobalMonitoringFramework().start();
}

inline void stop_performance_monitoring() {
    getGlobalMonitoringFramework().stop();
}

inline PerformanceReport collect_performance_data() {
    return getGlobalMonitoringFramework().collect_and_analyze();
}

inline std::vector<PerformanceTrend> analyze_performance_trends(const std::string& metric_name,
                                                                const std::chrono::nanoseconds& period) {
    return getGlobalMonitoringFramework().analyze_trends(metric_name, period);
}

inline std::vector<PerformanceAnomaly> detect_performance_anomalies(const std::string& metric_name,
                                                                   double threshold) {
    return getGlobalMonitoringFramework().detect_anomalies(metric_name, threshold);
}

inline std::vector<PerformancePrediction> predict_performance(const std::string& metric_name,
                                                               const std::chrono::nanoseconds& forecast_horizon,
                                                               const std::string& prediction_type = "linear") {
    return getGlobalMonitoringFramework().predict_performance(metric_name, forecast_horizon, prediction_type);
}

inline std::vector<OptimizationOpportunity> identify_optimization_opportunities(
    const std::map<std::string, double>& target_values) {
    return getGlobalMonitoringFramework().identify_optimization_opportunities(target_values);
}

inline void generate_performance_report(const PerformanceReport& report,
                                        const std::string& filename) {
    getGlobalMonitoringFramework().generate_report(report, filename);
}

inline void print_performance_report(const PerformanceReport& report) {
    getGlobalMonitoringFramework().print_report(report);
}

} // namespace monitoring
} // namespace nlm

// Example usage and demonstration
void demonstrate_performance_monitoring() {
    std::cout << "=== NLM Performance Monitoring Demonstration ===" << std::endl;
    
    // Start performance monitoring
    nlm::monitoring::start_performance_monitoring();
    
    // Add some alert rules
    PerformanceAlertRule cpu_alert;
    cpu_alert.rule_name = "high_cpu_usage";
    cpu_alert.metric_name = "cpu_usage_percentage";
    cpu_alert.condition = "greater_than";
    cpu_alert.threshold_value = 80.0;
    cpu_alert.severity = "warning";
    cpu_alert.message = "High CPU usage detected";
    cpu_alert.enabled = true;
    
    nlm::monitoring::add_alert_rule(cpu_alert);
    
    PerformanceAlertRule memory_alert;
    memory_alert.rule_name = "high_memory_usage";
    memory_alert.metric_name = "memory_usage_bytes";
    memory_alert.condition = "greater_than";
    memory_alert.threshold_value = 1024 * 1024 * 1024;  // 1GB
    memory_alert.severity = "critical";
    memory_alert.message = "High memory usage detected";
    memory_alert.enabled = true;
    
    nlm::monitoring::add_alert_rule(memory_alert);
    
    // Collect some data
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        PerformanceReport report = nlm::monitoring::collect_performance_data();
        nlm::monitoring::print_performance_report(report);
    }
    
    // Stop monitoring
    nlm::monitoring::stop_performance_monitoring();
}

// Command-line interface for performance monitoring
void performance_monitoring_cli(int argc, char* argv[]) {
    std::cout << "NLM Performance Monitoring CLI" << std::endl;
    std::cout << "Usage: nlm_monitor [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --demo                    Run monitoring demonstration" << std::endl;
    std::cout << "  --start                   Start performance monitoring" << std::endl;
    std::cout << "  --stop                    Stop performance monitoring" << std::endl;
    std::cout << "  --collect                 Collect and analyze performance data" << std::endl;
    std::cout << "  --trends <metric>         Analyze trends for a specific metric" << std::endl;
    std::cout << "  --anomalies <metric>      Detect anomalies for a specific metric" << std::endl;
    std::cout << "  --predict <metric>        Predict future performance for a metric" << std::endl;
    std::cout << "  --optimize                Identify optimization opportunities" << std::endl;
    std::cout << "  --report <filename>       Generate performance report" << std::endl;
    
    // Simple argument parsing (in real implementation would use proper parsing)
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--demo") {
            demonstrate_performance_monitoring();
        }
    }
}