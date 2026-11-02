#ifndef PYTHON_BINDINGS_H
#define PYTHON_BINDINGS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>

// Forward declarations
namespace nlm {
    class Brain;
    class Config;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Action;
    class Vision;
    class Audio;
    class InternalSignals;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
}

// Custom exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

class TrainingError : public NLMError {
public:
    explicit TrainingError(const std::string& msg) : NLMError("Training Error: " + msg) {}
};

class SerializationError : public NLMError {
public:
    explicit SerializationError(const std::string& msg) : NLMError("Serialization Error: " + msg) {}
};

// Configuration utilities
namespace config {
    /// Create default configuration with optimized settings for training
    std::shared_ptr<nlm::Config> create_training_config(
        const std::string& config_file = "",
        float learning_rate = 0.01f,
        size_t hidden_neurons = 1000,
        size_t population_size = 100
    );
    
    /// Validate configuration parameters
    void validate_config(const nlm::Config& config);
    
    /// Get default configuration keys and values
    std::map<std::string, std::string> get_default_config();
    
    /// Merge configuration dictionaries
    void merge_config(nlm::Config& config, 
                     const std::map<std::string, std::string>& overrides);
}

// Brain factory utilities
namespace brain {
    /// Create a brain with enhanced error checking and validation
    std::shared_ptr<nlm::Brain> create_brain(
        std::shared_ptr<nlm::Config> config,
        bool initialize_now = true,
        const std::string& name = ""
    );
    
    /// Create multiple brains for parallel training
    std::vector<std::shared_ptr<nlm::Brain>> create_multiple_brains(
        const std::vector<std::shared_ptr<nlm::Config>>& configs,
        bool initialize_now = true
    );
    
    /// Load brain from checkpoint with validation
    std::shared_ptr<nlm::Brain> load_brain(
        const std::string& filepath,
        const std::string& config_file = ""
    );
}

// Training utilities
namespace training {
    struct EpisodeResult {
        size_t steps;
        float total_reward;
        float average_reward;
        float max_reward;
        float std_reward;
        std::vector<float> reward_history;
        std::vector<float> action_values;
        float final_reward;
        bool success;
        std::string error_message;
    };
    
    /// Run a single training episode with advanced features
    EpisodeResult run_episode(
        nlm::Brain& brain,
        nlm::AgentBrain& agent,
        nlm::SimpleWorld& world,
        size_t max_steps,
        bool render = false,
        std::function<void(const nlm::SensoryPercept&)> observation_callback = nullptr
    );
    
    /// Training loop with advanced metrics and callbacks
    std::vector<EpisodeResult> train(
        std::shared_ptr<nlm::Brain> brain,
        std::shared_ptr<nlm::AgentBrain> agent,
        std::shared_ptr<nlm::SimpleWorld> world,
        size_t num_episodes,
        size_t steps_per_episode,
        bool render = false,
        std::function<void(size_t, const EpisodeResult&)> callback = nullptr,
        std::function<bool()> stop_condition = nullptr
    );
    
    /// Evaluate brain performance
    float evaluate(
        const nlm::Brain& brain,
        const nlm::AgentBrain& agent,
        const nlm::SimpleWorld& world,
        size_t num_episodes,
        size_t steps_per_episode = 100
    );
    
    /// Save training metrics to file
    void save_metrics(
        const std::vector<EpisodeResult>& metrics,
        const std::string& filepath,
        const std::string& format = "json"
    );
}

// Visualization utilities
namespace visualization {
    /// Generate training metrics plot
    void plot_training_metrics(
        const std::vector<training::EpisodeResult>& metrics,
        const std::string& output_file = "training_metrics.png",
        bool show_plots = false
    );
    
    /// Visualize brain structure
    void visualize_brain(
        const nlm::Brain& brain,
        const std::string& output_file = "brain_structure.png",
        bool show_neurons = true,
        bool show_connections = true,
        bool show_regions = false
    );
    
    /// Create training dashboard
    void create_training_dashboard(
        const std::vector<training::EpisodeResult>& metrics,
        const std::string& output_dir = "training_dashboard"
    );
}

// Model export utilities
namespace export {
    /// Export brain to TensorFlow SavedModel format
    void export_to_tensorflow(
        const nlm::Brain& brain,
        const std::string& output_path,
        const std::vector<std::string>& input_names = {"sensory_input"},
        const std::vector<std::string>& output_names = {"action_output"}
    );
    
