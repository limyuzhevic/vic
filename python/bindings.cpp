#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>

// Include all the missing headers
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/core/Types/Types.hpp"

// Include existing bindings
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// === UTILITY CLASSES ===

// Custom exception translation
class PyRuntimeError : public std::runtime_error {
public:
    PyRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyValueError : public std::runtime_error {
public:
    PyValueError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyMemoryError : public std::runtime_error {
public:
    PyMemoryError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyNotImplementedError : public std::runtime_error {
public:
    PyNotImplementedError(const std::string& msg) : std::runtime_error(msg) {}
};

// Context manager for resource management
class ResourceManager {
public:
    virtual ~ResourceManager() = default;
    virtual void cleanup() = 0;
};

// === CORE CLASSES ===

// Neuron class
void bindNeuron(py::module_& m) {
    py::class_<Neuron, std::shared_ptr<Neuron>>(m, "Neuron", R"pbdoc(
        Represents a single neuron in the neural network.
        
        Each neuron has membrane potential, threshold, and firing behavior.
        Implements Leaky Integrate-and-Fire (LIF) dynamics.
        
        Attributes:
            id: Unique identifier for the neuron
            type: Neuron type (Excitatory, Inhibitory, etc.)
            membranePotential: Current membrane potential in mV
            threshold: Firing threshold in mV
            isFiring: Whether neuron is currently firing
        
        Methods:
            stepLIF(currentTime, dt): Advance neuron dynamics
            reset(): Reset neuron to resting state
            addInput(amplitude): Add excitatory input
            addInhibitoryInput(amplitude): Add inhibitory input
        )pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"), "Create neuron with ID")
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential (mV)")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential (mV)")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential (mV)")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold (mV)")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold (mV)")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is refractory")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
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
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get last spike timestamp (returns -1 if never)")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "Advance neuron using LIF dynamics (returns True if fired)")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input (mV)")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input (mV)")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input (mV)")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject current into neuron (nA)")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current (nA)")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear injected current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike at timestamp")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history vector")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse handle")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse handle")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, "Get incoming synapse handles")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, "Get outgoing synapse handles")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, "Get plasticity flags")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"), "Advance neuron for one simulation step (Phase 2)")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize neuron with random parameters")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId().value) + ", type=" + std::to_string(static_cast<int>(n.getType())) + ", v=" + std::to_string(n.getMembranePotential()) + ">";
        }, "String representation of neuron")
        .def("__str__", [](const Neuron& n) {
            return "Neuron(" + std::to_string(n.getId().value) + ")";
        });
}

// Synapse class
void bindSynapse(py::module_& m) {
    py::class_<Synapse, std::shared_ptr<Synapse>>(m, "Synapse", R"pbdoc(
        Represents a synaptic connection between two neurons.
        
        Implements synaptic transmission with weight, delay, and short-term plasticity.
        
        Attributes:
            id: Unique identifier for the synapse
            sourceNeuron: Source neuron ID
            destinationNeuron: Destination neuron ID
            weight: Synaptic weight
            delay: Transmission delay in steps
            type: Synapse type (Excitatory, Inhibitory, etc.)
            isExcitatory: Whether synapse is excitatory
            isInhibitory: Whether synapse is inhibitory
        
        Methods:
            getId(): Get synapse ID
            getWeight(): Get synaptic weight
            setWeight(weight): Set synaptic weight
            addToWeight(delta): Add to synaptic weight
            getDelay(): Get transmission delay
            setDelay(delay): Set transmission delay
            recordPreSpike(timestamp): Record pre-synaptic spike time
            recordPostSpike(timestamp): Record post-synaptic spike time
            getEligibilityTrace(): Get eligibility trace for reward modulation
            decayEligibilityTrace(decayRate): Decay eligibility trace
        )pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"), "Create synapse with IDs")
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get transmission delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set transmission delay (steps)")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record pre-synaptic spike time")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record post-synaptic spike time")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get pre-synaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get post-synaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("setPreSpikeHistory", &Synapse::setPreSpikeHistory, py::arg("history"), "Set pre-synaptic spike history (for checkpointing)")
        .def("setPostSpikeHistory", &Synapse::setPostSpikeHistory, py::arg("history"), "Set post-synaptic spike history (for checkpointing)")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace (reward-modulated plasticity)")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy (use-dependent modulation)")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Advance synapse for one simulation step (Phase 2)")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize synapse with random parameters")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable plasticity rules")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) + ", w=" + std::to_string(s.getWeight()) + ", d=" + std::to_string(s.getDelay()) + ">";
        }, "String representation of synapse")
        .def("__str__", [](const Synapse& s) {
            return "Synapse(" + std::to_string(s.getId().value) + ")";
        });
}

// NeuralPopulation class
void bindNeuralPopulation(py::module_& m) {
    py::class_<NeuralPopulation, std::shared_ptr<NeuralPopulation>>(m, "NeuralPopulation", R"pbdoc(
        A group of neurons with shared properties and dynamics.
        
        Manages a collection of neurons, providing population-level statistics
        and operations. All neurons in a population share the same type.
        
        Attributes:
            id: Unique identifier for the population
            size: Number of neurons in the population
            neuronType: Type of neurons in this population
        
        Methods:
            getId(): Get population ID
            getSize(): Get number of neurons
            getNeuron(index): Get neuron at index
            addNeuron(neuron): Add neuron to population
            createNeurons(startId): Create neurons with sequential IDs
            getNeurons(): Get all neurons in population
            getActiveCount(): Get number of active neurons
            getFiringCount(): Get number of firing neurons
            getAverageFiringRate(): Get average firing rate
            initializeRandom(rng): Initialize all neurons randomly
            step(currentTime): Advance all neurons
            reset(): Reset all neurons
        )pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"), "Create population with ID and size")
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("getSize", &NeuralPopulation::getSize, "Get number of neurons")
        .def("isEmpty", &NeuralPopulation::isEmpty, "Check if population is empty")
        .def("getNeuron", py::overload_cast<NeuronIndex>(&NeuralPopulation::getNeuron), py::arg("index"), "Get neuron at index")
        .def("getNeuron", py::overload_cast<NeuronIndex>(&NeuralPopulation::getNeuron, py::const_overload), py::arg("index"), "Get neuron at index (const)")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population (caller retains ownership)")
        .def("createNeurons", &NeuralPopulation::createNeurons, py::arg("startId"), "Create neurons with sequential IDs")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons in population")
        .def("getNeuronType", &NeuralPopulation::getNeuronType, "Get neuron type for this population")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"), "Set neuron type for this population")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, "Get number of active neurons")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, "Get number of firing neurons")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate (Hz)")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel, "Get activity level (fraction of active neurons)")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize all neurons randomly")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Advance all neurons")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds, "Get neuron IDs in this population")
        .def("recordActivity", &NeuralPopulation::recordActivity, "Record current state for statistics")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential, "Get mean membrane potential across population")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential, "Get variance of membrane potential")
        .def("__len__", &NeuralPopulation::getSize, "Get population size")
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation id=" + std::to_string(p.getId().value) + ", size=" + std::to_string(p.getSize()) + ", type=" + std::to_string(static_cast<int>(p.getNeuronType())) + ">";
        }, "String representation of population")
        .def("__str__", [](const NeuralPopulation& p) {
            return "NeuralPopulation(" + std::to_string(p.getId().value) + ")";
        });
}

