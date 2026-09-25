#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "EnhancedPythonBindings.h"

namespace py = pybind11;
namespace nlm {

// Forward declarations for enhanced bindings
void enhanceConfigAPI(py::class_<Config>& cfgClass);
void enhanceBrainAPI(py::class_<Brain>& brainClass);
void enhanceAgentBrainAPI(py::class_<AgentBrain>& agentBrainClass);
void enhanceSimpleWorldAPI(py::class_<SimpleWorld>& worldClass);
void enhanceVisualizationAPI(py::class_<VisualizationInterface>& vizClass);

// Helper functions for Python API
std::string format_simulation_step(SimulationStep step, Timestamp time);
std::vector<float> get_neuron_activations(const Brain& brain);
void set_default_config_values(std::shared_ptr<Config> config);

// Enhanced Config class with additional functionality
void enhanceConfigAPI(py::class_<Config>& cfgClass);

// Enhanced Brain class with advanced simulation control
void enhanceBrainAPI(py::class_<Brain>& brainClass);

// Enhanced AgentBrain with more control methods
void enhanceAgentBrainAPI(py::class_<AgentBrain>& agentBrainClass);

// Enhanced SimpleWorld with better control
void enhanceSimpleWorldAPI(py::class_<SimpleWorld>& worldClass);

// Enhanced visualization interface
void enhanceVisualizationAPI(py::class_<VisualizationInterface>& vizClass);

// New utility functions for batch operations
void addBatchOperations(py::module_& m);

// New utility functions for simulation management
void addSimulationManagement(py::module_& m);

// New utility functions for advanced neuromodulation
void addAdvancedNeuromodulation(py::module_& m);

// New utility functions for memory management
void addMemoryManagement(py::module_& m);

// New utility functions for experiment configuration
void addExperimentConfiguration(py::module_& m);

// New utility functions for performance monitoring
void addPerformanceMonitoring(py::module_& m);

// New utility functions for debugging and development
void addDebugDevelopment(py::module_& m);

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ---------------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides both basic and enhanced functionality for:
        - Neural simulation and computation
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, reward, novelty)
        - Prediction systems
        - Cognitive mechanisms (attention, planning, concept formation)
        - Development systems
        - Sensory processing and motor control
        - World simulation and interaction
        
        The enhanced API includes comprehensive simulation control,
        advanced memory management, performance monitoring, and
        extensive debugging capabilities.
        
        For basic functionality, use the original pynlm module.
        For advanced features, use the enhanced_pynlm module.
    )pbdoc";
    
    // Version information
    m.attr("__version__") = "1.0.0";
    m.attr("__enhanced_version__") = "2.0.0";
    
    // Import core types and classes
    
    // Neuron ID types
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
    
    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();
    
    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();
    
    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();
    
    // Firing states
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();
    
    // Action types
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
    
    // Motor commands
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
    
    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
    
    // Basic Config class (original)
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
    
    // Enhance Config class with additional API
    enhanceConfigAPI(py::class_<Config>(m, "EnhancedConfig",
        R"pbdoc(An enhanced configuration class for NLM with additional functionality.)pbdoc"));
    
    // Sensory input classes
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
    
    // World object class
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
    
    // Agent body class
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
    
    // Action result class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
    
    // Sensory percept class
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
    
    // World class
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
    
    // Enhance SimpleWorld class with additional API
    enhanceSimpleWorldAPI(py::class_<SimpleWorld>(m, "EnhancedSimpleWorld",
        R"pbdoc(An enhanced world simulation with enhanced control.)pbdoc"));
    
    // Agent brain interface
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
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
    
    // Enhance AgentBrain class with additional API
    enhanceAgentBrainAPI(py::class_<AgentBrain>(m, "EnhancedAgentBrain",
        R"pbdoc(An enhanced agent brain interface with advanced control.)pbdoc"));
    
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
             py::arg("input"), "Inject sensory input into brain")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");
    
    // Enhance Brain class with additional API
    enhanceBrainAPI(py::class_<Brain>(m, "EnhancedBrain",
        R"pbdoc(An enhanced brain class with comprehensive simulation control.)pbdoc"));
    
    // Visualization interface
    py::class_<VisualizationInterface>(m, "VisualizationInterface",
        R"pbdoc(Visualization interface for NLM brain data)pbdoc")
        .def(py::init<>())
        .def("renderFrame", &VisualizationInterface::renderFrame,
             py::arg("brain"), py::arg("world"), py::arg("timestep"),
             "Render a visualization frame")
        .def("renderToFile", &VisualizationInterface::renderToFile,
             py::arg("filename"), py::arg("brain"), py::arg("world"),
             py::arg("timestep"), "Render to file")
        .def("saveImage", &VisualizationInterface::saveImage,
             py::arg("filename"), "Save current image to file")
        .def("setCameraPosition", &VisualizationInterface::setCameraPosition,
             py::arg("x"), py::arg("y"), py::arg("z"),
             "Set camera position")
        .def("setLighting", &VisualizationInterface::setLighting,
             py::arg("ambient"), py::arg("diffuse"), py::arg("specular"),
             "Set lighting parameters")
        .def("toggleDebugMode", &VisualizationInterface::toggleDebugMode,
             "Toggle debug visualization mode")
        .def("setNeuronVisualization", &VisualizationInterface::setNeuronVisualization,
             py::arg("enabled"), "Enable/disable neuron visualization")
        .def("setSynapseVisualization", &VisualizationInterface::setSynapseVisualization,
             py::arg("enabled"), "Enable/disable synapse visualization")
        .def("setMemoryVisualization", &VisualizationInterface::setMemoryVisualization,
             py::arg("enabled"), "Enable/disable memory visualization")
        .def("set3DEnabled", &VisualizationInterface::set3DEnabled,
             py::arg("enabled"), "Enable/disable 3D visualization");
    
    // Enhance Visualization interface with additional API
    enhanceVisualizationAPI(py::class_<VisualizationInterface>(m, "EnhancedVisualizationInterface",
        R"pbdoc(An enhanced visualization interface with advanced rendering options.)pbdoc"));
    
    // Factory functions
    m.def("createConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a configuration object")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain with configuration")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple world simulation")
    
    m.def("createVision", [](size_t width, size_t height, size_t channels = 3) {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       "Create a vision sensory input")
    
    m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"),
       "Create an audio sensory input")
    
    m.def("createInternalSignals", []() {
        return std::make_shared<InternalSignals>();
    }, "Create internal signals sensory input")
    
    m.def("createAction", [](ActionType type) {
        return std::make_shared<Action>(type);
    }, py::arg("type"), "Create an action")
    
    m.def("createActionWithParams", [](ActionType type, const std::vector<float>& params) {
        return std::make_shared<Action>(type, params);
    }, py::arg("type"), py::arg("params"), "Create an action with parameters")
    
    m.def("createWorldObject", [](float x, float y, WorldObjectType type, 
                                 float value = 0.0f, float radius = 0.5f) {
        return std::make_shared<WorldObject>(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"),
       py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
       "Create a world object")
    
    m.def("createAgentBody", []() {
        return std::make_shared<AgentBody>();
    }, "Create an agent body")
    
    m.def("createSensoryPercept", []() {
        return std::make_shared<SensoryPercept>();
    }, "Create a sensory percept")
    
    m.def("createVisualizationInterface", []() {
        return std::make_shared<VisualizationInterface>();
    }, "Create a visualization interface");
    
    // Error types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    
    // Utility functions
    m.def("formatSimulationStep", &format_simulation_step,
          py::arg("step"), py::arg("time"),
          "Format simulation step information")
    
    m.def("getNeuronActivations", &get_neuron_activations,
          py::arg("brain"), "Get activations of all neurons")
    
    m.def("setDefaultConfigValues", &set_default_config_values,
          py::arg("config"), "Set default configuration values for Python users")
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        set_default_config_values(config);
        return config;
    }, "Create a default configuration with sensible values")
    
    // Constants
    m.attr("DEFAULT_RANDOM_SEED") = 42;
    m.attr("DEFAULT_TIMESTEP") = 0.001;
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;
    
    // Enhanced version
    m.attr("__enhanced_version__") = "2.0.0";
    
    // Batch operations
    addBatchOperations(m);
    
    // Simulation management
    addSimulationManagement(m);
    
    // Advanced neuromodulation
    addAdvancedNeuromodulation(m);
    
    // Memory management
    addMemoryManagement(m);
    
    // Experiment configuration
    addExperimentConfiguration(m);
    
    // Performance monitoring
    addPerformanceMonitoring(m);
    
    // Debug and development
    addDebugDevelopment(m);
    
    // Logging utilities
    m.def("setLogLevel", [](const std::string& level) {
        auto logger = Logger::getInstance();
        if (level == "trace") logger.setLevel(Logger::Level::Trace);
        else if (level == "debug") logger.setLevel(Logger::Level::Debug);
        else if (level == "info") logger.setLevel(Logger::Level::Info);
        else if (level == "warning") logger.setLevel(Logger::Level::Warning);
        else if (level == "error") logger.setLevel(Logger::Level::Error);
        else if (level == "critical") logger.setLevel(Logger::Level::Critical);
        else logger.setLevel(Logger::Level::Info);
    }, py::arg("level"), "Set logging level")
    
    m.def("logMessage", [](const std::string& level, const std::string& message) {
        auto logger = Logger::getInstance();
        if (level == "trace") logger.logTrace(message);
        else if (level == "debug") logger.logDebug(message);
        else if (level == "info") logger.logInfo(message);
        else if (level == "warning") logger.logWarning(message);
        else if (level == "error") logger.logError(message);
        else if (level == "critical") logger.logCritical(message);
    }, py::arg("level"), py::arg("message"), "Log a message at specified level")
    
    // Error handling utilities
    m.def("handlePythonException", []() {
        try {
            throw;
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    }, "Handle Python exceptions (to be used in exception handlers)")
    
    // Memory cleanup utilities
    m.def("cleanup", []() {
        // Cleanup any resources
    }, "Clean up Python bindings resources")
    
    // Educational examples
    m.def("example_basic_usage", []() {
        return R"example(
        # Basic usage example
        import pynlm as nlm
        
        # Create configuration
        config = nlm.createConfig()
        config.loadFromFile("configs/default.cfg")
        
        # Create brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step, step * 0.001)
        
        # Get results
        print(f"Neurons: {brain.getTotalNeuronCount()}")
        print(f"Spikes: {brain.getTotalSpikeCount()}")
        
        # Clean up
        brain.reset()
        )example";
    }, "Get basic usage example")
    
    m.def("example_advanced_usage", []() {
        return R"example(
        # Advanced usage example
        import pynlm as nlm
        import pynlm.simulation as simulation
        import pynlm.memory as memory
        import pynlm.performance as performance
        
        # Create and configure brain
        config = nlm.createConfig()
        config.set("neuron_count", 1000)
        
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Enhanced processing with batch operations
        percepts = [nlm.createVision(16, 16) for _ in range(10)]
        actions = simulation.batchProduceActions(percepts, brain)
        
        # Memory management
        memory_stats = memory.getWorkingMemoryStats(brain)
        print(f"Working memory utilization: {memory_stats['workingMemoryUtilization']}")
        
        # Performance monitoring
        brain.reset()
        brain.initialize()
        perf_metrics = performance.getBrainPerformanceMetrics(brain)
        print(f"Performance: {perf_metrics['stepsPerSecond']} steps/sec")
        
        # Advanced neuromodulation
        simulation.setDopamineLevel(brain, 0.5)
        simulation.setCuriosityLevel(brain, 0.3)
        
        # Development control
        brain.getAgentBrain().enableDevelopment(True)
        brain.getAgentBrain().enableCuriosity(True)
        
        # Simulation control
        simulation.runSimulationLoop(brain, 1000, 0.001)
        )example";
    }, "Get advanced usage example")
    
    // Educational documentation
    m.def("getDocumentation", []() {
        return R"doc(
        NLM Python API Documentation
        =============================
        
        This module provides Python bindings for the NLM C++ neural simulation framework.
        
        Key Features:
        - Neural simulation and computation
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, reward, novelty)
        - Prediction systems
        - Cognitive mechanisms (attention, planning, concept formation)
        - Development systems
        - Sensory processing and motor control
        - World simulation and interaction
        
        Enhanced version available: nlm_enhanced_pynlm
        
        Version: 1.0.0
        Author: NLM Research Team
        )doc";
    }, "Get documentation")
    
    // Educational tutorials
    m.def("getTutorials", []() {
        return R"tutorials(
        Basic Tutorial:
        1. Create a configuration: config = nlm.createConfig()
        2. Create brain: brain = nlm.createBrain(config)
        3. Initialize: brain.initialize()
        4. Run simulation: for step in range(100): brain.step(step, step * 0.001)
        5. Get results: brain.getTotalSpikeCount()
        6. Clean up: brain.reset()
        
        Advanced Tutorial:
        1. Use batch operations: nlm.batchProcessSensory(percepts, brain)
        2. Monitor performance: perf = nlm.getBrainPerformanceMetrics(brain)
        3. Manage memory: stats = nlm.getMemoryStats(brain)
        4. Control neuromodulation: nlm.setDopamineLevel(brain, 0.5)
        5. Debug issues: info = nlm.inspectBrainState(brain)
        
        Example Applications:
        - Neural network simulation
        - Memory research
        - Neuromodulation studies
        - Cognitive modeling
        - Development studies
        )tutorials";
    }, "Get tutorials");
}

} // namespace nlm
