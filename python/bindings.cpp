#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

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

// Helper function to convert Python exception to string
std::string pyExceptionToString(const py::error_already_set& e) {
    return std::string("Python error: ") + e.what();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides Python interfaces to create, configure, and run
        neural networks that can learn, remember, and make decisions.
        
        Key Features:
        - Create and configure brains with multiple neuron types
        - Implement sensory perception and motor control
        - Enable learning through multiple plasticity rules (STDP, Hebbian)
        - Support memory systems (working, episodic, associative)
        - Neuromodulation for reward-based learning
        - Developmental stages affecting plasticity
        - Checkpoint save/load for persistent simulations
        - Integration with environments and agents
        
        The framework simulates spiking neural networks using biologically
        inspired dynamics and learning rules.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // NeuronId - unique identifier for a neuron
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron
        
        Attributes:
            value (int): Internal numeric value
        
        Note:
            NeuronId is a wrapper around uint64_t with additional methods
            for indexing and comparison.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get index for population lookup")
        .def("__eq__", &NeuronId::operator==, "Equality comparison")
        .def("__ne__", &NeuronId::operator!=, "Inequality comparison")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // SynapseId - unique identifier for a synapse
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse
        
        Attributes:
            value (int): Internal numeric value
        )pbdoc")
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

    // RegionId - unique identifier for a brain region
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region
        
        Attributes:
            value (int): Internal numeric value
        )pbdoc")
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

    // PopulationId - unique identifier for a neuron population
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population
        
        Attributes:
            value (int): Internal numeric value
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration
        
        Types:
            Excitatory: Standard excitatory neuron
            Inhibitory: Standard inhibitory neuron  
            Modulatory: Neuromodulatory neuron
            Sensory: Sensory neuron for input processing
            Motor: Motor neuron for output control
            Internal: Internal state neuron
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Standard excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Standard inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Neuromodulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron for input processing")
        .value("Motor", NeuronType::Motor, "Motor neuron for output control")
        .value("Internal", NeuronType::Internal, "Internal state neuron")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration
        
        Types:
            Excitatory: Standard excitatory synapse
            Inhibitory: Standard inhibitory synapse
            Modulatory: Neuromodulatory synapse
            Electrical: Direct electrical connection
            GapJunction: Gap junction coupling
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Neuromodulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Direct electrical connection")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction coupling")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration
        
        Stages:
            Initial: Primitive synaptic connections, high plasticity
            CriticalPeriod: Learning phase with strong synaptic modifications
            Maturation: Stabilization of connections, reduced plasticity
            Adult: Optimized performance with stable connections
            Aging: Declining plasticity, potential loss of connections
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Primitive synaptic connections")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Learning phase with strong plasticity")
        .value("Maturation", DevelopmentalStage::Maturation, "Stabilization phase")
        .value("Adult", DevelopmentalStage::Adult, "Optimized performance")
        .value("Aging", DevelopmentalStage::Aging, "Declining plasticity")
        .export_values();

    // Config - brain configuration management
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Brain configuration management class
        
        The Config class provides a key-value configuration system for
        NLM brain parameters. Configuration values include neuron counts,
        connection probabilities, plasticity parameters, and more.
        
        Configuration sources (in order of precedence):
        1. Default (hardcoded defaults)
        2. File (from config file)
        3. Command line (from command line arguments)
        4. Runtime (set programmatically)
        
        Attributes:
            Most configuration values are accessed via the get() method
            or using the getOr() method with default values.
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a file
             
             Args:
                 filepath (str): Path to configuration file
             
             Returns:
                 bool: True if file loaded successfully, False otherwise
             )pbdoc")
        .def("loadFromArgs", &Config::loadFromArgs, py::arg("argc"), py::arg("argv"),
             R"pbdoc(
             Load configuration from command line arguments
             
             Args:
                 argc (int): Number of command line arguments
                 argv (list): Command line arguments list
             
             Returns:
                 bool: True if arguments loaded successfully
             )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a file
             
             Args:
                 filepath (str): Path to save configuration file
             
             Returns:
                 bool: True if file saved successfully, False otherwise
             )pbdoc")
        .def("get", &Config::get<std::string>, py::arg("key"),
             R"pbdoc(
             Get configuration value as string
             
             Args:
                 key (str): Configuration key
             
             Returns:
                 Optional[str]: Value if key exists, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&, int>(&Config::get), py::arg("key"),
             R"pbdoc(
             Get configuration value as int
             
             Args:
                 key (str): Configuration key
             
             Returns:
                 Optional[int]: Value if key exists, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&, int64_t>(&Config::get), py::arg("key"),
             R"pbdoc(
             Get configuration value as int64_t
             
             Args:
                 key (str): Configuration key
             
             Returns:
                 Optional[int64_t]: Value if key exists, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&, double>(&Config::get), py::arg("key"),
             R"pbdoc(
             Get configuration value as double
             
             Args:
                 key (str): Configuration key
             
             Returns:
                 Optional[float]: Value if key exists, None otherwise
             )pbdoc")
        .def("get", py::overload_cast<const std::string&, bool>(&Config::get), py::arg("key"),
             R"pbdoc(
             Get configuration value as bool
             
             Args:
                 key (str): Configuration key
             
             Returns:
                 Optional[bool]: Value if key exists, None otherwise
             )pbdoc")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(
             Get configuration value with default
             
             Args:
                 key (str): Configuration key
                 defaultValue (str): Default value if key doesn't exist
             
             Returns:
                 str: Value if key exists, otherwise default value
             )pbdoc")
        .def("getOr", py::overload_cast<const std::string&, int, int>(&Config::getOr), py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(
             Get configuration value with default
             
             Args:
                 key (str): Configuration key
                 defaultValue (int): Default value if key doesn't exist
             
             Returns:
                 int: Value if key exists, otherwise default value
             )pbdoc")
        .def("getOr", py::overload_cast<const std::string&, int64_t, int64_t>(&Config::getOr), py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(
             Get configuration value with default
             
             Args:
                 key (str): Configuration key
                 defaultValue (int): Default value if key doesn't exist
             
             Returns:
                 int: Value if key exists, otherwise default value
             )pbdoc")
        .def("getOr", py::overload_cast<const std::string&, double, double>(&Config::getOr), py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(
             Get configuration value with default
             
             Args:
                 key (str): Configuration key
                 defaultValue (float): Default value if key doesn't exist
             
             Returns:
                 float: Value if key exists, otherwise default value
             )pbdoc")
        .def("getOr", py::overload_cast<const std::string&, bool, bool>(&Config::getOr), py::arg("key"), py::arg("defaultValue"),
             R"pbdoc(
             Get configuration value with default
             
             Args:
                 key (str): Configuration key
                 defaultValue (bool): Default value if key doesn't exist
             
             Returns:
                 bool: Value if key exists, otherwise default value
             )pbdoc")
        .def("set", &Config::set<std::string>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             R"pbdoc(
             Set configuration value (string)
             
             Args:
                 key (str): Configuration key
                 value (str): Value to set
                 source (ConfigSource): Source of configuration (default: Runtime)
             )pbdoc")
        .def("set", &Config::set<int>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             R"pbdoc(
             Set configuration value (int)
             
             Args:
                 key (str): Configuration key
                 value (int): Value to set
                 source (ConfigSource): Source of configuration (default: Runtime)
             )pbdoc")
        .def("set", &Config::set<int64_t>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             R"pbdoc(
             Set configuration value (int64_t)
             
             Args:
                 key (str): Configuration key
                 value (int): Value to set
                 source (ConfigSource): Source of configuration (default: Runtime)
             )pbdoc")
        .def("set", &Config::set<double>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             R"pbdoc(
             Set configuration value (float)
             
             Args:
                 key (str): Configuration key
                 value (float): Value to set
                 source (ConfigSource): Source of configuration (default: Runtime)
             )pbdoc")
        .def("set", &Config::set<bool>, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             R"pbdoc(
             Set configuration value (bool)
             
             Args:
                 key (str): Configuration key
                 value (bool): Value to set
                 source (ConfigSource): Source of configuration (default: Runtime)
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if configuration key exists
             
             Args:
                 key (str): Configuration key
             
             Returns:
                 bool: True if key exists, False otherwise
             )pbdoc")
        .def("remove", &Config::remove, py::arg("key"),
             R"pbdoc(
             Remove configuration key
             
             Args:
                 key (str): Configuration key to remove
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys
             
             Returns:
                 list[str]: List of all configuration keys
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get configuration summary
             
             Returns:
                 str: Human-readable configuration summary
             )pbdoc");

    // Brain - the central neural system
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        The central coordinator of the neural system
        
        The Brain class implements real spiking neural computation with
        event-driven dynamics and integrated memory, prediction, cognition,
        and neuromodulation systems.
        
        Key Features:
        - LIF neuron dynamics
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity rules (STDP, Hebbian, structural)
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning systems
        - Development stages
        - Checkpoint save/load
        
        Typical Usage:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("neuron_count", 1000)
            >>> config.set("random_seed", 42)
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            
            For simulation:
            >>> for step in range(1000):
            ...     brain.step(step)
        
        Attributes:
            Most methods return status information or results of operations.
            Error conditions are indicated by return values or exceptions.
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration
             
             Returns:
                 bool: True if initialization succeeded, False otherwise
             )pbdoc")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step), py::arg("currentStep"),
             R"pbdoc(
             Perform one simulation step
             
             Args:
                 currentStep (int): Current simulation step number
             )pbdoc")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step), py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform one simulation step with explicit time
             
             Args:
                 currentStep (int): Current simulation step number
                 currentTime (float): Current simulation time in seconds
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(
             Receive sensory input from environment
             
             Args:
                 input (SensoryInput): Sensory input data
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current into a specific neuron
             
             Args:
                 neuron (NeuronId): Target neuron
                 current (float): Current to inject
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type
             
             Args:
                 type (NeuronType): Neuron type to target
                 current (float): Current to inject
             )pbdoc")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             R"pbdoc(
             Get spike system for spike processing
             
             Returns:
                 SpikeSystem: Spike system object
             )pbdoc")
        .def("getSTDP", &Brain::getSTDP,
             R"pbdoc(
             Get STDP plasticity rule
             
             Returns:
                 STDP: STDP plasticity rule object
             )pbdoc")
        .def("getHebbian", &Brain::getHebbian,
             R"pbdoc(
             Get Hebbian plasticity rule
             
             Returns:
                 Hebbian: Hebbian plasticity rule object
             )pbdoc")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             R"pbdoc(
             Get structural plasticity rule
             
             Returns:
                 StructuralPlasticity: Structural plasticity rule object
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition ratio
             
             Returns:
                 float: E/I ratio (excitation divided by inhibition)
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count
             
             Returns:
                 int: Total number of spikes
             )pbdoc")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             R"pbdoc(
             Get number of pending spike events
             
             Returns:
                 int: Number of pending spike events
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor/action output based on motor neuron activity
             
             Returns:
                 Action: Motor command produced
             )pbdoc")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation, py::arg("signal"),
             R"pbdoc(
             Apply neuromodulatory signal
             
             Args:
                 signal (Neuromodulator): Neuromodulatory signal
             )pbdoc")
        .def("updatePlasticity", &Brain::updatePlasticity,
             R"pbdoc(
             Update plasticity rules (called automatically in step)
             )pbdoc")
        .def("develop", &Brain::develop,
             R"pbdoc(
             Apply developmental changes (called automatically in step)
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file (checkpointing)
             
             Args:
                 filepath (str): Path to save checkpoint file
             
             Returns:
                 bool: True if save succeeded, False otherwise
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file
             
             Args:
                 filepath (str): Path to checkpoint file
             
             Returns:
                 bool: True if load succeeded, False otherwise
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new brain region
             
             Args:
                 name (str): Optional region name
             
             Returns:
                 RegionId: ID of newly created region
             )pbdoc")
        .def("getRegion", py::overload_cast<RegionId, RegionId>(&Brain::getRegion), py::arg("id"),
             R"pbdoc(
             Get brain region by ID
             
             Args:
                 id (RegionId): Region identifier
             
             Returns:
                 NeuralRegion: Brain region object, or None if not found
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get number of regions
             
             Returns:
                 int: Number of regions
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total number of neurons
             
             Returns:
                 int: Total number of neurons
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total number of synapses
             
             Returns:
                 int: Total number of synapses
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get number of active neurons
             
             Returns:
                 int: Number of active neurons
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get number of firing neurons
             
             Returns:
                 int: Number of firing neurons
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate
             
             Returns:
                 float: Average firing rate in Hz
             )pbdoc")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             R"pbdoc(
             Get working memory system
             
             Returns:
                 NeuralWorkingMemory: Working memory object, or None if not available
             )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             R"pbdoc(
             Get episodic memory system
             
             Returns:
                 NeuralEpisodicMemory: Episodic memory object, or None if not available
             )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             R"pbdoc(
             Get associative memory system
             
             Returns:
                 NeuralAssociativeMemory: Associative memory object, or None if not available
             )pbdoc")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             R"pbdoc(
             Get prediction system
             
             Returns:
                 PredictionSystem: Prediction system object, or None if not available
             )pbdoc")
        .def("getPlanner", &Brain::getPlanner,
             R"pbdoc(
             Get neural planner
             
             Returns:
                 NeuralPlanner: Neural planner object, or None if not available
             )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             R"pbdoc(
             Get concept formation system
             
             Returns:
                 ConceptFormation: Concept formation object, or None if not available
             )pbdoc")
        .def("getAttention", &Brain::getAttention,
             R"pbdoc(
             Get attentional selection system
             
             Returns:
                 AttentionalSelection: Attentional selection object, or None if not available
             )pbdoc")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             R"pbdoc(
             Get development system
             
             Returns:
                 DevelopmentSystem: Development system object, or None if not available
             )pbdoc")
        .def("getDopamine", &Brain::getDopamine,
             R"pbdoc(
             Get dopamine neuromodulator
             
             Returns:
                 Dopamine: Dopamine object, or None if not available
             )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             R"pbdoc(
             Get curiosity neuromodulator
             
             Returns:
                 Curiosity: Curiosity object, or None if not available
             )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             R"pbdoc(
             Get novelty neuromodulator
             
             Returns:
                 Novelty: Novelty object, or None if not available
             )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             R"pbdoc(
             Get prediction error signal
             
             Returns:
                 PredictionError: Prediction error signal object, or None if not available
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             R"pbdoc(
             Get brain configuration
             
             Returns:
                 Config: Brain configuration object
             )pbdoc")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             R"pbdoc(
             Get random number generator
             
             Returns:
                 RandomGenerator: Random number generator object
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status information
             )pbdoc");

    // AgentBrain - brain-world interface
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Interface between NLM brain and the world
        
        The AgentBrain class connects the NLM brain to the world environment,
        handling sensory transduction and motor decoding.
        
        Key Features:
        - Sensory transduction: Convert world percepts to neural input
        - Motor decoding: Convert neural activity to motor commands
        - Neuromodulation: Apply reward signals to plasticity
        - Development: Update developmental stage
        - Attention: Selective processing through competition
        - Planning: Use predictions to select actions
        
        The AgentBrain acts as a bridge between the neural system and its
        environment, enabling the brain to interact with and learn from the world.
        
        Typical Usage:
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20)
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            
            For simulation loop:
            >>> for step in range(1000):
            ...     percept = world.getSensoryPercept()
            ...     agent.processSensoryInput(percept)
            ...     brain.step(step)
            ...     action = agent.decodeMotorCommand()
            ...     world.applyMotorCommand(action, world.getSimulationTime())
        
        Methods:
            Most methods return results or status information.
            Error conditions are indicated by return values or exceptions.
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize agent brain with world
             
             Args:
                 world (SimpleWorld): World environment
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
             Get expected sensory input size
             
             Returns:
                 int: Size of sensory input expected by brain
             )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
             Get expected motor output size
             
             Returns:
                 int: Size of motor output expected
             )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain
             
             Args:
                 percept (SensoryPercept): Sensory perception data
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command
             
             Returns:
                 MotorCommand: Motor command decoded from brain activity
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward") = 0.0f,
             R"pbdoc(
             Apply neuromodulation based on reward
             
             Args:
                 reward (float): Received reward
                 predictedReward (float): Predicted reward (default: 0.0)
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             R"pbdoc(
             Update development system
             
             Args:
                 timestep (float): Time step duration
             )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
             Get neuromodulation level
             
             Returns:
                 float: Current neuromodulation level
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get curiosity level
             
             Returns:
                 float: Current curiosity level
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get novelty level
             
             Returns:
                 float: Current novelty level
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get prediction error
             
             Returns:
                 float: Current prediction error
             )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
             Reset agent for new episode
             )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             R"pbdoc(
             Get brain pointer
             
             Returns:
                 Brain: Brain object
             )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             R"pbdoc(
             Enable or disable reward modulation
             
             Args:
                 enable (bool): True to enable, False to disable
             )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             R"pbdoc(
             Enable or disable structural plasticity
             
             Args:
                 enable (bool): True to enable, False to disable
             )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             R"pbdoc(
             Enable or disable development
             
             Args:
                 enable (bool): True to enable, False to disable
             )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             R"pbdoc(
             Enable or disable curiosity
             
             Args:
                 enable (bool): True to enable, False to disable
             )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(
             Check if reward modulation is enabled
             
             Returns:
                 bool: True if reward modulation is enabled
             )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(
             Check if structural plasticity is enabled
             
             Returns:
                 bool: True if structural plasticity is enabled
             )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(
             Check if development is enabled
             
             Returns:
                 bool: True if development is enabled
             )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(
             Check if curiosity is enabled
             
             Returns:
                 bool: True if curiosity is enabled
             )pbdoc");

    // SimpleWorld - simple world environment
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple world environment for agent interaction
        
        The SimpleWorld class provides a simple grid-based environment where
        agents can move, interact with objects, and receive sensory input.
        
        Key Features:
        - 2D grid world with configurable dimensions
        - Vision and other sensory modalities
        - Action system for agent interaction
        - Reward computation based on agent actions
        - Episode management and reset
        
        Typical Usage:
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> world.reset()
            
            For simulation loop:
            >>> for step in range(100):
            ...     world.update(0.1)  # Update world timestep
            ...     percept = world.getSensoryPercept()  # Get what agent sees
            ...     # Process percept with agent brain
            ...     # Get action from brain
            ...     world.applyMotorCommand(action, world.getSimulationTime())
            ...     # Check if episode ended
            ...     if world.isEpisodeEnded():
            ...         world.reset()  # Start new episode
        
        Methods:
            Most methods return results or status information.
            Error conditions are indicated by return values or exceptions.
        )pbdoc")
        .def(py::init<>())
        .def("initialize", &SimpleWorld::initialize, py::arg("width") = 10, py::arg("height") = 10, 
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             R"pbdoc(
             Initialize world environment
             
             Args:
                 width (int): World width in cells (default: 10)
                 height (int): World height in cells (default: 10)
                 visionWidth (int): Vision width in cells (default: 8)
                 visionHeight (int): Vision height in cells (default: 8)
             )pbdoc")
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
             R"pbdoc(
             Configure world dimensions and sensory capabilities
             
             Args:
                 width (int): World width in cells
                 height (int): World height in cells
                 visionWidth (int): Vision width in cells (default: 8)
                 visionHeight (int): Vision height in cells (default: 8)
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set random seed for reproducible world behavior
             
             Args:
                 seed (int): Random seed value
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
             Reset world to initial state (start of new episode)
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("dt"),
             R"pbdoc(
             Update world simulation state
             
             Args:
                 dt (float): Time step duration
             )pbdoc")
        .def("observe", &SimpleWorld::observe, py::arg("brain"),
             R"pbdoc(
             Get sensory percept from world for given brain
             
             Args:
                 brain (Brain): Brain object to generate percept for
             
             Returns:
                 SensoryPercept: Sensory perception data
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             R"pbdoc(
             Get sensory percept from world (for current brain)
             
             Returns:
                 SensoryPercept: Current sensory perception
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("command"), py::arg("time"),
             R"pbdoc(
             Apply motor command to world
             
             Args:
                 command (MotorCommand): Motor command to execute
                 time (float): Current simulation time
             
             Returns:
                 ActionResult: Result of applying the action
             )pbdoc")
        .def("computeReward", &SimpleWorld::computeReward, py::arg("brain"),
             R"pbdoc(
             Compute reward based on current world state
             
             Args:
                 brain (Brain): Brain object to compute reward for
             
             Returns:
                 float: Computed reward
             )pbdoc")
        .def("isEpisodeEnded", &SimpleWorld::isEpisodeEnded,
             R"pbdoc(
             Check if current episode has ended
             
             Returns:
                 bool: True if episode ended, False otherwise
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
             Get current simulation time
             
             Returns:
                 float: Current simulation time in seconds
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             R"pbdoc(
             Get agent body (position, orientation, etc.)
             
             Returns:
                 AgentBody: Agent body object
             )pbdoc")
        .def("setWorldProperty", &SimpleWorld::setWorldProperty, py::arg("key"), py::arg("value"),
             R"pbdoc(
             Set world property
             
             Args:
                 key (str): Property key
                 value: Property value
             )pbdoc")
        .def("getWorldProperty", &SimpleWorld::getWorldProperty, py::arg("key"),
             R"pbdoc(
             Get world property
             
             Args:
                 key (str): Property key
             
             Returns:
                 Property value
             )pbdoc");

    // SensoryPercept - sensory perception data
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory perception data from world
        
        The SensoryPercept class contains sensory information such as vision,
        touch, internal signals, proprioception, and other sensory modalities.
        
        Attributes:
            Contains multiple sensory channels as vectors of floats
            and a simulation time stamp.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<const std::vector<float>&, const std::vector<float>&, 
                     const std::vector<float>&, const std::vector<float>&, 
                     const std::vector<float>&, double>(),
             py::arg("vision"), py::arg("touch"), py::arg("internal"), 
             py::arg("proprioception"), py::arg("audio"), py::arg("time") = 0.0,
             R"pbdoc(
             Create sensory percept
             
             Args:
                 vision (list[float]): Vision sensory data
                 touch (list[float]): Touch sensory data
                 internal (list[float]): Internal signals
                 proprioception (list[float]): Proprioception data
                 audio (list[float]): Audio data
                 time (float): Simulation time (default: 0.0)
             )pbdoc")
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(
             Get vision sensory data
             
             Returns:
                 list[float]: Vision data
             )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(
             Get touch sensory data
             
             Returns:
                 list[float]: Touch data
             )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(
             Get internal signal data
             
             Returns:
                 list[float]: Internal signals
             )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(
             Get proprioception data
             
             Returns:
                 list[float]: Proprioception data
             )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(
             Get audio sensory data
             
             Returns:
                 list[float]: Audio data
             )pbdoc")
        .def("getTime", &SensoryPercept::getTime,
             R"pbdoc(
             Get simulation time
             
             Returns:
                 float: Simulation time
             )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(
             Set vision data
             
             Args:
                 vision (list[float]): New vision data
             )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(
             Set touch data
             
             Args:
                 touch (list[float]): New touch data
             )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(
             Set internal signals
             
             Args:
                 internal (list[float]): New internal signals
             )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(
             Set proprioception data
             
             Args:
                 proprioception (list[float]): New proprioception data
             )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(
             Set audio data
             
             Args:
                 audio (list[float]): New audio data
             )pbdoc")
        .def("setTime", &SensoryPercept::setTime, py::arg("time"),
             R"pbdoc(
             Set simulation time
             
             Args:
                 time (float): New simulation time
             )pbdoc")
        .def("getSensorySize", &SensoryPercept::getSensorySize,
             R"pbdoc(
             Get total sensory data size
             
             Returns:
                 int: Total size of all sensory data
             )pbdoc")
        .def("concatenate", &SensoryPercept::concatenate, py::arg("other"),
             R"pbdoc(
             Concatenate with another sensory percept
             
             Args:
                 other (SensoryPercept): Other sensory percept to concatenate
             )pbdoc");

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Motor command enumeration
        
        Commands:
            MoveForward: Move forward one cell
            MoveBackward: Move backward one cell
            TurnLeft: Turn left 90 degrees
            TurnRight: Turn right 90 degrees
            LookLeft: Look left
            LookRight: Look right
            Interact: Interact with object
            Wait: Do nothing (stay in place)
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // ActionResult - result of applying action
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of applying a motor command
        
        Contains the outcome of executing a motor command, including
        reward received and whether the action was successful.
        
        Attributes:
            reward (float): Reward received from action
            success (bool): Whether action was successful
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool>(), py::arg("reward") = 0.0f, py::arg("success") = false)
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def("__repr__", [](const ActionResult& result) {
            return "ActionResult(reward=" + std::to_string(result.reward) + 
                   ", success=" + std::to_string(result.success) + ")";
        });

    // AgentBody - agent physical representation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent physical representation (position, orientation, etc.)
        
        The AgentBody class represents the physical state of an agent in the world,
        including position, orientation, energy, and other physical properties.
        
        Attributes:
            position (list[float]): Agent position in world coordinates
            orientation (float): Agent orientation in radians
            energy (float): Current energy level
            velocity (list[float]): Agent velocity
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("position", &AgentBody::position)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("velocity", &AgentBody::velocity)
        .def("getX", &AgentBody::getX)
        .def("getY", &AgentBody::getY)
        .def("setPosition", &AgentBody::setPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set agent position
             
             Args:
                 x (float): X coordinate
                 y (float): Y coordinate
             )pbdoc")
        .def("getAngle", &AgentBody::getAngle)
        .def("setOrientation", &AgentBody::setOrientation, py::arg("angle"),
             R"pbdoc(
             Set agent orientation
             
             Args:
                 angle (float): Orientation in radians
             )pbdoc")
        .def("isNear", &AgentBody::isNear, py::arg("other"), py::arg("distance") = 1.0f,
             R"pbdoc(
             Check if agent is near another agent/body
             
             Args:
                 other (AgentBody): Other agent body
                 distance (float): Distance threshold (default: 1.0)
             
             Returns:
                 bool: True if within distance
             )pbdoc")
        .def("moveTo", &AgentBody::moveTo, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Move agent to position
             
             Args:
                 x (float): Target X coordinate
                 y (float): Target Y coordinate
             )pbdoc")
        .def("rotate", &AgentBody::rotate, py::arg("angle"),
             R"pbdoc(
             Rotate agent
             
             Args:
                 angle (float): Angle to rotate in radians
             )pbdoc");

    // Convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration
        
        Returns:
            Config: Default configuration object
        )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with configuration
        
        Args:
            config (Config): Configuration object
        
        Returns:
            Brain: Brain object
        )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world
        
        Returns:
            SimpleWorld: Simple world object
        )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface
        
        Args:
            brain (Brain): Brain object to interface with
        
        Returns:
            AgentBrain: Agent brain object
        )pbdoc");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
