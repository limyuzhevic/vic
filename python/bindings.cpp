#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <exception>
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
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception for Python bindings
class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class InitializationError : public BrainError {
public:
    explicit InitializationError(const std::string& msg) : BrainError("Initialization failed: " + msg) {}
};

class InvalidStateError : public BrainError {
public:
    explicit InvalidStateError(const std::string& msg) : BrainError("Invalid brain state: " + msg) {}
};

// Context manager support for Brain class
class BrainContextManager {
public:
    explicit BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    ~BrainContextManager() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    // Disable copying
    BrainContextManager(const BrainContextManager&) = delete;
    BrainContextManager& operator=(const BrainContextManager&) = delete;
    
    // Enable moving
    BrainContextManager(BrainContextManager&& other) noexcept : brain_(std::move(other.brain_)) {}
    BrainContextManager& operator=(BrainContextManager&& other) noexcept {
        if (this != &other) {
            brain_ = std::move(other.brain_);
        }
        return *this;
    }
    
    std::shared_ptr<Brain> get() { return brain_; }
    
    // Python-specific methods
    bool step(int currentStep) {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        brain_->step(currentStep);
        return true;
    }
    
    bool stepWithTime(int currentStep, double currentTime) {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        brain_->step(currentStep, currentTime);
        return true;
    }
    
    std::unique_ptr<Action> produceAction() {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        return brain_->produceAction();
    }
    
    std::unordered_map<std::string, double> getStats() {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        return brain_->getStats();
    }
    
    std::unordered_map<std::string, double> getMemoryStats() {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        return brain_->getMemoryStats();
    }
    
    std::unordered_map<std::string, double> getCognitionStats() {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        return brain_->getCognitionStats();
    }
    
    std::unordered_map<std::string, double> getNeuromodulationStats() {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        return brain_->getNeuromodulationStats();
    }
    
    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    bool save(const std::string& filepath) {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        return brain_->save(filepath);
    }
    
