// SimulationContext.hpp
// Context manager for NLM simulation lifecycle

#pragma once

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {

// Performance metrics for simulation
struct PerformanceMetrics {
    double initialization_time;
    double total_step_time;
    double avg_step_time;
    size_t total_spikes;
    float avg_firing_rate;
    float e_i_ratio;
    size_t active_neurons;
    size_t steps_completed;
    bool success;
    std::string error_message;
};

// Simulation statistics
struct SimulationStatistics {
    std::vector<double> step_times;
    std::vector<size_t> spike_counts;
    std::vector<float> firing_rates;
    std::vector<float> e_i_ratios;
    std::vector<size_t> active_neurons;
    std::vector<float> neuromod_levels;
    std::vector<float> curiosity_levels;
    std::vector<float> novelty_levels;
    std::vector<size_t> action_counts;
    double total_time;
    size_t total_steps;
};

// Result type for simulation operations
struct SimulationResult {
    bool success;
    std::string message;
    PerformanceMetrics metrics;
    SimulationStatistics statistics;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    std::string config_id;
};

// SimulationContext: High-level API for managing NLM simulations
class SimulationContext {
public:
    SimulationContext();
    ~SimulationContext();
    
    // Disable copying
    SimulationContext(const SimulationContext&) = delete;
    SimulationContext& operator=(const SimulationContext&) = delete;
    
    // Factory methods for creating common components
    static std::shared_ptr<Config> create_default_config();
    static std::shared_ptr<Brain> create_brain(const std::shared_ptr<Config>& config);
    static std::shared_ptr<AgentBrain> create_agent_brain(const std::shared_ptr<Brain>& brain);
    static std::shared_ptr<SimpleWorld> create_world();
    
    // Create and initialize complete simulation
    bool initialize(const std::shared_ptr<Config>& config, const std::shared_ptr<SimpleWorld>& world);
    
    // Run simulation methods
    bool step(size_t steps = 1, bool autoreset = false);
    bool run_episode(size_t max_steps, bool autoreset = false);
    bool run_agent_episode(size_t max_steps, bool enable_learning = true, bool autoreset = false);
    
    // Convenience simulation runners
    SimulationResult run_silent_brain(size_t steps);
    SimulationResult run_agent_episode(std::shared_ptr<SimpleWorld> world, size_t max_steps, bool enable_learning = true);
    
    // Getters
    std::shared_ptr<Brain> get_brain() const { return brain_; }
    std::shared_ptr<AgentBrain> get_agent() const { return agent_; }
    std::shared_ptr<SimpleWorld> get_world() const { return world_; }
    std::shared_ptr<Config> get_config() const { return config_; }
    
    // Statistics and performance
    SimulationStatistics get_statistics() const;
    PerformanceMetrics get_performance() const;
    std::string get_status_summary() const;
    
    // Configuration presets
    static std::vector<std::pair<std::string, std::shared_ptr<Config>>> get_presets();
    static std::tuple<std::vector<std::string>, std::vector<std::string>> validate_config(const std::shared_ptr<Config>& config);
    
    // Batch operations
    std::vector<SimulationResult> benchmark_configs(const std::vector<std::shared_ptr<Config>>& configs, size_t steps);
    std::vector<SimulationResult> run_multiple_episodes(const std::vector<size_t>& episode_lengths, bool enable_learning = true);
    
    // Resource management
    void reset();
    void cleanup();
    bool is_initialized() const { return is_initialized_; }
    
    // Monitoring and utilities
    void enable_monitoring(bool enable) { monitoring_enabled_ = enable; }
    void set_log_level(int level) { log_level_ = level; }
    bool is_monitoring_enabled() const { return monitoring_enabled_; }
    
        // Statistics collection utilities
    template<typename Func>
    SimulationStatistics collect_statistics(size_t steps, Func simulation_func);
    
    // Performance profiling utilities
    template<typename Func>
    PerformanceMetrics profile_performance(Func simulation_func, size_t iterations = 1);
    
    // Advanced profiling and benchmarking
    std::vector<PerformanceMetrics> benchmark_brain(const std::shared_ptr<Brain>& brain, 
                                                     const std::vector<size_t>& step_counts,
                                                     size_t warmup_steps = 0);
    
    std::vector<PerformanceMetrics> benchmark_agent(const std::shared_ptr<Config>& config,
                                                    const std::shared_ptr<SimpleWorld>& world,
                                                    const std::vector<size_t>& episode_lengths,
                                                    size_t warmup_steps = 0);
    
    SimulationStatistics collect_detailed_stats(size_t steps);
    PerformanceMetrics profile_steps(size_t steps, bool warmup = true);
    
    // Visualization and monitoring helpers
    std::string generate_html_dashboard(const std::string& output_path) const;
    void generate_performance_report(const std::string& output_path) const;
    void create_monitoring_script(const std::string& output_path) const;
    
    // Get configuration identifier
    std::string get_config_id() const { return config_id_; }
    
    void set_config_id(const std::string& id) { config_id_ = id; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    std::string config_id_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    
    SimulationStep current_step_;
    Timestamp current_time_;
    bool is_initialized_;
    bool monitoring_enabled_;
    int log_level_;
    double total_simulation_time_;
    size_t total_steps_completed_;
    
    // Timing for performance measurement
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point last_step_time_;
    
    // Statistics storage
    SimulationStatistics accumulated_stats_;
    PerformanceMetrics accumulated_performance_;
    
    Impl() : current_step_(0), current_time_(0.0), 
             is_initialized_(false), monitoring_enabled_(false),
             log_level_(0), total_simulation_time_(0.0), 
             total_steps_completed_(0) {}
    
    void reset_stats() {
        accumulated_stats_ = SimulationStatistics();
        accumulated_performance_ = PerformanceMetrics();
        accumulated_stats_.step_times.clear();
        accumulated_stats_.spike_counts.clear();
        accumulated_stats_.firing_rates.clear();
        accumulated_stats_.e_i_ratios.clear();
        accumulated_stats_.active_neurons.clear();
        accumulated_stats_.neuromod_levels.clear();
        accumulated_stats_.curiosity_levels.clear();
        accumulated_stats_.novelty_levels.clear();
        accumulated_stats_.action_counts.clear();
    }
    
    void record_step(double step_time, size_t spike_count, float firing_rate, 
                    float e_i_ratio, size_t active_neurons, 
                    float neuromod_level = 0.0f, float curiosity_level = 0.0f,
                    float novelty_level = 0.0f, size_t action_count = 0) {
        accumulated_stats_.step_times.push_back(step_time);
        accumulated_stats_.spike_counts.push_back(spike_count);
        accumulated_stats_.firing_rates.push_back(firing_rate);
        accumulated_stats_.e_i_ratios.push_back(e_i_ratio);
        accumulated_stats_.active_neurons.push_back(active_neurons);
        accumulated_stats_.neuromod_levels.push_back(neuromod_level);
        accumulated_stats_.curiosity_levels.push_back(curiosity_level);
        accumulated_stats_.novelty_levels.push_back(novelty_level);
        accumulated_stats_.action_counts.push_back(action_count);
    }
    
    void update_performance() {
        if (accumulated_stats_.total_steps > 0) {
            accumulated_performance_.avg_step_time = 
                accumulated_stats_.total_time / accumulated_stats_.total_steps;
        }
        if (!accumulated_stats_.firing_rates.empty()) {
            float sum = 0.0f;
            for (float rate : accumulated_stats_.firing_rates) {
                sum += rate;
            }
            accumulated_performance_.avg_firing_rate = sum / accumulated_stats_.firing_rates.size();
        }
        
        if (!accumulated_stats_.e_i_ratios.empty()) {
            float sum = 0.0f;
            for (float ratio : accumulated_stats_.e_i_ratios) {
                sum += ratio;
            }
            accumulated_performance_.e_i_ratio = sum / accumulated_stats_.e_i_ratios.size();
        }
        
        accumulated_performance_.total_spikes = 0;
        for (size_t count : accumulated_stats_.spike_counts) {
            accumulated_performance_.total_spikes += count;
        }
        
        accumulated_performance_.steps_completed = accumulated_stats_.total_steps;
    }
    
    // Helper methods
    void initialize_components();
    void cleanup_components();
    void log_step_info(size_t step) const;
    double get_current_time() const;
};