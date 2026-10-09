#pragma once

#include "PerformanceMonitor.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Brain.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() 
    : monitoring_(false)
    , sampling_interval_(1)
    , memory_limit_mb_(1024.0)
    , cpu_threshold_percent_(90.0)
    , real_time_reporting_(false)
    , total_steps_monitored_(0) {
    simulation_start_time_ = std::chrono::steady_clock::now();
}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

void PerformanceMonitor::start() {
    if (monitoring_.exchange(true)) {
        return; // Already monitoring
    }
    
    simulation_start_time_ = std::chrono::steady_clock::now();
    Logger::getInstance().logInfo("Performance monitoring started");
}

void PerformanceMonitor::stop() {
    if (!monitoring_.exchange(false)) {
        return; // Already stopped
    }
    
    Logger::getInstance().logInfo("Performance monitoring stopped");
    exportMetrics("performance_data.json", 0, total_steps_monitored_ - 1);
}

void PerformanceMonitor::pause() {
    monitoring_ = false;
}

void PerformanceMonitor::resume() {
    monitoring_ = true;
    simulation_start_time_ = std::chrono::steady_clock::now();
}

void PerformanceMonitor::recordStepMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    metrics_history_.push_back(metrics);
    total_steps_monitored_ = metrics_history_.size();
    
    if (total_steps_monitored_ % sampling_interval_ != 0 && sampling_interval_ > 1) {
        return; // Skip based on sampling interval
    }
    
    // Update system metrics
    updateSystemMetrics(metrics.system_resources);
    
    // Update neural computation metrics
    updateNeuralMetrics(metrics.neural_computation, total_steps_monitored_ > 1 ? metrics_history_[total_steps_monitored_ - 2] : PerformanceMetrics{});
    
    // Update memory metrics
    updateMemoryMetrics(metrics.memory_usage, total_steps_monitored_ > 1 ? metrics_history_[total_steps_monitored_ - 2] : PerformanceMetrics{});
    
    // Update spike metrics
    updateSpikeMetrics(metrics.spike_events, total_steps_monitored_ > 1 ? metrics_history_[total_steps_monitored_ - 2] : PerformanceMetrics{});
    
    // Real-time reporting
    if (real_time_reporting_) {
        std::ostringstream oss;
        oss << "Step " << metrics.simulation_step 
            << ", Time: " << metrics.wall_clock_time_ms << "ms, "
            << "Firing: " << metrics.neural_computation.firing_neuron_count << ", "
            << "Memory: " << metrics.memory_usage.total_memory_mb << "MB, "
            << "CPU: " << metrics.system_resources.cpu_usage_percent << "%";
        Logger::getInstance().logInfo(oss.str());
    }
}

void PerformanceMonitor::recordSystemResource(const SystemResourceMetrics& sys_metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (!metrics_history_.empty()) {
        metrics_history_.back().system_resources = sys_metrics;
    }
}

void PerformanceMonitor::recordNeuralComputation(const NeuralComputationMetrics& neural_metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (!metrics_history_.empty()) {
        metrics_history_.back().neural_computation = neural_metrics;
    }
}

void PerformanceMonitor::recordMemoryUsage(const MemoryUsageMetrics& memory_metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (!metrics_history_.empty()) {
        metrics_history_.back().memory_usage = memory_metrics;
    }
}

void PerformanceMonitor::recordSpikeEvents(const SpikeEventMetrics& spike_metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (!metrics_history_.empty()) {
        metrics_history_.back().spike_events = spike_metrics;
    }
}

void PerformanceMonitor::recordDevelopment(const DevelopmentMetrics& dev_metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (!metrics_history_.empty()) {
        metrics_history_.back().development = dev_metrics;
    }
}

void PerformanceMonitor::recordNeuromodulation(const NeuromodulationMetrics& neuromod_metrics) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (!metrics_history_.empty()) {
        metrics_history_.back().neuromodulation = neuromod_metrics;
    }
}

PerformanceMetrics PerformanceMonitor::getLatestMetrics() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty()) {
        return PerformanceMetrics{};
    }
    
    return metrics_history_.back();
}

