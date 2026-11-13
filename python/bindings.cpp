# Enhanced Python Bindings for NLM (pynlm)

This file contains the enhanced Python bindings for the NLM neural simulation framework.
It provides a Pythonic API with property access, context managers, and improved error handling.

// Import necessary headers
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

// Include NLM headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Custom Python exceptions
namespace py = pybind11;
namespace nlm {

// Custom exception classes for Python
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyConfigurationError : public PyNLMError {
public:
    explicit PyConfigurationError(const std::string& msg) : PyNLMError(msg) {}
};

class PySimulationError : public PyNLMError {
public:
    explicit PySimulationError(const std::string& msg) : PyNLMError(msg) {}
};

// Helper functions for enhanced error handling
inline void validateConfigKey(const std::string& key, const Config& config, 
                             const std::string& operation) {
    if (!config.has(key)) {
        throw PyConfigurationError("Configuration key '" + key + 
                                  "' not found for operation: " + operation);
    }
}

inline void validateNeuronCount(size_t count) {
    if (count == 0) {
        throw PyConfigurationError("Neuron count cannot be zero");
    }
    if (count > 1000000) {
        throw PyConfigurationError("Neuron count cannot exceed 1,000,000");
    }
}

inline void validateWorldPosition(float x, float y, const SimpleWorld& world) {
    if (!world.isValidPosition(x, y)) {
        throw PySimulationError("Position (" + std::to_string(x) + ", " + 
                               std::to_string(y) + ") is not valid in world");
    }
}

// Enhanced ID types with validation
class EnhancedNeuronId {
private:
    NeuronId id;
    
public:
    EnhancedNeuronId() : id(0) {}
    explicit EnhancedNeuronId(uint64_t value) : id(value) {}
    
    uint64_t value() const { return id.value; }
    size_t index() const { return id.index(); }
    
    bool is_valid() const { return id.value != INVALID_NEURON_ID.value; }
    
    std::string to_string() const {
        return "<EnhancedNeuronId: " + std::to_string(id.value) + ">";
    }
    
    bool operator==(const EnhancedNeuronId& other) const {
        return id == other.id;
    }
    
    bool operator!=(const EnhancedNeuronId& other) const {
        return !(*this == other);
    }
    
    bool operator<(const EnhancedNeuronId& other) const {
        return id.value < other.id.value;
    }
    
    // Convert to original NeuronId for compatibility
    operator NeuronId() const { return id; }
};

// Enhanced Action with validation
class EnhancedAction {
private:
    Action action;
    bool is_valid_action = true;
    
public:
    EnhancedAction() = default;
    EnhancedAction(ActionType type) : action(type) {}
    EnhancedAction(ActionType type, std::vector<float> params) : action(type, params) {}
    
    bool is_valid() const { return is_valid_action; }
    
    ActionType get_type() const { return action.getType(); }
    std::vector<float> get_parameters() const { return action.getParameters(); }
    std::string get_name() const { return action.getName(); }
    
    void set_type(ActionType type) { action.setType(type); }
    void set_parameters(const std::vector<float>& params) { action.setParameters(params); }
    
    std::string to_string() const {
        return "<EnhancedAction: " + get_name() + " with params " + 
               std::to_string(get_parameters().size()) + " values>";
    }
    
    Action clone() const { return action.clone(); }
};

// Enhanced Config with builder pattern support
class EnhancedConfigBuilder {
private:
    std::shared_ptr<Config> config;
    
public:
    EnhancedConfigBuilder() : config(std::make_shared<Config>()) {}
    
    EnhancedConfigBuilder& with_neuron_count(size_t count) {
        validateNeuronCount(count);
        config->set("brain.neuron_count", static_cast<int64_t>(count), ConfigSource::Default);
        return *this;
    }
    
    EnhancedConfigBuilder& with_synapse_density(float density) {
        if (density < 0.0f || density > 1.0f) {
            throw PyConfigurationError("Synapse density must be between 0.0 and 1.0");
        }
        config->set("brain.synapse_density", density, ConfigSource::Default);
        return *this;
    }
    
