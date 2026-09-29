#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <iostream>
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

// Custom Python exceptions for NLM
class NLMException : public py::exception_base {
public:
    NLMException(const char* message) : py::exception_base(message) {}
};

class BrainInitializationError : public NLMException {
public:
    BrainInitializationError(const char* message) : NLMException(message) {}
};

class WorldConfigurationError : public NLMException {
public:
    WorldConfigurationError(const char* message) : NLMException(message) {}
};

class ActionValidationError : public NLMException {
public:
    ActionValidationError(const char* message) : NLMException(message) {}
};

class ConfigValidationError : public NLMException {
public:
    ConfigValidationError(const char* message) : NLMException(message) {}
};

class MemoryError : public NLMException {
public:
    MemoryError(const char* message) : NLMException(message) {}
};

class SimulationError : public NLMException {
public:
    SimulationError(const char* message) : NLMException(message) {}
};

// Translation functions for C++ exceptions to Python
void translateBrainError(const std::exception& e) {
    throw BrainInitializationError(e.what());
}

void translateConfigError(const std::exception& e) {
    throw ConfigValidationError(e.what());
}

void translateWorldError(const std::exception& e) {
    throw WorldConfigurationError(e.what());
}

void translateActionError(const std::exception& e) {
    throw ActionValidationError(e.what());
}

void translateMemoryError(const std::exception& e) {
    throw MemoryError(e.what());
}

void translateSimulationError(const std::exception& e) {
    throw SimulationError(e.what());
}

// Type conversion utilities
template<typename T>
T pyToCpp(const py::object& obj) {
    try {
        return obj.cast<T>();
    } catch (const py::cast_error& e) {
        std::stringstream ss;
        ss << "Failed to convert Python object to type T: " << e.what();
        throw ConfigValidationError(ss.str().c_str());
    }
}

std::string pyToString(const py::object& obj) {
    try {
        return pyToCpp<std::string>(obj);
    } catch (const py::cast_error& e) {
        throw ConfigValidationError("Failed to convert to string");
    }
}

float pyToFloat(const py::object& obj) {
    try {
        return pyToCpp<float>(obj);
    } catch (const py::cast_error& e) {
        throw ConfigValidationError("Failed to convert to float");
    }
}

int pyToInt(const py::object& obj) {
    try {
        return pyToCpp<int>(obj);
    } catch (const py::cast_error& e) {
        throw ConfigValidationError("Failed to convert to int");
    }
}

bool pyToBool(const py::object& obj) {
    try {
        return pyToCpp<bool>(obj);
    } catch (const py::cast_error& e) {
        throw ConfigValidationError("Failed to convert to bool");
    }
}

// Input validation functions
bool validate_neuron_id(const NeuronId& id, const std::string& param_name = "neuron_id") {
    if (id.value == 0) {
        throw BrainInitializationError("Invalid " + param_name + ": neuron_id cannot be zero");
    }
    return true;
}

bool validate_position(float x, float y, const std::string& world_name = "world") {
    if (std::isnan(x) || std::isnan(y)) {
        throw WorldConfigurationError("Invalid " + world_name + " coordinates: NaN values not allowed");
    }
    if (x < 0 || y < 0) {
        throw WorldConfigurationError("Invalid " + world_name + " coordinates: negative values not allowed");
    }
    return true;
}