PerformanceMetrics PerformanceMonitor::getAverageMetrics(uint64_t steps) const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty() || steps == 0) {
        return PerformanceMetrics{};
    }
    
    uint64_t start_idx = std::max<uint64_t>(0, metrics_history_.size() - steps);
    PerformanceMetrics avg_metrics;
    uint64_t count = metrics_history_.size() - start_idx;
    
    for (uint64_t i = start_idx; i < metrics_history_.size(); ++i) {
        const auto& metrics = metrics_history_[i];
        
        // Average neural computation metrics
        avg_metrics.neural_computation.step_time_ms += metrics.neural_computation.step_time_ms;
        avg_metrics.neural_computation.neuron_update_time_ms += metrics.neural_computation.neuron_update_time_ms;
        avg_metrics.neural_computation.synaptic_update_time_ms += metrics.neural_computation.synaptic_update_time_ms;
        avg_metrics.neural_computation.spike_processing_time_ms += metrics.neural_computation.spike_processing_time_ms;
        avg_metrics.neural_computation.plasticity_time_ms += metrics.neural_computation.plasticity_time_ms;
        avg_metrics.neural_computation.memory_update_time_ms += metrics.neural_computation.memory_update_time_ms;
        avg_metrics.neural_computation.prediction_time_ms += metrics.neural_computation.prediction_time_ms;
        avg_metrics.neural_computation.cognition_time_ms += metrics.neural_computation.cognition_time_ms;
        avg_metrics.neural_computation.neuromodulation_time_ms += metrics.neural_computation.neuromodulation_time_ms;
        
        avg_metrics.neural_computation.neuron_count += metrics.neural_computation.neuron_count;
        avg_metrics.neural_computation.synapse_count += metrics.neural_computation.synapse_count;
        avg_metrics.neural_computation.firing_neuron_count += metrics.neural_computation.firing_neuron_count;
        avg_metrics.neural_computation.total_spikes += metrics.neural_computation.total_spikes;
        avg_metrics.neural_computation.excitation_inhibition_ratio += metrics.neural_computation.excitation_inhibition_ratio;
        avg_metrics.neural_computation.average_firing_rate += metrics.neural_computation.average_firing_rate;
        
        // Average system resource metrics
        avg_metrics.system_resources.cpu_usage_percent += metrics.system_resources.cpu_usage_percent;
        avg_metrics.system_resources.memory_usage_mb += metrics.system_resources.memory_usage_mb;
        avg_metrics.system_resources.memory_percent += metrics.system_resources.memory_percent;
        
        // Average memory usage metrics
        avg_metrics.memory_usage.working_memory_mb += metrics.memory_usage.working_memory_mb;
        avg_metrics.memory_usage.episodic_memory_mb += metrics.memory_usage.episodic_memory_mb;
        avg_metrics.memory_usage.concept_memory_mb += metrics.memory_usage.concept_memory_mb;
        avg_metrics.memory_usage.prediction_memory_mb += metrics.memory_usage.prediction_memory_mb;
        avg_metrics.memory_usage.total_memory_mb += metrics.memory_usage.total_memory_mb;
        avg_metrics.memory_usage.working_memory_capacity += metrics.memory_usage.working_memory_capacity;
        avg_metrics.memory_usage.episodic_memory_capacity += metrics.memory_usage.episodic_memory_capacity;
        avg_metrics.memory_usage.concept_memory_capacity += metrics.memory_usage.concept_memory_capacity;
        
        // Average spike event metrics
        avg_metrics.spike_events.spike_events_this_step += metrics.spike_events.spike_events_this_step;
        avg_metrics.spike_events.delayed_spike_events += metrics.spike_events.delayed_spike_events;
        avg_metrics.spike_events.spike_generation_rate += metrics.spike_events.spike_generation_rate;
        avg_metrics.spike_events.spike_delivery_rate += metrics.spike_events.spike_delivery_rate;
        
        // Average development metrics
        avg_metrics.development.stage_transition_time_ms += metrics.development.stage_transition_time_ms;
        avg_metrics.development.plasticity_update_time_ms += metrics.development.plasticity_update_time_ms;
        avg_metrics.development.structural_plasticity_time_ms += metrics.development.structural_plasticity_time_ms;
        avg_metrics.development.synaptogenesis_events += metrics.development.synaptogenesis_events;
        avg_metrics.development.pruning_events += metrics.development.pruning_events;
        avg_metrics.development.developmental_stage += metrics.development.developmental_stage;
        avg_metrics.development.developmental_progress += metrics.development.developmental_progress;
        
        // Average neuromodulation metrics
        avg_metrics.neuromodulation.dopamine_level += metrics.neuromodulation.dopamine_level;
        avg_metrics.neuromodulation.serotonin_level += metrics.neuromodulation.serotonin_level;
        avg_metrics.neuromodulation.norepinephrine_level += metrics.neuromodulation.norepinephrine_level;
        avg_metrics.neuromodulation.acetylcholine_level += metrics.neuromodulation.acetylcholine_level;
        avg_metrics.neuromodulation.curiosity_level += metrics.neuromodulation.curiosity_level;
        avg_metrics.neuromodulation.novelty_level += metrics.neuromodulation.novelty_level;
        avg_metrics.neuromodulation.prediction_error_level += metrics.neuromodulation.prediction_error_level;
        avg_metrics.neuromodulation.reward_level += metrics.neuromodulation.reward_level;
        avg_metrics.neuromodulation.neuromodulation_processing_time_ms += metrics.neuromodulation.neuromodulation_processing_time_ms;
    }
    
    // Calculate averages
    if (count > 0) {
        avg_metrics.simulation_step = metrics_history_[metrics_history_.size() - 1].simulation_step;
        avg_metrics.simulation_start_time = metrics_history_.front().simulation_start_time;
        
        // Neural computation averages
        avg_metrics.neural_computation.step_time_ms /= count;
        avg_metrics.neural_computation.neuron_update_time_ms /= count;
        avg_metrics.neural_computation.synaptic_update_time_ms /= count;
        avg_metrics.neural_computation.spike_processing_time_ms /= count;
        avg_metrics.neural_computation.plasticity_time_ms /= count;
        avg_metrics.neural_computation.memory_update_time_ms /= count;
        avg_metrics.neural_computation.prediction_time_ms /= count;
        avg_metrics.neural_computation.cognition_time_ms /= count;
        avg_metrics.neural_computation.neuromodulation_time_ms /= count;
        
        avg_metrics.neural_computation.neuron_count /= count;
        avg_metrics.neural_computation.synapse_count /= count;
        avg_metrics.neural_computation.firing_neuron_count /= count;
        avg_metrics.neural_computation.total_spikes /= count;
        avg_metrics.neural_computation.excitation_inhibition_ratio /= count;
        avg_metrics.neural_computation.average_firing_rate /= count;
        
        // System resource averages
        avg_metrics.system_resources.cpu_usage_percent /= count;
        avg_metrics.system_resources.memory_usage_mb /= count;
        avg_metrics.system_resources.memory_percent /= count;
        
        // Memory usage averages
        avg_metrics.memory_usage.working_memory_mb /= count;
        avg_metrics.memory_usage.episodic_memory_mb /= count;
        avg_metrics.memory_usage.concept_memory_mb /= count;
        avg_metrics.memory_usage.prediction_memory_mb /= count;
        avg_metrics.memory_usage.total_memory_mb /= count;
        avg_metrics.memory_usage.working_memory_capacity /= count;
        avg_metrics.memory_usage.episodic_memory_capacity /= count;
        avg_metrics.memory_usage.concept_memory_capacity /= count;
        
        // Spike event averages
        avg_metrics.spike_events.spike_events_this_step /= count;
        avg_metrics.spike_events.delayed_spike_events /= count;
        avg_metrics.spike_events.spike_generation_rate /= count;
        avg_metrics.spike_events.spike_delivery_rate /= count;
        
        // Development averages
        avg_metrics.development.stage_transition_time_ms /= count;
        avg_metrics.development.plasticity_update_time_ms /= count;
        avg_metrics.development.structural_plasticity_time_ms /= count;
        avg_metrics.development.synaptogenesis_events /= count;
        avg_metrics.development.pruning_events /= count;
        avg_metrics.development.developmental_stage /= count;
        avg_metrics.development.developmental_progress /= count;
        
        // Neuromodulation averages
        avg_metrics.neuromodulation.dopamine_level /= count;
        avg_metrics.neuromodulation.serotonin_level /= count;
        avg_metrics.neuromodulation.norepinephrine_level /= count;
        avg_metrics.neuromodulation.acetylcholine_level /= count;
        avg_metrics.neuromodulation.curiosity_level /= count;
        avg_metrics.neuromodulation.novelty_level /= count;
        avg_metrics.neuromodulation.prediction_error_level /= count;
        avg_metrics.neuromodulation.reward_level /= count;
        avg_metrics.neuromodulation.neuromodulation_processing_time_ms /= count;
    }
    
    return avg_metrics;
}

