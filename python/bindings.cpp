#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Forward declarations for integrated systems
namespace nlm {
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    class PredictionSystem;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class DevelopmentSystem;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
    class SpikeSystem;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    class CheckpointManager;
}

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Phase 6: Final Integrated Artificial Brain
        
        Provides comprehensive access to all integrated brain systems:
        - Central neural computation with LIF neurons
        - Working memory, episodic memory, and associative memory
        - Prediction systems and action planning
        - Concept formation and attention mechanisms
        - Development stages and neural maturation
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Checkpointing and state persistence
        
        Key features:
        - Event-driven spike propagation with synaptic delays
        - Advanced memory systems with encoding/replay capabilities
        - Neuromodulation-driven learning and exploration
        - Developmental trajectory effects on plasticity
        - Comprehensive world simulation interface
        - Advanced sensory processing (vision, audio, internal signals)
        - Complete motor action system with multiple action types
        - Real-time statistics and monitoring
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ========== BASIC IDENTIFIERS ==========

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
        .def("__ne__", &SynapseId::operator!=)
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

    // ========== CONFIGURATION ==========

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or key=value file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("set", static_cast<void (Config::*)(const std::string&, const ConfigValue&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", static_cast<void (Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("set", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set a configuration value")
        .def("getInt", static_cast<std::optional<int> (Config::*)(const std::string&) const>(&Config::get<int>),
             py::arg("key"), "Get an integer configuration value")
        .def("getString", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get<std::string>),
             py::arg("key"), "Get a string configuration value")
        .def("getDouble", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get<double>),
             py::arg("key"), "Get a double configuration value")
        .def("getBool", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get<bool>),
             py::arg("key"), "Get a boolean configuration value")
        .def("getOrInt", static_cast<int (Config::*)(const std::string&, int) const>(&Config::getOr<int>),
             py::arg("key"), py::arg("default"), "Get an integer with default")
        .def("getOrString", static_cast<std::string (Config::*)(const std::string&, std::string) const>(&Config::getOr<std::string>),
             py::arg("key"), py::arg("default"), "Get a string with default")
        .def("getOrDouble", static_cast<double (Config::*)(const std::string&, double) const>(&Config::getOr<double>),
             py::arg("key"), py::arg("default"), "Get a double with default")
        .def("getOrBool", static_cast<bool (Config::*)(const std::string&, bool) const>(&Config::getOr<bool>),
             py::arg("key"), py::arg("default"), "Get a boolean with default")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ========== SENSORY INPUT ==========

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

    // ========== ACTION SYSTEM ==========

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

    // ========== WORLD SYSTEM ==========

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

    // ========== BRAIN CLASS (Phase 6: Fully Integrated) ==========

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class - Phase 6: Integrated Artificial Brain)pbdoc")
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
             "Save brain state to file (checkpoint)")
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

    // ========== MEMORY SYSTEMS (Phase 6 Integrated) ==========

    // Working Memory - transient active information
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory - transient active information)pbdoc")
        .def("initialize", &Brain::getWorkingMemory,
             "Initialize working memory with brain reference")
        .def("update", [](const std::shared_ptr<NeuralWorkingMemory>& wm, float timestep) {
            if (wm) wm->update(timestep);
        }, py::arg("timestep"), "Update working memory")
        .def("storeToNeuron", [](const std::shared_ptr<NeuralWorkingMemory>& wm, NeuronId neuron, float activation) {
            if (wm) wm->storeToNeuron(neuron, activation);
        }, py::arg("neuron"), py::arg("activation"), "Store neuron activation in working memory")
        .def("getMemoryNeurons", [](const std::shared_ptr<NeuralWorkingMemory>& wm) {
            if (wm) return wm->getMemoryNeurons();
            return std::vector<NeuronId>();
        }, "Get neurons in working memory")
        .def("getActiveTraces", [](const std::shared_ptr<NeuralWorkingMemory>& wm) {
            if (wm) return wm->getActiveTraces();
            return size_t(0);
        }, "Get number of active traces")
        .def("clear", [](const std::shared_ptr<NeuralWorkingMemory>& wm) {
            if (wm) wm->clear();
        }, "Clear working memory");

    // Episodic Memory - experience storage with replay capability
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory - experience storage)pbdoc")
        .def("initialize", &Brain::getEpisodicMemory,
             "Initialize episodic memory with brain reference")
        .def("storeEpisode", [](const std::shared_ptr<NeuralEpisodicMemory>& em, const EpisodicMemoryItem& episode) {
            if (em) em->storeEpisode(episode);
        }, py::arg("episode"), "Store an episodic memory")
        .def("getEpisodesForReplay", [](const std::shared_ptr<NeuralEpisodicMemory>& em, size_t count) {
            if (em) return em->getEpisodesForReplay(count);
            return std::vector<const EpisodicMemoryItem*>();
        }, py::arg("count"), "Get episodes for replay")
        .def("replayEpisode", [](const std::shared_ptr<NeuralEpisodicMemory>& em, const EpisodicMemoryItem* episode) {
            if (em && episode) em->replayEpisode(episode);
        }, py::arg("episode"), "Replay an episodic memory")
        .def("getEpisodeCount", [](const std::shared_ptr<NeuralEpisodicMemory>& em) {
            if (em) return em->getEpisodeCount();
            return size_t(0);
        }, "Get number of stored episodes")
        .def("consolidate", [](const std::shared_ptr<NeuralEpisodicMemory>& em, float threshold) {
            if (em) em->consolidate(threshold);
        }, py::arg("threshold"), "Consolidate important memories")
        .def("clear", [](const std::shared_ptr<NeuralEpisodicMemory>& em) {
            if (em) em->clear();
        }, "Clear episodic memory");

    // Associative Memory - pattern associations
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory - pattern associations)pbdoc")
        .def("initialize", &Brain::getAssociativeMemory,
             "Initialize associative memory with brain reference")
        .def("associate", [](const std::shared_ptr<NeuralAssociativeMemory>& am, const std::vector<float>& pattern, float strength) {
            if (am) am->associate(pattern, strength);
        }, py::arg("pattern"), py::arg("strength"), "Create an association")
        .def("recall", [](const std::shared_ptr<NeuralAssociativeMemory>& am, const std::vector<float>& pattern, float threshold) {
            if (am) return am->recall(pattern, threshold);
            return std::vector<float>();
        }, py::arg("pattern"), py::arg("threshold"), "Recall associated pattern")
        .def("getAssociationsCount", [](const std::shared_ptr<NeuralAssociativeMemory>& am) {
            if (am) return am->getAssociationsCount();
            return size_t(0);
        }, "Get number of associations")
        .def("clear", [](const std::shared_ptr<NeuralAssociativeMemory>& am) {
            if (am) am->clear();
        }, "Clear associative memory");

    // ========== PREDICTION SYSTEM ==========

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def("initialize", &Brain::getPredictionSystem,
             "Initialize prediction system with brain reference")
        .def("update", [](const std::shared_ptr<PredictionSystem>& ps, const SensoryInput& input) {
            if (ps) ps->update(input);
        }, py::arg("input"), "Update prediction with sensory input")
        .def("getPredictionError", [](const std::shared_ptr<PredictionSystem>& ps) {
            if (ps) return ps->getPredictionError();
            return 0.0f;
        }, "Get prediction error")
        .def("getConfidence", [](const std::shared_ptr<PredictionSystem>& ps) {
            if (ps) return ps->getConfidence();
            return 0.0f;
        }, "Get prediction confidence")
        .def("getExpectedInput", [](const std::shared_ptr<PredictionSystem>& ps) {
            if (ps) return ps->getExpectedInput();
            return SensoryInput();
        }, "Get expected sensory input");

    // ========== COGNITION SYSTEMS ==========

    // Neural Planner - action sequence evaluation
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def("initialize", &Brain::getPlanner,
             "Initialize planner with brain reference")
        .def("plan", [](const std::shared_ptr<NeuralPlanner>& np, const std::vector<ActionType>& options) {
            if (np) return np->plan(options);
            return std::vector<ActionType>();
        }, py::arg("options"), "Plan an action sequence")
        .def("setPlanningDepth", [](const std::shared_ptr<NeuralPlanner>& np, size_t depth) {
            if (np) np->setPlanningDepth(depth);
        }, py::arg("depth"), "Set planning depth")
        .def("getActionValue", [](const std::shared_ptr<NeuralPlanner>& np, ActionType action) {
            if (np) return np->getActionValue(action);
            return 0.0f;
        }, py::arg("action"), "Get action value");

    // Concept Formation - pattern discovery
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def("initialize", &Brain::getConceptFormation,
             "Initialize concept formation with brain reference")
        .def("processPattern", [](const std::shared_ptr<ConceptFormation>& cf, const std::vector<float>& pattern) {
            if (cf) cf->processPattern(pattern);
        }, py::arg("pattern"), "Process a pattern for concept formation")
        .def("getConcepts", [](const std::shared_ptr<ConceptFormation>& cf) {
            if (cf) return cf->getConcepts();
            return std::vector<std::vector<float>>();
        }, "Get formed concepts")
        .def("getActivationStrength", [](const std::shared_ptr<ConceptFormation>& cf, size_t conceptId) {
            if (cf) return cf->getActivationStrength(conceptId);
            return 0.0f;
        }, py::arg("conceptId"), "Get concept activation strength");

    // Attentional Selection - competitive focus
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection for focus)pbdoc")
        .def("initialize", &Brain::getAttention,
             "Initialize attentional selection with brain reference")
        .def("update", [](const std::shared_ptr<AttentionalSelection>& att, float timestep) {
            if (att) att->update(timestep);
        }, py::arg("timestep"), "Update attentional selection")
        .def("processCompetition", [](const std::shared_ptr<AttentionalSelection>& att, const std::vector<NeuronId>& competitors) {
            if (att) return att->processCompetition(competitors);
            return NeuronId();
        }, py::arg("competitors"), "Process competitive selection")
        .def("setInhibitionStrength", [](const std::shared_ptr<AttentionalSelection>& att, float strength) {
            if (att) att->setInhibitionStrength(strength);
        }, py::arg("strength"), "Set inhibition strength");

    // ========== DEVELOPMENT SYSTEM ==========

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for neural maturation)pbdoc")
        .def("initialize", &Brain::getDevelopmentSystem,
             "Initialize development system with brain reference")
        .def("update", [](const std::shared_ptr<DevelopmentSystem>& ds, float timestep) {
            if (ds) ds->update(timestep);
        }, py::arg("timestep"), "Update development system")
        .def("getCurrentStage", [](const std::shared_ptr<DevelopmentSystem>& ds) {
            if (ds) return ds->getCurrentStage();
            return DevelopmentalStage::Initial;
        }, "Get current developmental stage")
        .def("setMaturationRate", [](const std::shared_ptr<DevelopmentSystem>& ds, float rate) {
            if (ds) ds->setMaturationRate(rate);
        }, py::arg("rate"), "Set maturation rate")
        .def("getPlasticityModulation", [](const std::shared_ptr<DevelopmentSystem>& ds) {
            if (ds) return ds->getPlasticityModulation();
            return 1.0f;
        }, "Get plasticity modulation factor");

    // ========== NEUROMODULATION SYSTEMS ==========

    // Dopamine - reward and reinforcement
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - reward and reinforcement)pbdoc")
        .def("initialize", &Brain::getDopamine,
             "Initialize dopamine system with brain reference")
        .def("update", [](const std::shared_ptr<Dopamine>& d, float reward, float predictionError) {
            if (d) d->update(reward, predictionError);
        }, py::arg("reward"), py::arg("predictionError"), "Update dopamine with reward and prediction error")
        .def("getLevel", [](const std::shared_ptr<Dopamine>& d) {
            if (d) return d->getLevel();
            return 0.0f;
        }, "Get dopamine level")
        .def("getPlasticityFactor", [](const std::shared_ptr<Dopamine>& d) {
            if (d) return d->getPlasticityFactor();
            return 1.0f;
        }, "Get dopamine's effect on plasticity");

    // Curiosity - exploration motivation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator - exploration motivation)pbdoc")
        .def("initialize", &Brain::getCuriosity,
             "Initialize curiosity system with brain reference")
        .def("update", [](const std::shared_ptr<Curiosity>& c, float novelty, float explorationValue) {
            if (c) c->update(novelty, explorationValue);
        }, py::arg("novelty"), py::arg("explorationValue"), "Update curiosity with novelty and exploration")
        .def("getLevel", [](const std::shared_ptr<Curiosity>& c) {
            if (c) return c->getLevel();
            return 0.0f;
        }, "Get curiosity level")
        .def("getExplorationValue", [](const std::shared_ptr<Curiosity>& c) {
            if (c) return c->getExplorationValue();
            return 0.0f;
        }, "Get exploration motivation value");

    // Novelty - novelty detection
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator - novelty detection)pbdoc")
        .def("initialize", &Brain::getNovelty,
             "Initialize novelty system with brain reference")
        .def("update", [](const std::shared_ptr<Novelty>& n, const SensoryInput& input) {
            if (n) n->update(input);
        }, py::arg("input"), "Update novelty detection with sensory input")
        .def("getLevel", [](const std::shared_ptr<Novelty>& n) {
            if (n) return n->getLevel();
            return 0.0f;
        }, "Get novelty level")
        .def("getNoveltyScore", [](const std::shared_ptr<Novelty>& n) {
            if (n) return n->getNoveltyScore();
            return 0.0f;
        }, "Get novelty detection score");

    // Prediction Error - learning signal
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator - learning signal)pbdoc")
        .def("initialize", &Brain::getPredictionErrorSignal,
             "Initialize prediction error system with brain reference")
        .def("update", [](const std::shared_ptr<PredictionError>& pe, const SensoryInput& input) {
            if (pe) pe->update(input);
        }, py::arg("input"), "Update prediction error with sensory input")
        .def("getLevel", [](const std::shared_ptr<PredictionError>& pe) {
            if (pe) return pe->getLevel();
            return 0.0f;
        }, "Get prediction error level")
        .def("getPrecision", [](const std::shared_ptr<PredictionError>& pe) {
            if (pe) return pe->getPrecision();
            return 1.0f;
        }, "Get prediction error precision");

    // ========== PLASTICITY SYSTEMS ==========

    // Spike System - event-driven neural dynamics
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike system for event-driven neural dynamics)pbdoc")
        .def("registerHandler", [](const std::shared_ptr<SpikeSystem>& ss, std::function<void(const SpikeEvent&)> handler) {
            if (ss) ss->registerHandler(handler);
        }, py::arg("handler"), "Register spike event handler")
        .def("registerDelayedHandler", [](const std::shared_ptr<SpikeSystem>& ss, std::function<void(const DelayedSpikeEvent&)> handler) {
            if (ss) ss->registerDelayedHandler(handler);
        }, py::arg("handler"), "Register delayed spike event handler")
        .def("queueSpike", [](const std::shared_ptr<SpikeSystem>& ss, const SpikeEvent& event) {
            if (ss) ss->queueSpike(event);
        }, py::arg("event"), "Queue a spike event")
        .def("processDelayedSpikes", [](const std::shared_ptr<SpikeSystem>& ss, SimulationStep step, Timestamp time) {
            if (ss) ss->processDelayedSpikes(step, time);
        }, py::arg("step"), py::arg("time"), "Process delayed spike events")
        .def("processSpikes", [](const std::shared_ptr<SpikeSystem>& ss, SimulationStep step) {
            if (ss) ss->processSpikes(step);
        }, py::arg("step"), "Process spike events")
        .def("getPendingSpikeCount", [](const std::shared_ptr<SpikeSystem>& ss) {
            if (ss) return ss->getPendingSpikeCount();
            return size_t(0);
        }, "Get number of pending spike events")
        .def("getMostActiveNeurons", [](const std::shared_ptr<SpikeSystem>& ss, size_t count) {
            if (ss) return ss->getMostActiveNeurons(count);
            return std::vector<NeuronId>();
        }, py::arg("count"), "Get most active neurons");

    // STDP - spike-timing dependent plasticity
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-timing dependent plasticity)pbdoc")
        .def("configure", [](const std::shared_ptr<STDP>& stdp, float ltpWeight, float ltdWeight, float tau) {
            if (stdp) stdp->configure(ltpWeight, ltdWeight, tau);
        }, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"), "Configure STDP parameters")
        .def("setLTPWeight", [](const std::shared_ptr<STDP>& stdp, float weight) {
            if (stdp) stdp->setLTPWeight(weight);
        }, py::arg("weight"), "Set LTP weight")
        .def("setLTDWeight", [](const std::shared_ptr<STDP>& stdp, float weight) {
            if (stdp) stdp->setLTDWeight(weight);
        }, py::arg("weight"), "Set LTD weight")
        .def("setTau", [](const std::shared_ptr<STDP>& stdp, float tau) {
            if (stdp) stdp->setTau(tau);
        }, py::arg("tau"), "Set STDP time constant")
        .def("getLTPWeight", [](const std::shared_ptr<STDP>& stdp) {
            if (stdp) return stdp->getLTPWeight();
            return 0.0f;
        }, "Get LTP weight")
        .def("getLTDWeight", [](const std::shared_ptr<STDP>& stdp) {
            if (stdp) return stdp->getLTDWeight();
            return 0.0f;
        }, "Get LTD weight");

    // Hebbian - simultaneous pre-post spike plasticity
    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning - simultaneous pre-post spike plasticity)pbdoc")
        .def("configure", [](const std::shared_ptr<Hebbian>& hebb, float learningRate, float baseline) {
            if (hebb) hebb->configure(learningRate, baseline);
        }, py::arg("learningRate"), py::arg("baseline"), "Configure Hebbian parameters")
        .def("setLearningRate", [](const std::shared_ptr<Hebbian>& hebb, float rate) {
            if (hebb) hebb->setLearningRate(rate);
        }, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", [](const std::shared_ptr<Hebbian>& hebb) {
            if (hebb) return hebb->getLearningRate();
            return 0.0f;
        }, "Get learning rate");

    // Structural Plasticity - synapse formation and elimination
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity - synapse formation and elimination)pbdoc")
        .def("update", [](const std::shared_ptr<StructuralPlasticity>& sp, const std::shared_ptr<Brain>& brain, const RandomGenerator& rng) {
            if (sp && brain) sp->update(brain.get(), rng);
        }, py::arg("brain"), py::arg("rng"), "Update structural plasticity")
        .def("setSynaptogenesisRate", [](const std::shared_ptr<StructuralPlasticity>& sp, float rate) {
            if (sp) sp->setSynaptogenesisRate(rate);
        }, py::arg("rate"), "Set synaptogenesis rate")
        .def("setPruningRate", [](const std::shared_ptr<StructuralPlasticity>& sp, float rate) {
            if (sp) sp->setPruningRate(rate);
        }, py::arg("rate"), "Set pruning rate")
        .def("getSynaptogenesisRate", [](const std::shared_ptr<StructuralPlasticity>& sp) {
            if (sp) return sp->getSynaptogenesisRate();
            return 0.0f;
        }, "Get synaptogenesis rate")
        .def("getPruningRate", [](const std::shared_ptr<StructuralPlasticity>& sp) {
            if (sp) return sp->getPruningRate();
            return 0.0f;
        }, "Get pruning rate");

    // ========== CHECKPOINT SYSTEM ==========

    py::class_<CheckpointManager>(m, "CheckpointManager", R"pbdoc(Checkpoint manager for brain state persistence)pbdoc")
        .def("configure", [](const std::shared_ptr<CheckpointManager>& cm, const std::string& directory, size_t maxCheckpoints, size_t maxMemoryMB, bool compression) {
            if (cm) cm->configure(directory, maxCheckpoints, maxMemoryMB, compression);
        }, py::arg("directory"), py::arg("maxCheckpoints"), py::arg("maxMemoryMB"), py::arg("compression"), "Configure checkpoint manager")
        .def("update", [](const std::shared_ptr<CheckpointManager>& cm, SimulationStep step, Timestamp time) {
            if (cm) cm->update(step, time);
        }, py::arg("step"), py::arg("time"), "Update checkpoint manager")
        .def("save", [](const std::shared_ptr<CheckpointManager>& cm, const std::string& filepath) {
            if (cm) return cm->save(filepath);
            return false;
        }, py::arg("filepath"), "Save checkpoint")
        .def("load", [](const std::shared_ptr<CheckpointManager>& cm, const std::string& filepath) {
            if (cm) return cm->load(filepath);
            return false;
        }, py::arg("filepath"), "Load checkpoint")
        .def("getBytesWritten", [](const std::shared_ptr<CheckpointManager>& cm) {
            if (cm) return cm->getBytesWritten();
            return size_t(0);
        }, "Get total bytes written");

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

    // ========== GLOBAL CONSTANTS ==========

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ========== ENUMERATION HELPERS ==========

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)")
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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ========== VERSION ==========

    m.attr("__version__") = "0.1.0";
    m.attr("__phase__") = "6: Final Integration";
    }

} // namespace nlm