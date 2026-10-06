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

        .def("getPlasticityFlags", &Synapse::getPlasticityFlags)
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("reset", &Synapse::reset)
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"))
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: " + std::to_string(s.getId().index()) + " -> " + std::to_string(s.getDestinationNeuron().index()) + " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // Add BrainRegion class - organizational structure
    py::class_<BrainRegion>(m, "BrainRegion", R"pbdoc(Brain region containing neurons and synapses)pbdoc")
        .def(py::init<>())
        .def("getId", &BrainRegion::getId)
        .def("setId", &BrainRegion::setId, py::arg("id"))
        .def("getName", &BrainRegion::getName)
        .def("setName", &BrainRegion::setName, py::arg("name"))
        .def("getRegionType", &BrainRegion::getRegionType)
        .def("setRegionType", &BrainRegion::setRegionType, py::arg("type"))
        .def("addNeuron", &BrainRegion::addNeuron, py::arg("neuron"))
        .def("addSynapse", &BrainRegion::addSynapse, py::arg("synapse"))
        .def("getNeuron", &BrainRegion::getNeuron, py::arg("id"))
        .def("getSynapse", &BrainRegion::getSynapse, py::arg("id"))
        .def("getNeuronCount", &BrainRegion::getNeuronCount)
        .def("getSynapseCount", &BrainRegion::getSynapseCount)
        .def("getAllNeurons", &BrainRegion::getAllNeurons)
        .def("getAllSynapses", &BrainRegion::getAllSynapses)
        .def("getNeuronsByType", &BrainRegion::getNeuronsByType, py::arg("type"))
        .def("getSynapsesByType", &BrainRegion::getSynapsesByType, py::arg("type"))
        .def("getNeuronsInPopulation", &BrainRegion::getNeuronsInPopulation, py::arg("population"))
        .def("getSynapsesToNeuron", &BrainRegion::getSynapsesToNeuron, py::arg("neuronId"))
        .def("getSynapsesFromNeuron", &BrainRegion::getSynapsesFromNeuron, py::arg("neuronId"))
        .def("update", &BrainRegion::update, py::arg("currentTime"))
        .def("reset", &BrainRegion::reset)
        .def("removeNeuron", &BrainRegion::removeNeuron, py::arg("neuronId"))
        .def("removeSynapse", &BrainRegion::removeSynapse, py::arg("synapseId"))
        .def("clear", &BrainRegion::clear)
        .def("__repr__", [](const BrainRegion& r) {
            return "<BrainRegion: " + std::to_string(r.getId().index()) + " name=\"" + r.getName() + "\" neurons=" + std::to_string(r.getNeuronCount()) + ">";
        });

    // Add NeuralPopulation class - neuron groups with shared properties
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralPopulation::getId)
        .def("setId", &NeuralPopulation::setId, py::arg("id"))
        .def("getName", &NeuralPopulation::getName)
        .def("setName", &NeuralPopulation::setName, py::arg("name"))
        .def("getRegionId", &NeuralPopulation::getRegionId)
        .def("setRegionId", &NeuralPopulation::setRegionId, py::arg("region"))
        .def("getPopulationType", &NeuralPopulation::getPopulationType)
        .def("setPopulationType", &NeuralPopulation::setPopulationType, py::arg("type"))
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuronId"))
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("id"))
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount)
        .def("getAllNeurons", &NeuralPopulation::getAllNeurons)
        .def("getNeuronsByType", &NeuralPopulation::getNeuronsByType, py::arg("type"))
        .def("getActiveNeuronCount", &NeuralPopulation::getActiveNeuronCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getTotalSpikeCount", &NeuralPopulation::getTotalSpikeCount)
        .def("update", &NeuralPopulation::update, py::arg("currentTime"))
        .def("reset", &NeuralPopulation::reset)
        .def("clear", &NeuralPopulation::clear)
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation: " + std::to_string(p.getId().index()) + " name=\"" + p.getName() + "\" neurons=" + std::to_string(p.getNeuronCount()) + ">";
        });

    // Add PlasticityRule class - base class for plasticity rules
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Base class for plasticity rules)pbdoc")
        .def(py::init<>())
        .def("getType", &PlasticityRule::getType)
        .def("setType", &PlasticityRule::setType, py::arg("type"))
        .def("computeChange", &PlasticityRule::computeChange, py::arg("preNeuron"), py::arg("postNeuron"), py::arg("preSpikeTime"), py::arg("postSpikeTime"))
        .def("apply", &PlasticityRule::apply, py::arg("preNeuron"), py::arg("postNeuron"), py::arg("preSpikeTime"), py::arg("postSpikeTime"));

    // Add Hebbian class - Hebbian learning rule
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("getLearningRate", &Hebbian::getLearningRate)
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"))
        .def("computeChange", &Hebbian::computeChange, py::arg("weight"), py::arg("preActive"), py::arg("postActive"));

    // Add STDP class - Spike-Timing Dependent Plasticity
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-Timing Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("getLTPWeight", &STDP::getLTPWeight)
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"))
        .def("getLTDWeight", &STDP::getLTDWeight)
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"))
        .def("getTau", &STDP::getTau)
        .def("setTau", &STDP::setTau, py::arg("tau"))
        .def("computeChange", &STDP::computeChange, py::arg("preSpikeTime"), py::arg("postSpikeTime"), py::arg("currentWeight"));

    // Add StructuralPlasticity class - growth and pruning
    py::class_<StructuralPlasticity, PlasticityRule>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for growth and pruning)pbdoc")
        .def(py::init<>())
        .def("getGrowthRate", &StructuralPlasticity::getGrowthRate)
        .def("setGrowthRate", &StructuralPlasticity::setGrowthRate, py::arg("rate"))
        .def("getPruningRate", &StructuralPlasticity::getPruningRate)
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"))
        .def("shouldGrow", &StructuralPlasticity::shouldGrow, py::arg("currentTime"))
        .def("shouldPrune", &StructuralPlasticity::shouldPrune, py::arg("currentTime"));

    // Add Neuromodulator class - base class for neuromodulators
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base class for neuromodulators)pbdoc")
        .def(py::init<>())
        .def("getType", &Neuromodulator::getType)
        .def("setType", &Neuromodulator::setType, py::arg("type"))
        .def("getLevel", &Neuromodulator::getLevel)
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"))
        .def("getEffect", &Neuromodulator::getEffect)
        .def("setEffect", &Neuromodulator::setEffect, py::arg("effect"))
        .def("update", &Neuromodulator::update, py::arg("currentTime"));

    // Add Reward class - reward signals
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward signals)pbdoc")
        .def(py::init<>())
        .def("getPredictionError", &Reward::getPredictionError)
        .def("applyRewardModulation", &Reward::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"));

    // Add PredictionError class - prediction error signals
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error signals)pbdoc")
        .def(py::init<>())
        .def("getError", &PredictionError::getError)
        .def("setError", &PredictionError::setError, py::arg("error"))
        .def("getConfidence", &PredictionError::getConfidence)
        .def("setConfidence", &PredictionError::setConfidence, py::arg("confidence"));

    // Add Novelty class - novelty detection
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty detection)pbdoc")
        .def(py::init<>())
        .def("getNovelty", &Novelty::getNovelty)
        .def("setNovelty", &Novelty::setNovelty, py::arg("novelty"))
        .def("getThreshold", &Novelty::getThreshold)
        .def("setThreshold", &Novelty::setThreshold, py::arg("threshold"))
        .def("shouldDriveLearning", &Novelty::shouldDriveLearning, py::arg("currentTime"));

    // Add Curiosity class - curiosity signals
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity signals)pbdoc")
        .def(py::init<>())
        .def("getCuriosityLevel", &Curiosity::getCuriosityLevel)
        .def("getExplorationBias", &Curiosity::getExplorationBias);

    // Add Memory class - base memory class
    py::class_<Memory>(m, "Memory", R"pbdoc(Base memory class)pbdoc")
        .def(py::init<>())
        .def("getCapacity", &Memory::getCapacity)
        .def("setCapacity", &Memory::setCapacity, py::arg("capacity"))
        .def("getConsolidationRate", &Memory::getConsolidationRate)
        .def("store", &Memory::store, py::arg("key"), py::arg("value"))
        .def("retrieve", &Memory::retrieve, py::arg("key"))
        .def("update", &Memory::update, py::arg("key"), py::arg("value"));

    // Add WorkingMemory class - short-term working memory
    py::class_<WorkingMemory, Memory>(m, "WorkingMemory", R"pbdoc(Short-term working memory)pbdoc")
        .def(py::init<>())
        .def("getActiveItems", &WorkingMemory::getActiveItems)
        .def("getItem", &WorkingMemory::getItem, py::arg("itemId"))
        .def("addItem", &WorkingMemory::addItem, py::arg("itemId"), py::arg("value"))
        .def("removeItem", &WorkingMemory::removeItem, py::arg("itemId"))
        .def("updateItems", &WorkingMemory::updateItems);

    // Add EpisodicMemory class - event-based episodic memory
    py::class_<EpisodicMemory, Memory>(m, "EpisodicMemory", R"pbdoc(Event-based episodic memory)pbdoc")
        .def(py::init<>())
        .def("recordEvent", &EpisodicMemory::recordEvent, py::arg("eventId"), py::arg("context"), py::arg("timestamp"))
        .def("getEvents", &EpisodicMemory::getEvents, py::arg("startTime"), py::arg("endTime"))
        .def("getEventsByContext", &EpisodicMemory::getEventsByContext, py::arg("context"))
        .def("replayEvents", &EpisodicMemory::replayEvents, py::arg("startEvent"), py::arg("endEvent"));

    // Add SemanticMemory class - knowledge-based semantic memory
    py::class_<SemanticMemory, Memory>(m, "SemanticMemory", R"pbdoc(Knowledge-based semantic memory)pbdoc")
        .def(py::init<>())
        .def("storeConcept", &SemanticMemory::storeConcept, py::arg("conceptId"), py::arg("conceptData"))
        .def("retrieveConcept", &SemanticMemory::retrieveConcept, py::arg("conceptId"))
        .def("getRelatedConcepts", &SemanticMemory::getRelatedConcepts, py::arg("conceptId"))
        .def("getConceptNetwork", &SemanticMemory::getConceptNetwork);

    // Add DevelopmentSystem class - brain development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Brain development system)pbdoc")
        .def(py::init<>())
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage)
        .def("setDevelopmentalStage", &DevelopmentSystem::setDevelopmentalStage, py::arg("stage"))
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate)
        .def("setPlasticityRate", &DevelopmentSystem::setPlasticityRate, py::arg("rate"))
        .def("update", &DevelopmentSystem::update, py::arg("timestep"));

    // Add Synaptogenesis class - formation of synapses
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Formation of synapses)pbdoc")
        .def(py::init<>())
        .def("getGrowthRate", &Synaptogenesis::getGrowthRate)
        .def("setGrowthRate", &Synaptogenesis::setGrowthRate, py::arg("rate"))
        .def("shouldCreateConnection", &Synaptogenesis::shouldCreateConnection, py::arg("currentTime"))
        .def("createConnection", &Synaptogenesis::createConnection, py::arg("sourceNeuron"), py::arg("destinationNeuron"));

    // Add Pruning class - elimination of synapses
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Elimination of synapses)pbdoc")
        .def(py::init<>())
        .def("getPruningRate", &Pruning::getPruningRate)
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"))
        .def("shouldPruneConnection", &Pruning::shouldPruneConnection, py::arg("currentTime"))
        .def("pruneConnection", &Pruning::pruneConnection, py::arg("synapseId"));

    // Add Maturation class - neural maturation
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation)pbdoc")
        .def(py::init<>())
        .def("getMaturationRate", &Maturation::getMaturationRate)
        .def("getCurrentCapacity", &Maturation::getCurrentCapacity)
        .def("updateMaturation", &Maturation::updateMaturation, py::arg("timestep"));

    // Add ConceptFormation class - abstract concept creation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Abstract concept creation)pbdoc")
        .def(py::init<>())
        .def("processInput", &ConceptFormation::processInput, py::arg("input"))
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("getConceptAssociations", &ConceptFormation::getConceptAssociations);

    // Add NeuralPlanner class - neural-based planning
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural-based planning)pbdoc")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("maxSteps"))
        .def("evaluateActionSequence", &NeuralPlanner::evaluateActionSequence, py::arg("sequence"), py::arg("currentState"))
        .def("getActionPreferences", &NeuralPlanner::getActionPreferences, py::arg("currentState"));

    // Add MotorSystem class - motor system control
    py::class_<MotorSystem>(m, "MotorSystem", R"pbdoc(Motor system control)pbdoc")
        .def(py::init<>())
        .def("getMotorCommand", &MotorSystem::getMotorCommand)
        .def("setMotorCommand", &MotorSystem::setMotorCommand, py::arg("command"))
        .def("getMotorOutput", &MotorSystem::getMotorOutput);

    // Add PredictionSystem class - prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system)pbdoc")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predict, py::arg("input"))
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("update", &PredictionSystem::update, py::arg("prediction"), py::arg("actual"));

    // Add NeuralPrediction class - neural prediction
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction)pbdoc")
        .def(py::init<>())
        .def("getPrediction", &NeuralPrediction::getPrediction)
        .def("setPrediction", &NeuralPrediction::setPrediction, py::arg("prediction"))
        .def("getConfidence", &NeuralPrediction::getConfidence);

    // Add RandomGenerator class - random number generation
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generation)pbdoc")
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def("uniformReal", &RandomGenerator::uniformReal, py::arg("min"), py::arg("max"))
        .def("uniformInt", &RandomGenerator::uniformInt, py::arg("min"), py::arg("max"));

    // Add SimulationClock class - simulation time management
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation time management)pbdoc")
        .def(py::init<double>(), py::arg("timestep"))
        .def("update", &SimulationClock::update)
        .def("getTime", &SimulationClock::getTime)
        .def("getStep", &SimulationClock::getStep)
        .def("getStepTime", &SimulationClock::getStepTime);

    // Add Logger class - logging functionality
    py::class_<Logger>(m, "Logger", R"pbdoc(Logging functionality)pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"))
        .def("removeLogger", &Logger::removeLogger, py::arg("logger"))
        .def("log", &Logger::log, py::arg("level"), py::arg("message"));

    // Add ConsoleLogger class - console logging
    py::class_<ConsoleLogger, Logger>(m, "ConsoleLogger", R"pbdoc(Console logging)pbdoc")
        .def(py::init<>(), py::arg("level"));

    // Add EventQueue class - event processing
    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event processing queue)pbdoc")
        .def(py::init<>())
        .def("pushEvent", &EventQueue::pushEvent, py::arg("event"))
        .def("processEvents", &EventQueue::processEvents, py::arg("maxEvents"))
        .def("getEventCount", &EventQueue::getEventCount)
        .def("clearEvents", &EventQueue::clearEvents);

    // Add MemoryPool class - memory management
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Memory management)pbdoc")
        .def(py::init<>())
        .def("allocate", &MemoryPool::allocate, py::arg("size"))
        .def("deallocate", &MemoryPool::deallocate, py::arg("pointer"))
        .def("getAvailableMemory", &MemoryPool::getAvailableMemory)
        .def("getAllocatedMemory", &MemoryPool::getAllocatedMemory);

    // Add OptimizedBrain class - optimized brain implementation
    py::class_<OptimizedBrain, Brain>(m, "OptimizedBrain", R"pbdoc(Optimized brain implementation)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("optimize", &OptimizedBrain::optimize)
        .def("getPerformanceMetrics", &OptimizedBrain::getPerformanceMetrics);

    // Add VisualizationInterface class - visualization
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface)pbdoc")
        .def(py::init<>())
        .def("update", &VisualizationInterface::update, py::arg("brain"))
        .def("render", &VisualizationInterface::render)
        .def("saveVisualization", &VisualizationInterface::saveVisualization, py::arg("filepath"));

    // Add Experiment class - base experiment class
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Base experiment class)pbdoc")
        .def(py::init<>())
        .def("run", &Experiment::run)
        .def("getResults", &Experiment::getResults)
        .def("getMetrics", &Experiment::getMetrics)
        .def("reset", &Experiment::reset);

    // Add ExperimentRunner class - experiment management
    py::class_<ExperimentRunner>(m, "ExperimentRunner", R"pbdoc(Experiment management)pbdoc")
        .def(py::init<>())
        .def("runExperiment", &ExperimentRunner::runExperiment, py::arg("experiment"))
        .def("runAllExperiments", &ExperimentRunner::runAllExperiments)
        .def("getExperimentResults", &ExperimentRunner::getExperimentResults);

    // Add Metrics class - performance metrics
    py::class_<Metrics>(m, "Metrics", R"pbdoc(Performance metrics)pbdoc")
        .def(py::init<>())
        .def("recordMetric", &Metrics::recordMetric, py::arg("metricName"), py::arg("value"), py::arg("timestamp"))
        .def("getMetric", &Metrics::getMetric, py::arg("metricName"))
        .def("getAllMetrics", &Metrics::getAllMetrics)
        .def("getMetricAverage", &Metrics::getMetricAverage, py::arg("metricName"), py::arg("startTime"), py::arg("endTime"));

    // Add PerformanceProfiler class - performance profiling
    py::class_<PerformanceProfiler>(m, "PerformanceProfiler", R"pbdoc(Performance profiling)pbdoc")
        .def(py::init<>())
        .def("startProfile", &PerformanceProfiler::startProfile)
        .def("stopProfile", &PerformanceProfiler::stopProfile)
        .def("getProfileResults", &PerformanceProfiler::getProfileResults)
        .def("getProfileDuration", &PerformanceProfiler::getProfileDuration);

    // Add AblationSystem class - ablation studies
    py::class_<AblationSystem>(m, "AblationSystem", R"pbdoc(Ablation studies)pbdoc")
        .def(py::init<>())
        .def("ablateComponent", &AblationSystem::ablateComponent, py::arg("componentId"))
        .def("restoreComponent", &AblationSystem::restoreComponent, py::arg("componentId"))
        .def("getAblationResults", &AblationSystem::getAblationResults);

    // Add ScalingBenchmark class - scaling benchmarks
    py::class_<ScalingBenchmark>(m, "ScalingBenchmark", R"pbdoc(Scaling benchmarks)pbdoc")
        .def(py::init<>())
        .def("runScalingTest", &ScalingBenchmark::runScalingTest, py::arg("networkSizes"), py::arg("timesteps"))
        .def("getScalingResults", &ScalingBenchmark::getScalingResults);

    // Add SIMDVectorization class - SIMD optimizations
    py::class_<SIMDVectorization>(m, "SIMDVectorization", R"pbdoc(SIMD optimizations)pbdoc")
        .def(py::init<>())
        .def("processVector", &SIMDVectorization::processVector, py::arg("data"))
        .def("getSIMDOptimization", &SIMDVectorization::getSIMDOptimization);

    // Add SparseConnectivity class - sparse connectivity
    py::class_<SparseConnectivity>(m, "SparseConnectivity", R"pbdoc(Sparse connectivity)pbdoc")
        .def(py::init<>())
        .def("addConnection", &SparseConnectivity::addConnection, py::arg("source"), py::arg("destination"))
        .def("removeConnection", &SparseConnectivity::removeConnection, py::arg("source"), py::arg("destination"))
        .def("getConnectionStats", &SparseConnectivity::getConnectionStats);

    // Add ParallelProcessing class - parallel processing
    py::class_<ParallelProcessing>(m, "ParallelProcessing", R"pbdoc(Parallel processing)pbdoc")
        .def(py::init<>())
        .def("processInParallel", &ParallelProcessing::processInParallel, py::arg("data"), py::arg("numThreads"))
        .def("getParallelStats", &ParallelProcessing::getParallelStats);

    // Add Phase3Experiment class - Phase 3 experiments
    py::class_<Phase3Experiment, Experiment>(m, "Phase3Experiment", R"pbdoc(Phase 3 experiments)pbdoc")
        .def(py::init<>())
        .def("runBasicConnectivityTest", &Phase3Experiment::runBasicConnectivityTest)
        .def("runPlasticityExperiment", &Phase3Experiment::runPlasticityExperiment)
        .def("runStdpVerification", &Phase3Experiment::runStdpVerification);

    // Add Phase4Experiment class - Phase 4 experiments
    py::class_<Phase4Experiment, Experiment>(m, "Phase4Experiment", R"pbdoc(Phase 4 experiments)pbdoc")
        .def(py::init<>())
        .def("runPredictionTest", &Phase4Experiment::runPredictionTest)
        .def("runCognitionTest", &Phase4Experiment::runCognitionTest)
        .def("runMemoryTest", &Phase4Experiment::runMemoryTest);

    // Add Phase5Experiment class - Phase 5 experiments
    py::class_<Phase5Experiment, Experiment>(m, "Phase5Experiment", R"pbdoc(Phase 5 experiments)pbdoc")
        .def(py::init<>())
        .def("runPerformanceTest", &Phase5Experiment::runPerformanceTest)
        .def("runScalabilityTest", &Phase5Experiment::runScalabilityTest)
        .def("runEfficiencyTest", &Phase5Experiment::runEfficiencyTest);

    // Add Phase6IntegratedExperiment class - Phase 6 integrated experiment
    py::class_<Phase6IntegratedExperiment, Experiment>(m, "Phase6IntegratedExperiment", R"pbdoc(Phase 6 integrated experiment)pbdoc")
        .def(py::init<>())
        .def("runIntegrationTest", &Phase6IntegratedExperiment::runIntegrationTest)
        .def("runAllPhasesTest", &Phase6IntegratedExperiment::runAllPhasesTest)
        .def("getIntegrationResults", &Phase6IntegratedExperiment::getIntegrationResults);

    // Add additional factory functions
    m.def("createRandomConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(std::random_device()()), ConfigSource::Runtime);
        return config;
    }, "Create a random configuration");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00005f, ConfigSource::Default);
        config->set("development_enabled", true, ConfigSource::Default);
        return config;
    }, "Create a development-focused configuration");

    m.def("createLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.reward_modulated.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        return config;
    }, "Create a learning-focused configuration");

    // Version and build information
    m.def("get_version", []() {
        return "NLM Python Bindings v0.1.0 (Phase 2)";
    }, "Get the version of NLM Python bindings");

    m.def("get_build_info", []() {
        return "Built with C++20, pybind11, CMake 3.16+";
    }, "Get build information");

    // Error handling
    py::register_exception<NLMException>(m, "NLMException");

    // Constants for common values
    m.attr("DEFAULT_TEMPERATURE") = 0.001;  // 1ms timestep
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1f;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.05f;
    m.attr("DEFAULT_RANDOM_SEED") = 42;
}

    // Add Neuron class - the fundamental computational unit
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Single neuron with Leaky Integrate-and-Fire dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"))
        .def("decrementRefractory", &Neuron::decrementRefractory)
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"))
        .def("getRestingPotential", &Neuron::getRestingPotential)
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"))
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"))
        .def("step", &Neuron::step, py::arg("currentTime"))
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"))
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"))
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"))
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("getTotalCurrent", &Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent)
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"))
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"))
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses)
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags)
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getRegionId", &Neuron::getRegionId)
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"))
        .def("getPopulationId", &Neuron::getPopulationId)
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"))
        .def("reset", &Neuron::reset)
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"))
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron: " + std::to_string(n.getId().index()) + " type=" + std::to_string(static_cast<int>(n.getType())) + ">";
        });

    // Add Synapse class - neural connections with plasticity
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
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
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags)
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("reset", &Synapse::reset)
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"))
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: " + std::to_string(s.getId().index()) + " -> " + std::to_string(s.getDestinationNeuron().index()) + " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // Add BrainRegion class - organizational structure
    py::class_<BrainRegion>(m, "BrainRegion", R"pbdoc(Brain region containing neurons and synapses)pbdoc")
        .def(py::init<>())
        .def("getId", &BrainRegion::getId)
        .def("setId", &BrainRegion::setId, py::arg("id"))
        .def("getName", &BrainRegion::getName)
        .def("setName", &BrainRegion::setName, py::arg("name"))
        .def("getRegionType", &BrainRegion::getRegionType)
        .def("setRegionType", &BrainRegion::setRegionType, py::arg("type"))
        .def("addNeuron", &BrainRegion::addNeuron, py::arg("neuron"))
        .def("addSynapse", &BrainRegion::addSynapse, py::arg("synapse"))
        .def("getNeuron", &BrainRegion::getNeuron, py::arg("id"))
        .def("getSynapse", &BrainRegion::getSynapse, py::arg("id"))
        .def("getNeuronCount", &BrainRegion::getNeuronCount)
        .def("getSynapseCount", &BrainRegion::getSynapseCount)
        .def("getAllNeurons", &BrainRegion::getAllNeurons)
        .def("getAllSynapses", &BrainRegion::getAllSynapses)
        .def("getNeuronsByType", &BrainRegion::getNeuronsByType, py::arg("type"))
        .def("getSynapsesByType", &BrainRegion::getSynapsesByType, py::arg("type"))
        .def("getNeuronsInPopulation", &BrainRegion::getNeuronsInPopulation, py::arg("population"))
        .def("getSynapsesToNeuron", &BrainRegion::getSynapsesToNeuron, py::arg("neuronId"))
        .def("getSynapsesFromNeuron", &BrainRegion::getSynapsesFromNeuron, py::arg("neuronId"))
        .def("update", &BrainRegion::update, py::arg("currentTime"))
        .def("reset", &BrainRegion::reset)
        .def("removeNeuron", &BrainRegion::removeNeuron, py::arg("neuronId"))
        .def("removeSynapse", &BrainRegion::removeSynapse, py::arg("synapseId"))
        .def("clear", &BrainRegion::clear)
        .def("__repr__", [](const BrainRegion& r) {
            return "<BrainRegion: " + std::to_string(r.getId().index()) + " name=\"" + r.getName() + "\" neurons=" + std::to_string(r.getNeuronCount()) + ">";
        });

    // Add NeuralPopulation class - neuron groups with shared properties
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralPopulation::getId)
        .def("setId", &NeuralPopulation::setId, py::arg("id"))
        .def("getName", &NeuralPopulation::getName)
        .def("setName", &NeuralPopulation::setName, py::arg("name"))
        .def("getRegionId", &NeuralPopulation::getRegionId)
        .def("setRegionId", &NeuralPopulation::setRegionId, py::arg("region"))
        .def("getPopulationType", &NeuralPopulation::getPopulationType)
        .def("setPopulationType", &NeuralPopulation::setPopulationType, py::arg("type"))
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuronId"))
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("id"))
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount)
        .def("getAllNeurons", &NeuralPopulation::getAllNeurons)
        .def("getNeuronsByType", &NeuralPopulation::getNeuronsByType, py::arg("type"))
        .def("getActiveNeuronCount", &NeuralPopulation::getActiveNeuronCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getTotalSpikeCount", &NeuralPopulation::getTotalSpikeCount)
        .def("update", &NeuralPopulation::update, py::arg("currentTime"))
        .def("reset", &NeuralPopulation::reset)
        .def("clear", &NeuralPopulation::clear)
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation: " + std::to_string(p.getId().index()) + " name=\"" + p.getName() + "\" neurons=" + std::to_string(p.getNeuronCount()) + ">";
        });

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

    // Additional factory functions for convenience
    m.def("createRandomConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(std::random_device()()), ConfigSource::Runtime);
        return config;
    }, "Create a random configuration");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00005f, ConfigSource::Default);
        config->set("development_enabled", true, ConfigSource::Default);
        return config;
    }, "Create a development-focused configuration");

    m.def("createLearningConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.reward_modulated.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        return config;
    }, "Create a learning-focused configuration");

    // Add setup helper functions
    m.def("setupDefaultNetwork", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        // Default network setup - create connections between neurons
        brain->initialize();
        agent->initialize(world);
        
        // Enable basic learning
        agent->enableRewardModulation(true);
        agent->enableDevelopment(true);
        
        return "Default network setup completed";
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Setup a default neural network");

    m.def("setupDevelopmentNetwork", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        // Development network setup - more connections for development
        brain->initialize();
        agent->initialize(world);
        
        // Enable all learning mechanisms
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return "Development network setup completed";
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Setup a development-focused neural network");

    m.def("setupLearningNetwork", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        // Learning network setup - optimized for learning performance
        brain->initialize();
        agent->initialize(world);
        
        // Enable advanced learning mechanisms
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        agent->enableNoveltyDetection(true);
        
        return "Learning network setup completed";
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), "Setup a learning-focused neural network");

    // Add utility functions
    m.def("get_version", []() {
        return "NLM Python Bindings v0.1.0 (Phase 2)";
    }, "Get the version of NLM Python bindings");

    m.def("get_build_info", []() {
        return "Built with C++20, pybind11, CMake 3.16+";
    }, "Get build information");

    m.def("get_supported_features", []() {
        std::vector<std::string> features = {
            "Neuron - LIF dynamics",
            "Synapse - Synaptic transmission",
            "BrainRegion - Brain organization",
            "NeuralPopulation - Neuron groups",
            "PlasticityRule - Learning mechanisms",
            "Hebbian - Hebbian learning",
            "STDP - Spike-timing dependent plasticity",
            "StructuralPlasticity - Growth and pruning",
            "Neuromodulator - Neuromodulation",
            "Reward - Reward signals",
            "PredictionError - Prediction errors",
            "Novelty - Novelty detection",
            "Curiosity - Curiosity signals",
            "Memory - Memory systems",
            "WorkingMemory - Short-term memory",
            "EpisodicMemory - Event memory",
            "SemanticMemory - Semantic knowledge",
            "Development - Brain development",
            "Synaptogenesis - Synapse formation",
            "Pruning - Synapse elimination",
            "Maturation - Neural maturation",
            "ConceptFormation - Abstract concepts",
            "NeuralPlanner - Neural planning",
            "MotorSystem - Motor control",
            "Action - Motor actions",
            "PredictionSystem - Prediction",
            "NeuralPrediction - Neural prediction",
            "RandomGenerator - Random numbers",
            "SimulationClock - Time management",
            "Logger - Logging",
            "ConsoleLogger - Console logging",
            "EventQueue - Event processing",
            "MemoryPool - Memory management",
            "OptimizedBrain - Optimized brain",
            "VisualizationInterface - Visualization",
            "Experiment - Base experiment",
            "ExperimentRunner - Experiment management",
            "Metrics - Performance metrics",
            "PerformanceProfiler - Performance profiling",
            "AblationSystem - Ablation studies",
            "ScalingBenchmark - Scaling benchmarks",
            "SIMDVectorization - SIMD optimizations",
            "SparseConnectivity - Sparse connectivity",
            "ParallelProcessing - Parallel processing",
            "Phase3Experiment - Phase 3 experiments",
            "Phase4Experiment - Phase 4 experiments",
            "Phase5Experiment - Phase 5 experiments",
            "Phase6IntegratedExperiment - Phase 6 experiments",
            "ConvenienceFunctions - Setup helpers",
            "ErrorHandling - Exception hierarchy",
            "UtilityFunctions - Helper functions"
        };
        return features;
    }, "Get list of supported features");

    // Error handling
    py::register_exception<NLMException>(m, "NLMException");

    // Constants for common values
    m.attr("DEFAULT_TEMPERATURE") = 0.001;  // 1ms timestep
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_SYNAPSE_DENSITY") = 0.1f;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.05f;
    m.attr("DEFAULT_RANDOM_SEED") = 42;
}

} // namespace nlm
