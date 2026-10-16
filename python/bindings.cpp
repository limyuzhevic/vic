#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Helper functions for Python bindings
template<typename T>
T py_arg(const py::object& obj) {
    return obj.cast<T>();
}

std::string py_str(const std::string& str) {
    return str;
}

// Convenience factory functions
inline std::shared_ptr<Config> create_default_config() {
    return std::make_shared<Config>();
}

inline std::shared_ptr<Brain> create_brain_with_config(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    return std::make_shared<Brain>(config);
}

inline std::shared_ptr<SimpleWorld> create_default_world() {
    return std::make_shared<SimpleWorld>();
}

inline std::shared_ptr<AgentBrain> create_agent_brain(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        throw std::invalid_argument("Brain cannot be null");
    }
    return std::make_shared<AgentBrain>(brain);
}

// Enhanced Config class with Pythonic API
class ConfigPy {
private:
    std::shared_ptr<Config> config_;
    
public:
    ConfigPy() : config_(std::make_shared<Config>()) {}
    ConfigPy(const std::string& filepath) : config_(std::make_shared<Config>()) {
        if (!config_->loadFromFile(filepath)) {
            throw std::runtime_error("Failed to load configuration from file: " + filepath);
        }
    }
    
    // Conversion constructor
    ConfigPy(const std::shared_ptr<Config>& config) : config_(config) {}
    
    // Pythonic get() with type checking
    template<typename T>
    T get(const std::string& key, T default_value = T{}) const {
        auto result = config_->get<T>(key);
        if (result.has_value()) {
            return result.value();
        }
        return default_value;
    }
    
    // Pythonic set() methods
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, int value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    // Pythonic has() method
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Convenient get methods
    std::string get_string(const std::string& key, const std::string& default_value = "") const {
        return get<std::string>(key, default_value);
    }
    
    int get_int(const std::string& key, int default_value = 0) const {
        return get<int>(key, default_value);
    }
    
    double get_float(const std::string& key, double default_value = 0.0) const {
        return get<double>(key, default_value);
    }
    
    bool get_bool(const std::string& key, bool default_value = false) const {
        return get<bool>(key, default_value);
    }
    
    // Get all keys
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    // Load from JSON or YAML file
    bool load_file(const std::string& filepath) {
        return config_->loadFromFile(filepath);
    }
    
    // Load from command line arguments
    bool load_args(int argc, char** argv) {
        return config_->loadFromArgs(argc, argv);
    }
    
    // Save to file
    bool save_file(const std::string& filepath) const {
        return config_->saveToFile(filepath);
    }
    
    // String representation
    std::string __repr__() const {
        return config_->summary();
    }
    
    // Get underlying config pointer
    std::shared_ptr<Config> get_config() const {
        return config_;
    }
};

// Enhanced Brain class with Pythonic API
class BrainPy {
private:
    std::shared_ptr<Brain> brain_;
    
public:
    BrainPy(const std::shared_ptr<Config>& config) : brain_(std::make_shared<Brain>(config)) {}
    BrainPy(const std::shared_ptr<ConfigPy>& config) : brain_(std::make_shared<Brain>(config->get_config())) {}
    
    // Initialize the brain
    bool initialize() {
        return brain_->initialize();
    }
    
    // Run simulation steps
    void step(int steps = 1) {
        for (int i = 0; i < steps; ++i) {
            brain_->step(brain_->getTotalSpikeCount());
        }
    }
    
    // Run for a specific duration
    void run_for_time(double seconds, double timestep = 0.001) {
        size_t steps = static_cast<size_t>(seconds / timestep);
        for (size_t i = 0; i < steps; ++i) {
            brain_->step(i);
        }
    }
    
    // Get various statistics
    size_t get_neuron_count() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t get_synapse_count() const {
        return brain_->getTotalSynapseCount();
    }
    
    size_t get_active_neuron_count() const {
        return brain_->getActiveNeuronCount();
    }
    
    size_t get_firing_neuron_count() const {
        return brain_->getFiringNeuronCount();
    }
    
    double get_average_firing_rate() const {
        return brain_->getAverageFiringRate();
    }
    
