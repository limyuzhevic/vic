#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Python-style wrapper for Config with enhanced features
class PythonConfig {
private:
    std::shared_ptr<Config> config;
    std::string name;
    
public:
    PythonConfig() : config(std::make_shared<Config>()), name("unnamed") {}
    PythonConfig(const std::string& n) : config(std::make_shared<Config>()), name(n) {}
    
    void set_name(const std::string& n) { name = n; }
    std::string get_name() const { return name; }
    
    // Configuration methods with Pythonic API
    void set(const std::string& key, int value, const std::string& source = "runtime") {
        ConfigSource src = ConfigSource::Runtime;
        if (source == "file") src = ConfigSource::File;
        else if (source == "command_line") src = ConfigSource::CommandLine;
        config->set(key, value, src);
    }
    
    void set(const std::string& key, double value, const std::string& source = "runtime") {
        ConfigSource src = ConfigSource::Runtime;
        if (source == "file") src = ConfigSource::File;
        else if (source == "command_line") src = ConfigSource::CommandLine;
        config->set(key, value, src);
    }
    
    void set(const std::string& key, const std::string& value, const std::string& source = "runtime") {
        ConfigSource src = ConfigSource::Runtime;
        if (source == "file") src = ConfigSource::File;
        else if (source == "command_line") src = ConfigSource::CommandLine;
        config->set(key, value, src);
    }
    
    void set(const std::string& key, bool value, const std::string& source = "runtime") {
        ConfigSource src = ConfigSource::Runtime;
        if (source == "file") src = ConfigSource::File;
        else if (source == "command_line") src = ConfigSource::CommandLine;
        config->set(key, value, src);
    }
    
    // Getters with type conversion
    int get_int(const std::string& key, int default_value = 0) {
        auto val = config->get<int>(key);
        return val.has_value() ? val.value() : default_value;
    }
    
    double get_double(const std::string& key, double default_value = 0.0) {
        auto val = config->get<double>(key);
        return val.has_value() ? val.value() : default_value;
    }
    
    std::string get_string(const std::string& key, const std::string& default_value = "") {
        auto val = config->get<std::string>(key);
        return val.has_value() ? val.value() : default_value;
    }
    
    bool get_bool(const std::string& key, bool default_value = false) {
        auto val = config->get<bool>(key);
        return val.has_value() ? val.value() : default_value;
    }
    
    // Environment variable support
    bool load_from_env(const std::string& prefix = "NLM_") {
        bool success = false;
        for (const auto& entry : *config.get()) {
            // Skip internal entries
            if (entry.key.find('.') != std::string::npos) continue;
            
            std::string env_key = prefix + entry.key;
            if (std::getenv(env_key.c_str())) {
                std::string env_value = std::getenv(env_key.c_str());
                set(entry.key, env_value, "environment");
                success = true;
            }
        }
        return success;
    }
    
    // Load from JSON file
    bool load_from_json(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        // For now, use simple key=value format
        // In a full implementation, this would parse JSON
        return config->loadFromFile(filepath);
    }
    
    // Save to JSON file
    bool save_to_json(const std::string& filepath) const {
        return config->saveToFile(filepath);
    }
    
