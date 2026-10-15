// NLM Performance Monitoring System
// Advanced performance monitoring and profiling for expert users
// Provides detailed performance metrics, bottleneck analysis, and optimization recommendations

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/WorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <thread>
#include <future>
#include <numeric>
#include <cmath>

using namespace nlm;
using namespace std;

// Performance metrics types
enum class PerformanceMetric {
    CPU_USAGE,
    MEMORY_USAGE,
    SIMULATION_TIME,
    NEURON_PROCESSING_TIME,
    SYNAPSE_PROCESSING_TIME,
    SPHERE_PROCESSING_TIME,
    IO_TIME,
    SERIALIZATION_TIME,
    LATENCY,
    THROUGHPUT,
    CONSISTENCY,
    ACCURACY,
    RESPONSE_TIME
};

// Performance sample structure
struct PerformanceSample {
    double timestamp;
    map<PerformanceMetric, double> metrics;
    map<string, string> context;
    
    PerformanceSample(double time) : timestamp(time) {}
};

// Performance statistics
struct PerformanceStats {
    string name;
    double minValue;
    double maxValue;
    double meanValue;
    double medianValue;
    double stdDev;
    double p95Value;  // 95th percentile
    double p99Value;  // 99th percentile
    size_t sampleCount;
    
    PerformanceStats(const string& n) : name(n), minValue(0.0), maxValue(0.0),
                                         meanValue(0.0), medianValue(0.0),
                                         stdDev(0.0), p95Value(0.0), p99Value(0.0),
                                         sampleCount(0) {}
};

// Performance analyzer class for detailed analysis
class PerformanceAnalyzer {
private:
    vector<PerformanceSample> samples;
    map<PerformanceMetric, PerformanceStats> metricStats;
    map<string, vector<double>> timeSeriesData;
    
    chrono::high_resolution_clock::time_point sessionStart;
    double currentTime;
    bool monitoringEnabled;
    bool autoAnalysisEnabled;
    size_t maxSampleHistory;
    size_t analysisInterval;
    
    // Performance thresholds
    map<PerformanceMetric, double> thresholds;
    map<PerformanceMetric, string> alertMessages;
    
    // Performance counters
    size_t samplesCollected;
    size_t alertsTriggered;
    map<string, double> accumulatedTimes;
    
public:
    PerformanceAnalyzer() : currentTime(0.0), monitoringEnabled(false),
                           autoAnalysisEnabled(false), maxSampleHistory(10000),
                           analysisInterval(100), samplesCollected(0),
                           alertsTriggered(0) {
        sessionStart = chrono::high_resolution_clock::now();
        
        // Initialize default thresholds
        thresholds[PerformanceMetric::CPU_USAGE] = 80.0;  // 80%
        thresholds[PerformanceMetric::MEMORY_USAGE] = 2048.0;  // 2GB
        thresholds[PerformanceMetric::SIMULATION_TIME] = 1000.0;  // 1 second per step
        thresholds[PerformanceMetric::LATENCY] = 10.0;  // 10ms
        thresholds[PerformanceMetric::THROUGHPUT] = 100.0;  // 100 steps/second
        
        // Initialize alert messages
        alertMessages[PerformanceMetric::CPU_USAGE] = "CPU usage too high";
        alertMessages[PerformanceMetric::MEMORY_USAGE] = "Memory usage too high";
        alertMessages[PerformanceMetric::SIMULATION_TIME] = "Simulation step too slow";
        alertMessages[PerformanceMetric::LATENCY] = "High latency detected";
    }
    
    void enableMonitoring(bool enable = true) {
        monitoringEnabled = enable;
        if (enable) {
            NLM_LOG_INFO("Performance monitoring enabled");
        }
    }
    
    void enableAutoAnalysis(bool enable = true) {
        autoAnalysisEnabled = enable;
        if (enable) {
            NLM_LOG_INFO("Auto analysis enabled");
        }
    }
    