PerformanceMetrics PerformanceMonitor::getMaxMetrics(uint64_t steps) const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty() || steps == 0) {
        return PerformanceMetrics{};
    }
    
    uint64_t start_idx = std::max<uint64_t>(0, metrics_history_.size() - steps);
    PerformanceMetrics max_metrics;
    
    for (uint64_t i = start_idx; i < metrics_history_.size(); ++i) {
        const auto& metrics = metrics_history_[i];
        
        // Max neural computation metrics
        max_metrics.neural_computation.step_time_ms = std::max(max_metrics.neural_computation.step_time_ms, metrics.neural_computation.step_time_ms);
        max_metrics.neural_computation.neuron_update_time_ms = std::max(max_metrics.neural_computation.neuron_update_time_ms, metrics.neural_computation.neuron_update_time_ms);
        max_metrics.neural_computation.synaptic_update_time_ms = std::max(max_metrics.neural_computation.synaptic_update_time_ms, metrics.neural_computation.synaptic_update_time_ms);
        max_metrics.neural_computation.spike_processing_time_ms = std::max(max_metrics.neural_computation.spike_processing_time_ms, metrics.neural_computation.spike_processing_time_ms);
        max_metrics.neural_computation.plasticity_time_ms = std::max(max_metrics.neural_computation.plasticity_time_ms, metrics.neural_computation.plasticity_time_ms);
        max_metrics.neural_computation.memory_update_time_ms = std::max(max_metrics.neural_computation.memory_update_time_ms, metrics.neural_computation.memory_update_time_ms);
        
        max_metrics.neural_computation.neuron_count = std::max(max_metrics.neural_computation.neuron_count, metrics.neural_computation.neuron_count);
        max_metrics.neural_computation.synapse_count = std::max(max_metrics.neural_computation.synapse_count, metrics.neural_computation.synapse_count);
        max_metrics.neural_computation.firing_neuron_count = std::max(max_metrics.neural_computation.firing_neuron_count, metrics.neural_computation.firing_neuron_count);
        max_metrics.neural_computation.total_spikes = std::max(max_metrics.neural_computation.total_spikes, metrics.neural_computation.total_spikes);
        max_metrics.neural_computation.excitation_inhibition_ratio = std::max(max_metrics.neural_computation.excitation_inhibition_ratio, metrics.neural_computation.excitation_inhibition_ratio);
        max_metrics.neural_computation.average_firing_rate = std::max(max_metrics.neural_computation.average_firing_rate, metrics.neural_computation.average_firing_rate);
        
        // Max system resource metrics
        max_metrics.system_resources.cpu_usage_percent = std::max(max_metrics.system_resources.cpu_usage_percent, metrics.system_resources.cpu_usage_percent);
        max_metrics.system_resources.memory_usage_mb = std::max(max_metrics.system_resources.memory_usage_mb, metrics.system_resources.memory_usage_mb);
        max_metrics.system_resources.memory_percent = std::max(max_metrics.system_resources.memory_percent, metrics.system_resources.memory_percent);
        
        // Max memory usage metrics
        max_metrics.memory_usage.working_memory_mb = std::max(max_metrics.memory_usage.working_memory_mb, metrics.memory_usage.working_memory_mb);
        max_metrics.memory_usage.episodic_memory_mb = std::max(max_metrics.memory_usage.episodic_memory_mb, metrics.memory_usage.episodic_memory_mb);
        max_metrics.memory_usage.concept_memory_mb = std::max(max_metrics.memory_usage.concept_memory_mb, metrics.memory_usage.concept_memory_mb);
        max_metrics.memory_usage.prediction_memory_mb = std::max(max_metrics.memory_usage.prediction_memory_mb, metrics.memory_usage.prediction_memory_mb);
        max_metrics.memory_usage.total_memory_mb = std::max(max_metrics.memory_usage.total_memory_mb, metrics.memory_usage.total_memory_mb);
    }
    
    return max_metrics;
}

