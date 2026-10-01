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
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                bool success = self.loadFromFile(filepath);
                if (!success) {
                    throw std::runtime_error("Failed to load configuration from file: " + filepath);
                }
                return success;
            } catch (const std::exception& e) {
                throw py::value_error("Configuration file error: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Load configuration with proper error handling")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                bool success = self.loadFromArgs(argc, argv);
                if (!success) {
                    throw std::runtime_error("Failed to load configuration from command line arguments");
                }
                return success;
            } catch (const std::exception& e) {
                throw py::value_error("Configuration argument error: " + std::string(e.what()));
            }
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments with error handling")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                bool success = self.saveToFile(filepath);
                if (!success) {
                    throw std::runtime_error("Failed to save configuration to file: " + filepath);
                }
                return success;
            } catch (const std::exception& e) {
                throw py::value_error("Configuration save error: " + std::string(e.what()));
            }
        }, py::arg("filepath"), "Save configuration to file with error handling")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("get", [](Config& self, const std::string& key) {
            try {
                auto value = self.get<std::string>(key);
                return py::cast(value);
            } catch (...) {
                throw py::value_error("Key '" + key + "' not found in configuration");
            }
        }, py::arg("key"), "Get configuration value with error handling")
        .def("getInt", [](Config& self, const std::string& key, int default_val = 0) {
            try {
                auto value = self.get<int>(key);
                return value.value_or(default_val);
            } catch (...) {
                throw py::value_error("Key '" + key + "' not found or cannot be converted to int");
            }
        }, py::arg("key"), py::arg("default_val") = 0,
           "Get integer configuration value with default")
        .def("getFloat", [](Config& self, const std::string& key, float default_val = 0.0f) {
            try {
                auto value = self.get<float>(key);
                return value.value_or(default_val);
            } catch (...) {
                throw py::value_error("Key '" + key + "' not found or cannot be converted to float");
            }
        }, py::arg("key"), py::arg("default_val") = 0.0f,
           "Get float configuration value with default")
        .def("getBool", [](Config& self, const std::string& key, bool default_val = false) {
            try {
                auto value = self.get<bool>(key);
                return value.value_or(default_val);
            } catch (...) {
                throw py::value_error("Key '" + key + "' not found or cannot be converted to bool");
            }
        }, py::arg("key"), py::arg("default_val") = false,
           "Get boolean configuration value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value with validation")
        .def("validateKey", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::value_error("Invalid configuration key: '" + key + "'");
            }
            return true;
        }, py::arg("key"), "Validate configuration key exists")
        .def("validateValue", [](const Config& self, const std::string& key, const py::object& value) {
            if (!self.has(key)) {
                throw py::value_error("Cannot validate non-existent key: '" + key + "'");
            }
            // Type checking based on key
            if (key.find("neuron_count") != std::string::npos || 
                key.find("synapse_density") != std::string::npos ||
                key.find("v_thresh") != std::string::npos ||
                key.find("v_rest") != std::string::npos ||
                key.find("tau_mem") != std::string::npos ||
                key.find("tau_ref") != std::string::npos) {
                if (!value.is<float>() && !value.is<int>()) {
                    throw py::value_error("Value for key '" + key + "' must be numeric");
                }
            }
            return true;
        }, py::arg("key"), py::arg("value"), "Validate configuration value type")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("toDict", [](const SensoryInput& self) {
            py::dict result;
            result["type"] = self.getType();
            result["data"] = self.getData();
            result["dimensions"] = self.getDimensions();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert SensoryInput to Python dictionary")
        .def_static("fromDict", [](const py::dict& data) {
            SensoryInput input;
            if (data.contains("timestamp")) {
                input.setTimestamp(data["timestamp"].cast<Timestamp>());
            }
            return input;
        }, py::arg("data"), "Create SensoryInput from dictionary");

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
        .def("updateVision", [](Vision& self, const std::vector<float>& data,
                               size_t width, size_t height, size_t channels) {
            self.setData(data);
            return self;
        }, py::arg("data"), py::arg("width"), py::arg("height"),
           py::arg("channels") = 3, "Update vision data with chainable return")
        .def(py::init([](const py::dict& params) {
            Vision v;
            if (params.contains("width")) v.setWidth(params["width"].cast<size_t>());
            if (params.contains("height")) v.setHeight(params["height"].cast<size_t>());
            if (params.contains("channels")) v.setChannels(params["channels"].cast<size_t>());
            return v;
        }), py::arg("params") = py::dict(),
             "Create Vision from dictionary parameters")
        .def("getResolution", [](const Vision& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get vision resolution as (width, height) tuple")
        .def("setResolution", [](Vision& self, size_t width, size_t height) {
            self.setWidth(width);
            self.setHeight(height);
            return self;
        }, py::arg("width"), py::arg("height"), "Set vision resolution with chainable return");

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
        .def("updateVision", [](Vision& self, const std::vector<float>& data,
                               size_t width, size_t height, size_t channels) {
            self.setData(data);
            return self;
        }, py::arg("data"), py::arg("width"), py::arg("height"),
           py::arg("channels") = 3, "Update vision data with chainable return")
        .def(py::init([](const py::dict& params) {
            Vision v;
            if (params.contains("width")) v.setWidth(params["width"].cast<size_t>());
            if (params.contains("height")) v.setHeight(params["height"].cast<size_t>());
            if (params.contains("channels")) v.setChannels(params["channels"].cast<size_t>());
            return v;
        }), py::arg("params") = py::dict(),
             "Create Vision from dictionary parameters")
        .def("getResolution", [](const Vision& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get vision resolution as (width, height) tuple")
        .def("setResolution", [](Vision& self, size_t width, size_t height) {
            self.setWidth(width);
            self.setHeight(height);
            return self;
        }, py::arg("width"), py::arg("height"), "Set vision resolution with chainable return");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("updateAudio", [](Audio& self, const std::vector<float>& data,
                              size_t sampleRate, size_t numSamples) {
            self.setData(data);
            self.setSampleRate(sampleRate);
            return self;
        }, py::arg("data"), py::arg("sampleRate"), py::arg("numSamples"),
           "Update audio data with chainable return")
        .def(py::init([](const py::dict& params) {
            Audio a;
            if (params.contains("sampleRate")) a.setSampleRate(params["sampleRate"].cast<size_t>());
            if (params.contains("numSamples")) {
                a.setData(std::vector<float>(params["numSamples"].cast<size_t>()));
            }
            return a;
        }), py::arg("params") = py::dict(),
             "Create Audio from dictionary parameters")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("setDuration", [](Audio& self, double duration) {
            if (self.getSampleRate() > 0) {
                self.setNumSamples(static_cast<size_t>(duration * self.getSampleRate()));
            }
            return self;
        }, py::arg("duration"), "Set audio duration with chainable return");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::size,
             "Get number of signals")
        .def("getSignal", &InternalSignals::getSignalAt,
             py::arg("index"), "Get signal at index")
        .def("toList", [](const InternalSignals& self) {
            py::list result;
            for (size_t i = 0; i < self.size(); ++i) {
                result.append(self.getSignalAt(i));
            }
            return result;
        }, "Convert internal signals to Python list")
        .def("addSignals", [](InternalSignals& self, const py::list& values) {
            for (auto item : values) {
                self.addSignal(item.cast<float>());
            }
            return self;
        }, py::arg("values"), "Add multiple signals from Python list")
        .def("getAverage", [](const InternalSignals& self) {
            if (self.size() == 0) return 0.0f;
            float sum = 0.0f;
            for (size_t i = 0; i < self.size(); ++i) {
                sum += self.getSignalAt(i);
            }
            return sum / self.size();
        }, "Get average of all signals")
        .def("getMax", [](const InternalSignals& self) {
            if (self.size() == 0) return 0.0f;
            float max = self.getSignalAt(0);
            for (size_t i = 1; i < self.size(); ++i) {
                if (self.getSignalAt(i) > max) max = self.getSignalAt(i);
            }
            return max;
        }, "Get maximum signal value")
        .def("getMin", [](const InternalSignals& self) {
            if (self.size() == 0) return 0.0f;
            float min = self.getSignalAt(0);
            for (size_t i = 1; i < self.size(); ++i) {
                if (self.getSignalAt(i) < min) min = self.getSignalAt(i);
            }
            return min;
        }, "Get minimum signal value");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def(py::init([](ActionType type, const py::dict& params) {
            std::vector<float> param_vector;
            for (auto item : params) {
                param_vector.push_back(item.second.cast<float>());
            }
            return Action(type, param_vector);
        }), py::arg("type"), py::arg("params") = py::dict(),
             "Create Action with named parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("update", [](Action& self, ActionType type, const py::dict& params) {
            self.setType(type);
            std::vector<float> param_vector;
            for (auto item : params) {
                param_vector.push_back(item.second.cast<float>());
            }
            self.setParameters(param_vector);
            return self;
        }, py::arg("type"), py::arg("params") = py::dict(),
           "Update action with chainable return")
        .def("hasParameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameter", &Action::getParameters, py::arg("index"),
             "Get parameter at index (deprecated - use getParameters())")
        .def("addParameter", [](Action& self, float value) {
            std::vector<float> params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
            return self;
        }, py::arg("value"), "Add parameter with chainable return");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def(py::init([](float x, float y, const std::string& type_str, float value = 0.0f, float radius = 0.5f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            // Convert string type to enum
            if (type_str == "Resource") obj.type = WorldObjectType::Resource;
            else if (type_str == "Hazard") obj.type = WorldObjectType::Hazard;
            else if (type_str == "Wall") obj.type = WorldObjectType::Wall;
            else if (type_str == "Marker") obj.type = WorldObjectType::Marker;
            else obj.type = WorldObjectType::Empty;
            obj.value = value;
            obj.radius = radius;
            return obj;
        }), py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create WorldObject from parameters including type string")
        .def("toDict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["value"] = self.value;
            result["type"] = static_cast<int>(self.type);
            result["active"] = self.active;
            return result;
        }, "Convert WorldObject to Python dictionary")
        .def_static("fromDict", [](const py::dict& data) {
            WorldObject obj;
            if (data.contains("x")) obj.x = data["x"].cast<float>();
            if (data.contains("y")) obj.y = data["y"].cast<float>();
            if (data.contains("radius")) obj.radius = data["radius"].cast<float>();
            if (data.contains("value")) obj.value = data["value"].cast<float>();
            if (data.contains("type")) obj.type = static_cast<WorldObjectType>(data["type"].cast<int>());
            if (data.contains("active")) obj.active = data["active"].cast<bool>();
            return obj;
        }, py::arg("data"), "Create WorldObject from dictionary")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
            return self;
        }, py::arg("x"), py::arg("y"), "Set position with chainable return")
        .def("getDistance", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point")
        .def("isColliding", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            return dist < (self.radius + other.radius);
        }, py::arg("other"), "Check collision with other object");

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
        .def("updatePosition", [](AgentBody& self, float x, float y, float orientation) {
            self.x = x;
            self.y = y;
            self.orientation = orientation;
            return self;
        }, py::arg("x"), py::arg("y"), py::arg("orientation"),
           "Update position with chainable return")
        .def("getLinearSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get linear speed magnitude")
        .def("getAngularSpeed", [](const AgentBody& self) {
            return std::abs(self.angularVelocity);
        }, "Get angular speed magnitude")
        .def("updateVelocity", [](AgentBody& self, float vx, float vy, float angularVel) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.angularVelocity = angularVel;
            return self;
        }, py::arg("vx"), py::arg("vy"), py::arg("angularVel"),
           "Update velocity with chainable return");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert ActionResult to Python dictionary")
        .def_static("fromDict", [](const py::dict& data) {
            ActionResult result;
            if (data.contains("reward")) result.reward = data["reward"].cast<float>();
            if (data.contains("success")) result.success = data["success"].cast<bool>();
            if (data.contains("message")) result.message = data["message"].cast<std::string>();
            return result;
        }, py::arg("data"), "Create ActionResult from dictionary")
        .def("isPositive", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is positive")
        .def("isNegative", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result is negative")
        .def("getFormattedResult", [](const ActionResult& self) {
            std::stringstream ss;
            ss << "ActionResult(reward=" << self.reward 
               << ", success=" << (self.success ? "true" : "false")
               << ", message=\"" << self.message << "\")";
            return ss.str();
        }, "Get formatted string representation");

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
        .def("toDict", [](const SensoryPercept& self) {
            py::dict result;
            result["vision"] = self.getVision();
            result["touch"] = self.getTouch();
            result["internal"] = self.getInternal();
            result["audio"] = self.getAudio();
            result["proprioception"] = self.getProprioception();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert SensoryPercept to Python dictionary")
        .def_static("fromDict", [](const py::dict& data) {
            SensoryPercept percept;
            if (data.contains("vision")) percept.setVision(data["vision"].cast<Vision>());
            if (data.contains("touch")) percept.setTouch(data["touch"].cast<Touch>());
            if (data.contains("internal")) percept.setInternal(data["internal"].cast<InternalSignals>());
            if (data.contains("audio")) percept.setAudio(data["audio"].cast<Audio>());
            if (data.contains("proprioception")) percept.setProprioception(data["proprioception"].cast<Proprioception>());
            if (data.contains("timestamp")) percept.setTimestamp(data["timestamp"].cast<Timestamp>());
            return percept;
        }, py::arg("data"), "Create SensoryPercept from dictionary")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if vision data exists")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if audio data exists")
        .def("hasInternalSignals", [](const SensoryPercept& self) {
            return !self.getInternal().getSignalCount();
        }, "Check if internal signals exist")
        .def("updateFromDict", [](SensoryPercept& self, const py::dict& data) {
            if (data.contains("vision")) self.setVision(data["vision"].cast<Vision>());
            if (data.contains("touch")) self.setTouch(data["touch"].cast<Touch>());
            if (data.contains("internal")) self.setInternal(data["internal"].cast<InternalSignals>());
            if (data.contains("audio")) self.setAudio(data["audio"].cast<Audio>());
            if (data.contains("proprioception")) self.setProprioception(data["proprioception"].cast<Proprioception>());
            if (data.contains("timestamp")) self.setTimestamp(data["timestamp"].cast<Timestamp>());
            return self;
        }, py::arg("data"), "Update SensoryPercept from dictionary with chainable return");

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
             py::return_value_policy::copy)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy)
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getWorldInfo", [](SimpleWorld& self) {
            py::dict info;
            info["width"] = self.getWidth();
            info["height"] = self.getHeight();
            info["maxEnergy"] = self.getMaxEnergy();
            info["energyDecayRate"] = self.getEnergyDecayRate();
            info["simulationTime"] = self.getSimulationTime();
            info["randomSeed"] = self.getRandomSeed();
            return info;
        }, "Get world configuration as dictionary")
        .def("configureFromDict", [](SimpleWorld& self, const py::dict& config) {
            if (config.contains("width")) self.configure(
                config["width"].cast<size_t>(),
                config.value("height", size_t(480)),
                config.value("visionWidth", size_t(640)),
                config.value("visionHeight", size_t(360)));
            return self;
        }, py::arg("config"), "Configure world from dictionary");

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
             py::return_value_policy::copy)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy)
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
             py::return_value_policy::copy,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::copy,
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
             py::return_value_policy::automatic_reference,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getBrainInfo", [](Brain& self) {
            py::dict info;
            info["totalNeurons"] = self.getTotalNeuronCount();
            info["totalSynapses"] = self.getTotalSynapseCount();
            info["firingNeurons"] = self.getFiringNeuronCount();
            info["avgFiringRate"] = self.getAverageFiringRate();
            info["spikeCount"] = self.getTotalSpikeCount();
            info["eirRatio"] = self.getExcitationInhibitionRatio();
            info["devStage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, "Get brain statistics as dictionary")
        .def("runEpisode", [](Brain& self, SimpleWorld& world, int steps) {
            world.reset();
            for (int i = 0; i < steps; ++i) {
                auto percept = world.getSensoryPercept();
                self.receiveSensoryInput(percept);
                self.step(i);
                world.update(i);
            }
            return world.getSensoryPercept();
        }, py::arg("world"), py::arg("steps") = 100,
           "Run a complete simulation episode")
        .def("configureNeuronType", [](Brain& self, NeuronType type, float v_thresh, float tau_mem, float tau_ref) {
            // Configure neurons of a specific type
            // This is a simplified example - actual implementation would need to iterate through regions
            return self;
        }, py::arg("type"), py::arg("v_thresh"), py::arg("tau_mem"), py::arg("tau_ref"),
           "Configure neuron parameters for specific type");

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
             py::return_value_policy::automatic_reference,
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
        .def("getAgentInfo", [](AgentBrain& self) {
            py::dict info;
            info["devStage"] = static_cast<int>(self.getDevelopmentalStage());
            info["neuromodulationLevel"] = self.getNeuromodulationLevel();
            info["curiosityLevel"] = self.getCuriosityLevel();
            info["noveltyLevel"] = self.getNoveltyLevel();
            info["predictionError"] = self.getPredictionError();
            return info;
        }, "Get agent statistics as dictionary")
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, AgentBrain& agent, int steps) {
            world.reset();
            agent.initialize(world);
            for (int i = 0; i < steps; ++i) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
            }
            return agent.getAgentInfo();
        }, py::arg("world"), py::arg("agent"), py::arg("steps") = 100,
           "Run a complete agent episode");

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
             py::return_value_policy::automatic_reference,
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

    // Convenience factory functions
    m.def("createSimpleConfig", [](const py::dict& config_dict) {
        auto config = std::make_shared<Config>();
        // Apply configuration from dictionary
        for (auto item : config_dict) {
            std::string key = item.first.cast<std::string>();
            if (item.second.is_none()) continue;
            // Handle different config types based on key
            if (key == "brain.neuron_count") {
                config->set(key, item.second.cast<int>());
            } else if (key == "brain.synapse_density") {
                config->set(key, item.second.cast<float>());
            } else if (key == "brain.v_thresh") {
                config->set(key, item.second.cast<float>());
            } else {
                config->set(key, item.second.cast<std::string>());
            }
        }
        return config;
    }, py::arg("config_dict") = py::dict(),
       "Create configuration from dictionary parameters");

    // Type conversion helpers
    m.def("toFloatVector", [](const py::list& list) {
        return std::vector<float>(list.begin(), list.end());
    }, "Convert Python list to float vector")

    .def("fromFloatVector", [](const std::vector<float>& vec) {
        py::list result;
        for (float val : vec) {
            result.append(val);
        }
        return result;
    }, "Convert float vector to Python list")

    .def("toStringVector", [](const py::list& list) {
        return std::vector<std::string>(list.begin(), list.end());
    }, "Convert Python list to string vector")

    .def("fromStringVector", [](const std::vector<std::string>& vec) {
        py::list result;
        for (const auto& val : vec) {
            result.append(val);
        }
        return result;
    }, "Convert string vector to Python list")

    // Configuration convenience functions
    .def("createConfigWithDefaults", []() {
        auto config = std::make_shared<Config>();
        // Set default configuration values
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.v_thresh", -50.0f);
        config->set("brain.v_rest", -70.0f);
        config->set("brain.tau_mem", 20.0f);
        config->set("brain.tau_ref", 2.0f);
        return config;
    }, "Create configuration with default values")

    .def("createWorldWithConfig", [](const py::dict& world_config) {
        auto world = std::make_shared<SimpleWorld>();
        if (world_config.contains("width")) world->configure(
            world_config["width"].cast<size_t>(),
            world_config.value("height", size_t(480)),
            world_config.value("visionWidth", size_t(640)),
            world_config.value("visionHeight", size_t(360)));
        return world;
    }, py::arg("world_config") = py::dict(),
       "Create world with configuration from dictionary");

    // Simulation convenience functions
    .def("runSimpleSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, int steps) {
        brain->initialize();
        world->reset();
        for (int i = 0; i < steps; ++i) {
            auto percept = world->getSensoryPercept();
            brain->receiveSensoryInput(percept);
            brain->step(i);
            world->update(i);
        }
        py::dict result;
        result["brain"] = brain;
        result["world"] = world;
        result["finalPercept"] = world->getSensoryPercept();
        return result;
    }, py::arg("brain"), py::arg("world"), py::arg("steps") = 100,
       "Run a complete simulation episode");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