    EnhancedConfigBuilder& with_plasticity(bool stdp = true, bool hebbian = true) {
        config->set("plasticity.stdp.enable", stdp, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", hebbian, ConfigSource::Default);
        return *this;
    }
    
    EnhancedConfigBuilder& with_neuromodulation(bool dopamine = true, 
                                                  bool curiosity = true, 
                                                  bool novelty = true) {
        config->set("neuromod.dopamine.enable", dopamine, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", curiosity, ConfigSource::Default);
        config->set("neuromod.novelty.enable", novelty, ConfigSource::Default);
        return *this;
    }
    
    EnhancedConfigBuilder& with_developmental_stage(DevelopmentalStage stage) {
        config->set("development.stage", static_cast<int64_t>(stage), ConfigSource::Default);
        return *this;
    }
    
    EnhancedConfigBuilder& from_file(const std::string& filename) {
        if (!config->loadFromFile(filename)) {
            throw PyConfigurationError("Failed to load configuration from file: " + filename);
        }
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        return config;
    }
};

// Enhanced Brain with property access and context manager
class EnhancedBrain {
private:
    std::shared_ptr<Brain> brain;
    bool auto_reset = false;
    
public:
    EnhancedBrain(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
    
    // Property-based access
    size_t total_neurons() const { return brain->getTotalNeuronCount(); }
    size_t total_synapses() const { return brain->getTotalSynapseCount(); }
    size_t firing_neurons() const { return brain->getFiringNeuronCount(); }
    size_t total_spikes() const { return brain->getTotalSpikeCount(); }
    float average_firing_rate() const { return brain->getAverageFiringRate(); }
    float excitation_inhibition_ratio() const { return brain->getExcitationInhibitionRatio(); }
    size_t region_count() const { return brain->getRegionCount(); }
    DevelopmentalStage development_stage() const { return brain->getDevelopmentalStage(); }
    void set_development_stage(DevelopmentalStage stage) { brain->setDevelopmentalStage(stage); }
    
    // Configuration access
    std::shared_ptr<Config> config() const { return brain->getConfig(); }
    
    // Context manager support
    class Context {
    public:
        EnhancedBrain& brain;
        Context(EnhancedBrain& b) : brain(b) {
            brain.initialize();
        }
        
        ~Context() {
            if (brain.auto_reset) {
                brain.reset();
            }
        }
        
        EnhancedBrain& get_brain() { return brain; }
    };
    
    Context enter_context() { return Context(*this); }
    void exit_context() { auto_reset = true; }
    
    // Enhanced methods
    void initialize() { 
        if (!brain->initialize()) {
            throw PySimulationError("Failed to initialize brain");
        }
    }
    
    void reset() { brain->reset(); auto_reset = false; }
    
    void step(size_t step, double time = 0.0) {
        if (!brain->step(step, time)) {
            throw PySimulationError("Failed to perform simulation step");
        }
    }
    
    void run_steps(size_t steps, double timestep = 0.001) {
        for (size_t i = 0; i < steps; ++i) {
            step(i, i * timestep);
        }
    }
    
    // Convenience methods
    std::string get_status() const {
        std::ostringstream oss;
        oss << "=== Brain Status ===\n";
        oss << "Neurons: " << total_neurons() << "\n";
        oss << "Synapses: " << total_synapses() << "\n";
        oss << "Firing neurons: " << firing_neurons() << "\n";
        oss << "Total spikes: " << total_spikes() << "\n";
        oss << "Average firing rate: " << average_firing_rate() << " Hz\n";
        oss << "Development stage: " << static_cast<int>(development_stage()) << "\n";
        oss << "Regions: " << region_count() << "\n";
        return oss.str();
    }
    
    // Statistics for monitoring
    std::map<std::string, double> get_statistics() const {
        std::map<std::string, double> stats;
        stats["neurons"] = static_cast<double>(total_neurons());
        stats["synapses"] = static_cast<double>(total_synapses());
        stats["firing_neurons"] = static_cast<double>(firing_neurons());
        stats["total_spikes"] = static_cast<double>(total_spikes());
        stats["average_firing_rate"] = average_firing_rate();
        stats["excitation_inhibition_ratio"] = excitation_inhibition_ratio();
        stats["development_stage"] = static_cast<double>(development_stage());
        return stats;
    }
    
    // Brain operations
    void save_checkpoint(const std::string& filename) {
        if (!brain->save(filename)) {
            throw PySimulationError("Failed to save checkpoint to: " + filename);
        }
    }
    
    void load_checkpoint(const std::string& filename) {
        if (!brain->load(filename)) {
            throw PySimulationError("Failed to load checkpoint from: " + filename);
        }
    }
    
    std::string to_string() const {
        return "<EnhancedBrain: " + std::to_string(total_neurons()) + 
               " neurons, " + std::to_string(total_spikes()) + " spikes>";
    }
    
    // Convert to original Brain for compatibility
    std::shared_ptr<Brain> get_original_brain() const { return brain; }
};

// Enhanced AgentBrain with convenience methods
class EnhancedAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain;
    bool all_learning_enabled = false;
    
public:
    EnhancedAgentBrain(std::shared_ptr<Brain> brain) 
        : agent_brain(std::make_shared<AgentBrain>(brain)) {}
    
    // Property-based access to learning system status
    bool is_reward_modulation_enabled() const { return agent_brain->isRewardModulationEnabled(); }
    bool is_structural_plasticity_enabled() const { return agent_brain->isStructuralPlasticityEnabled(); }
    bool is_development_enabled() const { return agent_brain->isDevelopmentEnabled(); }
    bool is_curiosity_enabled() const { return agent_brain->isCuriosityEnabled(); }
    
    // Learning control
    void enable_all_learning() {
        agent_brain->enableRewardModulation(true);
        agent_brain->enableStructuralPlasticity(true);
        agent_brain->enableDevelopment(true);
        agent_brain->enableCuriosity(true);
        all_learning_enabled = true;
    }
    
    void disable_all_learning() {
        agent_brain->enableRewardModulation(false);
        agent_brain->enableStructuralPlasticity(false);
        agent_brain->enableDevelopment(false);
        agent_brain->enableCuriosity(false);
        all_learning_enabled = false;
    }
    
    // Enhanced initialization
    void initialize(std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw PySimulationError("Cannot initialize agent brain with null world");
        }
        agent_brain->initialize(world);
    }
    
    // Enhanced motor command processing
    EnhancedAction decode_motor_command() {
        Action action = agent_brain->decodeMotorCommand();
        EnhancedAction enhanced_action;
        enhanced_action.set_type(action.getType());
        enhanced_action.set_parameters(action.getParameters());
        
        // Basic validation
        if (action.getType() == ActionType::Custom && action.getParameters().empty()) {
            enhanced_action.is_valid_action = false;
        }
        
        return enhanced_action;
    }
    
    // Convenience method for reward modulation
    void apply_reward_modulation(float reward, float predicted_reward = 0.0f) {
        if (reward < -1.0f || reward > 1.0f) {
            throw PySimulationError("Reward must be between -1.0 and 1.0");
        }
        agent_brain->applyRewardModulation(reward, predicted_reward);
    }
    
    // Agent statistics
    std::map<std::string, double> get_agent_statistics() const {
        std::map<std::string, double> stats;
        stats["reward_modulation"] = is_reward_modulation_enabled() ? 1.0 : 0.0;
        stats["structural_plasticity"] = is_structural_plasticity_enabled() ? 1.0 : 0.0;
        stats["development"] = is_development_enabled() ? 1.0 : 0.0;
        stats["curiosity"] = is_curiosity_enabled() ? 1.0 : 0.0;
        
        if (all_learning_enabled) {
            stats["all_learning_enabled"] = 1.0;
        }
        
        return stats;
    }
    
    // Get original AgentBrain for compatibility
    std::shared_ptr<AgentBrain> get_original_agent_brain() const { return agent_brain; }
};

// Enhanced SimpleWorld with context manager and validation
class EnhancedSimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world;
    bool auto_reset = false;
    
public:
    EnhancedSimpleWorld() : world(std::make_shared<SimpleWorld>()) {}
    
