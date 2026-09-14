#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/brain/NeuralRegion.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/motor/Action.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/performance/Performance.hpp"
#include "src/performance/EventQueue.hpp"
#include "src/performance/CheckpointSystem.hpp"
#include "src/performance/MemoryPool.hpp"
#include "src/performance/SIMDVectorization.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/plasticity/PlasticityRule.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/development/Synaptogenesis.hpp"
#include "src/development/Pruning.hpp"
#include "src/development/Maturation.hpp"
#include "src/dynamics/SpikeSystem.hpp"
#include "src/dynamics/NeuralDynamics.hpp"
#include "src/memory/WorkingMemory.hpp"
#include "src/memory/EpisodicMemory.hpp"
#include "src/memory/AssociativeMemory.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/prediction/NeuralPrediction.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/AttentionalSelection.hpp"
#include "src/environment/Environment.hpp"
#include "src/environment/Observation.hpp"
#include "src/sensory/Vision.hpp"
#include "src/sensory/Audio.hpp"
#include "src/sensory/InternalSignals.hpp"
#include "src/motor/MotorSystem.hpp"
#include "src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (熙然) - Neural Learning Machine Python Bindings
        ---------------------------------------------------------
        A comprehensive Python interface for the NLM C++ neural simulation framework.
        
        This module provides access to the complete artificial brain system including:
        - Real spiking neural dynamics (LIF neurons)
        - Multiple memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, reward)
        - Developmental processes (synaptogenesis, pruning, maturation)
        - Sensory-motor integration
        - Prediction and planning systems
        - Plasticity mechanisms (STDP, Hebbian, structural)
        - Visualization and checkpointing
        
        The Python bindings offer both high-level convenience APIs and low-level
        access to the C++ neural simulation core for maximum flexibility.
    )pbdoc";

    // ============= CORE TYPES AND IDENTIFIERS =============
    
    // NeuronId class - unique identifier for neurons
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

    // SynapseId class - unique identifier for synapses
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

    // RegionId class - unique identifier for brain regions
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

    // PopulationId class - unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // ============= ENUMERATIONS =============

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration
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

    // MotorCommand enumeration
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ============= CORE SIMULATION COMPONENTS =============

    // Neuron class - core spiking neuron implementation
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Leaky Integrate-and-Fire neuron with real dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, R"pbdoc(Get the neuron's unique identifier)pbdoc")
        .def("getType", &Neuron::getType, R"pbdoc(Get neuron type)pbdoc")
        .def("setType", &Neuron::setType, py::arg("type"), R"pbdoc(Set neuron type)pbdoc")
        .def("getMembranePotential", &Neuron::getMembranePotential, R"pbdoc(Get membrane potential in mV)pbdoc")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), R"pbdoc(Set membrane potential)pbdoc")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), R"pbdoc(Add to membrane potential)pbdoc")
        .def("getThreshold", &Neuron::getThreshold, R"pbdoc(Get firing threshold in mV)pbdoc")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), R"pbdoc(Set firing threshold)pbdoc")
        .def("isFiring", &Neuron::isFiring, R"pbdoc(Check if neuron is currently firing)pbdoc")
        .def("isRefractory", &Neuron::isRefractory, R"pbdoc(Check if neuron is in refractory period)pbdoc")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), R"pbdoc(Set neuron firing state)pbdoc")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), R"pbdoc(Set refractory period length)pbdoc")
        .def("decrementRefractory", &Neuron::decrementRefractory, R"pbdoc(Decrement refractory counter)pbdoc")
        .def("getFiringRate", &Neuron::getFiringRate, R"pbdoc(Get firing rate in Hz)pbdoc")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), R"pbdoc(Set firing rate)pbdoc")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), R"pbdoc(Set leak conductance)pbdoc")
        .def("getLeakConductance", &Neuron::getLeakConductance, R"pbdoc(Get leak conductance)pbdoc")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, R"pbdoc(Get refractory period length)pbdoc")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), R"pbdoc(Set resting potential)pbdoc")
        .def("getRestingPotential", &Neuron::getRestingPotential, R"pbdoc(Get resting potential)pbdoc")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), R"pbdoc(Set reset potential)pbdoc")
        .def("checkThreshold", &Neuron::checkThreshold, R"pbdoc(Check if neuron has reached firing threshold)pbdoc")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, R"pbdoc(Get timestamp of last spike)pbdoc")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), R"pbdoc(Execute one LIF integration step, returns True if neuron fired)pbdoc")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), R"pbdoc(Receive excitatory synaptic input)pbdoc")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), R"pbdoc(Receive inhibitory synaptic input)pbdoc")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), R"pbdoc(Receive modulatory input for plasticity)pbdoc")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), R"pbdoc(Inject external current)pbdoc")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, R"pbdoc(Get total current input)pbdoc")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, R"pbdoc(Clear accumulated current)pbdoc")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), R"pbdoc(Record a spike event)pbdoc")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, py::return_value_policy::reference_internal, R"pbdoc(Get spike history)pbdoc")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, R"pbdoc(Clear spike history)pbdoc")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), R"pbdoc(Add incoming synapse)pbdoc")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), R"pbdoc(Add outgoing synapse)pbdoc")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, py::return_value_policy::reference_internal, R"pbdoc(Get incoming synapses)pbdoc")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, py::return_value_policy::reference_internal, R"pbdoc(Get outgoing synapses)pbdoc")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, py::return_value_policy::reference_internal, R"pbdoc(Get plasticity flags)pbdoc")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), R"pbdoc(Enable plasticity mechanisms)pbdoc")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), R"pbdoc(Set neuron region)pbdoc")
        .def("getRegionId", &Neuron::getRegionId, R"pbdoc(Get neuron region)pbdoc")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), R"pbdoc(Set neuron population)pbdoc")
        .def("getPopulationId", &Neuron::getPopulationId, R"pbdoc(Get neuron population)pbdoc")
        .def("step", &Neuron::step, py::arg("currentTime"), R"pbdoc(Execute one simulation step)pbdoc")
        .def("reset", &Neuron::reset, R"pbdoc(Reset neuron to initial state)pbdoc")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), R"pbdoc(Initialize neuron with random biological parameters)pbdoc")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId().value) + 
                   " type=" + (n.getType() == NeuronType::Excitatory ? "Excitatory" : "Other") +
                   " V=" + std::to_string(n.getMembranePotential()) + "mV>";
        });

    // Synapse class - connection between neurons
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Neural connection with synaptic dynamics and plasticity)pbdoc")
        .def(py::init<>())
        .def("getId", &Synapse::getId, R"pbdoc(Get synapse identifier)pbdoc")
        .def("setId", &Synapse::setId, py::arg("id"), R"pbdoc(Set synapse identifier)pbdoc")
        .def("getSourceNeuronId", &Synapse::getSourceNeuronId, R"pbdoc(Get source neuron identifier)pbdoc")
        .def("getTargetNeuronId", &Synapse::getTargetNeuronId, R"pbdoc(Get target neuron identifier)pbdoc")
        .def("getWeight", &Synapse::getWeight, R"pbdoc(Get synaptic weight)pbdoc")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), R"pbdoc(Set synaptic weight)pbdoc")
        .def("getDelay", &Synapse::getDelay, R"pbdoc(Get synaptic delay in steps)pbdoc")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), R"pbdoc(Set synaptic delay)pbdoc")
        .def("getType", &Synapse::getType, R"pbdoc(Get synapse type)pbdoc")
        .def("setType", &Synapse::setType, py::arg("type"), R"pbdoc(Set synapse type)pbdoc")
        .def("getPreSynapticTrace", &Synapse::getPreSynapticTrace, R"pbdoc(Get presynaptic eligibility trace)pbdoc")
        .def("getPostSynapticTrace", &Synapse::getPostSynapticTrace, R"pbdoc(Get postsynaptic eligibility trace)pbdoc")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, R"pbdoc(Get total eligibility trace)pbdoc")
        .def("updatePreSynapticTrace", &Synapse::updatePreSynapticTrace, R"pbdoc(Update presynaptic trace)pbdoc")
        .def("updatePostSynapticTrace", &Synapse::updatePostSynapticTrace, R"pbdoc(Update postsynaptic trace)pbdoc")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("rate"), R"pbdoc(Decay eligibility trace)pbdoc")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), R"pbdoc(Enable plasticity mechanisms)pbdoc")
        .def("isPlastic", &Synapse::isPlastic, R"pbdoc(Check if synapse supports plasticity)pbdoc")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), R"pbdoc(Add to synaptic weight)pbdoc")
        .def("clone", &Synapse::clone, py::return_value_policy::take_ownership, R"pbdoc(Create a copy of this synapse)pbdoc")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) +
                   " source=" + std::to_string(s.getSourceNeuronId().value) +
                   " target=" + std::to_string(s.getTargetNeuronId().value) +
                   " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // NeuralPopulation class - collection of neurons with shared properties
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Collection of neurons with shared characteristics)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralPopulation::getId, R"pbdoc(Get population identifier)pbdoc")
        .def("setId", &NeuralPopulation::setId, py::arg("id"), R"pbdoc(Set population identifier)pbdoc")
        .def("getType", &NeuralPopulation::getType, R"pbdoc(Get population neuron type)pbdoc")
        .def("setType", &NeuralPopulation::setType, py::arg("type"), R"pbdoc(Set population neuron type)pbdoc")
        .def("getSize", &NeuralPopulation::getSize, R"pbdoc(Get number of neurons in population)pbdoc")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"), py::return_value_policy::reference_internal, R"pbdoc(Get neuron at index)pbdoc")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), R"pbdoc(Add neuron to population)pbdoc")
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuron"), R"pbdoc(Remove neuron from population)pbdoc")
        .def("clear", &NeuralPopulation::clear, R"pbdoc(Remove all neurons)pbdoc")
        .def("getNeurons", &NeuralPopulation::getNeurons, py::return_value_policy::reference_internal, R"pbdoc(Get all neurons)pbdoc")
        .def("getSynapses", &NeuralPopulation::getSynapses, py::return_value_policy::reference_internal, R"pbdoc(Get all synapses)pbdoc")
        .def("getAvgMembranePotential", &NeuralPopulation::getAvgMembranePotential, R"pbdoc(Get average membrane potential)pbdoc")
        .def("getFiringRate", &NeuralPopulation::getFiringRate, R"pbdoc(Get average firing rate)pbdoc")
        .def("getSpikes", &NeuralPopulation::getSpikes, R"pbdoc(Get spike count)pbdoc")
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation id=" + std::to_string(p.getId().value) +
                   " type=" + (p.getType() == NeuronType::Excitatory ? "Excitatory" : "Other") +
                   " neurons=" + std::to_string(p.getSize()) + ">";
        });

    // NeuralRegion class - brain region with specialized functionality
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region with specialized neural populations and connections)pbdoc")
        .def(py::init<>())
        .def("getId", &NeuralRegion::getId, R"pbdoc(Get region identifier)pbdoc")
        .def("setId", &NeuralRegion::setId, py::arg("id"), R"pbdoc(Set region identifier)pbdoc")
        .def("getName", &NeuralRegion::getName, R"pbdoc(Get region name)pbdoc")
        .def("setName", &NeuralRegion::setName, py::arg("name"), R"pbdoc(Set region name)pbdoc")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("population"), R"pbdoc(Add neural population)pbdoc")
        .def("removePopulation", &NeuralRegion::removePopulation, py::arg("population"), R"pbdoc(Remove population)pbdoc")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get population by ID)pbdoc")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, R"pbdoc(Get number of populations)pbdoc")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations, py::return_value_policy::reference_internal, R"pbdoc(Get all populations)pbdoc")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("synapse"), R"pbdoc(Add synapse to region)pbdoc")
        .def("removeSynapse", &NeuralRegion::removeSynapse, py::arg("synapse"), R"pbdoc(Remove synapse)pbdoc")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get synapse by ID)pbdoc")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, R"pbdoc(Get number of synapses)pbdoc")
        .def("getAllSynapses", &NeuralRegion::getAllSynapses, py::return_value_policy::reference_internal, R"pbdoc(Get all synapses)pbdoc")
        .def("getNeuron", &NeuralRegion::getNeuron, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get neuron by ID)pbdoc")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, py::return_value_policy::reference_internal, R"pbdoc(Get all neurons)pbdoc")
        .def("getAllNeuronsByType", &NeuralRegion::getAllNeuronsByType, py::arg("type"), py::return_value_policy::reference_internal, R"pbdoc(Get neurons by type)pbdoc")
        .def("getExcitatoryNeurons", &NeuralRegion::getExcitatoryNeurons, py::return_value_policy::reference_internal, R"pbdoc(Get all excitatory neurons)pbdoc")
        .def("getInhibitoryNeurons", &NeuralRegion::getInhibitoryNeurons, py::return_value_policy::reference_internal, R"pbdoc(Get all inhibitory neurons)pbdoc")
        .def("getAvgFiringRate", &NeuralRegion::getAvgFiringRate, R"pbdoc(Get average firing rate)pbdoc")
        .def("getTotalSpikeCount", &NeuralRegion::getTotalSpikeCount, R"pbdoc(Get total spike count)pbdoc")
        .def("updatePlasticity", &NeuralRegion::updatePlasticity, py::arg("timeStep"), R"pbdoc(Update plasticity for all neurons)pbdoc")
        .def("advanceTime", &NeuralRegion::advanceTime, py::arg("dt"), R"pbdoc(Advance time and update dynamics)pbdoc")
        .def("reset", &NeuralRegion::reset, R"pbdoc(Reset region state)pbdoc")
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion id=" + std::to_string(r.getId().value) +
                   " name=\"" + r.getName() + "\" " +
                   "populations=" + std::to_string(r.getPopulationCount()) +
                   " neurons=" + std::to_string(r.getAllNeurons().size()) + ">";
        });

    // ============= MEMORY SYSTEMS =============

    // WorkingMemory class - short-term active information storage
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Short-term active information storage)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("key"), py::arg("value"), py::arg("timestamp"), R"pbdoc(Store information with timestamp)pbdoc")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("key"), py::arg("timestamp"), R"pbdoc(Retrieve stored information)pbdoc")
        .def("getValue", &WorkingMemory::getValue, py::arg("key"), py::arg("timestamp"), R"pbdoc(Get stored value)pbdoc")
        .def("has", &WorkingMemory::has, py::arg("key"), py::arg("timestamp"), R"pbdoc(Check if key exists)pbdoc")
        .def("remove", &WorkingMemory::remove, py::arg("key"), py::arg("timestamp"), R"pbdoc(Remove stored information)pbdoc")
        .def("clear", &WorkingMemory::clear, R"pbdoc(Clear all stored information)pbdoc")
        .def("getSize", &WorkingMemory::getSize, R"pbdoc(Get number of stored items)pbdoc")
        .def("getActiveItems", &WorkingMemory::getActiveItems, py::arg("maxAge"), py::return_value_policy::reference_internal, R"pbdoc(Get active items)pbdoc")
        .def("update", &WorkingMemory::update, py::arg("dt"), R"pbdoc(Update working memory)pbdoc")
        .def("decay", &WorkingMemory::decay, py::arg("rate"), R"pbdoc(Apply decay to all items)pbdoc")
        .def("__repr__", [](const WorkingMemory& wm) {
            return "<WorkingMemory items=" + std::to_string(wm.getSize()) + ">";
        });

    // EpisodicMemory class - episodic experience storage
    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory for experience storage and replay)pbdoc")
        .def(py::init<>())
        .def("encode", &EpisodicMemory::encode, py::arg("experience"), py::arg("timestamp"), R"pbdoc(Encode an experience)pbdoc")
        .def("recall", &EpisodicMemory::recall, py::arg("timeWindow"), py::arg("similarityThreshold"), py::return_value_policy::reference_internal, R"pbdoc(Recall similar experiences)pbdoc")
        .def("replay", &EpisodicMemory::replay, py::arg("startTime"), py::arg("duration"), py::arg("strength"), R"pbdoc(Replay encoded experiences)pbdoc")
        .def("getEpisode", &EpisodicMemory::getEpisode, py::arg("timestamp"), py::return_value_policy::reference_internal, R"pbdoc(Get specific episode)pbdoc")
        .def("getAllEpisodes", &EpisodicMemory::getAllEpisodes, py::return_value_policy::reference_internal, R"pbdoc(Get all episodes)pbdoc")
        .def("removeEpisode", &EpisodicMemory::removeEpisode, py::arg("timestamp"), R"pbdoc(Remove episode)pbdoc")
        .def("clear", &EpisodicMemory::clear, R"pbdoc(Clear all episodes)pbdoc")
        .def("getSize", &EpisodicMemory::getSize, R"pbdoc(Get number of episodes)pbdoc")
        .def("getReplayCount", &EpisodicMemory::getReplayCount, R"pbdoc(Get replay statistics)pbdoc")
        .def("update", &EpisodicMemory::update, py::arg("dt"), R"pbdoc(Update episodic memory)pbdoc")
        .def("consolidate", &EpisodicMemory::consolidate, py::arg("importanceThreshold"), R"pbdoc(Consolidate important episodes)pbdoc")
        .def("__repr__", [](const EpisodicMemory& em) {
            return "<EpisodicMemory episodes=" + std::to_string(em.getSize()) + ">";
        });

    // AssociativeMemory class - pattern association storage
    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory for pattern associations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("pattern1"), py::arg("pattern2"), py::arg("strength"), R"pbdoc(Create pattern association)pbdoc")
        .def("recall", &AssociativeMemory::recall, py::arg("pattern"), py::arg("similarityThreshold"), py::return_value_policy::reference_internal, R"pbdoc(Recall associated patterns)pbdoc")
        .def("getStrength", &AssociativeMemory::getStrength, py::arg("pattern1"), py::arg("pattern2"), R"pbdoc(Get association strength)pbdoc")
        .def("removeAssociation", &AssociativeMemory::removeAssociation, py::arg("pattern1"), py::arg("pattern2"), R"pbdoc(Remove association)pbdoc")
        .def("clear", &AssociativeMemory::clear, R"pbdoc(Clear all associations)pbdoc")
        .def("getSize", &AssociativeMemory::getSize, R"pbdoc(Get number of associations)pbdoc")
        .def("update", &AssociativeMemory::update, py::arg("dt"), R"pbdoc(Update associative memory)pbdoc")
        .def("__repr__", [](const AssociativeMemory& am) {
            return "<AssociativeMemory associations=" + std::to_string(am.getSize()) + ">";
        });

    // ============= NEUROMODULATION SYSTEMS =============

    // Dopamine class - reward prediction error signal
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward processing)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel, R"pbdoc(Get current dopamine level)pbdoc")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"), R"pbdoc(Set dopamine level)pbdoc")
        .def("update", &Dopamine::update, py::arg("predictionError"), py::arg("reward"), py::arg("dt"), R"pbdoc(Update based on prediction error)pbdoc")
        .def("getLearningRate", &Dopamine::getLearningRate, R"pbdoc(Get learning rate)pbdoc")
        .def("setLearningRate", &Dopamine::setLearningRate, py::arg("rate"), R"pbdoc(Set learning rate)pbdoc")
        .def("reset", &Dopamine::reset, R"pbdoc(Reset to baseline)pbdoc")
        .def("__repr__", [](const Dopamine& d) {
            return "<Dopamine level=" + std::to_string(d.getLevel()) + ">";
        });

    // Curiosity class - exploration motivation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"), R"pbdoc(Set curiosity level)pbdoc")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"), R"pbdoc(Update based on novelty and error)pbdoc")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, R"pbdoc(Get exploration drive strength)pbdoc")
        .def("setExplorationDrive", &Curiosity::setExplorationDrive, py::arg("drive"), R"pbdoc(Set exploration drive)pbdoc")
        .def("reset", &Curiosity::reset, R"pbdoc(Reset to baseline)pbdoc")
        .def("__repr__", [](const Curiosity& c) {
            return "<Curiosity level=" + std::to_string(c.getLevel()) + ">";
        });

    // Novelty class - novelty detection
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection for change detection)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), R"pbdoc(Set novelty level)pbdoc")
        .def("update", &Novelty::update, py::arg("inputPattern"), py::arg("memoryPattern"), py::arg("dt"), R"pbdoc(Update based on pattern comparison)pbdoc")
        .def("getDetectionThreshold", &Novelty::getDetectionThreshold, R"pbdoc(Get detection threshold)pbdoc")
        .def("setDetectionThreshold", &Novelty::setDetectionThreshold, py::arg("threshold"), R"pbdoc(Set detection threshold)pbdoc")
        .def("reset", &Novelty::reset, R"pbdoc(Reset to baseline)pbdoc")
        .def("__repr__", [](const Novelty& n) {
            return "<Novelty level=" + std::to_string(n.getLevel()) + ">";
        });

    // PredictionError class - prediction error signal
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel, R"pbdoc(Get prediction error level)pbdoc")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"), R"pbdoc(Set prediction error level)pbdoc")
        .def("update", &PredictionError::update, py::arg("actual"), py::arg("predicted"), py::arg("dt"), R"pbdoc(Update based on actual and predicted values)pbdoc")
        .def("getErrorMagnitude", &PredictionError::getErrorMagnitude, R"pbdoc(Get error magnitude)pbdoc")
        .def("setErrorMagnitude", &PredictionError::setErrorMagnitude, py::arg("magnitude"), R"pbdoc(Set error magnitude)pbdoc")
        .def("reset", &PredictionError::reset, R"pbdoc(Reset to baseline)pbdoc")
        .def("__repr__", [](const PredictionError& pe) {
            return "<PredictionError level=" + std::to_string(pe.getLevel()) + ">";
        });

    // Neuromodulator class - base class for all neuromodulators
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Base class for neuromodulatory signals)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Neuromodulator::getLevel, R"pbdoc(Get neuromodulator level)pbdoc")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), R"pbdoc(Set neuromodulator level)pbdoc")
        .def("update", &Neuromodulator::update, py::arg("dt"), R"pbdoc(Update neuromodulator)pbdoc")
        .def("getInfluence", &Neuromodulator::getInfluence, R"pbdoc(Get neuromodulator influence)pbdoc")
        .def("reset", &Neuromodulator::reset, R"pbdoc(Reset to baseline)pbdoc")
        .def("__repr__", [](const Neuromodulator& n) {
            return "<Neuromodulator level=" + std::to_string(n.getLevel()) + ">";
        });

    // ============= PLASTICITY SYSTEMS =============

    // PlasticityRule class - base class for plasticity mechanisms
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Base class for synaptic plasticity rules)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &PlasticityRule::setLTPWeight, py::arg("weight"), R"pbdoc(Set long-term potentiation weight)pbdoc")
        .def("setLTDWeight", &PlasticityRule::setLTDWeight, py::arg("weight"), R"pbdoc(Set long-term depression weight)pbdoc")
        .def("setTau", &PlasticityRule::setTau, py::arg("tau"), R"pbdoc(Set time constant)pbdoc")
        .def("getLTPWeight", &PlasticityRule::getLTPWeight, R"pbdoc(Get LTP weight)pbdoc")
        .def("getLTDWeight", &PlasticityRule::getLTDWeight, R"pbdoc(Get LTD weight)pbdoc")
        .def("getTau", &PlasticityRule::getTau, R"pbdoc(Get time constant)pbdoc")
        .def("apply", &PlasticityRule::apply, py::arg("preSynapticTrace"), py::arg("postSynapticTrace"), py::arg("weight"), R"pbdoc(Apply plasticity rule)pbdoc")
        .def("decayTraces", &PlasticityRule::decayTraces, py::arg("dt"), R"pbdoc(Decay eligibility traces)pbdoc")
        .def("reset", &PlasticityRule::reset, R"pbdoc(Reset to initial state)pbdoc")
        .def("__repr__", [](const PlasticityRule& pr) {
            return "<PlasticityRule LTP=" + std::to_string(pr.getLTPWeight()) + " LTD=" + std::to_string(pr.getLTDWeight()) + ">";
        });

    // STDP class - spike-timing dependent plasticity
    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Spike-timing dependent plasticity)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &STDP::setLearningRate, py::arg("rate"), R"pbdoc(Set learning rate)pbdoc")
        .def("getLearningRate", &STDP::getLearningRate, R"pbdoc(Get learning rate)pbdoc")
        .def("setTauPlus", &STDP::setTauPlus, py::arg("tau"), R"pbdoc(Set LTP time constant)pbdoc")
        .def("setTauMinus", &STDP::setTauMinus, py::arg("tau"), R"pbdoc(Set LTD time constant)pbdoc")
        .def("getTauPlus", &STDP::getTauPlus, R"pbdoc(Get LTP time constant)pbdoc")
        .def("getTauMinus", &STDP::getTauMinus, R"pbdoc(Get LTD time constant)pbdoc")
        .def("apply", &STDP::apply, py::arg("preSpikeTime"), py::arg("postSpikeTime"), py::arg("weight"), R"pbdoc(Apply STDP rule based on spike times)pbdoc")
        .def("reset", &STDP::reset, R"pbdoc(Reset to initial state)pbdoc")
        .def("__repr__", [](const STDP& stdp) {
            return "<STDP rate=" + std::to_string(stdp.getLearningRate()) + 
                   " tau+=" + std::to_string(stdp.getTauPlus()) + 
                   " tau-=" + std::to_string(stdp.getTauMinus()) + ">";
        });

    // Hebbian class - Hebbian learning
    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), R"pbdoc(Set learning rate)pbdoc")
        .def("getLearningRate", &Hebbian::getLearningRate, R"pbdoc(Get learning rate)pbdoc")
        .def("setThreshold", &Hebbian::setThreshold, py::arg("threshold"), R"pbdoc(Set activation threshold)pbdoc")
        .def("getThreshold", &Hebbian::getThreshold, R"pbdoc(Get activation threshold)pbdoc")
        .def("apply", &Hebbian::apply, py::arg("preSynapticActivity"), py::arg("postSynapticActivity"), py::arg("weight"), R"pbdoc(Apply Hebbian rule)pbdoc")
        .def("reset", &Hebbian::reset, R"pbdoc(Reset to initial state)pbdoc")
        .def("__repr__", [](const Hebbian& hebbian) {
            return "<Hebbian rate=" + std::to_string(hebbian.getLearningRate()) + 
                   " threshold=" + std::to_string(hebbian.getThreshold()) + ">";
        });

    // StructuralPlasticity class - structural changes (synaptogenesis/pruning)
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for synaptogenesis and pruning)pbdoc")
        .def(py::init<>())
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"), R"pbdoc(Set synaptogenesis rate)pbdoc")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"), R"pbdoc(Set pruning rate)pbdoc")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, R"pbdoc(Get synaptogenesis rate)pbdoc")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate, R"pbdoc(Get pruning rate)pbdoc")
        .def("addSynapse", &StructuralPlasticity::addSynapse, py::arg("synapse"), R"pbdoc(Add new synapse)pbdoc")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse, py::arg("synapse"), R"pbdoc(Remove synapse)pbdoc")
        .def("update", &StructuralPlasticity::update, py::arg("dt"), py::arg("randomGenerator"), R"pbdoc(Update structural connectivity)pbdoc")
        .def("getSynapseCount", &StructuralPlasticity::getSynapseCount, R"pbdoc(Get number of synapses)pbdoc")
        .def("getDensity", &StructuralPlasticity::getDensity, py::arg("regionSize"), R"pbdoc(Get connectivity density)pbdoc")
        .def("reset", &StructuralPlasticity::reset, R"pbdoc(Reset to initial state)pbdoc")
        .def("__repr__", [](const StructuralPlasticity& sp) {
            return "<StructuralPlasticity synapses=" + std::to_string(sp.getSynapseCount()) +
                   " synRate=" + std::to_string(sp.getSynaptogenesisRate()) +
                   " pruneRate=" + std::to_string(sp.getPruningRate()) + ">";
        });

    // ============= DEVELOPMENT SYSTEM =============

    // DevelopmentSystem class - developmental processes
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system for age-dependent changes)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"), R"pbdoc(Set developmental stage)pbdoc")
        .def("getAge", &DevelopmentSystem::getAge, R"pbdoc(Get current age)pbdoc")
        .def("setAge", &DevelopmentSystem::setAge, py::arg("age"), R"pbdoc(Set age)pbdoc")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, R"pbdoc(Get plasticity modifier)pbdoc")
        .def("setPlasticityModifier", &DevelopmentSystem::setPlasticityModifier, py::arg("modifier"), R"pbdoc(Set plasticity modifier)pbdoc")
        .def("update", &DevelopmentSystem::update, py::arg("dt"), R"pbdoc(Update developmental state)pbdoc")
        .def("getCriticalPeriodEnd", &DevelopmentSystem::getCriticalPeriodEnd, R"pbdoc(Get critical period end)pbdoc")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod, R"pbdoc(Check if in critical period)pbdoc")
        .def("reset", &DevelopmentSystem::reset, R"pbdoc(Reset to initial state)pbdoc")
        .def("__repr__", [](const DevelopmentSystem& ds) {
            return "<DevelopmentSystem stage=" + std::to_string(static_cast<int>(ds.getStage())) +
                   " age=" + std::to_string(ds.getAge()) +
                   " plasticity=" + std::to_string(ds.getPlasticityModifier()) + ">";
        });

    // ============= NEURAL DYNAMICS =============

    // SpikeSystem class - event-driven spike processing
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Event-driven spike processing and propagation)pbdoc")
        .def(py::init<>())
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("neurons"), R"pbdoc(Process neural spikes)pbdoc")
        .def("getSpikeCount", &SpikeSystem::getSpikeCount, R"pbdoc(Get total spike count)pbdoc")
        .def("getFiringNeurons", &SpikeSystem::getFiringNeurons, py::return_value_policy::reference_internal, R"pbdoc(Get neurons that fired)pbdoc")
        .def("reset", &SpikeSystem::reset, R"pbdoc(Reset spike counters)pbdoc")
        .def("setMaxDelay", &SpikeSystem::setMaxDelay, py::arg("maxDelay"), R"pbdoc(Set maximum synaptic delay)pbdoc")
        .def("getMaxDelay", &SpikeSystem::getMaxDelay, R"pbdoc(Get maximum synaptic delay)pbdoc")
        .def("set refractoryPeriod", &SpikeSystem::setRefractoryPeriod, py::arg("period"), R"pbdoc(Set refractory period)pbdoc")
        .def("getRefractoryPeriod", &SpikeSystem::getRefractoryPeriod, R"pbdoc(Get refractory period)pbdoc")
        .def("__repr__", [](const SpikeSystem& ss) {
            return "<SpikeSystem spikes=" + std::to_string(ss.getSpikeCount()) + ">";
        });

    // NeuralDynamics class - continuous neural dynamics
    py::class_<NeuralDynamics>(m, "NeuralDynamics", R"pbdoc(Neural dynamics and continuous modeling)pbdoc")
        .def(py::init<>())
        .def("integrate", &NeuralDynamics::integrate, py::arg("neurons"), py::arg("dt"), R"pbdoc(Integrate neural dynamics)pbdoc")
        .def("getTotalEnergy", &NeuralDynamics::getTotalEnergy, R"pbdoc(Get total neural energy)pbdoc")
        .def("getAverageVoltage", &NeuralDynamics::getAverageVoltage, R"pbdoc(Get average membrane potential)pbdoc")
        .def("setCouplingStrength", &NeuralDynamics::setCouplingStrength, py::arg("strength"), R"pbdoc(Set coupling strength)pbdoc")
        .def("getCouplingStrength", &NeuralDynamics::getCouplingStrength, R"pbdoc(Get coupling strength)pbdoc")
        .def("reset", &NeuralDynamics::reset, R"pbdoc(Reset dynamics state)pbdoc")
        .def("__repr__", [](const NeuralDynamics& nd) {
            return "<NeuralDynamics energy=" + std::to_string(nd.getTotalEnergy()) + ">";
        });

    // ============= PREDICTION SYSTEM =============

    // PredictionSystem class - forward models and prediction
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Forward models and prediction)pbdoc")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predict, py::arg("input"), py::arg("dt"), R"pbdoc(Generate prediction)pbdoc")
        .def("update", &PredictionSystem::update, py::arg("input"), py::arg("actual"), py::arg("dt"), R"pbdoc(Update prediction)pbdoc")
        .def("getPredictionError", &PredictionSystem::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        .def("getPrediction", &PredictionSystem::getPrediction, py::return_value_policy::reference_internal, R"pbdoc(Get prediction)pbdoc")
        .def("getConfidence", &PredictionSystem::getConfidence, R"pbdoc(Get prediction confidence)pbdoc")
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"), R"pbdoc(Set learning rate)pbdoc")
        .def("getLearningRate", &PredictionSystem::getLearningRate, R"pbdoc(Get learning rate)pbdoc")
        .def("reset", &PredictionSystem::reset, R"pbdoc(Reset to initial state)pbdoc")
        .def("__repr__", [](const PredictionSystem& ps) {
            return "<PredictionSystem error=" + std::to_string(ps.getPredictionError()) + 
                   " confidence=" + std::to_string(ps.getConfidence()) + ">";
        });

    // ============= COGNITION SYSTEMS =============

    // NeuralPlanner class - action planning
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural action planning and decision making)pbdoc")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction, py::arg("context"), py::arg("dt"), R"pbdoc(Plan action based on context)pbdoc")
        .def("updateGoal", &NeuralPlanner::updateGoal, py::arg("goal"), R"pbdoc(Update planning goal)pbdoc")
        .def("getActionPlan", &NeuralPlanner::getActionPlan, py::return_value_policy::reference_internal, R"pbdoc(Get current action plan)pbdoc")
        .def("getConfidence", &NeuralPlanner::getConfidence, py::arg("action"), R"pbdoc(Get confidence in action)pbdoc")
        .def("setMaxPlanSteps", &NeuralPlanner::setMaxPlanSteps, py::arg("steps"), R"pbdoc(Set maximum planning horizon)pbdoc")
        .def("getMaxPlanSteps", &NeuralPlanner::getMaxPlanSteps, R"pbdoc(Get maximum planning horizon)pbdoc")
        .def("reset", &NeuralPlanner::reset, R"pbdoc(Reset planner)pbdoc")
        .def("__repr__", [](const NeuralPlanner& np) {
            return "<NeuralPlanner goal=" + np.getActionPlan().getName() + ">";
        });

    // ConceptFormation class - pattern discovery
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation from experience patterns)pbdoc")
        .def(py::init<>())
        .def("learnConcept", &ConceptFormation::learnConcept, py::arg("pattern"), py::arg("concept"), py::arg("strength"), R"pbdoc(Learn new concept)pbdoc")
        .def("activateConcept", &ConceptFormation::activateConcept, py::arg("concept"), py::arg("input"), py::return_value_policy::reference_internal, R"pbdoc(Activate concept based on input)pbdoc")
        .def("getConceptStrength", &ConceptFormation::getConceptStrength, py::arg("concept"), R"pbdoc(Get concept strength)pbdoc")
        .def("removeConcept", &ConceptFormation::removeConcept, py::arg("concept"), R"pbdoc(Remove concept)pbdoc")
        .def("getConcepts", &ConceptFormation::getConcepts, py::return_value_policy::reference_internal, R"pbdoc(Get all concepts)pbdoc")
        .def("update", &ConceptFormation::update, py::arg("dt"), R"pbdoc(Update concept associations)pbdoc")
        .def("reset", &ConceptFormation::reset, R"pbdoc(Reset concept formation)pbdoc")
        .def("__repr__", [](const ConceptFormation& cf) {
            return "<ConceptFormation concepts=" + std::to_string(cf.getConcepts().size()) + ">";
        });

    // AttentionalSelection class - competitive attention
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attentional selection and competitive processing)pbdoc")
        .def(py::init<>())
        .def("select", &AttentionalSelection::select, py::arg("inputs"), py::arg("dt"), R"pbdoc(Select focus based on inputs)pbdoc")
        .def("updateWeights", &AttentionalSelection::updateWeights, py::arg("selected"), py::arg("dt"), R"pbdoc(Update attention weights)pbdoc")
        .def("getFocus", &AttentionalSelection::getFocus, py::return_value_policy::reference_internal, R"pbdoc(Get current focus)pbdoc")
        .def("getAttentionStrengths", &AttentionalSelection::getAttentionStrengths, py::return_value_policy::reference_internal, R"pbdoc(Get attention strengths)pbdoc")
        .def("setSaliencyThreshold", &AttentionalSelection::setSaliencyThreshold, py::arg("threshold"), R"pbdoc(Set saliency threshold)pbdoc")
        .def("getSaliencyThreshold", &AttentionalSelection::getSaliencyThreshold, R"pbdoc(Get saliency threshold)pbdoc")
        .def("reset", &AttentionalSelection::reset, R"pbdoc(Reset attentional system)pbdoc")
        .def("__repr__", [](const AttentionalSelection& as_) {
            return "<AttentionalSelection focus=" + as_.getFocus().getName() + ">";
        });

    // ============= PERFORMANCE AND OPTIMIZATION =============

    // MemoryPool class - efficient memory management
    py::class_<MemoryPool>(m, "MemoryPool", R"pbdoc(Efficient memory management for neural simulation)pbdoc")
        .def(py::init<>())
        .def("allocateNeuron", &MemoryPool::allocateNeuron, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Allocate neuron memory)pbdoc")
        .def("allocateSynapse", &MemoryPool::allocateSynapse, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Allocate synapse memory)pbdoc")
        .def("getNeuron", &MemoryPool::getNeuron, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get neuron from pool)pbdoc")
        .def("getSynapse", &MemoryPool::getSynapse, py::arg("id"), py::return_value_policy::reference_internal, R"pbdoc(Get synapse from pool)pbdoc")
        .def("releaseNeuron", &MemoryPool::releaseNeuron, py::arg("neuron"), R"pbdoc(Release neuron memory)pbdoc")
        .def("releaseSynapse", &MemoryPool::releaseSynapse, py::arg("synapse"), R"pbdoc(Release synapse memory)pbdoc")
        .def("getPoolSize", &MemoryPool::getPoolSize, R"pbdoc(Get pool size)pbdoc")
        .def("getFreeCount", &MemoryPool::getFreeCount, R"pbdoc(Get number of free slots)pbdoc")
        .def("clear", &MemoryPool::clear, R"pbdoc(Clear all allocated memory)pbdoc")
        .def("optimize", &MemoryPool::optimize, R"pbdoc(Optimize memory layout)pbdoc")
        .def("__repr__", [](const MemoryPool& mp) {
            return "<MemoryPool size=" + std::to_string(mp.getPoolSize()) + 
                   " free=" + std::to_string(mp.getFreeCount()) + ">";
        });

    // EventQueue class - event-driven simulation
    py::class_<EventQueue>(m, "EventQueue", R"pbdoc(Event-driven simulation queue)pbdoc")
        .def(py::init<>())
        .def("pushEvent", &EventQueue::pushEvent, py::arg("event"), R"pbdoc(Push event to queue)pbdoc")
        .def("popEvent", &EventQueue::popEvent, R"pbdoc(Pop event from queue)pbdoc")
        .def("getSize", &EventQueue::getSize, R"pbdoc(Get queue size)pbdoc")
        .def("isEmpty", &EventQueue::isEmpty, R"pbdoc(Check if queue is empty)pbdoc")
        .def("clear", &EventQueue::clear, R"pbdoc(Clear queue)pbdoc")
        .def("update", &EventQueue::update, py::arg("dt"), R"pbdoc(Process events)pbdoc")
        .def("setMaxEvents", &EventQueue::setMaxEvents, py::arg("maxEvents"), R"pbdoc(Set maximum events)pbdoc")
        .def("getMaxEvents", &EventQueue::getMaxEvents, R"pbdoc(Get maximum events)pbdoc")
        .def("__repr__", [](const EventQueue& eq) {
            return "<EventQueue size=" + std::to_string(eq.getSize()) + ">";
        });

    // CheckpointSystem class - persistence and save/load
    py::class_<CheckpointSystem>(m, "CheckpointSystem", R"pbdoc(Checkpoint system for persistence)pbdoc")
        .def(py::init<>())
        .def("save", &CheckpointSystem::save, py::arg("filepath"), R"pbdoc(Save checkpoint)pbdoc")
        .def("load", &CheckpointSystem::load, py::arg("filepath"), R"pbdoc(Load checkpoint)pbdoc")
        .def("hasCheckpoint", &CheckpointSystem::hasCheckpoint, py::arg("filepath"), R"pbdoc(Check if checkpoint exists)pbdoc")
        .def("deleteCheckpoint", &CheckpointSystem::deleteCheckpoint, py::arg("filepath"), R"pbdoc(Delete checkpoint)pbdoc")
        .def("getCheckpointInfo", &CheckpointSystem::getCheckpointInfo, py::arg("filepath"), py::return_value_policy::reference_internal, R"pbdoc(Get checkpoint info)pbdoc")
        .def("listCheckpoints", &CheckpointSystem::listCheckpoints, py::arg("directory"), py::return_value_policy::reference_internal, R"pbdoc(List checkpoints in directory)pbdoc")
        .def("setCompression", &CheckpointSystem::setCompression, py::arg("enabled"), R"pbdoc(Enable/disable compression)pbdoc")
        .def("isCompressionEnabled", &CheckpointSystem::isCompressionEnabled, R"pbdoc(Check if compression enabled)pbdoc")
        .def("setAutoSave", &CheckpointSystem::setAutoSave, py::arg("interval"), R"pbdoc(Set auto-save interval)pbdoc")
        .def("getAutoSaveInterval", &CheckpointSystem::getAutoSaveInterval, R"pbdoc(Get auto-save interval)pbdoc")
        .def("__repr__", [](const CheckpointSystem& cs) {
            return "<CheckpointSystem compression=" + std::to_string(cs.isCompressionEnabled()) + ">";
        });

    // ============= ENVIRONMENT AND WORLD SYSTEMS =============

    // Environment class - general environment interface
    py::class_<Environment>(m, "Environment", R"pbdoc(Environment interface for world simulation)pbdoc")
        .def(py::init<>())
        .def("reset", &Environment::reset, R"pbdoc(Reset environment)pbdoc")
        .def("update", &Environment::update, py::arg("dt"), R"pbdoc(Update environment)pbdoc")
        .def("getTime", &Environment::getTime, R"pbdoc(Get current time)pbdoc")
        .def("getWidth", &Environment::getWidth, R"pbdoc(Get environment width)pbdoc")
        .def("getHeight", &Environment::getHeight, R"pbdoc(Get environment height)pbdoc")
        .def("setRandomSeed", &Environment::setRandomSeed, py::arg("seed"), R"pbdoc(Set random seed)pbdoc")
        .def("getRandomSeed", &Environment::getRandomSeed, R"pbdoc(Get random seed)pbdoc")
        .def("addObject", &Environment::addObject, py::arg("obj"), R"pbdoc(Add object to environment)pbdoc")
        .def("removeObject", &Environment::removeObject, py::arg("x"), py::arg("y"), R"pbdoc(Remove object from environment)pbdoc")
        .def("getObject", &Environment::getObject, py::arg("x"), py::arg("y"), py::return_value_policy::reference_internal, R"pbdoc(Get object at position)pbdoc")
        .def("isValidPosition", &Environment::isValidPosition, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid)pbdoc")
        .def("getMaxEnergy", &Environment::getMaxEnergy, R"pbdoc(Get maximum energy)pbdoc")
        .def("setMaxEnergy", &Environment::setMaxEnergy, py::arg("energy"), R"pbdoc(Set maximum energy)pbdoc")
        .def("getEnergyDecayRate", &Environment::getEnergyDecayRate, R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &Environment::setEnergyDecayRate, py::arg("rate"), R"pbdoc(Set energy decay rate)pbdoc")
        .def("__repr__", [](const Environment& env) {
            return "<Environment time=" + std::to_string(env.getTime()) + 
                   " size=" + std::to_string(env.getWidth()) + "x" + std::to_string(env.getHeight()) + ">";
        });

    // Observation class - sensory observation data
    py::class_<Observation>(m, "Observation", R"pbdoc(Sensory observation data)pbdoc")
        .def(py::init<>())
        .def("getType", &Observation::getType, R"pbdoc(Get observation type)pbdoc")
        .def("getData", &Observation::getData, py::return_value_policy::reference_internal, R"pbdoc(Get observation data)pbdoc")
        .def("getDimensions", &Observation::getDimensions, py::return_value_policy::reference_internal, R"pbdoc(Get observation dimensions)pbdoc")
        .def("getTimestamp", &Observation::getTimestamp, R"pbdoc(Get observation timestamp)pbdoc")
        .def("setTimestamp", &Observation::setTimestamp, py::arg("timestamp"), R"pbdoc(Set observation timestamp)pbdoc")
        .def("getAllSignals", &Observation::getAllSignals, py::return_value_policy::reference_internal, R"pbdoc(Get all signals)pbdoc")
        .def("__repr__", [](const Observation& obs) {
            return "<Observation type=" + std::to_string(static_cast<int>(obs.getType())) +
                   " dims=" + obs.getDimensions().size() + ">";
        });

    // ============= AGENT SYSTEMS =============

    // AgentBrain class - neural interface for agents
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), R"pbdoc(Initialize with world)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, R"pbdoc(Get expected sensory input size)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, R"pbdoc(Get expected motor output size)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), R"pbdoc(Process sensory percept and inject into brain)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, R"pbdoc(Decode brain motor activity into motor command)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), R"pbdoc(Apply reward-based neuromodulation)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), R"pbdoc(Update development system)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, R"pbdoc(Get current neuromodulation level)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent for new episode)pbdoc")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, R"pbdoc(Get the underlying brain)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), R"pbdoc(Enable reward modulation)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), R"pbdoc(Enable structural plasticity)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), R"pbdoc(Enable development)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), R"pbdoc(Enable curiosity)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, R"pbdoc(Check if reward modulation enabled)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, R"pbdoc(Check if structural plasticity enabled)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, R"pbdoc(Check if development enabled)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, R"pbdoc(Check if curiosity enabled)pbdoc")

    // ============= SIMULATION AND UTILITY FUNCTIONS =============

    // Factory functions for common objects
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration with sensible defaults)"");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)"");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world)pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)"";

    m.def("createVision", [](size_t width, size_t height, size_t channels) -> std::shared_ptr<Vision> {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, R"pbdoc(Create a vision sensory input)"";

    m.def("createAudio", [](size_t sampleRate, size_t numSamples) -> std::shared_ptr<Audio> {
        return std::make_shared<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"), R"pbdoc(Create an audio sensory input)"";

    m.def("createInternalSignals", []() -> std::shared_ptr<InternalSignals> {
        return std::make_shared<InternalSignals>();
    }, R"pbdoc(Create internal signals sensory input)"";

    m.def("createAction", [](ActionType type) -> std::unique_ptr<Action> {
        return std::make_unique<Action>(type);
    }, py::arg("type"), R"pbdoc(Create an action from type)"";

    m.def("createActionWithParams", [](ActionType type, std::vector<float> params) -> std::unique_ptr<Action> {
        return std::make_unique<Action>(type, params);
    }, py::arg("type"), py::arg("params"), R"pbdoc(Create an action with parameters)"";

    m.def("createWorldObject", [](float x, float y, WorldObjectType type, float value, float radius) -> std::unique_ptr<WorldObject> {
        return std::make_unique<WorldObject>(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f, R"pbdoc(Create a world object)"";

    m.def("createAgentBody", []() -> std::unique_ptr<AgentBody> {
        return std::make_unique<AgentBody>();
    }, R"pbdoc(Create an agent body)"";

    m.def("createSensoryPercept", []() -> std::unique_ptr<SensoryPercept> {
        return std::make_unique<SensoryPercept>();
    }, R"pbdoc(Create a sensory percept)"";

    // Constant values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic helper functions
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                          std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       R"pbdoc(Run a complete simulation episode)"");

    m.def("saveBrain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Save brain state to file)"");

    m.def("loadBrain", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->load(filepath);
        return brain;
    }, py::arg("filepath"), R"pbdoc(Load brain state from file)"");

    m.def("visualizeBrain", [](std::shared_ptr<Brain> brain, const std::string& outputFile) {
        auto viz = std::make_shared<VisualizationInterface>();
        viz->generateVisualization(brain, outputFile);
    }, py::arg("brain"), py::arg("outputFile"), R"pbdoc(Generate visualization of brain state)"");

    // Add version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__description__") = "NLM (熙然) - Neural Learning Machine Python bindings";
    m.attr("__author__") = "NLM Authors";
    
    // Python utility functions
    m.def("get_neuron_count", [](const std::shared_ptr<Brain>& brain) {
        return brain->getTotalNeuronCount();
    }, py::arg("brain"), R"pbdoc(Get total neuron count)"");

    m.def("get_synapse_count", [](const std::shared_ptr<Brain>& brain) {
        return brain->getTotalSynapseCount();
    }, py::arg("brain"), R"pbdoc(Get total synapse count)"");

    m.def("get_firing_rate", [](const std::shared_ptr<Brain>& brain) {
        return brain->getAverageFiringRate();
    }, py::arg("brain"), R"pbdoc(Get average firing rate)"");

    m.def("get_developmental_stage", [](const std::shared_ptr<Brain>& brain) {
        return brain->getDevelopmentalStage();
    }, py::arg("brain"), R"pbdoc(Get developmental stage)"");

    m.def("set_developmental_stage", [](std::shared_ptr<Brain>& brain, DevelopmentalStage stage) {
        brain->setDevelopmentalStage(stage);
    }, py::arg("brain"), py::arg("stage"), R"pbdoc(Set developmental stage)"");

    // Error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");
    py::register_exception<std::logic_error>(m, "LogicError");
}

} // namespace nlm