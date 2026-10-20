#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <chrono>
#include <random>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for NLM
class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidParameterError : public RuntimeError {
public:
    explicit InvalidParameterError(const std::string& param, const std::string& reason)
        : RuntimeError("Invalid parameter '" + param + "': " + reason) {}
};

class InitializationError : public RuntimeError {
public:
    explicit InitializationError(const std::string& component, const std::string& reason)
        : RuntimeError("Failed to initialize " + component + ": " + reason) {}
};

class OperationError : public RuntimeError {
public:
    explicit OperationError(const std::string& operation, const std::string& reason)
        : RuntimeError("Operation '" + operation + "' failed: " + reason) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, Action,
        and advanced neuromodulation, development, and learning systems.
        
        Key Features:
        - Complete brain lifecycle management
        - Reward, neuromodulation, and development systems
        - Curiosity, novelty detection, and prediction error
        - Advanced learning mechanisms
        - Error handling and validation
        - Comprehensive documentation and type hints
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<RuntimeError>(m, "NLMError");
    py::register_exception<InvalidParameterError>(m, "InvalidParameterError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<OperationError>(m, "OperationError");

    // Forward declarations for complex types
    py::class_<NeuronId>(m, "NeuronId")
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

    py::class_<SynapseId>(m, "SynapseId")
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

    py::class_<RegionId>(m, "RegionId")
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

    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Type enumeration
    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
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

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Configuration class
    py::class_<Config>(m, "Config")
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
             "Get a summary string of the configuration");

    // Sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType)
        .def("getData", &SensoryInput::getData)
        .def("getDimensions", &SensoryInput::getDimensions)
        .def("getTimestamp", &SensoryInput::getTimestamp)
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"));

    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) { self.setData(data); }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) { self.setData(data); }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action and world object classes
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody")
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

    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept")
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
    py::class_<SimpleWorld>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept)
        .def("getAgentBody", &SimpleWorld::getAgentBody)
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

    // Neuromodulation system - Reward class
    py::class_<Reward>(m, "Reward")
        .def(py::init<>())
        .def("getValue", &Reward::getValue)
        .def("setValue", &Reward::setValue, py::arg("value"))
        .def("add", &Reward::add, py::arg("delta"))
        .def("reset", &Reward::reset)
        .def("computeReward", &Reward::computeReward, py::arg("observation"))
        .def("getHistory", &Reward::getHistory)
        .def("clearHistory", &Reward::clearHistory);

    // Neuromodulation system - Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", py::module_local())
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor)
        .def("update", &Neuromodulator::update, py::arg("dt"));

    // Neuromodulation system - Dopamine
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"));

    // Neuromodulation system - Acetylcholine (default implementation)
    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine")
        .def(py::init<>())
        .def("getName", &Acetylcholine::getName)
        .def("getLevel", &Acetylcholine::getLevel)
        .def("setLevel", &Acetylcholine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Acetylcholine::getPlasticityFactor)
        .def("update", &Acetylcholine::update, py::arg("dt"));

    // Neuromodulation system - Norepinephrine (default implementation)
    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine")
        .def(py::init<>())
        .def("getName", &Norepinephrine::getName)
        .def("getLevel", &Norepinephrine::getLevel)
        .def("setLevel", &Norepinephrine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Norepinephrine::getPlasticityFactor)
        .def("update", &Norepinephrine::update, py::arg("dt"));

    // Neuromodulation system - Serotonin (default implementation)
    py::class_<Serotonin, Neuromodulator>(m, "Serotonin")
        .def(py::init<>())
        .def("getName", &Serotonin::getName)
        .def("getLevel", &Serotonin::getLevel)
        .def("setLevel", &Serotonin::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Serotonin::getPlasticityFactor)
        .def("update", &Serotonin::update, py::arg("dt"));

    // Neuromodulation system - Curiosity
    py::class_<Curiosity>(m, "Curiosity")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("getLevel", &Curiosity::getLevel)
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &Curiosity::reset);

    // Neuromodulation system - Novelty
    py::class_<Novelty>(m, "Novelty")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"))
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& current, 
                               const std::vector<float>& previous) {
            self.detectNovelty(current, previous);
        }, py::arg("current"), py::arg("previous"))
        .def("update", &Novelty::update, py::arg("dt"))
        .def("getHistory", &Novelty::getHistory)
        .def("clearHistory", &Novelty::clearHistory);

    // Neuromodulation system - PredictionError
    py::class_<PredictionError>(m, "PredictionError")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"))
        .def("getError", &PredictionError::getError)
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &PredictionError::getHistory)
        .def("clearHistory", &PredictionError::clearHistory)
        .def("getMagnitude", &PredictionError::getMagnitude);

    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advanceStage", &DevelopmentSystem::advanceStage)
        .def("getStageName", &DevelopmentSystem::getStageName)
        .def("update", &DevelopmentSystem::update)
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress)
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge)
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"));

    // Core brain class
    py::class_<Brain>(m, "Brain")
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
        // Advanced systems access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
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
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random number generator")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules")
        .def("develop", &Brain::develop,
             "Update developmental state")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals");

    // Agent brain interface
    py::class_<AgentBrain>(m, "AgentBrain")
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

    // Factory functions with error handling
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set essential default configuration
        config->set("brain.neuronCount", 1000);
        config->set("brain.regionCount", 4);
        config->set("brain.connectionProbability", 0.1f);
        
        // Set neuromodulation defaults
        config->set("neuromodulation.rewardSignal", 0.0f);
        config->set("neuromodulation.curiosityWeight", 0.5f);
        config->set("neuromodulation.noveltyWeight", 0.3f);
        config->set("neuromodulation.predictionErrorWeight", 0.4f);
        
        // Set development defaults
        config->set("development.initialStage", "CriticalPeriod");
        config->set("development.plasticityFactor", 1.0f);
        config->set("development.criticalPeriodDuration", 100);
        
        // Set learning parameters
        config->set("learning.rate", 0.01f);
        config->set("learning.momentum", 0.9f);
        config->set("learning.discountFactor", 0.95f);
        
        // Set simulation parameters
        config->set("simulation.trialDuration", 1000);
        config->set("simulation.maxMemory", 1000.0f);
        
        return config;
    }, R"pbdoc(
        Create a default configuration with essential parameters.
        
        Returns a Config object pre-populated with sensible defaults for:
        - Neural architecture (neuron count, regions, connectivity)
        - Neuromodulation systems (reward, curiosity, novelty, prediction error)
        - Development parameters (stages, plasticity factors)
        - Learning mechanisms (rate, momentum, discounting)
        - Simulation settings (trial duration, memory limits)
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw InvalidParameterError("config", "cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw InitializationError("brain", "failed to initialize");
        }
        return brain;
    }, py::arg("config"), R"pbdoc(
        Create and initialize a new brain with configuration.
        
        Args:
            config: Configuration object for the brain
            
        Returns:
            Initialized Brain object
            
        Raises:
            InvalidParameterError: If config is null
            InitializationError: If brain initialization fails
    )pbdoc");

    m.def("createSimpleWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
       R"pbdoc(
        Create a configured simple world for simulation.
        
        Args:
            width: World width in pixels/units
            height: World height in pixels/units  
            visionWidth: Agent's visual field width
            visionHeight: Agent's visual field height
            
        Returns:
            Configured SimpleWorld object
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw InvalidParameterError("brain", "cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.
        
        Args:
            brain: Brain object to control
            
        Returns:
            AgentBrain object for world interaction
            
        Raises:
            InvalidParameterError: If brain is null
    )pbdoc");

    // Advanced helper functions
    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agentBrain, std::shared_ptr<SimpleWorld> world, 
                          size_t maxSteps) -> float {
        if (!agentBrain || !world) {
            throw InvalidParameterError("agentBrain or world", "cannot be null");
        }
        
        agentBrain->reset();
        float finalReward = 0.0f;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agentBrain->processSensoryInput(percept);
            
            // Decode motor command
            auto action = agentBrain->decodeMotorCommand();
            
            // Apply action in world
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Get reward from world
            auto result = world->getLastActionResult();
            if (result) {
                finalReward = result->reward;
            }
            
            // Check if episode ended
            if (result && !result->success) {
                break;
            }
            
            world->update(1.0f);
        }
        
        return finalReward;
    }, py::arg("agentBrain"), py::arg("world"), py::arg("maxSteps"),
       R"pbdoc(
        Run a complete episode and return final reward.
        
        This function simulates an agent interacting with a world until
        either the maximum steps are reached or the episode ends.
        
        Args:
            agentBrain: Agent brain interface
            world: World simulation environment  
            maxSteps: Maximum number of steps to simulate
            
        Returns:
            Final reward accumulated during the episode
            
        Raises:
            InvalidParameterError: If agentBrain or world is null
    )pbdoc");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw InvalidParameterError("brain", "cannot be null");
        }
        if (filepath.empty()) {
            throw InvalidParameterError("filepath", "cannot be empty");
        }
        if (!brain->save(filepath)) {
            throw OperationError("saveBrainState", "failed to save to file");
        }
    }, py::arg("brain"), py::arg("filepath"),
       R"pbdoc(
        Save brain state to file (checkpointing).
        
        Args:
            brain: Brain object to save
            filepath: Path to save file
            
        Raises:
            InvalidParameterError: If brain is null or filepath is empty
            OperationError: If save operation fails
    )pbdoc");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw InvalidParameterError("brain", "cannot be null");
        }
        if (filepath.empty()) {
            throw InvalidParameterError("filepath", "cannot be empty");
        }
        if (!brain->load(filepath)) {
            throw OperationError("loadBrainState", "failed to load from file");
        }
    }, py::arg("brain"), py::arg("filepath"),
       R"pbdoc(
        Load brain state from file.
        
        Args:
            brain: Brain object to load into
            filepath: Path to load file
            
        Raises:
            InvalidParameterError: If brain is null or filepath is empty
            OperationError: If load operation fails
    )pbdoc");

    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw InvalidParameterError("brain", "cannot be null");
        }
        
        std::map<std::string, std::variant<int, float, double, std::string>> stats;
        stats["neuronCount"] = brain->getTotalNeuronCount();
        stats["synapseCount"] = brain->getTotalSynapseCount();
        stats["activeNeuronCount"] = brain->getActiveNeuronCount();
        stats["firingNeuronCount"] = brain->getFiringNeuronCount();
        stats["averageFiringRate"] = brain->getAverageFiringRate();
        stats["excitementInhibitionRatio"] = brain->getExcitationInhibitionRatio();
        stats["totalSpikeCount"] = brain->getTotalSpikeCount();
        
        // Get developmental system stats
        auto devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            stats["developmentalStage"] = static_cast<int>(devSystem->getStage());
            stats["developmentalAge"] = devSystem->getDevelopmentalAge();
            stats["plasticityModifier"] = devSystem->getPlasticityModifier();
            stats["criticalPeriodProgress"] = devSystem->getCriticalPeriodProgress();
            stats["isCriticalPeriod"] = devSystem->isCriticalPeriod();
        }
        
        // Get neuromodulation system stats
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            stats["dopamineLevel"] = dopamine->getLevel();
        }
        
        auto curiosity = brain->getCuriosity();
        if (curiosity) {
            stats["curiosityLevel"] = curiosity->getLevel();
        }
        
        auto novelty = brain->getNovelty();
        if (novelty) {
            stats["noveltyLevel"] = novelty->getLevel();
        }
        
        auto predictionError = brain->getPredictionErrorSignal();
        if (predictionError) {
            stats["predictionError"] = predictionError->getError();
            stats["predictionErrorMagnitude"] = predictionError->getMagnitude();
        }
        
        return stats;
    }, py::arg("brain"),
       R"pbdoc(
        Get comprehensive brain statistics.
        
        Returns a dictionary containing various metrics about the brain's state,
        including neural activity, development status, and neuromodulation levels.
        
        Args:
            brain: Brain object to query
            
        Returns:
            Dictionary of brain statistics
            
        Raises:
            InvalidParameterError: If brain is null
    )pbdoc");

    m.def("buildNeuralNetwork", [](std::shared_ptr<Config> config, const std::string& name) -> std::shared_ptr<Brain> {
        if (!config) {
            throw InvalidParameterError("config", "cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        // Add default regions if specified
        if (!name.empty()) {
            // Could add regions based on name, but for now just initialize
            brain->addRegion(name);
        }
        
        if (!brain->initialize()) {
            throw InitializationError("neuralNetwork", "failed to initialize");
        }
        
        return brain;
    }, py::arg("config"), py::arg("name") = "",
       R"pbdoc(
        Build a complete neural network from configuration.
        
        Args:
            config: Configuration for the network
            name: Optional name for the network
            
        Returns:
            Initialized Brain object
            
        Raises:
            InvalidParameterError: If config is null
            InitializationError: If network initialization fails
    )pbdoc");

    m.def("resetEnvironment", [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agentBrain) {
        if (world) {
            world->reset();
        }
        if (agentBrain) {
            agentBrain->reset();
        }
    }, py::arg("world"), py::arg("agentBrain"),
       R"pbdoc(
        Reset simulation environment.
        
        Args:
            world: World to reset
            agentBrain: Agent brain to reset
    )pbdoc");

    m.def("benchmarkStep", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (!brain) {
            throw InvalidParameterError("brain", "cannot be null");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::map<std::string, double> results;
        results["totalSteps"] = static_cast<double>(steps);
        results["totalTimeMs"] = static_cast<double>(duration.count());
        results["avgTimePerStepMs"] = static_cast<double>(duration.count()) / steps;
        results["stepsPerSecond"] = steps / (duration.count() / 1000.0);
        
        return results;
    }, py::arg("brain"), py::arg("steps"),
       R"pbdoc(
        Benchmark brain stepping performance.
        
        Args:
            brain: Brain to benchmark
            steps: Number of steps to simulate
            
        Returns:
            Dictionary with performance metrics
            
        Raises:
            InvalidParameterError: If brain is null
    )pbdoc");

    m.def("validateConfig", [](std::shared_ptr<Config> config) -> bool {
        if (!config) {
            return false;
        }
        
        // Check essential configuration keys
        bool valid = true;
        
        auto neuronCount = config->getOr<size_t>("brain.neuronCount", 0);
        if (neuronCount == 0) {
            valid = false;
        }
        
        auto regionCount = config->getOr<size_t>("brain.regionCount", 0);
        if (regionCount == 0) {
            valid = false;
        }
        
        auto connProb = config->getOr<float>("brain.connectionProbability", -1.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            valid = false;
        }
        
        return valid;
    }, py::arg("config"),
       R"pbdoc(
        Validate configuration and return true if valid.
        
        Args:
            config: Configuration to validate
            
        Returns:
            True if configuration is valid, False otherwise
    )pbdoc");

    m.def("getConfigValidationError", [](std::shared_ptr<Config> config) -> std::string {
        if (!config) {
            return "Config is null";
        }
        
        std::string errors;
        
        auto neuronCount = config->getOr<size_t>("brain.neuronCount", 0);
        if (neuronCount == 0) {
            errors += "brain.neuronCount must be > 0; ";
        }
        
        auto regionCount = config->getOr<size_t>("brain.regionCount", 0);
        if (regionCount == 0) {
            errors += "brain.regionCount must be > 0; ";
        }
        
        auto connProb = config->getOr<float>("brain.connectionProbability", -1.0f);
        if (connProb < 0.0f || connProb > 1.0f) {
            errors += "brain.connectionProbability must be in range [0, 1]; ";
        }
        
        auto learningRate = config->getOr<float>("brain.learningRate", -1.0f);
        if (learningRate < 0.0f) {
            errors += "brain.learningRate must be >= 0; ";
        }
        
        return errors.empty() ? "Configuration is valid" : errors;
    }, py::arg("config"),
       R"pbdoc(
        Get human-readable validation errors for configuration.
        
        Args:
            config: Configuration to validate
            
        Returns:
            Error message if invalid, or "Configuration is valid" if valid
    )pbdoc");

    // Special constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm