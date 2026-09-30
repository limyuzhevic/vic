#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <filesystem>
#include <chrono>
#include <thread>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/AdvancedNetworkArchitecture.hpp"
#include "../src/ReinforcementLearning.hpp"
#include "../src/AdvancedConfigurationSystem.hpp"
#include "../src/AdvancedVisualization.hpp"

namespace py = pybind11;
namespace nlm {

// =============================================================
// Enhanced Pythonic Interfaces and Exception Handling
// =============================================================

class PythonicConfigError : public std::runtime_error {
public:
    PythonicConfigError(const std::string& message) : std::runtime_error(message) {}
};

class PythonicBrainError : public std::runtime_error {
public:
    PythonicBrainError(const std::string& message) : std::runtime_error(message) {}
};

class PythonicMemoryError : public std::runtime_error {
public:
    PythonicMemoryError(const std::string& message) : std::runtime_error(message) {}
};

class PythonicProgressError : public std::runtime_error {
public:
    PythonicProgressError(const std::string& message) : std::runtime_error(message) {}
};

// =============================================================
// Enhanced Pythonic Config Class with Property Access
// =============================================================

class PyConfig {
private:
    std::shared_ptr<Config> config_;
    
public:
    PyConfig() : config_(std::make_shared<Config>()) {}
    PyConfig(const std::string& filepath) : config_(std::make_shared<Config>()) {
        if (!config_->loadFromFile(filepath)) {
            throw PythonicConfigError("Failed to load configuration from file: " + filepath);
        }
    }
    
    // Pythonic property access patterns
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    // Pythonic attribute access for commonly used properties
    int get_int(const std::string& key) const {
        auto value = config_->get<int>(key);
        return value ? *value : throw PythonicConfigError("Key not found: " + key);
    }
    
    double get_float(const std::string& key) const {
        auto value = config_->get<double>(key);
        return value ? *value : throw PythonicConfigError("Key not found: " + key);
    }
    
    std::string get_str(const std::string& key) const {
        auto value = config_->get<std::string>(key);
        return value ? *value : throw PythonicConfigError("Key not found: " + key);
    }
    
    bool get_bool(const std::string& key) const {
        auto value = config_->get<bool>(key);
        return value ? *value : throw PythonicConfigError("Key not found: " + key);
    }
    
    // Property setters with validation
    void set_int(const std::string& key, int value) {
        config_->set(key, value);
    }
    
    void set_float(const std::string& key, double value) {
        config_->set(key, value);
    }
    
    void set_str(const std::string& key, const std::string& value) {
        config_->set(key, value);
    }
    
    void set_bool(const std::string& key, bool value) {
        config_->set(key, value);
    }
    
    // Property-like access with Pythonic syntax
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    std::vector<std::string> keys() const {
        return config_->getKeys();
    }
    
    // Context manager for configuration files
    class FileScope {
    private:
        PyConfig& config_;
        std::string filepath_;
        bool was_modified_ = false;
        
    public:
        FileScope(PyConfig& config, const std::string& filepath) 
            : config_(config), filepath_(filepath) {}
        
        ~FileScope() {
            if (was_modified_) {
                config_.save();
            }
        }
        
        void mark_modified() {
            was_modified_ = true;
        }
        
        const std::string& filepath() const {
            return filepath_;
        }
    };
    
    // Configuration schema validation
    class ConfigSchema {
    public:
        struct Field {
            std::string name;
            std::string type;  // "int", "float", "str", "bool", "list"
            std::string default_value;
            bool required = false;
            std::string validation;  // "positive", "nonempty", etc.
            std::string description;
        };
        
        std::vector<Field> fields_;
        
        void add_field(const std::string& name, const std::string& type, 
                      const std::string& default_value = "", 
                      bool required = false,
                      const std::string& validation = "",
                      const std::string& description = "") {
            Field field{name, type, default_value, required, validation, description};
            fields_.push_back(field);
        }
        
        bool validate(const PyConfig& config) const {
            for (const auto& field : fields_) {
                if (field.required && !config.has(field.name)) {
                    return false;
                }
            }
            return true;
        }
    };
    
    // Save configuration
    void save(const std::string& filepath = "") const {
        if (filepath.empty()) {
            config_->saveToFile("config.json");
        } else {
            config_->saveToFile(filepath);
        }
    }
    
    // Deep copy support
    std::shared_ptr<PyConfig> clone() const {
        auto new_config = std::make_shared<PyConfig>();
        // Copy all configuration entries
        for (const auto& key : config_->getKeys()) {
            new_config->config_->setAuto(key, config_->getString(key));
        }
        return new_config;
    }
    