    /// Export brain to ONNX format
    void export_to_onnx(
        const nlm::Brain& brain,
        const std::string& output_path,
        const std::vector<std::vector<size_t>>& input_shapes,
        const std::vector<std::string>& input_names,
        const std::vector<std::vector<size_t>>& output_shapes,
        const std::vector<std::string>& output_names
    );
    
    /// Export brain to pickle format
    void export_to_pickle(
        const nlm::Brain& brain,
        const std::string& output_path,
        bool include_weights = true,
        bool include_config = true
    );
}

// Memory and performance utilities
namespace memory {
    /// Clear Python garbage collector and optimize memory usage
    void optimize_memory();
    
    /// Get memory usage statistics
    std::map<std::string, double> get_memory_stats();
    
    /// Create deep copy of brain state
    std::shared_ptr<nlm::Brain> clone_brain(const nlm::Brain& brain);
    
    /// Serialize brain state to bytes
    std::vector<char> serialize_brain(const nlm::Brain& brain);
    
    /// Deserialize brain state from bytes
    std::shared_ptr<nlm::Brain> deserialize_brain(const std::vector<char>& data);
}

// Context manager for automatic cleanup
class BrainSession {
private:
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::AgentBrain> agent_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    bool auto_reset_;
    bool auto_cleanup_;
    
public:
    BrainSession(
        std::shared_ptr<nlm::Brain> brain,
        std::shared_ptr<nlm::AgentBrain> agent,
        std::shared_ptr<nlm::SimpleWorld> world,
        bool auto_reset = true,
        bool auto_cleanup = true
    ) : brain_(brain), agent_(agent), world_(world), 
        auto_reset_(auto_reset), auto_cleanup_(auto_cleanup) {
        if (!brain_ || !agent_ || !world_) {
            throw ValidationError("Invalid brain, agent, or world pointers");
        }
    }
    
    ~BrainSession() {
        if (auto_cleanup_) {
            reset();
        }
    }
    
    // Disable copying
    BrainSession(const BrainSession&) = delete;
    BrainSession& operator=(const BrainSession&) = delete;
    
    // Enable moving
    BrainSession(BrainSession&& other) noexcept
        : brain_(std::move(other.brain_)),
          agent_(std::move(other.agent_)),
          world_(std::move(other.world_)),
          auto_reset_(other.auto_reset_),
          auto_cleanup_(other.auto_cleanup_) {}
    
    BrainSession& operator=(BrainSession&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
            agent_ = std::move(other.agent_);
            world_ = std::move(other.world_);
            auto_reset_ = other.auto_reset_;
            auto_cleanup_ = other.auto_cleanup_;
        }
        return *this;
    }
    
    // Accessors
    nlm::Brain& get_brain() { return *brain_; }
    nlm::AgentBrain& get_agent() { return *agent_; }
    nlm::SimpleWorld& get_world() { return *world_; }
    
    const nlm::Brain& get_brain() const { return *brain_; }
    const nlm::AgentBrain& get_agent() const { return *agent_; }
    const nlm::SimpleWorld& get_world() const { return *world_; }
    
    // Convenience methods
    void reset() {
        if (brain_) brain_->reset();
        if (agent_) agent_->reset();
        if (world_) world_->reset();
    }
    
    void initialize() {
        if (world_ && agent_) {
            agent_->initialize(*world_);
        }
        if (brain_) {
            brain_->initialize();
        }
    }
    
    // Execute a single step with automatic cleanup
    bool step(size_t step, float time = 0.0f) {
        if (!brain_ || !agent_ || !world_) {
            return false;
        }
        
        try {
            // Get sensory input
            auto percept = world_->getSensoryPercept();
            agent_->processSensoryInput(percept);
            brain_->step(step, time);
            return true;
        } catch (const std::exception& e) {
            throw TrainingError(std::string("Step execution failed: ") + e.what());
        }
        return false;
    }
};

// Main binding functions
void bind_core_types(pybind11::module_& m);
void bind_config_module(pybind11::module_& m);
void bind_brain_module(pybind11::module_& m);
void bind_agent_module(pybind11::module_& m);
void bind_world_module(pybind11::module_& m);
void bind_sensory_module(pybind11::module_& m);
void bind_motor_module(pybind11::module_& m);
void bind_advanced_features(pybind11::module_& m);

#endif // PYTHON_BINDINGS_H