// NeuralRegion class
void bindNeuralRegion(py::module_& m) {
    py::class_<NeuralRegion, std::shared_ptr<NeuralRegion>>(m, "NeuralRegion", R"pbdoc(
        A brain region containing multiple populations and local connectivity.
        
        Manages neural populations, synapses, and local connectivity patterns.
        Implements region-specific dynamics and statistics.
        
        Attributes:
            id: Unique identifier for the region
            name: Human-readable name of the region
        
        Methods:
            getId(): Get region ID
            getName(): Get region name
            setName(name): Set region name
            addPopulation(size, type): Add neural population
            getPopulation(id): Get population by ID
            getPopulationCount(): Get number of populations
            getPopulations(): Get all populations
            addSynapse(source, destination, weight, delay): Add local synapse
            getSynapse(id): Get synapse by ID
            getSynapseCount(): Get number of synapses
            getSynapses(): Get all synapses
            getTotalNeuronCount(): Get total neurons in region
            getActiveNeuronCount(): Get active neurons
            getFiringNeuronCount(): Get firing neurons
            getAverageFiringRate(): Get average firing rate
            initializeRandomConnectivity(rng, prob, meanWeight, weightVariance): Initialize connectivity
            step(currentTime): Advance region
            reset(): Reset region
        )pbdoc")
        .def(py::init<RegionId>(), py::arg("id"), "Create region with ID")
        .def(py::init<RegionId, const std::string&>(), py::arg("id"), py::arg("name"), "Create region with ID and name")
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("setNextNeuronIndex", &NeuralRegion::setNextNeuronIndex, py::arg("next"), "Set next neuron index for allocation")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal, "Add neural population")
        .def("getPopulation", py::overload_cast<PopulationId>(&NeuralRegion::getPopulation), py::arg("id"), "Get population by ID")
        .def("getPopulation", py::overload_cast<PopulationId>(&NeuralRegion::getPopulation, py::const_overload), py::arg("id"), "Get population by ID (const)")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get number of populations")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations, "Get all populations (mutable)")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add local synapse")
        .def("getSynapse", py::overload_cast<SynapseId>(&NeuralRegion::getSynapse), py::arg("id"), "Get synapse by ID")
        .def("getSynapse", py::overload_cast<SynapseId>(&NeuralRegion::getSynapse, py::const_overload), py::arg("id"), "Get synapse by ID (const)")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get number of synapses")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get synapses originating from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get synapses targeting neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density (fraction)")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Advance all populations and synapses")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"), "Initialize connectivity")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons across populations")
        .def("getAllNeurons", py::overload_cast<const Neuron*>(&NeuralRegion::getAllNeurons, py::const_overload), "Get all neurons across populations (const)")
        .def("__len__", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion id=" + std::to_string(r.getId().value) + ", name=\"" + r.getName() + "\">";
        }, "String representation of region")
        .def("__str__", [](const NeuralRegion& r) {
            return "NeuralRegion(" + r.getName() + ")";
        });
}

// InterRegionConnection struct (from Brain.hpp)
void bindInterRegionConnection(py::module_& m) {
    py::class_<InterRegionConnection>(m, "InterRegionConnection", R"pbdoc(
        Represents a long-range connection between brain regions.
        
        Inter-regional connections enable communication between different
        neural regions, implementing global connectivity patterns.
        
        Attributes:
            sourceRegion: Source region ID
            targetRegion: Target region ID
            weight: Connection weight
            delay: Transmission delay in steps
            plasticityFlags: Plasticity rules to apply
        )pbdoc")
        .def(py::init<>(), "Default constructor")
        .def(py::init<RegionId, RegionId, float, Delay>(), py::arg("src"), py::arg("tgt"), py::arg("weight") = 0.0f, py::arg("delay") = 1, "Create inter-region connection")
        .def_readwrite("sourceRegion", &InterRegionConnection::sourceRegion)
        .def_readwrite("targetRegion", &InterRegionConnection::targetRegion)
        .def_readwrite("weight", &InterRegionConnection::weight)
        .def_readwrite("delay", &InterRegionConnection::delay)
        .def_readwrite("plasticityFlags", &InterRegionConnection::plasticityFlags)
        .def("__repr__", [](const InterRegionConnection& conn) {
            return "<InterRegionConnection " + std::to_string(conn.sourceRegion.value) + " -> " + std::to_string(conn.targetRegion.value) + ", w=" + std::to_string(conn.weight) + ">";
        });
}

// === NEUROMODULATION CLASSES ===

// Neuromodulator abstract base class
void bindNeuromodulator(py::module_& m) {
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator", py::abstract(), R"pbdoc(
        Abstract base class for neuromodulatory signals.
        
        Neuromodulators are brain chemicals that modulate neural activity
        and plasticity in a global or regionally-specific manner.
        
        Attributes:
            name: Human-readable name of the modulator
            level: Current concentration/level (0.0 to 1.0)
        
        Methods:
            getName(): Get modulator name
            getLevel(): Get current level
            setLevel(level): Set current level
            getPlasticityFactor(): Get plasticity modulation factor
            update(dt): Update modulator state
        )pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current level (0.0-1.0)")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current level (0.0-1.0)")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity modulation factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update modulator state (modulate dt)");
}

// Dopamine class (concrete neuromodulator)
void bindDopamine(py::module_& m) {
    py::class_<Dopamine, Neuromodulator, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(
        Dopamine neuromodulator: Reward and reinforcement learning signal.
        
        Dopamine signals prediction errors and rewards, driving reinforcement
        learning and motivated behavior. High levels indicate reward prediction
        errors and promote synaptic strengthening.
        
        Methods:
            signalReward(reward): Signal reward reception
            signalRewardPredictionError(error): Signal prediction error
        )pbdoc")
        .def(py::init<>(), "Create dopamine modulator")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal prediction error");
}

