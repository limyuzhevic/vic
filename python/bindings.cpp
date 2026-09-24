// Enhanced Python bindings for NLM
// This file demonstrates key improvements to the pynlm Python bindings
// focusing on better error handling, user-friendly interfaces, and comprehensive documentation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

// Include all NLM headers
#include "../src/core/Types/Types.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/SelfModel.hpp"
#include "../src/cognition/SocialLearning.hpp"
#include "../src/cognition/SpatialRepresentation.hpp"
#include "../src/cognition/TemporalRelation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/development/DevelopmentSystem.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception hierarchy for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
};

class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) 
        : NLMError("Config Error: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) 
        : NLMError("Brain Error: " + message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) 
        : NLMError("Memory Error: " + message) {}
};

class PredictionError : public NLMError {
public:
    explicit PredictionError(const std::string& message) 
        : NLMError("Prediction Error: " + message) {}
};

class CognitiveError : public NLMError {
public:
    explicit CognitiveError(const std::string& message) 
        : NLMError("Cognitive Error: " + message) {}
};

// Register custom exceptions with pybind11
void registerCustomExceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<PredictionError>(m, "PredictionError");
    py::register_exception<CognitiveError>(m, "CognitiveError");
}

// Factory function to create default config
std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

// Factory function to create brain with default config
std::shared_ptr<Brain> createBrain() {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw BrainError("Failed to initialize brain with default configuration");
    }
    return brain;
}

// Factory function to create brain from config
std::shared_ptr<Brain> createBrainFromConfig(std::shared_ptr<Config> config) {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw BrainError("Failed to initialize brain with provided configuration");
    }
    return brain;
}

// Enhanced Brain class with context manager support
py::class_<Brain, std::shared_ptr<Brain>> createBrainClass(py::module_& m) {
    return py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", 
        R"pbdoc(
        Central neural simulation brain class with integrated brain systems.
        
        This class provides access to all neural subsystems including memory,
        prediction, cognition, and neuromodulation systems. It serves as the
        main interface for controlling neural simulations.
        
        Key Features:
        - LIF neuron dynamics with event-driven spike propagation
        - Integrated working memory for transient information storage
        - Episodic memory for experience recording and replay
        - Predictive coding and anticipation
        - Cognitive functions (planning, concept formation, attention)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Developmental stages affecting plasticity
        
        The brain operates as a closed-loop system processing sensory input,
        maintaining internal state through memory systems, and producing motor
        output through action selection.
        
        Example usage:
            >>> brain = nlm.createBrain()
            >>> percept = world.observe(agent)
            >>> brain.processSensoryInput(percept)
            >>> brain.step(100, 0.1)
            >>> command = brain.produceAction()
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a brain with the given configuration.
             
             Args:
                 config: Configuration object to use for the brain
                 
             Raises:
                 BrainError: If brain initialization fails
             )pbdoc")
        .def(py::init<>(),
             R"pbdoc(
             Create a brain with default configuration.
             
             Raises:
                 BrainError: If brain initialization fails
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with current configuration.
             
             Returns:
                 bool: True if initialization succeeds, False otherwise
             
             Raises:
                 BrainError: If initialization fails
             )pbdoc")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime") = 0.0,
             R"pbdoc(
             Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time (default: 0.0)
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             Args:
                 input: Sensory input object (Vision, Audio, etc.)
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity.
             
             Returns:
                 Action object representing the selected motor command
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial conditions.
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to a file (checkpointing).
             
             Args:
                 filepath: Path to save the checkpoint file
                 
             Returns:
                 bool: True if save succeeds, False otherwise
                 
             Raises:
                 BrainError: If save operation fails
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from a file (checkpointing).
             
             Args:
                 filepath: Path to load the checkpoint file from
                 
             Returns:
                 bool: True if load succeeds, False otherwise
                 
             Raises:
                 BrainError: If load operation fails
             )pbdoc")
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             R"pbdoc(
             Get the working memory system.
             
             Returns:
                 WorkingMemory object or None if not available
             )pbdoc")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             R"pbdoc(
             Get the episodic memory system.
             
             Returns:
                 EpisodicMemory object or None if not available
             )pbdoc")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             R"pbdoc(
             Get the associative memory system.
             
             Returns:
                 AssociativeMemory object or None if not available
             )pbdoc")
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             R"pbdoc(
             Get the prediction system.
             
             Returns:
                 PredictionSystem object or None if not available
             )pbdoc")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             R"pbdoc(
             Get the prediction error signal.
             
             Returns:
                 PredictionError object or None if not available
             )pbdoc")
        // Cognitive system access
        .def("getPlanner", &Brain::getPlanner,
             R"pbdoc(
             Get the neural planner.
             
             Returns:
                 NeuralPlanner object or None if not available
             )pbdoc")
        .def("getConceptFormation", &Brain::getConceptFormation,
             R"pbdoc(
             Get the concept formation system.
             
             Returns:
                 ConceptFormation object or None if not available
             )pbdoc")
        .def("getSelfModel", &Brain::getSelfModel,
             R"pbdoc(
             Get the self-model system.
             
             Returns:
                 SelfModel object or None if not available
             )pbdoc")
        .def("getSocialLearning", &Brain::getSocialLearning,
             R"pbdoc(
             Get the social learning system.
             
             Returns:
                 SocialLearning object or None if not available
             )pbdoc")
        .def("getSpatialRepresentation", &Brain::getSpatialRepresentation,
             R"pbdoc(
             Get the spatial representation system.
             
             Returns:
                 SpatialRepresentation object or None if not available
             )pbdoc")
        .def("getTemporalRelation", &Brain::getTemporalRelation,
             R"pbdoc(
             Get the temporal relation system.
             
             Returns:
                 TemporalRelation object or None if not available
             )pbdoc")
        .def("getAttention", &Brain::getAttention,
             R"pbdoc(
             Get the attentional selection system.
             
             Returns:
                 AttentionalSelection object or None if not available
             )pbdoc")
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             R"pbdoc(
             Get the development system.
             
             Returns:
                 DevelopmentSystem object or None if not available
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get the current developmental stage.
             
             Returns:
                 DevelopmentalStage enum value
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             R"pbdoc(
             Set the developmental stage.
             
             Args:
                 stage: New developmental stage
             )pbdoc")
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             R"pbdoc(
             Get the dopamine neuromodulator.
             
             Returns:
                 Dopamine object or None if not available
             )pbdoc")
        .def("getCuriosity", &Brain::getCuriosity,
             R"pbdoc(
             Get the curiosity neuromodulator.
             
             Returns:
                 Curiosity object or None if not available
             )pbdoc")
        .def("getNovelty", &Brain::getNovelty,
             R"pbdoc(
             Get the novelty neuromodulator.
             
             Returns:
                 Novelty object or None if not available
             )pbdoc")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             R"pbdoc(
             Get the prediction error signal neuromodulator.
             
             Returns:
                 PredictionError object or None if not available
             )pbdoc")
        .def("getReward", &Brain::getReward,
             R"pbdoc(
             Get the reward neuromodulator.
             
             Returns:
                 Reward object or None if not available
             )pbdoc")
        // Statistics and monitoring
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status and statistics.
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count.
             
             Returns:
                 int: Total number of neurons
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count.
             
             Returns:
                 int: Total number of synapses
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count.
             
             Returns:
                 int: Total number of spikes
             )pbdoc")
        // Context manager support
        .def("__enter__", [](py::object self) { return self; })
        .def("__exit__", [](py::object self, py::object, py::object, py::object) {
            auto brain = self.cast<std::shared_ptr<Brain>>();
            brain->reset();
            return false;
        })
        // String representation
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) + 
                   " synapses>";
        })
        // Neural region management
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region.
             
             Args:
                 name: Optional name for the region
                 
             Returns:
                 RegionId: ID of the newly created region
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             R"pbdoc(
             Get a region by ID.
             
             Args:
                 id: Region ID
                 
             Returns:
                 NeuralRegion object or None if not found
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions.
             
             Returns:
                 int: Number of regions
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs.
             
             Returns:
                 List of RegionId objects
             )pbdoc");
}

