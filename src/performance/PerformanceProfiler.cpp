#include "PerformanceProfiler.hpp"
#include <thread>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace nlm {

RealTimeMonitor::RealTimeMonitor() {
    metrics_history.reserve(max_history_size.load());
}

RealTimeMonitor::~RealTimeMonitor() {
    stop();
}

void RealTimeMonitor::start() {
    if (running) return;
    
    running = true;
    monitor_thread = std::thread([this]() {
        while (running) {
            ProfileMetrics metrics = getCurrentMetrics();
            {
                std::lock_guard<std::mutex> lock(monitor_mutex);
                metrics_history.push_back(metrics);
                if (metrics_history.size() > max_history_size.load()) {
                    metrics_history.pop_front();
                }
            }
            
            // Call metrics callback if set
            if (metrics_callback) {
                metrics_callback(metrics);
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
}

void RealTimeMonitor::stop() {
    if (!running) return;
    
    running = false;
    if (monitor_thread.joinable()) {
        monitor_thread.join();
    }
}

void RealTimeMonitor::addComponentProfile(const std::string& name, const std::string& type) {
    std::lock_guard<std::mutex> lock(monitor_mutex);
    component_profiles[name] = ComponentProfile(name, type);
}

void RealTimeMonitor::updateComponentMetrics(const std::string& name, const ProfileMetrics& metrics) {
    std::lock_guard<std::mutex> lock(monitor_mutex);
    if (component_profiles.find(name) != component_profiles.end()) {
        ComponentProfile& profile = component_profiles[name];
        profile.operations_count++;
        if (metrics.cpu_time > 0.0) {
            profile.total_time += metrics.cpu_time;
        }
        // Update averages (simplified)
        profile.average_cpu_usage = (profile.average_cpu_usage * (profile.operations_count - 1) + metrics.cpu_utilization) / profile.operations_count;
        profile.average_memory_usage = (profile.average_memory_usage * (profile.operations_count - 1) + metrics.memory_utilization) / profile.operations_count;
    }
}

void RealTimeMonitor::setMetricsCallback(std::function<void(const ProfileMetrics&)> callback) {
    metrics_callback = std::move(callback);
}

void RealTimeMonitor::setProgressCallback(std::function<void(const std::string&, double)> callback) {
    progress_callback = std::move(callback);
}

ProfileMetrics RealTimeMonitor::getCurrentMetrics() const {
    ProfileMetrics metrics;
    metrics.timestamp = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Get system metrics (simplified - in real implementation, use system APIs)
    metrics.cpu_time = current_cpu_usage.load();
    metrics.wall_time = metrics.timestamp;
    metrics.real_time = metrics.timestamp;
    
    metrics.memory_used = current_memory_used.load();
    metrics.memory_peak = metrics.memory_used; // Would track peak in real implementation
    metrics.memory_allocated = metrics.memory_used;
    
    metrics.neuron_activity = 0.5; // Placeholder
    metrics.spike_rate = 10.0;     // Placeholder
    metrics.synapse_weight_change = 0.0; // Placeholder
    
    metrics.cpu_utilization = current_cpu_usage.load();
    metrics.memory_utilization = current_memory_usage.load();
    
    // Count events (simplified)
    metrics.neuron_creation_events = 0;
    metrics.synapse_creation_events = 0;
    metrics.plasticity_events = 0;
    metrics.memory_access_events = 0;
    
    metrics.error_count = 0;
    
    return metrics;
}

std::vector<ProfileMetrics> RealTimeMonitor::getMetricsHistory(size_t count) const {
    std::lock_guard<std::mutex> lock(monitor_mutex);
    size_t start_idx = metrics_history.size() > count ? metrics_history.size() - count : 0;
    return std::vector<ProfileMetrics>(metrics_history.begin() + start_idx, metrics_history.end());
}

ComponentProfile RealTimeMonitor::getComponentProfile(const std::string& name) const {
    std::lock_guard<std::mutex> lock(monitor_mutex);
    auto it = component_profiles.find(name);
    return it != component_profiles.end() ? it->second : ComponentProfile(name, "unknown");
}

void RealTimeMonitor::generatePerformanceReport(const std::string& output_file) const {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        return;
    }
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n<title>NLM Performance Report</title>\n<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
         << "h1 { color: #333; }\n"
         << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n"
         << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
         << "th { background-color: #f2f2f2; }\n"
         << "tr:nth-child(even) { background-color: #f9f9f9; }\n"
         << "</style>\n</head>\n<body>\n"
         << "<h1>NLM Performance Monitoring Report</h1>\n"
         << "<p>Generated: " << std::chrono::system_clock::now() << "</p>\n"
         << "<h2>Metrics History</h2>\n"
         << "<table>\n"
         << "<tr><th>Timestamp</th><th>CPU Usage</th><th>Memory Usage</th><th>CPU Time</th></tr>\n";
    
    for (const auto& metrics : metrics_history) {
        file << "<tr><td>" << metrics.timestamp
             << "</td><td>" << metrics.cpu_utilization << "%</td><td>" << metrics.memory_utilization << "%</td><td>"
             << metrics.cpu_time << "</td></tr>\n";
    }
    
    file << "</table>\n</body>\n</html>";
    file.close();
}

PerformanceProfiler::PerformanceProfiler(
    std::shared_ptr<Logger> logger,
    std::shared_ptr<SimulationClock> clock,
    std::shared_ptr<Brain> brain,
    const std::string& output_dir
) : logger(logger), clock(clock), brain(brain), output_directory(output_dir),
    sampling_interval(0.1), save_detailed_data(true), enable_real_time_monitoring(true) {
    
    // Create output directory
    std::filesystem::create_directories(output_directory);
    
    // Initialize monitor
    monitor = std::make_unique<RealTimeMonitor>();
    
    if (enable_real_time_monitoring) {
        monitor->start();
    }
    
    // Setup file outputs
    if (save_detailed_data) {
        std::filesystem::path metrics_path = std::filesystem::path(output_directory) / "metrics.csv";
        metrics_file.open(metrics_path);
        metrics_file << "timestamp,component,operation,duration,memory_usage,cpu_usage,success" << std::endl;
    }
    
    std::filesystem::path benchmarks_path = std::filesystem::path(output_directory) / "benchmarks.json";
    benchmarks_file.open(benchmarks_path);
    benchmarks_file << "[";
}

PerformanceProfiler::~PerformanceProfiler() {
    stopProfiling();
    
    if (metrics_file.is_open()) {
        metrics_file.close();
    }
    if (benchmarks_file.is_open()) {
        benchmarks_file << "]";
        benchmarks_file.close();
    }
}

void PerformanceProfiler::configure(
    double sampling_interval,
    bool save_detailed,
    bool enable_monitoring
) {
    std::lock_guard<std::mutex> lock(profiler_mutex);
    this->sampling_interval = sampling_interval;
    this->save_detailed_data = save_detailed;
    this->enable_real_time_monitoring = enable_monitoring;
}

void PerformanceProfiler::startProfiling() {
    std::lock_guard<std::mutex> lock(profiler_mutex);
    profiling_enabled = true;
    logger->log(INFO, "Performance profiling started");
}

void PerformanceProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(profiler_mutex);
    profiling_enabled = false;
    logger->log(INFO, "Performance profiling stopped");
    saveDataToFiles();
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(profiler_mutex);
    active_profiles.clear();
    profiling_history.clear();
    logger->log(INFO, "Performance profiler reset");
}

void PerformanceProfiler::startComponentProfile(const std::string& name, const std::string& type) {
    if (!profiling_enabled) return;
    
    std::lock_guard<std::mutex> lock(profiler_mutex);
    active_profiles[name] = ComponentProfile(name, type);
    logger->log(DEBUG, "Started profiling component: " + name);
}

void PerformanceProfiler::endComponentProfile(const std::string& name, const ProfileMetrics& metrics) {
    if (!profiling_enabled) return;
    
    std::lock_guard<std::mutex> lock(profiler_mutex);
    auto it = active_profiles.find(name);
    if (it != active_profiles.end()) {
        ComponentProfile& profile = it->second;
        profile.operations_count++;
        if (metrics.cpu_time > 0.0) {
            profile.total_time += metrics.cpu_time;
        }
        if (metrics.success) {
            profile.successful_operations++;
        } else {
            profile.failed_operations++;
        }
        
        // Update component metrics in monitor
        if (monitor) {
            monitor->updateComponentMetrics(name, metrics);
        }
        
        // Save detailed data
        if (save_detailed_data && metrics_file.is_open()) {
            metrics_file << std::fixed << std::setprecision(6)
                         << metrics.timestamp << "," << name << ","
                         << "component," << metrics.cpu_time << ","
                         << metrics.memory_used << "," << metrics.cpu_utilization
                         << "," << (metrics.success ? "1" : "0") << std::endl;
        }
        
        logger->log(DEBUG, "Ended profiling component: " + name + ", time: " + std::to_string(metrics.cpu_time) + "s");
        
        active_profiles.erase(it);
    }
}

void PerformanceProfiler::profileSystemState() {
    if (!profiling_enabled) return;
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    
    // Collect system metrics (simplified)
    metrics.cpu_time = 0.0; // Would get actual CPU time
    metrics.wall_time = 0.0; // Would get actual wall time
    metrics.real_time = metrics.timestamp;
    
    metrics.memory_used = 0; // Would get actual memory usage
    metrics.memory_peak = metrics.memory_used;
    metrics.memory_allocated = metrics.memory_used;
    
    metrics.cpu_utilization = 0.0; // Would get actual CPU utilization
    metrics.memory_utilization = 0.0; // Would get actual memory utilization
    
    metrics.error_count = 0; // Would count actual errors
    
    profiling_history.push_back(metrics);
    
    if (save_detailed_data && metrics_file.is_open()) {
        metrics_file << std::fixed << std::setprecision(6)
                     << metrics.timestamp << ",system,general," << metrics.cpu_time << ","
                     << metrics.memory_used << "," << metrics.cpu_utilization << ",1" << std::endl;
    }
}

void PerformanceProfiler::profileNeuralActivity() {
    if (!profiling_enabled || !brain) return;
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    
    // Collect neural activity metrics
    size_t total_neurons = brain->getTotalNeuronCount();
    size_t active_neurons = 0;
    double total_activity = 0.0;
    
    // Sample neurons to get activity data
    if (total_neurons > 0) {
        // In a real implementation, we would sample actual neuron states
        active_neurons = total_neurons / 10; // Placeholder: assume 10% active
        total_activity = static_cast<double>(active_neurons) * 5.0; // Placeholder activity
    }
    
    metrics.neuron_activity = total_neurons > 0 ? total_activity / total_neurons : 0.0;
    metrics.spike_rate = static_cast<double>(brain->getTotalSpikeCount()) / 1000.0; // Per second estimate
    
    profiling_history.push_back(metrics);
}

void PerformanceProfiler::profileMemoryUsage() {
    if (!profiling_enabled) return;
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    
    // Get memory usage (simplified)
    metrics.memory_used = 0; // Would get actual memory usage
    metrics.memory_peak = metrics.memory_used;
    metrics.memory_allocated = metrics.memory_used;
    metrics.memory_utilization = 0.0; // Would calculate actual utilization
    
    profiling_history.push_back(metrics);
}

void PerformanceProfiler::profileCPUUsage() {
    if (!profiling_enabled) return;
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    
    // Get CPU usage (simplified)
    metrics.cpu_time = 0.0; // Would get actual CPU time
    metrics.cpu_utilization = 0.0; // Would get actual CPU utilization
    
    profiling_history.push_back(metrics);
}

void PerformanceProfiler::profileNeuronOperations() {
    startComponentProfile("neuron_operations", "neuron");
    
    // Simulate neuron operation profiling
    // In real implementation, would wrap actual neuron operations
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    metrics.cpu_time = 0.01; // Simulated time
    metrics.cpu_utilization = 15.0; // Simulated utilization
    metrics.memory_used = 1024; // Simulated memory
    metrics.success = true;
    
    endComponentProfile("neuron_operations", metrics);
}

void PerformanceProfiler::profileSynapseOperations() {
    startComponentProfile("synapse_operations", "synapse");
    
    // Simulate synapse operation profiling
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    metrics.cpu_time = 0.02; // Simulated time
    metrics.cpu_utilization = 20.0; // Simulated utilization
    metrics.memory_used = 2048; // Simulated memory
    metrics.success = true;
    
    endComponentProfile("synapse_operations", metrics);
}

void PerformanceProfiler::profilePlasticityOperations() {
    startComponentProfile("plasticity_operations", "plasticity");
    
    // Simulate plasticity operation profiling
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    metrics.cpu_time = 0.03; // Simulated time
    metrics.cpu_utilization = 25.0; // Simulated utilization
    metrics.memory_used = 4096; // Simulated memory
    metrics.success = true;
    
    endComponentProfile("plasticity_operations", metrics);
}

void PerformanceProfiler::profileMemorySystems() {
    startComponentProfile("memory_systems", "memory");
    
    // Simulate memory system operation profiling
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    metrics.cpu_time = 0.005; // Simulated time
    metrics.cpu_utilization = 10.0; // Simulated utilization
    metrics.memory_used = 8192; // Simulated memory
    metrics.success = true;
    
    endComponentProfile("memory_systems", metrics);
}

void PerformanceProfiler::runNeuralNetworkSizeBenchmark(const std::vector<size_t>& neuron_counts) {
    startProfiling();
    
    BenchmarkResult benchmark("neural_network_size", "scalability");
    
    for (size_t neuron_count : neuron_counts) {
        // Run benchmark for this neuron count
        // In real implementation, would create actual brain with this many neurons
        
        BenchmarkResult result("neural_network_size_" + std::to_string(neuron_count), "neuron_count");
        result.benchmark_name = benchmark.benchmark_name;
        result.configuration = "neurons=" + std::to_string(neuron_count);
        
        // Simulate benchmark run
        result.total_time = static_cast<double>(neuron_count) * 0.001; // Simulated time
        result.total_operations = neuron_count * 10; // Placeholder operations
        result.throughput = result.total_operations / result.total_time;
        result.memory_peak = neuron_count * 1000; // Placeholder memory
        result.cpu_time = result.total_time * 0.8; // Placeholder CPU time
        
        // Add to measurements
        result.measurements.push_back(result.total_time);
        
        benchmark_results.push_back(result);
    }
    
    stopProfiling();
}

void PerformanceProfiler::runSimulationSpeedBenchmark(const std::vector<double>& durations) {
    startProfiling();
    
    BenchmarkResult benchmark("simulation_speed", "performance");
    
    for (double duration : durations) {
        BenchmarkResult result("simulation_speed_" + std::to_string(static_cast<size_t>(duration)), "duration");
        result.benchmark_name = benchmark.benchmark_name;
        result.configuration = "duration=" + std::to_string(duration) + "s";
        
        // Simulate benchmark run
        result.total_time = duration;
        result.total_operations = static_cast<size_t>(duration * 1000); // Placeholder operations
        result.throughput = result.total_operations / result.total_time;
        result.memory_peak = static_cast<size_t>(duration * 100); // Placeholder memory
        result.cpu_time = duration * 0.9; // Placeholder CPU time
        
        // Add to measurements
        result.measurements.push_back(result.total_time);
        
        benchmark_results.push_back(result);
    }
    
    stopProfiling();
}

void PerformanceProfiler::runPlasticityRuleBenchmark(const std::vector<std::string>& rule_names) {
    startProfiling();
    
    BenchmarkResult benchmark("plasticity_rules", "optimization");
    
    for (const std::string& rule_name : rule_names) {
        BenchmarkResult result("plasticity_rule_" + rule_name, "rule_type");
        result.benchmark_name = benchmark.benchmark_name;
        result.configuration = "rule=" + rule_name;
        
        // Simulate benchmark run
        result.total_time = static_cast<double>(rule_name.length()) * 0.01; // Simulated time
        result.total_operations = rule_name.length() * 100; // Placeholder operations
        result.throughput = result.total_operations / result.total_time;
        result.memory_peak = static_cast<size_t>(rule_name.length() * 50); // Placeholder memory
        result.cpu_time = result.total_time * 0.7; // Placeholder CPU time
        
        // Add to measurements
        result.measurements.push_back(result.total_time);
        
        benchmark_results.push_back(result);
    }
    
    stopProfiling();
}

void PerformanceProfiler::runMemoryEfficiencyBenchmark(const std::vector<size_t>& memory_sizes) {
    startProfiling();
    
    BenchmarkResult benchmark("memory_efficiency", "scalability");
    
    for (size_t memory_size : memory_sizes) {
        BenchmarkResult result("memory_efficiency_" + std::to_string(memory_size), "memory_size");
        result.benchmark_name = benchmark.benchmark_name;
        result.configuration = "memory_size=" + std::to_string(memory_size) + "bytes";
        
        // Simulate benchmark run
        result.total_time = static_cast<double>(memory_size) * 0.0001; // Simulated time
        result.total_operations = memory_size / 100; // Placeholder operations
        result.throughput = result.total_operations / result.total_time;
        result.memory_peak = memory_size;
        result.cpu_time = result.total_time * 0.6; // Placeholder CPU time
        
        // Add to measurements
        result.measurements.push_back(result.total_time);
        
        benchmark_results.push_back(result);
    }
    
    stopProfiling();
}

BenchmarkResult PerformanceProfiler::calculateStatistics(const std::vector<BenchmarkResult>& results) {
    BenchmarkResult aggregated("aggregated_results", "summary");
    
    if (results.empty()) return aggregated;
    
    std::vector<double> all_measurements;
    for (const auto& result : results) {
        all_measurements.insert(all_measurements.end(), result.measurements.begin(), result.measurements.end());
    }
    
    if (all_measurements.empty()) return aggregated;
    
    // Calculate statistics
    aggregated.mean = std::accumulate(all_measurements.begin(), all_measurements.end(), 0.0) / all_measurements.size();
    aggregated.median = all_measurements[all_measurements.size() / 2];
    aggregated.min_value = *std::min_element(all_measurements.begin(), all_measurements.end());
    aggregated.max_value = *std::max_element(all_measurements.begin(), all_measurements.end());
    
    double sum_squared_diff = 0.0;
    for (double val : all_measurements) {
        double diff = val - aggregated.mean;
        sum_squared_diff += diff * diff;
    }
    aggregated.std_deviation = std::sqrt(sum_squared_diff / all_measurements.size());
    
    return aggregated;
}

void PerformanceProfiler::compareBenchmarks(const std::string& baseline_name, const std::string& test_name) {
    BenchmarkResult baseline;
    BenchmarkResult test;
    
    // Find baseline and test results
    for (const auto& result : benchmark_results) {
        if (result.benchmark_name == baseline_name) {
            baseline = result;
        }
        if (result.benchmark_name == test_name) {
            test = result;
        }
    }
    
    if (baseline.total_time > 0 && test.total_time > 0) {
        test.improvement_factor = baseline.total_time / test.total_time;
    }
    
    test.baseline_name = baseline_name;
    benchmark_results.push_back(test); // Add comparison result
}

ProfileMetrics PerformanceProfiler::getCurrentMetrics() const {
    if (monitor) {
        return monitor->getCurrentMetrics();
    }
    
    ProfileMetrics metrics;
    metrics.timestamp = clock->getTime();
    return metrics;
}

void PerformanceProfiler::generatePerformanceReport(const std::string& output_file) {
    if (monitor) {
        monitor->generatePerformanceReport(output_file);
    }
}

void PerformanceProfiler::generateBenchmarkReport(const std::string& output_file) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        return;
    }
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n<title>NLM Benchmark Report</title>\n<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
         << "h1 { color: #333; }\n"
         << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n"
         << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
         << "th { background-color: #f2f2f2; }\n"
         << "tr:nth-child(even) { background-color: #f9f9f9; }\n"
         << "</style>\n</head>\n<body>\n"
         << "<h1>NLM Benchmark Results Report</h1>\n"
         << "<p>Generated: " << std::chrono::system_clock::now() << "</p>\n"
         << "<h2>Benchmark Summary</h2>\n"
         << "<table>\n"
         << "<tr><th>Benchmark Name</th><th>Type</th><th>Total Time</th><th>Throughput</th><th>Memory Peak</th></tr>\n";
    
    for (const auto& result : benchmark_results) {
        file << "<tr><td>" << result.benchmark_name << "</td><td>" << result.test_type << "</td><td>"
             << std::fixed << std::setprecision(3) << result.total_time << "</td><td>"
             << std::fixed << std::setprecision(0) << result.throughput << "</td><td>"
             << result.memory_peak << "</td></tr>\n";
    }
    
    file << "</table>\n</body>\n</html>";
    file.close();
}