    // Configuration validation
    bool validate() {
        for (const auto& entry : *config.get()) {
            std::string key = entry.key;
            auto value = entry.value;
            
            // Basic type validation based on key patterns
            if (key.find("rate") != std::string::npos || key.find("_rate") != std::string::npos) {
                if (std::holds_alternative<double>(value)) {
                    double val = std::get<double>(value);
                    if (val < 0.0 || val > 1.0) {
                        std::cerr << "Warning: Configuration parameter " << key 
                                  << " should be between 0.0 and 1.0, got " << val << std::endl;
                    }
                }
            }
            
            if (key.find("count") != std::string::npos || key.find("_count") != std::string::npos) {
                if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
                    int64_t val = std::holds_alternative<int>(value) ? 
                        std::get<int>(value) : std::get<int64_t>(value);
                    if (val < 0) {
                        std::cerr << "Warning: Configuration parameter " << key 
                                  << " should be non-negative, got " << val << std::endl;
                    }
                }
            }
        }
        return true;
    }
    
    // Configuration summary
    std::string summary() const {
        return config->summary();
    }
    
    // Get all configuration keys
    std::vector<std::string> get_keys() const {
        return config->getKeys();
    }
    
    // Merge with another configuration
    void merge(const PythonConfig& other) {
        for (const auto& key : other.get_keys()) {
            if (other.has(key)) {
                auto val = other.get_value(key);
                config->set(key, val, ConfigSource::Runtime);
            }
        }
    }
    
    // Get underlying config
    std::shared_ptr<Config> get_config() const { return config; }
    
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    ConfigValue get_value(const std::string& key) const {
        // Return the raw ConfigValue
        for (const auto& entry : *config.get()) {
            if (entry.key == key) {
                return entry.value;
            }
        }
        return ConfigValue();
    }
    
    // Remove configuration entry
    void remove(const std::string& key) {
        config->remove(key);
    }
    
    // Clear all configuration
    void clear() {
        config->clear();
    }
    
    // Create default configuration
    static PythonConfig create_default() {
        PythonConfig cfg("default");
        cfg.set("random_seed", 42);
        cfg.set("simulation_timestep", 0.001);
        cfg.set("neuron_count", 500);
        cfg.set("region_count", 1);
        cfg.set("connection_probability", 0.15f);
        cfg.set("stdp_ltp_weight", 0.02f);
        cfg.set("stdp_ltd_weight", 0.015f);
        cfg.set("stdp_time_constant", 20.0f);
        cfg.set("synaptogenesis_rate", 0.0001f);
        cfg.set("pruning_rate", 0.00001f);
        cfg.set("log_level", std::string("INFO"));
        return cfg;
    }
    
    // Create empty configuration
    static PythonConfig create_empty() {
        return PythonConfig("empty");
    }
    
    // Copy constructor
    PythonConfig(const PythonConfig& other) : config(other.config), name(other.name) {}
    
    PythonConfig& operator=(const PythonConfig& other) {
        if (this != &other) {
            config = other.config;
            name = other.name;
        }
        return *this;
    }
};

// Python wrapper for Brain with enhanced functionality
class PythonBrain {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<PythonConfig> config;
    
public:
    PythonBrain() : brain(std::make_shared<Brain>(std::make_shared<Config>())),
                   config(std::make_shared<PythonConfig>("brain_config")) {}
    
    PythonBrain(std::shared_ptr<PythonConfig> cfg) : 
        brain(std::make_shared<Brain>(cfg->get_config())),
        config(cfg) {}
    
    // Initialize the brain
    bool initialize() {
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }
        return true;
    }
    
    // Get configuration
    std::shared_ptr<PythonConfig> get_config() const { return config; }
    
    // Brain simulation step
    void step(int step, double time = 0.0) {
        brain->step(static_cast<SimulationStep>(step), time);
    }
    
    // Get various statistics
    int get_total_neuron_count() const { return static_cast<int>(brain->getTotalNeuronCount()); }
    int get_total_synapse_count() const { return static_cast<int>(brain->getTotalSynapseCount()); }
    int get_active_neuron_count() const { return static_cast<int>(brain->getActiveNeuronCount()); }
    int get_firing_neuron_count() const { return static_cast<int>(brain->getFiringNeuronCount()); }
    double get_average_firing_rate() const { return brain->getAverageFiringRate(); }
    double get_excitation_inhibition_ratio() const { return brain->getExcitationInhibitionRatio(); }
    int get_total_spike_count() const { return static_cast<int>(brain->getTotalSpikeCount()); }
    
    // Sensory input processing
    void receive_sensory_input(std::shared_ptr<SensoryInput> input) {
        brain->receiveSensoryInput(*input.get());
    }
    
    // Motor action production
    std::shared_ptr<Action> produce_action() {
        return brain->produceAction();
    }
    
    // Reset brain
    void reset() {
        brain->reset();
        if (config) {
            brain->initialize();
        }
    }
    
    // Save brain state
    bool save(const std::string& filepath) const {
        return brain->save(filepath);
    }
    
    // Load brain state
    bool load(const std::string& filepath) {
        if (brain->load(filepath)) {
            reset();
            return true;
        }
        return false;
    }
    
    // Get developmental stage
    std::string get_developmental_stage() const {
        return "DevelopmentStage_" + std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
    }
    
    // Set developmental stage
    void set_developmental_stage(const std::string& stage) {
        // Parse stage string (simplified)
        int stage_value = std::stoi(stage.substr(18)); // "DevelopmentStage_" prefix
        brain->setDevelopmentalStage(static_cast<DevelopmentalStage>(stage_value));
    }
    
    // Get underlying brain
    std::shared_ptr<Brain> get_brain() const { return brain; }
    
    // Apply neuromodulation
    void apply_neuromodulation(const Neuromodulator& signal) {
        brain->applyNeuromodulation(signal);
    }
    
    // Update plasticity
    void update_plasticity() {
        brain->updatePlasticity();
    }
    
    // Apply development
    void develop() {
        brain->develop();
    }
    
    // Inject current
    void inject_current(NeuronId neuron, double current) {
        brain->injectCurrent(neuron, static_cast<MembranePotential>(current));
    }
    
    // Inject current to neurons of a specific type
    void inject_current_to_neurons(NeuronType type, double current) {
        brain->injectCurrentToNeurons(type, static_cast<MembranePotential>(current));
    }
    
    // Get region
    std::shared_ptr<NeuralRegion> get_region(RegionId id) {
        auto region = brain->getRegion(id);
        return std::shared_ptr<NeuralRegion>(region, [](NeuralRegion*){});
    }
    
    // Add region
    RegionId add_region(const std::string& name = "") {
        return brain->addRegion(name);
    }
    
    // Get all regions
    std::vector<RegionId> get_region_ids() const {
        return brain->getRegionIds();
    }
    
    // Get status for logging
    std::string get_status() const {
        std::ostringstream oss;
        oss << "=== Brain Status ===\n";
        oss << "Neurons: " << get_total_neuron_count() << "\n";
        oss << "Synapses: " << get_total_synapse_count() << "\n";
        oss << "Active Neurons: " << get_active_neuron_count() << "\n";
        oss << "Firing Neurons: " << get_firing_neuron_count() << "\n";
        oss << "Total Spikes: " << get_total_spike_count() << "\n";
        oss << "Average Firing Rate: " << get_average_firing_rate() << "\n";
        oss << "E/I Ratio: " << get_excitation_inhibition_ratio() << "\n";
        oss << "Developmental Stage: " << get_developmental_stage() << "\n";
        return oss.str();
    }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double simulation_time;
        double processing_time;
        double memory_usage;
        int steps_completed;
        int spikes_generated;
    };
    
    PerformanceMetrics get_performance_metrics(double start_time, double end_time) {
        PerformanceMetrics metrics;
        metrics.simulation_time = end_time - start_time;
        metrics.processing_time = metrics.simulation_time * 0.9; // Approximate
        metrics.memory_usage = get_total_neuron_count() * 0.1; // Rough estimate
        metrics.steps_completed = get_total_spike_count(); // Approximate
        metrics.spikes_generated = get_total_spike_count();
        return metrics;
    }
};