// Novelty class
void bindNovelty(py::module_& m) {
    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty", R"pbdoc(
        Novelty detection signal for curiosity and learning.
        
        Computes novelty from comparison with previous observations or patterns.
        Novelty signals drive exploration and reward prediction learning.
        
        Attributes:
            level: Current novelty level (0.0 = familiar, 1.0 = highly novel)
        
        Methods:
            getLevel(): Get novelty level
            setLevel(level): Set novelty level
            detectNovelty(observation, previous): Detect from two observations
            detectNovelty(pattern, previous): Detect from two patterns
            update(dt): Decay novelty over time
            getHistory(): Get novelty history
            clearHistory(): Clear history
        )pbdoc")
        .def(py::init<>(), "Create novelty detector")
        .def("getLevel", &Novelty::getLevel, "Get current novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("detectNovelty", py::overload_cast<const Observation&, const Observation&>(&Novelty::detectNovelty), py::arg("observation"), py::arg("previousObservation"), "Detect novelty from two observations")
        .def("detectNovelty", py::overload_cast<const std::vector<float>&, const std::vector<float>&>(&Novelty::detectNovelty), py::arg("currentPattern"), py::arg("previousPattern"), "Detect novelty from two patterns")
        .def("update", &Novelty::update, py::arg("dt"), "Decay novelty over time (dt)")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history");
}

// Curiosity class
void bindCuriosity(py::module_& m) {
    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity", R"pbdoc(
        Curiosity drive: Exploration motivation based on novelty and prediction error.
        
        Curiosity represents the drive to explore novel or uncertain situations.
        Computed as weighted combination of novelty and prediction error signals.
        
        Attributes:
            level: Current curiosity/exploration drive (0.0 = no drive, 1.0 = max)
        
        Methods:
            getLevel(): Get curiosity level
            update(novelty, predictionError, dt): Update based on signals
            getExplorationDrive(): Get exploration drive (same as level)
            setNoveltyWeight(weight): Set novelty weighting factor
            setPredictionErrorWeight(weight): Set prediction error weighting factor
            reset(): Reset curiosity
        )pbdoc")
        .def(py::init<>(), "Create curiosity drive")
        .def("getLevel", &Curiosity::getLevel, "Get current curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), "Update based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weighting factor")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weighting factor")
        .def("reset", &Curiosity::reset, "Reset curiosity state");
}

// PredictionError class
void bindPredictionError(py::module_& m) {
    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError", R"pbdoc(
        Prediction error signal for curiosity and learning.
        
        Computes difference between predicted and actual values.
        Used by curiosity and other learning systems to drive adaptation.
        
        Attributes:
            error: Current prediction error (0.0 = perfect, higher = larger error)
        
        Methods:
            getError(): Get current prediction error
            computeError(predicted, actual): Compute prediction error
            updatePrediction(newPrediction): Update prediction
            getMagnitude(): Get error magnitude for neuromodulation
            getHistory(): Get prediction error history
            clearHistory(): Clear history
        )pbdoc")
        .def(py::init<>(), "Create prediction error signal")
        .def("getError", &PredictionError::getError, "Get current prediction error")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"), "Update prediction")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get error magnitude for neuromodulation")
        .def("getHistory", &PredictionError::getHistory, "Get prediction error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear history");
}

// === MEMORY SYSTEMS ===

// NeuralWorkingMemory class
void bindNeuralWorkingMemory(py::module_& m) {
    py::class_<NeuralWorkingMemory, std::shared_ptr<NeuralWorkingMemory>>(m, "NeuralWorkingMemory", R"pbdoc(
        Working memory: Temporary active storage of information.
        
        Stores transient neural information for immediate processing and use.
        Implements decay mechanisms to maintain capacity and prevent overload.
        
        Methods:
            store(neuron, value): Store value for neuron
            retrieve(neuron): Retrieve value for neuron
            contains(neuron): Check if contains value for neuron
            clear(): Clear all memory
            getCapacity(): Get memory capacity
            getCurrentSize(): Get current number of items
            decay(decayRate): Decay all items
        )pbdoc")
        .def(py::init<>(), "Create working memory")
        .def("store", &NeuralWorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store value for neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve, py::arg("neuron"), "Retrieve value for neuron")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"), "Check if contains value for neuron")
        .def("clear", &NeuralWorkingMemory::clear, "Clear all memory")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get memory capacity")
        .def("getCurrentSize", &NeuralWorkingMemory::getCurrentSize, "Get current number of items")
        .def("decay", &NeuralWorkingMemory::decay, py::arg("decayRate"), "Decay all items (decayRate per timestep)");
}

// NeuralEpisodicMemory class
void bindNeuralEpisodicMemory(py::module_& m) {
    py::class_<NeuralEpisodicMemory, std::shared_ptr<NeuralEpisodicMemory>>(m, "NeuralEpisodicMemory", R"pbdoc(
        Episodic memory: Storage of events and experiences.
        
        Stores temporal sequences of neural activity representing
        specific events or experiences. Supports retrieval and consolidation.
        
        Attributes:
            items: List of episodic memory items
        
        Methods:
            storeEpisode(episode): Store episodic memory item
            retrieveEpisode(index): Retrieve episode by index
            getEpisodeCount(): Get number of episodes
            getRecentEpisodes(count): Get recent episodes
            clear(): Clear all episodes
            consolidate(threshold): Consolidate to long-term memory
        )pbdoc")
        .def(py::init<>(), "Create episodic memory")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"), "Store episodic memory item")
        .def("retrieveEpisode", &NeuralEpisodicMemory::retrieveEpisode, py::arg("index"), "Retrieve episode by index")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get number of episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all episodes")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Consolidate to long-term memory");
}

// NeuralAssociativeMemory class (from Memory.hpp)
void bindNeuralAssociativeMemory(py::module_& m) {
    py::class_<AssociativeMemory, std::shared_ptr<AssociativeMemory>>(m, "NeuralAssociativeMemory", R"pbdoc(
        Associative memory: Relationships between representations.
        
        Stores associations (links) between neural representations,
        implementing pattern completion and similarity-based retrieval.
        
        Methods:
            associate(a, b, strength): Create association between neurons
            getAssociations(neuron): Get associated neurons
            getAssociationStrength(a, b): Get association strength
            updateAssociation(a, b, delta): Update association
            clear(): Clear all associations
        )pbdoc")
        .def(py::init<>(), "Create associative memory")
        .def("associate", &AssociativeMemory::associate, py::arg("a"), py::arg("b"), py::arg("strength"), "Create association between neurons")
        .def("getAssociations", &AssociativeMemory::getAssociations, py::arg("neuron"), "Get associated neurons")
        .def("getAssociationStrength", &AssociativeMemory::getAssociationStrength, py::arg("a"), py::arg("b"), "Get association strength")
        .def("updateAssociation", &AssociativeMemory::updateAssociation, py::arg("a"), py::arg("b"), py::arg("delta"), "Update association")
        .def("clear", &AssociativeMemory::clear, "Clear all associations");
}