    bool load(const std::string& filepath) {
        if (!brain_) {
            throw InvalidStateError("Brain is not initialized");
        }
        return brain_->load(filepath);
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Event callback system
class BrainEventCallbacks {
public:
    std::function<void(const std::unordered_map<std::string, double>&)> onStateChanged;
    std::function<void(int, double)> onStepCompleted;
    std::function<void(const std::string&)> onError;
    std::function<void()> onBrainReset;
    std::function<void()> onBrainSave;
    std::function<void()> onBrainLoad;
    
    void triggerStateChanged(const std::unordered_map<std::string, double>& stats) {
        if (onStateChanged) {
            onStateChanged(stats);
        }
    }
    
    void triggerStepCompleted(int step, double time) {
        if (onStepCompleted) {
            onStepCompleted(step, time);
        }
    }
    
    void triggerError(const std::string& error) {
        if (onError) {
            onError(error);
        }
    }
    
    void triggerBrainReset() {
        if (onBrainReset) {
            onBrainReset();
        }
    }
    
    void triggerBrainSave() {
        if (onBrainSave) {
            onBrainSave();
        }
    }
    
    void triggerBrainLoad() {
        if (onBrainLoad) {
            onBrainLoad();
        }
    }
};

// Configuration presets
namespace ConfigPresets {
    static std::shared_ptr<Config> createExploration() {
        auto config = std::make_shared<Config>();
        
        // High exploration parameters
        config->set("random_seed", "42");
        config->set("neuron_count", "2000");
        config->set("region_count", "3");
        config->set("connection_probability", "0.15f");
        
        // High plasticity for exploration
        config->set("stdp_ltp_weight", "0.02f");
        config->set("stdp_ltd_weight", "0.022f");
        config->set("synaptogenesis_rate", "0.002f");
        config->set("pruning_rate", "0.0005f");
        
        // High neuromodulation
        config->set("dopamine_level", "0.8f");
        config->set("curiosity_level", "0.9f");
        config->set("novelty_sensitivity", "0.7f");
        config->set("prediction_error_sensitivity", "0.6f");
        
        // Development
        config->set("development_enabled", "true");
        config->set("development_speed", "1.5f");
        
        // Memory
        config->set("working_memory_capacity", "500");
        config->set("episodic_memory_max_episodes", "2000");
        
        // Prediction and cognition
        config->set("prediction_enabled", "true");
        config->set("planning_depth", "8");
        config->set("concept_formation_enabled", "true");
        config->set("attention_inhibition_strength", "0.3f");
        
        // Environment
        config->set("simulation_timestep", "0.001f");
        config->set("replay_interval", "50");
        config->set("consolidation_interval", "500");
        
        config->set("exploration_preset", "true");
        config->set("description", "Exploration preset: High plasticity, curiosity, and novelty detection");
        
        return config;
    }
    
    static std::shared_ptr<Config> createExploitation() {
        auto config = std::make_shared<Config>();
        
        // Lower random seed for determinism
        config->set("random_seed", "123");
        config->set("neuron_count", "1000");
        config->set("region_count", "2");
        config->set("connection_probability", "0.05f");
        
        // Moderate plasticity for exploitation
        config->set("stdp_ltp_weight", "0.01f");
        config->set("stdp_ltd_weight", "0.012f");
        config->set("synaptogenesis_rate", "0.0005f");
        config->set("pruning_rate", "0.0002f");
        
        // Moderate neuromodulation
        config->set("dopamine_level", "0.5f");
        config->set("curiosity_level", "0.3f");
        config->set("novelty_sensitivity", "0.3f");
        config->set("prediction_error_sensitivity", "0.4f");
        
        // Development (but slower)
        config->set("development_enabled", "true");
        config->set("development_speed", "0.8f");
        
        // Memory
        config->set("working_memory_capacity", "200");
        config->set("episodic_memory_max_episodes", "500");
        
        // Prediction and cognition
        config->set("prediction_enabled", "true");
        config->set("planning_depth", "3");
        config->set("concept_formation_enabled", "true");
        config->set("attention_inhibition_strength", "0.7f");
        
        // Environment
        config->set("simulation_timestep", "0.001f");
        config->set("replay_interval", "200");
        config->set("consolidation_interval", "2000");
        
        config->set("exploitation_preset", "true");
        config->set("description", "Exploitation preset: High efficiency, stable performance");
        
        return config;
    }
    
    static std::shared_ptr<Config> createBalanced() {
        auto config = std::make_shared<Config>();
        
        // Balanced parameters
        config->set("random_seed", "999");
        config->set("neuron_count", "1500");
        config->set("region_count", "3");
        config->set("connection_probability", "0.1f");
        
        // Moderate plasticity
        config->set("stdp_ltp_weight", "0.015f");
        config->set("stdp_ltd_weight", "0.018f");
        config->set("synaptogenesis_rate", "0.001f");
        config->set("pruning_rate", "0.0003f");
        
        // Balanced neuromodulation
        config->set("dopamine_level", "0.6f");
        config->set("curiosity_level", "0.5f");
        config->set("novelty_sensitivity", "0.5f");
        config->set("prediction_error_sensitivity", "0.5f");
        
        // Normal development
        config->set("development_enabled", "true");
        config->set("development_speed", "1.0f");
        
        // Memory
        config->set("working_memory_capacity", "350");
        config->set("episodic_memory_max_episodes", "1000");
        
        // Prediction and cognition
        config->set("prediction_enabled", "true");
        config->set("planning_depth", "5");
        config->set("concept_formation_enabled", "true");
        config->set("attention_inhibition_strength", "0.5f");
        
        // Environment
        config->set("simulation_timestep", "0.001f");
        config->set("replay_interval", "100");
        config->set("consolidation_interval", "1000");
        
        config->set("balanced_preset", "true");
        config->set("description", "Balanced preset: Good trade-off between exploration and exploitation");
        
        return config;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This module provides:
        - Comprehensive error handling and validation
        - Context manager support for brain lifecycle management
        - Statistics and monitoring methods
        - Event callbacks for brain state changes
        - Configuration presets (exploration, exploitation, balanced)
        - Save/load serialization
        - Pythonic property access
        - Memory and cognition statistics
        - Neuromodulation statistics
        
        Features:
        * getStats() - Aggregates all system statistics
        * getMemoryStats() - Working memory and episodic memory stats
        * getCognitionStats() - Prediction, planning, concept formation stats
        * getNeuromodulationStats() - All neuromodulators stats
        * Event callbacks for brain state changes
        * Configuration presets
        * Context manager support
        * Enhanced validation and error handling
    )pbdoc";

    // Register custom exceptions
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<InvalidStateError>(m, "InvalidStateError");

    // NeuronId class
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

    // SynapseId class
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

    // RegionId class
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enumerations
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

    // Config class with enhanced validation
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or key=value file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a key=value file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get a configuration value as string (will raise if not found)")
        .def("getInt", &Config::get<int>, py::arg("key"),
             "Get a configuration value as integer (will raise if not found)")
        .def("getFloat", &Config::get<float>, py::arg("key"),
             "Get a configuration value as float (will raise if not found)")
        .def("getBool", &Config::get<bool>, py::arg("key"),
             "Get a configuration value as boolean (will raise if not found)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + ">";
        });

    // WorldObject class
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
        .def_readwrite("active", &WorldObject::active);

    // AgentBody class
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
        .def("reset", &AgentBody::reset);

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // SimpleWorld class
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Brain class with context manager support
    auto brain_class = py::class_<BrainContextManager>(m, "Brain", R"pbdoc(Enhanced brain class with context manager support)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def("initialize", &BrainContextManager::initialize,
             "Initialize the brain with configuration")
        .def("step", &BrainContextManager::step, py::arg("currentStep"),
             "Perform a simulation step")
        .def("stepWithTime", &BrainContextManager::stepWithTime, py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &BrainContextManager::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &BrainContextManager::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &BrainContextManager::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &BrainContextManager::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &BrainContextManager::reset,
             "Reset brain state")
        .def("save", &BrainContextManager::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainContextManager::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &BrainContextManager::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &BrainContextManager::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &BrainContextManager::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &BrainContextManager::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &BrainContextManager::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &BrainContextManager::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &BrainContextManager::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &BrainContextManager::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &BrainContextManager::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &BrainContextManager::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &BrainContextManager::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &BrainContextManager::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &BrainContextManager::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &BrainContextManager::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &BrainContextManager::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &BrainContextManager::logStatus,
             "Log brain status")
        .def("getStats", &BrainContextManager::getStats,
             "Get comprehensive brain statistics")
        .def("getMemoryStats", &BrainContextManager::getMemoryStats,
             "Get working memory and episodic memory statistics")
        .def("getCognitionStats", &BrainContextManager::getCognitionStats,
             "Get prediction, planning, concept formation statistics")
        .def("getNeuromodulationStats", &BrainContextManager::getNeuromodulationStats,
             "Get all neuromodulators statistics")
        .def("__enter__", [](BrainContextManager& self) -> BrainContextManager& { return self; })
        .def("__exit__", [](BrainContextManager& self, py::args) { self.reset(); });

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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

    // Event callbacks setup
    py::class_<BrainEventCallbacks>(m, "EventCallbacks", R"pbdoc(Brain event callbacks)pbdoc")
        .def(py::init<>())
        .def_readwrite("onStateChanged", &BrainEventCallbacks::onStateChanged)
        .def_readwrite("onStepCompleted", &BrainEventCallbacks::onStepCompleted)
        .def_readwrite("onError", &BrainEventCallbacks::onError)
        .def_readwrite("onBrainReset", &BrainEventCallbacks::onBrainReset)
        .def_readwrite("onBrainSave", &BrainEventCallbacks::onBrainSave)
        .def_readwrite("onBrainLoad", &BrainEventCallbacks::onBrainLoad);

    // Configuration presets
    m.def("createExplorationConfig", &ConfigPresets::createExploration,
          "Create an exploration configuration preset");
    m.def("createExploitationConfig", &ConfigPresets::createExploitation,
          "Create an exploitation configuration preset");
    m.def("createBalancedConfig", &ConfigPresets::createBalanced,
          "Create a balanced configuration preset");

    // Global factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<BrainContextManager> {
        auto brain = std::make_shared<Brain>(config);
        return std::make_shared<BrainContextManager>(brain);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm