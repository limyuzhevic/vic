#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
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
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Advanced Python API
        ------------------------------------------------
        
        An enhanced Python binding for the NLM C++ neural simulation framework.
        
        This advanced API provides:
        
        **Core Brain Control**
        - Advanced brain manipulation and configuration
        - Event-driven neural control
        - Batch operations for efficiency
        
        **Memory Systems**
        - Working memory management and analysis
        - Episodic memory storage and retrieval
        - Memory consolidation control
        - Memory statistics and metrics
        
        **Neuromodulation Systems**
        - Real-time neuromodulation control
        - Reward prediction error management
        - Curiosity and novelty detection
        - Integration with plasticity
        
        **Prediction and Cognition**
        - Neural prediction system access
        - Concept formation and management
        - Planning and decision making
        - Attention and focus control
        
        **Development and Plasticity**
        - Developmental stage management
        - Plasticity control (STDP, Hebbian, structural)
        - Synaptogenesis and pruning
        - Activity-dependent learning
        
        **Advanced Features**
        - Performance monitoring and profiling
        - Checkpoint management and replay
        - Simulation visualization
        - Real-time analytics
        
        **Integration Utilities**
        - Complete brain-world-agent integration
        - Episode recording and analysis
        - Multi-environment support
        - Advanced logging and debugging
        
        This API is designed for both beginners and advanced users who want to:
        - Build sophisticated neural simulations
        - Research artificial intelligence
        - Develop learning algorithms
        - Experiment with neural dynamics
        - Create autonomous agents
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // ===============================================================
    // BASIC IDENTIFIERS AND ENUMERATIONS
    // ===============================================================
    
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
        .def("to_string", [](const NeuronId& id) {
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

    // ===============================================================
    // COMPREHENSIVE ENUMERATIONS
    // ===============================================================
    
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

    // ===============================================================
    // ADVANCED CONFIGURATION SYSTEM
    // ===============================================================
    
    py::class_<Config> advanced_config_class(m, "AdvancedConfig", R"pbdoc(Advanced configuration system for NLM)
        
        Provides comprehensive configuration management with validation,
        inheritance, and parameter optimization features.
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
        .def("get", [](Config& self, const std::string& key) {
            auto result = self.get<std::string>(key);
            if (result) {
                return *result;
            }
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get configuration value as string")
        .def("get_int", [](Config& self, const std::string& key) {
            auto result = self.get<int64_t>(key);
            if (result) {
                return *result;
            }
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get configuration value as integer")
        .def("get_float", [](Config& self, const std::string& key) {
            auto result = self.get<float>(key);
            if (result) {
                return *result;
            }
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get configuration value as float")
        .def("get_double", [](Config& self, const std::string& key) {
            auto result = self.get<double>(key);
            if (result) {
                return *result;
            }
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get configuration value as double")
        .def("get_bool", [](Config& self, const std::string& key) {
            auto result = self.get<bool>(key);
            if (result) {
                return *result;
            }
            throw py::key_error("Key not found: " + key);
        }, py::arg("key"), "Get configuration value as boolean")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as string")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as integer")
        .def("set", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as float")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as double")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value as boolean")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getOr", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getOr", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", &Config::validate,
             "Validate configuration values")
        .def("apply", [](Config& self, const Config& other) {
            self.apply(other);
        }, py::arg("other"), "Apply another configuration")
        .def("merge", [](Config& self, const Config& other, bool overwrite = true) {
            self.merge(other, overwrite);
        }, py::arg("other"), py::arg("overwrite") = true, "Merge another configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===============================================================
    // ADVANCED SENSORY INPUT CLASSES
    // ===============================================================
    
    py::class_<SensoryInput> sensory_input_class(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        Abstract base class representing sensory information received by the agent.
        Provides access to different types of sensory data.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput> vision_class(m, "Vision", R"pbdoc(Vision sensory input)
        
        2D visual input representing what the agent sees.
        Supports multiple color channels and spatial dimensions.
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
        .def("normalize", [](Vision& self, float targetMin = 0.0f, float targetMax = 1.0f) {
            self.normalize(targetMin, targetMax);
        }, py::arg("targetMin") = 0.0f, py::arg("targetMax") = 1.0f,
            "Normalize vision data to specified range")
        .def("to_tensor", [](Vision& self) {
            return self.toTensor();
        }, "Convert vision data to tensor format");

    py::class_<Audio, SensoryInput> audio_class(m, "Audio", R"pbdoc(Audio sensory input)
        
        Audio input representing sound data.
        Supports sample rate and various audio features.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("compute_features", [](Audio& self) {
            return self.computeFeatures();
        }, "Compute audio features (MFCC, spectral features)")
        .def("preprocess", [](Audio& self, bool normalize = true, bool filter = false) {
            self.preprocess(normalize, filter);
        }, py::arg("normalize") = true, py::arg("filter") = false,
            "Preprocess audio data");

    py::class_<InternalSignals, SensoryInput> internal_signals_class(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        Internal state signals representing agent's internal state.
        Includes physiological and cognitive signals.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("get_signal_count", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("get_mean", &InternalSignals::getMean,
             "Get mean of all signals")
        .def("get_variance", &InternalSignals::getVariance,
             "Get variance of all signals");

    // ===============================================================
    // ADVANCED ACTION SYSTEM
    // ===============================================================
    
    py::class_<Action> action_class(m, "Action", R"pbdoc(Advanced action representation for motor output)
        
        Represents an action that an agent can perform in the environment.
        Supports multiple action types and parameters.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getParameter", [](Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index < params.size()) return params[index];
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"), "Get parameter by index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto& params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
            } else {
                params.resize(index + 1, 0.0f);
                params[index] = value;
            }
        }, py::arg("index"), py::arg("value"), "Set parameter by index")
        .def("getName", &Action::getName)
        .def("addParameter", [](Action& self, const std::string& name, float value) {
            self.addParameter(name, value);
        }, py::arg("name"), py::arg("value"), "Add a named parameter")
        .def("getNamedParameter", [](Action& self, const std::string& name) {
            return self.getNamedParameter(name);
        }, py::arg("name"), "Get named parameter")
        .def("setNamedParameter", [](Action& self, const std::string& name, float value) {
            self.setNamedParameter(name, value);
        }, py::arg("name"), py::arg("value"), "Set named parameter")
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& self) {
            return self.toDict();
        }, "Convert action to dictionary")
        .def("from_dict", [](Action& self, const py::dict& d) {
            self.fromDict(d);
        }, py::arg("dict"), "Load action from dictionary");

    // ===============================================================
    // ADVANCED WORLD OBJECT SYSTEM
    // ===============================================================
    
    py::class_<WorldObject> world_object_class(m, "WorldObject", R"pbdoc(Advanced world object representation)
        
        Represents an object in the simulated environment.
        Supports various properties and behaviors.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("z", &WorldObject::z)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def_readwrite("mass", &WorldObject::mass)
        .def_readwrite("velocityX", &WorldObject::velocityX)
        .def_readwrite("velocityY", &WorldObject::velocityY)
        .def_readwrite("velocityZ", &WorldObject::velocityZ)
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            return self.distanceTo(other);
        }, py::arg("other"), "Calculate distance to another object")
        .def("is_colliding", [](const WorldObject& self, const WorldObject& other) {
            return self.isColliding(other);
        }, py::arg("other"), "Check if colliding with another object")
        .def("move", [](WorldObject& self, float dx, float dy, float dz) {
            self.move(dx, dy, dz);
        }, py::arg("dx"), py::arg("dy"), py::arg("dz"), "Move object by deltas")
        .def("interact", [](WorldObject& self, WorldObject& other) {
            return self.interact(other);
        }, py::arg("other"), "Interact with another object");

    // ===============================================================
    // ADVANCED AGENT BODY SYSTEM
    // ===============================================================
    
    py::class_<AgentBody> agent_body_class(m, "AgentBody", R"pbdoc(Advanced agent body state)
        
        Complete representation of an agent's physical body state.
        Includes position, velocity, energy, health, and age.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("z", &AgentBody::z)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("velocityZ", &AgentBody::velocityZ)
        .def_readwrite("angularVelocityX", &AgentBody::angularVelocityX)
        .def_readwrite("angularVelocityY", &AgentBody::angularVelocityY)
        .def_readwrite("angularVelocityZ", &AgentBody::angularVelocityZ)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("maxEnergy", &AgentBody::maxEnergy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("maxHealth", &AgentBody::maxHealth)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("isAlive", &AgentBody::isAlive)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def_readwrite("currentAction", &AgentBody::currentAction)
        .def("reset", &AgentBody::reset)
        .def("update", [](AgentBody& self, float dt) {
            self.update(dt);
        }, py::arg("dt"), "Update body state over time step")
        .def("set_position", [](AgentBody& self, float x, float y, float z) {
            self.setPosition(x, y, z);
        }, py::arg("x"), py::arg("y"), py::arg("z"), "Set position")
        .def("get_position", [](const AgentBody& self) {
            return self.getPosition();
        }, "Get position as tuple")
        .def("set_velocity", [](AgentBody& self, float vx, float vy, float vz) {
            self.setVelocity(vx, vy, vz);
        }, py::arg("vx"), py::arg("vy"), py::arg("vz"), "Set velocity")
        .def("get_velocity", [](const AgentBody& self) {
            return self.getVelocity();
        }, "Get velocity as tuple")
        .def("get_speed", &AgentBody::getSpeed,
             "Get current speed")
        .def("get_energy_level", [](const AgentBody& self) {
            return self.getEnergyLevel();
        }, "Get normalized energy level (0-1)")
        .def("get_health_level", [](const AgentBody& self) {
            return self.getHealthLevel();
        }, "Get normalized health level (0-1)")
        .def("is_stable", [](const AgentBody& self) {
            return self.isStable();
        }, "Check if body is stable (not falling)");

    // ===============================================================
    // ADVANCED ACTION RESULT SYSTEM
    // ===============================================================
    
    py::class_<ActionResult> action_result_class(m, "ActionResult", R"pbdoc(Advanced action result from world)
        
        Result of an action performed by the agent in the environment.
        Contains reward, success status, and detailed feedback.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def_readwrite("timestamp", &ActionResult::timestamp)
        .def_readwrite("actionType", &ActionResult::actionType)
        .def("is_positive", [](const ActionResult& self) {
            return self.isPositive();
        }, "Check if result is positive")
        .def("is_negative", [](const ActionResult& self) {
            return self.isNegative();
        }, "Check if result is negative")
        .def("get_magnitude", [](const ActionResult& self) {
            return self.getMagnitude();
        }, "Get magnitude of result")
        .def("to_dict", [](const ActionResult& self) {
            return self.toDict();
        }, "Convert result to dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& d) {
            self.fromDict(d);
        }, py::arg("dict"), "Load result from dictionary");

    // ===============================================================
    // ADVANCED SENSORY PERCEPT SYSTEM
    // ===============================================================
    
    py::class_<SensoryPercept> sensory_percept_class(m, "SensoryPercept", R"pbdoc(Advanced sensory percept data)
        
        Complete sensory perception data from the environment.
        Integrates vision, touch, internal signals, and audio.
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
        .def("normalize", [](SensoryPercept& self) {
            self.normalize();
        }, "Normalize all sensory signals")
        .def("blend", [](SensoryPercept& self, const SensoryPercept& other, float weight) {
            self.blend(other, weight);
        }, py::arg("other"), py::arg("weight"), "Blend with another percept")
        .def("apply_noise", [](SensoryPercept& self, float noiseLevel = 0.1f) {
            self.applyNoise(noiseLevel);
        }, py::arg("noiseLevel") = 0.1f, "Apply noise to sensory signals")
        .def("to_dict", [](const SensoryPercept& self) {
            return self.toDict();
        }, "Convert percept to dictionary")
        .def("from_dict", [](SensoryPercept& self, const py::dict& d) {
            self.fromDict(d);
        }, py::arg("dict"), "Load percept from dictionary");

    // ===============================================================
    // ADVANCED BRAIN CLASS
    // ===============================================================
    
    py::class_<Brain> brain_class(m, "Brain", R"pbdoc(Advanced neural simulation brain class)
        
        Central neural simulation brain with complete integration of all systems:
        - Memory (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction system
        - Cognition (planning, concept formation, attention)
        - Development system
        - Plasticity (STDP, Hebbian, structural)
        
        Provides comprehensive control over neural computation and behavior.
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get the structural plasticity system")
        
        // Advanced brain control methods
        .def("batch_inject_current", [](Brain& self, NeuronType type, float current, size_t count) {
            for (size_t i = 0; i < count; ++i) {
                self.injectCurrentToNeurons(type, current);
            }
        }, py::arg("type"), py::arg("current"), py::arg("count"),
            "Inject current to multiple neurons of a type")
        .def("get_neuron_stats", [](Brain& self) {
            return self.getNeuronStats();
        }, "Get comprehensive neuron statistics")
        .def("get_plasticity_stats", [](Brain& self) {
            return self.getPlasticityStats();
        }, "Get plasticity system statistics")
        .def("get_memory_stats", [](Brain& self) {
            return self.getMemoryStats();
        }, "Get memory system statistics")
        .def("get_neuromodulation_stats", [](Brain& self) {
            return self.getNeuromodulationStats();
        }, "Get neuromodulation system statistics")
        .def("simulate_for_time", [](Brain& self, double duration, double timestep = 0.001) {
            size_t steps = static_cast<size_t>(duration / timestep);
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("duration"), py::arg("timestep") = 0.001,
            "Simulate for a specific duration")
        .def("simulate_episodes", [](Brain& self, size_t numEpisodes, size_t stepsPerEpisode, double timestep = 0.001) {
            for (size_t episode = 0; episode < numEpisodes; ++episode) {
                for (size_t step = 0; step < stepsPerEpisode; ++step) {
                    self.step(step, step * timestep);
                }
            }
        }, py::arg("numEpisodes"), py::arg("stepsPerEpisode"), py::arg("timestep") = 0.001,
            "Simulate multiple episodes")
        .def("enable_all_systems", [](Brain& self) {
            self.enableAllSystems();
        }, "Enable all brain systems")
        .def("disable_all_systems", [](Brain& self) {
            self.disableAllSystems();
        }, "Disable all brain systems")
        .def("get_system_status", [](Brain& self) {
            return self.getSystemStatus();
        }, "Get comprehensive system status");

    // ===============================================================
    // ADVANCED AGENT BRAIN CLASS
    // ===============================================================
    
    py::class_<AgentBrain> agent_brain_class(m, "AgentBrain", R"pbdoc(Advanced agent brain interface)
        
        Advanced interface connecting NLM brain to world environment.
        Provides complete control over agent behavior and interaction.
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
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("enableNovelty", &AgentBrain::enableNovelty,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("isNoveltyEnabled", &AgentBrain::isNoveltyEnabled)
        
        // Advanced agent control methods
        .def("create_episode", [](AgentBrain& self, size_t numSteps) {
            return self.createEpisode(numSteps);
        }, py::arg("numSteps"), "Create and run an episode")
        .def("get_episode_stats", [](AgentBrain& self) {
            return self.getEpisodeStats();
        }, "Get episode statistics")
        .def("get_learning_curve", [](AgentBrain& self) {
            return self.getLearningCurve();
        }, "Get learning curve data")
        .def("get_behavior_stats", [](AgentBrain& self) {
            return self.getBehaviorStats();
        }, "Get behavior statistics")
        .def("get_sensory_analysis", [](AgentBrain& self) {
            return self.getSensoryAnalysis();
        }, "Get sensory input analysis")
        .def("get_motor_analysis", [](AgentBrain& self) {
            return self.getMotorAnalysis();
        }, "Get motor output analysis");

    // ===============================================================
    // ADVANCED SIMPLE WORLD CLASS
    // ===============================================================
    
    py::class_<SimpleWorld> simple_world_class(m, "SimpleWorld", R"pbdoc(Advanced 2D world for NLM simulation)
        
        Advanced world simulation with complete physics and interaction
        capabilities for NLM agents.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
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
        .def("get_object_count", &SimpleWorld::getObjectCount,
             "Get number of objects in world")
        .def("get_agent_position", [](SimpleWorld& self) {
            return self.getAgentBody().getPosition();
        }, "Get agent position")
        .def("get_agent_velocity", [](SimpleWorld& self) {
            return self.getAgentBody().getVelocity();
        }, "Get agent velocity")
        .def("simulate_step", [](SimpleWorld& self, float dt) {
            self.update(dt);
        }, py::arg("dt"), "Simulate one step")
        .def("simulate_steps", [](SimpleWorld& self, size_t numSteps, float dt) {
            for (size_t i = 0; i < numSteps; ++i) {
                self.update(dt);
            }
        }, py::arg("numSteps"), py::arg("dt"), "Simulate multiple steps")
        .def("get_energy_stats", [](SimpleWorld& self) {
            return self.getEnergyStats();
        }, "Get energy statistics");

    // ===============================================================
    // FACTORY FUNCTIONS AND UTILITIES
    // ===============================================================
    
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

    // ===============================================================
    // CONSTANTS AND METADATA
    // ===============================================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // ===============================================================
    // ADVANCED UTILITY FUNCTIONS
    // ===============================================================
    
    m.def("create_complete_brain_world_agent", []() -> std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(brain, world, agent);
    }, "Create a complete brain-world-agent system");

    m.def("run_integration_demo", []() {
        Phase6IntegratedExperiment experiment;
        return experiment.verifyIntegration();
    }, "Run integration verification demo");

    m.def("run_phase6_experiment", [](const Phase6Config& config) {
        Phase6IntegratedExperiment experiment;
        return experiment.run(config);
    }, py::arg("config"), "Run complete Phase 6 integration experiment");

    // ===============================================================
    // ADVANCED ANALYTICS AND PROFILING
    // ===============================================================
    
    m.def("performance_profile", [](const std::shared_ptr<Brain>& brain, size_t steps = 1000) {
        auto profiler = std::make_shared<PerformanceProfiler>();
        return profiler->profile(brain, steps);
    }, py::arg("brain"), py::arg("steps") = 1000,
        "Profile brain performance over specified steps");

    m.def("neural_dynamics_analysis", [](const std::shared_ptr<Brain>& brain, size_t steps = 500) {
        auto analyzer = std::make_shared<NeuralDynamicsAnalyzer>();
        return analyzer->analyze(brain, steps);
    }, py::arg("brain"), py::arg("steps") = 500,
        "Analyze neural dynamics over specified steps");

    m.def("learning_progress_report", [](const std::shared_ptr<AgentBrain>& agent, size_t steps = 1000) {
        auto reporter = std::make_shared<LearningProgressReporter>();
        return reporter->generateReport(agent, steps);
    }, py::arg("agent"), py::arg("steps") = 1000,
        "Generate learning progress report");

    m.def("system_health_check", [](const std::shared_ptr<Brain>& brain) {
        return brain->getSystemHealth();
    }, py::arg("brain"), "Perform comprehensive system health check");

    m.def("neural_connectivity_analysis", [](const std::shared_ptr<Brain>& brain) {
        auto analyzer = std::make_shared<NeuralConnectivityAnalyzer>();
        return analyzer->analyzeConnectivity(brain);
    }, py::arg("brain"), "Analyze neural connectivity patterns");

    m.def("memory_efficiency_report", [](const std::shared_ptr<Brain>& brain) {
        auto reporter = std::make_shared<MemoryEfficiencyReporter>();
        return reporter->generateReport(brain);
    }, py::arg("brain"), "Generate memory efficiency report");

    // ===============================================================
    // ADVANCED HELP AND DOCUMENTATION
    // ===============================================================
    
    m.def("show_advanced_help", []() {
        std::cout << R"pbdoc(
NLM Advanced Python API - Help and Documentation
===============================================

This API provides comprehensive control over the NLM neural simulation framework.

QUICK START EXAMPLES:

# Basic brain setup
import pynlm
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Complete agent system
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Simulation loop
for i in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

ADVANCED FEATURES:

1. Memory Systems:
   brain.getWorkingMemory()
   brain.getEpisodicMemory()
   brain.getAssociativeMemory()

2. Neuromodulation:
   agent.enableRewardModulation(True)
   agent.enableCuriosity(True)
   agent.getCuriosityLevel()

3. Development:
   agent.enableDevelopment(True)
   brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

4. Prediction:
   brain.getPredictionSystem()
   brain.getPlanner()
   brain.getConceptFormation()

5. Performance:
   brain.getTotalSpikeCount()
   brain.getAverageFiringRate()
   brain.getExcitationInhibitionRatio()

6. Integration:
   pynlm.run_integration_demo()
   pynlm.run_phase6_experiment(config)

TROUBLESHOOTING:

- Always call brain.initialize() before stepping
- Use try-except blocks for file operations
- Monitor system health with getSystemHealth()
- Check memory usage with getMemoryStats()

For detailed documentation, see:
- HOW_TO_USE.md (basic usage)
- docs/ARCHITECTURE.md (system architecture)
- docs/SCIENCE.md (scientific background)
)pbdoc";
        std::cout << std::endl;
    }, "Show advanced help and documentation");

    // ===============================================================
    // VERSION AND METADATA
    // ===============================================================
    
    m.attr("__version__") = "2.0.0-advanced";
    m.attr("__description__") = "NLM Advanced Python API with complete Phase 6 integration";
    m.attr("__author__") = "NLM Project";
    m.attr("__license__") = "MIT";
    
    // ===============================================================
    // ERROR TYPES AND EXCEPTION HANDLING
    // ===============================================================
    
    py::class_<std::runtime_error>(m, "RuntimeError")
        .def(py::init<const std::string&>());
    
    py::class_<std::invalid_argument>(m, "InvalidArgumentError")
        .def(py::init<const std::string&>());
    
    py::class_<py::key_error>(m, "KeyError")
        .def(py::init<const std::string&>());
    
    py::class_<py::index_error>(m, "IndexError")
        .def(py::init<const std::string&>());
}

} // namespace nlm
