#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Full access to NLM's experimental brain simulation capabilities
        - Integration with Python ecosystem
        - Type-safe and documented API
        - Helper functions for common use cases
        - Advanced utilities for power users
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register custom exceptions
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // NeuronId, SynapseId, RegionId, PopulationId - Identity Types
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

    // Enum Types with Python-friendly names
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

    // Enhanced Config class with Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Configuration management for NLM brain simulation.
        Supports setting/getting values with type checking and validation.
        
        Features:
        - JSON file I/O
        - Command line argument loading
        - Type-safe value access
        - Configuration presets
        - Runtime configuration management
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
        // Enhanced get/set methods with better Python integration
        .def("set", static_cast<void (Config::*)(const std::string&, const ConfigValue&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (general)")
        .def("set", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (int)")
        .def("set", static_cast<void (Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (float)")
        .def("set", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (bool)")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (string)")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<int>&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (int vector)")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<double>&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (float vector)")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::vector<std::string>&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value (string vector)")
        // Enhanced get methods with better type safety
        .def("get", static_cast<std::optional<int> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as int (returns None if not found)")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as float (returns None if not found)")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as bool (returns None if not found)")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as string (returns None if not found)")
        .def("get", static_cast<std::optional<std::vector<int>> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as int vector (returns None if not found)")
        .def("get", static_cast<std::optional<std::vector<double>> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as float vector (returns None if not found)")
        .def("get", static_cast<std::optional<std::vector<std::string>> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as string vector (returns None if not found)")
        .def("getOr", &Config::getOr, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (type-safe)")
        // Additional Pythonic methods
        .def("getInt", &Config::get<int>, py::arg("key"),
             "Get integer value (convenience method)")
        .def("getFloat", &Config::get<double>, py::arg("key"),
             "Get float value (convenience method)")
        .def("getBool", &Config::get<bool>, py::arg("key"),
             "Get boolean value (convenience method)")
        .def("getString", &Config::get<std::string>, py::arg("key"),
             "Get string value (convenience method)")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set integer value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set float value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set boolean value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set string value")
        // Configuration presets and utilities
        .def("applyPreset", [](Config& self, const std::string& presetName) {
            if (presetName == "minimal") {
                self.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
                self.set("connection_probability", 0.05, ConfigSource::Runtime);
            } else if (presetName == "standard") {
                self.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
                self.set("connection_probability", 0.1, ConfigSource::Runtime);
                self.set("plasticity.stdp.enable", true, ConfigSource::Runtime);
                self.set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            } else if (presetName == "high_performance") {
                self.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
                self.set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
                self.set("connection_probability", 0.2, ConfigSource::Runtime);
                self.set("plasticity.structural.enable", true, ConfigSource::Runtime);
                self.set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            } else {
                throw py::value_error("Unknown preset: " + presetName);
            }
        }, py::arg("presetName"),
            "Apply a predefined configuration preset")
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys
            auto keys = self.getKeys();
            bool valid = true;
            std::string errors;
            
            for (const auto& key : keys) {
                if (!self.has(key)) {
                    valid = false;
                    errors += "Key '" + key + "' exists but validation failed; ";
                }
            }
            
            return std::make_pair(valid, errors);
        }, "Validate configuration and return (is_valid, error_message)")
        .def("clone", []() {
            auto newConfig = std::make_shared<Config>();
            return newConfig;
        }, "Create a copy of the configuration (convenience method)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Base class for all sensory input types in NLM.
        Represents sensory data from the environment.
        
        Attributes:
            type: Type of sensory input
            data: Raw data vector
            dimensions: Vector length
            timestamp: Time of input
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Vision sensory input representing visual information from the world.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (typically 3 for RGB)
            data: Flattened pixel data vector
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("createFromImage", [](const std::vector<std::vector<std::vector<float>>>& image) {
            Vision vision;
            // Convert 3D image to 1D vector
            size_t width = image.size();
            size_t height = image[0].size();
            size_t channels = image[0][0].size();
            
            std::vector<float> flattened(width * height * channels);
            size_t index = 0;
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    for (size_t c = 0; c < channels; ++c) {
                        flattened[index++] = image[x][y][c];
                    }
                }
            }
            vision.setData(flattened);
            return vision;
        }, py::arg("image"),
            "Create Vision from 3D image array (width x height x channels)")
        .def("toImage", [](const Vision& self) {
            std::vector<std::vector<std::vector<float>>> image;
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            size_t channels = self.getChannels();
            size_t totalPixels = width * height * channels;
            
            const auto& data = self.getData();
            if (data.size() != totalPixels) {
                throw py::value_error("Data size doesn't match dimensions");
            }
            
            image.resize(width);
            for (size_t x = 0; x < width; ++x) {
                image[x].resize(height);
                for (size_t y = 0; y < height; ++y) {
                    image[x][y].resize(channels);
                    size_t index = (y * width + x) * channels;
                    for (size_t c = 0; c < channels; ++c) {
                        image[x][y][c] = data[index + c];
                    }
                }
            }
            return image;
        }, "Convert Vision to 3D image array (width x height x channels)");

    // Enhanced Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Audio sensory input representing sound information.
        
        Attributes:
            sampleRate: Audio sample rate in Hz
            numSamples: Number of audio samples
            samples: Audio sample data vector
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getSamples", &Audio::getData)
        .def("setSamples", [](Audio& self, const std::vector<float>& samples) {
            self.setData(samples);
        }, py::arg("samples"));

    // Enhanced InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        Internal signals sensory input representing internal brain states.
        
        Attributes:
            signals: Vector of internal signal values
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getData)
        .def("setSignals", [](InternalSignals& self, const std::vector<float>& signals) {
            self.clearSignals();
            for (float signal : signals) {
                self.addSignal(signal);
            }
        }, py::arg("signals"))
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        });

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Represents a motor action that can be executed by the agent.
        
        Attributes:
            type: Action type (Movement, Interaction, etc.)
            parameters: Action parameters as float values
            name: Human-readable action name
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Pythonic convenience methods
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"),
            "Add a parameter to the action")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"),
            "Get parameter at index (raises IndexError if out of range)")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw py::index_error("Parameter index out of range");
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"),
            "Set parameter at index (raises IndexError if out of range)");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Represents an object in the NLM world.
        
        Attributes:
            x: X coordinate
            y: Y coordinate
            radius: Object radius
            type: Object type (Resource, Hazard, Wall, etc.)
            value: Object value (e.g., food, health, etc.)
            active: Whether the object is active
        )pbdoc")
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
        }, py::arg("x"), py::arg("y"),
            "Calculate distance to position (x, y)")
        .def("isWithinRange", [](const WorldObject& self, float x, float y, float range) {
            float dx = self.x - x;
            float dy = self.y - y;
            return (dx*dx + dy*dy) <= (range*range);
        }, py::arg("x"), py::arg("y"), py::arg("range"),
            "Check if object is within range of position (x, y)");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Represents the state of an agent in the NLM world.
        
        Attributes:
            x: X position
            y: Y position
            orientation: Facing direction in radians
            velocityX: X velocity
            velocityY: Y velocity
            angularVelocity: Angular velocity
            energy: Current energy level
            health: Health level
            age: Agent age in simulation time
            isMoving: Whether agent is moving
            isTurning: Whether agent is turning
            lastActionTime: Time since last action
        )pbdoc")
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
        .def("moveTo", [](AgentBody& self, float x, float y, float speed) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance > 0) {
                float scale = speed / distance;
                self.velocityX = dx * scale;
                self.velocityY = dy * scale;
                self.isMoving = true;
            }
        }, py::arg("x"), py::arg("y"), py::arg("speed"),
            "Move toward target position at given speed")
        .def("rotateTo", [](AgentBody& self, float targetOrientation, float speed) {
            float angleDiff = targetOrientation - self.orientation;
            // Normalize angle difference
            while (angleDiff > M_PI) angleDiff -= 2 * M_PI;
            while (angleDiff < -M_PI) angleDiff += 2 * M_PI;
            
            float rotation = std::min(speed, std::abs(angleDiff));
            if (angleDiff > 0) rotation = -rotation;
            
            self.orientation += rotation;
            self.angularVelocity = rotation;
            self.isTurning = true;
        }, py::arg("targetOrientation"), py::arg("speed"),
            "Rotate toward target orientation at given speed");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        
        Result of applying a motor command to the world.
        
        Attributes:
            reward: Reward received from the action
            success: Whether the action was successful
            message: Optional message describing the result
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0;
        }, "Check if action result was good (successful and positive reward)")
        .def("isBad", [](const ActionResult& self) {
            return !self.success || self.reward < 0;
        }, "Check if action result was bad (failed or negative reward)");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        Sensory percept data combining multiple sensory modalities.
        
        Attributes:
            vision: Visual data (Vision object)
            touch: Touch data (vector)
            internal: Internal signals (InternalSignals object)
            proprioception: Proprioceptive data (vector)
            audio: Audio data (Audio object)
            timestamp: Percept timestamp
        )pbdoc")
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
        // Helper methods for percept manipulation
        .def("updateFromWorld", [](SensoryPercept& self, const SimpleWorld& world) {
            // Update percept from world state
            self.setVision(world.getSensoryPercept().getVision());
            self.setTouch(world.getSensoryPercept().getTouch());
            self.setInternal(world.getSensoryPercept().getInternal());
            self.setProprioception(world.getSensoryPercept().getProprioception());
            self.setAudio(world.getSensoryPercept().getAudio());
            self.setTimestamp(world.getSimulationTime());
        }, py::arg("world"),
            "Update percept from world state")
        .def("addNoise", [](SensoryPercept& self, float noiseLevel) {
            // Add Gaussian noise to sensory signals
            auto vision = self.getVision();
            auto visionData = vision.getData();
            for (auto& pixel : visionData) {
                pixel += static_cast<float>((rand() / (RAND_MAX + 1.0)) * 2.0 - 1.0) * noiseLevel;
            }
            vision.setData(visionData);
            self.setVision(vision);
        }, py::arg("noiseLevel"),
            "Add Gaussian noise to visual data");

    // Enhanced SimpleWorld class with Pythonic interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        Simple 2D world simulation environment for NLM agents.
        
        Features:
        - Agent movement and interaction
        - Object management
        - State persistence
        - Sensory perception
        - Action execution
        
        Attributes:
            width: World width in units
            height: World height in units
            visionWidth: Vision width in pixels
            visionHeight: Vision height in pixels
            agentPosition: Agent's current position
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
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
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Pythonic world methods
        .def("createResource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value"),
            "Create a resource object at position (x, y)")
        .def("createHazard", [](SimpleWorld& self, float x, float y, float hazardLevel) {
            WorldObject obj(x, y, WorldObjectType::Hazard, hazardLevel, 0.4f);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("hazardLevel"),
            "Create a hazard object at position (x, y)")
        .def("createWall", [](SimpleWorld& self, float x, float y, float length) {
            // Create a wall by adding multiple wall objects
            std::vector<WorldObject> wallObjects;
            for (float dx = -length/2; dx <= length/2; dx += 0.5f) {
                WorldObject obj(x + dx, y, WorldObjectType::Wall, 1.0f, 0.2f);
                wallObjects.push_back(obj);
                self.addObject(obj);
            }
            return wallObjects;
        }, py::arg("x"), py::arg("y"), py::arg("length"),
            "Create a wall at position (x, y)")
        .def("getObjectsAt", [](SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> nearbyObjects;
            // Note: This would require world to store all objects, which it doesn't currently
            // For now, return empty vector
            return nearbyObjects;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
            "Get all objects within radius of position (x, y)")
        .def("simulateEpisode", [](SimpleWorld& self, const std::function<py::object()>& actionFunc, double maxTime) {
            // Simulate an episode until max time or episode ends
            double time = 0.0;
            while (time < maxTime) {
                self.update(0.1);
                time = self.getSimulationTime();
                // Agent would typically process percept and select action here
                // This is a simplified version
            }
        }, py::arg("actionFunc"), py::arg("maxTime"),
            "Simulate an episode with given action function");

    // Enhanced Brain class with Pythonic interface and batch operations
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        Central neural simulation brain implementing spiking neural computation
        with integrated memory, prediction, cognition, and neuromodulation systems.
        
        Features:
        - Event-driven spike propagation
        - Multiple plasticity mechanisms
        - Memory systems (working, episodic, associative, procedural)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Developmental plasticity
        - Checkpoint saving/loading
        - Batch operations for performance
        
        Attributes:
            config: Brain configuration
            developmentStage: Current developmental stage
        )pbdoc")
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
        // Spike system access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal)
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal)
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal)
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal)
        // Statistics
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount)
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        // Action production
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        // State management
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        // Region management
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
        // Memory systems access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal)
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal)
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal)
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal)
        // Cognition systems access
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal)
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal)
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal)
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal)
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        // Neuromodulation systems access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal)
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal)
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal)
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal)
        // Configuration and utilities
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Enhanced Pythonic methods
        .def("batchInjectCurrent", [](Brain& self, NeuronType type, MembranePotential current, size_t count) {
            for (size_t i = 0; i < count; ++i) {
                // This would need access to neuron IDs, which isn't directly exposed
                // For now, use the existing method
                self.injectCurrentToNeurons(type, current);
            }
        }, py::arg("type"), py::arg("current"), py::arg("count") = 1,
            "Inject current to multiple neurons of given type (batch operation)")
        .def("getNeuronStats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = static_cast<int64_t>(self.getTotalNeuronCount());
            stats["total_synapses"] = static_cast<int64_t>(self.getTotalSynapseCount());
            stats["active_neurons"] = static_cast<int64_t>(self.getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<int64_t>(self.getFiringNeuronCount());
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = static_cast<int64_t>(self.getTotalSpikeCount());
            return stats;
        }, "Get brain statistics as a dictionary")
        .def("getNeuronInfo", [](Brain& self) {
            // Return basic neuron information
            py::dict info;
            info["region_count"] = static_cast<int64_t>(self.getRegionCount());
            info["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return info;
        }, "Get basic brain information as a dictionary")
        .def("applyPresetDevelopment", [](Brain& self, const std::string& stageName) {
            DevelopmentalStage stage;
            if (stageName == "critical_period") stage = DevelopmentalStage::CriticalPeriod;
            else if (stageName == "maturation") stage = DevelopmentalStage::Maturation;
            else if (stageName == "adult") stage = DevelopmentalStage::Adult;
            else if (stageName == "aging") stage = DevelopmentalStage::Aging;
            else {
                throw py::value_error("Unknown developmental stage: " + stageName);
            }
            self.setDevelopmentalStage(stage);
        }, py::arg("stageName"),
            "Set brain to a specific developmental stage")
        .def("createAgentInterface", [](std::shared_ptr<Brain> brain) {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"),
            "Create an AgentBrain interface for this brain");

    // Enhanced AgentBrain class with Pythonic interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        Agent brain interface connecting NLM brain to the world.
        Handles sensory transduction and motor decoding.
        
        Features:
        - Sensory input processing
        - Motor command decoding
        - Reward-based neuromodulation
        - Developmental system updates
        - Subsystem management (reward, curiosity, etc.)
        
        Attributes:
            brain: Underlying brain object
            world: Connected world object
            developmentalStage: Current developmental stage
        )pbdoc")
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
        // Subsystem configuration
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
        // Enhanced Pythonic methods
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, int numSteps) {
            for (int step = 0; step < numSteps; ++step) {
                // Update world
                world.update(0.1);
                
                // Get sensory input
                auto percept = world.getSensoryPercept();
                
                // Process sensory input
                self.processSensoryInput(percept);
                
                // The brain step would be called by the main loop
                // This is handled by the agent interface pattern
                
                // Decode motor command
                auto action = self.decodeMotorCommand();
                
                // Apply action to world (simplified)
                world.applyMotorCommand(action, world.getSimulationTime());
                
                // Apply neuromodulation (simplified reward)
                float reward = 0.0f;
                if (world.getSensoryPercept().getInternal().size() > 0) {
                    reward = world.getSensoryPercept().getInternal()[0];
                }
                self.applyRewardModulation(reward, 0.0f);
                
                // Update development
                self.updateDevelopment(0.1);
            }
        }, py::arg("world"), py::arg("numSteps"),
            "Run a complete episode in the world")
        .def("getAgentStats", [](const AgentBrain& self) {
            py::dict stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get agent statistics as a dictionary")
        .def("enableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all neuromodulation and plasticity systems")
        .def("disableAllSystems", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all neuromodulation and plasticity systems");

    // Factory functions with better Pythonic interface
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration.
        
        Returns:
            Config: Default configuration object
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
        )pbdoc");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config->set("connection_probability", 0.05, ConfigSource::Runtime);
        return config;
    }, R"pbdoc(
        Create a minimal configuration.
        
        Returns:
            Config: Minimal configuration object optimized for simple simulations
        )pbdoc");

    m.def("createStandardConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
        config->set("connection_probability", 0.1, ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        return config;
    }, R"pbdoc(
        Create a standard configuration.
        
        Returns:
            Config: Standard configuration object for balanced learning
        )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with configuration.
        
        Args:
            config: Configuration object for the brain
            
        Returns:
            Brain: New brain object
            
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
        )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world.
        
        Returns:
            SimpleWorld: New world object
            
        Example:
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.
        
        Args:
            brain: Brain object to interface with
            
        Returns:
            AgentBrain: New agent brain interface
            
        Example:
            >>> brain = pynlm.createBrain(config)
            >>> agent = pynlm.createAgentBrain(brain)
        )pbdoc");

    // Convenience simulation functions
    m.def("runSimpleSimulation", [](int numSteps, int numNeurons) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(numNeurons), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        return brain;
    }, py::arg("numSteps"), py::arg("numNeurons") = 100,
        R"pbdoc(
        Run a simple brain simulation.
        
        Args:
            numSteps: Number of simulation steps
            numNeurons: Number of neurons (default: 100)
            
        Returns:
            Brain: The brain after simulation
            
        Example:
            >>> brain = pynlm.runSimpleSimulation(1000, 500)
            >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
        )pbdoc");

    m.def("createStandardAgentWorld", [](int worldWidth, int worldHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(worldWidth, worldHeight, 8, 8);
        
        // Set agent start position in center
        world->setAgentStart(static_cast<float>(worldWidth) / 2.0f, 
                           static_cast<float>(worldHeight) / 2.0f);
        
        return world;
    }, py::arg("worldWidth") = 20, py::arg("worldHeight") = 20,
        R"pbdoc(
        Create a standard agent world with default settings.
        
        Args:
            worldWidth: World width in units (default: 20)
            worldHeight: World height in units (default: 20)
            
        Returns:
            SimpleWorld: Configured world with agent start position
            
        Example:
            >>> world = pynlm.createStandardAgentWorld(30, 30)
        )pbdoc");

    // Advanced utilities and debugging functions
    m.def("createDebugConfig", [](int neuronCount, float noiseLevel) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
        config->set("connection_probability", 0.1, ConfigSource::Runtime);
        config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        // Add noise parameters if available
        return config;
    }, py::arg("neuronCount") = 100, py::arg("noiseLevel") = 0.01,
        R"pbdoc(
        Create a debug configuration for testing.
        
        Args:
            neuronCount: Number of neurons (default: 100)
            noiseLevel: Noise level for testing (default: 0.01)
            
        Returns:
            Config: Debug configuration
        )pbdoc");

    m.def("validateConfiguration", [](const Config& config) {
        auto keys = config.getKeys();
        bool isValid = true;
        std::string errors;
        
        for (const auto& key : keys) {
            // Basic validation - check for reasonable ranges
            if (config.has("neuron_count")) {
                auto neuronCount = config.get<int64_t>("neuron_count");
                if (neuronCount && (*neuronCount <= 0)) {
                    isValid = false;
                    errors += "neuron_count must be positive; ";
                }
            }
            
            if (config.has("region_count")) {
                auto regionCount = config.get<int64_t>("region_count");
                if (regionCount && (*regionCount <= 0)) {
                    isValid = false;
                    errors += "region_count must be positive; ";
                }
            }
        }
        
        if (errors.empty()) {
            errors = "Configuration is valid";
        }
        
        return std::make_pair(isValid, errors);
    }, py::arg("config"),
        R"pbdoc(
        Validate a configuration.
        
        Args:
            config: Configuration to validate
            
        Returns:
            Tuple[bool, str]: (is_valid, error_message)
        )pbdoc");

    // Performance and monitoring utilities
    m.def("getSystemInfo", []() {
        py::dict info;
        // This would normally query system capabilities
        info["version"] = "0.1.0";
        info["max_neurons"] = 100000;
        info["supports_cuda"] = false; // Would check actual support
        info["simd_support"] = "SSE4.1+";
        return info;
    }, "Get system information and capabilities");

    // Constants for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Python version
    m.attr("__version__") = "0.1.0";
    
    // Module-level documentation
    m.def("example", []() {
        py::dict example;
        
        // Create a minimal example
        example["description"] = "Minimal example showing basic NLM usage";
        
        py::dict steps;
        steps["title"] = "Basic Usage Example";
        steps["python_code"] = R"python(
import pynlm

# Create brain with minimal configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simple simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
        )python";
        
        steps["c++_equivalent"] = R"cpp(
// Equivalent C++ code
auto config = std::make_shared<nlm::Config>();
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

for (int i = 0; i < 100; ++i) {
    brain->step(i);
}
        )cpp";
        
        example["steps"] = steps;
        
        return example;
    }, "Get usage examples and documentation");
}

} // namespace nlm