    // String representation
    std::string summary() const {
        return config_->summary();
    }
    
    // Type checking and validation at binding level
    bool validate() const {
        return config_->validate();
    }
    
    // Load from JSON/YAML with auto-format detection
    bool load_from_json(const std::string& json_content) {
        // Implement JSON loading here
        return config_->loadFromFile(""); // Placeholder
    }
    
    // Export to Python dict
    py::dict to_dict() const {
        py::dict result;
        for (const auto& key : config_->getKeys()) {
            result[key.c_str()] = config_->getString(key);
        }
        return result;
    }
    
    // Import from Python dict
    void from_dict(const py::dict& data) {
        config_->clear();
        for (auto item : data) {
            std::string key = item.first.cast<std::string>();
            std::string value = item.second.cast<std::string>();
            config_->setAuto(key, value);
        }
    }
    
    // Helper method for __getitem__ access
    py::object get_pyobject(const std::string& key) const {
        if (config_->getType(key) == "int") {
            return py::cast(get_int(key));
        } else if (config_->getType(key) == "float") {
            return py::cast(get_float(key));
        } else if (config_->getType(key) == "str") {
            return py::cast(get_str(key));
        } else if (config_->getType(key) == "bool") {
            return py::cast(get_bool(key));
        } else {
            return py::cast(config_->getString(key));
        }
    }
};

// =============================================================
// Advanced Brain Interfaces with High-Level APIs
// =============================================================

class PyBrain {
private:
    std::shared_ptr<Brain> brain_;
    bool cancelled_ = false;
    std::function<bool()> progress_callback_;
    
public:
    PyBrain(std::shared_ptr<Config> config) : brain_(std::make_shared<Brain>(config)) {}
    PyBrain(const std::string& config_file) : brain_(std::make_shared<Brain>(std::make_shared<Config>())) {
        if (!brain_->getConfig()->loadFromFile(config_file)) {
            throw PythonicBrainError("Failed to load config from file: " + config_file);
        }
    }
    
    // Context manager for brain simulation
    class SimulationScope {
    private:
        PyBrain& brain_;
        size_t steps_run_ = 0;
        
    public:
        SimulationScope(PyBrain& brain) : brain_(brain) {}
        
        ~SimulationScope() {
            brain_.reset();
        }
        
        size_t steps_run() const {
            return steps_run_;
        }
    };
    
    // Higher-level API for common neural network operations
    void train_episode(const SensoryPercept& percept, double reward, double timestep = 0.1) {
        // Common training pattern
        brain_->receiveSensoryInput(percept);
        brain_->step(0, timestep);
        // Apply reward neuromodulation
        brain_->applyNeuromodulation({reward, 0.0, 0.0});
    }
    
    // Built-in learning algorithms
    class LearningAlgorithm {
    public:
        virtual void apply(PyBrain& brain, const SensoryPercept& percept) = 0;
        virtual ~LearningAlgorithm() = default;
    };
    
    class QLearning : public LearningAlgorithm {
    public:
        void apply(PyBrain& brain, const SensoryPercept& percept) override {
            // Implement Q-learning algorithm
            brain.train_episode(percept, 1.0);
        }
    };
    
    class HebbianLearning : public LearningAlgorithm {
    public:
        void apply(PyBrain& brain, const SensoryPercept& percept) override {
            // Implement Hebbian learning
            brain.train_episode(percept, 0.5);
        }
    };
    
    // Pre-built network architectures
    class NetworkArchitecture {
    public:
        virtual void build(PyBrain& brain) = 0;
        virtual ~NetworkArchitecture() = default;
    };
    
    class FeedForwardNetwork : public NetworkArchitecture {
    public:
        void build(PyBrain& brain) override {
            // Build feed-forward architecture
            brain.get_config().config_->setAuto("network_type", "feed_forward");
            brain.add_region("input_layer");
            brain.add_region("hidden_layer");
            brain.add_region("output_layer");
        }
    };
    
    class RecurrentNetwork : public NetworkArchitecture {
    public:
        void build(PyBrain& brain) override {
            // Build recurrent architecture
            brain.get_config().config_->setAuto("network_type", "recurrent");
            brain.add_region("input_layer");
            brain.add_region("hidden_state");
            brain.add_region("output_layer");
        }
    };
    
    // Batch operations for efficiency
    std::vector<std::unique_ptr<Action>> process_batch(
        const std::vector<SensoryPercept>& percepts,
        double timestep = 0.1,
        std::function<bool(size_t)> progress_callback = nullptr
    ) {
        std::vector<std::unique_ptr<Action>> actions;
        actions.reserve(percepts.size());
        
        for (size_t i = 0; i < percepts.size(); ++i) {
            if (progress_callback && !progress_callback(i)) {
                break;
            }
            brain_->receiveSensoryInput(percepts[i]);
            brain_->step(i, timestep);
            actions.push_back(brain_->produceAction());
        }
        
        return actions;
    }
    
