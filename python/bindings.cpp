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

    // ========== FUNDAMENTAL NEURAL CLASSES ==========
    
    // Neuron: Single neuron computation
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Single neuron implementing Leaky Integrate-and-Fire dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential (mV)")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold (mV)")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("getFiringState", &Neuron::getFiringState, "Get current firing state")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is in refractory period")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate")
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance (nS)")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period (steps)")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), "Set refractory period")
        .def("getRestingPotential", &Neuron::getRestingPotential, "Get resting potential (mV)")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), "Set resting potential")
        .def("getResetPotential", &Neuron::getResetPotential, "Get reset potential after spike (mV)")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), "Set reset potential")
        .def("checkThreshold", &Neuron::checkThreshold, "Check if neuron has reached threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get timestamp of last spike")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "LIF integration step, returns true if neuron fired")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory postsynaptic potential")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory postsynaptic potential")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject external current")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear injected current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike at timestamp")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history vector")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, "Get incoming synapses")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, "Get outgoing synapses")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable/disable plasticity rules")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"), "Update neuron for one simulation step")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId().value) + 
                   " type=" + std::to_string(static_cast<int>(n.getType())) + 
                   " potential=" + std::to_string(n.getMembranePotential()) + ">";
        });

    // Synapse: Neural connection between neurons
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection implementing transmission with delays and plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("target"))
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record presynaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record postsynaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get postsynaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable/disable plasticity rules")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace for reward-modulated learning")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Update synapse for one simulation step")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) + 
                   " from=" + std::to_string(s.getSourceNeuron().value) + 
                   " to=" + std::to_string(s.getDestinationNeuron().value) + 
                   " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // NeuralRegion: Brain region containing multiple populations and local connectivity
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing neural populations and local connections)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, const std::string&>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal, "Add a new population")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), py::return_value_policy::reference_internal, "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get number of populations")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add local synapse")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"), py::return_value_policy::reference_internal, "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get number of synapses")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get synapses originating from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get synapses terminating at neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density (fraction of possible connections)")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step all populations and synapses")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons across all populations")
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion id=" + std::to_string(r.getId().value) + 
                   " name=\"" + r.getName() + "\" populations=" + std::to_string(r.getPopulationCount()) + ">";
        });

    // NeuralPopulation: Group of neurons with shared properties
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared type and properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("getSize", &NeuralPopulation::getSize, "Get population size")
        .def("isEmpty", &NeuralPopulation::isEmpty, "Check if population is empty")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"), py::return_value_policy::reference_internal, "Get neuron by index")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons in population")
        .def("getNeuronType", &NeuralPopulation::getNeuronType, "Get neuron type for this population")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"), "Set neuron type")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, "Get count of active neurons")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel, "Get activity level (fraction active)")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize all neurons randomly")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Step all neurons")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds, "Get neuron IDs in this population")
        .def("recordActivity", &NeuralPopulation::recordActivity, "Record current state for statistics")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential, "Get mean membrane potential")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential, "Get variance of membrane potentials")
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation id=" + std::to_string(p.getId().value) + 
                   " size=" + std::to_string(p.getSize()) + 
                   " type=" + std::to_string(static_cast<int>(p.getNeuronType())) + ">";
        });

    // ========== DEVELOPMENT SYSTEMS ==========
    
    // DevelopmentSystem: orchestrates neural development
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Orchestrates neural development and plasticity)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage, "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName, "Get stage name")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("currentStep"), "Update development")
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress, "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge, "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"), "Set developmental age");

    // Synaptogenesis: formation of new synapses
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Formation of new synapses)pbdoc")
        .def(py::init<>())
        .def("getFormationRate", &Synaptogenesis::getFormationRate, "Get formation rate")
        .def("setFormationRate", &Synaptogenesis::setFormationRate, py::arg("rate"), "Set formation rate")
        .def("getTargetDensity", &Synaptogenesis::getTargetDensity, "Get target synapse density")
        .def("setTargetDensity", &Synaptogenesis::setTargetDensity, py::arg("density"), "Set target density")
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("rng"), "Update synapse formation");

    // Pruning: removal of weak or unused synapses
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Removal of weak or unused synapses)pbdoc")
        .def(py::init<>())
        .def("getThreshold", &Pruning::getThreshold, "Get pruning threshold")
        .def("setThreshold", &Pruning::setThreshold, py::arg("threshold"), "Set pruning threshold")
        .def("getPruningRate", &Pruning::getPruningRate, "Get pruning rate")
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"), "Set pruning rate")
        .def("update", &Pruning::update, py::arg("brain"), py::arg("rng"), "Update pruning")
        .def("pruneSynapse", &Pruning::pruneSynapse, py::arg("synapse"), "Force pruning of specific synapse");

    // Maturation: development of adult-level properties
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Development of adult-level neural properties)pbdoc")
        .def(py::init<>())
        .def("getProgress", &Maturation::getProgress, "Get maturation progress (0-1)")
        .def("setProgress", &Maturation::setProgress, py::arg("progress"), "Set maturation progress")
        .def("update", &Maturation::update, py::arg("brain"), py::arg("currentStep"), "Update maturation")
        .def("getMatureThreshold", &Maturation::getMatureThreshold, "Get mature threshold")
        .def("getMatureRestingPotential", &Maturation::getMatureRestingPotential, "Get mature resting potential")
        .def("getMatureTimeConstant", &Maturation::getMatureTimeConstant, "Get mature time constant");

    // ========== PLASTICITY SYSTEMS ==========
    
    // PlasticityRule: Abstract base for plasticity rules
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base for plasticity rules)pbdoc")
        .def("getLearningRate", &PlasticityRule::getLearningRate, "Get learning rate")
        .def("setLearningRate", &PlasticityRule::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getEligibilityTraceDecay", &PlasticityRule::getEligibilityTraceDecay, "Get eligibility trace decay rate")
        .def("setEligibilityTraceDecay", &PlasticityRule::setEligibilityTraceDecay, py::arg("decay"), "Set eligibility trace decay rate")
        .def("computeUpdate", &PlasticityRule::computeUpdate, py::arg("synapse"), py::arg("preSpike"), py::arg("postSpike"), py::arg("timeDelta"), "Compute weight update")
        .def("updateEligibility", &PlasticityRule::updateEligibility, py::arg("synapse"), py::arg("preSpike"), "Update eligibility trace")
        .def("decayEligibility", &PlasticityRule::decayEligibility, py::arg("synapse"), py::arg("decayRate"), "Decay eligibility trace");

    // Hebbian: Correlation-based learning
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Correlation-based learning rule)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getEligibilityTraceDecay", &Hebbian::getEligibilityTraceDecay, "Get eligibility trace decay rate")
        .def("computeUpdate", &Hebbian::computeUpdate, py::arg("synapse"), py::arg("preSpike"), py::arg("postSpike"), py::arg("timeDelta"), "Compute Hebbian weight update")
        .def("getTraceContribution", &Hebbian::getTraceContribution, py::arg("preSpike"), "Get eligibility trace contribution")
        .def("resetTraces", &Hebbian::resetTraces, "Reset all eligibility traces");

    // STDP: Spike timing dependent plasticity
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike timing dependent plasticity)pbdoc")
        .def(py::init<>())
        .def("setPreTraceDecay", &STDP::setPreTraceDecay, py::arg("decay"), "Set presynaptic trace decay rate")
        .def("setPostTraceDecay", &STDP::setPostTraceDecay, py::arg("decay"), "Set postsynaptic trace decay rate")
        .def("setLearningRate", &STDP::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("computeUpdate", &STDP::computeUpdate, py::arg("synapse"), py::arg("preSpike"), py::arg("postSpike"), py::arg("timeDelta"), "Compute STDP weight update")
        .def("getPreTrace", &STDP::getPreTrace, py::arg("synapse"), "Get presynaptic trace")
        .def("getPostTrace", &STDP::getPostTrace, py::arg("synapse"), "Get postsynaptic trace")
        .def("updateTraces", &STDP::updateTraces, py::arg("preSpike"), py::arg("postSpike"), "Update traces based on spikes");

    // StructuralPlasticity: Connection rewiring and rewiring
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Connection rewiring and rewiring)pbdoc")
        .def(py::init<>())
        .def("getRewiringRate", &StructuralPlasticity::getRewiringRate, "Get rewiring rate")
        .def("setRewiringRate", &StructuralPlasticity::setRewiringRate, py::arg("rate"), "Set rewiring rate")
        .def("getPruningThreshold", &StructuralPlasticity::getPruningThreshold, "Get synaptic strength threshold for pruning")
        .def("setPruningThreshold", &StructuralPlasticity::setPruningThreshold, py::arg("threshold"), "Set synaptic strength threshold for pruning")
        .def("rewireConnection", &StructuralPlasticity::rewireConnection, py::arg("synapse"), "Rewire connection to new target")
        .def("addNewConnection", &StructuralPlasticity::addNewConnection, py::arg("source"), py::arg("target"), "Add new synaptic connection")
        .def("pruneWeakConnections", &StructuralPlasticity::pruneWeakConnections, py::arg("threshold"), "Prune weak synaptic connections")
        .def("update", &StructuralPlasticity::update, py::arg("brain"), py::arg("rng"), "Update structural plasticity");

    // ========== PERFORMANCE SYSTEMS ==========
    
    // EventQueue: Spike timing and event scheduling
    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Spike timing and event scheduling)pbdoc")
        .def(py::init<>())
        .def("scheduleSpike", &EventQueue::scheduleSpike, py::arg("neuron"), py::arg("time"), "Schedule spike event")
        .def("schedulePlasticity", &EventQueue::schedulePlasticity, py::arg("synapse"), py::arg("time"), "Schedule plasticity event")
        .def("getNextEvent", &EventQueue::getNextEvent, "Get next scheduled event")
        .def("advanceTime", &EventQueue::advanceTime, py::arg("time"), "Advance time and process events")
        .def("getEventCount", &EventQueue::getEventCount, "Get number of events in queue")
        .def("clear", &EventQueue::clear, "Clear all events from queue")
        .def("isEmpty", &EventQueue::isEmpty, "Check if event queue is empty");

    // MemoryPool: Memory management and allocation
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Memory management and allocation)pbdoc")
        .def(py::init<>())
        .def("allocateNeuron", &MemoryPool::allocateNeuron, py::arg("type"), "Allocate new neuron")
        .def("allocateSynapse", &MemoryPool::allocateSynapse, py::arg("source"), py::arg("target"), "Allocate new synapse")
        .def("allocateRegion", &MemoryPool::allocateRegion, py::arg("name"), "Allocate new region")
        .def("allocatePopulation", &MemoryPool::allocatePopulation, py::arg("neuronType"), "Allocate new population")
        .def("getNeuronCount", &MemoryPool::getNeuronCount, "Get total number of neurons")
        .def("getSynapseCount", &MemoryPool::getSynapseCount, "Get total number of synapses")
        .def("getRegionCount", &MemoryPool::getRegionCount, "Get total number of regions")
        .def("getPopulationCount", &MemoryPool::getPopulationCount, "Get total number of populations")
        .def("freeNeuron", &MemoryPool::freeNeuron, py::arg("neuron"), "Free neuron memory")
        .def("freeSynapse", &MemoryPool::freeSynapse, py::arg("synapse"), "Free synapse memory")
        .def("freeRegion", &MemoryPool::freeRegion, py::arg("region"), "Free region memory")
        .def("freePopulation", &MemoryPool::freePopulation, py::arg("population"), "Free population memory")
        .def("compact", &MemoryPool::compact, "Compact memory pool")
        .def("getUsageStats", &MemoryPool::getUsageStats, "Get memory usage statistics");

    // Performance: Simulation performance monitoring
    py::class_<Performance>(m, "Performance", R"pbdoc(Simulation performance monitoring)pbdoc")
        .def(py::init<>())
        .def("startMonitoring", &Performance::startMonitoring, "Start performance monitoring")
        .def("stopMonitoring", &Performance::stopMonitoring, "Stop performance monitoring")
        .def("getCPUUsage", &Performance::getCPUUsage, "Get CPU usage percentage")
        .def("getMemoryUsage", &Performance::getMemoryUsage, "Get memory usage (MB)")
        .def("getSimulationTime", &Performance::getSimulationTime, "Get total simulation time (seconds)")
        .def("getStepCount", &Performance::getStepCount, "Get number of simulation steps taken")
        .def("getAverageStepTime", &Performance::getAverageStepTime, "Get average step execution time")
        .def("getMaxStepTime", &Performance::getMaxStepTime, "Get maximum step execution time")
        .def("resetStats", &Performance::resetStats, "Reset performance statistics")
        .def("isMonitoring", &Performance::isMonitoring, "Check if monitoring is active");

    // OptimizedBrain: Optimized neural simulation with vectorized operations
    py::class_<OptimizedBrain>(m, "OptimizedBrain", R"pbdoc(Optimized neural simulation with vectorized operations)pbdoc")
        .def(py::init<>())
        .def("initialize", &OptimizedBrain::initialize, py::arg("neuronCount"), py::arg("synapseCount"), "Initialize optimized brain")
        .def("step", &OptimizedBrain::step, py::arg("currentTime"), py::arg("dt"), "Optimized simulation step")
        .def("getFiringRate", &OptimizedBrain::getFiringRate, "Get population firing rate")
        .def("getAverageMembranePotential", &OptimizedBrain::getAverageMembranePotential, "Get average membrane potential")
        .def("getActivityPattern", &OptimizedBrain::getActivityPattern, "Get neural activity pattern")
        .def("setVectorizedMode", &OptimizedBrain::setVectorizedMode, py::arg("enabled"), "Enable/disable vectorized operations")
        .def("isVectorizedMode", &OptimizedBrain::isVectorizedMode, "Check if vectorized mode is enabled")
        .def("getVectorizedSpeedup", &OptimizedBrain::getVectorizedSpeedup, "Get vectorized performance speedup factor")
        .def("compareWithReference", &OptimizedBrain::compareWithReference, "Compare results with reference implementation");

    // ========== COGNIITION SYSTEMS ==========
    
    // ConceptFormation: Pattern discovery and concept learning
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Pattern discovery and concept learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("detectPattern", &ConceptFormation::detectPattern, py::arg("activityPattern"), "Detect patterns in neural activity")
        .def("extractConcept", &ConceptFormation::extractConcept, py::arg("pattern"), "Extract concept from pattern")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"), "Get concept by ID")
        .def("getAllConcepts", &ConceptFormation::getAllConcepts, "Get all learned concepts")
        .def("getConceptAssociations", &ConceptFormation::getConceptAssociations, "Get concept associations")
        .def("updateConcept", &ConceptFormation::updateConcept, py::arg("conceptId"), py::arg("newAssociations"), "Update concept associations")
        .def("forgetConcept", &ConceptFormation::forgetConcept, py::arg("conceptId"), "Forget concept")
        .def("getConceptStrength", &ConceptFormation::getConceptStrength, py::arg("conceptId"), "Get concept strength")
        .def("getConceptUsageCount", &ConceptFormation::getConceptUsageCount, py::arg("conceptId"), "Get concept usage count")
        .def("getLearningRate", &ConceptFormation::getLearningRate, "Get concept learning rate")
        .def("setLearningRate", &ConceptFormation::setLearningRate, py::arg("rate"), "Set concept learning rate")
        .def("update", &ConceptFormation::update, py::arg("brain"), py::arg("dt"), "Update concept formation");

    // NeuralPlanner: Action planning and decision making
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Action planning and decision making)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("availableActions"), "Plan action for current state")
        .def("getBestAction", &NeuralPlanner::getBestAction, py::arg("currentState"), "Get best action for current state")
        .def("setActionPriority", &NeuralPlanner::setActionPriority, py::arg("action"), py::arg("priority"), "Set action priority")
        .def("getActionPriority", &NeuralPlanner::getActionPriority, py::arg("action"), "Get action priority")
        .def("addActionValue", &NeuralPlanner::addActionValue, py::arg("action"), py::arg("value"), "Add value to action")
        .def("updateActionValues", &NeuralPlanner::updateActionValues, py::arg("reward"), py::arg("actionTaken"), "Update action values based on reward")
        .def("getActionValues", &NeuralPlanner::getActionValues, "Get current action values")
        .def("getPolicy", &NeuralPlanner::getPolicy, "Get current policy")
        .def("updatePolicy", &NeuralPlanner::updatePolicy, py::arg("learningRate"), "Update policy based on learning rate")
        .def("getExplorationRate", &NeuralPlanner::getExplorationRate, "Get exploration rate")
        .def("setExplorationRate", &NeuralPlanner::setExplorationRate, py::arg("rate"), "Set exploration rate")
        .def("selectAction", &NeuralPlanner::selectAction, py::arg("state"), py::arg("epsilon"), "Select action based on epsilon-greedy policy")
        .def("update", &NeuralPlanner::update, py::arg("brain"), py::arg("dt"), "Update neural planner");

    // ========== ENVIRONMENT SYSTEMS ==========
    
    // Observation: Sensory data and environmental information
    py::class_<Observation>(m, "Observation", R"pbdoc(Sensory data and environmental information)pbdoc")
        .def(py::init<>())
        .def("getSensorData", &Observation::getSensorData, "Get sensor data vector")
        .def("getObservationTime", &Observation::getObservationTime, "Get observation time")
        .def("setObservationTime", &Observation::setObservationTime, py::arg("time"), "Set observation time")
        .def("getSensorType", &Observation::getSensorType, "Get sensor type")
        .def("setSensorType", &Observation::setSensorType, py::arg("type"), "Set sensor type")
        .def("getFeatureVector", &Observation::getFeatureVector, "Get feature vector")
        .def("setFeatureVector", &Observation::setFeatureVector, py::arg("features"), "Set feature vector")
        .def("getAllFeatures", &Observation::getAllFeatures, "Get all features")
        .def("getFeature", &Observation::getFeature, py::arg("index"), "Get feature by index")
        .def("setFeature", &Observation::setFeature, py::arg("index"), py::arg("value"), "Set feature by index")
        .def("getFeatureCount", &Observation::getFeatureCount, "Get number of features")
        .def("addFeature", &Observation::addFeature, py::arg("value"), "Add feature")
        .def("clearFeatures", &Observation::clearFeatures, "Clear all features")
        .def("normalize", &Observation::normalize, "Normalize features to [0,1] range")
        .def("copy", &Observation::copy, "Create copy of observation")
        .def("equals", &Observation::equals, py::arg("other"), "Check if observation equals another")
        .def("__repr__", [](const Observation& obs) {
            return "<Observation features=" + std::to_string(obs.getFeatureCount()) + ">";
        });

    // Environment: World simulation and environmental dynamics
    py::class_<Environment>(m, "Environment", R"pbdoc(World simulation and environmental dynamics)pbdoc")
        .def(py::init<>())
        .def("initialize", &Environment::initialize, py::arg("width"), py::arg("height"), "Initialize environment dimensions")
        .def("getWidth", &Environment::getWidth, "Get environment width")
        .def("getHeight", &Environment::getHeight, "Get environment height")
        .def("setAgentPosition", &Environment::setAgentPosition, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getAgentPosition", &Environment::getAgentPosition, "Get agent position")
        .def("addObject", &Environment::addObject, py::arg("obj"), "Add object to environment")
        .def("removeObject", &Environment::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("getObjectAt", &Environment::getObjectAt, py::arg("x"), py::arg("y"), "Get object at position")
        .def("getAllObjects", &Environment::getAllObjects, "Get all objects in environment")
        .def("step", &Environment::step, py::arg("action"), "Process agent action and update environment")
        .def("reset", &Environment::reset, "Reset environment to initial state")
        .def("getReward", &Environment::getReward, py::arg("action"), "Get reward for action")
        .def("isTerminal", &Environment::isTerminal, "Check if environment is in terminal state")
        .def("getTerminalReason", &Environment::getTerminalReason, "Get terminal reason")
        .def("setTerminal", &Environment::setTerminal, py::arg("terminal"), "Set terminal state")
        .def("getEpisodeCount", &Environment::getEpisodeCount, "Get number of episodes")
        .def("getStepCount", &Environment::getStepCount, "Get number of steps in current episode")
        .def("getObservation", &Environment::getObservation, "Get current observation")
        .def("setObservation", &Environment::setObservation, py::arg("obs"), "Set current observation")
        .def("getAvailableActions", &Environment::getAvailableActions, "Get available actions")
        .def("isActionValid", &Environment::isActionValid, py::arg("action"), "Check if action is valid")
        .def("getAgentState", &Environment::getAgentState, "Get agent state")
        .def("setAgentState", &Environment::setAgentState, py::arg("state"), "Set agent state")
        .def("update", &Environment::update, py::arg("action"), py::arg("dt"), "Update environment based on action");

    // GridWorldEnvironment: 2D grid-based environment
    py::class_<GridWorldEnvironment, Environment>(m, "GridWorldEnvironment", R"pbdoc(2D grid-based environment)pbdoc")
        .def(py::init<>())
        .def("initialize", &GridWorldEnvironment::initialize, py::arg("width"), py::arg("height"), "Initialize grid world")
        .def("getCellType", &GridWorldEnvironment::getCellType, py::arg("x"), py::arg("y"), "Get cell type at position")
        .def("setCellType", &GridWorldEnvironment::setCellType, py::arg("x"), py::arg("y"), py::arg("type"), "Set cell type at position")
        .def("getTerrain", &GridWorldEnvironment::getTerrain, py::arg("x"), py::arg("y"), "Get terrain at position")
        .def("setTerrain", &GridWorldEnvironment::setTerrain, py::arg("x"), py::arg("y"), py::arg("terrain"), "Set terrain at position")
        .def("getResource", &GridWorldEnvironment::getResource, py::arg("x"), py::arg("y"), "Get resource at position")
        .def("setResource", &GridWorldEnvironment::setResource, py::arg("x"), py::arg("y"), py::arg("resource"), "Set resource at position")
        .def("getAgentEnergy", &GridWorldEnvironment::getAgentEnergy, "Get agent energy")
        .def("setAgentEnergy", &GridWorldEnvironment::setAgentEnergy, py::arg("energy"), "Set agent energy")
        .def("getMaxEnergy", &GridWorldEnvironment::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &GridWorldEnvironment::setMaxEnergy, py::arg("max"), "Set maximum energy")
        .def("getEnergyDecayRate", &GridWorldEnvironment::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &GridWorldEnvironment::setEnergyDecayRate, py::arg("rate"), "Set energy decay rate")
        .def("isWalkable", &GridWorldEnvironment::isWalkable, py::arg("x"), py::arg("y"), "Check if cell is walkable")
        .def("setWalkable", &GridWorldEnvironment::setWalkable, py::arg("x"), py::arg("y"), py::arg("walkable"), "Set walkability")
        .def("getRandomWalkablePosition", &GridWorldEnvironment::getRandomWalkablePosition, "Get random walkable position")
        .def("getVisibleArea", &GridWorldEnvironment::getVisibleArea, py::arg("centerX"), py::arg("centerY"), "Get visible area around position")
        .def("setVisibilityRange", &GridWorldEnvironment::setVisibilityRange, py::arg("range"), "Set visibility range")
        .def("getVisibilityRange", &GridWorldEnvironment::getVisibilityRange, "Get visibility range")
        .def("updateLightConditions", &GridWorldEnvironment::updateLightConditions, py::arg("timeOfDay"), "Update light conditions")
        .def("getLightLevel", &GridWorldEnvironment::getLightLevel, "Get current light level")
        .def("setLightLevel", &GridWorldEnvironment::setLightLevel, py::arg("level"), "Set light level")
        .def("addHazard", &GridWorldEnvironment::addHazard, py::arg("x"), py::arg("y"), py::arg("hazardType"), "Add hazard at position")
        .def("removeHazard", &GridWorldEnvironment::removeHazard, py::arg("x"), py::arg("y"), "Remove hazard at position")
        .def("getHazard", &GridWorldEnvironment::getHazard, py::arg("x"), py::arg("y"), "Get hazard at position")
        .def("getHazards", &GridWorldEnvironment::getHazards, "Get all hazards")
        .def("applyHazards", &GridWorldEnvironment::applyHazards, "Apply hazards to agent")
        .def("clearHazards", &GridWorldEnvironment::clearHazards, "Clear all hazards");

    // Memory systems

    // Working memory: temporary active storage of information
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Temporary active storage of information)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store item")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"), "Retrieve item")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"), "Check if item exists")
        .def("clear", &WorkingMemory::clear, "Clear all")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current size")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"), "Decay all items");
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("neuron"), py::arg("value"), "Store item")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("neuron"), "Retrieve item")
        .def("contains", &WorkingMemory::contains, py::arg("neuron"), "Check if item exists")
        .def("clear", &WorkingMemory::clear, "Clear all")
        .def("getCapacity", &WorkingMemory::getCapacity, "Get capacity")
        .def("getCurrentSize", &WorkingMemory::getCurrentSize, "Get current size")
        .def("decay", &WorkingMemory::decay, py::arg("decayRate"), "Decay all items");

    // Episodic memory: storage of events and experiences
    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("episode"), "Store episode")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("index"), "Retrieve episode")
        .def("getEpisodeCount", &EpisodicMemory::getEpisodeCount, "Get episode count")
        .def("getRecentEpisodes", &EpisodicMemory::getRecentEpisodes, py::arg("count"), "Get recent episodes")
        .def("clear", &EpisodicMemory::clear, "Clear all")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("relevanceThreshold"), "Consolidate memory");

    // Semantic memory: gradually acquired knowledge
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Continuously acquired knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("key"), py::arg("value"), "Store fact")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("key"), "Retrieve fact")
        .def("hasFact", &SemanticMemory::hasFact, py::arg("key"), "Check if fact exists")
        .def("getAllFacts", &SemanticMemory::getAllFacts, "Get all facts")
        .def("clear", &SemanticMemory::clear, "Clear all");

    // Procedural memory: learned skills and habits
    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", &ProceduralMemory::learnSkill, py::arg("name"), py::arg("pattern"), "Learn skill")
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"), "Get skill")
        .def("getAllSkills", &ProceduralMemory::getAllSkills, "Get all skills")
        .def("updateProficiency", &ProceduralMemory::updateProficiency, py::arg("name"), py::arg("delta"), "Update proficiency")
        .def("clear", &ProceduralMemory::clear, "Clear all");

    // NeuralWorkingMemory: Implements working memory using neural dynamics
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural-based working memory with persistent activity)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f, "Store information")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"), "Store to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve current working memory content")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"), "Check if neuron is in working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"), "Get activation level")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"), "Set capacity")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"), "Set decay rate")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons, "Get neurons in working memory")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"), "Strengthen memory")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition, "Run competition between traces")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"), "Check if neuron is winning")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity, "Get overall memory activity");

    // NeuralEpisodicMemory: Stores experiences as neural activity patterns
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural-based episodic memory with pattern storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"), "Store episode")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, 
             py::arg("sensoryPattern"), py::arg("maxResults") = 5, "Retrieve similar episodes")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10, "Retrieve temporal episodes")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5, "Retrieve episodes by location")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5, "Retrieve episodes after action")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"), "Replay episode")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance, 
             py::arg("episodeId"), py::arg("relevanceDelta"), "Update relevance")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"), "Get episode")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get episode count");

    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Neuromodulator: Abstract base for neuromodulatory signals
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current concentration/level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set current concentration/level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get neuromodulatory effect on plasticity")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update neuromodulator state");

    // Dopamine: Reward and reinforcement learning signal
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error")
        .def("update", &Dopamine::update, py::arg("dt"), "Update dopamine state")
        .def("__repr__", [](const Dopamine& d) {
            return "<Dopamine level=" + std::to_string(d.getLevel()) + ">";
        });

    // Acetylcholine: Attention and memory consolidation
    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Attention and memory consolidation signal)pbdoc")
        .def(py::init<>())
        .def("__repr__", [](const Acetylcholine& a) {
            return "<Acetylcholine>";
        });

    // Norepinephrine: Arousal and vigilance
    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Arousal and vigilance signal)pbdoc")
        .def(py::init<>())
        .def("__repr__", [](const Norepinephrine& n) {
            return "<Norepinephrine>";
        });

    // Serotonin: Mood, impulsivity, and social behavior
    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Mood, impulsivity, and social behavior signal)pbdoc")
        .def(py::init<>())
        .def("__repr__", [](const Serotonin& s) {
            return "<Serotonin>";
        });

    // Curiosity drive: exploration motivation based on novelty and prediction error
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration drive")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), "Update curiosity based on novelty and prediction error")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"), "Set novelty weight")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"), "Set prediction error weight")
        .def("reset", &Curiosity::reset, "Reset curiosity");

    // Novelty detection: measures unexpected sensory input
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Unexpected sensory input detection)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("observation"), "Detect novelty in sensory observation")
        .def("detectNovelty", static_cast<void (Novelty::*)(const std::vector<float>&)>(&Novelty::detectNovelty), 
             py::arg("pattern"), "Detect novelty in neural activity pattern")
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("update", &Novelty::update, py::arg("dt"), "Update novelty state")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("getHistory", &Novelty::getHistory, "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory, "Clear novelty history")
        .def("reset", &Novelty::reset, "Reset novelty");

    // Prediction error: difference between predicted and actual states
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction mismatch signal)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("getError", &PredictionError::getError, "Get prediction error value")
        .def("getMagnitude", &PredictionError::getMagnitude, "Get prediction error magnitude")
        .def("getHistory", &PredictionError::getHistory, "Get prediction error history")
        .def("clearHistory", &PredictionError::clearHistory, "Clear prediction error history")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"), "Update prediction")
        .def("reset", &PredictionError::reset, "Reset prediction error");

    // Reward signal: reinforcement learning feedback
    py::class_<Reward>(m, "Reward", R"pbdoc(Reinforcement learning feedback signal)pbdoc")
        .def(py::init<>())
        .def("add", &Reward::add, py::arg("delta"), "Add reward delta")
        .def("computeReward", &Reward::computeReward, py::arg("observation"), "Compute reward from observation")
        .def("getValue", &Reward::getValue, "Get current reward value")
        .def("setValue", &Reward::setValue, py::arg("value"), "Set reward value")
        .def("getHistory", &Reward::getHistory, "Get reward history")
        .def("clearHistory", &Reward::clearHistory, "Clear reward history")
        .def("reset", &Reward::reset, "Reset reward");

    // PredictionSystem: predicts future sensory states and consequences
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Predicts future sensory states and consequences)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"), "Make prediction for next timestep")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"), "Update predictions based on observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError, "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence, "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory, "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory, "Clear prediction history")
        .def("train", &PredictionSystem::train, py::arg("observation"), "Train prediction model");

    // NeuralPrediction: temporal prediction using spiking substrate
    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Temporal prediction using learned synaptic patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState,
             py::arg("sensoryState"), py::arg("currentStep"), "Record sensory state for prediction")
        .def("generatePrediction", &NeuralPrediction::generatePrediction,
             py::arg("currentStep"), "Generate prediction for next timestep")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation,
             py::arg("actualState"), py::arg("currentStep"), "Update predictions based on observation")
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence,
             py::arg("action"), py::arg("currentState"), "Predict consequences of action")
        .def("getPredictionError", &NeuralPrediction::getPredictionError, "Get prediction error")
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence, "Get prediction confidence")
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps,
             py::arg("currentStep"), py::arg("numSteps"), "Predict multiple steps into future")
        .def("recordAction", &NeuralPrediction::recordAction,
             py::arg("action"), py::arg("step"), "Record action for learning")
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory, "Get prediction error history")
        .def("clearHistory", &NeuralPrediction::clearHistory, "Clear prediction history")
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction,
             py::arg("enable"), "Enable/disable temporal prediction")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction,
             py::arg("enable"), "Enable/disable action consequence prediction")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize,
             py::arg("size"), "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon,
             py::arg("steps"), "Set prediction horizon")
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons, "Get neurons involved in prediction")
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons, "Get neurons for sequence prediction");

    // ActionConsequencePredictor: learns action -> consequence mappings
    py::class_<ActionConsequencePredictor>(m, "ActionConsequencePredictor", 
                                           R"pbdoc(Learns action -> consequence mappings)pbdoc")
        .def(py::init<>())
        .def("initialize", &ActionConsequencePredictor::initialize, py::arg("brain"), "Initialize with brain reference")
        .def("recordExperience", &ActionConsequencePredictor::recordExperience,
             py::arg("action"), py::arg("beforeState"), py::arg("afterState"), py::arg("reward"), "Record experience")
        .def("predictConsequence", &ActionConsequencePredictor::predictConsequence,
             py::arg("action"), py::arg("currentState"), "Predict consequence of action")
        .def("getConsequenceConfidence", &ActionConsequencePredictor::getConsequenceConfidence,
             py::arg("action"), py::arg("currentState"), "Get prediction confidence")
        .def("updatePrediction", &ActionConsequencePredictor::updatePrediction,
             py::arg("action"), py::arg("predicted"), py::arg("actual"), py::arg("reward"), "Update prediction")
        .def("getActionQuality", &ActionConsequencePredictor::getActionQuality, py::arg("action"), "Get action quality")
        .def("clear", &ActionConsequencePredictor::clear, "Clear learned associations");

    // PredictionErrorSignal: computes and broadcasts prediction error
    py::class_<PredictionErrorSignal>(m, "PredictionErrorSignal", R"pbdoc(Computes and broadcasts prediction error)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionErrorSignal::computeError,
             py::arg("predicted"), py::arg("actual"), "Compute error between predicted and actual")
        .def("getModulationSignal", &PredictionErrorSignal::getModulationSignal, "Get neuromodulation signal strength")
        .def("recordError", &PredictionErrorSignal::recordError,
             py::arg("error"), py::arg("step"), "Record error in history")
        .def("getErrorHistory", &PredictionErrorSignal::getErrorHistory, "Get error history")
        .def("isSurprising", &PredictionErrorSignal::isSurprising, "Check if error is surprising")
        .def("setSurpriseThreshold", &PredictionErrorSignal::setSurpriseThreshold,
             py::arg("t"), "Set surprise threshold")
        .def("getSurpriseThreshold", &PredictionErrorSignal::getSurpriseThreshold, "Get surprise threshold");
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

    // ========== FUNDAMENTAL NEURAL CLASSES ==========
    
    // Neuron: Single neuron computation
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Single neuron implementing Leaky Integrate-and-Fire dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential (mV)")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold (mV)")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("getFiringState", &Neuron::getFiringState, "Get current firing state")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is currently firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is in refractory period")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate (Hz)")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate")
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance (nS)")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, "Get refractory period (steps)")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), "Set refractory period")
        .def("getRestingPotential", &Neuron::getRestingPotential, "Get resting potential (mV)")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), "Set resting potential")
        .def("getResetPotential", &Neuron::getResetPotential, "Get reset potential after spike (mV)")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), "Set reset potential")
        .def("checkThreshold", &Neuron::checkThreshold, "Check if neuron has reached threshold")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get timestamp of last spike")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "LIF integration step, returns true if neuron fired")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory postsynaptic potential")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory postsynaptic potential")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject external current")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, "Clear injected current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike at timestamp")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history vector")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, "Get incoming synapses")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, "Get outgoing synapses")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable/disable plasticity rules")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("getRegionId", &Neuron::getRegionId, "Get region ID")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("getPopulationId", &Neuron::getPopulationId, "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"), "Update neuron for one simulation step")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId().value) + 
                   " type=" + std::to_string(static_cast<int>(n.getType())) + 
                   " potential=" + std::to_string(n.getMembranePotential()) + ">";
        });

    // Synapse: Neural connection between neurons
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection implementing transmission with delays and plasticity)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("target"))
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay (steps)")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record presynaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record postsynaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get postsynaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), "Enable/disable plasticity rules")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace for reward-modulated learning")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), "Decay eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Update synapse for one simulation step")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) + 
                   " from=" + std::to_string(s.getSourceNeuron().value) + 
                   " to=" + std::to_string(s.getDestinationNeuron().value) + 
                   " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // NeuralRegion: Brain region containing multiple populations and local connectivity
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing neural populations and local connections)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, const std::string&>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal, "Add a new population")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), py::return_value_policy::reference_internal, "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, "Get number of populations")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1, "Add local synapse")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"), py::return_value_policy::reference_internal, "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, "Get number of synapses")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), "Get synapses originating from neuron")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), "Get synapses terminating at neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, "Get total neuron count")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, "Get average firing rate")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, "Get average synaptic weight")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, "Get synaptic density (fraction of possible connections)")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step all populations and synapses")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, "Get all neurons across all populations")
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion id=" + std::to_string(r.getId().value) + 
                   " name=\"" + r.getName() + "\" populations=" + std::to_string(r.getPopulationCount()) + ">";
        });

    // NeuralPopulation: Group of neurons with shared properties
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared type and properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("getSize", &NeuralPopulation::getSize, "Get population size")
        .def("isEmpty", &NeuralPopulation::isEmpty, "Check if population is empty")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"), py::return_value_policy::reference_internal, "Get neuron by index")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron to population")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons in population")
        .def("getNeuronType", &NeuralPopulation::getNeuronType, "Get neuron type for this population")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"), "Set neuron type")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, "Get count of active neurons")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, "Get count of firing neurons")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, "Get average firing rate")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel, "Get activity level (fraction active)")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), "Initialize all neurons randomly")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Step all neurons")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds, "Get neuron IDs in this population")
        .def("recordActivity", &NeuralPopulation::recordActivity, "Record current state for statistics")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential, "Get mean membrane potential")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential, "Get variance of membrane potentials")
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation id=" + std::to_string(p.getId().value) + 
                   " size=" + std::to_string(p.getSize()) + 
                   " type=" + std::to_string(static_cast<int>(p.getNeuronType())) + ">";
        });

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

