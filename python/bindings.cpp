#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>

// Import all the NLM system headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"

// Forward declarations for complex types
namespace py = pybind11;

// Custom exception classes for Python-specific error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

// Helper function for type validation
def validate_range(const std::string& name, float value, float min, float max) {
    if (value < min || value > max) {
        throw ValidationError(name + " must be in range [" + 
                             std::to_string(min) + ", " + 
                             std::to_string(max) + "], got " + 
                             std::to_string(value));
    }
}

// Helper function for non-empty check
def validate_not_empty(const std::string& name, size_t size) {
    if (size <= 0) {
        throw ValidationError(name + " must not be empty");
    }
}

// Utility functions for common neural network patterns
def create_neural_network_config(size_t num_neurons, const std::string& filename = "") {
    auto config = std::make_shared<Config>();
    
    // Basic configuration
    config->setInt("brain.numRegions", 1);
    config->setInt("brain.region0.numPopulations", 1);
    config->setInt("brain.region0.population0.numNeurons", static_cast<int>(num_neurons));
    config->setString("brain.region0.population0.name", "main_population");
    
    if (!filename.empty()) {
        config->saveToFile(filename);
    }
    
    return config;
}

def simulate_steps(brain, world, steps, timestep = 0.01) {
    std::vector<std::string> results;
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i, i * timestep);
        world->update(timestep);
        
        // Record metrics
        std::stringstream ss;
        ss << "Step " << i << ": "
           << "Spikes: " << brain->getTotalSpikeCount()
           << ", Active: " << brain->getActiveNeuronCount()
           << ", Firing: " << brain->getFiringNeuronCount()
           << ", Ratio: " << std::fixed << std::setprecision(3)
           << brain->getExcitationInhibitionRatio();
        
        results.push_back(ss.str());
    }
    
    return results;
}

// Visualization helper
def print_neuron_stats(brain) {
    std::cout << "=== Brain Statistics ===" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Excitation/Inhibition ratio: " << 
        std::fixed << std::setprecision(3) << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << 
        std::fixed << std::setprecision(3) << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "=" << std::endl;
}

// Debug helper
def debug_spike_system(spike_system) {
    if (!spike_system) return "Null spike system";
    
    std::stringstream ss;
    ss << "Spike System Statistics:" << std::endl;
    ss << "  Total spikes: " << spike_system->getSpikeCount() << std::endl;
    ss << "  Pending spikes: " << spike_system->getPendingSpikeCount() << std::endl;
    ss << "  Pending delayed: " << spike_system->getPendingDelayedCount() << std::endl;
    ss << "  Average rate: " << std::fixed << std::setprecision(3) 
       << spike_system->getAverageSpikeRate() << " Hz" << std::endl;
    
    auto mostActive = spike_system->getMostActiveNeurons(5);
    if (!mostActive.empty()) {
        ss << "  Most active neurons: ";
        for (const auto& id : mostActive) {
            ss << id.index() << " ";
        }
        ss << std::endl;
    }
    
    return ss.str();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
NLM (Neural Learning Machine) Python Bindings
=============================================

Comprehensive Python bindings for the NLM C++ neural simulation framework.

Overview
--------
NLM provides a biologically-inspired framework for building adaptive neural systems
with learning, memory, and decision-making capabilities. The system integrates
spiking neural networks with plasticity mechanisms, neuromodulation, and cognitive
subsystems to create intelligent agents that can learn from experience.

Installation
------------

1. Install NLM Python bindings:
   pip install nlm

2. Alternatively, build from source:
   mkdir build && cd build
   cmake .. -DBUILD_PYTHON_BINDINGS=ON
   make -j$(nproc)
   cd python
   pip install -e .

Basic Usage Example
-------------------

>>> import nlm
>>> 
>>> # Create a simple brain configuration
>>> config = nlm.createDefaultConfig()
>>> config.set("brain.numRegions", 2)
>>> config.set("brain.region0.population0.numNeurons", 100)
>>> config.set("brain.region0.population0.neuronType", "Excitatory")
>>> 
>>> # Create and initialize brain
>>> brain = nlm.createBrain(config)
>>> brain.initialize()
>>> 
>>> # Create world and agent
>>> world = nlm.createSimpleWorld()
>>> agent_brain = nlm.createAgentBrain(brain)
>>> agent_brain.initialize(world)
>>> 
>>> # Run simulation
>>> world.setAgentStart(0, 0)
>>> for step in range(100):
...     # Simulate sensory input
...     percept = world.getSensoryPercept()
...     agent_brain.processSensoryInput(percept)
...     
...     # Get action
...     action = agent_brain.decodeMotorCommand()
...     world.applyMotorCommand(action, step * 0.01)
...     
...     # Update world
...     world.update(0.01)
...     
...     # Apply reward modulation (example)
...     reward = 0.1 if step % 10 == 0 else 0.0
...     agent_brain.applyRewardModulation(reward, 0.0)
>>> 
>>> # Save brain state
>>> brain.save("my_brain.nlm")
>>> 
>>> # Load brain state
>>> loaded_brain = nlm.createBrain(nlm.createDefaultConfig())
>>> loaded_brain.load("my_brain.nlm")

Advanced Usage Example
----------------------

>>> import nlm
>>> 
>>> # Configure advanced learning mechanisms
>>> config = nlm.createDefaultConfig()
>>> config.set("plasticity.stdp.enable", True)
>>> config.set("plasticity.stdp.ltpWeight", 0.01)
>>> config.set("plasticity.stdp.ltdWeight", 0.012)
>>> config.set("plasticity.stdp.timeConstant", 20.0)
>>> config.set("plasticity.hebbian.enable", True)
>>> config.set("plasticity.hebbian.learningRate", 0.01)
>>> 
>>> # Create brain with learning mechanisms
>>> brain = nlm.createBrain(config)
>>> brain.initialize()
>>> 
>>> # Access learning mechanisms
>>> stdp = brain.getSTDP()
>>> if stdp:
...     print(f"STDP LTP weight: {stdp.getLTPWeight()}")
...     print(f"STDP LTD weight: {stdp.getLTDWeight()}")
...     print(f"STDP time constant: {stdp.getTimeConstant()}")
>>> 
>>> hebbian = brain.getHebbian()
>>> if hebbian:
...     print(f"Hebbian learning rate: {hebbian.getLearningRate()}")
>>> 
>>> # Configure neuromodulation
>>> config.set("neuromodulation.dopamine.enable", True)
>>> config.set("neuromodulation.curiosity.enable", True)
>>> config.set("neuromodulation.novelty.enable", True)
>>> config.set("neuromodulation.predictionError.enable", True)
>>> 
>>> # Access neuromodulation systems
>>> dopamine = brain.getDopamine()
>>> if dopamine:
...     print(f"Dopamine level: {dopamine.getLevel()}")
...     print(f"Dopamine plasticity factor: {dopamine.getPlasticityFactor()}")
>>> 
>>> curiosity = brain.getCuriosity()
>>> if curiosity:
...     print(f"Curiosity level: {curiosity.getLevel()}")
...     print(f"Exploration drive: {curiosity.getExplorationDrive()}")
>>> 
>>> # Configure spike system for event-driven simulation
>>> spike_system = brain.getSpikeSystem()
>>> if spike_system:
...     print(spike_system.debug())
...     spike_system.registerHandler(lambda event: print(f"Spike at {event.timestamp}"))
>>> 
>>> # Run simulation with detailed logging
>>> import sys
>>> print("Initial brain state:")
>>> nlm.print_neuron_stats(brain)
>>> 
>>> # Simulate with learning
>>> world = nlm.createSimpleWorld()
>>> agent_brain = nlm.createAgentBrain(brain)
>>> agent_brain.initialize(world)
>>> 
>>> # Track learning progress
>>> learning_data = []
>>> for step in range(1000):
...     # Get current metrics
...     metrics = {
...         "step": step,
...         "spikes": brain.getTotalSpikeCount(),
...         "novelty": agent_brain.getNoveltyLevel(),
...         "curiosity": agent_brain.getCuriosityLevel(),
...         "prediction_error": agent_brain.getPredictionError(),
...         "dopamine": agent_brain.getNeuromodulationLevel(),
...         "plasticity": agent_brain.getBrain().getDevelopmentalStage()
...     }
...     learning_data.append(metrics)
...     
...     # Simulate step
...     percept = world.getSensoryPercept()
...     agent_brain.processSensoryInput(percept)
...     action = agent_brain.decodeMotorCommand()
...     world.applyMotorCommand(action, step * 0.01)
...     world.update(0.01)
...     
...     if step % 100 == 0:
...         print(f"Progress: Step {step}/{1000}")
>>> 
>>> # Analyze learning results
>>> final_metrics = learning_data[-1]
>>> print(f"Final novelty: {final_metrics['novelty']:.3f}")
>>> print(f"Final curiosity: {final_metrics['curiosity']:.3f}")
>>> print(f"Final prediction error: {final_metrics['prediction_error']:.3f}")
>>> 
>>> # Save learned behavior
>>> brain.save("learned_behavior.nlm")
>>> 
Reference Documentation
------------------------
The module provides the following key components:

- Brain: Central neural simulation engine with spiking neurons
- Config: Configuration management with JSON/YAML support
- AgentBrain: Interface for agent-world integration
- SimpleWorld: 2D simulation environment
- SensoryInput: Base class for sensory modalities (Vision, Audio, Internal)
- Action: Motor command representation
- STDP: Spike-timing-dependent plasticity
- Hebbian: Hebbian learning rule
- SpikeSystem: Event-driven spike processing
- PlasticityRule: Abstract base for plasticity mechanisms
- Neuromodulator: Abstract base for neuromodulatory signals
- Dopamine: Reward-based neuromodulation
- Curiosity: Exploration motivation
- Novelty: Novelty detection
- PredictionError: Prediction error computation

All classes include comprehensive docstrings with examples and parameter descriptions.
"pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", "NLMError");
    py::register_exception<SimulationError>(m, "SimulationError", "NLMError");
    py::register_exception<ValidationError>(m, "ValidationError", "NLMError");

    // Register type conversions and validation helpers
    m.def("validate_range", &validate_range,
          "Validate that a value is within a specified range",
          py::arg("name"), py::arg("value"), py::arg("min"), py::arg("max"));
    
    m.def("validate_not_empty", &validate_not_empty,
          "Validate that a value is not empty",
          py::arg("name"), py::arg("size"));
    
    m.def("create_neural_network_config", &create_neural_network_config,
          "Create a basic neural network configuration",
          py::arg("num_neurons"), py::arg("filename") = "");
    
    m.def("simulate_steps", &simulate_steps,
          "Run simulation for a specified number of steps",
          py::arg("brain"), py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.01);
    
    m.def("print_neuron_stats", &print_neuron_stats,
          "Print detailed brain statistics to console",
          py::arg("brain"));
    
    m.def("debug_spike_system", &debug_spike_system,
          "Get detailed statistics about the spike system",
          py::arg("spike_system"));

    // Neuron identifier types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