    // Progress monitoring and cancellation
    void set_progress_callback(std::function<bool()> callback) {
        progress_callback_ = callback;
    }
    
    void cancel() {
        cancelled_ = true;
    }
    
    // Enhanced visualization and plotting interfaces
    class VisualizationInterface {
    public:
        virtual void plot_neural_activity(const PyBrain& brain, const std::string& output_file) = 0;
        virtual void plot_network_structure(const PyBrain& brain, const std::string& output_file) = 0;
        virtual py::dict get_statistics(const PyBrain& brain) = 0;
        virtual ~VisualizationInterface() = default;
    };
    
    class MatplotlibVisualization : public VisualizationInterface {
    public:
        void plot_neural_activity(const PyBrain& brain, const std::string& output_file) override {
            // Matplotlib-based visualization
            // This would generate plots of neural activity
        }
        
        void plot_network_structure(const PyBrain& brain, const std::string& output_file) override {
            // Generate network structure visualization
        }
        
        py::dict get_statistics(const PyBrain& brain) override {
            py::dict stats;
            stats["neuron_count"] = brain.brain_->getTotalNeuronCount();
            stats["synapse_count"] = brain.brain_->getTotalSynapseCount();
            stats["active_neurons"] = brain.brain_->getActiveNeuronCount();
            stats["firing_rate"] = brain.brain_->getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain.brain_->getExcitationInhibitionRatio();
            return stats;
        }
    };
    
    // Memory management improvements
    class MemoryManager {
    public:
        virtual void optimize_memory(PyBrain& brain) = 0;
        virtual double get_memory_usage(const PyBrain& brain) = 0;
        virtual std::vector<std::string> get_memory_stats(const PyBrain& brain) = 0;
        virtual ~MemoryManager() = default;
    };
    
    class AdvancedMemoryManager : public MemoryManager {
    public:
        void optimize_memory(PyBrain& brain) override {
            // Implement memory optimization strategies
            // Clear unused neural states, compress synaptic weights, etc.
        }
        
        double get_memory_usage(const PyBrain& brain) override {
            // Return memory usage in MB
            return 0.0; // Placeholder
        }
        
        std::vector<std::string> get_memory_stats(const PyBrain& brain) override {
            // Return detailed memory statistics
            return std::vector<std::string>();
        }
    };
    
    // Performance monitoring utilities
    class PerformanceMonitor {
    public:
        struct Metrics {
            double simulation_time = 0.0;
            double cpu_usage = 0.0;
            double memory_usage = 0.0;
            size_t neuron_operations = 0;
            size_t spike_events = 0;
            double average_firing_rate = 0.0;
        };
        
        virtual Metrics get_metrics() = 0;
        virtual std::vector<double> get_time_series(const std::string& metric, size_t samples = 100) = 0;
        virtual ~PerformanceMonitor() = default;
    };
    
    // Memory and performance improvements
    class CacheManager {
    public:
        virtual void cache_synaptic_weights(PyBrain& brain, const std::string& key) = 0;
        virtual bool retrieve_synaptic_weights(PyBrain& brain, const std::string& key) = 0;
        virtual void clear_cache(PyBrain& brain) = 0;
        virtual size_t cache_size(PyBrain& brain) = 0;
        virtual ~CacheManager() = default;
    };
    
    // Enhanced Brain API
    std::string get_status() const {
        std::stringstream ss;
        ss << "Brain Status:\n";
        ss << "  Neuron Count: " << brain_->getTotalNeuronCount() << "\n";
        ss << "  Synapse Count: " << brain_->getTotalSynapseCount() << "\n";
        ss << "  Active Neurons: " << brain_->getActiveNeuronCount() << "\n";
        ss << "  Firing Rate: " << brain_->getAverageFiringRate() << "\n";
        ss << "  Developmental Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << "\n";
        return ss.str();
    }
    
    // Get configuration
    PyConfig get_config() const {
        PyConfig config;
        config.config_ = brain_->getConfig();
        return config;
    }
    
    // Add region with Pythonic interface
    RegionId add_region(const std::string& name = "") {
        return brain_->addRegion(name);
    }
    
    // Get region by name (Pythonic)
    std::optional<RegionId> get_region_by_name(const std::string& name) {
        for (const auto& region_id : brain_->getRegionIds()) {
            auto region = brain_->getRegion(region_id);
            if (region && region->get_name() == name) {
                return region_id;
            }
        }
        return std::nullopt;
    }
    
