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

// Neural architecture components
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"

// Plasticity mechanisms
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"

// Neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM neural simulation framework.
        
        This enhanced bindings implementation provides access to core neuroscience
        components that were missing from the original bindings, enabling advanced
        research capabilities:
        
        **Core Neural Architecture**
        - Neuron: LIF neuron with membrane potential dynamics, spike generation, plasticity
        - Synapse: Synaptic connections with delays, weights, and activity history
        - NeuralRegion: Brain regions containing populations and local connectivity
        - NeuralPopulation: Groups of neurons with shared properties and statistics
        
        **Plasticity Mechanisms**
        - STDP: Spike-Timing Dependent Plasticity
        - Hebbian: Hebbian learning "fire together, wire together"
        - PlasticityRule: Abstract base for all plasticity rules
        
        **Neuromodulation**
        - Dopamine: Reward signaling and reinforcement
        - Curiosity: Exploration motivation
        - Novelty: Novelty detection
        - PredictionError: Prediction error signals
        
        **Enhanced Research APIs**
        - Factory functions for common research scenarios
        - Batch operations for efficiency
        - Brain statistics collection
        - Visualization utilities
        
        Key improvements over original bindings:
        - Full neural architecture access
        - Advanced learning mechanisms
        - Neuromodulatory control
        - Research-oriented APIs
        - Backward compatibility maintained
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // =============================================================================
    // CORE TYPES AND IDENTIFIERS
    // =============================================================================

    // NeuronId - unique identifier for neurons
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

    // SynapseId - unique identifier for synapses
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

    // RegionId - unique identifier for brain regions
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

    // PopulationId - unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

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

    py::enum_<PlasticityFlags>(m, "PlasticityFlags", R"pbdoc(Plasticity rule flags)pbdoc")
        .value("HebbianEnabled", PlasticityFlags::HebbianEnabled)
        .value("STDPEnabled", PlasticityFlags::STDPEnabled)
        .value("RewardModulated", PlasticityFlags::RewardModulated)
        .export_values();

    // =============================================================================
    // CORE NEURAL ARCHITECTURE
    // =============================================================================

    // Neuron class - core computational unit
    py::class_<Neuron>(m, "Neuron", R"pbdoc(
        A single neuron implementing Leaky Integrate-and-Fire dynamics.
        
        Neurons receive inputs, integrate membrane potential, and generate spikes
        when threshold is reached. This is the fundamental computational unit
        of the neural simulation.
    )pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def(py::init<NeuronId, NeuronType>(), py::arg("id"), py::arg("type"))
        
        // Identity and type
        .def("getId", &Neuron::getId, R"pbdoc(Get neuron ID)pbdoc")
        .def("getType", &Neuron::getType, R"pbdoc(Get neuron type)pbdoc")
        .def("setType", &Neuron::setType, py::arg("type"), R"pbdoc(Set neuron type)pbdoc")
        
        // Membrane potential dynamics (LIF)
        .def("getMembranePotential", &Neuron::getMembranePotential, R"pbdoc(Get membrane potential (mV))pbdoc")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), R"pbdoc(Set membrane potential)pbdoc")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), R"pbdoc(Add to membrane potential)pbdoc")
        
        // Firing state and threshold
        .def("isFiring", &Neuron::isFiring, R"pbdoc(Check if neuron is currently firing)pbdoc")
        .def("isRefractory", &Neuron::isRefractory, R"pbdoc(Check if neuron is in refractory period)pbdoc")
        .def("checkThreshold", &Neuron::checkThreshold, R"pbdoc(Check if neuron has reached firing threshold)pbdoc")
        
        // LIF neuron parameters
        .def("getThreshold", &Neuron::getThreshold, R"pbdoc(Get firing threshold)pbdoc")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), R"pbdoc(Set firing threshold)pbdoc")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), R"pbdoc(Set resting potential)pbdoc")
        .def("getRestingPotential", &Neuron::getRestingPotential, R"pbdoc(Get resting potential)pbdoc")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), R"pbdoc(Set reset potential)pbdoc")
        .def("getResetPotential", &Neuron::getResetPotential, R"pbdoc(Get reset potential)pbdoc")
        
        // Leak conductance and refractory period
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), R"pbdoc(Set leak conductance)pbdoc")
        .def("getLeakConductance", &Neuron::getLeakConductance, R"pbdoc(Get leak conductance)pbdoc")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), R"pbdoc(Set refractory period in steps)pbdoc")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, R"pbdoc(Get refractory period in steps)pbdoc")
        .def("decrementRefractory", &Neuron::decrementRefractory, R"pbdoc(Decrement refractory counter)pbdoc")
        
        // Firing rate
        .def("getFiringRate", &Neuron::getFiringRate, R"pbdoc(Get firing rate (Hz))pbdoc")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), R"pbdoc(Set firing rate)pbdoc")
        
        // Spike management
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), R"pbdoc(Record a spike at given timestamp)pbdoc")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, py::return_value_policy::reference_internal, R"pbdoc(Get spike history timestamps)pbdoc")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, R"pbdoc(Get last spike time)pbdoc")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, R"pbdoc(Clear spike history)pbdoc")
        
        // Input and current management
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), R"pbdoc(Inject external current)pbdoc")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, R"pbdoc(Get total injected current)pbdoc")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, R"pbdoc(Clear total injected current)pbdoc")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), R"pbdoc(Receive excitatory input)pbdoc")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), R"pbdoc(Receive inhibitory input)pbdoc")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), R"pbdoc(Receive modulatory input)pbdoc")
        
        // Synaptic connections
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, py::return_value_policy::reference_internal, R"pbdoc(Get incoming synapse handles)pbdoc")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, py::return_value_policy::reference_internal, R"pbdoc(Get outgoing synapse handles)pbdoc")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), R"pbdoc(Add incoming synapse)pbdoc")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), R"pbdoc(Add outgoing synapse)pbdoc")
        
        // Plasticity management
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, py::return_value_policy::reference_internal, R"pbdoc(Get plasticity flags)pbdoc")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), R"pbdoc(Enable/disable plasticity rules)pbdoc")
        
        // Region/population membership
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), R"pbdoc(Set neuron region ID)pbdoc")
        .def("getRegionId", &Neuron::getRegionId, R"pbdoc(Get neuron region ID)pbdoc")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), R"pbdoc(Set neuron population ID)pbdoc")
        .def("getPopulationId", &Neuron::getPopulationId, R"pbdoc(Get neuron population ID)pbdoc")
        
        // Step functions
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), R"pbdoc(Perform LIF integration step (returns True if fired))pbdoc")
        .def("step", &Neuron::step, py::arg("currentTime"), R"pbdoc(Perform neuron update step)pbdoc")
        
        // Reset and initialization
        .def("reset", &Neuron::reset, R"pbdoc(Reset neuron to initial state)pbdoc")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), R"pbdoc(Initialize neuron with random parameters)pbdoc");

    // Synapse class - connection between neurons
    py::class_<Synapse>(m, "Synapse", R"pbdoc(
        A synaptic connection between neurons with dynamic properties.
        
        Synapses transmit signals from pre-synaptic to post-synaptic neurons
        with configurable delays, weights, and plasticity rules.
    )pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        
        // Identity
        .def("getId", &Synapse::getId, R"pbdoc(Get synapse ID)pbdoc")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, R"pbdoc(Get pre-synaptic neuron)pbdoc")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, R"pbdoc(Get post-synaptic neuron)pbdoc")
        
        // Weight and type
        .def("getWeight", &Synapse::getWeight, R"pbdoc(Get synaptic weight)pbdoc")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), R"pbdoc(Set synaptic weight)pbdoc")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), R"pbdoc(Add to synaptic weight)pbdoc")
        
        .def("getType", &Synapse::getType, R"pbdoc(Get synapse type)pbdoc")
        .def("setType", &Synapse::setType, py::arg("type"), R"pbdoc(Set synapse type)pbdoc")
        .def("isExcitatory", &Synapse::isExcitatory, R"pbdoc(Check if synapse is excitatory)pbdoc")
        .def("isInhibitory", &Synapse::isInhibitory, R"pbdoc(Check if synapse is inhibitory)pbdoc")
        
        // Delay
        .def("getDelay", &Synapse::getDelay, R"pbdoc(Get transmission delay)pbdoc")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), R"pbdoc(Set transmission delay)pbdoc")
        
        // Plasticity history
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), R"pbdoc(Record pre-synaptic spike)pbdoc")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), R"pbdoc(Record post-synaptic spike)pbdoc")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, py::return_value_policy::reference_internal, R"pbdoc(Get pre-synaptic spike history)pbdoc")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, py::return_value_policy::reference_internal, R"pbdoc(Get post-synaptic spike history)pbdoc")
        .def("clearHistory", &Synapse::clearHistory, R"pbdoc(Clear spike history)pbdoc")
        
        // Plasticity flags
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, py::return_value_policy::reference_internal, R"pbdoc(Get plasticity flags)pbdoc")
        .def("getPlasticityFlagsRef", &Synapse::getPlasticityFlags, py::return_value_policy::reference_internal, R"pbdoc(Get plasticity flags (reference))pbdoc")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), R"pbdoc(Enable/disable plasticity rules)pbdoc")
        
        // Eligibility traces
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, R"pbdoc(Get eligibility trace)pbdoc")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), R"pbdoc(Set eligibility trace)pbdoc")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), R"pbdoc(Decay eligibility trace)pbdoc")
        
        // Synaptic efficacy
        .def("getEfficacy", &Synapse::getEfficacy, R"pbdoc(Get synaptic efficacy)pbdoc")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), R"pbdoc(Set synaptic efficacy)pbdoc")
        
        // Step and reset
        .def("step", &Synapse::step, py::arg("currentTime"), R"pbdoc(Update synapse for one simulation step)pbdoc")
        .def("reset", &Synapse::reset, R"pbdoc(Reset synapse to initial state)pbdoc")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), R"pbdoc(Initialize synapse with random parameters)pbdoc");

    // =============================================================================
    // PLASTICITY MECHANISMS
    // =============================================================================

    // PlasticityRule - abstract base class
    py::class_<PlasticityRule, std::shared_ptr<PlasticityRule>>(m, "PlasticityRule", R"pbdoc(
        Abstract base class for synaptic plasticity rules.
        
        Plasticity rules implement learning algorithms that modify synaptic weights
        based on neuronal activity patterns.
    )pbdoc")
        .def("update", &PlasticityRule::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"), R"pbdoc(Update synaptic weights)pbdoc")
        .def("applyWeightChange", &PlasticityRule::applyWeightChange, py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &PlasticityRule::getName, R"pbdoc(Get rule name)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled, R"pbdoc(Check if rule is enabled)pbdoc")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"), R"pbdoc(Enable/disable rule)pbdoc");

    // STDP - Spike-Timing Dependent Plasticity
    py::class_<STDP, std::shared_ptr<STDP>>(m, "STDP", R"pbdoc(
        Spike-Timing Dependent Plasticity (STDP) learning rule.
        
        STDP modifies synaptic weights based on precise timing between
        pre-synaptic and post-synaptic spikes. This implements the biological
        principle that "neurons that fire together, wire together".
    )pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"), R"pbdoc(Update based on spike timing)pbdoc")
        .def("applyWeightChange", &STDP::applyWeightChange, py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &STDP::getName, R"pbdoc(Get rule name)pbdoc")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), R"pbdoc(Set LTP weight)pbdoc")
        .def("getLTPWeight", &STDP::getLTPWeight, R"pbdoc(Get LTP weight)pbdoc")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), R"pbdoc(Set LTD weight)pbdoc")
        .def("getLTDWeight", &STDP::getLTDWeight, R"pbdoc(Get LTD weight)pbdoc")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), R"pbdoc(Set time constant)pbdoc")
        .def("getTimeConstant", &STDP::getTimeConstant, R"pbdoc(Get time constant)pbdoc")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"), R"pbdoc(Configure STDP parameters)pbdoc");

    // Hebbian - Hebbian learning rule
    py::class_<Hebbian, std::shared_ptr<Hebbian>>(m, "Hebbian", R"pbdoc(
        Hebbian learning rule implementing "fire together, wire together".
        
        This is one of the simplest and most biologically plausible learning rules,
        where synaptic strength is increased based on correlated pre- and post-synaptic activity.
    )pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update, py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"), R"pbdoc(Update based on Hebbian learning)pbdoc")
        .def("applyWeightChange", &Hebbian::applyWeightChange, py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &Hebbian::getName, R"pbdoc(Get rule name)pbdoc")
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), R"pbdoc(Set learning rate)pbdoc")
        .def("getLearningRate", &Hebbian::getLearningRate, R"pbdoc(Get learning rate)pbdoc")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"), R"pbdoc(Set maximum weight)pbdoc")
        .def("getMaxWeight", &Hebbian::getMaxWeight, R"pbdoc(Get maximum weight)pbdoc");

    // =============================================================================
    // NEUROMODULATION SYSTEMS
    // =============================================================================

    // Neuromodulator - abstract base class
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator", R"pbdoc(
        Abstract base class for neuromodulatory signals.
        
        Neuromodulators provide global regulatory signals that influence neural
        processing, learning, attention, and motivation.
    )pbdoc")
        .def("getName", &Neuromodulator::getName, R"pbdoc(Get neuromodulator name)pbdoc")
        .def("getLevel", &Neuromodulator::getLevel, R"pbdoc(Get neuromodulator level)pbdoc")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), R"pbdoc(Set neuromodulator level)pbdoc")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, R"pbdoc(Get plasticity modulation factor)pbdoc")
        .def("update", &Neuromodulator::update, py::arg("dt"), R"pbdoc(Update neuromodulator state)pbdoc");

    // Dopamine - reward and reinforcement
    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(
        Dopamine neuromodulator for reward signaling and reinforcement learning.
        
        Dopamine encodes reward prediction errors and modulates plasticity
        to strengthen associations between actions and their outcomes.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName, R"pbdoc(Get neuromodulator name)pbdoc")
        .def("getLevel", &Dopamine::getLevel, R"pbdoc(Get dopamine level)pbdoc")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"), R"pbdoc(Set dopamine level)pbdoc")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor, R"pbdoc(Get plasticity factor)pbdoc")
        .def("update", &Dopamine::update, py::arg("dt"), R"pbdoc(Update dopamine state)pbdoc")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), R"pbdoc(Signal reward)pbdoc")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), R"pbdoc(Signal reward prediction error)pbdoc");

    // Curiosity - exploration motivation
    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity", R"pbdoc(
        Curiosity neuromodulator for exploration and intrinsic motivation.
        
        Curiosity drives agents to explore novel and informative states,
        promoting discovery and learning through intrinsic motivation.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &Curiosity::getName, R"pbdoc(Get neuromodulator name)pbdoc")
        .def("getLevel", &Curiosity::getLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"), R"pbdoc(Set curiosity level)pbdoc")
        .def("getPlasticityFactor", &Curiosity::getPlasticityFactor, R"pbdoc(Get plasticity factor)pbdoc")
        .def("update", &Curiosity::update, py::arg("dt"), R"pbdoc(Update curiosity state)pbdoc");

    // Novelty - novelty detection
    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty", R"pbdoc(
        Novelty detection neuromodulator for change detection.
        
        Novelty signals respond to unexpected or changing sensory input,
        promoting learning when the environment deviates from predictions.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &Novelty::getName, R"pbdoc(Get neuromodulator name)pbdoc")
        .def("getLevel", &Novelty::getLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), R"pbdoc(Set novelty level)pbdoc")
        .def("getPlasticityFactor", &Novelty::getPlasticityFactor, R"pbdoc(Get plasticity factor)pbdoc")
        .def("update", &Novelty::update, py::arg("dt"), R"pbdoc(Update novelty state)pbdoc");

    // PredictionError - prediction error signal
    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError", R"pbdoc(
        Prediction error neuromodulator for predictive coding.
        
        Prediction error signals the difference between expected and actual
        sensory input, driving learning to improve predictive accuracy.
    )pbdoc")
        .def(py::init<>())
        .def("getName", &PredictionError::getName, R"pbdoc(Get neuromodulator name)pbdoc")
        .def("getLevel", &PredictionError::getLevel, R"pbdoc(Get prediction error level)pbdoc")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"), R"pbdoc(Set prediction error level)pbdoc")
        .def("getPlasticityFactor", &PredictionError::getPlasticityFactor, R"pbdoc(Get plasticity factor)pbdoc")
        .def("update", &PredictionError::update, py::arg("dt"), R"pbdoc(Update prediction error state)pbdoc");

    // =============================================================================
    // HIGH-LEVEL RESEARCH APIS
    // =============================================================================

    // Factory functions for common research scenarios
    m.def("createResearchBrain", [](size_t neuronCount, float connectionDensity) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int>(neuronCount));
        config->set("brain.synapse_density", connectionDensity);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("neuronCount"), py::arg("connectionDensity"),
        R"pbdoc(Create a research brain with specified size and connectivity)pbdoc");

    m.def("createSimpleResearchWorld", [](int width, int height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, std::min(width/4, 8), std::min(height/4, 8));
        world->reset();
        return world;
    }, py::arg("width"), py::arg("height"),
        R"pbdoc(Create a simple research world)pbdoc");

    m.def("createAgentWithDefaults", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        R"pbdoc(Create agent with default subsystems enabled)pbdoc");

    m.def("createCompleteResearchSetup", []() {
        // Complete research setup with all subsystems
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Enable all learning systems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return std::make_tuple(brain, world, agent);
    }, R"pbdoc(Create complete research setup with all subsystems)pbdoc");

    // Advanced batch operations
    m.def("createNeuralPopulations", [](std::shared_ptr<NeuralRegion> region, size_t size, NeuronType type) {
        std::vector<std::shared_ptr<NeuralPopulation>> populations;
        for (size_t i = 0; i < 5; ++i) {
            PopulationId popId = region->addPopulation(size, type);
            auto pop = region->getPopulation(popId);
            if (pop) {
                populations.push_back(std::shared_ptr<NeuralPopulation>(pop));
            }
        }
        return populations;
    }, py::arg("region"), py::arg("size"), py::arg("type"),
        R"pbdoc(Create multiple neural populations)pbdoc");

    m.def("initializeAllBrains", [](const std::vector<std::shared_ptr<Brain>>& brains, size_t steps) {
        for (auto& brain : brains) {
            brain->initialize();
        }
        // Run warmup steps
        for (size_t step = 0; step < steps; ++step) {
            for (auto& brain : brains) {
                brain->step(step);
            }
        }
    }, py::arg("brains"), py::arg("steps"),
        R"pbdoc(Initialize and warmup multiple brains)pbdoc");

    // =============================================================================
    // UTILITY FUNCTIONS AND STATISTICS
    // =============================================================================

    // Neural network analysis utilities
    m.def("analyzeConnectome", [](std::shared_ptr<NeuralRegion> region) {
        std::unordered_map<std::string, float> analysis;
        analysis["neuron_count"] = static_cast<float>(region->getTotalNeuronCount());
        analysis["population_count"] = static_cast<float>(region->getPopulationCount());
        analysis["synapse_count"] = static_cast<float>(region->getSynapseCount());
        analysis["synaptic_density"] = region->getSynapticDensity();
        analysis["average_synaptic_weight"] = region->getAverageSynapticWeight();
        analysis["firing_rate"] = region->getAverageFiringRate();
        return analysis;
    }, py::arg("region"),
        R"pbdoc(Analyze neural connectome statistics)pbdoc");

    // Convenience function for brain statistics
    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        std::unordered_map<std::string, float> stats;
        stats["total_neurons"] = static_cast<float>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<float>(brain->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<float>(brain->getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<float>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = static_cast<float>(brain->getTotalSpikeCount());
        stats["pending_spike_events"] = static_cast<float>(brain->getPendingSpikeEventCount());
        return stats;
    }, py::arg("brain"),
        R"pbdoc(Get comprehensive brain statistics)pbdoc");

    // Convenience function for brain simulation
    m.def("runBrainSimulation", [](std::shared_ptr<Brain> brain, size_t steps, bool verbose = false) {
        std::vector<std::unordered_map<std::string, float>> stepStats;
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            if (verbose && step % 100 == 0) {
                std::unordered_map<std::string, float> stats;
                stats["step"] = static_cast<float>(step);
                stats["firing_neurons"] = static_cast<float>(brain->getFiringNeuronCount());
                stats["total_spikes"] = static_cast<float>(brain->getTotalSpikeCount());
                stepStats.push_back(stats);
            }
        }
        return stepStats;
    }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false,
        R"pbdoc(Run brain simulation for specified steps)pbdoc");

    // =============================================================================
    // LEGACY COMPATIBILITY
    // =============================================================================

    // Maintain backward compatibility with existing API
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

    // Export constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