    double get_ei_ratio() const {
        return brain_->getExcitationInhibitionRatio();
    }
    
    size_t get_total_spike_count() const {
        return brain_->getTotalSpikeCount();
    }
    
    // Memory system access
    std::vector<float> get_working_memory() const {
        if (brain_->getWorkingMemory()) {
            return brain_->getWorkingMemory()->retrieve();
        }
        return std::vector<float>();
    }
    
    size_t get_episode_count() const {
        if (brain_->getEpisodicMemory()) {
            return brain_->getEpisodicMemory()->getEpisodeCount();
        }
        return 0;
    }
    
    // Action production
    std::string produce_action() {
        auto action = brain_->produceAction();
        return action->getName();
    }
    
    // Configuration access
    std::shared_ptr<ConfigPy> get_config() const {
        return std::make_shared<ConfigPy>(brain_->getConfig());
    }
    
    // Save and load
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Reset brain
    void reset() {
        brain_->reset();
    }
    
    // String representation
    std::string __repr__() const {
        std::stringstream ss;
        ss << "Brain(neurons=" << get_neuron_count()
           << ", synapses=" << get_synapse_count()
           << ", active=" << get_active_neuron_count()
           << ", firing=" << get_firing_neuron_count() << ")";
        return ss.str();
    }
};

// Enhanced SimpleWorld class with Pythonic API
class SimpleWorldPy {
private:
    std::shared_ptr<SimpleWorld> world_;
    
public:
    SimpleWorldPy() : world_(std::make_shared<SimpleWorld>()) {}
    
    // Configure world
    void configure(int width, int height, int vision_width = 64, int vision_height = 64) {
        world_->configure(width, height, vision_width, vision_height);
    }
    
    // Set agent start position
    void set_agent_start(float x, float y) {
        world_->setAgentStart(x, y);
    }
    
    // Reset world
    void reset() {
        world_->reset();
    }
    
    // Update world
    void update(double timestep) {
        world_->update(timestep);
    }
    
    // Apply motor command
    bool apply_motor_command(const std::string& command_name, double current_time) {
        // For now, just log the command
        NLM_LOG_INFO("Applying motor command: " + command_name + " at time " + std::to_string(current_time));
        return true;
    }
    
    // Get sensory percept
    std::vector<float> get_sensory_percept() const {
        auto percept = world_->getSensoryPercept();
        return percept.getAllSignals();
    }
    
    // Get agent body
    std::string get_agent_body() const {
        auto body = world_->getAgentBody();
        std::stringstream ss;
        ss << "AgentBody(x=" << body.x << ", y=" << body.y 
           << ", orientation=" << body.orientation << ")";
        return ss.str();
    }
    
    // World dimensions
    int get_width() const {
        return world_->getWidth();
    }
    
    int get_height() const {
        return world_->getHeight();
    }
    
    // Simulation time
    double get_simulation_time() const {
        return world_->getSimulationTime();
    }
    
    // String representation
    std::string __repr__() const {
        std::stringstream ss;
        ss << "SimpleWorld(width=" << get_width() << ", height=" << get_height() << ")";
        return ss.str();
    }
};

// Convenience functions
inline std::shared_ptr<ConfigPy> create_config() {
    return std::make_shared<ConfigPy>();
}

inline std::shared_ptr<BrainPy> create_brain() {
    auto config = std::make_shared<ConfigPy>();
    return std::make_shared<BrainPy>(config);
}

inline std::shared_ptr<SimpleWorldPy> create_world() {
    return std::make_shared<SimpleWorldPy>();
}

inline std::shared_ptr<AgentBrain> create_agent_brain_py(const std::shared_ptr<BrainPy>& brain_py) {
    if (!brain_py) {
        throw std::invalid_argument("Brain cannot be null");
    }
    return brain_py->brain_->getAgentBrain();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        --------------------------------------------------------
        
        A user-friendly Python interface to the NLM neural simulation framework.
        
        Key features:
        - Pythonic APIs for easy usage
        - Type-safe operations with automatic conversion
        - Comprehensive error handling with descriptive messages
        - Convenience functions for common operations
        - Enhanced documentation and examples
        
        The NLM framework implements biologically-inspired neural networks
        that can learn, remember, and make decisions through neural dynamics.
    )pbdoc";

    // Configure Python exception handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    
    // Basic types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
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
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Core configuration class
    py::class_<ConfigPy>(m, "Config", R"pbdoc(Pythonic configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"))
        
        // Basic operations
        .def("load", &ConfigPy::load_file, py::arg("filepath"),
             "Load configuration from a file (JSON, YAML, or simple key=value format)")
        .def("save", &ConfigPy::save_file, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &ConfigPy::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &ConfigPy::get_keys,
             "Get all configuration keys")
        .def("clear", &ConfigPy::clear,
             "Clear all configuration entries")
        
        // Pythonic get methods with type conversion
        .def("get", static_cast<std::string (ConfigPy::*)(const std::string&, const std::string&) const>(&ConfigPy::get_string),
             py::arg("key"), py::arg("default_value") = "",
             "Get a string value from configuration")
        .def("get", static_cast<int (ConfigPy::*)(const std::string&, int) const>(&ConfigPy::get_int),
             py::arg("key"), py::arg("default_value") = 0,
             "Get an integer value from configuration")
        .def("get", static_cast<double (ConfigPy::*)(const std::string&, double) const>(&ConfigPy::get_float),
             py::arg("key"), py::arg("default_value") = 0.0,
             "Get a float value from configuration")
        .def("get", static_cast<bool (ConfigPy::*)(const std::string&, bool) const>(&ConfigPy::get_bool),
             py::arg("key"), py::arg("default_value") = false,
             "Get a boolean value from configuration")
        
        // Pythonic set methods
        .def("set", static_cast<void (ConfigPy::*)(const std::string&, const std::string&)>(&ConfigPy::set),
             py::arg("key"), py::arg("value"),
             "Set a string configuration value")
        .def("set", static_cast<void (ConfigPy::*)(const std::string&, int)>(&ConfigPy::set),
             py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("set", static_cast<void (ConfigPy::*)(const std::string&, double)>(&ConfigPy::set),
             py::arg("key"), py::arg("value"),
             "Set a float configuration value")
        .def("set", static_cast<void (ConfigPy::*)(const std::string&, bool)>(&ConfigPy::set),
             py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        
        // String representation
        .def("__repr__", &ConfigPy::__repr__);
    
    // Enhanced Brain class
    py::class_<BrainPy>(m, "Brain", R"pbdoc(Extended Pythonic brain simulation class)pbdoc")
        .def(py::init<const std::shared_ptr<ConfigPy>&>(), py::arg("config"),
             "Create a brain with a config")
        .def(py::init<const std::shared_ptr<Config>&>(), py::arg("config"),
             "Create a brain with a C++ config (internal use)")
        
        // Core operations
        .def("initialize", &BrainPy::initialize,
             "Initialize the brain with configuration and all integrated systems")
        .def("reset", &BrainPy::reset,
             "Reset the brain to initial state")
        .def("step", &BrainPy::step, py::arg("steps") = 1,
             "Advance the simulation by specified number of steps")
        .def("run", &BrainPy::run_for_time, py::arg("seconds") = 1.0, py::arg("timestep") = 0.001,
             "Run the simulation for a specified duration (seconds)")
        
        // Statistics and monitoring
        .def("get_neuron_count", &BrainPy::get_neuron_count,
             "Get total number of neurons in the brain")
        .def("get_synapse_count", &BrainPy::get_synapse_count,
             "Get total number of synapses in the brain")
        .def("get_active_neuron_count", &BrainPy::get_active_neuron_count,
             "Get number of currently active neurons")
        .def("get_firing_neuron_count", &BrainPy::get_firing_neuron_count,
             "Get number of neurons currently firing")
        .def("get_average_firing_rate", &BrainPy::get_average_firing_rate,
             "Get average firing rate across all neurons (Hz)")
        .def("get_ei_ratio", &BrainPy::get_ei_ratio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &BrainPy::get_total_spike_count,
             "Get total spike count since initialization")
        
        // Memory system access
        .def("get_working_memory", &BrainPy::get_working_memory,
             "Get current working memory as activity pattern")
        .def("get_episode_count", &BrainPy::get_episode_count,
             "Get number of stored episodes in episodic memory")
        
        // Action production
        .def("produce_action", &BrainPy::produce_action,
             "Produce motor action based on neural activity")
        
        // Configuration access
        .def("get_config", &BrainPy::get_config,
             "Get the brain's configuration object")
        
        // Save and load
        .def("save", &BrainPy::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainPy::load, py::arg("filepath"),
             "Load brain state from file")
        
        // String representation
        .def("__repr__", &BrainPy::__repr__)
        .def("__str__", &BrainPy::__repr__);
    
    // Enhanced SimpleWorld class
    py::class_<SimpleWorldPy>(m, "SimpleWorld", R"pbdoc(Pythonic wrapper for SimpleWorld simulation)pbdoc")
        .def(py::init<>())
        
        // World configuration
        .def("configure", &SimpleWorldPy::configure,
             py::arg("width"), py::arg("height"), py::arg("vision_width") = 64, py::arg("vision_height") = 64,
             "Configure the world dimensions and vision capabilities")
        .def("set_agent_start", &SimpleWorldPy::set_agent_start, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        
        // Simulation control
        .def("reset", &SimpleWorldPy::reset,
             "Reset the world to initial state")
        .def("update", &SimpleWorldPy::update, py::arg("timestep") = 0.001,
             "Update the world simulation by one timestep")
        
        // State access
        .def("get_sensory_percept", &SimpleWorldPy::get_sensory_percept,
             "Get current sensory percept as signal vector")
        .def("get_agent_body", &SimpleWorldPy::get_agent_body,
             "Get current agent body state")
        
        // World properties
        .def("get_width", &SimpleWorldPy::get_width,
             "Get world width in simulation units")
        .def("get_height", &SimpleWorldPy::get_height,
             "Get world height in simulation units")
        .def("get_simulation_time", &SimpleWorldPy::get_simulation_time,
             "Get current simulation time")
        
        // String representation
        .def("__repr__", &SimpleWorldPy::__repr__);
    
    // Convenience factory functions
    m.def("create_default_config", &create_default_config,
          "Create a default configuration object")
     .def("create_brain", &create_brain,
          "Create a brain with default configuration")
     .def("create_default_world", &create_default_world,
          "Create a default world simulation")
     .def("create_agent_brain", &create_agent_brain_py,
          py::arg("brain"), "Create an agent brain interface from a brain object")
     .def("create_config", &create_config,
          "Pythonic alias for create_default_config")
     .def("create_world", &create_world,
          "Pythonic alias for create_default_world");
    
    // System information and utilities
    m.def("get_version", []() { return "NLM 0.1.0 (Python Enhanced)"; },
          "Get the version of the NLM Python bindings")
     .def("get_system_info", []() {
           std::stringstream ss;
           ss << "NLM Neural Learning Machine - Enhanced Python Bindings\n";
           ss << "Version: 0.1.0\n";
           ss << "Build: Enhanced with Pythonic APIs, convenience functions, and error handling\n";
           ss << "Features: JSON/YAML support, type conversion, comprehensive documentation\n";
           return ss.str();
       }, "Get system information and version details")
    
    // Example usage function
    m.def("example_usage", []() {
           std::string example = R"(
Example usage of NLM Python bindings:

import pynlm

# Create a brain with default configuration
brain = pynlm.create_brain()
brain.initialize()

# Configure world
world = pynlm.create_world()
world.configure(100, 100, vision_width=32, vision_height=32)

# Run simulation
brain.run(5.0)  # Run for 5 seconds

# Get statistics
print(f"Brain state: {brain}")
print(f"Neurons: {brain.get_neuron_count()}")
print(f"Synapses: {brain.get_synapse_count()}")
print(f"Active neurons: {brain.get_active_neuron_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")

# Save the brain state
brain.save("brain_state.nlm")

print("Example completed successfully!")
           )";
           return example;
       }, "Get example usage code");
}

} // namespace nlm