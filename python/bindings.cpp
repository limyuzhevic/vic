// NLM (熙然) - Neural Learning Machine Python Bindings
// Enhanced Python API for NLM C++ neural simulation framework
// Provides comprehensive bindings for Phase 6 memory, neuromodulation, and prediction systems

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"

// Memory system bindings
#include "src/memory/WorkingMemory.hpp"
#include "src/memory/EpisodicMemory.hpp"
#include "src/memory/AssociativeMemory.hpp"

// Neuromodulation system bindings
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/Curiosity.hpp"

// Prediction system bindings
#include "src/prediction/PredictionSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Enhanced WorkingMemory bindings
py::class_<WorkingMemory> bindWorkingMemory(py::module_ &m) {
    return py::class_<WorkingMemory>(m, "WorkingMemory")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("content"), py::arg("activation") = 1.0)
        .def("retrieve", &WorkingMemory::retrieve, py::arg("threshold") = 0.1)
        .def("clear", &WorkingMemory::clear)
        .def("get_entry_count", &WorkingMemory::getEntryCount)
        .def("get_active_entry_count", &WorkingMemory::getActiveEntryCount)
        .def("add_observer", &WorkingMemory::addObserver)
        .def("remove_observer", &WorkingMemory::removeObserver)
        .def("set_decay_rate", &WorkingMemory::setDecayRate)
        .def("get_decay_rate", &WorkingMemory::getDecayRate)
        .def("set_capacity", &WorkingMemory::setCapacity)
        .def("get_capacity", &WorkingMemory::getCapacity);
}

// Enhanced EpisodicMemory bindings
py::class_<EpisodicMemory> bindEpisodicMemory(py::module_ &m) {
    return py::class_<EpisodicMemory>(m, "EpisodicMemory")
        .def(py::init<>())
        .def("store_episode", &EpisodicMemory::storeEpisode, py::arg("context"), py::arg("outcome"))
        .def("retrieve_episodes", &EpisodicMemory::retrieveEpisodes, py::arg("context_pattern") = "")
        .def("replay_episodes", &EpisodicMemory::replayEpisodes, py::arg("target_neurons"), py::arg("replay_rate") = 0.05)
        .def("get_episode_count", &EpisodicMemory::getEpisodeCount)
        .def("set_max_episodes", &EpisodicMemory::setMaxEpisodes)
        .def("get_max_episodes", &EpisodicMemory::getMaxEpisodes)
        .def("enable_replay", &EpisodicMemory::enableReplay)
        .def("disable_replay", &EpisodicMemory::disableReplay)
        .def("is_replay_enabled", &EpisodicMemory::isReplayEnabled)
        .def("clear_episodes", &EpisodicMemory::clearEpisodes);
}

// Enhanced AssociativeMemory bindings
py::class_<AssociativeMemory> bindAssociativeMemory(py::module_ &m) {
    return py::class_<AssociativeMemory>(m, "AssociativeMemory")
        .def(py::init<>())
        .def("store", &AssociativeMemory::store, py::arg("key"), py::arg("value"), py::arg("strength") = 0.5)
        .def("retrieve", &AssociativeMemory::retrieve, py::arg("key"))
        .def("search", &AssociativeMemory::search, py::arg("pattern"))
        .def("strengthen", &AssociativeMemory::strengthen, py::arg("key"), py::arg("amount") = 0.1)
        .def("weaken", &AssociativeMemory::weaken, py::arg("key"), py::arg("amount") = 0.1)
        .def("remove", &AssociativeMemory::remove, py::arg("key"))
        .def("get_entry_count", &AssociativeMemory::getEntryCount)
        .def("set_capacity", &AssociativeMemory::setCapacity)
        .def("get_capacity", &AssociativeMemory::getCapacity)
        .def("set_learning_rate", &AssociativeMemory::setLearningRate)
        .def("get_learning_rate", &AssociativeMemory::getLearningRate);
}

