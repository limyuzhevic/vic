// NLM Python Bindings - Comprehensive High-Level API
// File: python/nlm_python_bindings.cpp

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <sstream>

// Include original NLM headers
#include "nlm_python_bindings.h"
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

// Helper functions for type conversions
std::string developmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

DevelopmentalStage stringToDevelopmentalStage(const std::string& stage) {
    if (stage == "CriticalPeriod") return DevelopmentalStage::CriticalPeriod;
    if (stage == "Maturation") return DevelopmentalStage::Maturation;
    if (stage == "Adult") return DevelopmentalStage::Adult;
    if (stage == "Aging") return DevelopmentalStage::Aging;
    return DevelopmentalStage::Initial;
}

std::string neuronTypeToString(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

// High-level configuration management
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM - Comprehensive Python Bindings for Neural Learning Machine
        ----------------------------------------------------------------
        
        A high-level Python API for the NLM C++ neural simulation framework.
        
        This module provides classes for:
        
        - Brain: Easy brain manipulation and control
        - Experiment: Easy experiment running and management
        - Config: Easy configuration management
        - VisualizationTools: Visualization and plotting
        - AnalysisTools: Data analysis and statistics
        - DebugTools: Debug and monitoring tools
        - NeuromodulationController: Neuromodulation control
        - MemorySystem: Memory system access and management
        - DevelopmentSystem: Development system control
        - PredictionSystem: Prediction system control
        - CognitionSystem: Cognition system control
        
        The API is designed to be both easy to use for beginners and powerful for advanced users.
    )pbdoc";
    
    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Register core data types
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
    
    // Register enumeration types
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
    
    // Register original Config class
    py::class_<Config>(m, "OriginalConfig", R"pbdoc(Configuration class for NLM system)pbdoc")
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
    
    // Register convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    // Register original Brain class
    py::class_<Brain>(m, "OriginalBrain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
    
    // Register original AgentBrain class
    py::class_<AgentBrain>(m, "OriginalAgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
    
    // Register original SimpleWorld class
    py::class_<SimpleWorld>(m, "OriginalSimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
    
    // Register original sensory and motor classes
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
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
    
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
    
    // Register convenience functions
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
    
    // Register high-level classes
    
    // Register PyConfig class
    py::class_<PyConfig>(m, "Config", R"pbdoc(High-level configuration class)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&>(), py::arg("filepath"))
        .def("set", &PyConfig::set, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("get", &PyConfig::get, py::arg("key"), py::arg("defaultValue") = "",
             "Get configuration value")
        .def("getInt", &PyConfig::getInt, py::arg("key"), py::arg("defaultValue") = 0,
             "Get integer configuration value")
        .def("getFloat", &PyConfig::getFloat, py::arg("key"), py::arg("defaultValue") = 0.0f,
             "Get float configuration value")
        .def("getBool", &PyConfig::getBool, py::arg("key"), py::arg("defaultValue") = false,
             "Get boolean configuration value")
        .def("has", &PyConfig::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getAllKeys", &PyConfig::getAllKeys,
             "Get all configuration keys")
        .def("loadFromFile", &PyConfig::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("saveToFile", &PyConfig::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("getSummary", &PyConfig::getSummary,
             "Get a summary string of the configuration")
        .def("toDict", &PyConfig::toDict,
             "Convert configuration to Python dictionary")
        .def("__repr__", &PyConfig::__repr__)
        .def("__eq__", &PyConfig::operator==)
        .def("__ne__", &PyConfig::operator!=);
    
    m.def("createDefaultConfig", &createDefaultConfig,
          "Create a default configuration");
    
    m.def("createHighPerformanceConfig", &createHighPerformanceConfig,
          "Create a high-performance configuration");
    
    m.def("createMinimalConfig", &createMinimalConfig,
          "Create a minimal configuration");
    
    // Register PyBrain class
    py::class_<PyBrain>(m, "Brain", R"pbdoc(High-level brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>())
        .def_static("create", &PyBrain::create, py::arg("config"), "Create a brain from config")
        .def_static("createDefault", &PyBrain::createDefault, py::arg("neuronCount") = 1000, "Create default brain")
        .def("initialize", &PyBrain::initialize)
        .def("run", &PyBrain::run, py::arg("steps"))
        .def("step", &PyBrain::step, py::arg("stepNumber"))
        .def("pause", &PyBrain::pause)
        .def("resume", &PyBrain::resume)
        .def("isPaused", &PyBrain::isPaused)
        .def("getMemorySystem", &PyBrain::getMemorySystem)
        .def("getNeuromodulationController", &PyBrain::getNeuromodulationController)
        .def("getDevelopmentSystem", &PyBrain::getDevelopmentSystem)
        .def("getPredictionSystem", &PyBrain::getPredictionSystem)
        .def("getCognitionSystem", &PyBrain::getCognitionSystem)
        .def("getTotalNeuronCount", &PyBrain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &PyBrain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &PyBrain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &PyBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &PyBrain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &PyBrain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &PyBrain::getTotalSpikeCount)
        .def("getDevelopmentalStage", &PyBrain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &PyBrain::setDevelopmentalStage, py::arg("stage"))
        .def("save", &PyBrain::save, py::arg("filepath"))
        .def("load", &PyBrain::load, py::arg("filepath"))
        .def("reset", &PyBrain::reset)
        .def("addRegion", &PyBrain::addRegion, py::arg("name") = "")
        .def("getRegion", &PyBrain::getRegion, py::arg("id"))
        .def("getRegionCount", &PyBrain::getRegionCount)
        .def("getRegionIds", &PyBrain::getRegionIds)
        .def("getRegions", &PyBrain::getRegions)
        .def("receiveSensoryInput", &PyBrain::receiveSensoryInput, py::arg("input"))
        .def("injectCurrent", &PyBrain::injectCurrent, py::arg("neuronValue"), py::arg("current"))
        .def("injectCurrentToNeurons", &PyBrain::injectCurrentToNeurons, py::arg("type"), py::arg("current"))
        .def("produceAction", &PyBrain::produceAction)
        .def("logStatus", &PyBrain::logStatus)
        .def("getBrainInfo", &PyBrain::getBrainInfo);
    
    // Register PyExperiment class
    py::class_<PyExperiment>(m, "Experiment", R"pbdoc(High-level experiment class)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>())
        .def_static("create", &PyExperiment::create, py::arg("brain"), py::arg("agent_brain"), py::arg("world"))
        .def("start", &PyExperiment::start)
        .def("stop", &PyExperiment::stop)
        .def("isRunning", &PyExperiment::isRunning)
        .def("step", &PyExperiment::step)
        .def("run", &PyExperiment::run, py::arg("steps"))
        .def("pause", &PyExperiment::pause)
        .def("resume", &PyExperiment::resume)
        .def("isPaused", &PyExperiment::isPaused)
        .def("configure", &PyExperiment::configure, py::arg("duration"), py::arg("timestep"))
        .def("setGoal", &PyExperiment::setGoal, py::arg("goal"))
        .def("setEvaluationMetric", &PyExperiment::setEvaluationMetric, py::arg("metric"))
        .def("getPerformance", &PyExperiment::getPerformance)
        .def("getReward", &PyExperiment::getReward)
        .def("getAccuracy", &PyExperiment::getAccuracy)
        .def("getStepsCompleted", &PyExperiment::getStepsCompleted)
        .def("getTotalSteps", &PyExperiment::getTotalSteps)
        .def("getPerformanceTrace", &PyExperiment::getPerformanceTrace)
        .def("getRewardTrace", &PyExperiment::getRewardTrace)
        .def("saveState", &PyExperiment::saveState, py::arg("filepath"))
        .def("loadState", &PyExperiment::loadState, py::arg("filepath"))
        .def("reset", &PyExperiment::reset)
        .def("getExperimentSummary", &PyExperiment::getExperimentSummary);
    
    // Register convenience functions
    m.def("runSimulation", [](PyBrain& brain, int steps) {
        brain.run(steps);
    }, py::arg("brain"), py::arg("steps"), "Run a simulation with the given brain");
    
    m.def("createExperiment", [](PyBrain& brain, PyBrain& agent_brain, SimpleWorld& world) {
        return PyExperiment::create(brain, agent_brain, world);
    }, py::arg("brain"), py::arg("agent_brain"), py::arg("world"), "Create an experiment");
    
    // Register PyVisualizationTools class
    py::class_<PyVisualizationTools>(m, "VisualizationTools", R"pbdoc(Visualization and plotting tools)pbdoc")
        .def_static("plotNeurons", &PyVisualizationTools::plotNeurons, py::arg("brain"), py::arg("filename") = "neurons.png")
        .def_static("plotConnections", &PyVisualizationTools::plotConnections, py::arg("brain"), py::arg("filename") = "connections.png")
        .def_static("plotActivityOverTime", &PyVisualizationTools::plotActivityOverTime, py::arg("brain"), py::arg("filename") = "activity.png")
        .def_static("plotMemory", &PyVisualizationTools::plotMemory, py::arg("brain"), py::arg("filename") = "memory.png")
        .def_static("plotNeuromodulation", &PyVisualizationTools::plotNeuromodulation, py::arg("brain"), py::arg("filename") = "neuromodulation.png")
        .def_static("plotDevelopment", &PyVisualizationTools::plotDevelopment, py::arg("brain"), py::arg("filename") = "development.png")
        .def_static("plotExperimentResults", &PyVisualizationTools::plotExperimentResults, py::arg("experiment"), py::arg("filename") = "experiment_results.png")
        .def_static("createAnimation", &PyVisualizationTools::createAnimation, py::arg("brain"), py::arg("filename") = "animation.gif")
        .def_static("createInteractivePlot", &PyVisualizationTools::createInteractivePlot, py::arg("brain"), py::arg("filename") = "interactive.html")
        .def_static("getMetrics", &PyVisualizationTools::getMetrics, py::arg("brain"));
    
    // Register PyAnalysisTools class
    py::class_<PyAnalysisTools>(m, "AnalysisTools", R"pbdoc(Data analysis and statistics tools)pbdoc")
        .def_static("computeSpikingStatistics", &PyAnalysisTools::computeSpikingStatistics, py::arg("brain"))
        .def_static("computeNetworkTopology", &PyAnalysisTools::computeNetworkTopology, py::arg("brain"))
        .def_static("computeMemoryStatistics", &PyAnalysisTools::computeMemoryStatistics, py::arg("brain"))
        .def_static("computeNeuromodulationStatistics", &PyAnalysisTools::computeNeuromodulationStatistics, py::arg("brain"))
        .def_static("computeDevelopmentStatistics", &PyAnalysisTools::computeDevelopmentStatistics, py::arg("brain"))
        .def_static("computePredictionStatistics", &PyAnalysisTools::computePredictionStatistics, py::arg("brain"))
        .def_static("computeCognitionStatistics", &PyAnalysisTools::computeCognitionStatistics, py::arg("brain"))
        .def_static("generateReport", &PyAnalysisTools::generateReport, py::arg("brain"));
    
    // Register PyDebugTools class
    py::class_<PyDebugTools>(m, "DebugTools", R"pbdoc(Debug and monitoring tools)pbdoc")
        .def_static("enableDebugMode", &PyDebugTools::enableDebugMode)
        .def_static("disableDebugMode", &PyDebugTools::disableDebugMode)
        .def_static("isDebugModeEnabled", &PyDebugTools::isDebugModeEnabled)
        .def_static("log", &PyDebugTools::log, py::arg("message"))
        .def_static("setLogLevel", &PyDebugTools::setLogLevel, py::arg("level"))
        .def_static("enableProfiling", &PyDebugTools::enableProfiling)
        .def_static("disableProfiling", &PyDebugTools::disableProfiling)
        .def_static("isProfilingEnabled", &PyDebugTools::isProfilingEnabled)
        .def_static("getDebugMessages", &PyDebugTools::getDebugMessages)
        .def_static("clearDebugMessages", &PyDebugTools::clearDebugMessages)
        .def_static("enableMemoryTracking", &PyDebugTools::enableMemoryTracking)
        .def_static("disableMemoryTracking", &PyDebugTools::disableMemoryTracking)
        .def_static("isMemoryTrackingEnabled", &PyDebugTools::isMemoryTrackingEnabled)
        .def_static("getMemoryUsage", &PyDebugTools::getMemoryUsage)
        .def_static("getSystemInfo", &PyDebugTools::getSystemInfo);
    
    // Register PyAdvancedFeatures class
    py::class_<PyAdvancedFeatures>(m, "AdvancedFeatures", R"pbdoc(Advanced features for power users)pbdoc")
        .def_static("enableAdvancedMode", &PyAdvancedFeatures::enableAdvancedMode)
        .def_static("disableAdvancedMode", &PyAdvancedFeatures::disableAdvancedMode)
        .def_static("isAdvancedModeEnabled", &PyAdvancedFeatures::isAdvancedModeEnabled)
        .def_static("enableOptimization", &PyAdvancedFeatures::enableOptimization)
        .def_static("disableOptimization", &PyAdvancedFeatures::disableOptimization)
        .def_static("isOptimizationEnabled", &PyAdvancedFeatures::isOptimizationEnabled)
        .def_static("addStepCallback", &PyAdvancedFeatures::addStepCallback, py::arg("callback"))
        .def_static("addEventCallback", &PyAdvancedFeatures::addEventCallback, py::arg("callback"))
        .def_static("removeStepCallback", &PyAdvancedFeatures::removeStepCallback, py::arg("callback"))
        .def_static("removeEventCallback", &PyAdvancedFeatures::removeEventCallback, py::arg("callback"));
}

#endif // NLM_PYTHON_BINDINGS_H