void PerformanceProfiler::generateDetailedReport(const std::string& output_file) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        return;
    }
    
    file << "<!DOCTYPE html>\n<html>\n<head>\n<title>NLM Detailed Performance Report</title>\n<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
         << "h1 { color: #333; }\n"
         << "h2 { color: #555; }\n"
         << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n"
         << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
         << "th { background-color: #f2f2f2; }\n"
         << "tr:nth-child(even) { background-color: #f9f9f9; }\n"
         << "pre { background-color: #f5f5f5; padding: 10px; }\n"
         << "</style>\n</head>\n<body>\n"
         << "<h1>NLM Detailed Performance Analysis Report</h1>\n"
         << "<p>Generated: " << std::chrono::system_clock::now() << "</p>\n";
    
    // Add component profiling data
    file << "<h2>Component Profiling Data</h2>\n<table>\n"
         << "<tr><th>Component Name</th><th>Type</th><th>Operations</th><th>Successful</th><th>Failed</th><th>Total Time</th><th>Avg Time</th></tr>\n";
    
    for (const auto& pair : active_profiles) {
        const ComponentProfile& profile = pair.second;
        file << "<tr><td>" << profile.name << "</td><td>" << profile.type << "</td><td>"
             << profile.operations_count << "</td><td>" << profile.successful_operations << "</td><td>"
             << profile.failed_operations << "</td><td>" << std::fixed << std::setprecision(3) << profile.total_time << "</td><td>"
             << (profile.operations_count > 0 ? profile.total_time / profile.operations_count : 0.0) << "</td></tr>\n";
    }
    
    file << "</table>\n"
         << "<h2>Performance Metrics History</h2>\n"
         << "<table>\n"
         << "<tr><th>Timestamp</th><th>CPU Usage</th><th>Memory Usage</th><th>CPU Time</th><th>Memory Used</th></tr>\n";
    
    for (const auto& metrics : profiling_history) {
        file << "<tr><td>" << metrics.timestamp << "</td><td>" << metrics.cpu_utilization << "%</td><td>"
             << metrics.memory_utilization << "%</td><td>" << metrics.cpu_time << "</td><td>"
             << metrics.memory_used << "</td></tr>\n";
    }
    
    file << "</table>\n"
         << "<h2>Benchmark Results</h2>\n"
         << "<table>\n"
         << "<tr><th>Benchmark Name</th><th>Configuration</th><th>Total Time</th><th>Throughput</th><th>Mean</th><th>Std Dev</th></tr>\n";
    
    for (const auto& result : benchmark_results) {
        file << "<tr><td>" << result.benchmark_name << "</td><td>" << result.configuration << "</td><td>"
             << std::fixed << std::setprecision(3) << result.total_time << "</td><td>"
             << std::fixed << std::setprecision(0) << result.throughput << "</td><td>"
             << std::fixed << std::setprecision(3) << result.mean << "</td><td>"
             << std::fixed << std::setprecision(3) << result.std_deviation << "</td></tr>\n";
    }
    
    file << "</table>\n</body>\n</html>";
    file.close();
}

