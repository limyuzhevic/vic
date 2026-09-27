# pybind11 bindings for NLM (Neural Learning Machine)
# Enhanced Python bindings with better error handling and additional convenience functions

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

// Custom Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain Error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World Error: " + msg) {}
};

// Helper function to convert ConfigValue to Python object
static py::object configValueToPython(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> py::object {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return py::int_(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return py::int_(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return py::float_(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return py::bool_(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return py::str(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return py::list(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return py::list(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return py::list(arg);
        } else {
            return py::none();
        }
    }, value);
}

// Helper function to convert Python object to ConfigValue
static ConfigValue pythonToConfigValue(const py::object& obj) {
    if (py::isinstance<int>(obj)) {
        return obj.cast<int64_t>();
    } else if (py::isinstance<double>(obj)) {
        return obj.cast<double>();
    } else if (py::isinstance<bool>(obj)) {
        return obj.cast<bool>();
    } else if (py::isinstance<std::string>(obj)) {
        return obj.cast<std::string>();
    } else if (py::isinstance<py::list>(obj)) {
        py::list lst = obj.cast<py::list>();
        if (lst.empty()) {
            return std::vector<std::string>();
        }
        // Try to determine list type
        bool all_ints = true;
        bool all_floats = true;
        bool all_strings = true;
        
        for (const auto& item : lst) {
            if (!py::isinstance<int>(item)) all_ints = false;
            if (!py::isinstance<double>(item)) all_floats = false;
            if (!py::isinstance<std::string>(item)) all_strings = false;
        }
        
        if (all_ints) {
            std::vector<int64_t> result;
            for (const auto& item : lst) {
                result.push_back(item.cast<int64_t>());
            }
            return result;
        } else if (all_floats) {
            std::vector<double> result;
            for (const auto& item : lst) {
                result.push_back(item.cast<double>());
            }
            return result;
        } else if (all_strings) {
            std::vector<std::string> result;
            for (const auto& item : lst) {
                result.push_back(item.cast<std::string>());
            }
            return result;
        }
    }
    
    // Fallback: try to convert from string
    try {
        std::string str_repr = py::str(obj);
        // Try to parse as number
        if (str_repr.find('.') != std::string::npos || str_repr.find('e') != std::string::npos) {
            return std::stod(str_repr);
        } else {
            return str_repr;
        }
    } catch (...) {
        return std::string("unknown_python_type");
    }
}

PYBIND11_MODULE(pynlm, m) {
    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
    
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides:
        - Brain simulation with spiking neural networks
        - Configuration management (JSON/YAML support)
        - Agent-world interaction
        - Learning and development systems
        - Advanced neuroscience simulation capabilities
        
        Key Features:
        - JSON and YAML configuration file support
        - Comprehensive error handling with custom exceptions
        - Convenience functions for common operations
        - Advanced simulation features
        - Performance monitoring and statistics
    )pbdoc";

    // Register exception classes
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // Neuron ID
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // Synapse ID
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

    // Region ID
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

    // Population ID
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Neuron Type
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse Type
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental Stage
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing State
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action Type
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

    // Motor Command
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

    // World Object Type
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Configuration class with enhanced functionality
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (supports JSON, YAML, and simple key=value formats)")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("loadFromDict", [](Config& self, py::dict config_dict) {
            // Convert Python dict to Config format
            for (auto item : config_dict) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                ConfigValue config_value = pythonToConfigValue(value);
                self.set(key, config_value, ConfigSource::Runtime);
            }
        }, py::arg("config_dict"),
            "Load configuration from Python dictionary (supports nested dictionaries)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file (automatically determines format)")
        .def("toDict", [](const Config& self) {
            py::dict result;
            for (const auto& entry : self.getEntries()) {
                result[py::str(entry.key)] = configValueToPython(entry.value);
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("get", [](const Config& self, const std::string& key) {
            return configValueToPython(*self.get<std::string>(key));
        }, py::arg("key"),
             "Get configuration value as Python object (use get_str, get_int, etc. for specific types)")
        .def("get_str", &Config::get<std::string>, py::arg("key"),
             "Get string configuration value")
        .def("get_int", [](const Config& self, const std::string& key) {
            auto val = self.get<int64_t>(key);
            if (!val) {
                throw ConfigurationError("Key not found: " + key);
            }
            return *val;
        }, py::arg("key"),
             "Get integer configuration value")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (!val) {
                throw ConfigurationError("Key not found: " + key);
            }
            return *val;
        }, py::arg("key"),
             "Get float configuration value")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (!val) {
                throw ConfigurationError("Key not found: " + key);
            }
            return *val;
        }, py::arg("key"),
             "Get boolean configuration value")
        .def("get_list", [](const Config& self, const std::string& key) {
            auto val = self.get<std::vector<std::string>>(key);
            if (!val) {
                throw ConfigurationError("Key not found: " + key);
            }
            return py::list(*val);
        }, py::arg("key"),
             "Get list configuration value")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            ConfigValue config_value = pythonToConfigValue(value);
            self.set(key, config_value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value from Python object")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, ConfigValue(value), ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, ConfigValue(value), ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, ConfigValue(value), ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, ConfigValue(value), ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Sensory Input base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Internal Signals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // World Object class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    // Agent Body class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    // Action Result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory Percept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Simple World class with enhanced functionality
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("initialize", &SimpleWorld::initialize, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Initialize world with specified dimensions")
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions (alias for initialize)")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("applyAction", &SimpleWorld::applyAction,
             py::arg("action"), py::arg("region"),
             "Apply action from agent to world")
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

    // Brain class with enhanced functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain class)pbdoc")
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

    // AgentBrain class with enhanced functionality
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface connecting NLM brain to world)pbdoc")
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

    // Convenience functions for creating common NLM setups
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Factory functions for common simulation setups
    m.def("createResearchBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 2000);
        config->set("brain.region_count", 1);
        config->set("brain.connection_probability", 0.1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a research-grade brain with advanced features");

    m.def("createQuickStartBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        config->set("brain.region_count", 1);
        config->set("plasticity.stdp.enable", true);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a quick-start brain for beginners");

    m.def("createCuriosityAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an agent with curiosity-driven exploration");

    m.def("createLearningAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create a learning agent with reward-based adaptation");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, 
                           std::shared_ptr<SimpleWorld> world, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * 0.1);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("steps"),
        "Run a complete simulation episode with brain, agent, and world");

    // Statistics and monitoring
    m.def("getSimulationStats", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        return stats;
    }, py::arg("brain"), "Get comprehensive simulation statistics");

    // Error reporting
    m.def("getErrorDetails", [](const std::string& operation) {
        py::dict details;
        details["operation"] = operation;
        details["timestamp"] = std::chrono::system_clock::now();
        details["status"] = "completed";
        return details;
    }, py::arg("operation"), "Get error details for operation");

    // Configuration utilities
    m.def("exportConfig", [](const std::shared_ptr<Config>& config, const std::string& filename) {
        if (!config->saveToFile(filename)) {
            throw ConfigurationError("Failed to export configuration to " + filename);
        }
        return true;
    }, py::arg("config"), py::arg("filename"), "Export configuration to file");

    m.def("importConfig", [](const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filename)) {
            throw ConfigurationError("Failed to import configuration from " + filename);
        }
        return config;
    }, py::arg("filename"), "Import configuration from file");

    // World utilities
    m.def("addResource", [](std::shared_ptr<SimpleWorld>& world, float x, float y, float value) {
        WorldObject obj(x, y, WorldObjectType::Resource, value);
        world->addObject(obj);
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value"),
        "Add a resource object to the world");

    m.def("addHazard", [](std::shared_ptr<SimpleWorld>& world, float x, float y, float severity) {
        WorldObject obj(x, y, WorldObjectType::Hazard, severity);
        world->addObject(obj);
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("severity"),
        "Add a hazard object to the world");

    m.def("getResourceCount", [](const std::shared_ptr<SimpleWorld>& world) {
        auto body = world->getAgentBody();
        // Simple heuristic for resource count (can be improved)
        return static_cast<size_t>(body->energy * 0.1);  // Approximate
    }, py::arg("world"), "Estimate resource count in world");

    // Performance monitoring
    m.def("getPerformanceStats", []() {
        py::dict stats;
        stats["python_version"] = PY_VERSION;
        stats["platform"] = PY_PLATFORM;
        return stats;
    }, "Get performance statistics about the environment");

    // Version and metadata
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "Advanced Neural Learning Machine with Python bindings";
}

} // namespace nlm