Unique identifier for a neuron with strong typing and safety.

Represents the memory address or index of a specific neuron in the network.
Provides comparison, hashing, and indexing operations for use in dictionaries
and sets. NeuronId ensures type safety when accessing neurons.
)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the neuron index as integer")
        .def("__eq__", &NeuronId::operator==, "Check if two neuron IDs are equal")
        .def("__ne__", &NeuronId::operator!=, "Check if two neuron IDs are not equal")
        .def("__lt__", &NeuronId::operator<, "Check if this neuron ID is less than another")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Synapse identifier types
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
Unique identifier for a synapse connection between neurons.

Represents the memory address or index of a specific synaptic connection
in the neural network. Provides type safety and enables efficient lookup
in data structures. SynapseId ensures that connections can be uniquely
referenced throughout the simulation.
)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the synapse index as integer")
        .def("__eq__", &SynapseId::operator==, "Check if two synapse IDs are equal")
        .def("__ne__", &SynapseId::operator!=, "Check if two synapse IDs are not equal")
        .def("__lt__", &SynapseId::operator<, "Check if this synapse ID is less than another")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // Region identifier types
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
Unique identifier for a neural region within the brain.

Represents a collection of neuron populations that perform specific functions
(sensory, motor, association, etc.). RegionId enables type-safe access to
brain regions and supports hierarchical organization of neural circuitry.
)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the region index as integer")
        .def("__eq__", &RegionId::operator==, "Check if two region IDs are equal")
        .def("__ne__", &RegionId::operator!=, "Check if two region IDs are not equal")
        .def("__lt__", &RegionId::operator<, "Check if this region ID is less than another")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // Population identifier types
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
Unique identifier for a neuron population within a brain region.

Represents a homogeneous group of neurons that share similar properties
(neuron type, connectivity pattern, function). PopulationId enables
organized management of neuron groups and supports population-level
operations and statistics.
)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the population index as integer")
        .def("__eq__", &PopulationId::operator==, "Check if two population IDs are equal")
        .def("__ne__", &PopulationId::operator!=, "Check if two population IDs are not equal")
        .def("__lt__", &PopulationId::operator<, "Check if this population ID is less than another")
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return "PopulationId(" + std::to_string(id.value) + ")";
        });

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
Enumeration of neuron types in the neural network.

Each type represents different functional roles and properties in the neural
circuitry, affecting connectivity, excitability, and computational role.
)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Standard excitatory neurons that promote firing in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Neurons that suppress firing in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neurons that release neuromodulators affecting broader network state")
        .value("Sensory", NeuronType::Sensory, "Neurons that receive and process external sensory inputs")
        .value("Motor", NeuronType::Motor, "Neurons that generate motor commands for action")
        .value("Internal", NeuronType::Internal, "Internal neurons that maintain internal state and representations");

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
Enumeration of synaptic connection types between neurons.

Different synapse types implement different transmission mechanisms and
plasticity rules, enabling diverse computational capabilities in the network.
)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard chemical synapse that increases postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Chemical synapse that decreases postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, "Synapse releasing neuromodulators for long-term effects")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling through gap junctions")
        .value("GapJunction", SynapseType::GapJunction, "Electrical synapse allowing direct ion flow");

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
Enumeration of developmental stages for neural plasticity and learning.

The brain undergoes different developmental phases with distinct learning
capabilities and plasticity rules, mimicking biological maturation processes.
)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Early development with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Sensitive period for certain learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Middle development with moderate plasticity")
        .value("Adult", DevelopmentalStage::Adult, "Adult stability with refined learning")
        .value("Aging", DevelopmentalStage::Aging, "Age-related changes and decline");

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
Enumeration of neuron firing states in the spiking neuron model.

Represents the dynamic state of a neuron at any given simulation step,
affecting its behavior and responsiveness to inputs.
)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not firing")
        .value("Active", FiringState::Active, "Neuron currently firing action potentials")
        .value("Refractory", FiringState::Refractory, "Neuron recovering from firing, unable to fire")
        .value("Inhibited", FiringState::Inhibited, "Neuron actively inhibited, unable to fire");

    // Action type enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
Enumeration of possible action types for motor output.

Represents different classes of behaviors an agent can perform in the environment,
ranging from basic movement to complex interactions and cognitive operations.
)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Turn and move left")
        .value("MoveRight", ActionType::MoveRight, "Turn and move right")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Consume food or resources")
        .value("Drink", ActionType::Drink, "Drink water or other liquids")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom user-defined action");

    // Motor command enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
Enumeration of low-level motor commands for execution.

Represents discrete, executable actions that can be directly sent to
 actuators or simulated effects in the environment.
)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait/idle");

    // World object type enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
Enumeration of object types in the simulated world.

Represents different categories of objects that can exist in the environment,
affecting their behavior, interactions, and sensory properties.
)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space with no object")
        .value("Resource", WorldObjectType::Resource, "Consumable resource (food, water)")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object (predator, obstacle)")
        .value("Wall", WorldObjectType::Wall, "Solid barrier")
        .value("Marker", WorldObjectType::Marker, "Special point of interest");

    // ===== CONFIGURATION CLASS =====
    py::class_<Config>(m, "Config", R"pbdoc(
Configuration management class for the NLM neural system.

Provides a key-value storage system with type safety and persistence
functionality. Supports loading from and saving to JSON/YAML files,
command-line arguments, and programmatic configuration. Essential for
tuning brain parameters, simulation settings, and learning mechanisms.

Configuration keys follow dot notation for hierarchical organization:
"brain.numRegions", "brain.region0.population0.numNeurons", etc.
)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
Load configuration from a JSON file.

Args:
    filepath: Path to the JSON configuration file

Example:
    >>> config = nlm.Config()
    >>> config.loadFromFile("config.json")
    >>> print(config.get("brain.numRegions"))
    2

Returns:
    True if file was successfully loaded, False otherwise
)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            R"pbdoc(
Load configuration from command-line arguments.

Args:
    argc: Number of command-line arguments (sys.argv length)
    argv: Array of command-line argument strings

Example:
    >>> config = nlm.Config()
    >>> config.loadFromArgs(len(sys.argv), sys.argv)
    >>> print(config.get("learning.rate"))
    0.01

Returns:
    True if arguments were successfully parsed, False otherwise
)pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
Save configuration to a JSON file.

Args:
    filepath: Path where the configuration will be saved

Example:
    >>> config = nlm.Config()
    >>> config.set("brain.numRegions", 3)
    >>> config.saveToFile("my_config.json")
    >>> # Later...
    >>> new_config = nlm.Config()
    >>> new_config.loadFromFile("my_config.json")
)pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
Check if a configuration key exists.