// Python wrapper for SimpleWorld
class PythonSimpleWorld {
private:
    std::shared_ptr<SimpleWorld> world;
    
public:
    PythonSimpleWorld() : world(std::make_shared<SimpleWorld>()) {}
    
    // Configure world
    void configure(int width, int height, int vision_width = 5, int vision_height = 5) {
        world->configure(width, height, vision_width, vision_height);
    }
    
    // Reset world
    void reset() {
        world->reset();
    }
    
    // Set agent start position
    void set_agent_start(int x, int y) {
        world->setAgentStart(x, y);
    }
    
    // Update world
    void update(double timestep = 0.001) {
        world->update(timestep);
    }
    
    // Apply motor command
    void apply_motor_command(MotorCommand cmd, double currentTime = 0.0) {
        world->applyMotorCommand(cmd, currentTime);
    }
    
    // Get sensory percept
    std::shared_ptr<SensoryPercept> get_sensory_percept() {
        return world->getSensoryPercept();
    }
    
    // Get agent body
    std::shared_ptr<AgentBody> get_agent_body() {
        return world->getAgentBody();
    }
    
    // Add world object
    void add_object(WorldObject obj) {
        world->addObject(obj);
    }
    
    // Remove world object
    void remove_object(float x, float y) {
        world->removeObject(x, y);
    }
    
    // Check if position is valid
    bool is_valid_position(float x, float y) {
        return world->isValidPosition(x, y);
    }
    
    // Get world dimensions
    std::pair<int, int> get_dimensions() const {
        return std::make_pair(world->getWidth(), world->getHeight());
    }
    
    // Get underlying world
    std::shared_ptr<SimpleWorld> get_world() const { return world; }
    
    // Get simulation time
    double get_simulation_time() const {
        return world->getSimulationTime();
    }
    
    // Set random seed
    void set_random_seed(int seed) {
        world->setRandomSeed(seed);
    }
    
    // Get random seed
    int get_random_seed() const {
        return world->getRandomSeed();
    }
    
    // Get max energy
    double get_max_energy() const {
        return world->getMaxEnergy();
    }
    
    // Set max energy
    void set_max_energy(double energy) {
        world->setMaxEnergy(energy);
    }
    
    // Get energy decay rate
    double get_energy_decay_rate() const {
        return world->getEnergyDecayRate();
    }
    
