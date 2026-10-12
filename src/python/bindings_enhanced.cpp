#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

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

// Custom Python exception classes
class NLMError : public std::exception {
public:
    std::string message;
    NLMError(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class ConfigError : public NLMError {
public:
    ConfigError(const std::string& msg) : NLMError(msg) {}
};

class BrainError : public NLMError {
public:
    BrainError(const std::string& msg) : NLMError(msg) {}
};

class WorldError : public NLMError {
public:
    WorldError(const std::string& msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& msg) : NLMError(msg) {}
};

// Context manager implementation for Config
class ConfigManager {
public:
    std::shared_ptr<Config> config;
    
    ConfigManager() : config(std::make_shared<Config>()) {}
    ConfigManager(const std::string& filepath) : config(std::make_shared<Config>()) {
        config->loadFromFile(filepath);
    }
    
    py::object getPyConfig() {
        return py::cast(config);
    }
    
    bool enter(py::object) {
        return true;
    }
    
    void exit(py::object, py::object, py::object) {
        config.reset();
    }
};

// Context manager implementation for Brain
class BrainManager {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    
    BrainManager(std::shared_ptr<Brain> b) : brain(b) {}
    
    py::object getPyBrain() {
        return py::cast(brain);
    }
    
    bool enter(py::object) {
        return true;
    }
    
    void exit(py::object, py::object, py::object) {
        if (brain) {
            brain->reset();
        }
        brain.reset();
        world.reset();
    }
};

// Context manager implementation for World
class WorldManager {
public:
    std::shared_ptr<SimpleWorld> world;
    
    WorldManager() : world(std::make_shared<SimpleWorld>()) {}
    WorldManager(size_t width, size_t height, size_t visionWidth, size_t visionHeight) 
        : world(std::make_shared<SimpleWorld>()) {
        world->configure(width, height, visionWidth, visionHeight);
    }
    
    py::object getPyWorld() {
        return py::cast(world);
    }
    
    bool enter(py::object) {
        return true;
    }
    
    void exit(py::object, py::object, py::object) {
        if (world) {
            world->reset();
        }
        world.reset();
    }
};

PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework with enhanced Pythonic APIs.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Pythonic context managers for resource management
        - Convenience factory methods with sensible defaults
        - Better error handling with descriptive Python exceptions
        - Properties and getters/setters for Pythonic access
        - Batch processing methods for efficiency
        - Comprehensive documentation strings
        - Support for Python patterns (iterators, context managers, etc.)
        - Helper methods for common workflows
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // NeuronId class with enhanced Pythonic APIs
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
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

    // SynapseId class with enhanced Pythonic APIs
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get synapse index")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId class with enhanced Pythonic APIs
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get region index")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId class with enhanced Pythonic APIs
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get population index")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron")
        .value("Motor", NeuronType::Motor, "Motor neuron")
        .value("Internal", NeuronType::Internal, "Internal neuron")
        .export_values();

