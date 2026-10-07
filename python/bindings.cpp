#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Core system headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Neuromodulation headers
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"

// Memory headers
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

// Development headers
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

// Plasticity headers
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

// Cognition headers
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

// Performance headers
#include "../src/performance/Performance.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/OptimizedBrain.hpp"
#include "../src/performance/ParallelProcessing.hpp"
#include "../src/performance/SIMDVectorization.hpp"
#include "../src/performance/SparseConnectivity.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        --------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        Provides comprehensive bindings for all neural systems:
        - Neuromodulators (Dopamine, Acetylcholine, Norepinephrine, Serotonin, etc.)
        - Memory systems (Working, Episodic, Semantic, Procedural, Associative, Neural variants)
        - Development systems (DevelopmentSystem, Synaptogenesis, Pruning, Maturation)
        - Plasticity systems (PlasticityRule, HebbianRule, STDP, StructuralPlasticity)
        - Cognition systems (NeuralPlanner, ConceptFormation, etc.)
        - Performance optimizations (Performance, MemoryPool, etc.)
        - Core systems (Brain, Config, AgentBrain, SimpleWorld, Sensory, Motor)
        
        Features:
        - Pythonic properties and context managers
        - Comprehensive error handling
        - Rich documentation and examples
        - Backward compatible with existing code
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== Basic Type Definitions =====
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

    // ===== Enum Definitions =====
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

    // ===== Core Configuration =====
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

    // ===== Sensory Systems =====
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

    // ===== Motor Systems =====
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

    // ===== Agent State =====
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

    // ===== Environment =====
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

    // ===== Neuromodulation Systems =====
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulators)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error")
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"));

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator - attention and memory consolidation)pbdoc")
        .def(py::init<>())
        .def("getName", &Acetylcholine::getName)
        .def("getLevel", &Acetylcholine::getLevel)
        .def("setLevel", &Acetylcholine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Acetylcholine::getPlasticityFactor)
        .def("update", &Acetylcholine::update, py::arg("dt"));

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator - arousal and vigilance)pbdoc")
        .def(py::init<>())
        .def("getName", &Norepinephrine::getName)
        .def("getLevel", &Norepinephrine::getLevel)
        .def("setLevel", &Norepinephrine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Norepinephrine::getPlasticityFactor)
        .def("update", &Norepinephrine::update, py::arg("dt"));

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator - mood, impulsivity, and social behavior)pbdoc")
        .def(py::init<>())
        .def("getName", &Serotonin::getName)
        .def("getLevel", &Serotonin::getLevel)
        .def("setLevel", &Serotonin::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Serotonin::getPlasticityFactor)
        .def("update", &Serotonin::update, py::arg("dt"));

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive - exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity system");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal - reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue, "Get current reward value")
        .def("setValue", &Reward::setValue, py::arg("value"), "Set reward value")
        .def("add", &Reward::add, py::arg("delta"), "Accumulate reward")
        .def("reset", &Reward::reset, "Reset accumulated reward")
        .def("computeReward", &Reward::computeReward, py::arg("observation"),
             "Compute reward from environment state")
        .def("getHistory", &Reward::getHistory, "Get reward history")
        .def("clearHistory", &Reward::clearHistory, "Clear reward history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error - computes difference between predicted and actual values)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getError", &PredictionError::getError, "Get error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory, "Get error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get error magnitude for neuromodulation");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection - computes novelty from comparison with previous observations)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detectNovelty", [](Novelty& self, const py::list& observation, const py::list& previousObservation) {
            std::vector<float> obsVec(py::cast<std::vector<float>>(observation));
            std::vector<float> prevObsVec(py::cast<std::vector<float>>(previousObservation));
            self.detectNovelty(obsVec, prevObsVec);
        }, py::arg("observation"), py::arg("previousObservation"), "Detect novelty from observation")
        .def("detectNovelty", [](Novelty& self, const py::list& currentPattern, const py::list& previousPattern) {
            std::vector<float> curPattern(py::cast<std::vector<float>>(currentPattern));
            std::vector<float> prevPattern(py::cast<std::vector<float>>(previousPattern));
            self.detectNovelty(curPattern, prevPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"), "Detect novelty from sensory input pattern")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history");

    // ===== Memory Systems =====
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory - temporary active storage of information)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store item")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"), "Retrieve item")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"), "Check if item exists")
        .def("clear", &WorkingMemory::clear, "Clear all")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current size")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"), "Decay all items");

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory - storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"), "Store episode")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"), "Retrieve episode")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount, "Get episode count")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("clear", &EpisodicMemory::clear, "Clear all")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Memory consolidation");

    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory - gradually acquired knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"), "Store fact")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"), "Retrieve fact")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"), "Check if fact exists")
        .def("getAllFacts", &SemanticMemory::getAllFacts, "Get all facts")
        .def("clear", &SemanticMemory::clear, "Clear all");

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory - learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"), "Learn skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"), "Retrieve skill")
        .def("getAllSkills", &ProceduralMemory::getAllSkills, "Get all skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"), "Update proficiency")
        .def("clear", &ProceduralMemory::clear, "Clear all");

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory - relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"), "Create association")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"), "Get associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"), "Get association strength")
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"), "Update association")
        .def("clear", &AssociativeMemory::clear, "Clear all");

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory - implements working memory using neural dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f, "Store information")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"), "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"), "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"), "Get activation level")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"), "Set capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"), "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons, "Get neurons in memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"), "Strengthen memory")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition, "Run competition between traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"), "Check if neuron is winning")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity, "Get overall memory activity");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory - stores experiences in neural substrate)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"), "Store a new experience")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensoryPattern"), py::arg("maxResults") = 5, "Retrieve similar episodes")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10, "Retrieve episodes from time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5, "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5, "Retrieve episodes after action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"), "Replay an episode")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"), "Update episode relevance")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"), "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward, "Get average reward")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Consolidate old episodes")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"), "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled, "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"), "Get episodes for replay")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"), "Replay multiple episodes");

    // ===== Development Systems =====
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system - orchestrates neural development)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get stage name")
        .def("update", [](DevelopmentSystem& self, Brain* brain, SimulationStep currentStep) {
            self.update(brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"), "Update development (simple version)")
        .def("update", [](DevelopmentSystem& self, Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
            self.update(brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age");

    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis - formation of new synapses)pbdoc")
        .def(py::init<>())
        .def("getSynaptogenesisRate", &Synaptogenesis::getSynaptogenesisRate, "Get creation rate")
        .def("setSynaptogenesisRate", &Synaptogenesis::setSynaptogenesisRate, py::arg("rate"), "Set creation rate")
        .def("getPruningRate", &Synaptogenesis::getPruningRate, "Get pruning rate")
        .def("setPruningRate", &Synaptogenesis::setPruningRate, py::arg("rate"), "Set pruning rate")
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("rng"), "Update synaptogenesis");

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Pruning - removal of synapses and neurons)pbdoc")
        .def(py::init<>())
        .def("getPruningRate", &Pruning::getPruningRate, "Get pruning rate")
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"), "Set pruning rate")
        .def("update", &Pruning::update, py::arg("brain"), py::arg("rng"), "Update pruning");

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Maturation - developmental growth and specialization)pbdoc")
        .def(py::init<>())
        .def("getMaturationRate", &Maturation::getMaturationRate, "Get maturation rate")
        .def("setMaturationRate", &Maturation::setMaturationRate, py::arg("rate"), "Set maturation rate")
        .def("update", &Maturation::update, py::arg("brain"), py::arg("rng"), "Update maturation");

    // ===== Plasticity Systems =====
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for plasticity rules)pbdoc")
        .def(py::init<>())
        .def("isEnabled", &PlasticityRule::isEnabled, "Check if rule is enabled")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"), "Enable/disable rule")
        .def("getName", &PlasticityRule::getName, "Get rule name");

    py::class_<HebbianRule, PlasticityRule>(m, "HebbianRule", R"pbdoc(Hebbian plasticity rule - 'neurons that fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &HebbianRule::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &HebbianRule::getLearningRate, "Get learning rate")
        .def("getName", &HebbianRule::getName)
        .def("isEnabled", &HebbianRule::isEnabled)
        .def("setEnabled", &HebbianRule::setEnabled, py::arg("enabled"));

    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set LTP weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set LTD weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), "Set time constant")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get time constant")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"), "Update parameters from config")
        .def("getName", &STDP::getName)
        .def("isEnabled", &STDP::isEnabled)
        .def("setEnabled", &STDP::setEnabled, py::arg("enabled"));

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity - creation and removal of synapses and neurons)pbdoc")
        .def(py::init<>())
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, "Get creation rate")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"), "Set creation rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate, "Get pruning rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"), "Set pruning rate")
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"), "Update structural plasticity");

    // ===== Cognition Systems =====
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner - uses learned predictions to plan multi-step actions)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f, "Plan next action")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"), "Evaluate action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, "Get planning depth")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"), "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence, "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"), "Update plans based on outcome")
        .def("clearCache", &NeuralPlanner::clearCache, "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"), "Set action quality")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal, "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful, "Check if recent plan was successful");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation - discovers recurring patterns from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"), "Present new experience")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f, "Get matching concept")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get all concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"), "Get specific concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"), "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"), "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"), "Get concept stability")
        .def("getConceptCount", &ConceptFormation::getConceptCount, "Get concept count")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"), "Merge concepts")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"), "Update concept")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"), "Compute pattern similarity")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f, "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"), "Find concept for pattern")
        .def("clear", &ConceptFormation::clear, "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"), "Get generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("t"), "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("w"), "Set stability window");

    // ===== Performance Systems =====
    py::class_<Performance>(m, "Performance", R"pbdoc(Performance infrastructure - comprehensive optimization for large-scale neural simulation)pbdoc")
        .def(py::init<>())
        .def("getMemoryUsage", &Performance::getMemoryUsage, "Get memory usage statistics")
        .def("getActiveThreads", &Performance::getActiveThreads, "Get active thread count")
        .def("getOptimizationLevel", &Performance::getOptimizationLevel, "Get current optimization level")
        .def("setOptimizationLevel", &Performance::setOptimizationLevel, py::arg("level"), "Set optimization level")
        .def("getLastUpdateTime", &Performance::getLastUpdateTime, "Get last update time")
        .def("getAverageFrameTime", &Performance::getAverageFrameTime, "Get average frame time")
        .def("getFrameTimeMin", &Performance::getFrameTimeMin, "Get minimum frame time")
        .def("getFrameTimeMax", &Performance::getFrameTimeMax, "Get maximum frame time")
        .def("resetStatistics", &Performance::resetStatistics, "Reset performance statistics")
        .def("configure", [](Performance& self, bool useMemoryPool, bool useSOA, bool useEventQueue, bool useSparseConnectivity, bool useMultithreading, bool useSIMD) {
            // Configure performance features based on flags
            self.configure(useMemoryPool, useSOA, useEventQueue, useSparseConnectivity, useMultithreading, useSIMD);
        }, py::arg("useMemoryPool") = true, py::arg("useSOA") = true, py::arg("useEventQueue") = true,
           py::arg("useSparseConnectivity") = true, py::arg("useMultithreading") = true, py::arg("useSIMD") = true,
           "Configure performance features");

    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Memory pool allocators - pre-allocated neuron and synapse pools)pbdoc")
        .def(py::init<>())
        .def("allocateNeuron", &MemoryPool::allocateNeuron, "Allocate neuron from pool")
        .def("freeNeuron", &MemoryPool::freeNeuron, py::arg("neuronId"), "Free neuron back to pool")
        .def("allocateSynapse", &MemoryPool::allocateSynapse, "Allocate synapse from pool")
        .def("freeSynapse", &MemoryPool::freeSynapse, py::arg("synapseId"), "Free synapse back to pool")
        .def("getFreeNeuronCount", &MemoryPool::getFreeNeuronCount, "Get free neuron count")
        .def("getUsedNeuronCount", &MemoryPool::getUsedNeuronCount, "Get used neuron count")
        .def("getFreeSynapseCount", &MemoryPool::getFreeSynapseCount, "Get free synapse count")
        .def("getUsedSynapseCount", &MemoryPool::getUsedSynapseCount, "Get used synapse count")
        .def("getMemoryUsage", &MemoryPool::getMemoryUsage, "Get memory usage statistics")
        .def("reset", &MemoryPool::reset, "Reset memory pool");

    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Checkpoint system - save and restore brain state)pbdoc")
        .def(py::init<>())
        .def("saveState", &CheckpointSystem::saveState, py::arg("brain"), py::arg("filepath"), "Save brain state to file")
        .def("loadState", &CheckpointSystem::loadState, py::arg("brain"), py::arg("filepath"), "Load brain state from file")
        .def("clearState", &CheckpointSystem::clearState, "Clear current state")
        .def("getCheckpointCount", &CheckpointSystem::getCheckpointCount, "Get checkpoint count")
        .def("getMaxCheckpoints", &CheckpointSystem::getMaxCheckpoints, "Get maximum checkpoints")
        .def("setMaxCheckpoints", &CheckpointSystem::setMaxCheckpoints, py::arg("max"), "Set maximum checkpoints");

    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event queue - lock-free ring buffers for spike events)pbdoc")
        .def(py::init<>())
        .def("addEvent", &EventQueue::addEvent, py::arg("neuronId"), py::arg("eventTime"), py::arg("eventType"), "Add spike event")
        .def("processEvents", [](EventQueue& self, SimulationStep currentTime) {
            return self.processEvents(currentTime);
        }, py::arg("currentTime"), "Process events up to current time")
        .def("getEventCount", &EventQueue::getEventCount, "Get event count")
        .def("clear", &EventQueue::clear, "Clear all events")
        .def("getBufferUsage", &EventQueue::getBufferUsage, "Get buffer usage statistics");

    py::class_<OptimizedBrain>(m, "OptimizedBrain", R"pbdoc(Optimized brain - structure-of-arrays data layout for better performance)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("getNeuronCount", &OptimizedBrain::getNeuronCount, "Get neuron count")
        .def("getSynapseCount", &OptimizedBrain::getSynapseCount, "Get synapse count")
        .def("getRegionCount", &OptimizedBrain::getRegionCount, "Get region count")
        .def("getActiveNeuronCount", &OptimizedBrain::getActiveNeuronCount, "Get active neuron count")
        .def("getSpikeRate", &OptimizedBrain::getSpikeRate, "Get spike rate")
        .def("getEfficiency", &OptimizedBrain::getEfficiency, "Get efficiency metrics")
        .def("configure", &OptimizedBrain::configure, py::arg("useSOA") = true, py::arg("useSparseConnectivity") = true, "Configure optimization settings")
        .def("isSOAMode", &OptimizedBrain::isSOAMode, "Check if in SOA mode")
        .def("isSparseConnectivityMode", &OptimizedBrain::isSparseConnectivityMode, "Check if in sparse connectivity mode");

    py::class_<ParallelProcessing>(m, "ParallelProcessing", R"pbdoc(Parallel processing - multithreading support for large networks)pbdoc")
        .def(py::init<>())
        .def("getThreadCount", &ParallelProcessing::getThreadCount, "Get thread count")
        .def("setThreadCount", &ParallelProcessing::setThreadCount, py::arg("count"), "Set thread count")
        .def("isThreadSafe", &ParallelProcessing::isThreadSafe, "Check if thread-safe")
        .def("getLoadBalance", &ParallelProcessing::getLoadBalance, "Get load balance metrics")
        .def("getPerformanceMetrics", &ParallelProcessing::getPerformanceMetrics, "Get performance metrics")
        .def("update", &ParallelProcessing::update, py::arg("currentStep"), "Update parallel processing");

    py::class_<SIMDVectorization>(m, "SIMDVectorization", R"pbdoc(SIMD vectorization - efficient SIMD operations)pbdoc")
        .def(py::init<>())
        .def("getSIMDWidth", &SIMDVectorization::getSIMDWidth, "Get SIMD width")
        .def("getSIMDInstructionSet", &SIMDVectorization::getSIMDInstructionSet, "Get SIMD instruction set")
        .def("getSIMDEfficiency", &SIMDVectorization::getSIMDEfficiency, "Get SIMD efficiency")
        .def("processBatch", &SIMDVectorization::processBatch, "Process neuron batch with SIMD")
        .def("isSIMDEnabled", &SIMDVectorization::isSIMDEnabled, "Check if SIMD is enabled")
        .def("getVectorizationLevel", &SIMDVectorization::getVectorizationLevel, "Get vectorization level");

    py::class_<SparseConnectivity>(m, "SparseConnectivity", R"pbdoc(Sparse connectivity - adjacency lists instead of dense matrices)pbdoc")
        .def(py::init<>())
        .def("addConnection", &SparseConnectivity::addConnection, py::arg("source"), py::arg("destination"), "Add connection")
        .def("removeConnection", &SparseConnectivity::removeConnection, py::arg("source"), py::arg("destination"), "Remove connection")
        .def("hasConnection", &SparseConnectivity::hasConnection, py::arg("source"), py::arg("destination"), "Check if connection exists")
        .def("getConnections", &SparseConnectivity::getConnections, py::arg("source"), "Get connections from source")
        .def("getConnectionCount", &SparseConnectivity::getConnectionCount, "Get total connection count")
        .def("getDensity", &SparseConnectivity::getDensity, "Get connectivity density")
        .def("getMemoryUsage", &SparseConnectivity::getMemoryUsage, "Get memory usage statistics")
        .def("clear", &SparseConnectivity::clear, "Clear all connections");

    // ===== Agent Interface =====
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity enabled");

    // ===== Factory Functions =====
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

    // ===== Convenience Functions =====
    m.def("createNeuralSimulation", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(config, brain, world, agentBrain);
    }, "Create a complete neural simulation with default settings");

    // ===== Invalid Identifiers =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===== Pythonic Interface Enhancements =====

    // Add property-like access for commonly used attributes
    py::class_<Config>(m, "Config", py::dynamic_attr(), R"pbdoc(Configuration class with Pythonic properties)pbdoc")
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance(value, py::int_())) {
                self.setInt(key, value.cast<int>());
            } else if (py::isinstance(value, py::float_())) {
                self.setFloat(key, value.cast<float>());
            } else if (py::isinstance(value, py::str_())) {
                self.setString(key, value.cast<std::string>());
            } else if (py::isinstance(value, py::bool_())) {
                self.setBool(key, value.cast<bool>());
            } else {
                throw std::runtime_error("Unsupported value type for Config[" + key + "]");
            }
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            py::object result;
            if (self.hasInt(key)) {
                result = py::cast(self.getInt(key));
            } else if (self.hasFloat(key)) {
                result = py::cast(self.getFloat(key));
            } else if (self.hasString(key)) {
                result = py::cast(self.getString(key));
            } else if (self.hasBool(key)) {
                result = py::cast(self.getBool(key));
            } else {
                throw py::key_error("Key '" + key + "' not found in Config");
            }
            return result;
        })
        .def("__delitem__", &Config::remove)
        .def("__contains__", &Config::has)
        .def("__len__", &Config::getKeys)
        .def("__iter__", [](const Config& self) {
            return py::make_key_iterator(self.getKeys());
        })
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Context manager for cleanup operations
    py::class_<SimpleWorld>(m, "SimpleWorldContext", R"pbdoc(SimpleWorld with context manager support)pbdoc")
        .def(py::init<>())
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        })
        .def("__exit__", [](SimpleWorld& self, py::args) {
            self.reset();
        });

    // Property-like access for Brain class
    py::class_<Brain>(m, "Brain", py::dynamic_attr(), R"pbdoc(Brain class with Pythonic properties)pbdoc")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses>";
        })
        .def("__getitem__", [](Brain& self, const std::string& key) {
            if (key == "neuronCount") {
                return py::cast(self.getTotalNeuronCount());
            } else if (key == "synapseCount") {
                return py::cast(self.getTotalSynapseCount());
            } else if (key == "activeNeuronCount") {
                return py::cast(self.getActiveNeuronCount());
            } else if (key == "firingNeuronCount") {
                return py::cast(self.getFiringNeuronCount());
            } else if (key == "averageFiringRate") {
                return py::cast(self.getAverageFiringRate());
            } else if (key == "excitationInhibitionRatio") {
                return py::cast(self.getExcitationInhibitionRatio());
            } else {
                throw py::key_error("Key '" + key + "' not supported for Brain");
            }
        })
        .def("__setitem__", [](Brain& self, const std::string& key, float value) {
            if (key == "developmentalStage") {
                self.setDevelopmentalStage(static_cast<DevelopmentalStage>(static_cast<int>(value)));
            } else {
                throw py::key_error("Key '" + key + "' is read-only for Brain");
            }
        });

    // Convenience method collections
    m.def("neuromodulation", []() {
        return std::map<std::string, std::function<void(float)>>();
    }, "Get neuromodulation convenience function map");

    m.def("memory_system", []() {
        return std::map<std::string, std::function<void(const std::vector<float>&)>>();
    }, "Get memory system convenience function map");

    m.def("plasticity_rule", []() {
        return std::map<std::string, std::function<void(float)>>();
    }, "Get plasticity rule convenience function map");

    // Examples and utilities
    m.def("example_basic_simulation", []() {
        auto config = std::make_shared<Config>();
        config->setInt("neuronCount", 100);
        config->setFloat("connectionProbability", 0.1f);

        auto brain = std::make_shared<Brain>(config);
        brain->initialize();

        auto world = std::make_shared<SimpleWorld>();
        world->configure(800, 600, 80, 60);

        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);

        return std::make_tuple(config, brain, world, agent);
    }, "Create a basic neural simulation example");

    m.def("example_advanced_simulation", []() {
        auto config = std::make_shared<Config>();
        config->setInt("neuronCount", 1000);
        config->setFloat("connectionProbability", 0.05f);
        config->setBool("enableRewardModulation", true);
        config->setBool("enableCuriosity", true);
        config->setBool("enableDevelopment", true);
        config->setBool("enableStructuralPlasticity", true);

        auto brain = std::make_shared<Brain>(config);
        brain->initialize();

        auto world = std::make_shared<SimpleWorld>();
        world->configure(1024, 768, 128, 96);

        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);

        // Add neuromodulators
        auto dopamine = std::make_shared<Dopamine>();
        auto acetylcholine = std::make_shared<Acetylcholine>();

        // Add memory systems
        auto workingMemory = std::make_shared<NeuralWorkingMemory>();
        auto episodicMemory = std::make_shared<NeuralEpisodicMemory>();

        // Add development system
        auto development = std::make_shared<DevelopmentSystem>();

        // Add plasticity rules
        auto stdp = std::make_shared<STDP>();
        auto hebbian = std::make_shared<HebbianRule>();

        // Add cognition systems
        auto planner = std::make_shared<NeuralPlanner>();
        auto conceptForm = std::make_shared<ConceptFormation>();

        // Add performance optimizations
        auto memoryPool = std::make_shared<MemoryPool>();
        auto checkpoint = std::make_shared<CheckpointSystem>();

        return std::make_tuple(config, brain, world, agent,
                             dopamine, acetylcholine,
                             workingMemory, episodicMemory,
                             development, stdp, hebbian,
                             planner, conceptForm,
                             memoryPool, checkpoint);
    }, "Create an advanced neural simulation with all systems enabled");

    // Version information
    m.attr("version") = "2.0.0-enhanced";
    m.attr("description") = "Enhanced NLM Python bindings with complete system integration";

} // namespace nlm

#undef NLM_PERFORMANCE_HPP
#undef NLM_SIMD_AVX2
#undef NLM_THREAD_COUNT
#undef NLM_MEMORY_POOL_PAGE_SIZE
#undef NLM_MAX_POOL_SIZE