// === PREDICTION SYSTEMS ===

// PredictionSystem class
void bindPredictionSystem(py::module_& m) {
    py::class_<PredictionSystem, std::shared_ptr<PredictionSystem>>(m, "PredictionSystem", R"pbdoc(
        Prediction system for sensory prediction and error computation.
        
        Implements predictive coding: generates predictions of future sensory states
        and computes prediction errors to drive learning.
        
        Methods:
            predictNextState(currentState): Make prediction for next timestep
            updatePredictions(predicted, actual): Update based on actual observation
            getPredictionError(): Get current prediction error
            getConfidence(): Get prediction confidence
            getErrorHistory(): Get prediction error history
            clearHistory(): Clear error history
            train(observation): Train prediction model
        )pbdoc")
        .def(py::init<>(), "Create prediction system")
        .def("predictNextState", &PredictionSystem::predictNextState, py::arg("currentState"), "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions, py::arg("predicted"), py::arg("actual"), "Update predictions based on observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get current prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence, "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory, "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory, "Clear error history")
        .def("train", &PredictionSystem::train, py::arg("observation"), "Train prediction model");
}

// NeuralPrediction class
void bindNeuralPrediction(py::module_& m) {
    py::class_<NeuralPrediction, std::shared_ptr<NeuralPrediction>>(m, "NeuralPrediction", R"pbdoc(
        Neural prediction: temporal prediction using the spiking substrate.
        
        NOT a Transformer architecture - prediction emerges from learned
        synaptic dynamics and neural activity patterns.
        
        Attributes:
            error: Current prediction error (0.0 = perfect, 1.0 = total error)
            confidence: Prediction confidence (0.0 = no confidence, 1.0 = high)
        
        Methods:
            initialize(brain): Initialize with brain reference
            recordSensoryState(state, currentStep): Record sensory state for prediction
            generatePrediction(currentStep): Generate prediction for next timestep
            updateWithObservation(actualState, currentStep): Update with actual observation
            predictActionConsequence(action, currentState): Predict consequences of action
            getPredictionError(): Get current prediction error
            getPredictionConfidence(): Get prediction confidence
            predictMultipleSteps(currentStep, numSteps): Multi-step prediction
            recordAction(action, step): Record action for learning
            clearHistory(): Clear prediction history
        )pbdoc")
        .def(py::init<>(), "Create neural prediction system")
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState, py::arg("sensoryState"), py::arg("currentStep"), "Record sensory state for prediction")
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("currentStep"), "Generate prediction for next timestep")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation, py::arg("actualState"), py::arg("currentStep"), "Update with actual observation (returns error)")
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence, py::arg("action"), py::arg("currentState"), "Predict consequences of action")
        .def("getPredictionError", &NeuralPrediction::getPredictionError, "Get current prediction error")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence, "Get prediction confidence")
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps, py::arg("currentStep"), py::arg("numSteps"), "Multi-step prediction")
        .def("recordAction", &NeuralPrediction::recordAction, py::arg("action"), py::arg("step"), "Record action for learning")
        .def("clearHistory", &NeuralPrediction::clearHistory, "Clear prediction history")
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"), "Enable/disable temporal prediction")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"), "Enable/disable action consequence prediction")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"), "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("steps"), "Set prediction horizon (steps)")
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons, "Get neurons involved in prediction")
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons, "Get neurons storing temporal sequences");
}

// === PERFORMANCE AND CORE CLASSES ===

// RandomGenerator class
void bindRandomGenerator(py::module_& m) {
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(
        Deterministic random number generator.
        
        Supports explicit seeds for reproducible experiments.
        Provides various probability distributions.
        
        Methods:
            seed(seed): Set random seed
            getSeed(): Get current seed
            serializeState(): Get engine state for checkpointing
            restoreState(state): Restore engine state from checkpoint
            uniformInt(min, max): Uniform integer [min, max]
            uniformReal(min, max): Uniform real [min, max)
            normal(mean, stddev): Normal distribution
            bernoulli(p): Bernoulli trial
            poisson(lambda): Poisson distribution
            exponential(lambda): Exponential distribution
            randomBool(): Random boolean
            randomBit(): Random bit
            shuffle(container): Shuffle container in-place
            pickRandom(container): Pick random element
            pickRandomIndex(n): Pick random index [0, n-1]
            fillBytes(data, size): Fill buffer with random bytes
            __call__(): Generate random uint32_t (operator())
        )pbdoc")
        .def(py::init<uint64_t>(), py::arg("seed") = 0, "Create with seed (0 = time-based)")
        .def("seed", &RandomGenerator::seed, py::arg("seed"), "Set random seed")
        .def("getSeed", &RandomGenerator::getSeed, "Get current seed")
        .def("serializeState", &RandomGenerator::serializeState, "Get engine state for checkpointing")
        .def("restoreState", &RandomGenerator::restoreState, py::arg("state"), "Restore engine state from checkpoint")
        .def("uniformInt", py::overload_cast<uint32_t, uint32_t>(&RandomGenerator::uniformInt), py::arg("min"), py::arg("max"), "Uniform integer in [min, max]")
        .def("uniformInt", py::overload_cast<uint64_t, uint64_t>(&RandomGenerator::uniformInt), py::arg("min"), py::arg("max"), "Uniform integer in [min, max]")
        .def("uniformReal", py::overload_cast<double, double>(&RandomGenerator::uniformReal), py::arg("min"), py::arg("max"), "Uniform real in [min, max)")
        .def("uniformReal", py::overload_cast<float, float>(&RandomGenerator::uniformReal), py::arg("min"), py::arg("max"), "Uniform real in [min, max)")
        .def("normal", py::overload_cast<double, double>(&RandomGenerator::normal), py::arg("mean"), py::arg("stddev"), "Normal distribution")
        .def("normal", py::overload_cast<float, float>(&RandomGenerator::normal), py::arg("mean"), py::arg("stddev"), "Normal distribution")
        .def("bernoulli", &RandomGenerator::bernoulli, py::arg("p") = 0.5, "Bernoulli trial (p)")
        .def("poisson", &RandomGenerator::poisson, py::arg("lambda"), "Poisson distribution (lambda)")
        .def("exponential", &RandomGenerator::exponential, py::arg("lambda"), "Exponential distribution (lambda)")
        .def("randomBool", &RandomGenerator::randomBool, "Random boolean")
        .def("randomBit", &RandomGenerator::randomBit, "Random bit (32-bit)")
        .def("shuffle", &RandomGenerator::shuffle, py::arg("container"), "Shuffle container in-place")
        .def("pickRandom", &RandomGenerator::pickRandom, py::arg("container"), "Pick random element from container")
        .def("pickRandomIndex", &RandomGenerator::pickRandomIndex, py::arg("n"), "Pick random index from [0, n-1]")
        .def("fillBytes", &RandomGenerator::fillBytes, py::arg("data"), py::arg("size"), "Fill buffer with random bytes")
        .def("__call__", &RandomGenerator::operator(), "Generate random uint32_t (operator())");
}