    // Set energy decay rate
    void set_energy_decay_rate(double rate) {
        world->setEnergyDecayRate(rate);
    }
};

// Python wrapper for AgentBrain
class PythonAgentBrain {
private:
    std::shared_ptr<AgentBrain> agent_brain;
    std::shared_ptr<PythonBrain> python_brain;
    
public:
    PythonAgentBrain(std::shared_ptr<PythonBrain> brain) : 
        agent_brain(std::make_shared<AgentBrain>(brain->get_brain())),
        python_brain(brain) {}
    
    // Initialize with world
    bool initialize(std::shared_ptr<PythonSimpleWorld> world) {
        return agent_brain->initialize(*world->get_world().get());
    }
    
    // Get expected sensory input size
    int get_sensory_input_size() const {
        return agent_brain->getSensoryInputSize();
    }
    
    // Get expected motor output size
    int get_motor_output_size() const {
        return agent_brain->getMotorOutputSize();
    }
    
    // Process sensory input
    void process_sensory_input(std::shared_ptr<SensoryPercept> percept) {
        agent_brain->processSensoryInput(*percept.get());
    }
    
    // Decode motor command
    MotorCommand decode_motor_command() {
        return agent_brain->decodeMotorCommand();
    }
    
    // Apply reward modulation
    void apply_reward_modulation(double reward, double predicted_reward) {
        agent_brain->applyRewardModulation(reward, predicted_reward);
    }
    
    // Update development
    void update_development(double timestep = 0.001) {
        agent_brain->updateDevelopment(timestep);
    }
    
    // Get developmental stage
    std::string get_developmental_stage() const {
        return agent_brain->getDevelopmentalStage();
    }
    
    // Get neuromodulation level
    double get_neuromodulation_level() const {
        return agent_brain->getNeuromodulationLevel();
    }
    
    // Get curiosity level
    double get_curiosity_level() const {
        return agent_brain->getCuriosityLevel();
    }
    
    // Get novelty level
    double get_novelty_level() const {
        return agent_brain->getNoveltyLevel();
    }
    
    // Get prediction error
    double get_prediction_error() const {
        return agent_brain->getPredictionError();
    }
    
    // Reset agent
    void reset() {
        agent_brain->reset();
    }
    
    // Get underlying agent brain
    std::shared_ptr<AgentBrain> get_agent_brain() const { return agent_brain; }
    
    // Get underlying brain
    std::shared_ptr<PythonBrain> get_brain() const { return python_brain; }
    
    // Enable/disable features
    void enable_reward_modulation(bool enable) {
        agent_brain->enableRewardModulation(enable);
    }
    
    void enable_structural_plasticity(bool enable) {
        agent_brain->enableStructuralPlasticity(enable);
    }
    
    void enable_development(bool enable) {
        agent_brain->enableDevelopment(enable);
    }
    
    void enable_curiosity(bool enable) {
        agent_brain->enableCuriosity(enable);
    }
    
    bool is_reward_modulation_enabled() const {
        return agent_brain->isRewardModulationEnabled();
    }
    
    bool is_structural_plasticity_enabled() const {
        return agent_brain->isStructuralPlasticityEnabled();
    }
    
    bool is_development_enabled() const {
        return agent_brain->isDevelopmentEnabled();
    }
    
    bool is_curiosity_enabled() const {
        return agent_brain->isCuriosityEnabled();
    }
};

// Factory functions for easy creation
inline PythonConfig make_config(const std::string& name = "config") {
    return PythonConfig(name);
}

inline std::shared_ptr<PythonBrain> make_brain() {
    return std::make_shared<PythonBrain>();
}

inline std::shared_ptr<PythonBrain> make_brain(std::shared_ptr<PythonConfig> config) {
    return std::make_shared<PythonBrain>(config);
}

inline PythonSimpleWorld make_world() {
    return PythonSimpleWorld();
}

inline PythonAgentBrain make_agent_brain(std::shared_ptr<PythonBrain> brain) {
    return PythonAgentBrain(brain);
}