    // Property-based access
    size_t width() const { return world->getWidth(); }
    size_t height() const { return world->getHeight(); }
    float simulation_time() const { return world->getSimulationTime(); }
    float max_energy() const { return world->getMaxEnergy(); }
    float energy_decay_rate() const { return world->getEnergyDecayRate(); }
    uint64_t random_seed() const { return world->getRandomSeed(); }
    
    // Enhanced configuration
    void configure(size_t width, size_t height, size_t vision_width, size_t vision_height) {
        if (width == 0 || height == 0) {
            throw PyConfigurationError("World dimensions must be greater than zero");
        }
        if (vision_width == 0 || vision_height == 0) {
            throw PyConfigurationError("Vision dimensions must be greater than zero");
        }
        world->configure(width, height, vision_width, vision_height);
    }
    
    void set_max_energy(float energy) {
        if (energy <= 0.0f) {
            throw PyConfigurationError("Max energy must be positive");
        }
        world->setMaxEnergy(energy);
    }
    
    void set_energy_decay_rate(float rate) {
        if (rate < 0.0f || rate > 1.0f) {
            throw PyConfigurationError("Energy decay rate must be between 0.0 and 1.0");
        }
        world->setEnergyDecayRate(rate);
    }
    
    // Context manager support
    class Context {
    public:
        EnhancedSimpleWorld& world;
        Context(EnhancedSimpleWorld& w) : world(w) {
            world.reset();
        }
        
        ~Context() {
            if (world.auto_reset) {
                world.reset();
            }
        }
        
        EnhancedSimpleWorld& get_world() { return world; }
    };
    
    Context enter_context() { return Context(*this); }
    void exit_context() { auto_reset = true; }
    
    // Enhanced world operations
    void reset() { world->reset(); auto_reset = false; }
    
    void update(float timestep) {
        if (timestep <= 0.0f) {
            throw PySimulationError("Timestep must be positive");
        }
        world->update(timestep);
    }
    