void PerformanceProfiler::saveDataToFiles() {
    if (!save_detailed_data) return;
    
    // Save profiling data
    std::filesystem::path prof_path = std::filesystem::path(output_directory) / "profiling_data.csv";
    std::ofstream prof_file(prof_path, std::ios::app);
    if (prof_file.is_open()) {
        for (const auto& metrics : profiling_history) {
            prof_file << std::fixed << std::setprecision(6)
                      << metrics.timestamp << ",system,general," << metrics.cpu_time << ","
                      << metrics.memory_used << "," << metrics.cpu_utilization << ",1\n";
        }
        prof_file.close();
    }
    
    // Save benchmarks data
    std::filesystem::path bench_path = std::filesystem::path(output_directory) / "benchmarks_data.json";
    std::ofstream bench_file(bench_path);
    if (bench_file.is_open()) {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& result : benchmark_results) {
            j.push_back({
                {"benchmark_name", result.benchmark_name},
                {"test_type", result.test_type},
                {"configuration", result.configuration},
                {"total_time", result.total_time},
                {"throughput", result.throughput},
                {"total_operations", result.total_operations},
                {"memory_peak", result.memory_peak},
                {"cpu_time", result.cpu_time},
                {"mean", result.mean},
                {"median", result.median},
                {"std_deviation", result.std_deviation},
                {"min_value", result.min_value},
                {"max_value", result.max_value}
            });
        }
        bench_file << j.dump(2);
        bench_file.close();
    }
}

