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

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing neural populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"),
             py::return_value_policy::reference_internal)
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal)
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"),
             py::return_value_policy::reference_internal)
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"),
             py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity)
        .def("getAllNeurons", &NeuralRegion::getAllNeurons,
             py::return_value_policy::reference_internal)
        .def("getAllPopulations", &NeuralRegion::getAllPopulations,
             py::return_value_policy::reference_internal);

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population with shared neuron properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"),
             py::return_value_policy::reference_internal)
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("getNeurons", &NeuralPopulation::getNeurons,
             py::return_value_policy::reference_internal)
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"))
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        .def("recordActivity", &NeuralPopulation::recordActivity)
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential)
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential);

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Neural connection with dynamic weight and delay)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId)
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"))
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"))
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory)
        .def("clearHistory", &Synapse::clearHistory)
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags,
             py::return_value_policy::reference_internal)
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("reset", &Synapse::reset)
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"));

    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Working memory for active maintenance and competition)pbdoc")
        .def("storeToNeuron", &WorkingMemory::storeToNeuron, py::arg("neuronId"), py::arg("activation"))
        .def("update", &WorkingMemory::update, py::arg("timestep"))
        .def("getMemoryNeurons", &WorkingMemory::getMemoryNeurons,
             py::return_value_policy::reference_internal)
        .def("getActiveTraces", &WorkingMemory::getActiveTraces)
        .def("getCapacity", &WorkingMemory::getCapacity)
        .def("setCapacity", &WorkingMemory::setCapacity, py::arg("capacity"))
        .def("clear", &WorkingMemory::clear)
        .def("getCompetitionScore", [](const WorkingMemory& self, NeuronId neuronId) {
            return self.getCompetitionScore(neuronId);
        }, py::arg("neuronId"));

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory for storing experience episodes)pbdoc")
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"))
        .def("getEpisodesForReplay", &EpisodicMemory::getEpisodesForReplay, py::arg("maxEpisodes"),
             py::return_value_policy::reference_internal)
        .def("replayEpisode", &EpisodicMemory::replayEpisode, py::arg("episode"))
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount)
        .def("setMaxEpisodes", &EpisodicMemory::setMaxEpisodes, py::arg("max"))
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("threshold"))
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("maxAge"),
             py::return_value_policy::reference_internal)
        .def("clear", &EpisodicMemory::clear)
        .def("isEpisodeImportant", [](const EpisodicMemory& self, const EpisodicMemoryItem& episode) {
            return self.isEpisodeImportant(episode);
        }, py::arg("episode"));

    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory for pattern completion and cueing)pbdoc")
        .def("storePattern", &AssociativeMemory::storePattern, py::arg("pattern"), py::arg("activation"))
        .def("retrievePattern", &AssociativeMemory::retrievePattern, py::arg("cue"))
        .def("getPatternStrength", &AssociativeMemory::getPatternStrength, py::arg("patternId"))
        .def("setActivationThreshold", &AssociativeMemory::setActivationThreshold, py::arg("threshold"))
        .def("getActivePatterns", &AssociativeMemory::getActivePatterns,
             py::return_value_policy::reference_internal)
        .def("clear", &AssociativeMemory::clear);

    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for expectation and error calculation)pbdoc")
        .def("updatePrediction", &PredictionSystem::updatePrediction, py::arg("sensoryInput"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("getExpectedValue", &PredictionSystem::getExpectedValue)
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &PredictionSystem::getLearningRate)
        .def("reset", &PredictionSystem::reset);

    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action sequence evaluation)pbdoc")
        .def("evaluateActionSequence", &NeuralPlanner::evaluateActionSequence, py::arg("sequence"))
        .def("planAction", &NeuralPlanner::planAction, py::arg("context"))
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getActionPreferences", &NeuralPlanner::getActionPreferences,
             py::return_value_policy::reference_internal)
        .def("updatePreferences", &NeuralPlanner::updatePreferences, py::arg("action"), py::arg("value"))
        .def("clear", &NeuralPlanner::clear);

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from neural activity patterns)pbdoc")
        .def("formConcept", &ConceptFormation::formConcept, py::arg("activityPattern"), py::arg("features"))
        .def("activateConcept", &ConceptFormation::activateConcept, py::arg("conceptId"))
        .def("getConceptFeatures", &ConceptFormation::getConceptFeatures, py::arg("conceptId"),
             py::return_value_policy::reference_internal)
        .def("getActiveConcepts", &ConceptFormation::getActiveConcepts,
             py::return_value_policy::reference_internal)
        .def("setActivationThreshold", &ConceptFormation::setActivationThreshold, py::arg("threshold"))
        .def("getSimilarityScore", [](const ConceptFormation& self, const std::vector<float>& pattern1, const std::vector<float>& pattern2) {
            return self.getSimilarityScore(pattern1, pattern2);
        }, py::arg("pattern1"), py::arg("pattern2"));

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attention mechanism for competitive selection)pbdoc")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"))
        .def("selectWinner", &AttentionalSelection::selectWinner)
        .def("updateSelectionStrength", &AttentionalSelection::updateSelectionStrength, py::arg("strength"))
        .def("getSelectionThreshold", &AttentionalSelection::getSelectionThreshold)
        .def("setSelectionThreshold", &AttentionalSelection::setSelectionThreshold, py::arg("threshold"))
        .def("getInhibitionStrength", &AttentionalSelection::getInhibitionStrength)
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"))
        .def("getExcitationStrength", &AttentionalSelection::getExcitationStrength)
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"))
        .def("getWinnerHistory", &AttentionalSelection::getWinnerHistory,
             py::return_value_policy::reference_internal)
        .def("reset", &AttentionalSelection::reset);

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for neural maturation)pbdoc")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("timeDelta"))
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("setDevelopmentalStage", &DevelopmentSystem::setDevelopmentalStage, py::arg("stage"))
        .def("getCriticalPeriodActive", &DevelopmentSystem::getCriticalPeriodActive)
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress)
        .def("enableStage", &DevelopmentSystem::enableStage, py::arg("stage"), py::arg("enable"))
        .def("getStageEnabled", &DevelopmentSystem::getStageEnabled, py::arg("stage"))
        .def("reset", &DevelopmentSystem::reset);

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulation for reward prediction error)pbdoc")
        .def("update", &Dopamine::update, py::arg("timestep"))
        .def("getLevel", &Dopamine::getLevel)
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"))
        .def("getBaseline", &Dopamine::getBaseline)
        .def("setLearningRate", &Dopamine::setLearningRate, py::arg("rate"))
        .def("getLearningRate", &Dopamine::getLearningRate)
        .def("reset", &Dopamine::reset);

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulation for exploration)pbdoc")
        .def("update", &Curiosity::update, py::arg("timestep"))
        .def("getLevel", &Curiosity::getLevel)
        .def("setBaseline", &Curiosity::setBaseline, py::arg("baseline"))
        .def("getBaseline", &Curiosity::getBaseline)
        .def("setExplorationRate", &Curiosity::setExplorationRate, py::arg("rate"))
        .def("getExplorationRate", &Curiosity::getExplorationRate)
        .def("reset", &Curiosity::reset);

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulation)pbdoc")
        .def("update", &PredictionError::update, py::arg("timestep"))
        .def("getLevel", &PredictionError::getLevel)
        .def("getErrorMagnitude", &PredictionError::getErrorMagnitude)
        .def("reset", &PredictionError::reset);

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection neuromodulation)pbdoc")
        .def("update", &Novelty::update, py::arg("timestep"))
        .def("getLevel", &Novelty::getLevel)
        .def("setThreshold", &Novelty::setThreshold, py::arg("threshold"))
        .def("getThreshold", &Novelty::getThreshold)
        .def("reset", &Novelty::reset);

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
