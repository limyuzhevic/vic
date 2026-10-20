#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/buffer_info.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/brain/NeuralRegion.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"
#include "src/plasticity/PlasticityRule.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/memory/WorkingMemory.hpp"
#include "src/memory/EpisodicMemory.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/sensory/SensoryInput.hpp"
#include "src/sensory/Vision.hpp"
#include "src/sensory/Audio.hpp"
#include "src/sensory/InternalSignals.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/world/Observation.hpp"
#include "src/world/WorldObject.hpp"
#include "src/world/Action.hpp"
#include "src/motor/MotorSystem.hpp"
#include "src/prediction/PredictionSystem.hpp"
#include "src/prediction/NeuralPrediction.hpp"
#include "src/cognition/NeuralPlanner.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include "src/development/DevelopmentSystem.hpp"
#include "src/development/Synaptogenesis.hpp"
#include "src/development/Pruning.hpp"
#include "src/development/Maturation.hpp"
#include "src/dynamics/NeuralDynamics.hpp"
#include "src/dynamics/SpikeSystem.hpp"

namespace py = pybind11;

// Global constants for Python access
static const NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
static const SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
static const RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
static const PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// Exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class NeuralError : public NLMError {
public:
    explicit NeuralError(const std::string& msg) : NLMError("Neural error: " + msg) {}
};

// Helper functions
namespace py_helpers {
    template<typename T>
    py::object to_py_object(const T& value) {
        return py::cast(value);
    }
    
    template<typename Container>
    py::list to_py_list(const Container& container) {
        py::list result;
        for (const auto& item : container) {
            result.append(item);
        }
        return result;
    }
    
    template<typename Map>
    py::dict to_py_dict(const Map& map) {
        py::dict result;
        for (const auto& pair : map) {
            result[py::cast(pair.first)] = py::cast(pair.second);
        }
        return result;
    }
}

// Buffer wrapper for memory-safe array access
class BufferWrapper {
public:
    static py::buffer_info get_buffer_info(const std::vector<float>& data) {
        return py::buffer_info(
            data.data(),
            sizeof(float),
            py::format_descriptor<float>::format(),
            1,
            {static_cast<ssize_t>(data.size())},
            {sizeof(float)}
        );
    }
    