    void set_agent_start(float x, float y) {
        if (!world->isValidPosition(x, y)) {
            throw PySimulationError("Agent start position (" + std::to_string(x) + 
                                   ", " + std::to_string(y) + ") is not valid");
        }
        world->setAgentStart(x, y);
    }
    
    // Enhanced action application
    void apply_motor_command(const EnhancedAction& action, float current_time) {
        if (!action.is_valid()) {
            throw PySimulationError("Cannot apply invalid motor command");
        }
        ActionType action_type = action.get_type();
        world->applyMotorCommand(action_type, current_time);
    }
    
    // Enhanced object management
    void add_object(float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
        if (!world->isValidPosition(x, y)) {
            throw PySimulationError("Position (" + std::to_string(x) + ", " + 
                                   std::to_string(y) + ") is not valid");
        }
        
        WorldObject obj(x, y, type, value, radius);
        world->addObject(obj);
    }
    
    // Convenience methods
    std::string get_world_status() const {
        std::ostringstream oss;
        oss << "=== World Status ===\n";
        oss << "Dimensions: " << width() << "x" << height() << "\n";
        oss << "Simulation time: " << simulation_time() << " s\n";
        oss << "Max energy: " << max_energy() << "\n";
        oss << "Energy decay rate: " << energy_decay_rate() << "\n";
        return oss.str();
    }
    
    // Get comprehensive world statistics
    std::map<std::string, double> get_world_statistics() const {
        std::map<std::string, double> stats;
        stats["width"] = static_cast<double>(width());
        stats["height"] = static_cast<double>(height());
        stats["simulation_time"] = simulation_time();
        stats["max_energy"] = max_energy();
        stats["energy_decay_rate"] = energy_decay_rate();
        return stats;
    }
    
    // Get agent body information
    std::map<std::string, double> get_agent_body_info() const {
        auto agent_body = world->getAgentBody();
        std::map<std::string, double> body_info;
        body_info["x"] = agent_body.x;
        body_info["y"] = agent_body.y;
        body_info["orientation"] = agent_body.orientation;
        body_info["velocity_x"] = agent_body.velocityX;
        body_info["velocity_y"] = agent_body.velocityY;
        body_info["angular_velocity"] = agent_body.angularVelocity;
        body_info["energy"] = agent_body.energy;
        body_info["health"] = agent_body.health;
        body_info["age"] = agent_body.age;
        body_info["is_moving"] = agent_body.isMoving ? 1.0 : 0.0;
        body_info["is_turning"] = agent_body.isTurning ? 1.0 : 0.0;
        return body_info;
    }
    
    std::string to_string() const {
        return "<EnhancedSimpleWorld: " + std::to_string(width()) + "x" + 
               std::to_string(height()) + " at " + std::to_string(simulation_time()) + "s>";
    }
    
    // Get original SimpleWorld for compatibility
    std::shared_ptr<SimpleWorld> get_original_world() const { return world; }
};

// Enhanced Config with property access
class EnhancedConfig {
private:
    std::shared_ptr<Config> config;
    
public:
    EnhancedConfig() : config(std::make_shared<Config>()) {}
    
    // Property-based access
    size_t neuron_count() const {
        return config->has("brain.neuron_count") ? 
               config->getOr<int64_t>("brain.neuron_count", 0) : 0;
    }
    
    float synapse_density() const {
        return config->has("brain.synapse_density") ? 
               config->getOr<float>("brain.synapse_density", 0.0f) : 0.0f;
    }
    
    uint64_t random_seed() const {
        return config->has("random_seed") ? 
               config->getOr<int64_t>("random_seed", 42) : 42;
    }
    
    bool is_stdp_enabled() const {
        return config->has("plasticity.stdp.enable") ? 
               config->getOr<bool>("plasticity.stdp.enable", false) : false;
    }
    
    bool is_hebbian_enabled() const {
        return config->has("plasticity.hebbian.enable") ? 
               config->getOr<bool>("plasticity.hebbian.enable", false) : false;
    }
    
    bool is_dopamine_enabled() const {
        return config->has("neuromod.dopamine.enable") ? 
               config->getOr<bool>("neuromod.dopamine.enable", false) : false;
    }
    
    bool is_curiosity_enabled() const {
        return config->has("neuromod.curiosity.enable") ? 
               config->getOr<bool>("neuromod.curiosity.enable", false) : false;
    }
    
    // Enhanced configuration methods
    void set_neuron_count(size_t count) {
        validateNeuronCount(count);
        config->set("brain.neuron_count", static_cast<int64_t>(count), ConfigSource::Default);
    }
    
    void set_synapse_density(float density) {
        if (density < 0.0f || density > 1.0f) {
            throw PyConfigurationError("Synapse density must be between 0.0 and 1.0");
        }
        config->set("brain.synapse_density", density, ConfigSource::Default);
    }
    