// Example function to demonstrate Python API usage
inline std::string get_example_code() {
    return R"(
# Example NLM Python API Usage

import pynlm

# Create default configuration
config = pynlm.make_config("experiment")
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Create brain with configuration
brain = pynlm.make_brain(config)

# Initialize brain
if brain.initialize():
    print("Brain initialized successfully!")

# Create simple world
world = pynlm.make_world()
world.configure(20, 20)  # 20x20 world
world.set_agent_start(10, 10)  # Start in center
world.reset()

# Create agent brain
agent = pynlm.make_agent_brain(brain)

# Create default agent body
agent_body = pynlm.make_agent_body()
agent_body.x = 10.0
agent_body.y = 10.0
agent_body.orientation = 0.0
agent_body.energy = 100.0
agent_body.health = 100.0

# Simulation loop
for step in range(100):
    # Get sensory percept from world
    percept = world.get_sensory_percept()
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Decode motor command
    action = agent.decode_motor_command()
    
    # Apply action to world
    world.apply_motor_command(action.motor_command)
    
    # Update development
    agent.update_development(0.001)
    
    # Apply reward
    reward = 1.0 if action.success else -1.0
    agent.apply_reward_modulation(reward, 0.0)
    
    # Update world
    world.update(0.001)
    
    # Print progress every 10 steps
    if step % 10 == 0:
        print(f"Step {step}: Energy={agent_body.energy:.1f}, Spikes={brain.get_total_spike_count()}")

# Save experiment results
brain.save("experiment_state.json")
world.save("world_state.json")

print("Experiment completed successfully!")
print(f"Final spikes: {brain.get_total_spike_count()}")
print(f"Final energy: {agent_body.energy}")
)";
}

// Get version information
inline std::string get_version() {
    return "NLM Python Bindings v1.0.0";
}

// Check if valid neuron ID
inline bool is_valid_neuron_id(const NeuronId& id) {
    return id.value != static_cast<uint64_t>(-1);
}

// Check if valid synapse ID
inline bool is_valid_synapse_id(const SynapseId& id) {
    return id.value != static_cast<uint64_t>(-1);
}

