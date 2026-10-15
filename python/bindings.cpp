#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

// Core types and configurations
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"

// Brain components
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Brain.hpp"

// Agent systems
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Sensory systems
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Motor systems
#include "../src/motor/Action.hpp"
#include "../src/motor/MotorSystem.hpp"

// Plasticity systems
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/plasticity/RewardModulatedSTDP.hpp"
#include "../src/plasticity/TripletSTDP.hpp"

// Neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"

// Memory systems
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

// Prediction systems
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/PredictionSystem.hpp"

// Cognition systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"

// Development systems
#include "../src/development/DevelopmentSystem.hpp"

// World systems
#include "../src/world/SimpleWorld.hpp"
#include "../src/world/WorldObject.hpp"

// Environment
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Core Python Bindings
        ---------------------------------------------
        Python bindings for the NLM C++ neural simulation framework.
        
        Exposes all core NLM functionality:
        - Brain and neural system management
        - Memory systems (working, episodic)
        - Plasticity rules (STDP, Hebbian, structural)
        - Neuromodulation (dopamine, curiosity, novelty, reward)
        - Prediction system
        - Cognition systems (planning, concept formation)
        - Development system
        - Sensory and motor interfaces
        - World simulation and environment
        
        Note: This is the core bindings. Advanced features like performance
        monitoring, real-time control, and visualization are available in
        the nlm_python module for enhanced functionality.
        
        Usage: import nlm; brain = nlm.Brain(config)
    )pbdoc";

    // Register exception handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::logic_error>(m, "LogicError");

    // ============================================================================
    // Core Types and Identifiers
    // ============================================================================

    // Neuron identifiers
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

    // Synapse identifiers
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

    // Region identifiers
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

    // Population identifiers
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Memory identifiers
    py::class_<MemoryId>(m, "MemoryId", R"pbdoc(Unique identifier for a memory item)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &MemoryId::value)
        .def("index", &MemoryId::index)
        .def("__eq__", &MemoryId::operator==)
        .def("__ne__", &MemoryId::operator!=);

    // Error components
    py::class_<PredictionErrorSignal::ErrorComponents>(m, "ErrorComponents")
        .def_readwrite("intensityError", &PredictionErrorSignal::ErrorComponents::intensityError)
        .def_readwrite("spatialError", &PredictionErrorSignal::ErrorComponents::spatialError)
        .def_readwrite("temporalError", &PredictionErrorSignal::ErrorComponents::temporalError)
        .def_readwrite("totalError", &PredictionErrorSignal::ErrorComponents::totalError);

    // ============================================================================
    // Enumeration Types
    // ============================================================================

    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing states
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action types
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

    // Motor commands
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

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Plasticity flags
    py::enum_<PlasticityFlags>(m, "PlasticityFlags", R"pbdoc(Plasticity configuration flags)pbdoc")
        .value("Hebbian", PlasticityFlags::Hebbian)
        .value("STDP", PlasticityFlags::STDP)
        .value("RewardModulated", PlasticityFlags::RewardModulated)
        .value("Structural", PlasticityFlags::Structural)
        .export_values();

    // ============================================================================
    // Configuration System
    // ============================================================================

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

    // ============================================================================
    // Brain Components
    // ============================================================================

    // Neuron class
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron with LIF dynamics)pbdoc")
        .def(py::init<NeuronId>())
        .def(py::init<NeuronId, NeuronType>(), py::arg("id"), py::arg("type"))
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("getFiringState", &Neuron::getFiringState, "Get firing state")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is refractory")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate")
        .def("checkThreshold", &Neuron::checkThreshold, "Check if neuron has crossed threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get last spike time")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record a spike")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("getState", &Neuron::getState, py::return_value_policy::copy, "Get neuron state")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject current")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear total current")
        .def("getExcitationInput", &Neuron::getExcitationInput, "Get excitation input")
        .def("getInhibitionInput", &Neuron::getInhibitionInput, "Get inhibition input")
        .def("getModulationInput", &Neuron::getModulationInput, "Get modulation input")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("synapse"), "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("synapse"), "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, "Get incoming synapses")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, "Get outgoing synapses")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable plasticity mechanisms")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, "Get plasticity flags")
        .def("step", &Neuron::step, py::arg("currentTime"), "Update neuron for one simulation step")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "LIF neuron step function")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("logStatus", &Neuron::logStatus, "Log neuron status");

    // Synapse class
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Connection between neurons)pbdoc")
        .def(py::init<SynapseId>())
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("target"))
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSource", &Synapse::getSource, "Get source neuron ID")
        .def("getTarget", &Synapse::getTarget, "Get target neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("getDelay", &Synapse::getDelay, "Get transmission delay")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set transmission delay")
        .def("getMaxWeight", &Synapse::getMaxWeight, "Get maximum weight")
        .def("setMaxWeight", &Synapse::setMaxWeight, py::arg("maxWeight"), "Set maximum weight")
        .def("getMinWeight", &Synapse::getMinWeight, "Get minimum weight")
        .def("setMinWeight", &Synapse::setMinWeight, py::arg("minWeight"), "Set minimum weight")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("setPlasticityFlags", &Synapse::setPlasticityFlags, py::arg("flags"), "Set plasticity flags")
        .def("getLastPreSpike", &Synapse::getLastPreSpike, "Get last presynaptic spike time")
        .def("getLastPostSpike", &Synapse::getLastPostSpike, "Get last postsynaptic spike time")
        .def("getTotalCurrent", &Synapse::getTotalCurrent, "Get total current")
        .def("getExcitationContribution", &Synapse::getExcitationContribution, "Get excitation contribution")
        .def("getInhibitionContribution", &Synapse::getInhibitionContribution, "Get inhibition contribution")
        .def("getModulationContribution", &Synapse::getModulationContribution, "Get modulation contribution")
        .def("updatePlasticity", &Synapse::updatePlasticity, py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"), "Update synaptic plasticity")
        .def("applyWeightChange", &Synapse::applyWeightChange, py::arg("delta"), "Apply weight change")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("logStatus", &Synapse::logStatus, "Log synapse status");

    // NeuralRegion class
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region containing neurons)pbdoc")
        .def(py::init<RegionId>())
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("getRegionType", &NeuralRegion::getRegionType, "Get region type")
        .def("setRegionType", &NeuralRegion::setRegionType, py::arg("type"), "Set region type")
        .def("getNeuronCount", &NeuralRegion::getNeuronCount, "Get neuron count")
        .def("getRegionCapacity", &NeuralRegion::getRegionCapacity, "Get region capacity")
        .def("setRegionCapacity", &NeuralRegion::setRegionCapacity, py::arg("capacity"), "Set region capacity")
        .def("addNeuron", &NeuralRegion::addNeuron, py::arg("neuron"), "Add neuron to region")
        .def("removeNeuron", &NeuralRegion::removeNeuron, py::arg("neuronId"), "Remove neuron from region")
        .def("getNeuron", &NeuralRegion::getNeuron, py::arg("neuronId"), "Get neuron by ID")
        .def("hasNeuron", &NeuralRegion::hasNeuron, py::arg("neuronId"), "Check if region has neuron")
        .def("getNeurons", &NeuralRegion::getNeurons, "Get all neurons in region")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("synapse"), "Add synapse to region")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses in region")
        .def("getExcitationInhibitionRatio", &NeuralRegion::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalFiringRate", &NeuralRegion::getTotalFiringRate, "Get total firing rate")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get firing neuron count")
        .def("getRegionStatistics", &NeuralRegion::getRegionStatistics, "Get region statistics")
        .def("reset", &NeuralRegion::reset, "Reset region to initial state")
        .def("logStatus", &NeuralRegion::logStatus, "Log region status");

    // NeuralPopulation class
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of similar neurons)pbdoc")
        .def(py::init<PopulationId>())
        .def(py::init<PopulationId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("getName", &NeuralPopulation::getName, "Get population name")
        .def("setName", &NeuralPopulation::setName, py::arg("name"), "Set population name")
        .def("getPopulationType", &NeuralPopulation::getPopulationType, "Get population type")
        .def("setPopulationType", &NeuralPopulation::setPopulationType, py::arg("type"), "Set population type")
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount, "Get neuron count")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("neuronId"), "Get neuron by ID")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons in population")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("getFiringRateStandardDeviation", &NeuralPopulation::getFiringRateStandardDeviation, "Get firing rate standard deviation")
        .def("getRegion", &NeuralPopulation::getRegion, "Get parent region")
        .def("setRegion", &NeuralPopulation::setRegion, py::arg("region"), "Set parent region")
        .def("logStatus", &NeuralPopulation::logStatus, "Log population status");

    // ============================================================================
    // Memory Systems
    // ============================================================================

    // WorkingMemory class
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Short-term active information storage)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store item")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"), "Retrieve item")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"), "Check if item exists")
        .def("clear", &WorkingMemory::clear, "Clear all items")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"), "Decay all items")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current size")
        .def("getUtilization", &WorkingMemory::getUtilization, "Get utilization")
        .def("logStatus", &WorkingMemory::logStatus, "Log working memory status");

    // EpisodicMemory class
    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Experience storage and replay)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("item"), "Store episode")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"), "Retrieve episode")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount, "Get episode count")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("clear", &EpisodicMemory::clear, "Clear all episodes")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Consolidate episodes")
        .def("replayMemory", &EpisodicMemory::replayMemory, "Replay memory")
        .def("getMemoryStatistics", &EpisodicMemory::getMemoryStatistics, "Get memory statistics")
        .def("logStatus", &EpisodicMemory::logStatus, "Log episodic memory status");

    // ============================================================================
    // Plasticity Systems
    // ============================================================================

    // PlasticityRule base class
    py::class_<PlasticityRule>(m, "PlasticityRule", py::module_local(),
        R"pbdoc(Abstract base class for plasticity rules)pbdoc")
        .def("update", &PlasticityRule::update,
              py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
              "Update synaptic weights")
        .def("applyWeightChange", &PlasticityRule::applyWeightChange,
              py::arg("synapse"), py::arg("delta"),
              "Apply weight change to synapse")
        .def("getName", &PlasticityRule::getName, "Get rule name")
        .def("reset", &PlasticityRule::reset, "Reset rule to initial state");

    // STDP class
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("configure", &STDP::configure,
              py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
              "Configure basic STDP parameters")
        .def("configureAdvanced", &STDP::configureAdvanced,
              py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
              py::arg("backpropSpeed"), py::arg("nmdaProbability"), py::arg("calciumDecayRate"),
              py::arg("facilitationTau"), py::arg("depressionTau"), py::arg("eligibilityDecay"),
              py::arg("neuromodulatorBaseline"), py::arg("neuromodulatorSensitivity"),
              py::arg("metaplasticityThreshold"), py::arg("activityHistoryWeight"),
              "Configure advanced STDP parameters")
        .def("configureStandard", &STDP::configureStandard, "Configure standard STDP")
        .def("configureFast", &STDP::configureFast, "Configure fast STDP")
        .def("configureSlow", &STDP::configureSlow, "Configure slow STDP")
        .def("configureRewardSensitive", &STDP::configureRewardSensitive, "Configure reward-sensitive STDP")
        .def("enableTripletSTDP", &STDP::enableTripletSTDP, py::arg("enable"), "Enable triplet STDP")
        .def("enableCalciumDynamics", &STDP::enableCalciumDynamics, py::arg("enable"), "Enable calcium dynamics")
        .def("enableNMDAReceptor", &STDP::enableNMDAReceptor, py::arg("enable"), "Enable NMDA receptor")
        .def("enableRewardModulation", &STDP::enableRewardModulation, py::arg("enable"), "Enable reward modulation")
        .def("enableNeuromodulators", &STDP::enableNeuromodulators, py::arg("enable"), "Enable neuromodulators")
        .def("enableMetaplasticity", &STDP::enableMetaplasticity, py::arg("enable"), "Enable metaplasticity")
        .def("enableMultiExpWindow", &STDP::enableMultiExpWindow, py::arg("enable"), "Enable multi-exponential window")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get STDP time constant")
        .def("getCalciumConcentration", &STDP::getCalciumConcentration, "Get calcium concentration")
        .def("getNMDAConductance", &STDP::getNMDAConductance, "Get NMDA conductance")
        .def("getFacilitationTrace", &STDP::getFacilitationTrace, "Get facilitation trace")
        .def("getDepressionTrace", &STDP::getDepressionTrace, "Get depression trace")
        .def("resetToDefaults", &STDP::resetToDefaults, "Reset to default configuration")
        .def("logStatus", &STDP::logStatus, "Log STDP status");

    // Hebbian class
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("configure", &Hebbian::configure,
              py::arg("learningRate"), py::arg("threshold"),
              "Configure Hebbian parameters")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get learning rate")
        .def("getThreshold", &Hebbian::getThreshold, "Get threshold")
        .def("getActivationHistorySize", &Hebbian::getActivationHistorySize, "Get activation history size")
        .def("reset", &Hebbian::reset, "Reset to default configuration")
        .def("logStatus", &Hebbian::logStatus, "Log Hebbian status");

    // StructuralPlasticity class
    py::class_<StructuralPlasticity, PlasticityRule>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity)pbdoc")
        .def(py::init<>())
        .def("configure", &StructuralPlasticity::configure,
              py::arg("creationRate"), py::arg("growthRate"), py::arg("maxConnections"),
              "Configure structural plasticity parameters")
        .def("getCreationRate", &StructuralPlasticity::getCreationRate, "Get creation rate")
        .def("getGrowthRate", &StructuralPlasticity::getGrowthRate, "Get growth rate")
        .def("getMaxConnections", &StructuralPlasticity::getMaxConnections, "Get maximum connections")
        .def("isConnectionCreationEnabled", &StructuralPlasticity::isConnectionCreationEnabled, "Check if connection creation is enabled")
        .def("isGrowthEnabled", &StructuralPlasticity::isGrowthEnabled, "Check if growth is enabled")
        .def("reset", &StructuralPlasticity::reset, "Reset to default configuration")
        .def("logStatus", &StructuralPlasticity::logStatus, "Log structural plasticity status");

    // RewardModulatedSTDP class
    py::class_<RewardModulatedSTDP, PlasticityRule>(m, "RewardModulatedSTDP", R"pbdoc(Reward-modulated STDP)pbdoc")
        .def(py::init<>())
        .def("setBaseSTDP", &RewardModulatedSTDP::setBaseSTDP, py::arg("stdp"), "Set base STDP")
        .def("setRewardGain", &RewardModulatedSTDP::setRewardGain, py::arg("gain"), "Set reward gain")
        .def("setRewardSignal", &RewardModulatedSTDP::setRewardSignal, py::arg("signal"), "Set reward signal")
        .def("setTemporalDifference", &RewardModulatedSTDP::setTemporalDifference, py::arg("td"), "Set temporal difference")
        .def("enableAccumulation", &RewardModulatedSTDP::enableAccumulation, py::arg("enable"), "Enable accumulation")
        .def("enableEligibilityTrace", &RewardModulatedSTDP::enableEligibilityTrace, py::arg("enable"), "Enable eligibility trace")
        .def("getRewardGain", &RewardModulatedSTDP::getRewardGain, "Get reward gain")
        .def("getRewardSignal", &RewardModulatedSTDP::getRewardSignal, "Get reward signal")
        .def("getTemporalDifference", &RewardModulatedSTDP::getTemporalDifference, "Get temporal difference")
        .def("getAccumulationDecay", &RewardModulatedSTDP::getAccumulationDecay, "Get accumulation decay")
        .def("getEligibilityDecay", &RewardModulatedSTDP::getEligibilityDecay, "Get eligibility decay")
        .def("reset", &RewardModulatedSTDP::reset, "Reset to default configuration")
        .def("logStatus", &RewardModulatedSTDP::logStatus, "Log reward-modulated STDP status");

    // TripletSTDP class
    py::class_<TripletSTDP, PlasticityRule>(m, "TripletSTDP", R"pbdoc(Triplet STDP)pbdoc")
        .def(py::init<>())
        .def("setPrePostPostWeight", &TripletSTDP::setPrePostPostWeight, py::arg("weight"), "Set pre-post-post weight")
        .def("setPostPrePreWeight", &TripletSTDP::setPostPrePreWeight, py::arg("weight"), "Set post-pre-pre weight")
        .def("setFacilitationStrength", &TripletSTDP::setFacilitationStrength, py::arg("strength"), "Set facilitation strength")
        .def("setDepressionStrength", &TripletSTDP::setDepressionStrength, py::arg("strength"), "Set depression strength")
        .def("enableSynapticResources", &TripletSTDP::enableSynapticResources, py::arg("enable"), "Enable synaptic resources")
        .def("setResourceLimit", &TripletSTDP::setResourceLimit, py::arg("limit"), "Set resource limit")
        .def("setResourceRecovery", &TripletSTDP::setResourceRecovery, py::arg("recovery"), "Set resource recovery")
        .def("enableFacilitationTrace", &TripletSTDP::enableFacilitationTrace, py::arg("enable"), "Enable facilitation trace")
        .def("enableDepressionTrace", &TripletSTDP::enableDepressionTrace, py::arg("enable"), "Enable depression trace")
        .def("setTraceDecay", &TripletSTDP::setTraceDecay, py::arg("decay"), "Set trace decay")
        .def("getPrePostPostWeight", &TripletSTDP::getPrePostPostWeight, "Get pre-post-post weight")
        .def("getPostPrePreWeight", &TripletSTDP::getPostPrePreWeight, "Get post-pre-pre weight")
        .def("getFacilitationStrength", &TripletSTDP::getFacilitationStrength, "Get facilitation strength")
        .def("getDepressionStrength", &TripletSTDP::getDepressionStrength, "Get depression strength")
        .def("getResourceLimit", &TripletSTDP::getResourceLimit, "Get resource limit")
        .def("getResourceRecovery", &TripletSTDP::getResourceRecovery, "Get resource recovery")
        .def("getTraceDecay", &TripletSTDP::getTraceDecay, "Get trace decay")
        .def("reset", &TripletSTDP::reset, "Reset to default configuration")
        .def("logStatus", &TripletSTDP::logStatus, "Log triplet STDP status");

    // ============================================================================
    // Neuromodulation Systems
    // ============================================================================

    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", py::module_local(),
        R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get neuromodulator level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set neuromodulator level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Dopamine class
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error")
        .def("getPredictionErrorSignal", &Dopamine::getPredictionErrorSignal, "Get prediction error signal")
        .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"), "Set baseline level")
        .def("setGain", &Dopamine::setGain, py::arg("gain"), "Set gain")
        .def("getBaseline", &Dopamine::getBaseline, "Get baseline level")
        .def("getGain", &Dopamine::getGain, "Get gain")
        .def("reset", &Dopamine::reset, "Reset to default configuration")
        .def("logStatus", &Dopamine::logStatus, "Log dopamine status");

    // Curiosity class
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setNoveltySensitivity", &Curiosity::setNoveltySensitivity, py::arg("sensitivity"), "Set novelty sensitivity")
        .def("setExplorationDrive", &Curiosity::setExplorationDrive, py::arg("drive"), "Set exploration drive")
        .def("getNoveltySensitivity", &Curiosity::getNoveltySensitivity, "Get novelty sensitivity")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("reset", &Curiosity::reset, "Reset to default configuration")
        .def("logStatus", &Curiosity::logStatus, "Log curiosity status");

    // Novelty class
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setDetectionThreshold", &Novelty::setDetectionThreshold, py::arg("threshold"), "Set detection threshold")
        .def("setSensitivity", &Novelty::setSensitivity, py::arg("sensitivity"), "Set sensitivity")
        .def("getDetectionThreshold", &Novelty::getDetectionThreshold, "Get detection threshold")
        .def("getSensitivity", &Novelty::getSensitivity, "Get sensitivity")
        .def("reset", &Novelty::reset, "Reset to default configuration")
        .def("logStatus", &Novelty::logStatus, "Log novelty status");

    // PredictionError class
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setErrorThreshold", &PredictionError::setErrorThreshold, py::arg("threshold"), "Set error threshold")
        .def("setErrorSensitivity", &PredictionError::setErrorSensitivity, py::arg("sensitivity"), "Set error sensitivity")
        .def("getErrorThreshold", &PredictionError::getErrorThreshold, "Get error threshold")
        .def("getErrorSensitivity", &PredictionError::getErrorSensitivity, "Get error sensitivity")
        .def("reset", &PredictionError::reset, "Reset to default configuration")
        .def("logStatus", &PredictionError::logStatus, "Log prediction error status");

    // Reward class
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setRewardThreshold", &Reward::setRewardThreshold, py::arg("threshold"), "Set reward threshold")
        .def("setRewardSensitivity", &Reward::setRewardSensitivity, py::arg("sensitivity"), "Set reward sensitivity")
        .def("getRewardThreshold", &Reward::getRewardThreshold, "Get reward threshold")
        .def("getRewardSensitivity", &Reward::getRewardSensitivity, "Get reward sensitivity")
        .def("reset", &Reward::reset, "Reset to default configuration")
        .def("logStatus", &Reward::logStatus, "Log reward status");

    // ============================================================================
    // Prediction Systems
    // ============================================================================

    // NeuralPrediction class
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural temporal prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState, py::arg("state"), py::arg("step"), "Record sensory state")
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("step"), "Generate prediction")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation, py::arg("actualState"), py::arg("step"), "Update with observation")
        .def("getPredictionError", &NeuralPrediction::getPredictionError, "Get prediction error")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence, "Get prediction confidence")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"), "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("steps"), "Set prediction horizon")
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"), "Enable temporal prediction")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"), "Enable action consequence prediction")
        .def("recordAction", &NeuralPrediction::recordAction, py::arg("action"), py::arg("step"), "Record action")
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons, "Get prediction neurons")
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons, "Get sequence neurons")
        .def("clearHistory", &NeuralPrediction::clearHistory, "Clear history")
        .def("logStatus", &NeuralPrediction::logStatus, "Log prediction status");

    // ============================================================================
    // Cognition Systems
    // ============================================================================

    // NeuralPlanner class
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural action planning)pbdoc")
        .def(py::init<>())
        .def("configure", &NeuralPlanner::configure, py::arg("maxPlans"), py::arg("planHorizon"), "Configure planner")
        .def("setMaxPlans", &NeuralPlanner::setMaxPlans, py::arg("maxPlans"), "Set maximum plans")
        .def("setPlanHorizon", &NeuralPlanner::setPlanHorizon, py::arg("horizon"), "Set plan horizon")
        .def("planActions", &NeuralPlanner::planActions, py::arg("options"), "Plan actions")
        .def("evaluatePlan", &NeuralPlanner::evaluatePlan, py::arg("plan"), py::arg("context"), "Evaluate plan")
        .def("getCurrentPlan", &NeuralPlanner::getCurrentPlan, "Get current plan")
        .def("getPlanHistory", &NeuralPlanner::getPlanHistory, "Get plan history")
        .def("clearPlans", &NeuralPlanner::clearPlans, "Clear plans")
        .def("logStatus", &NeuralPlanner::logStatus, "Log planner status");

    // ConceptFormation class
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Pattern-based concept formation)pbdoc")
        .def(py::init<>())
        .def("configure", &ConceptFormation::configure, py::arg("conceptCount"), py::arg("similarityThreshold"), "Configure concept formation")
        .def("setConceptCount", &ConceptFormation::setConceptCount, py::arg("count"), "Set concept count")
        .def("setSimilarityThreshold", &ConceptFormation::setSimilarityThreshold, py::arg("threshold"), "Set similarity threshold")
        .def("processSensoryInput", &ConceptFormation::processSensoryInput, py::arg("input"), "Process sensory input")
        .def("getConcepts", &ConceptFormation::getConcepts, "Get current concepts")
        .def("getConceptStrength", &ConceptFormation::getConceptStrength, py::arg("conceptId"), "Get concept strength")
        .def("updateConcepts", &ConceptFormation::updateConcepts, py::arg("dt"), "Update concepts")
        .def("clearConcepts", &ConceptFormation::clearConcepts, "Clear concepts")
        .def("logStatus", &ConceptFormation::logStatus, "Log concept formation status");

    // AttentionalSelection class
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attention selection)pbdoc")
        .def(py::init<>())
        .def("configure", &AttentionalSelection::configure, py::arg("attentionSpan"), py::arg("competitionStrength"), "Configure attention selection")
        .def("setAttentionSpan", &AttentionalSelection::setAttentionSpan, py::arg("span"), "Set attention span")
        .def("setCompetitionStrength", &AttentionalSelection::setCompetitionStrength, py::arg("strength"), "Set competition strength")
        .def("focusAttention", &AttentionalSelection::focusAttention, py::arg("stimuli"), "Focus attention on stimuli")
        .def("getCurrentFocus", &AttentionalSelection::getCurrentFocus, "Get current focus")
        .def("getFocusHistory", &AttentionalSelection::getFocusHistory, "Get focus history")
        .def("updateAttention", &AttentionalSelection::updateAttention, py::arg("dt"), "Update attention")
        .def("logStatus", &AttentionalSelection::logStatus, "Log attentional selection status");

    // ============================================================================
    // Development Systems
    // ============================================================================

    // DevelopmentSystem class
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Brain development and maturation)pbdoc")
        .def(py::init<>())
        .def("configure", &DevelopmentSystem::configure,
              py::arg("criticalPeriodLength"), py::arg("maturationRate"), py::arg("plasticityDecay"),
              "Configure development system")
        .def("setCriticalPeriodLength", &DevelopmentSystem::setCriticalPeriodLength, py::arg("length"), "Set critical period length")
        .def("setMaturationRate", &DevelopmentSystem::setMaturationRate, py::arg("rate"), "Set maturation rate")
        .def("setPlasticityDecay", &DevelopmentSystem::setPlasticityDecay, py::arg("decay"), "Set plasticity decay")
        .def("isInCriticalPeriod", &DevelopmentSystem::isInCriticalPeriod, "Check if in critical period")
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage, "Get developmental stage")
        .def("advanceDevelopment", &DevelopmentSystem::advanceDevelopment, py::arg("dt"), "Advance development")
        .def("getDevelopmentalProgress", &DevelopmentSystem::getDevelopmentalProgress, "Get developmental progress")
        .def("getCurrentMaturationLevel", &DevelopmentSystem::getCurrentMaturationLevel, "Get current maturation level")
        .def("getCurrentPlasticityRate", &DevelopmentSystem::getCurrentPlasticityRate, "Get current plasticity rate")
        .def("reset", &DevelopmentSystem::reset, "Reset to initial state")
        .def("logStatus", &DevelopmentSystem::logStatus, "Log development status");

    // ============================================================================
    // Sensory and Motor Systems
    // ============================================================================

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("getCenterPixel", &Vision::getCenterPixel, "Get center pixel value")
        .def("getEdgeFeatures", &Vision::getEdgeFeatures, "Get edge features");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getFrequencySpectrum", &Audio::getFrequencySpectrum, "Get frequency spectrum")
        .def("getAmplitudeEnvelope", &Audio::getAmplitudeEnvelope, "Get amplitude envelope");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal neural signals)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add signal value")
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get signal count")
        .def("getSignal", &InternalSignals::getSignal, py::arg("index"), "Get signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getAverageSignal", &InternalSignals::getAverageSignal, "Get average signal")
        .def("getSignalRange", &InternalSignals::getSignalRange, "Get signal range");

    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Motor action output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("getDescription", &Action::getDescription, "Get action description")
        .def("setExecutionTime", &Action::setExecutionTime, py::arg("time"), "Set execution time")
        .def("getExecutionTime", &Action::getExecutionTime, "Get execution time")
        .def("clone", &Action::clone, "Clone action")
        .def("logStatus", &Action::logStatus, "Log action status");

    // MotorSystem class
    py::class_<MotorSystem>(m, "MotorSystem", R"pbdoc(Motor output system)pbdoc")
        .def(py::init<>())
        .def("configure", &MotorSystem::configure, py::arg("actionSpace"), py::arg("feedbackEnabled"), "Configure motor system")
        .def("setActionSpace", &MotorSystem::setActionSpace, py::arg("space"), "Set action space")
        .def("setFeedbackEnabled", &MotorSystem::setFeedbackEnabled, py::arg("enabled"), "Set feedback enabled")
        .def("executeAction", &MotorSystem::executeAction, py::arg("action"), "Execute action")
        .def("getActionFeedback", &MotorSystem::getActionFeedback, py::arg("action"), "Get action feedback")
        .def("getAvailableActions", &MotorSystem::getAvailableActions, "Get available actions")
        .def("getActionCount", &MotorSystem::getActionCount, "Get action count")
        .def("logStatus", &MotorSystem::logStatus, "Log motor system status");

    // ============================================================================
    // World Systems
    // ============================================================================

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D simulation world)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
              py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
              "Configure world dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("energy"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("rate"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("logStatus", &SimpleWorld::logStatus, "Log world status");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
              py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
              "Create world object")
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        .def("getDistanceTo", &WorldObject::getDistanceTo, py::arg("x"), py::arg("y"), "Get distance to point")
        .def("isColliding", &WorldObject::isColliding, py::arg("other"), "Check collision with other object")
        .def("logStatus", &WorldObject::logStatus, "Log object status");

    // ============================================================================
    // Agent Systems
    // ============================================================================

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
              py::arg("reward"), py::arg("predictedReward"), "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity enabled")
        .def("logStatus", &AgentBrain::logStatus, "Log agent brain status");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent physical body)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Is moving flag")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Is turning flag")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Last action time")
        .def("reset", &AgentBody::reset, "Reset body state")
        .def("updatePosition", &AgentBody::updatePosition, py::arg("dx"), py::arg("dy"), "Update position")
        .def("rotate", &AgentBody::rotate, py::arg("angle"), "Rotate body")
        .def("applyForces", &AgentBody::applyForces, py::arg("forces"), "Apply forces")
        .def("getPosition", &AgentBody::getPosition, "Get current position")
        .def("getVelocity", &AgentBody::getVelocity, "Get current velocity")
        .def("logStatus", &AgentBody::logStatus, "Log agent body status");

    // ============================================================================
    // ActionResult class
    // ============================================================================

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "",
              "Create action result")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success flag")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("getConfidence", &ActionResult::getConfidence, "Get confidence level")
        .def("setConfidence", &ActionResult::setConfidence, py::arg("confidence"), "Set confidence level")
        .def("logStatus", &ActionResult::logStatus, "Log action result status");

    // ============================================================================
    // SensoryPercept class
    // ============================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception data)pbdoc")
        .def(py::init<>())
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp")
        .def("addNoise", &SensoryPercept::addNoise, py::arg("noiseLevel"), "Add noise to signals")
        .def("clear", &SensoryPercept::clear, "Clear all signals")
        .def("logStatus", &SensoryPercept::logStatus, "Log sensory percept status");

    // ============================================================================
    // System Utilities
    // ============================================================================

    // Brain factory functions
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

    m.def("createVisionSensoryInput", []() -> std::shared_ptr<Vision> {
        return std::make_shared<Vision>();
    }, "Create vision sensory input");

    m.def("createAudioSensoryInput", [](size_t sampleRate, size_t numSamples) -> std::shared_ptr<Audio> {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create audio sensory input");

    m.def("createInternalSignalsSensoryInput", []() -> std::shared_ptr<InternalSignals> {
        return std::make_shared<InternalSignals>();
    }, "Create internal signals sensory input");

    m.def("createAction", [](ActionType type, std::vector<float> parameters) -> std::shared_ptr<Action> {
        return std::make_shared<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters"), "Create action");

    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value = 0.0f, float radius = 0.5f) -> std::shared_ptr<WorldObject> {
        return std::make_shared<WorldObject>(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f, "Create world object");

    // System constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    m.attr("INVALID_MEMORY_ID") = py::cast(INVALID_MEMORY_ID);

    // ============================================================================
    // Module-level functions
    // ============================================================================

    m.def("getVersion", []() {
        return "NLM Python Bindings 0.1.0";
    }, "Get version information");

    m.def("getSystemInfo", []() {
        return std::unordered_map<std::string, std::string>({
            {"version", "0.1.0"},
            {"description", "Neural Learning Machine Python Bindings"},
            {"author", "NLM Authors"},
            {"license", "MIT"}
        });
    }, "Get system information");

    // ============================================================================
    // Initialization and cleanup
    // ============================================================================

    // Initialize system
    m.def("initializeSystem", []() {
        // Note: Logger class may not exist in current codebase
        // This is a placeholder for future implementation
    }, "Initialize NLM system");

    // Cleanup system
    m.def("cleanupSystem", []() {
        // Note: Logger class may not exist in current codebase
        // This is a placeholder for future implementation
    }, "Cleanup NLM system");

} // namespace nlm