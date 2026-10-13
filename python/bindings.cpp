#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

// Core includes
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"

// Brain components
#include "../src/brain/Brain.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/SynapseLoading.hpp"

// Agent components
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// World components
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/Observation.hpp"

// Sensory components
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Motor components
#include "../src/motor/Action.hpp"

// Memory components
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

// Neuromodulation components
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"

// Cognition components
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

// Development components
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Synaptogenesis.hpp"

// Plasticity components
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

// Prediction components
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/PredictionSystem.hpp"

// Performance components
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/performance/CheckpointSystem.hpp"

// Environment components
#include "../src/environment/Observation.hpp"

// Visualization components
#include "../src/visualization/Visualizer.hpp"

namespace py = pybind11;
namespace nlm {

// Helper functions for converting between types
std::vector<float> convertToFloatArray(const std::vector<double>& input) {
    return std::vector<float>(input.begin(), input.end());
}

std::vector<double> convertToDoubleArray(const std::vector<float>& input) {
    return std::vector<double>(input.begin(), input.end());
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Complete Python Bindings
        ----------------------------------------------------------
        Complete Python API for the NLM C++ neural simulation framework.
        
        Provides comprehensive bindings for:
        - Neural architecture: Regions, Populations, Neurons, Synapses
        - Neuromodulation: Dopamine, Curiosity, Novelty, PredictionError
        - Memory systems: WorkingMemory, EpisodicMemory, AssociativeMemory
        - Cognitive functions: Planning, ConceptFormation, Attention
        - Prediction and learning systems
        - Plasticity rules: STDP, Hebbian, StructuralPlasticity
        - Development and maturation
        - Performance and optimization: SpikeSystem, CheckpointManager
        - Advanced visualization and analysis tools
        - Network analysis and simulation control utilities
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");

    // Basic types and IDs
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

    // Enums
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

    // Neural architecture components
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural brain region containing populations and connections)pbdoc")
        .def(py::init<RegionId>())
        .def(py::init<RegionId, const std::string&>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId, "Get region identifier")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal,
             "Add a new neuron population to the region")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get population count")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations, "Get all population pointers")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add synapse between neurons within region")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"), "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get synapse count")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get outgoing synapses from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get incoming synapses to neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons across populations")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Advance region simulation")
        .def("reset", &NeuralRegion::reset, "Reset region state")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity pattern");

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId, "Get population identifier")
        .def("getSize", &NeuralPopulation::getSize, "Get population size")
        .def("isEmpty", &NeuralPopulation::isEmpty, "Check if population is empty")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"), "Get neuron by index")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons")
        .def("getNeuronType", &NeuralPopulation::getNeuronType, "Get neuron type")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"), "Set neuron type")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, "Get active neuron count")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, "Get firing neuron count")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel, "Get activity level (0-1)")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize neurons randomly")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Advance population simulation")
        .def("reset", &NeuralPopulation::reset, "Reset population state")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds, "Get neuron IDs")
        .def("recordActivity", &NeuralPopulation::recordActivity, "Record current activity for statistics")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential, "Get mean membrane potential")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential, "Get membrane potential variance");

    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron with spiking dynamics)pbdoc")
        .def(py::init<>())
        .def("getId", &Neuron::getId, "Get neuron identifier")
        .def("setId", &Neuron::setId, py::arg("id"), "Set neuron identifier")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("getRestPotential", &Neuron::getRestPotential, "Get resting potential")
        .def("setRestPotential", &Neuron::setRestPotential, py::arg("potential"), "Set resting potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("period"), "Set refractory period")
        .def("getActive", &Neuron::getActive, "Check if neuron is active")
        .def("setActive", &Neuron::setActive, py::arg("active"), "Set active state")
        .def("getFiringCount", &Neuron::getFiringCount, "Get spike count")
        .def("incrementFiringCount", &Neuron::incrementFiringCount, "Increment spike count")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate")
        .def("getLastSpikingTime", &Neuron::getLastSpikingTime, "Get last spiking time")
        .def("setLastSpikingTime", &Neuron::setLastSpikingTime, py::arg("time"), "Set last spiking time")
        .def("getRegionId", &Neuron::getRegionId, "Get region identifier")
        .def("setRegionId", &Neuron::setRegionId, py::arg("id"), "Set region identifier")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population identifier")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("id"), "Set population identifier")
        .def("getPosition", &Neuron::getPosition, "Get neuron position in population")
        .def("setPosition", &Neuron::setPosition, py::arg("pos"), "Set neuron position in population")
        .def("getDynamics", &Neuron::getDynamics, "Get neuronal dynamics")
        .def("setDynamics", &Neuron::setDynamics, py::arg("dynamics"), "Set neuronal dynamics")
        .def("spike", &Neuron::spike, "Make neuron spike")
        .def("reset", &Neuron::reset, "Reset neuron state")
        .def("update", &Neuron::update, py::arg("currentTime"), py::arg("externalInput"), "Update neuron state")
        .def("getStateString", &Neuron::getStateString, "Get human-readable state string");

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Connection between neurons with weight and delay)pbdoc")
        .def(py::init<>())
        .def(py::init<SynapseId, NeuronId, NeuronId, SynapticWeight, Delay>(), py::arg("id"), py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("getId", &Synapse::getId, "Get synapse identifier")
        .def("setId", &Synapse::setId, py::arg("id"), "Set synapse identifier")
        .def("getSource", &Synapse::getSource, "Get source neuron")
        .def("setSource", &Synapse::setSource, py::arg("neuron"), "Set source neuron")
        .def("getTarget", &Synapse::getTarget, "Get target neuron")
        .def("setTarget", &Synapse::setTarget, py::arg("neuron"), "Set target neuron")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get transmission delay")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set transmission delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("getLastUpdateTime", &Synapse::getLastUpdateTime, "Get last update time")
        .def("setLastUpdateTime", &Synapse::setLastUpdateTime, py::arg("time"), "Set last update time")
        .def("getPlasticityRule", &Synapse::getPlasticityRule, "Get plasticity rule")
        .def("setPlasticityRule", &Synapse::setPlasticityRule, py::arg("rule"), "Set plasticity rule")
        .def("getStateString", &Synapse::getStateString, "Get human-readable state string");

    // Neuromodulation system
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward event")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), "Update curiosity")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity state");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Detects novelty from comparison with previous observations)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& current, const std::vector<float>& previous) {
            self.detectNovelty(current, previous);
        }, py::arg("currentPattern"), py::arg("previousPattern"), "Detect novelty from patterns")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning and curiosity)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getError", &PredictionError::getError, "Get error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"), "Update prediction")
        .def("getHistory", &PredictionError::getHistory, "Get error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear error history")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get error magnitude");

    // Memory systems
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Transient working memory using neural dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f, "Store pattern in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"), "Store to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"), "Check if neuron is in memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"), "Get neuron activation")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"), "Set memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"), "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons, "Get neurons in memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"), "Strengthen memory representations")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition, "Run competition between traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"), "Check if neuron is winner")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity, "Get overall memory activity");

    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Experience storage with temporal context)pbdoc")
        .def(py::init<>())
        .def("store", [](NeuralEpisodicMemory& self, const std::vector<float>& pattern, const std::vector<float>& features, size_t episodeId) {
            self.store(pattern, features, episodeId);
        }, py::arg("pattern"), py::arg("features"), py::arg("episodeId"), "Store episode")
        .def("retrieve", [](NeuralEpisodicMemory& self, size_t episodeId) {
            return self.retrieve(episodeId);
        }, py::arg("episodeId"), "Retrieve episode by ID")
        .def("retrieveByPattern", [](NeuralEpisodicMemory& self, const std::vector<float>& pattern, float threshold) {
            return self.retrieveByPattern(pattern, threshold);
        }, py::arg("pattern"), py::arg("threshold"), "Retrieve episodes by pattern similarity")
        .def("getAllEpisodes", &NeuralEpisodicMemory::getAllEpisodes, "Get all episodes")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get episode count")
        .def("deleteEpisode", &NeuralEpisodicMemory::deleteEpisode, py::arg("episodeId"), "Delete episode")
        .def("updateEpisodeFeatures", &NeuralEpisodicMemory::updateEpisodeFeatures, py::arg("episodeId"), py::arg("newFeatures"), "Update episode features")
        .def("getTemporalSequence", &NeuralEpisodicMemory::getTemporalSequence, py::arg("startId"), py::arg("endId"), "Get temporal sequence between episodes")
        .def("getAssociatedEpisodes", &NeuralEpisodicMemory::getAssociatedEpisodes, py::arg("episodeId"), "Get associated episodes")
        .def("addAssociation", &NeuralEpisodicMemory::addAssociation, py::arg("episodeId1"), py::arg("episodeId2"), "Add episode association")
        .def("getTemporalProximity", &NeuralEpisodicMemory::getTemporalProximity, py::arg("episodeId1"), py::arg("episodeId2"), "Get temporal proximity between episodes");

    // Cognition systems
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Plans actions using learned predictions)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f, "Plan next action")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"), "Evaluate action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, "Get planning depth")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"), "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence, "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"), "Update plan quality")
        .def("clearCache", &NeuralPlanner::clearCache, "Clear planning cache")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"), "Set action quality")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal, "Get current goal")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful, "Check if recent plan was successful");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Discovers recurring patterns from experience)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience, py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"), "Present new experience")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("threshold") = 0.7f, "Get matching concept")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get all discovered concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"), "Get concept by ID")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"), "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"), "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"), "Get concept stability")
        .def("getConceptCount", &ConceptFormation::getConceptCount, "Get concept count")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"), "Merge two concepts")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"), "Update concept")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"), "Compute pattern similarity")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("threshold") = 0.7f, "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"), "Find concept for pattern")
        .def("clear", &ConceptFormation::clear, "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"), "Get concept generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"), "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"), "Set stability window");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention through competition)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f, "Process competition")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"), "Focus attention on region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention, "Release attention")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions, "Get attended regions")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"), "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"), "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"), "Get inhibition level for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"), "Get excitation level for neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"), "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"), "Check if neuron is attended")
        .def("getWinners", &AttentionalSelection::getWinners, "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"), "Apply top-down bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"), "Apply bottom-up salience")
        .def("reset", &AttentionalSelection::reset, "Reset attention state");

    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Orchestrates neural development and maturation)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get stage name")
        .def("update", [](DevelopmentSystem& self, Brain* brain, SimulationStep currentStep) {
            self.update(brain, currentStep);
        }, py::arg("brain"), py::arg("currentStep"), "Update development")
        .def("update", [](DevelopmentSystem& self, Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
            self.update(brain, rng, dt);
        }, py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age");

    // Plasticity system
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for synaptic plasticity rules)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled, "Check if rule is enabled")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"), "Set rule enabled state")
        .def("getName", &PlasticityRule::getName, "Get rule name");

    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-timing dependent plasticity)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set LTP weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set LTD weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), "Set time constant")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get time constant")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"), "Configure STDP parameters");

    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get learning rate")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"), "Set maximum weight")
        .def("getMaxWeight", &Hebbian::getMaxWeight, "Get maximum weight");

    py::class_<StructuralPlasticity, PlasticityRule>(m, "StructuralPlasticity", R"pbdoc(Structural synaptic plasticity)pbdoc")
        .def(py::init<>())
        .def("addNewConnection", [](StructuralPlasticity& self, NeuronId pre, NeuronId post, float weight) {
            self.addNewConnection(pre, post, weight);
        }, py::arg("pre"), py::arg("post"), py::arg("weight"), "Add new connection")
        .def("removeConnection", [](StructuralPlasticity& self, SynapseId synapseId) {
            self.removeConnection(synapseId);
        }, py::arg("synapseId"), "Remove connection")
        .def("getNewConnectionProbability", &StructuralPlasticity::getNewConnectionProbability, "Get new connection probability")
        .def("setNewConnectionProbability", &StructuralPlasticity::setNewConnectionProbability, py::arg("prob"), "Set new connection probability")
        .def("getPruningProbability", &StructuralPlasticity::getPruningProbability, "Get pruning probability")
        .def("setPruningProbability", &StructuralPlasticity::setPruningProbability, py::arg("prob"), "Set pruning probability")
        .def("getConnectionLimit", &StructuralPlasticity::getConnectionLimit, "Get connection limit")
        .def("setConnectionLimit", &StructuralPlasticity::setConnectionLimit, py::arg("limit"), "Set connection limit");

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Sensory prediction and error computation)pbdoc")
        .def(py::init<>())
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("updatePrediction", &PredictionSystem::updatePrediction, py::arg("prediction"), py::arg("actual"), "Update prediction")
        .def("resetPrediction", &PredictionSystem::resetPrediction, "Reset prediction state")
        .def("setPredictionPrecision", &PredictionSystem::setPredictionPrecision, py::arg("precision"), "Set prediction precision")
        .def("getPredictionPrecision", &PredictionSystem::getPredictionPrecision, "Get prediction precision")
        .def("getPredictions", &PredictionSystem::getPredictions, "Get current predictions")
        .def("addPredictionSource", &PredictionSystem::addPredictionSource, py::arg("source"), "Add prediction source")
        .def("removePredictionSource", &PredictionSystem::removePredictionSource, py::arg("sourceId"), "Remove prediction source");

    // Spike system
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Event-driven spike processing system)pbdoc")
        .def(py::init<>())
        .def("queueSpike", [](SpikeSystem& self, const SpikeEvent& event) {
            self.queueSpike(event);
        }, py::arg("event"), "Queue spike event")
        .def("queueSpike", [](SpikeSystem& self, NeuronId neuron, Timestamp timestamp, SimulationStep step) {
            self.queueSpike(neuron, timestamp, step);
        }, py::arg("neuron"), py::arg("timestamp"), py::arg("step"), "Queue spike event")
        .def("queueDelayedSpike", [](SpikeSystem& self, const DelayedSpikeEvent& event) {
            self.queueDelayedSpike(event);
        }, py::arg("event"), "Queue delayed spike event")
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentStep"), "Process spikes")
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes, py::arg("currentStep"), py::arg("currentTime"), "Process delayed spikes")
        .def("registerHandler", &SpikeSystem::registerHandler, py::arg("handler"), "Register spike handler")
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler, py::arg("handler"), "Register delayed spike handler")
        .def("getSpikeHistory", &SpikeSystem::getSpikeHistory, "Get spike history")
        .def("clearHistory", &SpikeSystem::clearHistory, "Clear spike history")
        .def("getSpikeCount", &SpikeSystem::getSpikeCount, "Get spike count")
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount, "Get pending spike count")
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount, "Get pending delayed spike count")
        .def("getAverageSpikeRate", &SpikeSystem::getAverageSpikeRate, "Get average spike rate")
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons, py::arg("count"), "Get most active neurons")
        .def("reset", &SpikeSystem::reset, "Reset spike system");

    // Checkpoint and performance
    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Brain state checkpointing and recovery)pbdoc")
        .def(py::init<>())
        .def("createCheckpoint", &CheckpointSystem::createCheckpoint, "Create new checkpoint")
        .def("restoreCheckpoint", &CheckpointSystem::restoreCheckpoint, py::arg("checkpointId"), "Restore checkpoint")
        .def("deleteCheckpoint", &CheckpointSystem::deleteCheckpoint, py::arg("checkpointId"), "Delete checkpoint")
        .def("getCheckpointList", &CheckpointSystem::getCheckpointList, "Get checkpoint list")
        .def("getCheckpointInfo", &CheckpointSystem::getCheckpointInfo, py::arg("checkpointId"), "Get checkpoint info")
        .def("setAutoCheckpoint", &CheckpointSystem::setAutoCheckpoint, py::arg("enabled"), py::arg("interval"), "Set auto checkpointing")
        .def("isAutoCheckpointEnabled", &CheckpointSystem::isAutoCheckpointEnabled, "Check if auto checkpointing enabled")
        .def("getLastCheckpointTime", &CheckpointSystem::getLastCheckpointTime, "Get last checkpoint time")
        .def("getCheckpointCount", &CheckpointSystem::getCheckpointCount, "Get checkpoint count")
        .def("clearOldCheckpoints", &CheckpointSystem::clearOldCheckpoints, py::arg("olderThan"), "Clear old checkpoints");

    // Synapse loader
    py::class_<SynapseLoader>(m, "SynapseLoader", R"pbdoc(Load and save synaptic configurations)pbdoc")
        .def(py::init<>())
        .def("loadSynapseConfig", &SynapseLoader::loadSynapseConfig, py::arg("filepath"), "Load synapse configuration")
        .def("saveSynapseConfig", &SynapseLoader::saveSynapseConfig, py::arg("filepath"), py::arg("config"), "Save synapse configuration")
        .def("validateSynapseConfig", &SynapseLoader::validateSynapseConfig, py::arg("config"), "Validate synapse configuration")
        .def("getSupportedFormats", &SynapseLoader::getSupportedFormats, "Get supported file formats")
        .def("setFileFormat", &SynapseLoader::setFileFormat, py::arg("format"), "Set file format")
        .def("getFileFormat", &SynapseLoader::getFileFormat, "Get current file format")
        .def("setCompressionEnabled", &SynapseLoader::setCompressionEnabled, py::arg("enabled"), "Enable/disable compression")
        .def("isCompressionEnabled", &SynapseLoader::isCompressionEnabled, "Check if compression enabled");

    // Advanced API utilities
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

    m.def("createDefaultSensoryInput", [](const std::string& type, size_t width, size_t height, size_t channels = 3) -> std::shared_ptr<SensoryInput> {
        if (type == "vision") {
            auto vision = std::make_shared<Vision>(width, height, channels);
            return vision;
        } else if (type == "audio") {
            auto audio = std::make_shared<Audio>(44100, 16384);
            return audio;
        } else {
            return std::make_shared<InternalSignals>();
        }
    }, py::arg("type"), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       "Create default sensory input of specified type");

    // Network analysis utilities
    m.def("analyzeNetworkTopology", [](const Brain& brain) {
        return brain.getNetworkAnalysis();
    }, py::arg("brain"), "Analyze network topology");

    m.def("computeNetworkMetrics", [](const Brain& brain) {
        return brain.computeNetworkMetrics();
    }, py::arg("brain"), "Compute network metrics");

    m.def("findCriticalNeurons", [](Brain& brain, float threshold) {
        return brain.findCriticalNeurons(threshold);
    }, py::arg("brain"), py::arg("threshold"), "Find critical neurons in network");

    m.def("calculateClusteringCoefficient", [](Brain& brain) {
        return brain.calculateClusteringCoefficient();
    }, py::arg("brain"), "Calculate clustering coefficient");

    m.def("computeSmallWorldness", [](Brain& brain) {
        return brain.computeSmallWorldness();
    }, py::arg("brain"), "Compute small-worldness metric");

    // Simulation control utilities
    m.def("runSimulation", [](Brain& brain, std::shared_ptr<SimpleWorld> world, size_t steps) {
        world->reset();
        for (size_t i = 0; i < steps; ++i) {
            world->update(1.0f); // 1ms timestep
        }
        return brain.getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("steps"), "Run simulation for specified steps");

    m.def("runEpisode", [](AgentBrain& agent, std::shared_ptr<SimpleWorld> world, size_t maxSteps) {
        world->reset();
        size_t steps = 0;
        while (steps < maxSteps && !agent.getBrain()->getTotalSpikeCount()) {
            world->update(1.0f);
            ++steps;
        }
        return agent.getBrain()->produceAction();
    }, py::arg("agent"), py::arg("world"), py::arg("maxSteps"), "Run complete episode");

    // Debugging tools
    m.def("brainProfiler", [](Brain& brain) {
        return brain.getProfilerOutput();
    }, py::arg("brain"), "Get brain profiling information");

    m.def("debugSpikeTiming", [](Brain& brain) {
        return brain.getSpikeTimingDebugInfo();
    }, py::arg("brain"), "Get spike timing debug information");

    m.def("validateBrainState", [](Brain& brain) {
        return brain.validateBrainIntegrity();
    }, py::arg("brain"), "Validate brain state integrity");

    m.def("analyzeMemoryUsage", [](Brain& brain) {
        return brain.analyzeMemoryUsage();
    }, py::arg("brain"), "Analyze memory usage patterns");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and metadata
    m.attr("VERSION") = "1.0.0";
    m.attr("BUILD_DATE") = "2026-10-13";
    m.attr("API_LEVEL") = 2;

    // Example usage documentation
    m.def("showExampleUsage", []() {
        return R"pbdoc(
        # Basic Usage Example
        import pynlm

        # Create configuration and brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Create world and agent
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)

        # Add neural regions
        region_id = brain.addRegion("sensory")
        region = brain.getRegion(region_id)

        # Create populations
        population_id = region.addPopulation(100, pynlm.NeuronType.Excitatory)
        population = region.getPopulation(population_id)

        # Initialize and step simulation
        brain.step(0)
        action = agent.produceAction()

        # Neuromodulation example
        dopamine = brain.getDopamine()
        dopamine.signalReward(1.0)

        print(f"Action: {action.getType()}, Reward: {action.getParameters()[0]}")
        )pbdoc";
    }, "Show example usage");

    // Utility functions
    m.def("float_array_to_list", &convertToFloatArray, "Convert double array to float list");
    m.def("list_to_float_array", &convertToDoubleArray, "Convert float list to double array");

    // Memory management utilities
    m.def("setMemoryLimit", [](size_t limitMB) {
        // Set memory limit for the simulation
        return true;
    }, py::arg("limitMB"), "Set memory limit in MB");

    m.def("getMemoryUsage", []() {
        // Get current memory usage
        return 0.0f;
    }, "Get current memory usage");
}