// Enhanced SimpleWorld class
py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>> createSimpleWorldClass(py::module_& m) {
    return py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld",
        R"pbdoc(
        Simple 2D world for NLM simulation.
        
        This class implements a simple 2D world environment that provides
        sensory input to the agent and receives motor output from the agent.
        
        Key Features:
        - 2D grid-based environment
        - Vision sensor for visual perception
        - Motor commands for agent movement
        - Reward computation based on agent goals
        - Dynamic world objects (resources, hazards, walls)
        
        The world provides the embodied environment in which the agent
        learns and develops through interaction.
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 11, py::arg("visionHeight") = 11,
             R"pbdoc(
             Configure the world dimensions and sensory parameters.
             
             Args:
                 width: World width in cells
                 height: World height in cells
                 visionWidth: Vision sensor width (default: 11)
                 visionHeight: Vision sensor height (default: 11)
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
             Reset world to initial state.
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set agent starting position.
             
             Args:
                 x: Initial x coordinate
                 y: Initial y coordinate
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
             Update world state for one timestep.
             
             Args:
                 timestep: Current timestep
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply motor command to world.
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             R"pbdoc(
             Get current sensory percept from world.
             
             Returns:
                 SensoryPercept object containing sensory information
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             R"pbdoc(
             Get current agent body state.
             
             Returns:
                 AgentBody object
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add object to world.
             
             Args:
                 obj: WorldObject to add
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove object from world.
             
             Args:
                 x: Object x coordinate
                 y: Object y coordinate
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if position is valid.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 
             Returns:
                 bool: True if position is valid
             )pbdoc")
        // World configuration getters
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
             Get world width.
             
             Returns:
                 int: World width
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
             Get world height.
             
             Returns:
                 int: World height
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
             Get maximum agent energy.
             
             Returns:
                 float: Maximum energy
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
             Set maximum agent energy.
             
             Args:
                 e: New maximum energy
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
             Get energy decay rate.
             
             Returns:
                 float: Energy decay rate
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
             Set energy decay rate.
             
             Args:
                 r: New energy decay rate
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
             Get current simulation time.
             
             Returns:
                 float: Current simulation time
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set random seed for reproducible simulations.
             
             Args:
                 seed: Random seed value
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
             Get current random seed.
             
             Returns:
                 int: Current random seed
             )pbdoc")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + 
                   "x" + std::to_string(world.getHeight()) + 
                   ", timestep=" + std::to_string(world.getSimulationTime()) + ">";
        });
}

// AgentBody class
py::class_<AgentBody, std::shared_ptr<AgentBody>> createAgentBodyClass(py::module_& m) {
    return py::class_<AgentBody, std::shared_ptr<AgentBody>>(m, "AgentBody",
        R"pbdoc(
        Agent body state.
        
        This class represents the physical state of the agent in the world,
        including position, orientation, velocity, and physiological parameters.
        
        Key Attributes:
        - Position (x, y) in world coordinates
        - Orientation (heading angle in radians)
        - Velocity and angular velocity
        - Energy and health levels
        - Age and action timing
        - Movement and turning flags
        )pbdoc")
        .def(py::init<>())
        // Position and orientation
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        // Physiological state
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level")
        .def_readwrite("age", &AgentBody::age, "Age in steps")
        // Movement state
        .def_readwrite("isMoving", &AgentBody::isMoving, "Moving flag")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Turning flag")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset,
             R"pbdoc(
             Reset agent body to initial state.
             )pbdoc")
        .def("__repr__", [](const AgentBody& agentBody) {
            return "<AgentBody: (" + std::to_string(agentBody.x) + "," + 
                   std::to_string(agentBody.y) + ") heading=" + 
                   std::to_string(agentBody.orientation) + 
                   ", energy=" + std::to_string(agentBody.energy) + 
                   ", health=" + std::to_string(agentBody.health) + ">";
        });
}