Args:
    key: Configuration key in dot notation (e.g., "brain.numRegions")

Returns:
    True if the key exists, False otherwise

Example:
    >>> config = nlm.Config()
    >>> if config.has("brain.numRegions"):
    ...     print(config.getInt("brain.numRegions"))
)pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
Get all configuration keys.

Returns:
    List of all configuration keys in dot notation

Example:
    >>> config = nlm.Config()
    >>> keys = config.getKeys()
    >>> print(keys)
    ['brain.numRegions', 'brain.region0.population0.numNeurons', ...]
)pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
Clear all configuration entries.

Example:
    >>> config = nlm.Config()
    >>> config.set("brain.numRegions", 2)
    >>> config.clear()
    >>> config.getKeys()
    []
)pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
Get a summary string of the configuration.

Returns:
    Formatted string with configuration overview

Example:
    >>> config = nlm.Config()
    >>> config.set("brain.numRegions", 2)
    >>> print(config.summary())
    Configuration (2 regions, 1 population, 0 neurons)
)pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===== SENSORY INPUT CLASSES =====
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
Base class for all sensory input modalities in the environment.

Represents the raw data collected from sensory receptors or measurements.
Provides common interface for accessing sensor data, dimensions, and timestamps.
Subclasses include Vision, Audio, and InternalSignals for specific modalities.
)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input",
             py::arg("self"))
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector",
             py::arg("self"))
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality",
             py::arg("self"))
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp",
             py::arg("self"))
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input", py::arg("self"));

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
Visual sensory input representing camera or eye observations.

Provides access to 2D visual data with configurable resolution and color channels.
Vision input is essential for object recognition, scene understanding, and
navigation tasks in visual environments.

Attributes:
    width: Horizontal resolution in pixels
    height: Vertical resolution in pixels
    channels: Number of color channels (1=grayscale, 3=RGB)
)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(
Create a Vision sensory input.

Args:
    width: Image width in pixels
    height: Image height in pixels
    channels: Number of color channels (default: 3 for RGB)

Example:
    >>> vision = nlm.Vision(64, 64, 3)
    >>> print(f"Vision size: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    Vision size: 64x64x3
)pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
Set vision data from a flat array.

Args:
    data: Flattened array of pixel values in row-major order

Example:
    >>> vision = nlm.Vision(2, 2, 3)
    >>> vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2])
    >>> pixels = vision.getData()
    >>> print(pixels)
    [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2]
)pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get the image width in pixels.)pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get the image height in pixels.)pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get the number of color channels.)pbdoc");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
Audio sensory input representing sound measurements.

Provides access to audio signals with configurable sample rate and duration.
Audio input is crucial for processing environmental sounds, speech, and
musical patterns in auditory environments.

Attributes:
    sampleRate: Samples per second (Hz)
    numSamples: Total number of audio samples
)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(
Create an Audio sensory input.

Args:
    sampleRate: Samples per second (Hz)
    numSamples: Total number of audio samples

Example:
    >>> audio = nlm.Audio(44100, 4410)  # 1 second of audio at 44.1kHz
    >>> print(f"Audio: {audio.getNumSamples()} samples at {audio.getSampleRate()} Hz")
)pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
Set audio data from a flat array.

Args:
    data: Array of audio sample values normalized between -1.0 and 1.0

Example:
    >>> audio = nlm.Audio(44100, 4410)
    >>> audio.setData([0.0, 0.5, 0.0, -0.5] * 1102 + [0.0])
)pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set the audio sample rate in Hz.)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get the audio sample rate in Hz.)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get the number of audio samples.)pbdoc");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
Internal neural signals representing internal brain state.

Provides access to internal neural activity patterns, memory states,
and cognitive signals. InternalSignals represent the brain's ongoing
computational state, including working memory, predictions, and goals.
)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(
Add a signal value to the internal signals.

Args:
    value: Signal value to add
)pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(
Clear all internal signals.

Example:
    >>> internal = nlm.InternalSignals()
    >>> internal.addSignal(0.5)
    >>> internal.addSignal(-0.3)
    >>> print(f"Internal signals: {internal.getData()}")
    [0.5, -0.3]
)pbdoc");

    // ===== ACTION CLASS =====
    py::class_<Action>(m, "Action", R"pbdoc(
Represents a motor action or behavior generated by the brain.

Actions are the output of neural processing that result in changes to
the agent's body state or the environment. Each action has a type and
optional parameters that specify the details of the behavior.
)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(
Create a basic action with just a type.

Args:
    type: The type of action to perform

Example:
    >>> action = nlm.Action(nlm.ActionType.MoveForward)
    >>> print(f"Action type: {action.getType()}")
    Action type: MoveForward
)pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(
Create an action with type and parameters.

Args:
    type: The type of action to perform
    parameters: Optional parameters for fine-tuning the action

Example:
    >>> action = nlm.Action(nlm.ActionType.Look, [0.5, 0.5])
    >>> print(f"Action: {action.getType()} at {action.getParameters()}")
)pbdoc")
        .def("getType", &Action::getType,
             R"pbdoc(Get the action type.)pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set the action type.)pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get the action parameters.)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set the action parameters.)pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(Get the human-readable name of the action.)pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(Create a copy of this action.)pbdoc");

    // ===== WORLD OBJECT CLASS =====
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
Represents an object in the simulated world environment.

Objects populate the world with resources, hazards, landmarks, and other
entities that the agent can interact with. Each object has physical
properties (position, size) and semantic properties (type, value).
)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(
Create a world object with position and properties.

Args:
    x: X coordinate in world space
    y: Y coordinate in world space
    type: Type of object (Wall, Resource, Hazard, etc.)
    value: Value or size of the object
    radius: Physical radius of the object

Example:
    >>> wall = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Wall, 1.0, 0.5)
    >>> print(f"Wall at ({wall.x}, {wall.y}) with radius {wall.radius}")
)pbdoc")
        .def_readwrite("x", &WorldObject::x, "X coordinate of the object")
        .def_readwrite("y", &WorldObject::y, "Y coordinate of the object")
        .def_readwrite("radius", &WorldObject::radius, "Physical radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of the object")
        .def_readwrite("value", &WorldObject::value, "Value or size of the object")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active");

    // ===== AGENT BODY CLASS =====
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
Represents the physical state of an agent in the world.

AgentBody contains all kinematic and physiological properties that define
the agent's position, orientation, and physical capabilities. It tracks
energy, health, age, and movement state, providing a complete model of the
agent's embodied state.

Attributes:
    x, y: Position coordinates
    orientation: Facing direction in radians
    velocityX, velocityY: Linear velocity components
    angularVelocity: Rotational velocity
    energy: Current energy level
    health: Health status (0-1)
    age: Age in simulation time units
    isMoving: Whether the agent is currently moving
    isTurning: Whether the agent is currently turning
)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate of the agent")
        .def_readwrite("y", &AgentBody::y, "Y coordinate of the agent")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X component of linear velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y component of linear velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Rotational velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health status (0-1)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation time units")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset,
             R"pbdoc(
Reset the agent body to initial state.

Example:
    >>> body = nlm.AgentBody()
    >>> body.x = 0.0
    >>> body.y = 0.0
    >>> body.orientation = 0.0
    >>> body.energy = 1.0
    >>> body.health = 1.0
    >>> body.reset()  # Reset to defaults
)pbdoc");

    // ===== ACTION RESULT CLASS =====
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
Represents the outcome of an action performed by the agent.

ActionResult captures the immediate consequences of an action, including
reward received, success status, and descriptive information. This data
is essential for reinforcement learning and reward-modulated plasticity.

Attributes:
    reward: Numerical reward value (positive = beneficial, negative = harmful)
    success: Whether the action was successful
    message: Descriptive text about the outcome
)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(
Create an action result.

Args:
    reward: Numerical reward value
    success: Whether the action was successful
    message: Descriptive text about the outcome

Example:
    >>> result = nlm.ActionResult(1.0, True, "Found food resource")
    >>> print(f"Reward: {result.reward}, Success: {result.success}")
)pbdoc")
        .def_readwrite("reward", &ActionResult::reward, "Numerical reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive outcome message");

    // ===== SENSORY PERCEPT CLASS =====
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
Represents the complete sensory input received by the agent at a time step.

SensoryPercept integrates data from all sensory modalities (vision, touch,
proprioception, internal signals) into a unified representation that can
be processed by the agent brain. It includes timing information and methods
to access individual sensory components.