std::vector<PerformanceMetrics> PerformanceMonitor::getMetricsHistory(uint64_t steps) const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (steps == 0 || steps > metrics_history_.size()) {
        return metrics_history_;
    }
    
    uint64_t start_idx = metrics_history_.size() - steps;
    return std::vector<PerformanceMetrics>(metrics_history_.begin() + start_idx, metrics_history_.end());
}

void PerformanceMonitor::startProfiling(const std::string& profile_name) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    ProfilingData profiling_data;
    profiling_data.start_time = std::chrono::steady_clock::now();
    profiling_sessions_[profile_name] = profiling_data;
    
    Logger::getInstance().logInfo("Profiling started for: " + profile_name);
}

void PerformanceMonitor::stopProfiling(const std::string& profile_name) {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    auto it = profiling_sessions_.find(profile_name);
    if (it != profiling_sessions_.end()) {
        auto& profiling_data = it->second;
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - profiling_data.start_time);
        
        Logger::getInstance().logInfo("Profiling stopped for: " + profile_name + ", duration: " + std::to_string(duration.count()) + "ms");
        
        profiling_sessions_.erase(it);
    }
}

void PerformanceMonitor::profileFunction(const std::string& function_name, std::function<void()> func) {
    auto start_time = std::chrono::steady_clock::now();
    func();
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (profiling_sessions_.find(function_name) == profiling_sessions_.end()) {
        ProfilingData profiling_data;
        profiling_data.start_time = std::chrono::steady_clock::now();
        profiling_sessions_[function_name] = profiling_data;
    }
    
    ProfilingData& profiling_data = profiling_sessions_[function_name];
    profiling_data.function_times[function_name] += duration.count() * 0.001; // Convert to ms
    profiling_data.call_counts[function_name] += 1;
}

