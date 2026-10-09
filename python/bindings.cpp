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
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for advanced systems
class NeuralPrediction;
class ActionConsequencePredictor;
class PredictionErrorSignal;
class ConceptFormation;
class NeuralPlanner;
class SpatialRepresentation;
class TemporalRelation;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class Neuromodulator;
class Dopamine;
class Serotonin;
class Norepinephrine;
class Acetylcholine;
class Curiosity;
class Novelty;
class Reward;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        === BASIC CLASSES ===
        - Config: Configuration class for NLM system
        - SensoryInput, Vision, Audio, InternalSignals: Sensory input classes
        - Action: Motor output representation
        - WorldObject: World object representation
        - AgentBody: Agent body state
        - ActionResult: Action result from world
        - SensoryPercept: Sensory percept data
        
        === CORE SYSTEMS ===
        - SimpleWorld: Simple 2D world for NLM simulation
        - Brain: Central neural simulation brain class
        - AgentBrain: Agent brain interface connecting NLM brain to world
        
        === ADVANCED PREDICTION SYSTEMS ===
        - NeuralPrediction: Temporal prediction with sequence learning
        - ActionConsequencePredictor: Learns action->consequence mappings
        - PredictionErrorSignal: Computes prediction error
        - PredictionSystem: Basic prediction system
        
        === ADVANCED COGNITION SYSTEMS ===
        - ConceptFormation: Pattern discovery, prototype formation
        - NeuralPlanner: Action planning with sequence evaluation
        - SpatialRepresentation: Place cell formation, spatial navigation
        - TemporalRelation: Temporal sequence learning
        
        === ADVANCED MEMORY SYSTEMS ===
        - NeuralWorkingMemory: Working memory with persistent activity
        - NeuralEpisodicMemory: Episodic memory with experience storage
        
        === NEUROMODULATION SYSTEMS ===
        - Neuromodulator: Base class for all neuromodulators
        - Dopamine: Reward and reinforcement learning signal
        - Serotonin: Mood, learning, sleep, attention, avoidance learning
        - Norepinephrine: Arousal, attention, working memory, cognitive control
        - Acetylcholine: Attention, memory consolidation, sleep, cortical activation
        - Curiosity: Exploration motivation based on novelty and prediction error
        - Novelty: Novelty detection from observations
        - PredictionError: Prediction error signal for learning
        - Reward: Reward signaling system
        
        Factory functions available for creating instances of all advanced systems.
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
             "Log brain status");

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

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction system for temporal and action prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState, py::arg("sensoryState"), py::arg("currentStep"),
             "Record current sensory state for future prediction")
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("currentStep"),
             "Generate prediction for next timestep based on learned sequences")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation, py::arg("actualState"), py::arg("currentStep"),
             "Update predictions based on actual observed state")
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence, py::arg("action"), py::arg("currentState"),
             "Predict consequences of a potential action")
        .def("getPredictionError", &NeuralPrediction::getPredictionError,
             "Get current prediction error (0 = perfect prediction, 1 = total error)")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence,
             "Get prediction confidence based on consistency of learned patterns")
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps, py::arg("currentStep"), py::arg("numSteps"),
             "Multi-step prediction: predict N steps into the future")
        .def("recordAction", &NeuralPrediction::recordAction, py::arg("action"), py::arg("step"),
             "Record action that was taken for action-consequence learning")
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory,
             "Get the prediction history for analysis")
        .def("clearHistory", &NeuralPrediction::clearHistory,
             "Clear prediction history")
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"),
             "Enable/disable temporal prediction mechanism")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"),
             "Enable/disable action-consequence prediction mechanism")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"),
             "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("steps"),
             "Set prediction horizon in steps")
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons,
             "Get neurons involved in prediction")
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons,
             "Get sequence neurons");

    py::class_<ActionConsequencePredictor>(m, "ActionConsequencePredictor", R"pbdoc(Action-consequence predictor)pbdoc")
        .def(py::init<>())
        .def("initialize", &ActionConsequencePredictor::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordExperience", &ActionConsequencePredictor::recordExperience, py::arg("action"), py::arg("beforeState"), py::arg("afterState"), py::arg("reward"),
             "Record that taking an action in a state led to a specific consequence")
        .def("predictConsequence", &ActionConsequencePredictor::predictConsequence, py::arg("action"), py::arg("currentState"),
             "Predict consequence of taking an action in a given state")
        .def("getConsequenceConfidence", &ActionConsequencePredictor::getConsequenceConfidence, py::arg("action"), py::arg("currentState"),
             "Get prediction confidence for an action")
        .def("updatePrediction", &ActionConsequencePredictor::updatePrediction, py::arg("action"), py::arg("predicted"), py::arg("actual"), py::arg("reward"),
             "Update based on actual observed consequence")
        .def("getActionQuality", &ActionConsequencePredictor::getActionQuality, py::arg("action"),
             "Get learned action quality (average expected reward)")
        .def("clear", &ActionConsequencePredictor::clear,
             "Clear learned associations")
        .def("getPredictionNeurons", &ActionConsequencePredictor::getPredictionNeurons,
             "Get neurons involved in prediction")
        .def("getSequenceNeurons", &ActionConsequencePredictor::getSequenceNeurons,
             "Get sequence neurons");

    py::class_<PredictionErrorSignal>(m, "PredictionErrorSignal", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionErrorSignal::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("computeError", &PredictionErrorSignal::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute error between predicted and actual values")
        .def("getErrorComponents", &PredictionErrorSignal::getErrorComponents,
             "Get error components (what aspect was unexpected)")
        .def("getModulationSignal", &PredictionErrorSignal::getModulationSignal,
             "Get neuromodulation signal strength (0-1)")
        .def("recordError", &PredictionErrorSignal::recordError, py::arg("error"), py::arg("step"),
             "Update history")
        .def("getErrorHistory", &PredictionErrorSignal::getErrorHistory,
             "Get error history")
        .def("isSurprising", &PredictionErrorSignal::isSurprising,
             "Is this a surprising event (error > threshold)?")
        .def("setSurpriseThreshold", &PredictionErrorSignal::setSurpriseThreshold, py::arg("threshold"),
             "Set surprise threshold")
        .def("getSurpriseThreshold", &PredictionErrorSignal::getSurpriseThreshold,
             "Get surprise threshold");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present a new experience to the concept formation system")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all concepts")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get the concept that best matches a pattern")
        .def("getMatchingConcept", [](const ConceptFormation& self, const std::vector<float>& pattern) {
            return self.getMatchingConcept(pattern);
        }, py::arg("pattern"),
             "Get concept that matches pattern with default threshold")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             "Measure concept stability (0-1)")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             py::return_value_policy::reference_internal,
             "Get a specific concept")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             "Merge two concepts")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"),
             "Compute similarity between two patterns")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("isNovel", [](const ConceptFormation& self, const std::vector<float>& pattern) {
            return self.isNovel(pattern);
        }, py::arg("pattern"),
             "Check if pattern is novel with default threshold")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             "Get concept for a given pattern")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             "Get generalization ability (can concept handle variations?)")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             "Set stability window");

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural action planner)pbdoc")
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
             "Has recent planning been successful?")
        .def("getActionQuality", [](const NeuralPlanner& self, ActionType action) {
            return self.getActionQuality(action);
        }, py::arg("action"),
             "Get action quality for specific action");

    py::class_<SpatialRepresentation>(m, "SpatialRepresentation", R"pbdoc(Spatial representation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &SpatialRepresentation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordPosition", &SpatialRepresentation::recordPosition, py::arg("x"), py::arg("y"), py::arg("sensoryCues"),
             "Record current position experience")
        .def("getPredictedPosition", &SpatialRepresentation::getPredictedPosition,
             "Get predicted position based on path integration")
        .def("integrateMovement", &SpatialRepresentation::integrateMovement, py::arg("dx"), py::arg("dy"),
             "Update position estimate based on movement")
        .def("getPositionActivation", &SpatialRepresentation::getPositionActivation, py::arg("x"), py::arg("y"),
             "Get position neuron activation for a given position")
        .def("getPlaceNeuronsNear", &SpatialRepresentation::getPlaceNeuronsNear, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Find place neurons near a position")
        .def("getGridSpacing", &SpatialRepresentation::getGridSpacing,
             "Get grid spacing (if grid cells have formed)")
        .def("clear", &SpatialRepresentation::clear,
             "Clear spatial representations")
        .def("hasPlaceCells", &SpatialRepresentation::hasPlaceCells,
             "Have place cells formed?")
        .def("getCurrentPosition", []() -> std::pair<float, float> { return std::make_pair(0.0f, 0.0f); })
        .def("setCurrentPosition", [](SpatialRepresentation& self, float x, float y) {
            self.currentX_ = x;
            self.currentY_ = y;
        }, py::arg("x"), py::arg("y"),
             "Set current position (use recordPosition for full integration)");

    py::class_<TemporalRelation>(m, "TemporalRelation", R"pbdoc(Temporal relation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &TemporalRelation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("recordSequence", &TemporalRelation::recordSequence, py::arg("eventA"), py::arg("eventB"), py::arg("reward"),
             "Record that event A was followed by event B")
        .def("predictNext", &TemporalRelation::predictNext, py::arg("eventA"),
             "Given event A, predict what comes next")
        .def("predictPrevious", &TemporalRelation::predictPrevious, py::arg("eventB"),
             "Given event B, predict what came before")
        .def("getPredictionConfidence", &TemporalRelation::getPredictionConfidence, py::arg("event"),
             "Get confidence in prediction")
        .def("getSequenceLength", &TemporalRelation::getSequenceLength,
             "Get sequence length")
        .def("clear", &TemporalRelation::clear,
             "Clear sequences")
        .def("getSequenceCount", [](const TemporalRelation& self) {
            return self.getSequenceLength();
        },
             "Get number of sequences");

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             "Get activation level of a specific neuron in working memory")
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
             "Strengthen working memory representation (for rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Competition between memory traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to a query pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from a specific time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5,
             "Retrieve episodes following a specific action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             "Replay an episode - reactivates neural patterns")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"),
             "Update episode relevance based on recent use")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             py::return_value_policy::reference_internal,
             "Get episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             "Get recent episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"),
             "Clear old episodes")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear all episodes")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             "Enable/disable replay")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             "Check if replay is enabled")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
             "Get episodes for replay (selection based on relevance and recency)")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
             "Replay multiple episodes (for sleep-like consolidation")
        .def("getMaxEpisodes", []() -> size_t { return 1000; })
        .def("setMaxEpisodes", [](NeuralEpisodicMemory& self, size_t max) {
            self.maxEpisodes_ = max;
        }, py::arg("max"),
             "Set maximum number of episodes");

    // Neuromodulator system bindings
    py::class_<Neuromodulator>(m, "Neuromodulator", py::module_local(), R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName,
             "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get neuromodulatory effect on plasticity")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Dopamine::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error");

    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Serotonin::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("signalMood", &Serotonin::signalMood, py::arg("mood"),
             "Signal mood")
        .def("signalLearning", &Serotonin::signalLearning, py::arg("learningSignal"),
             "Signal learning")
        .def("signalAvoidance", &Serotonin::signalAvoidance, py::arg("avoidanceStrength"),
             "Signal avoidance learning")
        .def("getAttentionEffect", &Serotonin::getAttentionEffect,
             "Get attention effect")
        .def("getAvoidanceLearningEffect", &Serotonin::getAvoidanceLearningEffect,
             "Get avoidance learning effect");

    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Norepinephrine::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("signalArousal", &Norepinephrine::signalArousal, py::arg("arousal"),
             "Signal arousal")
        .def("signalAttention", &Norepinephrine::signalAttention, py::arg("attention"),
             "Signal attention")
        .def("signalWorkingMemory", &Norepinephrine::signalWorkingMemory, py::arg("memoryUpdate"),
             "Signal working memory update")
        .def("getAttentionEffect", &Norepinephrine::getAttentionEffect,
             "Get attention effect")
        .def("getWorkingMemoryEffect", &Norepinephrine::getWorkingMemoryEffect,
             "Get working memory effect");

    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Acetylcholine::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("signalAttention", &Acetylcholine::signalAttention, py::arg("attention"),
             "Signal attention")
        .def("signalMemory", &Acetylcholine::signalMemory, py::arg("memorySignal"),
             "Signal memory consolidation")
        .def("signalSleep", &Acetylcholine::signalSleep, py::arg("sleepState"),
             "Signal sleep state")
        .def("getAttentionEffect", &Acetylcholine::getAttentionEffect,
             "Get attention effect")
        .def("getMemoryConsolidationEffect", &Acetylcholine::getMemoryConsolidationEffect,
             "Get memory consolidation effect");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive (same as level)")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set prediction error weight")
        .def("reset", &Curiosity::reset,
             "Reset");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel,
             "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set novelty level")
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from sensory input pattern")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& currentPattern) {
            std::vector<float> emptyPattern(currentPattern.size(), 0.0f);
            return self.detectNovelty(currentPattern, emptyPattern);
        }, py::arg("currentPattern"),
             "Detect novelty from current pattern (assuming empty previous)")
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
             "Get error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction")
        .def("getHistory", &PredictionError::getHistory,
             "Get history")
        .def("clearHistory", &PredictionError::clearHistory,
             "Clear history")
        .def("getMagnitude", &PredictionError::getMagnitude,
             "Get error magnitude for neuromodulation");

    py::class_<Reward>(m, "Reward", R"pbdoc(Reward neuromodulator)pbdoc")
        .def(py::init<>())
        .def("initialize", &Reward::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Reward::getLevel,
             "Get reward level")
        .def("signalReward", &Reward::signalReward, py::arg("reward"),
             "Signal reward")
        .def("signalPredictionError", &Reward::signalPredictionError, py::arg("error"),
             "Signal prediction error on reward")
        .def("update", &Reward::update, py::arg("dt"),
             "Update reward");

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

    // Advanced system factory functions
    m.def("createNeuralPrediction", []() -> std::unique_ptr<NeuralPrediction> {
        return std::make_unique<NeuralPrediction>();
    }, "Create a new neural prediction system");

    m.def("createActionConsequencePredictor", []() -> std::unique_ptr<ActionConsequencePredictor> {
        return std::make_unique<ActionConsequencePredictor>();
    }, "Create a new action consequence predictor");

    m.def("createPredictionErrorSignal", []() -> std::unique_ptr<PredictionErrorSignal> {
        return std::make_unique<PredictionErrorSignal>();
    }, "Create a new prediction error signal");

    m.def("createPredictionSystem", []() -> std::unique_ptr<PredictionSystem> {
        return std::make_unique<PredictionSystem>();
    }, "Create a new prediction system");

    m.def("createConceptFormation", []() -> std::unique_ptr<ConceptFormation> {
        return std::make_unique<ConceptFormation>();
    }, "Create a new concept formation system");

    m.def("createNeuralPlanner", []() -> std::unique_ptr<NeuralPlanner> {
        return std::make_unique<NeuralPlanner>();
    }, "Create a new neural planner");

    m.def("createSpatialRepresentation", []() -> std::unique_ptr<SpatialRepresentation> {
        return std::make_unique<SpatialRepresentation>();
    }, "Create a new spatial representation system");

    m.def("createTemporalRelation", []() -> std::unique_ptr<TemporalRelation> {
        return std::make_unique<TemporalRelation>();
    }, "Create a new temporal relation system");

    m.def("createNeuralWorkingMemory", []() -> std::unique_ptr<NeuralWorkingMemory> {
        return std::make_unique<NeuralWorkingMemory>();
    }, "Create a new neural working memory");

    m.def("createNeuralEpisodicMemory", []() -> std::unique_ptr<NeuralEpisodicMemory> {
        return std::make_unique<NeuralEpisodicMemory>();
    }, "Create a new neural episodic memory");

    m.def("createDopamine", []() -> std::unique_ptr<Dopamine> {
        return std::make_unique<Dopamine>();
    }, "Create a new dopamine neuromodulator");

    m.def("createSerotonin", []() -> std::unique_ptr<Serotonin> {
        return std::make_unique<Serotonin>();
    }, "Create a new serotonin neuromodulator");

    m.def("createNorepinephrine", []() -> std::unique_ptr<Norepinephrine> {
        return std::make_unique<Norepinephrine>();
    }, "Create a new norepinephrine neuromodulator");

    m.def("createAcetylcholine", []() -> std::unique_ptr<Acetylcholine> {
        return std::make_unique<Acetylcholine>();
    }, "Create a new acetylcholine neuromodulator");

    m.def("createCuriosity", []() -> std::unique_ptr<Curiosity> {
        return std::make_unique<Curiosity>();
    }, "Create a new curiosity neuromodulator");

    m.def("createNovelty", []() -> std::unique_ptr<Novelty> {
        return std::make_unique<Novelty>();
    }, "Create a new novelty neuromodulator");

    m.def("createPredictionError", []() -> std::unique_ptr<PredictionError> {
        return std::make_unique<PredictionError>();
    }, "Create a new prediction error neuromodulator");

    m.def("createReward", []() -> std::unique_ptr<Reward> {
        return std::make_unique<Reward>();
    }, "Create a new reward neuromodulator");

} // namespace nlm
