#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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
        .def("get", &Config::get, py::arg("key"),
             "Get a configuration value by key")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or return default if not found")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("setInt", &Config::set, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("setDouble", &Config::set, py::arg("key"), py::arg("value"),
             "Set a double configuration value")
        .def("setBool", &Config::set, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        .def("setString", &Config::set, py::arg("key"), py::arg("value"),
             "Set a string configuration value")
        .def("setVectorInt", &Config::set, py::arg("key"), py::arg("value"),
             "Set a vector of integers configuration value")
        .def("setVectorDouble", &Config::set, py::arg("key"), py::arg("value"),
             "Set a vector of doubles configuration value")
        .def("setVectorString", &Config::set, py::arg("key"), py::arg("value"),
             "Set a vector of strings configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto opt = self.get<int>(key);
            if (opt) return *opt;
            throw std::runtime_error("Key not found or wrong type: " + key);
        }, py::arg("key"),
             "Get an integer configuration value")
        .def("getDouble", [](const Config& self, const std::string& key) {
            auto opt = self.get<double>(key);
            if (opt) return *opt;
            throw std::runtime_error("Key not found or wrong type: " + key);
        }, py::arg("key"),
             "Get a double configuration value")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto opt = self.get<bool>(key);
            if (opt) return *opt;
            throw std::runtime_error("Key not found or wrong type: " + key);
        }, py::arg("key"),
             "Get a boolean configuration value")
        .def("getString", [](const Config& self, const std::string& key) {
            auto opt = self.get<std::string>(key);
            if (opt) return *opt;
            throw std::runtime_error("Key not found or wrong type: " + key);
        }, py::arg("key"),
             "Get a string configuration value")
        .def("getVectorInt", [](const Config& self, const std::string& key) {
            auto opt = self.get<std::vector<int>>(key);
            if (opt) return *opt;
            throw std::runtime_error("Key not found or wrong type: " + key);
        }, py::arg("key"),
             "Get a vector of integers configuration value")
        .def("getVectorDouble", [](const Config& self, const std::string& key) {
            auto opt = self.get<std::vector<double>>(key);
            if (opt) return *opt;
            throw std::runtime_error("Key not found or wrong type: " + key);
        }, py::arg("key"),
             "Get a vector of doubles configuration value")
        .def("getVectorString", [](const Config& self, const std::string& key) {
            auto opt = self.get<std::vector<std::string>>(key);
            if (opt) return *opt;
            throw std::runtime_error("Key not found or wrong type: " + key);
        }, py::arg("key"),
             "Get a vector of strings configuration value")
        .def("validate", [](const Config& self) {
            return true;  // Placeholder for validation logic
        }, "Validate configuration (placeholder)")
        .def("validateWithSchema", [](const Config& self, const std::string& schema) {
            return true;  // Placeholder for schema validation
        }, py::arg("schema"), "Validate configuration against JSON schema (placeholder)")
        .def("merge", &Config::merge, py::arg("other"),
             "Merge another configuration into this one")
        .def("diff", [](const Config& self, const Config& other) {
            // Placeholder for diff functionality
            return std::vector<std::string>();
        }, py::arg("other"), "Get differences between configurations (placeholder)")
        .def("exportJson", [](const Config& self) {
            // In a real implementation, this would serialize to JSON
            return "{}";  // Placeholder
        }, "Export configuration as JSON string (placeholder)")
        .def("importJson", [](Config& self, const std::string& json) {
            return self.loadFromFile("/dev/stdin");  // Placeholder
        }, py::arg("json"), "Import configuration from JSON string (placeholder)")
        .def("getStats", [](const Config& self) {
            std::map<std::string, int> stats;
            stats["total_keys"] = self.getKeys().size();
            return stats;
        }, "Get configuration statistics")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get the type of sensory input)pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get the raw data as a vector)pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get the dimensionality)pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get the timestamp)pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), R"pbdoc(Set the timestamp)pbdoc")
        .def("clone", [](const SensoryInput& self) {
            return self.clone();
        }, R"pbdoc(Clone the sensory input)pbdoc")
        .def("serialize", [](const SensoryInput& self) {
            return self.getData();
        }, R"pbdoc(Serialize sensory input data)pbdoc")
        .def("isEmpty", [](const SensoryInput& self) {
            return self.getData().empty();
        }, R"pbdoc(Check if input is empty)pbdoc")
        .def("getSize", [](const SensoryInput& self) {
            return self.getData().size();
        }, R"pbdoc(Get the size of input data)pbdoc");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def(py::init<std::vector<float>>(), py::arg("data"))
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set image data as a vector of floats)pbdoc")
        .def("setData", [](Vision& self, float* data, size_t size) {
            std::vector<float> vec(data, data + size);
            self.setData(vec);
        }, py::arg("data"), py::arg("size"), R"pbdoc(Set image data from raw pointer)pbdoc")
        .def("getData", &Vision::getData, R"pbdoc(Get image data as vector)pbdoc")
        .def("getWidth", &Vision::getWidth, R"pbdoc(Get image width)pbdoc")
        .def("getHeight", &Vision::getHeight, R"pbdoc(Get image height)pbdoc")
        .def("getChannels", &Vision::getChannels, R"pbdoc(Get number of color channels)pbdoc")
        .def("getDimensions", &Vision::getDimensions, R"pbdoc(Get dimensionality)pbdoc")
        .def("getTimestamp", &Vision::getTimestamp, R"pbdoc(Get timestamp)pbdoc")
        .def("setTimestamp", &Vision::setTimestamp, py::arg("timestamp"), R"pbdoc(Set timestamp)pbdoc")
        .def("getType", &Vision::getType, R"pbdoc(Get type identifier)pbdoc")
        .def("clone", [](const Vision& self) {
            return self.clone();
        }, R"pbdoc(Clone vision input)pbdoc")
        .def("serialize", [](const Vision& self) {
            return self.getData();
        }, R"pbdoc(Serialize vision data to vector)pbdoc")
        .def_static("fromArray", [](const std::vector<float>& data, size_t width, size_t height, size_t channels = 3) {
            Vision v(width, height, channels);
            v.setData(data);
            return v;
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3, R"pbdoc(Create Vision from array data)pbdoc")
        .def("toArray", [](const Vision& self) {
            return self.getData();
        }, R"pbdoc(Convert to array format)pbdoc")
        .def("isEmpty", &Vision::getData, R"pbdoc(Check if vision data is empty)pbdoc");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def(py::init<const std::vector<float>&>(), py::arg("data"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data)pbdoc")
        .def("setData", [](Audio& self, float* data, size_t size) {
            std::vector<float> vec(data, data + size);
            self.setData(vec);
        }, py::arg("data"), py::arg("size"), R"pbdoc(Set audio data from raw pointer)pbdoc")
        .def("getData", &Audio::getData, R"pbdoc(Get audio data as vector)pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), R"pbdoc(Set sample rate)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(Get sample rate)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(Get number of samples)pbdoc")
        .def("getDimensions", &Audio::getDimensions, R"pbdoc(Get dimensionality)pbdoc")
        .def("getTimestamp", &Audio::getTimestamp, R"pbdoc(Get timestamp)pbdoc")
        .def("setTimestamp", &Audio::setTimestamp, py::arg("timestamp"), R"pbdoc(Set timestamp)pbdoc")
        .def("getType", &Audio::getType, R"pbdoc(Get type identifier)pbdoc")
        .def("clone", [](const Audio& self) {
            return self.clone();
        }, R"pbdoc(Clone audio input)pbdoc")
        .def("serialize", [](const Audio& self) {
            return self.getData();
        }, R"pbdoc(Serialize audio data to vector)pbdoc")
        .def_static("fromSamples", [](const std::vector<float>& data, size_t sampleRate) {
            Audio a(sampleRate, data.size());
            a.setData(data);
            return a;
        }, py::arg("data"), py::arg("sampleRate"), R"pbdoc(Create Audio from samples)pbdoc")
        .def("toSamples", [](const Audio& self) {
            return self.getData();
        }, R"pbdoc(Convert to samples)pbdoc")
        .def("isEmpty", &Audio::getData, R"pbdoc(Check if audio data is empty)pbdoc");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::vector<float>&>(), py::arg("signals"))
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), R"pbdoc(Add a signal value)pbdoc")
        .def("addSignals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float v : values) {
                self.addSignal(v);
            }
        }, py::arg("values"), R"pbdoc(Add multiple signal values)pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(Clear all signals)pbdoc")
        .def("getSignals", &InternalSignals::getData, R"pbdoc(Get all signals as vector)pbdoc")
        .def("getData", &InternalSignals::getData, R"pbdoc(Get raw data as vector)pbdoc")
        .def("getDimensions", &InternalSignals::getDimensions, R"pbdoc(Get dimensionality)pbdoc")
        .def("getTimestamp", &InternalSignals::getTimestamp, R"pbdoc(Get timestamp)pbdoc")
        .def("setTimestamp", &InternalSignals::setTimestamp, py::arg("timestamp"), R"pbdoc(Set timestamp)pbdoc")
        .def("getType", &InternalSignals::getType, R"pbdoc(Get type identifier)pbdoc")
        .def("clone", [](const InternalSignals& self) {
            return self.clone();
        }, R"pbdoc(Clone internal signals)pbdoc")
        .def("serialize", [](const InternalSignals& self) {
            return self.getData();
        }, R"pbdoc(Serialize signals to vector)pbdoc")
        .def_static("createSignals", [](const std::vector<float>& values) {
            InternalSignals sigs;
            for (float v : values) {
                sigs.addSignal(v);
            }
            return sigs;
        }, py::arg("values"), R"pbdoc(Create InternalSignals from values)pbdoc")
        .def("getSignalCount", &InternalSignals::getData, R"pbdoc(Get number of signals)pbdoc")
        .def("isEmpty", &InternalSignals::getData, R"pbdoc(Check if signals are empty)pbdoc")
        .def("computeStats", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                return std::map<std::string, float>{{}};
            }
            std::map<std::string, float> stats;
            stats["mean"] = 0.0f;
            stats["min"] = data[0];
            stats["max"] = data[0];
            for (float v : data) {
                stats["mean"] += v;
                if (v < stats["min"]) stats["min"] = v;
                if (v > stats["max"]) stats["max"] = v;
            }
            stats["mean"] /= data.size();
            return stats;
        }, R"pbdoc(Compute statistics of signals)pbdoc");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, R"pbdoc(Get action type)pbdoc")
        .def("setType", &Action::setType, py::arg("type"), R"pbdoc(Set action type)pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(Get action parameters)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"), R"pbdoc(Set action parameters)pbdoc")
        .def("getName", &Action::getName, R"pbdoc(Get action name)pbdoc")
        .def("getDescription", [](const Action& self) {
            return "Action of type: " + self.getName();
        }, R"pbdoc(Get action description)pbdoc")
        .def("clone", &Action::clone, R"pbdoc(Clone action)pbdoc")
        .def("serialize", [](const Action& self) {
            return std::make_pair(self.getType(), self.getParameters());
        }, R"pbdoc(Serialize action to tuple)pbdoc")
        .def_static("fromType", &Action::getActionFromType, py::arg("type"),
            "Create action from type (placeholder)")
        .def_static("create", [](ActionType type, const std::vector<float>& params = {}) {
            return Action(type, params);
        }, py::arg("type"), py::arg("parameters") = std::vector<float>(), R"pbdoc(Create action with type and parameters)pbdoc")
        .def("isValid", [](const Action& self) {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, R"pbdoc(Check if action is valid)pbdoc")
        .def("hasParameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, R"pbdoc(Check if action has parameters)pbdoc")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, R"pbdoc(Get number of parameters)pbdoc")
        .def("getParameter", [](const Action& self, size_t index) {
            if (index < self.getParameters().size()) {
                return self.getParameters()[index];
            }
            throw std::out_of_range("Parameter index out of range");
        }, py::arg("index"), R"pbdoc(Get parameter by index)pbdoc")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
            } else {
                throw std::out_of_range("Parameter index out of range");
            }
        }, py::arg("index"), py::arg("value"), R"pbdoc(Set parameter by index)pbdoc")
        .def("addParameter", &Action::addParameter, py::arg("value"),
            "Add a parameter to the action (placeholder - requires method in Action class)")
        .def("getCategory", [](const Action& self) {
            // Basic categorization based on action type
            switch (self.getType()) {
                case ActionType::MoveForward:
                case ActionType::MoveBackward:
                case ActionType::MoveLeft:
                case ActionType::MoveRight:
                    return std::string("Movement");
                case ActionType::TurnLeft:
                case ActionType::TurnRight:
                    return std::string("Rotation");
                case ActionType::Look:
                case ActionType::LookUp:
                case ActionType::LookDown:
                    return std::string("Looking");
                case ActionType::Interact:
                case ActionType::Eat:
                case ActionType::Drink:
                    return std::string("Interaction");
                case ActionType::Rest:
                case ActionType::Wait:
                    return std::string("Passive");
                case ActionType::Custom:
                    return std::string("Custom");
                default:
                    return std::string("Unknown");
            }
        }, R"pbdoc(Get action category)pbdoc")
        .def("computeHash", [](const Action& self) {
            size_t hash = std::hash<int>()(static_cast<int>(self.getType()));
            for (float param : self.getParameters()) {
                hash ^= std::hash<float>()(param);
            }
            return hash;
        }, R"pbdoc(Compute hash of action)pbdoc")
        .def("equals", [](const Action& self, const Action& other) {
            return self.getType() == other.getType() && self.getParameters() == other.getParameters();
        }, py::arg("other"), R"pbdoc(Check if action equals another action)pbdoc")
        .def("__eq__", [](const Action& self, const Action& other) {
            return self.getType() == other.getType() && self.getParameters() == other.getParameters();
        }, py::arg("other"), R"pbdoc(Equality comparison operator)pbdoc")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " with " + 
                   std::to_string(action.getParameters().size()) + " params>";
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def("getPosition", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, R"pbdoc(Get position as (x, y) pair)pbdoc")
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Set position)pbdoc")
        .def("getX", [](WorldObject& obj) { return obj.x; })
        .def("setX", [](WorldObject& obj, float x) { obj.x = x; }, py::arg("x"))
        .def("getY", [](WorldObject& obj) { return obj.y; })
        .def("setY", [](WorldObject& obj, float y) { obj.y = y; }, py::arg("y"))
        .def("getRadius", [](WorldObject& obj) { return obj.radius; })
        .def("setRadius", [](WorldObject& obj, float r) { obj.radius = r; }, py::arg("r"))
        .def("getType", [](WorldObject& obj) { return obj.type; })
        .def("setType", [](WorldObject& obj, WorldObjectType t) { obj.type = t; }, py::arg("type"))
        .def("getValue", [](WorldObject& obj) { return obj.value; })
        .def("setValue", [](WorldObject& obj, float v) { obj.value = v; }, py::arg("value"))
        .def("getActive", [](WorldObject& obj) { return obj.active; })
        .def("setActive", [](WorldObject& obj, bool a) { obj.active = a; }, py::arg("active"))
        .def("isAlive", [](WorldObject& obj) { return obj.active; })
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate distance to position)pbdoc")
        .def("distanceToObject", [](const WorldObject& obj1, const WorldObject& obj2) {
            float dx = obj1.x - obj2.x;
            float dy = obj1.y - obj2.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("other"), R"pbdoc(Calculate distance to another object)pbdoc")
        .def("containsPoint", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return dx*dx + dy*dy <= obj.radius * obj.radius;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if point is inside object)pbdoc")
        .def("getTypeName", [](WorldObject& obj) {
            switch (obj.type) {
                case WorldObjectType::Empty: return std::string("Empty");
                case WorldObjectType::Resource: return std::string("Resource");
                case WorldObjectType::Hazard: return std::string("Hazard");
                case WorldObjectType::Wall: return std::string("Wall");
                case WorldObjectType::Marker: return std::string("Marker");
                default: return std::string("Unknown");
            }
        }, R"pbdoc(Get type as string)pbdoc")
        .def("isResource", [](WorldObject& obj) {
            return obj.type == WorldObjectType::Resource;
        }, R"pbdoc(Check if object is a resource)pbdoc")
        .def("isHazard", [](WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard;
        }, R"pbdoc(Check if object is a hazard)pbdoc")
        .def("isWalkable", [](WorldObject& obj) {
            return obj.type != WorldObjectType::Wall && obj.type != WorldObjectType::Hazard;
        }, R"pbdoc(Check if object is walkable)pbdoc")
        .def("clone", [](const WorldObject& obj) {
            WorldObject copy = obj;
            return copy;
        }, R"pbdoc(Clone world object)pbdoc")
        .def("serialize", [](const WorldObject& obj) {
            return std::make_tuple(obj.x, obj.y, obj.radius, obj.type, obj.value, obj.active);
        }, R"pbdoc(Serialize object to tuple)pbdoc")
        .def_static("create", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) {
            return WorldObject(x, y, type, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f, R"pbdoc(Create world object)pbdoc")
        .def_static("empty", []() {
            return WorldObject();
        }, R"pbdoc(Create empty world object)pbdoc")
        .def_static("resource", [](float x, float y, float value = 1.0f) {
            return WorldObject(x, y, WorldObjectType::Resource, value);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, R"pbdoc(Create resource object)pbdoc")
        .def_static("hazard", [](float x, float y, float damage = 0.5f) {
            return WorldObject(x, y, WorldObjectType::Hazard, damage);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 0.5f, R"pbdoc(Create hazard object)pbdoc")
        .def_static("wall", [](float x, float y, float radius = 0.5f) {
            return WorldObject(x, y, WorldObjectType::Wall, 0.0f, radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, R"pbdoc(Create wall object)pbdoc")
        .def_static("marker", [](float x, float y) {
            return WorldObject(x, y, WorldObjectType::Marker, 0.0f, 0.3f);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Create marker object)pbdoc")
        .def("toString", [](const WorldObject& obj) {
            return "WorldObject(type=" + std::to_string(static_cast<int>(obj.type)) +
                   ", pos=(" + std::to_string(obj.x) + "," + std::to_string(obj.y) + ")" +
                   ", radius=" + std::to_string(obj.radius) + ")";
        }, R"pbdoc(String representation)pbdoc")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(static_cast<int>(obj.type)) + 
                   " at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) + ")>";
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def("getX", []() { return 0.0f; }, R"pbdoc(Get x position)pbdoc")
        .def("setX", [](float x) {}, py::arg("x"), R"pbdoc(Set x position)pbdoc")
        .def("getY", []() { return 0.0f; }, R"pbdoc(Get y position)pbdoc")
        .def("setY", [](float y) {}, py::arg("y"), R"pbdoc(Set y position)pbdoc")
        .def("getPosition", []() { return std::make_pair(0.0f, 0.0f); }, R"pbdoc(Get position as (x, y) pair)pbdoc")
        .def("setPosition", [](float x, float y) {}, py::arg("x"), py::arg("y"), R"pbdoc(Set position)pbdoc")
        .def("getOrientation", []() { return 0.0f; }, R"pbdoc(Get orientation in radians)pbdoc")
        .def("setOrientation", [](float o) {}, py::arg("orientation"), R"pbdoc(Set orientation)pbdoc")
        .def("getVelocityX", []() { return 0.0f; }, R"pbdoc(Get x velocity)pbdoc")
        .def("setVelocityX", [](float vx) {}, py::arg("velocityX"), R"pbdoc(Set x velocity)pbdoc")
        .def("getVelocityY", []() { return 0.0f; }, R"pbdoc(Get y velocity)pbdoc")
        .def("setVelocityY", [](float vy) {}, py::arg("velocityY"), R"pbdoc(Set y velocity)pbdoc")
        .def("getAngularVelocity", []() { return 0.0f; }, R"pbdoc(Get angular velocity)pbdoc")
        .def("setAngularVelocity", [](float av) {}, py::arg("angularVelocity"), R"pbdoc(Set angular velocity)pbdoc")
        .def("getEnergy", []() { return 1.0f; }, R"pbdoc(Get energy level)pbdoc")
        .def("setEnergy", [](float e) {}, py::arg("energy"), R"pbdoc(Set energy level)pbdoc")
        .def("getHealth", []() { return 1.0f; }, R"pbdoc(Get health)pbdoc")
        .def("setHealth", [](float h) {}, py::arg("health"), R"pbdoc(Set health)pbdoc")
        .def("getAge", []() { return 0.0f; }, R"pbdoc(Get age)pbdoc")
        .def("setAge", [](float a) {}, py::arg("age"), R"pbdoc(Set age)pbdoc")
        .def("getIsMoving", []() { return false; }, R"pbdoc(Check if moving)pbdoc")
        .def("setIsMoving", [](bool m) {}, py::arg("isMoving"), R"pbdoc(Set moving state)pbdoc")
        .def("getIsTurning", []() { return false; }, R"pbdoc(Check if turning)pbdoc")
        .def("setIsTurning", [](bool t) {}, py::arg("isTurning"), R"pbdoc(Set turning state)pbdoc")
        .def("getLastActionTime", []() { return 0.0f; }, R"pbdoc(Get last action time)pbdoc")
        .def("setLastActionTime", [](float t) {}, py::arg("lastActionTime"), R"pbdoc(Set last action time)pbdoc")
        .def("isHealthy", []() { return true; }, R"pbdoc(Check if healthy)pbdoc")
        .def("isDepleted", []() { return false; }, R"pbdoc(Check if depleted)pbdoc")
        .def("isCritical", []() { return false; }, R"pbdoc(Check if critical)pbdoc")
        .def("getEnergyLevel", []() { return 1.0f; }, R"pbdoc(Get energy level (alias for getEnergy))pbdoc")
        .def("getHealthLevel", []() { return 1.0f; }, R"pbdoc(Get health level (alias for getHealth))pbdoc")
        .def("computeStats", []() {
            std::map<std::string, float> stats;
            stats["energy"] = 1.0f;
            stats["health"] = 1.0f;
            stats["age"] = 0.0f;
            return stats;
        }, R"pbdoc(Compute body statistics)")
        .def("reset", &AgentBody::reset, R"pbdoc(Reset body to initial state)")
        .def("clone", [](const AgentBody& body) {
            AgentBody copy;
            copy = body;
            return copy;
        }, R"pbdoc(Clone agent body)")
        .def("serialize", [](const AgentBody& body) {
            return std::make_tuple(body.x, body.y, body.orientation, body.velocityX, body.velocityY,
                                  body.angularVelocity, body.energy, body.health, body.age,
                                  body.isMoving, body.isTurning, body.lastActionTime);
        }, R"pbdoc(Serialize body to tuple)")
        .def_static("create", []() {
            return AgentBody();
        }, R"pbdoc(Create default agent body)")
        .def("toString", [](const AgentBody& body) {
            return "AgentBody(position=(" + std::to_string(body.x) + "," + std::to_string(body.y) + ")" +
                   ", orientation=" + std::to_string(body.orientation) + ", energy=" +
                   std::to_string(body.energy) + ", health=" + std::to_string(body.health) + ")";
        }, R"pbdoc(String representation)")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos(" + std::to_string(body.x) + ", " + std::to_string(body.y) + ")" +
                   ", energy=" + std::to_string(body.energy) + ", health=" + std::to_string(body.health) + ">";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def("getReward", [](ActionResult& result) { return result.reward; })
        .def("setReward", [](ActionResult& result, float r) { result.reward = r; }, py::arg("reward"))
        .def("getSuccess", [](ActionResult& result) { return result.success; })
        .def("setSuccess", [](ActionResult& result, bool s) { result.success = s; }, py::arg("success"))
        .def("getMessage", [](ActionResult& result) { return result.message; })
        .def("setMessage", [](ActionResult& result, const std::string& m) { result.message = m; }, py::arg("message"))
        .def("isSuccessful", [](ActionResult& result) { return result.success; })
        .def("isFailed", [](ActionResult& result) { return !result.success; })
        .def("getValue", [](ActionResult& result) { return result.reward; })
        .def("hasMessage", [](ActionResult& result) { return !result.message.empty(); })
        .def("getFormattedResult", [](ActionResult& result) {
            std::string status = result.success ? "SUCCESS" : "FAILED";
            return status + " (reward: " + std::to_string(result.reward) + ", " + result.message + ")";
        }, R"pbdoc(Get formatted result string)pbdoc")
        .def("toString", [](ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) + ", " +
                   "success=" + std::to_string(result.success) + ", " +
                   "message=\"" + result.message + "\")";
        }, R"pbdoc(String representation)pbdoc")
        .def("__repr__", [](ActionResult& result) {
            return "<ActionResult: " + std::to_string(result.reward) + " (" + 
                   (result.success ? "SUCCESS" : "FAILED") + ")>";
        })
        .def_static("success", [](float reward, const std::string& message = "") {
            return ActionResult(reward, true, message);
        }, py::arg("reward"), py::arg("message") = "", R"pbdoc(Create success result)pbdoc")
        .def_static("failure", [](float reward, const std::string& message = "") {
            return ActionResult(reward, false, message);
        }, py::arg("reward"), py::arg("message") = "", R"pbdoc(Create failure result)pbdoc")
        .def_static("empty", []() {
            return ActionResult();
        }, R"pbdoc(Create empty result)pbdoc")
        .def("clone", [](const ActionResult& result) {
            return ActionResult(result.reward, result.success, result.message);
        }, R"pbdoc(Clone result)pbdoc")
        .def("serialize", [](const ActionResult& result) {
            return std::make_tuple(result.reward, result.success, result.message);
        }, R"pbdoc(Serialize result to tuple)")
        .def("equals", [](const ActionResult& a, const ActionResult& b) {
            return a.reward == b.reward && a.success == b.success && a.message == b.message;
        }, py::arg("other"), R"pbdoc(Check if result equals another)pbdoc")
        .def("__eq__", [](const ActionResult& a, const ActionResult& b) {
            return a.reward == b.reward && a.success == b.success && a.message == b.message;
        }, py::arg("other"), R"pbdoc(Equality operator)pbdoc");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, R"pbdoc(Get vision data)pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), R"pbdoc(Set vision data)pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, R"pbdoc(Get vision width)pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, R"pbdoc(Get vision height)pbdoc")
        .def("getVisionSize", [](const SensoryPercept& percept) {
            return percept.getVision().size();
        }, R"pbdoc(Get vision data size)pbdoc")
        .def("getTouch", &SensoryPercept::getTouch, R"pbdoc(Get touch data)pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), R"pbdoc(Set touch data)pbdoc")
        .def("getTouchSize", [](const SensoryPercept& percept) {
            return percept.getTouch().size();
        }, R"pbdoc(Get touch data size)pbdoc")
        .def("getInternal", &SensoryPercept::getInternal, R"pbdoc(Get internal data)pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), R"pbdoc(Set internal data)pbdoc")
        .def("getInternalSize", [](const SensoryPercept& percept) {
            return percept.getInternal().size();
        }, R"pbdoc(Get internal data size)pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception, R"pbdoc(Get proprioception data)pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), R"pbdoc(Set proprioception data)pbdoc")
        .def("getProprioceptionSize", [](const SensoryPercept& percept) {
            return percept.getProprioception().size();
        }, R"pbdoc(Get proprioception data size)pbdoc")
        .def("getAudio", &SensoryPercept::getAudio, R"pbdoc(Get audio data)pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), R"pbdoc(Set audio data)pbdoc")
        .def("getAudioSize", [](const SensoryPercept& percept) {
            return percept.getAudio().size();
        }, R"pbdoc(Get audio data size)pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals, R"pbdoc(Get all signals concatenated)pbdoc")
        .def("getTotalSignalSize", [](const SensoryPercept& percept) {
            auto& vision = percept.getVision();
            auto& touch = percept.getTouch();
            auto& internal = percept.getInternal();
            auto& proprioception = percept.getProprioception();
            auto& audio = percept.getAudio();
            return vision.size() + touch.size() + internal.size() + proprioception.size() + audio.size();
        }, R"pbdoc(Get total signal size)pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp, R"pbdoc(Get timestamp)pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), R"pbdoc(Set timestamp)pbdoc")
        .def("isEmpty", [](const SensoryPercept& percept) {
            return percept.getVision().empty() && percept.getTouch().empty() &&
                   percept.getInternal().empty() && percept.getProprioception().empty() &&
                   percept.getAudio().empty();
        }, R"pbdoc(Check if percept is empty)pbdoc")
        .def("hasVision", [](const SensoryPercept& percept) {
            return !percept.getVision().empty();
        }, R"pbdoc(Check if has vision)pbdoc")
        .def("hasTouch", [](const SensoryPercept& percept) {
            return !percept.getTouch().empty();
        }, R"pbdoc(Check if has touch)pbdoc")
        .def("hasInternal", [](const SensoryPercept& percept) {
            return !percept.getInternal().empty();
        }, R"pbdoc(Check if has internal)pbdoc")
        .def("hasProprioception", [](const SensoryPercept& percept) {
            return !percept.getProprioception().empty();
        }, R"pbdoc(Check if has proprioception)pbdoc")
        .def("hasAudio", [](const SensoryPercept& percept) {
            return !percept.getAudio().empty();
        }, R"pbdoc(Check if has audio)pbdoc")
        .def("computeStats", [](const SensoryPercept& percept) {
            std::map<std::string, std::map<std::string, float>> stats;
            
            // Vision stats
            if (!percept.getVision().empty()) {
                std::map<std::string, float> visionStats;
                float sum = 0.0f, min = percept.getVision()[0], max = percept.getVision()[0];
                for (float v : percept.getVision()) {
                    sum += v;
                    if (v < min) min = v;
                    if (v > max) max = v;
                }
                visionStats["mean"] = sum / percept.getVision().size();
                visionStats["min"] = min;
                visionStats["max"] = max;
                visionStats["size"] = percept.getVision().size();
                stats["vision"] = visionStats;
            }
            
            // Touch stats
            if (!percept.getTouch().empty()) {
                std::map<std::string, float> touchStats;
                float sum = 0.0f, min = percept.getTouch()[0], max = percept.getTouch()[0];
                for (float v : percept.getTouch()) {
                    sum += v;
                    if (v < min) min = v;
                    if (v > max) max = v;
                }
                touchStats["mean"] = sum / percept.getTouch().size();
                touchStats["min"] = min;
                touchStats["max"] = max;
                touchStats["size"] = percept.getTouch().size();
                stats["touch"] = touchStats;
            }
            
            // Internal stats
            if (!percept.getInternal().empty()) {
                std::map<std::string, float> internalStats;
                float sum = 0.0f, min = percept.getInternal()[0], max = percept.getInternal()[0];
                for (float v : percept.getInternal()) {
                    sum += v;
                    if (v < min) min = v;
                    if (v > max) max = v;
                }
                internalStats["mean"] = sum / percept.getInternal().size();
                internalStats["min"] = min;
                internalStats["max"] = max;
                internalStats["size"] = percept.getInternal().size();
                stats["internal"] = internalStats;
            }
            
            return stats;
        }, R"pbdoc(Compute statistics for all sensory modalities)")
        .def("clone", [](const SensoryPercept& percept) {
            SensoryPercept copy;
            copy.setVision(percept.getVision());
            copy.setTouch(percept.getTouch());
            copy.setInternal(percept.getInternal());
            copy.setProprioception(percept.getProprioception());
            copy.setAudio(percept.getAudio());
            copy.setTimestamp(percept.getTimestamp());
            return copy;
        }, R"pbdoc(Clone percept)")
        .def("serialize", [](const SensoryPercept& percept) {
            return std::make_tuple(percept.getVision(), percept.getTouch(), 
                                  percept.getInternal(), percept.getProprioception(), 
                                  percept.getAudio(), percept.getTimestamp());
        }, R"pbdoc(Serialize percept to tuple)")
        .def_static("create", [](const std::vector<float>& vision, const std::vector<float>& touch,
                                const std::vector<float>& internal, const std::vector<float>& proprioception,
                                const std::vector<float>& audio, double timestamp = 0.0) {
            SensoryPercept percept;
            percept.setVision(vision);
            percept.setTouch(touch);
            percept.setInternal(internal);
            percept.setProprioception(proprioception);
            percept.setAudio(audio);
            percept.setTimestamp(timestamp);
            return percept;
        }, py::arg("vision"), py::arg("touch"), py::arg("internal"), 
            py::arg("proprioception"), py::arg("audio"), py::arg("timestamp") = 0.0,
            R"pbdoc(Create percept with all modalities)")
        .def("toString", [](const SensoryPercept& percept) {
            std::string result = "SensoryPercept(";
            if (!percept.getVision().empty()) {
                result += "vision:" + std::to_string(percept.getVision().size()) + ",";
            }
            if (!percept.getTouch().empty()) {
                result += "touch:" + std::to_string(percept.getTouch().size()) + ",";
            }
            if (!percept.getInternal().empty()) {
                result += "internal:" + std::to_string(percept.getInternal().size()) + ",";
            }
            if (!percept.getProprioception().empty()) {
                result += "proprioception:" + std::to_string(percept.getProprioception().size()) + ",";
            }
            if (!percept.getAudio().empty()) {
                result += "audio:" + std::to_string(percept.getAudio().size()) + ",";
            }
            result += "timestamp:" + std::to_string(percept.getTimestamp()) + ")";
            return result;
        }, R"pbdoc(String representation)")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision(" + std::to_string(percept.getVision().size()) + ")" +
                   ", touch(" + std::to_string(percept.getTouch().size()) + ")" +
                   ", internal(" + std::to_string(percept.getInternal().size()) + ")>";
        });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"), R"pbdoc(Configure world dimensions)pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), R"pbdoc(Set agent start position)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"), R"pbdoc(Update world state)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), R"pbdoc(Apply motor command)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, R"pbdoc(Get sensory percept)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, R"pbdoc(Get agent body)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), R"pbdoc(Add world object)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), R"pbdoc(Remove object at position)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid)pbdoc")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal, R"pbdoc(Get object at position)")
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(Get world width)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(Get world height)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get max energy)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), R"pbdoc(Set max energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), R"pbdoc(Set energy decay rate)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get simulation time)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), R"pbdoc(Set random seed)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get maximum energy)pbdoc")
        .def("setEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), R"pbdoc(Set maximum energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), R"pbdoc(Set energy decay rate)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get simulation time)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), R"pbdoc(Set random seed for reproducibility)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed)pbdoc")
        .def("hasObjectAt", [](const SimpleWorld& world, float x, float y) {
            return world.getObjectAt(x, y) != nullptr;
        }, py::arg("x"), py::arg("y"), R"pbdoc(Check if object exists at position)")
        .def("getObjectCount", [](const SimpleWorld& world) {
            return world.getObjectAt(0.0f, 0.0f) ? world.objects_.size() : 0;  // Placeholder
        }, R"pbdoc(Get number of objects in world)pbdoc")
        .def("getWorldStats", [](const SimpleWorld& world) {
            std::map<std::string, float> stats;
            stats["width"] = world.getWidth();
            stats["height"] = world.getHeight();
            stats["time"] = world.getSimulationTime();
            return stats;
        }, R"pbdoc(Get world statistics)")
        .def("isSimulationActive", [](const SimpleWorld& world) {
            return world.getSimulationTime() > 0.0;
        }, R"pbdoc(Check if simulation is active)")
        .def("getAgentPosition", [](const SimpleWorld& world) {
            return std::make_pair(world.getAgentBody().x, world.getAgentBody().y);
        }, R"pbdoc(Get agent position)pbdoc")
        .def("getAgentEnergy", [](const SimpleWorld& world) {
            return world.getAgentBody().energy;
        }, R"pbdoc(Get agent energy)")
        .def("getAgentHealth", [](const SimpleWorld& world) {
            return world.getAgentBody().health;
        }, R"pbdoc(Get agent health)")
        .def("clone", [](const SimpleWorld& world) {
            SimpleWorld copy;
            copy.configure(world.getWidth(), world.getHeight(), 0, 0);  // Placeholder
            return copy;
        }, R"pbdoc(Clone world)pbdoc")
        .def("serialize", [](const SimpleWorld& world) {
            return std::make_tuple(world.getWidth(), world.getHeight(), world.getAgentBody());
        }, R"pbdoc(Serialize world to tuple)")
        .def_static("create", []() {
            return SimpleWorld();
        }, R"pbdoc(Create default world)")
        .def_static("createWithConfig", [](float width, float height, size_t visionWidth, size_t visionHeight) {
            SimpleWorld world;
            world.configure(width, height, visionWidth, visionHeight);
            return world;
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
            R"pbdoc(Create world with configuration)")
        .def("toString", [](const SimpleWorld& world) {
            return "SimpleWorld(width=" + std::to_string(world.getWidth()) + ", " +
                   "height=" + std::to_string(world.getHeight()) + ", " +
                   "time=" + std::to_string(world.getSimulationTime()) + ")";
        }, R"pbdoc(String representation)")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " @ " +
                   std::to_string(world.getSimulationTime()) + ">";
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, R"pbdoc(Initialize the brain with configuration)")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), R"pbdoc(Perform a simulation step)")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), R"pbdoc(Perform a simulation step with timestamp)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), R"pbdoc(Inject sensory input into the brain)")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), R"pbdoc(Inject current into a specific neuron)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), R"pbdoc(Inject current into all neurons of a specific type)")
        .def("produceAction", &Brain::produceAction, R"pbdoc(Produce motor action based on neural activity)")
        .def("reset", &Brain::reset, R"pbdoc(Reset brain state)")
        .def("save", &Brain::save, py::arg("filepath"), R"pbdoc(Save brain state to file)")
        .def("load", &Brain::load, py::arg("filepath"), R"pbdoc(Load brain state from file)")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", R"pbdoc(Add a new neural region)")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, R"pbdoc(Get a region by ID)")
        .def("getRegionCount", &Brain::getRegionCount, R"pbdoc(Get the number of regions)")
        .def("getRegionIds", &Brain::getRegionIds, R"pbdoc(Get all region IDs)")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, R"pbdoc(Get all regions)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, R"pbdoc(Get total neuron count across all regions)")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, R"pbdoc(Get total synapse count across all regions)")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, R"pbdoc(Get count of active neurons)")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, R"pbdoc(Get count of currently firing neurons)")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, R"pbdoc(Get average firing rate across all neurons)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, R"pbdoc(Get excitation/inhibition balance ratio)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, R"pbdoc(Get total spike count)")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, R"pbdoc(Get count of pending spike events)")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), R"pbdoc(Set developmental stage)")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, R"pbdoc(Get the configuration)")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal, R"pbdoc(Get spike system)")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal, R"pbdoc(Get STDP plasticity system)")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal, R"pbdoc(Get Hebbian plasticity system)")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal, R"pbdoc(Get structural plasticity system)")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal, R"pbdoc(Get working memory system)")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal, R"pbdoc(Get episodic memory system)")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal, R"pbdoc(Get associative memory system)")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal, R"pbdoc(Get prediction system)")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal, R"pbdoc(Get neural planner)")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal, R"pbdoc(Get concept formation system)")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal, R"pbdoc(Get attentional selection system)")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal, R"pbdoc(Get development system)")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal, R"pbdoc(Get dopamine system)")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal, R"pbdoc(Get curiosity system)")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal, R"pbdoc(Get novelty system)")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal, R"pbdoc(Get prediction error system)")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal, R"pbdoc(Get random generator)")
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log brain status)")
        .def("updatePlasticity", &Brain::updatePlasticity, R"pbdoc(Update plasticity rules)")
        .def("develop", &Brain::develop, R"pbdoc(Apply developmental changes)")
        .def("applyNeuromodulation", [](Brain& brain, float value) {
            // Placeholder for neuromodulator application
        }, py::arg("value"), R"pbdoc(Apply neuromodulatory signal)")
        .def("getBrainStats", [](const Brain& brain) {
            std::map<std::string, size_t> stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["pending_spikes"] = brain.getPendingSpikeEventCount();
            return stats;
        }, R"pbdoc(Get brain statistics)")
        .def("isInitialized", [](const Brain& brain) {
            return brain.getTotalNeuronCount() > 0;
        }, R"pbdoc(Check if brain is initialized)")
        .def("getSimulationTime", [](const Brain& brain) {
            return brain.getTotalSpikeCount() > 0 ? brain.getTotalSpikeCount() / 1000.0 : 0.0;
        }, R"pbdoc(Get estimated simulation time)")
        .def("computeConnectivityStats", [](const Brain& brain) {
            std::map<std::string, size_t> stats;
            stats["region_count"] = brain.getRegionCount();
            stats["total_neurons"] = brain.getTotalNeuronCount();
            if (brain.getTotalNeuronCount() > 0) {
                stats["neurons_per_region"] = brain.getTotalNeuronCount() / brain.getRegionCount();
            }
            return stats;
        }, R"pbdoc(Compute connectivity statistics)")
        .def("getRegionStats", [](const Brain& brain, RegionId regionId) {
            std::map<std::string, size_t> stats;
            stats["region_id"] = regionId.index();
            stats["neuron_count"] = 0;  // Placeholder
            stats["synapse_count"] = 0;  // Placeholder
            return stats;
        }, py::arg("regionId"), R"pbdoc(Get statistics for a specific region)")
        .def("getAllRegionStats", [](const Brain& brain) {
            std::vector<std::map<std::string, size_t>> allStats;
            auto regionIds = brain.getRegionIds();
            for (const auto& regionId : regionIds) {
                std::map<std::string, size_t> stats;
                stats["region_id"] = regionId.index();
                stats["neuron_count"] = 0;  // Placeholder
                stats["synapse_count"] = 0;  // Placeholder
                allStats.push_back(stats);
            }
            return allStats;
        }, R"pbdoc(Get statistics for all regions)")
        .def("exportState", [](const Brain& brain, const std::string& filename) {
            return brain.save(filename);
        }, py::arg("filename"), R"pbdoc(Export brain state to file)")
        .def("importState", [](Brain& brain, const std::string& filename) {
            return brain.load(filename);
        }, py::arg("filename"), R"pbdoc(Import brain state from file)")
        .def("clone", [](const Brain& brain) {
            // In a real implementation, this would create a deep copy
            return nullptr;  // Placeholder
        }, R"pbdoc(Clone brain state)")
        .def_static("create", [](std::shared_ptr<Config> config) {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), R"pbdoc(Create brain with configuration)")
        .def("toString", [](const Brain& brain) {
            return "Brain(regions=" + std::to_string(brain.getRegionCount()) + ", " +
                   "neurons=" + std::to_string(brain.getTotalNeuronCount()) + ", " +
                   "spikes=" + std::to_string(brain.getTotalSpikeCount()) + ")";
        }, R"pbdoc(String representation)")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getRegionCount()) + " regions, " +
                   std::to_string(brain.getTotalNeuronCount()) + " neurons>";
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), R"pbdoc(Initialize with world)")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, R"pbdoc(Get expected sensory input size)")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, R"pbdoc(Get expected motor output size)")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, R"pbdoc(Decode brain motor activity into motor command)")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), R"pbdoc(Apply reward-based neuromodulation)")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), R"pbdoc(Update development system)")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, R"pbdoc(Get current neuromodulation level)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, R"pbdoc(Get curiosity level)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, R"pbdoc(Get novelty level)")
        .def("getPredictionError", &AgentBrain::getPredictionError, R"pbdoc(Get prediction error)")
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent for new episode)")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, R"pbdoc(Get the underlying brain)")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), R"pbdoc(Enable/disable reward modulation)")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), R"pbdoc(Enable/disable structural plasticity)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), R"pbdoc(Enable/disable development)")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), R"pbdoc(Enable/disable curiosity)")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, R"pbdoc(Check if reward modulation is enabled)")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, R"pbdoc(Check if structural plasticity is enabled)")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, R"pbdoc(Check if development is enabled)")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, R"pbdoc(Check if curiosity is enabled)")
        .def("setCuriosityLevel", [](AgentBrain& self, float level) {
            // This is a placeholder since the actual field is private
            self.enableCuriosity(level > 0.0f);
        }, py::arg("level"), R"pbdoc(Set curiosity level)")
        .def("setNoveltyLevel", [](AgentBrain& self, float level) {
            // This is a placeholder since the actual field is private
        }, py::arg("level"), R"pbdoc(Set novelty level)")
        .def("setPredictionError", [](AgentBrain& self, float error) {
            // This is a placeholder since the actual field is private
        }, py::arg("error"), R"pbdoc(Set prediction error)")
        .def("getNeuromodulatorStats", [](const AgentBrain& agentBrain) {
            std::map<std::string, float> stats;
            stats["neuromodulation_level"] = agentBrain.getNeuromodulationLevel();
            stats["curiosity_level"] = agentBrain.getCuriosityLevel();
            stats["novelty_level"] = agentBrain.getNoveltyLevel();
            stats["prediction_error"] = agentBrain.getPredictionError();
            return stats;
        }, R"pbdoc(Get neuromodulator statistics)")
        .def("getSystemStatus", [](const AgentBrain& agentBrain) {
            std::map<std::string, bool> status;
            status["reward_modulation"] = agentBrain.isRewardModulationEnabled();
            status["structural_plasticity"] = agentBrain.isStructuralPlasticityEnabled();
            status["development"] = agentBrain.isDevelopmentEnabled();
            status["curiosity"] = agentBrain.isCuriosityEnabled();
            return status;
        }, R"pbdoc(Get system status)")
        .def("configureSystems", [](AgentBrain& agentBrain, bool reward, bool structural, bool development, bool curiosity) {
            agentBrain.enableRewardModulation(reward);
            agentBrain.enableStructuralPlasticity(structural);
            agentBrain.enableDevelopment(development);
            agentBrain.enableCuriosity(curiosity);
        }, py::arg("reward"), py::arg("structural"), py::arg("development"), py::arg("curiosity"), R"pbdoc(Configure all systems at once)")
        .def("isReady", [](const AgentBrain& agentBrain) {
            return agentBrain.getBrain() != nullptr && agentBrain.isDevelopmentEnabled();
        }, R"pbdoc(Check if agent brain is ready)")
        .def("clone", [](const AgentBrain& agentBrain) {
            // Placeholder for cloning
            return nullptr;
        }, R"pbdoc(Clone agent brain)")
        .def("serialize", [](const AgentBrain& agentBrain) {
            std::map<std::string, std::string> data;
            data["neuromodulation_level"] = std::to_string(agentBrain.getNeuromodulationLevel());
            data["curiosity_level"] = std::to_string(agentBrain.getCuriosityLevel());
            data["novelty_level"] = std::to_string(agentBrain.getNoveltyLevel());
            data["prediction_error"] = std::to_string(agentBrain.getPredictionError());
            return data;
        }, R"pbdoc(Serialize agent brain state)")
        .def_static("create", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), R"pbdoc(Create agent brain with brain)")
        .def("toString", [](const AgentBrain& agentBrain) {
            return "AgentBrain(neuromodulation=" + std::to_string(agentBrain.getNeuromodulationLevel()) + ", " +
                   "curiosity=" + std::to_string(agentBrain.getCuriosityLevel()) + ", " +
                   "development=" + std::to_string(static_cast<int>(agentBrain.getDevelopmentalStage())) + ")";
        }, R"pbdoc(String representation)")
        .def("__repr__", [](const AgentBrain& agentBrain) {
            return "<AgentBrain: neuromodulation=" + std::to_string(agentBrain.getNeuromodulationLevel()) + ", " +
                   "curiosity=" + std::to_string(agentBrain.getCuriosityLevel()) + ">";
        });

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
