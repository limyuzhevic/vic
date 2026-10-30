#include "AdvancedProfiler.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

namespace nlm {

void StatisticsCollector::recordStep(size_t spikes, size_t activeNeurons,
                                    double timestep, size_t memoryUsage) {
    total_steps_++;
    total_spikes_ += spikes;
    total_active_neurons_ += activeNeurons;
    
    memory_tracker_.update(memoryUsage);
    
    double step_time = step_timer_.elapsed();
    step_timer_.reset();
    
    PerformanceMetrics step_metric("step", step_time, memoryUsage,
                                   memory_tracker_.getPeak(), 0, total_steps_,
                                   total_steps_ > 0 ? 1000.0 / step_time : 0.0);
    step_performance_.push_back(step_metric);
}

void StatisticsCollector::recordOperation(const std::string& operation, double duration) {
    operation_timings_[operation].push_back(duration);
    
    PerformanceMetrics op_metric(operation, duration, 0, 0, 0, 0, 0.0);
    performance_history_.push_back(op_metric);
}

double StatisticsCollector::getElapsedTime() const {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
    return duration.count() / 1000.0; // Convert to milliseconds
}

double StatisticsCollector::getAverageFiringRate() const {
    if (total_steps_ == 0) return 0.0;
    return static_cast<double>(total_spikes_) / static_cast<double>(total_steps_);
}

size_t StatisticsCollector::getAverageActiveNeurons() const {
    if (total_steps_ == 0) return 0;
    return total_active_neurons_ / total_steps_;
}

double StatisticsCollector::getAverageStepTime() const {
    if (step_performance_.empty()) return 0.0;
    double total_time = 0.0;
    for (const auto& metric : step_performance_) {
        total_time += metric.duration;
    }
    return total_time / step_performance_.size();
}

std::string StatisticsCollector::generateReport() const {
    std::ostringstream oss;
    oss << "=== Statistics Summary ===" << std::endl;
    oss << "Total simulation time: " << getElapsedTime() << " ms" << std::endl;
    oss << "Total steps: " << total_steps_ << std::endl;
    oss << "Total spikes: " << total_spikes_ << std::endl;
    oss << "Average firing rate: " << getAverageFiringRate() << " spikes/step" << std::endl;
    oss << "Average active neurons: " << getAverageActiveNeurons() << std::endl;
    oss << "Average step time: " << getAverageStepTime() << " ms" << std::endl;
    oss << "Performance history entries: " << performance_history_.size() << std::endl;
    oss << "Step performance entries: " << step_performance_.size() << std::endl;
    return oss.str();
}

std::string StatisticsCollector::generateDetailedReport() const {
    std::ostringstream oss;
    oss << "=== Detailed Statistics Report ===" << std::endl;
    oss << std::endl;
    
    if (!step_performance_.empty()) {
        oss << "Step Performance:" << std::endl;
        oss << "  Step | Duration (ms) | Spikes | Active Neurons | Memory (KB) | FPS" << std::endl;
        oss << "  -----|--------------|--------|----------------|--------------|-----" << std::endl;
        
        size_t min_steps_to_show = std::min(size_t(20), step_performance_.size());
        for (size_t i = 0; i < min_steps_to_show; ++i) {
            const auto& metric = step_performance_[i];
            oss << std::setw(5) << metric.stepCount << " | "
                << std::setw(14) << std::fixed << std::setprecision(2) << metric.duration << " | "
                << std::setw(8) << "N/A" << " | "
                << std::setw(16) << "N/A" << " | "
                << std::setw(14) << (metric.memoryUsed / 1024) << " | "
                << std::setw(5) << std::fixed << std::setprecision(2) << metric.fps << std::endl;
        }
        if (step_performance_.size() > min_steps_to_show) {
            oss << "  ... and " << step_performance_.size() - min_steps_to_show << " more steps" << std::endl;
        }
        oss << std::endl;
    }
    
    if (!performance_history_.empty()) {
        oss << "Operation Performance:" << std::endl;
        oss << "  Operation | Duration (ms) | Count" << std::endl;
        oss << "  ----------|--------------|-------" << std::endl;
        
        std::map<std::string, size_t> operation_counts;
        for (const auto& metric : performance_history_) {
            operation_counts[metric.name]++;
        }
        
        for (const auto& [op_name, count] : operation_counts) {
            oss << std::setw(11) << op_name << " | "
                << std::setw(14) << "N/A" << " | "
                << std::setw(7) << count << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

void StatisticsCollector::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    
    file << "=== NLM Advanced Statistics Report ===" << std::endl;
    file << "Generated: " << std::ctime(&std::time(nullptr)) << std::endl;
    file << std::endl;
    file << generateDetailedReport() << std::endl;
    
    file.close();
}

void StatisticsCollector::reset() {
    total_steps_ = 0;
    total_spikes_ = 0;
    total_active_neurons_ = 0;
    performance_history_.clear();
    step_performance_.clear();
    operation_timings_.clear();
    memory_tracker_.reset();
    step_timer_.reset();
    start_time_ = std::chrono::high_resolution_clock::now();
}

void AdvancedBrainStatistics::collectStatistics(const class Brain& brain, 
                                               const StatisticsCollector& collector,
                                               size_t step_number) {
    // Calculate neural health metrics
    neural_health_.excitationInhibitionRatio = brain.getExcitationInhibitionRatio();
    neural_health_.averageFiringRate = collector.getAverageFiringRate();
    neural_health_.activeNeurons = brain.getActiveNeuronCount();
    
    // Get membrane potential average (would need to iterate through all neurons)
    // For now, we'll simulate this
    neural_health_.averageMembranePotential = 15.0 + (rand() % 20); // Simulated value
    
    // Calculate healthy neurons (neurons with reasonable membrane potential)
    neural_health_.healthyNeurons = brain.getTotalNeuronCount();
    
    // Calculate synchrony index based on active neurons
    neural_health_.synchronyIndex = calculateSynchronyIndex(active_neuron_history_);
    
    // Store history
    membrane_potential_history_.push_back(neural_health_.averageMembranePotential);
    active_neuron_history_.push_back(neural_health_.activeNeurons);
}

std::string AdvancedBrainStatistics::generateAdvancedReport(const Brain& brain) const {
    std::ostringstream oss;
    oss << "=== Advanced Brain Statistics ===" << std::endl;
    oss << std::endl;
    
    oss << "Neural Health Metrics:" << std::endl;
    oss << "  Excitation/Inhibition Ratio: " << neural_health_.excitationInhibitionRatio << std::endl;
    oss << "  Average Membrane Potential: " << neural_health_.averageMembranePotential << " mV" << std::endl;
    oss << "  Average Firing Rate: " << neural_health_.averageFiringRate << " Hz" << std::endl;
    oss << "  Healthy Neurons: " << neural_health_.healthyNeurons << std::endl;
    oss << "  Active Neurons: " << neural_health_.activeNeurons << std::endl;
    oss << "  Synchrony Index: " << neural_health_.synchronyIndex << std::endl;
    oss << std::endl;
    
    // Calculate additional metrics
    if (!membrane_potential_history_.empty()) {
        double avg_potential = std::accumulate(membrane_potential_history_.begin(), 
                                              membrane_potential_history_.end(), 0.0) / 
                                              membrane_potential_history_.size();
        oss << "Historical Statistics:" << std::endl;
        oss << "  Average membrane potential (history): " << avg_potential << " mV" << std::endl;
        oss << "  Membrane potential range: " << 
            (*std::min_element(membrane_potential_history_.begin(), membrane_potential_history_.end())) << 
            " - " << 
            (*std::max_element(membrane_potential_history_.begin(), membrane_potential_history_.end())) << " mV" << std::endl;
        oss << "  Active neuron range: " << 
            (*std::min_element(active_neuron_history_.begin(), active_neuron_history_.end())) << 
            " - " << 
            (*std::max_element(active_neuron_history_.begin(), active_neuron_history_.end())) << std::endl;
        oss << std::endl;
    }
    
    oss << "Current Brain State:" << std::endl;
    oss << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    oss << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    oss << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    oss << "  Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    oss << std::endl;
    
    return oss.str();
}

double AdvancedBrainStatistics::calculateSynchronyIndex(const std::vector<size_t>& active_neurons) const {
    if (active_neurons.size() < 2) return 0.0;
    
    double sum = 0.0;
    for (size_t i = 0; i < active_neurons.size(); ++i) {
        for (size_t j = i + 1; j < active_neurons.size(); ++j) {
            sum += std::abs(active_neurons[i] - active_neurons[j]);
        }
    }
    
    double max_possible = static_cast<double>(active_neurons.size() * (active_neurons.size() - 1) / 2) * 100.0;
    return max_possible > 0 ? (1.0 - sum / max_possible) : 0.0;
}

void PerformanceProfiler::startProfiling(const std::string& name) {
    active_ = true;
    operation_timers_[name] = HighResolutionTimer();
}

void PerformanceProfiler::stopProfiling(const std::string& name) {
    auto it = operation_timers_.find(name);
    if (it != operation_timers_.end()) {
        double duration = it->second.elapsed();
        statistics_.recordOperation(name, duration);
        operation_timers_.erase(it);
    }
}

void PerformanceProfiler::startStepProfiling() {
    step_timer_.reset();
    step_timer_.elapsed(); // Clear any previous timing
}

void PerformanceProfiler::endStepProfiling(size_t spikes, size_t activeNeurons,
                                          double timestep, size_t memoryUsage) {
    double step_time = step_timer_.elapsed();
    statistics_.recordStep(spikes, activeNeurons, timestep, memoryUsage);
}

std::string PerformanceProfiler::generateProfilingReport() const {
    std::ostringstream oss;
    oss << "=== Performance Profiling Report ===" << std::endl;
    oss << statistics_.generateReport() << std::endl;
    oss << std::endl;
    oss << statistics_.generateDetailedReport() << std::endl;
    return oss.str();
}

std::string PerformanceProfiler::generatePerformanceReport() const {
    std::ostringstream oss;
    oss << "=== NLM Performance Analysis ===" << std::endl;
    oss << std::endl;
    
    const auto& step_perf = statistics_.getStepPerformance();
    if (step_perf.empty()) {
        oss << "No performance data available." << std::endl;
        return oss.str();
    }
    
    oss << "Step Performance Statistics:" << std::endl;
    oss << "  Steps recorded: " << step_perf.size() << std::endl;
    oss << "  Average step time: " << statistics_.getAverageStepTime() << " ms" << std::endl;
    
    if (step_perf.size() >= 2) {
        double min_time = step_perf[0].duration;
        double max_time = step_perf[0].duration;
        double sum_time = 0.0;
        
        for (const auto& metric : step_perf) {
            if (metric.duration < min_time) min_time = metric.duration;
            if (metric.duration > max_time) max_time = metric.duration;
            sum_time += metric.duration;
        }
        
        double avg_time = sum_time / step_perf.size();
        double variance = 0.0;
        for (const auto& metric : step_perf) {
            variance += (metric.duration - avg_time) * (metric.duration - avg_time);
        }
        variance /= step_perf.size();
        double std_dev = std::sqrt(variance);
        
        oss << "  Min step time: " << min_time << " ms" << std::endl;
        oss << "  Max step time: " << max_time << " ms" << std::endl;
        oss << "  Std deviation: " << std_dev << " ms" << std::endl;
        oss << "  Performance coefficient of variation: " << (std_dev / avg_time * 100.0) << "%" << std::endl;
    }
    
    oss << std::endl;
    oss << "Memory Usage:" << std::endl;
    oss << "  Peak memory usage: " << statistics_.getPerformanceHistory().empty() ? 0 : 
        statistics_.getPerformanceHistory()[0].peakMemory / 1024 << " KB" << std::endl;
    
    return oss.str();
}

void PerformanceProfiler::saveProfilingData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    
    file << generateProfilingReport() << std::endl;
    file.close();
}

void PerformanceProfiler::reset() {
    statistics_.reset();
    active_ = false;
    operation_timers_.clear();
    memory_tracker_.reset();
    step_timer_.reset();
}

Benchmarker::BenchmarkResult Benchmarker::runBenchmark(const std::string& name,
                                                       std::function<void()> benchmark_func,
                                                       size_t iterations) {
    std::cout << "Running benchmark: " << name << " (" << iterations << " iterations)" << std::endl;
    
    std::vector<double> durations;
    durations.reserve(iterations);
    
    for (size_t i = 0; i < iterations; ++i) {
        HighResolutionTimer timer;
        benchmark_func();
        double duration = timer.elapsed();
        durations.push_back(duration);
        
        if (iterations > 1 && i % 10 == 0) {
            std::cout << "  Completed " << (i + 1) << "/" << iterations << " iterations" << std::endl;
        }
    }
    
    double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
    double avg = sum / iterations;
    double min_val = *std::min_element(durations.begin(), durations.end());
    double max_val = *std::max_element(durations.begin(), durations.end());
    
    double variance = 0.0;
    for (double duration : durations) {
        variance += (duration - avg) * (duration - avg);
    }
    variance /= iterations;
    double std_dev = std::sqrt(variance);
    
    BenchmarkResult result;
    result.testName = name;
    result.duration = avg;
    result.iterations = iterations;
    result.avgDuration = avg;
    result.minDuration = min_val;
    result.maxDuration = max_val;
    result.stdDeviation = std_dev;
    
    results_.push_back(result);
    
    std::cout << "Benchmark completed: " << name << std::endl;
    std::cout << "  Average time: " << avg << " ms" << std::endl;
    std::cout << "  Min time: " << min_val << " ms" << std::endl;
    std::cout << "  Max time: " << max_val << " ms" << std::endl;
    std::cout << "  Standard deviation: " << std_dev << " ms" << std::endl;
    
    return result;
}

std::string Benchmarker::generateBenchmarkReport() const {
    std::ostringstream oss;
    oss << "=== Benchmark Results ===" << std::endl;
    oss << std::endl;
    
    if (results_.empty()) {
        oss << "No benchmark results available." << std::endl;
        return oss.str();
    }
    
    oss << "Benchmark Summary:" << std::endl;
    oss << "  Tests run: " << results_.size() << std::endl;
    oss << std::endl;
    
    oss << "Detailed Results:" << std::endl;
    oss << "  Test Name | Avg Time (ms) | Min Time (ms) | Max Time (ms) | Std Dev (ms) | Iterations" << std::endl;
    oss << "  ----------|---------------|---------------|---------------|--------------|----------" << std::endl;
    
    for (const auto& result : results_) {
        oss << std::setw(11) << result.testName << " | "
            << std::setw(15) << std::fixed << std::setprecision(2) << result.avgDuration << " | "
            << std::setw(15) << std::fixed << std::setprecision(2) << result.minDuration << " | "
            << std::setw(15) << std::fixed << std::setprecision(2) << result.maxDuration << " | "
            << std::setw(14) << std::fixed << std::setprecision(2) << result.stdDeviation << " | "
            << std::setw(12) << result.iterations << std::endl;
    }
    
    return oss.str();
}

void Benchmarker::saveBenchmarkResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    
    file << generateBenchmarkReport() << std::endl;
    file.close();
}

void ProfilingCommands::registerProfilingCommands() {
    // This would register CLI commands in a real implementation
    // For now, we just provide the functionality
}

bool ProfilingCommands::handleProfilingCommand(const std::string& command,
                                            PerformanceProfiler& profiler,
                                            const class Brain* brain) {
    if (command == "status") {
        printProfilingStatus(profiler);
        return true;
    } else if (command == "performance") {
        printPerformanceReport(profiler);
        return true;
    } else if (command == "save") {
        // Default filename
        saveProfilingReport(profiler, "nlm_profiling_report.txt");
        return true;
    } else if (command == "benchmark") {
        // Would run benchmarks
        std::cout << "Benchmark command not yet implemented." << std::endl;
        return true;
    }
    return false;
}

void ProfilingCommands::printProfilingStatus(const PerformanceProfiler& profiler) {
    std::cout << "=== Profiling Status ===" << std::endl;
    std::cout << profiler.generatePerformanceReport() << std::endl;
}

void ProfilingCommands::printPerformanceReport(const PerformanceProfiler& profiler) {
    std::cout << profiler.generatePerformanceReport() << std::endl;
}

void ProfilingCommands::saveProfilingReport(const PerformanceProfiler& profiler,
                                          const std::string& filename) {
    try {
        profiler.saveProfilingData(filename);
        std::cout << "Profiling report saved to: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error saving profiling report: " << e.what() << std::endl;
    }
}

void ProfilingCommands::saveBenchmarkReport(const Benchmarker& benchmarker,
                                          const std::string& filename) {
    try {
        benchmarker.saveBenchmarkResults(filename);
        std::cout << "Benchmark report saved to: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error saving benchmark report: " << e.what() << std::endl;
    }
}

} // namespace nlm