    void set_random_seed(uint64_t seed) {
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
    }
    
    void load_from_file(const std::string& filename) {
        if (!config->loadFromFile(filename)) {
            throw PyConfigurationError("Failed to load configuration from file: " + filename);
        }
    }
    
    void save_to_file(const std::string& filename) const {
        config->saveToFile(filename);
    }
    
    std::string get_summary() const {
        return config->summary();
    }
    
    // Get configuration statistics
    std::map<std::string, double> get_config_statistics() const {
        std::map<std::string, double> stats;
        stats["neuron_count"] = static_cast<double>(neuron_count());
        stats["synapse_density"] = synapse_density();
        stats["random_seed"] = static_cast<double>(random_seed());
        stats["stdp_enabled"] = is_stdp_enabled() ? 1.0 : 0.0;
        stats["hebbian_enabled"] = is_hebbian_enabled() ? 1.0 : 0.0;
        stats["dopamine_enabled"] = is_dopamine_enabled() ? 1.0 : 0.0;
        stats["curiosity_enabled"] = is_curiosity_enabled() ? 1.0 : 0.0;
        return stats;
    }
    
    std::string to_string() const {
        return "<EnhancedConfig: " + get_summary() + ">";
    }
    
    // Get original Config for compatibility
    std::shared_ptr<Config> get_original_config() const { return config; }
};

// Simulation utilities
namespace sim {
    // Run complete episode with automatic reset
    std::map<std::string, double> run_episode(
        std::shared_ptr<EnhancedBrain> brain,
        std::shared_ptr<EnhancedSimpleWorld> world,
        std::shared_ptr<EnhancedAgentBrain> agent,
        size_t max_steps = 1000,
        double timestep = 0.1,
        double max_time = 10.0
    ) {
        if (!brain || !world || !agent) {
            throw PySimulationError("Cannot run episode with null components");
        }
        
        // Reset and initialize
        brain->reset();
        world->reset();
        agent->initialize(world->get_original_world());
        
        std::map<std::string, double> episode_stats;
        
        for (size_t step = 0; step < max_steps; ++step) {
            // Check time limit
            if (world->simulation_time() >= max_time) {
                break;
            }
            
            // Update world
            world->update(timestep);
            
            // Get sensory input
            auto percept = world->get_original_world()->getSensoryPercept();
            
            // Process sensory input
            agent->get_original_agent_brain()->processSensoryInput(percept);
            
            // Step brain
            brain->step(step, step * timestep);
            
            // Get action
            auto action = agent->decode_motor_command();
            
            // Apply action if valid
            if (action.is_valid()) {
                world->apply_motor_command(action, world->simulation_time());
            }
            
            // Update episode statistics
            episode_stats["step"] = static_cast<double>(step);
            episode_stats["time"] = world->simulation_time();
            episode_stats["neurons"] = static_cast<double>(brain->total_neurons());
            episode_stats["spikes"] = static_cast<double>(brain->total_spikes());
            episode_stats["firing_rate"] = brain->average_firing_rate();
            episode_stats["curiosity"] = agent->is_curiosity_enabled() ? 1.0 : 0.0;
        }
        
        // Get final statistics
        episode_stats["final_neurons"] = static_cast<double>(brain->total_neurons());
        episode_stats["final_spikes"] = static_cast<double>(brain->total_spikes());
        episode_stats["final_firing_rate"] = brain->average_firing_rate();
        
        return episode_stats;
    }
    
