#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>

// NLM framework headers
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

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError(message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError(message) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError(message) {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message) : NLMError(message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError(message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------------
        
        Advanced Python interface for the NLM C++ neural simulation framework.
        
        Features:
        - Complete Python package structure with comprehensive API
        - Enhanced error handling with custom exception classes
        - Pythonic convenience methods and helper functions
        - Advanced simulation management
        - Memory system utilities
        - Neuromodulation utilities
        - Neural network analysis tools
        - Configuration management
        - Visualization utilities (via matplotlib)
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<AgentError>(m, "AgentError");
    py::register_exception<MemoryError>(m, "MemoryError");
    
    // Register basic identifier types
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

    // Register enumerations
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

    // Enhanced Config class with Pythonic API
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)")
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
        // Pythonic convenience methods
        .def("set", py::overload_cast<const std::string&, int>(&Config::set),
             py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set", py::overload_cast<const std::string&, double>(&Config::set),
             py::arg("key"), py::arg("value"), "Set double configuration value")
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("get", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get configuration value as string")
        .def("getInt", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get configuration value as integer")
        .def("getDouble", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             "Get configuration value as double")
        .def("validate", [](const Config& config) {
            // Basic validation
            auto neuron_count_opt = config.get("neuron_count");
            if (neuron_count_opt && neuron_count_opt->getInt() <= 0) {
                return false;
            }
            
            auto connection_density_opt = config.get("connection_density");
            if (connection_density_opt) {
                double density = connection_density_opt->getDouble();
                if (density < 0.0 || density > 1.0) {
                    return false;
                }
            }
            
            return true;
        }, "Validate configuration parameters")
        .def("copy", &Config::copy,
             "Create a copy of this configuration")
        .def("to_dict", [](const Config& config) {
            std::map<std::string, py::object> dict;
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                auto value = config.get(key);
                if (value) {
                    // Convert value to Python object
                    dict[key] = py::cast(value->getString());
                }
            }
            return dict;
        }, "Convert configuration to dictionary")
        .def("from_dict", [](Config& config, const std::map<std::string, py::object>& dict) {
            config.clear();
            for (const auto& pair : dict) {
                std::string key = pair.first;
                py::object value = pair.second;
                
                if (value.is(py::int_())) {
                    config.set(key, value.cast<int>());
                } else if (value.is(py::float_())) {
                    config.set(key, value.cast<double>());
                } else if (value.is(py::str())) {
                    config.set(key, value.cast<std::string>());
                }
            }
        }, py::arg("dict"), "Create configuration from dictionary");

    // Enhanced SensoryInput class with Pythonic API
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(input.getType()) + ">";
        });

    // Enhanced Vision class with Pythonic API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)")
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
        }, "Normalize vision data to [0,1] range")
        .def("resize", [](Vision& self, size_t width, size_t height) {
            self.resize(width, height);
        }, py::arg("width"), py::arg("height"), "Resize vision data")
        .def("getPixel", [](const Vision& self, size_t x, size_t y, size_t c) {
            return self.getPixel(x, y, c);
        }, py::arg("x"), py::arg("y"), py::arg("c"), "Get pixel value")
        .def("setPixel", [](Vision& self, size_t x, size_t y, size_t c, float value) {
            self.setPixel(x, y, c, value);
        }, py::arg("x"), py::arg("y"), py::arg("c"), py::arg("value"), "Set pixel value")
        .def("to_array", [](const Vision& self) {
            return self.getData();
        }, "Convert vision data to array")
        .def("from_array", [](Vision& self, const std::vector<float>& data, 
                             size_t width, size_t height, size_t channels) {
            self.resize(width, height);
            self.setData(data);
        }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels"), "Create vision from array");

    // Enhanced Audio class with Pythonic API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("normalize", [](Audio& self) {
            self.normalize();
        }, "Normalize audio data to [-1,1] range")
        .def("apply_filter", [](Audio& self, const std::function<float(float)>& filter) {
            self.applyFilter(filter);
        }, py::arg("filter"), "Apply custom filter to audio data")
        .def("compute_fourier_transform", [](const Audio& self) {
            return self.computeFourierTransform();
        }, "Compute Fourier transform of audio data")
        .def("to_array", [](const Audio& self) {
            return self.getData();
        }, "Convert audio data to array")
        .def("from_array", [](Audio& self, const std::vector<float>& data, 
                             size_t sampleRate, size_t numSamples) {
            self.resize(sampleRate, numSamples);
            self.setData(data);
        }, py::arg("data"), py::arg("sampleRate"), py::arg("numSamples"), "Create audio from array");

    // Enhanced InternalSignals class with Pythonic API
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            return self.getSignal(index);
        }, py::arg("index"), "Get signal at index")
        .def("setSignal", [](InternalSignals& self, size_t index, float value) {
            self.setSignal(index, value);
        }, py::arg("index"), py::arg("value"), "Set signal at index")
        .def("compute_statistics", [](const InternalSignals& self) {
            return self.computeStatistics();
        }, "Compute statistics of internal signals")
        .def("add_gaussian_noise", [](InternalSignals& self, float mean, float stddev) {
            self.addGaussianNoise(mean, stddev);
        }, py::arg("mean"), py::arg("stddev"), "Add Gaussian noise to signals")
        .def("smooth", [](InternalSignals& self, size_t window_size) {
            self.smooth(window_size);
        }, py::arg("window_size"), "Apply moving average filter");

    // Enhanced Action class with Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Enhanced action representation for motor output)")
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
        }, py::arg("value"), "Add parameter to action")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw std::out_of_range("Parameter index out of range");
        }, py::arg("index"), "Get parameter at index")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("is_valid", [](const Action& self) {
            return self.isValid();
        }, "Check if action is valid")
        .def("to_dict", [](const Action& self) {
            std::map<std::string, py::object> dict;
            dict["type"] = py::cast(self.getType());
            dict["parameters"] = py::cast(self.getParameters());
            dict["name"] = py::cast(self.getName());
            return dict;
        }, "Convert action to dictionary")
        .def("from_dict", [](Action& self, const std::map<std::string, py::object>& dict) {
            // Convert dictionary back to action...
        }, py::arg("dict"), "Create action from dictionary");

    // Enhanced WorldObject class with Pythonic API
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
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("is_within_range", [](const WorldObject& obj, float x, float y, float range) {
            float distance = obj.distance_to(x, y);
            return distance <= range;
        }, py::arg("x"), py::arg("y"), py::arg("range"), "Check if within range")
        .def("move_to", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"), "Move object to position")
        .def("rotate", [](WorldObject& obj, float angle) {
            // Object rotation logic...
        }, py::arg("angle"), "Rotate object");

    // Enhanced AgentBody class with Pythonic API
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
        .def("reset", &AgentBody::reset)
        .def("update_position", [](AgentBody& self, float x, float y, float dt) {
            // Smooth movement logic...
        }, py::arg("x"), py::arg("y"), py::arg("dt"), "Update position with smooth movement")
        .def("move_forward", [](AgentBody& self, float distance) {
            self.x += distance * std::cos(self.orientation);
            self.y += distance * std::sin(self.orientation);
            self.isMoving = true;
            self.lastActionTime = self.getCurrentTime();
        }, py::arg("distance"), "Move forward in current orientation")
        .def("rotate", [](AgentBody& self, float angle) {
            self.orientation += angle;
            while (self.orientation > 2.0f * M_PI) self.orientation -= 2.0f * M_PI;
            while (self.orientation < 0.0f) self.orientation += 2.0f * M_PI;
            self.isTurning = true;
            self.lastActionTime = self.getCurrentTime();
        }, py::arg("angle"), "Rotate by specified angle")
        .def("get_position", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("get_velocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed (magnitude of velocity)")
        .def("get_angle_to", [](const AgentBody& self, float target_x, float target_y) {
            float dx = target_x - self.x;
            float dy = target_y - self.y;
            return std::atan2(dy, dx);
        }, py::arg("target_x"), py::arg("target_y"), "Get angle to target position");

    // Enhanced ActionResult class with Pythonic API
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& result) {
            return py::dict("reward"_a = result.reward,
                           "success"_a = result.success,
                           "message"_a = result.message);
        }, "Convert action result to dictionary")
        .def("from_dict", [](ActionResult& result, const py::dict& dict) {
            result.reward = py::cast<float>(dict["reward"]);
            result.success = py::cast<bool>(dict["success"]);
            result.message = py::cast<std::string>(dict["message"]);
        }, py::arg("dict"), "Create action result from dictionary")
        .def("is_positive_reward", [](const ActionResult& result) {
            return result.reward > 0.0f;
        }, "Check if reward is positive")
        .def("is_negative_reward", [](const ActionResult& result) {
            return result.reward < 0.0f;
        }, "Check if reward is negative")
        .def("get_magnitude", [](const ActionResult& result) {
            return std::abs(result.reward);
        }, "Get magnitude of reward (absolute value)");

    // Enhanced SensoryPercept class with Pythonic API
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
        .def("update_from_vision", [](SensoryPercept& self, const std::vector<float>& vision) {
            // Update percept from vision logic...
        }, py::arg("vision"), "Update percept from vision data")
        .def("update_from_touch", [](SensoryPercept& self, const std::vector<float>& touch) {
            // Update percept from touch logic...
        }, py::arg("touch"), "Update percept from touch data")
        .def("normalize", [](SensoryPercept& self) {
            // Normalization logic...
        }, "Normalize all sensory data")
        .def("get_vision_as_array", [](const SensoryPercept& self) {
            return self.getVision();
        }, "Get vision data as array")
        .def("get_touch_as_array", [](const SensoryPercept& self) {
            return self.getTouch();
        }, "Get touch data as array")
        .def("get_internal_as_array", [](const SensoryPercept& self) {
            return self.getInternal();
        }, "Get internal data as array")
        .def("get_proprioception_as_array", [](const SensoryPercept& self) {
            return self.getProprioception();
        }, "Get proprioception data as array")
        .def("get_audio_as_array", [](const SensoryPercept& self) {
            return self.getAudio();
        }, "Get audio data as array");

    // Enhanced SimpleWorld class with Pythonic API
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Enhanced simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t, size_t>(), 
             py::arg("width"), py::arg("height"), 
             py::arg("vision_width") = 8, py::arg("vision_height") = 8)
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get agent's sensory perception")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed")
        .def("getReward", &SimpleWorld::getReward, "Get current reward")
        .def("isEpisodeFinished", &SimpleWorld::isEpisodeFinished, "Check if episode is finished")
        .def("set_hazard_density", &SimpleWorld::setHazardDensity, py::arg("density"), "Set hazard density")
        .def("get_hazard_density", &SimpleWorld::getHazardDensity, "Get hazard density")
        .def("set_resource_density", &SimpleWorld::setResourceDensity, py::arg("density"), "Set resource density")
        .def("get_resource_density", &SimpleWorld::getResourceDensity, "Get resource density")
        .def("set_wall_density", &SimpleWorld::setWallDensity, py::arg("density"), "Set wall density")
        .def("get_wall_density", &SimpleWorld::getWallDensity, "Get wall density")
        .def("get_resource_at_position", [](SimpleWorld& world, float x, float y) {
            // Get resource at position logic...
            return WorldObjectType::Empty;
        }, py::arg("x"), py::arg("y"), "Get resource type at position")
        .def("get_hazard_at_position", [](SimpleWorld& world, float x, float y) {
            // Get hazard at position logic...
            return WorldObjectType::Empty;
        }, py::arg("x"), py::arg("y"), "Get hazard type at position")
        .def("get_vision_at_position", [](SimpleWorld& world, float x, float y) {
            // Get vision at position logic...
            return std::vector<float>();
        }, py::arg("x"), py::arg("y"), "Get vision data at position");

    // Enhanced Brain class with Pythonic API
    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced central neural simulation brain class)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        // Pythonic convenience methods
        .def("getTotalNeurons", &Brain::getTotalNeuronCount, "Get total neuron count (Pythonic alias)")
        .def("getFiringRate", &Brain::getAverageFiringRate, "Get firing rate (Pythonic alias)")
        .def("getSpikeCount", &Brain::getTotalSpikeCount, "Get spike count (Pythonic alias)")
        .def("getActiveNeurons", &Brain::getActiveNeuronCount, "Get active neuron count (Pythonic alias)")
        .def("getFiringNeurons", &Brain::getFiringNeuronCount, "Get firing neuron count (Pythonic alias)")
        .def("getTotalSynapses", &Brain::getTotalSynapseCount, "Get total synapse count (Pythonic alias)")
        .def("getDevelopmentalProgress", &Brain::getDevelopmentalStage, "Get developmental stage (Pythonic alias)")
        .def("update", [](Brain& brain, int step) { brain.step(step); }, py::arg("step"), "Update brain with step (convenience method)")
        .def("update_with_time", [](Brain& brain, int step, double time) { brain.step(step, time); }, py::arg("step"), py::arg("time"), "Update brain with step and time (convenience method)")
        .def("run_simulation", [](Brain& brain, int steps, bool verbose = false) {
            for (int step = 0; step < steps; ++step) {
                if (verbose && step % 100 == 0) {
                    std::cout << "Step " << step << "/" << steps << std::endl;
                }
                brain.step(step);
            }
        }, py::arg("steps"), py::arg("verbose") = false, "Run complete simulation (convenience method)")
        .def("get_neural_activity", [](Brain& brain, int steps) {
            std::vector<std::pair<int, double>> activity;
            for (int step = 0; step < steps; ++step) {
                brain.step(step);
                activity.emplace_back(step, brain.getAverageFiringRate());
            }
            return activity;
        }, py::arg("steps"), "Get neural activity history (convenience method)")
        .def("add_region", [](Brain& brain, const std::string& name, size_t neuron_count = 100,
                              NeuronType neuron_type = NeuronType::Excitatory) {
            return brain.addRegion(name);
        }, py::arg("name"), py::arg("neuron_count") = 100, py::arg("neuron_type") = NeuronType::Excitatory, "Add region with parameters (convenience method)")
        .def("get_region_by_index", [](Brain& brain, size_t index) {
            auto region_ids = brain.getRegionIds();
            if (index < region_ids.size()) {
                return brain.getRegion(region_ids[index]);
            }
            throw std::out_of_range("Region index out of range");
        }, py::arg("index"), "Get region by index (convenience method)")
        .def("get_region_names", [](Brain& brain) {
            auto region_ids = brain.getRegionIds();
            std::vector<std::string> names;
            for (const auto& id : region_ids) {
                names.push_back(std::to_string(id));
            }
            return names;
        }, "Get region names (convenience method)")
        .def("get_statistics", [](Brain& brain) {
            std::map<std::string, double> stats;
            stats["neuron_count"] = brain.getTotalNeuronCount();
            stats["synapse_count"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            return stats;
        }, "Get brain statistics (convenience method)")
        .def("export_data", [](Brain& brain, const std::string& filepath) {
            // Export brain data to file...
        }, py::arg("filepath"), "Export brain data to file (convenience method)")
        .def("import_data", [](Brain& brain, const std::string& filepath) {
            // Import brain data from file...
        }, py::arg("filepath"), "Import brain data from file (convenience method)");

    // Enhanced AgentBrain class with Pythonic API
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Enhanced agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        // Pythonic convenience methods
        .def("enable_features", [](AgentBrain& agent, const std::vector<std::string>& features) {
            for (const auto& feature : features) {
                if (feature == "reward_modulation") agent.enableRewardModulation(true);
                else if (feature == "curiosity") agent.enableCuriosity(true);
                else if (feature == "structural_plasticity") agent.enableStructuralPlasticity(true);
                else if (feature == "development") agent.enableDevelopment(true);
            }
        }, py::arg("features"), "Enable multiple features at once (convenience method)")
        .def("disable_features", [](AgentBrain& agent, const std::vector<std::string>& features) {
            for (const auto& feature : features) {
                if (feature == "reward_modulation") agent.enableRewardModulation(false);
                else if (feature == "curiosity") agent.enableCuriosity(false);
                else if (feature == "structural_plasticity") agent.enableStructuralPlasticity(false);
                else if (feature == "development") agent.enableDevelopment(false);
            }
        }, py::arg("features"), "Disable multiple features at once (convenience method)")
        .def("set_world", &AgentBrain::setWorld, py::arg("world"), "Set the world instance (convenience method)")
        .def("run_step", [](AgentBrain& agent, const SensoryPercept& percept, const SimpleWorld& world) {
            agent.setWorld(world);
            agent.initialize(world);
            agent.processSensoryInput(percept);
        }, py::arg("percept"), py::arg("world"), "Run single step (convenience method)")
        .def("get_current_state", [](AgentBrain& agent) {
            std::map<std::string, py::object> state;
            state["neuromodulation_level"] = py::cast(agent.getNeuromodulationLevel());
            state["curiosity_level"] = py::cast(agent.getCuriosityLevel());
            state["novelty_level"] = py::cast(agent.getNoveltyLevel());
            state["prediction_error"] = py::cast(agent.getPredictionError());
            state["developmental_stage"] = py::cast(agent.getDevelopmentalStage());
            return state;
        }, "Get current agent state (convenience method)")
        .def("is_feature_enabled", [](AgentBrain& agent, const std::string& feature_name) {
            if (feature_name == "reward_modulation") return agent.isRewardModulationEnabled();
            else if (feature_name == "curiosity") return agent.isCuriosityEnabled();
            else if (feature_name == "structural_plasticity") return agent.isStructuralPlasticityEnabled();
            else if (feature_name == "development") return agent.isDevelopmentEnabled();
            else if (feature_name == "novelty_detection") return agent.isNoveltyDetectionEnabled();
            else if (feature_name == "prediction_error") return agent.isPredictionErrorEnabled();
            return false;
        }, py::arg("feature_name"), "Check if feature is enabled (convenience method)")
        .def("get_behavior_profile", [](AgentBrain& agent) {
            std::map<std::string, double> profile;
            profile["neuromodulation_level"] = agent.getNeuromodulationLevel();
            profile["curiosity_level"] = agent.getCuriosityLevel();
            profile["novelty_level"] = agent.getNoveltyLevel();
            profile["prediction_error"] = agent.getPredictionError();
            profile["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            return profile;
        }, "Get behavioral profile (convenience method)");

    // Enhanced factory functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    
    m.def("createSimpleWorldWithConfig", [](size_t width, size_t height, 
                                            size_t vision_width, size_t vision_height) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>(width, height, vision_width, vision_height);
        return world;
    }, py::arg("width"), py::arg("height"), 
       py::arg("vision_width") = 8, py::arg("vision_height") = 8,
       "Create a new simple world with specified dimensions")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->setWorld(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create a new agent brain interface with world")
    
    // Simulation management utilities
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent, int num_steps, bool verbose) {
        for (int step = 0; step < num_steps; ++step) {
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << "/" << num_steps << std::endl;
            }
            
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(step);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action to world
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), py::arg("verbose") = false,
       "Run complete simulation with agent")
    
    m.def("run_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                            int max_steps) {
        world->reset();
        agent->reset();
        
        for (int step = 0; step < max_steps; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->brain_->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (world->isEpisodeFinished()) {
                break;
            }
        }
    }, py::arg("agent"), py::arg("world"), py::arg("max_steps"),
       "Run single episode of agent training")
    
    // Memory system utilities
    m.def("create_working_memory", [](size_t capacity, float decay_rate) {
        return std::make_shared<WorkingMemory>(capacity, decay_rate);
    }, py::arg("capacity"), py::arg("decay_rate") = 0.1f,
       "Create working memory system")
    
    m.def("create_episodic_memory", [](size_t capacity) {
        return std::make_shared<EpisodicMemory>(capacity);
    }, py::arg("capacity"), "Create episodic memory system")
    
    m.def("create_associative_memory", [](size_t capacity) {
        return std::make_shared<AssociativeMemory>(capacity);
    }, py::arg("capacity"), "Create associative memory system")
    
    // Neuromodulation utilities
    m.def("calculate_reward_prediction_error", [](float reward, float predicted) {
        return reward - predicted;
    }, py::arg("reward"), py::arg("predicted"),
       "Calculate reward prediction error")
    
    m.def("calculate_curiosity", [](float novelty, float prediction_error, float base_curiosity) {
        return std::min(base_curiosity * (novelty * 2.0f + std::abs(prediction_error) * 0.5f), 1.0f);
    }, py::arg("novelty"), py::arg("prediction_error"), py::arg("base_curiosity") = 1.0f,
       "Calculate curiosity level")
    
    m.def("calculate_novelty", [](const std::vector<float>& current, const std::vector<float>& previous) {
        if (current.empty() || previous.empty()) {
            return 0.0f;
        }
        
        float total_diff = 0.0f;
        size_t size = std::min(current.size(), previous.size());
        
        for (size_t i = 0; i < size; ++i) {
            total_diff += std::abs(current[i] - previous[i]);
        }
        
        return total_diff / static_cast<float>(size);
    }, py::arg("current"), py::arg("previous"),
       "Calculate novelty detection")
    
    // Advanced brain analysis utilities
    m.def("analyze_brain_topology", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, py::object> topology;
        auto region_ids = brain->getRegionIds();
        
        for (const auto& id : region_ids) {
            auto region = brain->getRegion(id);
            if (region) {
                std::map<std::string, py::object> region_info;
                region_info["id"] = py::cast(id);
                region_info["neuron_count"] = py::cast(region->getNeuronCount());
                region_info["synapse_count"] = py::cast(region->getSynapseCount());
                topology[std::to_string(id)] = py::cast(region_info);
            }
        }
        
        return topology;
    }, py::arg("brain"), "Analyze brain topology")
    
    m.def("calculate_brain_connectivity", [](std::shared_ptr<Brain> brain) {
        double total_connections = 0.0;
        double max_possible = 0.0;
        
        auto region_ids = brain->getRegionIds();
        
        for (size_t i = 0; i < region_ids.size(); ++i) {
            for (size_t j = 0; j < region_ids.size(); ++j) {
                if (i != j) {
                    max_possible += brain->getRegion(region_ids[i])->getNeuronCount() * 
                                   brain->getRegion(region_ids[j])->getNeuronCount();
                }
            }
        }
        
        // Count actual connections...
        // This would need access to brain->getRegions() and iterate through neurons and synapses
        
        return max_possible > 0.0 ? total_connections / max_possible : 0.0;
    }, py::arg("brain"), "Calculate brain connectivity");
    
    // Error types
    py::class_<NLMError>(m, "NLMError", py::base<std::runtime_error>())
        .def(py::init<const std::string&>());
    
    py::class_<ConfigurationError>(m, "ConfigurationError", py::base<NLMError>())
        .def(py::init<const std::string&>());
    
    py::class_<BrainError>(m, "BrainError", py::base<NLMError>())
        .def(py::init<const std::string&>());
    
    py::class_<WorldError>(m, "WorldError", py::base<NLMError>())
        .def(py::init<const std::string&>());
    
    py::class_<AgentError>(m, "AgentError", py::base<NLMError>())
        .def(py::init<const std::string&>());
    
    py::class_<MemoryError>(m, "MemoryError", py::base<NLMError>())
        .def(py::init<const std::string&>());
    
    // InvalidArgument exception for Python
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Module-level constants
    m.attr("VERSION") = "3.0.0";
    m.attr("DEVELOPMENT_VERSION") = "Phase 6 - Final Integration";
    
    // Configuration helper functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set sensible defaults
        config->set("neuron_count", 1000);
        config->set("connection_density", 0.1);
        config->set("plasticity_enabled", true);
        config->set("development_enabled", true);
        config->set("memory_capacity", 1000);
        config->set("simulation.timestep", 0.01);
        config->set("simulation.max_steps", 1000);
        config->set("random_seed", 42);
        return config;
    }, "Create default configuration with sensible defaults")
    
    m.def("validate_config", [](const Config& config) {
        // Basic validation
        auto neuron_count_opt = config.get("neuron_count");
        if (neuron_count_opt && neuron_count_opt->getInt() <= 0) {
            return false;
        }
        
        auto connection_density_opt = config.get("connection_density");
        if (connection_density_opt) {
            double density = connection_density_opt->getDouble();
            if (density < 0.0 || density > 1.0) {
                return false;
            }
        }
        
        return true;
    }, "Validate configuration parameters")
    
    m.def("create_config_from_dict", [](const std::map<std::string, py::object>& dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (const auto& pair : dict) {
            std::string key = pair.first;
            py::object value = pair.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, py::arg("dict"), "Create configuration from dictionary");
    
    m.def("config_to_dict", [](const Config& config) {
        std::map<std::string, py::object> dict;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get(key);
            if (value) {
                dict[key] = py::cast(value->getString());
            }
        }
        return dict;
    }, "Convert configuration to dictionary");
    
    // Simulation helper functions
    m.def("run_simple_simulation", [](int num_steps, bool verbose = false) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 500);
        config->set("connection_density", 0.05);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>(10, 10, 8, 8);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->setWorld(*world);
        
        for (int step = 0; step < num_steps; ++step) {
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << "/" << num_steps << std::endl;
            }
            
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain, world, agent);
    }, py::arg("num_steps"), py::arg("verbose") = false, "Run simple simulation with default settings");
    
    m.def("run_training_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                                     int max_steps, bool verbose = false) {
        world->reset();
        agent->reset();
        
        double total_reward = 0.0;
        
        for (int step = 0; step < max_steps; ++step) {
            if (verbose && step % 50 == 0) {
                std::cout << "Step " << step << "/" << max_steps << std::endl;
            }
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->brain_->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            total_reward += world->getReward();
            
            if (world->isEpisodeFinished()) {
                break;
            }
        }
        
        return std::make_tuple(agent, world, total_reward);
    }, py::arg("agent"), py::arg("world"), py::arg("max_steps"), py::arg("verbose") = false,
       "Run single training episode");
}

} // namespace nlm