// ========== UTILITY FUNCTIONS ==========

// Statistical analysis functions
m.def("getNetworkStatistics", &Brain::getNetworkStatistics, R"pbdoc(Get comprehensive brain statistics)pbdoc");
m.def("getConnectionStatistics", [](std::shared_ptr<Brain> brain) {
    return brain->getConnectionStatistics();
}, py::arg("brain"), R"pbdoc(Get synapse and connectivity statistics)pbdoc");
m.def("getActivityPattern", [](std::shared_ptr<Brain> brain, int steps) {
    return brain->getActivityPattern(steps);
}, py::arg("brain"), py::arg("steps"), R"pbdoc(Get neural activity pattern)pbdoc");
m.def("analyzePlasticity", [](std::shared_ptr<Brain> brain, float timeWindow) {
    return brain->analyzePlasticity(timeWindow);
}, py::arg("brain"), py::arg("timeWindow"), R"pbdoc(Analyze plasticity rates)pbdoc");

// Helper factory functions
m.def("create_network", [](int neuron_count, float connection_probability) {
    auto config = std::make_shared<Config>();
    config->set_int("neuron_count", neuron_count);
    config->set_float("connection_probability", connection_probability);
    return std::make_shared<Brain>(config);
}, py::arg("neuron_count"), py::arg("connection_probability"), R"pbdoc(Create a neural network)pbdoc");
 
 m.def("create_region", [](std::shared_ptr<Brain> brain, const std::string& name, NeuronType neuron_type, size_t size) {
    return brain->addRegion(name);
}, py::arg("brain"), py::arg("name"), py::arg("neuron_type"), py::arg("size"), R"pbdoc(Create a neural region)pbdoc");
 
 m.def("create_synaptic_connection", [](std::shared_ptr<NeuralRegion> region, NeuronId source, NeuronId target, float weight) {
    return region->addSynapse(source, target, weight);
}, py::arg("region"), py::arg("source"), py::arg("target"), py::arg("weight"), R"pbdoc(Create a synaptic connection)pbdoc");
 
 m.def("create_neurons_with_pattern", [](std::shared_ptr<NeuralPopulation> population, const std::vector<float>& pattern) {
    population->initializeRandom(nullptr); // Use default RNG
    return pattern.size();
}, py::arg("population"), py::arg("pattern"), R"pbdoc(Create neurons with activity pattern)pbdoc");