// SimulationClock class
void bindSimulationClock(py::module_& m) {
    py::class_<SimulationClock, std::shared_ptr<SimulationClock>>(m, "SimulationClock", R"pbdoc(
        Simulation clock for managing time in the neural simulation.
        
        Provides deterministic time tracking separate from wall-clock time.
        Manages timestep counting and time conversions.
        
        Attributes:
            timestep: Current timestep duration (seconds)
            step: Current simulation step (integer)
            time: Current simulation time (seconds)
        
        Methods:
            reset(): Reset clock to initial state
            advance(): Advance to next timestep
            getStep(): Get current simulation step
            setStep(step): Set current simulation step
            getTime(): Get current simulation time
            setTime(time): Set current simulation time
            getTimestep(): Get timestep duration
            setTimestep(timestep): Set timestep duration
            isFirstStep(): Check if on first step
            isLastStep(totalSteps): Check if on last step
            stepToTime(step): Convert step to time
            timeToStep(time): Convert time to step
            advanceBy(steps): Advance by multiple steps
            timeAtFutureStep(stepsAhead): Get time at future step
            getRealTimeElapsed(): Get real-time elapsed (for real-time mode)
            setRealTimeStart(): Set real-time start for real-time mode
        )pbdoc")
        .def(py::init<TimestepDuration>(), py::arg("timestep") = 0.001, "Create clock with timestep (default 1ms)")
        .def("reset", &SimulationClock::reset, "Reset clock to initial state")
        .def("advance", &SimulationClock::advance, "Advance to next timestep")
        .def("getStep", &SimulationClock::getStep, "Get current simulation step")
        .def("setStep", &SimulationClock::setStep, py::arg("step"), "Set current simulation step")
        .def("getTime", &SimulationClock::getTime, "Get current simulation time (seconds)")
        .def("setTime", &SimulationClock::setTime, py::arg("time"), "Set current simulation time (seconds)")
        .def("getTimestep", &SimulationClock::getTimestep, "Get timestep duration (seconds)")
        .def("setTimestep", &SimulationClock::setTimestep, py::arg("timestep"), "Set timestep duration (seconds)")
        .def("isFirstStep", &SimulationClock::isFirstStep, "Check if on first step")
        .def("isLastStep", &SimulationClock::isLastStep, py::arg("totalSteps"), "Check if on last step (totalSteps)")
        .def("stepToTime", &SimulationClock::stepToTime, py::arg("step"), "Convert simulation step to time")
        .def("timeToStep", &SimulationClock::timeToStep, py::arg("time"), "Convert time to simulation step")
        .def("advanceBy", &SimulationClock::advanceBy, py::arg("steps"), "Advance by multiple steps")
        .def("timeAtFutureStep", &SimulationClock::timeAtFutureStep, py::arg("stepsAhead"), "Get time at future step from current position")
        .def("getRealTimeElapsed", &SimulationClock::getRealTimeElapsed, "Get real-time elapsed (for real-time mode)")
        .def("setRealTimeStart", &SimulationClock::setRealTimeStart, "Set real-time start for real-time mode")
        .def("__repr__", [](const SimulationClock& clock) {
            return "<SimulationClock step=" + std::to_string(clock.getStep()) + ", time=" + std::to_string(clock.getTime()) + ", dt=" + std::to_string(clock.getTimestep()) + ">";
        }, "String representation of clock")
        .def("__str__", [](const SimulationClock& clock) {
            return "SimulationClock(step=" + std::to_string(clock.getStep()) + ", t=" + std::to_string(clock.getTime()) + ")";
        });
}

