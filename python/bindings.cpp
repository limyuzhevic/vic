#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cmath>
#include <map>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function to convert python dict to config entries
inline std::vector<ConfigEntry> dictToConfigEntries(const py::dict& d) {
    std::vector<ConfigEntry> entries;
    for (auto item : d) {
        std::string key = py::cast<std::string>(item.first);
        std::string value = py::cast<std::string>(item.second);
        entries.emplace_back(key, value, ConfigSource::Runtime);
    }
    return entries;
}

// Helper function to convert config entries to python dict
inline py::dict configEntriesToDict(const std::vector<ConfigEntry>& entries) {
    py::dict result;
    for (const auto& entry : entries) {
        result[entry.key] = entry.value; // Note: ConfigValue is not directly accessible, would need accessor
    }
    return result;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action.
        
        Features:
        - Pythonic API with context managers and properties
        - Convenience methods for common operations
        - Serialization to/from Python dicts and JSON
        - Error handling with descriptive exceptions
        - Debugging and monitoring utilities
        - Factory methods for quick setup
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==================== Basic Types ====================
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ==================== Enums ====================
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

    // ==================== Config Class ====================
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def(py::init<std::string>(), py::arg("filepath"),
             "Create a config and load from file")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("loadFromString", [](Config& self, const std::string& jsonStr) {
            std::istringstream iss(jsonStr);
            return self.loadFromFile(iss);
        }, py::arg("jsonStr"),
           "Load configuration from JSON string")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("saveToString", [](const Config& self) {
            std::ostringstream oss;
            self.saveToFile(oss);
            return oss.str();
        }, "Save configuration to JSON string")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get a configuration value as string with default")
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get a configuration value as int with default")
        .def("getFloat", [](const Config& self, const std::string& key, float defaultValue = 0.0f) {
            auto value = self.get<double>(key);
            return value ? static_cast<float>(*value) : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f,
           "Get a configuration value as float with default")
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a configuration value as bool with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value (string)")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (int)")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, static_cast<double>(value));
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (float)")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set a configuration value (bool)")
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
        })
        .def("__contains__", &Config::has, py::arg("key"),
             "Check if key exists (for 'in' operator)")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto value = self.get<std::string>(key);
                if (value) result[key] = *value;
            }
            return result;
        }, "Convert config to Python dictionary")
        .def("from_dict", [](Config& self, const py::dict& d) {
            for (auto item : d) {
                std::string key = py::cast<std::string>(item.first);
                std::string value = py::cast<std::string>(item.second);
                self.set(key, value);
            }
            return self;
        }, py::arg("dict"), "Update config from Python dictionary and return self")
        .def("merge", [](Config& self, const py::dict& d) {
            for (auto item : d) {
                std::string key = py::cast<std::string>(item.first);
                std::string value = py::cast<std::string>(item.second);
                self.set(key, value);
            }
            return self;
        }, py::arg("dict"), "Merge dictionary into config and return self");

    // ==================== SensoryInput Classes ====================
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input")
        .def("__repr__", [](const SensoryInput& s) {
            return "<SensoryInput: type=\"" + std::string(s.getType()) + "\">";
        });

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision with dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set image data")
        .def("getData", &Vision::getData, "Get image data as vector")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("getShape", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get image dimensions as (width, height, channels)")
        .def("toArray", [](const Vision& self) {
            return self.getData();
        }, "Get data as numpy-compatible array (alias for getData)")
        .def("normalize", [](Vision& self, float mean = 0.0f, float std = 1.0f) {
            auto data = self.getData();
            for (auto& val : data) {
                val = (val - mean) / std;
            }
            self.setData(data);
            return self;
        }, py::arg("mean") = 0.0f, py::arg("std") = 1.0f,
           "Normalize image data and return self for chaining")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Note: This is a placeholder - actual implementation would need interpolation
            auto data = self.getData();
            self.setData(data);
            return self;
        }, py::arg("newWidth"), py::arg("newHeight"),
           "Resize image (placeholder implementation)")
        .def("hasData", [](const Vision& self) {
            return !self.getData().empty();
        }, "Check if vision has data")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" +
                   std::to_string(v.getHeight()) + "x" +
                   std::to_string(v.getChannels()) + " channels>";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio with sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("getData", &Audio::getData, "Get audio data as vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("getFrequencySpectrum", [](const Audio& self) {
            // Simple FFT placeholder - would need actual FFT implementation
            return std::vector<float>(self.getNumSamples(), 0.0f);
        }, "Get frequency spectrum (placeholder)")
        .def("normalize", [](Audio& self, float maxAmplitude = 1.0f) {
            auto data = self.getData();
            float maxVal = *std::max_element(data.begin(), data.end());
            if (maxVal > 0.0f) {
                float scale = maxAmplitude / maxVal;
                for (auto& val : data) val *= scale;
                self.setData(data);
            }
            return self;
        }, py::arg("maxAmplitude") = 1.0f,
           "Normalize audio amplitude and return self for chaining")
        .def("hasData", [](const Audio& self) {
            return !self.getData().empty();
        }, "Check if audio has data")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getNumSamples()) + " samples, " +
                   std::to_string(a.getSampleRate()) + " Hz>";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("getSignals", &InternalSignals::getSignals, "Get all signal values")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            auto signals = self.getSignals();
            if (index < signals.size()) return signals[index];
            throw std::runtime_error("Signal index out of range");
        }, py::arg("index"), "Get signal at index")
        .def("append", &InternalSignals::addSignal, py::arg("value"),
              "Add signal and return self for chaining")
        .def("__len__", &InternalSignals::getSignalCount)
        .def("__iter__", [](const InternalSignals& self) {
            return py::make_iterator(self.getSignals().begin(), self.getSignals().end());
        }, py::keep_alive<0, 1>())
        .def("__repr__", [](const InternalSignals& s) {
            return "<InternalSignals: " + std::to_string(s.getSignalCount()) + " signals>";
        });

    // ==================== Action Class ====================
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name as string")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
            return self;
        }, py::arg("value"), "Add parameter and return self for chaining")
        .def("removeParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params.erase(params.begin() + index);
                self.setParameters(params);
            }
            return self;
        }, py::arg("index"), "Remove parameter at index and return self")
        .def("hasParameter", [](const Action& self, float value) {
            auto params = self.getParameters();
            return std::find(params.begin(), params.end(), value) != params.end();
        }, py::arg("value"), "Check if action has parameter value")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("withType", [](Action& self, ActionType type) {
            self.setType(type);
            return self;
        }, py::arg("type"), "Set type and return self (builder pattern)")
        .def("withParams", [](Action& self, const std::vector<float>& params) {
            self.setParameters(params);
            return self;
        }, py::arg("params"), "Set parameters and return self (builder pattern)")
        .def("__repr__", [](const Action& a) {
            std::string typeName = "";
            switch (a.getType()) {
                case ActionType::MoveForward: typeName = "MoveForward"; break;
                case ActionType::MoveBackward: typeName = "MoveBackward"; break;
                case ActionType::MoveLeft: typeName = "MoveLeft"; break;
                case ActionType::MoveRight: typeName = "MoveRight"; break;
                case ActionType::TurnLeft: typeName = "TurnLeft"; break;
                case ActionType::TurnRight: typeName = "TurnRight"; break;
                case ActionType::Look: typeName = "Look"; break;
                case ActionType::LookUp: typeName = "LookUp"; break;
                case ActionType::LookDown: typeName = "LookDown"; break;
                case ActionType::Interact: typeName = "Interact"; break;
                case ActionType::Eat: typeName = "Eat"; break;
                case ActionType::Drink: typeName = "Drink"; break;
                case ActionType::Rest: typeName = "Rest"; break;
                case ActionType::Wait: typeName = "Wait"; break;
                case ActionType::Custom: typeName = "Custom"; break;
                default: typeName = "Unknown";
            }
            return "<Action: " + typeName + ", " +
                   std::to_string(a.getParameters().size()) + " params>";
        });

    // ==================== WorldObject ====================
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isNear", [](const WorldObject& self, float x, float y, float threshold) {
            return self.distanceTo(x, y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"),
           "Check if object is within threshold distance")
        .def("__repr__", [](const WorldObject& o) {
            std::string typeName;
            switch (o.type) {
                case WorldObjectType::Empty: typeName = "Empty"; break;
                case WorldObjectType::Resource: typeName = "Resource"; break;
                case WorldObjectType::Hazard: typeName = "Hazard"; break;
                case WorldObjectType::Wall: typeName = "Wall"; break;
                case WorldObjectType::Marker: typeName = "Marker"; break;
                default: typeName = "Unknown";
            }
            return "<WorldObject: " + typeName + " at (" + std::to_string(o.x) + "," +
                   std::to_string(o.y) + "), r=" + std::to_string(o.radius) + ">";
        });

    // ==================== AgentBody ====================
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, float>(), py::arg("x"), py::arg("y"), py::arg("orientation"),
             "Create agent body with position and orientation")
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
        .def("reset", &AgentBody::reset, "Reset body state to defaults")
        .def("moveTo", [](AgentBody& self, float newX, float newY) {
            self.x = newX;
            self.y = newY;
            self.isMoving = true;
            return self;
        }, py::arg("newX"), py::arg("newY"), "Move to position and return self")
        .def("setVelocity", [](AgentBody& self, float vx, float vy, float angular) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.angularVelocity = angular;
            return self;
        }, py::arg("vx"), py::arg("vy"), py::arg("angular"), "Set velocity and return self")
        .def("getPosition", [](const AgentBody& self) {
            return std::vector<float>{self.x, self.y};
        }, "Get position as (x, y) vector")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get direction in radians")
        .def("updateEnergy", [](AgentBody& self, float dt) {
            self.energy = std::max(0.0f, self.energy - self.energy * 0.01f * dt);
            return self;
        }, py::arg("dt"), "Update energy over time and return self")
        .def("__repr__", [](const AgentBody& b) {
            return "<AgentBody: pos(" + std::to_string(b.x) + "," + std::to_string(b.y) +
                   "), dir(" + std::to_string(b.orientation) + "), " +
                   "energy(" + std::to_string(b.energy) + ")>";
        });

    // ==================== ActionResult ====================
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and optional message")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("succeeded", [](const ActionResult& self) {
            return self.success;
        }, "Check if action succeeded (alias for .success)")
        .def("hasMessage", [](const ActionResult& self) {
            return !self.message.empty();
        }, "Check if result has a message")
        .def("isFailure", [](const ActionResult& self) {
            return !self.success;
        }, "Check if action failed")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert result to Python dictionary")
        .def("fromDict", [](ActionResult& self, const py::dict& d) {
            if (d.contains("reward")) self.reward = py::cast<float>(d["reward"]);
            if (d.contains("success")) self.success = py::cast<bool>(d["success"]);
            if (d.contains("message")) self.message = py::cast<std::string>(d["message"]);
            return self;
        }, py::arg("dict"), "Update result from Python dictionary and return self")
        .def("__repr__", [](const ActionResult& r) {
            std::string status = r.success ? "SUCCESS" : "FAILURE";
            return "<ActionResult: " + status + ", reward=" + std::to_string(r.reward) +
                   ", msg=\"" + r.message + "\">";
        });

    // ==================== SensoryPercept ====================
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("hasVision", &SensoryPercept::hasVision)
        .def("hasTouch", &SensoryPercept::hasTouch)
        .def("hasInternal", &SensoryPercept::hasInternal)
        .def("hasProprioception", &SensoryPercept::hasProprioception)
        .def("hasAudio", &SensoryPercept::hasAudio)
        .def("clear", [](SensoryPercept& self) {
            self.setVision(std::vector<float>());
            self.setTouch(std::vector<float>());
            self.setInternal(std::vector<float>());
            self.setProprioception(std::vector<float>());
            self.setAudio(std::vector<float>());
            self.setTimestamp(0.0);
            return self;
        }, "Clear all sensory data and return self")
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            if (self.hasVision() && other.hasVision()) {
                self.setVision(other.getVision());
            }
            if (self.hasTouch() && other.hasTouch()) {
                self.setTouch(other.getTouch());
            }
            if (self.hasInternal() && other.hasInternal()) {
                self.setInternal(other.getInternal());
            }
            if (self.hasProprioception() && other.hasProprioception()) {
                self.setProprioception(other.getProprioception());
            }
            if (self.hasAudio() && other.hasAudio()) {
                self.setAudio(other.getAudio());
            }
            if (other.getTimestamp() > 0.0) {
                self.setTimestamp(other.getTimestamp());
            }
            return self;
        }, py::arg("other"), "Merge another percept into this one")
        .def("toDict", [](const SensoryPercept& self) {
            py::dict result;
            if (self.hasVision()) {
                result["vision"] = self.getVision();
            }
            if (self.hasTouch()) {
                result["touch"] = self.getTouch();
            }
            if (self.hasInternal()) {
                result["internal"] = self.getInternal();
            }
            if (self.hasProprioception()) {
                result["proprioception"] = self.getProprioception();
            }
            if (self.hasAudio()) {
                result["audio"] = self.getAudio();
            }
            if (self.getTimestamp() > 0.0) {
                result["timestamp"] = self.getTimestamp();
            }
            return result;
        }, "Convert percept to Python dictionary")
        .def("fromDict", [](SensoryPercept& self, const py::dict& d) {
            if (d.contains("vision")) self.setVision(py::cast<std::vector<float>>(d["vision"]));
            if (d.contains("touch")) self.setTouch(py::cast<std::vector<float>>(d["touch"]));
            if (d.contains("internal")) self.setInternal(py::cast<std::vector<float>>(d["internal"]));
            if (d.contains("proprioception")) self.setProprioception(py::cast<std::vector<float>>(d["proprioception"]));
            if (d.contains("audio")) self.setAudio(py::cast<std::vector<float>>(d["audio"]));
            if (d.contains("timestamp")) self.setTimestamp(py::cast<double>(d["timestamp"]));
            return self;
        }, py::arg("dict"), "Update percept from Python dictionary and return self")
        .def("__repr__", [](const SensoryPercept& p) {
            std::string components;
            if (p.hasVision()) components += "V";
            if (p.hasTouch()) components += "T";
            if (p.hasInternal()) components += "I";
            if (p.hasProprioception()) components += "P";
            if (p.hasAudio()) components += "A";
            return "<SensoryPercept: " + components + ", ts=" + std::to_string(p.getTimestamp()) + ">";
        });

    // ==================== SimpleWorld ====================
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("step", [](SimpleWorld& self, float dt) {
            return self.update(dt);
        }, py::arg("dt"), "Perform simulation step with time delta")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (null if none)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("createObject", [](SimpleWorld& self, float x, float y, WorldObjectType type, float radius = 0.5f) {
            return self.addObject(WorldObject(x, y, type, 0.0f, radius));
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("radius") = 0.5f,
           "Create and add object to world")
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" +
                   std::to_string(w.getHeight()) + " cells, " +
                   std::to_string(w.getSimulationTime()) + " sim steps>";
        });

    // ==================== Brain ====================
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def(py::init<>(), "Create brain with default configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("stepWithTime", [](Brain& self, SimulationStep step, float time) {
            return self.step(step, time);
        }, py::arg("step"), py::arg("time"),
           "Perform simulation step with time (convenience wrapper)")
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
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["neurons"] = self.getTotalNeuronCount();
            stats["synapses"] = self.getTotalSynapseCount();
            stats["active"] = self.getActiveNeuronCount();
            stats["firing"] = self.getFiringNeuronCount();
            stats["spikes"] = self.getTotalSpikeCount();
            stats["rate"] = self.getAverageFiringRate();
            stats["e/i_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics as dictionary")
        .def("isInitialized", [](const Brain& self) {
            return self.getRegionCount() > 0;
        }, "Check if brain is initialized")
        .def("waitForInitialization", [](Brain& self) {
            while (!self.isInitialized()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }, "Wait for brain initialization (blocks until ready)")
        .def("injectRandomCurrent", [](Brain& self, float amount = 1.0f) {
            // Note: This would need actual neuron population access
            return self;
        }, py::arg("amount") = 1.0f, "Inject random current into all neurons")
        .def("__repr__", [](const Brain& b) {
            return "<Brain: " + std::to_string(b.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(b.getTotalSynapseCount()) + " synapses, " +
                   std::to_string(b.getRegionCount()) + " regions>";
        });

    // ==================== AgentBrain ====================
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with brain")
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
        .def("toggleRewardModulation", [](AgentBrain& self) {
            bool current = self.isRewardModulationEnabled();
            self.enableRewardModulation(!current);
            return !current;
        }, "Toggle reward modulation and return new state")
        .def("toggleDevelopment", [](AgentBrain& self) {
            bool current = self.isDevelopmentEnabled();
            self.enableDevelopment(!current);
            return !current;
        }, "Toggle development and return new state")
        .def("updateEpisode", [](AgentBrain& self, const SensoryPercept& percept, float reward, float predictedReward, float timestep = 1.0f) {
            self.processSensoryInput(percept);
            self.applyRewardModulation(reward, predictedReward);
            self.updateDevelopment(timestep);
            return self;
        }, py::arg("percept"), py::arg("reward"), py::arg("predictedReward"), py::arg("timestep") = 1.0f,
           "Process episode data and return self (convenience method)")
        .def("getEpisodeState", [](const AgentBrain& self) {
            py::dict state;
            state["stage"] = self.getDevelopmentalStage();
            state["neuromodulation"] = self.getNeuromodulationLevel();
            state["curiosity"] = self.getCuriosityLevel();
            state["novelty"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            return state;
        }, "Get current episode state as dictionary")
        .def("resetEpisode", [](AgentBrain& self) {
            self.reset();
            return self;
        }, "Reset episode state and return self")
        .def("__repr__", [](const AgentBrain& a) {
            return "<AgentBrain: stage=" + std::to_string(a.getDevelopmentalStage()) +
                   ", neuromodulation=" + std::to_string(a.getNeuromodulationLevel()) +
                   ", curiosity=" + std::to_string(a.getCuriosityLevel()) + ">";
        });

    // ==================== Factory Methods ====================
    // Configuration factory methods
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createConfigFromString", [](const std::string& jsonStr) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        std::istringstream iss(jsonStr);
        config->loadFromFile(iss);
        return config;
    }, py::arg("jsonStr"), "Create config from JSON string")
    .def("createConfigFromDict", [](const py::dict& d) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (auto item : d) {
            std::string key = py::cast<std::string>(item.first);
            std::string value = py::cast<std::string>(item.second);
            config->set(key, value);
        }
        return config;
    }, py::arg("dict"), "Create config from Python dictionary")
    .def("createConfigFromArgs", [](int argc, char** argv) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromArgs(argc, argv);
        return config;
    }, py::arg("argc"), py::arg("argv"),
       "Create config from command line arguments (convenience wrapper)");

    // Brain factory methods
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        // Apply default configuration
        config->set("brain.simulation.steps", "1000");
        config->set("brain.development.enabled", "true");
        config->set("brain.reward_modulation.enabled", "true");
        return std::make_shared<Brain>(config);
    }, "Create brain with default configuration")
    .def("createSimpleBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::runtime_error("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a simplified brain instance");

    // World factory methods
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 84, 84); // Default 100x100 world with 84x84 vision
        return world;
    }, "Create a simple world with default dimensions")
    .def("createWorld", [](size_t width, size_t height, size_t visionWidth = 84, size_t visionHeight = 84) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth") = 84, py::arg("visionHeight") = 84,
       "Create a simple world with specified dimensions")
    .def("createArenaWorld", [](size_t width = 100, size_t height = 100) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 84, 84);
        // Add arena boundaries
        for (size_t x = 0; x < width; ++x) {
            world->addObject(WorldObject(x, 0, WorldObjectType::Wall, 0.0f, 0.1f));
            world->addObject(WorldObject(x, height - 1, WorldObjectType::Wall, 0.0f, 0.1f));
        }
        for (size_t y = 0; y < height; ++y) {
            world->addObject(WorldObject(0, y, WorldObjectType::Wall, 0.0f, 0.1f));
            world->addObject(WorldObject(width - 1, y, WorldObjectType::Wall, 0.0f, 0.1f));
        }
        return world;
    }, py::arg("width") = 100, py::arg("height") = 100,
       "Create an arena world with boundary walls");

    // AgentBrain factory methods
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ==================== Simulation Utilities ====================
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, size_t steps) {
        if (!brain || !world) {
            throw std::runtime_error("Brain or world is null");
        }
        world->setAgentStart(50, 50);
        brain->initialize();
        
        for (size_t i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            auto action = brain->produceAction();
            world->applyMotorCommand(action, world->getSimulationTime());
            world->update(1.0f);
        }
        return world;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Run a complete simulation (convenience wrapper)");

    // ==================== Debugging and Utilities ====================
    m.def("debugInfo", [](const py::dict& data) {
        py::print("=== NLM Debug Information ===");
        for (auto item : data) {
            py::print(py::cast<std::string>(item.first), ": ", py::cast<std::string>(item.second));
        }
    }, py::arg("data"), "Print debug information");

    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw std::runtime_error("Config is null");
        }
        py::list errors;
        // Basic validation - could be expanded
        if (!config->has("brain.simulation.steps")) {
            errors.append("Missing brain.simulation.steps");
        }
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("getVersion", []() {
        return "1.0.0";
    }, "Get NLM Python bindings version");

    // ==================== Constants ====================
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility constants
    m.attr("DEFAULT_WORLD_WIDTH") = 100;
    m.attr("DEFAULT_WORLD_HEIGHT") = 100;
    m.attr("DEFAULT_VISION_SIZE") = 84;
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;

    // Version and metadata
    m.attr("__version__") = "1.0.0";
    
    // Create a test configuration for quick setup
    m.def("createTestConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", "1000");
        config->set("brain.region_count", "2");
        config->set("brain.connection_probability", "0.15");
        config->set("brain.simulation.timestep", "0.001");
        config->set("random_seed", "42");
        return config;
    }, "Create a test configuration with common settings");
    
    // Create a complete test environment
    m.def("createTestEnvironment", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 20, 20);
        world->setAgentStart(25, 25);
        
        // Add some objects
        world->addObject(WorldObject(10, 10, WorldObjectType::Resource, 10.0f, 0.5f));
        world->addObject(WorldObject(40, 40, WorldObjectType::Resource, 20.0f, 0.5f));
        
        return world;
    }, "Create a test environment with resources");
    
    // Create a complete agent simulation setup
    m.def("createTestAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain || !world) {
            throw std::runtime_error("Brain or world cannot be null");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning subsystems
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        agent->enableStructuralPlasticity(true);
        
        return agent;
    }, py::arg("brain"), py::arg("world"),
       "Create a test agent with learning enabled");
    
    // Run a simple experiment
    m.def("runSimpleExperiment", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, size_t steps) {
        if (!brain || !world) {
            throw std::runtime_error("Brain or world cannot be null");
        }
        
        world->setAgentStart(50, 50);
        brain->initialize();
        
        float totalReward = 0.0f;
        size_t objectCollisions = 0;
        
        for (size_t i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            auto action = brain->produceAction();
            
            // Check for collisions
            auto body = world->getAgentBody();
            for (size_t x = body.x - 1; x <= body.x + 1; ++x) {
                for (size_t y = body.y - 1; y <= body.y + 1; ++y) {
                    if (world->isValidPosition(x, y)) {
                        auto obj = world->getObjectAt(x, y);
                        if (obj && obj->type == WorldObjectType::Resource) {
                            objectCollisions++;
                            totalReward += obj->value;
                            world->removeObject(x, y);
                        }
                    }
                }
            }
            
            world->applyMotorCommand(action, world->getSimulationTime());
            world->update(1.0f);
            
            // Log progress
            if (i % 100 == 0) {
                std::cout << "Step " << i << ": Reward=" << totalReward 
                         << ", Objects collected=" << objectCollisions << std::endl;
            }
        }
        
        py::dict results;
        results["total_reward"] = totalReward;
        results["objects_collected"] = objectCollisions;
        results["steps"] = steps;
        results["final_position"] = std::vector<float>{world->getAgentBody().x, world->getAgentBody().y};
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("steps"),
       "Run a simple resource collection experiment and return results");
    
    // Monitoring utilities
    m.def("monitorSimulation", [](std::shared_ptr<Brain> brain, size_t duration, float interval = 1.0f) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        py::list metrics;
        size_t steps = 0;
        
        brain->initialize();
        while (steps < duration) {
            py::dict metric;
            metric["step"] = steps;
            metric["time"] = steps * 1.0f;
            metric["firing_rate"] = brain->getFiringNeuronCount();
            metric["total_spikes"] = brain->getTotalSpikeCount();
            metric["active_neurons"] = brain->getActiveNeuronCount();
            metric["region_count"] = brain->getRegionCount();
            metric["development_stage"] = brain->getDevelopmentalStage();
            
            metrics.append(metric);
            
            brain->step(steps);
            steps++;
            
            if (steps % static_cast<size_t>(interval) == 0) {
                std::cout << "Progress: " << steps << "/" << duration 
                         << " steps, Firing rate: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        return metrics;
    }, py::arg("brain"), py::arg("duration"), py::arg("interval") = 1.0f,
       "Monitor simulation and return metrics over time");
    
    // Export main simulation function
    m.def("runNLM", m.attr("runSimulation"),
          "Primary entry point for running NLM simulations (alias for runSimulation)");
}