// Serialization utilities
m.def("save_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
    brain->save(filepath);
}, py::arg("brain"), py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc");
 
 m.def("load_brain_state", [](const std::string& filepath) -> std::shared_ptr<Brain> {
    auto config = std::make_shared<Config>();
    config->loadFromFile(filepath);
    return std::make_shared<Brain>(config);
}, py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc");
 
 m.def("serialize_to_dict", [](std::shared_ptr<Brain> brain) {
    return brain->toDict();
}, py::arg("brain"), R"pbdoc(Serialize brain to Python dict)pbdoc");
 
 m.def("deserialize_from_dict", [](const py::dict& data) -> std::shared_ptr<Brain> {
    auto config = std::make_shared<Config>();
    config->loadFromDict(data);
    return std::make_shared<Brain>(config);
}, py::arg("data"), R"pbdoc(Deserialize brain from Python dict)pbdoc");

// Performance monitoring APIs
m.def("get_performance_metrics", [](std::shared_ptr<Brain> brain) {
    return brain->getPerformanceMetrics();
}, py::arg("brain"), R"pbdoc(Get brain performance metrics)pbdoc");
 
 m.def("start_profiling", [](std::shared_ptr<Brain> brain) {
    brain->startProfiling();
}, py::arg("brain"), R"pbdoc(Start performance profiling)pbdoc");
 
 m.def("stop_profiling", [](std::shared_ptr<Brain> brain) {
    return brain->stopProfiling();
}, py::arg("brain"), R"pbdoc(Stop profiling and get results)pbdoc");
 
 m.def("benchmark_operation", [](std::shared_ptr<Brain> brain, std::function<void(std::shared_ptr<Brain>)> operation, int steps) {
    return brain->benchmark(operation, steps);
}, py::arg("brain"), py::arg("operation"), py::arg("steps"), R"pbdoc(Benchmark an operation)pbdoc");

// Visualization helpers
m.def("plot_neural_activity", [](std::shared_ptr<Brain> brain, int steps, const std::string& filename) {
    brain->plotNeuralActivity(steps, filename);
}, py::arg("brain"), py::arg("steps"), py::arg("filename"), R"pbdoc(Plot neural activity)pbdoc");
 
 m.def("plot_connectivity_matrix", [](std::shared_ptr<Brain> brain, const std::string& filename) {
    brain->plotConnectivityMatrix(filename);
}, py::arg("brain"), py::arg("filename"), R"pbdoc(Plot connectivity matrix)pbdoc");
 
 m.def("plot_spike_raster", [](std::shared_ptr<Brain> brain, int steps, const std::string& filename) {
    brain->plotSpikeRaster(steps, filename);
}, py::arg("brain"), py::arg("steps"), py::arg("filename"), R"pbdoc(Create spike raster plot)pbdoc");
 
 m.def("plot_plasticity_dynamics", [](std::shared_ptr<Brain> brain, const std::string& filename) {
    brain->plotPlasticityDynamics(filename);
}, py::arg("brain"), py::arg("filename"), R"pbdoc(Plot plasticity dynamics)pbdoc");

// Configuration validation
m.def("validate_configuration", [](const std::shared_ptr<Config>& config) {
    return config->validate();
}, py::arg("config"), R"pbdoc(Validate configuration)pbdoc");
 
 m.def("get_configuration_suggestions", [](const std::shared_ptr<Config>& config) {
    return config->getSuggestions();
}, py::arg("config"), R"pbdoc(Get configuration suggestions)pbdoc");
 
 m.def("compare_configurations", [](const std::shared_ptr<Config>& config1, const std::shared_ptr<Config>& config2) {
    return config1->compare(config2);
}, py::arg("config1"), py::arg("config2"), R"pbdoc(Compare two configurations)pbdoc");

// ========== FACTORY FUNCTIONS ==========

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