// Logger class
void bindLogger(py::module_& m) {
    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log severity levels)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
        .export_values();
    
    py::class_<LogEntry>(m, "LogEntry", R"pbdoc(Log entry structure)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &LogEntry::level)
        .def_readwrite("message", &LogEntry::message)
        .def_readwrite("file", &LogEntry::file)
        .def_readwrite("line", &LogEntry::line)
        .def_readwrite("function", &LogEntry::function)
        .def_readwrite("timestamp", &LogEntry::timestamp);
    
    py::class_<ConsoleLogger, std::shared_ptr<ConsoleLogger>>(m, "ConsoleLogger", R"pbdoc(
        Console logger implementation.
        
        Outputs log messages to console with optional colors.
        
        Methods:
            setLevel(level): Set minimum log level
            getLevel(): Get minimum log level
            setUseColors(useColors): Enable/disable colors
            getUseColors(): Check if colors are enabled
            setOutputFile(filepath): Set output file (in addition to console)
            closeOutputFile(): Close output file
        )pbdoc")
        .def(py::init<>())
        .def(py::init<LogLevel>(), py::arg("level"), "Create console logger with level")
        .def("log", &ConsoleLogger::log, py::arg("entry"), "Log entry")
        .def("setLevel", &ConsoleLogger::setLevel, py::arg("level"), "Set minimum log level")
        .def("getLevel", &ConsoleLogger::getLevel, "Get minimum log level")
        .def("setUseColors", &ConsoleLogger::setUseColors, py::arg("useColors"), "Enable/disable colors")
        .def("getUseColors", &ConsoleLogger::getUseColors, "Check if colors are enabled")
        .def("setOutputFile", &ConsoleLogger::setOutputFile, py::arg("filepath"), "Set output file (in addition to console)")
        .def("closeOutputFile", &ConsoleLogger::closeOutputFile, "Close output file");
    
    py::class_<Logger, std::shared_ptr<Logger>>(m, "Logger", R"pbdoc(
        Main logger class.
        
        Manages multiple logger sinks and provides global logging functions.
        
        Methods:
            addLogger(logger): Add logger sink
            clearLoggers(): Remove all loggers
            setLevel(level): Set minimum level for all loggers
            log(level, message, file, line, function): Log at level
            debug(message, file, line, function): Log debug message
            info(message, file, line, function): Log info message
            warning(message, file, line, function): Log warning
            error(message, file, line, function): Log error
            critical(message, file, line, function): Log critical
            isEnabled(level): Check if level is enabled
            getGlobal(): Get global logger instance
            setGlobal(logger): Set global logger
        )pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"), "Add logger sink")
        .def("clearLoggers", &Logger::clearLoggers, "Remove all loggers")
        .def("setLevel", &Logger::setLevel, py::arg("level"), "Set minimum level for all loggers")
        .def("log", &Logger::log, py::arg("level"), py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log at level")
        .def("debug", &Logger::debug, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log debug message")
        .def("info", &Logger::info, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log info message")
        .def("warning", &Logger::warning, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log warning")
        .def("error", &Logger::error, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log error")
        .def("critical", &Logger::critical, py::arg("message"), py::arg("file") = "", py::arg("line") = 0, py::arg("function") = "", "Log critical")
        .def("isEnabled", &Logger::isEnabled, py::arg("level"), "Check if level is enabled")
        .def("getGlobal", &Logger::getGlobal, "Get global logger instance", py::return_value_policy::reference)
        .def("setGlobal", &Logger::setGlobal, py::arg("logger"), "Set global logger");
    
    // Convenience macros as functions
    m.def("log_debug", [](const std::string& msg) { NLM_LOG_DEBUG(msg); }, py::arg("msg"), "Debug logging macro");
    m.def("log_info", [](const std::string& msg) { NLM_LOG_INFO(msg); }, py::arg("msg"), "Info logging macro");
    m.def("log_warning", [](const std::string& msg) { NLM_LOG_WARNING(msg); }, py::arg("msg"), "Warning logging macro");
    m.def("log_error", [](const std::string& msg) { NLM_LOG_ERROR(msg); }, py::arg("msg"), "Error logging macro");
    m.def("log_critical", [](const std::string& msg) { NLM_LOG_CRITICAL(msg); }, py::arg("msg"), "Critical logging macro");
}

// MemoryPool class
void bindMemoryPool(py::module_& m) {
    py::class_<MemoryPool<Neuron>, std::shared_ptr<MemoryPool<Neuron>>>(m, "NeuronPool", R"pbdoc(
        Memory pool for neurons with cache-aligned states.
        
        Provides efficient allocation and deallocation of neurons with
        SoA (Structure of Arrays) storage for cache efficiency.
        
        Attributes:
            size: Number of allocated neurons
            capacity: Pool capacity
            memoryUsage: Memory usage in bytes
        
        Methods:
            allocate(): Allocate a neuron
            deallocate(id): Deallocate a neuron
            getState<Member>(index): Access neuron state member (SoA)
            size(): Get allocated count
            memoryUsage(): Get memory usage
        )pbdoc")
        .def(py::init<>())
        .def("allocate", &MemoryPool<Neuron>::allocate, "Allocate a neuron")
        .def("deallocate", &MemoryPool<Neuron>::deallocate, py::arg("id"), "Deallocate a neuron")
        .def("getState", &MemoryPool<Neuron>::getState<NeuronState>, py::arg("index"), "Get neuron state member (SoA)")
        .def("size", &MemoryPool<Neuron>::size, "Get allocated count")
        .def("memoryUsage", &MemoryPool<Neuron>::memoryUsage, "Get memory usage in bytes");
    
    py::class_<MemoryPool<Synapse>, std::shared_ptr<MemoryPool<Synapse>>>(m, "SynapsePool", R"pbdoc(
        Memory pool for synapses.
        
        Provides efficient allocation and deallocation of synapses with
        SoA (Structure of Arrays) storage for cache efficiency.
        
        Methods:
            allocate(source, destination): Allocate a synapse
            deallocate(id): Deallocate a synapse
            getState<Member>(index): Access synapse state member (SoA)
            getSource(index): Get source neuron
            getDestination(index): Get destination neuron
            size(): Get allocated count
            memoryUsage(): Get memory usage
        )pbdoc")
        .def(py::init<>())
        .def("allocate", &MemoryPool<Synapse>::allocate, py::arg("source"), py::arg("destination"), "Allocate a synapse")
        .def("deallocate", &MemoryPool<Synapse>::deallocate, py::arg("id"), "Deallocate a synapse")
        .def("getState", &MemoryPool<Synapse>::getState<SynapseState>, py::arg("index"), "Get synapse state member (SoA)")
        .def("getSource", &MemoryPool<Synapse>::getSource, py::arg("index"), "Get source neuron")
        .def("getDestination", &MemoryPool<Synapse>::getDestination, py::arg("index"), "Get destination neuron")
        .def("size", &MemoryPool<Synapse>::size, "Get allocated count")
        .def("memoryUsage", &MemoryPool<Synapse>::memoryUsage, "Get memory usage in bytes");
}

// EventQueue class
void bindEventQueue(py::module_& m) {
    py::class_<EventQueue, std::shared_ptr<EventQueue>>(m, "EventQueue", R"pbdoc(
        Event-driven computation queue for spike processing.
        
        Provides lock-free event queue using ring buffers.
        Designed for event-driven neural simulation.
        
        Methods:
            getImmediateEvents(): Get immediate events
            popImmediate(): Pop immediate event
            scheduleDelayed(source, dest, synapseId, weight, exc, deliveryStep): Schedule delayed spike
            getDelayedEvents(currentStep): Get delayed events due at step
            pendingCount(): Get pending delayed event count
            advanceTime(currentStep): Advance time (cleanup old buckets)
            clear(): Clear all events
            size(): Get total event count
        )pbdoc")
        .def(py::init<>())
        .def("getImmediateEvents", &EventQueue::getImmediateEvents, "Get immediate events")
        .def("popImmediate", &EventQueue::popImmediate, "Pop immediate event")
        .def("scheduleDelayed", &EventQueue::scheduleDelayed, py::arg("sourceNeuron"), py::arg("destNeuron"), py::arg("synapseId"), py::arg("weight"), py::arg("isExcitatory"), py::arg("deliveryStep"), "Schedule delayed spike")
        .def("getDelayedEvents", &EventQueue::getDelayedEvents, py::arg("currentStep"), "Get delayed events due at step")
        .def("pendingCount", &EventQueue::pendingCount, "Get pending delayed event count")
        .def("advanceTime", &EventQueue::advanceTime, py::arg("currentStep"), "Advance time (cleanup old buckets)")
        .def("clear", &EventQueue::clear, "Clear all events")
        .def("size", &EventQueue::size, "Get total event count");
}

// CheckpointSystem class
void bindCheckpointSystem(py::module_& m) {
    py::class_<CheckpointSystem, std::shared_ptr<CheckpointSystem>>(m, "CheckpointSystem", R"pbdoc(
        Brain state serialization and checkpointing.
        
        Provides robust checkpointing for large-scale neural simulations.
        Supports incremental saves, version compatibility, and validation.
        
        Methods:
            create(filepath, compression): Create new checkpoint file
            open(filepath): Open checkpoint file for reading
            isOpen(): Check if checkpoint is open
            saveImmediately(name): Save checkpoint immediately
            load(name): Load checkpoint
            listCheckpoints(): List available checkpoints
            shouldSave(currentStep): Check if should save
            getCheckpointDir(): Get checkpoint directory
            getLastCheckpointPath(): Get last checkpoint path
        )pbdoc")
        .def(py::init<>())
        .def("create", &CheckpointSystem::create, py::arg("filepath"), py::arg("compression") = CompressionLevel::Balanced, "Create new checkpoint file")
        .def("open", &CheckpointSystem::open, py::arg("filepath"), "Open checkpoint file for reading")
        .def("isOpen", &CheckpointSystem::isOpen, "Check if checkpoint is open")
        .def("saveImmediately", &CheckpointSystem::saveImmediately, py::arg("name") = "", "Save checkpoint immediately")
        .def("load", &CheckpointSystem::load, py::arg("name"), "Load checkpoint")
        .def("listCheckpoints", &CheckpointSystem::listCheckpoints, "List available checkpoints")
        .def("shouldSave", &CheckpointSystem::shouldSave, py::arg("currentStep"), "Check if should save")
        .def("getCheckpointDir", &CheckpointSystem::getCheckpointDir, "Get checkpoint directory")
        .def("getLastCheckpointPath", &CheckpointSystem::getLastCheckpointPath, "Get last checkpoint path");
    
    // Compression levels
    py::enum_<CompressionLevel>(m, "CompressionLevel", R"pbdoc(Compression options)pbdoc")
        .value("None", CompressionLevel::None)
        .value("Fast", CompressionLevel::Fast)
        .value("Balanced", CompressionLevel::Balanced)
        .value("Best", CompressionLevel::Best)
        .export_values();
}

// === ENVIRONMENT AND VISUALIZATION ===

// Environment class
void bindEnvironment(py::module_& m) {
    py::class_<Environment, std::shared_ptr<Environment>>(m, "Environment", py::abstract(), R"pbdoc(
        Simulation environment interface.
        
        Abstract base class for simulation environments. Implementations
        provide specific simulation contexts like grid world, physics, etc.
        
        Methods:
            reset(): Reset environment to initial state
            step(action): Take action and get observation
            getState(): Get current environment state
            getLastReward(): Get reward for last action
            isDone(): Check if episode is done
            getName(): Get environment name
            getActionSpaceSize(): Get action space size
            getObservationSpaceSize(): Get observation space size
        )pbdoc")
        .def("reset", py::pure_virtual(&Environment::reset), "Reset environment to initial state")
        .def("step", py::pure_virtual(&Environment::step), py::arg("action"), "Take action and get observation")
        .def("getState", py::pure_virtual(&Environment::getState), "Get current environment state")
        .def("getLastReward", py::pure_virtual(&Environment::getLastReward), "Get reward for last action")
        .def("isDone", py::pure_virtual(&Environment::isDone), "Check if episode is done")
        .def("getName", py::pure_virtual(&Environment::getName), "Get environment name")
        .def("getActionSpaceSize", py::pure_virtual(&Environment::getActionSpaceSize), "Get action space size")
        .def("getObservationSpaceSize", py::pure_virtual(&Environment::getObservationSpaceSize), "Get observation space size");
    
    py::class_<EnvironmentState>(m, "EnvironmentState", R"pbdoc(Environment state)")
        .def(py::init<>())
        .def("is_terminal", &EnvironmentState::is_terminal, "Check if terminal")
        .def("set_terminal", &EnvironmentState::set_terminal, py::arg("terminal"), "Set terminal state");
    
    py::class_<GridWorldEnvironment, Environment, std::shared_ptr<GridWorldEnvironment>>(m, "GridWorldEnvironment", R"pbdoc(
        Grid-world environment for testing.
        
        Simple 2D grid environment for testing and development.
        
        Attributes:
            width: Grid width
            height: Grid height
        )pbdoc")
        .def(py::init<int, int>(), py::arg("width") = 10, py::arg("height") = 10, "Create grid world environment");
}

// Observation class
void bindObservation(py::module_& m) {
    py::class_<Observation, std::shared_ptr<Observation>>(m, "Observation", R"pbdoc(
        Sensory input observation.
        
        Wraps a SensoryInput to provide observation interface.
        Used for novelty detection and prediction systems.
        
        Methods:
            getSensoryInput(): Get the sensory input
            isValid(): Check if observation is valid
            clone(): Clone observation
        )pbdoc")
        .def(py::init<>())
        .def(py::init<const SensoryInput&>(), py::arg("input"), "Create observation from sensory input")
        .def("getSensoryInput", &Observation::getSensoryInput, "Get the sensory input")
        .def("isValid", &Observation::isValid, "Check if observation is valid")
        .def("clone", &Observation::clone, "Clone observation");
}

// VisualizationInterface class
void bindVisualizationInterface(py::module_& m) {
    py::class_<VisualizationInterface, std::shared_ptr<VisualizationInterface>>(m, "VisualizationInterface", R"pbdoc(
        Visualization interface for debugging and analysis.
        
        Provides visualization of neural activity, network structure,
        and simulation results. Used for debugging and analysis.
        
        Methods:
            initialize(): Initialize visualization
            update(): Update visualization data
            render(): Render current state
            close(): Close visualization
            isActive(): Check if visualization is active
            setUpdateRate(hz): Set update rate
            getUpdateRate(): Get update rate
            visualizeNetwork(brain): Visualize network structure
            visualizeActivity(brain): Visualize neural activity
            visualizeSpikes(spikes): Visualize spike raster plot
            visualizeWeights(brain): Visualize weight matrix
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &VisualizationInterface::initialize, "Initialize visualization")
        .def("update", &VisualizationInterface::update, "Update visualization data")
        .def("render", &VisualizationInterface::render, "Render current state")
        .def("close", &VisualizationInterface::close, "Close visualization")
        .def("isActive", &VisualizationInterface::isActive, "Check if visualization is active")
        .def("setUpdateRate", &VisualizationInterface::setUpdateRate, py::arg("hz"), "Set update rate (Hz)")
        .def("getUpdateRate", &VisualizationInterface::getUpdateRate, "Get update rate (Hz)")
        .def("visualizeNetwork", &VisualizationInterface::visualizeNetwork, py::arg("brain"), "Visualize network structure")
        .def("visualizeActivity", &VisualizationInterface::visualizeActivity, py::arg("brain"), "Visualize neural activity")
        .def("visualizeSpikes", &VisualizationInterface::visualizeSpikes, py::arg("spikes"), "Visualize spike raster plot")
        .def("visualizeWeights", &VisualizationInterface::visualizeWeights, py::arg("brain"), "Visualize weight matrix");
}

// === DEVELOPMENT SYSTEM ===

// DevelopmentSystem class
void bindDevelopmentSystem(py::module_& m) {
    py::class_<DevelopmentSystem, std::shared_ptr<DevelopmentSystem>>(m, "DevelopmentSystem", R"pbdoc(
        Development system: orchestrates neural development.
        
        Implements developmental stages, plasticity modulation, and structural changes.
        Manages the brain's development from initial to aging stages.
        
        Attributes:
            stage: Current developmental stage
            stageAge: Age since entering current stage
            developmentalAge: Total developmental age (seconds)
        
        Methods:
            getStage(): Get current developmental stage
            setStage(stage): Set developmental stage
            advanceStage(): Advance to next stage
            getStageName(): Get stage name string
            update(brain, currentStep): Update development (simple version)
            update(brain, rng, dt): Update development with parameters
            getPlasticityModifier(): Get plasticity modifier for current stage
            isCriticalPeriod(): Check if in critical period
            getCriticalPeriodProgress(): Get critical period progress (0-1)
            getDevelopmentalAge(): Get developmental age
            setDevelopmentalAge(age): Set developmental age
        )pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance to next developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get current stage name")
        .def("update", py::overload_cast<Brain*, SimulationStep>(&DevelopmentSystem::update), py::arg("brain"), py::arg("currentStep"), "Update development (simple version)")
        .def("update", py::overload_cast<Brain*, RandomGenerator&, TimestepDuration>(&DevelopmentSystem::update), py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with full parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if currently in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress (0-1)")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age (seconds)")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age (seconds)");
}

// === FACTORY FUNCTIONS AND UTILITIES ===

void bindFactoryFunctions(py::module_& m) {
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
    
    m.def("createConsoleLogger", []() -> std::shared_ptr<ConsoleLogger> {
        return std::make_shared<ConsoleLogger>();
    }, "Create console logger");
    
    m.def("createConsoleLoggerWithLevel", [](LogLevel level) -> std::shared_ptr<ConsoleLogger> {
        return std::make_shared<ConsoleLogger>(level);
    }, py::arg("level"), "Create console logger with level");
    
    m.def("createRandomGenerator", [](uint64_t seed) -> std::shared_ptr<RandomGenerator> {
        return std::make_shared<RandomGenerator>(seed);
    }, py::arg("seed"), "Create random generator with seed");
    
    m.def("createSimulationClock", [](TimestepDuration timestep) -> std::shared_ptr<SimulationClock> {
        return std::make_shared<SimulationClock>(timestep);
    }, py::arg("timestep") = 0.001, "Create simulation clock with timestep");
    
    m.def("createVisualizationInterface", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create visualization interface");
    
    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create development system");
    
    m.def("createNeuromodulator", [](const std::string& type) -> std::shared_ptr<Neuromodulator> {
        if (type == "dopamine" || type == "Dopamine") {
            return std::make_shared<Dopamine>();
        } else if (type == "novelty" || type == "Novelty") {
            return std::make_shared<Novelty>();
        } else if (type == "curiosity" || type == "Curiosity") {
            return std::make_shared<Curiosity>();
        } else if (type == "prediction_error" || type == "PredictionError") {
            return std::make_shared<PredictionError>();
        } else {
            throw PyNotImplementedError("Neuromodulator type not implemented: " + type);
        }
    }, py::arg("type"), "Create neuromodulator of specified type");
    
    m.def("createMemoryPool", [](size_t blockSize, size_t initialBlocks) -> std::shared_ptr<MemoryPool<Neuron>> {
        return std::make_shared<MemoryPool<Neuron>>(blockSize, initialBlocks);
    }, py::arg("blockSize") = 1024, py::arg("initialBlocks") = 4, "Create neuron memory pool");
}

void bindPythonicFeatures(py::module_& m) {
    // Add Pythonic features to classes
    
    // Exception translation
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::bad_alloc>(m, "MemoryError");
    
    // Add context manager support (simplified)
    py::class_<ResourceManager>(m, "ResourceManager")
        .def("cleanup", &ResourceManager::cleanup, "Cleanup resources");
}

// === MAIN BINDING FUNCTION ===

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides Python interfaces to all NLM classes including:
        
        - **Core Neural Classes**: Neuron, Synapse, NeuralPopulation, NeuralRegion
        - **Neuromodulation Systems**: Neuromodulator, Dopamine, Novelty, Curiosity, PredictionError
        - **Memory Systems**: NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
        - **Prediction Systems**: PredictionSystem, NeuralPrediction
        - **Performance/Core**: RandomGenerator, SimulationClock, Logger, MemoryPool, EventQueue, CheckpointSystem
        - **Environment**: Environment, GridWorldEnvironment, Observation
        - **Visualization**: VisualizationInterface
        - **Development**: DevelopmentSystem
        
        The bindings follow Pythonic design patterns with:
        - Comprehensive documentation
        - Exception safety with proper translation
        - Context managers for resource management
        - Property-based access where appropriate
        - Iterator support for collections
        - Enhanced error handling
        
        Examples:
        
        >>> from pynlm import createBrain, createRandomGenerator
        >>> rng = createRandomGenerator(42)
        >>> brain = createBrain(createDefaultConfig())
        >>> brain.initialize()
        >>> step = brain.step(0)
        
        >>> from pynlm import Neuron, Synapse
        >>> neuron = Neuron(NeuronId(1))
        >>> synapse = Synapse(SynapseId(1), NeuronId(1), NeuronId(2))
        
        >>> from pynlm import createConsoleLogger, LogLevel
        >>> logger = createConsoleLoggerWithLevel(LogLevel.Info)
        >>> logger.info("Simulation started")
    )pbdoc";
    
    // Add exception translation
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::bad_alloc>(m, "MemoryError");
    
    // Add constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Bind core types
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
    
    // Bind enums
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
    
    // Bind existing classes (minimal for completeness)
    bindNeuron(m);
    bindSynapse(m);
    bindNeuralPopulation(m);
    bindNeuralRegion(m);
    bindInterRegionConnection(m);
    bindNeuromodulator(m);
    bindDopamine(m);
    bindNovelty(m);
    bindCuriosity(m);
    bindPredictionError(m);
    bindNeuralWorkingMemory(m);
    bindNeuralEpisodicMemory(m);
    bindNeuralAssociativeMemory(m);
    bindPredictionSystem(m);
    bindNeuralPrediction(m);
    bindRandomGenerator(m);
    bindSimulationClock(m);
    bindLogger(m);
    bindMemoryPool(m);
    bindEventQueue(m);
    bindCheckpointSystem(m);
    bindEnvironment(m);
    bindObservation(m);
    bindVisualizationInterface(m);
    bindDevelopmentSystem(m);
    
    // Bind existing classes (from original bindings.cpp)
    // (These would be included if not already bound above)
    
    // Bind factory functions
    bindFactoryFunctions(m);
    bindPythonicFeatures(m);
    
    // Log initialization
    Logger::getGlobal().info("NLM Python bindings initialized successfully");
}

} // namespace nlm