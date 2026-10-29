#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
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

// Enhanced Python bindings with advanced features
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This enhanced version includes:
        - Advanced configuration utilities
        - Convenience methods for common operations
        - Pythonic API design
        - Comprehensive error handling
        - Type conversion utilities
        - Performance monitoring tools
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // === ID TYPES ===
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
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // === ENUMERATIONS ===
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

    // === CONFIGURATION CLASS ===
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration system for NLM)")
        // Basic config methods
        .def(py::init<>())
        .def("loadFromFile", py::overload_cast<const std::string&>(&Config::loadFromFile),
             py::arg("filepath"), "Load configuration from a config file (simple key=value format)")
        .def("loadFromConfigFile", &Config::loadFromConfigFile,
             py::arg("filepath"), "Load configuration with type conversion from file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile,
             py::arg("filepath"), "Save configuration to file")
        
        // Enhanced configuration methods
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            self.set(key, static_cast<double>(value), ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        
        // Value getting methods with type conversion
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            if (self.has(key)) {
                return self.getOr<std::string>(key, defaultValue);
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get string configuration value")
        
        .def("getInt", [](Config& self, const std::string& key, int defaultValue) {
            if (self.has(key)) {
                return self.getOr<int>(key, defaultValue);
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get integer configuration value")
        
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            if (self.has(key)) {
                return static_cast<float>(self.getOr<double>(key, static_cast<double>(defaultValue)));
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get float configuration value")
        
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            if (self.has(key)) {
                return self.getOr<bool>(key, defaultValue);
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean configuration value")
        
        // Enhanced query methods
        .def("getConfigKey", &Config::has, py::arg("key"), "Check if a configuration key exists")
        .def("getAllKeys", &Config::getKeys, "Get all configuration keys")
        .def("clear", &Config::clear, "Clear all configuration entries")
        .def("summary", &Config::summary, "Get configuration summary")
        
        // Comparison and validation
        .def("validate", [](Config& self, const std::vector<std::string>& requiredKeys) {
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    throw py::value_error("Missing required configuration key: " + key);
                }
            }
            return true;
        }, py::arg("requiredKeys"), "Validate that all required keys exist")
        
        // Configuration templates and presets
        .def("createBrainConfig", [](Config& self, int neuronCount = 1000, int regionCount = 1, 
                                    float connectionProb = 0.1) {
            self.clear();
            self.set("brain.neuron_count", neuronCount);
            self.set("brain.region_count", regionCount);
            self.set("brain.connection_probability", connectionProb);
            // Set default values for Phase 2
            self.set("random_seed", 42);
            self.set("simulation_timestep", 0.001);
        }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
           py::arg("connectionProb") = 0.1, "Create a brain-specific configuration")
        
        .def("createAgentConfig", [](Config& self) {
            self.clear();
            // Set default values for agent system
            self.set("agent.reward_modulation", true);
            self.set("agent.structural_plasticity", true);
            self.set("agent.development", true);
            self.set("agent.curiosity", true);
            self.set("agent.planning", true);
        }, "Create an agent-specific configuration")
        
        .def("createWorldConfig", [](Config& self, int width = 20, int height = 20,
                                    int visionWidth = 8, int visionHeight = 8) {
            self.clear();
            self.set("world.width", width);
            self.set("world.height", height);
            self.set("world.vision_width", visionWidth);
            self.set("world.vision_height", visionHeight);
        }, py::arg("width") = 20, py::arg("height") = 20,
           py::arg("visionWidth") = 8, py::arg("visionHeight") = 8, "Create a world-specific configuration")
        
        // Advanced methods
        .def("mergeConfig", [](Config& self, const Config& other) {
            for (const auto& key : other.getKeys()) {
                if (other.has(key)) {
                    self.set(key, other.getOr<std::string>(key, ""), ConfigSource::Runtime);
                }
            }
            return self;
        }, py::arg("other"), "Merge another configuration into this one")
        
        .def("copyConfig", [](Config& self) {
            Config result;
            for (const auto& key : self.getKeys()) {
                if (self.has(key)) {
                    auto val = self.get<std::string>(key);
                    if (val.has_value()) {
                        result.set(key, val.value(), ConfigSource::Runtime);
                    }
                }
            }
            return result;
        }, "Create a copy of this configuration")
        
        // Pythonic dunder methods
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__contains__", [](const Config& cfg, const std::string& key) {
            return cfg.has(key);
        })
        .def("__getitem__", [](Config& cfg, const std::string& key) {
            if (!cfg.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return cfg.getOr<std::string>(key, "");
        }, py::arg("key"))
        .def("__setitem__", [](Config& cfg, const std::string& key, const std::string& value) {
            cfg.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"))
        .def("__len__", [](const Config& cfg) {
            return cfg.getKeys().size();
        })
        .def("__iter__", [](const Config& cfg) {
            return py::make_iterator(cfg.getKeys().begin(), cfg.getKeys().end());
        }, py::return_value_policy::reference);

    // === SENSORY INPUT CLASS ===
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(input.getType()) + 
                   ", dims=" + std::to_string(input.getDimensions()) + ">";
        });

    // === VISION CLASS ===
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataRGB", [](Vision& self, const std::vector<std::vector<std::vector<float>>>& rgbData) {
            // Convert 3D RGB to 1D vector
            std::vector<float> flatData;
            for (const auto& row : rgbData) {
                for (const auto& pixel : row) {
                    for (float channel : pixel) {
                        flatData.push_back(channel);
                    }
                }
            }
            self.setData(flatData);
        }, py::arg("rgbData"), "Set vision data from RGB format")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getPixelCount", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels")
        .def("normalize", [](Vision& self, float mean = 0.0f, float std = 1.0f) {
            // Normalize vision data
            auto data = self.getData();
            for (auto& value : data) {
                value = (value - mean) / std;
            }
            self.setData(data);
        }, py::arg("mean") = 0.0f, py::arg("std") = 1.0f, "Normalize vision data")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Note: This would require actual image resizing implementation
            // For now, just log a warning
            std::cerr << "Vision resize not yet implemented\n";
        }, py::arg("newWidth"), py::arg("newHeight"), "Resize vision data (placeholder)");

    // === AUDIO CLASS ===
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataWaveform", [](Audio& self, const std::vector<std::vector<float>>& waveform) {
            // Convert 2D waveform to 1D vector
            std::vector<float> flatData;
            for (const auto& sample : waveform) {
                for (float value : sample) {
                    flatData.push_back(value);
                }
            }
            self.setData(flatData);
        }, py::arg("waveform"), "Set audio data from waveform format")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("normalize", [](Audio& self, float mean = 0.0f, float std = 1.0f) {
            // Normalize audio data
            auto data = self.getData();
            for (auto& value : data) {
                value = (value - mean) / std;
            }
            self.setData(data);
        }, py::arg("mean") = 0.0f, py::arg("std") = 1.0f, "Normalize audio data");

    // === INTERNAL SIGNALS CLASS ===
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add an internal signal")
        .def("addSignalWithTimestamp", [](InternalSignals& self, float value, uint64_t timestamp) {
            self.addSignal(value);
            self.setTimestamp(timestamp);
        }, py::arg("value"), py::arg("timestamp"), "Add signal with timestamp")
        .def("getMean", [](const InternalSignals& self) {
            auto signals = self.getData();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get mean of internal signals")
        .def("getStd", [](const InternalSignals& self) {
            auto signals = self.getData();
            if (signals.size() < 2) return 0.0f;
            float mean = std::accumulate(signals.begin(), signals.end(), 0.0f) / signals.size();
            float variance = 0.0f;
            for (float value : signals) {
                variance += (value - mean) * (value - mean);
            }
            variance /= signals.size();
            return std::sqrt(variance);
        }, "Get standard deviation of internal signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    // === ACTION CLASS ===
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation)")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"), "Get parameter at index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set parameter at index")
        .def("clone", &Action::clone)
        .def("isValid", [](const Action& self) {
            return !self.getName().empty();
        }, "Check if action is valid")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " (type=" + std::to_string(static_cast<int>(action.getType())) + ")>";
        });

    // === WORLD OBJECT CLASS ===
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)")
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("isNear", [](const WorldObject& self, float x, float y, float threshold) {
            return self.distanceTo(x, y) <= threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if near position")
        .def("move", [](WorldObject& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
        }, py::arg("dx"), py::arg("dy"), "Move object by offset")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = static_cast<int>(self.type);
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert object to dictionary representation")
        .def_static("from_dict", [](py::dict data) {
            WorldObject obj;
            obj.x = py::cast<float>(data["x"]);
            obj.y = py::cast<float>(data["y"]);
            obj.radius = py::cast<float>(data["radius"]);
            obj.type = static_cast<WorldObjectType>(py::cast<int>(data["type"]));
            obj.value = py::cast<float>(data["value"]);
            obj.active = py::cast<bool>(data["active"]);
            return obj;
        }, py::arg("data"), "Create object from dictionary representation");

    // === AGENT BODY CLASS ===
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)")
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
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as tuple (x, y)")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as tuple (vx, vy)")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set velocity")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed")
        .def("getDirection", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get direction in radians")
        .def("moveForward", [](AgentBody& self, float distance, float angle) {
            self.x += distance * std::cos(angle);
            self.y += distance * std::sin(angle);
            self.isMoving = true;
        }, py::arg("distance"), py::arg("angle"), "Move forward by distance at angle")
        .def("turn", [](AgentBody& self, float angleDelta) {
            self.orientation += angleDelta;
            self.isTurning = true;
            // Keep orientation in [0, 2π)
            if (self.orientation >= 2.0f * M_PI) self.orientation -= 2.0f * M_PI;
            else if (self.orientation < 0.0f) self.orientation += 2.0f * M_PI;
        }, py::arg("angleDelta"), "Turn by angle delta")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        .def("to_dict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert body to dictionary representation")
        .def_static("from_dict", [](py::dict data) {
            AgentBody body;
            body.x = py::cast<float>(data["x"]);
            body.y = py::cast<float>(data["y"]);
            body.orientation = py::cast<float>(data["orientation"]);
            body.velocityX = py::cast<float>(data["velocityX"]);
            body.velocityY = py::cast<float>(data["velocityY"]);
            body.angularVelocity = py::cast<float>(data["angularVelocity"]);
            body.energy = py::cast<float>(data["energy"]);
            body.health = py::cast<float>(data["health"]);
            body.age = py::cast<float>(data["age"]);
            body.isMoving = py::cast<bool>(data["isMoving"]);
            body.isTurning = py::cast<bool>(data["isTurning"]);
            body.lastActionTime = py::cast<float>(data["lastActionTime"]);
            return body;
        }, py::arg("data"), "Create body from dictionary representation");

    // === ACTION RESULT CLASS ===
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("getReward", [](const ActionResult& self) {
            return self.reward;
        }, "Get reward value")
        .def("getMessage", [](const ActionResult& self) {
            return self.message;
        }, "Get result message")
        .def("to_dict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        }, "Convert result to dictionary")
        .def_static("from_dict", [](py::dict data) {
            ActionResult result;
            result.reward = py::cast<float>(data["reward"]);
            result.success = py::cast<bool>(data["success"]);
            result.message = py::cast<std::string>(data["message"]);
            return result;
        }, py::arg("data"), "Create result from dictionary");

    // === SENSORY PERCEPT CLASS ===
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)")
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
        .def("getSignalCount", [](const SensoryPercept& self) {
            return static_cast<size_t>(self.getAllSignals().size());
        }, "Get total number of signals")
        .def("hasVision", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            return vision && vision->getWidth() > 0 && vision->getHeight() > 0;
        }, "Check if percept contains vision")
        .def("hasAudio", [](const SensoryPercept& self) {
            auto audio = self.getAudio();
            return audio && audio->getNumSamples() > 0;
        }, "Check if percept contains audio")
        .def("hasInternal", [](const SensoryPercept& self) {
            auto internal = self.getInternal();
            return internal && !internal->getAllSignals().empty();
        }, "Check if percept contains internal signals")
        .def("getSensorySummary", [](const SensoryPercept& self) {
            std::string summary = "SensoryPercept: ";
            if (self.hasVision()) summary += "Vision present, ";
            if (self.hasAudio()) summary += "Audio present, ";
            if (self.hasInternal()) summary += "Internal present, ";
            if (summary.size() > strlen("SensoryPercept: ")) summary.erase(summary.size() - 2);
            return summary;
        }, "Get human-readable sensory summary")
        .def("to_dict", [](const SensoryPercept& percept) {
            py::dict result;
            
            auto vision = percept.getVision();
            if (vision) {
                py::dict v;
                v["width"] = vision->getWidth();
                v["height"] = vision->getHeight();
                v["channels"] = vision->getChannels();
                result["vision"] = v;
            }
            
            auto audio = percept.getAudio();
            if (audio) {
                py::dict a;
                a["sampleRate"] = audio->getSampleRate();
                a["numSamples"] = audio->getNumSamples();
                result["audio"] = a;
            }
            
            auto internal = percept.getInternal();
            if (internal) {
                result["internal"] = py::cast(internal->getAllSignals());
            }
            
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert percept to dictionary representation")
        .def_static("from_dict", [](py::dict data) {
            SensoryPercept percept;
            
            if (data.contains("vision")) {
                auto vision = std::make_shared<Vision>();
                auto vdata = data["vision"].cast<py::dict>();
                vision->configure(
                    vdata["width"].cast<size_t>(),
                    vdata["height"].cast<size_t>(),
                    vdata["channels"].cast<size_t>()
                );
                percept.setVision(vision);
            }
            
            if (data.contains("audio")) {
                auto audio = std::make_shared<Audio>();
                auto adata = data["audio"].cast<py::dict>();
                audio->configure(
                    adata["sampleRate"].cast<size_t>(),
                    adata["numSamples"].cast<size_t>()
                );
                percept.setAudio(audio);
            }
            
            if (data.contains("internal")) {
                auto internal = std::make_shared<InternalSignals>();
                auto idata = data["internal"].cast<std::vector<float>>();
                for (float value : idata) {
                    internal->addSignal(value);
                }
                percept.setInternal(internal);
            }
            
            if (data.contains("timestamp")) {
                percept.setTimestamp(data["timestamp"].cast<uint64_t>());
            }
            
            return percept;
        }, py::arg("data"), "Create percept from dictionary representation");

    // === SIMPLE WORLD CLASS ===
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory capabilities")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getVisionWidth", &SimpleWorld::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SimpleWorld::getVisionHeight, "Get vision height")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("getAgentStart", [](const SimpleWorld& self) {
            return py::make_tuple(self.getAgentBody().x, self.getAgentBody().y);
        }, "Get agent starting position as tuple")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy capacity")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjectCount", [](const SimpleWorld& self) {
            return self.getAgentBody().x; // Placeholder - would need actual object count
        }, "Get number of objects in world")
        .def("clearObjects", [](SimpleWorld& self) {
            // Clear objects by removing them one by one
            for (float y = self.getHeight() - 1; y >= 0; --y) {
                for (float x = self.getWidth() - 1; x >= 0; --x) {
                    self.removeObject(x, y);
                }
            }
        }, "Clear all objects from world")
        .def("to_dict", [](const SimpleWorld& world) {
            py::dict result;
            result["width"] = world.getWidth();
            result["height"] = world.getHeight();
            result["visionWidth"] = world.getVisionWidth();
            result["visionHeight"] = world.getVisionHeight();
            result["maxEnergy"] = world.getMaxEnergy();
            result["energyDecayRate"] = world.getEnergyDecayRate();
            result["simulationTime"] = world.getSimulationTime();
            result["randomSeed"] = world.getRandomSeed();
            
            auto body = world.getAgentBody();
            py::dict bodyDict;
            bodyDict["x"] = body.x;
            bodyDict["y"] = body.y;
            bodyDict["orientation"] = body.orientation;
            result["agentBody"] = bodyDict;
            
            return result;
        }, "Convert world to dictionary representation")
        .def_static("from_dict", [](py::dict data) {
            SimpleWorld world;
            world.configure(
                data["width"].cast<size_t>(),
                data["height"].cast<size_t>(),
                data["visionWidth"].cast<size_t>(),
                data["visionHeight"].cast<size_t>()
            );
            world.setMaxEnergy(data["maxEnergy"].cast<float>());
            world.setEnergyDecayRate(data["energyDecayRate"].cast<float>());
            world.setRandomSeed(data["randomSeed"].cast<uint64_t>());
            
            auto bodyDict = data["agentBody"].cast<py::dict>();
            AgentBody body;
            body.x = bodyDict["x"].cast<float>();
            body.y = bodyDict["y"].cast<float>();
            body.orientation = bodyDict["orientation"].cast<float>();
            world.setAgentStart(body.x, body.y);
            
            return world;
        }, py::arg("data"), "Create world from dictionary representation");

    // === BRAIN CLASS ===
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
        
        // Enhanced brain methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system (internal access)")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system (internal access)")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system (internal access)")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system (internal access)")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system (internal access)")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system (internal access)")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system (internal access)")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system (internal access)")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system (internal access)")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system (internal access)")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get planner system (internal access)")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system (internal access)")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attention system (internal access)")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system (internal access)")
        
        // Simulation utilities
        .def("runEpisode", [](Brain& self, uint64_t steps, float timestep = 0.001f) {
            for (uint64_t step = 0; step < steps; ++step) {
                self.step(step, step * timestep);
            }
            return self.getTotalSpikeCount();
        }, py::arg("steps"), py::arg("timestep") = 0.001f,
            "Run simulation episode for given steps")
        
        .def("runLearningEpisode", [](Brain& self, uint64_t steps, float timestep = 0.001f,
                                     const SensoryInput& input = SensoryInput()) {
            if (!input.getData().empty()) {
                self.receiveSensoryInput(input);
            }
            return self.runEpisode(steps, timestep);
        }, py::arg("steps"), py::arg("timestep") = 0.001f, py::arg("input") = SensoryInput(),
            "Run learning episode with optional sensory input")
        
        .def("simulateUntilTime", [](Brain& self, float maxTime, float timestep = 0.001f) {
            uint64_t steps = static_cast<uint64_t>(maxTime / timestep);
            return self.runEpisode(steps, timestep);
        }, py::arg("maxTime"), py::arg("timestep") = 0.001f,
            "Run simulation until specified time")
        
        // Statistics and analysis
        .def("getPerformanceStats", [](const Brain& self) {
            py::dict stats;
            stats["totalNeurons"] = self.getTotalNeuronCount();
            stats["totalSynapses"] = self.getTotalSynapseCount();
            stats["activeNeurons"] = self.getActiveNeuronCount();
            stats["firingNeurons"] = self.getFiringNeuronCount();
            stats["totalSpikes"] = self.getTotalSpikeCount();
            stats["averageFiringRate"] = self.getAverageFiringRate();
            stats["eI_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive performance statistics")
        
        .def("analyzeActivityPattern", [](const Brain& self, uint64_t windowSize = 100) {
            py::dict pattern;
            pattern["firingRate"] = self.getAverageFiringRate();
            pattern["activityLevel"] = static_cast<float>(self.getFiringNeuronCount()) / 
                                       std::max(1.0f, static_cast<float>(self.getTotalNeuronCount()));
            pattern["burstPotential"] = pattern["firingRate"] > 10.0f; // Simple heuristic
            return pattern;
        }, py::arg("windowSize") = 100, "Analyze current activity pattern")
        
        .def("getDevelopmentalStatus", [](const Brain& self) {
            py::dict status;
            status["stage"] = static_cast<int>(self.getDevelopmentalStage());
            status["isCriticalPeriod"] = (self.getDevelopmentalStage() == DevelopmentalStage::CriticalPeriod);
            status["isAdult"] = (self.getDevelopmentalStage() == DevelopmentalStage::Adult);
            return status;
        }, "Get developmental stage information")
        
        .def("getSystemConnectivity", [](const Brain& self) {
            py::dict connectivity;
            connectivity["neuronCount"] = self.getTotalNeuronCount();
            connectivity["synapseCount"] = self.getTotalSynapseCount();
            connectivity["connectionDensity"] = 
                static_cast<float>(self.getTotalSynapseCount()) / 
                (static_cast<float>(self.getTotalNeuronCount()) * 
                 static_cast<float>(self.getTotalNeuronCount()));
            return connectivity;
        }, "Get brain system connectivity metrics");

    // === AGENT BRAIN CLASS ===
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
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
        
        // Enhanced agent brain methods
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        
        // Agent behavior control
        .def("setActionSelectionMode", [](AgentBrain& self, const std::string& mode) {
            if (mode == "exploration") {
                self.enableCuriosity(true);
            } else if (mode == "exploitation") {
                self.enableCuriosity(false);
            } else if (mode == "balanced") {
                self.enableCuriosity(true);
                self.enableRewardModulation(true);
            }
        }, py::arg("mode"), "Set action selection mode (exploration/exploitation/balanced)")
        
        .def("getBehaviorProfile", [](const AgentBrain& self) {
            py::dict profile;
            profile["rewardModulation"] = self.isRewardModulationEnabled();
            profile["structuralPlasticity"] = self.isStructuralPlasticityEnabled();
            profile["development"] = self.isDevelopmentEnabled();
            profile["curiosity"] = self.isCuriosityEnabled();
            return profile;
        }, "Get current behavior profile")
        
        .def("updateBehavior", [](AgentBrain& self, const py::dict& parameters) {
            if (parameters.contains("reward")) {
                self.enableRewardModulation(parameters["reward"].cast<bool>());
            }
            if (parameters.contains("plasticity")) {
                self.enableStructuralPlasticity(parameters["plasticity"].cast<bool>());
            }
            if (parameters.contains("development")) {
                self.enableDevelopment(parameters["development"].cast<bool>());
            }
            if (parameters.contains("curiosity")) {
                self.enableCuriosity(parameters["curiosity"].cast<bool>());
            }
        }, py::arg("parameters"), "Update agent behavior based on parameters")
        
        .def("simulateStep", [](AgentBrain& self, const SimpleWorld& world, float timestep = 0.1f) {
            // Simulate one complete step
            auto percept = world.getSensoryPercept();
            self.processSensoryInput(percept);
            
            // Note: This would require access to the underlying brain to call step
            // For now, this is a placeholder that would be implemented in the full C++ code
            return Action(); // Return empty action
        }, py::arg("world"), py::arg("timestep") = 0.1f, "Simulate one agent step")
        
        .def("runAgentEpisode", [](AgentBrain& self, SimpleWorld& world, uint64_t steps, float timestep = 0.1f) {
            // Run complete agent episode
            uint64_t successfulSteps = 0;
            for (uint64_t step = 0; step < steps; ++step) {
                // This would require the full implementation with access to brain's step method
                // Placeholder implementation
                world.update(timestep);
                successfulSteps++;
            }
            return successfulSteps;
        }, py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1f, "Run complete agent episode");

    // === FACTORY FUNCTIONS ===
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->createBrainConfig(1000); // Default brain config
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    m.def("createAgentSetup", [](std::shared_ptr<Brain> brain, SimpleWorld& world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize an agent brain with world")
    
    m.def("createCompleteSimulation", []() -> std::tuple<std::shared_ptr<Config>, std::shared_ptr<Brain>, 
                                                        std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        // Create complete simulation setup
        auto config = std::make_shared<Config>();
        config->createBrainConfig();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(config, brain, world, agent);
    }, "Create a complete simulation setup with all components")
    
    // === UTILITY FUNCTIONS ===
    m.def("version", []() {
        return "NLM Enhanced Python Bindings v2.0";
    }, "Get the version of the enhanced bindings")
    
    m.def("validateConfig", [](const Config& config, const std::vector<std::string>& requiredKeys) {
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                throw py::value_error("Missing required configuration key: " + key);
            }
        }
        return true;
    }, py::arg("config"), py::arg("requiredKeys"), "Validate that all required keys exist in config")
    
    m.def("benchmarkSimulation", [](const Brain& brain, uint64_t steps, float timestep = 0.001f) {
        auto start = std::chrono::high_resolution_clock::now();
        brain.step(steps, steps * timestep);
        auto end = std::chrono::high_resolution_clock::now();
        
        py::dict results;
        results["steps"] = steps;
        results["timeElapsed"] = std::chrono::duration<double>(end - start).count();
        results["spikesGenerated"] = brain.getTotalSpikeCount();
        results["spikesPerSecond"] = brain.getTotalSpikeCount() / std::chrono::duration<double>(end - start).count();
        
        return results;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.001f,
        "Run benchmark simulation and return performance metrics")
    
    m.def("compareBrains", [](const Brain& brain1, const Brain& brain2, uint64_t steps, float timestep = 0.001f) {
        // Run both brains and compare their activity
        brain1.step(steps, steps * timestep);
        brain2.step(steps, steps * timestep);
        
        py::dict comparison;
        comparison["brain1_spikes"] = brain1.getTotalSpikeCount();
        comparison["brain2_spikes"] = brain2.getTotalSpikeCount();
        comparison["brain1_firingRate"] = brain1.getAverageFiringRate();
        comparison["brain2_firingRate"] = brain2.getAverageFiringRate();
        comparison["brain1_eI_ratio"] = brain1.getExcitationInhibitionRatio();
        comparison["brain2_eI_ratio"] = brain2.getExcitationInhibitionRatio();
        
        if (brain2.getTotalSpikeCount() > 0) {
            comparison["spikeRatio"] = static_cast<float>(brain1.getTotalSpikeCount()) / 
                                      brain2.getTotalSpikeCount();
        } else {
            comparison["spikeRatio"] = 0.0f;
        }
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), py::arg("steps"), py::arg("timestep") = 0.001f,
        "Compare two brains' activity over time")
    
    // === ENUMERATION ACCESS ===
    m.attr("NeuronTypes") = py::module::import("enum?").attr("NeuronType"); // This will need adjustment
    
    // === INVALID ID CONSTANTS ===
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // === ADVANCED TYPE CONVERSIONS ===
    py::class_<py::array_t<float>>(m, "FloatArray", R"pbdoc(Float array utility)")
        .def(py::init([](py::buffer buf) {
            auto info = buf.request();
            if (info.ndim != 1) throw py::value_error("Buffer must be 1-dimensional");
            
            auto data = std::vector<float>(info.size);
            std::memcpy(data.data(), info.ptr, info.size * sizeof(float));
            
            py::array_t<float> result({info.size});
            auto acc = result.mutable_unchecked<1>();
            for (ssize_t i = 0; i < info.size; ++i) {
                acc(i) = data[i];
            }
            
            return result;
        }));
    
    // === PERFORMANCE MONITORING ===
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring)")
        .def(py::init<>())
        .def("start", &PerformanceMonitor::start, "Start monitoring")
        .def("stop", &PerformanceMonitor::stop, "Stop monitoring")
        .def("getMetrics", &PerformanceMonitor::getMetrics, "Get performance metrics");
}

} // namespace nlm