// Enhanced Neuromodulator bindings
py::class_<Neuromodulator> bindNeuromodulator(py::module_ &m) {
    return py::class_<Neuromodulator>(m, "Neuromodulator")
        .def(py::init<>())
        .def("update_state", &Neuromodulator::updateState)
        .def("get_dopamine_level", &Neuromodulator::getDopamineLevel)
        .def("get_curiosity_level", &Neuromodulator::getCuriosityLevel)
        .def("get_novelty_level", &Neuromodulator::getNoveltyLevel)
        .def("get_prediction_error_level", &Neuromodulator::getPredictionErrorLevel)
        .def("set_dopamine_level", &Neuromodulator::setDopamineLevel, py::arg("level"))
        .def("set_curiosity_level", &Neuromodulator::setCuriosityLevel, py::arg("level"))
        .def("set_novelty_level", &Neuromodulator::setNoveltyLevel, py::arg("level"))
        .def("set_prediction_error_level", &Neuromodulator::setPredictionErrorLevel, py::arg("level"))
        .def("add_reward", &Neuromodulator::addReward, py::arg("reward"))
        .def("get_reward_history", &Neuromodulator::getRewardHistory)
        .def("reset", &Neuromodulator::reset)
        .def("enable_plasticity_modulation", &Neuromodulator::enablePlasticityModulation)
        .def("disable_plasticity_modulation", &Neuromodulator::disablePlasticityModulation)
        .def("is_plasticity_modulation_enabled", &Neuromodulator::isPlasticityModulationEnabled);
}

// Enhanced Dopamine bindings
py::class_<Dopamine, Neuromodulator> bindDopamine(py::module_ &m) {
    return py::class_<Dopamine, Neuromodulator>(m, "Dopamine")
        .def(py::init<>())
        .def("set_dopamine_response", &Dopamine::setDopamineResponse, py::arg("response"))
        .def("get_dopamine_response", &Dopamine::getDopamineResponse)
        .def("set_learning_modulation", &Dopamine::setLearningModulation, py::arg("modulation"))
        .def("get_learning_modulation", &Dopamine::getLearningModulation);
}

// Enhanced PredictionError bindings
py::class_<PredictionError, Neuromodulator> bindPredictionError(py::module_ &m) {
    return py::class_<PredictionError, Neuromodulator>(m, "PredictionError")
        .def(py::init<>())
        .def("set_error_threshold", &PredictionError::setErrorThreshold, py::arg("threshold"))
        .def("get_error_threshold", &PredictionError::getErrorThreshold)
        .def("set_confidence_threshold", &PredictionError::setConfidenceThreshold, py::arg("threshold"))
        .def("get_confidence_threshold", &PredictionError::getConfidenceThreshold)
        .def("calculate_error", &PredictionError::calculateError, py::arg("predicted"), py::arg("actual"));
}

// Enhanced Novelty bindings
py::class_<Novelty, Neuromodulator> bindNovelty(py::module_ &m) {
    return py::class_<Novelty, Neuromodulator>(m, "Novelty")
        .def(py::init<>())
        .def("set_novelty_sensitivity", &Novelty::setNoveltySensitivity, py::arg("sensitivity"))
        .def("get_novelty_sensitivity", &Novelty::getNoveltySensitivity)
        .def("set_stimulus_history_size", &Novelty::setStimulusHistorySize, py::arg("size"))
        .def("get_stimulus_history_size", &Novelty::getStimulusHistorySize)
        .def("calculate_novelty", &Novelty::calculateNovelty, py::arg("stimulus"));
}

// Enhanced Curiosity bindings
py::class_<Curiosity, Neuromodulator> bindCuriosity(py::module_ &m) {
    return py::class_<Curiosity, Neuromodulator>(m, "Curiosity")
        .def(py::init<>())
        .def("set_curiosity_threshold", &Curiosity::setCuriosityThreshold, py::arg("threshold"))
        .def("get_curiosity_threshold", &Curiosity::getCuriosityThreshold)
        .def("set_exploration_rate", &Curiosity::setExplorationRate, py::arg("rate"))
        .def("get_exploration_rate", &Curiosity::getExplorationRate)
        .def("calculate_curiosity", &Curiosity::calculateCuriosity, py::arg("environment"));
}

