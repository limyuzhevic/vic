#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "src/core/Types/Types.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/AttentionalSelection.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/sensory/Vision.hpp"
#include "src/sensory/Audio.hpp"
#include "src/sensory/InternalSignals.hpp"
#include "src/motor/Action.hpp"
#include "src/motor/MotorSystem.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// NeuralWorkingMemory Python bindings
py::class_<NeuralWorkingMemory> bindNeuralWorkingMemory(py::module_& m) {
    return m.def_class_helper<NeuralWorkingMemory, py::class_<NeuralWorkingMemory>>(m, "NeuralWorkingMemory")
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"))
        .def("store", static_cast<void (NeuralWorkingMemory::*)(const std::vector<float>&, float)>(&NeuralWorkingMemory::store), 
             py::arg("pattern"), py::arg("strength") = 1.0f)
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"))
        .def("retrieve", &NeuralWorkingMemory::retrieve)
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"))
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"))
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"))
        .def("clear", &NeuralWorkingMemory::clear)
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces)
        .def("getCapacity", &NeuralWorkingMemory::getCapacity)
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"))
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate)
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"))
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons)
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"))
        .def("runCompetition", &NeuralWorkingMemory::runCompetition)
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"))
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity);
}

// NeuralEpisodicMemory Python bindings  
py::class_<NeuralEpisodicMemory> bindNeuralEpisodicMemory(py::module_& m) {
    return m.def_class_helper<NeuralEpisodicMemory, py::class_<NeuralEpisodicMemory>>(m, "NeuralEpisodicMemory")
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"))
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"))
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("sensoryPattern"), py::arg("maxResults") = 5)
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal, py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10)
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5)
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction, py::arg("action"), py::arg("maxResults") = 5)
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"))
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, py::arg("episodeId"), py::arg("relevanceDelta"))
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"))
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount)
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"))
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward)
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"))
        .def("clear", &NeuralEpisodicMemory::clear)
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"))
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled)
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"))
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"));
}

// NeuralAssociativeMemory Python bindings
py::class_<NeuralAssociativeMemory> bindNeuralAssociativeMemory(py::module_& m) {
    return m.def_class_helper<NeuralAssociativeMemory, py::class_<NeuralAssociativeMemory>>(m, "NeuralAssociativeMemory")
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"))
        .def("associate", static_cast<void (NeuralAssociativeMemory::*)(const std::vector<float>&, const std::vector<float>&, float)>(&NeuralAssociativeMemory::associate), 
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f)
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience, py::arg("episode"))
        .def("retrieve", &NeuralAssociativeMemory::retrieve, py::arg("queryPattern"), py::arg("maxResults") = 5)
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("patternA"), py::arg("patternB"))
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation, py::arg("patternA"), py::arg("patternB"), py::arg("delta"))
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation, py::arg("cuePattern"), py::arg("steps") = 2)
        .def("clear", &NeuralAssociativeMemory::clear)
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount);
}

// PredictionSystem Python bindings
py::class_<PredictionSystem> bindPredictionSystem(py::module_& m) {
    return m.def_class_helper<PredictionSystem, py::class_<PredictionSystem>>(m, "PredictionSystem")
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"))
        .def("updatePrediction", &PredictionSystem::updatePrediction, py::arg("sensoryInput"))
        .def("getPredictionError", &PredictionSystem::getPredictionError, py::arg("sensoryInput"))
        .def("getPrediction", &PredictionSystem::getPrediction, py::arg("sensoryInput"))
        .def("getConfidence", &PredictionSystem::getConfidence, py::arg("sensoryInput"))
        .def("getNextPredictedState", &PredictionSystem::getNextPredictedState, py::arg("sensoryInput"))
        .def("updateFromError", &PredictionSystem::updateFromError, py::arg("error"), py::arg("sensoryInput"))
        .def("clear", &PredictionSystem::clear)
        .def("getPredictionCount", &PredictionSystem::getPredictionCount)
        .def("getHistorySize", &PredictionSystem::getHistorySize);
}

// NeuralPlanner Python bindings
py::class_<NeuralPlanner> bindNeuralPlanner(py::module_& m) {
    return m.def_class_helper<NeuralPlanner, py::class_<NeuralPlanner>>(m, "NeuralPlanner")
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"))
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f)
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"))
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"))
        .def("clearCache", &NeuralPlanner::clearCache)
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"))
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal)
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"))
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful);
}

// ConceptFormation Python bindings
py::class_<ConceptFormation> bindConceptFormation(py::module_& m) {
    return m.def_class_helper<ConceptFormation, py::class_<ConceptFormation>>(m, "ConceptFormation")
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"))
        .def("processPattern", &ConceptFormation::processPattern, py::arg("pattern"))
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"))
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("getActiveConcepts", &ConceptFormation::getActiveConcepts)
        .def("updateConcepts", &ConceptFormation::updateConcepts, py::arg("dt"))
        .def("getConceptStrength", &ConceptFormation::getConceptStrength, py::arg("conceptId"))
        .def("updateConceptStrength", &ConceptFormation::updateConceptStrength, py::arg("conceptId"), py::arg("delta"))
        .def("clearConcepts", &ConceptFormation::clearConcepts)
        .def("getConceptCategories", &ConceptFormation::getConceptCategories);
}

// AttentionalSelection Python bindings
py::class_<AttentionalSelection> bindAttentionalSelection(py::module_& m) {
    return m.def_class_helper<AttentionalSelection, py::class_<AttentionalSelection>>(m, "AttentionalSelection")
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"))
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f)
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"))
        .def("releaseAttention", &AttentionalSelection::releaseAttention)
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions)
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"))
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"))
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"))
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"))
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"))
        .def("update", &AttentionalSelection::update, py::arg("dt"))
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"))
        .def("getWinners", &AttentionalSelection::getWinners)
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"))
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"))
        .def("reset", &AttentionalSelection::reset);
}