    // SynapseType enum with better documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // DevelopmentalStage enum with better documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial development stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // FiringState enum with better documentation
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Resting state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory period")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state")
        .export_values();

    // ActionType enum with better documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // MotorCommand enum with better documentation
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left command")
        .value("LookRight", MotorCommand::LookRight, "Look right command")
        .value("Interact", MotorCommand::Interact, "Interact command")
        .value("Wait", MotorCommand::Wait, "Wait command")
        .export_values();

    // WorldObjectType enum with better documentation
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object")
        .value("Wall", WorldObjectType::Wall, "Wall object")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Enhanced Config class with context manager and Pythonic APIs
    py::class_<Config, ConfigManager>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"),
             "Load configuration from a JSON file")
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
        .def("get", [](const Config& self, const std::string& key, const py::object& defaultValue) {
            try {
                return self.get(key);
            } catch (const std::exception&) {
                return py::cast(defaultValue);
            }
        }, py::arg("key"), py::arg("default") = py::none(),
            "Get a configuration value with optional default")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance<py::int_>(value)) {
                self.set(key, py::cast<int>(value));
            } else if (py::isinstance<py::float_>(value)) {
                self.set(key, py::cast<double>(value));
            } else if (py::isinstance<py::str>(value)) {
                self.set(key, py::cast<std::string>(value));
            } else if (py::isinstance<py::bool_>(value)) {
                self.set(key, py::cast<bool>(value));
            } else {
                throw ConfigError("Unsupported type for config value: " + std::to_string(value.get_type().hash_code()));
            }
        }, py::arg("key"), py::arg("value"),
            "Set a configuration value")
        .def("getInt", &Config::get<int>, py::arg("key"), py::arg("default") = 0,
             "Get an integer configuration value")
        .def("getDouble", &Config::get<double>, py::arg("key"), py::arg("default") = 0.0,
             "Get a double configuration value")
        .def("getString", &Config::get<std::string>, py::arg("key"), py::arg("default") = "",
             "Get a string configuration value")
        .def("getBool", &Config::get<bool>, py::arg("key"), py::arg("default") = false,
             "Get a boolean configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set an integer configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a double configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
            "Set a boolean configuration value")
        .def("getAllAsDict", [](const Config& self) {
            std::vector<std::string> keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                result[key.c_str()] = py::cast(self.get(key));
            }
            return result;
        }, "Get all configuration entries as a Python dictionary")
        .def("mergeFromDict", [](Config& self, const py::dict& data) {
            py::object keys = data.keys();
            py::object values = data.values();
            py::tuple items = py::cast<std::pair<py::object, py::object>>(py::make_tuple(keys, values));
            
            for (size_t i = 0; i < keys.size(); ++i) {
                std::string key = py::cast<std::string>(keys[i]);
                py::object value = values[i];
                
                if (py::isinstance<py::int_>(value)) {
                    self.set(key, py::cast<int>(value));
                } else if (py::isinstance<py::float_>(value)) {
                    self.set(key, py::cast<double>(value));
                } else if (py::isinstance<py::str>(value)) {
                    self.set(key, py::cast<std::string>(value));
                } else if (py::isinstance<py::bool_>(value)) {
                    self.set(key, py::cast<bool>(value));
                }
            }
        }, py::arg("data"),
            "Merge configuration from a Python dictionary")
        .def("validate", [](const Config& self, const std::string& key, const std::string& type) {
            try {
                if (type == "int" || type == "integer") {
                    self.get<int>(key);
                } else if (type == "double" || type == "float") {
                    self.get<double>(key);
                } else if (type == "string") {
                    self.get<std::string>(key);
                } else if (type == "bool" || type == "boolean") {
                    self.get<bool>(key);
                } else {
                    throw ConfigError("Unsupported validation type: " + type);
                }
                return true;
            } catch (const std::exception&) {
                return false;
            }
        }, py::arg("key"), py::arg("type"),
            "Validate that a configuration key has the specified type")
        .def("validateRequired", [](const Config& self, const std::vector<std::string>& keys) {
            for (const auto& key : keys) {
                if (!self.has(key)) {
                    throw ConfigError("Missing required configuration key: " + key);
                }
            }
            return true;
        }, py::arg("keys"),
            "Validate that all specified configuration keys exist")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Context manager support for Config
    m.def("config_manager", [](const std::string& filepath) {
        return py::cast<ConfigManager>(ConfigManager(filepath));
    }, py::arg("filepath"), "Create a config context manager");

    // Create default config with factory methods
    m.def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_config_from_file", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Create a configuration from a file");

    // Enhanced SensoryInput class with Pythonic APIs
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with enhanced Pythonic APIs)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("to_dict", [](const SensoryInput& input) {
            py::dict result;
            result["type"] = input.getType();
            result["data"] = input.getData();
            result["dimensions"] = input.getDimensions();
            result["timestamp"] = input.getTimestamp();
            return result;
        }, "Convert sensory input to a Python dictionary")
        .def("from_dict", [](SensoryInput& input, const py::dict& data) {
            if (data.contains("timestamp")) {
                input.setTimestamp(py::cast<Timestamp>(data["timestamp"]));
            }
        }, py::arg("data"),
            "Update sensory input from a Python dictionary");

    // Enhanced Vision class with Pythonic APIs
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Vision::getData, "Get vision data as a vector")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("reshape", [](Vision& self, size_t width, size_t height, size_t channels) {
            std::vector<float> data = self.getData();
            size_t expectedSize = width * height * channels;
            if (data.size() != expectedSize) {
                throw SimulationError("Data size mismatch for reshape");
            }
            self.setData(data);
        }, py::arg("width"), py::arg("height"), py::arg("channels"),
            "Reshape vision data")
        .def("normalize", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            std::vector<float> data = self.getData();
            for (auto& value : data) {
                value = (value - min) / (max - min);
            }
            self.setData(data);
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
            "Normalize vision data to [min, max] range");

    // Enhanced Audio class with Pythonic APIs
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Audio::getData, "Get audio data as a vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("duration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("resample", [](Audio& self, size_t newSampleRate) {
            std::vector<float> data = self.getData();
            size_t newNumSamples = static_cast<size_t>(
                data.size() * newSampleRate / self.getSampleRate()
            );
            std::vector<float> resampled(newNumSamples);
            for (size_t i = 0; i < newNumSamples; ++i) {
                resampled[i] = data[static_cast<size_t>(i * static_cast<float>(self.getSampleRate()) / newSampleRate)];
            }
            self.setData(resampled);
        }, py::arg("newSampleRate"),
            "Resample audio");

    // Enhanced InternalSignals class with Pythonic APIs
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData, "Get all signals as vector")
        .def("addSignalBatch", [](InternalSignals& self, const std::vector<float>& values) {
            for (float value : values) {
                self.addSignal(value);
            }
        }, py::arg("values"),
            "Add multiple signals at once")
        .def("getMean", [](const InternalSignals& self) {
            const std::vector<float>& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = 0.0f;
            for (float value : data) sum += value;
            return sum / static_cast<float>(data.size());
        }, "Get mean of all signals")
        .def("getStdDev", [](const InternalSignals& self) {
            const std::vector<float>& data = self.getData();
            if (data.size() <= 1) return 0.0f;
            float mean = 0.0f;
            for (float value : data) mean += value;
            mean /= static_cast<float>(data.size());
            
            float variance = 0.0f;
            for (float value : data) {
                float diff = value - mean;
                variance += diff * diff;
            }
            variance /= static_cast<float>(data.size());
            return std::sqrt(variance);
        }, "Get standard deviation of all signals");

    // Enhanced Action class with Pythonic APIs
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            result["name"] = action.getName();
            return result;
        }, "Convert action to a Python dictionary")
        .def("from_dict", [](Action& action, const py::dict& data) {
            if (data.contains("type")) {
                action.setType(py::cast<ActionType>(data["type"]));
            }
            if (data.contains("parameters")) {
                action.setParameters(py::cast<std::vector<float>>(data["parameters"]));
            }
        }, py::arg("data"),
            "Update action from a Python dictionary");

    // Enhanced WorldObject class with Pythonic APIs
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
            "Calculate distance to a point")
        .def("isVisible", [](const WorldObject& obj, float agentX, float agentY, float visionRange) {
            return obj.active && obj.distanceTo(agentX, agentY) <= visionRange;
        }, py::arg("agentX"), py::arg("agentY"), py::arg("visionRange"),
            "Check if object is visible from agent's position")
        .def("to_dict", [](const WorldObject& obj) {
            py::dict result;
            result["x"] = obj.x;
            result["y"] = obj.y;
            result["radius"] = obj.radius;
            result["type"] = obj.type;
            result["value"] = obj.value;
            result["active"] = obj.active;
            return result;
        }, "Convert world object to a Python dictionary")
        .def("from_dict", [](WorldObject& obj, const py::dict& data) {
            if (data.contains("x")) obj.x = py::cast<float>(data["x"]);
            if (data.contains("y")) obj.y = py::cast<float>(data["y"]);
            if (data.contains("radius")) obj.radius = py::cast<float>(data["radius"]);
            if (data.contains("type")) obj.type = py::cast<WorldObjectType>(data["type"]);
            if (data.contains("value")) obj.value = py::cast<float>(data["value"]);
            if (data.contains("active")) obj.active = py::cast<bool>(data["active"]);
        }, py::arg("data"),
            "Update world object from a Python dictionary");

    // Enhanced AgentBody class with Pythonic APIs
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with enhanced Pythonic APIs)pbdoc")
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& body) {
            py::list result;
            result.append(body.x);
            result.append(body.y);
            return result;
        }, "Get position as a Python list [x, y]")
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"),
            "Set position from coordinates")
        .def("getVelocity", [](const AgentBody& body) {
            py::list result;
            result.append(body.velocityX);
            result.append(body.velocityY);
            return result;
        }, "Get velocity as a Python list [vx, vy]")
        .def("setVelocity", [](AgentBody& body, float vx, float vy) {
            body.velocityX = vx;
            body.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"),
            "Set velocity from components")
        .def("getSpeed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get speed (magnitude of velocity)")
        .def("getDirection", [](const AgentBody& body) {
            return std::atan2(body.velocityY, body.velocityX);
        }, "Get direction in radians")
        .def("distanceTo", [](const AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
            "Calculate distance to a point")
        .def("to_dict", [](const AgentBody& body) {
            py::dict result;
            result["x"] = body.x;
            result["y"] = body.y;
            result["orientation"] = body.orientation;
            result["velocityX"] = body.velocityX;
            result["velocityY"] = body.velocityY;
            result["angularVelocity"] = body.angularVelocity;
            result["energy"] = body.energy;
            result["health"] = body.health;
            result["age"] = body.age;
            result["isMoving"] = body.isMoving;
            result["isTurning"] = body.isTurning;
            result["lastActionTime"] = body.lastActionTime;
            return result;
        }, "Convert agent body to a Python dictionary")
        .def("from_dict", [](AgentBody& body, const py::dict& data) {
            if (data.contains("x")) body.x = py::cast<float>(data["x"]);
            if (data.contains("y")) body.y = py::cast<float>(data["y"]);
            if (data.contains("orientation")) body.orientation = py::cast<float>(data["orientation"]);
            if (data.contains("velocityX")) body.velocityX = py::cast<float>(data["velocityX"]);
            if (data.contains("velocityY")) body.velocityY = py::cast<float>(data["velocityY"]);
            if (data.contains("angularVelocity")) body.angularVelocity = py::cast<float>(data["angularVelocity"]);
            if (data.contains("energy")) body.energy = py::cast<float>(data["energy"]);
            if (data.contains("health")) body.health = py::cast<float>(data["health"]);
            if (data.contains("age")) body.age = py::cast<float>(data["age"]);
            if (data.contains("isMoving")) body.isMoving = py::cast<bool>(data["isMoving"]);
            if (data.contains("isTurning")) body.isTurning = py::cast<bool>(data["isTurning"]);
            if (data.contains("lastActionTime")) body.lastActionTime = py::cast<float>(data["lastActionTime"]);
        }, py::arg("data"),
            "Update agent body from a Python dictionary");

    // Enhanced ActionResult class with Pythonic APIs
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& result) {
            return result.success && result.reward > 0.0f;
        }, "Check if result is good (successful with positive reward)")
        .def("isBad", [](const ActionResult& result) {
            return !result.success || result.reward < 0.0f;
        }, "Check if result is bad (failed or negative reward)")
        .def("to_dict", [](const ActionResult& result) {
            py::dict dict;
            dict["reward"] = result.reward;
            dict["success"] = result.success;
            dict["message"] = result.message;
            return dict;
        }, "Convert action result to a Python dictionary")
        .def("from_dict", [](ActionResult& result, const py::dict& data) {
            if (data.contains("reward")) result.reward = py::cast<float>(data["reward"]);
            if (data.contains("success")) result.success = py::cast<bool>(data["success"]);
            if (data.contains("message")) result.message = py::cast<std::string>(data["message"]);
        }, py::arg("data"),
            "Update action result from a Python dictionary");

    // Enhanced SensoryPercept class with Pythonic APIs
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced Pythonic APIs)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("clear", &SensoryPercept::clear,
             "Clear all sensory data")
        .def("hasVision", [](const SensoryPercept& percept) {
            return percept.getVision() != nullptr;
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& percept) {
            return !percept.getTouch().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& percept) {
            return percept.getInternal() != nullptr;
        }, "Check if percept has internal signals")
        .def("hasProprioception", [](const SensoryPercept& percept) {
            return !percept.getProprioception().empty();
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& percept) {
            return percept.getAudio() != nullptr;
        }, "Check if percept has audio data")
        .def("getAllDataAsDict", [](const SensoryPercept& percept) {
            py::dict result;
            
            auto vision = percept.getVision();
            if (vision) result["vision"] = py::cast(*vision);
            
            auto touch = percept.getTouch();
            if (touch) result["touch"] = py::cast(*touch);
            
            auto internal = percept.getInternal();
            if (internal) result["internal"] = py::cast(*internal);
            
            auto proprioception = percept.getProprioception();
            if (proprioception) result["proprioception"] = py::cast(*proprioception);
            
            auto audio = percept.getAudio();
            if (audio) result["audio"] = py::cast(*audio);
            
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Get all sensory data as a Python dictionary")
        .def("updateFromDict", [](SensoryPercept& percept, const py::dict& data) {
            if (data.contains("vision")) {
                percept.setVision(py::cast<Vision>(data["vision"]));
            }
            if (data.contains("touch")) {
                percept.setTouch(py::cast<std::vector<float>>(data["touch"]));
            }
            if (data.contains("internal")) {
                percept.setInternal(py::cast<InternalSignals>(data["internal"]));
            }
            if (data.contains("proprioception")) {
                percept.setProprioception(py::cast<std::vector<float>>(data["proprioception"]));
            }
            if (data.contains("audio")) {
                percept.setAudio(py::cast<Audio>(data["audio"]));
            }
            if (data.contains("timestamp")) {
                percept.setTimestamp(py::cast<Timestamp>(data["timestamp"]));
            }
        }, py::arg("data"),
            "Update percept from a Python dictionary");

    // Enhanced SimpleWorld class with context manager and Pythonic APIs
    py::class_<SimpleWorld, WorldManager>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced Pythonic APIs)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensing")
        .def("reset", &SimpleWorld::reset,
             "Reset world state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectCount", [](const SimpleWorld& world) {
            int count = 0;
            for (const auto& obj : world.getObjects()) {
                if (obj.active) count++;
            }
            return count;
        }, "Get count of active objects")
        .def("to_dict", [](const SimpleWorld& world) {
            py::dict result;
            result["width"] = world.getWidth();
            result["height"] = world.getHeight();
            result["visionWidth"] = world.getVisionWidth();
            result["visionHeight"] = world.getVisionHeight();
            result["maxEnergy"] = world.getMaxEnergy();
            result["energyDecayRate"] = world.getEnergyDecayRate();
            result["simulationTime"] = world.getSimulationTime();
            
            py::list objects;
            for (const auto& obj : world.getObjects()) {
                if (obj.active) {
                    py::dict objDict;
                    objDict["x"] = obj.x;
                    objDict["y"] = obj.y;
                    objDict["radius"] = obj.radius;
                    objDict["type"] = obj.type;
                    objDict["value"] = obj.value;
                    objects.append(objDict);
                }
            }
            result["objects"] = objects;
            return result;
        }, "Convert world to a Python dictionary")
        .def("from_dict", [](SimpleWorld& world, const py::dict& data) {
            if (data.contains("width")) {
                size_t visionWidth = data.contains("visionWidth") ? 
                    py::cast<size_t>(data["visionWidth"]) : world.getVisionWidth();
                size_t visionHeight = data.contains("visionHeight") ? 
                    py::cast<size_t>(data["visionHeight"]) : world.getVisionHeight();
                world.configure(
                    py::cast<float>(data["width"]),
                    py::cast<float>(data["height"]),
                    visionWidth,
                    visionHeight
                );
            }
            if (data.contains("maxEnergy")) {
                world.setMaxEnergy(py::cast<float>(data["maxEnergy"]));
            }
            if (data.contains("energyDecayRate")) {
                world.setEnergyDecayRate(py::cast<float>(data["energyDecayRate"]));
            }
            if (data.contains("objects")) {
                py::list objects = py::cast<py::list>(data["objects"]);
                for (size_t i = 0; i < objects.size(); ++i) {
                    py::dict objData = py::cast<py::dict>(objects[i]);
                    WorldObject obj;
                    if (objData.contains("x")) obj.x = py::cast<float>(objData["x"]);
                    if (objData.contains("y")) obj.y = py::cast<float>(objData["y"]);
                    if (objData.contains("radius")) obj.radius = py::cast<float>(objData["radius"]);
                    if (objData.contains("type")) obj.type = py::cast<WorldObjectType>(objData["type"]);
                    if (objData.contains("value")) obj.value = py::cast<float>(objData["value"]);
                    obj.active = true;
                    world.addObject(obj);
                }
            }
        }, py::arg("data"),
            "Update world from a Python dictionary");

    // Context manager support for World
    m.def("world_manager", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        return py::cast<WorldManager>(WorldManager(width, height, visionWidth, visionHeight));
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Create a world context manager with dimensions");

    // Create default world with factory methods
    m.def("create_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a default world");

    m.def("create_world_with_size", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
        "Create a world with specified dimensions");

    // Enhanced Brain class with Pythonic APIs and batch processing
    py::class_<Brain, BrainManager>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced Pythonic APIs)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Initialize brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("step_batch", [](Brain& self, const std::vector<SimulationStep>& steps, Timestamp time = 0) {
            for (const auto& step : steps) {
                self.step(step, time);
            }
        }, py::arg("steps"), py::arg("time") = 0,
            "Perform multiple simulation steps in batch")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentBatch", [](Brain& self, const std::vector<std::pair<NeuronId, float>>& injections) {
            for (const auto& injection : injections) {
                self.injectCurrent(injection.first, injection.second);
            }
        }, py::arg("injections"),
            "Inject current into multiple neurons in batch")
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
        .def("runSimulation", [](Brain& self, const std::vector<SimulationStep>& steps, std::shared_ptr<SimpleWorld> world = nullptr) {
            Timestamp time = 0;
            for (const auto& step : steps) {
                self.step(step, time);
                if (world) {
                    world->update(step);
                }
                time++;
            }
        }, py::arg("steps"), py::arg("world") = py::none(),
            "Run a complete simulation with optional world updates")
        .def("getBrainState", [](const Brain& self) {
            py::dict state;
            state["region_count"] = self.getRegionCount();
            state["neuron_count"] = self.getTotalNeuronCount();
            state["synapse_count"] = self.getTotalSynapseCount();
            state["active_neurons"] = self.getActiveNeuronCount();
            state["firing_neurons"] = self.getFiringNeuronCount();
            state["average_firing_rate"] = self.getAverageFiringRate();
            state["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            state["total_spikes"] = self.getTotalSpikeCount();
            state["developmental_stage"] = self.getDevelopmentalStage();
            return state;
        }, "Get comprehensive brain state as a dictionary")
        .def("findNeuronsByType", [](const Brain& self, NeuronType type) {
            py::list result;
            for (const auto& region : self.getRegions()) {
                for (const auto& population : region.getPopulations()) {
                    for (const auto& neuron : population.getNeurons()) {
                        if (neuron.getType() == type) {
                            result.append(neuron.getId());
                        }
                    }
                }
            }
            return result;
        }, py::arg("type"),
            "Find all neurons of a specific type")
        .def("findSynapsesByType", [](const Brain& self, SynapseType type) {
            py::list result;
            for (const auto& region : self.getRegions()) {
                for (const auto& population : region.getPopulations()) {
                    for (const auto& neuron : population.getNeurons()) {
                        for (const auto& synapse : neuron.getSynapses()) {
                            if (synapse.getType() == type) {
                                result.append(synapse.getId());
                            }
                        }
                    }
                }
            }
            return result;
        }, py::arg("type"),
            "Find all synapses of a specific type")
        .def("clearAllSynapticWeights", [](Brain& self) {
            for (auto& region : self.getRegions()) {
                for (auto& population : region.getPopulations()) {
                    for (auto& neuron : population.getNeurons()) {
                        for (auto& synapse : neuron.getSynapses()) {
                            synapse.setWeight(0.0f);
                        }
                    }
                }
            }
        }, "Clear all synaptic weights in the brain");

    // Context manager support for Brain
    m.def("brain_manager", [](std::shared_ptr<Brain> brain) {
        return py::cast<BrainManager>(BrainManager(brain));
    }, py::arg("brain"), "Create a brain context manager");

    // Create default brain with factory methods
    m.def("create_brain", []() -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(std::make_shared<Config>());
    }, "Create a brain with default configuration");

    m.def("create_brain_with_config", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain with specified configuration");

    // Enhanced AgentBrain class with Pythonic APIs
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced Pythonic APIs)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize with brain")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, const std::vector<SimulationStep>& steps) {
            self.initialize(world);
            for (const auto& step : steps) {
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, step);
                world.update(step);
            }
        }, py::arg("world"), py::arg("steps"),
            "Run a complete episode with world interactions")
        .def("getAgentState", [](const AgentBrain& self) {
            py::dict state;
            state["sensory_input_size"] = self.getSensoryInputSize();
            state["motor_output_size"] = self.getMotorOutputSize();
            state["developmental_stage"] = self.getDevelopmentalStage();
            state["neuromodulation_level"] = self.getNeuromodulationLevel();
            state["curiosity_level"] = self.getCuriosityLevel();
            state["novelty_level"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            state["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            state["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            state["development_enabled"] = self.isDevelopmentEnabled();
            state["curiosity_enabled"] = self.isCuriosityEnabled();
            return state;
        }, "Get comprehensive agent brain state as a dictionary");

    // Create default agent brain with factory method
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Additional global functions and constants
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain_global", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Add invalid constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced documentation for key classes
    m.def("get_version", []() {
        return "Enhanced NLM Python Bindings v2.0";
    }, "Get version information");

    // Add examples in documentation
    m.def("get_examples", []() {
        return R"pbdoc(
            # Example 1: Basic usage with context managers
            from pynlm_enhanced import Config, Brain, SimpleWorld
            
            with Config() as cfg:
                cfg.set("learning_rate", 0.01)
                cfg.set("population_size", 100)
                
                with Brain(cfg) as brain:
                    brain.initialize()
                    
                    with SimpleWorld(100, 100, 32, 32) as world:
                        # Run a simple simulation
                        steps = [0, 1, 2, 3, 4]
                        brain.run_simulation(steps, world)
                        
                        # Process sensory input
                        percept = world.getSensoryPercept()
                        brain.receiveSensoryInput(percept)
                        
                        # Produce action
                        action = brain.produce_action()
            
            # Example 2: Batch processing
            from pynlm_enhanced import create_config, create_brain
            
            config = create_config()
            config.set("population_size", 1000)
            
            brain = create_brain(config)
            steps = list(range(1000))
            brain.step_batch(steps)
            
            # Example 3: Advanced workflow
            from pynlm_enhanced import Config, create_brain, create_world
            
            cfg = Config()
            cfg.set("neurons_per_population", 50)
            cfg.set("synapses_per_neuron", 10)
            cfg.set("learning_rate", 0.001)
            
            brain = create_brain(cfg)
            brain.add_region("visual")
            brain.add_region("motor")
            
            world = create_world_with_size(200, 200, 64, 64)
            world.set_agent_start(100, 100)
            
            # Run episode
            agent_brain = create_agent_brain(brain)
            agent_brain.run_episode(world, list(range(100)))
        )pbdoc";
    }, "Get usage examples");

    // Add utility functions
    m.def("dump_system_info", []() {
        py::dict info;
        info["version"] = "Enhanced NLM Python Bindings v2.0";
        info["description"] = "Enhanced Python bindings for NLM neural simulation framework";
        info["features"] = {
            "Pythonic context managers",
            "Convenience factory methods", 
            "Better error handling",
            "Pythonic property access",
            "Batch processing methods",
            "Comprehensive documentation",
            "Python pattern support",
            "Helper methods for workflows"
        };
        return info;
    }, "Dump system information and features");

    // Add type checking and validation helpers
    m.def("validate_config", [](const Config& config) {
        bool valid = true;
        std::string errors;
        
        if (!config.has("neurons_per_population")) {
            valid = false;
            errors += "Missing neurons_per_population; ";
        }
        
        if (!config.has("synapses_per_neuron")) {
            valid = false;
            errors += "Missing synapses_per_neuron; ";
        }
        
        return std::make_pair(valid, errors);
    }, py::arg("config"), "Validate configuration and return validity and error messages");
}

} // namespace nlm