Attributes:
    vision: Visual data as 2D array (width x height x channels)
    touch: Tactile/force sensor data
    internal: Internal neural signals
    proprioception: Body position and movement data
    audio: Audio signal data
    timestamp: Time when this percept was recorded
)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data",
             py::arg("self"))
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data", py::arg("self"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width",
             py::arg("self"))
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height",
             py::arg("self"))
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data",
             py::arg("self"))
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data", py::arg("self"))
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals",
             py::arg("self"))
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals", py::arg("self"))
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data",
             py::arg("self"))
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data", py::arg("self"))
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data",
             py::arg("self"))
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data", py::arg("self"))
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals",
             py::arg("self"))
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp",
             py::arg("self"))
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp", py::arg("self"));

    // ===== SIMPLE WORLD CLASS =====
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
Simple 2D simulation environment for the NLM neural system.

SimpleWorld provides a basic physics-based environment where agents can
move, sense, and interact with objects. It implements collision detection,
kinematics, and provides sensory input to agents. This class serves as
the foundation for reinforcement learning and embodied AI experiments.

Key Features:
- 2D continuous space with collision detection
- Multiple sensory modalities (vision, touch, proprioception, internal)
- Physics simulation with energy decay
- Object management (add/remove resources, hazards, walls)
- Agent movement and interaction capabilities

Attributes:
    width, height: World boundaries
    maxEnergy: Maximum energy capacity
    energyDecayRate: Rate at which energy depletes over time
)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
Configure the world dimensions and agent sensing.

Args:
    width: World width in arbitrary units
    height: World height in arbitrary units  
    visionWidth: Width of agent's vision field
    visionHeight: Height of agent's vision field

Example:
    >>> world = nlm.createSimpleWorld()
    >>> world.configure(100, 100, 16, 16)  # 100x100 world, 16x16 vision
    >>> print(f"World: {world.getWidth()}x{world.getHeight()}, Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
)pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
Reset the world to its initial state.

Example:
    >>> world.reset()
)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
Set the agent's starting position.

Args:
    x: X coordinate for agent start
    y: Y coordinate for agent start

Example:
    >>> world.setAgentStart(50, 50)
)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
Update world state for one simulation timestep.

Args:
    timestep: Duration of the time step in seconds

Example:
    >>> for i in range(100):
    ...     world.update(0.01)  # 10ms timesteps
)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
Apply a motor command to the agent.

Args:
    cmd: Motor command to execute
    currentTime: Current simulation time

Example:
    >>> world.applyMotorCommand(nlm.MotorCommand.MoveForward, time_step)
)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
Get the current sensory percept from the world.

Returns:
    SensoryPercept containing vision, touch, proprioception, and internal signals

Example:
    >>> percept = world.getSensoryPercept()
    >>> vision = percept.getVision()
    >>> touch = percept.getTouch()
)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
Get the agent's body state.

Returns:
    AgentBody containing position, velocity, energy, and health

Example:
    >>> body = world.getAgentBody()
    >>> print(f"Position: ({body.x}, {body.y}), Energy: {body.energy}")
)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
Add an object to the world.

Args:
    obj: WorldObject to add

Example:
    >>> food = nlm.WorldObject(10, 10, nlm.WorldObjectType.Resource, 1.0)
    >>> world.addObject(food)
)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
Remove an object at specific coordinates.

Args:
    x: X coordinate
    y: Y coordinate
)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
Check if a position is valid (within bounds and collision-free).

Args:
    x: X coordinate
    y: Y coordinate

Returns:
    True if position is valid for agent placement
)pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width.)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height.)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum agent energy.)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum agent energy.)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate.)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate.)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time.)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
Set random seed for reproducible simulations.

Args:
    seed: Random seed value

Example:
    >>> world.setRandomSeed(42)
    >>> # Run the same simulation multiple times for reproducibility
)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get current random seed.)pbdoc");

    // ===== BRAIN CLASS =====
    py::class_<Brain>(m, "Brain", R"pbdoc(
Central neural simulation brain for spiking neural networks.

Brain implements a biologically-inspired neural network that performs
real-time spiking computation with integrated plasticity, neuromodulation,
and cognitive systems. It serves as the core processing engine for
intelligent agents, providing perception, action selection, and learning
capabilities.

Key Features:
- Spiking neural network with event-driven dynamics
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
- Memory systems (working memory, episodic memory, associative memory)
- Cognitive subsystems (planner, concept formation, attention)
- Developmental system with stage-dependent learning
- Comprehensive statistics and monitoring capabilities

Brain uses a modular architecture where different subsystems can be
enabled/disabled independently, allowing for flexible configuration of
learning and computational capabilities.

Example Usage:
    >>> import nlm
    >>> 
    >>> # Create and configure brain
    >>> config = nlm.createDefaultConfig()
    >>> config.set("brain.numRegions", 2)
    >>> brain = nlm.createBrain(config)
    >>> brain.initialize()
    >>> 
    >>> # Add regions with populations
    >>> region1 = brain.addRegion("Sensory")
    >>> pop1 = brain.addPopulation(region1, 100, "Excitatory")
    >>> region2 = brain.addRegion("Motor")
    >>> pop2 = brain.addPopulation(region2, 50, "Motor")
    >>> 
    >>> # Access subsystems
    >>> spike_system = brain.getSpikeSystem()
    >>> stdp = brain.getSTDP()
    >>> dopamine = brain.getDopamine()
    >>> curiosity = brain.getCuriosity()
    >>> 
    >>> # Run simulation
    >>> brain.step(0, 0.0)
    >>> percept = world.getSensoryPercept()
    >>> brain.receiveSensoryInput(percept)
    >>> action = brain.produceAction()
)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
Create a brain with configuration.

Args:
    config: Configuration object containing brain parameters

Example:
    >>> config = nlm.createDefaultConfig()
    >>> brain = nlm.Brain(config)
)pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
Initialize the brain with configuration.

This method sets up the neural network structure, allocates memory for
neurons and synapses, and initializes all subsystems (plasticity, 
neuromodulation, memory, cognition) according to configuration.

Example:
    >>> brain.initialize()  # Complete initialization
)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
Perform a simulation step with current step index.

Args:
    currentStep: Integer step index for simulation tracking

Example:
    >>> for step in range(1000):
    ...     brain.step(step)
    ...     # Process inputs and produce actions
)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
Perform a simulation step with timestamp.

Args:
    currentStep: Integer step index for simulation tracking
    currentTime: Absolute simulation time in seconds

Example:
    >>> brain.step(step, step * 0.01)  # 10ms timesteps
)pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
Inject sensory input into the brain.

Args:
    input: SensoryInput object containing environmental data

Example:
    >>> percept = world.getSensoryPercept()
    >>> brain.receiveSensoryInput(percept)
)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
Inject current directly into a specific neuron.

Args:
    neuron: Target neuron ID
    current: Current value to inject (positive = depolarizing)

Example:
    >>> neuron_id = nlm.INVALID_NEURON_ID  # Would need valid ID
    >>> brain.injectCurrent(neuron_id, 5.0)
)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
Inject current into all neurons of a specific type.

Args:
    type: Neuron type (Excitatory, Inhibitory, etc.)
    current: Current value to inject

Example:
    >>> brain.injectCurrentToNeurons(nlm.NeuronType.Excitatory, 2.0)
)pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
Produce motor action based on neural activity.

Returns:
    Action object representing the generated motor output

Example:
    >>> action = brain.produceAction()
    >>> print(f"Action: {action.getName()}, Type: {action.getType()}")
)pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
Reset brain state to initial conditions.

Example:
    >>> brain.reset()  # Clear all neural activity and state
)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
Save brain state to file (checkpointing).

Args:
    filepath: Path where brain state will be saved

Example:
    >>> brain.save("brain_checkpoint.nlm")
    >>> # Later...
    >>> new_brain = nlm.Brain(nlm.createDefaultConfig())
    >>> new_brain.load("brain_checkpoint.nlm")
)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
Load brain state from file.

Args:
    filepath: Path to brain state file

Example:
    >>> brain = nlm.Brain(nlm.createDefaultConfig())
    >>> brain.load("brain_checkpoint.nlm")
)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
Add a new neural region.

Args:
    name: Optional name for the region (for debugging)

Returns:
    RegionId of the newly created region

Example:
    >>> sensory_region = brain.addRegion("Sensory")
    >>> motor_region = brain.addRegion("Motor")
)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
Get a region by ID.

Args:
    id: RegionId of the region to retrieve

Returns:
    NeuralRegion object
)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get the number of regions.)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.)pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get all regions.)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage.)pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the configuration.)pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status and statistics.)pbdoc");

    // ===== AGENT BRAIN CLASS =====
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
Agent brain interface connecting NLM brain to world interaction.

AgentBrain bridges the gap between the neural brain and the external
environment by providing high-level interfaces for perception-action
loop processing. It handles sensory input processing, motor command
decoding, and neuromodulation integration, making it suitable for
embodied AI and reinforcement learning applications.

