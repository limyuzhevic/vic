#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <deque>

#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/core/RandomGenerator.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/core/Types/Types.hpp"

// Forward declarations for brain-related types
namespace nlm {
    class Brain;
    class NeuralRegion;
    class NeuralAssociativeMemory;
    class WorkingMemory;
    class EpisodicMemory;
    class SemanticMemory;
    class ProceduralMemory;
    class AttentionalSelection;
    class SelfModel;
    class SocialLearning;
    class SpatialRepresentation;
    class TemporalRelation;
}

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

    // NEURON CLASS BINDINGS
    py::class_<Neuron>(m, "Neuron", R"pbdoc(
        Neuron class representing a single neuron
        
        Implements Leaky Integrate-and-Fire (LIF) dynamics with support for
        excitatory/inhibitory inputs, spike history tracking, and plasticity flags.
    )pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"),
             "Create neuron with ID")
        .def("getId", &Neuron::getId,
             "Get the neuron's unique identifier")
        .def("getType", &Neuron::getType,
             "Get the neuron type")
        .def("setType", &Neuron::setType, py::arg("type"),
             "Set the neuron type")
        .def("getState", &Neuron::getState,
             "Get the neuron state structure")
        .def("getMembranePotential", &Neuron::getMembranePotential,
             "Get the current membrane potential (mV)")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"),
             "Set the membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"),
             "Add to the membrane potential")
        .def("getThreshold", &Neuron::getThreshold,
             "Get the firing threshold (mV)")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"),
             "Set the firing threshold")
        .def("isFiring", &Neuron::isFiring,
             "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory,
             "Check if neuron is in refractory period")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"),
             "Set the firing state")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"),
             "Set the refractory period in steps")
        .def("decrementRefractory", &Neuron::decrementRefractory,
             "Decrement refractory counter")
        .def("getFiringRate", &Neuron::getFiringRate,
             "Get the current firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"),
             "Set the firing rate (Hz)")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"),
             "Set the leak conductance (nS)")
        .def("getLeakConductance", &Neuron::getLeakConductance,
             "Get the leak conductance (nS)")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod,
             "Get the refractory period in steps")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"),
             "Set the resting potential (mV)")
        .def("getRestingPotential", &Neuron::getRestingPotential,
             "Get the resting potential (mV)")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"),
             "Set the reset potential (mV)")
        .def("checkThreshold", &Neuron::checkThreshold,
             "Check if neuron has reached firing threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime,
             "Get the timestamp of the last spike (-1.0 if none)")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Perform one LIF integration step (returns true if neuron fired)")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("strength"),
             "Receive excitatory input (positive membrane potential change)")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("strength"),
             "Receive inhibitory input (negative membrane potential change)")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("strength"),
             "Receive modulatory input (affects plasticity)")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"),
             "Inject external current into the neuron")
        .def("getTotalCurrent", &Neuron::getTotalCurrent,
             "Get the total current (synaptic + injected)")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent,
             "Clear total current (reset for next timestep)")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"),
             "Record a spike event at the given timestamp")
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             "Get the vector of recorded spike timestamps")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory,
             "Clear the spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"),
             "Add an incoming synapse handle to the neuron")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"),
             "Add an outgoing synapse handle to the neuron")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses,
             "Get the vector of incoming synapse handles")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses,
             "Get the vector of outgoing synapse handles")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             "Get the plasticity flags structure")
        .def("enablePlasticity", &Neuron::enablePlasticity,
             py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"),
             "Enable plasticity rules (Hebbian, STDP, reward-modulated)")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"),
             "Set the neuron's region ID")
        .def("getRegionId", &Neuron::getRegionId,
             "Get the neuron's region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"),
             "Set the neuron's population ID")
        .def("getPopulationId", &Neuron::getPopulationId,
             "Get the neuron's population ID")
        .def("step", &Neuron::step, py::arg("currentTime"),
             "Update neuron for one simulation step")
        .def("reset", &Neuron::reset,
             "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"),
             "Initialize neuron with random parameters")
        .def("toString", &Neuron::toString,
             "Get a string representation of the neuron")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron: id=" + std::to_string(n.getId().value) +
                   ", type=" + std::to_string(static_cast<int>(n.getType())) +
                   ", potential=" + std::to_string(n.getMembranePotential()) +
                   ">";
        });

    // SYNAPSE CLASS BINDINGS
    py::class_<Synapse>(m, "Synapse", R"pbdoc(
        Synapse class representing a connection between neurons
        
        Implements synaptic transmission with delays, short-term plasticity,
        and multiple plasticity mechanisms (Hebbian, STDP, structural).
    )pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(),
             py::arg("id"), py::arg("source"), py::arg("destination"),
             "Create synapse with IDs")
        .def("getId", &Synapse::getId,
             "Get the synapse's unique identifier")
        .def("getSourceNeuron", &Synapse::getSourceNeuron,
             "Get the source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron,
             "Get the destination neuron ID")
        .def("getWeight", &Synapse::getWeight,
             "Get the synaptic weight (-1.0 to 1.0)")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"),
             "Set the synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"),
             "Add to the synaptic weight")
        .def("getDelay", &Synapse::getDelay,
             "Get the synaptic delay in simulation steps")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"),
             "Set the synaptic delay")
        .def("getType", &Synapse::getType,
             "Get the synapse type")
        .def("setType", &Synapse::setType, py::arg("type"),
             "Set the synapse type")
        .def("isExcitatory", &Synapse::isExcitatory,
             "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory,
             "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"),
             "Record a presynaptic spike at the given timestamp")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"),
             "Record a postsynaptic spike at the given timestamp")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory,
             "Get the vector of presynaptic spike timestamps")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory,
             "Get the vector of postsynaptic spike timestamps")
        .def("clearHistory", &Synapse::clearHistory,
             "Clear both spike history vectors")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags,
             "Get the plasticity flags structure")
        .def("enablePlasticity", &Synapse::enablePlasticity,
             py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"),
             "Enable plasticity rules (Hebbian, STDP, reward-modulated)")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace,
             "Get the eligibility trace value")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"),
             "Set the eligibility trace value")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decay_rate"),
             "Decay the eligibility trace by the given rate")
        .def("getEfficacy", &Synapse::getEfficacy,
             "Get the synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"),
             "Set the synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"),
             "Update synapse for one simulation step")
        .def("reset", &Synapse::reset,
             "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"),
             "Initialize synapse with random parameters")
        .def("toString", &Synapse::toString,
             "Get a string representation of the synapse")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: id=" + std::to_string(s.getId().value) +
                   ", source=" + std::to_string(s.getSourceNeuron().value) +
                   ", dest=" + std::to_string(s.getDestinationNeuron().value) +
                   ", weight=" + std::to_string(s.getWeight()) +
                   ">";
        });

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

    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

    py::class_<ConfigEntry>(m, "ConfigEntry", R"pbdoc(Configuration entry)pbdoc")
        .def(py::init<>())
        .def(py::init<const std::string&, const ConfigValue&, ConfigSource, const std::string&>(),
             py::arg("key"), py::arg("value"), py::arg("source"), py::arg("description") = "")
        .def_readwrite("key", &ConfigEntry::key)
        .def_readwrite("value", &ConfigEntry::value)
        .def_readwrite("source", &ConfigEntry::source)
        .def_readwrite("description", &ConfigEntry::description);

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
        
        // ConfigValue variants
        .def("set", [](Config& self, const std::string& key, const ConfigValue& value,
                      ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (ConfigValue)")
        .def("set", [](Config& self, const std::string& key, const std::string& value,
                      ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, int value,
                      ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (integer)")
        .def("set", [](Config& self, const std::string& key, double value,
                      ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (floating point)")
        .def("set", [](Config& self, const std::string& key, bool value,
                      ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (boolean)")

        // get() methods - exposing all supported types from ConfigValue variant
        .def("get", py::overload_cast<const std::string&>(&Config::get<int>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (int).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is int type, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get<int64_t>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (int64_t).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is int64_t type, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get<double>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (double).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is double type, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get<bool>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (bool).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is bool type, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::string>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (string).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is string type, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::vector<int>>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (vector<int>).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is vector<int> type, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::vector<double>>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (vector<double>).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is vector<double> type, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get<std::vector<std::string>>, py::const_), py::arg("key"),
             R"pbdoc(Get configuration value (vector<string>).
             
             Args:
                 key: Configuration key
                 
             Returns:
                 Value if key exists and is vector<string> type, None otherwise
             )pbdoc")

        // getOr() methods - exposing all supported types from ConfigValue variant
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (int)")
        .def("getOr", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (int64_t)")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (double)")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (bool)")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (string)")
        .def("getOr", [](const Config& self, const std::string& key, const std::vector<int>& defaultValue) {
            return self.getOr<std::vector<int>>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (vector<int>)")
        .def("getOr", [](const Config& self, const std::string& key, const std::vector<double>& defaultValue) {
            return self.getOr<std::vector<double>>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (vector<double>)")
        .def("getOr", [](const Config& self, const std::string& key, const std::vector<std::string>& defaultValue) {
            return self.getOr<std::vector<std::string>>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (vector<string>)")

        // remove() method
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        
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

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        The Brain class is the core component of the NLM (Neural Learning Machine) system.
        It implements a complete integrated artificial brain with real spiking neural computation,
        incorporating memory, prediction, cognition, and neuromodulation systems.
        
        Features:
        - Real-time event-driven neural simulation with LIF dynamics
        - Multiple memory systems (working, episodic, associative)
        - Prediction system for sensory prediction and error computation
        - Cognitive systems (neural planner, concept formation, attentional selection)
        - Development system for plasticity changes over time
        - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
        - Plasticity mechanisms (STDP, Hebbian, structural plasticity)
        - Spike-based communication with precise timing
        - Checkpoint saving and loading for state persistence
        - Comprehensive statistics and logging capabilities
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
              Constructor for Brain class.
              
              Args:
                  config: Shared pointer to configuration object
                  
              Returns:
                  Brain instance initialized with configuration
              )pbdoc")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration",
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
              Perform a simulation step with time-based stepping.
              
              Args:
                  currentStep: Current simulation step number
              )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
              Perform a simulation step with precise timestamp.
              
              Args:
                  currentStep: Current simulation step number
                  currentTime: Current simulation time in seconds
              )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
              Inject sensory input into the brain.
              
              This method processes sensory input and injects appropriate currents into
              sensory neurons based on the input pattern.
              
              Args:
                  input: SensoryInput object containing sensor data
              )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
              Inject current directly into a specific neuron.
              
              Args:
                  neuron: Target NeuronId to inject current into
                  current: Current value to inject (in nanoamps)
              )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
              Inject current into all neurons of a specific type.
              
              Args:
                  type: NeuronType specifying which neurons to target
                  current: Current value to inject (in nanoamps)
              )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
              Produce motor action based on neural activity.
              
              Analyzes motor neuron population activity and generates appropriate
              motor actions based on the highest firing motor neuron groups.
              
              Returns:
                  Action: Generated motor action with type and parameters
              )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
              Reset brain state to initial conditions.
              
              This method resets all neural populations, memory systems, and
              internal state variables to their initial values.
              )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
              Save brain state to file (checkpointing).
              
              Args:
                  filepath: Path to save the brain state to
                  
              Returns:
                  bool: True if save was successful, False otherwise
              )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
              Load brain state from file.
              
              Args:
                  filepath: Path to load brain state from
                  
              Returns:
                  bool: True if load was successful, False otherwise
              )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
              Add a new neural region to the brain.
              
              Args:
                  name: Optional name for the region (default: "")
                  
              Returns:
                  RegionId: Unique identifier for the newly created region
              )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get a neural region by its ID.
              
              Args:
                  id: RegionId of the region to retrieve
                  
              Returns:
                  NeuralRegion*: Pointer to the neural region, or nullptr if not found
              )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
              Get the number of regions in the brain.
              
              Returns:
                  size_t: Number of neural regions
              )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
              Get all region IDs in the brain.
              
              Returns:
                  std::vector<RegionId>: Vector of all region IDs
              )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get all neural regions in the brain.
              
              Returns:
                  const std::vector<std::unique_ptr<NeuralRegion>>&: All neural regions
              )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
              Get total neuron count across all regions.
              
              Returns:
                  size_t: Total number of neurons in the brain
              )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
              Get total synapse count across all regions.
              
              Returns:
                  size_t: Total number of synapses in the brain
              )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
              Get count of active neurons (above firing threshold).
              
              Returns:
                  size_t: Number of currently active neurons
              )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
              Get count of currently firing neurons (spiking this step).
              
              Returns:
                  size_t: Number of neurons that fired in the current step
              )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
              Get average firing rate across all neurons.
              
              Returns:
                  float: Average firing rate (spikes per second)
              )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
              Get excitation/inhibition balance ratio.
              
              Returns:
                  float: Ratio of total excitatory to inhibitory synaptic weights.
                         Higher values indicate more excitation dominance.
              )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
              Get total spike count since brain initialization.
              
              Returns:
                  size_t: Cumulative number of spikes across all neurons
              )pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             R"pbdoc(
              Get count of pending spike events (both immediate and delayed).
              
              Returns:
                  size_t: Number of unprocessed spike events
              )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
              Get current developmental stage.
              
              Returns:
                  DevelopmentalStage: Current developmental stage (Initial, CriticalPeriod, etc.)
              )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
              Set developmental stage.
              
              Args:
                  stage: DevelopmentalStage to set
              )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get the configuration object.
              
              Returns:
                  std::shared_ptr<const Config>: Configuration object reference
              )pbdoc")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the spike system.
              
              Returns:
                  SpikeSystem*: Pointer to the spike system component
              )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the STDP plasticity system.
              
              Returns:
                  STDP*: Pointer to the STDP plasticity component
              )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the Hebbian plasticity system.
              
              Returns:
                  Hebbian*: Pointer to the Hebbian plasticity component
              )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the structural plasticity system.
              
              Returns:
                  StructuralPlasticity*: Pointer to the structural plasticity component
              )pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the working memory system.
              
              Returns:
                  NeuralWorkingMemory*: Pointer to the working memory component
              )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the episodic memory system.
              
              Returns:
                  NeuralEpisodicMemory*: Pointer to the episodic memory component
              )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the associative memory system.
              
              Returns:
                  NeuralAssociativeMemory*: Pointer to the associative memory component
              )pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the prediction system.
              
              Returns:
                  PredictionSystem*: Pointer to the prediction component
              )pbdoc")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the neural planner system.
              
              Returns:
                  NeuralPlanner*: Pointer to the planner component
              )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the concept formation system.
              
              Returns:
                  ConceptFormation*: Pointer to the concept formation component
              )pbdoc")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the attentional selection system.
              
              Returns:
                  AttentionalSelection*: Pointer to the attentional selection component
              )pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the development system.
              
              Returns:
                  DevelopmentSystem*: Pointer to the development system component
              )pbdoc")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the dopamine neuromodulation system.
              
              Returns:
                  Dopamine*: Pointer to the dopamine neuromodulation component
              )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the curiosity neuromodulation system.
              
              Returns:
                  Curiosity*: Pointer to the curiosity neuromodulation component
              )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the novelty neuromodulation system.
              
              Returns:
                  Novelty*: Pointer to the novelty neuromodulation component
              )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the prediction error neuromodulation system.
              
              Returns:
                  PredictionError*: Pointer to the prediction error component
              )pbdoc")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             R"pbdoc(
              Get access to the random number generator.
              
              Returns:
                  RandomGenerator*: Pointer to the random number generator component
              )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
              Log brain status and statistics to the logger.
              
              This method outputs comprehensive information about the current state of the brain,
              including region statistics, memory system status, and neuromodulation levels.
              """)
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             R"pbdoc(
              Apply neuromodulatory signals to the brain.
              
              Args:
                  signal: Neuromodulator object containing neuromodulatory signal
              )pbdoc");

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

    // SPARK SYSTEM BINDINGS
    
    py::class_<SpikeEvent>(m, "SpikeEvent", R"pbdoc(
        Spike event for event-driven neural computation.
        
        Represents a spike generated by a neuron at a specific time and simulation step.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, Timestamp, SimulationStep>(),
             py::arg("source_neuron"), py::arg("timestamp"), py::arg("step"))
        .def_readwrite("source_neuron", &SpikeEvent::source_neuron)
        .def_readwrite("timestamp", &SpikeEvent::timestamp)
        .def_readwrite("step", &SpikeEvent::step)
        .def("__repr__", [](const SpikeEvent& event) {
            return "<SpikeEvent: source=" + std::to_string(event.source_neuron.value) +
                   ", timestamp=" + std::to_string(event.timestamp) +
                   ", step=" + std::to_string(event.step) + ">";
        });
    
    py::class_<DelayedSpikeEvent>(m, "DelayedSpikeEvent", R"pbdoc(
        Delayed spike event for synaptic transmission with delay.
        
        Represents a spike that will be delivered after a delay, containing
        all necessary information for synaptic transmission including source,
        destination, weight, and timing information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, NeuronId, SynapseId, SynapticWeight, SynapseType, 
                      Timestamp, Timestamp, SimulationStep, SimulationStep>(),
             py::arg("source_neuron"), py::arg("destination_neuron"), 
             py::arg("synapse_id"), py::arg("weight"), py::arg("synapse_type"),
             py::arg("timestamp"), py::arg("delivery_time"),
             py::arg("step"), py::arg("delivery_step"))
        .def_readwrite("source_neuron", &DelayedSpikeEvent::source_neuron)
        .def_readwrite("destination_neuron", &DelayedSpikeEvent::destination_neuron)
        .def_readwrite("synapse_id", &DelayedSpikeEvent::synapse_id)
        .def_readwrite("weight", &DelayedSpikeEvent::weight)
        .def_readwrite("synapse_type", &DelayedSpikeEvent::synapse_type)
        .def_readwrite("timestamp", &DelayedSpikeEvent::timestamp)
        .def_readwrite("delivery_time", &DelayedSpikeEvent::delivery_time)
        .def_readwrite("step", &DelayedSpikeEvent::step)
        .def_readwrite("delivery_step", &DelayedSpikeEvent::delivery_step)
        .def_readwrite("is_excitatory", &DelayedSpikeEvent::is_excitatory)
        .def("__repr__", [](const DelayedSpikeEvent& event) {
            return "<DelayedSpikeEvent: " + std::to_string(event.source_neuron.value) +
                   " -> " + std::to_string(event.destination_neuron.value) +
                   ", weight=" + std::to_string(event.weight) +
                   ", delivery_time=" + std::to_string(event.delivery_time) + ">";
        });
    
    py::class_<DetailedSpikeEvent>(m, "DetailedSpikeEvent", R"pbdoc(
        Detailed spike event with additional metadata.
        
        Enhanced spike event containing neuron identification and simulation
        context information including region and population IDs for comprehensive
        neural event tracking and analysis.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, Timestamp, SimulationStep, RegionId, PopulationId>(),
             py::arg("source"), py::arg("timestamp"), py::arg("step"),
             py::arg("region_id"), py::arg("population_id"))
        .def_readwrite("source", &DetailedSpikeEvent::source)
        .def_readwrite("timestamp", &DetailedSpikeEvent::timestamp)
        .def_readwrite("step", &DetailedSpikeEvent::step)
        .def_readwrite("region_id", &DetailedSpikeEvent::regionId)
        .def_readwrite("population_id", &DetailedSpikeEvent::populationId)
        .def("__repr__", [](const DetailedSpikeEvent& event) {
            return "<DetailedSpikeEvent: source=" + std::to_string(event.source.value) +
                   ", region_id=" + std::to_string(event.regionId.value) +
                   ", population_id=" + std::to_string(event.populationId.value) +
                   ", timestamp=" + std::to_string(event.timestamp) +
                   ", step=" + std::to_string(event.step) + ">";
        });
    
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(
        Event-driven neural computation system that handles spike event processing
        and delayed synaptic transmission.
        
        The SpikeSystem is a core component of the NLM (Neural Learning Machine)
        framework that provides event-driven programming interfaces with spike
        handlers and delayed spike handlers for real-time neural simulation.
        
        Key features:
        - Handles spike event processing and delayed synaptic transmission
        - Provides event-driven programming interfaces with spike handlers
        - Includes statistics and analysis methods
        - Has reset and history management
    )pbdoc")
        .def(py::init<>())
        .def("queueSpike", 
             static_cast<void (SpikeSystem::*)(const SpikeEvent&)>(&SpikeSystem::queueSpike),
             py::arg("event"),
             R"pbdoc(
             Queue a spike event for processing.
             
             Args:
                 event: SpikeEvent object containing source, timestamp, and step information
             )pbdoc")
        .def("queueSpike",
             static_cast<void (SpikeSystem::*)(NeuronId, Timestamp, SimulationStep)>(&SpikeSystem::queueSpike),
             py::arg("neuron"), py::arg("timestamp"), py::arg("step"),
             R"pbdoc(
             Queue a spike event from a specific neuron.
             
             Args:
                 neuron: NeuronId of the neuron generating the spike
                 timestamp: Time when the spike occurred (seconds)
                 step: Simulation step when the spike occurred
             )pbdoc")
        .def("queueDelayedSpike",
             &SpikeSystem::queueDelayedSpike,
             py::arg("event"),
             R"pbdoc(
             Queue a delayed spike event for synaptic transmission.
             
             Args:
                 event: DelayedSpikeEvent object containing destination,
                        weight, and delivery timing information
             )pbdoc")
        .def("processSpikes",
             &SpikeSystem::processSpikes,
             py::arg("currentStep"),
             R"pbdoc(
             Process all queued spikes for the current simulation step.
             
             Args:
                 currentStep: Current simulation step number
             )pbdoc")
        .def("processDelayedSpikes",
             &SpikeSystem::processDelayedSpikes,
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Process pending delayed spikes that should be delivered now.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
             )pbdoc")
        .def("registerHandler",
             &SpikeSystem::registerHandler,
             py::arg("handler"),
             R"pbdoc(
             Register a spike handler callback (called when spikes are generated).
             
             Args:
                 handler: Function that takes a DetailedSpikeEvent parameter
                          and is called when a spike is processed
             )pbdoc")
        .def("registerDelayedHandler",
             &SpikeSystem::registerDelayedHandler,
             py::arg("handler"),
             R"pbdoc(
             Register a delayed spike handler (called when delayed spikes are delivered).
             
             Args:
                 handler: Function that takes a DelayedSpikeEvent parameter
                          and is called when a delayed spike is delivered
             )pbdoc")
        .def("getSpikeHistory",
             &SpikeSystem::getSpikeHistory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get spike history as a vector of DetailedSpikeEvent objects.
             
             Returns:
                 List of DetailedSpikeEvent objects representing all recorded spikes
             )pbdoc")
        .def("clearHistory",
             &SpikeSystem::clearHistory,
             R"pbdoc(
             Clear the spike history.
             
             Removes all recorded spike events from memory.
             )pbdoc")
        .def("getSpikeCount",
             &SpikeSystem::getSpikeCount,
             R"pbdoc(
             Get total spike count since system initialization.
             
             Returns:
                 Total number of spikes processed
             )pbdoc")
        .def("getPendingSpikeCount",
             &SpikeSystem::getPendingSpikeCount,
             R"pbdoc(
             Get number of pending immediate spike events.
             
             Returns:
                 Number of unprocessed spike events
             )pbdoc")
        .def("getPendingDelayedCount",
             &SpikeSystem::getPendingDelayedCount,
             R"pbdoc(
             Get number of pending delayed spike events.
             
             Returns:
                 Number of delayed spike events waiting to be delivered
             )pbdoc")
        .def("getAverageSpikeRate",
             &SpikeSystem::getAverageSpikeRate,
             R"pbdoc(
             Get average spike rate across all neurons.
             
             Returns:
                 Average firing rate in spikes per second
             )pbdoc")
        .def("getMostActiveNeurons",
             &SpikeSystem::getMostActiveNeurons,
             py::arg("count"),
             R"pbdoc(
             Get the most active neurons by spike count.
             
             Args:
                 count: Number of most active neurons to return
                 
             Returns:
                 List of NeuronId objects sorted by activity level
             )pbdoc")
        .def("reset",
             &SpikeSystem::reset,
             R"pbdoc(
             Reset the spike system to initial state.
             
             This method clears all pending events and resets statistics,
             returning the system to its initial configuration.
             )pbdoc")
        .def("__repr__", [](const SpikeSystem& system) {
            return "<SpikeSystem: spikes=" + std::to_string(system.getSpikeCount()) +
                   ", pending_immediate=" + std::to_string(system.getPendingSpikeCount()) +
                   ", pending_delayed=" + std::to_string(system.getPendingDelayedCount()) + ">";
        });

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
        
        // ConfigValue variants
        .def("set", [](Config& self, const std::string& key, const ConfigValue& value,
                     ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (ConfigValue)")
        .def("set", [](Config& self, const std::string& key, const std::string& value,
                     ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, int value,
                     ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (integer)")
        .def("set", [](Config& self, const std::string& key, double value,
                     ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (floating point)")
        .def("set", [](Config& self, const std::string& key, bool value,
                     ConfigSource source) { self.set(key, value, source); },
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set configuration value (boolean)")
        
        // get() methods
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<int>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (int)")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<int64_t>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (int64_t)")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<double>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (double)")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<bool>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (bool)")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<std::string>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (string)")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<std::vector<int>>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (vector<int>)")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<std::vector<double>>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (vector<double>)")
        .def("get", [](const Config& self, const std::string& key) -> py::object {
            auto val = self.get<std::vector<std::string>>(key);
            if (val) return py::cast(*val);
            return py::none();
        }, py::arg("key"), "Get configuration value (vector<string>)")
        
        // getOr() methods
        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (int)")
        .def("getOr", [](const Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (int64_t)")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (double)")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (bool)")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (string)")
        .def("getOr", [](const Config& self, const std::string& key, const std::vector<int>& defaultValue) {
            return self.getOr<std::vector<int>>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (vector<int>)")
        .def("getOr", [](const Config& self, const std::string& key, const std::vector<double>& defaultValue) {
            return self.getOr<std::vector<double>>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (vector<double>)")
        .def("getOr", [](const Config& self, const std::string& key, const std::vector<std::string>& defaultValue) {
            return self.getOr<std::vector<std::string>>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value or default (vector<string>)")
        
        // remove() method
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        
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

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        The Brain class is the core component of the NLM (Neural Learning Machine) system.
        It implements a complete integrated artificial brain with real spiking neural computation,
        incorporating memory, prediction, cognition, and neuromodulation systems.
        
        Features:
        - Real-time event-driven neural simulation with LIF dynamics
        - Multiple memory systems (working, episodic, associative)
        - Prediction system for sensory prediction and error computation
        - Cognitive systems (neural planner, concept formation, attentional selection)
        - Development system for plasticity changes over time
        - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
        - Plasticity mechanisms (STDP, Hebbian, structural plasticity)
        - Spike-based communication with precise timing
        - Checkpoint saving and loading for state persistence
        - Comprehensive statistics and logging capabilities
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Constructor for Brain class.
             
             Args:
                 config: Shared pointer to configuration object
                 
             Returns:
                 Brain instance initialized with configuration
             )pbdoc")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration",
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a simulation step with time-based stepping.
             
             Args:
                 currentStep: Current simulation step number
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with precise timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             This method processes sensory input and injects appropriate currents into
             sensory neurons based on the input pattern.
             
             Args:
                 input: SensoryInput object containing sensor data
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current directly into a specific neuron.
             
             Args:
                 neuron: Target NeuronId to inject current into
                 current: Current value to inject (in nanoamps)
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: NeuronType specifying which neurons to target
                 current: Current value to inject (in nanoamps)
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity.
             
             Analyzes motor neuron population activity and generates appropriate
             motor actions based on the highest firing motor neuron groups.
             
             Returns:
                 Action: Generated motor action with type and parameters
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial conditions.
             
             This method resets all neural populations, memory systems, and
             internal state variables to their initial values.
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file (checkpointing).
             
             Args:
                 filepath: Path to save the brain state to
                 
             Returns:
                 bool: True if save was successful, False otherwise
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file.
             
             Args:
                 filepath: Path to load brain state from
                 
             Returns:
                 bool: True if load was successful, False otherwise
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region to the brain.
             
             Args:
                 name: Optional name for the region (default: "")
                 
             Returns:
                 RegionId: Unique identifier for the newly created region
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a neural region by its ID.
             
             Args:
                 id: RegionId of the region to retrieve
                 
             Returns:
                 NeuralRegion*: Pointer to the neural region, or nullptr if not found
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions in the brain.
             
             Returns:
                 size_t: Number of neural regions
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs in the brain.
             
             Returns:
                 std::vector<RegionId>: Vector of all region IDs
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all neural regions in the brain.
             
             Returns:
                 const std::vector<std::unique_ptr<NeuralRegion>>&: All neural regions
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all regions.
             
             Returns:
                 size_t: Total number of neurons in the brain
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count across all regions.
             
             Returns:
                 size_t: Total number of synapses in the brain
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get count of active neurons (above firing threshold).
             
             Returns:
                 size_t: Number of currently active neurons
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons (spiking this step).
             
             Returns:
                 size_t: Number of neurons that fired in the current step
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons.
             
             Returns:
                 float: Average firing rate (spikes per second)
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio.
             
             Returns:
                 float: Ratio of total excitatory to inhibitory synaptic weights.
                        Higher values indicate more excitation dominance.
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count since brain initialization.
             
             Returns:
                 size_t: Cumulative number of spikes across all neurons
             )pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             R"pbdoc(
             Get count of pending spike events (both immediate and delayed).
             
             Returns:
                 size_t: Number of unprocessed spike events
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage (Initial, CriticalPeriod, etc.)
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
             Set developmental stage.
             
             Args:
                 stage: DevelopmentalStage to set
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the configuration object.
             
             Returns:
                 std::shared_ptr<const Config>: Configuration object reference
             )pbdoc")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the spike system.
             
             Returns:
                 SpikeSystem*: Pointer to the spike system component
             )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the STDP plasticity system.
             
             Returns:
                 STDP*: Pointer to the STDP plasticity component
             )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the Hebbian plasticity system.
             
             Returns:
                 Hebbian*: Pointer to the Hebbian plasticity component
             )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the structural plasticity system.
             
             Returns:
                 StructuralPlasticity*: Pointer to the structural plasticity component
             )pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the working memory system.
             
             Returns:
                 NeuralWorkingMemory*: Pointer to the working memory component
             )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the episodic memory system.
             
             Returns:
                 NeuralEpisodicMemory*: Pointer to the episodic memory component
             )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the associative memory system.
             
             Returns:
                 NeuralAssociativeMemory*: Pointer to the associative memory component
             )pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the prediction system.
             
             Returns:
                 PredictionSystem*: Pointer to the prediction component
             )pbdoc")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the neural planner system.
             
             Returns:
                 NeuralPlanner*: Pointer to the planner component
             )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the concept formation system.
             
             Returns:
                 ConceptFormation*: Pointer to the concept formation component
             )pbdoc")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the attentional selection system.
             
             Returns:
                 AttentionalSelection*: Pointer to the attentional selection component
             )pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the development system.
             
             Returns:
                 DevelopmentSystem*: Pointer to the development system component
             )pbdoc")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the dopamine neuromodulation system.
             
             Returns:
                 Dopamine*: Pointer to the dopamine neuromodulation component
             )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the curiosity neuromodulation system.
             
             Returns:
                 Curiosity*: Pointer to the curiosity neuromodulation component
             )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the novelty neuromodulation system.
             
             Returns:
                 Novelty*: Pointer to the novelty neuromodulation component
             )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the prediction error neuromodulation system.
             
             Returns:
                 PredictionError*: Pointer to the prediction error component
             )pbdoc")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get access to the random number generator.
             
             Returns:
                 RandomGenerator*: Pointer to the random number generator component
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status and statistics to the logger.
             
             This method outputs comprehensive information about the current state of the brain,
             including region statistics, memory system status, and neuromodulation levels.
             """)
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             R"pbdoc(
             Apply neuromodulatory signals to the brain.
             
             Args:
                 signal: Neuromodulator object containing neuromodulatory signal
             )pbdoc");

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

    // SPARK SYSTEM BINDINGS
    
    py::class_<SpikeEvent>(m, "SpikeEvent", R"pbdoc(
        Spike event for event-driven neural computation.
        
        Represents a spike generated by a neuron at a specific time and simulation step.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, Timestamp, SimulationStep>(),
             py::arg("source_neuron"), py::arg("timestamp"), py::arg("step"))
        .def_readwrite("source_neuron", &SpikeEvent::source_neuron)
        .def_readwrite("timestamp", &SpikeEvent::timestamp)
        .def_readwrite("step", &SpikeEvent::step)
        .def("__repr__", [](const SpikeEvent& event) {
            return "<SpikeEvent: source=" + std::to_string(event.source_neuron.value) +
                   ", timestamp=" + std::to_string(event.timestamp) +
                   ", step=" + std::to_string(event.step) + ">";
        });
    
    py::class_<DelayedSpikeEvent>(m, "DelayedSpikeEvent", R"pbdoc(
        Delayed spike event for synaptic transmission with delay.
        
        Represents a spike that will be delivered after a delay, containing
        all necessary information for synaptic transmission including source,
        destination, weight, and timing information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, NeuronId, SynapseId, SynapticWeight, SynapseType, 
                      Timestamp, Timestamp, SimulationStep, SimulationStep>(),
             py::arg("source_neuron"), py::arg("destination_neuron"), 
             py::arg("synapse_id"), py::arg("weight"), py::arg("synapse_type"),
             py::arg("timestamp"), py::arg("delivery_time"),
             py::arg("step"), py::arg("delivery_step"))
        .def_readwrite("source_neuron", &DelayedSpikeEvent::source_neuron)
        .def_readwrite("destination_neuron", &DelayedSpikeEvent::destination_neuron)
        .def_readwrite("synapse_id", &DelayedSpikeEvent::synapse_id)
        .def_readwrite("weight", &DelayedSpikeEvent::weight)
        .def_readwrite("synapse_type", &DelayedSpikeEvent::synapse_type)
        .def_readwrite("timestamp", &DelayedSpikeEvent::timestamp)
        .def_readwrite("delivery_time", &DelayedSpikeEvent::delivery_time)
        .def_readwrite("step", &DelayedSpikeEvent::step)
        .def_readwrite("delivery_step", &DelayedSpikeEvent::delivery_step)
        .def_readwrite("is_excitatory", &DelayedSpikeEvent::is_excitatory)
        .def("__repr__", [](const DelayedSpikeEvent& event) {
            return "<DelayedSpikeEvent: " + std::to_string(event.source_neuron.value) +
                   " -> " + std::to_string(event.destination_neuron.value) +
                   ", weight=" + std::to_string(event.weight) +
                   ", delivery_time=" + std::to_string(event.delivery_time) + ">";
        });
    
    py::class_<DetailedSpikeEvent>(m, "DetailedSpikeEvent", R"pbdoc(
        Detailed spike event with additional metadata.
        
        Enhanced spike event containing neuron identification and simulation
        context information including region and population IDs for comprehensive
        neural event tracking and analysis.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<NeuronId, Timestamp, SimulationStep, RegionId, PopulationId>(),
             py::arg("source"), py::arg("timestamp"), py::arg("step"),
             py::arg("region_id"), py::arg("population_id"))
        .def_readwrite("source", &DetailedSpikeEvent::source)
        .def_readwrite("timestamp", &DetailedSpikeEvent::timestamp)
        .def_readwrite("step", &DetailedSpikeEvent::step)
        .def_readwrite("region_id", &DetailedSpikeEvent::regionId)
        .def_readwrite("population_id", &DetailedSpikeEvent::populationId)
        .def("__repr__", [](const DetailedSpikeEvent& event) {
            return "<DetailedSpikeEvent: source=" + std::to_string(event.source.value) +
                   ", region_id=" + std::to_string(event.regionId.value) +
                   ", population_id=" + std::to_string(event.populationId.value) +
                   ", timestamp=" + std::to_string(event.timestamp) +
                   ", step=" + std::to_string(event.step) + ">";
        });
    
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(
        Event-driven neural computation system that handles spike event processing
        and delayed synaptic transmission.
        
        The SpikeSystem is a core component of the NLM (Neural Learning Machine)
        framework that provides event-driven programming interfaces with spike
        handlers and delayed spike handlers for real-time neural simulation.
        
        Key features:
        - Handles spike event processing and delayed synaptic transmission
        - Provides event-driven programming interfaces with spike handlers
        - Includes statistics and analysis methods
        - Has reset and history management
    )pbdoc")
        .def(py::init<>())
        .def("queueSpike", 
             static_cast<void (SpikeSystem::*)(const SpikeEvent&)>(&SpikeSystem::queueSpike),
             py::arg("event"),
             R"pbdoc(
             Queue a spike event for processing.
             
             Args:
                 event: SpikeEvent object containing source, timestamp, and step information
             )pbdoc")
        .def("queueSpike",
             static_cast<void (SpikeSystem::*)(NeuronId, Timestamp, SimulationStep)>(&SpikeSystem::queueSpike),
             py::arg("neuron"), py::arg("timestamp"), py::arg("step"),
             R"pbdoc(
             Queue a spike event from a specific neuron.
             
             Args:
                 neuron: NeuronId of the neuron generating the spike
                 timestamp: Time when the spike occurred (seconds)
                 step: Simulation step when the spike occurred
             )pbdoc")
        .def("queueDelayedSpike",
             &SpikeSystem::queueDelayedSpike,
             py::arg("event"),
             R"pbdoc(
             Queue a delayed spike event for synaptic transmission.
             
             Args:
                 event: DelayedSpikeEvent object containing destination,
                        weight, and delivery timing information
             )pbdoc")
        .def("processSpikes",
             &SpikeSystem::processSpikes,
             py::arg("currentStep"),
             R"pbdoc(
             Process all queued spikes for the current simulation step.
             
             Args:
                 currentStep: Current simulation step number
             )pbdoc")
        .def("processDelayedSpikes",
             &SpikeSystem::processDelayedSpikes,
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Process pending delayed spikes that should be delivered now.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
             )pbdoc")
        .def("registerHandler",
             &SpikeSystem::registerHandler,
             py::arg("handler"),
             R"pbdoc(
             Register a spike handler callback (called when spikes are generated).
             
             Args:
                 handler: Function that takes a DetailedSpikeEvent parameter
                          and is called when a spike is processed
             )pbdoc")
        .def("registerDelayedHandler",
             &SpikeSystem::registerDelayedHandler,
             py::arg("handler"),
             R"pbdoc(
             Register a delayed spike handler (called when delayed spikes are delivered).
             
             Args:
                 handler: Function that takes a DelayedSpikeEvent parameter
                          and is called when a delayed spike is delivered
             )pbdoc")
        .def("getSpikeHistory",
             &SpikeSystem::getSpikeHistory,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get spike history as a vector of DetailedSpikeEvent objects.
             
             Returns:
                 List of DetailedSpikeEvent objects representing all recorded spikes
             )pbdoc")
        .def("clearHistory",
             &SpikeSystem::clearHistory,
             R"pbdoc(
             Clear the spike history.
             
             Removes all recorded spike events from memory.
             )pbdoc")
        .def("getSpikeCount",
             &SpikeSystem::getSpikeCount,
             R"pbdoc(
             Get total spike count since system initialization.
             
             Returns:
                 Total number of spikes processed
             )pbdoc")
        .def("getPendingSpikeCount",
             &SpikeSystem::getPendingSpikeCount,
             R"pbdoc(
             Get number of pending immediate spike events.
             
             Returns:
                 Number of unprocessed spike events
             )pbdoc")
        .def("getPendingDelayedCount",
             &SpikeSystem::getPendingDelayedCount,
             R"pbdoc(
             Get number of pending delayed spike events.
             
             Returns:
                 Number of delayed spike events waiting to be delivered
             )pbdoc")
        .def("getAverageSpikeRate",
             &SpikeSystem::getAverageSpikeRate,
             R"pbdoc(
             Get average spike rate across all neurons.
             
             Returns:
                 Average firing rate in spikes per second
             )pbdoc")
        .def("getMostActiveNeurons",
             &SpikeSystem::getMostActiveNeurons,
             py::arg("count"),
             R"pbdoc(
             Get the most active neurons by spike count.
             
             Args:
                 count: Number of most active neurons to return
                 
             Returns:
                 List of NeuronId objects sorted by activity level
             )pbdoc")
        .def("reset",
             &SpikeSystem::reset,
             R"pbdoc(
             Reset the spike system to initial state.
             
             This method clears all pending events and resets statistics,
             returning the system to its initial configuration.
             )pbdoc")
        .def("__repr__", [](const SpikeSystem& system) {
            return "<SpikeSystem: spikes=" + std::to_string(system.getSpikeCount()) +
                   ", pending_immediate=" + std::to_string(system.getPendingSpikeCount()) +
                   ", pending_delayed=" + std::to_string(system.getPendingDelayedCount()) + ">";
        });

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