    // Record performance sample
    void recordSample(const map<PerformanceMetric, double>& metrics, 
                      const map<string, string>& context = {}) {
        if (!monitoringEnabled) return;
        
        PerformanceSample sample(currentTime);
        sample.metrics = metrics;
        sample.context = context;
        
        samples.push_back(sample);
        samplesCollected++;
        
        // Update metrics statistics
        for (const auto& [metric, value] : metrics) {
            updateMetricStats(metric, value);
        }
        
        // Update time series data
        for (const auto& [metric, value] : metrics) {
            timeSeriesData[getMetricName(metric)].push_back(value);
        }
        
        // Check for performance alerts
        if (autoAnalysisEnabled) {
            checkPerformanceAlerts(metrics);
        }
        
        // Collect statistics periodically
        if (samples.size() >= analysisInterval) {
            analyzePerformanceData();
        }
    }
    
    // Update metric statistics
    void updateMetricStats(PerformanceMetric metric, double value) {
        string metricName = getMetricName(metric);
        
        auto it = metricStats.find(metricName);
        if (it == metricStats.end()) {
            metricStats[metricName] = PerformanceStats(metricName);
            it = metricStats.find(metricName);
        }
        
        PerformanceStats& stats = it->second;
        
        // Update basic statistics
        if (stats.sampleCount == 0) {
            stats.minValue = value;
            stats.maxValue = value;
            stats.meanValue = value;
        } else {
            stats.minValue = min(stats.minValue, value);
            stats.maxValue = max(stats.maxValue, value);
            stats.meanValue = (stats.meanValue * stats.sampleCount + value) / (stats.sampleCount + 1);
        }
        
        stats.sampleCount++;
        
        // Sort values for percentile calculations
        vector<double>& values = timeSeriesData[metricName];
        sort(values.begin(), values.end());
        
        size_t size = values.size();
        if (size >= 1) {
            stats.medianValue = values[size / 2];
        }
        if (size >= 20) {
            stats.p95Value = values[(size * 95) / 100];
        }
        if (size >= 100) {
            stats.p99Value = values[(size * 99) / 100];
        }
        
        // Calculate standard deviation
        if (stats.sampleCount > 1) {
            double variance = 0.0;
            for (double val : values) {
                variance += (val - stats.meanValue) * (val - stats.meanValue);
            }
            variance /= (values.size() - 1);
            stats.stdDev = sqrt(variance);
        }
    }
    
    // Check for performance alerts
    void checkPerformanceAlerts(const map<PerformanceMetric, double>& metrics) {
        for (const auto& [metric, value] : metrics) {
            string metricName = getMetricName(metric);
            auto it = thresholds.find(metric);
            
            if (it != thresholds.end() && value > it->second) {
                string alertMessage = alertMessages.count(metric) ? alertMessages[metric] : "Performance issue";
                NLM_LOG_WARNING("[PERFORMANCE ALERT] " + alertMessage + 
                               " (" + metricName + ": " + to_string(value) + 
                               " > threshold " + to_string(it->second) + ")");
                alertsTriggered++;
            }
        }
    }
    
    // Analyze performance data
    void analyzePerformanceData() {
        NLM_LOG_INFO("=== Performance Analysis ===");
        NLM_LOG_INFO("Samples collected: " + to_string(samplesCollected));
        NLM_LOG_INFO("Alerts triggered: " + to_string(alertsTriggered));
        NLM_LOG_INFO("Active metrics: " + to_string(metricStats.size()));
        
        // Identify bottlenecks
        identifyBottlenecks();
        
        // Generate recommendations
        generateRecommendations();
    }
    
