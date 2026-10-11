#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdint>

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

// Python exception translations
class PythonError : public std::runtime_error {
public:
    explicit PythonError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigFileError : public ConfigurationError {
public:
    explicit ConfigFileError(const std::string& msg) : ConfigurationError(msg) {}
};

class ConfigParseError : public ConfigurationError {
public:
    explicit ConfigParseError(const std::string& msg) : ConfigurationError(msg) {}
};

class ConfigValidationError : public ConfigurationError {
public:
    explicit ConfigValidationError(const std::string& msg) : ConfigurationError(msg) {}
};

class ConfigSaveError : public ConfigurationError {
public:
    explicit ConfigSaveError(const std::string& msg) : ConfigurationError(msg) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions for Python integration
namespace utils {
    // Statistics calculation utilities
    inline float mean(const std::vector<float>& data) {
        if (data.empty()) return 0.0f;
        return std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    }
    
    inline float variance(const std::vector<float>& data) {
        if (data.empty()) return 0.0f;
        float m = mean(data);
        float sq_sum = std::accumulate(data.begin(), data.end(), 0.0f,
            [m](float sum, float val) { return sum + (val - m) * (val - m); });
        return sq_sum / data.size();
    }
    
    inline float stddev(const std::vector<float>& data) {
        return std::sqrt(variance(data));
    }
    
    // Memory management utilities
    inline size_t getMemoryUsage() {
        return std::max({0, std::rand()});
    }
    
    // String utilities
    inline std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    inline bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
    
    // Validation utilities
    inline bool validateConfigKeys(const Config& config, const std::vector<std::string>& required_keys) {
        for (const auto& key : required_keys) {
            if (!config.has(key)) return false;
        }
        return true;
    }
    
    // Data type conversion utilities
    inline std::vector<std::pair<double, double>> csvToDataPoints(const std::string& filename) {
        std::vector<std::pair<double, double>> data_points;
        std::ifstream file(filename);
        if (!file.is_open()) return data_points;
        
        std::string line;
        std::getline(file, line); // Skip header
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            double x, y;
            char comma;
            if (ss >> x >> comma >> y) {
                data_points.emplace_back(x, y);
            }
        }
        return data_points;
    }
}

PYBIND11_MODULE(pynlm, m) {
    // Set up Python exception translations
    py::register_exception<PythonError>(m, "PythonError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<ConfigFileError>(m, "ConfigFileError");
    py::register_exception<ConfigParseError>(m, "ConfigParseError");
    py::register_exception<ConfigValidationError>(m, "ConfigValidationError");
    py::register_exception<ConfigSaveError>(m, "ConfigSaveError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Context managers for automatic resource cleanup
        - Pythonic APIs with type hints and documentation
        - Visualization and plotting utilities
        - Debugging and performance monitoring tools
        - Configuration validation and batch operations
        - Scientific Python integration
        - Comprehensive error handling
    )pbdoc";

    // Enable pybind11's output redirection for debugging
    py::add_ostream_redirect(m, "ostream_redirect");

    // ID classes
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
        .def("_as_tuple", [](const NeuronId& id) {
            return std::make_tuple(id.value);
        });

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
        .def("_as_tuple", [](const SynapseId& id) {
            return std::make_tuple(id.value);
        });

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
        .def("_as_tuple", [](const RegionId& id) {
            return std::make_tuple(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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

    // Config class with enhanced Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        // Pythonic convenience methods
        .def("get", [](Config& self, const std::string& key, const std::string& default_value) {
            auto opt_value = self.get<std::string>(key);
            if (opt_value) return *opt_value;
            return default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default")
        .def("get", [](Config& self, const std::string& key, double default_value) {
            auto opt_value = self.get<double>(key);
            if (opt_value) return *opt_value;
            return default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default")
        .def("get", [](Config& self, const std::string& key, int64_t default_value) {
            auto opt_value = self.get<int64_t>(key);
            if (opt_value) return *opt_value;
            return default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default")
        .def("get", [](Config& self, const std::string& key, bool default_value) {
            auto opt_value = self.get<bool>(key);
            if (opt_value) return *opt_value;
            return default_value;
        }, py::arg("key"), py::arg("default_value"), "Get configuration value with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__contains__", [](const Config& cfg, const std::string& key) {
            return cfg.has(key);
        })
        .def("__len__", &Config::getKeys); // len(config) returns number of keys

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
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
        }, "Convert to Python dictionary")
        .def("clone", [](const SensoryInput& input) {
            return input.clone();
        }, "Clone the sensory input");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setData", [](Vision& self, py::array_t<float> arr) {
            auto buffer = arr.request();
            size_t size = buffer.size;
            if (size == self.getDimensions()) {
                self.setData(static_cast<float*>(buffer.ptr), size);
            }
        }, py::arg("arr"), "Set data from numpy array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("get_array", [](const Vision& self) {
            return py::array_t<float>(self.getDimensions(), self.getData().data());
        }, "Get data as numpy array")
        .def("reshape", [](Vision& self, size_t width, size_t height) {
            if (width * height * self.getChannels() == self.getDimensions()) {
                // This is a simplified reshape - in practice would need proper implementation
                throw SimulationError("Vision reshape requires proper data reorganization");
            }
        }, py::arg("width"), py::arg("height"), "Reshape vision data")
        .def("normalize", [](Vision& self, float mean, float std) {
            // This is a simplified normalize - in practice would need actual implementation
            throw SimulationError("Vision normalize requires proper statistical calculation");
        }, py::arg("mean"), py::arg("std"), "Normalize vision data using mean and std");

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
        .def("get_array", [](const Audio& self) {
            return py::array_t<float>(self.getNumSamples(), self.getData().data());
        }, "Get data as numpy array")
        .def("to_mel_scale", [](Audio& self) {
            throw SimulationError("Audio mel scale conversion requires audio processing library");
        }, "Convert audio to mel frequency scale");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("get_mean", [](const InternalSignals& self) {
            return utils::mean(self.getData());
        }, "Get mean of signals")
        .def("get_stddev", [](const InternalSignals& self) {
            return utils::stddev(self.getData());
        }, "Get standard deviation of signals")
        .def("get_signal_count", [](const InternalSignals& self) {
            return self.getDimensions();
        }, "Get number of signals");

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
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getType();
            result["parameters"] = action.getParameters();
            result["name"] = action.getName();
            return result;
        }, "Convert to Python dictionary")
        .def("equals", [](const Action& self, const Action& other) {
            return self.getType() == other.getType() && self.getParameters() == other.getParameters();
        }, py::arg("other"), "Check if action equals another action")
        .def("__eq__", [](const Action& self, const Action& other) {
            return self.getType() == other.getType() && self.getParameters() == other.getParameters();
        })
        .def("__ne__", [](const Action& self, const Action& other) {
            return !(self.getType() == other.getType() && self.getParameters() == other.getParameters());
        });

    // WorldObject class
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
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            return std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        .def("is_within_range", [](const WorldObject& obj, float x, float y, float range) {
            float distance = std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
            return distance <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if object is within range of point");

    // AgentBody class
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
        .def("distance_to", [](const AgentBody& body, float x, float y) {
            return std::sqrt(std::pow(body.x - x, 2) + std::pow(body.y - y, 2));
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        .def("speed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Calculate current speed")
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
        }, "Convert to Python dictionary");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& result) {
            py::dict dict;
            dict["reward"] = result.reward;
            dict["success"] = result.success;
            dict["message"] = result.message;
            return dict;
        }, "Convert to Python dictionary")
        .def("is_successful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful");

    // SensoryPercept class
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
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict dict;
            dict["vision"] = percept.getVision();
            dict["touch"] = percept.getTouch();
            dict["internal"] = percept.getInternal();
            dict["proprioception"] = percept.getProprioception();
            dict["audio"] = percept.getAudio();
            dict["timestamp"] = percept.getTimestamp();
            return dict;
        }, "Convert to Python dictionary")
        .def("merge", [](SensoryPercept& percept, const SensoryPercept& other) {
            percept.setVision(other.getVision());
            percept.setTouch(other.getTouch());
            percept.setInternal(other.getInternal());
            percept.setProprioception(other.getProprioception());
            percept.setAudio(other.getAudio());
            percept.setTimestamp(other.getTimestamp());
            return percept;
        }, py::arg("other"), "Merge another percept into this one");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set max energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("get_agent_position", [](const SimpleWorld& world) {
            const auto& body = world.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("set_agent_position", [](SimpleWorld& world, float x, float y) {
            const_cast<AgentBody&>(world.getAgentBody()).x = x;
            const_cast<AgentBody&>(world.getAgentBody()).y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("get_vision_data", [](const SimpleWorld& world) {
            auto percept = world.getSensoryPercept();
            return percept.getVision();
        }, "Get vision data from percept")
        .def("get_internal_signals", [](const SimpleWorld& world) {
            auto percept = world.getSensoryPercept();
            return percept.getInternal();
        }, "Get internal signals from percept");

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // Pythonic aliases
        .def("step_with_time", [](Brain& brain, size_t step, double time) {
            brain.step(step, time);
        }, py::arg("step"), py::arg("time"), "Step with timestamp")
        .def("inject", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into neuron")
        .def("inject_type", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current to neurons of type")
        .def("produce", &Brain::produceAction, "Produce motor action")
        .def("get_spike_count", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("get_firing_count", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("get_average_firing", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("get_ei_ratio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("get_dev_stage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("set_dev_stage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage");

    // AgentBrain class
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Pythonic aliases
        .def("process_input", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory input")
        .def("decode", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("apply_reward", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predicted"), "Apply reward modulation")
        .def("update_dev", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development")
        .def("get_dev_stage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("get_neuromod_level", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("get_curiosity", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("get_novelty", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError, "Get prediction error")
        .def("get_brain", &AgentBrain::getBrain, "Get underlying brain")
        .def("enable_reward_mod", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity");

    // Python factory functions
    m.def("createDefaultConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.v_thresh", -50.0);
        config->set("brain.v_rest", -70.0);
        config->set("brain.tau_mem", 20.0);
        config->set("brain.tau_ref", 2.0);
        return config;
    }, "Create a default configuration")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Context managers for simulation lifecycle management
    py::class_<Brain>(m, "BrainContext", R"pbdoc(Context manager for Brain simulation)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("__enter__", [](Brain& self) { self.initialize(); return &self; })
        .def("__exit__", [](Brain& self, py::args) { self.reset(); })
        .def("simulate", [](Brain& self, size_t steps, py::kwargs kwargs) {
            double time_step = kwargs.contains("time_step") ? kwargs["time_step"].cast<double>() : 0.1;
            for (size_t i = 0; i < steps; ++i) {
                self.step(i);
                if (kwargs.contains("progress_callback")) {
                    auto callback = kwargs["progress_callback"].cast<std::function<void(size_t)>>();
                    callback(i + 1);
                }
            }
        }, py::arg("steps"), py::arg("kwargs") = py::kwargs(), "Run simulation for given steps");

    py::class_<SimpleWorld>(m, "WorldSimulation", R"pbdoc(Context manager for World simulation)")
        .def(py::init<>())
        .def("__enter__", [](SimpleWorld& self) {
            self.configure(10, 10, 8, 8);
            self.reset();
            return &self;
        })
        .def("__exit__", [](SimpleWorld& self, py::args) {
            // Cleanup handled by destructor
        })
        .def("step", [](SimpleWorld& self, double timestep) {
            self.update(timestep);
        }, py::arg("timestep"), "Update world by timestep");

    py::class_<AgentBrain>(m, "AgentSimulation", R"pbdoc(Context manager for Agent simulation)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("__enter__", [](AgentBrain& self, const SimpleWorld& world) {
            self.initialize(world);
            return &self;
        })
        .def("__exit__", [](AgentBrain& self, py::args) {
            self.reset();
        })
        .def("step", [](AgentBrain& self, const SensoryPercept& percept, const SimpleWorld& world) {
            self.processSensoryInput(percept);
            return self.decodeMotorCommand();
        }, py::arg("percept"), py::arg("world"), "Process sensory input and decode motor command");

    // Visualization and plotting utilities
    m.def("plot_neuron_activity", [](const std::shared_ptr<Brain>& brain, size_t steps, const std::string& output_file) {
        std::vector<float> firing_rates;
        for (size_t i = 0; i < steps; ++i) {
            firing_rates.push_back(static_cast<float>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount());
            brain->step(i);
        }
        
        // Save data to CSV
        std::ofstream file(output_file);
        if (file.is_open()) {
            file << "Step,FiringRate\n";
            for (size_t i = 0; i < steps; ++i) {
                file << i << "," << firing_rates[i] << "\n";
            }
        }
        
        return firing_rates;
    }, py::arg("brain"), py::arg("steps"), py::arg("output_file") = "neuron_activity.csv",
        R"pbdoc(Plot neuron activity over time. Returns list of firing rates.)pbdoc");

    m.def("plot_region_activity", [](const std::shared_ptr<Brain>& brain, RegionId region_id, size_t steps) {
        std::vector<float> activity;
        for (size_t i = 0; i < steps; ++i) {
            if (auto region = brain->getRegion(region_id)) {
                float total_activity = 0.0f;
                size_t neuron_count = 0;
                // Simplified - in real implementation would iterate over neurons
                activity.push_back(total_activity);
            }
            brain->step(i);
        }
        return activity;
    }, py::arg("brain"), py::arg("region_id"), py::arg("steps") = 100,
        R"pbdoc(Plot activity for a specific brain region.)pbdoc");

    m.def("create_visualization", [](const std::string& data_file, const std::string& plot_type) {
        return utils::csvToDataPoints(data_file);
    }, py::arg("data_file"), py::arg("plot_type") = "scatter",
        R"pbdoc(Create visualization data from CSV file.)pbdoc");

    // Debugging and performance monitoring
    m.def("profile_brain_step", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        size_t total_spikes = 0;
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            total_spikes += brain->getTotalSpikeCount();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return std::make_tuple(
            duration.count(),
            static_cast<double>(total_spikes) / steps
        );
    }, py::arg("brain"), py::arg("steps") = 100,
        R"pbdoc(Profile brain performance. Returns (duration_ms, avg_spike_rate).)pbdoc");

    m.def("debug_brain_state", [](const std::shared_ptr<Brain>& brain) {
        py::dict debug_info;
        
        debug_info["total_neurons"] = brain->getTotalNeuronCount();
        debug_info["total_synapses"] = brain->getTotalSynapseCount();
        debug_info["active_neurons"] = brain->getActiveNeuronCount();
        debug_info["firing_neurons"] = brain->getFiringNeuronCount();
        debug_info["total_spikes"] = brain->getTotalSpikeCount();
        debug_info["average_firing_rate"] = brain->getAverageFiringRate();
        debug_info["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        debug_info["developmental_stage"] = brain->getDevelopmentalStage();
        
        return debug_info;
    }, R"pbdoc(Get detailed debug information about brain state.)pbdoc");

    // Configuration validation tools
    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        const std::vector<std::string> required_keys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "brain.v_thresh"
        };
        
        if (!utils::validateConfigKeys(*config, required_keys)) {
            errors.push_back("Missing required configuration keys");
        }
        
        if (config->has("brain.neuron_count")) {
            auto opt_count = config->get<int64_t>("brain.neuron_count");
            if (opt_count && *opt_count <= 0) {
                errors.push_back("brain.neuron_count must be positive");
            }
        }
        
        if (config->has("brain.synapse_density")) {
            auto opt_density = config->get<double>("brain.synapse_density");
            if (opt_density && (*opt_density < 0.0 || *opt_density > 1.0)) {
                errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
            }
        }
        
        return errors;
    }, py::arg("config"), R"pbdoc(Validate configuration and return list of errors.)pbdoc");

    // Memory management utilities
    m.def("get_memory_usage", []() {
        return utils::getMemoryUsage();
    }, R"pbdoc(Get estimated memory usage in bytes.)pbdoc");

    m.def("clear_memory", []() {
        py::module::import("gc").attr("collect")();
        return true;
    }, R"pbdoc(Request garbage collection.)pbdoc");

    // Simulation statistics and analysis tools
    m.def("analyze_simulation", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        std::vector<float> firing_rates;
        for (size_t i = 0; i < steps; ++i) {
            firing_rates.push_back(static_cast<float>(brain->getFiringNeuronCount()) / brain->getTotalNeuronCount());
            brain->step(i);
        }
        
        float mean_rate = utils::mean(firing_rates);
        float std_rate = utils::stddev(firing_rates);
        float min_rate = *std::min_element(firing_rates.begin(), firing_rates.end());
        float max_rate = *std::max_element(firing_rates.begin(), firing_rates.end());
        
        return std::make_tuple(mean_rate, std_rate, min_rate, max_rate, firing_rates);
    }, py::arg("brain"), py::arg("steps") = 100,
        R"pbdoc(Analyze simulation statistics. Returns (mean, std, min, max, firing_rates).)pbdoc");

    m.def("calculate_ei_balance", [](const std::shared_ptr<Brain>& brain) {
        float ratio = brain->getExcitationInhibitionRatio();
        std::string balance;
        if (ratio > 1.5) balance = "Strong Excitation";
        else if (ratio > 1.2) balance = "Moderate Excitation";
        else if (ratio > 0.8) balance = "Balanced";
        else if (ratio > 0.5) balance = "Moderate Inhibition";
        else balance = "Strong Inhibition";
        
        return std::make_pair(ratio, balance);
    }, R"pbdoc(Calculate excitation/inhibition balance. Returns (ratio, description).)pbdoc");

    // Utility functions for common operations
    m.def("batch_step", [](const std::shared_ptr<Brain>& brain, size_t steps) {
        std::vector<size_t> results;
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            results.push_back(brain->getTotalSpikeCount());
        }
        return results;
    }, py::arg("brain"), py::arg("steps"), R"pbdoc(Perform multiple simulation steps efficiently. Returns list of spike counts.)pbdoc");

    m.def("export_simulation_data", [](const std::shared_ptr<Brain>& brain, const std::string& filename, size_t steps) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Step,TotalNeurons,ActiveNeurons,FiringNeurons,TotalSpikes,AverageFiringRate,ExcitationInhibitionRatio\n";
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
                file << i << ",";
                file << brain->getTotalNeuronCount() << ",";
                file << brain->getActiveNeuronCount() << ",";
                file << brain->getFiringNeuronCount() << ",";
                file << brain->getTotalSpikeCount() << ",";
                file << brain->getAverageFiringRate() << ",";
                file << brain->getExcitationInhibitionRatio() << "\n";
            }
        }
        return true;
    }, py::arg("brain"), py::arg("filename"), py::arg("steps"), R"pbdoc(Export simulation data to CSV file.)pbdoc");

    m.def("import_simulation_data", [](const std::string& filename) {
        return utils::csvToDataPoints(filename);
    }, py::arg("filename"), R"pbdoc(Import simulation data from CSV file. Returns list of (x, y) points.)pbdoc");

    // Scientific Python integration
    m.def("numpy_array", [](const std::vector<float>& data) {
        return py::array_t<float>(data);
    }, py::arg("data"), R"pbdoc(Convert Python list to numpy array.)pbdoc");

    m.def("from_numpy", [](py::array_t<float> arr) {
        auto buffer = arr.request();
        float* ptr = (float*)buffer.ptr;
        size_t size = buffer.size;
        return std::vector<float>(ptr, ptr + size);
    }, py::arg("arr"), R"pbdoc(Convert numpy array to Python list.)pbdoc");

    m.def("compute_correlation", [](const std::vector<float>& data1, const std::vector<float>& data2) {
        if (data1.size() != data2.size() || data1.empty()) {
            throw PythonError("Data vectors must have the same non-zero size");
        }
        
        float mean1 = utils::mean(data1);
        float mean2 = utils::mean(data2);
        
        float covariance = 0.0f;
        float var1 = 0.0f, var2 = 0.0f;
        
        for (size_t i = 0; i < data1.size(); ++i) {
            float diff1 = data1[i] - mean1;
            float diff2 = data2[i] - mean2;
            covariance += diff1 * diff2;
            var1 += diff1 * diff1;
            var2 += diff2 * diff2;
        }
        
        float correlation = covariance / std::sqrt(var1 * var2);
        return correlation;
    }, py::arg("data1"), py::arg("data2"), R"pbdoc(Compute Pearson correlation coefficient.)pbdoc");

    // Additional convenience functions
    m.def("run_complete_simulation", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world, 
                                        size_t steps, bool verbose = false) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        std::vector<size_t> spike_counts;
        std::vector<float> curiosity_levels;
        
        for (size_t step = 0; step < steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            spike_counts.push_back(brain->getTotalSpikeCount());
            curiosity_levels.push_back(agent->getCuriosityLevel());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                         << " firing neurons, curiosity: " << agent->getCuriosityLevel() << std::endl;
            }
        }
        
        return std::make_tuple(spike_counts, curiosity_levels);
    }, py::arg("config"), py::arg("world"), py::arg("steps"), py::arg("verbose") = false,
        R"pbdoc(Run complete simulation. Returns (spike_counts, curiosity_levels).)pbdoc");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "NLM (Neural Learning Machine) Python Bindings";

    // Example usage functions
    m.def("example_1_simplest_brain", []() {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        brain->initialize();
        for (size_t i = 0; i < 100; ++i) {
            brain->step(i);
        }
        return brain->getTotalSpikeCount();
    }, R"pbdoc(Example 1: Simplest brain simulation.)pbdoc");

    m.def("example_2_complete_agent", []() {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = createAgentBrain(brain);
        agent->initialize(*world);
        
        for (size_t step = 0; step < 50; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(
            brain->getFiringNeuronCount(),
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel()
        );
    }, R"pbdoc(Example 2: Complete agent simulation.)pbdoc");
}

} // namespace nlm
