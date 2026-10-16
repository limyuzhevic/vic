#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/WorkingMemory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/cognition/NeuralAttention.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        ----------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Phase 6: Final Integration - All brain systems exposed with full functionality.
        
        This enhanced API provides direct access to all integrated systems:
        - Memory systems (working memory, episodic memory, associative memory)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Prediction and cognition (prediction system, planner, concept formation, attention)
        - Development system (developmental stages, plasticity modulation)
        - Advanced utilities for simulation management and monitoring
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // === BASIC TYPES ===

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

    // === CONFIGURATION ===

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
        // Add common convenience methods
        .def("set", py::overload_cast<const std::string&, const std::string&>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration string value")
        .def("set", py::overload_cast<const std::string&, int>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration integer value")
        .def("set", py::overload_cast<const std::string&, float>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration float value")
        .def("set", py::overload_cast<const std::string&, bool>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Set configuration boolean value")
        .def("get", py::overload_cast<const std::string&, const std::string&>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration string value with default")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration integer value with default")
        .def("get", py::overload_cast<const std::string&, float>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration float value with default")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get),
             py::arg("key"), py::arg("default_value"),
             "Get configuration boolean value with default")
        .def("getInt", &Config::getInt, py::arg("key"), py::arg("default_value") = 0,
             "Get configuration integer value")
        .def("getFloat", &Config::getFloat, py::arg("key"), py::arg("default_value") = 0.0f,
             "Get configuration float value")
        .def("getBool", &Config::getBool, py::arg("key"), py::arg("default_value") = false,
             "Get configuration boolean value")
        .def("getString", &Config::getString, py::arg("key"), py::arg("default_value") = "",
             "Get configuration string value");

    // === SENSORY INPUT ===

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

    // === ACTION ===

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

    // === WORLD OBJECT ===

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

    // === AGENT BODY ===

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

    // === ACTION RESULT ===

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // === SENSORY PERCEPT ===

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

    // === SIMPLE WORLD ===

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

    // === BRAIN ===

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
             "Log brain status")
        // Enhanced brain access to integrated systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Apply developmental changes")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system");

    // === AGENT BRAIN ===

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

    // === WORKING MEMORY ===

    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of specific neuron")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    // === EPISODIC MEMORY ===

    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Episodic memory item)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons)
        .def_readwrite("values", &EpisodicMemoryItem::values)
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata);

    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store episode")
        .def("retrieveEpisode", &NeuralEpisodicMemory::retrieveEpisode, py::arg("index"),
             "Retrieve episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get episode count")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"),
             "Memory consolidation (move to long-term)");

    // === NEUROMODULATION ===

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Dopamine::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Dopamine::getLevel,
             "Get current dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"),
             "Set dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity factor")
        .def("update", &Dopamine::update, py::arg("dt"),
             "Update neuromodulator state")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward event")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get current curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset,
             "Reset curiosity system");

    py::class_<Novelty>(m, "Novelty", R"pbdoc( novelty detection neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel,
             "Get current novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set novelty level")
        .def("detectNovelty", py::overload_cast<const Observation&, const Observation&>(&Novelty::detectNovelty),
             py::arg("observation"), py::arg("previousObservation"),
             "Detect novelty from observations")
        .def("detectNovelty", py::overload_cast<const std::vector<float>&, const std::vector<float>&>(&Novelty::detectNovelty),
             py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from patterns")
        .def("update", &Novelty::update, py::arg("dt"),
             "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory,
             "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory,
             "Clear novelty history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getError", &PredictionError::getError,
             "Get current prediction error")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionError::clearHistory,
             "Clear prediction error history")
        .def("getMagnitude", &PredictionError::getMagnitude,
             "Get error magnitude for neuromodulation");

    // === PREDICTION SYSTEM ===

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"),
             "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model");

    // === COGNITION SYSTEMS ===

    py::class_<PlanningCandidate>(m, "PlanningCandidate", R"pbdoc(Planning candidate)pbdoc")
        .def(py::init<>())
        .def_readwrite("actions", &PlanningCandidate::actions)
        .def_readwrite("predictedStates", &PlanningCandidate::predictedStates)
        .def_readwrite("expectedReward", &PlanningCandidate::expectedReward)
        .def_readwrite("confidence", &PlanningCandidate::confidence)
        .def_readwrite("depth", &PlanningCandidate::depth);

    py::class_<NeuralPlanner>(m, "Planner", R"pbdoc(Neural planner)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate a potential action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plans based on actual outcome")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             "Set action quality function")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?");

    py::class_<ConceptInstance>(m, "ConceptInstance", R"pbdoc(Concept instance)pbdoc")
        .def(py::init<>())
        .def_readwrite("pattern", &ConceptInstance::pattern)
        .def_readwrite("features", &ConceptInstance::features)
        .def_readwrite("observationCount", &ConceptInstance::observationCount)
        .def_readwrite("lastObserved", &ConceptInstance::lastObserved)
        .def_readwrite("avgReward", &ConceptInstance::avgReward)
        .def_readwrite("stability", &ConceptInstance::stability);

    py::class_<DiscoveredConcept>(m, "DiscoveredConcept", R"pbdoc(Discovered concept)pbdoc")
        .def(py::init<>())
        .def_readwrite("id", &DiscoveredConcept::id)
        .def_readwrite("prototype", &DiscoveredConcept::prototype)
        .def_readwrite("instances", &DiscoveredConcept::instances)
        .def_readwrite("categoryHint", &DiscoveredConcept::categoryHint)
        .def_readwrite("associatedConceptIds", &DiscoveredConcept::associatedConceptIds)
        .def_readwrite("totalObservations", &DiscoveredConcept::totalObservations)
        .def_readwrite("avgStability", &DiscoveredConcept::avgStability);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present a new experience to concept formation system")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get concept that best matches a pattern")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             "Get specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Measure concept stability")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"),
             "Compute similarity between patterns")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Get concept for given pattern")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             "Get generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set stability window");

    py::class_<AttentionalSelection>(m, "Attention", R"pbdoc(Attentional selection system)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Is this neuron currently attended?")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             "Apply top-down attention bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             "Apply bottom-up salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention");

    // === DEVELOPMENT SYSTEM ===

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get stage name")
        .def("update", py::overload_cast<Brain*, SimulationStep>(&DevelopmentSystem::update), py::arg("brain"), py::arg("currentStep"),
             "Update development (simple version)")
        .def("update", py::overload_cast<Brain*, RandomGenerator&, TimestepDuration>(&DevelopmentSystem::update), py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Get critical period info")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             "Set developmental age");

    // === FACTORY FUNCTIONS ===

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

    // === INVALID ID CONSTANTS ===

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // === ADVANCED UTILITIES ===

    // Simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent, int numSteps) {
        for (int step = 0; step < numSteps; ++step) {
            if (world) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                }
                if (brain) {
                    brain->step(step);
                }
                if (agent) {
                    auto action = agent->decodeMotorCommand();
                    if (world) {
                        world->applyMotorCommand(action, world->getSimulationTime());
                    }
                }
            } else {
                if (brain) {
                    brain->step(step);
                }
            }
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps") = 100,
       "Run a complete simulation loop");

    m.def("runAgentEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent, int numSteps) {
        // Reset systems for new episode
        if (brain) brain->reset();
        if (world) {
            world->reset();
            world->setAgentStart(10.0, 10.0);
        }
        if (agent) {
            agent->reset();
            if (world) agent->initialize(*world);
        }
        
        // Run episode
        for (int step = 0; step < numSteps; ++step) {
            if (world) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                if (agent) {
                    agent->processSensoryInput(percept);
                }
                if (brain) {
                    brain->step(step);
                }
                if (agent) {
                    auto action = agent->decodeMotorCommand();
                    if (world) {
                        world->applyMotorCommand(action, world->getSimulationTime());
                    }
                    // Get reward and apply neuromodulation
                    if (agent) {
                        float reward = 0.0f;
                        if (percept.getInternal() && !percept.getInternal()->empty()) {
                            reward = percept.getInternal()->front();
                        }
                        agent->applyRewardModulation(reward, 0.0);
                    }
                }
            }
        }
        
        // Return final stats
        return std::map<std::string, double>{
            {"firing_rate", brain ? brain->getAverageFiringRate() : 0.0},
            {"total_spikes", static_cast<double>(brain ? brain->getTotalSpikeCount() : 0)},
            {"firing_neurons", static_cast<double>(brain ? brain->getFiringNeuronCount() : 0)}
        };
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps") = 100,
       "Run a complete agent episode");

    m.def("batchSimulate", [](const std::vector<std::shared_ptr<Brain>>& brains,
                           const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                           const std::vector<std::shared_ptr<AgentBrain>>& agents,
                           int stepsPerBrain) {
        std::vector<std::map<std::string, double>> results;
        for (size_t i = 0; i < brains.size(); ++i) {
            auto result = pynlm::runAgentEpisode(brains[i], worlds[i], agents[i], stepsPerBrain);
            results.push_back(result);
        }
        return results;
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"), py::arg("stepsPerBrain") = 100,
       "Run batch simulation across multiple brains");

    m.def("getSystemStats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        if (brain) {
            stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
            stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
            stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
            
            // Memory system stats
            auto workingMem = brain->getWorkingMemory();
            if (workingMem) {
                stats["working_memory_traces"] = static_cast<double>(workingMem->getActiveTraces());
                stats["working_memory_capacity"] = static_cast<double>(workingMem->getCapacity());
            }
            
            auto episodicMem = brain->getEpisodicMemory();
            if (episodicMem) {
                stats["episodic_memory_episodes"] = static_cast<double>(episodicMem->getEpisodeCount());
            }
            
            // Neuromodulation stats
            auto dopamine = brain->getDopamine();
            if (dopamine) {
                stats["dopamine_level"] = dopamine->getLevel();
            }
            
            auto curiosity = brain->getCuriosity();
            if (curiosity) {
                stats["curiosity_level"] = curiosity->getLevel();
            }
            
            auto novelty = brain->getNovelty();
            if (novelty) {
                stats["novelty_level"] = novelty->getLevel();
            }
            
            auto predictionError = brain->getPredictionErrorSignal();
            if (predictionError) {
                stats["prediction_error"] = predictionError->getError();
            }
            
            // Development stats
            auto devSystem = brain->getDevelopmentSystem();
            if (devSystem) {
                stats["developmental_stage"] = static_cast<double>(devSystem->getStage());
                stats["plasticity_modifier"] = devSystem->getPlasticityModifier();
                stats["developmental_age"] = devSystem->getDevelopmentalAge();
            }
        }
        return stats;
    }, py::arg("brain"), "Get comprehensive system statistics");

    m.def("createConfigFromDict", [](const std::map<std::string, py::object>& config_dict) {
        auto config = std::make_shared<Config>();
        for (const auto& item : config_dict) {
            const std::string& key = item.first;
            const py::object& value = item.second;
            
            if (value.is(py::int_())) {
                config->set(key, value.cast<int>());
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<float>());
            } else if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>());
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>());
            }
        }
        return config;
    }, py::arg("config_dict"), "Create configuration from dictionary");

    m.def("saveBrainWithStats", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (brain->save(filepath)) {
            // Also save statistics to a companion file
            auto stats = pynlm::getSystemStats(brain);
            std::string stats_path = filepath + ".stats.json";
            
            std::ofstream stats_file(stats_path);
            if (stats_file.is_open()) {
                stats_file << "{\n";
                bool first = true;
                for (const auto& item : stats) {
                    if (!first) stats_file << ",\n";
                    first = false;
                    stats_file << "  \"" << item.first << "\": " << item.second;
                }
                stats_file << "\n}\n";
            }
            return true;
        }
        return false;
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state with statistics");

    m.def("loadBrainWithStats", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(filepath)) {
            // Check for stats file
            std::string stats_path = filepath + ".stats.json";
            std::ifstream stats_file(stats_path);
            if (stats_file.is_open()) {
                // Stats file is optional, we don't need to load it
                stats_file.close();
            }
            return brain;
        }
        return std::shared_ptr<Brain>(nullptr);
    }, py::arg("filepath"),
       "Load brain state with statistics");

    // === PERFORMANCE MONITORING ===

    m.def("monitorPerformance", [](std::shared_ptr<Brain> brain, int duration_steps) {
        std::vector<double> firing_rates;
        std::vector<double> spike_counts;
        std::vector<double> memory_activities;
        
        for (int step = 0; step < duration_steps; ++step) {
            if (brain) {
                brain->step(step);
                firing_rates.push_back(brain->getAverageFiringRate());
                spike_counts.push_back(static_cast<double>(brain->getTotalSpikeCount()));
                
                auto workingMem = brain->getWorkingMemory();
                if (workingMem) {
                    memory_activities.push_back(workingMem->getMemoryActivity());
                }
            }
        }
        
        return std::map<std::string, std::vector<double>>{
            {"firing_rates", firing_rates},
            {"spike_counts", spike_counts},
            {"memory_activities", memory_activities}
        };
    }, py::arg("brain"), py::arg("duration_steps"),
       "Monitor performance metrics over time");

    m.def("calculateEfficiencyScore", [](const std::map<std::string, std::vector<double>>& performance_data) {
        if (performance_data.empty()) return 0.0;
        
        double avg_firing_rate = 0.0;
        double max_firing_rate = 0.0;
        double total_spikes = 0.0;
        double memory_efficiency = 0.0;
        
        if (performance_data.count("firing_rates")) {
            const auto& rates = performance_data.at("firing_rates");
            if (!rates.empty()) {
                double sum = 0.0;
                for (double rate : rates) {
                    sum += rate;
                    if (rate > max_firing_rate) max_firing_rate = rate;
                }
                avg_firing_rate = sum / rates.size();
            }
        }
        
        if (performance_data.count("spike_counts")) {
            const auto& spikes = performance_data.at("spike_counts");
            if (!spikes.empty()) {
                double sum = 0.0;
                for (double spike : spikes) {
                    sum += spike;
                }
                total_spikes = sum / spikes.size();
            }
        }
        
        if (performance_data.count("memory_activities")) {
            const auto& mems = performance_data.at("memory_activities");
            if (!mems.empty()) {
                double sum = 0.0;
                for (double mem : mems) {
                    sum += mem;
                }
                memory_efficiency = sum / mems.size();
            }
        }
        
        // Calculate efficiency score: balance of activity vs efficiency
        double efficiency = 0.0;
        if (max_firing_rate > 0) {
            double activity_factor = avg_firing_rate / max_firing_rate;
            double spike_factor = 1.0 / (1.0 + total_spikes / 1000.0);
            double memory_factor = memory_efficiency / 10.0;
            
            efficiency = (activity_factor * 0.4) + (spike_factor * 0.3) + (memory_factor * 0.3);
        }
        
        return efficiency;
    }, py::arg("performance_data"),
       "Calculate efficiency score from performance metrics");

    // === UTILITY FUNCTIONS ===

    m.def("version", []() { return "NLM 0.1.0 (Phase 6 - Enhanced)"; },
          "Get NLM version information");

    m.def("clearAllCaches", []() {
        // Clear any cached computations or temporary data
        // (Implementation depends on what needs to be cleared)
    }, "Clear all caches and temporary data");

    m.def("validateBrain", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> errors;
        if (!brain) {
            errors.push_back("Brain pointer is null");
            return errors;
        }
        
        if (!brain->initialize()) {
            errors.push_back("Brain initialization failed");
        }
        
        // Validate key systems
        auto workingMem = brain->getWorkingMemory();
        if (!workingMem) {
            errors.push_back("Working memory system not available");
        }
        
        auto episodicMem = brain->getEpisodicMemory();
        if (!episodicMem) {
            errors.push_back("Episodic memory system not available");
        }
        
        auto dopamine = brain->getDopamine();
        if (!dopamine) {
            errors.push_back("Dopamine neuromodulator not available");
        }
        
        auto curiosity = brain->getCuriosity();
        if (!curiosity) {
            errors.push_back("Curiosity neuromodulator not available");
        }
        
        auto predictionSystem = brain->getPredictionSystem();
        if (!predictionSystem) {
            errors.push_back("Prediction system not available");
        }
        
        return errors;
    }, py::arg("brain"),
       "Validate brain system integrity and return any errors");

    m.def("printBrainStatus", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            std::cout << "Brain is null!" << std::endl;
            return;
        }
        
        std::cout << "=== NLM Brain Status ===" << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        auto workingMem = brain->getWorkingMemory();
        if (workingMem) {
            std::cout << "Working memory traces: " << workingMem->getActiveTraces() << std::endl;
        }
        
        auto episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            std::cout << "Episodic memory episodes: " << episodicMem->getEpisodeCount() << std::endl;
        }
        
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
        }
        
        auto curiosity = brain->getCuriosity();
        if (curiosity) {
            std::cout << "Curiosity level: " << curiosity->getLevel() << std::endl;
        }
        
        auto novelty = brain->getNovelty();
        if (novelty) {
            std::cout << "Novelty level: " << novelty->getLevel() << std::endl;
        }
        
        auto predictionError = brain->getPredictionErrorSignal();
        if (predictionError) {
            std::cout << "Prediction error: " << predictionError->getError() << std::endl;
        }
        
        auto devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            std::cout << "Developmental stage: " << static_cast<int>(devSystem->getStage()) << std::endl;
            std::cout << "Plasticity modifier: " << devSystem->getPlasticityModifier() << std::endl;
        }
        
        std::cout << "========================" << std::endl;
    }, py::arg("brain"),
       "Print comprehensive brain status information");

} // namespace nlm