std::map<std::string, double> PerformanceMonitor::getProfileResults() const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    std::map<std::string, double> results;
    
    for (const auto& pair : profiling_sessions_) {
        const auto& profiling_data = pair.second;
        
        for (const auto& func_pair : profiling_data.function_times) {
            const std::string& func_name = func_pair.first;
            double total_time = func_pair.second;
            uint64_t call_count = profiling_data.call_counts.at(func_name);
            
            results[func_name] = total_time;
        }
    }
    
    return results;
}

bool PerformanceMonitor::exportMetrics(const std::string& filepath, uint64_t start_step, uint64_t end_step) const {
    if (!metrics_history_.empty()) {
        uint64_t start_idx = std::max<uint64_t>(0, start_step);
        uint64_t end_idx = std::min<uint64_t>(metrics_history_.size(), end_step + 1);
        
        if (start_idx >= end_idx) {
            return false;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "step,time_ms,neuron_count,synapse_count,firing_neuron_count,total_spikes," <<
                 "cpu_usage_percent,memory_usage_mb,working_memory_mb,episodic_memory_mb," <<
                 "step_time_ms,neuron_update_time_ms,plasticity_time_ms,memory_update_time_ms" << std::endl;
        
        for (uint64_t i = start_idx; i < end_idx; ++i) {
            const auto& metrics = metrics_history_[i];
            
            file << metrics.simulation_step << ","
                 << metrics.wall_clock_time_ms << ","
                 << metrics.neural_computation.neuron_count << ","
                 << metrics.neural_computation.synapse_count << ","
                 << metrics.neural_computation.firing_neuron_count << ","
                 << metrics.neural_computation.total_spikes << ","
                 << metrics.system_resources.cpu_usage_percent << ","
                 << metrics.system_resources.memory_usage_mb << ","
                 << metrics.memory_usage.working_memory_mb << ","
                 << metrics.memory_usage.episodic_memory_mb << ","
                 << metrics.neural_computation.step_time_ms << ","
                 << metrics.neural_computation.neuron_update_time_ms << ","
                 << metrics.neural_computation.plasticity_time_ms << ","
                 << metrics.neural_computation.memory_update_time_ms << std::endl;
        }
        
        file.close();
        Logger::getInstance().logInfo("Metrics exported to: " + filepath);
        return true;
    }
    
    return false;
}

bool PerformanceMonitor::exportJSON(const std::string& filepath, uint64_t start_step, uint64_t end_step) const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty()) {
        return false;
    }
    
    uint64_t start_idx = std::max<uint64_t>(0, start_step);
    uint64_t end_idx = std::min<uint64_t>(metrics_history_.size(), end_step + 1);
    
    if (start_idx >= end_idx) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "[";
    
    for (uint64_t i = start_idx; i < end_idx; ++i) {
        const auto& metrics = metrics_history_[i];
        
        file << metricsToJSON(metrics);
        
        if (i < end_idx - 1) {
            file << ",";
        }
    }
    
    file << "]";
    file.close();
    
    Logger::getInstance().logInfo("Metrics exported to JSON: " + filepath);
    return true;
}

bool PerformanceMonitor::exportCSV(const std::string& filepath, uint64_t start_step, uint64_t end_step) const {
    return exportMetrics(filepath, start_step, end_step);
}

std::string PerformanceMonitor::getMetricsSummary(uint64_t steps) const {
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty() || steps == 0) {
        return "No metrics data available";
    }
    
    uint64_t start_idx = std::max<uint64_t>(0, metrics_history_.size() - steps);
    uint64_t count = metrics_history_.size() - start_idx;
    
    if (count == 0) {
        return "No metrics data available";
    }
    
    double avg_step_time = 0.0;
    double avg_cpu_usage = 0.0;
    double avg_memory_usage = 0.0;
    double avg_firing_rate = 0.0;
    
    for (uint64_t i = start_idx; i < metrics_history_.size(); ++i) {
        const auto& metrics = metrics_history_[i];
        avg_step_time += metrics.neural_computation.step_time_ms;
        avg_cpu_usage += metrics.system_resources.cpu_usage_percent;
        avg_memory_usage += metrics.system_resources.memory_usage_mb;
        avg_firing_rate += metrics.neural_computation.average_firing_rate;
    }
    
    avg_step_time /= count;
    avg_cpu_usage /= count;
    avg_memory_usage /= count;
    avg_firing_rate /= count;
    
    std::ostringstream oss;
    oss << "Performance Summary (last " << count << " steps):" << std::endl;
    oss << "  Average step time: " << std::fixed << std::setprecision(3) << avg_step_time << " ms" << std::endl;
    oss << "  Average CPU usage: " << std::fixed << std::setprecision(1) << avg_cpu_usage << "%" << std::endl;
    oss << "  Average memory usage: " << std::fixed << std::setprecision(1) << avg_memory_usage << " MB" << std::endl;
    oss << "  Average firing rate: " << std::fixed << std::setprecision(2) << avg_firing_rate << " Hz" << std::endl;
    
    return oss.str();
}

PerformanceMonitor::SpikeAnalysis PerformanceMonitor::analyzeSpikeEvents(uint64_t steps) const {
    SpikeAnalysis analysis;
    
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty() || steps == 0) {
        return analysis;
    }
    
    uint64_t start_idx = std::max<uint64_t>(0, metrics_history_.size() - steps);
    
    for (uint64_t i = start_idx; i < metrics_history_.size(); ++i) {
        const auto& metrics = metrics_history_[i];
        analysis.total_spikes += metrics.neural_computation.total_spikes;
        analysis.spike_intervals.insert(analysis.spike_intervals.end(),
            metrics.spike_events.spike_times_per_region.begin(),
            metrics.spike_events.spike_times_per_region.end());
    }
    
    if (analysis.total_spikes > 0) {
        analysis.average_spike_frequency = static_cast<double>(analysis.total_spikes) / steps;
        
        if (!analysis.spike_intervals.empty()) {
            double sum_intervals = std::accumulate(analysis.spike_intervals.begin(), analysis.spike_intervals.end(), 0.0);
            analysis.average_spike_frequency = 1.0 / (sum_intervals / analysis.spike_intervals.size());
        }
    }
    
    // Detect bursts
    if (analysis.spike_intervals.size() > 3) {
        double burst_threshold = 2.0 * analysis.average_spike_frequency; // Simple burst detection
        bool has_bursts = false;
        
        for (size_t i = 1; i < analysis.spike_intervals.size(); ++i) {
            if (analysis.spike_intervals[i] < burst_threshold) {
                has_bursts = true;
                break;
            }
        }
        
        analysis.has_bursts = has_bursts;
    }
    
    return analysis;
}