    // Identify performance bottlenecks
    void identifyBottlenecks() {
        NLM_LOG_INFO("Bottleneck Analysis:");
        
        // Find metrics with highest variance
        vector<pair<string, double>> varianceRanked;
        for (const auto& [name, stats] : metricStats) {
            if (stats.sampleCount > 1) {
                varianceRanked.push_back({name, stats.stdDev});
            }
        }
        
        sort(varianceRanked.begin(), varianceRanked.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (size_t i = 0; i < min(size_t(5), varianceRanked.size()); ++i) {
            const auto& [name, variance] = varianceRanked[i];
            NLM_LOG_INFO("  " + name + ": high variance (" + to_string(variance) + ")");
        }
    }
    
    // Generate performance recommendations
    void generateRecommendations() {
        NLM_LOG_INFO("Performance Recommendations:");
        
        // Check for slow metrics
        for (const auto& [name, stats] : metricStats) {
            if (name == "simulation_time" && stats.meanValue > 1000.0) {
                NLM_LOG_INFO("  Consider reducing simulation timestep");
            } else if (name == "neuron_processing_time" && stats.meanValue > 10.0) {
                NLM_LOG_INFO("  Consider optimizing neuron processing");
            } else if (name == "memory_usage" && stats.meanValue > 2048.0) {
                NLM_LOG_INFO("  Consider reducing memory footprint");
            } else if (name == "latency" && stats.meanValue > 10.0) {
                NLM_LOG_INFO("  Consider optimizing I/O operations");
            }
        }
    }
    
    // Generate performance report
    string generatePerformanceReport() const {
        ostringstream report;
        report << "=== NLM Performance Report ===" << endl;
        report << "Session duration: " << getSessionDuration() / 1000.0 << "s" << endl;
        report << "Samples collected: " << samplesCollected << endl;
        report << "Alerts triggered: " << alertsTriggered << endl;
        report << "Active metrics: " << metricStats.size() << endl;
        report << endl;
        
        // Summary statistics
        report << "Metric Summary:" << endl;
        for (const auto& [name, stats] : metricStats) {
            report << "  " << name << ": "
                   << "min=" << stats.minValue << ", "
                   << "max=" << stats.maxValue << ", "
                   << "mean=" << stats.meanValue << ", "
                   << "p95=" << stats.p95Value << ", "
                   << "p99=" << stats.p99Value << ", "
                   << "stddev=" << stats.stdDev << endl;
        }
        
        return report.str();
    }
    
    // Save performance data to file
    bool savePerformanceData(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "=== NLM Performance Data ===" << endl;
        file << "Format: CSV" << endl;
        file << "Timestamp,Metric,Value,Context" << endl;
        
        for (const auto& sample : samples) {
            for (const auto& [metric, value] : sample.metrics) {
                string metricName = getMetricName(metric);
                string contextStr = "";
                if (sample.context.find("step") != sample.context.end()) {
                    contextStr += "step=" + sample.context.at("step") + ",";
                }
                if (sample.context.find("region") != sample.context.end()) {
                    contextStr += "region=" + sample.context.at("region");
                }
                
                file << fixed << setprecision(3);
                file << sample.timestamp << "," << metricName << "," << value << "," << contextStr << endl;
            }
        }
        
        return true;
    }
    
    // Set performance threshold
    void setThreshold(PerformanceMetric metric, double threshold) {
        thresholds[metric] = threshold;
    }
    
    // Set alert message
    void setAlertMessage(PerformanceMetric metric, const string& message) {
        alertMessages[metric] = message;
    }
    
    // Get metric statistics
    const PerformanceStats* getMetricStats(PerformanceMetric metric) const {
        string metricName = getMetricName(metric);
        auto it = metricStats.find(metricName);
        return it != metricStats.end() ? &it->second : nullptr;
    }
    
    // Get metric statistics by name
    const PerformanceStats* getMetricStats(const string& name) const {
        auto it = metricStats.find(name);
        return it != metricStats.end() ? &it->second : nullptr;
    }
    
    // Update current time
    void updateTime(double time) {
        currentTime = time;
    }
    
    // Utility functions
    static string getMetricName(PerformanceMetric metric) {
        switch (metric) {
            case PerformanceMetric::CPU_USAGE: return "cpu_usage";
            case PerformanceMetric::MEMORY_USAGE: return "memory_usage";
            case PerformanceMetric::SIMULATION_TIME: return "simulation_time";
            case PerformanceMetric::NEURON_PROCESSING_TIME: return "neuron_processing_time";
            case PerformanceMetric::SYNAPSE_PROCESSING_TIME: return "synapse_processing_time";
            case PerformanceMetric::SENSOR_PROCESSING_TIME: return "sensor_processing_time";
            case PerformanceMetric::IO_TIME: return "io_time";
            case PerformanceMetric::SERIALIZATION_TIME: return "serialization_time";
            case PerformanceMetric::LATENCY: return "latency";
            case PerformanceMetric::THROUGHPUT: return "throughput";
            case PerformanceMetric::CONSISTENCY: return "consistency";
            case PerformanceMetric::ACCURACY: return "accuracy";
            case PerformanceMetric::RESPONSE_TIME: return "response_time";
            default: return "unknown";
        }
    }
    
    double getSessionDuration() const {
        auto now = chrono::high_resolution_clock::now();
        return chrono::duration<double, milli>(now - sessionStart).count() / 1000.0;
    }
    
    size_t getSampleCount() const { return samplesCollected; }
    bool isMonitoring() const { return monitoringEnabled; }
    bool isAutoAnalysisEnabled() const { return autoAnalysisEnabled; }
}; 

// Performance profiler for detailed function timing
class PerformanceProfiler {
private:
    struct ProfileData {
        string functionName;
        string category;
        chrono::high_resolution_clock::time_point startTime;
        chrono::duration<double, milli> duration;
        bool active;
    };
    