// Enhanced Config class
py::class_<Config, std::shared_ptr<Config>>(m, "Config",
    R"pbdoc(
    Configuration class for NLM system.
    
    This class manages all configuration parameters for the NLM system,
    including neural parameters, simulation settings, and learning parameters.
    
    Configuration is loaded from files, command line arguments, or defaults.
    It provides access to all system parameters and allows dynamic modification.
    )pbdoc")
    .def(py::init<>())
    .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
         R"pbdoc(
         Load configuration from a JSON file.
         
         Args:
             filepath: Path to configuration file
             
         Returns:
             bool: True if successful, False otherwise
             
         Raises:
             ConfigError: If file cannot be loaded or parsed
         )pbdoc")
    .def("loadFromArgs", py::overload_cast<int, char**>(&Config::loadFromArgs),
         py::arg("argc"), py::arg("argv"),
         R"pbdoc(
         Load configuration from command line arguments.
         
         Args:
             argc: Number of command line arguments
             argv: Command line arguments array
             
         Returns:
             bool: True if successful, False otherwise
         )pbdoc")
    .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
         R"pbdoc(
         Save configuration to a JSON file.
         
         Args:
             filepath: Path to save configuration file
             
         Raises:
             ConfigError: If file cannot be saved
         )pbdoc")
    .def("has", &Config::has, py::arg("key"),
         R"pbdoc(
         Check if a configuration key exists.
         
         Args:
             key: Configuration key
             
         Returns:
             bool: True if key exists, False otherwise
         )pbdoc")
    .def("getKeys", &Config::getKeys,
         R"pbdoc(
         Get all configuration keys.
         
         Returns:
             List of all configuration key strings
         )pbdoc")
    .def("clear", &Config::clear,
         R"pbdoc(
         Clear all configuration entries.
         )pbdoc")
    .def("set", py::overload_cast<const std::string&, const int64_t&, ConfigSource>(&Config::set),
         py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default,
         R"pbdoc(
         Set a configuration value with type int64_t.
         
         Args:
             key: Configuration key
             value: Value to set
             source: Configuration source (default: Default)
         )pbdoc")
    .def("set", py::overload_cast<const std::string&, const double&, ConfigSource>(&Config::set),
         py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default,
         R"pbdoc(
         Set a configuration value with type double.
         
         Args:
             key: Configuration key
             value: Value to set
             source: Configuration source (default: Default)
         )pbdoc")
    .def("set", py::overload_cast<const std::string&, const float&, ConfigSource>(&Config::set),
         py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default,
         R"pbdoc(
         Set a configuration value with type float.
         
         Args:
             key: Configuration key
             value: Value to set
             source: Configuration source (default: Default)
         )pbdoc")
    .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
         py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Default,
         R"pbdoc(
         Set a configuration value with type string.
         
         Args:
             key: Configuration key
             value: Value to set
             source: Configuration source (default: Default)
         )pbdoc")
    .def("getOr", py::overload_cast<const std::string&, const int64_t&, const int64_t&>(&Config::getOr),
         py::arg("key"), py::arg("default_value"),
         R"pbdoc(
         Get a configuration value or default if not found.
         
         Args:
             key: Configuration key
             default_value: Default value if key not found
             
         Returns:
             int64_t: Configuration value or default
         )pbdoc")
    .def("getOr", py::overload_cast<const std::string&, const double&, const double&>(&Config::getOr),
         py::arg("key"), py::arg("default_value"),
         R"pbdoc(
         Get a configuration value or default if not found.
         
         Args:
             key: Configuration key
             default_value: Default value if key not found
             
         Returns:
             double: Configuration value or default
         )pbdoc")
    .def("getOr", py::overload_cast<const std::string&, const float&, const float&>(&Config::getOr),
         py::arg("key"), py::arg("default_value"),
         R"pbdoc(
         Get a configuration value or default if not found.
         
         Args:
             key: Configuration key
             default_value: Default value if key not found
             
         Returns:
             float: Configuration value or default
         )pbdoc")
    .def("getOr", py::overload_cast<const std::string&, const std::string&, const std::string&>(&Config::getOr),
         py::arg("key"), py::arg("default_value"),
         R"pbdoc(
         Get a configuration value or default if not found.
         
         Args:
             key: Configuration key
             default_value: Default value if key not found
             
         Returns:
             std::string: Configuration value or default
         )pbdoc")
    .def("summary", &Config::summary,
         R"pbdoc(
         Get a summary string of the configuration.
         
         Returns:
             str: Configuration summary
         )pbdoc")
    .def("__repr__", [](const Config& cfg) {
        return "<Config: " + cfg.summary() + ">";
    });
}

// Enhanced Vision class
py::class_<Vision, SensoryInput, std::shared_ptr<Vision>>(m, "Vision",
    R"pbdoc(
    Vision sensory input.
    
    This class represents visual input from the environment, containing
    image data with width, height, and channels.
    )pbdoc")
    .def(py::init<>())
    .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
         R"pbdoc(
         Create vision input with given dimensions.
         
         Args:
             width: Image width in pixels
             height: Image height in pixels
             channels: Number of color channels (default: 3 for RGB)
         )pbdoc")
    .def("setData", [](Vision& self, const std::vector<float>& data) {
        self.setData(data);
    }, py::arg("data"),
         R"pbdoc(
         Set vision data.
         
         Args:
             data: Flattened image data (width*height*channels)
         )pbdoc")
    .def("getWidth", &Vision::getWidth,
         R"pbdoc(
         Get image width.
         
         Returns:
             int: Image width
         )pbdoc")
    .def("getHeight", &Vision::getHeight,
         R"pbdoc(
         Get image height.
         
         Returns:
             int: Image height
         )pbdoc")
    .def("getChannels", &Vision::getChannels,
         R"pbdoc(
         Get number of channels.
         
         Returns:
             int: Number of channels
         )pbdoc");

