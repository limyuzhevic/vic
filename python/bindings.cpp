#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Neuromodulation
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

// Memory systems
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

// Cognition systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

// Performance systems
#include "../src/performance/OptimizedBrain.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/MemoryPool.hpp"

// Development system
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive bindings for neuromodulation, memory,
        cognition, performance, and development systems.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ID classes
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

    // Enumerations
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

    // Neuromodulators
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

    // Core classes
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

    // Neuromodulation
    py::enum_<Neuromodulator::Type>(m, "NeuromodulatorType", R"pbdoc(Neuromodulator type enumeration)pbdoc")
        .value("Dopamine", Neuromodulator::Type::Dopamine)
        .value("Acetylcholine", Neuromodulator::Type::Acetylcholine)
        .value("Norepinephrine", Neuromodulator::Type::Norepinephrine)
        .value("Serotonin", Neuromodulator::Type::Serotonin);

    // Specific neuromodulators
    py::class_<Acetylcholine, nlm::Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator - attention and memory consolidation)pbdoc")
        .def(py::init<>())
        .def("getName", &nlm::Acetylcholine::getName)
        .def("getLevel", &nlm::Acetylcholine::getLevel)
        .def("setLevel", &nlm::Acetylcholine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &nlm::Acetylcholine::getPlasticityFactor)
        .def("update", &nlm::Acetylcholine::update, py::arg("dt"));

    py::class_<Norepinephrine, nlm::Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator - arousal and vigilance)pbdoc")
        .def(py::init<>())
        .def("getName", &nlm::Norepinephrine::getName)
        .def("getLevel", &nlm::Norepinephrine::getLevel)
        .def("setLevel", &nlm::Norepinephrine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &nlm::Norepinephrine::getPlasticityFactor)
        .def("update", &nlm::Norepinephrine::update, py::arg("dt"));

    py::class_<Serotonin, nlm::Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator - mood, impulsivity, and social behavior)pbdoc")
        .def(py::init<>())
        .def("getName", &nlm::Serotonin::getName)
        .def("getLevel", &nlm::Serotonin::getLevel)
        .def("setLevel", &nlm::Serotonin::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &nlm::Serotonin::getPlasticityFactor)
        .def("update", &nlm::Serotonin::update, py::arg("dt"));

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator - exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::Curiosity::initialize, py::arg("brain"))
        .def("getLevel", &nlm::Curiosity::getLevel)
        .def("getExplorationDrive", &nlm::Curiosity::getExplorationDrive)
        .def("update", &nlm::Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"))
        .def("setNoveltyWeight", &nlm::Curiosity::setNoveltyWeight, py::arg("weight"))
        .def("setPredictionErrorWeight", &nlm::Curiosity::setPredictionErrorWeight, py::arg("weight"))
        .def("reset", &nlm::Curiosity::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection signal - computes novelty from comparison with previous observations)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::Novelty::initialize, py::arg("brain"))
        .def("getLevel", &nlm::Novelty::getLevel)
        .def("setLevel", &nlm::Novelty::setLevel, py::arg("level"))
        .def("detectNovelty", [](nlm::Novelty& self, const std::vector<float>& current, const std::vector<float>& previous) {
            self.detectNovelty(current, previous);
        }, py::arg("currentPattern"), py::arg("previousPattern"))
        .def("detectNovelty", [](nlm::Novelty& self, const class nlm::Observation& current, const class nlm::Observation& previous) {
            self.detectNovelty(current, previous);
        })
        .def("update", &nlm::Novelty::update, py::arg("dt"))
        .def("getHistory", &nlm::Novelty::getHistory)
        .def("clearHistory", &nlm::Novelty::clearHistory)
        .def("reset", &nlm::Novelty::reset);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal - computes difference between predicted and actual values)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::PredictionError::initialize, py::arg("brain"))
        .def("getError", &nlm::PredictionError::getError)
        .def("computeError", &nlm::PredictionError::computeError, py::arg("predicted"), py::arg("actual"))
        .def("updatePrediction", &nlm::PredictionError::updatePrediction, py::arg("newPrediction"))
        .def("getHistory", &nlm::PredictionError::getHistory)
        .def("clearHistory", &nlm::PredictionError::clearHistory)
        .def("getMagnitude", &nlm::PredictionError::getMagnitude);

    py::class_<Dopamine, nlm::Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator - reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("getName", &nlm::Dopamine::getName)
        .def("getLevel", &nlm::Dopamine::getLevel)
        .def("setLevel", &nlm::Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &nlm::Dopamine::getPlasticityFactor)
        .def("update", &nlm::Dopamine::update, py::arg("dt"))
        .def("signalReward", &nlm::Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &nlm::Dopamine::signalRewardPredictionError, py::arg("error"))
        .def("getBaseline", &nlm::Dopamine::getLevel)  // Placeholder method
        .def("getPeak", [](const nlm::Dopamine& self) { return 1.0f; })  // Placeholder method
        .def("getDecayRate", [](const nlm::Dopamine& self) { return 0.1f; });  // Placeholder method

    // TripletSTDP plasticity rule implementation
    py::class_<TripletSTDP, nlm::PlasticityRule>(m, "TripletSTDP", R"pbdoc(Spike-timing dependent plasticity with triplet interactions)pbdoc")
        .def(py::init<>())
        .def("update", &nlm::TripletSTDP::update)
        .def("applyWeightChange", &nlm::TripletSTDP::applyWeightChange)
        .def("getName", &nlm::TripletSTDP::getName)
        .def("setUPlus", &nlm::TripletSTDP::setUPlus, py::arg("uPlus"))
        .def("getUPlus", &nlm::TripletSTDP::getUPlus)
        .def("setUMinus", &nlm::TripletSTDP::setUMinus, py::arg("uMinus"))
        .def("getUMinus", &nlm::TripletSTDP::getUMinus)
        .def("setTauPlus", &nlm::TripletSTDP::setTauPlus, py::arg("tauPlus"))
        .def("getTauPlus", &nlm::TripletSTDP::getTauPlus);

    // RewardModulatedSTDP plasticity rule implementation
    py::class_<RewardModulatedSTDP, nlm::PlasticityRule>(m, "RewardModulatedSTDP", R"pbdoc(Reward-modulated spike-timing dependent plasticity)pbdoc")
        .def(py::init<>())
        .def("update", &nlm::RewardModulatedSTDP::update)
        .def("applyWeightChange", &nlm::RewardModulatedSTDP::applyWeightChange)
        .def("getName", &nlm::RewardModulatedSTDP::getName)
        .def("setLearningRate", &nlm::RewardModulatedSTDP::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &nlm::RewardModulatedSTDP::getLearningRate)
        .def("setTemperature", &nlm::RewardModulatedSTDP::setTemperature, py::arg("temp"))
        .def("getTemperature", &nlm::RewardModulatedSTDP::getTemperature);

    // Memory system implementations
    py::class_<nlm::SemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory - gradually acquired knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &nlm::SemanticMemory::storeFact, py::arg("key"), py::arg("value"))
        .def("retrieveFact", &nlm::SemanticMemory::retrieveFact, py::arg("key"))
        .def("hasFact", &nlm::SemanticMemory::hasFact, py::arg("key"))
        .def("getAllFacts", &nlm::SemanticMemory::getAllFacts)
        .def("clear", &nlm::SemanticMemory::clear);

    py::class_<nlm::ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory - learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &nlm::ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"))
        .def("getSkill", &nlm::ProceduralMemory::getSkill, py::arg("name"))
        .def("getAllSkills", &nlm::ProceduralMemory::getAllSkills)
        .def("updateProficiency", &nlm::ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"))
        .def("clear", &nlm::ProceduralMemory::clear);

    py::class_<nlm::AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory - relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", &nlm::AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"))
        .def("getAssociations", &nlm::AssociativeMemory::getAssociations, py::arg("neuron"))
        .def("getAssociationStrength", &nlm::AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"))
        .def("updateAssociation", &nlm::AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"))
        .def("clear", &nlm::AssociativeMemory::clear);

    // HebbianRule implementation (already exists but may need updates)
    py::class_<HebbianRule, nlm::PlasticityRule>(m, "HebbianRule", R"pbdoc(Hebbian plasticity rule - 'fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("update", &HebbianRule::update)
        .def("applyWeightChange", &HebbianRule::applyWeightChange)
        .def("getName", &HebbianRule::getName)
        .def("setLearningRate", &HebbianRule::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &HebbianRule::getLearningRate)
        .def("setMaxWeight", [](HebbianRule& self, float max) {}, py::arg("maxWeight"))  // Placeholder
        .def("getMaxWeight", []() { return 1.0f; });  // Placeholder

    // Additional plasticity rules
    py::class_<AntiHebbianRule, nlm::PlasticityRule>(m, "AntiHebbianRule", R"pbdoc(Anti-Hebbian rule - 'fire together, weaken together')pbdoc")
        .def(py::init<>())
        .def("update", &AntiHebbianRule::update)
        .def("applyWeightChange", &AntiHebbianRule::applyWeightChange)
        .def("getName", &AntiHebbianRule::getName);

    py::class_<BCMRule, nlm::PlasticityRule>(m, "BCMRule", R"pbdoc(Bienenstock-Cooper-Munro rule - sliding threshold learning)pbdoc")
        .def(py::init<>())
        .def("update", &BCMRule::update)
        .def("applyWeightChange", &BCMRule::applyWeightChange)
        .def("getName", &BCMRule::getName);

    // Constants and utilities
    m.attr("DAILY_DOPAMINE_BURST_SIZE") = py::cast(0.5f);
    m.attr("DEFAULT_ACETYLCHOLINE_LEVEL") = py::cast(0.5f);
    m.attr("DEFAULT_NOREPINEPHRINE_LEVEL") = py::cast(0.3f);
    m.attr("DEFAULT_SEROTONIN_LEVEL") = py::cast(0.3f);
    m.attr("MAX_PLASTICITY_FACTOR") = py::cast(2.0f);
    m.attr("MIN_PLASTICITY_FACTOR") = py::cast(0.1f);
    m.attr("DEFAULT_LEARNING_RATE") = py::cast(0.01f);
    m.attr("DEFAULT_NOVELTY_THRESHOLD") = py::cast(0.5f);

    // Factory functions for creating complex components
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuromodulation.enabled", true);
        config->set("memory.capacity.working", 1000);
        config->set("memory.capacity.episodic", 10000);
        config->set("plasticity.rule.stdp.enable", true);
        config->set("plasticity.rule.hebbian.enable", true);
        return config;
    }, "Create an advanced configuration with optimized settings");

    m.def("createCompleteBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        // Initialize with default neuromodulators
        brain->setDopamineLevel(0.5f);
        brain->setAcetylcholineLevel(0.3f);
        brain->setNorepinephrineLevel(0.3f);
        brain->setSerotoninLevel(0.3f);
        return brain;
    }, py::arg("config"), "Create a brain with default neuromodulator settings");

    m.def("createFullAgent", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize();
        return agent;
    }, py::arg("brain"), "Create an agent brain interface with automatic initialization");

    m.def("createWorldWithDefaults", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 20, 20);
        world->setAgentStart(50.0f, 50.0f);
        return world;
    }, "Create a world with default settings");

    m.def("createNeuromodulatorSystem", []() {
        py::dict system;
        system["dopamine"] = std::make_shared<Dopamine>();
        system["acetylcholine"] = std::make_shared<Acetylcholine>();
        system["norepinephrine"] = std::make_shared<Norepinephrine>();
        system["serotonin"] = std::make_shared<Serotonin>();
        return system;
    }, "Create a complete neuromodulator system with all four modulators");

    m.def("createMemorySystem", []() {
        py::dict memory;
        memory["working"] = std::make_shared<nlm::NeuralWorkingMemory>();
        memory["episodic"] = std::make_shared<nlm::NeuralEpisodicMemory>();
        memory["semantic"] = std::make_shared<nlm::SemanticMemory>();
        memory["procedural"] = std::make_shared<nlm::ProceduralMemory>();
        memory["associative"] = std::make_shared<nlm::AssociativeMemory>();
        return memory;
    }, "Create a complete memory system with all memory types");

    m.def("createPlasticitySystem", []() {
        py::dict plasticity;
        plasticity["stdp"] = std::make_shared<STDP>();
        plasticity["hebbian"] = std::make_shared<HebbianRule>();
        plasticity["reward_modulated"] = std::make_shared<RewardModulatedSTDP>();
        plasticity["triplet"] = std::make_shared<TripletSTDP>();
        plasticity["anti_hebbian"] = std::make_shared<AntiHebbianRule>();
        plasticity["bcm"] = std::make_shared<BCMRule>();
        return plasticity;
    }, "Create a complete plasticity system with all plasticity rules");

    m.def("createDevelopmentPackage", []() {
        py::dict development;
        development["system"] = std::make_shared<nlm::DevelopmentSystem>();
        development["synaptogenesis"] = std::make_shared<nlm::Synaptogenesis>();
        development["pruning"] = std::make_shared<nlm::Pruning>();
        development["maturation"] = std::make_shared<nlm::Maturation>();
        return development;
    }, "Create a complete development system package");

    m.def("createOptimizationPackage", []() {
        py::dict optimization;
        optimization["event_queue"] = std::make_shared<nlm::EventQueue>();
        optimization["neuron_pool"] = std::make_shared<nlm::MemoryPool<Neuron>>();
        optimization["synapse_pool"] = std::make_shared<nlm::MemoryPool<Synapse>>();
        optimization["optimized_brain"] = std::make_shared<nlm::OptimizedBrain>();
        return optimization;
    }, "Create an optimization package with event queue and memory pools");

    m.def("createNeuromodulationBundle", []() {
        py::dict bundle;
        bundle["dopamine"] = std::make_shared<Dopamine>();
        bundle["acetylcholine"] = std::make_shared<Acetylcholine>();
        bundle["norepinephrine"] = std::make_shared<Norepinephrine>();
        bundle["serotonin"] = std::make_shared<Serotonin>();
        bundle["curiosity"] = std::make_shared<Curiosity>();
        bundle["novelty"] = std::make_shared<Novelty>();
        bundle["prediction_error"] = std::make_shared<PredictionError>();
        bundle["reward"] = std::make_shared<Reward>();
        return bundle;
    }, "Create a complete neuromodulation bundle with all neuromodulators");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", &nlm::Reward::getValue)
        .def("setValue", &nlm::Reward::setValue, py::arg("value"))
        .def("add", &nlm::Reward::add, py::arg("delta"))
        .def("reset", &nlm::Reward::reset)
        .def("computeReward", &nlm::Reward::computeReward, py::arg("observation"))
        .def("getHistory", &nlm::Reward::getHistory)
        .def("clearHistory", &nlm::Reward::clearHistory);

    // Memory systems
    py::class_<nlm::NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory - transient active information storage using neural dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", &nlm::NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &nlm::NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &nlm::NeuralWorkingMemory::retrieve)
        .def("contains", &nlm::NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &nlm::NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &nlm::NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &nlm::NeuralWorkingMemory::clear)
        .def("getActiveTraces", &nlm::NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &nlm::NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &nlm::NeuralWorkingMemory::setCapacity, py::arg("cap"))
        .def("getDecayRate", &nlm::NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &nlm::NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &nlm::NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &nlm::NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("runCompetition", &nlm::NeuralWorkingMemory::runCompetition)
        .def("isWinning", &nlm::NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &nlm::NeuralWorkingMemory::getMemoryActivity)
        .def("reset", &nlm::NeuralWorkingMemory::reset);

    py::class_<nlm::NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory - long-term storage of experiences and events)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEvent", &nlm::NeuralEpisodicMemory::storeEvent, py::arg("pattern"), py::arg("features"), py::arg("reward"))
        .def("recallEvent", &nlm::NeuralEpisodicMemory::recallEvent, py::arg("pattern"))
        .def("getEventCount", &nlm::NeuralEpisodicMemory::getEventCount)
        .def("update", &nlm::NeuralEpisodicMemory::update, py::arg("dt"))
        .def("clear", &nlm::NeuralEpisodicMemory::clear)
        .def("reset", &nlm::NeuralEpisodicMemory::reset);

    // Cognition systems
    py::class_<nlm::NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner - action planning using predictive neural dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &nlm::NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f)
        .def("evaluateSequence", &nlm::NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"))
        .def("getPlanningDepth", &nlm::NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &nlm::NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getPlanningConfidence", &nlm::NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &nlm::NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"))
        .def("clearCache", &nlm::NeuralPlanner::clearCache)
        .def("setActionQuality", &nlm::NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"))
        .def("getCurrentGoal", &nlm::NeuralPlanner::getCurrentGoal)
        .def("setCurrentGoal", &nlm::NeuralPlanner::setCurrentGoal, py::arg("goal"))
        .def("wasRecentPlanSuccessful", &nlm::NeuralPlanner::wasRecentPlanSuccessful)
        .def("reset", &nlm::NeuralPlanner::reset);

    py::class_<nlm::ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation - discovers recurring patterns from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::ConceptFormation::initialize, py::arg("brain"))
        .def("presentExperience", &nlm::ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"))
        .def("getMatchingConcept", &nlm::ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("getConcepts", &nlm::ConceptFormation::getConcepts)
        .def("getConcept", &nlm::ConceptFormation::getConcept, py::arg("conceptId"))
        .def("getConceptPrototype", &nlm::ConceptFormation::getConceptPrototype, py::arg("conceptId"))
        .def("getConceptInstances", &nlm::ConceptFormation::getConceptInstances, py::arg("conceptId"))
        .def("getConceptStability", &nlm::ConceptFormation::getConceptStability, py::arg("conceptId"))
        .def("getConceptCount", &nlm::ConceptFormation::getConceptCount)
        .def("mergeConcepts", &nlm::ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"))
        .def("updateConcept", &nlm::ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"))
        .def("computeSimilarity", &nlm::ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"))
        .def("isNovel", &nlm::ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f)
        .def("findConceptForPattern", &nlm::ConceptFormation::findConceptForPattern, py::arg("pattern"))
        .def("clear", &nlm::ConceptFormation::clear)
        .def("getGeneralizationAbility", &nlm::ConceptFormation::getGeneralizationAbility, py::arg("conceptId"))
        .def("setFormationThreshold", &nlm::ConceptFormation::setFormationThreshold, py::arg("t"))
        .def("setStabilityWindow", &nlm::ConceptFormation::setStabilityWindow, py::arg("w"))
        .def("reset", &nlm::ConceptFormation::reset);

    py::class_<nlm::AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection - neural attention through competition and salience)pbdoc")
        .def(py::init<>())
        .def("initialize", &nlm::AttentionalSelection::initialize, py::arg("brain"))
        .def("processCompetition", &nlm::AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f)
        .def("focusOnRegion", &nlm::AttentionalSelection::focusOnRegion, py::arg("region"))
        .def("releaseAttention", &nlm::AttentionalSelection::releaseAttention)
        .def("getAttendedRegions", &nlm::AttentionalSelection::getAttendedRegions)
        .def("setInhibitionStrength", &nlm::AttentionalSelection::setInhibitionStrength, py::arg("strength"))
        .def("setExcitationStrength", &nlm::AttentionalSelection::setExcitationStrength, py::arg("strength"))
        .def("setCompetitionThreshold", &nlm::AttentionalSelection::setCompetitionThreshold, py::arg("threshold"))
        .def("getInhibitionFor", &nlm::AttentionalSelection::getInhibitionFor, py::arg("neuron"))
        .def("getExcitationFor", &nlm::AttentionalSelection::getExcitationFor, py::arg("neuron"))
        .def("update", &nlm::AttentionalSelection::update, py::arg("dt"))
        .def("isAttended", &nlm::AttentionalSelection::isAttended, py::arg("neuron"))
        .def("getWinners", &nlm::AttentionalSelection::getWinners)
        .def("applyTopDownBias", &nlm::AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"))
        .def("applyBottomUpSalience", &nlm::AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"))
        .def("reset", &nlm::AttentionalSelection::reset);

    // Performance systems
    py::class_<nlm::EventQueue>(m, "EventQueue", R"pbdoc(Event queue for spike and event processing)pbdoc")
        .def(py::init<>())
        .def("pushSpike", &nlm::EventQueue::pushSpike, py::arg("event"))
        .def("popSpike", &nlm::EventQueue::popSpike)
        .def("pushEvent", &nlm::EventQueue::pushEvent, py::arg("event"))
        .def("popEvent", &nlm::EventQueue::popEvent)
        .def("empty", &nlm::EventQueue::empty)
        .def("full", &nlm::EventQueue::full)
        .def("size", &nlm::EventQueue::size)
        .def("capacity", &nlm::EventQueue::capacity)
        .def("clear", &nlm::EventQueue::clear);

    py::class_<nlm::MemoryPool<Neuron>>(m, "NeuronMemoryPool", R"pbdoc(Memory pool for neurons)pbdoc")
        .def(py::init<>()) // Default constructor
        .def("allocate", &nlm::MemoryPool<Neuron>::allocate)
        .def("deallocate", &nlm::MemoryPool<Neuron>::deallocate)
        .def("empty", &nlm::MemoryPool<Neuron>::empty)
        .def("allocatedCount", &nlm::MemoryPool<Neuron>::allocatedCount)
        .def("availableCount", &nlm::MemoryPool<Neuron>::availableCount)
        .def("memoryUsage", &nlm::MemoryPool<Neuron>::memoryUsage)
        .def("reserve", &nlm::MemoryPool<Neuron>::reserve);

    py::class_<nlm::MemoryPool<Synapse>>(m, "SynapseMemoryPool", R"pbdoc(Memory pool for synapses)pbdoc")
        .def(py::init<>()) // Default constructor
        .def("allocate", &nlm::MemoryPool<Synapse>::allocate)
        .def("deallocate", &nlm::MemoryPool<Synapse>::deallocate)
        .def("empty", &nlm::MemoryPool<Synapse>::empty)
        .def("allocatedCount", &nlm::MemoryPool<Synapse>::allocatedCount)
        .def("availableCount", &nlm::MemoryPool<Synapse>::availableCount)
        .def("memoryUsage", &nlm::MemoryPool<Synapse>::memoryUsage)
        .def("reserve", &nlm::MemoryPool<Synapse>::reserve);

    // Development system
    py::class_<nlm::DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system - orchestrates neural development)pbdoc")
        .def(py::init<>())
        .def("getStage", &nlm::DevelopmentSystem::getStage)
        .def("setStage", &nlm::DevelopmentSystem::setStage, py::arg("stage"))
        .def("advanceStage", &nlm::DevelopmentSystem::advanceStage)
        .def("getStageName", &nlm::DevelopmentSystem::getStageName)
        .def("update", &nlm::DevelopmentSystem::update, py::arg("brain"), py::arg("currentStep"))
        .def("update", &nlm::DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("getPlasticityModifier", &nlm::DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &nlm::DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &nlm::DevelopmentSystem::getCriticalPeriodProgress)
        .def("getDevelopmentalAge", &nlm::DevelopmentSystem::getDevelopmentalAge)
        .def("setDevelopmentalAge", &nlm::DevelopmentSystem::setDevelopmentalAge, py::arg("age"));

    py::class_<nlm::Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis - formation of new synapses)pbdoc")
        .def(py::init<>())
        .def("getFormationRate", &nlm::Synaptogenesis::getFormationRate)
        .def("setFormationRate", &nlm::Synaptogenesis::setFormationRate, py::arg("rate"))
        .def("getTargetDensity", &nlm::Synaptogenesis::getTargetDensity)
        .def("setTargetDensity", &nlm::Synaptogenesis::setTargetDensity, py::arg("density"))
        .def("update", &nlm::Synaptogenesis::update, py::arg("brain"), py::arg("rng"));

    py::class_<nlm::Pruning>(m, "Pruning", R"pbdoc(Synapse pruning - removal of weak or unused synapses)pbdoc")
        .def(py::init<>())
        .def("getThreshold", &nlm::Pruning::getThreshold)
        .def("setThreshold", &nlm::Pruning::setThreshold, py::arg("threshold"))
        .def("getPruningRate", &nlm::Pruning::getPruningRate)
        .def("setPruningRate", &nlm::Pruning::setPruningRate, py::arg("rate"))
        .def("update", &nlm::Pruning::update, py::arg("brain"), py::arg("rng"))
        .def("pruneSynapse", &nlm::Pruning::pruneSynapse, py::arg("synapse"));

    py::class_<nlm::Maturation>(m, "Maturation", R"pbdoc(Maturation - neural maturation to adult properties)pbdoc")
        .def(py::init<>())
        .def("getProgress", &nlm::Maturation::getProgress)
        .def("setProgress", &nlm::Maturation::setProgress, py::arg("progress"))
        .def("update", &nlm::Maturation::update, py::arg("brain"), py::arg("currentStep"))
        .def("getMatureThreshold", &nlm::Maturation::getMatureThreshold)
        .def("getMatureRestingPotential", &nlm::Maturation::getMatureRestingPotential)
        .def("getMatureTimeConstant", &nlm::Maturation::getMatureTimeConstant);

    // Performance systems - Optimized Brain
    py::class_<nlm::OptimizedBrain>(m, "OptimizedBrain", R"pbdoc(Optimized brain - high-performance neural simulation with event-driven dynamics)pbdoc")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::OptimizedBrain::initialize)
        .def("step", static_cast<void (nlm::OptimizedBrain::*)(nlm::SimulationStep)>(&nlm::OptimizedBrain::step), py::arg("currentStep"))
        .def("step", static_cast<void (nlm::OptimizedBrain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::OptimizedBrain::step), py::arg("currentStep"), py::arg("currentTime"))
        .def("receiveSensoryInput", &nlm::OptimizedBrain::receiveSensoryInput, py::arg("input"))
        .def("injectCurrent", &nlm::OptimizedBrain::injectCurrent, py::arg("neuron"), py::arg("current"))
        .def("injectCurrentToNeurons", &nlm::OptimizedBrain::injectCurrentToNeurons, py::arg("type"), py::arg("current"))
        .def("produceAction", &nlm::OptimizedBrain::produceAction)
        .def("reset", &nlm::OptimizedBrain::reset)
        .def("save", &nlm::OptimizedBrain::save, py::arg("filepath"))
        .def("load", &nlm::OptimizedBrain::load, py::arg("filepath"))
        .def("addRegion", &nlm::OptimizedBrain::addRegion, py::arg("name") = "")
        .def("getRegion", &nlm::OptimizedBrain::getRegion, py::arg("id"))
        .def("getRegionCount", &nlm::OptimizedBrain::getRegionCount)
        .def("getRegionIds", &nlm::OptimizedBrain::getRegionIds)
        .def("getRegions", &nlm::OptimizedBrain::getRegions)
        .def("getTotalNeuronCount", &nlm::OptimizedBrain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &nlm::OptimizedBrain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &nlm::OptimizedBrain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &nlm::OptimizedBrain::getFiringNeuronCount)
        .def("getAverageFiringRate", &nlm::OptimizedBrain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &nlm::OptimizedBrain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &nlm::OptimizedBrain::getTotalSpikeCount)
        .def("getDevelopmentalStage", &nlm::OptimizedBrain::getDevelopmentalStage)
        .def("setDevelopmentalStage", &nlm::OptimizedBrain::setDevelopmentalStage, py::arg("stage"))
        .def("getConfig", &nlm::OptimizedBrain::getConfig)
        .def("logStatus", &nlm::OptimizedBrain::logStatus)
        .def("getWorkingMemory", &nlm::OptimizedBrain::getWorkingMemory)
        .def("getEpisodicMemory", &nlm::OptimizedBrain::getEpisodicMemory)
        .def("getAssociativeMemory", &nlm::OptimizedBrain::getAssociativeMemory)
        .def("getPredictionSystem", &nlm::OptimizedBrain::getPredictionSystem)
        .def("getPlanner", &nlm::OptimizedBrain::getPlanner)
        .def("getConceptFormation", &nlm::OptimizedBrain::getConceptFormation)
        .def("getAttention", &nlm::OptimizedBrain::getAttention)
        .def("getDevelopmentSystem", &nlm::OptimizedBrain::getDevelopmentSystem)
        .def("getDopamine", &nlm::OptimizedBrain::getDopamine)
        .def("getCuriosity", &nlm::OptimizedBrain::getCuriosity)
        .def("getNovelty", &nlm::OptimizedBrain::getNovelty)
        .def("getPredictionErrorSignal", &nlm::OptimizedBrain::getPredictionErrorSignal)
        .def("getRandomGenerator", &nlm::OptimizedBrain::getRandomGenerator)
        .def("enableEventDriven", &nlm::OptimizedBrain::enableEventDriven, py::arg("enable"))
        .def("enableMultithreading", &nlm::OptimizedBrain::enableMultithreading, py::arg("enable"))
        .def("enableSIMD", &nlm::OptimizedBrain::enableSIMD, py::arg("enable"))
        .def("enableSparseConnectivity", &nlm::OptimizedBrain::enableSparseConnectivity, py::arg("enable"))
        .def("isEventDrivenEnabled", &nlm::OptimizedBrain::isEventDrivenEnabled)
        .def("isMultithreadingEnabled", &nlm::OptimizedBrain::isMultithreadingEnabled)
        .def("isSIMDEnabled", &nlm::OptimizedBrain::isSIMDEnabled)
        .def("isSparseEnabled", &nlm::OptimizedBrain::isSparseEnabled)
        .def("setThreadCount", &nlm::OptimizedBrain::setThreadCount, py::arg("count"))
        .def("getThreadCount", &nlm::OptimizedBrain::getThreadCount)
        .def("getPerformanceStats", &nlm::OptimizedBrain::getPerformanceStats);

    // Factory functions
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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm"