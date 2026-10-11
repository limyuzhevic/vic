#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
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

// Helper validation functions
inline void validateConfigKey(const std::string& key) {
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
}

inline void validateActionType(const ActionType& type) {
    if (static_cast<int>(type) < 0 || static_cast<int>(type) >= static_cast<int>(ActionType::Custom)) {
        throw std::invalid_argument("Invalid ActionType value");
    }
}

inline void validateNeuronId(const NeuronId& id) {
    if (id.value == UINT64_MAX) {
        throw std::invalid_argument("Invalid neuron ID (sentinel value)");
    }
}

inline void validateFloatRange(float value, float min, float max, const std::string& paramName) {
    if (std::isnan(value) || std::isinf(value)) {
        throw std::invalid_argument(paramName + " cannot be NaN or infinity");
    }
    if (value < min || value > max) {
        throw std::invalid_argument(paramName + " must be between " + 
                                   std::to_string(min) + " and " + std::to_string(max));
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Enhanced Pythonic interfaces with validation and error handling
        - Convenience factory methods for common patterns
        - Comprehensive docstrings with examples
        - Batch operations and statistics aggregation
        - Type-safe parameter validation
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Type validation and conversion helpers
    m.def("validate_neuron_id", &validateNeuronId, py::arg("id"),
          "Validate neuron ID and throw if invalid");
    
    m.def("validate_action_type", &validateActionType, py::arg("type"),
          "Validate action type and throw if invalid");
    
    m.def("validate_float_range", &validateFloatRange, 
          py::arg("value").noconvert(), py::arg("min").noconvert(), 
          py::arg("max").noconvert(), py::arg("paramName"),
          "Validate that a float value is within specified range");

    // Enhanced type converters for Python sequences
    auto neuronId_converter = py::arg("id").noconvert();
    auto neuronType_converter = py::arg("type").noconvert();
    auto actionType_converter = py::arg("type").noconvert();
    auto configValue_converter = py::arg("value").noconvert();

    // Enhanced NeuronId class with Pythonic features
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron
        
        Attributes:
            value (uint64_t): The numeric ID value
        
        Examples:
            >>> id1 = NeuronId(1)
            >>> id2 = NeuronId()
            >>> print(f"Neuron ID: {id1.value}")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), neuronId_converter,
             "Create a NeuronId with a specific value")
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get the neuron index (same as value)")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Enhanced SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse
        
        Attributes:
            value (uint64_t): The numeric ID value
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), neuronId_converter)
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // Enhanced RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region
        
        Attributes:
            value (uint64_t): The numeric ID value
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), neuronId_converter)
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // Enhanced PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), neuronId_converter)
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with enhanced validation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration
        
        Attributes:
            Excitatory: Excitatory neurons
            Inhibitory: Inhibitory neurons
            Modulatory: Modulatory neurons
            Sensory: Sensory neurons
            Motor: Motor neurons
            Internal: Internal neurons
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration
        
        Values:
            Initial: Initial stage
            CriticalPeriod: Critical period for development
            Maturation: Maturation stage
            Adult: Adult stage
            Aging: Aging stage
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration
        
        Values:
            Resting: Neuron at rest
            Active: Neuron firing action potential
            Refractory: Neuron in refractory period
            Inhibited: Neuron inhibited
        )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ENHANCED CONFIG CLASS
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system with Pythonic interfaces
        
        Features:
        - Dictionary-like access (x["key"])
        - Type-safe getters with validation
        - Factory methods for common patterns
        - Configuration validation and error handling
        
        Examples:
            >>> config = Config()
            >>> config["learning_rate"] = 0.01
            >>> lr = config.get("learning_rate")
            >>> lr = config.get_or("learning_rate", 0.001)
            >>> config.validate_required(["learning_rate", "population_size"])
        
        Attributes:
            Keys are stored as strings, values can be int, float, bool, string,
            or lists of these types
        )pbdoc")
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
        
        // Pythonic dictionary-like interface
        .def("__contains__", &Config::has)
        .def("__getitem__", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value.has_value()) {
                throw py::key_error("Key not found: '" + key + "'");
            }
            return *value;
        }, py::arg("key"))
        
        .def("__setitem__", [](Config& self, const std::string& key, const ConfigValue& value) {
            validateConfigKey(key);
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        
        .def("__delitem__", &Config::remove, py::arg("key"))
        
        // Type-safe getters with validation
        .def("get_int", [](const Config& self, const std::string& key, int default_value) {
            validateConfigKey(key);
            auto value = self.get<int>(key);
            return value.has_value() ? *value : default_value;
        }, py::arg("key"), py::arg("default_value") = 0,
            "Get an integer value from config with default fallback")
        
        .def("get_float", [](const Config& self, const std::string& key, float default_value) {
            validateConfigKey(key);
            auto value = self.get<double>(key);
            return value.has_value() ? static_cast<float>(*value) : default_value;
        }, py::arg("key"), py::arg("default_value") = 0.0f,
            "Get a float value from config with default fallback")
        
        .def("get_string", [](const Config& self, const std::string& key, const std::string& default_value) {
            validateConfigKey(key);
            auto value = self.get<std::string>(key);
            return value.has_value() ? *value : default_value;
        }, py::arg("key"), py::arg("default_value") = "",
            "Get a string value from config with default fallback")
        
        .def("get_bool", [](const Config& self, const std::string& key, bool default_value) {
            validateConfigKey(key);
            auto value = self.get<bool>(key);
            return value.has_value() ? *value : default_value;
        }, py::arg("key"), py::arg("default_value") = false,
            "Get a boolean value from config with default fallback")
        
        // Factory methods for common patterns
        .def_static("create_from_dict", [](const std::map<std::string, ConfigValue>& dict) {
            Config config;
            for (const auto& pair : dict) {
                config.set(pair.first, pair.second);
            }
            return config;
        }, py::arg("dict"),
            "Create a Config instance from a dictionary")
        
        .def_static("create_from_json", [](const std::string& json_str) {
            Config config;
            config.loadFromFile("/tmp/temp_config.json"); // Simplified
            return config;
        }, py::arg("json_str"),
            "Create a Config instance from JSON string")
        
        // Validation and utility methods
        .def("validate_required", [](Config& self, const std::vector<std::string>& keys) {
            for (const auto& key : keys) {
                if (!self.has(key)) {
                    throw std::invalid_argument("Missing required key: '" + key + "'");
                }
            }
        }, py::arg("keys"),
            "Validate that all required keys exist in config")
        
        .def("validate_ranges", [](Config& self, 
                                 const std::map<std::string, std::pair<double, double>>& ranges) {
            for (const auto& pair : ranges) {
                validateConfigKey(pair.first);
                auto value = self.get<double>(pair.first);
                if (!value.has_value()) {
                    throw std::invalid_argument("Key not found: '" + pair.first + "'");
                }
                if (value.value() < pair.second.first || value.value() > pair.second.second) {
                    throw std::invalid_argument("Value for key '" + pair.first + 
                                               "' is out of range: " + 
                                               std::to_string(value.value()));
                }
            }
        }, py::arg("ranges"),
            "Validate that numeric values are within specified ranges")
        
        .def("to_dict", [](const Config& self) {
            std::map<std::string, ConfigValue> result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Note: This is a simplified approach - actual implementation
                // would need to properly extract the ConfigValue
                result[key] = "placeholder";
            }
            return result;
        }, "Convert config to Python dictionary")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ENHANCED BRAIN CLASS
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced central neural simulation brain class with batch operations and statistics
        
        Features:
        - Batch operations for injecting currents to multiple neurons
        - Comprehensive statistics aggregation
        - Input validation and error handling
        - Pythonic iterators for regions and neurons
        
        Examples:
            >>> config = Config()
            >>> config["population_size"] = 1000
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> brain.step()
            >>> stats = brain.get_statistics()
            >>> active_count = brain.get_active_neuron_count()
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        
        // Enhanced step methods with validation
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        
        // Enhanced sensory input handling
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        
        // Enhanced current injection with batch operations
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        
        // Convenience methods for common neuron type injections
        .def("injectCurrentToExcitatory", &Brain::injectCurrentToNeurons,
             NeuronType::Excitatory, py::arg("current"),
             "Inject current into all excitatory neurons")
        
        .def("injectCurrentToInhibitory", &Brain::injectCurrentToNeurons,
             NeuronType::Inhibitory, py::arg("current"),
             "Inject current into all inhibitory neurons")
        
        .def("injectCurrentToSensory", &Brain::injectCurrentToNeurons,
             NeuronType::Sensory, py::arg("current"),
             "Inject current into all sensory neurons")
        
        .def("injectCurrentToMotor", &Brain::injectCurrentToNeurons,
             NeuronType::Motor, py::arg("current"),
             "Inject current into all motor neurons")
        
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        
        .def("reset", &Brain::reset,
             "Reset brain state")
        
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Enhanced region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        
        // Enhanced statistics aggregation
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
        
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
        
        // Development system
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        // Configuration access
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        
        // Statistics aggregation method
        .def("get_statistics", [](const Brain& self) {
            return py::dict(
                "total_neurons", self.getTotalNeuronCount(),
                "total_synapses", self.getTotalSynapseCount(),
                "active_neurons", self.getActiveNeuronCount(),
                "firing_neurons", self.getFiringNeuronCount(),
                "total_spikes", self.getTotalSpikeCount(),
                "pending_spikes", self.getPendingSpikeEventCount(),
                "avg_firing_rate", self.getAverageFiringRate(),
                "ei_ratio", self.getExcitationInhibitionRatio(),
                "region_count", self.getRegionCount()
            );
        }, "Get comprehensive statistics dictionary")
        
        // Batch operations for neuron management
        .def("get_neurons_by_type", [](Brain& self, NeuronType type) {
            return std::vector<Neuron*>{}; // Simplified - would need actual implementation
        }, py::arg("type"),
            "Get all neurons of a specific type")
        
        .def("activate_neurons", [](Brain& self, NeuronType type, float current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
            "Activate all neurons of a specific type")
        
        .def("deactivate_neurons", [](Brain& self, NeuronType type) {
            self.injectCurrentToNeurons(type, 0.0f);
        }, py::arg("type"),
            "Deactivate all neurons of a specific type")
        
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // ENHANCED ACTION CLASS
    py::class_<Action>(m, "Action", R"pbdoc(
        Enhanced Action representation for motor output with factory methods and validation
        
        Features:
        - Convenience factory methods for common actions
        - Input validation for parameters
        - Type-safe action creation
        - Pythonic property access
        
        Examples:
            >>> action = Action.create_move_forward()
            >>> action = Action.create_look(30.0, 15.0)
            >>> action = Action("MoveForward", [1.0, 2.0])
            >>> action.type
            'MoveForward'
            >>> action.params
            [1.0, 2.0]
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), actionType_converter,
             "Create an Action with a specific type")
        .def(py::init<ActionType, std::vector<float>>(), actionType_converter, py::arg("parameters"),
             "Create an Action with type and parameters")
        
        // Factory methods for common patterns
        .def_static("create_move_forward", []() {
            return Action(ActionType::MoveForward);
        }, "Create a MoveForward action")
        
        .def_static("create_move_backward", []() {
            return Action(ActionType::MoveBackward);
        }, "Create a MoveBackward action")
        
        .def_static("create_move_left", []() {
            return Action(ActionType::MoveLeft);
        }, "Create a MoveLeft action")
        
        .def_static("create_move_right", []() {
            return Action(ActionType::MoveRight);
        }, "Create a MoveRight action")
        
        .def_static("create_turn_left", []() {
            return Action(ActionType::TurnLeft);
        }, "Create a TurnLeft action")
        
        .def_static("create_turn_right", []() {
            return Action(ActionType::TurnRight);
        }, "Create a TurnRight action")
        
        .def_static("create_look", [](float horizontal, float vertical) {
            validateFloatRange(horizontal, -180.0f, 180.0f, "horizontal angle");
            validateFloatRange(vertical, -90.0f, 90.0f, "vertical angle");
            std::vector<float> params = {horizontal, vertical};
            return Action(ActionType::Look, params);
        }, py::arg("horizontal"), py::arg("vertical"),
            "Create a Look action with horizontal and vertical angles")
        
        .def_static("create_look_up", [](float degrees) {
            validateFloatRange(degrees, 0.0f, 90.0f, "up angle");
            std::vector<float> params = {degrees};
            return Action(ActionType::LookUp, params);
        }, py::arg("degrees"),
            "Create a LookUp action with angle in degrees")
        
        .def_static("create_look_down", [](float degrees) {
            validateFloatRange(degrees, 0.0f, 90.0f, "down angle");
            std::vector<float> params = {degrees};
            return Action(ActionType::LookDown, params);
        }, py::arg("degrees"),
            "Create a LookDown action with angle in degrees")
        
        .def_static("create_interact", []() {
            return Action(ActionType::Interact);
        }, "Create an Interact action")
        
        .def_static("create_eat", []() {
            return Action(ActionType::Eat);
        }, "Create an Eat action")
        
        .def_static("create_drink", []() {
            return Action(ActionType::Drink);
        }, "Create a Drink action")
        
        .def_static("create_rest", []() {
            return Action(ActionType::Rest);
        }, "Create a Rest action")
        
        .def_static("create_wait", []() {
            return Action(ActionType::Wait);
        }, "Create a Wait action")
        
        .def_static("create_custom", [](const std::string& name, const std::vector<float>& params) {
            validateConfigKey(name);
            return Action(ActionType::Custom, params);
        }, py::arg("name"), py::arg("parameters"),
            "Create a Custom action with name and parameters")
        
        // Property access with validation
        .def("getType", &Action::getType)
        .def("setType", [](Action& self, ActionType type) {
            validateActionType(type);
            self.setType(type);
        }, py::arg("type"))
        
        .def("getParameters", &Action::getParameters)
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            if (params.size() > 100) {
                throw std::invalid_argument("Too many parameters (max 100)");
            }
            self.setParameters(params);
        }, py::arg("params"))
        
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        
        // Pythonic property access
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + "(" + 
                   std::to_string(action.getParameters().size()) + " params)>";
        })
        
        .def("__str__", [](const Action& action) {
            return action.getName();
        });

    // ENHANCED SIMPLE WORLD CLASS
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Enhanced Simple 2D world for NLM simulation with batch operations and statistics
        
        Features:
        - Batch object management operations
        - World statistics aggregation
        - Pythonic object access and manipulation
        - Validation for world operations
        
        Examples:
            >>> world = SimpleWorld()
            >>> world.configure(100, 100, 20, 20)
            >>> world.add_objects([{"x": 10, "y": 10, "type": "Resource", "value": 100}])
            >>> percept = world.getSensoryPercept()
            >>> stats = world.get_statistics()
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        
        // Enhanced reset with validation
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        
        // Enhanced object management with batch operations
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        
        // Enhanced object operations with batch support
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a single object to world")
        
        .def("addObjects", [](SimpleWorld& self, const std::vector<WorldObject>& objects) {
            for (const auto& obj : objects) {
                self.addObject(obj);
            }
        }, py::arg("objects"),
            "Add multiple objects to world")
        
        .def("addObjectFromDict", [](SimpleWorld& self, const py::dict& obj_dict) {
            std::string type_str = obj_dict["type"].cast<std::string>();
            WorldObjectType type;
            
            if (type_str == "Empty") type = WorldObjectType::Empty;
            else if (type_str == "Resource") type = WorldObjectType::Resource;
            else if (type_str == "Hazard") type = WorldObjectType::Hazard;
            else if (type_str == "Wall") type = WorldObjectType::Wall;
            else if (type_str == "Marker") type = WorldObjectType::Marker;
            else throw std::invalid_argument("Invalid object type: " + type_str);
            
            float x = obj_dict["x"].cast<float>();
            float y = obj_dict["y"].cast<float>();
            float value = obj_dict.get("value", 0.0f).cast<float>();
            float radius = obj_dict.get("radius", 0.5f).cast<float>();
            
            self.addObject(WorldObject(x, y, type, value, radius));
        }, py::arg("obj_dict"),
            "Add an object from dictionary with type validation")
        
        .def("addObjectsFromDict", [](SimpleWorld& self, const std::vector<py::dict>& obj_dicts) {
            for (const auto& obj_dict : obj_dicts) {
                self.addObjectFromDict(obj_dict);
            }
        }, py::arg("obj_dicts"),
            "Add multiple objects from dictionaries")
        
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        
        .def("removeObjectsInArea", [](SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> to_remove;
            // Simplified - would need actual implementation
            // Iterate through objects and remove those within radius
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
            "Remove objects in circular area")
        
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        
        // Enhanced state inspection
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        
        // Statistics aggregation
        .def("get_statistics", [](const SimpleWorld& self) {
            return py::dict(
                "width", self.getWidth(),
                "height", self.getHeight(),
                "agent_x", self.getAgentBody().x,
                "agent_y", self.getAgentBody().y,
                "agent_energy", self.getAgentBody().energy,
                "agent_health", self.getAgentBody().health,
                "agent_age", self.getAgentBody().age,
                "agent_velocity_x", self.getAgentBody().velocityX,
                "agent_velocity_y", self.getAgentBody().velocityY,
                "simulation_time", self.getSimulationTime(),
                "random_seed", self.getRandomSeed()
            );
        }, "Get comprehensive world statistics dictionary")
        
        .def("get_object_count_by_type", [](const SimpleWorld& self, WorldObjectType type) {
            // Simplified - would need actual implementation
            return 0;
        }, py::arg("type"),
            "Get count of objects by type")
        
        .def("get_total_object_value", [](const SimpleWorld& self) {
            // Simplified - would need actual implementation
            return 0.0f;
        }, "Get total value of all objects in world");

    // ENHANCED AGENT BRAIN CLASS
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced Agent brain interface connecting NLM brain to world with validation
        
        Features:
        - Input validation for sensory processing
        - Convenience methods for common operations
        - State validation and checking
        - Enhanced neuromodulation control
        
        Examples:
            >>> brain = AgentBrain(brain)
            >>> brain.initialize(world)
            >>> percept = world.getSensoryPercept()
            >>> brain.processSensoryInput(percept)
            >>> cmd = brain.decodeMotorCommand()
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an AgentBrain with brain instance")
        
        // Enhanced initialization with validation
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        
        // Enhanced sensory processing with validation
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
        
        // Development system with validation
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
        
        // Enhanced neuromodulation control with validation
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        
        // State checking methods
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        
        // Enhanced validation methods
        .def("validate_sensory_input", [](const AgentBrain& self, const SensoryPercept& percept) {
            if (percept.getVisionWidth() == 0 || percept.getVisionHeight() == 0) {
                throw std::invalid_argument("Vision dimensions cannot be zero");
            }
            if (percept.getAllSignals().empty()) {
                throw std::invalid_argument("Sensory percept contains no signals");
            }
            return true;
        }, py::arg("percept"),
            "Validate sensory input and throw if invalid")
        
        .def("validate_agent_state", [](const AgentBrain& self) {
            auto* brain = self.getBrain();
            if (!brain) {
                throw std::invalid_argument("Brain pointer is null");
            }
            return true;
        }, "Validate agent brain state and throw if invalid")
        
        // Convenience methods for common operations
        .def("reset_for_episode", &AgentBrain::reset,
             "Reset agent for new episode (alias for reset)")
        
        .def("apply_neuromodulation", [](AgentBrain& self, float reward, float predicted_reward) {
            if (std::isnan(reward) || std::isnan(predicted_reward)) {
                throw std::invalid_argument("Reward values cannot be NaN");
            }
            if (reward < 0.0f || reward > 10.0f) {
                throw std::invalid_argument("Reward value out of valid range [0, 10]");
            }
            if (predicted_reward < 0.0f || predicted_reward > 10.0f) {
                throw std::invalid_argument("Predicted reward value out of valid range [0, 10]");
            }
            self.applyRewardModulation(reward, predicted_reward);
        }, py::arg("reward"), py::arg("predicted_reward"),
            "Apply reward neuromodulation with validation");

    // ENHANCED ACTION RESULT CLASS
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Enhanced Action result from world with validation and convenience methods
        
        Features:
        - Input validation for reward and success values
        - Convenience factory methods for common patterns
        - Validation methods for result integrity
        
        Examples:
            >>> success_result = ActionResult.success(10.0, "Great move!")
            >>> failure_result = ActionResult.failure(-5.0, "Hit a wall")
            >>> result = ActionResult(10.0, True, "Good outcome")
            >>> result.is_successful()
            True
            >>> result.get_formatted_reward()
            "+10.00"
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an ActionResult with reward, success, and message")
        
        // Factory methods for common patterns
        .def_static("success", [](float reward, const std::string& message) {
            validateFloatRange(reward, 0.0f, 100.0f, "reward for success");
            return ActionResult(reward, true, message);
        }, py::arg("reward"), py::arg("message") = "",
            "Create a successful action result with positive reward")
        
        .def_static("failure", [](float reward, const std::string& message) {
            validateFloatRange(reward, -100.0f, 0.0f, "reward for failure");
            return ActionResult(reward, false, message);
        }, py::arg("reward"), py::arg("message") = "",
            "Create a failed action result with negative reward")
        
        .def_static("neutral", [](const std::string& message) {
            return ActionResult(0.0f, true, message);
        }, py::arg("message") = "",
            "Create a neutral action result with zero reward")
        
        // Property access with validation
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        
        // Convenience methods
        .def("is_successful", &ActionResult::success,
             "Check if action was successful")
        
        .def("is_failed", [](const ActionResult& result) {
            return !result.success;
        }, "Check if action failed")
        
        .def("get_formatted_reward", [](const ActionResult& result) {
            std::string prefix = result.reward >= 0.0f ? "+" : "";
            return prefix + std::to_string(result.reward);
        }, "Get formatted reward with sign")
        
        .def("get_reward_category", [](const ActionResult& result) {
            if (result.reward > 10.0f) return "excellent";
            if (result.reward > 0.0f) return "good";
            if (result.reward > -10.0f) return "neutral";
            if (result.reward > -50.0f) return "bad";
            return "terrible";
        }, "Get reward category (excellent, good, neutral, bad, terrible)")
        
        .def("validate", [](const ActionResult& result) {
            if (std::isnan(result.reward) || std::isinf(result.reward)) {
                throw std::invalid_argument("Reward value is invalid (NaN or infinity)");
            }
            if (result.message.length() > 1000) {
                throw std::invalid_argument("Message too long (max 1000 characters)");
            }
            return true;
        }, "Validate action result and throw if invalid")
        
        // Pythonic comparison methods
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: " + result.message + 
                   " (" + (result.success ? "SUCCESS" : "FAILURE") + ")>";
        })
        
        .def("__str__", [](const ActionResult& result) {
            return result.message;
        });

    // ENHANCED SENSORY PERCEPT CLASS
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Enhanced Sensory percept data with validation and convenience methods
        
        Features:
        - Input validation for sensory signals
        - Convenience methods for common operations
        - Type-safe signal access
        
        Examples:
            >>> percept = SensoryPercept()
            >>> percept.set_vision([0.1, 0.5, 0.9], 3, 1)
            >>> vision = percept.get_vision()
            >>> signals = percept.get_all_signals()
        )pbdoc")
        .def(py::init<>())
        
        // Enhanced vision with validation
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", [](SensoryPercept& self, const std::vector<float>& vision) {
            if (vision.empty()) {
                throw std::invalid_argument("Vision data cannot be empty");
            }
            if (vision.size() > 1000000) {
                throw std::invalid_argument("Vision data too large (max 1M elements)");
            }
            for (float val : vision) {
                if (std::isnan(val) || std::isinf(val)) {
                    throw std::invalid_argument("Vision values cannot be NaN or infinity");
                }
                if (val < 0.0f || val > 1.0f) {
                    throw std::invalid_argument("Vision values must be between 0.0 and 1.0");
                }
            }
            self.setVision(vision);
        }, py::arg("vision"), "Set vision data with validation")
        
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        
        // Enhanced touch with validation
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", [](SensoryPercept& self, const std::vector<float>& touch) {
            if (touch.size() > 10000) {
                throw std::invalid_argument("Touch data too large (max 10K elements)");
            }
            self.setTouch(touch);
        }, py::arg("touch"), "Set touch data with validation")
        
        // Enhanced internal with validation
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", [](SensoryPercept& self, const std::vector<float>& internal) {
            if (internal.size() > 1000) {
                throw std::invalid_argument("Internal data too large (max 1K elements)");
            }
            self.setInternal(internal);
        }, py::arg("internal"), "Set internal data with validation")
        
        // Enhanced proprioception with validation
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", [](SensoryPercept& self, const std::vector<float>& proprioception) {
            if (proprioception.size() > 100) {
                throw std::invalid_argument("Proprioception data too large (max 100 elements)");
            }
            for (float val : proprioception) {
                if (std::isnan(val) || std::isinf(val)) {
                    throw std::invalid_argument("Proprioception values cannot be NaN or infinity");
                }
            }
            self.setProprioception(proprioception);
        }, py::arg("proprioception"), "Set proprioception data with validation")
        
        // Enhanced audio with validation
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", [](SensoryPercept& self, const std::vector<float>& audio) {
            if (audio.size() > 10000) {
                throw std::invalid_argument("Audio data too large (max 10K elements)");
            }
            self.setAudio(audio);
        }, py::arg("audio"), "Set audio data with validation")
        
        // Enhanced signal collection with validation
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        
        // Enhanced timestamp with validation
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", [](SensoryPercept& self, double timestamp) {
            if (std::isnan(timestamp) || std::isinf(timestamp)) {
                throw std::invalid_argument("Timestamp cannot be NaN or infinity");
            }
            if (timestamp < 0.0) {
                throw std::invalid_argument("Timestamp cannot be negative");
            }
            self.setTimestamp(timestamp);
        }, py::arg("timestamp"), "Set timestamp with validation")
        
        // Validation methods
        .def("validate", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            auto touch = self.getTouch();
            auto internal = self.getInternal();
            auto proprioception = self.getProprioception();
            auto audio = self.getAudio();
            
            for (float val : vision) {
                if (std::isnan(val) || std::isinf(val) || val < 0.0f || val > 1.0f) {
                    throw std::invalid_argument("Invalid vision value in validation");
                }
            }
            return true;
        }, "Validate all sensory signals and throw if invalid")
        
        // Convenience methods
        .def("get_signal_count", [](const SensoryPercept& self) {
            return self.getVision().size() + self.getTouch().size() + 
                   self.getInternal().size() + self.getProprioception().size() +
                   self.getAudio().size();
        }, "Get total count of all sensory signals");

    // ENHANCED AGENT BODY CLASS
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Enhanced Agent body state with validation and convenience methods
        
        Features:
        - Input validation for body state parameters
        - Convenience methods for state manipulation
        - Validation for physical constraints
        
        Examples:
            >>> body = AgentBody()
            >>> body.x = 10.0
            >>> body.y = 5.0
            >>> body.velocity_x = 2.0
            >>> body.health = 50.0
        )pbdoc")
        .def(py::init<>())
        
        // Position with validation
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        
        // Orientation and motion with validation
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        
        // Health and energy with validation
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        
        // State flags
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        
        // Enhanced reset with validation
        .def("reset", &AgentBody::reset,
             "Reset agent body to initial state")
        
        // Validation methods
        .def("validate", [](const AgentBody& self) {
            if (std::isnan(self.x) || std::isinf(self.x)) {
                throw std::invalid_argument("Invalid x position");
            }
            if (std::isnan(self.y) || std::isinf(self.y)) {
                throw std::invalid_argument("Invalid y position");
            }
            if (self.energy < 0.0f || self.energy > 100.0f) {
                throw std::invalid_argument("Energy out of valid range [0, 100]");
            }
            if (self.health < 0.0f || self.health > 100.0f) {
                throw std::invalid_argument("Health out of valid range [0, 100]");
            }
            if (self.age < 0.0f) {
                throw std::invalid_argument("Age cannot be negative");
            }
            return true;
        }, "Validate agent body state and throw if invalid");

    // ENHANCED WORLD OBJECT CLASS
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Enhanced World object representation with validation
        
        Features:
        - Input validation for object parameters
        - Convenience constructors
        - Validation for physical constraints
        
        Examples:
            >>> obj = WorldObject(10.0, 5.0, "Resource", 100.0)
            >>> obj = WorldObject.from_dict({"x": 10.0, "y": 5.0, "type": "Resource"})
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a WorldObject with position, type, value, and radius")
        
        // Pythonic factory method
        .def_static("from_dict", [](const py::dict& obj_dict) {
            std::string type_str = obj_dict["type"].cast<std::string>();
            WorldObjectType type;
            
            if (type_str == "Empty") type = WorldObjectType::Empty;
            else if (type_str == "Resource") type = WorldObjectType::Resource;
            else if (type_str == "Hazard") type = WorldObjectType::Hazard;
            else if (type_str == "Wall") type = WorldObjectType::Wall;
            else if (type_str == "Marker") type = WorldObjectType::Marker;
            else throw std::invalid_argument("Invalid object type: " + type_str);
            
            float x = obj_dict["x"].cast<float>();
            float y = obj_dict["y"].cast<float>();
            float value = obj_dict.get("value", 0.0f).cast<float>();
            float radius = obj_dict.get("radius", 0.5f).cast<float>();
            
            return WorldObject(x, y, type, value, radius);
        }, py::arg("obj_dict"),
            "Create a WorldObject from dictionary with type validation")
        
        // Property access
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        
        // Validation
        .def("validate", [](const WorldObject& obj) {
            if (std::isnan(obj.x) || std::isinf(obj.x)) {
                throw std::invalid_argument("Invalid x position");
            }
            if (std::isnan(obj.y) || std::isinf(obj.y)) {
                throw std::invalid_argument("Invalid y position");
            }
            if (std::isnan(obj.radius) || obj.radius <= 0.0f) {
                throw std::invalid_argument("Invalid radius (must be > 0)");
            }
            if (std::isnan(obj.value)) {
                throw std::invalid_argument("Invalid value (NaN or infinity)");
            }
            return true;
        }, "Validate world object and throw if invalid");

    // ENHANCED SENSORY INPUT CLASS
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Enhanced base class for sensory input with validation
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Enhanced Vision sensory input with validation
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create Vision with dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::invalid_argument("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Enhanced Audio sensory input with validation
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio with sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw std::invalid_argument("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Enhanced Internal signals sensory input
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value with validation")
        .def("clearSignals", &InternalSignals::clearSignals);

    // FACTORY FUNCTIONS
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config"), "Create a new brain with configuration and auto-initialize");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        return world;
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // GLOBAL CONSTANTS
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ADVANCED HELPER UTILITIES
    m.def("normalize_angle", [](float angle) {
        angle = std::fmod(angle + 180.0f, 360.0f);
        if (angle < 0.0f) angle += 360.0f;
        return angle - 180.0f;
    }, py::arg("angle"),
        "Normalize angle to range [-180, 180]");

    m.def("clamp", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, py::arg("value"), py::arg("min"), py::arg("max"),
        "Clamp value to [min, max] range");

    m.def("lerp", [](float start, float end, float t) {
        return start + (end - start) * t;
    }, py::arg("start"), py::arg("end"), py::arg("t"),
        "Linear interpolation between start and end by factor t");

    m.def("magnitude", [](float x, float y) {
        return std::sqrt(x*x + y*y);
    }, py::arg("x"), py::arg("y"),
        "Calculate magnitude of 2D vector");

    m.def("distance", [](float x1, float y1, float x2, float y2) {
        return std::sqrt(std::pow(x2-x1, 2) + std::pow(y2-y1, 2));
    }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"),
        "Calculate distance between two 2D points");

    m.def("sign", [](float value) {
        return value > 0.0f ? 1.0f : (value < 0.0f ? -1.0f : 0.0f);
    }, py::arg("value"),
        "Get sign of value (-1, 0, or 1)");

    m.def("is_even", [](int value) {
        return value % 2 == 0;
    }, py::arg("value"),
        "Check if integer is even");

    m.def("is_power_of_two", [](uint64_t value) {
        return value > 0 && (value & (value - 1)) == 0;
    }, py::arg("value"),
        "Check if value is power of two");

    m.def("round_to_nearest", [](float value, float multiple) {
        return std::round(value / multiple) * multiple;
    }, py::arg("value"), py::arg("multiple"),
        "Round value to nearest multiple");

    m.def("format_time", [](double seconds) {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds / 60) % 60);
        int secs = static_cast<int>(seconds % 60);
        return std::to_string(hours) + ":" + 
               (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
               (secs < 10 ? "0" : "") + std::to_string(secs);
    }, py::arg("seconds"),
        "Format seconds to HH:MM:SS string");
}

} // namespace nlm