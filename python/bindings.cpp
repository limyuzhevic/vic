#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

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

// Custom Python exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class ExperimentError : public NLMError {
public:
    explicit ExperimentError(const std::string& msg) : NLMError("Experiment Error: " + msg) {}
};

// Convenience functions and factories
inline std::shared_ptr<Config> create_default_config() {
    auto config = std::make_shared<Config>();
    return config;
}

inline std::shared_ptr<Brain> create_brain(std::shared_ptr<Config> config) {
    if (!config) {
        throw BrainError("Cannot create brain with null configuration");
    }
    return std::make_shared<Brain>(config);
}

inline std::shared_ptr<SimpleWorld> create_simple_world() {
    return std::make_shared<SimpleWorld>();
}

inline std::shared_ptr<AgentBrain> create_agent_brain(std::shared_ptr<Brain> brain) {
    if (!brain) {
        throw BrainError("Cannot create AgentBrain with null brain");
    }
    return std::make_shared<AgentBrain>(brain);
}

// Pythonic wrapper classes
class _ConfigWrapper {
    std::shared_ptr<Config> config_;
    
public:
    explicit _ConfigWrapper(std::shared_ptr<Config> config = nullptr) : config_(config) {
        if (!config_) {
            config_ = std::make_shared<Config>();
        }
    }
    
    // Load configuration from file
    void load_from_file(const std::string& filepath) {
        if (!config_->loadFromFile(filepath)) {
            throw ConfigError("Failed to load config from " + filepath);
        }
    }
    
    // Save configuration to file
    void save_to_file(const std::string& filepath) const {
        if (!config_->saveToFile(filepath)) {
            throw ConfigError("Failed to save config to " + filepath);
        }
    }
    
    // Get value with type conversion
    template<typename T>
    T get(const std::string& key, const T& default_value) const {
        auto value = config_->get<T>(key);
        return value ? *value : default_value;
    }
    
    // Set value
    void set(const std::string& key, const std::string& value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, int value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        config_->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        config_->set(key, value);
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all configuration keys
    std::vector<std::string> get_keys() const {
        return config_->getKeys();
    }
    
    // Clear configuration
    void clear() {
        config_->clear();
    }
    
    // Get config summary
    std::string get_summary() const {
        return config_->summary();
    }
    
    // Cast to shared_ptr
    std::shared_ptr<Config> get_config() const {
        return config_;
    }
    
    // String representation
    std::string __repr__() const {
        return "<Config: " + config_->summary() + ">";
    }
};

class _BrainWrapper {
    std::shared_ptr<Brain> brain_;
    
public:
    explicit _BrainWrapper(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (!brain_) {
            throw BrainError("Brain wrapper requires non-null brain");
        }
    }
    
    // Context manager for automatic cleanup
    bool __enter__() {
        brain_->reset();
        return brain_->initialize();
    }
    
    void __exit__(py::object /*type*/, py::object /*value*/, py::object /*traceback*/) {
        brain_->reset();
    }
    
    // Brain setup with configuration
    bool initialize() {
        return brain_->initialize();
    }
    
    // Single simulation step
    void step(int step_num, double time = 0.0) {
        if (time > 0.0) {
            brain_->step(step_num, time);
        } else {
            brain_->step(step_num);
        }
    }
    
    // Add sensory input
    void add_sensory_input(const SensoryInput& input) {
        brain_->receiveSensoryInput(input);
    }
    
    // Produce motor output
    std::unique_ptr<Action> produce_action() {
        return brain_->produceAction();
    }
    
    // Save brain state
    bool save(const std::string& filepath) const {
        return brain_->save(filepath);
    }
    
    // Load brain state
    bool load(const std::string& filepath) {
        return brain_->load(filepath);
    }
    
    // Convenience properties
    size_t get_total_neuron_count() const {
        return brain_->getTotalNeuronCount();
    }
    
    size_t get_total_synapse_count() const {
        return brain_->getTotalSynapseCount();
    }
    
    float get_average_firing_rate() const {
        return brain_->getAverageFiringRate();
    }
    
    // Access to underlying brain
    std::shared_ptr<Brain> get_brain() const {
        return brain_;
    }
    
    std::string __repr__() const {
        std::stringstream ss;
        ss << "<Brain neurons=" << brain_->getTotalNeuronCount()
           << " synapses=" << brain_->getTotalSynapseCount()
           << " stage=" << static_cast<int>(brain_->getDevelopmentalStage())
           << ">";
        return ss.str();
    }
};

class _ExperimentRunner {
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_brain_;
    
public:
    _ExperimentRunner(std::shared_ptr<Brain> brain,
                     std::shared_ptr<SimpleWorld> world = nullptr,
                     std::shared_ptr<AgentBrain> agent_brain = nullptr)
        : brain_(brain), world_(world), agent_brain_(agent_brain) {
        if (!brain_) {
            throw ExperimentError("ExperimentRunner requires a brain");
        }
        if (!world_) {
            world_ = create_simple_world();
        }
        if (!agent_brain_) {
            agent_brain_ = create_agent_brain(brain_);
        }
    }
    
    // Quick setup for common experiment
    void setup_quick(int width = 20, int height = 20, int vision_width = 5, int vision_height = 5) {
        world_->configure(static_cast<float>(width), static_cast<float>(height),
                         static_cast<size_t>(vision_width), static_cast<size_t>(vision_height));
        agent_brain_->initialize(*world_);
    }
    
    // Run single simulation step with sensory input
    double run_step(int step_num, double timestep = 0.1) {
        auto& percept = world_->getSensoryPercept();
        agent_brain_->processSensoryInput(percept);
        brain_->step(step_num, timestep);
        world_->update(timestep);
        return world_->getSimulationTime();
    }
    
    // Run learning episode
    void run_episode(int steps, double timestep = 0.1) {
        if (!brain_->initialize()) {
            throw ExperimentError("Failed to initialize brain for episode");
        }
        
        for (int step = 0; step < steps; ++step) {
            run_step(step, timestep);
        }
    }
    