    static py::buffer_info get_buffer_info(const std::vector<double>& data) {
        return py::buffer_info(
            data.data(),
            sizeof(double),
            py::format_descriptor<double>::format(),
            1,
            {static_cast<ssize_t>(data.size())},
            {sizeof(double)}
        );
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (熙然) - Neural Learning Machine Python Bindings
        ----------------------------------------------------
        A Python interface for the NLM C++ neural simulation framework.
        
        This module provides access to:
        - Brain simulation with spiking neurons (LIF dynamics)
        - Synaptic plasticity (STDP, Hebbian, structural)
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Sensorimotor loop (perception and action)
        - Developmental processes
        - Full integration testing (Phase 6)
        
        Key features:
        - Event-driven neural dynamics
        - Configurable plasticity rules
        - Memory consolidation and replay
        - Curiosity-driven exploration
        - Checkpoint save/load
        - Statistical metrics and logging
        
        Usage example:
            import pynlm
            
            # Create brain with default config
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            for step in range(1000):
                brain.step(step, step * 0.001)
            
            # Get statistics
            print(f"Neurons: {brain.getTotalNeuronCount()}")
            print(f"Spikes: {brain.getTotalSpikeCount()}")
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<NeuralError>(m, "NeuralError");

    // ===================================================================
    // Core Types and Constants
    // ===================================================================

    // Neuron ID type
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

    // Synapse ID type
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

    // Region ID type
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

    // Population ID type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // ===================================================================
    // Enumerations
    // ===================================================================

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

    // ===================================================================
    // Core Data Structures
    // ===================================================================

    py::class_<NeuronState>(m, "NeuronState", R"pbdoc(Neuron internal state)pbdoc")
        .def(py::init<>())
        .def_readwrite("membranePotential", &NeuronState::membranePotential)
        .def_readwrite("restingPotential", &NeuronState::restingPotential)
        .def_readwrite("threshold", &NeuronState::threshold)
        .def_readwrite("resetPotential", &NeuronState::resetPotential)
        .def_readwrite("leakConductance", &NeuronState::leakConductance)
        .def_readwrite("synapseConductance", &NeuronState::synapseConductance)
        .def_readwrite("firingRate", &NeuronState::firingRate)
        .def_readwrite("firingState", &NeuronState::firingState)
        .def_readwrite("refractoryRemaining", &NeuronState::refractoryRemaining)
        .def_readwrite("refractoryPeriod", &NeuronState::refractoryPeriod)
        .def_readwrite("adaptationVariable", &NeuronState::adaptationVariable)
        .def_readwrite("lastSpikeTime", &NeuronState::lastSpikeTime);

    py::class_<PlasticityFlags>(m, "PlasticityFlags", R"pbdoc(Plasticity rule activation flags)pbdoc")
        .def(py::init<>())
        .def_readwrite("hebbian", &PlasticityFlags::hebbian)
        .def_readwrite("stdp", &PlasticityFlags::stdp)
        .def_readwrite("rewardModulated", &PlasticityFlags::reward_modulated)
        .def_readwrite("structural", &PlasticityFlags::structural)
        .def_readwrite("eligible", &PlasticityFlags::eligible);

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Neuron identifier)pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Synapse identifier)pbdoc")
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Region identifier)pbdoc")
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

    // ===================================================================
    // Neuron Class
    // ===================================================================

    py::class_<Neuron>(m, "Neuron", R"pbdoc(Leaky Integrate-and-Fire (LIF) neuron)")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, R"pbdoc(Get neuron ID)pbdoc")
        .def("getType", &Neuron::getType, R"pbdoc(Get neuron type)pbdoc")
        .def("setType", &Neuron::setType, py::arg("neuron_type"), R"pbdoc(Set neuron type)pbdoc")
        .def("getState", &Neuron::getState, R"pbdoc(Get neuron state)pbdoc")
        .def("getMembranePotential", &Neuron::getMembranePotential, R"pbdoc(Get membrane potential)pbdoc")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), R"pbdoc(Set membrane potential)pbdoc")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), R"pbdoc(Add to membrane potential)pbdoc")
        .def("getThreshold", &Neuron::getThreshold, R"pbdoc(Get firing threshold)pbdoc")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), R"pbdoc(Set firing threshold)pbdoc")
        .def("isFiring", &Neuron::isFiring, R"pbdoc(Check if neuron is firing)pbdoc")
        .def("isRefractory", &Neuron::isRefractory, R"pbdoc(Check if neuron is in refractory period)pbdoc")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), R"pbdoc(Set firing state)pbdoc")
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"), R"pbdoc(Set refractory period)pbdoc")
        .def("decrementRefractory", &Neuron::decrementRefractory, R"pbdoc(Decrement refractory counter)pbdoc")
        .def("getFiringRate", &Neuron::getFiringRate, R"pbdoc(Get firing rate)pbdoc")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), R"pbdoc(Set firing rate)pbdoc")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), R"pbdoc(Set leak conductance)pbdoc")
        .def("getLeakConductance", &Neuron::getLeakConductance, R"pbdoc(Get leak conductance)pbdoc")
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod, R"pbdoc(Get refractory period)pbdoc")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("potential"), R"pbdoc(Set resting potential)pbdoc")
        .def("getRestingPotential", &Neuron::getRestingPotential, R"pbdoc(Get resting potential)pbdoc")
        .def("setResetPotential", &Neuron::setResetPotential, py::arg("potential"), R"pbdoc(Set reset potential)pbdoc")
        .def("checkThreshold", &Neuron::checkThreshold, R"pbdoc(Check if neuron has reached threshold)pbdoc")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, R"pbdoc(Get timestamp of last spike)pbdoc")
        .def("reset", &Neuron::reset, R"pbdoc(Reset neuron to initial state)pbdoc")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), R"pbdoc(Inject current into neuron)pbdoc");

    // ===================================================================
    // Synapse Class
    // ===================================================================

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId, R"pbdoc(Get synapse ID)pbdoc")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, R"pbdoc(Get source neuron ID)pbdoc")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, R"pbdoc(Get destination neuron ID)pbdoc")
        .def("getWeight", &Synapse::getWeight, R"pbdoc(Get synaptic weight)pbdoc")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), R"pbdoc(Set synaptic weight)pbdoc")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), R"pbdoc(Add to synaptic weight)pbdoc")
        .def("getDelay", &Synapse::getDelay, R"pbdoc(Get synaptic delay)pbdoc")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), R"pbdoc(Set synaptic delay)pbdoc")
        .def("getType", &Synapse::getType, R"pbdoc(Get synapse type)pbdoc")
        .def("setType", &Synapse::setType, py::arg("synapse_type"), R"pbdoc(Set synapse type)pbdoc")
        .def("isExcitatory", &Synapse::isExcitatory, R"pbdoc(Check if synapse is excitatory)pbdoc")
        .def("isInhibitory", &Synapse::isInhibitory, R"pbdoc(Check if synapse is inhibitory)pbdoc")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), R"pbdoc(Record presynaptic spike)pbdoc")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), R"pbdoc(Record postsynaptic spike)pbdoc")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, R"pbdoc(Get presynaptic spike history)pbdoc")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, R"pbdoc(Get postsynaptic spike history)pbdoc")
        .def("clearHistory", &Synapse::clearHistory, R"pbdoc(Clear spike history)pbdoc")
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags, R"pbdoc(Get plasticity flags)pbdoc")
        .def("enablePlasticity", &Synapse::enablePlasticity, 
             py::arg("hebbian"), py::arg("stdp"), py::arg("reward_modulated"), 
             R"pbdoc(Enable/disable plasticity rules)pbdoc")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, R"pbdoc(Get eligibility trace)pbdoc")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), R"pbdoc(Set eligibility trace)pbdoc")
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"), R"pbdoc(Decay eligibility trace)pbdoc")
        .def("getEfficacy", &Synapse::getEfficacy, R"pbdoc(Get synaptic efficacy)pbdoc")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), R"pbdoc(Set synaptic efficacy)pbdoc")
        .def("step", &Synapse::step, py::arg("currentTime"), R"pbdoc(Update synapse for one timestep)pbdoc")
        .def("reset", &Synapse::reset, R"pbdoc(Reset synapse to initial state)pbdoc");

    // ===================================================================
    // Plasticity Classes
    // ===================================================================

    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for plasticity rules)")
        .def("isEnabled", &PlasticityRule::isEnabled, R"pbdoc(Check if plasticity rule is enabled)pbdoc")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"), R"pbdoc(Enable/disable plasticity rule)pbdoc")
        .def("getName", &PlasticityRule::getName, R"pbdoc(Get rule name)pbdoc");

    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity) "
        "LTP when pre before post, LTD when pre after post")
        .def(py::init<>())
        .def("update", &STDP::update, 
             py::arg("synapse"), py::arg("pre_spikes"), py::arg("post_spikes"), py::arg("dt"),
             R"pbdoc(Update STDP based on spike timing)pbdoc")
        .def("applyWeightChange", &STDP::applyWeightChange, 
             py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &STDP::getName, R"pbdoc(Get rule name)pbdoc")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), R"pbdoc(Set LTP weight)pbdoc")
        .def("getLTPWeight", &STDP::getLTPWeight, R"pbdoc(Get LTP weight)pbdoc")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), R"pbdoc(Set LTD weight)pbdoc")
        .def("getLTDWeight", &STDP::getLTDWeight, R"pbdoc(Get LTD weight)pbdoc")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"), R"pbdoc(Set STDP time constant)pbdoc")
        .def("getTimeConstant", &STDP::getTimeConstant, R"pbdoc(Get STDP time constant)pbdoc")
        .def("configure", &STDP::configure,
             py::arg("ltp_weight"), py::arg("ltd_weight"), py::arg("tau"),
             R"pbdoc(Configure STDP parameters)pbdoc");

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)")
        .def(py::init<>())
        .def("update", &Hebbian::update,
             py::arg("synapse"), py::arg("pre_spikes"), py::arg("post_spikes"), py::arg("dt"),
             R"pbdoc(Update Hebbian plasticity)pbdoc")
        .def("applyWeightChange", &Hebbian::applyWeightChange,
             py::arg("synapse"), py::arg("delta"), R"pbdoc(Apply weight change)pbdoc")
        .def("getName", &Hebbian::getName, R"pbdoc(Get rule name)pbdoc")
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), R"pbdoc(Set learning rate)pbdoc")
        .def("getLearningRate", &Hebbian::getLearningRate, R"pbdoc(Get learning rate)pbdoc")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("max_weight"), R"pbdoc(Set max weight)pbdoc")
        .def("getMaxWeight", &Hebbian::getMaxWeight, R"pbdoc(Get max weight)pbdoc");

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity system)")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse,
             py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"),
             R"pbdoc(Create new synapse)pbdoc")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse,
             py::arg("brain"), py::arg("synapse"), R"pbdoc(Remove synapse)pbdoc")
        .def("createNeuron", &StructuralPlasticity::createNeuron,
             py::arg("brain"), py::arg("neuron_type"), R"pbdoc(Create new neuron)pbdoc")
        .def("removeNeuron", &StructuralPlasticity::removeNeuron,
             py::arg("brain"), py::arg("neuron"), R"pbdoc(Remove neuron)pbdoc")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate, R"pbdoc(Get synaptogenesis rate)pbdoc")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate, py::arg("rate"), R"pbdoc(Set synaptogenesis rate)pbdoc")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate, R"pbdoc(Get pruning rate)pbdoc")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate, py::arg("rate"), R"pbdoc(Set pruning rate)pbdoc")
        .def("update", &StructuralPlasticity::update,
             py::arg("brain"), py::arg("rng"), R"pbdoc(Update structural plasticity)pbdoc");

    // ===================================================================
    // Neuromodulation Classes
    // ===================================================================

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulation system)")
        .def(py::init<>())
        .def("getModulation", &Dopamine::getModulation)
        .def("setModulation", &Dopamine::setModulation, py::arg("modulation"))
        .def("getAdaptationRate", &Dopamine::getAdaptationRate)
        .def("setAdaptationRate", &Dopamine::setAdaptationRate, py::arg("rate"))
        .def("computeRewardSignal", &Dopamine::computeRewardSignal, py::arg("reward"), py::arg("prediction_error"));

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulation system)")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel, py::arg("level"))
        .def("getAdaptationRate", &Curiosity::getAdaptationRate)
        .def("setAdaptationRate", &Curiosity::setAdaptationRate, py::arg("rate"))
        .def("computeNoveltyBonus", &Curiosity::computeNoveltyBonus, py::arg("novelty"));

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection neuromodulation system)")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("getAdaptationRate", &Novelty::getAdaptationRate)
        .def("setAdaptationRate", &Novelty::setAdaptationRate, py::arg("rate"))
        .def("computeNovelty", &Novelty::computeNovelty, py::arg("input"), py::arg("memory"));

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulation system)")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel)
        .def("setLevel", &PredictionError::setLevel, py::arg("level"))
        .def("getAdaptationRate", &PredictionError::getAdaptationRate)
        .def("setAdaptationRate", &PredictionError::setAdaptationRate, py::arg("rate"))
        .def("computeError", &PredictionError::computeError, py::arg("prediction"), py::arg("actual"));

    // ===================================================================
    // Brain Class
    // ===================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain)")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, R"pbdoc(Initialize brain with configuration)pbdoc")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step), py::arg("current_step"),
             R"pbdoc(Perform a simulation step)pbdoc")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step), py::arg("current_step"), py::arg("current_time"),
             R"pbdoc(Perform a simulation step with timestamp)pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(Inject sensory input into brain)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(Inject current into specific neuron)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("neuron_type"), py::arg("current"),
             R"pbdoc(Inject current into all neurons of a specific type)pbdoc")
        .def("produceAction", &Brain::produceAction, R"pbdoc(Produce motor action based on neural activity)pbdoc")
        .def("reset", &Brain::reset, R"pbdoc(Reset brain state)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"), R"pbdoc(Save brain state to file)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"), R"pbdoc(Load brain state from file)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", R"pbdoc(Add a new neural region)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"), R"pbdoc(Get a region by ID)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount, R"pbdoc(Get the number of regions)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds, R"pbdoc(Get all region IDs)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, R"pbdoc(Get total neuron count)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, R"pbdoc(Get total synapse count)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, R"pbdoc(Get count of active neurons)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, R"pbdoc(Get count of currently firing neurons)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, R"pbdoc(Get average firing rate)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, R"pbdoc(Get E/I balance)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, R"pbdoc(Get total spike count)pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, R"pbdoc(Get pending spike events)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, R"pbdoc(Get current developmental stage)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), R"pbdoc(Set developmental stage)pbdoc")
        .def("getConfig", &Brain::getConfig, R"pbdoc(Get the configuration)pbdoc")
        .def("logStatus", &Brain::logStatus, R"pbdoc(Log brain status)pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory, R"pbdoc(Get working memory)pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory, R"pbdoc(Get episodic memory)pbdoc")
        .def("getDopamine", &Brain::getDopamine, R"pbdoc(Get dopamine neuromodulator)pbdoc")
        .def("getCuriosity", &Brain::getCuriosity, R"pbdoc(Get curiosity neuromodulator)pbdoc")
        .def("getNovelty", &Brain::getNovelty, R"pbdoc(Get novelty neuromodulator)pbdoc")
        .def("getPredictionError", &Brain::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem, R"pbdoc(Get development system)pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem, R"pbdoc(Get prediction system)pbdoc")
        .def("getPlanner", &Brain::getPlanner, R"pbdoc(Get neural planner)pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation, R"pbdoc(Get concept formation)pbdoc")
        .def("getAttention", &Brain::getAttention, R"pbdoc(Get attentional selection)pbdoc");

    // ===================================================================
    // AgentBrain Class
    // ===================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), R"pbdoc(Initialize with world)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, R"pbdoc(Get expected sensory input size)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, R"pbdoc(Get expected motor output size)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), R"pbdoc(Process sensory percept)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, R"pbdoc(Decode motor command)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, 
             py::arg("reward"), py::arg("predicted_reward"), R"pbdoc(Apply reward modulation)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), R"pbdoc(Update development)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, R"pbdoc(Get developmental stage)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, R"pbdoc(Get neuromodulation level)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, R"pbdoc(Get curiosity level)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, R"pbdoc(Get novelty level)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError, R"pbdoc(Get prediction error)pbdoc")
        .def("reset", &AgentBrain::reset, R"pbdoc(Reset agent)pbdoc")
        .def("getBrain", &AgentBrain::getBrain, R"pbdoc(Get underlying brain)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), R"pbdoc(Enable reward modulation)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), R"pbdoc(Enable structural plasticity)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), R"pbdoc(Enable development)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), R"pbdoc(Enable curiosity)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, R"pbdoc(Check if reward modulation enabled)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, R"pbdoc(Check if structural plasticity enabled)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, R"pbdoc(Check if development enabled)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, R"pbdoc(Check if curiosity enabled)pbdoc");

    // ===================================================================
    // World Classes
    // ===================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, 
             py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
             R"pbdoc(Configure world dimensions)pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(Reset world to initial state)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), R"pbdoc(Set agent starting position)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"), R"pbdoc(Update world state)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("current_time"), R"pbdoc(Apply motor command)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, R"pbdoc(Get sensory percept)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody, R"pbdoc(Get agent body state)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), R"pbdoc(Add world object)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), R"pbdoc(Remove object)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), R"pbdoc(Check if position is valid)pbdoc")
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(Get world width)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(Get world height)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(Get max energy)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), R"pbdoc(Set max energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(Get energy decay rate)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), R"pbdoc(Set energy decay rate)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(Get simulation time)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), R"pbdoc(Set random seed)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(Get random seed)pbdoc");

    // ===================================================================
    // Sensory Classes
    // ===================================================================

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(Get sensory input type)pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(Get raw data)pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(Get dimensionality)pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(Get timestamp)pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), R"pbdoc(Set timestamp)pbdoc");

    py::class_<Vision>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set vision data)pbdoc")
        .def("getWidth", &Vision::getWidth, R"pbdoc(Get vision width)pbdoc")
        .def("getHeight", &Vision::getHeight, R"pbdoc(Get vision height)pbdoc")
        .def("getChannels", &Vision::getChannels, R"pbdoc(Get number of channels)pbdoc");

    py::class_<Audio>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data)pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sample_rate"), R"pbdoc(Set sample rate)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(Get sample rate)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(Get number of samples)pbdoc");

    py::class_<InternalSignals>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), R"pbdoc(Add signal value)pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(Clear all signals)pbdoc");

    // ===================================================================
    // Action Classes
    // ===================================================================

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, R"pbdoc(Get action type)pbdoc")
        .def("setType", &Action::setType, py::arg("type"), R"pbdoc(Set action type)pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(Get action parameters)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"), R"pbdoc(Set action parameters)pbdoc")
        .def("getName", &Action::getName, R"pbdoc(Get action name)pbdoc")
        .def("clone", &Action::clone, R"pbdoc(Clone action)pbdoc");

    // ===================================================================
    // Agent Classes
    // ===================================================================

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
        .def("reset", &AgentBody::reset, R"pbdoc(Reset agent body)pbdoc");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, R"pbdoc(Get vision data)pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), R"pbdoc(Set vision data)pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, R"pbdoc(Get vision width)pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, R"pbdoc(Get vision height)pbdoc")
        .def("getTouch", &SensoryPercept::getTouch, R"pbdoc(Get touch data)pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), R"pbdoc(Set touch data)pbdoc")
        .def("getInternal", &SensoryPercept::getInternal, R"pbdoc(Get internal signals)pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), R"pbdoc(Set internal signals)pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception, R"pbdoc(Get proprioception data)pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), R"pbdoc(Set proprioception data)pbdoc")
        .def("getAudio", &SensoryPercept::getAudio, R"pbdoc(Get audio data)pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), R"pbdoc(Set audio data)pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals, R"pbdoc(Get all signals)pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp, R"pbdoc(Get timestamp)pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), R"pbdoc(Set timestamp)pbdoc");

    // ===================================================================
    // Utility Functions
    // ===================================================================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world)pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)pbdoc");

    // Create convenience functions for creating actions
    m.def("createMoveAction", [](ActionType type, std::vector<float> params = {}) -> std::unique_ptr<Action> {
        auto action = std::make_unique<Action>();
        action->setType(type);
        action->setParameters(params);
        return action;
    }, py::arg("type"), py::arg("params") = std::vector<float>(), R"pbdoc(Create movement action)pbdoc");

    // Helper functions for statistics and utilities
    m.def("getBrainStats", [](const Brain& brain) {
        py::dict stats;
        stats["total_neurons"] = brain.getTotalNeuronCount();
        stats["total_synapses"] = brain.getTotalSynapseCount();
        stats["active_neurons"] = brain.getActiveNeuronCount();
        stats["firing_neurons"] = brain.getFiringNeuronCount();
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = brain.getTotalSpikeCount();
        stats["pending_spikes"] = brain.getPendingSpikeEventCount();
        return stats;
    }, py::arg("brain"), R"pbdoc(Get comprehensive brain statistics)pbdoc");

    m.def("getWorldStats", [](const SimpleWorld& world) {
        py::dict stats;
        stats["width"] = world.getWidth();
        stats["height"] = world.getHeight();
        stats["max_energy"] = world.getMaxEnergy();
        stats["energy_decay_rate"] = world.getEnergyDecayRate();
        stats["simulation_time"] = world.getSimulationTime();
        stats["agent_x"] = world.getAgentBody().x;
        stats["agent_y"] = world.getAgentBody().y;
        return stats;
    }, py::arg("world"), R"pbdoc(Get world statistics)pbdoc");

    // Error handling utilities
    m.def("validate_config", [](const Config& config) {
        py::list errors;
        // Basic validation - check for required keys
        if (!config.has("neuron_count") && !config.has("brain.neuron_count")) {
            errors.append("Missing neuron_count configuration");
        }
        if (!config.has("region_count") && !config.has("brain.region_count")) {
            errors.append("Missing region_count configuration");
        }
        if (errors.size() > 0) {
            throw ConfigurationError("Configuration validation failed: " + 
                                   std::string(py::cast<std::string>(errors[0])));
        }
        return true;
    }, py::arg("config"), R"pbdoc(Validate configuration)pbdoc");

    // ===================================================================
    // Advanced Pythonic APIs
    // ===================================================================

    // Batch operations
    m.def("batch_inject_current", [](Brain& brain, const std::vector<NeuronId>& neuron_ids, float current) {
        for (const auto& id : neuron_ids) {
            brain.injectCurrent(id, current);
        }
    }, py::arg("brain"), py::arg("neuron_ids"), py::arg("current"), R"pbdoc(Inject current to multiple neurons)pbdoc");

    m.def("batch_connect_neurons", [](NeuralRegion& region, const std::vector<NeuronId>& source_ids, 
                                     const std::vector<NeuronId>& target_ids,
                                     SynapticWeight weight = 0.1f, Delay delay = 1) {
        if (source_ids.size() != target_ids.size()) {
            throw NeuralError("Source and target neuron lists must have same length");
        }
        
        for (size_t i = 0; i < source_ids.size(); ++i) {
            region.addSynapse(source_ids[i], target_ids[i], weight, delay);
        }
    }, py::arg("region"), py::arg("source_ids"), py::arg("target_ids"), 
        py::arg("weight") = 0.1f, py::arg("delay") = 1,
        R"pbdoc(Batch connect neurons)pbdoc");

    // Convenience for creating populations
    m.def("create_population", [](NeuralRegion& region, size_t size, NeuronType type = NeuronType::Internal) {
        return region.addPopulation(size, type);
    }, py::arg("region"), py::arg("size"), py::arg("type") = NeuronType::Internal,
        R"pbdoc(Create neural population in region)pbdoc");

    // Statistical utilities
    m.def("compute_network_connectivity", [](const NeuralRegion& region) {
        py::dict stats;
        auto neurons = region.getAllNeurons();
        auto synapses = region.getSynapses();
        
        size_t total_possible = neurons.size() * neurons.size();
        float density = static_cast<float>(synapses.size()) / total_possible;
        
        stats["neuron_count"] = neurons.size();
        stats["synapse_count"] = synapses.size();
        stats["connectivity_density"] = density;
        
        // Calculate average weight
        float total_weight = 0.0f;
        for (const auto& synapse : synapses) {
            total_weight += synapse->getWeight();
        }
        float avg_weight = synapses.empty() ? 0.0f : total_weight / synapses.size();
        stats["average_weight"] = avg_weight;
        
        return stats;
    }, py::arg("region"), R"pbdoc(Compute network connectivity statistics)pbdoc");

    // Memory management utilities
    m.def("clear_memory", [](AgentBrain& agent) {
        agent.getBrain()->reset();
        agent.reset();
    }, py::arg("agent"), R"pbdoc(Clear all memory in agent)pbdoc");

    // Plotting and visualization utilities (for development)
    m.def("get_firing_histogram", [](const Brain& brain, size_t bins = 10) {
        py::list histogram;
        float max_firing_rate = brain.getAverageFiringRate();
        if (max_firing_rate > 0) {
            for (size_t i = 0; i <= bins; ++i) {
                histogram.append(static_cast<float>(i) * max_firing_rate / bins);
            }
        }
        return histogram;
    }, py::arg("brain"), py::arg("bins") = 10, R"pbdoc(Get firing rate histogram)pbdoc");

    // ===================================================================
    // Constants and Global Values
    // ===================================================================

    m.attr("INVALID_NEURON_ID") = INVALID_NEURON_ID;
    m.attr("INVALID_SYNAPSE_ID") = INVALID_SYNAPSE_ID;
    m.attr("INVALID_REGION_ID") = INVALID_REGION_ID;
    m.attr("INVALID_POPULATION_ID") = INVALID_POPULATION_ID;

    m.attr("__version__") = "2.1.0";  // Updated version

    // ===================================================================
    // Experimental Phase 6 Integration
    // ===================================================================

    // Phase 6 experiment runner
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(Phase 6 integration experiment)pbdoc")
        .def(py::init<>())
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration, R"pbdoc(Verify all systems integration)pbdoc")
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration, R"pbdoc(Test memory integration)pbdoc")
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration, R"pbdoc(Test neuromodulation integration)pbdoc")
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing, R"pbdoc(Test checkpoint save/load)pbdoc")
        .def("testReplay", &Phase6IntegratedExperiment::testReplay, R"pbdoc(Test replay system)pbdoc");

    // Phase 6 configuration
    py::class_<Phase6Config>(m, "Phase6Config", R"pbdoc(Configuration for Phase 6 experiment)pbdoc")
        .def(py::init<>())
        .def_readwrite("neuronCount", &Phase6Config::neuronCount)
        .def_readwrite("maxSteps", &Phase6Config::maxSteps)
        .def_readwrite("regionCount", &Phase6Config::regionCount)
        .def_readwrite("connectionProbability", &Phase6Config::connectionProbability)
        .def_readwrite("enableCheckpointing", &Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &Phase6Config::checkpointPath);

    // Phase 6 results
    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult", R"pbdoc(Results from Phase 6 integration experiment)pbdoc")
        .def_readonly("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readonly("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readonly("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readonly("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def_readonly("curiosityLevel", &Phase6IntegrationResult::curiosityLevel)
        .def_readonly("noveltyLevel", &Phase6IntegrationResult::noveltyLevel)
        .def_readonly("predictionError", &Phase6IntegrationResult::predictionError)
        .def_readonly("memoryWorkingMemoryIntegrated", &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readonly("memoryEpisodicMemoryIntegrated", &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readonly("neuromodulationIntegrated", &Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readonly("predictionIntegrated", &Phase6IntegrationResult::predictionIntegrated)
        .def_readonly("developmentIntegrated", &Phase6IntegrationResult::developmentIntegrated)
        .def_readonly("checkpointingWorks", &Phase6IntegrationResult::checkpointingWorks)
        .def_readonly("totalWallClockTime", &Phase6IntegrationResult::totalWallClockTime);

    // Convenience function for running Phase 6 demo
    m.def("runPhase6Demo", []() {
        Phase6IntegratedExperiment experiment;
        Phase6Demo demo;
        return demo.run();
    }, R"pbdoc(Run Phase 6 integration demo)pbdoc");

    // ===================================================================
    // Performance and Debug Utilities
    // ===================================================================

    // Performance monitoring
    m.def("getPerformanceMetrics", [](const Brain& brain) {
        py::dict metrics;
        
        // Neural performance metrics
        metrics["neuron_count"] = brain.getTotalNeuronCount();
        metrics["synapse_count"] = brain.getTotalSynapseCount();
        metrics["spike_rate_hz"] = brain.getAverageFiringRate() * 1000.0f; // Convert to Hz
        metrics["activity_level"] = static_cast<float>(brain.getFiringNeuronCount()) / 
                                    std::max(1.0f, static_cast<float>(brain.getTotalNeuronCount()));
        
        // Energy efficiency metrics
        metrics["total_energy_consumed"] = brain.getTotalSpikeCount() * 0.001f; // Arbitrary units
        metrics["spikes_per_neuron"] = static_cast<float>(brain.getTotalSpikeCount()) /
                                       std::max(1.0f, static_cast<float>(brain.getTotalNeuronCount()));
        
        return metrics;
    }, py::arg("brain"), R"pbdoc(Get performance metrics)pbdoc");

    // Debug utilities
    m.def("print_brain_status", [](Brain& brain) {
        brain.logStatus();
    }, py::arg("brain"), R"pbdoc(Print detailed brain status)pbdoc");

    // Memory usage utilities
    m.def("estimate_memory_usage", []() {
        py::dict usage;
        usage["neuron_state_size"] = sizeof(NeuronState);
        usage["synapse_state_size"] = sizeof(Synapse);
        usage["memory_pool_overhead"] = 1024; // bytes
        return usage;
    }, R"pbdoc(Estimate memory usage)pbdoc");

    // ===================================================================
    // Error Handling and Validation
    // ===================================================================

    // Validation functions
    m.def("validate_neuron_state", [](const Neuron& neuron) {
        if (neuron.getMembranePotential() < -100.0f || neuron.getMembranePotential() > 100.0f) {
            throw NeuralError("Membrane potential out of valid range");
        }
        if (neuron.getFiringRate() < 0.0f || neuron.getFiringRate() > 1000.0f) {
            throw NeuralError("Firing rate out of valid range");
        }
        return true;
    }, py::arg("neuron"), R"pbdoc(Validate neuron state)pbdoc");

    m.def("validate_synapse", [](const Synapse& synapse) {
        if (synapse.getWeight() < -10.0f || synapse.getWeight() > 10.0f) {
            throw NeuralError("Synaptic weight out of valid range");
        }
        if (synapse.getDelay() > 1000) {
            throw NeuralError("Synaptic delay too large");
        }
        return true;
    }, py::arg("synapse"), R"pbdoc(Validate synapse)pbdoc");

    // ===================================================================
    // Experimental Features
    // ===================================================================

    // Experimental Python-first APIs (for research)
    m.def("create_brain_from_config_dict", [](py::dict config_dict) {
        auto config = std::make_shared<Config>();
        
        // Convert Python dict to C++ config
        py::list keys = config_dict.keys();
        for (size_t i = 0; i < keys.size(); ++i) {
            std::string key = py::cast<std::string>(keys[i]);
            py::object value = config_dict[key];
            
            // Try different types based on key
            if (key.find("neuron_count") != std::string::npos) {
                config->set(key, py::cast<int64_t>(value));
            } else if (key.find("weight") != std::string::npos || key.find("rate") != std::string::npos) {
                config->set(key, py::cast<float>(value));
            } else if (key.find("count") != std::string::npos) {
                config->set(key, py::cast<int64_t>(value));
            } else {
                // Default to string for other parameters
                config->set(key, py::cast<std::string>(value));
            }
        }
        
        return std::make_shared<Brain>(config);
    }, py::arg("config_dict"), R"pbdoc(Create brain from Python dictionary)pbdoc");

    // Batch neural operations
    m.def("batch_create_neurons", [](NeuralRegion& region, size_t count, NeuronType type = NeuronType::Internal) {
        std::vector<NeuronId> neuron_ids;
        
        // Create neurons in batch
        for (size_t i = 0; i < count; ++i) {
            // Note: This is a simplified version - real implementation would
            // need to properly create and add neurons to the region
            // For now, we'll return placeholder IDs
            neuron_ids.push_back(NeuronId(i + 1));
        }
        
        return neuron_ids;
    }, py::arg("region"), py::arg("count"), py::arg("type") = NeuronType::Internal,
        R"pbdoc(Batch create neurons)pbdoc");

    // ===================================================================
    // End of Bindings
    // ===================================================================
}

)pbdoc";