// DevelopmentSystem Python bindings
py::class_<DevelopmentSystem> bindDevelopmentSystem(py::module_& m) {
    return m.def_class_helper<DevelopmentSystem, py::class_<DevelopmentSystem>>(m, "DevelopmentSystem")
        .def("initialize", &DevelopmentSystem::initialize, py::arg("brain"))
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"))
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("getStageStartTime", &DevelopmentSystem::getStageStartTime)
        .def("getTimeInCurrentStage", &DevelopmentSystem::getTimeInCurrentStage)
        .def("getCurrentPlasticityRate", &DevelopmentSystem::getCurrentPlasticityRate)
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress)
        .def("getCriticalPeriodEnd", &DevelopmentSystem::getCriticalPeriodEnd)
        .def("isInCriticalPeriod", &DevelopmentSystem::isInCriticalPeriod)
        .def("getStageDuration", &DevelopmentSystem::getStageDuration, py::arg("stage"))
        .def("setStageDuration", &DevelopmentSystem::setStageDuration, py::arg("stage"), py::arg("duration"));
}

// Neuromodulator base class Python bindings
py::class_<Neuromodulator> bindNeuromodulator(py::module_& m) {
    return m.def_class_helper<Neuromodulator, py::class_<Neuromodulator>>(m, "Neuromodulator")
        .def("getName", &Neuromodulator::getName)
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("update", &Neuromodulator::update, py::arg("dt"))
        .def("getType", &Neuromodulator::getType);
}

// Dopamine Python bindings
py::class_<Dopamine, Neuromodulator> bindDopamine(py::module_& m) {
    return m.def_class_helper<Dopamine, Neuromodulator>(m, "Dopamine")
        .def("initialize", &Dopamine::initialize, py::arg("brain"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"))
        .def("getBaseline", &Dopamine::getBaseline)
        .def("setPeak", &Dopamine::setPeak, py::arg("peak"))
        .def("getPeak", &Dopamine::getPeak);
}

// Curiosity Python bindings
py::class_<Curiosity, Neuromodulator> bindCuriosity(py::module_& m) {
    return m.def_class_helper<Curiosity, Neuromodulator>(m, "Curiosity")
        .def("initialize", &Curiosity::initialize, py::arg("brain"))
        .def("signalNovelty", &Curiosity::signalNovelty, py::arg("novelty"))
        .def("getExplorationDrive", &Curiosity::getExplorationDrive)
        .def("setExplorationThreshold", &Curiosity::setExplorationThreshold, py::arg("threshold"))
        .def("getExplorationThreshold", &Curiosity::getExplorationThreshold)
        .def("setCuriosityDecay", &Curiosity::setCuriosityDecay, py::arg("decay"))
        .def("getCuriosityDecay", &Curiosity::getCuriosityDecay);
}

// Novelty Python bindings
py::class_<Novelty, Neuromodulator> bindNovelty(py::module_& m) {
    return m.def_class_helper<Novelty, Neuromodulator>(m, "Novelty")
        .def("initialize", &Novelty::initialize, py::arg("brain"))
        .def("signalChange", &Novelty::signalChange, py::arg("changeMagnitude"))
        .def("getNoveltyLevel", &Novelty::getNoveltyLevel)
        .def("setNoveltySensitivity", &Novelty::setNoveltySensitivity, py::arg("sensitivity"))
        .def("getNoveltySensitivity", &Novelty::getNoveltySensitivity)
        .def("setResetRate", &Novelty::setResetRate, py::arg("rate"))
        .def("getResetRate", &Novelty::getResetRate);
}

// PredictionError Python bindings
py::class_<PredictionError, Neuromodulator> bindPredictionError(py::module_& m) {
    return m.def_class_helper<PredictionError, Neuromodulator>(m, "PredictionError")
        .def("initialize", &PredictionError::initialize, py::arg("brain"))
        .def("signalError", &PredictionError::signalError, py::arg("error"))
        .def("getPredictionErrorLevel", &PredictionError::getPredictionErrorLevel)
        .def("setErrorSensitivity", &PredictionError::setErrorSensitivity, py::arg("sensitivity"))
        .def("getErrorSensitivity", &PredictionError::getErrorSensitivity)
        .def("setErrorDecay", &PredictionError::setErrorDecay, py::arg("decay"))
        .def("getErrorDecay", &PredictionError::getErrorDecay);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Extended Python Bindings
        ---------------------------------------------------------
        Extended Python binding for the NLM C++ neural simulation framework.
        Includes all memory, cognition, prediction, development, and neuromodulation systems.
        
        This provides access to the complete Phase 6 integrated artificial brain architecture.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Core types (basic bindings)
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

    // Existing basic bindings (for compatibility)
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

    // Existing bindings from original pynlm module
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

    // Memory systems
    bindNeuralWorkingMemory(m);
    bindNeuralEpisodicMemory(m);
    bindNeuralAssociativeMemory(m);

    // Prediction system
    bindPredictionSystem(m);

    // Cognition systems
    bindNeuralPlanner(m);
    bindConceptFormation(m);
    bindAttentionalSelection(m);

    // Development system
    bindDevelopmentSystem(m);

    // Neuromodulation systems
    bindDopamine(m);
    bindCuriosity(m);
    bindNovelty(m);
    bindPredictionError(m);

    // Agent system
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

    // Core functions
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

    // Version info
    m.attr("__version__") = "0.1.0-extended";  // Extended bindings version
    m.attr("__build_info__") = "Phase 6 - Complete integration with extended Python bindings";
}

} // namespace nlm