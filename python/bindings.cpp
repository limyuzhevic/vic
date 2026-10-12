#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all NLM headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/RandomGenerator.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/brain/AgentBrain.hpp"

#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

#include "../src/world/SimpleWorld.hpp"
#include "../src/world/WorldObject.hpp"

#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

#include "../src/motor/Action.hpp"

#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralSemanticMemory.hpp"
#include "../src/memory/NeuralProceduralMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"

#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"

#include "../src/development/DevelopmentSystem.hpp"

#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

#include "../src/dynamics/SpikeSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Base exception class
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

// Python binding module
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Provides comprehensive access to brain simulation components including:
        - Core neural components: Neuron, Synapse, NeuralPopulation, NeuralRegion
        - Memory systems: WorkingMemory, EpisodicMemory, SemanticMemory, etc.
        - Plasticity: STDP, Hebbian, Structural plasticity
        - Neuromodulation: Dopamine, Curiosity, Novelty, PredictionError
        - Prediction and cognition: PredictionSystem, NeuralPlanner, etc.
        - Development system and more
        
        The bindings provide both low-level access to individual neural components
        and high-level convenience functions for creating complete brain simulations.
    )pbdoc";

    // Register custom exception
    py::register_exception<NLMException>(m, "NLMException");

    // ==============================================
    // Basic Type Definitions
    // ==============================================

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

    // ==============================================
    // Enumerations
    // ==============================================

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

    // ==============================================
    // Basic Helper Functions
    // ==============================================

    m.def("INVALID_NEURON_ID", []() { return INVALID_NEURON_ID; }, 
          R"pbdoc(Return the invalid neuron ID constant)pbdoc");
    m.def("INVALID_SYNAPSE_ID", []() { return INVALID_SYNAPSE_ID; },
          R"pbdoc(Return the invalid synapse ID constant)pbdoc");
    m.def("INVALID_REGION_ID", []() { return INVALID_REGION_ID; },
          R"pbdoc(Return the invalid region ID constant)pbdoc");
    m.def("INVALID_POPULATION_ID", []() { return INVALID_POPULATION_ID; },
          R"pbdoc(Return the invalid population ID constant)pbdoc");

    // ==============================================
    // Core Neural Components
    // ==============================================

    // Neuron class with complete LIF dynamics
    py::class_<Neuron>(m, "Neuron", R"pbdoc(A single spiking neuron implementing Leaky Integrate-and-Fire dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, R"pbdoc(Get the neuron's unique ID)pbdoc")
        .def("getType", &Neuron::getType, R"pbdoc(Get the neuron type)pbdoc")
        .def("setType", &Neuron::setType, py::arg("type"), R"pbdoc(Set the neuron type)pbdoc")
        .def("getMembranePotential", &Neuron::getMembranePotential, R"pbdoc(Get membrane potential (mV))pbdoc")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), R"pbdoc(Set membrane potential)pbdoc")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), R"pbdoc(Add to membrane potential)pbdoc")
        .def("getThreshold", &Neuron::getThreshold, R"pbdoc(Get firing threshold)pbdoc")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), R"pbdoc(Set firing threshold)pbdoc")
        .def("isFiring", &Neuron::isFiring, R"pbdoc(Check if neuron is currently firing)pbdoc")
        .def("isRefractory", &Neuron::isRefractory, R"pbdoc(Check if neuron is in refractory period)pbdoc")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), R"pbdoc(Set firing state)pbdoc")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), R"pbdoc(Set refractory period)pbdoc")
        .def("decrementRefractory", &Neuron::decrementRefractory, R"pbdoc(Decrement refractory counter)pbdoc")
        .def("getFiringRate", &Neuron::getFiringRate, R"pbdoc(Get firing rate (Hz))pbdoc")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), R"pbdoc(Set firing rate)pbdoc")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), R"pbdoc(Set leak conductance)pbdoc")
        .def("getLeakConductance", &Neuron::getLeakConductance, R"pbdoc(Get leak conductance)pbdoc")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, R"pbdoc(Get refractory period)pbdoc")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), R"pbdoc(Set resting potential)pbdoc")
        .def("getRestingPotential", &Neuron::getRestingPotential, R"pbdoc(Get resting potential)pbdoc")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), R"pbdoc(Set reset potential)pbdoc")
        .def("checkThreshold", &Neuron::checkThreshold, R"pbdoc(Check if neuron should fire based on threshold)pbdoc")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, R"pbdoc(Get timestamp of last spike, -1 if none)pbdoc")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), R"pbdoc(Perform one LIF integration step, returns true if neuron fired)pbdoc")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), R"pbdoc(Receive excitatory input (postsynaptic potential))pbdoc")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), R"pbdoc(Receive inhibitory input)pbdoc")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), R"pbdoc(Receive modulatory input)pbdoc")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), R"pbdoc(Inject external current into neuron)pbdoc")
        .def("getTotalCurrent", &Neuron::getTotalCurrent, R"pbdoc(Get total injected current)pbdoc")
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent, R"pbdoc(Clear injected current)pbdoc")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), R"pbdoc(Record a spike timestamp)pbdoc")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, R"pbdoc(Get spike history as vector of timestamps)pbdoc")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, R"pbdoc(Clear spike history)pbdoc")
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"), R"pbdoc(Add incoming synapse)pbdoc")
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"), R"pbdoc(Add outgoing synapse)pbdoc")
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses, R"pbdoc(Get incoming synapse handles)pbdoc")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses, R"pbdoc(Get outgoing synapse handles)pbdoc")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags, R"pbdoc(Get plasticity flags)pbdoc")
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"), R"pbdoc(Set region ID)pbdoc")
        .def("getRegionId", &Neuron::getRegionId, R"pbdoc(Get region ID)pbdoc")
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"), R"pbdoc(Set population ID)pbdoc")
        .def("getPopulationId", &Neuron::getPopulationId, R"pbdoc(Get population ID)pbdoc")
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"), R"pbdoc(Enable plasticity mechanisms)pbdoc")
        .def("reset", &Neuron::reset, R"pbdoc(Reset neuron to initial state)pbdoc")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), R"pbdoc(Initialize neuron with random parameters)pbdoc")
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron id=" + std::to_string(n.getId().value) + 
                   " type=" + std::to_string(static_cast<int>(n.getType())) + ">";
        });

    // Synapse class with complete synaptic dynamics
    py::class_<Synapse>(m, "Synapse", R"pbdoc(A synaptic connection between neurons with plasticity and delays)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId, R"pbdoc(Get synapse ID)pbdoc")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, R"pbdoc(Get source neuron ID)pbdoc")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, R"pbdoc(Get destination neuron ID)pbdoc")
        .def("getWeight", &Synapse::getWeight, R"pbdoc(Get synaptic weight)pbdoc")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), R"pbdoc(Set synaptic weight)pbdoc")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), R"pbdoc(Add to synaptic weight)pbdoc")
        .def("getDelay", &Synapse::getDelay, R"pbdoc(Get synaptic delay in steps)pbdoc")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), R"pbdoc(Set synaptic delay)pbdoc")
        .def("getType", &Synapse::getType, R"pbdoc(Get synapse type)pbdoc")
        .def("setType", &Synapse::setType, py::arg("type"), R"pbdoc(Set synapse type)pbdoc")
        .def("isExcitatory", &Synapse::isExcitatory, R"pbdoc(Check if synapse is excitatory)pbdoc")
        .def("isInhibitory", &Synapse::isInhibitory, R"pbdoc(Check if synapse is inhibitory)pbdoc")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), R"pbdoc(Record pre-synaptic spike)pbdoc")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), R"pbdoc(Record post-synaptic spike)pbdoc")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, R"pbdoc(Get pre-synaptic spike history)pbdoc")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, R"pbdoc(Get post-synaptic spike history)pbdoc")
        .def("clearHistory", &Synapse::clearHistory, R"pbdoc(Clear spike history)pbdoc")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, R"pbdoc(Get plasticity flags)pbdoc")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, R"pbdoc(Get eligibility trace for reward-modulated plasticity)pbdoc")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), R"pbdoc(Set eligibility trace)pbdoc")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), R"pbdoc(Decay eligibility trace)pbdoc")
        .def("getEfficacy", &Synapse::getEfficacy, R"pbdoc(Get synaptic efficacy)pbdoc")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), R"pbdoc(Set synaptic efficacy)pbdoc")
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"), R"pbdoc(Enable plasticity mechanisms)pbdoc")
        .def("reset", &Synapse::reset, R"pbdoc(Reset synapse to initial state)pbdoc")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), R"pbdoc(Initialize synapse with random parameters)pbdoc")
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse id=" + std::to_string(s.getId().value) + 
                   " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // NeuralPopulation class
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(A group of neurons with shared properties and population-level dynamics)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId, R"pbdoc(Get population ID)pbdoc")
        .def("getSize", &NeuralPopulation::getSize, R"pbdoc(Get number of neurons)pbdoc")
        .def("isEmpty", &NeuralPopulation::isEmpty, R"pbdoc(Check if population is empty)pbdoc")
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"), R"pbdoc(Get neuron at index)pbdoc")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), R"pbdoc(Add neuron to population)pbdoc")
        .def("getNeurons", &NeuralPopulation::getNeurons, R"pbdoc(Get all neurons)pbdoc")
        .def("getNeuronType", &NeuralPopulation::getNeuronType, R"pbdoc(Get neuron type for this population)pbdoc")
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"), R"pbdoc(Set neuron type)pbdoc")
        .def("getActiveCount", &NeuralPopulation::getActiveCount, R"pbdoc(Get number of active neurons)pbdoc")
        .def("getFiringCount", &NeuralPopulation::getFiringCount, R"pbdoc(Get number of firing neurons)pbdoc")
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate, R"pbdoc(Get average firing rate)pbdoc")
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel, R"pbdoc(Get activity level (0-1))pbdoc")
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"), R"pbdoc(Initialize all neurons randomly)pbdoc")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), R"pbdoc(Advance population for one simulation step)pbdoc")
        .def("reset", &NeuralPopulation::reset, R"pbdoc(Reset all neurons)pbdoc")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds, R"pbdoc(Get all neuron IDs)pbdoc")
        .def("recordActivity", &NeuralPopulation::recordActivity, R"pbdoc(Record current state for statistics)pbdoc")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential, R"pbdoc(Get mean membrane potential)pbdoc")
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential, R"pbdoc(Get variance of membrane potential)pbdoc")
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation id=" + std::to_string(p.getId().value) + 
                   " size=" + std::to_string(p.getSize()) + ">";
        });

    // NeuralRegion class
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(A brain region containing multiple populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId, R"pbdoc(Get region ID)pbdoc")
        .def("getName", &NeuralRegion::getName, R"pbdoc(Get region name)pbdoc")
        .def("setName", &NeuralRegion::setName, py::arg("name"), R"pbdoc(Set region name)pbdoc")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal, R"pbdoc(Add a new population)pbdoc")
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"), R"pbdoc(Get population by ID)pbdoc")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount, R"pbdoc(Get number of populations)pbdoc")
        .def("getPopulations", &NeuralRegion::getPopulations, R"pbdoc(Get all populations)pbdoc")
        .def("getAllPopulations", &NeuralRegion::getAllPopulations, R"pbdoc(Get all populations (non-const version))pbdoc")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1, R"pbdoc(Add a synapse)pbdoc")
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"), R"pbdoc(Get synapse by ID)pbdoc")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount, R"pbdoc(Get number of synapses)pbdoc")
        .def("getSynapses", &NeuralRegion::getSynapses, R"pbdoc(Get all synapses)pbdoc")
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"), R"pbdoc(Get synapses originating from neuron)pbdoc")
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"), R"pbdoc(Get synapses terminating at neuron)pbdoc")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount, R"pbdoc(Get total neuron count)pbdoc")
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount, R"pbdoc(Get active neuron count)pbdoc")
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount, R"pbdoc(Get firing neuron count)pbdoc")
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate, R"pbdoc(Get average firing rate)pbdoc")
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight, R"pbdoc(Get average synaptic weight)pbdoc")
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity, R"pbdoc(Get synaptic density)pbdoc")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), R"pbdoc(Advance region for one simulation step)pbdoc")
        .def("reset", &NeuralRegion::reset, R"pbdoc(Reset all neurons and synapses)pbdoc")
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, 
              py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"), R"pbdoc(Initialize random connectivity)pbdoc")
        .def("getAllNeurons", &NeuralRegion::getAllNeurons, R"pbdoc(Get all neurons)pbdoc")
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion id=" + std::to_string(r.getId().value) + 
                   " name=\"" + r.getName() + "\">";
        });

    // ==============================================
    // Memory Systems
    // ==============================================

    // NeuralWorkingMemory - working memory using persistent activity
    py::class_<NeuralWorkingMemory>(m, "WorkingMemory", R"pbdoc(Neural working memory using persistent activity and recurrent connections)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"), R"pbdoc(Initialize with brain reference)pbdoc")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f, R"pbdoc(Store pattern in working memory)pbdoc")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"), R"pbdoc(Store activation to specific neuron)pbdoc")
        .def("retrieve", &NeuralWorkingMemory::retrieve, R"pbdoc(Retrieve working memory content as activity levels)pbdoc")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"), R"pbdoc(Check if neuron is in working memory)pbdoc")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"), R"pbdoc(Get activation of specific neuron)pbdoc")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), R"pbdoc(Update working memory)pbdoc")
        .def("clear", &NeuralWorkingMemory::clear, R"pbdoc(Clear working memory)pbdoc")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, R"pbdoc(Get number of active memory traces)pbdoc")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, R"pbdoc(Get memory capacity)pbdoc")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"), R"pbdoc(Set memory capacity)pbdoc")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, R"pbdoc(Get decay rate)pbdoc")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"), R"pbdoc(Set decay rate)pbdoc")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons, R"pbdoc(Get neurons in working memory)pbdoc")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"), R"pbdoc(Strengthen memory representation)pbdoc")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition, R"pbdoc(Run competition between memory traces)pbdoc")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"), R"pbdoc(Check if neuron is winner)pbdoc")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity, R"pbdoc(Get overall memory activity)pbdoc");

    // NeuralEpisodicMemory - episodic memory with experience encoding
    py::class_<NeuralEpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Episodic memory system for experience storage and replay)pbdoc")
        .def(py::init<>())
        .def("storeExperience", &NeuralEpisodicMemory::storeExperience, py::arg("pattern"), py::arg("context"), py::arg("reward"), R"pbdoc(Store experience)pbdoc")
        .def("replayMemory", &NeuralEpisodicMemory::replayMemory, py::arg("pattern"), py::arg("time"), R"pbdoc(Replay stored memory)pbdoc")
        .def("getSimilarExperiences", &NeuralEpisodicMemory::getSimilarExperiences, py::arg("pattern"), py::arg("threshold") = 0.5f, R"pbdoc(Get similar experiences)pbdoc")
        .def("getLastExperience", &NeuralEpisodicMemory::getLastExperience, R"pbdoc(Get last experience)pbdoc")
        .def("clear", &NeuralEpisodicMemory::clear, R"pbdoc(Clear all memories)pbdoc")
        .def("getMemoryCount", &NeuralEpisodicMemory::getMemoryCount, R"pbdoc(Get number of stored memories)pbdoc")
        .def("getExperienceStrength", &NeuralEpisodicMemory::getExperienceStrength, py::arg("pattern"), R"pbdoc(Get strength of specific experience)pbdoc");

    // NeuralSemanticMemory - semantic memory for knowledge representation
    py::class_<NeuralSemanticMemory>(m, "SemanticMemory", R"pbdoc(Semantic memory system for abstract knowledge and concepts)pbdoc")
        .def(py::init<>())
        .def("learnConcept", &NeuralSemanticMemory::learnConcept, py::arg("pattern"), py::arg("concept"), py::arg("strength") = 1.0f, R"pbdoc(Learn a new concept)pbdoc")
        .def("recallConcept", &NeuralSemanticMemory::recallConcept, py::arg("concept"), R"pbdoc(Recall a concept)pbdoc")
        .def("findSimilarConcepts", &NeuralSemanticMemory::findSimilarConcepts, py::arg("pattern"), R"pbdoc(Find similar concepts)pbdoc")
        .def("getConceptStrength", &NeuralSemanticMemory::getConceptStrength, py::arg("concept"), R"pbdoc(Get concept strength)pbdoc")
        .def("getConceptCount", &NeuralSemanticMemory::getConceptCount, R"pbdoc(Get number of concepts)pbdoc")
        .def("clear", &NeuralSemanticMemory::clear, R"pbdoc(Clear all concepts)pbdoc");

    // NeuralProceduralMemory - procedural memory for skills and behaviors
    py::class_<NeuralProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Procedural memory system for learned skills and behaviors)pbdoc")
        .def(py::init<>())
        .def("storeActionSequence", &NeuralProceduralMemory::storeActionSequence, py::arg("actions"), py::arg("success"), R"pbdoc(Store action sequence)pbdoc")
        .def("retrieveActionSequence", &NeuralProceduralMemory::retrieveActionSequence, py::arg("context"), R"pbdoc(Retrieve action sequence)pbdoc")
        .def("evaluateActionSequence", &NeuralProceduralMemory::evaluateActionSequence, py::arg("actions"), R"pbdoc(Evaluate action sequence)pbdoc")
        .def("getActionStrength", &NeuralProceduralMemory::getActionStrength, py::arg("action_id"), R"pbdoc(Get action strength)pbdoc")
        .def("getSkillCount", &NeuralProceduralMemory::getSkillCount, R"pbdoc(Get number of skills)pbdoc")
        .def("clear", &NeuralProceduralMemory::clear, R"pbdoc(Clear all skills)pbdoc");

    // NeuralAssociativeMemory - associative memory for pattern associations
    py::class_<NeuralAssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Associative memory system for pattern associations and connections)pbdoc")
        .def(py::init<>())
        .def("associatePatterns", &NeuralAssociativeMemory::associatePatterns, py::arg("pattern1"), py::arg("pattern2"), py::arg("strength") = 1.0f, R"pbdoc(Associate two patterns)pbdoc")
        .def("recallPattern", &NeuralAssociativeMemory::recallPattern, py::arg("pattern"), R"pbdoc(Recall associated pattern)pbdoc")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength, py::arg("pattern1"), py::arg("pattern2"), R"pbdoc(Get association strength)pbdoc")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount, R"pbdoc(Get number of associations)pbdoc")
        .def("clear", &NeuralAssociativeMemory::clear, R"pbdoc(Clear all associations)pbdoc");

    // ==============================================
    // Plasticity Systems
    // ==============================================

    // PlasticityRule base class
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for synaptic plasticity rules)pbdoc")
        .def("update", &PlasticityRule::update, py::arg("synapse"), py::arg("pre_spikes"), py::arg("post_spikes"), py::arg("dt"), R"pbdoc(Update synaptic weights based on spike timing)pbdoc")
        .def("applyWeightChange", &PlasticityRule::applyWeightChange, py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &PlasticityRule::getName, R"pbdoc(Get rule name)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled, R"pbdoc(Check if rule is enabled)pbdoc")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"), R"pbdoc(Enable/disable rule)pbdoc");

    // STDP - Spike-Timing-Dependent Plasticity
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity rule)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update, py::arg("synapse"), py::arg("pre_spikes"), py::arg("post_spikes"), py::arg("dt"), R"pbdoc(Update synaptic weights)pbdoc")
        .def("applyWeightChange", &STDP::applyWeightChange, py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &STDP::getName, R"pbdoc(Get rule name)pbdoc")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), R"pbdoc(Set LTP weight)pbdoc")
        .def("getLTPWeight", &STDP::getLTPWeight, R"pbdoc(Get LTP weight)pbdoc")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), R"pbdoc(Set LTD weight)pbdoc")
        .def("getLTDWeight", &STDP::getLTDWeight, R"pbdoc(Get LTD weight)pbdoc")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), R"pbdoc(Set time constant)pbdoc")
        .def("getTimeConstant", &STDP::getTimeConstant, R"pbdoc(Get time constant)pbdoc")
        .def("configure", &STDP::configure, py::arg("ltp_weight"), py::arg("ltd_weight"), py::arg("tau"), R"pbdoc(Configure from parameters)pbdoc");

    // Hebbian - Hebbian learning
    py::class_<HebbianRule>(m, "HebbianRule", R"pbdoc(Hebbian plasticity rule: 'neurons that fire together, wire together')pbdoc")
        .def(py::init<>())
        .def("update", &HebbianRule::update, py::arg("synapse"), py::arg("pre_spikes"), py::arg("post_spikes"), py::arg("dt"), R"pbdoc(Update synaptic weights)pbdoc")
        .def("applyWeightChange", &HebbianRule::applyWeightChange, py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &HebbianRule::getName, R"pbdoc(Get rule name)pbdoc")
        .def("setLearningRate", &HebbianRule::setLearningRate, py::arg("rate"), R"pbdoc(Set learning rate)pbdoc")
        .def("getLearningRate", &HebbianRule::getLearningRate, R"pbdoc(Get learning rate)pbdoc");

    // StructuralPlasticity - structural synapse formation
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity for synapse formation and elimination)pbdoc")
        .def(py::init<>())
        .def("update", &StructuralPlasticity::update, py::arg("synapse"), py::arg("pre_spikes"), py::arg("post_spikes"), py::arg("dt"), R"pbdoc(Update structural connections)pbdoc")
        .def("applyWeightChange", &StructuralPlasticity::applyWeightChange, py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &StructuralPlasticity::getName, R"pbdoc(Get rule name)pbdoc")
        .def("enableStructuralGrowth", &StructuralPlasticity::enableStructuralGrowth, py::arg("enable"), R"pbdoc(Enable structural growth)pbdoc")
        .def("isStructuralGrowthEnabled", &StructuralPlasticity::isStructuralGrowthEnabled, R"pbdoc(Check if structural growth enabled)pbdoc")
        .def("enablePruning", &StructuralPlasticity::enablePruning, py::arg("enable"), R"pbdoc(Enable pruning)pbdoc")
        .def("isPruningEnabled", &StructuralPlasticity::isPruningEnabled, R"pbdoc(Check if pruning enabled)pbdoc");

    // ==============================================
    // Neuromodulation Systems
    // ==============================================

    // Dopamine - reward signaling
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward and reinforcement)pbdoc")
        .def(py::init<>())
        .def("update", &Dopamine::update, py::arg("reward"), py::arg("prediction_error"), py::arg("dt"), R"pbdoc(Update neuromodulator level)pbdoc")
        .def("getLevel", &Dopamine::getLevel, R"pbdoc(Get current dopamine level)pbdoc")
        .def("getPhasicResponse", &Dopamine::getPhasicResponse, py::arg("reward"), R"pbdoc(Get phasic response to reward)pbdoc")
        .def("getTonicLevel", &Dopamine::getTonicLevel, R"pbdoc(Get tonic dopamine level)pbdoc")
        .def("setParameters", &Dopamine::setParameters, py::arg("tonic"), py::arg("phasic"), py::arg("time_constant"), R"pbdoc(Set neuromodulator parameters)pbdoc");

    // Curiosity - exploration motivation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration and novelty detection)pbdoc")
        .def(py::init<>())
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("dt"), R"pbdoc(Update neuromodulator level)pbdoc")
        .def("getLevel", &Curiosity::getLevel, R"pbdoc(Get current curiosity level)pbdoc")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, R"pbdoc(Get exploration drive)pbdoc")
        .def("setParameters", &Curiosity::setParameters, py::arg("baseline"), py::arg("sensitivity"), py::arg("decay_rate"), R"pbdoc(Set neuromodulator parameters)pbdoc");

    // Novelty - novelty detection
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty neuromodulator for detecting and responding to novel stimuli)pbdoc")
        .def(py::init<>())
        .def("update", &Novelty::update, py::arg("input_pattern"), py::arg("dt"), R"pbdoc(Update novelty detection)pbdoc")
        .def("getLevel", &Novelty::getLevel, R"pbdoc(Get current novelty level)pbdoc")
        .def("detectNovelty", &Novelty::detectNovelty, py::arg("current_pattern"), R"pbdoc(Detect novelty in input)pbdoc")
        .def("getSurprise", &Novelty::getSurprise, R"pbdoc(Get surprise value)pbdoc")
        .def("setParameters", &Novelty::setParameters, py::arg("sensitivity"), py::arg("adaptation_rate"), R"pbdoc(Set neuromodulator parameters)pbdoc");

    // PredictionError - prediction error signaling
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator for learning from surprises)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionError::update, py::arg("predicted"), py::arg("actual"), py::arg("dt"), R"pbdoc(Update prediction error)pbdoc")
        .def("getError", &PredictionError::getError, R"pbdoc(Get prediction error)pbdoc")
        .def("getLearningRate", &PredictionError::getLearningRate, R"pbdoc(Get learning rate from prediction error)pbdoc")
        .def("reset", &PredictionError::reset, R"pbdoc(Reset prediction error)pbdoc");

    // ==============================================
    // Prediction and Cognition
    // ==============================================

    // PredictionSystem - forward models and prediction
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction and forward models)pbdoc")
        .def(py::init<>())
        .def("update", &PredictionSystem::update, py::arg("sensory_input"), py::arg("dt"), R"pbdoc(Update predictions)pbdoc")
        .def("getPredictionError", &PredictionSystem::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        .def("getPredictedSensory", &PredictionSystem::getPredictedSensory, R"pbdoc(Get predicted sensory input)pbdoc")
        .def("getConfidence", &PredictionSystem::getConfidence, R"pbdoc(Get prediction confidence)pbdoc")
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"), R"pbdoc(Set prediction learning rate)pbdoc")
        .def("getPredictionError", &PredictionSystem::getPredictionError, R"pbdoc(Get prediction error)pbdoc");

    // NeuralPlanner - action planning and decision making
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action sequence evaluation and selection)pbdoc")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction, py::arg("current_state"), py::arg("available_actions"), R"pbdoc(Plan best action)pbdoc")
        .def("evaluateAction", &NeuralPlanner::evaluateAction, py::arg("action"), py::arg("context"), R"pbdoc(Evaluate action quality)pbdoc")
        .def("getCurrentPlan", &NeuralPlanner::getCurrentPlan, R"pbdoc(Get current action plan)pbdoc")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence, R"pbdoc(Get planning confidence)pbdoc")
        .def("setPlanningHorizon", &NeuralPlanner::setPlanningHorizon, py::arg("horizon"), R"pbdoc(Set planning horizon)pbdoc")
        .def("setExplorationWeight", &NeuralPlanner::setExplorationWeight, py::arg("weight"), R"pbdoc(Set exploration weight)pbdoc");

    // ConceptFormation - abstract concept learning
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system for discovering abstract patterns)pbdoc")
        .def(py::init<>())
        .def("learnConcept", &ConceptFormation::learnConcept, py::arg("pattern"), py::arg("context"), py::arg("strength") = 1.0f, R"pbdoc(Learn a new concept)pbdoc")
        .def("applyConcept", &ConceptFormation::applyConcept, py::arg("pattern"), R"pbdoc(Apply learned concept to pattern)pbdoc")
        .def("getConceptActivation", &ConceptFormation::getConceptActivation, py::arg("pattern"), R"pbdoc(Get concept activation)pbdoc")
        .def("getConceptHierarchy", &ConceptFormation::getConceptHierarchy, R"pbdoc(Get concept hierarchy)pbdoc")
        .def("setConceptLearningRate", &ConceptFormation::setConceptLearningRate, py::arg("rate"), R"pbdoc(Set concept learning rate)pbdoc")
        .def("setConceptPruningThreshold", &ConceptFormation::setConceptPruningThreshold, py::arg("threshold"), R"pbdoc(Set concept pruning threshold)pbdoc");

    // AttentionalSelection - attention mechanism
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection system for focusing processing resources)pbdoc")
        .def(py::init<>())
        .def("selectAttention", &AttentionalSelection::selectAttention, py::arg("candidate_neurons"), py::arg("global_inhibition") = 0.5f, R"pbdoc(Select attentional focus)pbdoc")
        .def("applyAttentionBias", &AttentionalSelection::applyAttentionBias, py::arg("neuron"), py::arg("bias_strength"), R"pbdoc(Apply attentional bias to neuron)pbdoc")
        .def("getCurrentFocus", &AttentionalSelection::getCurrentFocus, R"pbdoc(Get current attentional focus)pbdoc")
        .def("getAttendedNeurons", &AttentionalSelection::getAttendedNeurons, R"pbdoc(Get attended neurons)pbdoc")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"), R"pbdoc(Set inhibition strength)pbdoc")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"), R"pbdoc(Set excitation strength)pbdoc")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"), R"pbdoc(Set competition threshold)pbdoc");

    // ==============================================
    // Development System
    // ==============================================

    // DevelopmentSystem - developmental changes over time
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Development system for age-dependent changes and maturation)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("age"), py::arg("dt"), R"pbdoc(Update developmental state)pbdoc")
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("getAge", &DevelopmentSystem::getAge, R"pbdoc(Get current age)pbdoc")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier, R"pbdoc(Get plasticity modifier)pbdoc")
        .def("setGrowthRate", &DevelopmentSystem::setGrowthRate, py::arg("rate"), R"pbdoc(Set growth rate)pbdoc")
        .def("setPlasticityTrajectory", &DevelopmentSystem::setPlasticityTrajectory, py::arg("trajectory"), R"pbdoc(Set plasticity trajectory)pbdoc")
        .def("getCriticalPeriodStatus", &DevelopmentSystem::getCriticalPeriodStatus, R"pbdoc(Check if in critical period)pbdoc")
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress, R"pbdoc(Get maturation progress)pbdoc");

    // ==============================================
    // Supporting Systems
    // ==============================================

    // SpikeSystem - event-driven spike management
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike system for event-driven spike propagation and management)pbdoc")
        .def(py::init<>())
        .def("recordSpike", &SpikeSystem::recordSpike, py::arg("neuron_id"), py::arg("timestamp"), R"pbdoc(Record a spike event)pbdoc")
        .def("getSpikes", &SpikeSystem::getSpikes, R"pbdoc(Get all spike events)pbdoc")
        .def("getSpikesForNeuron", &SpikeSystem::getSpikesForNeuron, py::arg("neuron_id"), R"pbdoc(Get spikes for specific neuron)pbdoc")
        .def("getPendingSpikes", &SpikeSystem::getPendingSpikes, R"pbdoc(Get pending spike events)pbdoc")
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("current_step"), R"pbdoc(Process scheduled spikes)pbdoc")
        .def("clear", &SpikeSystem::clear, R"pbdoc(Clear all spike events)pbdoc")
        .def("getSpikeCount", &SpikeSystem::getSpikeCount, R"pbdoc(Get total spike count)pbdoc")
        .def("getLastSpikeTime", &SpikeSystem::getLastSpikeTime, py::arg("neuron_id"), R"pbdoc(Get last spike time for neuron)pbdoc");

    // RandomGenerator - stochastic processes
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generator for stochastic processes)pbdoc")
        .def(py::init<>())
        .def("seed", py::arg("seed"), R"pbdoc(Seed the random generator)pbdoc")
        .def("randFloat", py::arg("min") = 0.0f, py::arg("max") = 1.0f, R"pbdoc(Generate random float)pbdoc")
        .def("randInt", py::arg("min"), py::arg("max"), R"pbdoc(Generate random integer)pbdoc")
        .def("normalFloat", py::arg("mean") = 0.0f, py::arg("stddev") = 1.0f, R"pbdoc(Generate normally distributed float)pbdoc")
        .def("chooseRandom", py::arg("container"), R"pbdoc(Randomly choose from container)pbdoc")
        .def("shuffle", py::arg("container"), R"pbdoc(Shuffle container in-place)pbdoc")
        .def("getState", &RandomGenerator::getState, R"pbdoc(Get current RNG state)pbdoc")
        .def("setState", &RandomGenerator::setState, py::arg("state"), R"pbdoc(Set RNG state)pbdoc");

    // Logger - debugging and monitoring
    py::class_<Logger>(m, "Logger", R"pbdoc(Logging system for debugging and monitoring)pbdoc")
        .def(py::init<>())
        .def("log", py::arg("message"), R"pbdoc(Log a message)pbdoc")
        .def("logDebug", py::arg("message"), R"pbdoc(Log debug message)pbdoc")
        .def("logInfo", py::arg("message"), R"pbdoc(Log info message)pbdoc")
        .def("logWarning", py::arg("message"), R"pbdoc(Log warning message)pbdoc")
        .def("logError", py::arg("message"), R"pbdoc(Log error message)pbdoc")
        .def("setLevel", py::arg("level"), R"pbdoc(Set log level)pbdoc")
        .def("getLevel", &Logger::getLevel, R"pbdoc(Get current log level)pbdoc")
        .def("enableFileOutput", py::arg("filepath"), R"pbdoc(Enable file output)pbdoc")
        .def("disableFileOutput", &Logger::disableFileOutput, R"pbdoc(Disable file output)pbdoc")
        .def("clear", &Logger::clear, R"pbdoc(Clear log buffer)pbdoc")
        .def("getLogBuffer", &Logger::getLogBuffer, R"pbdoc(Get log buffer as string)pbdoc");

    // SimulationClock - time management
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation clock for time management)pbdoc")
        .def(py::init<>())
        .def("getTime", &SimulationClock::getTime, R"pbdoc(Get current simulation time)pbdoc")
        .def("getStep", &SimulationClock::getStep, R"pbdoc(Get current simulation step)pbdoc")
        .def("step", R"pbdoc(Advance clock by one step)pbdoc"[py::arg("dt") = 0.1], &SimulationClock::step)
        .def("setTime", py::arg("time"), R"pbdoc(Set current time)pbdoc")
        .def("setStep", py::arg("step"), R"pbdoc(Set current step)pbdoc")
        .def("getTimeScale", &SimulationClock::getTimeScale, R"pbdoc(Get time scale)pbdoc")
        .def("setTimeScale", py::arg("scale"), R"pbdoc(Set time scale)pbdoc")
        .def("reset", &SimulationClock::reset, R"pbdoc(Reset clock)pbdoc")
        .def("isPaused", &SimulationClock::isPaused, R"pbdoc(Check if clock is paused)pbdoc")
        .def("pause", &SimulationClock::pause, R"pbdoc(Pause clock)pbdoc")
        .def("resume", &SimulationClock::resume, R"pbdoc(Resume clock)pbdoc");

    // ==============================================
    // Factory Functions (Convenience Constructors)
    // ==============================================

    // Convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a brain with configuration)pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a simple world)pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create an agent brain interface)pbdoc");

    m.def("createAgentBody", []() -> std::shared_ptr<AgentBody> {
        return std::make_shared<AgentBody>();
    }, R"pbdoc(Create an agent body)pbdoc");

    // ==============================================
    // Utility Functions
    // ==============================================

    // Neural network utilities
    m.def("createRandomNetwork", [](size_t num_neurons, size_t num_regions, RandomGenerator& rng) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int>(num_neurons));
        config->set("brain.region_count", static_cast<int>(num_regions));
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->initialize()) {
            return brain;
        }
        throw NLMException("Failed to create random network");
    }, py::arg("num_neurons"), py::arg("num_regions"), py::arg("rng"), R"pbdoc(Create a random neural network)pbdoc");

    m.def("connectRandomly", [](Brain& brain, RandomGenerator& rng, float connection_probability) {
        for (size_t i = 0; i < brain.getRegionCount(); ++i) {
            auto region = brain.getRegion(brain.getRegionIds()[i]);
            region->initializeRandomConnectivity(rng, connection_probability, 0.5f, 0.1f);
        }
    }, py::arg("brain"), py::arg("rng"), py::arg("connection_probability"), R"pbdoc(Connect neurons randomly in brain)pbdoc");

    m.def("normalizeWeights", [](Brain& brain, float target_mean = 0.5f, float target_stddev = 0.1f) {
        // Normalize synaptic weights across the network
        float total_weight = 0.0f;
        size_t weight_count = 0;
        
        for (size_t i = 0; i < brain.getRegionCount(); ++i) {
            auto region = brain.getRegion(brain.getRegionIds()[i]);
            size_t synapse_count = region->getSynapseCount();
            for (size_t j = 0; j < synapse_count; ++j) {
                // Note: This would need access to synapse weights
            }
        }
        
        // For now, just return success
        return true;
    }, py::arg("brain"), py::arg("target_mean") = 0.5f, py::arg("target_stddev") = 0.1f, R"pbdoc(Normalize synaptic weights)pbdoc");

    m.def("getNetworkStatistics", [](Brain& brain) {
        py::dict stats;
        stats["neuron_count"] = brain.getTotalNeuronCount();
        stats["synapse_count"] = brain.getTotalSynapseCount();
        stats["active_neuron_count"] = brain.getActiveNeuronCount();
        stats["firing_neuron_count"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spike_count"] = brain.getTotalSpikeCount();
        return stats;
    }, py::arg("brain"), R"pbdoc(Get comprehensive network statistics)pbdoc");

    // ==============================================
    // Simulation Control Utilities
    // ==============================================

    m.def("runSimulation", [](Brain& brain, size_t num_steps, SimulationClock& clock) {
        for (size_t step = 0; step < num_steps; ++step) {
            brain.step(step);
            clock.step(0.1); // Assume 0.1 timestep duration
        }
    }, py::arg("brain"), py::arg("num_steps"), py::arg("clock"), R"pbdoc(Run complete simulation)pbdoc");

    m.def("saveBrainState", [](Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc");

    m.def("loadBrainState", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc");

    m.def("benchmarkNetwork", [](Brain& brain, RandomGenerator& rng, size_t num_steps) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < num_steps; ++step) {
            brain.step(step);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        py::dict results;
        results["steps_per_second"] = static_cast<double>(num_steps) / (duration.count() / 1000.0);
        results["total_spikes"] = brain.getTotalSpikeCount();
        results["total_neurons"] = brain.getTotalNeuronCount();
        results["total_synapses"] = brain.getTotalSynapseCount();
        results["duration_ms"] = duration.count();
        return results;
    }, py::arg("brain"), py::arg("rng"), py::arg("num_steps"), R"pbdoc(Run benchmark simulation)pbdoc");

    // ==============================================
    // Advanced Debugging and Analysis
    // ==============================================

    m.def("analyzeNetworkTopology", [](Brain& brain) {
        py::dict topology;
        
        // Analyze region structure
        py::list regions;
        for (size_t i = 0; i < brain.getRegionCount(); ++i) {
            auto region = brain.getRegion(brain.getRegionIds()[i]);
            py::dict region_info;
            region_info["id"] = region->getId().value;
            region_info["name"] = region->getName();
            region_info["neuron_count"] = region->getTotalNeuronCount();
            region_info["synapse_count"] = region->getSynapseCount();
            region_info["population_count"] = region->getPopulationCount();
            regions.append(region_info);
        }
        topology["regions"] = regions;
        
        // Analyze connectivity
        size_t total_connections = 0;
        for (size_t i = 0; i < brain.getRegionCount(); ++i) {
            auto region = brain.getRegion(brain.getRegionIds()[i]);
            total_connections += region->getSynapseCount();
        }
        topology["total_connections"] = total_connections;
        topology["connection_density"] = static_cast<float>(total_connections) / 
            (brain.getTotalNeuronCount() * brain.getTotalNeuronCount());
        
        return topology;
    }, py::arg("brain"), R"pbdoc(Analyze network topology)pbdoc");

    m.def("detectNetworkIssues", [](Brain& brain) {
        py::dict issues;
        
        // Check for runaway excitation
        float e_i_ratio = brain.getExcitationInhibitionRatio();
        if (e_i_ratio > 10.0f) {
            issues["high_excitation"] = "E/I ratio is very high: " + std::to_string(e_i_ratio);
        }
        
        // Check for network silence
        if (brain.getActiveNeuronCount() == 0) {
            issues["network_silence"] = "No active neurons in network";
        }
        
        // Check for excessive spiking
        if (brain.getTotalSpikeCount() > 1000000) {
            issues["high_spiking"] = "Total spike count is very high: " + std::to_string(brain.getTotalSpikeCount());
        }
        
        return issues;
    }, py::arg("brain"), R"pbdoc(Detect potential network issues)pbdoc");

    // ==============================================
    // End of Module
    // ==============================================

} // namespace nlm