Key Features:
- Sensory perception processing and injection into brain
- Motor command decoding from neural activity
- Reward-modulated learning with dopamine signals
- Curiosity-driven exploration
- Developmental stage management
- Neuromodulation system integration

AgentBrain maintains internal state for novelty detection, prediction,
and curiosity, providing a complete agent-centered interface for neural
simulation in interactive environments.

Example Usage:
    >>> import nlm
    >>> 
    >>> # Setup agent in world
    >>> config = nlm.createDefaultConfig()
    >>> brain = nlm.createBrain(config)
    >>> brain.initialize()
    >>> world = nlm.createSimpleWorld()
    >>> agent_brain = nlm.createAgentBrain(brain)
    >>> agent_brain.initialize(world)
    >>> world.setAgentStart(50, 50)
    >>> 
    >>> # Main agent loop
    >>> for episode in range(100):
    ...     world.reset()
    ...     agent_brain.reset()
    ...     
    ...     # Reset world and agent
    ...     # This part would continue with environment interaction
    ...     # including perception, action, learning, and reward
)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
Create an agent brain with underlying neural brain.

Args:
    brain: The Brain instance to control

Example:
    >>> brain = nlm.createBrain(nlm.createDefaultConfig())
    >>> agent_brain = nlm.AgentBrain(brain)
)pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
Initialize agent brain with world reference.

Args:
    world: SimpleWorld instance containing the environment

Example:
    >>> world = nlm.createSimpleWorld()
    >>> agent_brain.initialize(world)
)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
Process sensory percept and inject into brain.

Args:
    percept: SensoryPercept from world containing all sensory data

Example:
    >>> percept = world.getSensoryPercept()
    >>> agent_brain.processSensoryInput(percept)
)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
Decode brain motor activity into motor command.

Returns:
    MotorCommand based on neural activity patterns

Example:
    >>> action = agent_brain.decodeMotorCommand()
    >>> world.applyMotorCommand(action, current_time)
)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
Apply reward-based neuromodulation to learning.

Args:
    reward: Actual reward received from environment
    predictedReward: Expected reward based on current policy

Example:
    >>> agent_brain.applyRewardModulation(reward=1.0, predicted_reward=0.5)
)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
Update development system based on elapsed time.

Args:
    timestep: Time step duration in seconds

Example:
    >>> agent_brain.updateDevelopment(0.01)
)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
Reset agent for new episode.

Example:
    >>> agent_brain.reset()  # Start fresh for new episode
)pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(Get the underlying brain.)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(Enable or disable reward modulation.)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(Enable or disable structural plasticity.)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(Enable or disable developmental processes.)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(Enable or disable curiosity-driven exploration.)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.)pbdoc");

    // ===== LEARNING MECHANISM BINDINGS =====

    // ----- STDP CLASS -----
    py::class_<STDP>(m, "STDP", R"pbdoc(
Spike-Timing-Dependent Plasticity (STDP) learning rule.

STDP is a biologically-inspired synaptic learning rule that strengthens
or weakens synapses based on the precise timing of pre- and postsynaptic
spikes. This implements the fundamental Hebbian learning principle where
"neurons that fire together, wire together," but with precise temporal
constraints.

STDP mechanisms:
- Long-Term Potentiation (LTP): Pre-before-post spikes strengthen synapses
- Long-Term Depression (LTD): Post-before-pre spikes weaken synapses
- Time Constant: Controls the window of temporal influence

Key Parameters:
- ltpWeight: LTP magnitude (typically 0.01-0.1)
- ltdWeight: LTD magnitude (typically 0.01-0.12)
- timeConstant: Temporal window in milliseconds (typically 10-50ms)

STDP is essential for temporal sequence learning and memory formation
in spiking neural networks.

Example Configuration:
    >>> config = nlm.createDefaultConfig()
    >>> config.set("plasticity.stdp.enable", True)
    >>> config.set("plasticity.stdp.ltpWeight", 0.01)
    >>> config.set("plasticity.stdp.ltdWeight", 0.012)
    >>> config.set("plasticity.stdp.timeConstant", 20.0)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> stdp = brain.getSTDP()
    >>> if stdp:
    ...     print(f"LTP Weight: {stdp.getLTPWeight()}")
    ...     print(f"LTD Weight: {stdp.getLTDWeight()}")
    ...     print(f"Time Constant: {stdp.getTimeConstant()} ms")
)pbdoc")
        .def(py::init<>())
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"),
             R"pbdoc(
Set the Long-Term Potentiation (LTP) weight.

LTP strengthens synapses when presynaptic spikes precede postsynaptic spikes.
The weight magnitude determines the strength of potentiation.

Args:
    weight: LTP weight magnitude (typically 0.01-0.1)

Example:
    >>> stdp = nlm.STDP()
    >>> stdp.setLTPWeight(0.01)
)pbdoc")
        .def("getLTPWeight", &STDP::getLTPWeight,
             R"pbdoc(Get the Long-Term Potentiation (LTP) weight.)pbdoc")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"),
             R"pbdoc(
Set the Long-Term Depression (LTD) weight.

LTD weakens synapses when postsynaptic spikes precede presynaptic spikes.
The weight magnitude determines the strength of depression.

Args:
    weight: LTD weight magnitude (typically 0.01-0.12)

Example:
    >>> stdp.setLTDWeight(0.012)
)pbdoc")
        .def("getLTDWeight", &STDP::getLTDWeight,
             R"pbdoc(Get the Long-Term Depression (LTD) weight.)pbdoc")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"),
             R"pbdoc(
Set the STDP time constant.

The time constant controls the temporal window for spike timing-dependent
plasticity effects. Larger values allow for longer temporal gaps between
pre- and postsynaptic spikes to have effects.

Args:
    tau: Time constant in milliseconds (typically 10-50ms)

Example:
    >>> stdp.setTimeConstant(20.0)
)pbdoc")
        .def("getTimeConstant", &STDP::getTimeConstant,
             R"pbdoc(Get the STDP time constant in milliseconds.)pbdoc")
        .def("configure", &STDP::configure,
             py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             R"pbdoc(
Configure STDP with multiple parameters at once.

Args:
    ltpWeight: Long-Term Potentiation weight
    ltdWeight: Long-Term Depression weight
    tau: Time constant in milliseconds

Example:
    >>> stdp.configure(0.01, 0.012, 20.0)
)pbdoc")
        .def("getName", &STDP::getName,
             R"pbdoc(Get the rule name (always "STDP").)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled,
             R"pbdoc(Check if STDP rule is enabled.)pbdoc")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"),
             R"pbdoc(Enable or disable STDP.)pbdoc");

    // ----- HEBBIAN CLASS -----
    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(
Hebbian learning rule implementing the classic "neurons that fire together,
wire together" principle.

Hebbian learning strengthens synapses when presynaptic and postsynaptic
neurons fire in close temporal proximity, regardless of spike timing order.
This implements a symmetric, additive form of synaptic plasticity that
captures the essence of associative learning.

Hebbian mechanisms:
- Weight Increase: Co-activation of neurons increases synaptic strength
- Weight Decrease: Lack of co-activation can lead to weight decay
- Learning Rate: Controls the magnitude of weight changes
- Max Weight: Sets an upper bound for synaptic strength

Hebbian learning is fundamental for pattern completion, memory formation,
and clustering in neural networks.

Example Configuration:
    >>> config = nlm.createDefaultConfig()
    >>> config.set("plasticity.hebbian.enable", True)
    >>> config.set("plasticity.hebbian.learningRate", 0.01)
    >>> config.set("plasticity.hebbian.maxWeight", 1.0)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> hebbian = brain.getHebbian()
    >>> if hebbian:
    ...     print(f"Learning Rate: {hebbian.getLearningRate()}")
    ...     print(f"Max Weight: {hebbian.getMaxWeight()}")
)pbdoc")
        .def(py::init<>())
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"),
             R"pbdoc(
Set the learning rate for Hebbian updates.

The learning rate controls how much synaptic weights change in response
to co-activation. Higher values lead to faster learning but risk
instability.

Args:
    rate: Learning rate (typically 0.001-0.1)

Example:
    >>> hebbian.setLearningRate(0.01)
)pbdoc")
        .def("getLearningRate", &Hebbian::getLearningRate,
             R"pbdoc(Get the Hebbian learning rate.)pbdoc")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"),
             R"pbdoc(
Set the maximum synaptic weight allowed.

This provides a bounds on synaptic strength, preventing runaway excitation
and ensuring network stability.

Args:
    maxWeight: Maximum weight value (typically 0.5-2.0)

