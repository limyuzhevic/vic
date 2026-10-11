#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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

// Custom exception for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

// Custom exception for configuration errors
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Config error: " + message) {}
};

// Custom exception for brain errors
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain error: " + message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Neural simulation with spiking neurons
        - Neuromodulation and learning
        - Integration with simulated worlds
        - Developmental stages and maturation
        - Memory systems (working, episodic, associative)
        - Prediction and planning
        - Complete Python API for research and education
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError.ptr());
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError.ptr());
    
    // Helper functions for enum conversions
    auto neuronTypeToString = [](NeuronType type) {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    };
    
    auto developmentalStageToString = [](DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    };
    
    auto actionTypeToString = [](ActionType type) {
        switch (type) {
            case ActionType::MoveForward: return "MoveForward";
            case ActionType::MoveBackward: return "MoveBackward";
            case ActionType::MoveLeft: return "MoveLeft";
            case ActionType::MoveRight: return "MoveRight";
            case ActionType::TurnLeft: return "TurnLeft";
            case ActionType::TurnRight: return "TurnRight";
            case ActionType::Look: return "Look";
            case ActionType::LookUp: return "LookUp";
            case ActionType::LookDown: return "LookDown";
            case ActionType::Interact: return "Interact";
            case ActionType::Eat: return "Eat";
            case ActionType::Drink: return "Drink";
            case ActionType::Rest: return "Rest";
            case ActionType::Wait: return "Wait";
            case ActionType::Custom: return "Custom";
            default: return "Unknown";
        }
    };
    
    auto motorCommandToString = [](MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    };

    // Neuron ID with Python-friendly methods
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
        })
        .def("is_valid", [](const NeuronId& id) { return id.value != INVALID_NEURON_ID.value; });

    // Synapse ID with Python-friendly methods
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
        })
        .def("is_valid", [](const SynapseId& id) { return id.value != INVALID_SYNAPSE_ID.value; });

    // Region ID with Python-friendly methods
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
        })
        .def("is_valid", [](const RegionId& id) { return id.value != INVALID_REGION_ID.value; });

    // Population ID with Python-friendly methods
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // Neuron Type enum with string conversion
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("__str__", [neuronTypeToString](NeuronType type) { return neuronTypeToString(type); });

    // Synapse Type enum with string conversion
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("__str__", [synapseTypeToString](SynapseType type) {
            switch (type) {
                case SynapseType::Excitatory: return "Excitatory";
                case SynapseType::Inhibitory: return "Inhibitory";
                case SynapseType::Modulatory: return "Modulatory";
                case SynapseType::Electrical: return "Electrical";
                case SynapseType::GapJunction: return "GapJunction";
                default: return "Unknown";
            }
        });

    // Developmental Stage enum with string conversion
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("__str__", [developmentalStageToString](DevelopmentalStage stage) { return developmentalStageToString(stage); });

    // Action Type enum with string conversion
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
        .export_values()
        .def("__str__", [actionTypeToString](ActionType type) { return actionTypeToString(type); });

    // Motor Command enum with string conversion
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("__str__", [motorCommandToString](MotorCommand cmd) { return motorCommandToString(cmd); });

    // World Object Type enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Configuration class with enhanced Python API
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON/YAML file. Supports both JSON and YAML formats.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Supports --key=value and -key value formats.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a YAML file.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Raises ConfigError if key not found.")
        .def("get", &Config::get<int>, py::arg("key"),
             "Get integer configuration value. Raises ConfigError if key not found.")
        .def("get", &Config::get<int64_t>, py::arg("key"),
             "Get 64-bit integer configuration value. Raises ConfigError if key not found.")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get float/double configuration value. Raises ConfigError if key not found.")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get boolean configuration value. Raises ConfigError if key not found.")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get string configuration value. Raises ConfigError if key not found.")
        .def("getOr", py::overload_cast<const std::string&, const int&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get configuration value or return default if not found.")
        .def("getOr", py::overload_cast<const std::string&, const int64_t&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get 64-bit integer configuration value or return default if not found.")
        .def("getOr", py::overload_cast<const std::string&, const double&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get float/double configuration value or return default if not found.")
        .def("getOr", py::overload_cast<const std::string&, const bool&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get boolean configuration value or return default if not found.")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr),
             py::arg("key"), py::arg("default_value"),
             "Get string configuration value or return default if not found.")
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set string configuration value.")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set integer configuration value.")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set float/double configuration value.")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean configuration value.")
        .def("set", py::overload_cast<const std::string&, const ConfigValue&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value with type checking.")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get configuration summary as string.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("loadFromJSON", [](Config& self, const std::string& json) {
            // Create temporary file with JSON content
            std::string tempFile = "/tmp/nlm_config_" + std::to_string(std::time(nullptr)) + ".json";
            std::ofstream file(tempFile);
            if (!file.is_open()) {
                throw ConfigError("Cannot create temporary file for JSON loading");
            }
            file << json;
            file.close();
            bool result = self.loadFromFile(tempFile);
            std::remove(tempFile.c_str());
            if (!result) {
                throw ConfigError("Failed to parse JSON configuration");
            }
            return result;
        }, py::arg("json"), "Load configuration from JSON string.")
        .def("toJSON", [](const Config& self) {
            // Convert configuration to JSON string
            std::ostringstream oss;
            oss << "{\n";
            auto keys = self.getKeys();
            for (size_t i = 0; i < keys.size(); ++i) {
                oss << "  \"" << keys[i] << "\": ";
                auto val = self.get<std::string>(keys[i]);
                if (val.has_value()) {
                    oss << "\"" << val.value() << "\"";
                } else {
                    auto valInt = self.get<int64_t>(keys[i]);
                    if (valInt.has_value()) {
                        oss << valInt.value();
                    } else {
                        auto valDouble = self.get<double>(keys[i]);
                        if (valDouble.has_value()) {
                            oss << valDouble.value();
                        } else {
                            auto valBool = self.get<bool>(keys[i]);
                            if (valBool.has_value()) {
                                oss << (valBool.value() ? "true" : "false");
                            }
                        }
                    }
                }
                if (i < keys.size() - 1) {
                    oss << ",\n";
                } else {
                    oss << "\n";
                }
            }
            oss << "}\n";
            return oss.str();
        }, "Convert configuration to JSON string.");

    // Sensory Input base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with Python-friendly interface
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("normalize", [](Vision& self) {
            self.normalize();
        }, "Normalize vision data to [0, 1] range")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            self.resize(newWidth, newHeight);
        }, py::arg("newWidth"), py::arg("newHeight"),
            "Resize vision data to specified dimensions");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("computeFFT", [](Audio& self) {
            self.computeFFT();
            return self.getFFTData();
        }, "Compute FFT and return frequency domain data");

    // Internal Signals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a new internal signal value")
        .def("getAverage", [](const InternalSignals& self) {
            return self.getAverage();
        }, "Get average of all internal signals")
        .def("getMax", [](const InternalSignals& self) {
            return self.getMax();
        }, "Get maximum internal signal value")
        .def("getMin", [](const InternalSignals& self) {
            return self.getMin();
        }, "Get minimum internal signal value")
        .def("getRange", [](const InternalSignals& self) {
            return self.getRange();
        }, "Get range of internal signals (max - min)")
        .def("normalize", [](InternalSignals& self, float targetMean = 0.0f, float targetStd = 1.0f) {
            self.normalize(targetMean, targetStd);
        }, py::arg("targetMean") = 0.0f, py::arg("targetStd") = 1.0f,
            "Normalize internal signals to target mean and standard deviation");

    // Action class with Python-friendly interface
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getDescription", [](const Action& self) {
            return self.getDescription();
        }, "Get human-readable description of action")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& self) {
            return "<Action: " + actionTypeToString(self.getType()) + " (" + std::to_string(self.getParameters().size()) + " params)>";
        });

    // World Object class with Python-friendly interface
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
        .def_readwrite("active", &WorldObject::active)
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
            "Calculate distance to position (x, y)")
        .def("is_colliding", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"),
            "Check if colliding with another world object")
        .def("__repr__", [](const WorldObject& self) {
            return "<WorldObject: " + worldObjectTypeToString(self.type) + " at (" + std::to_string(self.x) + ", " + std::to_string(self.y) + ")>";
        });

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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get current position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set current position")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("isHealthy", [](const AgentBody& self) {
            return self.health > 0.5f;
        }, "Check if agent is healthy")
        .def("__repr__", [](const AgentBody& self) {
            return "<AgentBody: pos(" + std::to_string(self.x) + ", " + std::to_string(self.y) + ") speed(" + 
                   std::to_string(std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY)) + ")>";
        });

    // Action Result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_successful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("get_reward", [](const ActionResult& self) {
            return self.reward;
        }, "Get reward value")
        .def("get_message", [](const ActionResult& self) {
            return self.message;
        }, "Get result message")
        .def("__repr__", [](const ActionResult& self) {
            return "<ActionResult: success=" + std::string(self.success ? "true" : "false") + 
                   ", reward=" + std::to_string(self.reward) + ", msg=\"" + self.message + "\">";
        });

    // Sensory Percept class with Python-friendly interface
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("has_vision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("has_touch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept has internal data")
        .def("has_proprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if percept has proprioception data")
        .def("has_audio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        .def("get_signal_summary", [](const SensoryPercept& self) {
            std::stringstream ss;
            ss << "SensoryPercept: ";
            if (!self.getVision().empty()) ss << "Vision(" << self.getVision().size() << ") ";
            if (!self.getTouch().empty()) ss << "Touch(" << self.getTouch().size() << ") ";
            if (!self.getInternal().empty()) ss << "Internal(" << self.getInternal().size() << ") ";
            if (!self.getProprioception().empty()) ss << "Proprioception(" << self.getProprioception().size() << ") ";
            if (!self.getAudio().empty()) ss << "Audio(" << self.getAudio().size() << ") ";
            return ss.str();
        }, "Get string summary of all available signals");

    // Simple World class with enhanced Python API
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world at current time")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object to simulation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getMetrics", [](SimpleWorld& self) {
            // Create metrics dict
            py::dict metrics;
            metrics["width"] = self.getWidth();
            metrics["height"] = self.getHeight();
            metrics["time"] = self.getSimulationTime();
            return metrics;
        }, "Get world simulation metrics")
        .def("is_agent_at", [](SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            float dx = body->x - x;
            float dy = body->y - y;
            float distance = std::sqrt(dx*dx + dy*dy);
            return distance < body->getRadius();
        }, py::arg("x"), py::arg("y"),
            "Check if agent is at position (x, y)")
        .def("distance_to_agent", [](SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            float dx = body->x - x;
            float dy = body->y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
            "Calculate distance from position to agent")
        .def("__repr__", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return "<SimpleWorld: " + std::to_string(self.getWidth()) + "x" + std::to_string(self.getHeight()) +
                   ", agent@(" + std::to_string(body->x) + ", " + std::to_string(body->y) + ")>";
        });

    // Brain class with enhanced Python API
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Raises BrainError if initialization fails.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with default timestep")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with specified timestep")
        .def("stepWithTime", [](Brain& self, uint64_t step, double time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"),
            "Perform simulation step with explicit timestamp (convenience method)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Raises BrainError if invalid input.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Raises BrainError if neuron not found.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of specified type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Raises BrainError if save fails.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises BrainError if load fails.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get region by ID. Raises BrainError if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration object.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status information.")
        .def("isHealthy", [](Brain& self) {
            return self.getTotalNeuronCount() > 0 && self.getTotalSynapseCount() > 0;
        }, "Check if brain is in a healthy state.")
        .def("get_neurons_by_type", [](Brain& self, NeuronType type) {
            std::vector<NeuronId> neurons;
            for (size_t i = 0; i < self.getRegionCount(); ++i) {
                auto region = self.getRegion(RegionId(i + 1));
                if (region) {
                    for (size_t j = 0; j < region->getPopulationCount(); ++j) {
                        auto pop = region->getPopulation(PopulationId(j + 1));
                        if (pop && pop->getNeuronType() == type) {
                            for (auto* neuron : pop->getNeurons()) {
                                neurons.push_back(neuron->getId());
                            }
                        }
                    }
                }
            }
            return neurons;
        }, py::arg("type"),
            "Get all neurons of specified type.")
        .def("get_synapses_by_type", [](Brain& self, SynapseType type) {
            std::vector<SynapseId> synapses;
            for (size_t i = 0; i < self.getRegionCount(); ++i) {
                auto region = self.getRegion(RegionId(i + 1));
                if (region) {
                    for (auto* syn : region->getSynapses()) {
                        if (syn->getType() == type) {
                            synapses.push_back(syn->getId());
                        }
                    }
                }
            }
            return synapses;
        }, py::arg("type"),
            "Get all synapses of specified type.")
        .def("__repr__", [](Brain& self) {
            return "<Brain: " + std::to_string(self.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(self.getTotalSynapseCount()) + " synapses, " +
                   std::to_string(self.getFiringNeuronCount()) + " firing>";
        });

    // Agent Brain class with enhanced Python API
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world environment.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled.")
        .def("getFeatures", [](AgentBrain& self) {
            py::dict features;
            features["reward_modulation"] = self.isRewardModulationEnabled();
            features["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            features["development"] = self.isDevelopmentEnabled();
            features["curiosity"] = self.isCuriosityEnabled();
            return features;
        }, "Get dictionary of enabled features.")
        .def("enableFeatures", [](AgentBrain& self, py::dict features) {
            if (py::hasattr(features, "reward_modulation")) {
                self.enableRewardModulation(py::cast<bool>(features["reward_modulation"]));
            }
            if (py::hasattr(features, "structural_plasticity")) {
                self.enableStructuralPlasticity(py::cast<bool>(features["structural_plasticity"]));
            }
            if (py::hasattr(features, "development")) {
                self.enableDevelopment(py::cast<bool>(features["development"]));
            }
            if (py::hasattr(features, "curiosity")) {
                self.enableCuriosity(py::cast<bool>(features["curiosity"]));
            }
        }, py::arg("features"), "Enable multiple features at once.")
        .def("get_stats", [](AgentBrain& self) {
            py::dict stats;
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation"] = self.getNeuromodulationLevel();
            stats["development_stage"] = self.getDevelopmentalStage();
            return stats;
        }, "Get dictionary of agent statistics.")
        .def("__repr__", [](AgentBrain& self) {
            auto features = self.getFeatures();
            std::string featureStr = "";
            for (auto& item : features) {
                if (featureStr.empty()) featureStr = "";
                featureStr += py::cast<std::string>(item.first) + "=" + 
                             (py::cast<bool>(item.second) ? "true" : "false") + " ";
            }
            return "<AgentBrain: features=" + featureStr + ">";
        });

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with optimized settings for research.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration. Raises ConfigError if config is invalid.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple 2D world environment.")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. Raises BrainError if brain is invalid.")

    // Configuration constants
    m.attr("ConfigSource_Default") = py::cast(ConfigSource::Default);
    m.attr("ConfigSource_File") = py::cast(ConfigSource::File);
    m.attr("ConfigSource_CommandLine") = py::cast(ConfigSource::CommandLine);
    m.attr("ConfigSource_Runtime") = py::cast(ConfigSource::Runtime);

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error constants
    m.attr("ERROR_CODE_SUCCESS") = 0;
    m.attr("ERROR_CODE_FAILURE") = 1;
    m.attr("ERROR_CODE_INVALID_PARAM") = 2;
    m.attr("ERROR_CODE_NOT_FOUND") = 3;
    m.attr("ERROR_CODE_FILE_NOT_FOUND") = 4;
    m.attr("ERROR_CODE_CORRUPT_DATA") = 5;

    // Python-friendly shortcut functions
    m.def("quickStart", []() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return py::make_tuple(config, brain, world, agent);
    }, "Quick start function that creates a complete working NLM system.")

    m.def("run_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                          int numSteps, bool verbose = false) {
        for (int step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply curiosity and development
            agent->updateDevelopment(0.1);
            
            if (verbose && step % 10 == 0) {
                py::print("Step " + std::to_string(step) + ": " +
                         "neurons_firing=" + std::to_string(agent->getBrain()->getFiringNeuronCount()) +
                         ", curiosity=" + std::to_string(agent->getCuriosityLevel()) +
                         ", novelty=" + std::to_string(agent->getNoveltyLevel()));
            }
        }
        
        return agent->getBrain()->getTotalSpikeCount();
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("verbose") = false,
         "Run complete agent episode with brain-world interaction.");

    // Version and information
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python Bindings - Phase 6 Complete Implementation";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__status__") = "Production";
}

} // namespace nlm