// Enhanced PredictionSystem bindings
py::class_<PredictionSystem> bindPredictionSystem(py::module_ &m) {
    return py::class_<PredictionSystem>(m, "PredictionSystem")
        .def(py::init<>())
        .def("make_prediction", &PredictionSystem::makePrediction, py::arg("action"), py::arg("context"))
        .def("update_prediction", &PredictionSystem::updatePrediction, py::arg("action"), py::arg("reward"))
        .def("get_prediction", &PredictionSystem::getPrediction, py::arg("action"))
        .def("get_predicted_reward", &PredictionSystem::getPredictedReward, py::arg("action"))
        .def("get_confidence", &PredictionSystem::getConfidence, py::arg("action"))
        .def("set_learning_rate", &PredictionSystem::setLearningRate, py::arg("rate"))
        .def("get_learning_rate", &PredictionSystem::getLearningRate)
        .def("set_error_threshold", &PredictionSystem::setErrorThreshold, py::arg("threshold"))
        .def("get_error_threshold", &PredictionSystem::getErrorThreshold)
        .def("get_prediction_count", &PredictionSystem::getPredictionCount)
        .def("reset", &PredictionSystem::reset);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        -----------------------------------------------------------------
        A comprehensive Python API for the NLM C++ neural simulation framework.
        This enhanced version includes:
        
        **Memory Systems:**
        - WorkingMemory: Short-term memory with activation decay
        - EpisodicMemory: Event-based memory with replay capability
        - AssociativeMemory: Pattern-based memory associations
        
        **Neuromodulation Systems:**
        - Neuromodulator: Base neuromodulation system
        - Dopamine: Reward prediction and learning modulation
        - PredictionError: Error signal for prediction learning
        - Novelty: Novelty detection and exploration
        - Curiosity: Intrinsic motivation and exploration
        
        **Prediction System:**
        - PredictionSystem: Action prediction and reward forecasting
        
        **Core Systems:**
        - Brain: Central neural simulation brain
        - Config: Configuration management
        - SimpleWorld: 2D simulation environment
        - AgentBrain: Brain-world interface
        
        **Sensory/Motor:**
        - Vision, Audio, InternalSignals: Sensory inputs
        - Action, WorldObject, AgentBody: Motor outputs
        - SensoryPercept: Integrated sensory processing
        
        **Enhanced Python Utilities:**
        - MemoryIntegrator: Memory system integration
        - ExperimentRunner: Experiment management
        - NLM: High-level NLM interface
        
        **Type Definitions:**
        - NeuronId, SynapseId, RegionId, PopulationId
        - NeuronType, SynapseType, DevelopmentalStage, FiringState
        - ActionType, MotorCommand, WorldObjectType
        - SimulationStep, Timestamp
        
        **Constants:**
        - INVALID_NEURON_ID, INVALID_SYNAPSE_ID
        - INVALID_REGION_ID, INVALID_POPULATION_ID
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Core type definitions
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

    // Memory system classes
    bindWorkingMemory(m);
    bindEpisodicMemory(m);
    bindAssociativeMemory(m);

    // Neuromodulation system classes
    bindNeuromodulator(m);
    bindDopamine(m);
    bindPredictionError(m);
    bindNovelty(m);
    bindCuriosity(m);

    // Prediction system class
    bindPredictionSystem(m);

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

    // Motor classes
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

    // Agent classes
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

    // World and Agent classes
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

    // Main Brain class
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

    // AgentBrain class
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

    // Convenience factory functions
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

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Enhanced Python module exports (these will be included in nlm_core.py)
    // The actual enhanced functionality is provided through the nlm_core module
}

} // namespace nlm"