Example:
    >>> hebbian.setMaxWeight(1.0)
)pbdoc")
        .def("getMaxWeight", &Hebbian::getMaxWeight,
             R"pbdoc(Get the maximum synaptic weight.)pbdoc")
        .def("getName", &Hebbian::getName,
             R"pbdoc(Get the rule name (always "Hebbian").)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled,
             R"pbdoc(Check if Hebbian rule is enabled.)pbdoc")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"),
             R"pbdoc(Enable or disable Hebbian learning.)pbdoc");

    // ----- SPIKE SYSTEM CLASS -----
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(
Event-driven spike processing system for spiking neural networks.

SpikeSystem implements an efficient, event-based computation model where
spike events are processed asynchronously rather than in synchronized steps.
This enables more biologically realistic timing and allows for variable
timing between spikes, providing advantages for temporal processing and
energy efficiency.

Key Features:
- Spike Event Queue: Processes spikes in chronological order
- Delayed Spike Delivery: Handles synaptic transmission delays
- Callback System: Register handlers for spike and delayed spike events
- Statistics: Track spike counts, rates, and activity patterns
- Memory: Maintain spike history for analysis and learning

SpikeSystem is essential for implementing real-time neural processing
and event-driven dynamics in spiking neural networks.

Example Usage:
    >>> config = nlm.createDefaultConfig()
    >>> brain = nlm.createBrain(config)
    >>> brain.initialize()
    >>> spike_system = brain.getSpikeSystem()
    >>> if spike_system:
    ...     # Register spike handler
    ...     def handle_spike(event):
    ...         print(f"Spike at {event.timestamp} from neuron {event.source}")
    ...     
    ...     spike_system.registerHandler(handle_spike)
    ...     
    ...     # Process simulation steps
    ...     for step in range(100):
    ...         brain.step(step)
    ...         # Spikes are automatically processed and delivered
    ...     
    ...     # Get statistics
    ...     print(spike_system.debug())
    ...     print(f"Total spikes: {spike_system.getSpikeCount()}")
    ...     print(f"Average rate: {spike_system.getAverageSpikeRate()} Hz")
)pbdoc")
        .def(py::init<>())
        .def("queueSpike", [](SpikeSystem& self, const SpikeEvent& event) {
            self.queueSpike(event);
        }, py::arg("event"),
             R"pbdoc(
Queue a spike event for processing.

Args:
    event: SpikeEvent containing source neuron and timing information

Example:
    >>> event = nlm.SpikeEvent(neuron_id, timestamp, step)
    >>> spike_system.queueSpike(event)
)pbdoc")
        .def("queueSpike", [](SpikeSystem& self, NeuronId neuron, Timestamp timestamp, SimulationStep step) {
            self.queueSpike(SpikeEvent(neuron, timestamp, step));
        }, py::arg("neuron"), py::arg("timestamp"), py::arg("step"),
             R"pbdoc(
Queue a spike from a specific neuron.

Args:
    neuron: Neuron ID that spiked
    timestamp: Time when spike occurred
    step: Simulation step when spike occurred

Example:
    >>> spike_system.queueSpike(neuron_id, time, step)
)pbdoc")
        .def("queueDelayedSpike", &SpikeSystem::queueDelayedSpike, py::arg("event"),
             R"pbdoc(
Queue a delayed spike event for synaptic transmission.

Args:
    event: DelayedSpikeEvent with delivery timing information

Example:
    >>> delayed_event = nlm.DelayedSpikeEvent(src, dst, syn_id, weight, type, timestamp, delivery_time, step, delivery_step)
    >>> spike_system.queueDelayedSpike(delayed_event)
)pbdoc")
        .def("processSpikes", &SpikeSystem::processSpikes, py::arg("currentStep"),
             R"pbdoc(
Process all queued spikes for a given simulation step.

Args:
    currentStep: Current simulation step index
)pbdoc")
        .def("processDelayedSpikes", &SpikeSystem::processDelayedSpikes,
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
Process pending delayed spikes that should be delivered now.

Args:
    currentStep: Current simulation step index
    currentTime: Current simulation time
)pbdoc")
        .def("registerHandler", &SpikeSystem::registerHandler, py::arg("handler"),
             R"pbdoc(
Register a callback handler for spike events.

Args:
    handler: Function that will be called for each spike event
)pbdoc")
        .def("registerDelayedHandler", &SpikeSystem::registerDelayedHandler, py::arg("handler"),
             R"pbdoc(
Register a callback handler for delayed spike events.

Args:
    handler: Function that will be called for each delayed spike event
)pbdoc")
        .def("getSpikeHistory", &SpikeSystem::getSpikeHistory,
             R"pbdoc(
Get the spike history.

Returns:
    List of DetailedSpikeEvent objects representing all recorded spikes
)pbdoc")
        .def("clearHistory", &SpikeSystem::clearHistory,
             R"pbdoc(
Clear the spike history.

Example:
    >>> spike_system.clearHistory()
)pbdoc")
        .def("getSpikeCount", &SpikeSystem::getSpikeCount,
             R"pbdoc(Get total number of spikes processed.)pbdoc")
        .def("getPendingSpikeCount", &SpikeSystem::getPendingSpikeCount,
             R"pbdoc(Get number of queued but unprocessed spikes.)pbdoc")
        .def("getPendingDelayedCount", &SpikeSystem::getPendingDelayedCount,
             R"pbdoc(Get number of queued but unprocessed delayed spikes.)pbdoc")
        .def("getAverageSpikeRate", &SpikeSystem::getAverageSpikeRate,
             R"pbdoc(Get average spike rate across all neurons.)pbdoc")
        .def("getMostActiveNeurons", &SpikeSystem::getMostActiveNeurons, py::arg("count"),
             R"pbdoc(
Get the most active neurons based on spike count.

Args:
    count: Number of neurons to return

Returns:
    List of NeuronId objects for the most active neurons
)pbdoc")
        .def("reset", &SpikeSystem::reset,
             R"pbdoc(
Reset the spike system.

Example:
    >>> spike_system.reset()
)pbdoc")
        .def("debug", [](const SpikeSystem& self) {
            std::stringstream ss;
            ss << "=== Spike System Debug ===" << std::endl;
            ss << "Total spikes: " << self.getSpikeCount() << std::endl;
            ss << "Pending spikes: " << self.getPendingSpikeCount() << std::endl;
            ss << "Pending delayed: " << self.getPendingDelayedCount() << std::endl;
            ss << "Average rate: " << std::fixed << std::setprecision(3) 
               << self.getAverageSpikeRate() << " Hz" << std::endl;
            
            auto mostActive = self.getMostActiveNeurons(5);
            if (!mostActive.empty()) {
                ss << "Most active neurons: ";
                for (const auto& id : mostActive) {
                    ss << id.index() << " ";
                }
                ss << std::endl;
            }
            
            ss << "=" << std::endl;
            return ss.str();
        }, R"pbdoc(
Get detailed debug information about the spike system.

Returns:
    Formatted string with spike system statistics

Example:
    >>> print(spike_system.debug())
)pbdoc")
        .def("registerHandler", [](SpikeSystem& self, const py::function& handler) {
            self.registerHandler([handler](const DetailedSpikeEvent& event) {
                // Convert DetailedSpikeEvent to Python dict
                py::dict result;
                result["source"] = event.source;
                result["timestamp"] = event.timestamp;
                result["step"] = event.step;
                result["regionId"] = event.regionId;
                result["populationId"] = event.populationId;
                handler(result);
            });
        }, py::arg("handler"),
             R"pbdoc(
Register a Python function as a spike event handler.

Args:
    handler: Python function that takes a dict with spike event details

Example:
    >>> def spike_handler(event):
    ...     print(f"Spike from neuron {event['source']} at time {event['timestamp']}")
    >>> spike_system.registerHandler(spike_handler)
)pbdoc");

    // ----- PLASTICITY RULE BASE CLASS -----
    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(
Abstract base class for all synaptic plasticity rules.

PlasticityRule defines the interface for synaptic learning mechanisms
that modify connection strengths based on neural activity. Subclasses
implement specific learning algorithms like STDP, Hebbian, and others.

All plasticity rules follow a common interface for:
- Detecting pre- and postsynaptic activity patterns
- Computing weight changes based on activity
- Applying those changes to synapses
- Managing rule-specific parameters and configuration

PlasticityRule provides common functionality for:
- Enabling/disabling the rule
- Getting the rule name
- Basic state management

This abstract base class enables polymorphic handling of different
plasticity mechanisms within the neural network.

Example Polymorphic Usage:
    >>> config = nlm.createDefaultConfig()
    >>> config.set("plasticity.stdp.enable", True)
    >>> config.set("plasticity.hebbian.enable", True)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> # Get plasticity rules as base class
    >>> plasticity = brain.getSTDP()  # or brain.getHebbian()
    >>> print(f"Rule type: {plasticity.getName()}")
    >>> print(f"Enabled: {plasticity.isEnabled()}")
)pbdoc")
        .def("update", [](PlasticityRule& self, const py::dict& synapseInfo,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
            // For Python interface, we'll need to adapt this
            // This is a simplified version for demonstration
            throw NLMError("PlasticityRule.update requires C++ implementation details");
        }, py::arg("synapseInfo"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             R"pbdoc(
Update synaptic weights based on pre- and postsynaptic activity.

This is an abstract method that must be implemented by subclasses.
It computes weight changes based on spike timing and synaptic state.

Args:
    synapseInfo: Dictionary containing synapse information
    preSpikes: List of presynaptic spike times
    postSpikes: List of postsynaptic spike times
    dt: Time step duration

Example:
    >>> # This method is implemented by concrete subclasses
    >>> # STDP, Hebbian, etc. all override this method
)pbdoc")
        .def("applyWeightChange", [](PlasticityRule& self, const py::dict& synapseInfo,
                                   SynapticWeight delta) {
            // For Python interface, we'll need to adapt this
            // This is a simplified version for demonstration
            throw NLMError("PlasticityRule.applyWeightChange requires C++ implementation details");
        }, py::arg("synapseInfo"), py::arg("delta"),
             R"pbdoc(
Apply a weight change to a synapse.

This is an abstract method that must be implemented by subclasses.
It directly modifies the synaptic weight by the specified amount.

Args:
    synapseInfo: Dictionary containing synapse information
    delta: Weight change amount (positive or negative)

Example:
    >>> # This method is implemented by concrete subclasses
)pbdoc")
        .def("getName", &PlasticityRule::getName,
             R"pbdoc(Get the name of this plasticity rule.)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled,
             R"pbdoc(Check if the plasticity rule is enabled.)pbdoc")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"),
             R"pbdoc(Enable or disable the plasticity rule.)pbdoc");

    // ----- NEUROMODULATION CLASSES -----

    // ----- NEUROMODULATOR BASE CLASS -----
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(
Abstract base class for neuromodulatory signals in the neural network.

Neuromodulators are chemicals or electrical signals that affect neural
activity broadly across the network, modulating learning, attention,
arousal, and other global brain states. They provide a way to globally
regulate neural computation based on internal and external contexts.

Common neuromodulators:
- Dopamine: Reward prediction error, reinforcement learning
- Acetylcholine: Attention, memory consolidation
- Norepinephrine: Arousal, vigilance
- Serotonin: Mood, impulsivity, social behavior

Neuromodulators affect plasticity by providing multiplicative factors
that scale synaptic weight changes, and they influence neuronal excitability
and firing patterns globally.

Example Usage:
    >>> config = nlm.createDefaultConfig()
    >>> config.set("neuromodulation.dopamine.enable", True)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> dopamine = brain.getDopamine()
    >>> if dopamine:
    ...     print(f"Dopamine level: {dopamine.getLevel()}")
    ...     print(f"Plasticity factor: {dopamine.getPlasticityFactor()}")
    ...     # Apply neuromodulatory effects
    ...     dopamine.setLevel(0.5)
)pbdoc")
        .def("getName", &Neuromodulator::getName,
             R"pbdoc(Get the neuromodulator name.)pbdoc")
        .def("getLevel", &Neuromodulator::getLevel,
             R"pbdoc(Get the current concentration/level of neuromodulator.)pbdoc")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             R"pbdoc(Set the concentration/level of neuromodulator.)pbdoc")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             R"pbdoc(Get the current plasticity modulation factor.)pbdoc")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             R"pbdoc(Update neuromodulator state based on simulation timestep.)pbdoc");

    // ----- DOPAMINE NEUROMODULATOR -----
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(
Dopamine neuromodulator for reward prediction error and reinforcement learning.

Dopamine is a key neuromodulator in reinforcement learning that signals
prediction errors - the difference between expected and actual rewards.
This signal modulates synaptic plasticity, strengthening connections that
led to positive outcomes and weakening those that didn't.

Dopamine mechanisms:
- Reward signaling: Response to actual rewards
- Prediction error: Response to unexpected outcomes
- Plasticity modulation: Scales synaptic learning rates
- Learning rate adaptation: Adjusts based on prediction accuracy

Dopamine level ranges from -1.0 (punishment) to +1.0 (reward), with
intermediate values representing prediction errors.

Example Usage:
    >>> import nlm
    >>> 
    >>> # Create and configure dopamine system
    >>> config = nlm.createDefaultConfig()
    >>> config.set("neuromodulation.dopamine.enable", True)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> dopamine = brain.getDopamine()
    >>> if dopamine:
    ...     # Initialize with brain reference
    ...     dopamine.initialize(brain)
    ...     
    ...     # Signal reward
    ...     dopamine.signalReward(1.0)
    ...     print(f"Dopamine level: {dopamine.getLevel()}")
    ...     
    ...     # Signal prediction error
    ...     dopamine.signalRewardPredictionError(0.5)
    ...     print(f"Dopamine after prediction error: {dopamine.getLevel()}")
    ...     
    ...     # Apply plasticity factor
    ...     plasticity_factor = dopamine.getPlasticityFactor()
    ...     print(f"Plasticity factor: {plasticity_factor}")
)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             R"pbdoc(
Signal a reward to the dopamine system.

