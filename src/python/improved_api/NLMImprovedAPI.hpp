#pragma once

#include "../src/core/Config/Config.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * Configuration Builder - Fluent API for creating common configurations
 */
class ConfigBuilder {
public:
    ConfigBuilder();
    
    // Neural system configuration
    ConfigBuilder& with_neural_params(int neuron_count = 1000,
                                       float gain = 1.0f,
                                       float threshold = -50.0f);
    ConfigBuilder& with_synapse_params(float density = 0.1f,
                                        float weight_mean = 0.5f,
                                        float weight_std = 0.1f);
    
    // Plasticity configuration
    ConfigBuilder& with_plasticity(bool enable_stdp = true,
                                    bool enable_hebbian = true,
                                    bool enable_structural = true,
                                    float stdp_learning_rate = 0.001f,
                                    float hebbian_strength = 0.5f);
    
    // Development configuration  
    ConfigBuilder& with_development(const std::string& stage = "Research",
                                    double age_rate = 1.0,
                                    bool enable_critical_period = true);
    
    // Neuromodulation configuration
    ConfigBuilder& with_neuromodulation(bool enable_curiosity = true,
                                          bool enable_novelty = true,
                                          bool enable_reward = true,
                                          float dopamine_scale = 1.0f);
    
    // Memory configuration
    ConfigBuilder& with_memory(bool enable_working_memory = true,
                               bool enable_episodic_memory = true,
                               float memory_capacity = 1000.0f);
    
    // Prediction configuration
    ConfigBuilder& with_prediction(bool enable_prediction = true,
                                   float prediction_horizon = 0.1f,
                                   float prediction_error_weight = 1.0f);
    
    // World/sensory configuration
    ConfigBuilder& with_world(int width = 20,
                              int height = 20,
                              int vision_width = 8,
                              int vision_height = 8,
                              float vision_range = 1.0f);
    
    // Agent configuration
    ConfigBuilder& with_agent(bool enable_learning = true,
                              bool enable_planning = true,
                              bool enable_social = false);
    
    // Build the configuration
    std::shared_ptr<Config> build();
    
    // Quick preset configurations
    static std::shared_ptr<Config> create_research_config(int width = 20, int height = 20);
    static std::shared_ptr<Config> create_rl_config(float learning_rate = 0.01f);
    static std::shared_ptr<Config> create_developmental_config(double lifespan = 60.0);
    static std::shared_ptr<Config> create_reproduction_config();
    
    // Utility methods
    std::string get_config_summary() const;
    void print_config() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Brain Context Manager - Automatic brain lifecycle management
 */
class BrainContextManager {
public:
    BrainContextManager();
    explicit BrainContextManager(std::shared_ptr<Config> config);
    
    ~BrainContextManager();
    
    // Disable copying
    BrainContextManager(const BrainContextManager&) = delete;
    BrainContextManager& operator=(const BrainContextManager&) = delete;
    
    // Enable moving
    BrainContextManager(BrainContextManager&&) noexcept;
    BrainContextManager& operator=(BrainContextManager&&) noexcept;
    
    // Context manager interface
    bool enter();
    void exit(int exception_code = 0);
    
    // Accessors
    std::shared_ptr<Brain> brain() const;
    std::shared_ptr<Config> config() const;
    
    // Operations
    void initialize();
    void step(size_t step_count);
    void reset();
    void save(const std::string& filepath) const;
    void load(const std::string& filepath);
    
    // Convenience methods
    void run_simulation(size_t steps, double timestep = 0.001);
    void run_episode(std::shared_ptr<SimpleWorld> world,
                     std::shared_ptr<AgentBrain> agent,
                     size_t steps,
                     double timestep = 0.1);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Agent Context Manager - Automatic agent lifecycle management
 */
class AgentContextManager {
public:
    AgentContextManager();
    AgentContextManager(std::shared_ptr<Brain> brain,
                       std::shared_ptr<SimpleWorld> world);
    
    ~AgentContextManager();
    
    // Disable copying
    AgentContextManager(const AgentContextManager&) = delete;
    AgentContextManager& operator=(const AgentContextManager&) = delete;
    
    // Enable moving
    AgentContextManager(AgentContextManager&&) noexcept;
    AgentContextManager& operator=(AgentContextManager&&) noexcept;
    
    // Context manager interface
    bool enter();
    void exit(int exception_code = 0);
    
    // Accessors
    std::shared_ptr<AgentBrain> agent() const;
    std::shared_ptr<Brain> brain() const;
    std::shared_ptr<SimpleWorld> world() const;
    
    // Operations
    void initialize();
    void reset();
    
    // Convenience methods
    void process_sensor_data(const SensoryPercept& percept);
    Action decode_motor_command();
    void apply_reward(float reward, float predicted = 0.0f);
    void update_development(double timestep = 0.1);
    
    // Batch operations
    void process_sensor_batch(const std::vector<SensoryPercept>& percepts);
    std::vector<Action> decode_motor_batch(size_t count);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Simulation Helpers - Common simulation patterns
 */
namespace SimulationHelpers {
    // Quick simulation runner
    struct SimulationResult {
        size_t total_steps;
        size_t total_spikes;
        float avg_firing_rate;
        float final_reward;
        size_t active_neurons;
        std::chrono::duration<double> wall_clock_time;
        std::vector<std::string> log_messages;
    };
    
    SimulationResult run_brain_simulation(std::shared_ptr<Brain> brain,
                                           size_t steps,
                                           double timestep = 0.001);
    
    SimulationResult run_agent_simulation(std::shared_ptr<AgentBrain> agent,
                                          std::shared_ptr<SimpleWorld> world,
                                          size_t steps,
                                          double timestep = 0.1);
    
    SimulationResult run_learning_episode(std::shared_ptr<AgentBrain> agent,
                                          std::shared_ptr<SimpleWorld> world,
                                          size_t steps,
                                          bool enable_learning = true);
    
    // Pattern runners
    void run_research_pattern(std::shared_ptr<Brain> brain, size_t steps);
    void run_rl_pattern(std::shared_ptr<AgentBrain> agent,
                       std::shared_ptr<SimpleWorld> world,
                       size_t steps);
    void run_development_pattern(std::shared_ptr<Brain> brain,
                                size_t steps,
                                double lifespan = 60.0);
    
    // Data collection
    std::vector<float> collect_firing_rates(std::shared_ptr<Brain> brain,
                                            size_t samples,
                                            double timestep = 0.001);
    std::vector<float> collect_reward_history(std::shared_ptr<AgentBrain> agent,
                                              size_t steps);
}

/**
 * Error Handling - Pythonic exceptions for NLM errors
 */
namespace ErrorHandling {
    class NLMError : public std::runtime_error {
    public:
        explicit NLMError(const std::string& message);
        virtual ~NLMError() noexcept = default;
    };
    
    class BrainInitializationError : public NLMError {
    public:
        explicit BrainInitializationError(const std::string& message);
    };
    
    class ConfigurationError : public NLMError {
    public:
        explicit ConfigurationError(const std::string& message);
    };
    
    class SimulationError : public NLMError {
    public:
        explicit SimulationError(const std::string& message);
    };
    
    class MemoryError : public NLMError {
    public:
        explicit MemoryError(const std::string& message);
    };
    
    class ValidationError : public NLMError {
    public:
        explicit ValidationError(const std::string& message);
    };
    
    // Error recovery utilities
    bool attempt_recovery(std::shared_ptr<Brain> brain, int max_attempts = 3);
    std::string get_error_context(const std::exception& e);
    void log_error_safely(const std::exception& e);
}

/**
 * Performance Monitoring - Resource usage tracking
 */
class PerformanceMonitor {
public:
    struct PerformanceStats {
        double start_time;
        double end_time;
        size_t steps_completed;
        size_t total_spikes;
        float avg_firing_rate;
        size_t neurons_active;
        size_t memory_usage_mb;
        double cpu_usage_percent;
        std::vector<double> timestep_durations;
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    // Enable moving
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;
    
    // Control
    void start();
    PerformanceStats stop();
    bool is_running() const;
    
    // Configuration
    void set_sampling_interval(double interval);
    void enable_memory_tracking(bool enable);
    void enable_cpu_tracking(bool enable);
    
    // Data access
    PerformanceStats get_current_stats() const;
    std::string get_performance_report() const;
    
    // Static utilities
    static double get_memory_usage_mb();
    static double get_cpu_usage_percent();
    static size_t get_thread_count();
    static std::string get_system_info();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Quick Setup - One-line convenience functions
 */
namespace QuickSetup {
    // Complete agent world setup in one call
    std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
    create_agent_world_setup(int width = 10, int height = 10,
                            int vision_width = 8, int vision_height = 8,
                            bool enable_learning = true,
                            bool enable_development = true,
                            bool enable_curiosity = true);
    
    // Predefined configuration shortcuts
    std::shared_ptr<Config> create_research_config(int width = 20, int height = 20);
    std::shared_ptr<Config> create_rl_config(float learning_rate = 0.01f);
    std::shared_ptr<Config> create_developmental_config(double lifespan = 60.0);
    
    // Simulation shortcuts
    void run_research_simulation(size_t steps = 1000);
    void run_rl_simulation(size_t steps = 2000);
    void run_development_simulation(size_t steps = 3000);
    
    // Analysis shortcuts
    float analyze_brain_efficiency(std::shared_ptr<Brain> brain, size_t steps = 1000);
    float analyze_learning_efficiency(std::shared_ptr<AgentBrain> agent,
                                     std::shared_ptr<SimpleWorld> world,
                                     size_t steps = 1000);
}

/**
 * Utility Functions - Common helper operations
 */
namespace Utils {
    // String utilities
    std::string trim(const std::string& str);
    std::string to_lower(const std::string& str);
    std::string format_duration(double seconds);
    std::string format_number(size_t num);
    
    // Math utilities
    float clamp(float value, float min, float max);
    float lerp(float a, float b, float t);
    float sigmoid(float x);
    
    // File utilities
    bool file_exists(const std::string& path);
    std::string get_temp_filename(const std::string& prefix = "nlm_");
    bool safe_write_file(const std::string& path, const std::string& content);
    
    // Random utilities
    uint64_t seed_from_time();
    uint64_t seed_from_string(const std::string& str);
    
    // Time utilities
    double current_time_seconds();
    std::string timestamp_string();
    
    // Conversion utilities
    std::string neuron_type_to_string(NeuronType type);
    NeuronType string_to_neuron_type(const std::string& str);
    std::string action_type_to_string(ActionType type);
    ActionType string_to_action_type(const std::string& str);
}

} // namespace nlm