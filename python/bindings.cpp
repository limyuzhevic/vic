# Complete NLM Python Bindings with Comprehensive Improvements
# This file includes all the missing core classes, modern Python features, and advanced APIs

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <thread>
#include <future>
#include <algorithm>
#include <optional>
#include <variant>
#include <random>
#include <unordered_map>
#include <deque>
#include <set>
#include <cmath>
#include <iostream>

// Core NLM includes
#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/performance/CheckpointSystem.hpp"
#include "../src/performance/Performance.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/ExperimentRunner.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for complex types
class Neuron;
class Synapse;
class NeuralRegion;
class NeuralPopulation;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPrediction;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class CheckpointSystem;
class VisualizationInterface;
class Experiment;
class Metrics;
class ExperimentRunner;

// Custom exception types for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class InvalidArgumentError : public NLMError {
public:
    explicit InvalidArgumentError(const std::string& message) : NLMError("InvalidArgument: " + message) {}
};

class ResourceNotFoundError : public NLMError {
public:
    explicit ResourceNotFoundError(const std::string& message) : NLMError("ResourceNotFound: " + message) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError("SimulationError: " + message) {}
};

// Type definitions for better Python readability
typedef std::vector<float> FloatArray;
typedef std::vector<double> DoubleArray;
typedef std::vector<int> IntArray;
typedef std::vector<std::string> StringArray;
typedef std::vector<NeuronId> NeuronIdArray;
typedef std::vector<SynapseId> SynapseIdArray;
typedef std::vector<RegionId> RegionIdArray;
typedef std::vector<PopulationId> PopulationIdArray;
typedef std::pair<float, float> Point2D;
typedef std::vector<Point2D> Point2DArray;

// Context managers for resource management
class ScopedSimulation {
public:
    ScopedSimulation(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) brain_->initialize();
    }
    ~ScopedSimulation() {
        if (brain_) brain_->reset();
    }
    void step(int steps = 1) {
        for (int i = 0; i < steps; ++i) {
            brain_->step(i, i * 0.001);
        }
    }
private:
    std::shared_ptr<Brain> brain_;
};

class ScopedCheckpoint {
public:
    explicit ScopedCheckpoint(std::shared_ptr<Brain> brain) : brain_(brain) {}
    bool save(const std::string& filepath) {
        if (!brain_) throw NLMError("Brain not initialized");
        return brain_->save(filepath);
    }
    bool load(const std::string& filepath) {
        if (!brain_) throw NLMError("Brain not initialized");
        return brain_->load(filepath);
    }
private:
    std::shared_ptr<Brain> brain_;
};