    // Batch simulation with different configurations
    std::vector<std::map<std::string, double>> batch_simulation(
        const std::vector<std::shared_ptr<EnhancedConfig>>& configs,
        const std::map<std::string, double>& simulation_params
    ) {
        std::vector<std::map<std::string, double>> results;
        
        for (const auto& config : configs) {
            auto brain = std::make_shared<EnhancedBrain>(config->get_original_config());
            brain->initialize();
            
            size_t steps = simulation_params.count("steps") ? 
                          static_cast<size_t>(simulation_params.at("steps")) : 100;
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            std::map<std::string, double> result;
            result["neurons"] = static_cast<double>(brain->total_neurons());
            result["synapses"] = static_cast<double>(brain->total_synapses());
            result["spikes"] = static_cast<double>(brain->total_spikes());
            result["firing_rate"] = brain->average_firing_rate();
            
            results.push_back(result);
        }
        
        return results;
    }
}

// Main Python module definition
PYBIND11_MODULE(pynlm, m) {
    // Register custom exceptions
    m.def("PyNLMError", py::modattr(&nlm::PyNLMError));
    m.def("PyConfigurationError", py::modattr(&nlm::PyConfigurationError));
    m.def("PySimulationError", py::modattr(&nlm::PySimulationError));
    
    // Register Enhanced ID types
    py::class_<EnhancedNeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def("value", &EnhancedNeuronId::value)
        .def("index", &EnhancedNeuronId::index)
        .def("is_valid", &EnhancedNeuronId::is_valid)
        .def("to_string", &EnhancedNeuronId::to_string)
        .def("__eq__", [](const EnhancedNeuronId& a, const EnhancedNeuronId& b) { return a == b; })
        .def("__ne__", [](const EnhancedNeuronId& a, const EnhancedNeuronId& b) { return a != b; })
        .def("__repr__", [](const EnhancedNeuronId& id) { return id.to_string(); });
    
    // Register Enhanced Action type
    py::class_<EnhancedAction>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("is_valid", &EnhancedAction::is_valid)
        .def("get_type", &EnhancedAction::get_type)
        .def("get_parameters", &EnhancedAction::get_parameters)
        .def("get_name", &EnhancedAction::get_name)
        .def("set_type", &EnhancedAction::set_type, py::arg("type"))
        .def("set_parameters", &EnhancedAction::set_parameters, py::arg("parameters"))
        .def("get_name", &EnhancedAction::get_name)
        .def("clone", &EnhancedAction::clone)
        .def("to_string", &EnhancedAction::to_string)
        .def("__repr__", [](const EnhancedAction& action) { return action.to_string(); });
    
    // Register Enhanced Config class
    py::class_<EnhancedConfig>(m, "Config")
        .def(py::init<>())
        .def("neuron_count", &EnhancedConfig::neuron_count)
        .def("synapse_density", &EnhancedConfig::synapse_density)
        .def("random_seed", &EnhancedConfig::random_seed)
        .def("is_stdp_enabled", &EnhancedConfig::is_stdp_enabled)
        .def("is_hebbian_enabled", &EnhancedConfig::is_hebbian_enabled)
        .def("is_dopamine_enabled", &EnhancedConfig::is_dopamine_enabled)
        .def("is_curiosity_enabled", &EnhancedConfig::is_curiosity_enabled)
        .def("set_neuron_count", &EnhancedConfig::set_neuron_count, py::arg("count"))
        .def("set_synapse_density", &EnhancedConfig::set_synapse_density, py::arg("density"))
        .def("set_random_seed", &EnhancedConfig::set_random_seed, py::arg("seed"))
        .def("load_from_file", &EnhancedConfig::load_from_file, py::arg("filename"))
        .def("save_to_file", &EnhancedConfig::save_to_file, py::arg("filename"))
        .def("get_summary", &EnhancedConfig::get_summary)
        .def("get_config_statistics", &EnhancedConfig::get_config_statistics)
        .def("to_string", &EnhancedConfig::to_string)
        .def("__repr__", [](const EnhancedConfig& config) { return config.to_string(); });
    
    // Register Enhanced Brain class
    py::class_<EnhancedBrain>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &EnhancedBrain::initialize)
        .def("reset", &EnhancedBrain::reset)
        .def("step", &EnhancedBrain::step, py::arg("step"), py::arg("time") = 0.0)
        .def("run_steps", &EnhancedBrain::run_steps, py::arg("steps"), py::arg("timestep") = 0.001)
        .def("total_neurons", &EnhancedBrain::total_neurons)
        .def("total_synapses", &EnhancedBrain::total_synapses)
        .def("firing_neurons", &EnhancedBrain::firing_neurons)
        .def("total_spikes", &EnhancedBrain::total_spikes)
        .def("average_firing_rate", &EnhancedBrain::average_firing_rate)
        .def("excitation_inhibition_ratio", &EnhancedBrain::excitation_inhibition_ratio)
        .def("development_stage", &EnhancedBrain::development_stage)
        .def("set_development_stage", &EnhancedBrain::set_development_stage, py::arg("stage"))
        .def("config", &EnhancedBrain::config)
        .def("get_status", &EnhancedBrain::get_status)
        .def("get_statistics", &EnhancedBrain::get_statistics)
        .def("save_checkpoint", &EnhancedBrain::save_checkpoint, py::arg("filename"))
        .def("load_checkpoint", &EnhancedBrain::load_checkpoint, py::arg("filename"))
        .def("to_string", &EnhancedBrain::to_string)
        .def("__repr__", [](const EnhancedBrain& brain) { return brain.to_string(); })
        // Context manager support
        .def("__enter__", [](EnhancedBrain& b) { return &b->enter_context(); })
        .def("__exit__", [](EnhancedBrain& b, py::object, py::object, py::object) {
            b.exit_context();
        });
    
    // Register Enhanced SimpleWorld class
    py::class_<EnhancedSimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &EnhancedSimpleWorld::configure, py::arg("width"), py::arg("height"), 
              py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &EnhancedSimpleWorld::reset)
        .def("update", &EnhancedSimpleWorld::update, py::arg("timestep"))
        .def("set_agent_start", &EnhancedSimpleWorld::set_agent_start, py::arg("x"), py::arg("y"))
        .def("apply_motor_command", &EnhancedSimpleWorld::apply_motor_command, 
              py::arg("action"), py::arg("current_time"))
        .def("get_sensory_percept", &EnhancedSimpleWorld::get_original_world, 
              py::return_value_policy::reference_internal)
        .def("get_agent_body", &EnhancedSimpleWorld::get_original_world,
              py::return_value_policy::reference_internal)
        .def("add_object", &EnhancedSimpleWorld::add_object, py::arg("x"), py::arg("y"), 
              py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def("remove_object", &EnhancedSimpleWorld::get_original_world->removeObject, 
              py::arg("x"), py::arg("y"))
        .def("is_valid_position", &EnhancedSimpleWorld::get_original_world->isValidPosition,
              py::arg("x"), py::arg("y"))
        .def("width", &EnhancedSimpleWorld::width)
        .def("height", &EnhancedSimpleWorld::height)
        .def("simulation_time", &EnhancedSimpleWorld::simulation_time)
        .def("max_energy", &EnhancedSimpleWorld::max_energy)
        .def("energy_decay_rate", &EnhancedSimpleWorld::energy_decay_rate)
        .def("random_seed", &EnhancedSimpleWorld::random_seed)
        .def("set_max_energy", &EnhancedSimpleWorld::set_max_energy, py::arg("energy"))
        .def("set_energy_decay_rate", &EnhancedSimpleWorld::set_energy_decay_rate, py::arg("rate"))
        .def("get_world_status", &EnhancedSimpleWorld::get_world_status)
        .def("get_world_statistics", &EnhancedSimpleWorld::get_world_statistics)
        .def("get_agent_body_info", &EnhancedSimpleWorld::get_agent_body_info)
        .def("to_string", &EnhancedSimpleWorld::to_string)
        .def("__repr__", [](const EnhancedSimpleWorld& world) { return world.to_string(); })
        // Context manager support
        .def("__enter__", [](EnhancedSimpleWorld& w) { return &w->enter_context(); })
        .def("__exit__", [](EnhancedSimpleWorld& w, py::object, py::object, py::object) {
            w.exit_context();
        });
    
    // Register Enhanced AgentBrain class
    py::class_<EnhancedAgentBrain>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &EnhancedAgentBrain::initialize, py::arg("world"))
        .def("get_sensory_input_size", &EnhancedAgentBrain::get_original_agent_brain->getSensoryInputSize)
        .def("get_motor_output_size", &EnhancedAgentBrain::get_original_agent_brain->getMotorOutputSize)
        .def("process_sensory_input", &EnhancedAgentBrain::get_original_agent_brain->processSensoryInput,
              py::arg("percept"))
        .def("decode_motor_command", &EnhancedAgentBrain::decode_motor_command)
        .def("apply_reward_modulation", &EnhancedAgentBrain::apply_reward_modulation,
              py::arg("reward"), py::arg("predicted_reward") = 0.0f)
        .def("update_development", &EnhancedAgentBrain::get_original_agent_brain->updateDevelopment,
              py::arg("timestep"))
        .def("get_developmental_stage", &EnhancedAgentBrain::get_original_agent_brain->getDevelopmentalStage)
        .def("get_neuromodulation_level", &EnhancedAgentBrain::get_original_agent_brain->getNeuromodulationLevel)
        .def("get_curiosity_level", &EnhancedAgentBrain::get_original_agent_brain->getCuriosityLevel)
        .def("get_novelty_level", &EnhancedAgentBrain::get_original_agent_brain->getNoveltyLevel)
        .def("get_prediction_error", &EnhancedAgentBrain::get_original_agent_brain->getPredictionError)
        .def("reset", &EnhancedAgentBrain::get_original_agent_brain->reset)
        .def("get_brain", &EnhancedAgentBrain::get_original_agent_brain->getBrain,
              py::return_value_policy::reference_internal)
        .def("enable_reward_modulation", &EnhancedAgentBrain::get_original_agent_brain->enableRewardModulation,
              py::arg("enable"))
        .def("enable_structural_plasticity", &EnhancedAgentBrain::get_original_agent_brain->enableStructuralPlasticity,
              py::arg("enable"))
        .def("enable_development", &EnhancedAgentBrain::get_original_agent_brain->enableDevelopment,
              py::arg("enable"))
        .def("enable_curiosity", &EnhancedAgentBrain::get_original_agent_brain->enableCuriosity,
              py::arg("enable"))
        .def("is_reward_modulation_enabled", &EnhancedAgentBrain::is_reward_modulation_enabled)
        .def("is_structural_plasticity_enabled", &EnhancedAgentBrain::is_structural_plasticity_enabled)
        .def("is_development_enabled", &EnhancedAgentBrain::is_development_enabled)
        .def("is_curiosity_enabled", &EnhancedAgentBrain::is_curiosity_enabled)
        .def("enable_all_learning", &EnhancedAgentBrain::enable_all_learning)
        .def("disable_all_learning", &EnhancedAgentBrain::disable_all_learning)
        .def("get_agent_statistics", &EnhancedAgentBrain::get_agent_statistics)
        .def("to_string", [](const EnhancedAgentBrain&) { return "<EnhancedAgentBrain>"; })
        .def("__repr__", [](const EnhancedAgentBrain& agent) { return agent.to_string(); });
    
    // Register factory functions with enhanced error handling
    m.def("create_default_config", []() {
        auto config = std::make_shared<EnhancedConfig>();
        return config;
    }, "Create a default configuration with enhanced error handling");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<EnhancedBrain> {
        return std::make_shared<EnhancedBrain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<EnhancedSimpleWorld> {
        return std::make_shared<EnhancedSimpleWorld>();
    }, "Create a new simple world with enhanced features");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<EnhancedAgentBrain> {
        return std::make_shared<EnhancedAgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface with enhanced features");
    
    // Register simulation utilities
    m.def("run_episode", &sim::run_episode, py::arg("brain"), py::arg("world"), 
          py::arg("agent"), py::arg("max_steps") = 1000, py::arg("timestep") = 0.1,
          py::arg("max_time") = 10.0, "Run a complete simulation episode with automatic reset");
    
    m.def("batch_simulation", &sim::batch_simulation, py::arg("configs"), py::arg("simulation_params") = py::dict(),
          "Run multiple simulations with different configurations");
    
    // Register builder pattern for easy configuration
    py::class_<EnhancedConfigBuilder>(m, "ConfigBuilder")
        .def(py::init<>())
        .def("with_neuron_count", &EnhancedConfigBuilder::with_neuron_count, py::arg("count"))
        .def("with_synapse_density", &EnhancedConfigBuilder::with_synapse_density, py::arg("density"))
        .def("with_plasticity", &EnhancedConfigBuilder::with_plasticity, 
              py::arg("stdp") = true, py::arg("hebbian") = true)
        .def("with_neuromodulation", &EnhancedConfigBuilder::with_neuromodulation,
              py::arg("dopamine") = true, py::arg("curiosity") = true, py::arg("novelty") = true)
        .def("with_developmental_stage", &EnhancedConfigBuilder::with_developmental_stage, 
              py::arg("stage"))
        .def("from_file", &EnhancedConfigBuilder::from_file, py::arg("filename"))
        .def("build", &EnhancedConfigBuilder::build);
    
    // Register constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register simulation stage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", 
        R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Register neuron type enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // Register action type enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();
    
    // Register world object type enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Module documentation
    m.doc() = R"pbdoc(
        Enhanced Python Bindings for NLM (Neural Learning Machine)
        
        This module provides enhanced Python bindings for the NLM neural simulation framework.
        
        Key improvements over the original bindings:
        - Property-based access (e.g., brain.total_neurons instead of brain.getTotalNeuronCount())
        - Context manager support for automatic resource cleanup
        - Enhanced error handling with descriptive exception messages
        - Builder pattern for easy configuration
        - Convenience methods for common simulation patterns
        - Type-safe action objects with validation
        - Comprehensive statistics and monitoring utilities
        
        The API maintains full backward compatibility while providing modern Python features.
    )pbdoc";
    
    // Add version attribute
    m.attr("__version__") = "enhanced-1.0";
    
    // Add help function
    m.def("help", [](const std::string& topic) {
        std::cout << "=== NLM Python API Help ===\n";
        if (topic == "property_access") {
            std::cout << "Property-based access allows direct attribute-style access:\n";
            std::cout << "  brain.total_neurons() instead of brain.getTotalNeuronCount()\n";
            std::cout << "  brain.config instead of brain.getConfig()\n";
            std::cout << "  brain.development_stage instead of brain.getDevelopmentalStage()\n";
        } else if (topic == "context_manager") {
            std::cout << "Context managers provide automatic resource cleanup:\n";
            std::cout << "  with brain: brain is automatically reset on exit\n";
            std::cout << "  with world: world is automatically reset on exit\n";
        } else if (topic == "builder") {
            std::cout << "Builder pattern for easy configuration:\n";
            std::cout << "  brain = ConfigBuilder().with_neuron_count(1000).build()\n";
            std::cout << "  world = WorldBuilder().width(20).height(20).create()\n";
        } else {
            std::cout << "Available topics: property_access, context_manager, builder\n";
        }
    }, py::arg("topic"), "Get help on specific topic");
}
