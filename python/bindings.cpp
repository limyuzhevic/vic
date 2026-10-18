// Enhanced Python bindings for NLM with improved API design
// Provides both simple API for beginners and advanced features for experts
#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/utilities/ErrorHandling.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced Python bindings with better API design
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides both simple beginner-friendly API and advanced features for experts.
        
        Key Features:
        - Simple API for beginners
        - Advanced API for experts
        - Enhanced error handling
        - Type checking and validation
        - Performance monitoring
        - Advanced configuration options
        - Memory management utilities
        
        Usage:
        ```python
        import pynlm
        
        # Simple API (beginner)
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Advanced API (expert)
        config = pynlm.createAdvancedConfig()
        brain = pynlm.createBrain(config)
        ```
    )pbdoc";

    // Register basic exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<nlm::NLMError>(m, "NLMError");
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::InitializationError>(m, "InitializationError");
    py::register_exception<nlm::SimulationError>(m, "SimulationError");
    
    // Version information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Neural Learning Machine - Experimental Artificial Brain";

    // Type aliases for Python convenience
    m.attr("NeuronId") = py::type<>(NeuronId);
    m.attr("SynapseId") = py::type<>(SynapseId);
    m.attr("RegionId") = py::type<>(RegionId);
    m.attr("PopulationId") = py::type<>(PopulationId);
    m.attr("SimulationStep") = py::type<>(SimulationStep);
    m.attr("Timestamp") = py::type<>(Timestamp);

    // Enhanced NeuronType enum with additional methods
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_sensory", [](NeuronType type) {
            return type == NeuronType::Sensory;
        }, R"pbdoc(Check if neuron type is sensory)pbdoc")
        .def("is_motor", [](NeuronType type) {
            return type == NeuronType::Motor;
        }, R"pbdoc(Check if neuron type is motor)pbdoc")
        .def("is_excitatory", [](NeuronType type) {
            return type == NeuronType::Excitatory;
        }, R"pbdoc(Check if neuron type is excitatory)pbdoc")
        .def("is_inhibitory", [](NeuronType type) {
            return type == NeuronType::Inhibitory;
        }, R"pbdoc(Check if neuron type is inhibitory)pbdoc");

    // Enhanced SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_excitatory", [](SynapseType type) {
            return type == SynapseType::Excitatory;
        }, R"pbdoc(Check if synapse type is excitatory)pbdoc")
        .def("is_inhibitory", [](SynapseType type) {
            return type == SynapseType::Inhibitory;
        }, R"pbdoc(Check if synapse type is inhibitory)pbdoc");

    // Enhanced DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_developing", [](DevelopmentalStage stage) {
            return stage < DevelopmentalStage::Adult;
        }, R"pbdoc(Check if stage is developing)pbdoc")
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage >= DevelopmentalStage::Adult;
        }, R"pbdoc(Check if stage is mature)pbdoc");

    // Enhanced ActionType enum with more comprehensive options
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Comprehensive action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("LookLeft", ActionType::LookLeft)
        .value("LookRight", ActionType::LookRight)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values()
        .def("is_movement", [](ActionType type) {
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft || type == ActionType::MoveRight;
        }, R"pbdoc(Check if action is a movement action)pbdoc")
        .def("is_interaction", [](ActionType type) {
            return type == ActionType::Interact || type == ActionType::Eat || type == ActionType::Drink;
        }, R"pbdoc(Check if action is an interaction action)pbdoc");

    // Enhanced MotorCommand enum
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
        .def("requires_movement", [](MotorCommand cmd) {
            return cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward ||
                   cmd == MotorCommand::TurnLeft || cmd == MotorCommand::TurnRight;
        }, R"pbdoc(Check if motor command requires movement)pbdoc")
        .def("is_exploratory", [](MotorCommand cmd) {
            return cmd == MotorCommand::MoveLeft || cmd == MotorCommand::MoveRight ||
                   cmd == MotorCommand::LookLeft || cmd == MotorCommand::LookRight;
        }, R"pbdoc(Check if motor command is exploratory)pbdoc");

    // Enhanced WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("is_passable", [](WorldObjectType type) {
            return type == WorldObjectType::Empty || type == WorldObjectType::Resource || type == WorldObjectType::Marker;
        }, R"pbdoc(Check if world object is passable)pbdoc")
        .def("is_hazard", [](WorldObjectType type) {
            return type == WorldObjectType::Hazard;
        }, R"pbdoc(Check if world object is hazardous)pbdoc");

    // Enhanced Config class with better API
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)")pbdoc")
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
        .def("get", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw ConfigurationError("Key not found: " + key);
            }
            return self.get<std::string>(key);
        }, py::arg("key"),
             "Get configuration value as string (throws if not found)")
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value as string with default")
        .def("getInt", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw ConfigurationError("Key not found: " + key);
            }
            return self.getOr<int64_t>(key, 0);
        }, py::arg("key"),
             "Get configuration value as int (throws if not found)")
        .def("getInt", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value as int with default")
        .def("getFloat", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw ConfigurationError("Key not found: " + key);
            }
            return self.getOr<float>(key, 0.0f);
        }, py::arg("key"),
             "Get configuration value as float (throws if not found)")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value as float with default")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (int)")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::User);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (string)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &Config::validate,
             "Validate configuration (returns error message or empty string if valid)")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput with more methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("is_valid", [](const SensoryInput& self) {
            return self.getTimestamp() > 0.0;
        }, "Check if sensory input is valid");

    // Enhanced Vision with more functionality
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Enhanced vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataGrid", [](Vision& self, const std::vector<std::vector<float>>& grid) {
            // Convert 2D grid to 1D data
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            std::vector<float> data(width * height);
            size_t idx = 0;
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    data[idx++] = grid[y][x];
                }
            }
            self.setData(data);
        }, py::arg("grid"),
             "Set vision data from 2D grid (useful for image-like input)")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getArray", [](const Vision& self) {
            return py::array(self.getData());
        }, "Get vision data as NumPy array")
        .def("normalize", [](Vision& self, float min, float max) {
            auto& data = self.getData();
            for (float& value : data) {
                value = (value - min) / (max - min);
            }
        }, py::arg("min"), py::arg("max"),
             "Normalize vision data to [0, 1] range");

    // Enhanced Audio with more features
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Enhanced audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getFrequencySpectrum", [](const Audio& self) {
            // Simple FFT stub for demonstration
            size_t n = self.getNumSamples();
            std::vector<float> spectrum(n / 2);
            return py::array(spectrum);
        }, "Get frequency spectrum (placeholder)")
        .def("to_numpy", [](const Audio& self) {
            return py::array(self.getData());
        }, "Convert audio data to NumPy array");

    // Enhanced InternalSignals with more methods
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("addSignal", [](InternalSignals& self, double value) {
            self.addSignal(static_cast<float>(value));
        }, py::arg("value"), "Add internal signal (accepts float)")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of internal signals")
        .def("getAverage", [](const InternalSignals& self) {
            auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = 0.0f;
            for (float value : data) sum += value;
            return sum / data.size();
        }, "Get average of internal signals");

    // Enhanced Action with more functionality
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"),
             "Add a parameter to the action")
        .def("getParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            } else {
                throw std::out_of_range("Parameter index out of range");
            }
        }, py::arg("index"),
             "Get parameter by index (throws if out of range)")
        .def("hasParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            return std::find(params.begin(), params.end(), value) != params.end();
        }, py::arg("value"),
             "Check if action has parameter with given value");

    // Enhanced WorldObject with more features
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Enhanced world object representation)pbdoc")
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
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to point (x, y)")
        .def("contains", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= self.radius * self.radius;
        }, py::arg("x"), py::arg("y"),
             "Check if point (x, y) is inside this object")
        .def("is_resource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("is_hazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("is_wall", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        .def("get_color", [](const WorldObject& self) {
            // Return RGB color based on type
            if (self.type == WorldObjectType::Resource) return py::make_tuple(0.0, 1.0, 0.0);  // Green
            if (self.type == WorldObjectType::Hazard) return py::make_tuple(1.0, 0.0, 0.0);  // Red
            if (self.type == WorldObjectType::Wall) return py::make_tuple(0.5, 0.5, 0.5);  // Gray
            return py::make_tuple(0.0, 0.0, 0.0);  // Black (empty)
        }, "Get RGB color tuple based on object type");

    // Enhanced AgentBody with more properties
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Enhanced agent body state)pbdoc")
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
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
             "Set agent position")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
            self.isMoving = (vx != 0.0f || vy != 0.0f);
        }, py::arg("vx"), py::arg("vy"),
             "Set agent velocity and update moving state")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getDirection", [](const AgentBody& self) {
            float angle = std::atan2(self.velocityY, self.velocityX);
            return angle * 180.0f / M_PI;  // Convert to degrees
        }, "Get current direction in degrees")
        .def("is_low_energy", [](const AgentBody& self) {
            return self.energy < 0.3f;
        }, "Check if agent has low energy")
        .def("is_injured", [](const AgentBody& self) {
            return self.health < 0.5f;
        }, "Check if agent is injured");

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Enhanced action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_positive_reward", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("is_negative_reward", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if reward is negative")
        .def("get_satisfaction", [](const ActionResult& self) {
            return self.success ? self.reward : -std::abs(self.reward);
        }, "Get satisfaction score (reward for success, negative for failure)");

    // Enhanced SensoryPercept with more methods
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Enhanced sensory percept data)pbdoc")
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
        .def("add_signal", [](SensoryPercept& self, float value) {
            self.getInternal().push_back(value);
        }, py::arg("value"),
             "Add a signal to internal sensory data (convenience method)")
        .def("get_signal_count", [](const SensoryPercept& self) {
            return self.getInternal().size();
        }, "Get number of signals in percept")
        .def("get_all_as_numpy", [](const SensoryPercept& self) {
            // Return all signals as a dictionary
            py::dict result;
            result["vision"] = py::array(self.getVision());
            result["touch"] = py::array(self.getTouch());
            result["internal"] = py::array(self.getInternal());
            result["proprioception"] = py::array(self.getProprioception());
            result["audio"] = py::array(self.getAudio());
            return result;
        }, "Get all sensory data as NumPy arrays in a dictionary");

    // Enhanced SimpleWorld with advanced features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor properties")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state")
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
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
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
        .def("get_stats", [](const SimpleWorld& self) {
            py::dict stats;
            stats["width"] = self.getWidth();
            stats["height"] = self.getHeight();
            stats["simulation_time"] = self.getSimulationTime();
            stats["max_energy"] = self.getMaxEnergy();
            return stats;
        }, "Get world statistics as dictionary");

    // Enhanced Brain with advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)pbdoc")
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
        .def("is_initialized", [](const Brain& self) {
            // Brain doesn't have an explicit initialized flag, but we can check if it has regions
            return self.getRegionCount() > 0;
        }, "Check if brain is initialized")
        .def("get_neuron_stats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            return stats;
        }, "Get comprehensive brain statistics");

    // Enhanced AgentBrain with advanced features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)")pbdoc")
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
        .def("get_advanced_stats", [](const AgentBrain& self) {
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
        }, "Get comprehensive agent brain statistics");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with expert settings");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set minimal configuration for fast testing
        config->set("brain.neuron_count", 100, ConfigSource::User);
        config->set("brain.v_thresh", -50.0f, ConfigSource::User);
        config->set("brain.v_rest", -70.0f, ConfigSource::User);
        config->set("brain.tau_mem", 20.0f, ConfigSource::User);
        config->set("brain.connection_probability", 0.05f, ConfigSource::User);
        return config;
    }, "Create a minimal configuration for fast testing");

    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Enable all advanced features
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        config->set("plasticity.structural.enable", true, ConfigSource::User);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        config->set("neuromod.novelty.enable", true, ConfigSource::User);
        config->set("development.enabled", true, ConfigSource::User);
        return config;
    }, "Create an advanced configuration with all features enabled");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with advanced features");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Utility functions
    m.def("createDefaultVision", [](size_t width, size_t height, size_t channels = 3) -> std::shared_ptr<Vision> {
        auto vision = std::make_shared<Vision>(width, height, channels);
        // Initialize with zeros
        std::vector<float> data(width * height * channels, 0.0f);
        vision->setData(data);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       "Create a default Vision object with zero-initialized data");

    m.def("createAction", [](ActionType type, const std::vector<float>& parameters) -> std::shared_ptr<Action> {
        auto action = std::make_shared<Action>(type, parameters);
        return action;
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
       "Create a new Action object");

    // Error handling utilities
    m.def("create_nlm_error", [](const std::string& message) {
        throw nlm::NLMError(message);
    }, py::arg("message"),
         "Create and throw an NLMError (for testing error handling)");

    // Performance utilities
    m.def("get_version_info", []() {
        py::dict info;
        info["version"] = nlm::Version::STRING;
        info["full"] = nlm::Version::FULL;
        info["major"] = nlm::Version::MAJOR;
        info["minor"] = nlm::Version::MINOR;
        info["patch"] = nlm::Version::PATCH;
        return info;
    }, "Get version information");

    m.def("get_platform_info", []() {
        py::dict info;
        info["platform"] = nlm::Platform::getPlatformString();
        info["windows"] = nlm::Platform::WINDOWS;
        info["linux"] = nlm::Platform::LINUX;
        info["macos"] = nlm::Platform::MACOS;
        info["cpu_count"] = static_cast<size_t>(nlm::ThreadUtils::getCPUCount());
        return info;
    }, "Get platform and system information");

    // Advanced simulation utilities
    m.def("run_simulation_loop", [](std::shared_ptr<Brain> brain, 
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t num_steps,
                                   double timestep) {
        if (!brain || !world || !agent) {
            throw nlm::InitializationError("Invalid brain, world, or agent for simulation");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(static_cast<SimulationStep>(step));
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("num_steps"), py::arg("timestep") = 0.1,
       "Run a complete simulation loop with error checking");

    // Batch processing utilities
    m.def("batch_process_sensory", [](const std::vector<std::shared_ptr<SensoryInput>>& inputs) {
        std::vector<float> combined;
        size_t max_size = 0;
        
        for (const auto& input : inputs) {
            if (input) {
                max_size = std::max(max_size, input->getDimensions());
            }
        }
        
        combined.resize(max_size, 0.0f);
        
        for (size_t i = 0; i < inputs.size(); ++i) {
            if (inputs[i]) {
                const auto& data = inputs[i]->getData();
                for (size_t j = 0; j < std::min(data.size(), combined.size()); ++j) {
                    combined[j] += data[j];
                }
            }
        }
        
        return combined;
    }, py::arg("inputs"),
       "Batch process multiple sensory inputs by averaging");

    // Example setups for different use cases
    m.def("create_agent_learning_environment", []() {
        py::dict env;
        
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500, ConfigSource::User);
        config->set("plasticity.stdp.enable", true, ConfigSource::User);
        config->set("plasticity.hebbian.enable", true, ConfigSource::User);
        config->set("neuromod.curiosity.enable", true, ConfigSource::User);
        config->set("development.enabled", true, ConfigSource::User);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        env["brain"] = brain;
        env["world"] = world;
        env["agent"] = agent;
        
        return env;
    }, "Create a complete agent learning environment with all features enabled");

    m.def("create_test_environment", []() {
        py::dict env;
        
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100, ConfigSource::User);
        config->set("brain.connection_probability", 0.02f, ConfigSource::User);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 4, 4);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        env["brain"] = brain;
        env["world"] = world;
        env["agent"] = agent;
        
        return env;
    }, "Create a minimal test environment for quick testing");

    // Sentinel values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Constants for expert users
    py::module_ ref_module = py::module_::import("math");
    m.def("get_constants", [ref_module]() {
        py::dict constants;
        constants["pi"] = M_PI;
        constants["e"] = M_E;
        constants["golden_ratio"] = (1.0 + std::sqrt(5.0)) / 2.0;
        constants["speed_of_light"] = 299792458.0; // m/s
        constants["gravitational_constant"] = 6.67430e-11;
        return constants;
    }, "Get mathematical and physical constants");

    // Advanced configuration presets
    m.def("get_config_presets", []() {
        py::dict presets;
        
        // Fast preset
        auto fast_preset = std::make_shared<Config>();
        fast_preset->set("brain.neuron_count", 50, ConfigSource::User);
        fast_preset->set("brain.connection_probability", 0.01f, ConfigSource::User);
        presets["fast"] = fast_preset;
        
        // Balanced preset
        auto balanced_preset = std::make_shared<Config>();
        balanced_preset->set("brain.neuron_count", 500, ConfigSource::User);
        balanced_preset->set("brain.connection_probability", 0.05f, ConfigSource::User);
        balanced_preset->set("plasticity.stdp.enable", true, ConfigSource::User);
        balanced_preset->set("neuromod.curiosity.enable", true, ConfigSource::User);
        presets["balanced"] = balanced_preset;
        
        // High-performance preset
        auto performance_preset = std::make_shared<Config>();
        performance_preset->set("brain.neuron_count", 2000, ConfigSource::User);
        performance_preset->set("brain.connection_probability", 0.08f, ConfigSource::User);
        performance_preset->set("plasticity.stdp.enable", true, ConfigSource::User);
        performance_preset->set("plasticity.hebbian.enable", true, ConfigSource::User);
        performance_preset->set("plasticity.structural.enable", true, ConfigSource::User);
        performance_preset->set("neuromod.curiosity.enable", true, ConfigSource::User);
        performance_preset->set("neuromod.novelty.enable", true, ConfigSource::User);
        performance_preset->set("development.enabled", true, ConfigSource::User);
        presets["high_performance"] = performance_preset;
        
        return presets;
    }, "Get predefined configuration presets");

    // Error classification helper
    m.def("classify_error", [](const std::string& error_message) {
        std::string message = error_message;
        std::transform(message.begin(), message.end(), message.begin(), ::tolower);
        
        if (message.find("configuration") != std::string::npos ||
            message.find("config") != std::string::npos) {
            return "configuration";
        } else if (message.find("initialization") != std::string::npos) {
            return "initialization";
        } else if (message.find("simulation") != std::string::npos) {
            return "simulation";
        } else if (message.find("memory") != std::string::npos) {
            return "memory";
        } else {
            return "general";
        }
    }, py::arg("error_message"),
       "Classify error message into categories");

    // Performance profiling helper
    m.def("profile_performance", []() {
        py::dict profile;
        profile["cpu_count"] = static_cast<size_t>(nlm::ThreadUtils::getCPUCount());
        profile["is_multicore"] = nlm::ThreadUtils::isMultiCore();
        profile["platform"] = nlm::Platform::getPlatformString();
        profile["debug_build"] = nlm::Build::DEBUG;
        profile["assertions_enabled"] = nlm::Build::ASSERTIONS;
        profile["profiling_enabled"] = nlm::Build::PROFILING;
        profile["exceptions_enabled"] = nlm::Build::ENABLE_EXCEPTIONS;
        return profile;
    }, "Get performance profiling information");

    // Memory usage helper
    m.def("get_memory_usage", []() {
        py::dict memory;
        memory["estimated_max"] = 1024 * 1024 * 1024; // 1GB estimate
        memory["recommended_min"] = 256 * 1024 * 1024; // 256MB minimum
        memory["platform_specific"] = nlm::Platform::WINDOWS ? "windows" : 
                                      nlm::Platform::LINUX ? "linux" : "macos";
        return memory;
    }, "Get memory usage estimates and recommendations");

    // Tutorial and example helpers
    m.def("get_tutorials", []() {
        std::vector<std::string> tutorials = {
            "basic_brain_creation.py",
            "agent_world_simulation.py",
            "learning_environments.py",
            "advanced_plasticity.py",
            "neuromodulation_basics.py"
        };
        return tutorials;
    }, "Get list of available tutorial examples");

    // Quick start guide
    m.def("quick_start_example", []() {
        return R"python(
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a world
world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
)python";
    }, "Get a quick start example for beginners");

} // namespace nlm