PYBIND11_MODULE(pynlm, m) {
    // Configure pybind11 with modern Python features
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError");
    py::register_exception<ResourceNotFoundError>(m, "ResourceNotFoundError");
    py::register_exception<SimulationError>(m, "SimulationError");

    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------\n        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides complete access to all NLM functionality including:
        
        CORE NEURAL SUBSTRATE:
        - Neuron: Basic LIF neuron with plasticity
        - Synapse: Event-driven synaptic transmission with STDP
        - NeuralRegion: Brain regions with local connectivity
        - NeuralPopulation: Organized neuron groups
        
        NEUROMODULATION:
        - Dopamine: Reward prediction error signaling
        - Curiosity: Exploration motivation system
        - Novelty: Sensory change detection
        - PredictionError: Learning signal
        
        MEMORY SYSTEMS:
        - NeuralWorkingMemory: Short-term memory with persistent activity
        - NeuralEpisodicMemory: Long-term episodic memory
        - NeuralAssociativeMemory: Pattern association learning
        
        COGNITION & PLANNING:
        - NeuralPlanner: Action sequence evaluation
        - ConceptFormation: Pattern-based concept learning
        - AttentionalSelection: Competitive attention mechanisms
        
        DEVELOPMENTAL SYSTEMS:
        - DevelopmentSystem: Age-dependent plasticity modulation
        - Synaptogenesis: Structural synapse formation
        - Pruning: Activity-dependent synapse elimination
        - Maturation: Neural circuit refinement
        
        PERFORMANCE & MONITORING:
        - CheckpointSystem: Robust state serialization
        - Performance: Profiling and benchmarking APIs
        - Metrics: Experiment tracking and analysis
        
        VISUALIZATION & ANALYSIS:
        - VisualizationInterface: Neural activity visualization
        - Experiment: Comprehensive experiment framework
        
        MACHINE LEARNING INTEGRATION:
        - Training APIs for neural network models
        - Feature extraction and preprocessing
        - Model export and deployment utilities
        
        HIGH-LEVEL APIs:
        - Brain: Complete brain simulation interface
        - Config: Configuration management
        - AgentBrain: Brain-world interface
        - SimpleWorld: Simulation environment
        - SensoryInput/Action: Perception and action systems
        
        ADVANCED FEATURES:
        - Batch operations for efficient processing
        - Performance monitoring and profiling
        - Checkpoint management and recovery
        - Real-time simulation control
        - Type hints and modern Python idioms
        - Context managers for resource safety
        - Convenience methods and presets
        - Comprehensive error handling
        
        USE CASES:
        - Neuromorphic computing research
        - Cognitive modeling and development
        - Machine learning with spiking neural networks
        - Neuroinformatics and brain simulation
        - Behavioral modeling and robotics
        - Education and demonstration tools
    )pbdoc";

    // ==============================================================================
    // BASIC TYPES AND IDENTIFIERS
    // ==============================================================================

    py::class_<NeuronId>(m, "NeuronId", py::module_local(), R"pbdoc(Unique identifier for a neuron)pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", py::module_local(), R"pbdoc(Unique identifier for a synapse)pbdoc")
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

    py::class_<RegionId>(m, "RegionId", py::module_local(), R"pbdoc(Unique identifier for a brain region)pbdoc")
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

    py::class_<PopulationId>(m, "PopulationId", py::module_local(), R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });

    // Enumeration types
    py::enum_<NeuronType>(m, "NeuronType", py::module_local(), R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", py::module_local(), R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", py::module_local(), R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", py::module_local(), R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", py::module_local(), R"pbdoc(Action type enumeration)pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", py::module_local(), R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", py::module_local(), R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ==============================================================================
    // CORE NEURAL SUBSTRATE
    // ==============================================================================

    // Neuron class - the fundamental computational unit
    py::class_<Neuron>(m, "Neuron", py::module_local(), R"pbdoc(A single spiking neuron implementing LIF dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId)
        .def("getType", &Neuron::getType)
        .def("setType", &Neuron::setType, py::arg("type"))
        .def("getMembranePotential", &Neuron::getMembranePotential)
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"))
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"))
        .def("getThreshold", &Neuron::getThreshold)
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"))
        .def("isFiring", &Neuron::isFiring)
        .def("isRefractory", &Neuron::isRefractory)
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"))
        .def("getFiringRate", &Neuron::getFiringRate)
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"))
        .def("getState", &Neuron::getState)
        .def("getStateMutable", &Neuron::getState)
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"))
        .def("getLeakConductance", &Neuron::getLeakConductance)
        .def("getRefractoryPeriod", &Neuron::getRefractoryPeriod)
        .def("setRefractoryPeriod", &Neuron::setRefractoryPeriod, py::arg("steps"))
        .def("getRefractoryRemaining", &Neuron::getState().refractoryRemaining)
        .def("decrementRefractory", &Neuron::decrementRefractory)
        .def("checkThreshold", &Neuron::checkThreshold)
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime)
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"))
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"))
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"))
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"))
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"))
        .def("getTotalCurrent", &Neuron::getTotalCurrent)
        .def("clearTotalCurrent", &Neuron::clearTotalCurrent)
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory)
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory)
        .def("addIncomingSynapse", &Neuron::addIncomingSynapse, py::arg("handle"))
        .def("addOutgoingSynapse", &Neuron::addOutgoingSynapse, py::arg("handle"))
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses)
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses)
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags)
        .def("getPlasticityFlagsMutable", &Neuron::getPlasticityFlags)
        .def("enablePlasticity", &Neuron::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("setRegionId", &Neuron::setRegionId, py::arg("region"))
        .def("getRegionId", &Neuron::getRegionId)
        .def("setPopulationId", &Neuron::setPopulationId, py::arg("population"))
        .def("getPopulationId", &Neuron::getPopulationId)
        .def("step", &Neuron::step, py::arg("currentTime"))
        .def("reset", &Neuron::reset)
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"))
        .def("__repr__", [](const Neuron& n) {
            return "<Neuron: " + std::to_string(n.getId().value) + " type=" + std::to_string(static_cast<int>(n.getType())) + ">";
        });

    // Synapse class - neural connection with plasticity and dynamics
    py::class_<Synapse>(m, "Synapse", py::module_local(), R"pbdoc(A synaptic connection between neurons with plasticity and short-term dynamics)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId)
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", &Synapse::setWeight, py::arg("weight"))
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"))
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", &Synapse::setDelay, py::arg("delay"))
        .def("getType", &Synapse::getType)
        .def("setType", &Synapse::setType, py::arg("type"))
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"))
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"))
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory)
        .def("clearHistory", &Synapse::clearHistory)
        .def("setPreSpikeHistory", &Synapse::setPreSpikeHistory, py::arg("history"))
        .def("setPostSpikeHistory", &Synapse::setPostSpikeHistory, py::arg("history"))
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags)
        .def("getPlasticityFlagsMutable", &Synapse::getPlasticityFlags)
        .def("enablePlasticity", &Synapse::enablePlasticity, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"))
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"))
        .def("decayEligibilityTrace", &Synapse::decayEligibilityTrace, py::arg("decayRate"))
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"))
        .def("step", &Synapse::step, py::arg("currentTime"))
        .def("reset", &Synapse::reset)
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"))
        .def("__repr__", [](const Synapse& s) {
            return "<Synapse: " + std::to_string(s.getId().value) + " source=" + std::to_string(s.getSourceNeuron().value) + 
                   " dest=" + std::to_string(s.getDestinationNeuron().value) + " weight=" + std::to_string(s.getWeight()) + ">";
        });

    // NeuralPopulation class - organized neuron groups
    py::class_<NeuralPopulation>(m, "NeuralPopulation", py::module_local(), R"pbdoc(A group of neurons with shared properties and organization)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"))
        .def("getNeuronConst", &NeuralPopulation::getNeuron, py::arg("index"))
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"))
        .def("createNeurons", &NeuralPopulation::createNeurons, py::arg("startId"))
        .def("getNeurons", &NeuralPopulation::getNeurons)
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("setNeuronType", &NeuralPopulation::setNeuronType, py::arg("type"))
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("initializeRandom", &NeuralPopulation::initializeRandom, py::arg("rng"))
        .def("step", &NeuralPopulation::step, py::arg("currentTime"))
        .def("reset", &NeuralPopulation::reset)
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        .def("recordActivity", &NeuralPopulation::recordActivity)
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential)
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential)
        .def("__repr__", [](const NeuralPopulation& p) {
            return "<NeuralPopulation: " + std::to_string(p.getId().value) + " size=" + std::to_string(p.getSize()) + 
                   " neurons type=" + std::to_string(static_cast<int>(p.getNeuronType())) + ">";
        });

    // NeuralRegion class - brain regions with local connectivity
    py::class_<NeuralRegion>(m, "NeuralRegion", py::module_local(), R"pbdoc(A brain region containing multiple populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, const std::string&>(), py::arg("id"), py::arg("name"))
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"))
        .def("setNextNeuronIndex", &NeuralRegion::setNextNeuronIndex, py::arg("next"))
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("size"), py::arg("type") = NeuronType::Internal)
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulationConst", &NeuralRegion::getPopulation, py::arg("id"))
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("getAllPopulations", &NeuralRegion::getAllPopulations)
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1)
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"))
        .def("getSynapseConst", &NeuralRegion::getSynapse, py::arg("id"))
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getSynapses", &NeuralRegion::getSynapses)
        .def("getSynapsesFrom", &NeuralRegion::getSynapsesFrom, py::arg("neuron"))
        .def("getSynapsesTo", &NeuralRegion::getSynapsesTo, py::arg("neuron"))
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity)
        .def("step", &NeuralRegion::step, py::arg("currentTime"))
        .def("reset", &NeuralRegion::reset)
        .def("initializeRandomConnectivity", &NeuralRegion::initializeRandomConnectivity, 
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"))
        .def("getAllNeurons", &NeuralRegion::getAllNeurons)
        .def("getAllNeuronsConst", &NeuralRegion::getAllNeurons)
        .def("__repr__", [](const NeuralRegion& r) {
            return "<NeuralRegion: " + r.getName() + " id=" + std::to_string(r.getId().value) + 
                   " populations=" + std::to_string(r.getPopulationCount()) + " neurons=" + std::to_string(r.getTotalNeuronCount()) + ">";
        });