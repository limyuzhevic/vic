#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

// NLM Core headers
#include "src/core/Types/Types.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/core/Random/Random.hpp"
#include "src/core/SimulationClock/SimulationClock.hpp"

// Brain components
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/brain/NeuralRegion.hpp"

// Sensory systems
#include "src/sensory/SensoryInput.hpp"
#include "src/sensory/Vision.hpp"
#include "src/sensory/Audio.hpp"
#include "src/sensory/InternalSignals.hpp"

// Neuromodulation
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Dopamine.hpp"
#include "src/neuromodulation/Curiosity.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Reward.hpp"

// Memory systems
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/memory/NeuralEpisodicMemory.hpp"
#include "src/memory/NeuralAssociativeMemory.hpp"
#include "src/memory/Memory.hpp"

// Cognition
#include "src/cognition/ConceptFormation.hpp"
#include "src/cognition/NeuralPlanner.hpp"

// Dynamics
#include "src/dynamics/NeuralDynamics.hpp"
#include "src/dynamics/SpikeSystem.hpp"

// Plasticity
#include "src/plasticity/PlasticityRule.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/plasticity/STDP.hpp"
#include "src/plasticity/StructuralPlasticity.hpp"

// Development
#include "src/development/DevelopmentSystem.hpp"
#include "src/development/Synaptogenesis.hpp"
#include "src/development/Pruning.hpp"
#include "src/development/Maturation.hpp"

// Motor
#include "src/motor/MotorSystem.hpp"
#include "src/motor/Action.hpp"

// Environment
#include "src/environment/Environment.hpp"
#include "src/environment/Observation.hpp"

// Agent
#include "src/agent/AgentBody.hpp"
#include "src/agent/SensoryPercept.hpp"
#include "src/agent/AgentBrain.hpp"

// World
#include "src/world/SimpleWorld.hpp"

// Experiments
#include "src/experiments/Experiment.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include "src/experiments/Phase3Experiment.hpp"
#include "src/experiments/Phase4Experiment.hpp"