    // Get statistics
    std::map<std::string, double> get_stats() const {
        std::map<std::string, double> stats;
        stats["total_neurons"] = static_cast<double>(brain_->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain_->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
        stats["average_firing_rate"] = brain_->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain_->getExcitationInhibitionRatio();
        return stats;
    }
    
    // Access components
    std::shared_ptr<Brain> get_brain() const { return brain_; }
    std::shared_ptr<SimpleWorld> get_world() const { return world_; }
    std::shared_ptr<AgentBrain> get_agent_brain() const { return agent_brain_; }
    
    std::string __repr__() const {
        return "<ExperimentRunner brain=" + std::to_string(brain_->getTotalNeuronCount()) +
               " neurons world=" + std::to_string(static_cast<double>(world_->getWidth())) +
               "x" + std::to_string(static_cast<double>(world_->getHeight())) + "x" + ">";
    }
};

// QuickStart wizard for common use cases
inline std::shared_ptr<_ExperimentRunner> quick_start(const std::string& config_file = "", 
                                                     int width = 20, int height = 20,
                                                     int vision_width = 5, int vision_height = 5) {
    auto config = create_default_config();
    if (!config_file.empty()) {
        config->loadFromFile(config_file);
    }
    
    auto brain = create_brain(config);
    auto world = create_simple_world();
    auto agent_brain = create_agent_brain(brain);
    
    auto runner = std::make_shared<_ExperimentRunner>(brain, world, agent_brain);
    runner->setup_quick(width, height, vision_width, vision_height);
    
    return runner;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides Pythonic interfaces to neural network simulation,
        agent-based learning, and cognitive architectures.
        
        Key Features:
        - High-level brain creation and management
        - Simple experiment running with quick start wizard
        - Pythonic API with context managers and properties
        - Enhanced error handling with helpful messages
        - Serialization utilities for saving/loading experiments
        
        Examples:
        >>> import pynlm
        >>> # Quick start with default configuration
        >>> runner = pynlm.quick_start(width=30, height=30)
        >>> 
        >>> # Create brain with custom configuration
        >>> brain = pynlm.create_brain(pynlm.create_config())
        >>> 
        >>> # Use brain as context manager
        >>> with pynlm.brain_context(brain):
        >>>     brain.step(1)
        >>>
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<ExperimentError>(m, "ExperimentError", PyExc_RuntimeError);

    // NeuronId class with Pythonic features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, "Neuron ID value")
        .def("index", &NeuronId::index, "Get neuron index")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Convenience factory functions
    m.def("create_default_config", &create_default_config, 
          py::return_value_policy::take_ownership,
          "Create a default configuration");
    
    m.def("create_brain", &create_brain, py::arg("config"),
          py::return_value_policy::take_ownership,
          "Create a new brain with the given configuration");
    
    m.def("create_simple_world", &create_simple_world,
          py::return_value_policy::take_ownership,
          "Create a new simple world");
    
    m.def("create_agent_brain", &create_agent_brain, py::arg("brain"),
          py::return_value_policy::take_ownership,
          "Create a new agent brain interface");
    
    // High-level convenience functions
    m.def("quick_start", &quick_start,
          py::arg("config_file") = "",
          py::arg("width") = 20,
          py::arg("height") = 20,
          py::arg("vision_width") = 5,
          py::arg("vision_height") = 5,
          "QuickStart wizard for common use cases - creates a brain with world and agent",
          py::return_value_policy::take_ownership);

    // Pythonic wrapper classes
    py::class_<_ConfigWrapper>(m, "Config", R"pbdoc(Pythonic configuration wrapper)pbdoc")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create wrapper from existing config")
        .def("load_from_file", &nlm::_ConfigWrapper::load_from_file, py::arg("filepath"),
             "Load configuration from a JSON/YAML file")
        .def("save_to_file", &nlm::_ConfigWrapper::save_to_file, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&nlm::_ConfigWrapper::get),
             py::arg("key"), py::arg("default_value") = "",
             "Get configuration value with default fallback")
        .def("get", py::overload_cast<const std::string&, int>(&nlm::_ConfigWrapper::get),
             py::arg("key"), py::arg("default_value") = 0,
             "Get configuration value with default fallback")
        .def("get", py::overload_cast<const std::string&, double>(&nlm::_ConfigWrapper::get),
             py::arg("key"), py::arg("default_value") = 0.0,
             "Get configuration value with default fallback")
        .def("get", py::overload_cast<const std::string&, bool>(&nlm::_ConfigWrapper::get),
             py::arg("key"), py::arg("default_value") = false,
             "Get configuration value with default fallback")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&nlm::_ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("set", py::overload_cast<const std::string&, int>(&nlm::_ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("set", py::overload_cast<const std::string&, double>(&nlm::_ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("set", py::overload_cast<const std::string&, bool>(&nlm::_ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("has", &nlm::_ConfigWrapper::has, py::arg("key"),
             "Check if configuration key exists")
        .def("get_keys", &nlm::_ConfigWrapper::get_keys,
             "Get all configuration keys")
        .def("clear", &nlm::_ConfigWrapper::clear,
             "Clear all configuration entries")
        .def("get_summary", &nlm::_ConfigWrapper::get_summary,
             "Get a summary string of the configuration")
        .def("__repr__", &nlm::_ConfigWrapper::__repr__)
        .def("_get_internal_config", &nlm::_ConfigWrapper::get_config,
             py::return_value_policy::reference_internal,
             "Get the internal C++ config (for advanced use)");

    py::class_<_BrainWrapper>(m, "Brain", R"pbdoc(Pythonic brain wrapper with context management)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create wrapper from existing brain")
        .def("__enter__", &nlm::_BrainWrapper::__enter__,
             "Context manager entry - initializes brain")
        .def("__exit__", &nlm::_BrainWrapper::__exit__,
             py::arg("type"), py::arg("value"), py::arg("traceback"),
             "Context manager exit - resets brain")
        .def("initialize", &nlm::_BrainWrapper::initialize,
             "Initialize brain (or reset if already initialized)")
        .def("step", &nlm::_BrainWrapper::step,
             py::arg("step_num"), py::arg("time") = 0.0,
             "Perform a simulation step")
        .def("add_sensory_input", &nlm::_BrainWrapper::add_sensory_input, py::arg("input"),
             "Inject sensory input into the brain")
        .def("produce_action", &nlm::_BrainWrapper::produce_action,
             "Produce motor action based on neural activity")
        .def("save", &nlm::_BrainWrapper::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::_BrainWrapper::load, py::arg("filepath"),
             "Load brain state from file")
        .def_property_readonly("total_neuron_count", &nlm::_BrainWrapper::get_total_neuron_count,
                             "Total number of neurons in the brain")
        .def_property_readonly("total_synapse_count", &nlm::_BrainWrapper::get_total_synapse_count,
                             "Total number of synapses in the brain")
        .def_property_readonly("average_firing_rate", &nlm::_BrainWrapper::get_average_firing_rate,
                             "Average firing rate across all neurons")
        .def("get_brain", &nlm::_BrainWrapper::get_brain,
             py::return_value_policy::reference_internal,
             "Get the underlying C++ brain (for advanced use)")
        .def("__repr__", &nlm::_BrainWrapper::__repr__);

    py::class_<_ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(High-level experiment runner)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world") = nullptr, py::arg("agent_brain") = nullptr,
             "Create experiment runner with optional world and agent brain")
        .def("setup_quick", &nlm::_ExperimentRunner::setup_quick,
             py::arg("width") = 20, py::arg("height") = 20,
             py::arg("vision_width") = 5, py::arg("vision_height") = 5,
             "Quick setup with default vision and world configuration")
        .def("run_step", &nlm::_ExperimentRunner::run_step,
             py::arg("step_num"), py::arg("timestep") = 0.1,
             "Run a single simulation step and return simulation time")
        .def("run_episode", &nlm::_ExperimentRunner::run_episode,
             py::arg("steps"), py::arg("timestep") = 0.1,
             "Run a complete episode (sequence of steps)")
        .def("get_stats", &nlm::_ExperimentRunner::get_stats,
             "Get current simulation statistics")
        .def("get_brain", &nlm::_ExperimentRunner::get_brain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("get_world", &nlm::_ExperimentRunner::get_world,
             py::return_value_policy::reference_internal,
             "Get the world")
        .def("get_agent_brain", &nlm::_ExperimentRunner::get_agent_brain,
             py::return_value_policy::reference_internal,
             "Get the agent brain")
        .def("__repr__", &nlm::_ExperimentRunner::__repr__);

    // Original Config class (maintained for backward compatibility)
    py::class_<Config>(m, "Config", R"pbdoc(Low-level C++ configuration class)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Original SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Original Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Neural simulation brain)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // Original AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Keep backward compatibility - createConfig alias
    m.def("createDefaultConfig", &create_default_config,
          py::return_value_policy::take_ownership,
          "Create a default configuration (backward compatibility)");
    
    m.def("createBrain", &create_brain, py::arg("config"),
          py::return_value_policy::take_ownership,
          "Create a new brain with configuration (backward compatibility)");
    
    m.def("createSimpleWorld", &create_simple_world,
          py::return_value_policy::take_ownership,
          "Create a new simple world (backward compatibility)");
    
    m.def("createAgentBrain", &create_agent_brain, py::arg("brain"),
          py::return_value_policy::take_ownership,
          "Create a new agent brain interface (backward compatibility)");

    // Constants for backward compatibility
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Add Pythonic properties to original classes
    py::class_<Config>(m, "Config_Original", R"pbdoc(Low-level Config with Pythonic properties)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Convenience functions for common operations
    m.def("brain_context", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<_BrainWrapper>(brain);
    }, py::arg("brain"),
          "Create a brain context manager for safe resource management");
}

} // namespace nlm