This updates the dopamine level based on the received reward, contributing
to reinforcement learning and reward prediction error computation.

Args:
    reward: Reward value (positive = good outcome, negative = bad outcome)

Example:
    >>> dopamine.signalReward(1.0)  # Positive reward
    >>> dopamine.signalReward(-0.5)  # Negative punishment
)pbdoc")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             R"pbdoc(
Signal a reward prediction error to the dopamine system.

This represents the difference between expected and actual rewards,
which is a fundamental component of reinforcement learning algorithms.

Args:
    error: Prediction error (positive = better than expected, negative = worse)

Example:
    >>> dopamine.signalRewardPredictionError(0.3)  # Got more reward than expected
    >>> dopamine.signalRewardPredictionError(-0.2)  # Got less reward than expected
)pbdoc");

    // ----- CURIOUSY NEUROMODULATOR -----
    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(
Curiosity neuromodulator for exploration motivation and intrinsic reward.

Curiosity represents the drive to explore novel and interesting states,
driven by a combination of novelty detection and prediction error. It
promotes exploration behavior and helps agents discover valuable states
that weren't anticipated.

Curiosity mechanisms:
- Novelty detection: Response to unexpected sensory input
- Prediction error: Response to state prediction inaccuracies
- Exploration drive: Motivation to seek novel experiences
- Balanced integration: Combines multiple sources of motivation

Curiosity level typically ranges from 0.0 (no curiosity) to 1.0 (high
curiosity), influencing the balance between exploration and exploitation.

Example Usage:
    >>> import nlm
    >>> 
    >>> # Create and configure curiosity system
    >>> config = nlm.createDefaultConfig()
    >>> config.set("neuromodulation.curiosity.enable", True)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> curiosity = brain.getCuriosity()
    >>> if curiosity:
    ...     # Initialize with brain reference
    ...     curiosity.initialize(brain)
    ...     
    ...     # Update based on environment state
    ...     curiosity.update(novelty=0.7, prediction_error=-0.2, dt=0.01)
    ...     
    ...     # Get motivation levels
    ...     level = curiosity.getLevel()
    ...     exploration = curiosity.getExplorationDrive()
    ...     print(f"Curiosity level: {level}")
    ...     print(f"Exploration drive: {exploration}")
)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             R"pbdoc(
Initialize curiosity system with brain reference.

Args:
    brain: Pointer to the brain instance

Example:
    >>> curiosity.initialize(brain)
)pbdoc")
        .def("getLevel", &Curiosity::getLevel,
             R"pbdoc(Get current curiosity level.)pbdoc")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             R"pbdoc(Get exploration drive (same as level).)pbdoc")
        .def("update", &Curiosity::update,
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             R"pbdoc(
Update curiosity based on novelty and prediction error.

Args:
    novelty: Novelty level (0.0 = familiar, 1.0 = novel)
    predictionError: Prediction error magnitude
    dt: Time step duration

Example:
    >>> curiosity.update(novelty=0.7, prediction_error=-0.2, dt=0.01)
)pbdoc")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             R"pbdoc(
Set weight for novelty contribution to curiosity.

Args:
    weight: Weight for novelty (typically 0.5-1.0)
)pbdoc")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             R"pbdoc(
Set weight for prediction error contribution to curiosity.

Args:
    weight: Weight for prediction error (typically 0.5-1.0)
)pbdoc")
        .def("reset", &Curiosity::reset,
             R"pbdoc(
Reset curiosity system.

Example:
    >>> curiosity.reset()
)pbdoc");

    // ----- NOVELTY DETECTION CLASS -----
    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(
Novelty detection system for identifying novel sensory patterns.

Novelty represents the degree to which sensory input differs from
previous observations or expected patterns. It serves as a key driver
of curiosity, exploration, and learning in dynamic environments.

Novelty mechanisms:
- Pattern comparison: Compare current input with recent history
- Decay mechanism: Novelty fades over time
- Thresholding: Convert differences to novelty scores
- History tracking: Maintain recent sensory patterns for comparison

Novelty levels range from 0.0 (completely familiar) to 1.0 (very novel),
providing a continuous measure of environmental change and unexpectedness.

Example Usage:
    >>> import nlm
    >>> 
    >>> # Create and configure novelty detection
    >>> config = nlm.createDefaultConfig()
    >>> config.set("neuromodulation.novelty.enable", True)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> novelty = brain.getNovelty()
    >>> if novelty:
    ...     # Initialize with brain reference
    ...     novelty.initialize(brain)
    ...     
    ...     # Detect novelty from observations
    ...     current_obs = brain.getCurrentObservation()
    ...     previous_obs = brain.getPreviousObservation()
    ...     novelty.detectNovelty(current_obs, previous_obs)
    ...     
    ...     # Get novelty levels
    ...     level = novelty.getLevel()
    ...     print(f"Novelty level: {level}")
    ...     
    ...     # Update decay
    ...     novelty.update(dt=0.01)
    ...     print(f"Novelty after decay: {novelty.getLevel()}")
)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             R"pbdoc(
Initialize novelty detection system with brain reference.

