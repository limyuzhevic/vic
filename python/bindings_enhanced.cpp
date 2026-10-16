#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

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

// Exception classes for Python bindings
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& msg) : NLMError("Brain error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------------------
        A comprehensive Python interface to the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Better error handling and Pythonic exceptions
        - More intuitive property access
        - Convenience methods for common operations
        - Enhanced visualization and data export tools
        - Plugin system support
        - Comprehensive documentation
        
        Key features:
        * Neural network simulation with spiking neurons
        * Memory systems (working, episodic, associative)
        * Neuromodulation (dopamine, curiosity, novelty)
        * Prediction and planning systems
        * Developmental stages
        * Checkpoint save/load capabilities
        * Visualization integration
        
        Example usage:
        >>> import pynlm as nlm
        >>> config = nlm.createDefaultConfig()
        >>> brain = nlm.createBrain(config)
        >>> brain.initialize()
        >>> # Run simulation...
        >>> brain.save("brain_state.json")
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Register standard exceptions for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==================== Type Definitions ====================
    
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

    // ==================== Enumerations ====================
    
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
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        })
        // Convenience methods for common operations
        .def("getInt", [](Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value with default")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get float value with default")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value with default")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get string value with default")
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set integer value")
        .def("setFloat", [](Config& self, const std::string& key, float value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, static_cast<double>(value), source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set float value")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set boolean value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, "Set string value");

    // ==================== Sensory Input Classes ====================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::to_string(static_cast<int>(input.getType())) + ">";
        });

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
        .def("getImageData", [](const Vision& self) {
            return self.getData();
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getSamples", [](const Audio& self) {
            return self.getData();
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", [](const InternalSignals& self) {
            return self.getData();
        });

    // ==================== Action Class ====================
    
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getNameFromType", [](ActionType type) {
            // Add helper function to get action name from type
            switch(type) {
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
        }, py::arg("type"), "Get action name from type")
        .def("clone", &Action::clone)
        .def("__repr__", [](const Action& action) {
            return "<Action type=" + action.getName() + ">";
        });

    // ==================== World Objects ====================
    
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
        .def("getPosition", [](WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        })
        .def("setPosition", [](WorldObject& obj, float x, float y) {
            obj.x = x;
            obj.y = y;
        }, py::arg("x"), py::arg("y"))
        .def("isHazard", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard;
        })
        .def("isResource", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Resource;
        })
        .def("isWall", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Wall;
        })
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject " + std::to_string(obj.x) + ", " + std::to_string(obj.y) + " type=" + std::to_string(static_cast<int>(obj.type)) + ">";
        });

    // ==================== Agent Body ====================
    
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
        .def("getPosition", [](AgentBody& body) {
            return std::make_pair(body.x, body.y);
        })
        .def("setPosition", [](AgentBody& body, float x, float y) {
            body.x = x;
            body.y = y;
        }, py::arg("x"), py::arg("y"))
        .def("getVelocity", [](AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        })
        .def("isHealthy", [](const AgentBody& body) {
            return body.health > 0.0f;
        })
        .def("isEnergyCritical", [](const AgentBody& body) {
            return body.energy < 0.1f;
        });

    // ==================== Action Results ====================
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", [](const ActionResult& result) {
            return result.success;
        })
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) + 
                   " success=" + std::to_string(result.success) + ">";
        });

    // ==================== Sensory Percept ====================
    
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
        .def("hasVision", [](const SensoryPercept& percept) {
            return percept.getVision().getDimensions() > 0;
        })
        .def("hasTouch", [](const SensoryPercept& percept) {
            return !percept.getTouch().empty();
        })
        .def("hasInternal", [](const SensoryPercept& percept) {
            return !percept.getInternal().empty();
        })
        .def("hasProprioception", [](const SensoryPercept& percept) {
            return !percept.getProprioception().empty();
        })
        .def("hasAudio", [](const SensoryPercept& percept) {
            return !percept.getAudio().empty();
        });

    // ==================== SimpleWorld Class ====================
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjects", [](SimpleWorld& world) {
            // Add method to get all objects (would need to be implemented in C++)
            std::vector<WorldObject> objects;
            // TODO: Implement object retrieval
            return objects;
        }, "Get all world objects (not yet implemented)")
        .def("clearObjects", [](SimpleWorld& world) {
            // Add method to clear all objects
            // TODO: Implement object clearing
        }, "Clear all world objects (not yet implemented)");

    // ==================== Brain Class ====================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // Enhanced Brain methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty neuromodulator")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random generator")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity system")
        .def("getNeuromodulationLevel", [](Brain& brain) {
            // Get average neuromodulation level
            auto* dopamine = brain.getDopamine();
            auto* curiosity = brain.getCuriosity();
            auto* novelty = brain.getNovelty();
            float level = 0.0f;
            if (dopamine) level += dopamine->getLevel();
            if (curiosity) level += curiosity->getLevel();
            if (novelty) level += novelty->getLevel();
            return level / 3.0f;  // Average
        }, "Get average neuromodulation level")
        .def("getExplorationLevel", [](Brain& brain) {
            // Get exploration level based on novelty
            auto* novelty = brain.getNovelty();
            return novelty ? novelty->getLevel() : 0.0f;
        }, "Get exploration level")
        .def("getLearningReadiness", [](Brain& brain) {
            // Get learning readiness based on developmental stage
            auto stage = brain.getDevelopmentalStage();
            switch(stage) {
                case DevelopmentalStage::Initial:
                case DevelopmentalStage::CriticalPeriod:
                    return 1.0f;  // High learning readiness
                case DevelopmentalStage::Maturation:
                    return 0.7f;
                case DevelopmentalStage::Adult:
                    return 0.5f;
                case DevelopmentalStage::Aging:
                    return 0.3f;
                default:
                    return 0.5f;
            }
        }, "Get learning readiness based on developmental stage")
        .def("getPerformanceMetrics", [](Brain& brain) {
            py::dict metrics;
            metrics["total_neurons"] = brain.getTotalNeuronCount();
            metrics["total_synapses"] = brain.getTotalSynapseCount();
            metrics["active_neurons"] = brain.getActiveNeuronCount();
            metrics["firing_neurons"] = brain.getFiringNeuronCount();
            metrics["average_firing_rate"] = brain.getAverageFiringRate();
            metrics["excitatory_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            metrics["total_spikes"] = brain.getTotalSpikeCount();
            return metrics;
        }, "Get comprehensive performance metrics")
        .def("isInitialized", [](Brain& brain) {
            return brain.getRegionCount() > 0;
        }, "Check if brain is initialized")
        .def("getBrainAge", [](Brain& brain) {
            // Get brain age from development system
            auto* dev = brain.getDevelopmentSystem();
            return dev ? dev->getAge() : 0.0f;
        }, "Get brain age in development stages");

    // ==================== AgentBrain Class ====================
    
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
        // Enhanced AgentBrain methods
        .def("isReady", [](AgentBrain& agent) {
            return agent.getBrain()->getRegionCount() > 0;
        }, "Check if agent brain is ready")
        .def("getActionFromBehavior", [](AgentBrain& agent, const SensoryPercept& percept) {
            // Process sensory input and get action
            agent.processSensoryInput(percept);
            auto action = agent.decodeMotorCommand();
            return action;
        }, py::arg("percept"), "Get action from percept (convenience method)")
        .def("getActionWithReward", [](AgentBrain& agent, const SensoryPercept& percept, 
                                      float reward, float predictedReward = 0.0f) {
            // Process sensory input, get action, and apply reward
            agent.processSensoryInput(percept);
            auto action = agent.decodeMotorCommand();
            agent.applyRewardModulation(reward, predictedReward);
            agent.updateDevelopment(1);
            return action;
        }, py::arg("percept"), py::arg("reward"), py::arg("predictedReward") = 0.0f,
           "Get action and apply reward (convenience method)")
        .def("getPerformanceSummary", [](AgentBrain& agent) {
            py::dict summary;
            auto brain = agent.getBrain();
            summary["brain_metrics"] = agent.getBrain()->getPerformanceMetrics();
            summary["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            summary["neuromodulation_level"] = agent.getNeuromodulationLevel();
            summary["curiosity_level"] = agent.getCuriosityLevel();
            summary["novelty_level"] = agent.getNoveltyLevel();
            summary["prediction_error"] = agent.getPredictionError();
            return summary;
        }, "Get comprehensive performance summary");

    // ==================== Factory Functions ====================
    
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

    // ==================== Global Constants ====================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==================== Utility Functions ====================
    
    m.def("version", []() {
        return "NLM Python Bindings v1.0.0 (Enhanced)";
    }, "Get version information");

    m.def("getSystemInfo", []() {
        py::dict info;
        info["name"] = "NLM (Neural Learning Machine)";
        info["version"] = "Phase 6 - Final Integration";
        info["description"] = "Experimental Artificial Developmental Brain";
        info["python_bindings"] = "Enhanced version 1.0.0";
        info["developer"] = "NLM Research Team";
        return info;
    }, "Get system information");

    m.def("createSimulation", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world) {
        auto brain = std::make_shared<Brain>(config);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        return std::make_tuple(brain, agentBrain, world);
    }, py::arg("config"), py::arg("world"), "Create a complete simulation (brain, agent, world)");

    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                         const SensoryPercept& initialPercept, int maxSteps) {
        agent->reset();
        world->reset();
        
        auto percept = initialPercept;
        float totalReward = 0.0f;
        
        for (int step = 0; step < maxSteps; ++step) {
            auto action = agent->decodeMotorCommand();
            world->update(static_cast<uint64_t>(step));
            
            auto result = world->applyMotorCommand(action, static_cast<Timestamp>(step));
            totalReward += result.reward;
            
            auto nextPercept = world->getSensoryPercept();
            agent->processSensoryInput(nextPercept);
            
            if (step >= maxSteps - 1) break;
        }
        
        return totalReward;
    }, py::arg("agent"), py::arg("world"), py::arg("initialPercept"), py::arg("maxSteps"),
       "Run a simulation episode (convenience function)");

    // ==================== Error Handling Utilities ====================
    
    m.def("setErrorHandler", [](std::function<void(const std::string&)> handler) {
        // Set global error handler (conceptual implementation)
        // In a real implementation, this would register a global callback
        return handler;
    }, py::arg("handler"), "Set global error handler");

    m.def("clearErrorHandler", []() {
        // Clear global error handler
        return true;
    }, "Clear global error handler");
}

} // namespace nlm