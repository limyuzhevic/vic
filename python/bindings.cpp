// NLM Python Bindings - pybind11 C++ to Python bindings
// 
// This file contains pybind11 bindings for all NLM neural components.
// It exposes the C++ neural simulation framework to Python with
// comprehensive Pythonic interfaces and advanced features.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

// Include all neural component headers
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralRegion.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/core/Types/Types.hpp"
#include "src/core/Random/Random.hpp"

namespace py = pybind11;
namespace nlm = nlm;

// Helper functions for Pythonic conversions
inline std::vector<float> float_array_to_vector(const std::array<float, 3>& arr) {
    return std::vector<float>(arr.begin(), arr.end());
}

inline std::vector<double> double_array_to_vector(const std::array<double, 3>& arr) {
    return std::vector<double>(arr.begin(), arr.end());
}

// Neuron class with comprehensive Python bindings
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        
        This module provides Python bindings for the NLM C++ neural simulation framework.
        It exposes all core neural components with comprehensive Pythonic interfaces.
        
        Key Features:
        - Complete LIF neuron dynamics with full method bindings
        - Synapse management with weight control and plasticity
        - NeuralRegion and NeuralPopulation with statistics and bulk operations
        - Neuromodulator system (Dopamine, Acetylcholine, Norepinephrine, Serotonin)
        - Context managers for resource management
        - Pythonic operators (__getitem__, __iter__, etc.)
        - Property-based access for common attributes
        - Vector conversions for standard containers
        - Batch operations for efficiency
        - Statistics utilities and factory functions
        
        Usage Example:
            from pynlm import Neuron, Synapse, NeuralRegion, NeuralPopulation
            from pynlm import NeuronType, SynapseType
            
            # Create neural components
            neuron = Neuron(1)
            neuron.type = NeuronType.Excitatory
            neuron.V = -65.0
            
            region = NeuralRegion(1, "hippocampus")
            population = region.addPopulation(100, NeuronType.Excitatory)
            
            # Simulate
            for t in range(100):
                neuron.stepLIF(t * 0.001, 0.001)
                region.step(t * 0.001)
    )pbdoc";

    // Register exception classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId bindings
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

    // SynapseId bindings
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

    // RegionId bindings
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=);

    // PopulationId bindings
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum bindings
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

    // PlasticityFlags bindings
    py::class_<PlasticityFlags>(m, "PlasticityFlags", R"pbdoc(Synaptic plasticity state flags)pbdoc")
        .def(py::init<>())
        .def_readwrite("hebbian", &PlasticityFlags::hebbian)
        .def_readwrite("stdp", &PlasticityFlags::stdp)
        .def_readwrite("reward_modulated", &PlasticityFlags::reward_modulated)
        .def_readwrite("structural", &PlasticityFlags::structural)
        .def_readwrite("eligible", &PlasticityFlags::eligible);

    // Comprehensive Neuron class bindings
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Leaky Integrate-and-Fire (LIF) neuron model)")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def(py::init<NeuronId, NeuronType>(), py::arg("id"), py::arg("type"))
        .def(py::init<NeuronId, NeuronType, double>(), py::arg("id"), py::arg("type"), py::arg("resting_potential"))
        .def(py::init<const Neuron&>())  // Copy constructor
        .def(py::init<Neuron&&>())  // Move constructor
        .def(py::self == py::self)
        .def(py::self != py::self)

        // Core properties (read-write)
        .def_property("type", &Neuron::getType, &Neuron::setType, "Neuron type (read-write)")
        .def_property("membrane_potential", &Neuron::getMembranePotential, &Neuron::setMembranePotential, "Membrane potential (mV)")
        .def_property("threshold", &Neuron::getThreshold, &Neuron::setThreshold, "Firing threshold (mV)")
        .def_property("resting_potential", &Neuron::getRestingPotential, &Neuron::setRestingPotential, "Resting potential (mV)")
        .def_property("reset_potential", &Neuron::getResetPotential, &Neuron::setResetPotential, "Reset potential (mV)")
        .def_property("leak_conductance", &Neuron::getLeakConductance, &Neuron::setLeakConductance, "Leak conductance (nS)")
        .def_property("refractory_period", &Neuron::getRefractoryPeriod, &Neuron::setRefractoryPeriod, "Refractory period (steps)")

        // Read-only properties
        .def("get_id", &Neuron::getId, "Get neuron identifier")
        .def("is_firing", &Neuron::isFiring, "Check if neuron is firing")
        .def("is_refractory", &Neuron::isRefractory, "Check if neuron is in refractory period")
        .def("get_firing_rate", &Neuron::getFiringRate, "Get firing rate (Hz)")
        .def("get_last_spike_time", &Neuron::getLastSpikeTime, "Get last spike time")
        .def("get_spike_count", [](const Neuron& n) { return n.getSpikeHistory().size(); }, "Get number of recorded spikes")

        // LIF dynamics
        .def("check_threshold", &Neuron::checkThreshold, "Check if neuron has reached firing threshold")
        .def("step_lif", &Neuron::stepLIF, py::arg("current_time"), py::arg("dt"), "Step LIF dynamics, returns True if neuron fired")
        .def("step", &Neuron::step, py::arg("current_time"), "Step neuron with default timestep")

        // Input handling
        .def("receive_excitatory_input", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input")
        .def("receive_inhibitory_input", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input")
        .def("receive_modulatory_input", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("inject_current", &Neuron::injectCurrent, py::arg("current"), "Inject current from external source")
        .def("get_total_current", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clear_total_current", &Neuron::clearTotalCurrent, "Clear total current")

        // Spike history management
        .def("record_spike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike at given timestamp")
        .def("get_spike_history", &Neuron::getSpikeHistory, "Get spike history vector")
        .def("clear_spike_history", &Neuron::clearSpikeHistory, "Clear spike history")

        // Synapse management
        .def("add_incoming_synapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse handle")
        .def("add_outgoing_synapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse handle")
        .def("get_incoming_synapses", &Neuron::getIncomingSynapses, "Get incoming synapse handles")
        .def("get_outgoing_synapses", &Neuron::getOutgoingSynapses, "Get outgoing synapse handles")

        // Plasticity state
        .def("get_plasticity_flags", &Neuron::getPlasticityFlags, "Get plasticity flags (mutable)")
        .def("set_plasticity_flags", [](Neuron& n, const PlasticityFlags& flags) {
            n.getPlasticityFlags() = flags;
        }, py::arg("flags"), "Set plasticity flags")
        .def("enable_plasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"), "Enable plasticity rules")

        // Region/population membership
        .def("set_region_id", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("get_region_id", &Neuron::getRegionId, "Get region ID")
        .def("set_population_id", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("get_population_id", &Neuron::getPopulationId, "Get population ID")

        // State access
        .def("get_state", &Neuron::getState, "Get neuron state (mutable)")

        // Simulation control
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initialize_random", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters")

        // Context manager support
        .def("__enter__", [](Neuron& n) { return &n; })
        .def("__exit__", [](Neuron&, py::object, py::object, py::object) {})

        // Pythonic operators
        .def("__getitem__", [](Neuron& n, const std::string& key) {
            if (key == "V") return n.getMembranePotential();
            if (key == "threshold") return n.getThreshold();
            if (key == "firing_rate") return n.getFiringRate();
            if (key == "spike_count") return (int)n.getSpikeHistory().size();
            throw py::key_error("Invalid key for neuron: " + key);
        }, py::arg("key"), "Get neuron property using Pythonic syntax")

        .def("__setitem__", [](Neuron& n, const std::string& key, double value) {
            if (key == "V") n.setMembranePotential(value);
            else if (key == "threshold") n.setThreshold(value);
            else if (key == "firing_rate") n.setFiringRate(value);
            else throw py::key_error("Invalid key for neuron: " + key);
        }, py::arg("key"), py::arg("value"), "Set neuron property using Pythonic syntax")

        // Vector conversion
        .def("to_vector", [](const Neuron& n) {
            return std::vector<double>{n.getMembranePotential(), n.getThreshold(), n.getFiringRate()};
        }, "Convert neuron properties to vector")

        .def("as_dict", [](const Neuron& n) {
            py::dict d;
            d["id"] = n.getId();
            d["type"] = n.getType();
            d["V"] = n.getMembranePotential();
            d["threshold"] = n.getThreshold();
            d["firing_rate"] = n.getFiringRate();
            d["spike_count"] = n.getSpikeHistory().size();
            return d;
        }, "Convert neuron to dictionary representation")

        // String representation
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId().value) + 
                   " type=" + std::to_string(static_cast<int>(n.getType())) +
                   " V=" + std::to_string(n.getMembranePotential()) + "mV>";
        })

        // Batch operation helpers
        .def_static("batch_step", [](std::vector<Neuron*>& neurons, double current_time) {
            for (auto* n : neurons) {
                if (n) n->step(current_time);
            }
        }, py::arg("neurons"), py::arg("current_time"), "Step multiple neurons in batch")

        .def_static("batch_reset", [](std::vector<Neuron*>& neurons) {
            for (auto* n : neurons) {
                if (n) n->reset();
            }
        }, py::arg("neurons"), "Reset multiple neurons in batch")
;
    // Comprehensive Synapse class bindings
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons with plasticity)")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def(py::init<const Synapse&>())
        .def(py::init<Synapse&&>())
        .def(py::self == py::self)
        .def(py::self != py::self)

        // Core properties (read-write)
        .def_property("weight", &Synapse::getWeight, &Synapse::setWeight, "Synaptic weight (read-write)")
        .def_property("delay", &Synapse::getDelay, &Synapse::setDelay, "Synaptic delay (steps, read-write)")
        .def_property("type", &Synapse::getType, &Synapse::setType, "Synapse type (read-write)")

        // Read-only properties
        .def("get_id", &Synapse::getId, "Get synapse identifier")
        .def("get_source_neuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("get_destination_neuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("is_excitatory", &Synapse::isExcitatory, "Check if synapse is excitatory")
        .def("is_inhibitory", &Synapse::isInhibitory, "Check if synapse is inhibitory")
        .def("get_pre_spike_count", [](const Synapse& s) { return s.getPreSpikeHistory().size(); }, "Get pre-synaptic spike count")
        .def("get_post_spike_count", [](const Synapse& s) { return s.getPostSpikeHistory().size(); }, "Get post-synaptic spike count")

        // Spike history for plasticity
        .def("record_pre_spike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record pre-synaptic spike")
        .def("record_post_spike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record post-synaptic spike")
        .def("get_pre_spike_history", &Synapse::getPreSpikeHistory, "Get pre-synaptic spike history")
        .def("get_post_spike_history", &Synapse::getPostSpikeHistory, "Get post-synaptic spike history")
        .def("clear_history", &Synapse::clearHistory, "Clear spike history")

        // Plasticity flags
        .def("get_plasticity_flags", &Synapse::getPlasticityFlags, "Get plasticity flags (mutable)")
        .def("set_plasticity_flags", [](Synapse& s, const PlasticityFlags& flags) {
            s.getPlasticityFlags() = flags;
        }, py::arg("flags"), "Set plasticity flags")
        .def("enable_plasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"), "Enable plasticity rules")

        // Eligibility trace for reward-modulated learning
        .def_property("eligibility_trace", &Synapse::getEligibilityTrace, &Synapse::setEligibilityTrace, "Eligibility trace (read-write)")
        .def("decay_eligibility_trace", &Synapse::decayEligibilityTrace, py::arg("decay_rate"), "Decay eligibility trace")

        // Synaptic efficacy
        .def_property("efficacy", &Synapse::getEfficacy, &Synapse::setEfficacy, "Synaptic efficacy (read-write)")

        // Short-term plasticity
        .def("get_short_term_depression", &Synapse::get_short_term_depression, "Get short-term depression factor")
        .def("get_short_term_facilitation", &Synapse::get_short_term_facilitation, "Get short-term facilitation factor")

        // Simulation step
        .def("step", &Synapse::step, py::arg("current_time"), "Step synapse dynamics")

        // State management
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initialize_random", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters")

        // Context manager support
        .def("__enter__", [](Synapse& s) { return &s; })
        .def("__exit__", [](Synapse&, py::object, py::object, py::object) {})

        // Pythonic operators
        .def("__getitem__", [](Synapse& s, const std::string& key) {
            if (key == "weight") return s.getWeight();
            if (key == "delay") return s.getDelay();
            if (key == "type") return s.getType();
            if (key == "pre_spike_count") return (int)s.getPreSpikeHistory().size();
            if (key == "post_spike_count") return (int)s.getPostSpikeHistory().size();
            throw py::key_error("Invalid key for synapse: " + key);
        }, py::arg("key"), "Get synapse property using Pythonic syntax")

        .def("__setitem__", [](Synapse& s, const std::string& key, double value) {
            if (key == "weight") s.setWeight(value);
            else if (key == "delay") s.setDelay(static_cast<Delay>(value));
            else if (key == "type") s.setType(static_cast<SynapseType>(static_cast<int>(value)));
            else throw py::key_error("Invalid key for synapse: " + key);
        }, py::arg("key"), py::arg("value"), "Set synapse property using Pythonic syntax")

        // Vector conversion
        .def("to_vector", [](const Synapse& s) {
            return std::vector<double>{s.getWeight(), s.getDelay(), s.getEfficacy()};
        }, "Convert synapse properties to vector")

        .def("as_dict", [](const Synapse& s) {
            py::dict d;
            d["id"] = s.getId();
            d["source"] = s.getSourceNeuron();
            d["destination"] = s.getDestinationNeuron();
            d["weight"] = s.getWeight();
            d["delay"] = s.getDelay();
            d["type"] = s.getType();
            d["pre_spike_count"] = s.getPreSpikeHistory().size();
            d["post_spike_count"] = s.getPostSpikeHistory().size();
            d["eligibility_trace"] = s.getEligibilityTrace();
            d["efficacy"] = s.getEfficacy();
            return d;
        }, "Convert synapse to dictionary representation")

        // String representation
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) +
                   " source=" + std::to_string(s.getSourceNeuron().value) +
                   " dest=" + std::to_string(s.getDestinationNeuron().value) +
                   " weight=" + std::to_string(s.getWeight()) + ">";
        })

        // Batch operation helpers
        .def_static("batch_step", [](std::vector<Synapse*>& synapses, double current_time) {
            for (auto* s : synapses) {
                if (s) s->step(current_time);
            }
        }, py::arg("synapses"), py::arg("current_time"), "Step multiple synapses in batch")

        .def_static("batch_reset", [](std::vector<Synapse*>& synapses) {
            for (auto* s : synapses) {
                if (s) s->reset();
            }
        }, py::arg("synapses"), "Reset multiple synapses in batch")