// Check if valid region ID
inline bool is_valid_region_id(const RegionId& id) {
    return id.value != static_cast<uint64_t>(-1);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An enhanced Python API for the NLM C++ neural simulation framework.
        
        This module provides Python-style wrappers around the C++ NLM framework,
        making it easier to use for machine learning, neuroscience research,
        and AI applications.
        
        Key Features:
        - Pythonic configuration management
        - Enhanced brain simulation with monitoring
        - Agent brain interface for world interaction
        - Simplified world and sensory processing
        - Comprehensive documentation and examples
        - Type checking and validation
        - Performance monitoring tools
        
        Usage:
            import pynlm
            
            # Create configuration
            config = pynlm.make_config("my_experiment")
            config.set("neuron_count", 1000)
            config.set("simulation_timestep", 0.001)
            
            # Create brain
            brain = pynlm.make_brain(config)
            
            # Initialize and run
            if brain.initialize():
                brain.step(0, 0.0)
                
        See examples in get_example_code() for more usage patterns.
    )pbdoc";

    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Configuration class
    py::class_<PythonConfig>(m, "Config", R"pbdoc(Python-style configuration wrapper)"pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("name"))
        .def("set_name", &PythonConfig::set_name)
        .def("get_name", &PythonConfig::get_name)
        .def("set", py::overload_cast<const std::string&, int, const std::string&>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             R"pbdoc(Set integer configuration value)pbdoc")
        .def("set", py::overload_cast<const std::string&, double, const std::string&>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             R"pbdoc(Set float configuration value)pbdoc")
        .def("set", py::overload_cast<const std::string&, const std::string&, const std::string&>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             R"pbdoc(Set string configuration value)pbdoc")
        .def("set", py::overload_cast<const std::string&, bool, const std::string&>(&PythonConfig::set),
             py::arg("key"), py::arg("value"), py::arg("source") = "runtime",
             R"pbdoc(Set boolean configuration value)pbdoc")
        .def("get_int", &PythonConfig::get_int,
             py::arg("key"), py::arg("default_value") = 0,
             R"pbdoc(Get integer value)pbdoc")
        .def("get_double", &PythonConfig::get_double,
             py::arg("key"), py::arg("default_value") = 0.0,
             R"pbdoc(Get float value)pbdoc")
        .def("get_string", &PythonConfig::get_string,
             py::arg("key"), py::arg("default_value") = "",
             R"pbdoc(Get string value)pbdoc")
        .def("get_bool", &PythonConfig::get_bool,
             py::arg("key"), py::arg("default_value") = false,
             R"pbdoc(Get boolean value)pbdoc")
        .def("load_from_env", &PythonConfig::load_from_env,
             py::arg("prefix") = "NLM_",
             R"pbdoc(Load configuration from environment variables)pbdoc")
        .def("load_from_json", &PythonConfig::load_from_json,
             py::arg("filepath"),
             R"pbdoc(Load configuration from JSON file)pbdoc")
        .def("save_to_json", &PythonConfig::save_to_json,
             py::arg("filepath"),
             R"pbdoc(Save configuration to JSON file)pbdoc")
        .def("validate", &PythonConfig::validate,
             R"pbdoc(Validate configuration values)pbdoc")
        .def("summary", &PythonConfig::summary,
             R"pbdoc(Get configuration summary)pbdoc")
        .def("get_keys", &PythonConfig::get_keys,
             R"pbdoc(Get all configuration keys)pbdoc")
        .def("merge", &PythonConfig::merge,
             py::arg("other"),
             R"pbdoc(Merge with another configuration)pbdoc")
        .def("has", &PythonConfig::has,
             py::arg("key"),
             R"pbdoc(Check if configuration key exists)pbdoc")
        .def("remove", &PythonConfig::remove,
             py::arg("key"),
             R"pbdoc(Remove configuration key)pbdoc")
        .def("clear", &PythonConfig::clear,
             R"pbdoc(Clear all configuration)pbdoc")
        .def("create_default", &PythonConfig::create_default,
             R"pbdoc(Create default configuration)pbdoc")
        .def("create_empty", &PythonConfig::create_empty,
             R"pbdoc(Create empty configuration)pbdoc")
        .def_readwrite("name", &PythonConfig::name);

    // Brain class
    py::class_<PythonBrain>(m, "Brain", R"pbdoc(Enhanced brain simulation wrapper)"pbdoc")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<PythonConfig>>(), py::arg("config"))
        .def("initialize", &PythonBrain::initialize,
             R"pbdoc(Initialize the brain with current configuration)pbdoc")
        .def("step", &PythonBrain::step,
             py::arg("step"), py::arg("time") = 0.0,
             R"pbdoc(Perform a simulation step)pbdoc")
        .def("get_config", &PythonBrain::get_config,
             R"pbdoc(Get brain configuration)pbdoc")
        .def("get_total_neuron_count", &PythonBrain::get_total_neuron_count,
             R"pbdoc(Get total neuron count)pbdoc")
        .def("get_total_synapse_count", &PythonBrain::get_total_synapse_count,
             R"pbdoc(Get total synapse count)pbdoc")
        .def("get_active_neuron_count", &PythonBrain::get_active_neuron_count,
             R"pbdoc(Get active neuron count)pbdoc")
        .def("get_firing_neuron_count", &PythonBrain::get_firing_neuron_count,
             R"pbdoc(Get firing neuron count)pbdoc")
        .def("get_average_firing_rate", &PythonBrain::get_average_firing_rate,
             R"pbdoc(Get average firing rate)pbdoc")
        .def("get_excitation_inhibition_ratio", &PythonBrain::get_excitation_inhibition_ratio,
             R"pbdoc(Get excitation/inhibition ratio)pbdoc")
        .def("get_total_spike_count", &PythonBrain::get_total_spike_count,
             R"pbdoc(Get total spike count)pbdoc")
        .def("receive_sensory_input", &PythonBrain::receive_sensory_input,
             py::arg("input"),
             R"pbdoc(Receive sensory input)pbdoc")
        .def("produce_action", &PythonBrain::produce_action,
             R"pbdoc(Produce motor action based on neural activity)pbdoc")
        .def("reset", &PythonBrain::reset,
             R"pbdoc(Reset brain state)pbdoc")
        .def("save", &PythonBrain::save,
             py::arg("filepath"),
             R"pbdoc(Save brain state to file)pbdoc")
        .def("load", &PythonBrain::load,
             py::arg("filepath"),
             R"pbdoc(Load brain state from file)pbdoc")
        .def("get_developmental_stage", &PythonBrain::get_developmental_stage,
             R"pbdoc(Get developmental stage)pbdoc")
        .def("set_developmental_stage", &PythonBrain::set_developmental_stage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage)pbdoc")
        .def("get_status", &PythonBrain::get_status,
             R"pbdoc(Get brain status as string)pbdoc")
        .def("get_performance_metrics", &PythonBrain::get_performance_metrics,
             py::arg("start_time"), py::arg("end_time"),
             R"pbdoc(Get performance metrics)")
        .def("apply_neuromodulation", &PythonBrain::apply_neuromodulation,
             py::arg("signal"),
             R"pbdoc(Apply neuromodulation signal)pbdoc")
        .def("update_plasticity", &PythonBrain::update_plasticity,
             R"pbdoc(Update plasticity rules)pbdoc")
        .def("develop", &PythonBrain::develop,
             R"pbdoc(Apply developmental changes)pbdoc")
        .def("inject_current", &PythonBrain::inject_current,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into specific neuron)pbdoc")
        .def("inject_current_to_neurons", &PythonBrain::inject_current_to_neurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(Inject current into neurons of specific type)pbdoc")
        .def("get_region", &PythonBrain::get_region,
             py::arg("id"),
             R"pbdoc(Get neural region by ID)pbdoc")
        .def("add_region", &PythonBrain::add_region,
             py::arg("name") = "",
             R"pbdoc(Add new neural region)pbdoc")
        .def("get_region_ids", &PythonBrain::get_region_ids,
             R"pbdoc(Get all region IDs)pbdoc");

    // SimpleWorld class
    py::class_<PythonSimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world wrapper)"pbdoc")
        .def(py::init<>())
        .def("configure", &PythonSimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width") = 5, py::arg("vision_height") = 5,
             R"pbdoc(Configure the world)pbdoc")
        .def("reset", &PythonSimpleWorld::reset,
             R"pbdoc(Reset world state)pbdoc")
        .def("set_agent_start", &PythonSimpleWorld::set_agent_start,
             py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position)pbdoc")
        .def("update", &PythonSimpleWorld::update,
             py::arg("timestep") = 0.001,
             R"pbdoc(Update world state)pbdoc")
        .def("apply_motor_command", &PythonSimpleWorld::apply_motor_command,
             py::arg("cmd"), py::arg("current_time") = 0.0,
             R"pbdoc(Apply motor command to world)pbdoc")
        .def("get_sensory_percept", &PythonSimpleWorld::get_sensory_percept,
             R"pbdoc(Get sensory percept from world)pbdoc")
        .def("get_agent_body", &PythonSimpleWorld::get_agent_body,
             R"pbdoc(Get agent body)pbdoc")
        .def("add_object", &PythonSimpleWorld::add_object,
             py::arg("obj"),
             R"pbdoc(Add world object)pbdoc")
        .def("remove_object", &PythonSimpleWorld::remove_object,
             py::arg("x"), py::arg("y"),
             R"pbdoc(Remove world object)pbdoc")
        .def("is_valid_position", &PythonSimpleWorld::is_valid_position,
             py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid)pbdoc")
        .def("get_dimensions", &PythonSimpleWorld::get_dimensions,
             R"pbdoc(Get world dimensions)pbdoc")
        .def("get_simulation_time", &PythonSimpleWorld::get_simulation_time,
             R"pbdoc(Get simulation time)pbdoc")
        .def("set_random_seed", &PythonSimpleWorld::set_random_seed,
             py::arg("seed"),
             R"pbdoc(Set random seed)pbdoc")
        .def("get_random_seed", &PythonSimpleWorld::get_random_seed,
             R"pbdoc(Get random seed)pbdoc")
        .def("get_max_energy", &PythonSimpleWorld::get_max_energy,
             R"pbdoc(Get max energy)pbdoc")
        .def("set_max_energy", &PythonSimpleWorld::set_max_energy,
             py::arg("energy"),
             R"pbdoc(Set max energy)pbdoc")
        .def("get_energy_decay_rate", &PythonSimpleWorld::get_energy_decay_rate,
             R"pbdoc(Get energy decay rate)pbdoc")
        .def("set_energy_decay_rate", &PythonSimpleWorld::set_energy_decay_rate,
             py::arg("rate"),
             R"pbdoc(Set energy decay rate)pbdoc");

    // AgentBrain class
    py::class_<PythonAgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface wrapper)"pbdoc")
        .def(py::init<std::shared_ptr<PythonBrain>>(), py::arg("brain"))
        .def("initialize", &PythonAgentBrain::initialize,
             py::arg("world"),
             R"pbdoc(Initialize agent brain with world)pbdoc")
        .def("get_sensory_input_size", &PythonAgentBrain::get_sensory_input_size,
             R"pbdoc(Get expected sensory input size)pbdoc")
        .def("get_motor_output_size", &PythonAgentBrain::get_motor_output_size,
             R"pbdoc(Get expected motor output size)pbdoc")
        .def("process_sensory_input", &PythonAgentBrain::process_sensory_input,
             py::arg("percept"),
             R"pbdoc(Process sensory input and inject into brain)pbdoc")
        .def("decode_motor_command", &PythonAgentBrain::decode_motor_command,
             R"pbdoc(Decode brain motor activity into motor command)pbdoc")
        .def("apply_reward_modulation", &PythonAgentBrain::apply_reward_modulation,
             py::arg("reward"), py::arg("predicted_reward"),
             R"pbdoc(Apply reward-based neuromodulation)pbdoc")
        .def("update_development", &PythonAgentBrain::update_development,
             py::arg("timestep") = 0.001,
             R"pbdoc(Update development system)pbdoc")
        .def("get_developmental_stage", &PythonAgentBrain::get_developmental_stage,
             R"pbdoc(Get current developmental stage)pbdoc")
        .def("get_neuromodulation_level", &PythonAgentBrain::get_neuromodulation_level,
             R"pbdoc(Get current neuromodulation level)pbdoc")
        .def("get_curiosity_level", &PythonAgentBrain::get_curiosity_level,
             R"pbdoc(Get curiosity level)pbdoc")
        .def("get_novelty_level", &PythonAgentBrain::get_novelty_level,
             R"pbdoc(Get novelty level)pbdoc")
        .def("get_prediction_error", &PythonAgentBrain::get_prediction_error,
             R"pbdoc(Get prediction error)pbdoc")
        .def("reset", &PythonAgentBrain::reset,
             R"pbdoc(Reset agent for new episode)pbdoc")
        .def("get_brain", &PythonAgentBrain::get_brain,
             R"pbdoc(Get underlying brain)pbdoc")
        .def("enable_reward_modulation", &PythonAgentBrain::enable_reward_modulation,
             py::arg("enable"),
             R"pbdoc(Enable/disable reward modulation)pbdoc")
        .def("enable_structural_plasticity", &PythonAgentBrain::enable_structural_plasticity,
             py::arg("enable"),
             R"pbdoc(Enable/disable structural plasticity)pbdoc")
        .def("enable_development", &PythonAgentBrain::enable_development,
             py::arg("enable"),
             R"pbdoc(Enable/disable development)pbdoc")
        .def("enable_curiosity", &PythonAgentBrain::enable_curiosity,
             py::arg("enable"),
             R"pbdoc(Enable/disable curiosity)pbdoc")
        .def("is_reward_modulation_enabled", &PythonAgentBrain::is_reward_modulation_enabled,
             R"pbdoc(Check if reward modulation is enabled)pbdoc")
        .def("is_structural_plasticity_enabled", &PythonAgentBrain::is_structural_plasticity_enabled,
             R"pbdoc(Check if structural plasticity is enabled)pbdoc")
        .def("is_development_enabled", &PythonAgentBrain::is_development_enabled,
             R"pbdoc(Check if development is enabled)pbdoc")
        .def("is_curiosity_enabled", &PythonAgentBrain::is_curiosity_enabled,
             R"pbdoc(Check if curiosity is enabled)pbdoc");

    // Factory functions
    m.def("make_config", &make_config,
          py::arg("name") = "config",
          R"pbdoc(Create a new configuration)"pbdoc");

    m.def("make_brain", py::overload_cast<>(&make_brain),
          R"pbdoc(Create a new brain with default configuration)"pbdoc");

    m.def("make_brain", py::overload_cast<std::shared_ptr<PythonConfig>>(&make_brain),
          py::arg("config"),
          R"pbdoc(Create a new brain with specified configuration)"pbdoc");

    m.def("make_world", &make_world,
          R"pbdoc(Create a new simple world)"pbdoc");

    m.def("make_agent_brain", &make_agent_brain,
          py::arg("brain"),
          R"pbdoc(Create a new agent brain interface)"pbdoc");

    // Utility functions
    m.def("get_version", &get_version,
          R"pbdoc(Get NLM Python bindings version)"pbdoc");

    m.def("get_example_code", &get_example_code,
          R"pbdoc(Get example code for using the Python API)"pbdoc");

    m.def("is_valid_neuron_id", &is_valid_neuron_id,
          py::arg("id"),
          R"pbdoc(Check if neuron ID is valid)"pbdoc");

    m.def("is_valid_synapse_id", &is_valid_synapse_id,
          py::arg("id"),
          R"pbdoc(Check if synapse ID is valid)"pbdoc");

    m.def("is_valid_region_id", &is_valid_region_id,
          py::arg("id"),
          R"pbdoc(Check if region ID is valid)"pbdoc");

    // Type aliases for convenience
    py::class_<ConfigValue>(m, "ConfigValue", R"pbdoc(Configuration value type (internal)"pbdoc"))
        .def(py::init<>());

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron identifier)"pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value);

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse identifier)"pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value);

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region identifier)"pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value);

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Population identifier)"pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)"pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)"pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)"pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)"pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)"pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)"pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)"pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::enum_<ActionResultSuccess>(m, "ActionResultSuccess", R"pbdoc(Action result success enumeration)"pbdoc")
        .value("Success", ActionResultSuccess::Success)
        .value("Failure", ActionResultSuccess::Failure)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)"pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // Exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Add constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

} // namespace nlm