Args:
    brain: Pointer to the brain instance

Example:
    >>> novelty.initialize(brain)
)pbdoc")
        .def("getLevel", &Novelty::getLevel,
             R"pbdoc(Get current novelty level.)pbdoc")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             R"pbdoc(Set novelty level directly.)pbdoc")
        .def("detectNovelty", [](Novelty& self, const py::dict& currentObservation,
                               const py::dict& previousObservation) {
            // For Python interface - would need to adapt
            throw NLMError("Novelty.detectNovelty requires C++ implementation details for observation objects");
        }, py::arg("currentObservation"), py::arg("previousObservation"),
             R"pbdoc(
Detect novelty from observation comparison.

Args:
    currentObservation: Current sensory observation
    previousObservation: Previous sensory observation

Example:
    >>> novelty.detectNovelty(current_obs, previous_obs)
)pbdoc")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& currentPattern,
                               const std::vector<float>& previousPattern) {
            // This is an actual C++ method that can be called
            self.detectNovelty(currentPattern, previousPattern);
        }, py::arg("currentPattern"), py::arg("previousPattern"),
             R"pbdoc(
Detect novelty from sensory patterns.

Args:
    currentPattern: Current sensory input pattern
    previousPattern: Previous sensory input pattern

Example:
    >>> import nlm
    >>> current = [0.1, 0.2, 0.3, 0.4]
    >>> previous = [0.0, 0.2, 0.3, 0.5]
    >>> novelty.detectNovelty(current, previous)
)pbdoc")
        .def("update", &Novelty::update, py::arg("dt"),
             R"pbdoc(
Decay novelty over time.

Args:
    dt: Time step duration

Example:
    >>> novelty.update(dt=0.01)
)pbdoc")
        .def("getHistory", &Novelty::getHistory,
             R"pbdoc(Get novelty history.)pbdoc")
        .def("clearHistory", &Novelty::clearHistory,
             R"pbdoc(
Clear novelty history.

Example:
    >>> novelty.clearHistory()
)pbdoc");

    // ----- REWARD NEUROMODULATOR -----
    py::class_<Reward, Neuromodulator>(m, "Reward", R"pbdoc(
Reward neuromodulator for reinforcement learning signals.

Reward represents the immediate value of outcomes in the environment,
serving as the primary driver of reinforcement learning and reward-based
plasticity. It complements dopamine prediction error signals with
actual experienced value.

Reward mechanisms:
- Value accumulation: Sum rewards over time
- Reset capability: Clear accumulated value
- State-based computation: Compute rewards from environment state
- History tracking: Maintain reward history for analysis

Reward values can be positive (beneficial outcomes) or negative
(punishment), driving learning toward beneficial behaviors and away
from harmful ones.

Example Usage:
    >>> import nlm
    >>> 
    >>> # Create and configure reward system
    >>> config = nlm.createDefaultConfig()
    >>> config.set("neuromodulation.reward.enable", True)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> reward = brain.getReward()
    >>> if reward:
    ...     # Get current accumulated reward
    ...     value = reward.getValue()
    ...     print(f"Current reward: {value}")
    ...     
    ...     # Add to reward
    ...     reward.add(1.0)  # Positive outcome
    ...     reward.add(-0.5)  # Negative outcome
    ...     print(f"Reward after updates: {reward.getValue()}")
    ...     
    ...     # Reset reward
    ...     reward.reset()
)pbdoc")
        .def(py::init<>())
        .def("getValue", &Reward::getValue,
             R"pbdoc(Get current accumulated reward value.)pbdoc")
        .def("setValue", &Reward::setValue, py::arg("value"),
             R"pbdoc(Set accumulated reward value directly.)pbdoc")
        .def("add", &Reward::add, py::arg("delta"),
             R"pbdoc(
Add to accumulated reward.

Args:
    delta: Reward value to add (positive or negative)

Example:
    >>> reward.add(1.0)  # Good outcome
    >>> reward.add(-0.5)  # Bad outcome
)pbdoc")
        .def("reset", &Reward::reset,
             R"pbdoc(
Reset accumulated reward to zero.

Example:
    >>> reward.reset()
)pbdoc")
        .def("getHistory", &Reward::getHistory,
             R"pbdoc(Get reward history.)pbdoc")
        .def("clearHistory", &Reward::clearHistory,
             R"pbdoc(
Clear reward history.

Example:
    >>> reward.clearHistory()
)pbdoc");

    // ----- PREDICTION ERROR NEUROMODULATOR -----
    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(
Prediction error neuromodulator for learning from mismatches.

Prediction error represents the difference between predicted and actual
values, driving learning and adaptation. It signals when predictions
are incorrect, prompting the system to update its internal models
and improve future predictions.

Prediction error mechanisms:
- Error computation: Calculate difference between predicted and actual
- Prediction update: Adjust predictions based on errors
- Magnitude calculation: Quantify error for learning modulation
- History tracking: Maintain error history for analysis

Prediction error values can be positive (underprediction) or negative
(overprediction), driving learning in the appropriate direction.

Example Usage:
    >>> import nlm
    >>> 
    >>> # Create and configure prediction error system
    >>> config = nlm.createDefaultConfig()
    >>> config.set("neuromodulation.predictionError.enable", True)
    >>> 
    >>> brain = nlm.createBrain(config)
    >>> pred_error = brain.getPredictionErrorSignal()
    >>> if pred_error:
    ...     # Initialize with brain reference
    ...     pred_error.initialize(brain)
    ...     
    ...     # Compute error
    ...     pred_error.computeError(predicted=0.5, actual=0.8)
    ...     error = pred_error.getError()
    ...     print(f"Prediction error: {error}")
    ...     
    ...     # Update prediction
    ...     pred_error.updatePrediction(0.8)
    ...     
    ...     # Get magnitude
    ...     magnitude = pred_error.getMagnitude()
    ...     print(f"Error magnitude: {magnitude}")
)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             R"pbdoc(
Initialize prediction error system with brain reference.

Args:
    brain: Pointer to the brain instance

Example:
    >>> pred_error.initialize(brain)
)pbdoc")
        .def("getError", &PredictionError::getError,
             R"pbdoc(Get current prediction error value.)pbdoc")
        .def("computeError", &PredictionError::computeError,
             py::arg("predicted"), py::arg("actual"),
             R"pbdoc(
Compute prediction error between predicted and actual values.

Args:
    predicted: Expected value
    actual: Actual observed value

Example:
    >>> pred_error.computeError(predicted=0.5, actual=0.8)  # Got better than expected
    >>> pred_error.computeError(predicted=0.8, actual=0.5)  # Got worse than expected
)pbdoc")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             R"pbdoc(
Update prediction based on new information.

Args:
    newPrediction: New predicted value

Example:
    >>> pred_error.updatePrediction(0.8)
)pbdoc")
        .def("getHistory", &PredictionError::getHistory,
             R"pbdoc(Get prediction error history.)pbdoc")
        .def("clearHistory", &PredictionError::clearHistory,
             R"pbdoc(
Clear prediction error history.

Example:
    >>> pred_error.clearHistory()
)pbdoc")
        .def("getMagnitude", &PredictionError::getMagnitude,
             R"pbdoc(Get magnitude of prediction error.)pbdoc");

    // ===== FACTORY FUNCTIONS =====

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::return_value_policy::automatic);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::automatic);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world", py::return_value_policy::automatic);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::automatic);

    // ===== GLOBAL CONSTANTS =====

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===== END OF BINDINGS =====
}

} // namespace nlm