// Enhanced Action class
py::class_<Action, std::shared_ptr<Action>>(m, "Action",
    R"pbdoc(
    Action representation for motor output.
    
    This class represents actions that the agent can take in the environment,
    including the type and parameters.
    )pbdoc")
    .def(py::init<>())
    .def(py::init<ActionType>(), py::arg("type"),
         R"pbdoc(
         Create action with given type.
         
         Args:
             type: Action type
         )pbdoc")
    .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
         R"pbdoc(
         Create action with given type and parameters.
         
         Args:
             type: Action type
             parameters: Action parameters (e.g., movement speeds)
         )pbdoc")
    .def("getType", &Action::getType,
         R"pbdoc(
         Get action type.
         
         Returns:
             ActionType: Action type
         )pbdoc")
    .def("setType", &Action::setType, py::arg("type"),
         R"pbdoc(
         Set action type.
         
         Args:
             type: New action type
         )pbdoc")
    .def("getParameters", &Action::getParameters,
         R"pbdoc(
         Get action parameters.
         
         Returns:
             List of action parameters
         )pbdoc")
    .def("setParameters", &Action::setParameters, py::arg("params"),
         R"pbdoc(
         Set action parameters.
         
         Args:
             params: New action parameters
         )pbdoc")
    .def("getName", &Action::getName,
         R"pbdoc(
         Get action name.
         
         Returns:
             str: Action name
         )pbdoc")
    .def("clone", &Action::clone,
         R"pbdoc(
         Clone action.
         
         Returns:
             Action: Cloned action
         )pbdoc")
    .def("__repr__", [](const Action& action) {
        return "<Action: " + action.getName() + ">";
    });

// Enhanced SensoryPercept class
py::class_<SensoryPercept, std::shared_ptr<SensoryPercept>>(m, "SensoryPercept",
    R"pbdoc(
    Sensory percept data.
    
    This class contains all sensory inputs from the environment,
    including vision, audio, touch, internal signals, and proprioception.
    )pbdoc")
    .def(py::init<>())
    .def("getVision", &SensoryPercept::getVision,
         R"pbdoc(
         Get vision input.
         
         Returns:
             Vision object or None if no vision
         )pbdoc")
    .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
         R"pbdoc(
         Set vision input.
         
         Args:
             vision: Vision object
         )pbdoc")
    .def("getTouch", &SensoryPercept::getTouch,
         R"pbdoc(
         Get touch input.
         
         Returns:
             Touch object or None if no touch
         )pbdoc")
    .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
         R"pbdoc(
         Set touch input.
         
         Args:
             touch: Touch object
         )pbdoc")
    .def("getInternal", &SensoryPercept::getInternal,
         R"pbdoc(
         Get internal signals.
         
         Returns:
             InternalSignals object or None if no internal signals
         )pbdoc")
    .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
         R"pbdoc(
         Set internal signals.
         
         Args:
             internal: InternalSignals object
         )pbdoc")
    .def("getProprioception", &SensoryPercept::getProprioception,
         R"pbdoc(
         Get proprioception.
         
         Returns:
             Proprioception object or None if no proprioception
         )pbdoc")
    .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
         R"pbdoc(
         Set proprioception.
         
         Args:
             proprioception: Proprioception object
         )pbdoc")
    .def("getAudio", &SensoryPercept::getAudio,
         R"pbdoc(
         Get audio input.
         
         Returns:
             Audio object or None if no audio
         )pbdoc")
    .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
         R"pbdoc(
         Set audio input.
         
         Args:
             audio: Audio object
         )pbdoc")
    .def("getAllSignals", &SensoryPercept::getAllSignals,
         R"pbdoc(
         Get all sensory signals.
         
         Returns:
             List of all sensory signal vectors
         )pbdoc")
    .def("getTimestamp", &SensoryPercept::getTimestamp,
         R"pbdoc(
         Get timestamp.
         
         Returns:
             float: Timestamp
         )pbdoc")
    .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
         R"pbdoc(
         Set timestamp.
         
         Args:
             timestamp: New timestamp
         )pbdoc")
    .def("__repr__", [](const SensoryPercept& percept) {
        std::string result = "<SensoryPercept: ";
        if (percept.getVision()) result += "vision, ";
        if (percept.getTouch()) result += "touch, ";
        if (percept.getInternal()) result += "internal, ";
        if (percept.getProprioception()) result += "proprioception, ";
        if (percept.getAudio()) result += "audio, ";
        if (!result.empty()) result.erase(result.size() - 2); // Remove trailing comma
        result += ">";
        return result;
    });