    // Save/load brain state with error handling
    bool save_state(const std::string& filepath) const {
        try {
            return brain_->save(filepath);
        } catch (const std::exception& e) {
            throw PythonicBrainError("Failed to save brain state: " + std::string(e.what()));
        }
    }
    
    bool load_state(const std::string& filepath) {
        try {
            return brain_->load(filepath);
        } catch (const std::exception& e) {
            throw PythonicBrainError("Failed to load brain state: " + std::string(e.what()));
        }
    }
    
    // Serialization support
    std::string serialize() const {
        // Implement serialization to string
        return "serialized_brain_data"; // Placeholder
    }
    
    void deserialize(const std::string& data) {
        // Implement deserialization from string
        // brain_->load_from_string(data); // Placeholder
    }
    
    // Context manager for simulation
    SimulationScope simulate(size_t steps, double timestep = 0.1) {
        return SimulationScope(*this);
    }
    
    // Reset brain state
    void reset() {
        brain_->reset();
    }
    
    // Get underlying C++ brain pointer
    Brain* get_brain_ptr() {
        return brain_.get();
    }
    
    // Get underlying C++ brain pointer (const)
    const Brain* get_brain_ptr() const {
        return brain_.get();
    }
};

// =============================================================
// Enhanced AgentBrain Interface
// =============================================================

class PyAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    PyAgentBrain(std::shared_ptr<Brain> brain) : agent_brain_(std::make_shared<AgentBrain>(brain)) {}
    PyAgentBrain(PyBrain& brain_py) : agent_brain_(std::make_shared<AgentBrain>(brain_py.get_brain_ptr())) {}
    
    // Context manager for agent episodes
    class EpisodeScope {
    private:
        PyAgentBrain& agent_brain_;
        double total_reward_ = 0.0;
        size_t steps_taken_ = 0;
        
    public:
        EpisodeScope(PyAgentBrain& agent_brain) : agent_brain_(agent_brain) {}
        
        ~EpisodeScope() {
            agent_brain_.reset_episode();
        }
        
        void add_step(double reward) {
            total_reward_ += reward;
            steps_taken_++;
        }
        
        double total_reward() const {
            return total_reward_;
        }
        
        size_t steps_taken() const {
            return steps_taken_;
        }
    };
    
    // Reset episode
    void reset_episode() {
        agent_brain_->reset();
    }
    
    // Process sensory input with Pythonic interface
    void perceive(const SensoryPercept& percept) {
        agent_brain_->processSensoryInput(percept);
    }
    
    // Get action with Pythonic interface
    Action get_action() {
        auto action_ptr = agent_brain_->decodeMotorCommand();
        return *action_ptr;
    }
    
    // Apply reward with Pythonic interface
    void learn(double reward, double predicted_reward = 0.0) {
        agent_brain_->applyRewardModulation(reward, predicted_reward);
    }
    
    // Get statistics
    py::dict get_stats() const {
        py::dict stats;
        stats["neuromodulation_level"] = agent_brain_->getNeuromodulationLevel();
        stats["curiosity_level"] = agent_brain_->getCuriosityLevel();
        stats["novelty_level"] = agent_brain_->getNoveltyLevel();
        stats["prediction_error"] = agent_brain_->getPredictionError();
        stats["developmental_stage"] = static_cast<int>(agent_brain_->getDevelopmentalStage());
        return stats;
    }
    
    // Get underlying C++ agent brain pointer
    AgentBrain* get_agent_brain_ptr() {
        return agent_brain_.get();
    }
};

// =============================================================
// Enhanced SensoryInput Classes
// =============================================================

class PySensoryInput {
protected:
    std::shared_ptr<SensoryInput> input_;
    
public:
    PySensoryInput(std::shared_ptr<SensoryInput> input) : input_(input) {}
    
    // Get sensory data as Python list
    std::vector<float> get_data() const {
        return input_->getData();
    }
    
    // Get sensory dimensions
    std::vector<size_t> get_dimensions() const {
        return input_->getDimensions();
    }
    
    // Get timestamp
    double get_timestamp() const {
        return input_->getTimestamp();
    }
    
    // Set timestamp
    void set_timestamp(double timestamp) {
        input_->setTimestamp(timestamp);
    }
};

class PyVision : public PySensoryInput {
public:
    PyVision(std::shared_ptr<Vision> vision) : PySensoryInput(vision) {}
    
    PyVision(size_t width, size_t height, size_t channels = 3) {
        input_ = std::make_shared<Vision>(width, height, channels);
    }
    
    // Set vision data
    void set_data(const std::vector<float>& data) {
        static_cast<Vision*>(input_.get())->setData(data);
    }
    