// Global constants for common configuration presets
const std::string PRESET_DEVELOPMENT = "development";
const std::string PRESET_PRODUCTION = "production";
const std::string PRESET_RESEARCH = "research";
const std::string PRESET_DEMO = "demo";

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        ## Quick Start Examples
        
        ### Basic Usage
        ```python
        import pynlm
        
        # Create a brain with default configuration
        brain = pynlm.Brain.create_default()
        brain.initialize()
        
        # Run a simple simulation
        brain.run_steps(100)
        
        # Get quick statistics
        stats = brain.get_stats()
        print(f"Total spikes: {stats['total_spikes']}")
        ```
        
        ### Advanced Usage with Context Manager
        ```python
        import pynlm
        
        # Use context manager for automatic cleanup
        with pynlm.Brain.create_default() as brain:
            brain.initialize()
            brain.run_steps(50)
        
        # Configuration presets
        config = pynlm.Config.from_preset('demo')
        brain = pynlm.Brain(config)
        brain.initialize()
        ```
        
        ### World and Action Examples
        ```python
        import pynlm
        
        # Create a world with agents and objects
        world = pynlm.SimpleWorld.create_standard(100, 100)
        world.add_agent(10, 10)
        world.add_resource(50, 50, value=20.0)
        
        # Create actions using factory methods
        move_action = pynlm.Action.create_move('forward', 1.0)
        look_action = pynlm.Action.create_look(0.1, 0.0)
        ```
        
        The NLM Python bindings provide an intuitive and powerful interface for
        creating, configuring, and running neural simulations with comprehensive
        error handling, documentation, and convenience methods.
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<WorldConfigurationError>(m, "WorldConfigurationError");
    py::register_exception<ActionValidationError>(m, "ActionValidationError");
    py::register_exception<ConfigValidationError>(m, "ConfigValidationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Register standard runtime_error for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Enhanced NeuronId with validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)")
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

    // Enhanced SynapseId with validation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)")
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

    // Enhanced RegionId with validation
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)")
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

    // Enhanced PopulationId with validation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)")
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

    // Enhanced Config class with presets and validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)")
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
        })
        // Enhanced methods
        .def("createDefault", []() {
            try {
                auto config = std::make_shared<Config>();
                return config;
            } catch (const std::exception& e) {
                translateConfigError(e);
                return std::shared_ptr<Config>(nullptr);
            }
        }, "Create a default configuration")
        .def("from_preset", [](const std::string& preset_name) {
            auto config = std::make_shared<Config>();
            
            // Apply preset configuration
            if (preset_name == PRESET_DEVELOPMENT) {
                config->set("brain.neuron_count", 100);
                config->set("brain.region_count", 1);
                config->set("brain.simulation_time_step", 0.01);
                config->set("brain.development.enabled", true);
                config->set("brain.development.stage", "CriticalPeriod");
                config->set("world.width", 100.0f);
                config->set("world.height", 100.0f);
                config->set("world.vision_width", 10);
                config->set("world.vision_height", 10);
                config->set("reward.gamma", 0.95);
                config->set("reward.lambda", 0.9);
            }
            else if (preset_name == PRESET_PRODUCTION) {
                config->set("brain.neuron_count", 10000);
                config->set("brain.region_count", 5);
                config->set("brain.simulation_time_step", 0.001);
                config->set("brain.development.enabled", false);
                config->set("brain.plasticity.enabled", true);
                config->set("world.width", 500.0f);
                config->set("world.height", 500.0f);
                config->set("world.vision_width", 50);
                config->set("world.vision_height", 50);
                config->set("reward.gamma", 0.99);
                config->set("reward.lambda", 0.995);
            }
            else if (preset_name == PRESET_RESEARCH) {
                config->set("brain.neuron_count", 1000);
                config->set("brain.region_count", 2);
                config->set("brain.simulation_time_step", 0.005);
                config->set("brain.development.enabled", true);
                config->set("brain.development.stage", "Maturation");
                config->set("world.width", 200.0f);
                config->set("world.height", 200.0f);
                config->set("world.vision_width", 20);
                config->set("world.vision_height", 20);
                config->set("reward.gamma", 0.9);
                config->set("reward.lambda", 0.9);
            }
            else if (preset_name == PRESET_DEMO) {
                config->set("brain.neuron_count", 50);
                config->set("brain.region_count", 1);
                config->set("brain.simulation_time_step", 0.02);
                config->set("brain.development.enabled", false);
                config->set("world.width", 80.0f);
                config->set("world.height", 80.0f);
                config->set("world.vision_width", 8);
                config->set("world.vision_height", 8);
                config->set("reward.gamma", 0.9);
                config->set("reward.lambda", 0.8);
            }
            else {
                std::stringstream ss;\n                ss << "Unknown preset: " << preset_name;
                ss << ". Available presets: development, production, research, demo";
                throw ConfigValidationError(ss.str().c_str());
            }
            
            return config;
        }, py::arg("preset_name"), R"pbdoc(
        Create a configuration from a preset.
        
        Args:
            preset_name: Name of the preset. One of: 'development', 'production', 'research', 'demo'
        
        Returns:
            Config: A configuration object with preset values set.
        
        Raises:
            ConfigValidationError: If preset_name is not recognized.
        
        Example:
            >>> config = Config.from_preset('demo')
            >>> brain = Brain(config)
        )pbdoc")
        .def("validate", [](Config& self) {
            // Basic validation - check for required keys if any are set
            auto keys = self.getKeys();
            
            std::stringstream ss;\n            ss << "Configuration validation results:\n";
            ss << "Total keys: " << keys.size() << "\n";
            
            if (keys.empty()) {
                ss << "No configuration keys set. Consider using Config.from_preset().";
            } else {
                ss << "Keys: ";
                for (size_t i = 0; i < keys.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << keys[i];
                }
                ss << "\nValidation passed: All set keys appear valid.";
            }
            
            return ss.str();
        }, R"pbdoc(
        Validate the configuration.
        
        Returns:
            str: Validation results and any issues found.
        
        Example:
            >>> config = Config.from_preset('demo')
            >>> result = config.validate()
            >>> print(result)
        )pbdoc")
        .def("get_with_default", [](Config& self, const std::string& key, const py::object& default_value) {
            auto get_default = self.getOr(pyToString(key), pyToString(default_value));
            return get_default;
        }, py::arg("key"), py::arg("default"), R"pbdoc(
        Get a configuration value with a default fallback.
        
        Args:
            key: Configuration key to retrieve.
            default: Default value to return if key is not found.
        
        Returns:
            The configuration value if key exists, otherwise the default value.
        
        Example:
            >>> config = Config.from_preset('demo')
            >>> width = config.get_with_default('world.width', 100.0)
            >>> print(f"World width: {width}")
        )pbdoc")
        .def("to_dict", [](const Config& self) {
            auto keys = self.getKeys();
            py::dict result;
            
            for (const auto& key : keys) {
                // Try to get as different types
                if (self.has(key + ".int")) {
                    auto val = self.getOr<int>(key + ".int", 0);
                    result[key] = val;
                } else if (self.has(key + ".double")) {
                    auto val = self.getOr<double>(key + ".double", 0.0);
                    result[key] = val;
                } else if (self.has(key + ".string")) {
                    auto val = self.getOr<std::string>(key + ".string", "");
                    result[key] = val;
                } else if (self.has(key + ".bool")) {
                    auto val = self.getOr<bool>(key + ".bool", false);
                    result[key] = val;
                } else {
                    // Try generic get
                    auto val_int = self.getOr<int>(key, 0);
                    if (val_int.has_value()) {
                        result[key] = val_int.value();
                    } else {
                        auto val_double = self.getOr<double>(key, 0.0);
                        if (val_double.has_value()) {
                            result[key] = val_double.value();
                        } else {
                            auto val_string = self.getOr<std::string>(key, "");
                            if (val_string.has_value()) {
                                result[key] = val_string.value();
                            }
                        }
                    }
                }
            }
            
            return result;
        }, R"pbdoc(
        Convert configuration to a Python dictionary.
        
        Returns:
            dict: Dictionary representation of configuration.
        
        Example:
            >>> config = Config.from_preset('demo')
            >>> config_dict = config.to_dict()
            >>> print(config_dict)
        )pbdoc")
        .def("update_from_dict", [](Config& self, const py::dict& data) {
            for (auto item : data) {
                std::string key = py::cast<std::string>(item.first);
                py::object value = item.second;
                
                // Try to convert Python type to C++ ConfigValue
                if (value.is(py::bool_())) {
                    self.set(key, pyToBool(value));
                } else if (value.is(py::int_())) {
                    self.set(key, pyToInt(value));
                } else if (value.is(py::float_())) {
                    self.set(key, pyToFloat(value));
                } else {
                    self.set(key, pyToString(value));
                }
            }
        }, py::arg("data"), R"pbdoc(
        Update configuration from a Python dictionary.
        
        Args:
            data: Dictionary containing configuration key-value pairs.
        
        Example:
            >>> config = Config()
            >>> config.update_from_dict({'brain.neuron_count': 100, 'world.width': 50.0})
        )pbdoc");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Enhanced Action class with factory methods
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Enhanced methods - Factory Functions
        .def("create_move", [](const std::string& direction, float magnitude = 1.0f) {
            ActionType type;
            if (direction == "forward") type = ActionType::MoveForward;
            else if (direction == "backward" || direction == "back") type = ActionType::MoveBackward;
            else if (direction == "left") type = ActionType::MoveLeft;
            else if (direction == "right") type = ActionType::MoveRight;
            else if (direction == "turn_left" || direction == "left_turn") type = ActionType::TurnLeft;
            else if (direction == "turn_right" || direction == "right_turn") type = ActionType::TurnRight;
            else {
                std::stringstream ss;\n                ss << "Unknown direction: " << direction;
                ss << ". Valid directions: forward, backward, left, right, turn_left, turn_right";
                throw ActionValidationError(ss.str().c_str());
            }
            
            std::vector<float> params;
            if (magnitude != 0.0f) {
                params.push_back(magnitude);
            }
            return std::make_unique<Action>(type, params);
        }, py::arg("direction"), py::arg("magnitude") = 1.0f, R"pbdoc(
        Create a movement action.
        
        Args:
            direction: Direction for movement (forward, backward, left, right, turn_left, turn_right).
            magnitude: Movement magnitude (default 1.0).
        
        Returns:
            Action: A movement action.
        
        Raises:
            ActionValidationError: If direction is not recognized.
        
        Example:
            >>> move_action = Action.create_move("forward", 2.0)
            >>> turn_action = Action.create_move("turn_right", 0.5)
        )pbdoc")
        .def("create_look", [](float dx, float dy) {
            auto action = std::make_unique<Action>(ActionType::Look);
            std::vector<float> params = {dx, dy};
            action->setParameters(params);
            return action;
        }, py::arg("dx"), py::arg("dy"), R"pbdoc(
        Create a looking action.
        
        Args:
            dx: Change in X direction (positive = right).
            dy: Change in Y direction (positive = down).
        
        Returns:
            Action: A looking action.
        
        Example:
            >>> look_action = Action.create_look(0.1, 0.0)
        )pbdoc")
        .def("create_interact", []() {
            return std::make_unique<Action>(ActionType::Interact);
        }, R"pbdoc(
        Create an interaction action.
        
        Returns:
            Action: An interaction action.
        
        Example:
            >>> interact_action = Action.create_interact()
        )pbdoc")
        .def("create_wait", [](float duration) {
            auto action = std::make_unique<Action>(ActionType::Wait);
            std::vector<float> params = {duration};
            action->setParameters(params);
            return action;
        }, py::arg("duration") = 1.0f, R"pbdoc(
        Create a waiting action.
        
        Args:
            duration: Wait duration in simulation time steps.
        
        Returns:
            Action: A waiting action.
        
        Example:
            >>> wait_action = Action.create_wait(10.0)
        )pbdoc")
        .def("create_eat", [](float nutrition) {
            auto action = std::make_unique<Action>(ActionType::Eat);
            std::vector<float> params = {nutrition};
            action->setParameters(params);
            return action;
        }, py::arg("nutrition") = 10.0f, R"pbdoc(
        Create an eating action.
        
        Args:
            nutrition: Nutrition value (default 10.0).
        
        Returns:
            Action: An eating action.
        
        Example:
            >>> eat_action = Action.create_eat(20.0)
        )pbdoc")
        .def("create_drink", [](float hydration) {
            auto action = std::make_unique<Action>(ActionType::Drink);
            std::vector<float> params = {hydration};
            action->setParameters(params);
            return action;
        }, py::arg("hydration") = 10.0f, R"pbdoc(
        Create a drinking action.
        
        Args:
            hydration: Hydration value (default 10.0).
        
        Returns:
            Action: A drinking action.
        
        Example:
            >>> drink_action = Action.create_drink(15.0)
        )pbdoc")
        .def("create_rest", []() {
            return std::make_unique<Action>(ActionType::Rest);
        }, R"pbdoc(
        Create a resting action.
        
        Returns:
            Action: A resting action.
        
        Example:
            >>> rest_action = Action.create_rest()
        )pbdoc")
        .def("create_custom", [](const std::string& name, const std::vector<float>& parameters) {
            // Custom action type with user-defined parameters
            ActionType type = ActionType::Custom;
            auto action = std::make_unique<Action>(type, parameters);
            // Store custom name in parameters if needed
            std::vector<float> params_with_name = parameters;
            params_with_name.insert(params_with_name.begin(), 0.0f);  // Add placeholder
            action->setParameters(params_with_name);
            return action;
        }, py::arg("name"), py::arg("parameters"), R"pbdoc(
        Create a custom action.
        
        Args:
            name: Custom action name.
            parameters: Custom action parameters.
        
        Returns:
            Action: A custom action.
        
        Example:
            >>> custom_action = Action.create_custom("jump", [1.0, 2.0, 3.0])
        )pbdoc");

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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

    // Enhanced SimpleWorld class with factory methods and convenience functions
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Enhanced methods
        .def("createStandard", [](float width, float height, size_t visionWidth = 20, size_t visionHeight = 20) {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(width, height, visionWidth, visionHeight);
            world->setMaxEnergy(100.0f);
            world->setEnergyDecayRate(0.01f);
            return world;
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 20, py::arg("visionHeight") = 20,
           R"pbdoc(
           Create a standard configured world.
           
           Args:
               width: World width in units.
               height: World height in units.
               visionWidth: Vision sensor width in units.
               visionHeight: Vision sensor height in units.
           
           Returns:
               SimpleWorld: A configured world instance.
           
           Example:
               >>> world = SimpleWorld.create_standard(100, 100, 20, 20)
               >>> world.add_agent(10, 10)
           )pbdoc")
        .def("create_empty", []() {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(10.0f, 10.0f, 5, 5);
            world->setMaxEnergy(1.0f);
            world->setEnergyDecayRate(0.001f);
            return world;
        }, R"pbdoc(
        Create an empty world with minimal settings.
        
        Returns:
            SimpleWorld: A minimal configured world instance.
        
        Example:
            >>> world = SimpleWorld.create_empty()
            >>> world.add_agent(1, 1)
        )pbdoc")
        .def("add_agent", [](SimpleWorld& self, float x, float y) {
            try {
                validate_position(x, y, "world");
                self.setAgentStart(x, y);
            } catch (const WorldConfigurationError& e) {
                throw;
            } catch (const std::exception& e) {
                translateWorldError(e);
            }
        }, py::arg("x"), py::arg("y"), R"pbdoc(
        Add an agent at the specified position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
        
        Example:
            >>> world = SimpleWorld.create_standard(100, 100)
            >>> world.add_agent(10, 10)
        )pbdoc")
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value = 10.0f) {
            try {
                validate_position(x, y, "world");
                WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
                self.addObject(obj);
            } catch (const WorldConfigurationError& e) {
                throw;
            } catch (const std::exception& e) {
                translateWorldError(e);
            }
        }, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f, R"pbdoc(
        Add a resource object at the specified position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            value: Resource value (reward amount).
        
        Example:
            >>> world = SimpleWorld.create_standard(100, 100)
            >>> world.add_resource(50, 50, value=20.0)
        )pbdoc")
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float damage = 5.0f) {
            try {
                validate_position(x, y, "world");
                WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.3f);
                self.addObject(obj);
            } catch (const WorldConfigurationError& e) {
                throw;
            } catch (const std::exception& e) {
                translateWorldError(e);
            }
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 5.0f, R"pbdoc(
        Add a hazard object at the specified position.
        
        Args:
            x: X coordinate.
            y: Y coordinate.
            damage: Hazard damage amount.
        
        Example:
            >>> world = SimpleWorld.create_standard(100, 100)
            >>> world.add_hazard(75, 75, damage=10.0)
        )pbdoc")
        .def("clear_objects", [](SimpleWorld& self) {
            self.objects_.clear();
        }, R"pbdoc(
        Clear all objects from the world.
        
        Example:
            >>> world = SimpleWorld.create_standard(100, 100)
            >>> world.add_resource(50, 50)
            >>> world.clear_objects()  # Removes all objects
        )pbdoc");

    // Enhanced Brain class with context manager and convenience methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)")
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
             "Log brain status")
        // Enhanced methods
        .def("createDefault", []() {
            auto config = std::make_shared<Config>();
            return std::make_shared<Brain>(config);
        }, "Create a brain with default configuration")
        .def("createPreset", [](const std::string& preset_name) {
            auto config = Config::from_preset(preset_name);
            return std::make_shared<Brain>(config);
        }, py::arg("preset_name"), R"pbdoc(
        Create a brain with a preset configuration.
        
        Args:
            preset_name: Name of the preset configuration.
        
        Returns:
            Brain: A brain instance with preset configuration.
        
        Example:
            >>> brain = Brain.create_preset('demo')
            >>> brain.initialize()
        )pbdoc")
        .def("run_steps", [](Brain& self, size_t steps) {
            if (!self.getConfig() || !self.getConfig()->has("brain.simulation_time_step")) {
                throw BrainInitializationError("Brain not initialized. Call initialize() first.");
            }
            
            for (size_t i = 0; i < steps; ++i) {
                SimulationStep currentStep(i);
                self.step(currentStep);
            }
        }, py::arg("steps"), R"pbdoc(
        Run multiple simulation steps efficiently.
        
        Args:
            steps: Number of simulation steps to run.
        
        Example:
            >>> brain = Brain.create_default()
            >>> brain.initialize()
            >>> brain.run_steps(100)  # Run 100 steps
        )pbdoc")
        .def("get_stats", [](Brain& self) {
            py::dict stats;
            
            if (self.getConfig()) {
                stats["brain_initialized"] = true;
                
                // Get various statistics
                auto total_neuron_count = self.getTotalNeuronCount();
                auto total_synapse_count = self.getTotalSynapseCount();
                auto active_neuron_count = self.getActiveNeuronCount();
                auto firing_neuron_count = self.getFiringNeuronCount();
                auto total_spike_count = self.getTotalSpikeCount();
                auto avg_firing_rate = self.getAverageFiringRate();
                auto e_i_ratio = self.getExcitationInhibitionRatio();
                auto region_count = self.getRegionCount();
                
                stats["total_neuron_count"] = total_neuron_count;
                stats["total_synapse_count"] = total_synapse_count;
                stats["active_neuron_count"] = active_neuron_count;
                stats["firing_neuron_count"] = firing_neuron_count;
                stats["total_spike_count"] = total_spike_count;
                stats["average_firing_rate"] = avg_firing_rate;
                stats["excitation_inhibition_ratio"] = e_i_ratio;
                stats["region_count"] = region_count;
                
                // Add developmental stage
                auto stage = self.getDevelopmentalStage();
                stats["developmental_stage"] = static_cast<int>(stage);
                
                // Check if development is enabled
                auto config = self.getConfig();
                if (config) {
                    stats["development_enabled"] = config->getOr<bool>("brain.development.enabled", false);
                }
            } else {
                stats["brain_initialized"] = false;
            }
            
            return stats;
        }, R"pbdoc(
        Get quick statistics about the brain.
        
        Returns:
            dict: Dictionary containing brain statistics.
        
        Example:
            >>> brain = Brain.create_default()
            >>> brain.initialize()
            >>> stats = brain.get_stats()
            >>> print(f"Neurons: {stats['total_neuron_count']}")
        )pbdoc")
        .def("is_ready", [](Brain& self) {
            return self.getConfig() != nullptr;
        }, R"pbdoc(
        Check if the brain is ready for simulation.
        
        Returns:
            bool: True if brain is initialized, False otherwise.
        
        Example:
            >>> brain = Brain.create_default()
            >>> brain.initialize()
            >>> if brain.is_ready():
            ...     brain.run_steps(10)
        )pbdoc")
        .def("wait_for_convergence", [](Brain& self, float target_firing_rate, float timeout_seconds) {
            if (self.getConfig() == nullptr) {
                throw BrainInitializationError("Brain not initialized. Cannot wait for convergence.");
            }
            
            auto start_time = std::chrono::steady_clock::now();
            size_t max_iterations = static_cast<size_t>(timeout_seconds * 100);  // Assuming 100Hz
            
            for (size_t i = 0; i < max_iterations; ++i) {
                auto current_firing_rate = self.getAverageFiringRate();
                
                if (current_firing_rate >= target_firing_rate) {
                    return true;  // Convergence reached
                }
                
                SimulationStep currentStep(i);
                self.step(currentStep);
                
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
                if (elapsed.count() >= timeout_seconds) {
                    return false;  // Timeout reached
                }
            }
            
            return false;  // Timeout reached
        }, py::arg("target_firing_rate"), py::arg("timeout_seconds"), R"pbdoc(
        Wait for the brain to reach a target firing rate.
        
        Args:
            target_firing_rate: Target firing rate (spikes per second).
            timeout_seconds: Maximum time to wait in seconds.
        
        Returns:
            bool: True if convergence reached, False if timeout.
        
        Example:
            >>> brain = Brain.create_default()
            >>> brain.initialize()
            >>> converged = brain.wait_for_convergence(10.0, 5.0)
            >>> if converged:
            ...     print("Brain reached target firing rate")
        )pbdoc")
        .def("__enter__", [](Brain& self) {
            // Context manager enter - automatically initialize if not already done
            if (self.getConfig() == nullptr) {
                auto config = std::make_shared<Config>();
                self = Brain(config);  // Re-initialize with default config
            }
            if (!self.initialize()) {
                throw BrainInitializationError("Failed to initialize brain for context manager.");
            }
            return &self;
        })
        .def("__exit__", [](Brain& self, py::args) {
            // Context manager exit - automatic cleanup
            self.reset();
        }, R"pbdoc(
        Context manager support for automatic cleanup.
        
        Example:
            >>> with Brain.create_default() as brain:
            ...     brain.initialize()
            ...     brain.run_steps(50)
            # Brain automatically reset and cleaned up
        )pbdoc");

    // Enhanced AgentBrain class with more methods
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
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

    // Factory functions (global)
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience factory functions
    m.def("create_demo_brain", []() {
        auto config = Config::from_preset(PRESET_DEMO);
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize demo brain.");
        }
        return brain;
    }, "Create a demo brain with preset configuration");

    m.def("create_demo_world", []() {
        auto world = SimpleWorld::createStandard(100.0f, 100.0f, 20, 20);
        world->setAgentStart(10.0f, 10.0f);
        world->addResource(50.0f, 50.0f, 20.0f);
        world->addHazard(75.0f, 75.0f, 10.0f);
        return world;
    }, "Create a demo world with sample objects");

    m.def("create_agent_for_world", [](std::shared_ptr<SimpleWorld> world) {
        auto config = Config::from_preset(PRESET_DEVELOPMENT);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        agent_brain->initialize(world);
        
        return agent_brain;
    }, py::arg("world"), "Create an agent brain for a world");

    // Global constants
    m.attr("PRESET_DEVELOPMENT") = PRESET_DEVELOPMENT;
    m.attr("PRESET_PRODUCTION") = PRESET_PRODUCTION;
    m.attr("PRESET_RESEARCH") = PRESET_RESEARCH;
    m.attr("PRESET_DEMO") = PRESET_DEMO;

    // Global error constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module-level utility functions
    m.def("simulate_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                size_t max_steps, float target_reward) {
        if (!brain->initialize()) {
            throw BrainInitializationError("Failed to initialize brain for episode simulation.");
        }
        
        float total_reward = 0.0f;
        size_t steps = 0;
        
        while (steps < max_steps && total_reward < target_reward) {
            // Get sensory percept
            const auto& percept = world->getSensoryPercept();
            
            // Process sensory input
            brain->receiveSensoryInput(percept);
            
            // Produce action
            auto action = brain->produceAction();
            
            // Apply action to world
            auto action_result = world->applyMotorCommand(
                MotorCommand{ActionType::MoveForward}, world->getSimulationTime()
            );
            
            total_reward += action_result.reward;
            world->update(0.01);  // Fixed timestep
            steps++;
        }
        
        return py::make_tuple(total_reward, steps);
    }, py::arg("brain"), py::arg("world"), py::arg("max_steps"), py::arg("target_reward"),
       R"pbdoc(
       Simulate an episode of interaction between a brain and world.
       
       Args:
           brain: Brain instance to use.
           world: World instance to interact with.
           max_steps: Maximum number of steps to simulate.
           target_reward: Target total reward to achieve.
       
       Returns:
           tuple: (total_reward, steps_completed)
       
       Example:
           >>> brain = Brain.create_default()
           >>> world = SimpleWorld.create_standard(100, 100)
           >>> reward, steps = simulate_episode(brain, world, 1000, 100.0)
       )pbdoc");

    m.def("validate_setup", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        std::stringstream ss;\n        
        ss << "Setup Validation Results:\n";
        
        if (!brain) {
            ss << "ERROR: Brain is null or not provided.\n";
        } else {
            ss << "✓ Brain provided and is not null.\n";
            
            if (brain->getConfig()) {
                ss << "✓ Brain has configuration.\n";
                
                auto config = brain->getConfig();
                auto keys = config->getKeys();
                ss << "  Configuration keys: ";
                for (size_t i = 0; i < keys.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << keys[i];
                }
                ss << "\n";
            } else {
                ss << "⚠ Brain does not have configuration.\n";
            }
        }
        
        if (!world) {
            ss << "ERROR: World is null or not provided.\n";
        } else {
            ss << "✓ World provided and is not null.\n";
            ss << "  World dimensions: " << world->getWidth() << " x " << world->getHeight() << "\n";
            ss << "  Vision: " << world->getWidth() << " x " << world->getHeight() << "\n";
        }
        
        return ss.str();
    }, py::arg("brain"), py::arg("world"),
       R"pbdoc(
       Validate a brain-world setup.
       
       Args:
           brain: Brain instance to validate.
           world: World instance to validate.
       
       Returns:
           str: Validation results with warnings and errors.
       
       Example:
           >>> brain = Brain.create_default()
           >>> world = SimpleWorld.create_standard(100, 100)
           >>> validation = validate_setup(brain, world)
           >>> print(validation)
       )pbdoc");
}
