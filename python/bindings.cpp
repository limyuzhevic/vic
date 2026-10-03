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
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/BCM.hpp"
#include "../src/plasticity/AntiHebbian.hpp"

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

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Neural prediction system for future state prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"),
             "Initialize prediction system with brain reference")
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"),
             "Predict next sensory state based on current neural activity")
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"),
             "Update predictions based on actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error value")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence level")
        .def("getNeuralConfidence", &PredictionSystem::getNeuralConfidence,
             "Get neural prediction confidence based on activity patterns")
        .def("train", &PredictionSystem::train, py::arg("observation"),
             "Train prediction model with observation")
        .def("update", &PredictionSystem::update, py::arg("currentTime"),
             "Update prediction system with current time")
        .def("reset", &PredictionSystem::reset,
             "Reset prediction system to initial state")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel, "Get dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"), "Set dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get dopamine-modulated plasticity factor")
        .def("update", &Dopamine::update, py::arg("dt"), "Update dopamine state")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error")
        .def("getBaseline", &Dopamine::getBaseline,
             "Get dopamine baseline level")
        .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"),
             "Set dopamine baseline level")
        .def("getPeak", &Dopamine::getPeak,
             "Get dopamine peak level")
        .def("setPeak", &Dopamine::setPeak, py::arg("peak"), "Set dopamine peak level");

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator for attention and memory)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Acetylcholine::getLevel, "Get acetylcholine level")
        .def("setLevel", &Acetylcholine::setLevel, py::arg("level"), "Set acetylcholine level")
        .def("getPlasticityFactor", &Acetylcholine::getPlasticityFactor,
             "Get acetylcholine-modulated plasticity factor")
        .def("update", &Acetylcholine::update, py::arg("dt"), "Update acetylcholine state")
        .def("signalAttention", &Acetylcholine::signalAttention, py::arg("strength"),
             "Signal attentional focus strength");

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator for arousal and vigilance)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Norepinephrine::getLevel, "Get norepinephrine level")
        .def("setLevel", &Norepinephrine::setLevel, py::arg("level"), "Set norepinephrine level")
        .def("getPlasticityFactor", &Norepinephrine::getPlasticityFactor,
             "Get norepinephrine-modulated plasticity factor")
        .def("update", &Norepinephrine::update, py::arg("dt"), "Update norepinephrine state")
        .def("signalAlertness", &Norepinephrine::signalAlertness, py::arg("arousalLevel"),
             "Signal arousal/attention level");

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator for mood and impulsivity)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Serotonin::getLevel, "Get serotonin level")
        .def("setLevel", &Serotonin::setLevel, py::arg("level"), "Set serotonin level")
        .def("getPlasticityFactor", &Serotonin::getPlasticityFactor,
             "Get serotonin-modulated plasticity factor")
        .def("update", &Serotonin::update, py::arg("dt"), "Update serotonin state")
        .def("signalMood", &Serotonin::signalMood, py::arg("moodLevel"),
             "Signal mood/affect level");

    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration behavior)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"), "Set curiosity level")
        .def("getPlasticityFactor", &Curiosity::getPlasticityFactor,
             "Get curiosity-modulated plasticity factor")
        .def("update", &Curiosity::update, py::arg("dt"), "Update curiosity state")
        .def("signalExploration", &Curiosity::signalExploration,
             "Signal exploratory behavior");

    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty neuromodulator for detecting new stimuli)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("getPlasticityFactor", &Novelty::getPlasticityFactor,
             "Get novelty-modulated plasticity factor")
        .def("update", &Novelty::update, py::arg("dt"), "Update novelty state")
        .def("signalNewStimulus", &Novelty::signalNewStimulus, py::arg("stimulusIntensity"),
             "Signal detection of novel stimulus");

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator for learning from surprises)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel, "Get prediction error level")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"), "Set prediction error level")
        .def("getPlasticityFactor", &PredictionError::getPlasticityFactor,
             "Get prediction error-modulated plasticity factor")
        .def("update", &PredictionError::update, py::arg("dt"), "Update prediction error state")
        .def("signalError", &PredictionError::signalError, py::arg("errorMagnitude"),
             "Signal prediction error");

    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward neuromodulator for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Reward::getLevel, "Get reward level")
        .def("setLevel", &Reward::setLevel, py::arg("level"), "Set reward level")
        .def("getPlasticityFactor", &Reward::getPlasticityFactor,
             "Get reward-modulated plasticity factor")
        .def("update", &Reward::update, py::arg("dt"), "Update reward state")
        .def("signalOutcome", &Reward::signalOutcome, py::arg("outcomeValue"),
             "Signal environmental outcome");

    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian plasticity rule: 'neurons that fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on pre/post synaptic activity")
        .def("applyWeightChange", &Hebbian::applyWeightChange,
             py::arg("synapse"), py::arg("delta"), "Apply weight change to synapse")
        .def("getName", &Hebbian::getName, "Get plasticity rule name")
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get current learning rate")
        .def("getWeight", &Hebbian::getWeight, "Get current synaptic weight")
        .def("setWeight", &Hebbian::setWeight, py::arg("weight"), "Set synaptic weight");

    py::class_<BCM, PlasticityRule>(m, "BCM", R"pbdoc(Bienenstock-Cooper-Munro (BCM) plasticity rule for supervised learning)pbdoc")
        .def(py::init<>())
        .def("update", &BCM::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights using BCM rule")
        .def("applyWeightChange", &BCM::applyWeightChange,
             py::arg("synapse"), py::arg("delta"), "Apply weight change to synapse")
        .def("getName", &BCM::getName, "Get plasticity rule name")
        .def("setLearningRate", &BCM::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &BCM::getLearningRate, "Get current learning rate")
        .def("setThreshold", &BCM::setThreshold, py::arg("threshold"), "Set BCM threshold")
        .def("getThreshold", &BCM::getThreshold, "Get current BCM threshold");

    py::class_<AntiHebbian, PlasticityRule>(m, "AntiHebbian", R"pbdoc(Anti-Hebbian plasticity rule: 'neurons that fire together, unwire together')pbdoc")
        .def(py::init<>())
        .def("update", &AntiHebbian::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights using anti-Hebbian rule")
        .def("applyWeightChange", &AntiHebbian::applyWeightChange,
             py::arg("synapse"), py::arg("delta"), "Apply weight change to synapse")
        .def("getName", &AntiHebbian::getName, "Get plasticity rule name")
        .def("setLearningRate", &AntiHebbian::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &AntiHebbian::getLearningRate, "Get current learning rate");

    m.def("createDopamine", []() -> std::shared_ptr<Dopamine> {
        return std::make_shared<Dopamine>();
    }, "Create a new dopamine neuromodulator");

    m.def("createAcetylcholine", []() -> std::shared_ptr<Acetylcholine> {
        return std::make_shared<Acetylcholine>();
    }, "Create a new acetylcholine neuromodulator");

    m.def("createNorepinephrine", []() -> std::shared_ptr<Norepinephrine> {
        return std::make_shared<Norepinephrine>();
    }, "Create a new norepinephrine neuromodulator");

    m.def("createSerotonin", []() -> std::shared_ptr<Serotonin> {
        return std::make_shared<Serotonin>();
    }, "Create a new serotonin neuromodulator");

    m.def("createCuriosity", []() -> std::shared_ptr<Curiosity> {
        return std::make_shared<Curiosity>();
    }, "Create a new curiosity neuromodulator");

    m.def("createNovelty", []() -> std::shared_ptr<Novelty> {
        return std::make_shared<Novelty>();
    }, "Create a new novelty neuromodulator");

    m.def("createPredictionError", []() -> std::shared_ptr<PredictionError> {
        return std::make_shared<PredictionError>();
    }, "Create a new prediction error neuromodulator");

    m.def("createReward", []() -> std::shared_ptr<Reward> {
        return std::make_shared<Reward>();
    }, "Create a new reward neuromodulator");

    m.def("createHebbian", []() -> std::shared_ptr<Hebbian> {
        return std::make_shared<Hebbian>();
    }, "Create a new Hebbian plasticity rule");

    m.def("createBCM", []() -> std::shared_ptr<BCM> {
        return std::make_shared<BCM>();
    }, "Create a new BCM plasticity rule");

    m.def("createAntiHebbian", []() -> std::shared_ptr<AntiHebbian> {
        return std::make_shared<AntiHebbian>();
    }, "Create a new anti-Hebbian plasticity rule");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