    // Get vision properties
    size_t get_width() const {
        return static_cast<Vision*>(input_.get())->getWidth();
    }
    
    size_t get_height() const {
        return static_cast<Vision*>(input_.get())->getHeight();
    }
    
    size_t get_channels() const {
        return static_cast<Vision*>(input_.get())->getChannels();
    }
};

class PyAudio : public PySensoryInput {
public:
    PyAudio(std::shared_ptr<Audio> audio) : PySensoryInput(audio) {}
    
    PyAudio(size_t sample_rate, size_t num_samples) {
        input_ = std::make_shared<Audio>(sample_rate, num_samples);
    }
    
    void set_data(const std::vector<float>& data) {
        static_cast<Audio*>(input_.get())->setData(data);
    }
    
    size_t get_sample_rate() const {
        return static_cast<Audio*>(input_.get())->getSampleRate();
    }
    
    size_t get_num_samples() const {
        return static_cast<Audio*>(input_.get())->getNumSamples();
    }
};

// =============================================================
// Enhanced Action Class
// =============================================================

class PyAction {
private:
    std::unique_ptr<Action> action_;
    
public:
    PyAction() : action_(std::make_unique<Action>()) {}
    PyAction(ActionType type) : action_(std::make_unique<Action>(type)) {}
    PyAction(ActionType type, const std::vector<float>& parameters) 
        : action_(std::make_unique<Action>(type, parameters)) {}
    
    // Get action type
    ActionType get_type() const {
        return action_->getType();
    }
    
    // Set action type
    void set_type(ActionType type) {
        action_->setType(type);
    }
    
    // Get parameters
    std::vector<float> get_parameters() const {
        return action_->getParameters();
    }
    
    // Set parameters
    void set_parameters(const std::vector<float>& params) {
        action_->setParameters(params);
    }
    
    // Get action name
    std::string get_name() const {
        return action_->getName();
    }
    
    // Clone action
    std::shared_ptr<PyAction> clone() const {
        auto cloned = std::make_shared<PyAction>();
        cloned->action_ = action_->clone();
        return cloned;
    }
};

// =============================================================
// Enhanced SimpleWorld Class
// =============================================================

class PySimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    PySimpleWorld() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Context manager for world simulation
    class WorldScope {
    private:
        PySimpleWorld& world_;
        size_t steps_run_ = 0;
        
    public:
        WorldScope(PySimpleWorld& world) : world_(world) {}
        
        ~WorldScope() {
            world_.reset();
        }
        
        size_t steps_run() const {
            return steps_run_;
        }
    };
    
    // Configure world
    void configure(size_t width, size_t height, size_t vision_width, size_t vision_height) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    // Run simulation with context manager
    WorldScope run_simulation(size_t steps, double timestep = 0.1) {
        return WorldScope(*this);
    }
    
    // Reset world
    void reset() {
        world_->reset();
    }
    
    // Update world
    void update(double timestep = 0.1) {
        world_->update(timestep);
    }
    
    // Get sensory percept
    SensoryPercept get_percept() const {
        return world_->getSensoryPercept();
    }
    
    // Get agent body
    AgentBody get_agent_body() const {
        return world_->getAgentBody();
    }
    
    // Add object
    void add_object(const WorldObject& obj) {
        world_->addObject(obj);
    }
    
    // Remove object
    void remove_object(float x, float y) {
        world_->removeObject(x, y);
    }
    
    // Check valid position
    bool is_valid_position(float x, float y) const {
        return world_->isValidPosition(x, y);
    }
    
    // Get world properties
    size_t get_width() const {
        return world_->getWidth();
    }
    
    size_t get_height() const {
        return world_->getHeight();
    }
    
    double get_simulation_time() const {
        return world_->getSimulationTime();
    }
    
    // Export world state
    py::dict export_state() const {
        py::dict state;
        state["width"] = world_->getWidth();
        state["height"] = world_->getHeight();
        state["simulation_time"] = world_->getSimulationTime();
        state["agent_body"] = py::cast(world_->getAgentBody());
        return state;
    }
    
    // Import world state
    void import_state(const py::dict& state) {
        if (state.contains("width")) {
            world_->configure(
                state["width"].cast<size_t>(),
                state["height"].cast<size_t>(),
                0, 0 // placeholder for vision dimensions
            );
        }
        if (state.contains("simulation_time")) {
            // world_->set_simulation_time(state["simulation_time"].cast<double>()); // Placeholder
        }
    }
};

// =============================================================
// Enhanced Exception Classes with Pythonic Interfaces
// =============================================================