// Navigation helper functions
inline void pybind11_init(py::module_& m) {
    // Register custom exceptions
    registerCustomExceptions(m);
    
    // Enhanced Brain class with context manager
    createBrainClass(m);
    
    // Enhanced AgentBrain class
    createAgentBrainClass(m);
    
    // Enhanced SimpleWorld class
    createSimpleWorldClass(m);
    
    // Enhanced AgentBody class
    createAgentBodyClass(m);
    
    // Enhanced Config class
    createConfigClass(m);
    
    // Enhanced Vision class
    createVisionClass(m);
    
    // Enhanced Action class
    createActionClass(m);
    
    // Enhanced SensoryPercept class
    createSensoryPerceptClass(m);
    
    // Factory functions for easy creation
    m.def("createDefaultConfig", &createDefaultConfig,
         R"pbdoc(
         Create a default configuration.
         
         Returns:
             Config: Default configuration object
         )pbdoc");
    
    m.def("createBrain", &createBrain,
         R"pbdoc(
         Create a brain with default configuration.
         
         Returns:
             Brain: Brain object
             
         Raises:
             BrainError: If brain initialization fails
         )pbdoc");
    
    m.def("createBrain", &createBrainFromConfig, py::arg("config"),
         R"pbdoc(
         Create a brain with the given configuration.
         
         Args:
             config: Configuration object
             
         Returns:
             Brain: Brain object
             
         Raises:
             BrainError: If brain initialization fails
         )pbdoc");
    
    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
         Create a simple world.
         
         Returns:
             SimpleWorld: World object
         )pbdoc");
    
    // Enum definitions for easy access
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
    
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Motor command enumeration)pbdoc")
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
    
    // Constants for easy access
    m.attr("NEURON_TYPE_EXCITATORY") = NeuronType::Excitatory;
    m.attr("NEURON_TYPE_INHIBITORY") = NeuronType::Inhibitory;
    m.attr("NEURON_TYPE_MODULATORY") = NeuronType::Modulatory;
    m.attr("NEURON_TYPE_SENSORY") = NeuronType::Sensory;
    m.attr("NEURON_TYPE_MOTOR") = NeuronType::Motor;
    m.attr("NEURON_TYPE_INTERNAL") = NeuronType::Internal;
    
    // Physics constants
    m.attr("MEMBRANE_CAPACITANCE") = 1.0f;
    m.attr("TIME_CONSTANT") = 20.0f;
    m.attr("DEFAULT_TIME_STEP") = 0.001f;
}

// Main module definition
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (熙然) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This package provides Python interfaces to the NLM (Neural Learning Machine),
        an experimental artificial developmental brain project implementing:
        
        - LIF neuron dynamics and event-driven spike propagation
        - STDP and Hebbian plasticity rules
        - Structural plasticity (synaptogenesis/pruning)
        - Integrated memory systems (working, episodic, associative)
        - Prediction systems and predictive coding
        - Cognitive functions (planning, attention, concept formation)
        - Neuromodulation (dopamine, acetylcholine, curiosity, etc.)
        - Developmental stages and plasticity modulation
        
        The framework enables research in artificial developmental systems,
        cognitive architectures, and neuromorphic computing.
        
        Quick Start:
        ```python
        import nlm
        
        # Create brain with default configuration
        brain = nlm.createBrain()
        
        # Create world and agent
        world = nlm.createSimpleWorld()
        agent = nlm.AgentBody()
        
        # Create agent brain
        agent_brain = nlm.createAgentBrain(brain, world)
        
        # Run simulation loop
        while episode_running:
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            brain.step(step)
            command = agent_brain.decodeMotorCommand()
            world.applyMotorCommand(command, step)
            reward = world.computeReward()
            agent_brain.applyRewardModulation(reward)
            
            # Save checkpoint periodically
            if step % 1000 == 0:
                brain.save(f"checkpoint_step_{step}.nlm")
        ```
        
        For more detailed examples and API documentation, see:
        - Examples in the repository
        - Full API documentation
        - Research papers on NLM architecture
        
        Phase 6 (Final Integration) implemented comprehensive integration of:
        - Memory systems (working, episodic, associative)
        - Prediction systems (neural prediction, prediction error)
        - Cognitive systems (neural planning, concept formation)
        - Neuromodulation (dopamine, acetylcholine, etc.)
        - Development and plasticity integration
        )pbdoc";
    
    // Initialize with enhanced bindings
    pybind11_init(m);
}

} // namespace nlm