    vector<ProfileData> profileData;
    map<string, vector<chrono::duration<double, milli>>> categoryTimes;
    chrono::high_resolution_clock::time_point sessionStart;
    bool profilingEnabled;
    size_t maxProfileEntries;
    
public:
    PerformanceProfiler() : profilingEnabled(false), maxProfileEntries(10000) {
        sessionStart = chrono::high_resolution_clock::now();
    }
    
    void enableProfiling(bool enable = true) {
        profilingEnabled = enable;
        if (enable) {
            NLM_LOG_INFO("Performance profiler enabled");
        }
    }
    
    // Start profiling a function
    void startProfile(const string& functionName, const string& category = "general") {
        if (!profilingEnabled) return;
        
        profileData.push_back({
            functionName,
            category,
            chrono::high_resolution_clock::now(),
            chrono::duration<double, milli>(0),
            true
        });
    }
    
    // Stop profiling the last started function
    chrono::duration<double, milli> stopProfile() {
        if (!profilingEnabled || profileData.empty()) return chrono::duration<double, milli>(0);
        
        ProfileData& data = profileData.back();
        if (!data.active) return data.duration;
        
        auto endTime = chrono::high_resolution_clock::now();
        data.duration = chrono::duration<double, milli>(endTime - data.startTime);
        data.active = false;
        
        // Update category statistics
        categoryTimes[data.category].push_back(data.duration);
        
        return data.duration;
    }
    
    // Stop profiling by function name
    chrono::duration<double, milli> stopProfile(const string& functionName) {
        if (!profilingEnabled) return chrono::duration<double, milli>(0);
        
        for (auto& data : profileData) {
            if (data.functionName == functionName && data.active) {
                auto endTime = chrono::high_resolution_clock::now();
                data.duration = chrono::duration<double, milli>(endTime - data.startTime);
                data.active = false;
                categoryTimes[data.category].push_back(data.duration);
                return data.duration;
            }
        }
        
        return chrono::duration<double, milli>(0);
    }
    
    // Get total time for a category
    chrono::duration<double, milli> getCategoryTime(const string& category) const {
        auto it = categoryTimes.find(category);
        if (it == categoryTimes.end()) return chrono::duration<double, milli>(0);
        
        chrono::duration<double, milli> total(0);
        for (const auto& duration : it->second) {
            total += duration;
        }
        
        return total;
    }
    
    // Get top functions by execution time
    vector<pair<string, chrono::duration<double, milli>>> getTopFunctions(size_t limit = 10) const {
        map<string, chrono::duration<double, milli>> functionTimes;
        
        for (const auto& data : profileData) {
            functionTimes[data.functionName] += data.duration;
        }
        
        vector<pair<string, chrono::duration<double, milli>>> sorted(functionTimes.begin(), functionTimes.end());
        sort(sorted.begin(), sorted.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
        
        if (sorted.size() > limit) {
            sorted.resize(limit);
        }
        
        return sorted;
    }
    
    // Generate profile report
    string generateProfileReport() const {
        ostringstream report;
        report << "=== Performance Profile Report ===" << endl;
        report << "Session duration: " << getSessionDuration() / 1000.0 << "s" << endl;
        report << "Profile entries: " << profileData.size() << endl;
        report << "Categories: " << categoryTimes.size() << endl;
        report << endl;
        
        // Category summary
        report << "Category Summary:" << endl;
        for (const auto& [category, durations] : categoryTimes) {
            chrono::duration<double, milli> total(0);
            for (const auto& duration : durations) {
                total += duration;
            }
            
            report << "  " << category << ": " << total.count() / 1000.0 << "s ("
                   << durations.size() << " calls)" << endl;
        }
        
        report << endl;
        
        // Top functions
        report << "Top Functions:" << endl;
        auto topFunctions = getTopFunctions(10);
        for (size_t i = 0; i < topFunctions.size(); ++i) {
            const auto& [name, duration] = topFunctions[i];
            report << "  " << (i + 1) << ". " << name << ": "
                   << duration.count() / 1000.0 << "s" << endl;
        }
        
        return report.str();
    }
    
    // Save profile data
    bool saveProfile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "=== NLM Performance Profile ===" << endl;
        file << "Format: JSON" << endl;
        file << "Session duration: " << getSessionDuration() << "ms" << endl;
        file << "Profile entries: " << profileData.size() << endl;
        file << endl;
        
        for (const auto& data : profileData) {
            file << "{\"function\": \"" << data.functionName << "\", \"category\": \"" 
                 << data.category << "\", \"duration_ms\": " << data.duration.count()
                 << "}" << endl;
        }
        
        return true;
    }
    