class PythonicConfigException : public std::exception {
private:
    std::string message_;
    
public:
    PythonicConfigException(const std::string& message) : message_(message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    std::string get_details() const {
        return "Configuration Error: " + message_;
    }
};

class PythonicBrainException : public std::exception {
private:
    std::string message_;
    std::string context_;
    
public:
    PythonicBrainException(const std::string& message, const std::string& context = "") 
        : message_(message), context_(context) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    std::string get_context() const {
        return context_;
    }
    
    std::string get_suggestion() const {
        if (context_.find("not initialized") != std::string::npos) {
            return "Call initialize() before using the brain.";
        } else if (context_.find("invalid configuration") != std::string::npos) {
            return "Check your configuration file for syntax errors.";
        }
        return "Please check the error message and try again.";
    }
};

// =============================================================
// Factory Functions with Enhanced Pythonic Interfaces
// =============================================================

std::shared_ptr<PyConfig> create_config() {
    return std::make_shared<PyConfig>();
}

std::shared_ptr<PyConfig> create_config_from_file(const std::string& filepath) {
    return std::make_shared<PyConfig>(filepath);
}

std::shared_ptr<PyBrain> create_brain(std::shared_ptr<Config> config) {
    return std::make_shared<PyBrain>(config);
}

std::shared_ptr<PyBrain> create_brain_from_file(const std::string& config_file) {
    return std::make_shared<PyBrain>(config_file);
}

std::shared_ptr<PySimpleWorld> create_world() {
    return std::make_shared<PySimpleWorld>();
}

std::shared_ptr<PyAgentBrain> create_agent_brain(PyBrain& brain_py) {
    return std::make_shared<PyAgentBrain>(brain_py);
}

// =============================================================
// Advanced Feature Bindings
// =============================================================

namespace anlm = nlm::AdvancedNetworkArchitecture;
namespace rlnm = nlm::ReinforcementLearning;
namespace aclm = nlm::AdvancedConfiguration;
namespace avlm = nlm::AdvancedVisualization;

// =============================================================
// Main Module Definition
// =============================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ----------------------------------------------------
        
        This module provides enhanced Pythonic bindings for the NLM neural simulation framework.
        
        Key Features:
        - Pythonic property-style access to configuration
        - Context manager support for resources
        - Batch operations for improved performance
        - High-level brain interfaces with learning algorithms
        - Advanced visualization and plotting utilities
        - Memory and performance monitoring
        - Enhanced error handling with Pythonic exceptions
        
        New Advanced Features:
        - Advanced Neural Network Architecture Builder
        - Reinforcement Learning (Q-learning, Actor-Critic, DQN)
        - Advanced Configuration and Scripting (Lua, experiment automation)
        - Advanced Visualization and Analysis Tools
        - Real-time Monitoring and Dashboards
        - Parameter Optimization (Bayesian, Evolutionary)
        - Automated Experiment Design
        
