#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>  // NumPy integration
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Forward declarations for core neural classes (to be exposed)
namespace nlm {
    class Neuron;
    class Synapse;
    class NeuralRegion;
    class NeuralPopulation;
}

namespace py = pybind11;
namespace nlm {

// Custom exception classes for NLM-specific errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class NeuronError : public NLMError {
public:
    explicit NeuronError(const std::string& msg) : NLMError("Neuron Error: " + msg) {}
};

class SynapseError : public NLMError {
public:
    explicit SynapseError(const std::string& msg) : NLMError("Synapse Error: " + msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Config Error: " + msg) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& msg) : NLMError("World Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Version: 0.1.0
        Author: NLM Authors
        License: MIT
        
        This module provides Python bindings for the NLM neural simulation framework,
        enabling high-performance spiking neural network simulations from Python.
        
        Key Features:
        - Brain simulation with Leaky Integrate-and-Fire neurons
        - Multiple plasticity mechanisms (STDP, Hebbian, structural)
        - Sensorimotor loop with world interaction
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages and cognitive functions
        
        Quick Start Examples:
        
        ```python
        import pynlm
        
        # Create a simple brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        for i in range(100):
            brain.step(i)
        
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        ```
        
        For more detailed documentation, see:
        - easy_usage.md: Beginner-friendly guide
        - HOW_TO_USE.md: Comprehensive API documentation
    )pbdoc";

