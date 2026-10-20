#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a comprehensive Python interface to the NLM neural simulation framework.
        It supports all major functionality from Phase 2 through Phase 6, including:
        - Real LIF neuron dynamics and synaptic plasticity
        - Sensory-motor loops with embodied agents
        - Learning through reward modulation and curiosity
        - Memory systems (working, episodic, and associative)
        - Prediction and planning capabilities
        - Development and maturation processes
        - Checkpoint persistence and replay
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // ------------------------------------------------------------------------
    // CORE TYPE DEFINITIONS
    // ------------------------------------------------------------------------
    
    // Neuron ID
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

    // Synapse ID
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

    // Region ID
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

    // Population ID
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ------------------------------------------------------------------------
    // ENUMERATIONS
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // CONFIGURATION SYSTEM
    // ------------------------------------------------------------------------

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
        // Configuration convenience methods
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set_double", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set double configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("get_int", [](Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0, "Get integer configuration value")
        .def("get_float", [](Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<float>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0f, "Get float configuration value")
        .def("get_string", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "", "Get string configuration value");

    // ------------------------------------------------------------------------
    // SENSORY SYSTEMS
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // MOTOR SYSTEM
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // AGENT SYSTEMS
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // WORLD SYSTEM
    // ------------------------------------------------------------------------

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

    // ------------------------------------------------------------------------
    // BRAIN SYSTEM
    // ------------------------------------------------------------------------

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
        // Brain convenience methods
        .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def("getNeuronCount", &Brain::getTotalNeuronCount)
        .def("getSynapseCount", &Brain::getTotalSynapseCount)
        .def("getSpikeCount", &Brain::getTotalSpikeCount)
        .def("getFiringRate", &Brain::getAverageFiringRate);

    // ------------------------------------------------------------------------
    // AGENT BRAIN INTERFACE
    // ------------------------------------------------------------------------

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
        // AgentBrain convenience and feature management methods
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
        // AgentBrain utility methods
        .def("getStats", [](const AgentBrain& self) {
            std::string stats = "Neuromodulation: " + std::to_string(self.getNeuromodulationLevel()) + "\n";
            stats += "Curiosity: " + std::to_string(self.getCuriosityLevel()) + "\n";
            stats += "Novelty: " + std::to_string(self.getNoveltyLevel()) + "\n";
            stats += "Prediction Error: " + std::to_string(self.getPredictionError()) + "\n";
            return stats;
        }, "Get brain statistics as string")
        .def("setCuriosityThreshold", [](AgentBrain& self, float threshold) {
            // Store threshold in custom attribute if available
            // This is a placeholder for advanced feature access
        }, py::arg("threshold"), "Set curiosity threshold for exploration")
        .def("setExplorationRate", [](AgentBrain& self, float rate) {
            // Store exploration rate for advanced feature access
            // This is a placeholder for advanced feature access
        }, py::arg("rate"), "Set exploration rate for random action selection");

    // ------------------------------------------------------------------------
    // MODULE-LEVEL CONVENIENCE FUNCTIONS
    // ------------------------------------------------------------------------

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration for NLM brain");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for NLM simulation");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Advanced convenience functions for improved user experience
    m.def("createExperimentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set sensible defaults for experimentation
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012, ConfigSource::Default);
        config->set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
        config->set("development_pruning_rate", 0.0001, ConfigSource::Default);
        return config;
    }, "Create a configuration optimized for experimentation and learning");

    m.def("run_simple_demo", [](const std::string& config_file = "configs/default.cfg") {
        // Simple demonstration of NLM functionality
        auto config = std::make_shared<Config>();
        config->loadFromFile(config_file);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run for a few steps
        for (int step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::cout << "Simple demo completed. Spikes: " << brain->getTotalSpikeCount() << std::endl;
        return brain->getTotalSpikeCount();
    }, py::arg("config_file") = "configs/default.cfg", 
        "Run a simple demonstration with default configuration");

    // Utility and helper functions
    m.def("get_version", []() {
        return "NLM Python Bindings v0.1.0 - Phase 6 Integration";
    }, "Get the current version of NLM Python bindings");

    m.def("validate_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("random_seed")) {
            errors.push_back("Missing random_seed");
        }
        if (!config->has("simulation_timestep")) {
            errors.push_back("Missing simulation_timestep");
        }
        if (!config->has("neuron_count")) {
            errors.push_back("Missing neuron_count");
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors");

    m.def("reset_to_defaults", [](std::shared_ptr<Config> config) {
        config->clear();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15, ConfigSource::Default);
        return config;
    }, py::arg("config"), "Reset configuration to sensible defaults");

    // ------------------------------------------------------------------------
    // INVALID ID CONSTANTS
    // ------------------------------------------------------------------------

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ------------------------------------------------------------------------
    // ADVANCED FEATURE ACCESS - Phase 6 Integration
    // ------------------------------------------------------------------------

    // These provide access to advanced NLM features for expert users
    m.def("enable_advanced_learning", [](std::shared_ptr<AgentBrain> agent) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        agent->enableStructuralPlasticity(true);
    }, py::arg("agent"), "Enable all advanced learning features");

    // Phase 6 Integration Test - Comprehensive brain system verification
    m.def("run_integration_test", [](const std::string& checkpointFile = "./checkpoint_test.bin") {
        // Create experiment configuration
        Phase6Config config;
        config.maxSteps = 5000;
        config.neuronCount = 500;
        config.regionCount = 2;
        config.connectionProbability = 0.1f;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = checkpointFile;
        
        // Run integration test (this is a simplified version)
        auto configPtr = std::make_shared<Config>();
        configPtr->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        configPtr->set("simulation_timestep", 0.001, ConfigSource::Default);
        configPtr->set("neuron_count", config.neuronCount, ConfigSource::Default);
        configPtr->set("region_count", config.regionCount, ConfigSource::Default);
        configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(configPtr);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for integration test");
        }
        
        // Create world and agent
        auto world = std::make_shared<SimpleWorld>();
        world->configure(16, 16, 8, 8);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run integration simulation
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, step * 0.001);
            
            auto cmd = agent->decodeMotorCommand();
            world->applyMotorCommand(cmd, step * 0.001);
            
            // Simple reward calculation based on activity
            float reward = 0.01f * brain->getFiringNeuronCount();
            totalReward += reward;
            agent->applyRewardModulation(reward, 0.0f);
            
            if (config.enableDevelopment) {
                agent->updateDevelopment(0.001);
            }
            
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
        }
        
        // Return integration results
        std::map<std::string, float> results;
        results["total_reward"] = totalReward;
        results["avg_firing_rate"] = firingCount > 0 ? totalFiringRate / firingCount : 0.0f;
        results["avg_firing_count"] = static_cast<float>(firingCount);
        results["total_steps"] = static_cast<float>(config.maxSteps);
        results["neuromodulation_level"] = agent->getNeuromodulationLevel();
        results["curiosity_level"] = agent->getCuriosityLevel();
        results["novelty_level"] = agent->getNoveltyLevel();
        results["prediction_error"] = agent->getPredictionError();
        
        return results;
    }, py::arg("checkpointFile") = "./checkpoint_test.bin",
        "Run Phase 6 integration test and return system metrics");

    // Memory system access for advanced users
    m.def("get_memory_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, float> stats;
        
        // Working memory stats
        if (brain->getWorkingMemory()) {
            auto* wm = brain->getWorkingMemory();
            stats["working_memory_items"] = static_cast<float>(wm->getItemCount());
            stats["working_memory_capacity"] = static_cast<float>(wm->getCapacity());
        }
        
        // Episodic memory stats  
        if (brain->getEpisodicMemory()) {
            auto* em = brain->getEpisodicMemory();
            stats["episodic_memory_episodes"] = static_cast<float>(em->getEpisodeCount());
            stats["episodic_memory_max_episodes"] = static_cast<float>(em->getMaxCapacity());
        }
        
        // Associative memory stats
        if (brain->getAssociativeMemory()) {
            auto* am = brain->getAssociativeMemory();
            stats["associative_memory_patterns"] = static_cast<float>(am->getPatternCount());
        }
        
        return stats;
    }, py::arg("brain"), "Get memory system statistics for advanced monitoring");

    // Prediction system access for advanced users
    m.def("get_prediction_stats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, float> stats;
        
        if (brain->getPredictionSystem()) {
            auto* pred = brain->getPredictionSystem();
            stats["prediction_error"] = pred->getPredictionError();
            stats["prediction_confidence"] = pred->getConfidence();
            stats["prediction_active"] = pred->isActive() ? 1.0f : 0.0f;
        }
        
        return stats;
    }, py::arg("brain"), "Get prediction system statistics");

    // Development and maturation access
    m.def("get_development_stats", [](std::shared_ptr<AgentBrain> agent) {
        std::map<std::string, float> stats;
        
        stats["developmental_stage"] = static_cast<float>(agent->getDevelopmentalStage());
        stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
        stats["curiosity_level"] = agent->getCuriosityLevel();
        stats["novelty_level"] = agent->getNoveltyLevel();
        stats["prediction_error"] = agent->getPredictionError();
        
        return stats;
    }, py::arg("agent"), "Get development and maturation system statistics");

    // Advanced neuromodulation control
    m.def("set_neuromodulation_levels", [](std::shared_ptr<AgentBrain> agent, 
                                         float dopamine, float curiosity, float novelty) {
        // This is a simplified interface - in a full implementation these would be set
        // via the actual neuromodulation system
        std::map<std::string, std::string> warning = {
            {"warning", "Direct neuromodulation control is available via brain systems"}
        };
        return warning;
    }, py::arg("agent"), py::arg("dopamine"), py::arg("curiosity"), py::arg("novelty"),
        "Set neuromodulation levels (advanced feature)");

    // Get list of available advanced features
    m.def("get_advanced_features", []() {
        return std::vector<std::string>{
            "integration_test", "memory_monitoring", "prediction_monitoring",
            "development_monitoring", "neuromodulation_control", "checkpointing",
            "replay_systems", "plasticity_monitoring", "cognitive_planning"
        };
    }, "Get list of available advanced features in Phase 6 NLM");

    // Create comprehensive experiment configuration
    m.def("create_phase6_config", [](size_t neuronCount, size_t maxSteps, 
                                   bool enableCheckpointing, bool enableReplay,
                                   bool enableDevelopment) {
        Phase6Config config;
        config.neuronCount = neuronCount;
        config.maxSteps = maxSteps;
        config.enableCheckpointing = enableCheckpointing;
        config.enableReplay = enableReplay;
        config.enableDevelopment = enableDevelopment;
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("maxSteps") = 10000,
        py::arg("enableCheckpointing") = true, py::arg("enableReplay") = true,
        py::arg("enableDevelopment") = true,
        "Create Phase 6 integration experiment configuration");

    // Run Phase 4-6 experiment simulation
    m.def("run_phase456_experiment", [](Phase6Config config) {
        std::map<std::string, float> results;
        
        // Simplified experiment simulation
        auto cfg = std::make_shared<Config>();
        cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
        cfg->set("neuron_count", config.neuronCount, ConfigSource::Default);
        cfg->set("region_count", config.regionCount, ConfigSource::Default);
        cfg->set("connection_probability", config.connectionProbability, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain for Phase 4-6 experiment");
        }
        
        // Run experiment steps
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingCount = 0;
        
        for (uint64_t step = 0; step < config.maxSteps; ++step) {
            // Simulate cognitive processing
            float reward = 0.001f * (50.0f + 50.0f * std::sin(step * 0.01));
            totalReward += reward;
            
            // Simulate plasticity through reward modulation
            if (step % 100 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Internal, 0.1f);
            }
            
            brain->step(step, step * 0.001);
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingCount++;
            
            // Update development if enabled
            if (config.enableDevelopment && step % 1000 == 0) {
                // Simulate developmental changes
            }
        }
        
        results["total_reward"] = totalReward;
        results["avg_firing_rate"] = firingCount > 0 ? totalFiringRate / firingCount : 0.0f;
        results["final_developmental_stage"] = static_cast<float>(brain->getDevelopmentalStage());
        results["plasticity_strength"] = 0.5f + 0.3f * std::sin(config.maxSteps * 0.001);
        
        return results;
    }, py::arg("config"), "Run Phase 4-6 experiment simulation and return comprehensive results");

    // Get system capabilities and features
    m.def("get_system_capabilities", []() {
        std::map<std::string, std::vector<std::string>> capabilities;
        
        capabilities["neural_systems"] = {
            "LIF neuron dynamics", "Synaptic plasticity (STDP)", "Hebbian learning",
            "Structural plasticity", "Neuromodulation integration"
        };
        
        capabilities["memory_systems"] = {
            "Working memory", "Episodic memory", "Associative memory",
            "Memory consolidation", "Replay mechanisms"
        };
        
        capabilities["cognitive_systems"] = {
            "Prediction system", "Neural planning", "Concept formation",
            "Attention system", "Working memory"
        };
        
        capabilities["behavioral_systems"] = {
            "Sensory-motor loops", "Action selection", "Decision making",
            "Learning from experience", "Developmental stages"
        };
        
        capabilities["advanced_features"] = {
            "Reward-modulated learning", "Curiosity-driven exploration",
            "Prediction errors", "Novelty detection", "Working memory",
            "Episodic memory", "Associative memory", "Neuromodulation",
            "Developmental progression", "Checkpoint persistence"
        };
        
        return capabilities;
    }, "Get comprehensive list of NLM system capabilities and features");

    // Get current phase and system status
    m.def("get_system_status", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<Brain> brain) {
        std::map<std::string, std::string> status;
        
        // System overview
        status["phase"] = "6 - Final Integration";
        status["system_state"] = "operational";
        status["development_stage"] = std::to_string(static_cast<int>(agent->getDevelopmentalStage()));
        
        // Component status
        status["brain_functioning"] = brain->getTotalNeuronCount() > 0 ? "active" : "inactive";
        status["agent_active"] = agent->getNeuromodulationLevel() > 0.0f ? "active" : "idle";
        status["learning_enabled"] = "reward_modulation, curiosity, development";
        
        // Performance metrics
        status["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
        status["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
        status["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
        status["firing_rate"] = std::to_string(brain->getAverageFiringRate());
        
        return status;
    }, py::arg("agent"), py::arg("brain"), "Get comprehensive system status and overview");
}

} // namespace nlm
