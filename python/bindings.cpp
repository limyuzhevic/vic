#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

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

// Enhanced Python bindings with better API design and comprehensive documentation

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ================================================
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides an intuitive, Pythonic interface to create and control
        artificial developmental brains. The API is designed to be accessible for
        beginners while providing advanced capabilities for researchers.
        
        Key Features:
        - Simple brain creation and configuration
        - World simulation for agent-environment interaction
        - Advanced sensory processing and motor control
        - Neuromodulation and learning systems
        - Comprehensive monitoring and debugging tools
        
        Basic Usage Example:
        
        >>> import pynlm
        >>> # Create a brain with default configuration
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Create a simple world
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(20, 20, visionWidth=8, visionHeight=8)
        >>> 
        >>> # Create an agent that uses the brain and world
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Enable learning capabilities
        >>> agent.enableRewardModulation(True)
        >>> agent.enableCuriosity(True)
        >>> 
        >>> # Run simulation
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     agent.processSensoryInput(world.getSensoryPercept())
        ...     brain.step(step)
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
        ...     
        ...     if step % 20 == 0:
        ...         print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        >>> 
        >>> print("Simulation complete!")
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ======================
    // Core Types and Identifiers
    // ======================

    // Neuron and synapse identifiers with improved API
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId is used to reference specific neurons within brain regions.
        You can create neuron IDs and compare them for equality.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             R"pbdoc(
             Get the index component of this neuron ID.
             
             Returns:
                 The index value used for identifying neurons within regions.
             )pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId is used to reference specific synaptic connections.
        Synapses can be identified and manipulated through their IDs.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             R"pbdoc(
             Get the index component of this synapse ID.
             
             Returns:
                 The index value used for identifying synapses within populations.
             )pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId identifies different neural regions within the brain.
        Each brain region contains populations of neurons with specific functions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index,
             R"pbdoc(
             Get the index component of this region ID.
             
             Returns:
                 The index value used for identifying brain regions.
             )pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId identifies groups of neurons within a brain region.
        Populations may contain neurons with similar types or functions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index,
             R"pbdoc(
             Get the index component of this population ID.
             
             Returns:
                 The index value used for identifying neuron populations.
             )pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ======================
    // Neuron and Synapse Types
    // ======================

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type of neuron, determining its function and behavior.
        
        Each neuron type has different characteristics:
        - Excitatory: Increases firing probability in target neurons
        - Inhibitory: Decreases firing probability in target neurons
        - Modulatory: Releases neuromodulators affecting neural plasticity
        - Sensory: Processes external or internal sensory information
        - Motor: Generates motor commands for action execution
        - Internal: Maintains internal states and homeostasis
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that excites target neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that inhibits target neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that releases neuromodulators")
        .value("Sensory", NeuronType::Sensory, "Neuron that processes sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that generates motor commands")
        .value("Internal", NeuronType::Internal, "Neuron that maintains internal states")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type of synaptic connection, determining its properties.
        
        Different synapse types have distinct characteristics:
        - Excitatory: Releases excitatory neurotransmitters
        - Inhibitory: Releases inhibitory neurotransmitters
        - Modulatory: Releases neuromodulators affecting plasticity
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Specialized electrical synapses
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Synapse releasing neuromodulators")
        .value("Electrical", SynapseType::Electrical, "Direct electrical connection")
        .value("GapJunction", SynapseType::GapJunction, "Specialized electrical synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Current developmental stage of the brain.
        
        The brain goes through distinct developmental phases:
        - Initial: Early developmental stage with high plasticity
        - CriticalPeriod: Phase of rapid learning and development
        - Maturation: Refinement of neural connections
        - Adult: Stable, adult-like brain configuration
        - Aging: Age-related changes and decline
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Early developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Rapid learning phase")
        .value("Maturation", DevelopmentalStage::Maturation, "Neural refinement phase")
        .value("Adult", DevelopmentalStage::Adult, "Stable adult configuration")
        .value("Aging", DevelopmentalStage::Aging, "Age-related changes")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Current firing state of a neuron.
        
        Neurons can be in different firing states:
        - Resting: Not currently firing
        - Active: Currently firing action potentials
        - Refractory: Cannot fire until recovery period ends
        - Inhibited: Actively prevented from firing
    )pbdoc")
        .value("Resting", FiringState::Resting, "Not currently firing")
        .value("Active", FiringState::Active, "Currently firing")
        .value("Refractory", FiringState::Refractory, "Cannot fire until recovery")
        .value("Inhibited", FiringState::Inhibited, "Actively prevented from firing")
        .export_values();

    // ======================
    // Action and Movement Types
    // ======================

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of action that can be performed by an agent.
        
        Actions represent goal-directed behaviors:
        - Movement: Basic positional changes
        - Interaction: Manipulating environment objects
        - Consumption: Eating or drinking
        - Exploration: Searching for novel stimuli
        - Rest: Recovery and maintenance
        - Custom: User-defined action patterns
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the direction of orientation")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward opposite to orientation")
        .value("MoveLeft", ActionType::MoveLeft, "Move left relative to orientation")
        .value("MoveRight", ActionType::MoveRight, "Move right relative to orientation")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left by rotating orientation")
        .value("TurnRight", ActionType::TurnRight, "Turn right by rotating orientation")
        .value("Look", ActionType::Look, "Look in a specific direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Eat a consumed resource")
        .value("Drink", ActionType::Drink, "Drink a consumed resource")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without changing state")
        .value("Custom", ActionType::Custom, "User-defined custom action")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command for agent movement and control.
        
        Motor commands are the basic actions that agents can execute:
        - Movement commands control position and orientation
        - Look commands control sensory focus
        - Interact commands enable environment manipulation
        - Wait command maintains current state
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with environment")
        .value("Wait", MotorCommand::Wait, "Wait without action")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the simulated world.
        
        Different objects have different properties and effects:
        - Resource: Provides energy when consumed
        - Hazard: Causes damage when touched
        - Wall: Impassable boundary
        - Marker: Special landmark for navigation
        - Empty: No object present
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object present")
        .value("Resource", WorldObjectType::Resource, "Provides energy when consumed")
        .value("Hazard", WorldObjectType::Hazard, "Causes damage when touched")
        .value("Wall", WorldObjectType::Wall, "Impassable boundary")
        .value("Marker", WorldObjectType::Marker, "Special landmark for navigation")
        .export_values();

    // ======================
    // Configuration System
    // ======================

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM parameters.
        
        Config manages all simulation parameters including:
        - Brain parameters (neuron count, regions, connections)
        - Simulation parameters (timestep, random seed)
        - Plasticity parameters (STDP, synaptogenesis, pruning)
        - Neuromodulation parameters (dopamine, curiosity)
        - Memory parameters (working and episodic memory)
        - Development parameters (critical periods, maturation)
        - Performance parameters (SIMD, parallel processing)
        
        The configuration system supports:
        - Loading from files (JSON, YAML, key=value format)
        - Command-line parameter overrides
        - Parameter validation and type checking
        - Saving configurations for reproducibility
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a file.
             
             Args:
                 filepath: Path to configuration file.
                           Supports JSON, YAML, and simple key=value format.
             
             Returns:
                 True if file was loaded successfully, False otherwise.
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command line arguments.
           
           Supports --key=value and -key value formats.
           
           Args:
               argc: Number of command line arguments.
               argv: Array of command line argument strings.
           
           Returns:
               True if arguments were processed successfully.
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a file.
             
             Args:
                 filepath: Path where configuration will be saved.
                           Supports JSON and YAML formats based on extension.
             
             Returns:
                 True if configuration was saved successfully, False otherwise.
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check.
             
             Returns:
                 True if the key exists, False otherwise.
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 List of all configuration keys currently set.
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             This removes all configuration settings, leaving only defaults.
             "))
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary of the current configuration.
             
             Returns:
                 String representation of current configuration settings.
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ======================
    // Sensory Input System
    // ======================

    // Base sensory input class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input data.
        
        SensoryInput represents external or internal stimuli that neurons can process.
        It provides a unified interface for different types of sensory data.
        
        Subclasses include:
        - Vision: Visual information from the environment
        - Audio: Auditory information from the environment
        - InternalSignals: Internal state information (energy, health)
    )pbdoc")
        .def("getType", &SensoryInput::getType, R"pbdoc(
            Get the type of sensory input.
            
            Returns:
                NeuronType enum value indicating the input type.
        )pbdoc")
        .def("getData", &SensoryInput::getData, R"pbdoc(
            Get the raw sensory data as a vector of floats.
            
            Returns:
                Vector containing the sensory data values.
        )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions, R"pbdoc(
            Get the dimensionality of the sensory data.
            
            Returns:
                Number of values in the sensory data vector.
        )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp, R"pbdoc(
            Get the timestamp when this sensory data was generated.
            
            Returns:
                Simulation timestamp when the data was created.
        )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set the timestamp when this sensory data was generated.
             
             Args:
                 timestamp: Simulation time when data was created.
             )pbdoc");

    // Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input representing the agent's visual perception.
        
        Vision data represents a 2D visual field as a flattened array of intensity
        values. Each value represents how bright that visual angle is, with 0.0 being
        dark and 1.0 being bright.
        
        Vision input is used by visual processing neurons to perceive the environment.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(
             Create a Vision sensory input with specified dimensions.
             
             Args:
                 width: Visual field width in pixels.
                 height: Visual field height in pixels.
                 channels: Number of color channels (default: 3 for RGB).
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
            R"pbdoc(
            Set the vision data values.
            
            Args:
                data: Vector of float values representing visual intensity.
                      Values should be in range [0.0, 1.0].
            )pbdoc")
        .def("getWidth", &Vision::getWidth, R"pbdoc(
            Get the width of the visual field.
            
            Returns:
                Visual field width in pixels.
        )pbdoc")
        .def("getHeight", &Vision::getHeight, R"pbdoc(
            Get the height of the visual field.
            
            Returns:
                Visual field height in pixels.
        )pbdoc")
        .def("getChannels", &Vision::getChannels, R"pbdoc(
            Get the number of color channels.
            
            Returns:
                Number of color channels in the vision data.
        )pbdoc");

    // Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing the agent's auditory perception.
        
        Audio data represents sound samples over time. It can be used to model
        auditory processing in biological systems.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(
             Create an Audio sensory input with specified parameters.
             
             Args:
                 sampleRate: Number of samples per second (Hz).
                 numSamples: Number of audio samples to store.
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
            R"pbdoc(
            Set the audio data samples.
            
            Args:
                data: Vector of float values representing audio samples.
            )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(
             Set the audio sample rate.
             
             Args:
                 sampleRate: Number of samples per second (Hz).
             )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate, R"pbdoc(
            Get the audio sample rate.
            
            Returns:
                Number of samples per second (Hz).
        )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples, R"pbdoc(
            Get the number of audio samples.
            
            Returns:
                Number of audio samples stored.
        )pbdoc");

    // Internal signals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal sensory input representing the agent's internal state.
        
        InternalSignals provides data about the agent's internal condition,
        including energy levels, health status, and behavioral states.
        
        This allows neurons to respond to the agent's needs and conditions.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(
             Add a signal value to the internal signals.
             
             Args:
                 value: Signal value to add (typically in range [0.0, 1.0]).
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals, R"pbdoc(
            Clear all internal signals.
        )pbdoc");

    // ======================
    // Action and Motor System
    // ======================

    py::class_<Action>(m, "Action", R"pbdoc(
        Representation of an action or motor command.
        
        Action encapsulates both the type of action to perform and any
        parameters needed for that action (e.g., movement speed, target position).
        
        Actions are produced by the brain's motor system and executed by the world.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(
             Create an action with a specific type.
             
             Args:
                 type: Type of action to perform.
             )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(
             Create an action with type and parameters.
             
             Args:
                 type: Type of action to perform.
                 parameters: Vector of float parameters for the action.
            )pbdoc")
        .def("getType", &Action::getType, R"pbdoc(
            Get the type of this action.
            
            Returns:
                ActionType enum value indicating the action type.
        )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(
             Set the type of this action.
             
             Args:
                 type: New action type.
             )pbdoc")
        .def("getParameters", &Action::getParameters, R"pbdoc(
            Get the action parameters.
            
            Returns:
                Vector of float parameters for the action.
        )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(
             Set the action parameters.
             
             Args:
                 params: New vector of float parameters.
             )pbdoc")
        .def("getName", &Action::getName, R"pbdoc(
            Get a human-readable name for this action type.
            
            Returns:
                String describing the action.
        )pbdoc")
        .def("clone", &Action::clone, R"pbdoc(
            Create a copy of this action.
            
            Returns:
                New Action object with same type and parameters.
        )pbdoc");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Representation of an object in the simulated world.
        
        World objects include resources, hazards, walls, and markers that agents
        can interact with. Each object has a position, type, and properties.
        
        Objects can be consumed, damaged, or used for navigation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             R"pbdoc(
             Create a world object.
             
             Args:
                 x: X-coordinate in the world.
                 y: Y-coordinate in the world.
                 type: Type of object (Resource, Hazard, Wall, etc.).
                 value: Value associated with the object (energy, damage).
                 radius: Radius of the object for collision detection.
             )pbdoc")
        .def_readwrite("x", &WorldObject::x, R"pbdoc(
            X-coordinate of the object in the world.
        )pbdoc")
        .def_readwrite("y", &WorldObject::y, R"pbdoc(
            Y-coordinate of the object in the world.
        )pbdoc")
        .def_readwrite("radius", &WorldObject::radius, R"pbdoc(
            Radius of the object for collision detection.
        )pbdoc")
        .def_readwrite("type", &WorldObject::type, R"pbdoc(
            Type of the object (Resource, Hazard, Wall, etc.).
        )pbdoc")
        .def_readwrite("value", &WorldObject::value, R"pbdoc(
            Value associated with the object (energy, damage, etc.).
        )pbdoc")
        .def_readwrite("active", &WorldObject::active, R"pbdoc(
            Whether the object is active (can be interacted with).
        )pbdoc");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Physical state representation of an agent in the world.
        
        AgentBody stores all physical properties of an agent including:
        - Position and orientation
        - Velocity and angular velocity
        - Energy and health levels
        - Movement and turning states
        - Action timing and age
        
        This state is updated by the world simulation based on actions.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, R"pbdoc(
            X-coordinate in the world.
        )pbdoc")
        .def_readwrite("y", &AgentBody::y, R"pbdoc(
            Y-coordinate in the world.
        )pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation, R"pbdoc(
            Orientation angle in radians (-PI to PI).
        )pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX, R"pbdoc(
            X-component of linear velocity.
        )pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY, R"pbdoc(
            Y-component of linear velocity.
        )pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, R"pbdoc(
            Angular velocity (turning speed).
        )pbdoc")
        .def_readwrite("energy", &AgentBody::energy, R"pbdoc(
            Current energy level (0.0 to maxEnergy).
        )pbdoc")
        .def_readwrite("health", &AgentBody::health, R"pbdoc(
            Current health level (0.0 to 1.0).
        )pbdoc")
        .def_readwrite("age", &AgentBody::age, R"pbdoc(
            Age of the agent in simulation time.
        )pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving, R"pbdoc(
            Whether the agent is currently moving.
        )pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning, R"pbdoc(
            Whether the agent is currently turning.
        )pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, R"pbdoc(
            Time when the last action was executed.
        )pbdoc")
        .def("reset", &AgentBody::reset, R"pbdoc(
            Reset agent body to initial state.
        )pbdoc");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of applying a motor command to the world.
        
        ActionResult contains information about what happened when an agent
        performed an action, including:
        - Reward received (positive or negative)
        - Whether the action was successful
        - Descriptive message about the outcome
        
        This information is used by learning systems to update behavior.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(
             Create an action result.
             
             Args:
                 reward: Reward value (positive or negative).
                 success: Whether the action was successful.
                 message: Descriptive message about the outcome.
             )pbdoc")
        .def_readwrite("reward", &ActionResult::reward, R"pbdoc(
            Reward value received from the action (-inf to +inf).
        )pbdoc")
        .def_readwrite("success", &ActionResult::success, R"pbdoc(
            Whether the action was successful.
        )pbdoc")
        .def_readwrite("message", &ActionResult::message, R"pbdoc(
            Descriptive message about the action outcome.
        )pbdoc");

    // ======================
    // Agent Interface System
    // ======================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Collection of sensory inputs perceived by an agent.
        
        SensoryPercept encapsulates all sensory information available to an agent,
        including:
        - Vision: What the agent sees
        - Touch: What the agent feels nearby
        - Internal: The agent's internal state
        - Proprioception: The agent's body position and velocity
        - Audio: What the agent hears
        
        This is the main interface through which agents receive sensory information.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, R"pbdoc(
            Get the visual input data.
            
            Returns:
                Vector of float values representing visual intensity.
        )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(
             Set the visual input data.
             
             Args:
                 vision: Vector of float values representing visual intensity.
            )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, R"pbdoc(
            Get the width of the visual field.
            
            Returns:
                Visual field width in pixels.
        )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, R"pbdoc(
            Get the height of the visual field.
            
            Returns:
                Visual field height in pixels.
        )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch, R"pbdoc(
            Get the touch/proximity input data.
            
            Returns:
                Vector of float values representing touch intensity.
        )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(
             Set the touch/proximity input data.
             
             Args:
                 touch: Vector of float values representing touch intensity.
            )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal, R"pbdoc(
            Get the internal signals data.
            
            Returns:
                Vector of float values representing internal state.
        )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(
             Set the internal signals data.
             
             Args:
                 internal: Vector of float values representing internal state.
            )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception, R"pbdoc(
            Get the proprioception data.
            
            Returns:
                Vector of float values representing body position/velocity.
        )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(
             Set the proprioception data.
             
             Args:
                 proprioception: Vector of float values representing body state.
            )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio, R"pbdoc(
            Get the audio input data.
            
            Returns:
                Vector of float values representing audio samples.
        )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(
             Set the audio input data.
             
             Args:
                 audio: Vector of float values representing audio samples.
            )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals, R"pbdoc(
            Get all sensory signals as a single vector.
            
            Returns:
                Vector containing vision, touch, internal, proprioception, and audio data.
        )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp, R"pbdoc(
            Get the timestamp when this percept was created.
            
            Returns:
                Simulation timestamp when the percept was created.
        )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set the timestamp when this percept was created.
             
             Args:
                 timestamp: Simulation time when the percept was created.
             )pbdoc");

    // ======================
    // World Simulation
    // ======================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides a basic physics-based simulation environment where
        agents can move, interact with objects, and receive sensory input.
        
        The world includes:
        - Continuous physics simulation
        - Collision detection and response
        - Object management (resources, hazards, walls)
        - Vision sensing (raycasting)
        - Agent body state management
        
        This is the primary environment where agents learn and behave.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure the world dimensions and sensing parameters.
             
             Args:
                 width: World width in simulation units.
                 height: World height in simulation units.
                 visionWidth: Width of agent's visual field in pixels.
                 visionHeight: Height of agent's visual field in pixels.
             )pbdoc")
        .def("reset", &SimpleWorld::reset, R"pbdoc(
            Reset the world to its initial state.
            
            This places the agent at a random start position and
            spawns initial objects (resources and hazards).
        )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set the agent's starting position.
             
             Args:
                 x: X-coordinate for agent start position.
                 y: Y-coordinate for agent start position.
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
             Update the world simulation by one timestep.
             
             Args:
                 timestep: Duration of the simulation step in seconds.
            )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply a motor command and get the result.
             
             Args:
                 cmd: Motor command to execute.
                 currentTime: Current simulation time.
             
             Returns:
                 ActionResult describing the outcome of the action.
            )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the current sensory percept available to the agent.
             
             Returns:
                 SensoryPercept object containing all sensory information.
            )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the agent's current body state.
             
             Returns:
                 AgentBody object with position, velocity, and other state.
            )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add an object to the world.
             
             Args:
                 obj: WorldObject to add to the world.
            )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove an object at the specified position.
             
             Args:
                 x: X-coordinate where object should be removed.
                 y: Y-coordinate where object should be removed.
            )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if a position is valid (within world boundaries).
             
             Args:
                 x: X-coordinate to check.
                 y: Y-coordinate to check.
             
             Returns:
                 True if position is valid, False otherwise.
            )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth, R"pbdoc(
            Get the world width.
            
            Returns:
                World width in simulation units.
        )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight, R"pbdoc(
            Get the world height.
            
            Returns:
                World height in simulation units.
        )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, R"pbdoc(
            Get the maximum energy level for agents.
            
            Returns:
                Maximum energy value.
        )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
             Set the maximum energy level for agents.
             
             Args:
                 e: New maximum energy value.
            )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, R"pbdoc(
            Get the energy decay rate per timestep.
            
            Returns:
                Energy decay rate (energy lost per second).
        )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
             Set the energy decay rate per timestep.
             
             Args:
                 r: New energy decay rate.
            )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, R"pbdoc(
            Get the current simulation time.
            
            Returns:
                Current simulation time in seconds.
        )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set the random seed for reproducible object placement.
             
             Args:
                 seed: Random seed value.
            )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, R"pbdoc(
            Get the random seed.
            
            Returns:
                Current random seed value.
        )pbdoc");

    // ======================
    // Core Brain System
    // ======================

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Brain is the core computational component of NLM, implementing:
        - Spiking neural dynamics using Leaky Integrate-and-Fire neurons
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity rules (STDP, Hebbian, reward-modulated)
        - Integrated memory systems (working, episodic, semantic)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction systems for forward modeling
        - Cognitive systems (planning, concept formation, attention)
        - Developmental system for plasticity changes over time
        
        The brain processes sensory input, produces motor output, and learns
        from experience through synaptic plasticity.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a brain with configuration.
             
             Args:
                 config: Configuration object containing brain parameters.
            )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with current configuration.
             
             This creates neural regions, neurons, synapses, and initializes
             all internal systems.
             
             Returns:
                 True if initialization succeeded, False otherwise.
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a simulation step with step number.
             
             Args:
                 currentStep: Current step number in simulation.
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with timestamp.
             
             Args:
                 currentStep: Current step number in simulation.
                 currentTime: Current simulation time in seconds.
            )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             Args:
                 input: SensoryInput object containing perception data.
            )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current directly into a specific neuron.
             
             Args:
                 neuron: Neuron ID of the target neuron.
                 current: Amount of current to inject (microamps).
            )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type to target (Excitatory, Inhibitory, etc.).
                 current: Amount of current to inject (microamps).
            )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity.
             
             Returns:
                 Action object representing the motor output.
            )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial conditions.
             
             This clears all neural states and prepares for a new episode.
            )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to a file (checkpointing).
             
             Args:
                 filepath: Path where brain state will be saved.
            
             Returns:
                 True if save succeeded, False otherwise.
            )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from a file.
             
             Args:
                 filepath: Path from which brain state will be loaded.
            
            Returns:
                True if load succeeded, False otherwise.
            )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region to the brain.
             
             Args:
                 name: Optional name for the new region.
            
            Returns:
                RegionId of the newly created region.
            )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a region by its ID.
             
             Args:
                 id: RegionId of the region to retrieve.
             
            Returns:
                NeuralRegion object if found, None otherwise.
            )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions in the brain.
             
             Returns:
                 Number of neural regions.
            )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs in the brain.
             
             Returns:
                 Vector of RegionId objects representing all regions.
            )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all regions in the brain.
             
             Returns:
                 Vector of unique_ptr to NeuralRegion objects.
            )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all regions.
             
             Returns:
                 Total number of neurons in the brain.
            )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count across all regions.
             
             Returns:
                 Total number of synapses in the brain.
            )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get count of currently active neurons.
             
             Returns:
                 Number of neurons currently firing.
            )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons.
             
             Returns:
                 Number of neurons currently emitting action potentials.
            )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons.
             
             Returns:
                 Average firing rate in Hz (spikes per second).
            )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio.
             
             Returns:
                 Ratio of excitatory to inhibitory activity.
            )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count.
             
             Returns:
                 Total number of spikes emitted by all neurons.
            )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 Current developmental stage enum value.
            )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
             Set developmental stage manually.
             
             Args:
                 stage: Developmental stage to set.
            )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the brain's configuration.
             
             Returns:
                 Shared pointer to the Config object.
            )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status information.
             
             This outputs detailed information about the current brain state
             including neuron counts, firing rates, and system status.
            )pbdoc");

    // ======================
    // Agent Interface System
    // ======================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain provides the interface between a brain and its environment,
        handling:
        - Sensory input processing and injection into brain
        - Motor output decoding from brain activity
        - Neuromodulation application based on outcomes
        - Development system updates
        - Learning system management
        
        This class encapsulates the agent-specific functionality needed
        to interface with the world simulation.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Create an agent brain interface.
             
             Args:
                 brain: Shared pointer to the underlying Brain object.
            )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize the agent with a world.
             
             Args:
                 world: Shared pointer to the SimpleWorld object.
            )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
             Get expected sensory input size.
             
             Returns:
                 Number of sensory values the agent expects.
            )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
             Get expected motor output size.
             
             Returns:
                 Number of motor commands the agent can produce.
            )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain.
             
             Args:
                 percept: SensoryPercept object containing sensory data.
            )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand that best matches current brain activity.
            )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received.
                 predictedReward: Expected reward before action.
            )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system.
             
             Args:
                 timestep: Duration of the development update in seconds.
            )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 Current developmental stage enum value.
            )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
             Get current neuromodulation level.
             
             Returns:
                 Current neuromodulation level (-1.0 to 1.0).
            )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get curiosity level.
             
             Returns:
                 Current curiosity level (0.0 to 1.0).
            )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get novelty level.
             
             Returns:
                 Current novelty level (0.0 to 1.0).
            )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get prediction error.
             
             Returns:
                 Current prediction error (-1.0 to 1.0).
            )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
             Reset agent for new episode.
             
             This clears neuromodulation states and resets agent for
             a new learning episode.
            )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the underlying brain.
             
             Returns:
                 Shared pointer to the Brain object.
            )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable reward modulation learning.
             
             Args:
                 enable: True to enable, False to disable.
            )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable structural plasticity.
             
             Args:
                 enable: True to enable, False to disable.
            )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable developmental processes.
             
             Args:
                 enable: True to enable, False to disable.
            )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable curiosity-driven exploration.
             
             Args:
                 enable: True to enable, False to disable.
            )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(
             Check if reward modulation is enabled.
             
             Returns:
                 True if reward modulation is enabled, False otherwise.
            )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(
             Check if structural plasticity is enabled.
             
             Returns:
                 True if structural plasticity is enabled, False otherwise.
            )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(
             Check if development is enabled.
             
             Returns:
                 True if development is enabled, False otherwise.
            )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(
             Check if curiosity is enabled.
             
             Returns:
                 True if curiosity is enabled, False otherwise.
            )pbdoc");

    // ======================
    // Factory Functions
    // ======================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration with sensible starting values.
        
        Returns:
            Shared pointer to a new Config object with default parameters.
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with configuration.
        
        Args:
            config: Configuration object for the brain.
        
        Returns:
            Shared pointer to a new Brain object.
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world simulation.
        
        Returns:
            Shared pointer to a new SimpleWorld object.
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.
        
        Args:
            brain: Shared pointer to the underlying Brain object.
        
        Returns:
            Shared pointer to a new AgentBrain object.
    )pbdoc");

    // ======================
    // Constants and Invalid Values
    // ======================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ======================
    // Python-Specific Enhancements
    // ======================

    // Helper functions for common operations
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent, int numSteps) {
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
        R"pbdoc(
        Run a complete simulation episode.
        
        This is a convenience function that runs the full agent-world loop
        for a specified number of steps.
        
        Args:
            brain: The agent's brain.
            world: The simulation world.
            agent: The agent interface.
            numSteps: Number of simulation steps to run.
        )pbdoc");

    // Performance monitoring helper
    m.def("measurePerformance", [](std::function<void()> func, const std::string& operationName) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        nlm::Logger::getGlobal().info("[PERFORMANCE] " + operationName + ": " + 
                                     std::to_string(duration.count()) + "ms");
    }, py::arg("func"), py::arg("operationName"),
        R"pbdoc(
        Measure performance of a function.
        
        Args:
            func: Function to execute and measure.
            operationName: Name of the operation for logging.
        )pbdoc");

    // Configuration comparison helper
    m.def("compareConfigs", [](const Config& config1, const Config& config2) {
        auto keys1 = config1.getKeys();
        auto keys2 = config2.getKeys();
        
        std::vector<std::string> onlyIn1, onlyIn2, same;
        
        for (const auto& key : keys1) {
            if (config2.has(key)) {
                if (config1.getValue(key) != config2.getValue(key)) {
                    onlyIn1.push_back(key);
                } else {
                    same.push_back(key);
                }
            } else {
                onlyIn1.push_back(key);
            }
        }
        
        for (const auto& key : keys2) {
            if (!config1.has(key)) {
                onlyIn2.push_back(key);
            }
        }
        
        nlm::Logger::getGlobal().info("Config differences:");
        if (!onlyIn1.empty()) {
            nlm::Logger::getGlobal().info("  Only in first config: " + 
                                         "").join(onlyIn1.begin(), onlyIn1.end(), ", "));
        }
        if (!onlyIn2.empty()) {
            nlm::Logger::getGlobal().info("  Only in second config: " + 
                                         "").join(onlyIn2.begin(), onlyIn2.end(), ", "));
        }
        if (!same.empty()) {
            nlm::Logger::getGlobal().info("  Same in both configs: " + 
                                         "").join(same.begin(), same.end(), ", "));
    }, py::arg("config1"), py::arg("config2"),
        R"pbdoc(
        Compare two configurations and log differences.
        
        Args:
            config1: First configuration to compare.
            config2: Second configuration to compare.
        )pbdoc");

    // Save simulation state helper
    m.def("saveSimulationState", [](const std::string& brainPath, const std::string& configPath) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (brain->save(brainPath)) {
            nlm::Logger::getGlobal().info("Brain state saved to: " + brainPath);
        } else {
            nlm::Logger::getGlobal().error("Failed to save brain state to: " + brainPath);
        }
        
        if (config->saveToFile(configPath)) {
            nlm::Logger::getGlobal().info("Configuration saved to: " + configPath);
        } else {
            nlm::Logger::getGlobal().error("Failed to save configuration to: " + configPath);
        }
    }, py::arg("brainPath"), py::arg("configPath"),
        R"pbdoc(
        Save brain and configuration state to files.
        
        Args:
            brainPath: Path where brain state will be saved.
            configPath: Path where configuration will be saved.
        )pbdoc");

    // Load simulation state helper
    m.def("loadSimulationState", [](const std::string& brainPath, const std::string& configPath) {
        auto config = std::make_shared<Config>();
        
        if (config->loadFromFile(configPath)) {
            nlm::Logger::getGlobal().info("Configuration loaded from: " + configPath);
        } else {
            nlm::Logger::getGlobal().error("Failed to load configuration from: " + configPath);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (brain->load(brainPath)) {
            nlm::Logger::getGlobal().info("Brain state loaded from: " + brainPath);
        } else {
            nlm::Logger::getGlobal().error("Failed to load brain state from: " + brainPath);
        }
        
        return brain;
    }, py::arg("brainPath"), py::arg("configPath"),
        R"pbdoc(
        Load brain and configuration state from files.
        
        Args:
            brainPath: Path from which brain state will be loaded.
            configPath: Path from which configuration will be loaded.
        
        Returns:
            Shared pointer to the loaded Brain object.
        )pbdoc");

    // Educational examples helper
    m.def("example_simple_brain", []() {
        nlm::Logger::getGlobal().info("=== Simple Brain Example ===");
        nlm::Logger::getGlobal().info("Creating a brain with default configuration...");
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        nlm::Logger::getGlobal().info("Initializing brain...");
        if (brain->initialize()) {
            nlm::Logger::getGlobal().info("Brain initialized successfully!");
            nlm::Logger::getGlobal().info("Brain status:");
            brain->logStatus();
        } else {
            nlm::Logger::getGlobal().error("Failed to initialize brain");
        }
        
        nlm::Logger::getGlobal().info("Running 10 simulation steps...");
        for (int step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
            nlm::Logger::getGlobal().info("Step " + std::to_string(step) + 
                                         ": " + std::to_string(brain->getTotalSpikeCount()) + " spikes");
        }
        
        nlm::Logger::getGlobal().info("=== Example Complete ===");
        return brain;
    }, R"pbdoc(
        Run a simple educational example demonstrating basic brain usage.
        
        This example shows:
        - How to set up a brain with default configuration
        - How to initialize the brain
        - How to run simulation steps
        - How to observe brain activity
        
        Returns:
            Shared pointer to the created Brain object.
    )pbdoc");

    // Quick start helper
    m.def("quickStart", []() {
        nlm::Logger::getGlobal().info("=== NLM Quick Start ===");
        nlm::Logger::getGlobal().info("This creates a complete working example...");
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        world->configure(20, 20, visionWidth: 8, visionHeight: 8);
        world->reset();
        agent->initialize(world);
        
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        nlm::Logger::getGlobal().info("Running 50-step simulation...");
        for (int step = 0; step < 50; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (step % 10 == 0) {
                nlm::Logger::getGlobal().info("Step " + std::to_string(step) + 
                                             ": " + std::to_string(brain->getFiringNeuronCount()) + 
                                             " neurons firing, " + std::to_string(brain->getTotalSpikeCount()) + 
                                             " total spikes");
            }
        }
        
        nlm::Logger::getGlobal().info("=== Quick Start Complete ===");
        nlm::Logger::getGlobal().info("Simulation completed successfully!");
        return brain;
    }, R"pbdoc(
        Quick start guide for NLM beginners.
        
        This function creates a complete working example showing:
        - How to set up a brain, world, and agent
        - How to configure the world
        - How to run a simulation
        - How to observe agent behavior
        
        Returns:
            Shared pointer to the created Brain object.
    )pbdoc");

} // namespace nlm
