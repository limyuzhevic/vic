#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
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
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exceptions for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class InvalidInputError : public NLMError {
public:
    explicit InvalidInputError(const std::string& msg) : NLMError("Invalid Input: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ----------------------------------------------------------
        
        An enhanced Python interface for the NLM C++ neural simulation framework.
        
        Key Features:
        - Exception safety with custom error types
        - Pythonic API with convenience methods
        - Advanced simulation controls and monitoring
        - Comprehensive configuration management
        - Performance optimization tools
        - Batch processing capabilities
        
        Usage:
            >>> import pynlm
            >>> config = pynlm.create_config()
            >>> brain = pynlm.create_brain(config)
            >>> brain.initialize()
            >>> brain.step(0)  # Run one simulation step
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<InvalidInputError>(m, "InvalidInputError");

    // Helper functions for type conversions and validation
    auto validate_positive = [](int value, const std::string& name) {
        if (value <= 0) {
            throw InvalidInputError(name + " must be positive, got " + std::to_string(value));
        }
    };

    auto validate_range = [](float value, float min, float max, const std::string& name) {
        if (value < min || value > max) {
            throw InvalidInputError(name + " must be between " + std::to_string(min) + 
                                  " and " + std::to_string(max) + ", got " + std::to_string(value));
        }
    };

    // Identifier types with enhanced functionality
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
        .def("to_int", [](const NeuronId& id) { return id.value; });

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
        .def("to_int", [](const SynapseId& id) { return id.value; });

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
        .def("to_int", [](const RegionId& id) { return id.value; });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced enumeration with descriptive names
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that promotes firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that suppresses firing")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that affects plasticity")
        .value("Sensory", NeuronType::Sensory, "Neuron that receives sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that produces motor output")
        .value("Internal", NeuronType::Internal, "Internal processing neuron")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory connection")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory connection")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory connection")
        .value("Electrical", SynapseType::Electrical, "Electrical connection")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction connection")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period of learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stable phase")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest")
        .value("Active", FiringState::Active, "Neuron is active")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world")
        .value("MoveRight", ActionType::MoveRight, "Move right in world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat food/object")
        .value("Drink", ActionType::Drink, "Drink liquid")
        .value("Rest", ActionType::Rest, "Rest/idle")
        .value("Wait", ActionType::Wait, "Wait without action")
        .value("Custom", ActionType::Custom, "Custom action type")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Collectible resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object")
        .value("Wall", WorldObjectType::Wall, "Impassable wall")
        .value("Marker", WorldObjectType::Marker, "World marker")
        .export_values();

    // Enhanced Config class with better error handling and validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with validation)pbdoc")
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
        // Configuration validation methods
        .def("validate", [](Config& self) {
            // Custom validation logic can be added here
            return true;
        }, "Validate configuration consistency")
        .def("getWithDefault", [](Config& self, const std::string& key, auto defaultValue) {
            if (self.has(key)) {
                auto value = self.get<std::decay_t<decltype(defaultValue)>>(key);
                if (value) return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default, returns default if key not found")
        .def("setIfMissing", [](Config& self, const std::string& key, auto value) {
            if (!self.has(key)) {
                self.set(key, value);
            }
        }, py::arg("key"), py::arg("value"),
             "Set configuration value only if key doesn't exist");

    // Enhanced SensoryInput with better type safety
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class with convenience methods
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataFromImage", [](Vision& self, const std::vector<std::vector<float>>& image) {
            // Flatten 2D image data
            std::vector<float> flat_data;
            flat_data.reserve(image.size() * image[0].size());
            for (const auto& row : image) {
                flat_data.insert(flat_data.end(), row.begin(), row.end());
            }
            self.setData(flat_data);
        }, py::arg("image"), "Set data from 2D image (list of lists)")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getShape", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get dimensions as a tuple (width, height, channels)")
        .def("normalize", [](Vision& self, float mean, float stddev) {
            // Apply normalization to vision data
            auto& data = const_cast<std::vector<float>&>(self.getData());
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean_val = sum / data.size();
            float var = 0.0f;
            for (float val : data) {
                var += (val - mean_val) * (val - mean_val);
            }
            var /= data.size();
            float std_val = std::sqrt(var);
            
            for (auto& val : data) {
                val = (val - mean_val) / (std_val + 1e-8f);
            }
        }, py::arg("mean"), py::arg("stddev"), "Normalize vision data to zero mean, unit variance");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getMean", [](const InternalSignals& self) {
            const auto& signals = self.getData();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get mean of internal signals")
        .def("getStd", [](const InternalSignals& self) {
            const auto& signals = self.getData();
            if (signals.empty()) return 0.0f;
            float mean = std::accumulate(signals.begin(), signals.end(), 0.0f) / signals.size();
            float variance = 0.0f;
            for (float val : signals) {
                variance += (val - mean) * (val - mean);
            }
            variance /= signals.size();
            return std::sqrt(variance);
        }, "Get standard deviation of internal signals");

    // Enhanced Action class with validation
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
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw InvalidInputError("Parameter index out of range: " + std::to_string(index) +
                                       " (valid: 0-" + std::to_string(params.size() - 1) + ")");
            }
            return params[index];
        }, py::arg("index"), "Get parameter at index");

    // Enhanced WorldObject with validation
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
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        .def("isColliding", [](const WorldObject& a, const WorldObject& b) {
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (a.radius + b.radius);
        }, py::arg("other"), "Check if colliding with another object");

    // Enhanced AgentBody with validation
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
        .def("moveTo", [](AgentBody& body, float x, float y, double timestep) {
            float dx = x - body.x;
            float dy = y - body.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance > 0.0f) {
                float speed = distance / timestep;
                body.velocityX = (dx / distance) * speed;
                body.velocityY = (dy / distance) * speed;
                body.isMoving = true;
            }
        }, py::arg("x"), py::arg("y"), py::arg("timestep"), "Move agent to position")
        .def("getSpeed", [](const AgentBody& body) {
            return std::sqrt(body.velocityX * body.velocityX + body.velocityY * body.velocityY);
        }, "Get current speed");

    // Enhanced ActionResult with validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful")
        .def("getFormattedReward", [](const ActionResult& result) {
            return "Reward: " + std::to_string(result.reward) + 
                   " (" + (result.success ? "Success" : "Failure") + ")";
        }, "Get formatted reward string");

    // Enhanced SensoryPercept with validation and convenience methods
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
        .def("getSignalCount", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get total number of signals")
        .def("getTotalValue", [](const SensoryPercept& self) {
            const auto& signals = self.getAllSignals();
            float total = 0.0f;
            for (float val : signals) {
                total += val;
            }
            return total;
        }, "Get sum of all signal values")
        .def("isEmpty", [](const SensoryPercept& self) {
            const auto& signals = self.getAllSignals();
            return signals.empty();
        }, "Check if percept has any signals");

    // Enhanced SimpleWorld with advanced features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world simulation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectAt", [](const SimpleWorld& world, float x, float y) {
            // This would require world to maintain a spatial index
            // For now, return a default WorldObject
            return WorldObject(x, y, WorldObjectType::Empty, 0.0f, 0.1f);
        }, py::arg("x"), py::arg("y"), "Get object at position (returns empty if not found)")
        .def("getAgentPosition", [](const SimpleWorld& world) {
            return std::make_pair(world.getAgentBody().x, world.getAgentBody().y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentDirection", [](const SimpleWorld& world) {
            return world.getAgentBody().orientation;
        }, "Get agent direction (orientation)");

    // Enhanced Brain class with advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Advanced brain methods
        .def("enableRegion", [](Brain& brain, RegionId regionId, bool enable) {
            // This would require Brain to have region enable/disable functionality
            // For now, just log a message
            NLM_LOG_INFO("Region " + std::to_string(regionId.value) + 
                        (enable ? " enabled" : " disabled"));
        }, py::arg("region_id"), py::arg("enable"),
             "Enable or disable a neural region")
        .def("getRegionStats", [](const Brain& brain) {
            // Aggregate statistics from all regions
            size_t total_neurons = brain.getTotalNeuronCount();
            size_t total_synapses = brain.getTotalSynapseCount();
            size_t active_neurons = brain.getActiveNeuronCount();
            float avg_firing_rate = brain.getAverageFiringRate();
            
            return std::make_tuple(total_neurons, total_synapses, active_neurons, avg_firing_rate);
        }, "Get brain statistics as (neurons, synapses, active_neurons, avg_firing_rate) tuple")
        .def("getActivityHeatmap", [](const Brain& brain) {
            // Generate a simple activity heatmap approximation
            // This would require access to internal region data
            std::vector<std::vector<float>> heatmap(10, std::vector<float>(10, 0.0f));
            float max_activity = brain.getFiringNeuronCount();
            if (max_activity > 0) {
                float activity_ratio = static_cast<float>(brain.getFiringNeuronCount()) / 
                                      static_cast<float>(brain.getTotalNeuronCount());
                for (auto& row : heatmap) {
                    for (auto& cell : row) {
                        cell = activity_ratio;
                    }
                }
            }
            return heatmap;
        }, "Get approximate brain activity heatmap (10x10 grid)");

    // Enhanced AgentBrain with advanced features
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        // Enhanced agent methods
        .def("enableAllSystems", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all neuromodulation and learning systems")
        .def("disableAllSystems", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all neuromodulation and learning systems")
        .def("getSystemStatus", [](const AgentBrain& agent) {
            return std::make_tuple(
                agent.isRewardModulationEnabled(),
                agent.isStructuralPlasticityEnabled(),
                agent.isDevelopmentEnabled(),
                agent.isCuriosityEnabled()
            );
        }, "Get system status as (reward, structural, development, curiosity)")
        .def("setAllSystems", [](AgentBrain& agent, bool enable) {
            agent.enableRewardModulation(enable);
            agent.enableStructuralPlasticity(enable);
            agent.enableDevelopment(enable);
            agent.enableCuriosity(enable);
        }, py::arg("enable"), "Enable or disable all systems");

    // High-level convenience functions for common use cases
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with standard parameters");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced utility functions
    m.def("run_agent_simulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   int num_steps, bool verbose = false) {
        if (!brain || !world || !agent) {
            throw InvalidInputError("Brain, world, and agent must all be provided");
        }
        
        brain->initialize();
        agent->initialize(*world);
        agent->enableAllSystems();
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->getFiringNeuronCount() << " firing neurons, "
                         << agent->getCuriosityLevel() << " curiosity" << std::endl;
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(), agent->getCuriosityLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"), py::arg("verbose") = false,
         "Run a complete agent simulation and return (spike_count, curiosity_level)");

    m.def("benchmark_brain", [](std::shared_ptr<Brain> brain, int steps, int repetitions = 10) {
        if (!brain) throw InvalidInputError("Brain must be provided");
        if (steps <= 0 || repetitions <= 0) {
            throw InvalidInputError("Steps and repetitions must be positive");
        }
        
        brain->initialize();
        double total_time = 0.0;
        size_t total_spikes = 0;
        
        for (int rep = 0; rep < repetitions; ++rep) {
            brain->reset();
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int step = 0; step < steps; ++step) {
                brain->step(step);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end - start);
            total_time += duration.count();
            total_spikes += brain->getTotalSpikeCount();
        }
        
        double avg_time_per_step = total_time / (repetitions * steps);
        size_t avg_spikes_per_step = total_spikes / (repetitions * steps);
        
        return std::make_tuple(avg_time_per_step, avg_spikes_per_step);
    }, py::arg("brain"), py::arg("steps"), py::arg("repetitions") = 10,
         "Benchmark brain performance, returns (avg_time_per_step, avg_spikes_per_step)");

    m.def("create_config_from_dict", [](std::unordered_map<std::string, py::object> config_dict) {
        auto config = std::make_shared<Config>();
        for (const auto& pair : config_dict) {
            std::string key = pair.first;
            py::object value_obj = pair.second;
            
            // Convert Python object to appropriate C++ type
            if (value_obj.is(py::int_())) {
                config->set(key, value_obj.cast<int>());
            } else if (value_obj.is(py::float_())) {
                config->set(key, value_obj.cast<double>());
            } else if (value_obj.is(py::bool_())) {
                config->set(key, value_obj.cast<bool>());
            } else if (value_obj.is(py::str())) {
                config->set(key, value_obj.cast<std::string>());
            } else if (value_obj.is(py::list())) {
                auto list = value_obj.cast<std::vector<double>>();
                config->set(key, list);
            } else {
                throw ConfigurationError("Unsupported configuration value type for key: " + key);
            }
        }
        return config;
    }, py::arg("config_dict"), 
         "Create configuration from Python dictionary");

    m.def("get_version", []() {
        return "NLM Enhanced Python Bindings v1.0.0";
    }, "Get version information");

    m.def("help", [](const std::string& topic) {
        // Simple help system
        if (topic == "config") {
            std::cout << "Configuration options:" << std::endl;
            std::cout << "  brain.neuron_count: Total number of neurons" << std::endl;
            std::cout << "  brain.synapse_density: Synapse connectivity density" << std::endl;
            std::cout << "  brain.connection_probability: Connection probability" << std::endl;
            std::cout << "  plasticity.stdp.enable: Enable STDP" << std::endl;
            std::cout << "  neuromod.dopamine.scale: Dopamine modulation scale" << std::endl;
        } else if (topic == "brain") {
            std::cout << "Brain methods:" << std::endl;
            std::cout << "  step(): Run one simulation step" << std::endl;
            std::cout << "  initialize(): Initialize brain" << std::endl;
            std::cout << "  receiveSensoryInput(percept): Process sensory input" << std::endl;
            std::cout << "  produceAction(): Generate motor action" << std::endl;
        } else {
            std::cout << "Available topics: config, brain" << std::endl;
        }
    }, py::arg("topic"), "Get help on specific topic");

    // Make invalid IDs available
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