    double getSessionDuration() const {
        auto now = chrono::high_resolution_clock::now();
        return chrono::duration<double, milli>(now - sessionStart).count();
    }
    
    bool isProfiling() const { return profilingEnabled; }
    size_t getProfileEntryCount() const { return profileData.size(); }
}; 

// NLM performance monitoring system
class NLMPerformanceMonitoring {
private:
    PerformanceAnalyzer analyzer;
    PerformanceProfiler profiler;
    map<string, string> monitoringSettings;
    
public:
    NLMPerformanceMonitoring() {
        // Initialize default monitoring settings
        monitoringSettings["enable_profiling"] = "true";
        monitoringSettings["enable_analysis"] = "true";
        monitoringSettings["enable_alerts"] = "true";
        monitoringSettings["profile_categories"] = "neuron,synapse,io,serialization";
        monitoringSettings["analysis_interval"] = "100";
        monitoringSettings["max_samples"] = "10000";
        monitoringSettings["alert_threshold_cpu"] = "80";
        monitoringSettings["alert_threshold_memory"] = "2048";
        monitoringSettings["alert_threshold_latency"] = "10";
    }
    
    void initialize() {
        // Configure monitoring based on settings
        if (monitoringSettings["enable_profiling"] == "true") {
            profiler.enableProfiling(true);
        }
        
        if (monitoringSettings["enable_analysis"] == "true") {
            analyzer.enableMonitoring(true);
            analyzer.enableAutoAnalysis(true);
        }
        
        if (monitoringSettings["enable_alerts"] == "true") {
            // Configure alert thresholds
            analyzer.setThreshold(PerformanceMetric::CPU_USAGE, 
                                 stod(monitoringSettings["alert_threshold_cpu"]));
            analyzer.setThreshold(PerformanceMetric::MEMORY_USAGE,
                                 stod(monitoringSettings["alert_threshold_memory"]));
            analyzer.setThreshold(PerformanceMetric::LATENCY,
                                 stod(monitoringSettings["alert_threshold_latency"]));
        }
        
        NLM_LOG_INFO("NLM Performance Monitoring System initialized");
    }
    
    // Start profiling a function
    void startProfile(const string& functionName, const string& category = "general") {
        profiler.startProfile(functionName, category);
    }
    
    // Stop profiling a function
    chrono::duration<double, milli> stopProfile(const string& functionName = "") {
        if (functionName.empty()) {
            return profiler.stopProfile();
        } else {
            return profiler.stopProfile(functionName);
        }
    }
    
    // Record performance sample
    void recordSample(const map<PerformanceMetric, double>& metrics, 
                      const map<string, string>& context = {}) {
        analyzer.recordSample(metrics, context);
    }
    
    // Get performance report
    string getPerformanceReport() const {
        ostringstream report;
        report << "=== NLM Performance Monitoring Report ===" << endl;
        report << profiler.generateProfileReport() << endl;
        report << analyzer.generatePerformanceReport() << endl;
        
        return report.str();
    }
    
    // Save monitoring data
    bool saveMonitoringData(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << getPerformanceReport();
        file << "\n=== Profile Data ===\n";
        profiler.saveProfile("profile_data.json");
        file << "Profile data saved to: profile_data.json" << endl;
        
        file << "\n=== Performance Data ===\n";
        analyzer.savePerformanceData("performance_data.csv");
        file << "Performance data saved to: performance_data.csv" << endl;
        
        file.close();
        NLM_LOG_INFO("Performance monitoring data saved to: " + filename);
        return true;
    }
    
