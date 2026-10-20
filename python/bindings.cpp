#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Core NLM includes
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

// Brain components
#include "../src/brain/Brain.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"

// Memory systems
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"

// Neuromodulation
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"

// Development
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

// Plasticity
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

// Dynamics
#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"

// Visualization
#include "../src/visualization/VisualizationInterface.hpp"

// Cognition
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"

// Prediction
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

// Sensory
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Motor
#include "../src/motor/MotorSystem.hpp"
#include "../src/motor/Action.hpp"

// Environment
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

// World
#include "../src/world/SimpleWorld.hpp"

// Agent
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Experiment
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

// Performance
#include "../src/performance/PerformanceMonitor.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python API
        ------------------------------------------------------
        Advanced Python bindings for the NLM C++ neural simulation framework.
        
        This enhanced API provides comprehensive access to all neural systems
        including memory, neuromodulation, development, plasticity, cognition,
        prediction, and advanced simulation control for expert users.
        
        Key Features:
        - Complete memory system (Episodic, Working, Semantic, Neural Working)
        - Neuromodulation (Reward, Prediction Error, Novelty, Curiosity,
          Dopamine, Acetylcholine, Norepinephrine, Serotonin)
        - Development (Synaptogenesis, Pruning, Maturation, DevelopmentSystem)
        - Advanced processing (Neural Dynamics, Spike System, Plasticity Rules)
        - Visualization interface
        - Brain components (Prediction System, Concept Formation, Neural Planner)
        - Advanced configuration management
        - Simulation control and monitoring
        - Plugin/extension system
        - Checkpoint management
        - Performance monitoring
        - Advanced learning capabilities
        - Command-line interface
        
        For detailed documentation, visit https://nlm.readthedocs.io
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===========================================
    // CORE TYPES AND ENUMERATIONS
    // ===========================================
    
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

    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

    // ===========================================
    // BASIC TYPES (IDs AND IDENTIFIERS)
    // ===========================================
    
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

    // ===========================================
    // CONFIGURATION SYSTEM
    // ===========================================
    
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system for NLM)pbdoc")
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
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.getAny(key);
            if (value.has_value()) {
                return py::cast(value.value());
            }
            return py::none();
        }, py::arg("key"), "Get configuration value (returns None if not found)")
        .def("getOr", [](Config& self, const std::string& key, auto&& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, const std::vector<int>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, const std::vector<double>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("set", [](Config& self, const std::string& key, const std::vector<std::string>& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===========================================
    // SENSORY INPUT SYSTEM
    // ===========================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // ===========================================
    // MOTOR OUTPUT SYSTEM
    // ===========================================
    
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ===========================================
    // WORLD SYSTEM
    // ===========================================
    
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

    // ===========================================
    // NEURAL PROCESSING CORE
    // ===========================================
    
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
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity rule")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getNeuralPlanner", &Brain::getNeuralPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getAttentionalSelection", &Brain::getAttentionalSelection,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getVisualizationInterface", &Brain::getVisualizationInterface,
             py::return_value_policy::reference_internal,
             "Get the visualization interface")
        .def("getNeuromodulation", &Brain::getNeuromodulation,
             py::return_value_policy::reference_internal,
             "Get the neuromodulation system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system")
        .def("getPredictionError", &Brain::getPredictionError,
             py::return_value_policy::reference_internal,
             "Get the prediction error system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine system")
        .def("getAcetylcholine", &Brain::getAcetylcholine,
             py::return_value_policy::reference_internal,
             "Get the acetylcholine system")
        .def("getNorepinephrine", &Brain::getNorepinephrine,
             py::return_value_policy::reference_internal,
             "Get the norepinephrine system")
        .def("getSerotonin", &Brain::getSerotonin,
             py::return_value_policy::reference_internal,
             "Get the serotonin system")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("getSimulationTime", &Brain::getSimulationTime,
             "Get current simulation time")
        .def("getRandomSeed", &Brain::getRandomSeed,
             "Get the random seed")
        .def("setRandomSeed", &Brain::setRandomSeed, py::arg("seed"),
             "Set the random seed")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition ratio")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get firing neuron count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get active neuron count")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get region count")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get developmental stage");

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

    // ===========================================
    // MEMORY SYSTEMS
    // ===========================================
    
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("storePattern", &NeuralWorkingMemory::storePattern, py::arg("pattern"), py::arg("strength"))
        .def("recallPattern", &NeuralWorkingMemory::recallPattern, py::arg("pattern"), py::arg("threshold") = 0.5f)
        .def("getPatternStrength", &NeuralWorkingMemory::getPatternStrength, py::arg("pattern"))
        .def("updatePattern", &NeuralWorkingMemory::updatePattern, py::arg("pattern"), py::arg("newStrength"))
        .def("removePattern", &NeuralWorkingMemory::removePattern, py::arg("pattern"))
        .def("getActivePatterns", &NeuralWorkingMemory::getActivePatterns)
        .def("getPatternCount", &NeuralWorkingMemory::getPatternCount)
        .def("getTotalCapacity", &NeuralWorkingMemory::getTotalCapacity)
        .def("getUtilization", &NeuralWorkingMemory::getUtilization)
        .def("clear", &NeuralWorkingMemory::clear)
        .def("step", &NeuralWorkingMemory::step, py::arg("dt"))
        .def("competePatterns", &NeuralWorkingMemory::competePatterns)
        .def(" consolidate", &NeuralWorkingMemory:: consolidate)
        .def("getReplayCandidates", &NeuralWorkingMemory::getReplayCandidates)
        .def("replayPattern", &NeuralWorkingMemory::replayPattern, py::arg("patternId"))
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"))
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("getLoyalty", &NeuralWorkingMemory::getLoyalty, py::arg("patternId"))
        .def("setLoyalty", &NeuralWorkingMemory::setLoyalty, py::arg("patternId"), py::arg("loyalty"))
        .def("getTemporalContext", &NeuralWorkingMemory::getTemporalContext)
        .def("setTemporalContext", &NeuralWorkingMemory::setTemporalContext, py::arg("context"));

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("recordExperience", &NeuralEpisodicMemory::recordExperience, py::arg("eventId"), py::arg("features"), py::arg("timestamp"), py::arg("reward"))
        .def("replayExperience", &NeuralEpisodicMemory::replayExperience, py::arg("eventId"), py::arg("replaySpeed"))
        .def("getEventFeatures", &NeuralEpisodicMemory::getEventFeatures, py::arg("eventId"))
        .def("getEventReward", &NeuralEpisodicMemory::getEventReward, py::arg("eventId"))
        .def("getEventTimestamp", &NeuralEpisodicMemory::getEventTimestamp, py::arg("eventId"))
        .def("getEventImportance", &NeuralEpisodicMemory::getEventImportance, py::arg("eventId"))
        .def("getRecentEvents", &NeuralEpisodicMemory::getRecentEvents, py::arg("limit"))
        .def("getAllEventIds", &NeuralEpisodicMemory::getAllEventIds)
        .def("deleteEvent", &NeuralEpisodicMemory::deleteEvent, py::arg("eventId"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("step", &NeuralEpisodicMemory::step, py::arg("dt"))
        .def("consolidateMemories", &NeuralEpisodicMemory::consolidateMemories)
        .def("getConsolidationCandidates", &NeuralEpisodicMemory::getConsolidationCandidates)
        .def("getMemoryLoad", &NeuralEpisodicMemory::getMemoryLoad)
        .def("getActiveReplayNeurons", &NeuralEpisodicMemory::getActiveReplayNeurons)
        .def("setReplayRate", &NeuralEpisodicMemory::setReplayRate, py::arg("rate"))
        .def("getReplayRate", &NeuralEpisodicMemory::getReplayRate)
        .def("setConsolidationThreshold", &NeuralEpisodicMemory::setConsolidationThreshold, py::arg("threshold"))
        .def("getConsolidationThreshold", &NeuralEpisodicMemory::getConsolidationThreshold);

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def(py::init<>())
        .def("associatePatterns", &NeuralAssociativeMemory::associatePatterns, py::arg("pattern1"), py::arg("pattern2"), py::arg("strength"))
        .def("recallPattern", &NeuralAssociativeMemory::recallPattern, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("pattern1"), py::arg("pattern2"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("pattern1"), py::arg("pattern2"), py::arg("newStrength"))
        .def("removeAssociation", &NeuralAssociativeMemory::removeAssociation, py::arg("pattern1"), py::arg("pattern2"))
        .def("getAssociatedPatterns", &NeuralAssociativeMemory::getAssociatedPatterns, py::arg("pattern"))
        .def("getPatternCompleteness", &NeuralAssociativeMemory::getPatternCompleteness, py::arg("pattern"))
        .def("getAllPatternPairs", &NeuralAssociativeMemory::getAllPatternPairs)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("step", &NeuralAssociativeMemory::step, py::arg("dt"))
        .def("getPatternSimilarity", &NeuralAssociativeMemory::getPatternSimilarity, py::arg("pattern1"), py::arg("pattern2"))
        .def("getNetworkDensity", &NeuralAssociativeMemory::getNetworkDensity)
        .def("getWeakAssociations", &NeuralAssociativeMemory::getWeakAssociations, py::arg("threshold"))
        .def("getStrongAssociations", &NeuralAssociativeMemory::getStrongAssociations, py::arg("threshold"));

    // ===========================================
    // NEUROMODULATION SYSTEM
    // ===========================================
    
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def(py::init<>())
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    py::class_<Acetylcholine>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Acetylcholine::getName)
        .def("getLevel", &Acetylcholine::getLevel)
        .def("setLevel", &Acetylcholine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Acetylcholine::getPlasticityFactor)
        .def("update", &Acetylcholine::update, py::arg("dt"));

    py::class_<Norepinephrine>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Norepinephrine::getName)
        .def("getLevel", &Norepinephrine::getLevel)
        .def("setLevel", &Norepinephrine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Norepinephrine::getPlasticityFactor)
        .def("update", &Norepinephrine::update, py::arg("dt"));

    py::class_<Serotonin>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Serotonin::getName)
        .def("getLevel", &Serotonin::getLevel)
        .def("setLevel", &Serotonin::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Serotonin::getPlasticityFactor)
        .def("update", &Serotonin::update, py::arg("dt"));

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue, "Get current reward value")
        .def("setValue", &Reward::setValue, py::arg("value"), "Set current reward value")
        .def("add", &Reward::add, py::arg("delta"), "Accumulate reward")
        .def("reset", &Reward::reset, "Reset accumulated reward")
        .def("computeReward", &Reward::computeReward, py::arg("observation"), "Compute reward from observation")
        .def("getHistory", &Reward::getHistory, "Get reward history")
        .def("clearHistory", &Reward::clearHistory, "Clear reward history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getError", &PredictionError::getError, "Get error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"), "Update prediction")
        .def("getHistory", &PredictionError::getHistory, "Get error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get error magnitude")
        .def("isSurprising", &PredictionError::isSurprising, "Check if surprising")
        .def("setSurpriseThreshold", &PredictionError::setSurpriseThreshold, py::arg("threshold"), "Set surprise threshold")
        .def("getSurpriseThreshold", &PredictionError::getSurpriseThreshold, "Get surprise threshold");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detectNovelty", [](Novelty& self, const Observation& obs, const Observation& prev) {
            self.detectNovelty(obs, prev);
        }, py::arg("observation"), py::arg("previousObservation"), "Detect novelty from observations")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& current, const std::vector<float>& previous) {
            self.detectNovelty(current, previous);
        }, py::arg("currentPattern"), py::arg("previousPattern"), "Detect novelty from patterns")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"), "Set curiosity level")
        .def("detectSurprise", &Curiosity::detectSurprise, py::arg("prediction"), py::arg("observation"), "Detect surprise")
        .def("update", &Curiosity::update, py::arg("dt"), "Update curiosity over time")
        .def("getHistory", &Curiosity::getHistory, "Get curiosity history")
        .def("clearHistory", &Curiosity::clearHistory, "Clear curiosity history");

    // ===========================================
    // DEVELOPMENT SYSTEM
    // ===========================================
    
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis development process)pbdoc")
        .def(py::init<>())
        .def("startSynaptogenesis", &Synaptogenesis::startSynaptogenesis)
        .def("stopSynaptogenesis", &Synaptogenesis::stopSynaptogenesis)
        .def("getRate", &Synaptogenesis::getRate)
        .def("setRate", &Synaptogenesis::setRate, py::arg("rate"))
        .def("getTotalSynapsesCreated", &Synaptogenesis::getTotalSynapsesCreated)
        .def("getCurrentRate", &Synaptogenesis::getCurrentRate)
        .def("updateRate", &Synaptogenesis::updateRate, py::arg("dt"), py::arg("age"))
        .def("isActive", &Synaptogenesis::isActive);

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning development process)pbdoc")
        .def(py::init<>())
        .def("startPruning", &Pruning::startPruning)
        .def("stopPruning", &Pruning::stopPruning)
        .def("getRate", &Pruning::getRate)
        .def("setRate", &Pruning::setRate, py::arg("rate"))
        .def("getTotalSynapsesRemoved", &Pruning::getTotalSynapsesRemoved)
        .def("getCurrentRate", &Pruning::getCurrentRate)
        .def("updateRate", &Pruning::updateRate, py::arg("dt"), py::arg("age"))
        .def("isActive", &Pruning::isActive);

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Maturation development process)pbdoc")
        .def(py::init<>())
        .def("startMaturation", &Maturation::startMaturation)
        .def("stopMaturation", &Maturation::stopMaturation)
        .def("getRate", &Maturation::getRate)
        .def("setRate", &Maturation::setRate, py::arg("rate"))
        .def("getMaturationLevel", &Maturation::getMaturationLevel)
        .def("getCurrentRate", &Maturation::getCurrentRate)
        .def("updateRate", &Maturation::updateRate, py::arg("dt"), py::arg("age"))
        .def("isMature", &Maturation::isMature);

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Overall development system)pbdoc")
        .def(py::init<>())
        .def("startDevelopment", &DevelopmentSystem::startDevelopment)
        .def("stopDevelopment", &DevelopmentSystem::stopDevelopment)
        .def("update", &DevelopmentSystem::update, py::arg("dt"))
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("getAge", &DevelopmentSystem::getAge)
        .def("getCurrentStageRate", &DevelopmentSystem::getCurrentStageRate)
        .def("getTotalSynaptogenesisEvents", &DevelopmentSystem::getTotalSynaptogenesisEvents)
        .def("getTotalPruningEvents", &DevelopmentSystem::getTotalPruningEvents)
        .def("getTotalMaturationEvents", &DevelopmentSystem::getTotalMaturationEvents)
        .def("getDevelopmentEfficiency", &DevelopmentSystem::getDevelopmentEfficiency)
        .def("setExternalStimulus", &DevelopmentSystem::setExternalStimulus, py::arg("stimulus"))
        .def("getExternalStimulus", &DevelopmentSystem::getExternalStimulus)
        .def("addExperience", &DevelopmentSystem::addExperience, py::arg("experience"));

    // ===========================================
    // PLASTICITY SYSTEM
    // ===========================================
    
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for plasticity rules)pbdoc")
        .def(py::init<>())
        .def("update", &PlasticityRule::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &PlasticityRule::applyWeightChange,
             py::arg("synapse"), py::arg("delta"))
        .def("getName", &PlasticityRule::getName)
        .def("getLearningRate", &PlasticityRule::getLearningRate)
        .def("setLearningRate", &PlasticityRule::setLearningRate, py::arg("rate"));

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning implementation)pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &Hebbian::applyWeightChange,
             py::arg("synapse"), py::arg("delta"))
        .def("getName", &Hebbian::getName)
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &Hebbian::getLearningRate)
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"))
        .def("getMaxWeight", &Hebbian::getMaxWeight);

    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &STDP::applyWeightChange,
             py::arg("synapse"), py::arg("delta"))
        .def("getName", &STDP::getName)
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"))
        .def("getLTPWeight", &STDP::getLTPWeight)
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"))
        .def("getLTDWeight", &STDP::getLTDWeight)
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"))
        .def("getTimeConstant", &STDP::getTimeConstant)
        .def("configure", &STDP::configure,
             py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"));

    py::class_<RewardModulatedSTDP>(m, "RewardModulatedSTDP", R"pbdoc(Reward-modulated STDP)pbdoc")
        .def(py::init<>())
        .def("update", &RewardModulatedSTDP::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &RewardModulatedSTDP::applyWeightChange,
             py::arg("synapse"), py::arg("delta"))
        .def("getName", &RewardModulatedSTDP::getName);

    py::class_<TripletSTDP>(m, "TripletSTDP", R"pbdoc(Triplet STDP)pbdoc")
        .def(py::init<>())
        .def("update", &TripletSTDP::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"))
        .def("applyWeightChange", &TripletSTDP::applyWeightChange,
             py::arg("synapse"), py::arg("delta"))
        .def("getName", &TripletSTDP::getName);

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity implementation)pbdoc")
        .def(py::init<>())
        .def("addNewConnection", &StructuralPlasticity::addNewConnection,
             py::arg("preNeuron"), py::arg("postNeuron"), py::arg("type"))
        .def("removeConnection", &StructuralPlasticity::removeConnection,
             py::arg("synapseId"))
        .def("updateStructure", &StructuralPlasticity::updateStructure, py::arg("dt"))
        .def("getConnectionCount", &StructuralPlasticity::getConnectionCount)
        .def("getGrowthRate", &StructuralPlasticity::getGrowthRate)
        .def("setGrowthRate", &StructuralPlasticity::setGrowthRate, py::arg("rate"))
        .def("getPruneRate", &StructuralPlasticity::getPruneRate)
        .def("setPruneRate", &StructuralPlasticity::setPruneRate, py::arg("rate"))
        .def("getTotalNewConnections", &StructuralPlasticity::getTotalNewConnections)
        .def("getTotalPrunedConnections", &StructuralPlasticity::getTotalPrunedConnections);

    // ===========================================
    // DYNAMICS SYSTEM
    // ===========================================
    
    py::class_<NeuralDynamics>(m, "NeuralDynamics", R"pbdoc(Neural dynamics system)pbdoc")
        .def(py::init<>())
        .def("step", &NeuralDynamics::step,
             py::arg("neurons"), py::arg("dt"))
        .def("updateMembranePotential", &NeuralDynamics::updateMembranePotential,
             py::arg("neuron"), py::arg("currents"), py::arg("dt"))
        .def("updateSynapticWeights", &NeuralDynamics::updateSynapticWeights,
             py::arg("synapses"), py::arg("neuronStates"), py::arg("dt"))
        .def("getFiringNeurons", &NeuralDynamics::getFiringNeurons)
        .def("getActiveNeurons", &NeuralDynamics::getActiveNeurons)
        .def("getExcitationInhibitionRatio", &NeuralDynamics::getExcitationInhibitionRatio)
        .def("getAverageFiringRate", &NeuralDynamics::getAverageFiringRate)
        .def("getTotalSpikeCount", &NeuralDynamics::getTotalSpikeCount);

    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike system for neural events)pbdoc")
        .def(py::init<>())
        .def("processSpike", &SpikeSystem::processSpike,
             py::arg("neuronId"), py::arg("time"))
        .def("getPendingSpikes", &SpikeSystem::getPendingSpikes)
        .def("getSpikeCount", &SpikeSystem::getSpikeCount)
        .def("getLastSpikeTime", &SpikeSystem::getLastSpikeTime)
        .def("reset", &SpikeSystem::reset)
        .def("setRefractoryPeriod", &SpikeSystem::setRefractoryPeriod, py::arg("period"))
        .def("getRefractoryPeriod", &SpikeSystem::getRefractoryPeriod);

    // ===========================================
    // VISUALIZATION SYSTEM
    // ===========================================
    
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface)pbdoc")
        .def(py::init<>())
        .def("update", &VisualizationInterface::update,
             py::arg("brain"), py::arg("timestep"))
        .def("render", &VisualizationInterface::render,
             py::arg("outputType"), py::arg("outputPath"))
        .def("enableVisualization", &VisualizationInterface::enableVisualization, py::arg("enable"))
        .def("isVisualizationEnabled", &VisualizationInterface::isVisualizationEnabled)
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("rate"))
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate)
        .def("addRenderer", &VisualizationInterface::addRenderer, py::arg("renderer"))
        .def("removeRenderer", &VisualizationInterface::removeRenderer, py::arg("name"))
        .def("getRenderer", &VisualizationInterface::getRenderer, py::arg("name"));

    // ===========================================
    // COGNITION SYSTEMS
    // ===========================================
    
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("processObservation", &ConceptFormation::processObservation, py::arg("observation"))
        .def("getConceptVector", &ConceptFormation::getConceptVector, py::arg("conceptId"))
        .def("getConceptSimilarity", &ConceptFormation::getConceptSimilarity, py::arg("concept1"), py::arg("concept2"))
        .def("getActiveConcepts", &ConceptFormation::getActiveConcepts)
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("updateConceptWeights", &ConceptFormation::updateConceptWeights, py::arg("dt"))
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"));

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planning system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &NeuralPlanner::planAction, py::arg("goal"), py::arg("context"))
        .def("evaluateAction", &NeuralPlanner::evaluateAction, py::arg("action"), py::arg("context"))
        .def("getActionSequence", &NeuralPlanner::getActionSequence)
        .def("updatePlans", &NeuralPlanner::updatePlans, py::arg("dt"))
        .def("getPlanConfidence", &NeuralPlanner::getPlanConfidence)
        .def("getAlternativePlans", &NeuralPlanner::getAlternativePlans, py::arg("limit") = 5)
        .def("getPlanningTime", &NeuralPlanner::getPlanningTime)
        .def("setPlanningHorizon", &NeuralPlanner::setPlanningHorizon, py::arg("horizon"));

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection system)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("selectFocus", &AttentionalSelection::selectFocus, py::arg("stimuli"))
        .def("updateAttention", &AttentionalSelection::updateAttention, py::arg("dt"))
        .def("getCurrentFocus", &AttentionalSelection::getCurrentFocus)
        .def("getAttentionWeights", &AttentionalSelection::getAttentionWeights)
        .def("getAttentionStrength", &AttentionalSelection::getAttentionStrength)
        .def("setAttentionBias", &AttentionalSelection::setAttentionBias, py::arg("bias"))
        .def("getSalience", &AttentionalSelection::getSalience, py::arg("stimulusId"));

    // ===========================================
    // PREDICTION SYSTEM
    // ===========================================
    
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"))
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"))
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getConfidence", &PredictionSystem::getConfidence)
        .def("getErrorHistory", &PredictionSystem::getErrorHistory)
        .def("clearHistory", &PredictionSystem::clearHistory)
        .def("train", &PredictionSystem::train, py::arg("observation"))
        .def("setPredictionHorizon", &PredictionSystem::setPredictionHorizon, py::arg("steps"))
        .def("getPredictionHorizon", &PredictionSystem::getPredictionHorizon);

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"))
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState, py::arg("sensoryState"), py::arg("currentStep"))
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("currentStep"))
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation, py::arg("actualState"), py::arg("currentStep"))
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence, py::arg("action"), py::arg("currentState"))
        .def("getPredictionError", &NeuralPrediction::getPredictionError)
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence)
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps, py::arg("currentStep"), py::arg("numSteps"))
        .def("recordAction", &NeuralPrediction::recordAction, py::arg("action"), py::arg("step"))
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory)
        .def("clearHistory", &NeuralPrediction::clearHistory)
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"))
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"))
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"))
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("steps"))
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons)
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons);

    // ===========================================
    // FACTORY FUNCTIONS
    // ===========================================
    
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

    // ===========================================
    // ADVANCED PYTHON FEATURES
    // ===========================================
    
    // Plugin system
    py::class_<PluginManager>(m, "PluginManager", R"pbdoc(Plugin manager for NLM extensions)pbdoc")
        .def(py::init<>())
        .def("loadPlugin", &PluginManager::loadPlugin, py::arg("path"), py::arg("config") = py::none())
        .def("enablePlugin", &PluginManager::enablePlugin, py::arg("name"))
        .def("disablePlugin", &PluginManager::disablePlugin, py::arg("name"))
        .def("executePlugin", [](PluginManager& self, const std::string& name, const py::dict& context) {
            return self.executePlugin(name, py::cast<std::map<std::string, std::string>>(context));
        })
        .def("executeAllPlugins", [](PluginManager& self, const py::dict& context) {
            return self.executeAllPlugins(py::cast<std::map<std::string, std::string>>(context));
        })
        .def("removePlugin", &PluginManager::removePlugin, py::arg("name"))
        .def("listPlugins", &PluginManager::listPlugins)
        .def("savePlugins", &PluginManager::savePlugins, py::arg("path"))
        .def("loadPlugins", &PluginManager::loadPlugins, py::arg("path"));

    py::class_<PluginInterface, std::enable_shared_from_this<PluginInterface>>(m, "PluginInterface", R"pbdoc(Abstract base class for plugins)pbdoc")
        .def("initialize", &PluginInterface::initialize, py::arg("brain_interface"))
        .def("execute", [](const PluginInterface& self, const py::dict& context) {
            return self.execute(py::cast<std::map<std::string, std::string>>(context));
        })
        .def("cleanup", &PluginInterface::cleanup)
        .def("getInfo", &PluginInterface::getInfo);

    // Performance monitoring
    py::class_<PerformanceMonitor>(m, "PerformanceMonitor", R"pbdoc(Performance monitoring system)pbdoc")
        .def(py::init<>())
        .def("startMonitoring", &PerformanceMonitor::startMonitoring)
        .def("stopMonitoring", &PerformanceMonitor::stopMonitoring)
        .def("recordMetric", &PerformanceMonitor::recordMetric, py::arg("name"), py::arg("value"), py::arg("timestamp"))
        .def("getMetric", &PerformanceMonitor::getMetric, py::arg("name"))
        .def("getMetrics", &PerformanceMonitor::getMetrics)
        .def("getSystemStats", &PerformanceMonitor::getSystemStats)
        .def("generateReport", &PerformanceMonitor::generateReport);

    // Checkpoint management
    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint management system)pbdoc")
        .def(py::init<>())
        .def("save", &CheckpointManager::save, py::arg("name"), py::arg("data"))
        .def("load", &CheckpointManager::load, py::arg("name"))
        .def("has", &CheckpointManager::has, py::arg("name"))
        .def("list", &CheckpointManager::list)
        .def("delete", &CheckpointManager::delete, py::arg("name"))
        .def("clear", &CheckpointManager::clear)
        .def("backup", &CheckpointManager::backup, py::arg("output_dir"))
        .def("restore", &CheckpointManager::restore, py::arg("backup_file"));

    // Simulation control
    py::class_<SimulationManager>(m, "SimulationManager", R"pbdoc(Simulation control system)pbdoc")
        .def(py::init<>())
        .def("setSimulationSpeed", &SimulationManager::setSimulationSpeed, py::arg("speed"))
        .def("getSimulationSpeed", &SimulationManager::getSimulationSpeed)
        .def("pause", &SimulationManager::pause)
        .def("resume", &SimulationManager::resume)
        .def("isPaused", &SimulationManager::isPaused)
        .def("step", &SimulationManager::step, py::arg("steps") = 1)
        .def("getStats", &SimulationManager::getStats)
        .def("getTimeline", &SimulationManager::getTimeline)
        .def("exportTimeline", &SimulationManager::exportTimeline, py::arg("output_path"))
        .def("importTimeline", &SimulationManager::importTimeline, py::arg("input_path"));

    // Learning manager
    py::class_<LearningManager>(m, "LearningManager", R"pbdoc(Advanced learning system)pbdoc")
        .def(py::init<>())
        .def("startLearning", &LearningManager::startLearning)
        .def("stopLearning", &LearningManager::stopLearning)
        .def("updateLearning", &LearningManager::updateLearning, py::arg("dt"))
        .def("getLearningRate", &LearningManager::getLearningRate)
        .def("setLearningRate", &LearningManager::setLearningRate, py::arg("rate"))
        .def("getLearningStats", &LearningManager::getLearningStats)
        .def("getPerformanceHistory", &LearningManager::getPerformanceHistory)
        .def("getAdaptationPattern", &LearningManager::getAdaptationPattern)
        .def("configureLearning", &LearningManager::configureLearning, py::arg("strategy"), py::arg("params"))
        .def("evaluatePerformance", &LearningManager::evaluatePerformance, py::arg("metrics"));

    // ===========================================
    // VALIDATION AND ERROR HANDLING
    // ===========================================
    
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===========================================
    // IDENTIFIERS
    // ===========================================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===========================================
    // EASY ACCESS FUNCTIONS
    // ===========================================
    
    m.def("runPhase6Demo", []() {
        // Phase 6 demonstration
        auto config = createDefaultConfig();
        config->set("simulation.timestep", 0.001);
        config->set("simulation.max_steps", 1000);
        config->set("brain.neuron_count", 100);
        config->set("brain.region_count", 2);
        config->set("environment.width", 10);
        config->set("environment.height", 10);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (size_t step = 0; step < 100; ++step) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 10 == 0) {
                std::cout << "Step " << step << ": " 
                         << "Firing neurons: " << brain->getFiringNeuronCount() 
                         << ", Total spikes: " << brain->getTotalSpikeCount()
                         << std::endl;
            }
        }
        
        std::cout << "\nPhase 6 Demo completed successfully!" << std::endl;
        std::cout << "Statistics:" << std::endl;
        std::cout << "  - Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  - Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  - Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  - Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  - Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    }, "Run Phase 6 integration demo");
}

} // namespace nlm