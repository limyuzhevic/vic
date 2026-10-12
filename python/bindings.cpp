#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/agent/Action.hpp"
#include "../src/agent/WorldObject.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/ActionResult.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ========== TYPE DEFINITIONS ==========
    
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

    // ========== ENUMERATIONS ==========

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

    // ========== CONFIG CLASS ==========

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
        .def("get", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"),
             "Get configuration value as string")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (value) return py::int_(value.value());
            return py::none();
        }, py::arg("key"),
             "Get configuration value as integer")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) return value.value();
            return py::none();
        }, py::arg("key"),
             "Get configuration value as float")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) return value.value();
            return py::none();
        }, py::arg("key"),
             "Get configuration value as boolean")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default")
        .def("getOr", [](Config& self, const std::string& key, int64_t defaultValue) {
            auto value = self.get<int64,64_t>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? value.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
             "Set configuration value")
        .def("set", [](Config& self, const std::string& key, int64_t value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
             "Set configuration value")
        .def("set", [](Config& self, const std::string& key, double value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
             "Set configuration value")
        .def("set", [](Config& self, const std::string& key, bool value, int source = 3) {
            self.set(key, value, static_cast<ConfigSource>(source));
        }, py::arg("key"), py::arg("value"), py::arg("source") = 3,
             "Set configuration value")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ========== SENSORY INPUT CLASSES ==========

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

    // ========== ACTION CLASS ==========

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

    // ========== WORLD OBJECT CLASS ==========

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

    // ========== AGENT BODY CLASS ==========

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

    // ========== ACTION RESULT CLASS ==========

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // ========== SENSORY PERCEPT CLASS ==========

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

    // ========== SIMPLE WORLD CLASS ==========

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

    // ========== BRAIN CLASS ==========

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
             "Get pending spike event count")
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
        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system")
        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system")
        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system")
        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system")
        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error system")
        // Other systems
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
        .def("getNeuromodulationLevel", []() { return 0.5f; },  // Placeholder
             "Get neuromodulation level");

    // ========== AGENT BRAIN CLASS ==========

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

    // ========== FACTORY FUNCTIONS ==========

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

    // ========== CONSTANTS ==========

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ========== ADVANCED FUNCTIONALITY ==========

    // Benchmarking functionality
    m.def("runBenchmark", [](size_t neuronCount, size_t steps, size_t iterations = 1) {
        std::vector<double> times;
        for (size_t i = 0; i < iterations; ++i) {
            // Simple benchmark implementation
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto start = std::chrono::high_resolution_clock::now();
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }
            auto end = std::chrono::high_resolution_clock::now();
            
            double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
            times.push_back(timeMs);
        }
        
        double avgTime = 0.0;
        for (double t : times) avgTime += t;
        avgTime /= times.size();
        
        return py::dict(
            "neuron_count", neuronCount,
            "steps", steps,
            "iterations", iterations,
            "avg_time_ms", avgTime,
            "steps_per_second", (steps * 1000.0) / avgTime
        );
    }, py::arg("neuronCount"), py::arg("steps"), py::arg("iterations") = 1,
       "Run performance benchmark");

    // Serialization functionality
    m.def("saveBrain", [](const std::shared_ptr<Brain>& brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state to file");

    m.def("loadBrain", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        // Placeholder implementation - in real scenario would load from file
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("filepath"), "Load brain state from file");

    // Visualization helper
    m.def("generateVisionFromPercept", [](const SensoryPercept& percept) -> std::vector<float> {
        return percept.getVision();
    }, py::arg("percept"), "Extract vision data from sensory percept");

    m.def("convertActionToCommand", [](const Action& action) -> MotorCommand {
        // Convert action to motor command (simplified mapping)
        switch (action.getType()) {
            case ActionType::MoveForward: return MotorCommand::MoveForward;
            case ActionType::MoveBackward: return MotorCommand::MoveBackward;
            case ActionType::TurnLeft: return MotorCommand::TurnLeft;
            case ActionType::TurnRight: return MotorCommand::TurnRight;
            case ActionType::Interact: return MotorCommand::Interact;
            case ActionType::Wait: return MotorCommand::Wait;
            default: return MotorCommand::Wait;
        }
    }, py::arg("action"), "Convert Action to MotorCommand");

    // Error handling utilities
    m.def("getLastError", []() -> std::string {
        return "No error (placeholder)";
    }, "Get last error message");

    m.def("resetLastError", []() {
        // Reset error state
    }, "Reset error state");

    // Configuration convenience functions
    m.def("getDefaultBrainConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 1000, ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0, ConfigSource::Default);
        config->set("brain.v_rest", -70.0, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0, ConfigSource::Default);
        return config;
    }, "Get default brain configuration");

    m.def("createBrainFromConfig", [](const std::string& configJson) {
        auto config = std::make_shared<Config>();
        // In real implementation would parse JSON
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("configJson"), "Create brain from JSON configuration");

    // Development system helper
    m.def("updateDevelopmentSystem", [](std::shared_ptr<Brain> brain, double timestep) {
        if (auto devSystem = brain->getDevelopmentSystem()) {
            devSystem->update(brain.get(), 0);
        }
    }, py::arg("brain"), py::arg("timestep"), "Update development system");

    // Neuromodulation helper
    m.def("applyNeuromodulation", [](std::shared_ptr<Brain> brain, float reward, float predicted) {
        // Simplified neuromodulation
        auto curiosity = brain->getCuriosity();
        if (curiosity) {
            // Apply reward-based learning
        }
    }, py::arg("brain"), py::arg("reward"), py::arg("predicted"), "Apply neuromodulation");

    // Statistics and metrics
    m.def("computeMetrics", [](const std::shared_ptr<Brain>& brain) {
        return py::dict(
            "total_neurons", brain->getTotalNeuronCount(),
            "total_synapses", brain->getTotalSynapseCount(),
            "firing_neurons", brain->getFiringNeuronCount(),
            "active_neurons", brain->getActiveNeuronCount(),
            "total_spikes", brain->getTotalSpikeCount(),
            "avg_firing_rate", brain->getAverageFiringRate(),
            "e_i_ratio", brain->getExcitationInhibitionRatio(),
            "pending_spikes", brain->getPendingSpikeEventCount()
        );
    }, py::arg("brain"), "Compute brain statistics and metrics");

    // World utilities
    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f) {
        return WorldObject(x, y, type, value);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
       "Create a world object");

    // Convenience functions for common patterns
    m.def("runCompleteSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent, size_t steps) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Invalid simulation parameters");
        }
        
        brain->initialize();
        agent->initialize(*world);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return py::dict(
            "steps_completed", steps,
            "final_time", world->getSimulationTime(),
            "neurons_firing", brain->getFiringNeuronCount(),
            "total_spikes", brain->getTotalSpikeCount()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
       "Run complete simulation episode");

    // Version and information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Team";
    m.attr("__description__") = "NLM Python bindings with complete API";
}

} // namespace nlm