    // Get analyzer reference
    PerformanceAnalyzer& getAnalyzer() { return analyzer; }
    
    // Get profiler reference
    PerformanceProfiler& getProfiler() { return profiler; }
}; 

// Performance integration with NLM core
class NLMAdvancedPerformance {
public:
    static NLMPerformanceMonitoring createPerformanceSession() {
        NLMPerformanceMonitoring performance;
        performance.initialize();
        return performance;
    }
    
    static void attachPerformanceToSimulation(NLMPerformanceMonitoring& performance, 
                                             SimulationStep step, Timestamp timestamp) {
        // Record simulation step performance
        map<PerformanceMetric, double> metrics;
        metrics[PerformanceMetric::SIMULATION_TIME] = 1.0;  // Placeholder
        metrics[PerformanceMetric::LATENCY] = timestamp - static_cast<Timestamp>(step);  // Placeholder
        
        performance.recordSample(metrics, {
            {"step", to_string(step)},
            {"timestamp", to_string(timestamp)}
        });
        
        // Update profiler timing
        performance.stopProfile("simulation_step");
    }
}; 

// Performance benchmarking utilities
namespace Benchmarking {
    class Benchmark {
    public:
        string name;
        vector<double> times;
        chrono::high_resolution_clock::time_point startTime;
        
        Benchmark(const string& n) : name(n) {}
        
        void start() {
            startTime = chrono::high_resolution_clock::now();
        }
        
        void stop() {
            auto endTime = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> duration = endTime - startTime;
            times.push_back(duration.count());
        }
        
        double getMeanTime() const {
            if (times.empty()) return 0.0;
            double sum = accumulate(times.begin(), times.end(), 0.0);
            return sum / times.size();
        }
        
        double getMinTime() const {
            if (times.empty()) return 0.0;
            return *min_element(times.begin(), times.end());
        }
        
        double getMaxTime() const {
            if (times.empty()) return 0.0;
            return *max_element(times.begin(), times.end());
        }
        
        double getStdDev() const {
            if (times.size() <= 1) return 0.0;
            double mean = getMeanTime();
            double sum = 0.0;
            for (double time : times) {
                sum += (time - mean) * (time - mean);
            }
            return sqrt(sum / (times.size() - 1));
        }
    };
    
    // Run benchmark suite
    map<string, Benchmark> runBenchmarkSuite() {
        map<string, Benchmark> benchmarks;
        
        // Create benchmark instances
        benchmarks["neuron_creation"] = Benchmark("neuron_creation");
        benchmarks["synapse_creation"] = Benchmark("synapse_creation");
        benchmarks["simulation_step"] = Benchmark("simulation_step");
        benchmarks["sensory_processing"] = Benchmark("sensory_processing");
        benchmarks["motor_output"] = Benchmark("motor_output");
        benchmarks["memory_update"] = Benchmark("memory_update");
        benchmarks["plasticity_application"] = Benchmark("plasticity_application");
        
        // Run benchmarks (placeholder implementation)
        NLM_LOG_INFO("Running benchmark suite...");
        
        for (auto& [name, bench] : benchmarks) {
            bench.start();
            // Simulate work
            this_thread::sleep_for(chrono::milliseconds(10));
            bench.stop();
            
            NLM_LOG_INFO("  " + name + ": mean=" + to_string(bench.getMeanTime()) + 
                       "ms, min=" + to_string(bench.getMinTime()) + 
                       "ms, max=" + to_string(bench.getMaxTime()) + "ms");
        }
        
        NLM_LOG_INFO("Benchmark suite complete");
        return benchmarks;
    }
    
    // Generate benchmark report
    string generateBenchmarkReport(const map<string, Benchmark>& benchmarks) {
        ostringstream report;
        report << "=== NLM Benchmark Report ===" << endl;
        report << "Benchmark suite completed" << endl;
        report << endl;
        
        report << "Benchmark Results:" << endl;
        for (const auto& [name, bench] : benchmarks) {
            report << "  " << name << ":" << endl;
            report << "    Mean: " << bench.getMeanTime() << "ms" << endl;
            report << "    Min: " << bench.getMinTime() << "ms" << endl;
            report << "    Max: " << bench.getMaxTime() << "ms" << endl;
            report << "    StdDev: " << bench.getStdDev() << "ms" << endl;
            report << "    Samples: " << bench.times.size() << endl;
            report << endl;
        }
        
        return report.str();
    }
    