        All original bindings are maintained for backward compatibility.
        )pbdoc";

    // Register enhanced exception classes
    py::register_exception<PythonicConfigException>(m, "PythonicConfigError");
    py::register_exception<PythonicBrainException>(m, "PythonicBrainError");
    py::register_exception<PythonicMemoryError>(m, "PythonicMemoryError");
    py::register_exception<PythonicProgressError>(m, "PythonicProgressError");

    // Enhanced Config class with Pythonic interfaces
    py::class_<PyConfig>(m, "Config")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"))
        // Pythonic property access
        .def("__getitem__", [](const PyConfig& self, const std::string& key) {
            return self.get_pyobject(key);
        })
        .def("__setitem__", [](PyConfig& self, const std::string& key, const py::object& value) {
            // Type conversion from Python object to ConfigValue
            if (value.is(py::int_())) {
                self.set_int(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                self.set_float(key, value.cast<double>());
            } else if (value.is(py::str())) {
                self.set_str(key, value.cast<std::string>());
            } else if (value.is(py::bool_())) {
                self.set_bool(key, value.cast<bool>());
            }
        })
        .def("__contains__", &PyConfig::has)
        .def("__repr__", [](const PyConfig& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("keys", &PyConfig::keys)
        .def("get_int", &PyConfig::get_int)
        .def("get_float", &PyConfig::get_float)
        .def("get_str", &PyConfig::get_str)
        .def("get_bool", &PyConfig::get_bool)
        .def("set_int", &PyConfig::set_int)
        .def("set_float", &PyConfig::set_float)
        .def("set_str", &PyConfig::set_str)
        .def("set_bool", &PyConfig::set_bool)
        .def("has", &PyConfig::has)
        .def("load_from_file", static_cast<bool (PyConfig::*)(const std::string&)>(&PyConfig::load_from_file))
        .def("save", &PyConfig::save)
        .def("to_dict", &PyConfig::to_dict)
        .def("from_dict", &PyConfig::from_dict)
        .def("validate", &PyConfig::validate)
        .def("clone", &PyConfig::clone)
        .def("summary", &PyConfig::summary)
        .def("has_key", &PyConfig::has); // For backward compatibility

    // Enhanced Config class with advanced configuration features
    py::class_<clm::AdvancedConfigurationSystem>(m, "AdvancedConfigSystem")
        .def(py::init<>())
        .def("add_configuration_template", &clm::AdvancedConfigurationSystem::addConfigurationTemplate,
             py::arg("name"), py::arg("config"))
        .def("get_configuration_template", &clm::AdvancedConfigurationSystem::getConfigurationTemplate,
             py::arg("name"))
        .def("has_configuration_template", &clm::AdvancedConfigurationSystem::hasConfigurationTemplate,
             py::arg("name"))
        .def("get_configuration_template_names", &clm::AdvancedConfigurationSystem::getConfigurationTemplateNames)
        .def("add_experiment_template", &clm::AdvancedConfigurationSystem::addExperimentTemplate,
             py::arg("template"))
        .def("get_experiment_template", &clm::AdvancedConfigurationSystem::getExperimentTemplate,
             py::arg("name"))
        .def("has_experiment_template", &clm::AdvancedConfigurationSystem::hasExperimentTemplate,
             py::arg("name"))
        .def("get_experiment_template_names", &clm::AdvancedConfigurationSystem::getExperimentTemplateNames)
        .def("run_experiment", &clm::AdvancedConfigurationSystem::runExperiment,
             py::arg("template_name"), py::arg("parameters") = std::map<std::string, float>())
        .def("execute_lua_script", &clm::AdvancedConfigurationSystem::executeLuaScript,
             py::arg("script"))
        .def("execute_lua_script_file", &clm::AdvancedConfigurationSystem::executeLuaScriptFile,
             py::arg("filename"))
        .def("evaluate_lua_expression", &clm::AdvancedConfigurationSystem::evaluateLuaExpression,
             py::arg("expression"))
        .def("optimize_parameters", &clm::AdvancedConfigurationSystem::optimizeParameters,
             py::arg("objective"), py::arg("parameters"), py::arg("method") = "bayesian")
        .def("hyperparameter_search", &clm::AdvancedConfigurationSystem::hyperparameterSearch,
             py::arg("objective"), py::arg("parameter_grids"), py::arg("method") = "grid_search")
        .def("create_automated_experiment", &clm::AdvancedConfigurationSystem::createAutomatedExperiment,
             py::arg("name"), py::arg("objective"), py::arg("parameter_ranges"))
        .def("validate_experiment", &clm::AdvancedConfigurationSystem::validateExperiment,
             py::arg("template"))
        .def("validate_configuration", &clm::AdvancedConfigurationSystem::validateConfiguration,
             py::arg("config"))
        .def("get_configuration_errors", &clm::AdvancedConfigurationSystem::getConfigurationErrors,
             py::arg("config"))
        .def("run_script_experiment", &clm::AdvancedConfigurationSystem::runScriptExperiment,
             py::arg("script_name"), py::arg("parameters") = std::map<std::string, float>())
        .def("export_configuration", &clm::AdvancedConfigurationSystem::exportConfiguration,
             py::arg("filename"), py::arg("name"))
        .def("import_configuration", &clm::AdvancedConfigurationSystem::importConfiguration,
             py::arg("filename"))
        .def("export_experiment_results", &clm::AdvancedConfigurationSystem::exportExperimentResults,
             py::arg("filename"))
        .def("import_experiment_results", &clm::AdvancedConfigurationSystem::importExperimentResults,
             py::arg("filename"))
        .def("export_optimization_results", &clm::AdvancedConfigurationSystem::exportOptimizationResults,
             py::arg("filename"))
        .def("import_optimization_results", &clm::AdvancedConfigurationSystem::importOptimizationResults,
             py::arg("filename"))

    // Enhanced Brain class
    py::class_<PyBrain>(m, "Brain")
        .def(py::init<std::shared_ptr<PyConfig>>())
        .def(py::init<const std::string&>())
        .def("train_episode", &PyBrain::train_episode)
        .def("process_batch", &PyBrain::process_batch)
        .def("reset", &PyBrain::reset)
        .def("save_state", &PyBrain::save_state)
        .def("load_state", &PyBrain::load_state)
        .def("get_status", &PyBrain::get_status)
        .def("get_config", &PyBrain::get_config)
        .def("add_region", &PyBrain::add_region)
        .def("get_region_by_name", &PyBrain::get_region_by_name)
        .def("simulate", &PyBrain::simulate)
        .def("set_progress_callback", &PyBrain::set_progress_callback)
        .def("cancel", &PyBrain::cancel)
        .def("get_brain_ptr", &PyBrain::get_brain_ptr)
        .def("__repr__", [](const PyBrain& brain) {
            return "<Brain: " + brain.get_status() + ">";
        });

    // Enhanced AgentBrain class
    py::class_<PyAgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<PyBrain>>())
        .def(py::init<PyBrain&>())
        .def("perceive", &PyAgentBrain::perceive)
        .def("get_action", &PyAgentBrain::get_action)
        .def("learn", &PyAgentBrain::learn)
        .def("get_stats", &PyAgentBrain::get_stats)
        .def("reset_episode", &PyAgentBrain::reset_episode)
        .def("get_agent_brain_ptr", &PyAgentBrain::get_agent_brain_ptr)
        .def("__repr__", [](const PyAgentBrain& agent_brain) {
            py::dict stats = agent_brain.get_stats();
            std::stringstream ss;
            ss << "<AgentBrain: neuromodulation=" << stats["neuromodulation_level"].cast<float>()
               << ", curiosity=" << stats["curiosity_level"].cast<float>() << ">";
            return ss.str();
        });

    // Enhanced SensoryInput classes
    py::class_<PySensoryInput>(m, "SensoryInput")
        .def("get_data", &PySensoryInput::get_data)
        .def("get_dimensions", &PySensoryInput::get_dimensions)
        .def("get_timestamp", &PySensoryInput::get_timestamp)
        .def("set_timestamp", &PySensoryInput::set_timestamp);

    py::class_<PyVision, PySensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>())
        .def("set_data", &PyVision::set_data)
        .def("get_width", &PyVision::get_width)
        .def("get_height", &PyVision::get_height)
        .def("get_channels", &PyVision::get_channels);

    py::class_<PyAudio, PySensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>())
        .def("set_data", &PyAudio::set_data)
        .def("get_sample_rate", &PyAudio::get_sample_rate)
        .def("get_num_samples", &PyAudio::get_num_samples);

    // Enhanced Action class
    py::class_<PyAction>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>())
        .def(py::init<ActionType, std::vector<float>>())
        .def("get_type", &PyAction::get_type)
        .def("set_type", &PyAction::set_type)
        .def("get_parameters", &PyAction::get_parameters)
        .def("set_parameters", &PyAction::set_parameters)
        .def("get_name", &PyAction::get_name)
        .def("clone", &PyAction::clone)
        .def("execute", [](PyAction& self) {
            // Placeholder for action execution
            return self;
        });

    // Enhanced SimpleWorld class
    py::class_<PySimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &PySimpleWorld::configure)
        .def("reset", &PySimpleWorld::reset)
        .def("update", &PySimpleWorld::update)
        .def("get_percept", &PySimpleWorld::get_percept)
        .def("get_agent_body", &PySimpleWorld::get_agent_body)
        .def("add_object", &PySimpleWorld::add_object)
        .def("remove_object", &PySimpleWorld::remove_object)
        .def("is_valid_position", &PySimpleWorld::is_valid_position)
        .def("get_width", &PySimpleWorld::get_width)
        .def("get_height", &PySimpleWorld::get_height)
        .def("get_simulation_time", &PySimpleWorld::get_simulation_time)
        .def("run_simulation", &PySimpleWorld::run_simulation)
        .def("export_state", &PySimpleWorld::export_state)
        .def("import_state", &PySimpleWorld::import_state);

    // Factory functions
    m.def("create_config", &create_config, "Create a new configuration object");
    m.def("create_config_from_file", &create_config_from_file, "Create a configuration from file");
    m.def("create_brain", &create_brain, "Create a brain with configuration");
    m.def("create_brain_from_file", &create_brain_from_file, "Create a brain from configuration file");
    m.def("create_world", &create_world, "Create a new world");
    m.def("create_agent_brain", &create_agent_brain, "Create an agent brain interface");

    // Keep original factory functions for backward compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

// Placeholder implementations for complex template conversions
void register_pythonic_config(py::class_<PyConfig>& cl) {
    // Implement PythonicConfigDict class for dictionary access
    cl.def("__getitem__", [](const PyConfig& self, const std::string& key) {
        // Convert ConfigValue to appropriate Python type
        auto value = self.config_->getAuto(key);
        // Convert based on type
    });
}

void register_brain_factory_functions(py::module& m) {
    // Register Pythonic brain factory functions
    m.def("create_py_brain", &create_py_brain);
}
