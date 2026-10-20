/*
 * Enhanced NLM Python Bindings
 * 
 * This file provides enhanced Python bindings for the NLM (Neural Learning Machine)
 * framework with improved convenience functions, error handling, and Pythonic API patterns.
 * 
 * Key improvements:
 * 1. Pythonic API with snake_case method names
 * 2. High-level convenience functions for common operations
 * 3. Enhanced error handling with custom Python exceptions
 * 4. NumPy integration for vectorized operations
 * 5. Advanced agent functions with goal-directed behaviors
 * 6. Configuration presets and validation
 * 7. Debugging and monitoring tools
 * 8. Serialization improvements for checkpointing
 * 9. Threading and parallelization support
 * 10. Visualization and analysis tools
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <future>

// Forward declarations
namespace py = pybind11;

// Custom exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainInitializationError : public NLMError {
public:
    explicit BrainInitializationError(const std::string& msg) : NLMError("Brain initialization failed: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory error: " + msg) {}
};

// Forward declarations for NLM classes
namespace nlm {
    // Forward declare all needed classes
    class NeuronId;
    class SynapseId;
    class RegionId;
    class PopulationId;
    class NeuronType;
    class SynapseType;
    class DevelopmentalStage;
    class FiringState;
    class ActionType;
    class MotorCommand;
    class WorldObjectType;
    class Config;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class WorldObject;
    class AgentBody;
    class ActionResult;
    class SensoryPercept;
    class SimpleWorld;
    class Brain;
    class AgentBrain;
    class Neuron;
    class Synapse;
    class NeuralPopulation;
    class NeuralRegion;
    class NeuralDynamics;
    class SpikeSystem;
    class PlasticityRule;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    class DevelopmentSystem;
    class Synaptogenesis;
    class Pruning;
    class Maturation;
    class Neuromodulator;
    class Reward;
    class PredictionError;
    class Novelty;
    class Curiosity;
    class WorkingMemory;
    class EpisodicMemory;
    class SemanticMemory;
    class ProceduralMemory;
    class AssociativeMemory;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class ConceptFormation;
    class NeuralPlanner;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class PredictionSystem;
    class NeuralPrediction;
    class MotorSystem;
    class Environment;
    class Observation;
    class Experiments;
    class Experiment;
    class ExperimentRunner;
    class Metrics;
    class VisualizationInterface;
    
    // Function declarations
    std::shared_ptr<Config> createDefaultConfigImpl();
    std::shared_ptr<Brain> createBrainImpl(std::shared_ptr<Config> config);
    std::shared_ptr<SimpleWorld> createSimpleWorldImpl();
    std::shared_ptr<AgentBrain> createAgentBrainImpl(std::shared_ptr<Brain> brain);
    
    // Additional functions
    class SimulationMonitor;
    class BrainAnalyzer;
    class ConfigManager;
    class SessionManager;
    class ParallelRunner;
    class NumPyBridge;
}

// Enhanced Python bindings
namespace py = pybind11;
namespace nlm {

// Helper functions for enhanced bindings
namespace pybind_helpers {
    
    // Custom pickler for shared_ptr
    template<typename T>
    void register_shared_ptr(py::module& m, const std::string& name) {
        py::class_<std::shared_ptr<T>>(m, name.c_str())
            .def(py::init<>())
            .def(py::init<T*>())
            .def("get", &std::shared_ptr<T>::get)
            .def("reset", &std::shared_ptr<T>::reset);
    }
    
    // Safe conversion functions
    template<typename T>
    T* unsafe_get(py::handle obj) {
        auto* ptr = obj.cast<T*>();
        if (!ptr) throw NLMError("Null pointer conversion");
        return ptr;
    }
    
    // String utility functions
    std::string to_python_string(const std::string& str) {
        return str;
    }
    
    std::string format_summary(const std::string& prefix, const std::string& content) {
        return prefix + ": " + content;
    }
}

// Enhanced Configuration Management
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        -------------------------------------------------------
        
        An enhanced Python interface for the NLM C++ neural simulation framework.
        
        Features:
        - Pythonic API with snake_case method names
        - High-level convenience functions
        - Enhanced error handling with custom exceptions
        - NumPy integration for vectorized operations
        - Advanced agent functions with goal-directed behaviors
        - Configuration presets and validation
        - Debugging and monitoring tools
        - Serialization improvements for checkpointing
        - Threading and parallelization support
        - Visualization and analysis tools
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<MemoryError>(m, "MemoryError");

    // Register original exception for std::runtime_error
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // ====================================================================
    // Original bindings (maintained for backward compatibility)
    // ====================================================================

    // NeuronId
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

    // SynapseId
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

    // RegionId
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

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
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

    // Config class
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
        .def("clone", &Action::clone);

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

    // Brain class
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
             "Log brain status");

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Original factory functions (backward compatibility)
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

    // ====================================================================
    // Enhanced Pythonic API (NEW)
    // ====================================================================

    // Constants for configuration presets
    py::enum_<ConfigPreset>(m, "ConfigPreset", R"pbdoc(Configuration preset types)pbdoc")
        .value("Default", ConfigPreset::Default)
        .value("Explorer", ConfigPreset::Explorer)
        .value("Stable", ConfigPreset::Stable)
        .value("Balanced", ConfigPreset::Balanced)
        .value("Learner", ConfigPreset::Learner)
        .value("Dev", ConfigPreset::Dev)
        .export_values();

    // Simulation monitor class
    py::class_<nlm::SimulationMonitor>(m, "SimulationMonitor", R"pbdoc(Real-time simulation monitor)pbdoc")
        .def(py::init<>())
        .def("start_monitoring", &nlm::SimulationMonitor::startMonitoring,
             py::arg("brain"), py::arg("world"), py::arg("agent"),
             py::arg("interval") = 0.1)
        .def("stop_monitoring", &nlm::SimulationMonitor::stopMonitoring)
        .def("is_monitoring", &nlm::SimulationMonitor::isMonitoring)
        .def("get_status", &nlm::SimulationMonitor::getStatus)
        .def("should_stop", &nlm::SimulationMonitor::shouldStop)
        .def("wait", &nlm::SimulationMonitor::wait);

    // Brain analyzer class
    py::class_<nlm::BrainAnalyzer>(m, "BrainAnalyzer", R"pbdoc(Brain analysis and visualization tools)pbdoc")
        .def(py::init<>())
        .def("analyze_network_topology", &nlm::BrainAnalyzer::analyzeNetworkTopology,
             py::arg("brain"))
        .def("analyze_synaptic_dynamics", &nlm::BrainAnalyzer::analyzeSynapticDynamics,
             py::arg("brain"), py::arg("window") = 100)
        .def("analyze_neural_coherence", &nlm::BrainAnalyzer::analyzeNeuralCoherence,
             py::arg("brain"))
        .def("analyze_development_trajectory", &nlm::BrainAnalyzer::analyzeDevelopmentTrajectory,
             py::arg("brain"))
        .def("validate_brain_health", &nlm::BrainAnalyzer::validateBrainHealth,
             py::arg("brain"))
        .def("analyze_learning_curve", &nlm::BrainAnalyzer::analyzeLearningCurve,
             py::arg("brain"), py::arg("world"), py::arg("agent"));

    // Configuration manager class
    py::class_<nlm::ConfigManager>(m, "ConfigManager", R"pbdoc(Configuration management and presets)pbdoc")
        .def(py::init<>())
        .def("create_config_with_preset", &nlm::ConfigManager::createConfigWithPreset,
             py::arg("preset"), py::arg("overrides") = py::dict())
        .def("validate_config", &nlm::ConfigManager::validateConfig,
             py::arg("config"))
        .def("get_config_summary", &nlm::ConfigManager::getConfigSummary,
             py::arg("config"))
        .def("compare_configs", &nlm::ConfigManager::compareConfigs,
             py::arg("config1"), py::arg("config2"))
        .def("list_presets", &nlm::ConfigManager::listPresets)
        .def("create_custom_config", &nlm::ConfigManager::createCustomConfig,
             py::arg("settings"));

    // Session manager class
    py::class_<nlm::SessionManager>(m, "SessionManager", R"pbdoc(Simulation session management)pbdoc")
        .def(py::init<>())
        .def("create_session", &nlm::SessionManager::createSession,
             py::arg("world"), py::arg("agent"), py::arg("brain"))
        .def("save_checkpoint", &nlm::SessionManager::saveCheckpoint,
             py::arg("session_id"), py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("load_checkpoint", &nlm::SessionManager::loadCheckpoint,
             py::arg("checkpoint_name"), py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("list_sessions", &nlm::SessionManager::listSessions)
        .def("delete_session", &nlm::SessionManager::deleteSession,
             py::arg("session_id"))
        .def("get_session_info", &nlm::SessionManager::getSessionInfo,
             py::arg("session_id"));

    // Parallel runner class
    py::class_<nlm::ParallelRunner>(m, "ParallelRunner", R"pbdoc(Parallel simulation runner)pbdoc")
        .def(py::init<>())
        .def("run_parallel_episodes", &nlm::ParallelRunner::runParallelEpisodes,
             py::arg("num_episodes"), py::arg("agent_id_range"))
        .def("concurrent_brain_training", &nlm::ParallelRunner::concurrentBrainTraining,
             py::arg("brains"), py::arg("worlds"), py::arg("agents"))
        .def("set_max_workers", &nlm::ParallelRunner::setMaxWorkers,
             py::arg("num_workers"));

    // NumPy bridge class
    py::class_<nlm::NumPyBridge>(m, "NumPyBridge", R"pbdoc(NumPy integration for NLM)pbdoc")
        .def(py::init<>())
        .def("as_numpy_array", &nlm::NumPyBridge::asNumPyArray,
             py::arg("brain"))
        .def("from_numpy_array", &nlm::NumPyBridge::fromNumPyArray,
             py::arg("data"), py::arg("brain"))
        .def("inject_current_batch", &nlm::NumPyBridge::injectCurrentBatch,
             py::arg("brain"), py::arg("currents"), py::arg("neuron_indices"))
        .def("get_all_synaptic_weights", &nlm::NumPyBridge::getAllSynapticWeights,
             py::arg("brain"))
        .def("compute_brain_statistics", &nlm::NumPyBridge::computeBrainStatistics,
             py::arg("brain"), py::arg("world"), py::arg("agent"));

    // Enhanced agent functions
    py::class_<nlm::GoalDirectedAgent>(m, "GoalDirectedAgent", R"pbdoc(Goal-directed agent behavior)pbdoc")
        .def(py::init<>())
        .def("create_agent", &nlm::GoalDirectedAgent::createAgent,
             py::arg("brain"), py::arg("goals"))
        .def("evaluate_goals", &nlm::GoalDirectedAgent::evaluateGoals,
             py::arg("world"), py::arg("agent"))
        .def("update_goals", &nlm::GoalDirectedAgent::updateGoals,
             py::arg("world"), py::arg("agent"));

    py::class_<nlm::MemoryBasedAgent>(m, "MemoryBasedAgent", R"pbdoc(Memory-based agent behavior)pbdoc")
        .def(py::init<>())
        .def("create_agent", &nlm::MemoryBasedAgent::createAgent,
             py::arg("brain"), py::arg("memory_system"))
        .def("collect_experience", &nlm::MemoryBasedAgent::collectExperience,
             py::arg("world"), py::arg("agent"), py::arg("steps"))
        .def("recall_experience", &nlm::MemoryBasedAgent::recallExperience,
             py::arg("context"), py::arg("max_results"));

    py::class_<nlm::MetaLearningAgent>(m, "MetaLearningAgent", R"pbdoc(Meta-learning agent)pbdoc")
        .def(py::init<>())
        .def("create_agent", &nlm::MetaLearningAgent::createAgent,
             py::arg("brain"), py::arg("meta_params"))
        .def("meta_learn", &nlm::MetaLearningAgent::metaLearn,
             py::arg("world"), py::arg("agent"), py::arg("steps"))
        .def("get_meta_policy", &nlm::MetaLearningAgent::getMetaPolicy); // NEW FUNCTION

    // Enhanced neuromodulation classes
    py::class_<nlm::CuriosityDrivenExploration>(m, "CuriosityDrivenExploration", R"pbdoc(Curiosity-driven exploration)pbdoc")
        .def(py::init<>())
        .def("apply_curiosity_driven_exploration", &nlm::CuriosityDrivenExploration::applyCuriosityDrivenExploration,
             py::arg("agent"), py::arg("threshold"))
        .def("get_curiosity_policy", &nlm::CuriosityDrivenExploration::getCuriosityPolicy);

    py::class_<nlm::BayesianCuriosity>(m, "BayesianCuriosity", R"pbdoc(Bayesian curiosity model)pbdoc")
        .def(py::init<>())
        .def("create_bayesian_curiosity", &nlm::BayesianCuriosity::createBayesianCuriosity,
             py::arg("agent"), py::arg("model"))
        .def("update_curiosity_belief", &nlm::BayesianCuriosity::updateCuriosityBelief,
             py::arg("world"), py::arg("agent"))
        .def("get_curiosity_belief", &nlm::BayesianCuriosity::getCuriosityBelief);

    // Enhanced error handling
    py::class_<nlm::HErrorHandler>(m, "ErrorHandler", R"pbdoc(Comprehensive error handling)pbdoc")
        .def(py::init<>())
        .def("validate_simulation", &nlm::ErrorHandler::validateSimulation,
             py::arg("brain"), py::arg("world"), py::arg("agent"))
        .def("validate_brain_state", &nlm::ErrorHandler::validateBrainState,
             py::arg("brain"))
        .def("validate_config_state", &nlm::ErrorHandler::validateConfigState,
             py::arg("config"))
        .def("get_error_suggestions", &nlm::ErrorHandler::getErrorSuggestions,
             py::arg("error"));

    // Enhanced simulation utilities
    py::class_<nlm::SimulationUtils>(m, "SimulationUtils", R"pbdoc(High-level simulation utilities)pbdoc")
        .def(py::init<>())
        .def("run_simulation_with_monitoring", &nlm::SimulationUtils::runSimulationWithMonitoring,
             py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("max_steps"),
             py::arg("interval") = 0.1)
        .def("run_single_episode", &nlm::SimulationUtils::runSingleEpisode,
             py::arg("agent"), py::arg("world"), py::arg("max_steps"))
        .def("run_parallel_episodes", &nlm::SimulationUtils::runParallelEpisodes,
             py::arg("num_episodes"), py::arg("agent_id_range"))
        .def("create_multiple_brains", &nlm::SimulationUtils::createMultipleBrains,
             py::arg("count"), py::arg("**kwargs"))
        .def("run_batch_simulation", &nlm::SimulationUtils::runBatchSimulation,
             py::arg("brains"), py::arg("world"), py::arg("agent"));

    // ====================================================================
    // Factory functions for enhanced API
    // ====================================================================

    // Factory functions
    m.def("create_brain_with_config", [](ConfigPreset preset, py::kwargs kwargs) {
        auto config = nlm::createConfigWithPreset(preset, kwargs);
        return nlm::createBrainImpl(config);
    }, py::arg("preset"), py::arg("kwargs") = py::kwargs(),
       "Create a brain with a preset configuration");

    m.def("create_goal_directed_agent", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& goals) {
        return nlm::createGoalDirectedAgentImpl(brain, goals);
    }, py::arg("brain"), py::arg("goals"),
       "Create a goal-directed agent");

    m.def("create_memory_based_agent", [](std::shared_ptr<Brain> brain, std::shared_ptr<MemorySystem> memory) {
        return nlm::createMemoryBasedAgentImpl(brain, memory);
    }, py::arg("brain"), py::arg("memory"),
       "Create a memory-based agent");

    m.def("enable_curiosity_driven_exploration", [](std::shared_ptr<AgentBrain> agent, float threshold) {
        nlm::enableCuriosityDrivenExplorationImpl(agent, threshold);
    }, py::arg("agent"), py::arg("threshold"),
       "Enable curiosity-driven exploration");

    // Error handling factory functions
    m.def("get_error_handler", []() {
        return std::make_shared<nlm::ErrorHandler>();
    }, "Get error handler instance");

    m.def("get_simulation_utils", []() {
        return std::make_shared<nlm::SimulationUtils>();
    }, "Get simulation utils instance");

    m.def("get_brain_analyzer", []() {
        return std::make_shared<nlm::BrainAnalyzer>();
    }, "Get brain analyzer instance");

    m.def("get_config_manager", []() {
        return std::make_shared<nlm::ConfigManager>();
    }, "Get config manager instance");

    m.def("get_session_manager", []() {
        return std::make_shared<nlm::SessionManager>();
    }, "Get session manager instance");

    m.def("get_parallel_runner", []() {
        return std::make_shared<nlm::ParallelRunner>();
    }, "Get parallel runner instance");

    m.def("get_numpy_bridge", []() {
        return std::make_shared<nlm::NumPyBridge>();
    }, "Get NumPy bridge instance");

    // Enhanced monitoring and analysis
    m.def("start_brain_monitoring", [](std::shared_ptr<nlm::SimulationMonitor> monitor,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent,
                                     double interval) {
        monitor->startMonitoring(brain, world, agent, interval);
        return monitor;
    }, py::arg("monitor"), py::arg("brain"), py::arg("world"),
       py::arg("agent"), py::arg("interval") = 0.1,
       "Start brain monitoring");

    // Utility functions
    m.def("validate_brain_health", [](std::shared_ptr<Brain> brain) {
        auto analyzer = std::make_shared<nlm::BrainAnalyzer>();
        return analyzer->validateBrainHealth(brain);
    }, py::arg("brain"),
         "Validate brain health");

    m.def("analyze_brain_network", [](std::shared_ptr<Brain> brain) {
        auto analyzer = std::make_shared<nlm::BrainAnalyzer>();
        return analyzer->analyzeNetworkTopology(brain);
    }, py::arg("brain"),
         "Analyze brain network topology");

    m.def("analyze_synaptic_dynamics", [](std::shared_ptr<Brain> brain, int window) {
        auto analyzer = std::make_shared<nlm::BrainAnalyzer>();
        return analyzer->analyzeSynapticDynamics(brain, window);
    }, py::arg("brain"), py::arg("window") = 100,
         "Analyze synaptic dynamics");

    m.def("analyze_neural_coherence", [](std::shared_ptr<Brain> brain) {
        auto analyzer = std::make_shared<nlm::BrainAnalyzer>();
        return analyzer->analyzeNeuralCoherence(brain);
    }, py::arg("brain"),
         "Analyze neural coherence");

    m.def("analyze_development_trajectory", [](std::shared_ptr<Brain> brain) {
        auto analyzer = std::make_shared<nlm::BrainAnalyzer>();
        return analyzer->analyzeDevelopmentTrajectory(brain);
    }, py::arg("brain"),
         "Analyze development trajectory");

    m.def("analyze_learning_curve", [](std::shared_ptr<Brain> brain,
                                      std::shared_ptr<SimpleWorld> world,
                                      std::shared_ptr<AgentBrain> agent) {
        auto analyzer = std::make_shared<nlm::BrainAnalyzer>();
        return analyzer->analyzeLearningCurve(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
         "Analyze learning curve");

    // Enhanced serialization
    m.def("create_simulation_session", [](std::shared_ptr<SimpleWorld> world,
                                         std::shared_ptr<AgentBrain> agent,
                                         std::shared_ptr<Brain> brain) {
        auto session_manager = std::make_shared<nlm::SessionManager>();
        return session_manager->createSession(world, agent, brain);
    }, py::arg("world"), py::arg("agent"), py::arg("brain"),
         "Create simulation session");

    m.def("save_incremental_checkpoint", [](std::shared_ptr<nlm::SessionManager> session_manager,
                                          const std::string& session_id,
                                          std::shared_ptr<Brain> brain,
                                          std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent) {
        return session_manager->saveCheckpoint(session_id, brain, world, agent);
    }, py::arg("session_manager"), py::arg("session_id"),
       py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Save incremental checkpoint");

    m.def("load_from_checkpoint", [](std::shared_ptr<nlm::SessionManager> session_manager,
                                   const std::string& checkpoint_name,
                                   std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent) {
        session_manager->loadCheckpoint(checkpoint_name, brain, world, agent);
    }, py::arg("session_manager"), py::arg("checkpoint_name"),
       py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Load from checkpoint");

    m.def("list_simulation_sessions", [](std::shared_ptr<nlm::SessionManager> session_manager) {
        return session_manager->listSessions();
    }, py::arg("session_manager"),
         "List simulation sessions");

    // Configuration utilities
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        auto config_manager = std::make_shared<nlm::ConfigManager>();
        return config_manager->validateConfig(config);
    }, py::arg("config"),
         "Validate configuration");

    m.def("config_summary", [](std::shared_ptr<Config> config) {
        auto config_manager = std::make_shared<nlm::ConfigManager>();
        return config_manager->getConfigSummary(config);
    }, py::arg("config"),
         "Get configuration summary");

    m.def("config_compare", [](std::shared_ptr<Config> config1,
                              std::shared_ptr<Config> config2) {
        auto config_manager = std::make_shared<nlm::ConfigManager>();
        return config_manager->compareConfigs(config1, config2);
    }, py::arg("config1"), py::arg("config2"),
         "Compare configurations");

    m.def("list_config_presets", []() {
        auto config_manager = std::make_shared<nlm::ConfigManager>();
        return config_manager->listPresets();
    }, "List configuration presets");

    m.def("create_custom_config", [](const py::dict& settings) {
        auto config_manager = std::make_shared<nlm::ConfigManager>();
        return config_manager->createCustomConfig(settings);
    }, py::arg("settings"),
         "Create custom configuration");

    // NumPy utilities
    m.def("as_numpy_array", [](std::shared_ptr<Brain> brain) {
        auto numpy_bridge = std::make_shared<nlm::NumPyBridge>();
        return numpy_bridge->asNumPyArray(brain);
    }, py::arg("brain"),
         "Convert brain state to NumPy array");

    m.def("from_numpy_array", [](py::array data, std::shared_ptr<Brain> brain) {
        auto numpy_bridge = std::make_shared<nlm::NumPyBridge>();
        numpy_bridge->fromNumPyArray(data, brain);
    }, py::arg("data"), py::arg("brain"),
         "Import brain state from NumPy array");

    m.def("inject_current_batch", [](std::shared_ptr<Brain> brain,
                                    py::array currents,
                                    py::array neuron_indices) {
        auto numpy_bridge = std::make_shared<nlm::NumPyBridge>();
        numpy_bridge->injectCurrentBatch(brain, currents, neuron_indices);
    }, py::arg("brain"), py::arg("currents"), py::arg("neuron_indices"),
         "Inject current using NumPy arrays");

    m.def("get_all_synaptic_weights", [](std::shared_ptr<Brain> brain) {
        auto numpy_bridge = std::make_shared<nlm::NumPyBridge>();
        return numpy_bridge->getAllSynapticWeights(brain);
    }, py::arg("brain"),
         "Get all synaptic weights as NumPy array");

    m.def("compute_brain_statistics", [](std::shared_ptr<Brain> brain,
                                        std::shared_ptr<SimpleWorld> world,
                                        std::shared_ptr<AgentBrain> agent) {
        auto numpy_bridge = std::make_shared<nlm::NumPyBridge>();
        return numpy_bridge->computeBrainStatistics(brain, world, agent);
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
         "Compute brain statistics using NumPy");

    // Error handling utilities
    m.def("get_error_suggestions", [](const std::string& error) {
        auto error_handler = std::make_shared<nlm::ErrorHandler>();
        return error_handler->getErrorSuggestions(error);
    }, py::arg("error"),
         "Get error suggestions");

    // Parallel processing utilities
    m.def("run_parallel_episodes_utility", [](int num_episodes,
                                            const std::pair<int, int>& agent_id_range) {
        auto parallel_runner = std::make_shared<nlm::ParallelRunner>();
        return parallel_runner->runParallelEpisodes(num_episodes, agent_id_range);
    }, py::arg("num_episodes"), py::arg("agent_id_range"),
         "Run parallel episodes");

    m.def("concurrent_brain_training_utility", [](std::vector<std::shared_ptr<Brain>> brains,
                                                std::vector<std::shared_ptr<SimpleWorld>> worlds,
                                                std::vector<std::shared_ptr<AgentBrain>> agents) {
        auto parallel_runner = std::make_shared<nlm::ParallelRunner>();
        return parallel_runner->concurrentBrainTraining(brains, worlds, agents);
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"),
         "Run concurrent brain training");

    // Analysis and visualization utilities
    m.def("save_analysis_results", [](const py::dict& analysis,
                                     const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw MemoryError("Cannot open file for writing: " + filename);
        }
        
        // Convert analysis to JSON-like format
        file << "{\n";
        bool first = true;
        for (const auto& item : analysis) {
            if (!first) file << ",\n";
            file << "  \"" << item.first << "\": " << std::to_string(item.second.cast<double>());
            first = false;
        }
        file << "\n}\n";
        file.close();
    }, py::arg("analysis"), py::arg("filename"),
         "Save analysis results to file");

    m.def("validate_brain_health", [](std::shared_ptr<Brain> brain) {
        auto error_handler = std::make_shared<nlm::ErrorHandler>();
        return error_handler->validateBrainState(brain);
    }, py::arg("brain"),
         "Validate brain health");

    m.def("validate_config_state", [](std::shared_ptr<Config> config) {
        auto error_handler = std::make_shared<nlm::ErrorHandler>();
        return error_handler->validateConfigState(config);
    }, py::arg("config"),
         "Validate configuration state");

    // Metadata and information
    m.attr("__version__") = "2.0.0";  // Enhanced version
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced features";

    // Documentation
    m.def("get_help", []() {
        return R"pbdoc(
Enhanced NLM Python API Help

This enhanced API provides:

1. Basic API (Backward Compatible):
   - createBrain(config)
   - createSimpleWorld()
   - createAgentBrain(brain)
   - All original methods with same names

2. Enhanced Convenience API:
   - create_brain_with_config(preset, **kwargs)
   - run_simulation_with_monitoring(brain, world, agent, max_steps, interval)
   - run_single_episode(agent, world, max_steps)
   - create_multiple_brains(count, **kwargs)
   - run_batch_simulation(brains, world, agent)

3. NumPy Integration:
   - as_numpy_array(brain)
   - from_numpy_array(data, brain)
   - inject_current_batch(brain, currents, neuron_indices)
   - get_all_synaptic_weights(brain)
   - compute_brain_statistics(brain, world, agent)

4. Advanced Agent Functions:
   - create_goal_directed_agent(brain, goals)
   - create_memory_based_agent(brain, memory_system)
   - enable_curiosity_driven_exploration(agent, threshold)

5. Configuration Management:
   - validate_config(config)
   - config_summary(config)
   - config_compare(config1, config2)
   - list_config_presets()
   - create_custom_config(settings)

6. Debugging and Analysis:
   - analyze_brain_network(brain)
   - analyze_synaptic_dynamics(brain, window)
   - analyze_neural_coherence(brain)
   - analyze_development_trajectory(brain)
   - analyze_learning_curve(brain, world, agent)

7. Serialization and Persistence:
   - create_simulation_session(world, agent, brain)
   - save_incremental_checkpoint(session_manager, session_id, brain, world, agent)
   - load_from_checkpoint(session_manager, checkpoint_name, brain, world, agent)
   - list_simulation_sessions(session_manager)

8. Threading and Parallelization:
   - run_parallel_episodes_utility(num_episodes, agent_id_range)
   - concurrent_brain_training_utility(brains, worlds, agents)

9. Error Handling:
   - Comprehensive error classes: NLMError, BrainInitializationError, SimulationError, etc.
   - get_error_suggestions(error)
   - validate_brain_health(brain)
   - validate_config_state(config)

10. Utility Functions:
    - get_error_handler()
    - get_simulation_utils()
    - get_brain_analyzer()
    - get_config_manager()
    - get_session_manager()
    - get_parallel_runner()
    - get_numpy_bridge()

Usage examples are available in the documentation files.
        )pbdoc";
    }, "Get help and documentation");

    // Version information
    m.def("get_version", []() {
        return "2.0.0";
    }, "Get current version");

    m.def("get_feature_list", []() {
        return std::vector<std::string>{
            "Basic API (backward compatible)",
            "Enhanced convenience functions",
            "NumPy integration",
            "Advanced agent behaviors",
            "Configuration presets",
            "Brain analysis and monitoring",
            "Serialization and persistence",
            "Parallel processing",
            "Comprehensive error handling",
            "Visualization and analysis tools"
        };
    }, "Get list of available features");
}