PerformanceMonitor::MemoryAnalysis PerformanceMonitor::analyzeMemoryUsage(uint64_t steps) const {
    MemoryAnalysis analysis;
    
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty() || steps == 0) {
        return analysis;
    }
    
    uint64_t start_idx = std::max<uint64_t>(0, metrics_history_.size() - steps);
    analysis.memory_usage_over_time.reserve(steps);
    
    for (uint64_t i = start_idx; i < metrics_history_.size(); ++i) {
        const auto& metrics = metrics_history_[i];
        analysis.memory_usage_over_time.push_back(metrics.memory_usage.total_memory_mb);
    }
    
    if (!analysis.memory_usage_over_time.empty()) {
        analysis.peak_memory_mb = *std::max_element(analysis.memory_usage_over_time.begin(), analysis.memory_usage_over_time.end());
        analysis.average_memory_mb = std::accumulate(analysis.memory_usage_over_time.begin(), analysis.memory_usage_over_time.end(), 0.0) / analysis.memory_usage_over_time.size();
        
        if (analysis.memory_usage_over_time.size() > 1) {
            double first_mem = analysis.memory_usage_over_time.front();
            double last_mem = analysis.memory_usage_over_time.back();
            analysis.memory_growth_rate_mb_per_step = (last_mem - first_mem) / (analysis.memory_usage_over_time.size() - 1);
            
            // Simple memory leak detection (growth rate > 1MB per 100 steps)
            if (analysis.memory_growth_rate_mb_per_step > 0.01) {
                analysis.memory_leak_detected = true;
                analysis.memory_leak_rate = analysis.memory_growth_rate_mb_per_step;
            }
        }
    }
    
    return analysis;
}

PerformanceMonitor::SystemAnalysis PerformanceMonitor::analyzeSystemResources(uint64_t steps) const {
    SystemAnalysis analysis;
    
    std::lock_guard<std::mutex> lock(metrics_mutex_);
    
    if (metrics_history_.empty() || steps == 0) {
        return analysis;
    }
    
    uint64_t start_idx = std::max<uint64_t>(0, metrics_history_.size() - steps);
    analysis.cpu_usage_over_time.reserve(steps);
    analysis.memory_usage_over_time.reserve(steps);
    
    for (uint64_t i = start_idx; i < metrics_history_.size(); ++i) {
        const auto& metrics = metrics_history_[i];
        analysis.cpu_usage_over_time.push_back(metrics.system_resources.cpu_usage_percent);
        analysis.memory_usage_over_time.push_back(metrics.system_resources.memory_percent);
    }
    
    if (!analysis.cpu_usage_over_time.empty()) {
        analysis.average_cpu_usage = std::accumulate(analysis.cpu_usage_over_time.begin(), analysis.cpu_usage_over_time.end(), 0.0) / analysis.cpu_usage_over_time.size();
        analysis.peak_cpu_usage = *std::max_element(analysis.cpu_usage_over_time.begin(), analysis.cpu_usage_over_time.end());
        
        analysis.average_memory_percent = std::accumulate(analysis.memory_usage_over_time.begin(), analysis.memory_usage_over_time.end(), 0.0) / analysis.memory_usage_over_time.size();
        analysis.peak_memory_percent = *std::max_element(analysis.memory_usage_over_time.begin(), analysis.memory_usage_over_time.end());
        
        // Detect saturation events
        analysis.cpu_saturation_events = std::count_if(analysis.cpu_usage_over_time.begin(), analysis.cpu_usage_over_time.end(),
            [this](double cpu) { return cpu > cpu_threshold_percent_; });
        
        analysis.memory_pressure_events = std::count_if(analysis.memory_usage_over_time.begin(), analysis.memory_usage_over_time.end(),
            [](double mem) { return mem > 80.0; });
    }
    
    return analysis;
}

PerformanceMonitor::PerformanceComparison PerformanceMonitor::compareConfigurations(
    const std::vector<std::vector<PerformanceMetrics>>& datasets,
    const std::vector<std::string>& config_names) const {
    PerformanceComparison comparison;
    
    if (datasets.size() != config_names.size() || datasets.empty()) {
        return comparison;
    }
    
    std::map<std::string, double> overall_scores;
    
    for (size_t i = 0; i < datasets.size(); ++i) {
        const auto& dataset = datasets[i];
        if (dataset.empty()) {
            continue;
        }
        
        double score = 0.0;
        
        // Calculate performance score based on multiple metrics
        for (const auto& metrics : dataset) {
            score += metrics.neural_computation.average_firing_rate;
            score += (100.0 - metrics.system_resources.cpu_usage_percent); // Higher score for lower CPU usage
            score += (100.0 - metrics.system_resources.memory_percent); // Higher score for lower memory usage
            score += 100.0 / (metrics.neural_computation.step_time_ms + 0.001); // Higher score for faster steps
        }
        
        score /= dataset.size();
        overall_scores[config_names[i]] = score;
    }
    
    // Find best configuration
    double max_score = 0.0;
    for (const auto& pair : overall_scores) {
        if (pair.second > max_score) {
            max_score = pair.second;
            comparison.best_configuration = pair.first;
        }
    }
    
    comparison.performance_score = max_score;
    
    // Calculate improvements/degradations relative to best
    if (!comparison.best_configuration.empty()) {
        double best_score = overall_scores[comparison.best_configuration];
        
        for (const auto& pair : overall_scores) {
            if (pair.first != comparison.best_configuration) {
                double improvement = best_score - pair.second;
                if (improvement > 0) {
                    comparison.metric_improvements[pair.first] = improvement;
                } else {
                    comparison.metric_degradations[pair.first] = -improvement;
                }
            }
        }
    }
    
    return comparison;
}