;
    // Comprehensive NeuralRegion class bindings
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing populations and local connectivity)")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, const std::string&>(), py::arg("id"), py::arg("name"))
        .def(py::init<const NeuralRegion&>())
        .def(py::init<NeuralRegion&&>())
        .def(py::self == py::self)
        .def(py::self != py::self)

        // Core properties (read-write)
        .def_property("name", &NeuralRegion::getName, &NeuralRegion::setName, "Region name (read-write)")

        // Core methods
        .def("get_id", &NeuralRegion::getId, "Get region identifier")
        .def("get_name", &NeuralRegion::getName, "Get region name")
        .def("set_name", &NeuralRegion::setName, py::arg("name"), "Set region name")

        // Population management
        .def("add_population", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal, "Add new population")
        .def("get_population", [](NeuralRegion& r, PopulationId id) {
            return r.getPopulation(id);
        }, py::arg("id"), "Get population by ID (returns None if not found)")
        .def("get_population_count", &NeuralRegion::getPopulationCount, "Get number of populations")
        .def("get_populations", &NeuralRegion::getPopulations, "Get all populations (read-only)")
        .def("get_all_populations", &NeuralRegion::getAllPopulations, "Get all population pointers")

        // Synapse management
        .def("add_synapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add synapse")
        .def("get_synapse", [](NeuralRegion& r, SynapseId id) {
            return r.getSynapse(id);
        }, py::arg("id"), "Get synapse by ID (returns None if not found)")
        .def("get_synapse_count", &NeuralRegion::getSynapseCount, "Get number of synapses")
        .def("get_synapses", &NeuralRegion::getSynapses, "Get all synapses (read-only)")

        // Connectivity queries
        .def("get_synapses_from", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get synapses originating from neuron")
        .def("get_synapses_to", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get synapses targeting neuron")

        // Statistics
        .def("get_total_neuron_count", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("get_active_neuron_count", &NeuralRegion::getActiveNeuronCount, "Get active neuron count")
        .def("get_firing_neuron_count", &NeuralRegion::getFiringNeuronCount, "Get firing neuron count")
        .def("get_average_firing_rate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("get_average_synaptic_weight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("get_synaptic_density", &NeuralRegion::getSynapticDensity, "Get synaptic density")

        // Simulation control
        .def("step", &NeuralRegion::step, py::arg("current_time"), "Step all neurons and synapses")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")

        // Initialization
        .def("initialize_random_connectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connection_probability"), py::arg("mean_weight"), py::arg("weight_variance"),
             "Initialize random connectivity")

        // Neuron access
        .def("get_all_neurons", &NeuralRegion::getAllNeurons, "Get all neurons across populations")

        // Context manager support
        .def("__enter__", [](NeuralRegion& r) { return &r; })
        .def("__exit__", [](NeuralRegion&, py::object, py::object, py::object) {})

        // Vector conversion
        .def("as_dict", [](const NeuralRegion& r) {
            py::dict d;
            d["id"] = r.getId();
            d["name"] = r.getName();
            d["population_count"] = r.getPopulationCount();
            d["synapse_count"] = r.getSynapseCount();
            d["neuron_count"] = r.getTotalNeuronCount();
            d["active_count"] = r.getActiveNeuronCount();
            d["firing_count"] = r.getFiringNeuronCount();
            d["average_firing_rate"] = r.getAverageFiringRate();
            d["synaptic_density"] = r.getSynapticDensity();
            return d;
        }, "Convert region to dictionary representation")

        // Pythonic operators
        .def("__getitem__", [](NeuralRegion& r, const std::string& key) {
            if (key == "name") return r.getName();
            if (key == "population_count") return r.getPopulationCount();
            if (key == "synapse_count") return r.getSynapseCount();
            if (key == "neuron_count") return r.getTotalNeuronCount();
            throw py::key_error("Invalid key for region: " + key);
        }, py::arg("key"), "Get region property using Pythonic syntax")
;
    // Comprehensive NeuralPopulation class bindings
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def(py::init<const NeuralPopulation&>())
        .def(py::init<NeuralPopulation&&>())
        .def(py::self == py::self)
        .def(py::self != py::self)

        // Core properties (read-write)
        .def_property("neuron_type", &NeuralPopulation::getNeuronType, &NeuralPopulation::setNeuronType, "Neuron type (read-write)")

        // Core methods
        .def("get_id", &NeuralPopulation::getId, "Get population identifier")
        .def("get_size", &NeuralPopulation::getSize, "Get population size")
        .def("is_empty", &NeuralPopulation::isEmpty, "Check if population is empty")

        // Neuron access
        .def("get_neuron", &NeuralPopulation::getNeuron, py::arg("index"), "Get neuron by index")
        .def("get_neuron_ids", &NeuralPopulation::getNeuronIds, "Get all neuron IDs")

        // Statistics
        .def("get_active_count", &NeuralPopulation::getActiveCount, "Get active neuron count")
        .def("get_firing_count", &NeuralPopulation::getFiringCount, "Get firing neuron count")
        .def("get_average_firing_rate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("get_activity_level", &NeuralPopulation::getActivityLevel, "Get activity level (fraction active)")

        // Activity tracking
        .def("record_activity", &NeuralPopulation::recordActivity, "Record current activity state")
        .def("get_mean_membrane_potential", &NeuralPopulation::getMeanMembranePotential, "Get mean membrane potential")
        .def("get_variance_membrane_potential", &NeuralPopulation::getVarianceMembranePotential, "Get variance of membrane potentials")

        // Simulation control
        .def("step", &NeuralPopulation::step, py::arg("current_time"), "Step all neurons")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("initialize_random", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize all neurons randomly")

        // Context manager support
        .def("__enter__", [](NeuralPopulation& p) { return &p; })
        .def("__exit__", [](NeuralPopulation&, py::object, py::object, py::object) {})

        // Vector conversion
        .def("as_dict", [](const NeuralPopulation& p) {
            py::dict d;
            d["id"] = p.getId();
            d["size"] = p.getSize();
            d["neuron_type"] = p.getNeuronType();
            d["active_count"] = p.getActiveCount();
            d["firing_count"] = p.getFiringCount();
            d["activity_level"] = p.getActivityLevel();
            d["average_firing_rate"] = p.getAverageFiringRate();
            d["mean_membrane_potential"] = p.getMeanMembranePotential();
            return d;
        }, "Convert population to dictionary representation")

        // Pythonic operators
        .def("__getitem__", [](NeuralPopulation& p, size_t index) {
            return p.getNeuron(index);
        }, py::arg("index"), "Get neuron by index using Pythonic syntax")

        .def("__len__", &NeuralPopulation::getSize, "Get population size for len()")

        .def("__iter__", [](NeuralPopulation& p) {
            return py::make_iterator(p.getNeurons().begin(), p.getNeurons().end());
        }, "Iterate over neurons in population")
;
    // Neuromodulator base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)")
        .def("get_name", &Neuromodulator::getName, "Get modulator name")
        .def("get_level", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("set_level", &Neuromodulator::setLevel, py::arg("level"), "Set concentration/level")
        .def("get_plasticity_factor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Novelty neuromodulator
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection signal)")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("get_level", &Novelty::getLevel, "Get novelty level")
        .def("set_level", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detect_novelty", [](Novelty& n, const class Observation& obs1, const class Observation& obs2) {
            n.detectNovelty(obs1, obs2);
        }, py::arg("current_observation"), py::arg("previous_observation"), "Detect novelty from observations")
        .def("detect_novelty_pattern", &Novelty::detectNovelty, py::arg("current_pattern"), py::arg("previous_pattern"), "Detect novelty from patterns")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time")
        .def("get_history", &Novelty::getHistory, "Get novelty history")
        .def("clear_history", &Novelty::clearHistory, "Clear novelty history");

    // Curiosity neuromodulator
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive: exploration motivation)")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("get_level", &Curiosity::getLevel, "Get curiosity level")
        .def("get_exploration_drive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("prediction_error"), py::arg("dt"), "Update curiosity based on inputs")
        .def("set_novelty_weight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("set_prediction_error_weight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity");

    // Reward neuromodulator
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)")
        .def(py::init<>())
        .def("get_value", &Reward::getValue, "Get current reward value")
        .def("set_value", &Reward::setValue, py::arg("value"), "Set reward value")
        .def("add", &Reward::add, py::arg("delta"), "Accumulate reward")
        .def("reset", &Reward::reset, "Reset accumulated reward")
        .def("get_history", &Reward::getHistory, "Get reward history")
        .def("clear_history", &Reward::clearHistory, "Clear reward history");

    // Prediction error neuromodulator
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("get_error", &PredictionError::getError, "Get error value")
        .def("compute_error", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("update_prediction", &PredictionError::updatePrediction, py::arg("new_prediction"), "Update prediction")
        .def("get_history", &PredictionError::getHistory, "Get error history")
        .def("clear_history", &PredictionError::clearHistory, "Clear error history")
        .def("get_magnitude", &PredictionError::getMagnitude, "Get error magnitude");

    // Factory functions
    m.def("create_neuron", [](NeuronId id, NeuronType type) {
        return Neuron(id, type);
    }, py::arg("id"), py::arg("type"), "Create neuron with specified type");

    m.def("create_synapse", [](SynapseId id, NeuronId source, NeuronId destination) {
        return Synapse(id, source, destination);
    }, py::arg("id"), py::arg("source"), py::arg("destination"), "Create synapse");

    m.def("create_region", [](RegionId id, const std::string& name) {
        return NeuralRegion(id, name);
    }, py::arg("id"), py::arg("name") = "", "Create neural region");

    m.def("create_population", [](PopulationId id, size_t size, NeuronType type) {
        return NeuralPopulation(id, size);
    }, py::arg("id"), py::arg("size"), py::arg("type") = NeuronType::Internal, "Create neural population");

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Batch operation utilities
    m.def("batch_step_neurons", [](std::vector<Neuron*>& neurons, double current_time) {
        for (auto* n : neurons) {
            if (n) n->step(current_time);
        }
    }, py::arg("neurons"), py::arg("current_time"), "Step multiple neurons in batch");

    m.def("batch_step_synapses", [](std::vector<Synapse*>& synapses, double current_time) {
        for (auto* s : synapses) {
            if (s) s->step(current_time);
        }
    }, py::arg("synapses"), py::arg("current_time"), "Step multiple synapses in batch");

    m.def("batch_get_potential", [](const std::vector<const Neuron*>& neurons) {
        std::vector<double> potentials;
        potentials.reserve(neurons.size());
        for (const auto* n : neurons) {
            if (n) potentials.push_back(n->getMembranePotential());
        }
        return potentials;
    }, py::arg("neurons"), "Get membrane potentials for multiple neurons");

    m.def("batch_get_spike_counts", [](const std::vector<const Neuron*>& neurons) {
        std::vector<size_t> counts;
        counts.reserve(neurons.size());
        for (const auto* n : neurons) {
            if (n) counts.push_back(n->getSpikeHistory().size());
        }
        return counts;
    }, py::arg("neurons"), "Get spike counts for multiple neurons");

    m.def("batch_get_weights", [](const std::vector<const Synapse*>& synapses) {
        std::vector<double> weights;
        weights.reserve(synapses.size());
        for (const auto* s : synapses) {
            if (s) weights.push_back(s->getWeight());
        }
        return weights;
    }, py::arg("synapses"), "Get weights for multiple synapses");

    m.def("simulate_batch", [](const std::vector<Neuron*>& neurons,
                              const std::vector<Synapse*>& synapses,
                              double current_time,
                              double dt = 0.001) {
        // Step neurons
        for (auto* n : neurons) {
            if (n) n->step(current_time);
        }
        // Step synapses
        for (auto* s : synapses) {
            if (s) s->step(current_time);
        }
    }, py::arg("neurons"), py::arg("synapses"), py::arg("current_time"), py::arg("dt") = 0.001,
       "Simulate batch of neurons and synapses");

    // Vector conversion utilities
    m.def("to_vector", [](const Neuron& n) {
        return std::vector<double>{n.getMembranePotential(), n.getThreshold(), n.getFiringRate()};
    }, "Convert neuron properties to vector");

    m.def("to_vector", [](const Synapse& s) {
        return std::vector<double>{s.getWeight(), s.getDelay(), s.getEfficacy()};
    }, "Convert synapse properties to vector");

    // Statistics utilities
    m.def("calculate_mean", [](const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = 0.0;
        for (double v : values) sum += v;
        return sum / values.size();
    }, "Calculate mean of values");

    m.def("calculate_variance", [](const std::vector<double>& values) {
        if (values.size() < 2) return 0.0;
        double mean = calculate_mean(values);
        double sum_squares = 0.0;
        for (double v : values) {
            double diff = v - mean;
            sum_squares += diff * diff;
        }
        return sum_squares / values.size();
    }, "Calculate variance of values");

    m.def("calculate_stddev", [](const std::vector<double>& values) {
        return std::sqrt(calculate_variance(values));
    }, "Calculate standard deviation of values");

    // Export constants
    m.attr("NEURON_TYPE_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_TYPE_INHIBITORY") = NeuronType::Inhibitory;
    m.attr("NEURON_TYPE_MODULATORY") = NeuronType::Modulatory;
    m.attr("NEURON_TYPE_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_TYPE_MOTOR") = NeuronType::Motor;
    m.attr("NEURON_TYPE_INTERNAL") = NeuronType::Internal;

    m.attr("SYNAPSE_TYPE_EXCITATORY") = SynapseType::Excitatory;
    m.attr("SYNAPSE_TYPE_INHIBITORY") = SynapseType::Inhibitory;
    m.attr("SYNAPSE_TYPE_MODULATORY") = SynapseType::Modulatory;
    m.attr("SYNAPSE_TYPE_ELECTRICAL") = SynapseType::Electrical;
    m.attr("SYNAPSE_TYPE_GAP_JUNCTION") = SynapseType::GapJunction;

    m.attr("FIRING_STATE_RESTING") = FiringState::Resting;
    m.attr("FIRING_STATE_ACTIVE") = FiringState::Active;
    m.attr("FIRING_STATE_REFRACTORY") = FiringState::Refractory;
    m.attr("FIRING_STATE_INHIBITED") = FiringState::Inhibited;

    m.attr("DEVELOPMENTAL_STAGE_INITIAL") = DevelopmentalStage::Initial;
    m.attr("DEVELOPMENTAL_STAGE_CRITICAL_PERIOD") = DevelopmentalStage::CriticalPeriod;
    m.attr("DEVELOPMENTAL_STAGE_MATURATION") = DevelopmentalStage::Maturation;
    m.attr("DEVELOPMENTAL_STAGE_ADULT") = DevelopmentalStage::Adult;
    m.attr("DEVELOPMENTAL_STAGE_AGING") = DevelopmentalStage::Aging;

    // Module metadata
    m.def("version", []() { return "0.1.0"; }, "Get NLM Python bindings version");

    m.def("features", []() {
        return std::vector<std::string>{
            "Full Neuron class with LIF dynamics",
            "Complete Synapse class with plasticity",
            "NeuralRegion with population management",
            "NeuralPopulation with statistics",
            "Neuromodulator system (Novelty, Curiosity, Reward, PredictionError)",
            "Context managers (__enter__/__exit__)",
            "Pythonic operators (__getitem__, __iter__)",
            "Property-based access",
            "Vector conversions",
            "Batch operations",
            "Statistics utilities"
        };
    }, "Get list of available features");

            m.def("examples", []() {
                return std::vector<std::string>{
                    "basic_example.py - Basic neural simulation",
                    "neuromodulator_example.py - Neuromodulation demonstration",
                    "population_example.py - Population dynamics",
                    "batch_operations_example.py - Batch operations",
                    "novelty_example.py - Novelty detection",
                    "curiosity_example.py - Curiosity drive",
                    "reward_example.py - Reward signals",
                    "prediction_error_example.py - Prediction error"
                };
            }, "Get list of example scripts");

    // Usage example
    m.def("example_usage", []() {
        return R"pbdoc(
            from pynlm import Neuron, Synapse, NeuralRegion, NeuralPopulation
            from pynlm import Novelty, Curiosity, Reward, PredictionError
            from pynlm import NeuronType, SynapseType
            
            # Create neural components
            region = NeuralRegion(RegionId(1), "hippocampus")
            neuron = region.add_neuron(NeuronType.Excitatory)
            population = region.addPopulation(100, NeuronType.Excitatory)
            
            # Use Pythonic API
            neuron.V = -65.0
            neuron.threshold = -55.0
            neuron.refractory_period = 10
            
            # Create synapse
            target_neuron = Neuron(NeuronId(2), NeuronType.Inhibitory)
            synapse = region.add_synapse(neuron, target_neuron, weight=0.5)
            
            # Create neuromodulators
            novelty = Novelty()
            curiosity = Curiosity()
            reward = Reward()
            prediction_error = PredictionError()
            
            # Detect novelty
            novelty.detectNovelty(observation, previous_observation)
            
            # Update curiosity
            curiosity.update(novelty.getLevel(), prediction_error.getError(), 0.001)
            
            # Update reward
            reward.add(1.0)
            
            # Compute prediction error
            prediction_error.computeError(0.5, 0.3)
            
            # Simulate
            for t in range(100):
                neuron.stepLIF(t * 0.001)
                region.step(t * 0.001)
                novelty.update(0.001)
                curiosity.update(novelty.getLevel(), prediction_error.getError(), 0.001)
        )pbdoc";
    }, "Get example usage string");

} // namespace nlm