    // Run performance regression test
    bool runPerformanceRegressionTest(const map<string, Benchmark>& baseline) {
        NLM_LOG_INFO("Running performance regression test...");
        
        auto current = runBenchmarkSuite();
        
        bool regressionDetected = false;
        for (const auto& [name, currentBench] : current) {
            if (baseline.find(name) != baseline.end()) {
                const Benchmark& baselineBench = baseline.at(name);
                
                double currentMean = currentBench.getMeanTime();
                double baselineMean = baselineBench.getMeanTime();
                double regressionPercent = ((currentMean - baselineMean) / baselineMean) * 100.0;
                
                NLM_LOG_INFO("  " + name + ": baseline=" << baselineMean << "ms, "
                           << "current=" << currentMean << "ms, "
                           << "regression=" << regressionPercent << "%");
                
                if (regressionPercent > 10.0) {  // 10% regression threshold
                    NLM_LOG_WARNING("  REGRESSION DETECTED: " + name + " performance degraded by " +
                                  to_string(regressionPercent) + "%");
                    regressionDetected = true;
                }
            }
        }
        
        if (regressionDetected) {
            NLM_LOG_WARNING("Performance regression test FAILED");
        } else {
            NLM_LOG_INFO("Performance regression test PASSED");
        }
        
        return !regressionDetected;
    }
}; 

// Performance monitoring integration with NLM core
class NLMAdvancedPerformanceIntegration {
public:
    static NLMPerformanceMonitoring createIntegratedPerformanceSession() {
        NLMPerformanceMonitoring performance;
        performance.initialize();
        return performance;
    }
    
    static void attachPerformanceToBrainSimulation(NLMPerformanceMonitoring& performance, 
                                                   SimulationStep step, Timestamp timestamp) {
        // Record basic performance metrics
        map<PerformanceMetric, double> metrics;
        metrics[PerformanceMetric::SIMULATION_TIME] = 1.0;  // Placeholder - should measure actual time
        metrics[PerformanceMetric::LATENCY] = timestamp - static_cast<Timestamp>(step);  // Placeholder
        
        performance.recordSample(metrics, {
            {"step", to_string(step)},
            {"timestamp", to_string(timestamp)},
            {"operation", "simulation"}
        });
    }
}; 

// Performance utilities for advanced users
namespace Performance {
    // Simple timing utilities
    class Timer {
    public:
        chrono::high_resolution_clock::time_point startTime;
        
        void start() {
            startTime = chrono::high_resolution_clock::now();
        }
        
        double stop() {
            auto endTime = chrono::high_resolution_clock::now();
            chrono::duration<double, milli> duration = endTime - startTime;
            return duration.count();
        }
    };
    
    // Memory utilities
    namespace Memory {
        static size_t getAvailableMemory() {
            // Simple implementation - in real code would use system-specific calls
            return 0;
        }
        
        static void logMemoryUsage(const string& label = "Memory") {
            NLM_LOG_INFO("[Performance] " + label + ": " + to_string(getAvailableMemory()) + " bytes");
        }
    }
    
    // CPU utilities
    namespace CPU {
        static double getCPULoad() {
            // Simple implementation - in real code would use system-specific calls
            return 0.0;
        }
        
        static void logCPULoad(const string& label = "CPU") {
            NLM_LOG_INFO("[Performance] " + label + ": " + to_string(getCPULoad()) + "%");
        }
    }
    
    // I/O utilities
    namespace IO {
        static void logReadOperation(const string& filename, double time) {
            NLM_LOG_INFO("[Performance] IO Read: " + filename + " (" + to_string(time) + "ms)");
        }
        
        static void logWriteOperation(const string& filename, double time) {
            NLM_LOG_INFO("[Performance] IO Write: " + filename + " (" + to_string(time) + "ms)");
        }
    }
    
    // Network utilities (for distributed computing)
    namespace Network {
        static void logNetworkOperation(const string& operation, double time, const string& remote = "") {
            NLM_LOG_INFO("[Performance] Network " + operation + ": " + to_string(time) + "ms" +
                       (remote.empty() ? "" : " (" + remote + ")"));
        }
    }
}