void PerformanceMonitor::updateSystemMetrics(SystemResourceMetrics& metrics) {
    metrics.cpu_usage_percent = calculateCPUUsage();
    metrics.memory_usage_mb = calculateMemoryUsageMB();
    metrics.memory_percent = (metrics.memory_usage_mb / memory_limit_mb_) * 100.0;
    metrics.timestamp = std::chrono::steady_clock::now();
}

void PerformanceMonitor::updateNeuralMetrics(NeuralComputationMetrics& metrics, const PerformanceMetrics& prev_metrics) {
    // Calculate time differences
    auto current_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - prev_metrics.neural_computation.timestamp);
    
    // Update neural computation metrics (simplified calculation)
    metrics.timestamp = current_time;
}

void PerformanceMonitor::updateMemoryMetrics(MemoryUsageMetrics& metrics, const PerformanceMetrics& prev_metrics) {
    // Calculate memory growth rate
    auto current_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - prev_metrics.memory_usage.timestamp);
    
    if (duration.count() > 0) {
        metrics.memory_growth_rate_mb_per_step = 
            (metrics.total_memory_mb - prev_metrics.memory_usage.total_memory_mb) / (duration.count() / 1000.0);
    }
    
    metrics.timestamp = current_time;
}

void PerformanceMonitor::updateSpikeMetrics(SpikeEventMetrics& metrics, const PerformanceMetrics& prev_metrics) {
    auto current_time = std::chrono::steady_clock::now();
    metrics.timestamp = current_time;
}

double PerformanceMonitor::calculateCPUUsage() const {
    // Simplified CPU usage calculation (would need platform-specific implementation)
    return 50.0; // Placeholder
}

double PerformanceMonitor::calculateMemoryUsageMB() const {
    // Simplified memory usage calculation (would need platform-specific implementation)
    return 512.0; // Placeholder
}

uint64_t PerformanceMonitor::getProcessID() const {
    // Platform-specific process ID retrieval
    return 12345; // Placeholder
}

std::string PerformanceMonitor::metricsToJSON(const PerformanceMetrics& metrics) {
    std::ostringstream oss;
    oss << "{\"step\":\" << metrics.simulation_step << \",\"";
    oss << "\"wall_clock_time_ms\":\" << metrics.wall_clock_time_ms << \",\"";
    oss << "\"neuron_count\":\" << metrics.neural_computation.neuron_count << \",\"";
    oss << "\"synapse_count\":\" << metrics.neural_computation.synapse_count << \",\"";
    oss << "\"firing_neuron_count\":\" << metrics.neural_computation.firing_neuron_count << \",\"";
    oss << "\"total_spikes\":\" << metrics.neural_computation.total_spikes << \",\"";
    oss << "\"cpu_usage_percent\":\" << metrics.system_resources.cpu_usage_percent << \",\"";
    oss << "\"memory_usage_mb\":\" << metrics.system_resources.memory_usage_mb << \",\"";
    oss << "\"working_memory_mb\":\" << metrics.memory_usage.working_memory_mb << \",\"";
    oss << "\"episodic_memory_mb\":\" << metrics.memory_usage.episodic_memory_mb << \",\"";
    oss << "\"step_time_ms\":\" << metrics.neural_computation.step_time_ms << \",\"";
    oss << "\"neuron_update_time_ms\":\" << metrics.neural_computation.neuron_update_time_ms << \",\"";
    oss << "\"plasticity_time_ms\":\" << metrics.neural_computation.plasticity_time_ms << \",\"";
    oss << "\"memory_update_time_ms\":\" << metrics.neural_computation.memory_update_time_ms << \"}\"";
    return oss.str();
}

PerformanceMetrics PerformanceMonitor::metricsFromJSON(const std::string& json_str) {
    PerformanceMetrics metrics;
    // Simplified JSON parsing (would need proper JSON library)
    return metrics;
}

} // namespace nlm