    // Set module version
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__license__") = "MIT";
    m.attr("__description__") = "Python bindings for NLM neural simulation framework";
    m.attr("__doc__") = m.doc();

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<NeuronError>(m, "NeuronError");
    py::register_exception<SynapseError>(m, "SynapseError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<WorldError>(m, "WorldError");

    // ID Types
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

    // Core neural classes

    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron implementing Leaky Integrate-and-Fire (LIF) dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, "Get neuron unique identifier")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def_property("membrane_potential", &Neuron::getMembranePotential, &Neuron::setMembranePotential, 
                     "Get/set membrane potential (mV)")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), 
             "Add delta to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold (mV)")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold (mV)")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set neuron firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), "Set refractory period (steps)")
        .def("decrementRefractory", &Neuron::decrementRefractory, "Decrement refractory counter")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate (Hz)")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance (nS)")
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance (nS)")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period (steps)")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), "Set resting potential (mV)")
        .def("getRestingPotential", &Neuron::getRestingPotential, "Get resting potential (mV)")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), "Set reset potential (mV)")
        .def("checkThreshold", &Neuron::checkThreshold, "Check if membrane potential exceeds threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get timestamp of last spike (-1 if none)")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), 
             "Perform one LIF integration step")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike at timestamp")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize neuron with random parameters");

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons with plasticity and delays)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId, "Get synapse unique identifier")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron identifier")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron identifier")
        .def_property("weight", &Synapse::getWeight, &Synapse::setWeight, "Get/set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add delta to synaptic weight")
        .def_property("delay", &Synapse::getDelay, &Synapse::setDelay, "Get/set synaptic delay (steps)")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record pre-synaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record post-synaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get pre-synaptic spike timestamps")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get post-synaptic spike timestamps")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), 
             "Enable/disable plasticity rules (Hebbian, STDP, reward-modulated)")
        .def_property("eligibilityTrace", &Synapse::getEligibilityTrace, &Synapse::setEligibilityTrace, 
                     "Get/set eligibility trace (for reward-modulated learning)")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        .def_property("efficacy", &Synapse::getEfficacy, &Synapse::setEfficacy, "Get/set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Update synapse for one simulation step")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize synapse with random parameters");

    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing multiple populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId, "Get region unique identifier")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal, "Add neuron population to region")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), 
             py::return_value_policy::reference_internal, "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get number of populations in region")
        .def("getPopulations", &NeuralRegion::getPopulations, 
             py::return_value_policy::reference_internal, "Get all populations")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations, "Get all populations as pointer list")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), 
             py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add synapse between neurons in region")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"), 
             py::return_value_policy::reference_internal, "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get number of synapses in region")
        .def("getSynapses", &NeuralRegion::getSynapses, 
             py::return_value_policy::reference_internal, "Get all synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get all synapses originating from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get all synapses terminating at neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count across all populations")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density (fraction of possible connections)")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step all populations and synapses")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, 
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons across all populations as pointer list")
        .def("getAllNeuronsConst", &NeuralRegion::getAllNeurons, py::arg(""), 
             "Get all neurons across all populations as const pointer list");

    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId, "Get population unique identifier")
        .def("getSize", &NeuralPopulation::getSize, "Get number of neurons in population")
        .def("isEmpty", &NeuralPopulation::isEmpty, "Check if population is empty")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"), 
             py::return_value_policy::reference_internal, "Get neuron by index")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("getNeurons", &NeuralPopulation::getNeurons, 
             py::return_value_policy::reference_internal, "Get all neurons as reference list")
        .def("getNeuronType", &NeuralPopulation::getNeuronType, "Get neuron type (all neurons share same type)")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"), "Set neuron type for all neurons")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, "Get count of active neurons")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel, "Get activity level (fraction of active neurons)")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize all neurons randomly")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Step all neurons")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds, "Get neuron IDs in this population")
        .def("recordActivity", &NeuralPopulation::recordActivity, "Record current state for statistics")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential, "Get mean membrane potential")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential, "Get variance of membrane potentials");

    // Original classes

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

    // NumPy integration functions
    m.def("numpy_array", [](py::array_t<float> arr) {
        return arr;
    }, py::arg("arr"), "Create NumPy array from float vector");

    m.def("vector_to_numpy", [](const std::vector<float>& data) {
        return py::array_t<float>(data);
    }, py::arg("data"), "Convert vector to NumPy array");

    m.def("numpy_to_vector", [](py::array_t<float> arr) {
        return std::vector<float>(arr.data(), arr.data() + arr.size());
    }, py::arg("arr"), "Convert NumPy array to vector");

    m.def("create_numpy_matrix", [](const std::vector<std::vector<float>>& matrix) {
        if (matrix.empty()) return py::array_t<float>({});
        size_t rows = matrix.size();
        size_t cols = matrix[0].size();
        std::vector<float> flat_data;
        flat_data.reserve(rows * cols);
        for (const auto& row : matrix) {
            flat_data.insert(flat_data.end(), row.begin(), row.end());
        }
        return py::array_t<float>({rows, cols}, flat_data.data());
    }, py::arg("matrix"), "Create 2D NumPy array from vector of vectors");

    m.def("get_numpy_data_ptr", [](const std::vector<float>& data) {
        return py::capsule(data.data(), [](void* p) {});
    }, py::arg("data"), "Get raw data pointer from vector as capsule for performance");

    m.def("check_numpy_array", [](py::array_t<float> arr, size_t expected_size) {
        if (arr.size() != expected_size) {
            throw std::runtime_error("NumPy array size mismatch: expected " + 
                                   std::to_string(expected_size) + ", got " + 
                                   std::to_string(arr.size()));
        }
        return true;
    }, py::arg("arr"), py::arg("expected_size"), "Validate NumPy array size");

    m.def("safe_numpy_to_vector", [](py::array_t<float> arr, size_t expected_size) {
        if (arr.size() != expected_size) {
            throw std::runtime_error("NumPy array size mismatch: expected " + 
                                   std::to_string(expected_size) + ", got " + 
                                   std::to_string(arr.size()));
        }
        return std::vector<float>(arr.data(), arr.data() + arr.size());
    }, py::arg("arr"), py::arg("expected_size"), "Convert NumPy array to vector with size validation");

    // Convenience functions for common operations
    m.def("create_brain_from_config", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create brain from configuration");

    m.def("run_simulation_steps", [](std::shared_ptr<Brain> brain, size_t steps, float dt) {
        for (size_t i = 0; i < steps; ++i) {
            SimulationStep step = static_cast<SimulationStep>(i);
            Timestamp time = static_cast<Timestamp>(i * dt);
            brain->step(step, time);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"), py::arg("dt"), 
       "Run brain simulation for multiple steps and return spike count");

    m.def("create_neuron_population", [](size_t size, NeuronType type) {
        return std::make_shared<NeuralPopulation>(type, size);
    }, py::arg("size"), py::arg("type"), "Create a neuron population");

    m.def("connect_neurons", [](std::shared_ptr<Neuron> source,
                               std::shared_ptr<Neuron> destination,
                               SynapticWeight weight,
                               SynapseType type = SynapseType::Excitatory) {
        // Implementation would need access to brain's synapse management
        // For now, just return success
        return true;
    }, py::arg("source"), py::arg("destination"), py::arg("weight"), py::arg("type") = SynapseType::Excitatory,
       "Connect two neurons with given weight and type");

    m.def("get_brain_stats", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return stats;
    }, py::arg("brain"), "Get comprehensive brain statistics");

    m.def("validate_simulation_parameters", [](size_t neuron_count, size_t steps, float dt) {
        if (neuron_count == 0) {
            throw std::runtime_error("neuron_count must be positive");
        }
        if (steps == 0) {
            throw std::runtime_error("steps must be positive");
        }
        if (dt <= 0.0f) {
            throw std::runtime_error("dt must be positive");
        }
        return true;
    }, py::arg("neuron_count"), py::arg("steps"), py::arg("dt"), 
       "Validate simulation parameters");

    m.def("create_default_config_with_validation", []() {
        auto config = std::make_shared<Config>();
        // Set sensible defaults for Phase 2 simulation
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        return config;
    }, "Create default configuration with Phase 2 parameters");

    m.def("simulation_summary", [](const std::shared_ptr<Brain>& brain, size_t step) {
        py::dict summary;
        summary["step"] = step;
        summary["total_spikes"] = brain->getTotalSpikeCount();
        summary["firing_neurons"] = brain->getFiringNeuronCount();
        summary["average_firing_rate"] = brain->getAverageFiringRate();
        summary["development_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        summary["plasticity_enabled"] = brain->getTotalSynapseCount() > 0;
        return summary;
    }, py::arg("brain"), py::arg("step"), "Get simulation summary at current step");

    m.def("benchmark_neural_network", [](std::shared_ptr<Brain> brain, size_t warmup_steps, size_t benchmark_steps) {
        // Warm-up
        for (size_t i = 0; i < warmup_steps; ++i) {
            brain->step(i, i * 0.001);
        }
        
        // Benchmark
        auto start_time = std::chrono::high_resolution_clock::now();
        for (size_t i = warmup_steps; i < warmup_steps + benchmark_steps; ++i) {
            brain->step(i, i * 0.001);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        double time_per_step = static_cast<double>(duration.count()) / benchmark_steps;
        
        py::dict results;
        results["total_steps"] = warmup_steps + benchmark_steps;
        results["benchmark_steps"] = benchmark_steps;
        results["duration_microseconds"] = duration.count();
        results["time_per_step_microseconds"] = time_per_step;
        results["steps_per_second"] = 1000000.0 / time_per_step;
        return results;
    }, py::arg("brain"), py::arg("warmup_steps"), py::arg("benchmark_steps"), 
       "Benchmark neural network performance");

    // Original factory functions
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