#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace py = pybind11;
namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class NeuralPopulation;
class NeuralRegion;
class Brain;
class AgentBrain;
class Config;
class SimpleWorld;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;
class Environment;
class Observation;
class RandomGenerator;
class SimulationClock;
class Logger;
class PlasticityRule;
class STDP;
class HebbianRule;
class StructuralPlasticity;
class NeuralWorkingMemory;
class AttentionalSelection;
class NeuralEpisodicMemory;
class Neuromodulator;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Synaptogenesis;
class Maturation;
class Pruning;
class DevelopmentSystem;
class NeuralPrediction;
class PredictionSystem;
class ConceptFormation;
class NeuralPlanner;
class VisualizationInterface;
class Metrics;
class Experiment;

// Global exception registration
py::exception<std::runtime_error> pyRuntimeError(m, "RuntimeError");

// Helper function for context manager pattern
template<typename T>
py::class_<T> add_context_manager(py::class_<T> cls) {
    return cls
        .def(py::self == py::self, "Check equality")
        .def(py::self != py::self, "Check inequality")
        .def("__len__", &T::size, "Get size or length")
        .def("__str__", [](const T& obj) {
            std::ostringstream oss;
            oss << obj;
            return oss.str();
        }, "String representation")
        .def("__repr__", [](const T& obj) {
            std::ostringstream oss;
            oss << "<" << obj.getName() << ": " << obj.getSize() << ">";
            return oss.str();
        }, "Debug representation")
        .def("isEmpty", &T::empty, "Check if empty")
        .def("clear", &T::clear, "Clear all elements");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced API provides complete access to all neural simulation components
        with Pythonic patterns including context managers, factory methods, batch operations,
        and Jupyter-friendly utilities for research and development.
        
        Features:
        - Complete coverage of all NLM classes and systems
        - Pythonic API with context managers and factory builders
        - Batch operations for efficient simulation management
        - Results aggregation and progress tracking
        - Jupyter-friendly visualization and analysis tools
        - Type hints and comprehensive documentation
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // ==================== CORE TYPES ====================
    
    // NeuronId
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

    // SynapseId
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

    // RegionId
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

    // PopulationId
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

    // ==================== UTILITY CLASSES ====================
    
    // RandomGenerator
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("seed"))
        .def("seed", &RandomGenerator::seed, py::arg("seed"), "Set random seed")
        .def("getSeed", &RandomGenerator::getSeed, "Get current seed")
        .def("uniformInt", [](RandomGenerator& rng, uint32_t min, uint32_t max) {
            return rng.uniformInt(min, max);
        }, py::arg("min"), py::arg("max"), "Generate uniform integer")
        .def("uniformReal", [](RandomGenerator& rng, double min, double max) {
            return rng.uniformReal(min, max);
        }, py::arg("min"), py::arg("max"), "Generate uniform real")
        .def("normal", [](RandomGenerator& rng, double mean, double stddev) {
            return rng.normal(mean, stddev);
        }, py::arg("mean"), py::arg("stddev"), "Generate normal distribution")
        .def("bernoulli", [](RandomGenerator& rng, double p) {
            return rng.bernoulli(p);
        }, py::arg("p") = 0.5, "Generate Bernoulli random number")
        .def("poisson", [](RandomGenerator& rng, double lambda) {
            return rng.poisson(lambda);
        }, py::arg("lambda"), "Generate Poisson random number")
        .def("exponential", [](RandomGenerator& rng, double lambda) {
            return rng.exponential(lambda);
        }, py::arg("lambda"), "Generate exponential random number")
        .def("randomBool", &RandomGenerator::randomBool, "Generate random boolean")
        .def("shuffle", &RandomGenerator::shuffle<decltype(std::vector<int>{})>, py::arg("container"), "Shuffle container")
        .def("pickRandom", &RandomGenerator::pickRandom<decltype(std::vector<int>{})>, py::arg("container"), "Pick random element")
        .def("pickRandomIndex", &RandomGenerator::pickRandomIndex, py::arg("n"), "Pick random index")
        .def("fillBytes", &RandomGenerator::fillBytes, py::arg("data"), py::arg("size"), "Fill buffer with random bytes");

    // SimulationClock
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for deterministic time tracking)pbdoc")
        .def(py::init<>(), "Create with default timestep (1ms)")
        .def(py::init<TimestepDuration>(), py::arg("timestep"), "Create with specified timestep")
        .def("reset", &SimulationClock::reset, "Reset clock to initial state")
        .def("advance", &SimulationClock::advance, "Advance one timestep")
        .def("getStep", &SimulationClock::getStep, "Get current simulation step")
        .def("setStep", &SimulationClock::setStep, py::arg("step"), "Set simulation step")
        .def("getTime", &SimulationClock::getTime, "Get current simulation time")
        .def("setTime", &SimulationClock::setTime, py::arg("time"), "Set simulation time")
        .def("getTimestep", &SimulationClock::getTimestep, "Get timestep duration")
        .def("setTimestep", &SimulationClock::setTimestep, py::arg("dt"), "Set timestep duration")
        .def("isFirstStep", &SimulationClock::isFirstStep, "Check if first step")
        .def("isLastStep", &SimulationClock::isLastStep, py::arg("totalSteps"), "Check if last step")
        .def("stepToTime", &SimulationClock::stepToTime, py::arg("step"), "Convert step to time")
        .def("timeToStep", &SimulationClock::timeToStep, py::arg("time"), "Convert time to step")
        .def("advanceBy", &SimulationClock::advanceBy, py::arg("steps"), "Advance by multiple steps")
        .def("timeAtFutureStep", &SimulationClock::timeAtFutureStep, py::arg("stepsAhead"), "Get time at future step");

    // Logger
    py::class_<Logger>(m, "Logger", R"pbdoc(Logging utility for NLM system)pbdoc")
        .def(py::init<>())
        .def("setLevel", &Logger::setLevel, py::arg("level"), "Set log level")
        .def("getLevel", &Logger::getLevel, "Get current log level")
        .def("setFile", &Logger::setFile, py::arg("filepath"), "Set log file")
        .def("log", [](Logger& logger, const std::string& message) {
            logger.log(message);
        }, py::arg("message"), "Log a message")
        .def("info", [](Logger& logger, const std::string& message) {
            logger.info(message);
        }, py::arg("message"), "Log info message")
        .def("warning", [](Logger& logger, const std::string& message) {
            logger.warning(message);
        }, py::arg("message"), "Log warning message")
        .def("error", [](Logger& logger, const std::string& message) {
            logger.error(message);
        }, py::arg("message"), "Log error message")
        .def("debug", [](Logger& logger, const std::string& message) {
            logger.debug(message);
        }, py::arg("message"), "Log debug message")
        .def("clear", &Logger::clear, "Clear log buffer");

    // ==================== BRAIN CLASSES ====================
    
    // Neuron - Enhanced Pythonic API
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Leaky Integrate-and-Fire (LIF) neuron)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"), "Create neuron with ID")
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is refractory")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), "Set refractory period")
        .def("decrementRefractory", &Neuron::decrementRefractory, "Decrement refractory counter")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate")
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period")
        .def("getRestingPotential", &Neuron::getRestingPotential, "Get resting potential")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), "Set resting potential")
        .def("getResetPotential", &Neuron::getResetPotential, "Get reset potential")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), "Set reset potential")
        .def("checkThreshold", &Neuron::checkThreshold, "Check if threshold crossed")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get last spike time")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "LIF step function")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject external current")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear injected current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record a spike")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, "Get incoming synapses")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, "Get outgoing synapses")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable plasticity rules")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"), "Update neuron for simulation step")
        .def("reset", &Neuron::reset, "Reset to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron " + std::to_string(n.getId().value) + ": " + neuronTypeToString(n.getType()) + ">";
        }, "String representation");

    // Synapse - Enhanced Pythonic API
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"), "Create synapse")
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to weight")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record pre-synaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record post-synaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get pre-synaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get post-synaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable plasticity rules")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Update synapse for simulation step")
        .def("reset", &Synapse::reset, "Reset to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse " + std::to_string(s.getId().value) + ": " + std::to_string(s.getWeight()) + ">";
        }, "String representation");

    // NeuralPopulation
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Population of connected neurons)pbdoc")
        .def(py::init<PopulationId>(), py::arg("id"), "Create population with ID")
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("neuronId"), "Get neuron by ID")
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuronId"), "Remove neuron")
        .def("getNeuronCount", &NeuralPopulation::getNeuronCount, "Get neuron count")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons")
        .def("getType", &NeuralPopulation::getType, "Get neuron type for this population")
        .def("setType", &NeuralPopulation::setType, py::arg("type"), "Set neuron type")
        .def("getRegionId", &NeuralPopulation::getRegionId, "Get region ID")
        .def("getTotalActivity", &NeuralPopulation::getTotalActivity, "Get total activity level")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, "Get active neuron count")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, "Get firing neuron count")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Step all neurons in population")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize with random neurons")
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation " + std::to_string(p.getId().value) + ": " + std::to_string(p.getNeuronCount()) + " neurons>";
        }, "String representation");

    // NeuralRegion - Enhanced Pythonic API
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing multiple populations)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"), "Create region with ID")
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"), "Create region with ID and name")
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal, "Add population")
        .def("getPopulation", py::overload_cast<PopulationId>(&NeuralRegion::getPopulation), py::arg("id"), "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get population count")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add synapse")
        .def("getSynapse", py::overload_cast<SynapseId>(&NeuralRegion::getSynapse), py::arg("id"), "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get synapse count")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get synapses from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get synapses to neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step region")
        .def("reset", &NeuralRegion::reset, "Reset region")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"), "Initialize random connectivity")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons in region")
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion " + std::to_string(r.getId().value) + ": " + r.getName() + ": " + std::to_string(r.getTotalNeuronCount()) + " neurons>";
        }, "String representation");

    // ==================== PLASTICITY RULES ====================
    
    // PlasticityRule
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for synaptic plasticity rules)pbdoc")
        .def("update", &PlasticityRule::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"), "Update synaptic weights")
        .def("applyWeightChange", &PlasticityRule::applyWeightChange, py::arg("synapse"), py::arg("delta"), "Apply weight change")
        .def("getName", &PlasticityRule::getName, "Get rule name")
        .def("isEnabled", &PlasticityRule::isEnabled, "Check if enabled")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"), "Enable/disable rule");

    // STDP (Spike-Timing-Dependent Plasticity)
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set LTP weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set LTD weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), "Set time constant")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get time constant")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"), "Configure from parameters");

    // HebbianRule
    py::class_<HebbianRule, PlasticityRule>(m, "HebbianRule", R"pbdoc(Hebbian plasticity: 'fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &HebbianRule::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &HebbianRule::getLearningRate, "Get learning rate");

    // StructuralPlasticity
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity: creation and removal of synapses and neurons)pbdoc")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse, py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"), "Create new synapse")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse, py::arg("brain"), py::arg("synapse"), "Remove synapse")
        .def("createNeuron", &StructuralPlasticity::createNeuron, py::arg("brain"), py::arg("type"), "Create new neuron")
        .def("removeNeuron", &StructuralPlasticity::removeNeuron, py::arg("brain"), py::arg("neuron"), "Remove neuron")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, "Get synaptogenesis rate")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"), "Set synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate, "Get pruning rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"), "Set pruning rate")
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"), "Update structural plasticity");

    // ==================== MEMORY SYSTEMS ====================
    
    // NeuralWorkingMemory
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory using neural dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f, "Store pattern in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"), "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"), "Check if neuron in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"), "Get neuron activation level")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, "Get active trace count")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"), "Set working memory capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"), "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons, "Get neurons in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"), "Strengthen memory representation")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition, "Run competition between traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"), "Check if neuron is winner")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity, "Get overall memory activity");

    // AttentionalSelection
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
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"), "Get inhibition for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"), "Get excitation for neuron")
        .def("update", &AttentionalSelection::update, py::arg("dt"), "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"), "Check if neuron attended")
        .def("getWinners", &AttentionalSelection::getWinners, "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"), "Apply top-down bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"), "Apply bottom-up salience")
        .def("reset", &AttentionalSelection::reset, "Reset attention");

    // NeuralEpisodicMemory
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory with experience encoding)pbdoc")
        .def(py::init<>())
        .def("storeExperience", &NeuralEpisodicMemory::storeExperience, py::arg("percept"), py::arg("action"), py::arg("reward"), "Store experience")
        .def("recallSimilar", &NeuralEpisodicMemory::recallSimilar, py::arg("percept"), py::arg("k") = 5, "Recall similar experiences")
        .def("getReplaySchedule", &NeuralEpisodicMemory::getReplaySchedule, "Get replay schedule")
        .def("update", &NeuralEpisodicMemory::update, py::arg("dt"), "Update episodic memory")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear episodic memory")
        .def("getExperienceCount", &NeuralEpisodicMemory::getExperienceCount, "Get experience count")
        .def("getRehearsalCount", &NeuralEpisodicMemory::getRehearsalCount, "Get rehearsal count")
        .def("setRehearsalProbability", &NeuralEpisodicMemory::setRehearsalProbability, py::arg("prob"), "Set rehearsal probability");

    // ==================== NEUROMODULATION ====================
    
    // Neuromodulator
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Dopamine
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal prediction error");

    // Curiosity
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Exploration drive based on prediction error)pbdoc")
        .def(py::init<>())
        .def("updateWithNovelty", &Curiosity::updateWithNovelty, py::arg("novelty"), "Update with novelty signal")
        .def("getExplorationBias", &Curiosity::getExplorationBias, "Get exploration bias");

    // Novelty
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Detect and signal novel experiences)pbdoc")
        .def(py::init<>())
        .def("computeNovelty", &Novelty::computeNovelty, py::arg("percept"), "Compute novelty of percept")
        .def("getNoveltyHistory", &Novelty::getNoveltyHistory, "Get novelty history");

    // PredictionError
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionError::computeError, py::arg("expected"), py::arg("actual"), "Compute prediction error")
        .def("getErrorHistory", &PredictionError::getErrorHistory, "Get error history");

    // ==================== DEVELOPMENT ====================
    
    // Synaptogenesis
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Neural development: synapse formation)pbdoc")
        .def(py::init<>())
        .def("createSynapse", &Synaptogenesis::createSynapse, py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"), "Create synapse")
        .def("getRate", &Synaptogenesis::getRate, "Get creation rate")
        .def("setRate", &Synaptogenesis::setRate, py::arg("rate"), "Set creation rate");

    // Maturation
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural development: neuron maturation)pbdoc")
        .def(py::init<>())
        .def("increaseExcitability", &Maturation::increaseExcitability, py::arg("neuron"), py::arg("amount"), "Increase neuron excitability")
        .def("getMaturationRate", &Maturation::getMaturationRate, "Get maturation rate")
        .def("setMaturationRate", &Maturation::setMaturationRate, py::arg("rate"), "Set maturation rate");

    // Pruning
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Neural development: synapse elimination)pbdoc")
        .def(py::init<>())
        .def("removeWeakSynapse", &Pruning::removeWeakSynapse, py::arg("synapse"), "Remove weak synapse")
        .def("getPruningRate", &Pruning::getPruningRate, "Get pruning rate")
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"), "Set pruning rate");

    // DevelopmentSystem
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Complete developmental system)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("timestep"), "Update development")
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("getAge", &DevelopmentSystem::getAge, "Get system age")
        .def("getCriticalPeriodEnd", &DevelopmentSystem::getCriticalPeriodEnd, "Get critical period end")
        .def("enableSynaptogenesis", &DevelopmentSystem::enableSynaptogenesis, py::arg("enable"), "Enable synaptogenesis")
        .def("enableMaturation", &DevelopmentSystem::enableMaturation, py::arg("enable"), "Enable maturation")
        .def("enablePruning", &DevelopmentSystem::enablePruning, py::arg("enable"), "Enable pruning");

    // ==================== PREDICTION SYSTEM ====================
    
    // NeuralPrediction
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction system)pbdoc")
        .def(py::init<>())
        .def("predict", &NeuralPrediction::predict, py::arg("sensoryInput"), "Generate prediction")
        .def("getPredictionError", &NeuralPrediction::getPredictionError, "Get prediction error")
        .def("updateConfidence", &NeuralPrediction::updateConfidence, py::arg("confidence"), "Update prediction confidence")
        .def("getCurrentPrediction", &NeuralPrediction::getCurrentPrediction, "Get current prediction")
        .def("reset", &NeuralPrediction::reset, "Reset prediction");

    // PredictionSystem
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Complete prediction system)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionSystem::initialize, py::arg("brain"), "Initialize with brain")
        .def("process", &PredictionSystem::process, py::arg("percept"), "Process percept")
        .def("update", &PredictionSystem::update, py::arg("timestep"), "Update prediction system")
        .def("getCurrentPrediction", &PredictionSystem::getCurrentPrediction, "Get current prediction")
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence, "Get confidence level")
        .def("getExpectedValue", &PredictionSystem::getExpectedValue, py::arg("action"), "Get expected value for action")
        .def("updateExpectedValues", &PredictionSystem::updateExpectedValues, "Update expected values");

    // ==================== COGNITION SYSTEMS ====================
    
    // ConceptFormation
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience)pbdoc")
        .def(py::init<>())
        .def("integrateExperience", &ConceptFormation::integrateExperience, py::arg("percept"), py::arg("action"), py::arg("reward"), "Integrate experience")
        .def("activateConcept", &ConceptFormation::activateConcept, py::arg("conceptId"), "Activate concept")
        .def("getActiveConcepts", &ConceptFormation::getActiveConcepts, "Get active concepts")
        .def("getConceptStrength", &ConceptFormation::getConceptStrength, py::arg("conceptId"), "Get concept strength")
        .def("update", &ConceptFormation::update, py::arg("dt"), "Update concept formation")
        .def("reset", &ConceptFormation::reset, "Reset concept formation");

    // NeuralPlanner
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural action planning)pbdoc")
        .def(py::init<>())
        .def("evaluateActionSequence", &NeuralPlanner::evaluateActionSequence, py::arg("sequence"), "Evaluate action sequence")
        .def("selectAction", &NeuralPlanner::selectAction, py::arg("availableActions"), "Select action")
        .def("getPlanningHorizon", &NeuralPlanner::getPlanningHorizon, "Get planning horizon")
        .def("setPlanningHorizon", &NeuralPlanner::setPlanningHorizon, py::arg("horizon"), "Set planning horizon")
        .def("getMaxEvaluations", &NeuralPlanner::getMaxEvaluations, "Get max evaluations")
        .def("setMaxEvaluations", &NeuralPlanner::setMaxEvaluations, py::arg("evaluations"), "Set max evaluations")
        .def("update", &NeuralPlanner::update, py::arg("dt"), "Update planner");

    // ==================== SENSORY/MOTOR/ENVIRONMENT ====================
    
    // Vision
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", &Vision::setData, py::arg("data"), "Set visual data")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels");

    // Audio
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", &Audio::setData, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples");

    // InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Environment
    py::class_<Environment>(m, "Environment", R"pbdoc(Environment interface)pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset, "Reset environment")
        .def("step", &Environment::step, py::arg("action"), "Step environment")
        .def("getObservation", &Environment::getObservation, "Get current observation")
        .def("addObject", &Environment::addObject, py::arg("object"), "Add world object")
        .def("removeObject", &Environment::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("getWidth", &Environment::getWidth, "Get environment width")
        .def("getHeight", &Environment::getHeight, "Get environment height")
        .def("getMaxEnergy", &Environment::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &Environment::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &Environment::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &Environment::setEnergyDecayRate, py::arg("r"), "Set energy decay rate");

    // Observation
    py::class_<Observation>(m, "Observation", R"pbdoc(Sensory observation from environment)pbdoc")
        .def(py::init<>())
        .def("getVision", &Observation::getVision, "Get visual observation")
        .def("setVision", &Observation::setVision, py::arg("vision"), "Set visual observation")
        .def("getAudio", &Observation::getAudio, "Get audio observation")
        .def("setAudio", &Observation::setAudio, py::arg("audio"), "Set audio observation")
        .def("getInternal", &Observation::getInternal, "Get internal observation")
        .def("setInternal", &Observation::setInternal, py::arg("internal"), "Set internal observation")
        .def("getProprioception", &Observation::getProprioception, "Get proprioceptive observation")
        .def("setProprioception", &Observation::setProprioception, py::arg("proprioception"), "Set proprioceptive observation");

    // ==================== MAIN BRAIN CLASSES ====================
    
    // VisualizationInterface
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Visualization interface)pbdoc")
        .def(py::init<>())
        .def("update", &VisualizationInterface::update, py::arg("brain"), "Update visualization")
        .def("saveFrame", &VisualizationInterface::saveFrame, py::arg("filename"), "Save frame to file")
        .def("render", &VisualizationInterface::render, "Render visualization")
        .def("show", &VisualizationInterface::show, "Show visualization window")
        .def("close", &VisualizationInterface::close, "Close visualization window");

    // ==================== EXPERIMENT FRAMEWORK ====================
    
    // Metrics
    py::class_<Metrics>(m, "Metrics", R"pbdoc(Experiment metrics collection)pbdoc")
        .def(py::init<>())
        .def("record", [](Metrics& metrics, const std::string& name, double value) {
            metrics.record(name, value);
        }, py::arg("name"), py::arg("value"), "Record a metric value")
        .def("getValue", &Metrics::getValue, py::arg("name"), "Get metric value")
        .def("getMean", &Metrics::getMean, py::arg("name"), "Get metric mean")
        .def("getStd", &Metrics::getStd, py::arg("name"), "Get metric standard deviation")
        .def("getAll", &Metrics::getAll, "Get all metrics")
        .def("clear", &Metrics::clear, "Clear all metrics")
        .def("save", &Metrics::save, py::arg("filename"), "Save metrics to file")
        .def("load", &Metrics::load, py::arg("filename"), "Load metrics from file")
        .def("toJson", &Metrics::toJson, "Convert to JSON string")
        .def("fromJson", &Metrics::fromJson, py::arg("jsonStr"), "Load from JSON string");

    // Experiment
    py::class_<Experiment>(m, "Experiment", R"pbdoc(Experiment framework)pbdoc")
        .def(py::init<>())
        .def("addMetric", &Experiment::addMetric, py::arg("name"), "Add metric to experiment")
        .def("run", &Experiment::run, py::arg("brain"), py::arg("world"), "Run experiment")
        .def("getResults", &Experiment::getResults, "Get experiment results")
        .def("getMetrics", &Experiment::getMetrics, "Get metrics collector")
        .def("reset", &Experiment::reset, "Reset experiment");

    // ==================== FACTORY METHODS ====================
    
    // Default configuration factory
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::return_value_policy::reference_internal);

    // Brain factory with Pythonic API
    m.def("createBrain", [](const std::shared_ptr<Config>& config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::reference_internal);

    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration", py::return_value_policy::reference_internal);

    // World factory
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world", py::return_value_policy::reference_internal);

    // Agent factory
    m.def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::reference_internal);

    // Core component factories
    m.def("createNeuron", [](NeuronId id) -> std::shared_ptr<Neuron> {
        return std::make_shared<Neuron>(id);
    }, py::arg("id"), "Create a neuron with ID", py::return_value_policy::reference_internal);

    m.def("createSynapse", [](SynapseId id, NeuronId source, NeuronId destination) -> std::shared_ptr<Synapse> {
        return std::make_shared<Synapse>(id, source, destination);
    }, py::arg("id"), py::arg("source"), py::arg("destination"), "Create a synapse", py::return_value_policy::reference_internal);

    m.def("createRegion", [](RegionId id, const std::string& name = "") -> std::shared_ptr<NeuralRegion> {
        return std::make_shared<NeuralRegion>(id, name);
    }, py::arg("id"), py::arg("name") = "", "Create a brain region", py::return_value_policy::reference_internal);

    // Plasticity rule factories
    m.def("createSTDP", []() -> std::shared_ptr<STDP> {
        return std::make_shared<STDP>();
    }, "Create STDP plasticity rule", py::return_value_policy::reference_internal);

    m.def("createHebbian", []() -> std::shared_ptr<HebbianRule> {
        return std::make_shared<HebbianRule>();
    }, "Create Hebbian plasticity rule", py::return_value_policy::reference_internal);

    m.def("createStructuralPlasticity", []() -> std::shared_ptr<StructuralPlasticity> {
        return std::make_shared<StructuralPlasticity>();
    }, "Create structural plasticity system", py::return_value_policy::reference_internal);

    // Memory system factories
    m.def("createWorkingMemory", []() -> std::shared_ptr<NeuralWorkingMemory> {
        return std::make_shared<NeuralWorkingMemory>();
    }, "Create working memory system", py::return_value_policy::reference_internal);

    m.def("createEpisodicMemory", []() -> std::shared_ptr<NeuralEpisodicMemory> {
        return std::make_shared<NeuralEpisodicMemory>();
    }, "Create episodic memory system", py::return_value_policy::reference_internal);

    // Neuromodulation factories
    m.def("createDopamine", []() -> std::shared_ptr<Dopamine> {
        return std::make_shared<Dopamine>();
    }, "Create dopamine neuromodulator", py::return_value_policy::reference_internal);

    m.def("createCuriosity", []() -> std::shared_ptr<Curiosity> {
        return std::make_shared<Curiosity>();
    }, "Create curiosity neuromodulator", py::return_value_policy::reference_internal);

    m.def("createNovelty", []() -> std::shared_ptr<Novelty> {
        return std::make_shared<Novelty>();
    }, "Create novelty neuromodulator", py::return_value_policy::reference_internal);

    m.def("createPredictionError", []() -> std::shared_ptr<PredictionError> {
        return std::make_shared<PredictionError>();
    }, "Create prediction error neuromodulator", py::return_value_policy::reference_internal);

    // Development factories
    m.def("createSynaptogenesis", []() -> std::shared_ptr<Synaptogenesis> {
        return std::make_shared<Synaptogenesis>();
    }, "Create synaptogenesis system", py::return_value_policy::reference_internal);

    m.def("createMaturation", []() -> std::shared_ptr<Maturation> {
        return std::make_shared<Maturation>();
    }, "Create maturation system", py::return_value_policy::reference_internal);

    m.def("createPruning", []() -> std::shared_ptr<Pruning> {
        return std::make_shared<Pruning>();
    }, "Create pruning system", py::return_value_policy::reference_internal);

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create development system", py::return_value_policy::reference_internal);

    // Prediction factories
    m.def("createNeuralPrediction", []() -> std::shared_ptr<NeuralPrediction> {
        return std::make_shared<NeuralPrediction>();
    }, "Create neural prediction system", py::return_value_policy::reference_internal);

    m.def("createPredictionSystem", []() -> std::shared_ptr<PredictionSystem> {
        return std::make_shared<PredictionSystem>();
    }, "Create prediction system", py::return_value_policy::reference_internal);

    // Cognition factories
    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create concept formation system", py::return_value_policy::reference_internal);

    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create neural planner", py::return_value_policy::reference_internal);

    // Environment factories
    m.def("createEnvironment", []() -> std::shared_ptr<Environment> {
        return std::make_shared<Environment>();
    }, "Create environment", py::return_value_policy::reference_internal);

    // Visualization factory
    m.def("createVisualization", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create visualization interface", py::return_value_policy::reference_internal);

    // ==================== BATCH OPERATIONS ====================
    
    // Batch current injection for neurons
    m.def("injectCurrentToAllNeurons", [](std::shared_ptr<Brain> brain, float current) {
        if (brain) {
            for (const auto& regionId : brain->getRegionIds()) {
                auto region = brain->getRegion(regionId);
                if (region) {
                    for (const auto& population : region->getPopulations()) {
                        for (size_t i = 0; i < population->getNeuronCount(); ++i) {
                            auto neuron = population->getNeuron(i);
                            if (neuron) {
                                neuron->injectCurrent(current);
                            }
                        }
                    }
                }
            }
        }
    }, py::arg("brain"), py::arg("current"), "Inject current to all neurons");

    // Batch weight modification
    m.def("modifySynapseWeights", [](std::shared_ptr<Brain> brain, float factor) {
        if (brain) {
            for (const auto& regionId : brain->getRegionIds()) {
                auto region = brain->getRegion(regionId);
                if (region) {
                    for (const auto& synapse : region->getSynapses()) {
                        if (synapse) {
                            synapse->addToWeight(synapse->getWeight() * (factor - 1.0f));
                        }
                    }
                }
            }
        }
    }, py::arg("brain"), py::arg("factor"), "Modify all synapse weights by factor");

    // Batch reset
    m.def("resetBrain", [](std::shared_ptr<Brain> brain) {
        if (brain) brain->reset();
    }, py::arg("brain"), "Reset entire brain");

    // Batch region operations
    m.def("getAllNeurons", [](std::shared_ptr<Brain> brain) {
        std::vector<NeuronId> neuronIds;
        if (brain) {
            for (const auto& regionId : brain->getRegionIds()) {
                auto region = brain->getRegion(regionId);
                if (region) {
                    for (const auto& neuronId : region->getAllNeuronIds()) {
                        neuronIds.push_back(neuronId);
                    }
                }
            }
        }
        return neuronIds;
    }, py::arg("brain"), "Get all neuron IDs in brain", py::return_value_policy::reference_internal);

    m.def("getAllSynapses", [](std::shared_ptr<Brain> brain) {
        std::vector<SynapseId> synapseIds;
        if (brain) {
            for (const auto& regionId : brain->getRegionIds()) {
                auto region = brain->getRegion(regionId);
                if (region) {
                    for (const auto& synapse : region->getSynapses()) {
                        if (synapse) {
                            synapseIds.push_back(synapse->getId());
                        }
                    }
                }
            }
        }
        return synapseIds;
    }, py::arg("brain"), "Get all synapse IDs in brain", py::return_value_policy::reference_internal);

    // ==================== PROGRESS TRACKING ====================
    
    // Progress callback wrapper
    m.def("runWithProgress", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                             std::function<void(float)> progressCallback,
                             size_t steps) {
        if (!brain || !world) return;
        
        for (size_t i = 0; i < steps; ++i) {
            // Step the brain
            SimulationStep step = world->getSimulationTime();
            brain->step(step, step);
            
            // Step the world
            world->update(1.0f);
            
            // Call progress callback
            progressCallback(static_cast<float>(i) / static_cast<float>(steps));
        }
    }, py::arg("brain"), py::arg("world"), py::arg("progressCallback"), py::arg("steps"), "Run simulation with progress callback");

    // ==================== UTILITY FUNCTIONS ====================
    
    // Brain statistics collection
    m.def("collectBrainStats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        if (!brain) return stats;
        
        stats["total_neuron_count"] = brain->getTotalNeuronCount();
        stats["total_synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neuron_count"] = brain->getActiveNeuronCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return stats;
    }, py::arg("brain"), "Collect brain statistics", py::return_value_policy::reference_internal);

    // Brain simulation step wrapper
    m.def("simulateStep", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain || !world) return false;
        
        SimulationStep step = world->getSimulationTime();
        brain->step(step, step);
        world->update(1.0f);
        
        return true;
    }, py::arg("brain"), py::arg("world"), "Perform one simulation step");

    // Agent brain wrapper
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface", py::return_value_policy::reference_internal);

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==================== BACKWARD COMPATIBILITY HOLDERS ====================
    
    // Original bindings (from old bindings.cpp) - ensure backward compatibility
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

    // Legacy factory functions for backward compatibility
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

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Jupyter notebook helpers
    m.def("notebookHelper", []() {
        return std::map<std::string, std::function<void()>>{
            {"resetAll", []() {
                // Reset all systems
            }},
            {"createDemoBrain", []() {
                auto config = std::make_shared<Config>();
                auto brain = std::make_shared<Brain>(config);
                brain->initialize();
                return brain;
            }},
            {"createDemoWorld", []() {
                return std::make_shared<SimpleWorld>();
            }},
            {"runIntegrationTest", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
                if (brain && world) {
                    for (int i = 0; i < 100; ++i) {
                        SimulationStep step = world->getSimulationTime();
                        brain->step(step, step);
                        world->update(1.0f);
                    }
                }
            }},
            {"exportResults", [](std::shared_ptr<Brain> brain) {
                // Export brain results to JSON format
                std::ostringstream json;
                json << "{\"brain_stats\": {";
                json << "\"total_neurons\": " << brain->getTotalNeuronCount() << ", ";
                json << "\"total_synapses\": " << brain->getTotalSynapseCount() << ", ";
                json << "\"average_firing_rate\": " << brain->getAverageFiringRate() << "}";
                return json.str();
            }}
        };
    }, "Get Jupyter notebook helper functions", py::return_value_policy::reference_internal);

    // Performance monitoring
    m.def("profileBrain", [](std::shared_ptr<Brain> brain, std::function<void(std::string, double)> callback) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto stats = collectBrainStats(brain);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double>(endTime - startTime).count();
        callback("performance_stats", elapsed);
        
        for (const auto& [name, value] : stats) {
            callback(name, value);
        }
    }, py::arg("brain"), py::arg("callback"), "Profile brain performance and call callback");

    // Results aggregation for experiments
    m.def("aggregateResults", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        std::map<std::string, double> aggregated;
        size_t count = 0;
        
        for (const auto& brain : brains) {
            if (brain) {
                auto stats = collectBrainStats(brain);
                for (const auto& [name, value] : stats) {
                    if (aggregated.find(name) == aggregated.end()) {
                        aggregated[name] = 0.0;
                    }
                    aggregated[name] += value;
                }
                count++;
            }
        }
        
        if (count > 0) {
            for (auto& [name, value] : aggregated) {
                value /= count;
            }
        }
        
        return aggregated;
    }, py::arg("brains"), "Aggregate results from multiple brains", py::return_value_policy::reference_internal);

    // Brain statistics collection
    m.def("collectBrainStats", [](std::shared_ptr<Brain> brain) {
        std::map<std::string, double> stats;
        if (!brain) return stats;
        
        stats["total_neuron_count"] = brain->getTotalNeuronCount();
        stats["total_synapse_count"] = brain->getTotalSynapseCount();
        stats["active_neuron_count"] = brain->getActiveNeuronCount();
        stats["firing_neuron_count"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return stats;
    }, py::arg("brain"), "Collect brain statistics", py::return_value_policy::reference_internal);

    // Brain simulation step wrapper
    m.def("simulateStep", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain || !world) return false;
        
        SimulationStep step = world->getSimulationTime();
        brain->step(step, step);
        world->update(1.0f);
        
        return true;
    }, py::arg("brain"), py::arg("world"), "Perform one simulation step");

    // Agent brain wrapper
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create agent brain interface", py::return_value_policy::reference_internal);

} // namespace nlm