void PerformanceProfiler::loadDataFromFiles() {
    // Load profiling data
    std::filesystem::path prof_path = std::filesystem::path(output_directory) / "profiling_data.csv";
    if (std::filesystem::exists(prof_path)) {
        std::ifstream prof_file(pro_path);
        if (prof_file.is_open()) {
            std::string line;
            while (std::getline(prof_file, line)) {
                // Parse CSV line (simplified)
                // In real implementation, would parse properly
            }
            prof_file.close();
        }
    }
    
    // Load benchmarks data
    std::filesystem::path bench_path = std::filesystem::path(output_directory) / "benchmarks_data.json";
    if (std::filesystem::exists(bench_path)) {
        std::ifstream bench_file(bench_path);
        if (bench_file.is_open()) {
            nlohmann::json j = nlohmann::json::parse(bench_file);
            for (const auto& item : j) {
                BenchmarkResult result(item.value("benchmark_name", ""), item.value("test_type", ""));
                result.configuration = item.value("configuration", "");
                result.total_time = item.value("total_time", 0.0);
                result.throughput = item.value("throughput", 0.0);
                result.total_operations = item.value("total_operations", 0);
                result.memory_peak = item.value("memory_peak", 0);
                result.cpu_time = item.value("cpu_time", 0.0);
                result.mean = item.value("mean", 0.0);
                result.median = item.value("median", 0.0);
                result.std_deviation = item.value("std_deviation", 0.0);
                result.min_value = item.value("min_value", 0.0);
                result.max_value = item.value("max_value", 0.0);
                
                benchmark_results.push_back(result);
            }
            bench_file.close();
        }
    }
}

} // namespace nlm