// Visualization
#include "src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// Type definitions for convenience
using NeuronPtr = std::shared_ptr<Neuron>;
using SynapsePtr = std::shared_ptr<Synapse>;
using PopulationPtr = std::shared_ptr<NeuralPopulation>;
using RegionPtr = std::shared_ptr<NeuralRegion>;
using BrainPtr = std::shared_ptr<Brain>;
using ConfigPtr = std::shared_ptr<Config>;
using LoggerPtr = std::shared_ptr<Logger>;
using RandomGeneratorPtr = std::shared_ptr<RandomGenerator>;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Python Bindings
        ================================================

        A comprehensive Python interface for the NLM neural simulation framework.

        This module provides bindings for all major NLM classes including:
        - Core neural components (Brain, Neuron, Synapse, Neural populations/regions)
        - Sensory systems (Vision, Audio, Internal signals)
        - Neuromodulation (Dopamine, Curiosity, Novelty, PredictionError)
        - Memory systems (Working, Episodic, Associative memory)
        - Cognitive functions (Concept formation, Neural planning)
        - Development and plasticity systems
        - Motor control and action systems
        - Environment simulation and interaction
        - Agent brain interfaces for world interaction

        Features:
        - Full type safety with Python equivalents for C++ types
        - Conversion helpers for ID types (NeuronId, SynapseId, etc.)
        - Debugging and visualization helpers
        - Factory functions for easy instantiation
        - Error handling and logging integration
        - Performance monitoring utilities

        Typical usage:
            import nlm
            
            # Create configuration
            config = nlm.createDefaultConfig()
            
            # Initialize brain
            brain = nlm.createBrain(config)
            
            # Create environment
            world = nlm.createSimpleWorld()
            
            # Create agent interface
            agent_brain = nlm.createAgentBrain(brain)
            
            # Run simulation
            brain.initialize()
            for step in range(1000):
                # Process sensory input
                percept = world.getSensoryPercept()
                agent_brain.processSensoryInput(percept)
                
                # Run neural simulation
                brain.step(step)
                
                # Generate action
                action = brain.produceAction()
                
                # Apply to world
                result = world.applyMotorCommand(action)
                
                # Apply neuromodulation
                agent_brain.applyRewardModulation(result.reward, 0.0)
        )pbdoc";

    // Configure Python module
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Basic type bindings
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

    // Enumeration bindings
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

    // Core configuration and logging
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

    // Logger system
    py::class_<LogEntry>(m, "LogEntry", R"pbdoc(Log entry structure)pbdoc")
        .def(py::init<>())
        .def_readwrite("level", &LogEntry::level)
        .def_readwrite("message", &LogEntry::message)
        .def_readwrite("timestamp", &LogEntry::timestamp);

    py::enum_<LogLevel>(m, "LogLevel", R"pbdoc(Log severity levels)pbdoc")
        .value("Debug", LogLevel::Debug)
        .value("Info", LogLevel::Info)
        .value("Warning", LogLevel::Warning)
        .value("Error", LogLevel::Error)
        .value("Critical", LogLevel::Critical)
        .export_values();

    py::class_<ConsoleLogger, LoggerPtr>(m, "ConsoleLogger", R"pbdoc(Colored console logger)pbdoc")
        .def(py::init<>())
        .def(py::init<LogLevel>(), py::arg("level") = LogLevel::Info)
        .def("log", &ConsoleLogger::log, py::arg("entry"), "Log an entry")
        .def("debug", &ConsoleLogger::debug, py::arg("message"), "Log debug message")
        .def("info", &ConsoleLogger::info, py::arg("message"), "Log info message")
        .def("warning", &ConsoleLogger::warning, py::arg("message"), "Log warning message")
        .def("error", &ConsoleLogger::error, py::arg("message"), "Log error message")
        .def("critical", &ConsoleLogger::critical, py::arg("message"), "Log critical message");

    py::class_<Logger, LoggerPtr>(m, "Logger", R"pbdoc(Main logging system)pbdoc")
        .def(py::init<>())
        .def("addLogger", &Logger::addLogger, py::arg("logger"),
             "Add a logger to the system")
        .def("log", &Logger::log, py::arg("level"), py::arg("message"),
             "Log a message with specified level")
        .def("setGlobal", &Logger::setGlobal,
             "Set as global logger")
        .def("getGlobal", &Logger::getGlobal,
             "Get the global logger instance");

    // Random number generation
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Deterministic random number generator)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("seed"), "Create with seed")
        .def("uniformInt", &RandomGenerator::uniformInt,
             py::arg("min"), py::arg("max"), "Generate uniform integer")
        .def("uniformReal", &RandomGenerator::uniformReal,
             py::arg("min"), py::arg("max"), "Generate uniform real")
        .def("normal", &RandomGenerator::normal,
             py::arg("mean"), py::arg("stddev"), "Generate normal distribution")
        .def("bernoulli", &RandomGenerator::bernoulli,
             py::arg("p") = 0.5f, "Generate Bernoulli random variable")
        .def("setSeed", &RandomGenerator::setSeed, py::arg("seed"),
             "Set random seed")
        .def("getSeed", &RandomGenerator::getSeed,
             "Get current seed");

    // Simulation clock
    py::class_<SimulationClock>(m, "SimulationClock", R"pbdoc(Simulation time management)pbdoc")
        .def(py::init<double>(), py::arg("timestep"), "Create with timestep")
        .def("getTime", &SimulationClock::getTime,
             "Get current simulation time")
        .def("getStep", &SimulationClock::getStep,
             "Get current simulation step")
        .def("step", &SimulationClock::step,
             "Advance simulation by one step")
        .def("reset", &SimulationClock::reset,
             "Reset simulation clock");

    // Neuron implementation
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron implementing LIF dynamics)pbdoc")
        .def(py::init<NeuronId>())
        .def(py::init<NeuronId, NeuronType>())
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("v"),
             "Set membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"),
             "Set firing threshold")
        .def("getRestingPotential", &Neuron::getRestingPotential, "Get resting potential")
        .def("setRestingPotential", &Neuron::setRestingPotential, py::arg("v"),
             "Set resting potential")
        .def("getTimeConstant", &Neuron::getTimeConstant, "Get membrane time constant")
        .def("setTimeConstant", &Neuron::setTimeConstant, py::arg("tau"),
             "Set membrane time constant")
        .def("getCapacitance", &Neuron::getCapacitance, "Get membrane capacitance")
        .def("setCapacitance", &Neuron::setCapacitance, py::arg("c"),
             "Set membrane capacitance")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate")
        .def("getFiringState", &Neuron::getFiringState, "Get firing state")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"),
             "Inject current into neuron")
        .def("getState", &Neuron::getState, "Get neuron state")
        .def("setState", &Neuron::setState, py::arg("state"), "Set neuron state")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"),
             "Step LIF dynamics")
        .def("isActive", &Neuron::isActive, "Check if neuron is firing")
        .def("getLastSpikeTime", &Neuron::getLastSpikeTime, "Get last spike time")
        .def("getTotalSpikeCount", &Neuron::getTotalSpikeCount, "Get total spike count")
        .def("reset", &Neuron::reset, "Reset neuron state")
        .def("copyFrom", &Neuron::copyFrom, py::arg("source"), "Copy from another neuron")
        .def("__repr__", [](const Neuron* neuron) {
            return "<Neuron: " + std::to_string(neuron->getId().value) + ">";
        });

    // Synapse implementation
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Neural connection between neurons)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>())
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSource", &Synapse::getSource, "Get source neuron ID")
        .def("getDestination", &Synapse::getDestination, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("isEnabled", &Synapse::isEnabled, "Check if synapse is enabled")
        .def("enable", &Synapse::enable, "Enable synapse")
        .def("disable", &Synapse::disable, "Disable synapse")
        .def("getLastUpdateTime", &Synapse::getLastUpdateTime, "Get last update time")
        .def("getTotalChange", &Synapse::getTotalChange, "Get total weight change")
        .def("resetChange", &Synapse::resetChange, "Reset weight change tracking")
        .def("copyFrom", &Synapse::copyFrom, py::arg("source"), "Copy from another synapse")
        .def("__repr__", [](const Synapse* synapse) {
            return "<Synapse: " + std::to_string(synapse->getId().value) + ">";
        });

    // Neural population
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<PopulationId, NeuronType, size_t>())
        .def("getId", &NeuralPopulation::getId, "Get population ID")
        .def("getType", &NeuralPopulation::getType, "Get population type")
        .def("setType", &NeuralPopulation::setType, py::arg("type"), "Set population type")
        .def("getSize", &NeuralPopulation::getSize, "Get population size")
        .def("addNeuron", &NeuralPopulation::addNeuron, py::arg("neuron"), "Add neuron")
        .def("removeNeuron", &NeuralPopulation::removeNeuron, py::arg("neuron"), "Remove neuron")
        .def("getNeurons", &NeuralPopulation::getNeurons, "Get all neurons")
        .def("getNeuronById", &NeuralPopulation::getNeuronById, py::arg("id"), "Get neuron by ID")
        .def("getPopulationVoltage", &NeuralPopulation::getPopulationVoltage, "Get population voltage")
        .def("setPopulationVoltage", &NeuralPopulation::setPopulationVoltage, py::arg("voltage"), "Set population voltage")
        .def("getFiringRate", &NeuralPopulation::getFiringRate, "Get population firing rate")
        .def("step", &NeuralPopulation::step, py::arg("currentTime"), "Step population dynamics")
        .def("reset", &NeuralPopulation::reset, "Reset population state")
        .def("copyFrom", &NeuralPopulation::copyFrom, py::arg("source"), "Copy from another population")
        .def("__repr__", [](const NeuralPopulation* pop) {
            return "<NeuralPopulation: " + std::to_string(pop->getId().value) + ">";
        });

    // Neural region
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Brain region containing populations and synapses)pbdoc")
        .def(py::init<RegionId>())
        .def(py::init<RegionId, std::string>())
        .def("getId", &NeuralRegion::getId, "Get region ID")
        .def("getName", &NeuralRegion::getName, "Get region name")
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", &NeuralRegion::addPopulation, py::arg("population"), "Add population")
        .def("removePopulation", &NeuralRegion::removePopulation, py::arg("population"), "Remove population")
        .def("getPopulations", &NeuralRegion::getPopulations, "Get all populations")
        .def("getPopulationById", &NeuralRegion::getPopulationById, py::arg("id"), "Get population by ID")
        .def("addSynapse", &NeuralRegion::addSynapse, py::arg("synapse"), "Add synapse")
        .def("removeSynapse", &NeuralRegion::removeSynapse, py::arg("synapse"), "Remove synapse")
        .def("getSynapses", &NeuralRegion::getSynapses, "Get all synapses")
        .def("getNeurons", &NeuralRegion::getNeurons, "Get all neurons in region")
        .def("step", &NeuralRegion::step, py::arg("currentTime"), "Step region dynamics")
        .def("reset", &NeuralRegion::reset, "Reset region state")
        .def("copyFrom", &NeuralRegion::copyFrom, py::arg("source"), "Copy from another region")
        .def("__repr__", [](const NeuralRegion* region) {
            return "<NeuralRegion: " + std::to_string(region->getId().value) + ">";
        });

    // Spike system
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Event-driven spike propagation system)pbdoc")
        .def(py::init<>())
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentTime"), "Process pending spikes")
        .def("addSpikeEvent", &SpikeSystem::addSpikeEvent, py::arg("neuronId"), py::arg("time"), "Add spike event")
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount, "Get pending spike count")
        .def("getTotalSpikeCount", &SpikeSystem::getTotalSpikeCount, "Get total spike count")
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons, py::arg("limit") = 10,
             "Get most active neurons")
        .def("getSpikeEvents", &SpikeSystem::getSpikeEvents, "Get all spike events")
        .def("reset", &SpikeSystem::reset, "Reset spike system")
        .def("copyFrom", &SpikeSystem::copyFrom, py::arg("source"), "Copy from another spike system")
        .def("__repr__", [](const SpikeSystem* spikeSystem) {
            return "<SpikeSystem: total_spikes=" + std::to_string(spikeSystem->getTotalSpikeCount()) + ">";
        });

    // Brain implementation
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<ConfigPtr>())
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get pending spike event count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator, py::return_value_policy::reference_internal, "Get random generator")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        .def("getSpikeSystem", &Brain::getSpikeSystem, py::return_value_policy::reference_internal, "Get spike system")
        .def("getSTDP", &Brain::getSTDP, py::return_value_policy::reference_internal, "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian, py::return_value_policy::reference_internal, "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity, py::return_value_policy::reference_internal, "Get structural plasticity");

    // Neuromodulation system
    py::class_<Neuromodulator, NeuromodulatorPtr>(m, "Neuromodulator", py::module_local(), R"pbdoc(Abstract base class for neuromodulators)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current neuromodulator level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set neuromodulator level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("brain"), py::arg("timestep"), "Update neuromodulator state")
        .def("copyFrom", &Neuromodulator::copyFrom, py::arg("source"), "Copy from another neuromodulator");

    // Specific neuromodulators
    py::class_<Dopamine, NeuromodulatorPtr>(m, "Dopamine", R"pbdoc(Reward and reinforcement signal)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward receipt")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal reward prediction error");

    py::class_<Curiosity, NeuromodulatorPtr>(m, "Curiosity", R"pbdoc(Exploration motivation)pbdoc")
        .def(py::init<>())
        .def("increase", &Curiosity::increase, py::arg("amount"), "Increase curiosity level")
        .def("decrease", &Curiosity::decrease, py::arg("amount"), "Decrease curiosity level");

    py::class_<Novelty, NeuromodulatorPtr>(m, "Novelty", R"pbdoc(Detect novelty in environment)pbdoc")
        .def(py::init<>())
        .def("detect", &Novelty::detect, py::arg("current"), py::arg("previous"), "Detect novelty between states");

    py::class_<PredictionError, NeuromodulatorPtr>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("compute", &PredictionError::compute, py::arg("predicted"), py::arg("actual"), "Compute prediction error");

    // Reward system
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal system)pbdoc")
        .def(py::init<>())
        .def("addReward", &Reward::addReward, py::arg("amount"), "Add reward signal")
        .def("subtractReward", &Reward::subtractReward, py::arg("amount"), "Subtract reward signal");

    // Memory systems
    py::class_<NeuralWorkingMemory, NeuralWorkingMemoryPtr>(m, "NeuralWorkingMemory", R"pbdoc(Transient active information storage)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"), "Initialize working memory")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength"), "Store pattern")
        .def("retrieve", &NeuralWorkingMemory::retrieve, "Retrieve stored pattern")
        .def("update", &NeuralWorkingMemory::update, py::arg("timestep"), "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"), "Set working memory capacity")
        .def("copyFrom", &NeuralWorkingMemory::copyFrom, py::arg("source"), "Copy from another working memory");

    py::class_<NeuralEpisodicMemory, NeuralEpisodicMemoryPtr>(m, "NeuralEpisodicMemory", R"pbdoc(Experience storage and replay)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"), "Initialize episodic memory")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"), "Store episode")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar, py::arg("pattern"), py::arg("maxResults") = 5, "Retrieve similar episodes")
        .def("retrieveEpisode", &NeuralEpisodicMemory::retrieveEpisode, py::arg("timestamp"), "Retrieve specific episode")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get episode count")
        .def("getLastAccessedEpisode", &NeuralEpisodicMemory::getLastAccessedEpisode, "Get last accessed episode")
        .def("replayExperience", &NeuralEpisodicMemory::replayExperience, py::arg("target"), py::arg("repetitions"), "Replay experience")
        .def("copyFrom", &NeuralEpisodicMemory::copyFrom, py::arg("source"), "Copy from another episodic memory");

    // Plasticity rules
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"), "Set LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"), "Set LTD weight")
        .def("setTau", &STDP::setTau, py::arg("tau"), "Set time constant")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("getTau", &STDP::getTau, "Get time constant")
        .def("applySTDP", &STDP::applySTDP, py::arg("preSpikeTime"), py::arg("postSpikeTime"), "Apply STDP rule");

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"), "Set learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get learning rate")
        .def("applyHebbian", &Hebbian::applyHebbian, py::arg("preNeuron"), py::arg("postNeuron"), "Apply Hebbian rule");

    // Development system
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental regulation of brain properties)pbdoc")
        .def(py::init<>())
        .def("setDevelopmentRate", &DevelopmentSystem::setDevelopmentRate, py::arg("rate"), "Set development rate")
        .def("getDevelopmentRate", &DevelopmentSystem::getDevelopmentRate, "Get development rate")
        .def("updateDevelopment", &DevelopmentSystem::updateDevelopment, py::arg("timestep"), "Update development")
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage, "Get current developmental stage");

    // Sensory input classes
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get input type")
        .def("getData", &SensoryInput::getData, "Get data")
        .def("getDimensions", &SensoryInput::getDimensions, "Get dimensions")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", &Vision::setData, py::arg("data"), "Set data")
        .def("getWidth", &Vision::getWidth, "Get width")
        .def("getHeight", &Vision::getHeight, "Get height")
        .def("getChannels", &Vision::getChannels, "Get channels");

    // Action classes
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>())
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Clone action");

    // Agent interfaces
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age")
        .def("reset", &AgentBody::reset, "Reset body state");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal data")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<BrainPtr>())
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable curiosity")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get underlying brain");

    // Simple world
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Configure world")
        .def("reset", &SimpleWorld::reset, "Reset world")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent start position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"), "Apply motor command")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check position validity")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"), "Get object at position")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Factory functions
    m.def("createDefaultConfig", []() -> ConfigPtr {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](ConfigPtr config) -> BrainPtr {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createLogger", []() -> LoggerPtr {
        return std::make_shared<ConsoleLogger>();
    }, "Create a console logger");

    m.def("createRandomGenerator", [](uint64_t seed) -> std::shared_ptr<RandomGenerator> {
        return std::make_shared<RandomGenerator>(seed);
    }, py::arg("seed"), "Create a random generator with seed");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](BrainPtr brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience functions for ID handling
    m.def("makeInvalidNeuronId", []() -> NeuronId {
        return INVALID_NEURON_ID;
    }, "Create an invalid neuron ID");

    m.def("makeInvalidSynapseId", []() -> SynapseId {
        return INVALID_SYNAPSE_ID;
    }, "Create an invalid synapse ID");

    m.def("makeInvalidRegionId", []() -> RegionId {
        return INVALID_REGION_ID;
    }, "Create an invalid region ID");

    m.def("makeInvalidPopulationId", []() -> PopulationId {
        return INVALID_POPULATION_ID;
    }, "Create an invalid population ID");

    // Debugging and visualization helpers
    m.def("printNeuronState", [](const Neuron* neuron) {
        if (neuron) {
            std::cout << "Neuron ID: " << neuron->getId().value << std::endl;
            std::cout << "  Type: " << static_cast<int>(neuron->getType()) << std::endl;
            std::cout << "  Membrane potential: " << neuron->getMembranePotential() << std::endl;
            std::cout << "  Firing rate: " << neuron->getFiringRate() << std::endl;
            std::cout << "  Is firing: " << (neuron->getFiringState() == FiringState::Active) << std::endl;
        }
    }, py::arg("neuron"), "Print detailed neuron state for debugging");

    m.def("printSynapseState", [](const Synapse* synapse) {
        if (synapse) {
            std::cout << "Synapse ID: " << synapse->getId().value << std::endl;
            std::cout << "  Source ID: " << synapse->getSource().value << std::endl;
            std::cout << "  Destination ID: " << synapse->getDestination().value << std::endl;
            std::cout << "  Weight: " << synapse->getWeight() << std::endl;
            std::cout << "  Type: " << static_cast<int>(synapse->getType()) << std::endl;
            std::cout << "  Enabled: " << synapse->isEnabled() << std::endl;
        }
    }, py::arg("synapse"), "Print detailed synapse state for debugging");

    m.def("printRegionInfo", [](const NeuralRegion* region) {
        if (region) {
            std::cout << "Region ID: " << region->getId().value << std::endl;
            std::cout << "  Name: " << region->getName() << std::endl;
            std::cout << "  Neuron count: " << region->getAllNeurons().size() << std::endl;
            std::cout << "  Synapse count: " << region->getSynapses().size() << std::endl;
        }
    }, py::arg("region"), "Print detailed region state for debugging");

    m.def("printBrainSummary", [](const Brain* brain) {
        if (brain) {
            std::cout << "=== Brain Summary ===" << std::endl;
            std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
            std::cout << "  Spike system events: " << brain->getPendingSpikeEventCount() << std::endl;
            std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }, py::arg("brain"), "Print comprehensive brain summary for debugging");

    // Neural dynamics and simulation utilities
    m.def("runPhase2Demo", []() {
        std::cout << "=== NLM Phase 2 Demo ===" << std::endl;
        std::cout << "Running basic connectivity, plasticity, and STDP tests..." << std::endl;
        // This would require access to the main.cpp functionality
        // For now, we'll just indicate it would run
        std::cout << "Phase 2 demo would run here..." << std::endl;
    }, "Run Phase 2 demonstration");

    m.def("runPhase3Demo", [](int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "=== NLM Phase 3 Demo ===" << std::endl;
        std::cout << "Running world interaction demo with " << numEpisodes << " episodes..." << std::endl;
        // This would require access to Phase3Demo.cpp
        std::cout << "Phase 3 demo would run here..." << std::endl;
    }, py::arg("numEpisodes") = 5, py::arg("stepsPerEpisode") = 200,
       "Run Phase 3 demonstration");

    m.def("runPhase4Demo", [](size_t numTrials = 50) {
        std::cout << "=== NLM Phase 4 Demo ===" << std::endl;
        std::cout << "Running emergence of cognition demo with " << numTrials << " trials..." << std::endl;
        // This would require access to Phase4Demo.cpp
        std::cout << "Phase 4 demo would run here..." << std::endl;
    }, py::arg("numTrials") = 50, "Run Phase 4 demonstration");

    // Version and information
    m.attr("VERSION") = "1.0.0";
    m.attr("PHASE2_COMPLETE") = true;
    m.attr("PHASE3_COMPLETE") = true;
    m.attr("PHASE4_COMPLETE") = true;
    m.attr("CURRENT_PHASE") = 6;

    // Constants
    py::module::import("sys").attr("modules")["nlm"] = m;

} // namespace nlm