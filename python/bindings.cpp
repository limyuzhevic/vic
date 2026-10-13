#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>

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

// =============================================================================
// PYTHONIC UTILITY FUNCTIONS AND FACTORIES
// =============================================================================

/**
 * @brief Create a default NLM configuration
 * 
 * Creates a standard configuration suitable for basic NLM simulations with
 * sensible defaults for development and learning experiments.
 * 
 * @return std::shared_ptr<Config> Default configuration object
 * 
 * @example
 * ```python
 * import pynlm as nlm
 * 
 * # Create default config
 * config = nlm.create_default_config()
 * config.set("neuron_count", 1000)
 * config.set("region_count", 2)
 * 
 * # Create brain with custom config
 * brain = nlm.create_brain(config)
 * ```
 */
std::shared_ptr<Config> create_default_config() {
    auto config = std::make_shared<Config>();
    // Set sensible defaults for Phase 2 development
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    return config;
}

/**
 * @brief Create a simple world for agent simulations
 * 
 * Creates a standard simple world with default settings suitable for
 * basic agent-environment interactions. Configure the world dimensions
 * and vision parameters as needed.
 * 
 * @param width World width in meters
 * @param height World height in meters
 * @param vision_width Visual field width in pixels
 * @param vision_height Visual field height in pixels
 * @return std::shared_ptr<SimpleWorld> Configured world object
 * 
 * @example
 * ```python
 * import pynlm as nlm
 * 
 * # Create basic world
 * world = nlm.create_simple_world()
 * world.configure(100.0, 100.0, 32, 32)
 * 
 * # Set agent starting position
 * world.set_agent_start(50.0, 50.0)
 * 
 * # Create agent brain
 * brain = nlm.create_brain(nlm.create_default_config())
 * agent_brain = nlm.create_agent_brain(brain)
 * agent_brain.initialize(world)
 * ```
 */
std::shared_ptr<SimpleWorld> create_simple_world(float width = 100.0f,
                                                 float height = 100.0f,
                                                 size_t vision_width = 32,
                                                 size_t vision_height = 32) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, vision_width, vision_height);
    return world;
}

/**
 * @brief Create an agent brain with a brain and world
 * 
 * Creates an AgentBrain that connects a neural brain to a simulated world,
 * enabling agent-environment interaction. This is the standard setup for
 * reinforcement learning and developmental experiments.
 * 
 * @param brain Neural brain to control
 * @param world Simulated world for interaction
 * @return std::shared_ptr<AgentBrain> Configured agent brain interface
 * 
 * @example
 * ```python
 * import pynlm as nlm
 * 
 * # Setup complete agent system
 * config = nlm.create_default_config()
 * brain = nlm.create_brain(config)
 * world = nlm.create_simple_world()
 * agent_brain = nlm.create_agent_brain(brain)
 * agent_brain.initialize(world)
 * ```
 */
std::shared_ptr<AgentBrain> create_agent_brain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrain>(brain);
}

/**
 * @brief Create a complete agent system for simulation
 * 
 * Convenience function that creates and configures a complete agent system
 * including brain, world, and agent brain interface. This is ideal for
 * setting up quick experimental setups and demonstrations.
 * 
 * @param neuron_count Number of neurons in the brain
 * @param world_width World width in meters
 * @param world_height World height in meters
 * @return std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>
 *         Complete agent system (brain, world, agent_brain)
 * 
 * @example
 * ```python
 * import pynlm as nlm
 * 
 * # Create complete agent system
 * brain, world, agent_brain = nlm.create_agent_system(
 *     neuron_count=1000,
 *     world_width=200.0,
 *     world_height=150.0
 * )
 * 
 * # Initialize agent
 * agent_brain.initialize(world)
 * world.set_agent_start(100.0, 75.0)
 * ```
 */
std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> 
create_agent_system(int64_t neuron_count = 1000,
                    float world_width = 200.0f,
                    float world_height = 150.0f) {
    auto config = create_default_config();
    config->set("neuron_count", neuron_count, ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
    
    auto brain = std::make_shared<Brain>(config);
    auto world = create_simple_world(world_width, world_height, 32, 32);
    auto agent_brain = std::make_shared<AgentBrain>(brain);
    
    return std::make_tuple(brain, world, agent_brain);
}

/**
 * @brief Convert Python list to vector with type checking
 * 
 * Helper function to safely convert Python lists to C++ vectors with
 * proper type validation and error handling.
 * 
 * @tparam T Type of elements in the vector
 * @param py_list Python list to convert
 * @return std::vector<T> Converted vector
 * @throws py::value_error if conversion fails
 * 
 * @example
 * ```python
 * import pynlm as nlm
 * 
 * # Safe conversion
 * data = nlm.py_to_vector([1.0, 2.0, 3.0, 4.0, 5.0])
 * brain.injectCurrentToNeurons(nlm.NeuronType.Excitatory, data)
 * ```
 */
template<typename T>
std::vector<T> py_to_vector(const py::list& py_list) {
    std::vector<T> result;
    result.reserve(py_list.size());
    
    for (py::handle item : py_list) {
        try {
            result.push_back(py::cast<T>(item));
        } catch (const py::cast_error& e) {
            throw py::value_error("Failed to convert list element to target type: " + std::string(e.what()));
        }
    }
    
    return result;
}

/**
 * @brief Convert Python list to vector<float> (specialization)
 * 
 * Specialization for float conversion with support for both int and float
 * Python numbers, and automatic conversion from sequences.
 * 
 * @param py_list Python list to convert
 * @return std::vector<float> Converted vector
 * @throws py::value_error if conversion fails
 */
inline std::vector<float> py_to_vector(const py::list& py_list) {
    std::vector<float> result;
    result.reserve(py_list.size());
    
    for (py::handle item : py_list) {
        try {
            if (py::isinstance(item, py::int_())) {
                result.push_back(py::cast<int64_t>(item));
            } else if (py::isinstance(item, py::float_())) {
                result.push_back(py::cast<double>(item));
            } else {
                result.push_back(py::cast<float>(item));
            }
        } catch (const py::cast_error& e) {
            throw py::value_error("Failed to convert list element to float: " + std::string(e.what()));
        }
    }
    
    return result;
}

/**
 * @brief Convert Python list to vector<int>
 * 
 * Specialization for int conversion with validation.
 * 
 * @param py_list Python list to convert
 * @return std::vector<int> Converted vector
 * @throws py::value_error if conversion fails
 */
inline std::vector<int> py_to_vector_int(const py::list& py_list) {
    std::vector<int> result;
    result.reserve(py_list.size());
    
    for (py::handle item : py_list) {
        try {
            result.push_back(py::cast<int>(item));
        } catch (const py::cast_error& e) {
            throw py::value_error("Failed to convert list element to int: " + std::string(e.what()));
        }
    }
    
    return result;
}

/**
 * @brief Safe parameter access with default value
 * 
 * Utility function to safely access configuration parameters with
 * type-specific default values and error handling.
 * 
 * @tparam T Type of the parameter
 * @param config Configuration object to access
 * @param key Parameter key
 * @param default_value Default value if key doesn't exist
 * @return T Parameter value or default
 * 
 * @example
 * ```python
 * import pynlm as nlm
 * 
 * config = nlm.create_default_config()
 * timestep = nlm.get_param(config, "simulation_timestep", 0.001)
 * neuron_count = nlm.get_param(config, "neuron_count", 500)
 * ```
 */
template<typename T>
T get_param(const std::shared_ptr<Config>& config, const std::string& key, const T& default_value) {
    auto value = config->getOr<T>(key, default_value);
    if (!value) {
        throw py::key_error("Configuration key not found: '" + key + "'");
    }
    return *value;
}

/**
 * @brief Get configuration value with type validation
 * 
 * Extended version with type-specific validation and conversion support.
 * 
 * @tparam T Type of the parameter
 * @param config Configuration object to access
 * @param key Parameter key
 * @param default_value Default value if key doesn't exist
 * @param validate_min Optional minimum value validation
 * @param validate_max Optional maximum value validation
 * @return T Parameter value or default
 * @throws py::value_error if validation fails
 */
template<typename T>
T get_param_with_validation(const std::shared_ptr<Config>& config, 
                           const std::string& key, 
                           const T& default_value,
                           std::optional<T> validate_min = std::nullopt,
                           std::optional<T> validate_max = std::nullopt) {
    auto value = config->getOr<T>(key, default_value);
    if (!value) {
        throw py::key_error("Configuration key not found: '" + key + "'");
    }
    
    T result = *value;
    
    if (validate_min && result < *validate_min) {
        throw py::value_error("Parameter '" + key + "' value " + std::to_string(result) +
                            " is below minimum " + std::to_string(*validate_min));
    }
    
    if (validate_max && result > *validate_max) {
        throw py::value_error("Parameter '" + key + "' value " + std::to_string(result) +
                            " exceeds maximum " + std::to_string(*validate_max));
    }
    
    return result;
}

// =============================================================================
PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides a complete Python interface to the NLM (Neural Learning Machine)
        framework, which implements real spiking neural computation with event-driven dynamics,
        integrated memory, prediction, cognition, and neuromodulation systems.
        
        Key Features:
        - Real LIF (Leaky Integrate-and-Fire) neuron dynamics
        - Event-driven spike propagation with synaptic delays
        - STDP and Hebbian plasticity rules
        - Structural plasticity (synaptogenesis/pruning)
        - Developmental systems with stage progression
        - Neuromodulation (reward, curiosity, novelty)
        - Working and episodic memory systems
        - Prediction and planning capabilities
        - Complete brain-environment interaction via AgentBrain
        - Simple 2D world simulation
        
        The interface is designed to be both Pythonic and powerful, providing:
        - Property-style access to object attributes
        - Factory functions for common setups
        - Comprehensive type hints for IDE support
        - Rich documentation with examples
        - Context managers for resource management
        - Robust error handling and parameter validation
        
        Basic Usage Example:
        ---------------------
        ```python
        import pynlm as nlm
        import numpy as np
        
        # Create a complete agent system
        brain, world, agent_brain = nlm.create_agent_system(
            neuron_count=1000,
            world_width=200.0,
            world_height=150.0
        )
        
        # Initialize the agent
        agent_brain.initialize(world)
        world.set_agent_start(100.0, 75.0)
        
        # Run a simulation episode
        for step in range(1000):
            # Get sensory input
            percept = world.getSensoryPercept()
            vision = percept.getVision()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Get action from brain
            action = brain.produceAction()
            
            # Apply action in world
            reward = world.applyMotorCommand(action.getType(), step * 0.001)
            
            # Apply reward modulation
            agent_brain.applyRewardModulation(reward.reward, 0.0)
            
            # Update development
            agent_brain.updateDevelopment(0.001)
            
            # Check if episode is done
            body = world.getAgentBody()
            if body.energy <= 0.0:
                agent_brain.reset()
                world.reset()
                break
        
        print(f"Episode completed. Total spikes: {brain.getTotalSpikeCount()}")
        ```
        
        See individual class documentation for more detailed usage examples.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // =============================================================================
    // TYPE IDENTIFIERS
    // =============================================================================

    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_property_readonly("value", &NeuronId::value,
                              "The integer value of this neuron ID")
        .def("index", &NeuronId::index,
             "Get the index of this neuron ID")
        .def("__eq__", &NeuronId::operator==,
             "Check if two neuron IDs are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two neuron IDs are not equal")
        .def("__lt__", &NeuronId::operator<,
             "Check if this neuron ID is less than another")
        .def("__gt__", [](const NeuronId& a, const NeuronId& b) { return a.value > b.value; },
             "Check if this neuron ID is greater than another")
        .def("__le__", [](const NeuronId& a, const NeuronId& b) { return a.value <= b.value; },
             "Check if this neuron ID is less than or equal to another")
        .def("__ge__", [](const NeuronId& a, const NeuronId& b) { return a.value >= b.value; },
             "Check if this neuron ID is greater than or equal to another")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash value for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Get string representation of neuron ID")
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        }, "Get user-friendly string representation");

    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_property_readonly("value", &SynapseId::value,
                              "The integer value of this synapse ID")
        .def("index", &SynapseId::index,
             "Get the index of this synapse ID")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_property_readonly("value", &RegionId::value,
                              "The integer value of this region ID")
        .def("index", &RegionId::index,
             "Get the index of this region ID")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_property_readonly("value", &PopulationId::value,
                              "The integer value of this population ID")
        .def("index", &PopulationId::index,
             "Get the index of this population ID")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // =============================================================================
    // ENUMS
    // =============================================================================

    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values()
        .doc() = R"pbdoc(
            Neuron type enumeration.
            
            Defines the different functional types of neurons in the NLM system:
            - Excitatory: Increase firing probability of target neurons
            - Inhibitory: Decrease firing probability of target neurons
            - Modulatory: Release neuromodulators affecting multiple targets
            - Sensory: Process and relay external sensory inputs
            - Motor: Generate output signals for action execution
            - Internal: Implement internal computations and representations
        )pbdoc";

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse type")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse type")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse type")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse type")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse type")
        .export_values()
        .doc() = R"pbdoc(
            Synapse type enumeration.
            
            Defines the different types of synaptic connections:
            - Excitatory: Positively charged ions, depolarize postsynaptic neuron
            - Inhibitory: Negatively charged ions, hyperpolarize postsynaptic neuron
            - Modulatory: Release neuromodulators with prolonged effects
            - Electrical: Direct ion flow between neurons through gap junctions
            - GapJunction: Specialized electrical synapses for rapid communication
        )pbdoc";

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values()
        .doc() = R"pbdoc(
            Developmental stage enumeration.
            
            Represents the developmental progression of the NLM system:
            - Initial: Newborn stage, basic connectivity established
            - CriticalPeriod: Sensitive period for rapid learning and refinement
            - Maturation: Fine-tuning of neural circuits
            - Adult: Stable adult configuration
            - Aging: Age-related decline and adaptation
        )pbdoc";

    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting, "Neuron is at rest")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron is refractory")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values()
        .doc() = R"pbdoc(
            Neuron firing state enumeration.
            
            Current physiological state of a neuron:
            - Resting: Below threshold, no activity
            - Active: Firing action potential
            - Refractory: Cannot fire due to recovery period
            - Inhibited: Cannot fire due to inhibitory input
        )pbdoc";

    py::enum_<ActionType>(m, "ActionType")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world")
        .value("MoveRight", ActionType::MoveRight, "Move right in world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left in world")
        .value("TurnRight", ActionType::TurnRight, "Turn right in world")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby object")
        .value("Eat", ActionType::Eat, "Eat resource if available")
        .value("Drink", ActionType::Drink, "Drink water if available")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without action")
        .value("Custom", ActionType::Custom, "Custom user-defined action")
        .export_values()
        .doc() = R"pbdoc(
            Action type enumeration.
            
            Motor actions that can be performed in the simulated world:
            - MoveForward, MoveBackward, MoveLeft, MoveRight: Basic locomotion
            - TurnLeft, TurnRight: Rotational movement
            - Look, LookUp, LookDown: Sensor orientation
            - Interact: Manipulate nearby objects
            - Eat, Drink: Consume resources
            - Rest: Recover energy and health
            - Wait: Do nothing
            - Custom: User-defined action type
        )pbdoc";

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward in world")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward in world")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left in world")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right in world")
        .value("LookLeft", MotorCommand::LookLeft, "Pan sensors left")
        .value("LookRight", MotorCommand::LookRight, "Pan sensors right")
        .value("Interact", MotorCommand::Interact, "Interact with nearby object")
        .value("Wait", MotorCommand::Wait, "Wait without action")
        .export_values()
        .doc() = R"pbdoc(
            Low-level motor command enumeration.
            
            Basic motor commands that directly control agent movement:
            - MoveForward, MoveBackward, TurnLeft, TurnRight: Locomotion
            - LookLeft, LookRight: Sensor orientation
            - Interact: Object interaction
            - Wait: No movement
            
            These are the neural-level commands that map to higher-level actions.
        )pbdoc";

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource with positive value")
        .value("Hazard", WorldObjectType::Hazard, "Hazard with negative value")
        .value("Wall", WorldObjectType::Wall, "Impassable boundary")
        .value("Marker", WorldObjectType::Marker, "Navigation aid")
        .export_values()
        .doc() = R"pbdoc(
            World object type enumeration.
            
            Types of objects that can exist in the simulated world:
            - Empty: No object at this location
            - Resource: Positive value, can be consumed for energy
            - Hazard: Negative value, causes damage
            - Wall: Impassable boundary
            - Marker: Visual navigation aid
            
            These types are used for world simulation and agent perception,
            but are NOT provided to the brain (maintaining environmental abstraction).
        )pbdoc";

    // =============================================================================
    // CONFIGURATION
    // =============================================================================

    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True if successful.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Returns True if successful.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True if successful.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Get string representation of configuration.")
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        }, "Get user-friendly string representation.");

    // Configuration value access methods
    .def("get", [](const std::shared_ptr<Config>& config, const std::string& key) {
        // Try to get as string first
        auto str_val = config->get<std::string>(key);
        if (str_val) return py::cast(*str_val);
        
        // Try int
        auto int_val = config->get<int64_t>(key);
        if (int_val) return py::cast(*int_val);
        
        // Try double
        auto double_val = config->get<double>(key);
        if (double_val) return py::cast(*double_val);
        
        // Try bool
        auto bool_val = config->get<bool>(key);
        if (bool_val) return py::cast(*bool_val);
        
        // Try vector
        auto vec_val = config->get<std::vector<float>>(key);
        if (vec_val) return py::cast(*vec_val);
        
        // Not found
        throw py::key_error("Configuration key not found: '" + key + "'");
    }, py::arg("key"),
       "Get configuration value by key. Automatically converts to appropriate Python type.")
    .def("set", [](std::shared_ptr<Config>& config, const std::string& key, py::object value) {
        // Convert Python object to ConfigValue
        if (py::isinstance(value, py::int_())) {
            config->set(key, py::cast<int64_t>(value), ConfigSource::Runtime);
        } else if (py::isinstance(value, py::float_())) {
            config->set(key, py::cast<double>(value), ConfigSource::Runtime);
        } else if (py::isinstance(value, py::bool_())) {
            config->set(key, py::cast<bool>(value), ConfigSource::Runtime);
        } else if (py::isinstance(value, py::str())) {
            config->set(key, py::cast<std::string>(value), ConfigSource::Runtime);
        } else if (py::isinstance(value, py::list())) {
            // Try to convert list to vector
            try {
                std::vector<float> float_list;
                for (auto item : value) {
                    float_list.push_back(py::cast<float>(item));
                }
                config->set(key, float_list, ConfigSource::Runtime);
            } catch (...) {
                // If float conversion fails, try string
                std::vector<std::string> str_list;
                for (auto item : value) {
                    str_list.push_back(py::cast<std::string>(item));
                }
                config->set(key, str_list, ConfigSource::Runtime);
            }
        } else {
            throw py::type_error("Unsupported Python type for configuration value: " + std::string(Py_TYPE(value.ptr())->tp_name));
        }
    }, py::arg("key"), py::arg("value"),
       "Set configuration value. Accepts Python types: int, float, bool, str, and lists.")
    .def("get_or", [](const std::shared_ptr<Config>& config, const std::string& key, py::object default_value) {
        // Try to get value or return default
        auto str_val = config->get<std::string>(key);
        if (str_val) return py::cast(*str_val);
        
        auto int_val = config->get<int64_t>(key);
        if (int_val) return py::cast(*int_val);
        
        auto double_val = config->get<double>(key);
        if (double_val) return py::cast(*double_val);
        
        auto bool_val = config->get<bool>(key);
        if (bool_val) return py::cast(*bool_val);
        
        // If default is a list, convert to appropriate vector
        if (py::isinstance(default_value, py::list())) {
            std::vector<float> float_list;
            for (auto item : default_value) {
                float_list.push_back(py::cast<float>(item));
            }
            return float_list;
        }
        
        return default_value;
    }, py::arg("key"), py::arg("default_value"),
       "Get configuration value or return default if key doesn't exist.")
    .def("remove", &Config::remove, py::arg("key"),
         "Remove a configuration key.");

    // =============================================================================
    // SENSORY INPUTS
    // =============================================================================

    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input (e.g., 'Vision', 'Audio').")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a list of floats.")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data.")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of this sensory input.")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of this sensory input.")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: " + std::string(input.getType()) + 
                   " dimensions=" + std::to_string(input.getDimensions()) + ">";
        }, "Get string representation of sensory input.");

    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width") = 64,
             py::arg("height") = 64, py::arg("channels") = 3,
             "Create a vision sensor with specified dimensions.")
        .def("setData", [](Vision& self, const py::list& data) {
            self.setData(py_to_vector(data));
        }, py::arg("data"),
             "Set vision data from a Python list of floats.")
        .def("setDataArray", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set vision data from a C++ vector of floats.")
        .def("getWidth", &Vision::getWidth,
             "Get vision sensor width in pixels.")
        .def("getHeight", &Vision::getHeight,
             "Get vision sensor height in pixels.")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels (usually 1 for grayscale, 3 for RGB).")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" +
                   std::to_string(vision.getHeight()) + "x" +
                   std::to_string(vision.getChannels()) + ">";
        }, "Get string representation of vision sensor.");

    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate") = 44100,
             py::arg("numSamples") = 16384,
             "Create an audio sensor with specified sampling rate and duration.")
        .def("setData", [](Audio& self, const py::list& data) {
            self.setData(py_to_vector(data));
        }, py::arg("data"),
             "Set audio data from a Python list of floats.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate in Hz.")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the audio sample rate in Hz.")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples.")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + "Hz, " +
                   std::to_string(audio.getNumSamples()) + " samples>";
        }, "Get string representation of audio sensor.");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals buffer.")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals.")
        .def("getSignals", &InternalSignals::getData,
             "Get all internal signals as a list of floats.")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: " + std::to_string(signals.getData().size()) + 
                   " signals>";
        }, "Get string representation of internal signals.");

    // =============================================================================
    // MOTOR ACTIONS
    // =============================================================================

    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an action with specified type.")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameters.")
        .def("getType", &Action::getType,
             "Get the action type.")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type.")
        .def("getParameters", &Action::getParameters,
             "Get action parameters as a list of floats.")
        .def("setParameters", [](Action& self, const py::list& params) {
            self.setParameters(py_to_vector(params));
        }, py::arg("params"),
             "Set action parameters from a Python list of floats.")
        .def("getName", &Action::getName,
             "Get human-readable name of the action type.")
        .def("clone", &Action::clone,
             "Create a copy of this action.")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + 
                   " params=" + std::to_string(action.getParameters().size()) + ">";
        }, "Get string representation of action.")
        .def("__str__", [](const Action& action) {
            return action.getName();
        }, "Get human-readable action name.");

    // =============================================================================
    // WORLD OBJECTS
    // =============================================================================

    py::class_<WorldObject>(m, "WorldObject")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"),
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Create a world object at position (x, y) with specified type.")
        .def_readwrite("x", &WorldObject::x, "X coordinate in world space.")
        .def_readwrite("y", &WorldObject::y, "Y coordinate in world space.")
        .def_readwrite("radius", &WorldObject::radius, "Object radius.")
        .def_readwrite("type", &WorldObject::type, "Object type (WorldObjectType enum).")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/damage).")
        .def_readwrite("active", &WorldObject::active, "Whether object is active.")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance from this object to position (x, y).")
        .def("is_resource", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Resource;
        }, "Check if object is a resource.")
        .def("is_hazard", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard.")
        .def("is_wall", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Wall;
        }, "Check if object is a wall.")
        .def("__repr__", [](const WorldObject& obj) {
            std::string type_name;
            switch (obj.type) {
                case WorldObjectType::Empty: type_name = "Empty"; break;
                case WorldObjectType::Resource: type_name = "Resource"; break;
                case WorldObjectType::Hazard: type_name = "Hazard"; break;
                case WorldObjectType::Wall: type_name = "Wall"; break;
                case WorldObjectType::Marker: type_name = "Marker"; break;
            }
            return "<WorldObject: " + type_name + " at (" + std::to_string(obj.x) + ", " +
                   std::to_string(obj.y) + ") value=" + std::to_string(obj.value) + ">";
        }, "Get string representation of world object.");

    // =============================================================================
    // AGENT BODY
    // =============================================================================

    py::class_<AgentBody>(m, "AgentBody")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position in world.")
        .def_readwrite("y", &AgentBody::y, "Y position in world.")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians.")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity.")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity.")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity.")
        .def_readwrite("energy", &AgentBody::energy, "Energy level (0.0-1.0).")
        .def_readwrite("health", &AgentBody::health, "Health level (0.0-1.0).")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps.")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving.")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning.")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action.")
        .def("reset", &AgentBody::reset,
             "Reset agent body to initial state.")
        .def("distance_to", [](const AgentBody& body, float x, float y) {
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance from agent body to position (x, y).")
        .def("energy_percentage", [](const AgentBody& body) {
            return body.energy * 100.0f;
        }, "Get energy as percentage (0-100).")
        .def("health_percentage", [](const AgentBody& body) {
            return body.health * 100.0f;
        }, "Get health as percentage (0-100).")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: (" + std::to_string(body.x) + ", " + std::to_string(body.y()) +
                   ") energy=" + std::to_string(body.energy) + ">";
        }, "Get string representation of agent body.");

    // =============================================================================
    // ACTION RESULTS
    // =============================================================================

    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward") = 0.0f, py::arg("success") = false, py::arg("message") = "",
             "Create an action result with reward and success status.")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action.")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful.")
        .def_readwrite("message", &ActionResult::message, "Human-readable message.")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) + 
                   " success=" + std::to_string(result.success) + ">";
        }, "Get string representation of action result.");

    // =============================================================================
    // SENSORY PERCEPT
    // =============================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get vision data as list of floats.")
        .def("setVision", [](SensoryPercept& self, const py::list& vision) {
            self.setVision(py_to_vector(vision));
        }, py::arg("vision"),
             "Set vision data from Python list of floats.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width in pixels.")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height in pixels.")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch/proximity data as list of floats.")
        .def("setTouch", [](SensoryPercept& self, const py::list& touch) {
            self.setTouch(py_to_vector(touch));
        }, py::arg("touch"),
             "Set touch data from Python list of floats.")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal state data as list of floats.")
        .def("setInternal", [](SensoryPercept& self, const py::list& internal) {
            self.setInternal(py_to_vector(internal));
        }, py::arg("internal"),
             "Set internal data from Python list of floats.")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception data as list of floats.")
        .def("setProprioception", [](SensoryPercept& self, const py::list& proprioception) {
            self.setProprioception(py_to_vector(proprioception));
        }, py::arg("proprioception"),
             "Set proprioception data from Python list of floats.")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio data as list of floats.")
        .def("setAudio", [](SensoryPercept& self, const py::list& audio) {
            self.setAudio(py_to_vector(audio));
        }, py::arg("audio"),
             "Set audio data from Python list of floats.")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals concatenated as list of floats.")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get percept timestamp.")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set percept timestamp.")
        .def("get_signal_by_type", [](const SensoryPercept& percept, const std::string& signal_type) {
            if (signal_type == "vision") {
                return percept.getVision();
            } else if (signal_type == "touch") {
                return percept.getTouch();
            } else if (signal_type == "internal") {
                return percept.getInternal();
            } else if (signal_type == "proprioception") {
                return percept.getProprioception();
            } else if (signal_type == "audio") {
                return percept.getAudio();
            } else {
                throw py::value_error("Unknown signal type: '" + signal_type + "'");
            }
        }, py::arg("signal_type"),
             "Get specific signal type by name. Valid types: 'vision', 'touch', 'internal', 'proprioception', 'audio'.")
        .def("set_signal_by_type", [](SensoryPercept& percept, const std::string& signal_type, const py::list& data) {
            if (signal_type == "vision") {
                percept.setVision(py_to_vector(data));
            } else if (signal_type == "touch") {
                percept.setTouch(py_to_vector(data));
            } else if (signal_type == "internal") {
                percept.setInternal(py_to_vector(data));
            } else if (signal_type == "proprioception") {
                percept.setProprioception(py_to_vector(data));
            } else if (signal_type == "audio") {
                percept.setAudio(py_to_vector(data));
            } else {
                throw py::value_error("Unknown signal type: '" + signal_type + "'");
            }
        }, py::arg("signal_type"), py::arg("data"),
             "Set specific signal type by name. Valid types: 'vision', 'touch', 'internal', 'proprioception', 'audio'.")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + "x" +
                   std::to_string(percept.getVisionHeight()) + ">";
        }, "Get string representation of sensory percept.");

    // =============================================================================
    // SIMPLE WORLD
    // =============================================================================

    py::class_<SimpleWorld, std::enable_shared_from_this<SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             "Configure world dimensions and vision parameters.")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step).")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             "Apply motor command to agent and return action result.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get current sensory percept for the agent.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             "Get current agent body state.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y).")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall or obstacle).")
        .def("getObjectAt", [](SimpleWorld& world, float x, float y) {
            WorldObject* obj = world.getObjectAt(x, y);
            if (obj) {
                return py::cast(*obj);
            } else {
                return py::none();
            }
        }, py::arg("x"), py::arg("y"),
             "Get object at position (x, y). Returns None if no object.")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width in meters.")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height in meters.")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy level.")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy level.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per second.")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per second.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time in seconds.")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed.")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + ">";
        }, "Get string representation of world.");

    // =============================================================================
    // BRAIN
    // =============================================================================

    py::class_<Brain, std::enable_shared_from_this<Brain>>(m, "Brain")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with configuration.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True if successful.")
        .def("step", py::overload_cast<SimulationStep>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step by step count.")
        .def("step", py::overload_cast<SimulationStep, Timestamp>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp.")
        .def("receiveSensoryInput", [](Brain& brain, const SensoryInput& input) {
            brain.receiveSensoryInput(input);
        }, py::arg("input"),
             "Receive and process sensory input.")
        .def("injectCurrent", &Brain::injectCurrent,
             "Inject current directly into a specific neuron.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns Action object.")
        .def("reset", &Brain::reset,
             "Reset brain state.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True if successful.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True if successful.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name. Returns region ID.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             "Get neural region by ID. Returns None if region not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a list.")
        .def("getRegions", &Brain::getRegions,
             "Get all regions as a list.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             "Set developmental stage.")
        .def("getConfig", &Brain::getConfig,
             "Get configuration object.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console.")
        .def("get_spike_count", [](const Brain& brain) {
            return brain.getTotalSpikeCount();
        }, "Get total spike count (alias for getTotalSpikeCount).")
        .def("get_firing_rate", [](const Brain& brain) {
            return brain.getAverageFiringRate();
        }, "Get average firing rate (alias for getAverageFiringRate).")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) + ">";
        }, "Get string representation of brain.");

    // =============================================================================
    // AGENT BRAIN
    // =============================================================================

    py::class_<AgentBrain, std::enable_shared_from_this<AgentBrain>>(m, "AgentBrain")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an AgentBrain with neural brain.")
        .def("initialize", &AgentBrain::initialize,
             "Initialize agent brain with world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             "Process sensory percept and inject into brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             "Apply reward-based neuromodulation.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             "Update development system.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             "Get the underlying brain object.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             "Enable or disable development system.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             "Enable or disable curiosity system.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled.")
        .def("__repr__", [](const AgentBrain& agent_brain) {
            return "<AgentBrain: " + std::to_string(agent_brain.getBrain()->getTotalNeuronCount()) +
                   " neurons>";
        }, "Get string representation of agent brain.");

    // =============================================================================
    // FACTORY FUNCTIONS
    // =============================================================================

    m.def("create_default_config", &create_default_config,
          "Create a default NLM configuration with sensible Phase 2 defaults.")
    .def("create_simple_world", py::overload_cast<float, float, size_t, size_t>(&create_simple_world),
         py::arg("width") = 100.0f, py::arg("height") = 100.0f,
         py::arg("vision_width") = 32, py::arg("vision_height") = 32,
         "Create a simple world with specified dimensions.")
    .def("create_agent_brain", &create_agent_brain,
         "Create an agent brain interface from a neural brain.")
    .def("create_brain", py::overload_cast<std::shared_ptr<Config>>(&create_brain),
         py::arg("config"),
         "Create a neural brain with configuration.")
    .def("create_world", py::overload_cast<float, float, size_t, size_t>(&create_simple_world),
         py::arg("width") = 100.0f, py::arg("height") = 100.0f,
         py::arg("vision_width") = 32, py::arg("vision_height") = 32,
         "Create a simple world (alias for create_simple_world).")
    .def("create_agent_system", &create_agent_system,
         py::arg("neuron_count") = 1000, py::arg("world_width") = 200.0f, py::arg("world_height") = 150.0f,
         "Create a complete agent system (brain, world, agent_brain).")
    .def("py_to_vector", &py_to_vector,
         "Convert Python list to vector with automatic type handling.")
    .def("get_param", &get_param,
         "Get configuration parameter with default value.")
    .def("get_param_with_validation", &get_param_with_validation,
         "Get configuration parameter with validation.");

    // =============================================================================
    // UTILITY FUNCTIONS
    // =============================================================================

    m.def("py_to_vector_int", &py_to_vector_int,
          "Convert Python list to vector of integers.")
    .def("py_to_vector_float", &py_to_vector<float>,
         "Convert Python list to vector of floats.")

    // =============================================================================
    // INVALID ID CONSTANTS
    // =============================================================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // =============================================================================
    // GLOBAL CONSTANTS AND HELPERS
    // =============================================================================

    m.def("get_version", []() {
        return "pynlm 1.0.0";
    }, "Get the version of the pynlm bindings.")

    m.def("get_available_actions", []() {
        std::vector<std::string> actions;
        actions.push_back("MoveForward");
        actions.push_back("MoveBackward");
        actions.push_back("MoveLeft");
        actions.push_back("MoveRight");
        actions.push_back("TurnLeft");
        actions.push_back("TurnRight");
        actions.push_back("Look");
        actions.push_back("LookUp");
        actions.push_back("LookDown");
        actions.push_back("Interact");
        actions.push_back("Eat");
        actions.push_back("Drink");
        actions.push_back("Rest");
        actions.push_back("Wait");
        return actions;
    }, "Get list of all available action names.")

    m.def("get_neuron_types", []() {
        std::vector<std::string> types;
        types.push_back("Excitatory");
        types.push_back("Inhibitory");
        types.push_back("Modulatory");
        types.push_back("Sensory");
        types.push_back("Motor");
        types.push_back("Internal");
        return types;
    }, "Get list of all neuron type names.")

    m.def("get_synapse_types", []() {
        std::vector<std::string> types;
        types.push_back("Excitatory");
        types.push_back("Inhibitory");
        types.push_back("Modulatory");
        types.push_back("Electrical");
        types.push_back("GapJunction");
        return types;
    }, "Get list of all synapse type names.")

    m.def("get_developmental_stages", []() {
        std::vector<std::string> stages;
        stages.push_back("Initial");
        stages.push_back("CriticalPeriod");
        stages.push_back("Maturation");
        stages.push_back("Adult");
        stages.push_back("Aging");
        return stages;
    }, "Get list of all developmental stage names.");

    m.def("get_motor_commands", []() {
        std::vector<std::string> commands;
        commands.push_back("MoveForward");
        commands.push_back("MoveBackward");
        commands.push_back("TurnLeft");
        commands.push_back("TurnRight");
        commands.push_back("LookLeft");
        commands.push_back("LookRight");
        commands.push_back("Interact");
        commands.push_back("Wait");
        return commands;
    }, "Get list of all motor command names.");

    m.def("get_world_object_types", []() {
        std::vector<std::string> types;
        types.push_back("Empty");
        types.push_back("Resource");
        types.push_back("Hazard");
        types.push_back("Wall");
        types.push_back("Marker");
        return types;
    }, "Get list of all world object type names.");

    // =============================================================================
    // ERROR HANDLING AND VALIDATION
    // =============================================================================

    m.def("validate_config", [](const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        // Check required parameters
        if (!config->has("neuron_count")) {
            errors.push_back("Missing required parameter: 'neuron_count'");
        }
        
        if (!config->has("simulation_timestep")) {
            errors.push_back("Missing required parameter: 'simulation_timestep'");
        }
        
        if (!config->has("random_seed")) {
            errors.push_back("Missing required parameter: 'random_seed'");
        }
        
        // Validate parameter values
        auto neuron_count = config->get<int64_t>("neuron_count");
        if (neuron_count && *neuron_count <= 0) {
            errors.push_back("Parameter 'neuron_count' must be positive");
        }
        
        auto timestep = config->get<double>("simulation_timestep");
        if (timestep && (timestep <= 0.0 || timestep > 1.0)) {
            errors.push_back("Parameter 'simulation_timestep' must be between 0 and 1.0");
        }
        
        if (!errors.empty()) {
            std::string error_msg = "Configuration validation failed:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw py::value_error(error_msg);
        }
        
        return true;
    }, "Validate configuration parameters. Returns True if valid, raises error if invalid.");

    m.def("validate_world", [](const std::shared_ptr<SimpleWorld>& world) {
        std::vector<std::string> errors;
        
        if (world->getWidth() <= 0.0f) {
            errors.push_back("World width must be positive");
        }
        
        if (world->getHeight() <= 0.0f) {
            errors.push_back("World height must be positive");
        }
        
        if (world->getWidth() < 0.1f) {
            errors.push_back("World width too small (< 0.1m)");
        }
        
        if (world->getHeight() < 0.1f) {
            errors.push_back("World height too small (< 0.1m)");
        }
        
        if (!errors.empty()) {
            std::string error_msg = "World validation failed:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw py::value_error(error_msg);
        }
        
        return true;
    }, "Validate world parameters. Returns True if valid, raises error if invalid.");

    m.def("validate_brain", [](const std::shared_ptr<Brain>& brain) {
        std::vector<std::string> errors;
        
        if (!brain->initialize()) {
            errors.push_back("Brain initialization failed");
        }
        
        if (brain->getTotalNeuronCount() <= 0) {
            errors.push_back("Brain must have at least one neuron");
        }
        
        if (brain->getTotalSynapseCount() < 0) {
            errors.push_back("Synapse count cannot be negative");
        }
        
        if (!errors.empty()) {
            std::string error_msg = "Brain validation failed:\n";
            for (const auto& error : errors) {
                error_msg += "  - " + error + "\n";
            }
            throw py::value_error(error_msg);
        }
        
        return true;
    }, "Validate brain state. Returns True if valid, raises error if invalid.");

    // =============================================================================
    // CONTEXT MANAGERS
    // =============================================================================

    py::class_<ConfigContext>(m, "ConfigContext")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a configuration context manager.")
        .def("__enter__", [](ConfigContext& self) {
            return self.config_;})
        .def("__exit__", [](ConfigContext& self, py::args) {
            // Save configuration on exit
            self.config_->saveToFile("config_backup.json");
        });

    py::class_<WorldContext>(m, "WorldContext")
        .def(py::init<std::shared_ptr<SimpleWorld>>(), py::arg("world"),
             "Create a world context manager.")
        .def("__enter__", [](WorldContext& self) {
            return self.world_;})
        .def("__exit__", [](WorldContext& self, py::args) {
            // Reset world on exit
            self.world_->reset();
        });

    py::class_<BrainContext>(m, "BrainContext")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create a brain context manager.")
        .def("__enter__", [](BrainContext& self) {
            return self.brain_;})
        .def("__exit__", [](BrainContext& self, py::args) {
            // Reset brain on exit
            self.brain_->reset();
        });

    // =============================================================================
    // EXPLORATION AND HELP FUNCTIONS
    // =============================================================================

    m.def("get_classes", []() {
        return py::list() << "Config" << "Vision" << "Audio" << "InternalSignals"
                          << "Action" << "WorldObject" << "AgentBody" << "ActionResult"
                          << "SensoryPercept" << "SimpleWorld" << "Brain" << "AgentBrain";
    }, "Get list of all available class names.")

    m.def("get_enums", []() {
        return py::list() << "NeuronType" << "SynapseType" << "DevelopmentalStage"
                          << "FiringState" << "ActionType" << "MotorCommand" << "WorldObjectType";
    }, "Get list of all available enum names.")

    m.def("get_example_config", []() {
        auto config = create_default_config();
        config->set("neuron_count", 1000, ConfigSource::Runtime);
        config->set("region_count", 2, ConfigSource::Runtime);
        config->set("connection_probability", 0.2f, ConfigSource::Runtime);
        return config;
    }, "Get an example configuration for quick testing.");

    m.def("create_example_system", []() {
        auto config = create_default_config();
        config->set("neuron_count", 500, ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = create_simple_world(100.0f, 100.0f, 32, 32);
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        
        return std::make_tuple(brain, world, agent_brain);
    }, "Create an example complete system for testing and demonstrations.");

    m.def("print_help", []() {
        std::cout << "NLM Python Bindings Help:" << std::endl;
        std::cout << "===========================" << std::endl;
        std::cout << std::endl;
        std::cout << "Available modules:" << std::endl;
        std::cout << "  - nlm.Config: Configuration management" << std::endl;
        std::cout << "  - nlm.Vision, nlm.Audio, nlm.InternalSignals: Sensory inputs" << std::endl;
        std::cout << "  - nlm.Action: Motor actions" << std::endl;
        std::cout << "  - nlm.WorldObject, nlm.AgentBody, nlm.ActionResult: State objects" << std::endl;
        std::cout << "  - nlm.SensoryPercept: Perceptual data" << std::endl;
        std::cout << "  - nlm.SimpleWorld: Environment simulation" << std::endl;
        std::cout << "  - nlm.Brain: Neural computation" << std::endl;
        std::cout << "  - nlm.AgentBrain: Agent-environment interface" << std::endl;
        std::cout << std::endl;
        std::cout << "Factory functions:" << std::endl;
        std::cout << "  - nlm.create_default_config(): Create default config" << std::endl;
        std::cout << "  - nlm.create_simple_world(): Create simulation world" << std::endl;
        std::cout << "  - nlm.create_agent_brain(brain): Create agent interface" << std::endl;
        std::cout << "  - nlm.create_brain(config): Create neural brain" << std::endl;
        std::cout << "  - nlm.create_agent_system(): Create complete agent system" << std::endl;
        std::cout << std::endl;
        std::cout << "Utilities:" << std::endl;
        std::cout << "  - nlm.py_to_vector(data): Convert Python list to vector" << std::endl;
        std::cout << "  - nlm.get_param(config, key, default): Get config parameter" << std::endl;
        std::cout << "  - nlm.validate_config(config): Validate configuration" << std::endl;
        std::cout << std::endl;
        std::cout << "See individual class and function documentation for details." << std::endl;
    }, "Print comprehensive help information for the NLM Python bindings.");

    m.def("get_documentation", []() {
        return "";
    }, "Get documentation string for the NLM Python bindings.");
} // namespace nlm

// Helper classes for context managers
struct ConfigContext {
    std::shared_ptr<Config> config_;
    ConfigContext(std::shared_ptr<Config> config) : config_(config) {}
};

struct WorldContext {
    std::shared_ptr<SimpleWorld> world_;
    WorldContext(std::shared_ptr<SimpleWorld> world) : world_(world) {}
};

struct BrainContext {
    std::shared_ptr<Brain> brain_;
    BrainContext(std::shared_ptr<Brain> brain) : brain_(brain) {}
};
