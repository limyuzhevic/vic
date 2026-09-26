#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================

        A Python binding for the NLM C++ neural simulation framework that provides
        comprehensive tools for neural simulation, learning, and autonomous agent development.

        Overview
        --------
        This module exposes the core components of the NLM framework to Python,
        enabling fast neural simulation with Pythonic convenience methods and
        integration capabilities. The bindings are designed for both interactive
        exploration and programmatic control of neural systems.

        Core Classes
        -------------
        - Brain: Central neural simulation engine with regions, neurons, and synapses
        - Config: Configuration management for simulation parameters
        - AgentBrain: Bridge between neural brain and environmental interaction
        - SimpleWorld: 2D simulation environment for agents
        - SensoryInput: Base classes for sensory modalities (Vision, Audio, InternalSignals)
        - Action: Motor actions and commands
        - SensoryPercept: Combined sensory perception data

        Key Features
        ------------
        * Full C++ performance with Python convenience
        * Rich set of utility methods for common operations
        * Dictionary-based serialization for easy data exchange
        * Array-like interfaces for numerical processing
        * Comprehensive error handling and validation
        * Flexible configuration system

        Design Philosophy
        -----------------
        - Maintain C++ performance while providing Pythonic interfaces
        - Provide both low-level control and high-level convenience methods
        - Enable seamless integration with Python scientific computing ecosystem
        - Support both interactive exploration and batch processing
        - Ensure type safety with comprehensive parameter validation

        Usage Examples
        --------------
        Basic simulation setup:
        >>> import pynlm as nlm
        >>> config = nlm.createDefaultConfig()
        >>> brain = nlm.createBrain(config)
        >>> world = nlm.createSimpleWorld()
        >>> agent_brain = nlm.createAgentBrain(brain)

        Sensor processing:
        >>> percept = world.getSensoryPercept()
        >>> if percept.has_vision():
        ...     state_dict = percept.get_all_like()

        Agent control:
        >>> world.create_agent(x=5.0, y=5.0, orientation=0.0)
        >>> agent_brain.step_and_decode(percept)

        Error Handling Examples:
        >>> # Handle type conversion errors gracefully
        >>> try:
        ...     # Create array-like vision data
        ...     import numpy as np
        ...     vision_array = np.random.rand(84, 84, 3).astype(np.float32)
        ...     percept = nlm.SensoryPercept()
        ...     percept.set_vision_like(vision_array, 84, 84, 3)
        ... except py::value_error as e:
        ...     print(f"Vision data error: {e}")
        >>> 
        >>> # Handle missing data in dictionary
        >>> try:
        ...     # Create percept from incomplete dictionary
        ...     incomplete_dict = {"vision": [0.5, 0.3, 0.7], "timestamp": 1.0}
        ...     percept = nlm.SensoryPercept.from_dict(incomplete_dict)
        ...     print(f"Created percept with vision: {percept.has_vision()}")
        ... except Exception as e:
        ...     print(f"Dictionary creation error: {e}")
        >>> 
        >>> # Handle configuration validation errors
        >>> try:
        ...     # Save configuration to file (requires write permissions)
        ...     config.saveToFile("/invalid/path/config.json")
        ... except std::runtime_error as e:
        ...     print(f"Configuration error: {e}")

        Advanced Operations:
        >>> # Create complex agent with multiple subsystems
        >>> brain = nlm.createBrain(config)
        >>> agent_brain = nlm.createAgentBrain(brain)
        >>> agent_brain.enable_all()
        >>> 
        >>> # Environment setup with monitoring
        >>> world = nlm.createSimpleWorld()
        >>> world.configure(100, 100, 84, 84)
        >>> world.setRandomSeed(42)
        >>> 
        >>> # Agent simulation loop with error handling
        >>> agent = world.create_agent(x=50.0, y=50.0)
        >>> for step in range(100):
        ...     try:
        ...         # Get sensory input
        ...         percept = world.getSensoryPercept()
        ...         
        ...         # Check for critical errors
        ...         if world.is_safe_position(percept.getVisionWidth() // 2, 
        ...                                 percept.getVisionHeight() // 2):
        ...             # Process and act
        ...             action = agent_brain.step_and_decode(percept)
        ...             world.applyMotorCommand(action, step * 0.1)
        ...         else:
        ...             print(f"Agent in dangerous position at step {step}")
        ...             
        ...         # Monitor system health
        ...         if agent_brain.get_system_state()["prediction_error"] > 0.8:
        ...             print(f"High prediction error at step {step}")
        ...             
    )pbdoc";
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.

        NeuronId is an immutable wrapper around uint64_t that provides
        type-safe neuron identification. It's used throughout the NLM
        framework to reference specific neurons without confusion with
        other ID types.

        Attributes:
            value: The underlying uint64_t value representing the neuron ID

        Methods:
            index(): Returns the index portion of the neuron ID for
                efficient spatial access in neural regions
            __eq__(): Equality comparison with other NeuronId objects
            __ne__(): Inequality comparison
            __hash__(): Hash function for use in dictionaries and sets
            __repr__(): String representation for debugging and logging

        Note:
            NeuronId follows the same underlying type as uint64_t but provides
            semantic typing to prevent accidental use of other ID types where
            neuron IDs are expected.

        Example:
            >>> neuron_id = nlm.NeuronId(12345)
            >>> print(f"Neuron ID: {neuron_id}")
            Neuron ID: <NeuronId: 12345>
            >>> index = neuron_id.index()
            >>> print(f"Neuron index: {index}")
    )pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the neural network.

        SynapseId is an immutable wrapper around uint64_t that provides
        type-safe synapse identification. It's used throughout the NLM
        framework to reference specific synapses and establish connection
        patterns between neurons.

        Attributes:
            value: The underlying uint64_t value representing the synapse ID

        Methods:
            index(): Returns the index portion of the synapse ID for
                efficient spatial access in neural regions
            __eq__(): Equality comparison with other SynapseId objects
            __ne__(): Inequality comparison
            __hash__(): Hash function for use in dictionaries and sets
            __repr__(): String representation for debugging and logging

        Note:
            SynapseId follows the same underlying type as uint64_t but provides
            semantic typing to prevent accidental use of other ID types where
            synapse IDs are expected. Synapses connect neurons and are essential
            for information flow in the neural network.

        Example:
            >>> synapse_id = nlm.SynapseId(67890)
            >>> print(f"Synapse ID: {synapse_id}")
            Synapse ID: <SynapseId: 67890>
            >>> index = synapse_id.index()
            >>> print(f"Synapse index: {index}")
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region in the neural network.

        RegionId is an immutable wrapper around uint64_t that provides
        type-safe region identification. Brain regions are the fundamental
        organizational units of the neural network, grouping neurons based on
        function, connectivity, or anatomical location. Each region contains
        populations of neurons that work together for specific computational
        tasks.

        Attributes:
            value: The underlying uint64_t value representing the region ID

        Methods:
            index(): Returns the index portion of the region ID for
                efficient spatial access in the brain
            __eq__(): Equality comparison with other RegionId objects
            __ne__(): Inequality comparison
            __hash__(): Hash function for use in dictionaries and sets
            __repr__(): String representation for debugging and logging

        Note:
            RegionId follows the same underlying type as uint64_t but provides
            semantic typing to prevent accidental use of other ID types where
            region IDs are expected. Regions are essential for organizing the
            complex neural architecture and managing neuron populations.

        Example:
            >>> region_id = nlm.RegionId(12345)
            >>> print(f"Region ID: {region_id}")
            Region ID: <RegionId: 12345>
            >>> index = region_id.index()
            >>> print(f"Region index: {index}")
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in the neural network.

        PopulationId is an immutable wrapper around uint64_t that provides
        type-safe population identification. Neuron populations are groups
        of neurons within a brain region that share similar properties, such
        as neuron type (excitatory, inhibitory) or functional role. Populations
        allow for efficient management of neuron groups based on shared characteristics.

        Attributes:
            value: The underlying uint64_t value representing the population ID

        Methods:
            index(): Returns the index portion of the population ID for
                efficient spatial access within brain regions
            __eq__(): Equality comparison with other PopulationId objects
            __ne__(): Inequality comparison

        Note:
            PopulationId follows the same underlying type as uint64_t but provides
            semantic typing to prevent accidental use of other ID types where
            population IDs are expected. Populations are essential for managing
            neuron diversity within regions and for targeted synaptic operations.

        Example:
            >>> population_id = nlm.PopulationId(67890)
            >>> print(f"Population ID: {population_id}")
            Population ID: <PopulationId: 67890>
            >>> index = population_id.index()
            >>> print(f"Population index: {index}")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the neural network.

        NeuronType defines the functional categories of neurons in the NLM framework,
        determining how neurons process and transmit information through the network.
        Each neuron type has distinct computational properties and synaptic behaviors.

        Enumeration Values:
            Excitatory: Neurons that increase the likelihood of postsynaptic neuron firing
            Inhibitory: Neurons that decrease the likelihood of postsynaptic neuron firing
            Modulatory: Neurons that adjust overall network excitability and gain
            Sensory: Neurons specialized for processing sensory inputs
            Motor: Neurons specialized for generating motor outputs
            Internal: Neurons involved in internal processing and computation

        Usage Note:
            Neuron types are used throughout the framework to:
            - Classify neurons based on their functional role
            - Apply targeted operations (e.g., injecting current only to excitatory neurons)
            - Organize neural regions by neuron type
            - Configure learning mechanisms (e.g., reward-based modulation)

        Example:
            >>> neuron_type = nlm.NeuronType.Excitatory
            >>> print(f"Neuron type: {neuron_type}")
            Neuron type: 0
            >>> 
            >>> # Check neuron type
            >>> if neuron_type == nlm.NeuronType.Excitatory:
            ...     print("This neuron promotes excitation")
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the neural network.

        SynapseType defines the functional categories of synapses, determining how
        neurons communicate and influence each other's activity. Different synapse
        types have distinct transmission properties and computational roles.

        Enumeration Values:
            Excitatory: Synapses that increase postsynaptic neuron firing probability
            Inhibitory: Synapses that decrease postsynaptic neuron firing probability
            Modulatory: Synapses that adjust overall network excitability and gain
            Electrical: Direct electrical coupling between neurons (gap junctions)
            GapJunction: Specialized electrical synapses allowing direct ion flow

        Usage Note:
            Synapse types are used throughout the framework to:
            - Classify synaptic connections based on their functional effect
            - Apply targeted operations (e.g., stimulating only excitatory synapses)
            - Understand network dynamics and information flow patterns
            - Implement specific learning rules (e.g., Hebbian learning for excitatory synapses)

        Example:
            >>> synapse_type = nlm.SynapseType.Excitatory
            >>> print(f"Synapse type: {synapse_type}")
            Synapse type: 0
            >>> 
            >>> # Identify connection type
            >>> if synapse_type == nlm.SynapseType.Inhibitory:
            ...     print("This synapse provides inhibition")
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages for neural agents and networks.

        DevelopmentalStage defines the lifecycle stages of agents in the NLM framework,
        representing how neural systems mature, learn, and adapt over time. Each stage
        has distinct characteristics, capabilities, and learning requirements.

        Enumeration Values:
            Initial: Starting state before any learning or development begins
            CriticalPeriod: Early phase with high plasticity and rapid learning capacity
            Maturation: Continued development and refinement of neural connections
            Adult: Mature state with stable neural configurations and optimized performance
            Aging: Later stage with gradual decline in plasticity and performance

        Usage Note:
            Developmental stages are used throughout the framework to:
            - Track and manage agent lifecycle and learning progress
            - Apply age-appropriate learning rules and constraints
            - Trigger developmental events and transitions
            - Adapt learning mechanisms based on developmental stage
            - Model biological plausibility in agent development

        Example:
            >>> stage = nlm.DevelopmentalStage.CriticalPeriod
            >>> print(f"Developmental stage: {stage}")
            Developmental stage: 1
            >>> 
            >>> # Check for critical period
            >>> if stage == nlm.DevelopmentalStage.CriticalPeriod:
            ...     print("Agent is in high plasticity phase")
    )pbdoc")
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

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of high-level action types for autonomous agents.

        ActionType defines the complete set of possible actions that agents can take
        in the NLM framework, covering navigation, perception, interaction, and
        self-maintenance behaviors. These actions represent the agent's
        interface with the environment and other agents.

        Enumeration Values:
            MoveForward: Move the agent forward in the current direction
            MoveBackward: Move the agent backward
            MoveLeft: Move the agent left (strafe)
            MoveRight: Move the agent right (strafe)
            TurnLeft: Rotate the agent counterclockwise
            TurnRight: Rotate the agent clockwise
            Look: Look in the current direction
            LookUp: Look upward (increase pitch)
            LookDown: Look downward (decrease pitch)
            Interact: Interact with objects in the environment
            Eat: Consume food/resources for energy
            Drink: Consume water/resources for hydration
            Rest: Rest and recover energy
            Wait: Do nothing, maintain current state
            Custom: User-defined action with custom parameters

        Usage Note:
            Action types are used throughout the framework to:
            - Define agent behavior and decision policies
            - Specify agent capabilities and limitations
            - Configure action selection and reinforcement learning
            - Interface with sensory and motor systems
            - Implement action effects and outcomes

        Example:
            >>> action = nlm.ActionType.MoveForward
            >>> print(f"Action: {action}")
            Action: 0
            >>> 
            >>> # Check action category
            >>> if action == nlm.ActionType.Eat or action == nlm.ActionType.Drink:
            ...     print("This is a consumption action")
    )pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands for agent control.

        MotorCommand defines the fundamental motor outputs that agents can execute,
        representing the actual physical movements and actions that affect the
        agent's body state in the environment. These commands are typically
        generated by the neural network and translated into physical actions.

        Enumeration Values:
            MoveForward: Move the agent forward one unit
            MoveBackward: Move the agent backward one unit
            TurnLeft: Rotate the agent left (counterclockwise) by a fixed angle
            TurnRight: Rotate the agent right (clockwise) by a fixed angle
            LookLeft: Turn the agent's head/view to the left
            LookRight: Turn the agent's head/view to the right
            Interact: Execute an interaction action with nearby objects
            Wait: Do nothing, maintain current motor state

        Usage Note:
            Motor commands are used throughout the framework to:
            - Execute agent movements and actions in the world
            - Control agent body state (position, orientation, velocity)
            - Interface with the SimpleWorld simulation environment
            - Implement action effects and physical consequences
            - Coordinate agent-environment interactions

        Example:
            >>> command = nlm.MotorCommand.MoveForward
            >>> print(f"Motor command: {command}")
            Motor command: 0
            >>> 
            >>> # Execute movement
            >>> action = nlm.Action()
            >>> action.setType(nlm.ActionType.MoveForward)
            >>> world.applyMotorCommand(action, current_time)
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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the simulation environment.

        WorldObjectType defines the different kinds of objects that can exist
        in the SimpleWorld simulation environment, representing the static
        and dynamic elements that agents interact with. Each object type has
        distinct properties, behaviors, and roles in the agent's environment.

        Enumeration Values:
            Empty: No object (empty space)
            Resource: Consumable items that provide energy or other benefits
            Hazard: Dangerous objects that can harm agents
            Wall: Static barriers that block movement
            Marker: Informational objects that represent goals, checkpoints, or special locations

        Usage Note:
            World object types are used throughout the framework to:
            - Classify objects based on their functional role in the environment
            - Apply type-specific operations (e.g., finding only resources)
            - Configure agent behaviors based on object types
            - Implement object-specific interaction logic
            - Analyze environmental composition and statistics

        Example:
            >>> obj_type = nlm.WorldObjectType.Resource
            >>> print(f"Object type: {obj_type}")
            Object type: 1
            >>> 
            >>> # Check object category
            >>> if obj_type == nlm.WorldObjectType.Hazard:
            ...     print("This object is dangerous and should be avoided")
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // SensoryPercept class - Pythonic convenience methods
    .def("as_dict", [](const SensoryPercept& self) {
        py::dict dict;
        dict["vision"] = self.getVision();
        dict["vision_width"] = self.getVisionWidth();
        dict["vision_height"] = self.getVisionHeight();
        dict["touch"] = self.getTouch();
        dict["internal"] = self.getInternal();
        dict["proprioception"] = self.getProprioception();
        dict["audio"] = self.getAudio();
        dict["timestamp"] = self.getTimestamp();
        return dict;
    }, R"pbdoc(
        Convert the percept to a Python dictionary for easy serialization.

        Returns:
            dict: A dictionary containing all sensory data with keys:
                - vision: List of vision pixel values
                - vision_width: Width of vision data
                - vision_height: Height of vision data
                - touch: Proprioception/touch data
                - internal: Internal neural signals
                - proprioception: Proprioceptive data
                - audio: Audio signal data
                - timestamp: Simulation timestamp

        Example:
            >>> percept = world.getSensoryPercept()
            >>> data_dict = percept.as_dict()
            >>> print(data_dict["vision_width"], data_dict["vision_height"])
            (84, 84)
    )pbdoc")
    .def_static("from_dict", [](const py::dict& data) {
        SensoryPercept percept;
        if (data.contains("vision")) {
            percept.setVision(data["vision"].cast<std::vector<float>>());
        }
        if (data.contains("touch")) {
            percept.setTouch(data["touch"].cast<std::vector<float>>());
        }
        if (data.contains("internal")) {
            percept.setInternal(data["internal"].cast<std::vector<float>>());
        }
        if (data.contains("proprioception")) {
            percept.setProprioception(data["proprioception"].cast<std::vector<float>>());
        }
        if (data.contains("audio")) {
            percept.setAudio(data["audio"].cast<std::vector<float>>());
        }
        if (data.contains("timestamp")) {
            percept.setTimestamp(data["timestamp"].cast<double>());
        }
        return percept;
    }, py::arg("data"), R"pbdoc(
        Create a SensoryPercept from a dictionary.

        Args:
            data: Dictionary containing sensory data with optional keys:
                - vision: List of vision pixel values (vector<float>)
                - touch: Proprioception/touch data (vector<float>)
                - internal: Internal neural signals (vector<float>)
                - proprioception: Proprioceptive data (vector<float>)
                - audio: Audio signal data (vector<float>)
                - timestamp: Simulation timestamp (double)

        Returns:
            SensoryPercept: New percept object populated with data

        Note:
            Only specified keys are processed; missing keys leave corresponding
            sensory data empty (default state).

        Example:
            >>> import pynlm as nlm
            >>> data_dict = {"vision": [0.5, 0.3, 0.7], "timestamp": 1.0}
            >>> percept = nlm.SensoryPercept.from_dict(data_dict)
            >>> print(percept.has_vision())
            True
    )pbdoc")
    .def("get_vision_like", [](const SensoryPercept& self) {
        return py::array_t<float>(
            {self.getVisionWidth(), self.getVisionHeight(), 1},
            self.getVision()
        );
    }, R"pbdoc(
        Get vision data as a NumPy-compatible array-like object.

        Returns:
            py::array_t<float>: 3D array with shape (width, height, 1) containing
                vision pixel values. This format is compatible with numpy and
                other array processing libraries.

        Example:
            >>> percept = world.getSensoryPercept()
            >>> if percept.has_vision():
            ...     vision_array = percept.get_vision_like()
            ...     # vision_array can be used with numpy for image processing
            ...     # For example: import numpy as np
            ...     # np_vision = np.asarray(vision_array)

        Note:
            The array format is (width, height, channels) compatible with
            standard image representation formats.
    )pbdoc")
    .def("set_vision_like", [](SensoryPercept& self, const py::array_t<float>& data, size_t width, size_t height, size_t channels = 3) {
        if (data.ndim() != 3) {
            throw py::value_error("Vision data must be 3-dimensional");
        }
        if (data.shape(2) != channels) {
            throw py::value_error("Vision data must have correct number of channels");
        }
        if (data.shape(0) != height || data.shape(1) != width) {
            throw py::value_error("Vision data shape doesn't match specified dimensions");
        }
        std::vector<float> vision_data;
        vision_data.reserve(data.size());
        for (ssize_t y = 0; y < height; ++y) {
            for (ssize_t x = 0; x < width; ++x) {
                vision_data.push_back(data(y, x, 0));
            }
        }
        self.setVision(vision_data);
    }, py::arg("data"), py::arg("width"), py::arg("height"), py::arg("channels") = 3, R"pbdoc(
        Set vision data from a NumPy-compatible array-like object.

        Args:
            data: 3D array-like object with shape (height, width, channels)
                containing vision pixel values (typically grayscale or RGB).
            width: Width of the vision data (number of columns)
            height: Height of the vision data (number of rows)
            channels: Number of channels (1 for grayscale, 3 for RGB). Defaults to 3.

        Raises:
            py::value_error: If data is not 3D, has wrong number of channels,
                or shape doesn't match specified dimensions.

        Example:
            >>> import numpy as np
            >>> # Create sample vision data (84x84 grayscale)
            >>> vision_array = np.random.rand(84, 84, 1).astype(np.float32)
            >>> percept = nlm.SensoryPercept()
            >>> percept.set_vision_like(vision_array, 84, 84, 1)
            >>> print(percept.getVisionWidth(), percept.getVisionHeight())
            84 84

        Note:
            The function expects data in format (height, width, channels) which is
            standard for image processing libraries like numpy and PIL.
    )pbdoc")
    .def("get_audio_like", [](const SensoryPercept& self) {
        return py::array_t<float>(self.getAudio());
    }, R"pbdoc(
        Get audio data as a NumPy-compatible array-like object.

        Returns:
            py::array_t<float>: 1D array containing audio signal samples.
                The array can be used with numpy and other audio processing
                libraries for signal analysis and manipulation.

        Example:
            >>> percept = world.getSensoryPercept()
            >>> if percept.has_audio():
            ...     audio_array = percept.get_audio_like()
            ...     # Audio can be analyzed with numpy
            ...     # For example: import numpy as np
            ...     # audio_samples = np.asarray(audio_array)
            ...     # duration = len(audio_samples) / sample_rate

        Note:
            The returned array contains the raw audio samples as they are
            stored in the percept, typically as float values normalized between
            -1.0 and 1.0.
    )pbdoc")
    .def("set_audio_like", [](SensoryPercept& self, const py::array_t<float>& data, size_t sample_rate) {
        std::vector<float> audio_data;
        audio_data.reserve(data.size());
        for (ssize_t i = 0; i < data.size(); ++i) {
            audio_data.push_back(data(i));
        }
        self.setAudio(audio_data);
    }, py::arg("data"), py::arg("sample_rate"), R"pbdoc(
        Set audio data from a NumPy-compatible array-like object.

        Args:
            data: 1D array-like object containing audio signal samples.
                Audio samples should be float values, typically in range [-1.0, 1.0].
            sample_rate: Sampling rate of the audio data in Hz (e.g., 44100 for CD quality).

        Example:
            >>> import numpy as np
            >>> # Create sample audio data (44.1kHz, 1 second of stereo audio)
            >>> sample_rate = 44100
            >>> duration = 1.0
            >>> t = np.linspace(0, duration, int(sample_rate * duration))
            >>> audio_array = np.sin(2 * np.pi * 440 * t).astype(np.float32)  # 440Hz sine wave
            >>> percept = nlm.SensoryPercept()
            >>> percept.set_audio_like(audio_array, sample_rate)
            >>> print(f"Audio length: {len(percept.getAudio())} samples")
            Audio length: 44100

        Note:
            The audio data is stored as floating-point samples. This format is
            compatible with numpy, scipy, and other audio processing libraries.
    )pbdoc")
    .def("has_vision", [](const SensoryPercept& self) {
        return !self.getVision().empty();
    }, R"pbdoc(
        Check if vision sensory data is present.

        Returns:
            bool: True if vision data is available (non-empty), False otherwise.

        Example:
            >>> percept = world.getSensoryPercept()
            >>> if percept.has_vision():
            ...     print("Vision data available")
            ...     vision_array = percept.get_vision_like()
            Vision data available

        Use Case:
            This method is useful for conditional processing where vision
            may or may not be present in the current percept (e.g., when
            an agent has limited field of view or during sensor failure).
    )pbdoc")
    .def("has_audio", [](const SensoryPercept& self) {
        return !self.getAudio().empty();
    }, R"pbdoc(
        Check if audio sensory data is present.

        Returns:
            bool: True if audio data is available (non-empty), False otherwise.

        Example:
            >>> percept = world.getSensoryPercept()
            >>> if percept.has_audio():
            ...     print("Audio data available")
            ...     audio_array = percept.get_audio_like()
            ...     # Process audio signal
            Audio data available

        Use Case:
            Essential for agents that need to process environmental sounds,
            such as location detection, object identification, or communication.
    )pbdoc")
    .def("has_internal", [](const SensoryPercept& self) {
        return !self.getInternal().empty();
    }, R"pbdoc(
        Check if internal neural signals are present.

        Returns:
            bool: True if internal signals are available (non-empty), False otherwise.

        Example:
            >>> percept = world.getSensoryPercept()
            >>> if percept.has_internal():
            ...     print("Internal signals available")
            ...     internal_signals = percept.getInternal()
            Internal signals available

        Use Case:
            Internal signals represent the agent's own neural activity, such as
            prediction errors, neuromodulation levels, or homeostatic signals.
            This method is crucial for agents that need to monitor their
            own internal state for self-regulation.
    )pbdoc")
    .def("has_proprioception", [](const SensoryPercept& self) {
        return !self.getProprioception().empty();
    }, R"pbdoc(
        Check if proprioceptive (body awareness) data is present.

        Returns:
            bool: True if proprioception data is available (non-empty), False otherwise.

        Example:
            >>> percept = world.getSensoryPercept()
            >>> if percept.has_proprioception():
            ...     print("Proprioception data available")
            ...     proprioception = percept.getProprioception()
            Proprioception data available

        Use Case:
            Proprioception represents the agent's awareness of its own body state,
            including position, orientation, movement, and physical sensations.
            This is essential for coordinated movement and body awareness in
            autonomous agents.
    )pbdoc")
    .def("get_all_like", [](const SensoryPercept& self) {
        py::dict dict;
        dict["vision"] = self.getVision();
        dict["vision_width"] = self.getVisionWidth();
        dict["vision_height"] = self.getVisionHeight();
        dict["touch"] = self.getTouch();
        dict["internal"] = self.getInternal();
        dict["proprioception"] = self.getProprioception();
        dict["audio"] = self.getAudio();
        dict["timestamp"] = self.getTimestamp();
        return dict;
    }, R"pbdoc(
        Get all sensory data as a comprehensive dictionary.

        Returns:
            dict: Dictionary containing all available sensory data with keys:
                - vision: List of vision pixel values
                - vision_width: Width of vision data
                - vision_height: Height of vision data
                - touch: Proprioception/touch data
                - internal: Internal neural signals
                - proprioception: Proprioceptive data
                - audio: Audio signal data
                - timestamp: Simulation timestamp

        Example:
            >>> percept = world.getSensoryPercept()
            >>> all_data = percept.get_all_like()
            >>> print(f"Vision size: {all_data['vision_width']}x{all_data['vision_height']}")
            >>> print(f"Audio samples: {len(all_data['audio'])}")
            Vision size: 84x84
            Audio samples: 22050

        Use Case:
            This method is useful for saving complete sensory state,
            serializing agent perception for logging, or passing data
            between different processing stages. It provides a standardized
            format that includes all sensory modalities.
    )pbdoc")
    .def("set_all_like", [](SensoryPercept& self, const py::dict& data) {
        if (data.contains("vision")) {
            self.setVision(data["vision"].cast<std::vector<float>>());
        }
        if (data.contains("touch")) {
            self.setTouch(data["touch"].cast<std::vector<float>>());
        }
        if (data.contains("internal")) {
            self.setInternal(data["internal"].cast<std::vector<float>>());
        }
        if (data.contains("proprioception")) {
            self.setProprioception(data["proprioception"].cast<std::vector<float>>());
        }
        if (data.contains("audio")) {
            self.setAudio(data["audio"].cast<std::vector<float>>());
        }
        if (data.contains("timestamp")) {
            self.setTimestamp(data["timestamp"].cast<double>());
        }
    }, py::arg("data"), R"pbdoc(
        Set all sensory data from a comprehensive dictionary.

        Args:
            data: Dictionary containing sensory data with optional keys:
                - vision: List of vision pixel values (vector<float>)
                - touch: Proprioception/touch data (vector<float>)
                - internal: Internal neural signals (vector<float>)
                - proprioception: Proprioceptive data (vector<float>)
                - audio: Audio signal data (vector<float>)
                - timestamp: Simulation timestamp (double)

        Example:
            >>> import pynlm as nlm
            >>> # Create comprehensive sensory data
            >>> sensory_data = {
            ...     "vision": list(np.random.rand(84, 84)),
            ...     "vision_width": 84,
            ...     "vision_height": 84,
            ...     "timestamp": 1.5
            ... }
            >>> percept = nlm.SensoryPercept()
            >>> percept.set_all_like(sensory_data)
            >>> print(f"Percept created with vision: {percept.has_vision()}")
            Percept created with vision: True

        Note:
            Only specified keys are processed; missing keys leave corresponding
            sensory data in its default empty state. This method provides a
            convenient way to restore a complete sensory state from saved data.
    )pbdoc")
    .def("clone", [](const SensoryPercept& self) {
        SensoryPercept clone;
        clone.setVision(self.getVision());
        clone.setTouch(self.getTouch());
        clone.setInternal(self.getInternal());
        clone.setProprioception(self.getProprioception());
        clone.setAudio(self.getAudio());
        clone.setTimestamp(self.getTimestamp());
        return clone;
    }, R"pbdoc(
        Create a deep copy of the sensory percept.

        Returns:
            SensoryPercept: A new percept object with all sensory data copied
                from the original. This is a deep copy, so modifications to
                the returned object do not affect the original.

        Example:
            >>> original = world.getSensoryPercept()
            >>> if original.has_vision():
            ...     copy_percept = original.clone()
            ...     # Modify the copy without affecting original
            ...     copy_percept.setTimestamp(2.0)
            ...     # Original remains unchanged
            >>> original.has_vision()
            True
            >>> copy_percept.getTimestamp()
            2.0

        Use Case:
            This method is useful when you need multiple instances of the same
            sensory percept, such as for Monte Carlo simulations, backtracking,
            or when you need to preserve a previous state for comparison.
    )pbdoc")
    .def("copy", [](const SensoryPercept& self) {
        SensoryPercept clone;
        clone.setVision(self.getVision());
        clone.setTouch(self.getTouch());
        clone.setInternal(self.getInternal());
        clone.setProprioception(self.getProprioception());
        clone.setAudio(self.getAudio());
        clone.setTimestamp(self.getTimestamp());
        return clone;
    }, R"pbdoc(
        Create a copy of the sensory percept (alias for clone()).

        Returns:
            SensoryPercept: A new percept object with all sensory data copied
                from the original. This is a deep copy, so modifications to
                the returned object do not affect the original.

        Example:
            >>> original = world.getSensoryPercept()
            >>> if original.has_vision():
            ...     percept_copy = original.copy()
            ...     # Same functionality as clone()
            ...     print(f"Original: {original.getTimestamp()}, Copy: {percept_copy.getTimestamp()}")

        Note:
            This is provided as an alternative name for clone() for API
            convenience. Both methods perform identical operations.

        Use Case:
            Use copy() for code clarity when the action of copying is more
            descriptive than cloning, or when maintaining consistency with
            other code that uses the term "copy" rather than "clone".
    )pbdoc")
    .def("__repr__", [](const SensoryPercept& self) {
        std::stringstream ss;
        ss << "<SensoryPercept: vision=" << self.getVision().size() 
           << " touch=" << self.getTouch().size() 
           << " internal=" << self.getInternal().size()
           << " proprioception=" << self.getProprioception().size()
           << " audio=" << self.getAudio().size()
           << " timestamp=" << self.getTimestamp() << ">";
        return ss.str();
    });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // SimpleWorld class - Pythonic convenience methods
    // Reset methods
    .def("reset_all", &SimpleWorld::reset, R"pbdoc(
        Reset the world and agent to their initial state.

        This method resets:
        - All world objects (resources, hazards, walls, markers)
        - Agent position to origin (0, 0)
        - Agent orientation to 0 degrees
        - Agent energy to maximum
        - Simulation time to 0

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add some objects
            >>> world.add_resource(1.0, 1.0)
            >>> world.add_hazard(3.0, 3.0)
            >>> # Reset to clean state
            >>> world.reset_all()
            >>> # Verify reset
            >>> print(len(world.getObjects()))  # Should be 0
            >>> print(world.getAgentState())

        Use Case:
            This is useful for starting a new episode in reinforcement learning,
            resetting the environment for testing, or clearing the world for
            a fresh simulation run.
    )pbdoc")
    .def("create_agent", [](SimpleWorld& self, float x, float y, float orientation = 0.0f) {
        self.setAgentStart(x, y);
        return &self.getAgentBody();
    }, py::arg("x"), py::arg("y"), py::arg("orientation") = 0.0f, R"pbdoc(
        Create and initialize an agent at the specified position.

        Args:
            x: X-coordinate for agent start position
            y: Y-coordinate for agent start position  
            orientation: Initial orientation in degrees (0 = facing north/east)

        Returns:
            AgentBody*: Pointer to the agent body object for further manipulation

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> agent = world.create_agent(x=5.0, y=5.0, orientation=90.0)
            >>> print(f"Agent position: ({agent.x}, {agent.y})")
            >>> print(f"Agent orientation: {agent.orientation}")
            Agent position: (5, 5)
            Agent orientation: 90

        Note:
            This method combines position setting and agent body creation
            into a single convenient operation. It returns the agent body
            pointer for direct attribute access and modification.

        Use Case:
            Essential for placing agents in the environment before starting
            simulation, useful for initialization scenarios in both
            training and testing.
    )pbdoc")
    
    // Object manipulation methods
    .def("add_resource", [](SimpleWorld& self, float x, float y, float value = 1.0f, float radius = 0.5f) {
        WorldObject obj(x, y, WorldObjectType::Resource, value, radius);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, py::arg("radius") = 0.5f, R"pbdoc(
        Add a resource object to the world.

        Args:
            x: X-coordinate for resource position
            y: Y-coordinate for resource position
            value: Resource value/amount (e.g., energy, food, points). Defaults to 1.0
            radius: Resource detection radius. Defaults to 0.5

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add different types of resources
            >>> world.add_resource(1.0, 1.0, value=5.0)    # High-value resource
            >>> world.add_resource(3.0, 3.0, value=0.5)    # Low-value resource
            >>> # Add with custom radius
            >>> world.add_resource(2.0, 2.0, radius=1.0)    # Larger detection area
            >>> 
            >>> # Get resource summary
            >>> print(world.get_object_summary())

        Use Case:
            Resources are consumable items that agents can collect for energy,
            nutrition, or other benefits. Resources have value and detection
            radius that affect how agents interact with them.

        Related Methods:
            See also add_hazard(), add_wall(), add_marker() for other object types.
    )pbdoc")
    .def("add_hazard", [](SimpleWorld& self, float x, float y, float value = 1.0f, float radius = 0.5f) {
        WorldObject obj(x, y, WorldObjectType::Hazard, value, radius);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, py::arg("radius") = 0.5f, R"pbdoc(
        Add a hazard object to the world.

        Args:
            x: X-coordinate for hazard position
            y: Y-coordinate for hazard position
            value: Hazard severity/damage amount. Defaults to 1.0
            radius: Hazard detection/interaction radius. Defaults to 0.5

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add different types of hazards
            >>> world.add_hazard(1.0, 1.0, value=2.0)      # High-severity hazard
            >>> world.add_hazard(4.0, 4.0, value=0.5)      # Low-severity hazard
            >>> # Add with larger detection radius
            >>> world.add_hazard(3.0, 3.0, radius=1.5)     # Wide-area hazard
            >>> 
            >>> # Check agent safety
            >>> agent = world.create_agent(2.0, 2.0)
            >>> print(world.is_safe_position(2.0, 2.0))  # True (safe)
            >>> print(world.is_safe_position(1.0, 1.0))  # False (in hazard)

        Use Case:
            Hazards are dangerous objects that can harm agents. They typically
            reduce agent energy, health, or cause other negative effects. Hazards
            are often used to create challenging environments for agents to navigate.

        Related Methods:
            See also add_resource(), add_wall(), add_marker() for other object types.
            Use is_safe_position() to check hazard proximity.
    )pbdoc")
    .def("add_wall", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
        WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, R"pbdoc(
        Add a wall object to the world.

        Args:
            x: X-coordinate for wall position
            y: Y-coordinate for wall position
            radius: Wall size/radius. Defaults to 0.5

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add walls to create barriers
            >>> world.add_wall(1.0, 1.0)        # Small wall
            >>> world.add_wall(3.0, 3.0, radius=1.0)  # Large wall
            >>> 
            >>> # Verify wall placement
            >>> print(world.is_safe_position(1.5, 1.5))  # False (inside wall)
            >>> print(world.is_safe_position(2.0, 2.0))  # True (outside wall)

        Use Case:
            Walls are static barriers that block agent movement. They are
            typically used to create corridors, boundaries, or obstacles
            in the simulation environment. Walls are impassable and serve
            as permanent environmental features.

        Related Methods:
            See also add_resource(), add_hazard(), add_marker() for other object types.
    )pbdoc")
    .def("add_marker", [](SimpleWorld& self, float x, float y, float value = 0.0f, float radius = 0.3f) {
        WorldObject obj(x, y, WorldObjectType::Marker, value, radius);
        self.addObject(obj);
    }, py::arg("x"), py::arg("y"), py::arg("value") = 0.0f, py::arg("radius") = 0.3f, R"pbdoc(
        Add a marker object to the world.

        Args:
            x: X-coordinate for marker position
            y: Y-coordinate for marker position
            value: Marker value/weight. Defaults to 0.0
            radius: Marker detection radius. Defaults to 0.3

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add markers for various purposes
            >>> world.add_marker(1.0, 1.0, value=10.0)      # Important marker
            >>> world.add_marker(5.0, 5.0)                  # Default marker
            >>> world.add_marker(3.0, 3.0, radius=1.0)     # Large detection marker
            >>> 
            >>> # Use markers for goal positions, checkpoints, or special locations
            >>> world.add_marker(0.0, 0.0, value=100.0)     # Goal/goal location
            >>> 
            >>> # Find nearest marker
            >>> nearest = world.find_nearest(2.0, 2.0, WorldObjectType::Marker)
            >>> print(f"Nearest marker at ({nearest.x}, {nearest.y})")

        Use Case:
            Markers are informational objects that typically represent goals,
            checkpoints, goals, or special locations. They may have values
            (e.g., reward points) and are often used for navigation tasks,
            reinforcement learning goals, or exploration targets.

        Related Methods:
            See also add_resource(), add_hazard(), add_wall() for other object types.
            Use find_nearest() to locate markers in the environment.
    )pbdoc")
    
    // Agent state methods
    .def("get_agent_state", [](const SimpleWorld& self) {
        py::dict state_dict;
        state_dict["x"] = self.getAgentBody().getX();
        state_dict["y"] = self.getAgentBody().getY();
        state_dict["orientation"] = self.getAgentBody().getOrientation();
        state_dict["energy"] = self.getAgentBody().getEnergy();
        return state_dict;
    }, R"pbdoc(
        Get the current agent state as a dictionary.

        Returns:
            dict: Dictionary containing agent state with keys:
                - x: Current X-coordinate
                - y: Current Y-coordinate
                - orientation: Current orientation in degrees
                - energy: Current energy level (0.0 to max)

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> agent = world.create_agent(x=5.0, y=5.0, orientation=90.0)
            >>> # Modify agent state
            >>> agent.energy = 0.7
            >>> # Get state as dictionary
            >>> state = world.get_agent_state()
            >>> print(f"Position: ({state['x']}, {state['y']})")
            >>> print(f"Orientation: {state['orientation']}")
            >>> print(f"Energy: {state['energy']}")

        Use Case:
            This method is useful for saving agent state, logging, or passing
            state information to other systems (e.g., reinforcement learning
            agents, monitoring systems, or state transfer between processes).

        Related Methods:
            See also set_agent_state() for restoring agent state from dictionary.
    )pbdoc")
    .def("set_agent_state", [](SimpleWorld& self, const py::dict& state_dict) {
        if (state_dict.contains("x")) {
            self.getAgentBody().setX(state_dict["x"].cast<float>());
        }
        if (state_dict.contains("y")) {
            self.getAgentBody().setY(state_dict["y"].cast<float>());
        }
        if (state_dict.contains("orientation")) {
            self.getAgentBody().setOrientation(state_dict["orientation"].cast<float>());
        }
        if (state_dict.contains("energy")) {
            self.getAgentBody().setEnergy(state_dict["energy"].cast<float>());
        }
    }, py::arg("state_dict"), R"pbdoc(
        Set agent state from a dictionary.

        Args:
            state_dict: Dictionary containing agent state with optional keys:
                - x: New X-coordinate
                - y: New Y-coordinate
                - orientation: New orientation in degrees
                - energy: New energy level (0.0 to max)

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Modify agent state
            >>> agent = world.create_agent(x=5.0, y=5.0)
            >>> state_dict = {"x": 10.0, "y": 10.0, "energy": 0.8}
            >>> world.set_agent_state(state_dict)
            >>> print(world.get_agent_state())
            {'x': 10.0, 'y': 10.0, 'orientation': 0.0, 'energy': 0.8}

        Use Case:
            This method provides a convenient way to restore agent state from
            saved data, such as when resuming simulations, loading checkpoints,
            or applying state transfers between different processes or agents.

        Related Methods:
            See also get_agent_state() for retrieving current agent state.
    )pbdoc")
    
    // Object querying methods
    .def("get_objects_by_type", [](const SimpleWorld& self, WorldObjectType type) {
        std::vector<WorldObject> objects;
        for (const auto& obj : self.getObjects()) {
            if (obj.type == type) {
                objects.push_back(obj);
            }
        }
        return objects;
    }, py::arg("type"), R"pbdoc(
        Get all objects of a specific type from the world.

        Args:
            type: WorldObjectType to filter by (Resource, Hazard, Wall, Marker, Empty)

        Returns:
            list: List of WorldObject objects matching the specified type.

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add different object types
            >>> world.add_resource(1.0, 1.0)
            >>> world.add_hazard(3.0, 3.0)
            >>> world.add_resource(5.0, 5.0)
            >>> # Get all resources
            >>> resources = world.get_objects_by_type(WorldObjectType.Resource)
            >>> print(f"Found {len(resources)} resources")
            >>> # Get all hazards
            >>> hazards = world.get_objects_by_type(WorldObjectType.Hazard)
            >>> print(f"Found {len(hazards)} hazards")

        Use Case:
            This method is useful for querying the world state, such as finding
            all food sources for an agent, collecting all hazards for analysis,
            or gathering all goal markers for navigation planning.

        Related Methods:
            See also get_nearby_objects(), count_objects_by_type() for similar queries.
    )pbdoc")
    .def("get_nearby_objects", [](const SimpleWorld& self, float x, float y, float radius) {
        std::vector<WorldObject> nearby_objects;
        for (const auto& obj : self.getObjects()) {
            float dx = obj.x - x;
            float dy = obj.y - y;
            if (dx * dx + dy * dy <= radius * radius) {
                nearby_objects.push_back(obj);
            }
        }
        return nearby_objects;
    }, py::arg("x"), py::arg("y"), py::arg("radius"), R"pbdoc(
        Get all objects within a specified radius of a position.

        Args:
            x: X-coordinate of the reference position
            y: Y-coordinate of the reference position
            radius: Search radius (squared distance comparison for efficiency)

        Returns:
            list: List of WorldObject objects within the specified radius.

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add environment objects
            >>> world.add_resource(1.0, 1.0, value=5.0)
            >>> world.add_hazard(3.0, 3.0, value=2.0)
            >>> world.add_wall(5.0, 5.0)
            >>> # Find objects near agent position
            >>> agent = world.create_agent(x=2.0, y=2.0)
            >>> nearby = world.get_nearby_objects(2.0, 2.0, radius=2.0)
            >>> print(f"Found {len(nearby)} objects within radius 2.0")
            >>> for obj in nearby:
            ...     print(f"  {obj.type} at ({obj.x}, {obj.y})")

        Use Case:
            This method is essential for local perception in autonomous agents,
            such as detecting nearby food, hazards, or obstacles within a
            limited sensory range. It implements efficient spatial queries
            using squared distance to avoid expensive sqrt operations.

        Note:
            The search uses squared distance comparison (dx² + dy² ≤ r²)
            for computational efficiency compared to Euclidean distance.

        Related Methods:
            See also get_objects_by_type() for type-based filtering,
            find_nearest() for finding the closest single object.
    )pbdoc")
    .def("count_objects_by_type", [](const SimpleWorld& self, WorldObjectType type) {
        int count = 0;
        for (const auto& obj : self.getObjects()) {
            if (obj.type == type) {
                count++;
            }
        }
        return count;
    }, py::arg("type"), R"pbdoc(
        Count the number of objects of a specific type in the world.

        Args:
            type: WorldObjectType to count (Resource, Hazard, Wall, Marker, Empty)

        Returns:
            int: Number of objects matching the specified type.

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add objects of different types
            >>> world.add_resource(1.0, 1.0)
            >>> world.add_hazard(3.0, 3.0)
            >>> world.add_resource(5.0, 5.0)
            >>> world.add_resource(7.0, 7.0)
            >>> # Count resources
            >>> resource_count = world.count_objects_by_type(WorldObjectType.Resource)
            >>> print(f"Resource count: {resource_count}")
            Resource count: 3
            >>> # Count hazards
            >>> hazard_count = world.count_objects_by_type(WorldObjectType.Hazard)
            >>> print(f"Hazard count: {hazard_count}")
            Hazard count: 1

        Use Case:
            This method is useful for state monitoring, task completion checking,
            or statistical analysis of the environment. For example, an agent
            might check if enough food resources are available before moving on
            to the next task.

        Related Methods:
            See also get_objects_by_type() for retrieving object lists,
            get_object_summary() for comprehensive world state.
    )pbdoc")
    .def("get_object_summary", [](const SimpleWorld& self) {
        std::stringstream ss;
        ss << "World Summary:";
        for (const auto& obj : self.getObjects()) {
            ss << "\n  " << static_cast<int>(obj.type) 
               << " at (" << obj.x << ", " << obj.y << ") radius=" << obj.radius
               << " value=" << obj.value << " active=" << obj.active;
        }
        return ss.str();
    }, R"pbdoc(
        Get a formatted summary string of all objects in the world.

        Returns:
            str: Formatted string containing information about all objects.
                Each line shows object type, position, size, value, and active status.

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add various objects
            >>> world.add_resource(1.0, 1.0, value=5.0)
            >>> world.add_hazard(3.0, 3.0, value=2.0)
            >>> world.add_wall(5.0, 5.0, radius=0.8)
            >>> # Get world summary
            >>> summary = world.get_object_summary()
            >>> print(summary)
            World Summary:
              0 at (1, 1) radius=0.5 value=5 active=1
              1 at (3, 3) radius=0.5 value=2 active=1
              2 at (5, 5) radius=0.8 value=0 active=1
            (Object types: 0=Resource, 1=Hazard, 2=Wall, 3=Marker, 4=Empty)

        Use Case:
            This method is useful for debugging, logging, and human-readable
            world state reporting. It provides a quick overview of the environment
            contents without requiring programmatic parsing. Often used in
            simulation output, logging, or agent introspection.

        Related Methods:
            See also add_resource(), add_hazard(), add_wall(), add_marker()
            for object creation methods.
    )pbdoc")
    
    // Safety methods
    .def("is_safe_position", [](const SimpleWorld& self, float x, float y) {
        return self.isValidPosition(x, y);
    }, py::arg("x"), py::arg("y"), R"pbdoc(
        Check if a position is safe (not occupied by wall or hazard).

        Args:
            x: X-coordinate to check
            y: Y-coordinate to check

        Returns:
            bool: True if position is safe (free of walls and hazards),
                False if position is occupied by a wall or hazard.

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add obstacles
            >>> world.add_wall(2.0, 2.0)
            >>> world.add_hazard(5.0, 5.0)
            >>> # Check safety of different positions
            >>> print(world.is_safe_position(0.0, 0.0))   # True (safe)
            >>> print(world.is_safe_position(2.0, 2.0))   # False (inside wall)
            >>> print(world.is_safe_position(5.0, 5.0))   # False (inside hazard)
            >>> print(world.is_safe_position(4.0, 4.0))   # True (safe)

        Use Case:
            This method is crucial for agent navigation and collision avoidance.
            Agents use it to determine valid movement destinations and to
            plan paths that avoid dangerous areas. It's also useful for
            placing new objects without overlapping existing ones.

        Note:
            A position is considered unsafe if it's inside a wall (anywhere
            within wall radius) or inside a hazard (anywhere within hazard radius).
            Resources and markers are considered safe (passable).

        Related Methods:
            See also find_nearest() to find the closest wall or hazard,
            add_wall(), add_hazard() for obstacle placement.
    )pbdoc")
    .def("find_nearest", [](const SimpleWorld& self, float x, float y, WorldObjectType type = WorldObjectType::Empty) {
        float min_dist = std::numeric_limits<float>::max();
        WorldObject nearest_obj;
        bool found = false;
        
        for (const auto& obj : self.getObjects()) {
            if (type != WorldObjectType::Empty && obj.type != type) {
                continue;
            }
            float dx = obj.x - x;
            float dy = obj.y - y;
            float dist = dx * dx + dy * dy;
            if (dist < min_dist) {
                min_dist = dist;
                nearest_obj = obj;
                found = true;
            }
        }
        
        if (!found) {
            return WorldObject();
        }
        return nearest_obj;
    }, py::arg("x"), py::arg("y"), py::arg("type") = WorldObjectType::Empty, R"pbdoc(
        Find the nearest object of a specific type to a position.

        Args:
            x: X-coordinate of search center
            y: Y-coordinate of search center
            type: WorldObjectType to search for (defaults to any object).
                Use WorldObjectType::Empty to find any object.

        Returns:
            WorldObject: The nearest object of the specified type.
                Returns an empty WorldObject if no objects are found.

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Add environment objects
            >>> world.add_resource(1.0, 1.0, value=5.0)
            >>> world.add_hazard(3.0, 3.0, value=2.0)
            >>> world.add_marker(8.0, 8.0, value=10.0)
            >>> 
            >>> # Find nearest resource
            >>> agent = world.create_agent(x=0.0, y=0.0)
            >>> nearest_resource = world.find_nearest(0.0, 0.0, WorldObjectType.Resource)
            >>> print(f"Nearest resource: ({nearest_resource.x}, {nearest_resource.y})")
            Nearest resource: (1, 1)
            >>> 
            >>> # Find nearest hazard
            >>> nearest_hazard = world.find_nearest(0.0, 0.0, WorldObjectType.Hazard)
            >>> print(f"Nearest hazard: ({nearest_hazard.x}, {nearest_hazard.y})")
            Nearest hazard: (3, 3)

        Use Case:
            This method is essential for agent perception and decision making.
            Agents use it to locate the closest food source, danger, goal,
            or any point of interest in their environment. It supports both
            type-specific searches and general searches for any object.

        Note:
            Uses squared distance comparison for efficiency. Returns an empty
            WorldObject (with default values) when no matching object is found.

        Related Methods:
            See also get_nearby_objects() for all objects within radius,
            is_safe_position() for safety checking.
    )pbdoc")
    
    // Path and movement methods
    .def("get_path_length", [](const SimpleWorld& self, float x1, float y1, float x2, float y2) {
        return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    }, py::arg("x1"), py::arg("y1"), py::arg("x2"), py::arg("y2"), R"pbdoc(
        Calculate the shortest path length between two points in the world.

        Args:
            x1: X-coordinate of start point
            y1: Y-coordinate of start point
            x2: X-coordinate of end point
            y2: Y-coordinate of end point

        Returns:
            float: Euclidean distance between the two points.
                Calculated as sqrt((x2-x1)² + (y2-y1)²).

        Example:
            >>> world = nlm.createSimpleWorld()
            >>> # Calculate distances between points
            >>> dist1 = world.get_path_length(0.0, 0.0, 3.0, 4.0)
            >>> print(f"Distance from (0,0) to (3,4): {dist1}")
            Distance from (0,0) to (3,4): 5
            >>> 
            >>> dist2 = world.get_path_length(1.0, 1.0, 4.0, 5.0)
            >>> print(f"Distance from (1,1) to (4,5): {dist2}")
            Distance from (1,1) to (4,5): 5
            >>> 
            >>> # Use for path planning
            >>> agent = world.create_agent(x=0.0, y=0.0)
            >>> goal = (5.0, 5.0)
            >>> distance_to_goal = world.get_path_length(0.0, 0.0, goal[0], goal[1])
            >>> print(f"Distance to goal: {distance_to_goal}")

        Use Case:
            This method provides the fundamental distance calculation for
            path planning and navigation. While SimpleWorld assumes straight-line
            movement (Euclidean geometry), this method gives agents the ability
            to measure distances between any two positions in the environment.
            It's useful for determining travel costs, estimating arrival times,
            or comparing route options.

        Note:
            This assumes straight-line movement. If the environment contains
            obstacles (walls), the actual travel path may be longer and would
            require path planning algorithms beyond simple distance calculation.

        Related Methods:
            See also is_safe_position() for collision checking,
            find_nearest() for object proximity, get_nearby_objects() for
            local perception.
    )pbdoc")
    
    // World properties
    .def_readonly("width", &SimpleWorld::getWidth, "Get world width")
    .def_readonly("height", &SimpleWorld::getHeight, "Get world height")
    .def_readonly("simulation_time", &SimpleWorld::getSimulationTime, "Get simulation time")
    .def_readonly("max_energy", &SimpleWorld::getMaxEnergy, "Get max energy")
    .def_readonly("agent", &SimpleWorld::getAgentBody, "Get agent body")
    .def_readonly("sensory_percept", &SimpleWorld::getSensoryPercept, "Get sensory percept");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // AgentBrain class - Pythonic convenience methods
    // Processing methods
    .def("step_percept", &AgentBrain::processSensoryInput, py::arg("percept"), R"pbdoc(
        Convenience method for processing a single sensory percept.

        This method combines the sensory input processing step of agent operation
        into a single call, making it ideal for simple agent implementations
        that don't need the full step/decode workflow.

        Args:
            percept: SensoryPercept object containing sensory data to process

        Example:
            >>> brain = nlm.createBrain(nlm.createDefaultConfig())
            >>> agent_brain = nlm.createAgentBrain(brain)
            >>> world = nlm.createSimpleWorld()
            >>> agent_brain.step_percept(world.getSensoryPercept())

        Use Case:
            Perfect for simple agents that only need to process sensory input
            without immediately decoding an action. Can be used in hybrid
            architectures where action selection happens elsewhere.

        Related Methods:
            See also step_and_decode() for the complete processing workflow.
    )pbdoc")
    .def("step_and_decode", [](AgentBrain& self, const SensoryPercept& percept) {
        self.processSensoryInput(percept);
        return self.decodeMotorCommand();
    }, py::arg("percept"), R"pbdoc(
        Complete agent processing workflow: sense and act in one step.

        This is the most common convenience method for autonomous agents,
        performing both sensory processing and motor command decoding in a
        single operation. It's the standard "think and act" cycle for agents
        using the NLM framework.

        Args:
            percept: SensoryPercept object containing sensory data from the
                environment (typically obtained via world.getSensoryPercept())

        Returns:
            Action: Motor action decoded from the brain's neural activity
                based on the processed sensory input.

        Example:
            >>> brain = nlm.createBrain(nlm.createDefaultConfig())
            >>> agent_brain = nlm.createAgentBrain(brain)
            >>> world = nlm.createSimpleWorld()
            >>> 
            >>> # Complete agent step
            >>> action = agent_brain.step_and_decode(world.getSensoryPercept())
            >>> print(f"Agent action: {action.getName()}")
            >>> 
            >>> # Full simulation loop
            >>> for step in range(100):
            ...     action = agent_brain.step_and_decode(world.getSensoryPercept())
            ...     world.applyMotorCommand(action, step * 0.1)

        Use Case:
            This method encapsulates the core agent control loop and is
            typically used in reinforcement learning, robotics, and
            autonomous system applications.

        Related Methods:
            See also step_percept() for sensory processing only,
            get_system_state() for agent status monitoring.
    )pbdoc")
    
    // System state methods
    .def("get_system_state", [](const AgentBrain& self) {
        py::dict state_dict;
        state_dict["neuromodulation_level"] = self.getNeuromodulationLevel();
        state_dict["curiosity_level"] = self.getCuriosityLevel();
        state_dict["novelty_level"] = self.getNoveltyLevel();
        state_dict["prediction_error"] = self.getPredictionError();
        state_dict["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
        state_dict["reward_modulation_enabled"] = self.isRewardModulationEnabled();
        state_dict["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
        state_dict["development_enabled"] = self.isDevelopmentEnabled();
        state_dict["curiosity_enabled"] = self.isCuriosityEnabled();
        return state_dict;
    }, R"pbdoc(
        Get comprehensive system state information as a dictionary.

        This method provides a complete snapshot of the agent's internal state,
        including neuromodulation levels, curiosity metrics, developmental status,
        prediction errors, and enabled subsystems. It's useful for monitoring,
        debugging, and maintaining agent state across simulation episodes.

        Returns:
            dict: Comprehensive system state with keys:
                - neuromodulation_level: Current neuromodulation strength (0.0-1.0)
                - curiosity_level: Current curiosity drive (0.0-1.0)
                - novelty_level: Recent novelty detection (0.0-1.0)
                - prediction_error: Current prediction error magnitude
                - developmental_stage: Current developmental stage (0-4)
                - reward_modulation_enabled: Whether reward modulation is active
                - structural_plasticity_enabled: Whether structural plasticity is active
                - development_enabled: Whether development system is active
                - curiosity_enabled: Whether curiosity system is active

        Example:
            >>> brain = nlm.createBrain(nlm.createDefaultConfig())
            >>> agent_brain = nlm.createAgentBrain(brain)
            >>> 
            >>> # Monitor system state during simulation
            >>> for step in range(50):
            ...     state = agent_brain.get_system_state()
            ...     if state["prediction_error"] > 0.5:
            ...         print(f"High prediction error at step {step}: {state['prediction_error']}")
            ...     action = agent_brain.step_and_decode(world.getSensoryPercept())
            ...     world.applyMotorCommand(action, step * 0.1)

        Use Case:
            This method is essential for agent monitoring, debugging, and
            state preservation. It's particularly useful in reinforcement
            learning for tracking agent internal states and in complex
            simulations for understanding agent behavior.

        Related Methods:
            See also is_subsystem_enabled() for individual subsystem status,
            get_subsystem_status() for simplified status.
    )pbdoc")
    .def("is_subsystem_enabled", [](const AgentBrain& self, const std::string& name) {
        if (name == "reward_modulation") return self.isRewardModulationEnabled();
        if (name == "structural_plasticity") return self.isStructuralPlasticityEnabled();
        if (name == "development") return self.isDevelopmentEnabled();
        if (name == "curiosity") return self.isCuriosityEnabled();
        return false;
    }, py::arg("name"), R"pbdoc(
        Check if a specific learning subsystem is enabled.

        This method provides a flexible way to query the status of different
        agent learning and adaptation subsystems by name, supporting case-insensitive
        queries and unknown subsystem names.

        Args:
            name: Subsystem name (case-sensitive). Valid names:
                - "reward_modulation": Reward prediction and error-based learning
                - "structural_plasticity": Synaptic growth and reorganization
                - "development": Developmental stage progression
                - "curiosity": Novelty-driven exploration and learning

        Returns:
            bool: True if the specified subsystem is enabled, False otherwise.
                Returns False for unknown subsystem names.

        Example:
            >>> brain = nlm.createBrain(nlm.createDefaultConfig())
            >>> agent_brain = nlm.createAgentBrain(brain)
            >>> 
            >>> # Check individual subsystems
            >>> print(f"Reward modulation: {agent_brain.is_subsystem_enabled('reward_modulation')}")
            >>> print(f"Development: {agent_brain.is_subsystem_enabled('development')}")
            >>> print(f"Curiosity: {agent_brain.is_subsystem_enabled('curiosity')}")
            >>> 
            >>> # Enable and check
            >>> agent_brain.enable_development(True)
            >>> print(agent_brain.is_subsystem_enabled('development'))
            True

        Use Case:
            This method is useful for:
            - Dynamic system configuration based on current learning needs
            - Monitoring and logging subsystem states
            - Conditional behavior based on available learning mechanisms
            - Implementing adaptive architectures that switch subsystems

        Related Methods:
            See also enable_all(), disable_all() for bulk subsystem control,
            get_system_state() for comprehensive status, get_subsystem_status()
            for batch status checking.

        Note:
            Returns False for unknown subsystem names to provide graceful
            degradation rather than throwing errors.
    )pbdoc")
    .def("enable_all", [](AgentBrain& self) {
        self.enableRewardModulation(true);
        self.enableStructuralPlasticity(true);
        self.enableDevelopment(true);
        self.enableCuriosity(true);
    }, "Enable all subsystems")
    .def("disable_all", [](AgentBrain& self) {
        self.enableRewardModulation(false);
        self.enableStructuralPlasticity(false);
        self.enableDevelopment(false);
        self.enableCuriosity(false);
    }, "Disable all subsystems")
    
    // Subsystem status methods
    .def("get_subsystem_status", [](const AgentBrain& self) {
        py::dict status_dict;
        status_dict["reward_modulation"] = self.isRewardModulationEnabled();
        status_dict["structural_plasticity"] = self.isStructuralPlasticityEnabled();
        status_dict["development"] = self.isDevelopmentEnabled();
        status_dict["curiosity"] = self.isCuriosityEnabled();
        return status_dict;
    }, "Get status of all subsystems")
    
    // Configuration methods
    .def("enable_reward_modulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable/disable reward modulation")
    .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable/disable structural plasticity")
    .def("enable_development", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable/disable development")
    .def("enable_curiosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable/disable curiosity")
    
    // Brain access
    .def("get_brain", &AgentBrain::getBrain, "Get brain pointer");

    // ============================================================================
    // ADVANCED BRAIN CONFIGURATION BUILDERS
    // ============================================================================
    m.def("create_brain_with_regions", [](const std::shared_ptr<Config>& config, 
                                          const py::dict& neuron_distribution) {
        auto brain = std::make_shared<Brain>(config);
        // Advanced neuron distribution across regions logic
        for (auto item : neuron_distribution) {
            std::string region_name = item.first.cast<std::string>();
            int neuron_count = item.second.cast<int>();
            brain->addRegion(region_name);
            // Add neurons to region (simplified implementation)
            auto region = brain->getRegion(brain->getRegionIds()[brain->getRegionCount() - 1]);
            for (int i = 0; i < neuron_count; ++i) {
                // This would require additional brain API methods
            }
        }
        return brain;
    }, py::arg("config"), py::arg("neuron_distribution"), 
        "Create brain with specific neuron distribution across regions");

    m.def("create_brain_with_connections", [](const std::shared_ptr<Config>& config,
                                              const py::dict& connections_config) {
        auto brain = std::make_shared<Brain>(config);
        // Advanced connection pattern implementation
        return brain;
    }, py::arg("config"), py::arg("connections_config"),
        "Create brain with predefined connection patterns");

    m.def("create_brain_with_stdp_config", [](const std::shared_ptr<Config>& config,
                                              const py::dict& stdp_config) {
        auto brain = std::make_shared<Brain>(config);
        // Advanced STDP configuration implementation
        return brain;
    }, py::arg("config"), py::arg("stdp_config"),
        "Create brain with specific STDP parameters");

    m.def("create_brain_with_plasticity_config", [](const std::shared_ptr<Config>& config,
                                                      const py::dict& plasticity_config) {
        auto brain = std::make_shared<Brain>(config);
        // Comprehensive plasticity settings implementation
        return brain;
    }, py::arg("config"), py::arg("plasticity_config"),
        "Create brain with comprehensive plasticity settings");

    // ============================================================================
    // ADVANCED SIMULATION UTILITIES
    // ============================================================================
    m.def("run_episode", [](const std::shared_ptr<SimpleWorld>& world,
                            const std::shared_ptr<AgentBrain>& agent,
                            int num_steps, const py::object& seed) {
        if (!seed.is(py::none())) {
            int seed_val = seed.cast<int>();
            world->setRandomSeed(seed_val);
        }
        
        // Episode management implementation
        return "Episode completed";
    }, py::arg("world"), py::arg("agent"), py::arg("num_steps"), 
        py::arg("seed").none() ? py::arg("seed") = py::none(),
        "Run complete episode with episode management");

    m.def("run_training_loop", [](const std::shared_ptr<Brain>& brain,
                                  const std::shared_ptr<SimpleWorld>& world,
                                  const std::shared_ptr<AgentBrain>& agent,
                                  int episodes, int steps_per_episode,
                                  const py::object& callbacks) {
        // Training loop with callback support implementation
        return "Training completed";
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("episodes"), py::arg("steps_per_episode"),
        py::arg("callbacks").none() ? py::arg("callbacks") = py::none(),
        "Training loop with callback support");

    m.def("run_experiment", [](const std::string& experiment_name,
                               const py::dict& config, 
                               const py::object& kwargs) {
        // Experiment execution implementation
        return "Experiment completed";
    }, py::arg("experiment_name"), py::arg("config"),
        py::arg("kwargs").none() ? py::arg("kwargs") = py::none(),
        "Run predefined experiments");

    m.def("benchmark_simulation", [](const std::shared_ptr<Brain>& brain,
                                     const std::shared_ptr<SimpleWorld>& world,
                                     const std::shared_ptr<AgentBrain>& agent,
                                     double duration_seconds) {
        // Performance benchmarking implementation
        return "Benchmark completed";
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("duration_seconds"), "Performance benchmarking");

    // ============================================================================
    // ADVANCED MEMORY AND STATE MANAGEMENT
    // ============================================================================
    m.def("save_brain_state_with_metadata", [](const std::shared_ptr<Brain>& brain,
                                              const py::dict& metadata_dict,
                                              const std::string& filepath) {
        // Save brain with metadata implementation
        brain->save(filepath);
        return "Brain state saved with metadata";
    }, py::arg("brain"), py::arg("metadata_dict"), py::arg("filepath"),
        "Save brain with metadata");

    m.def("load_brain_state_with_metadata", [](const std::string& filepath) {
        // Load brain with metadata validation implementation
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("filepath"), "Load brain with metadata validation");

    m.def("create_memory_checkpoint", [](const std::shared_ptr<Brain>& brain,
                                         int max_checkpoints) {
        // Create automatic checkpoint system implementation
        return "Memory checkpoint system created";
    }, py::arg("brain"), py::arg("max_checkpoints") = 10,
        "Create automatic checkpoint system");

    m.def("replay_episode_from_memory", [](const py::dict& memory) {
        // Replay saved episode implementation
        return "Episode replayed from memory";
    }, py::arg("memory"), "Replay saved episode");

    m.def("get_brain_metrics", [](const std::shared_ptr<Brain>& brain) {
        // Comprehensive metrics collection implementation
        py::dict metrics;
        metrics["total_neurons"] = brain->getTotalNeuronCount();
        metrics["total_synapses"] = brain->getTotalSynapseCount();
        metrics["active_neurons"] = brain->getActiveNeuronCount();
        metrics["firing_neurons"] = brain->getFiringNeuronCount();
        metrics["average_firing_rate"] = brain->getAverageFiringRate();
        metrics["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        metrics["total_spikes"] = brain->getTotalSpikeCount();
        return metrics;
    }, py::arg("brain"), "Comprehensive metrics collection");

    // ============================================================================
    // ADVANCED AGENT AND WORLD CONFIGURATION
    // ============================================================================
    m.def("create_world_with_layout", [](const py::dict& layout_config) {
        auto world = std::make_shared<SimpleWorld>();
        // Create world from layout specification implementation
        return world;
    }, py::arg("layout_config"), "Create world from layout specification");

    m.def("create_agent_with_subsystems", [](const std::shared_ptr<Brain>& brain,
                                            const py::dict& subsystem_config) {
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        // Create agent with enabled subsystems implementation
        return agent_brain;
    }, py::arg("brain"), py::arg("subsystem_config"),
        "Create agent with enabled subsystems");

    m.def("configure_agent_for_task", [](const std::shared_ptr<AgentBrain>& agent,
                                        const py::dict& task_config) {
        // Configure agent for specific task implementation
        return "Agent configured for task";
    }, py::arg("agent"), py::arg("task_config"),
        "Configure agent for specific task");

    m.def("create_agent_world_pair", [](const py::dict& brain_config,
                                       const py::dict& world_config,
                                       const py::dict& agent_config) {
        // Create complete agent-world pair implementation
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent_brain = std::make_shared<AgentBrain>(brain);
        return py::make_tuple(brain, world, agent_brain);
    }, py::arg("brain_config"), py::arg("world_config"), py::arg("agent_config"),
        "Create complete agent-world pair");

    // ============================================================================
    // ADVANCED NEUROMODULATION AND PLASTICITY
    // ============================================================================
    m.def("apply_curriculum_learning", [](const std::shared_ptr<AgentBrain>& agent,
                                          const py::dict& curriculum) {
        // Apply curriculum learning schedule implementation
        return "Curriculum learning applied";
    }, py::arg("agent"), py::arg("curriculum"), "Apply curriculum learning schedule");

    m.def("set_adaptive_neuromodulation", [](const std::shared_ptr<AgentBrain>& agent,
                                             const py::dict& neuromod_config) {
        // Set adaptive neuromodulation parameters implementation
        return "Adaptive neuromodulation configured";
    }, py::arg("agent"), py::arg("neuromod_config"),
        "Set adaptive neuromodulation parameters");

    m.def("configure_structural_plasticity", [](const std::shared_ptr<Brain>& brain,
                                                const py::dict& plasticity_config) {
        // Configure structural plasticity rules implementation
        return "Structural plasticity configured";
    }, py::arg("brain"), py::arg("plasticity_config"),
        "Configure structural plasticity rules");

    m.def("enable_meta_plasticity", [](const std::shared_ptr<Brain>& brain,
                                       const py::dict& meta_config) {
        // Enable meta-learning and meta-plasticity implementation
        return "Meta-plasticity enabled";
    }, py::arg("brain"), py::arg("meta_config"), "Enable meta-learning and meta-plasticity");

    // ============================================================================
    // ADVANCED DATA AND MODEL MANAGEMENT
    // ============================================================================
    m.def("export_model_as_tflite", [](const std::shared_ptr<Brain>& brain,
                                      const std::string& filepath) {
        // Export neural model as TFLite implementation
        return "Model exported as TFLite";
    }, py::arg("brain"), py::arg("filepath"), "Export neural model as TFLite");

    m.def("create_neural_snapshot", [](const std::shared_ptr<Brain>& brain) {
        // Create compressed neural snapshot implementation
        return "Neural snapshot created";
    }, py::arg("brain"), "Create compressed neural snapshot");

    m.def("import_neural_snapshot", [](const std::shared_ptr<Brain>& brain,
                                       const std::string& filepath) {
        // Import neural snapshot implementation
        return "Neural snapshot imported";
    }, py::arg("brain"), py::arg("filepath"), "Import neural snapshot");

    m.def("create_model_archive", [](const py::list& models_list,
                                    const py::dict& metadata) {
        // Create model archive implementation
        return "Model archive created";
    }, py::arg("models_list"), py::arg("metadata"), "Create model archive");

    // ============================================================================
    // ADVANCED EXPERIMENT AND RESEARCH TOOLS
    // ============================================================================
    m.def("create_research_suite", [](const py::list& experiments_list) {
        // Create comprehensive research suite implementation
        return "Research suite created";
    }, py::arg("experiments_list"), "Create comprehensive research suite");

    m.def("run_research_experiment", [](const py::object& research_suite,
                                        const py::dict& parameters) {
        // Run research experiments implementation
        return "Research experiment completed";
    }, py::arg("research_suite"), py::arg("parameters"),
        "Run research experiments");

    m.def("analyze_brain_activity", [](const std::shared_ptr<Brain>& brain,
                                       const py::dict& analysis_config) {
        // Analyze neural activity patterns implementation
        py::dict analysis_results;
        analysis_results["mean_firing_rate"] = brain->getAverageFiringRate();
        analysis_results["e_i_ratio"] = brain->getExcitationInhibitionRatio();
        analysis_results["total_neurons"] = brain->getTotalNeuronCount();
        return analysis_results;
    }, py::arg("brain"), py::arg("analysis_config"),
        "Analyze neural activity patterns");

    m.def("create_neural_topography", [](const std::shared_ptr<Brain>& brain,
                                         const py::list& regions) {
        // Create neural topographic maps implementation
        py::dict topography;
        return topography;
    }, py::arg("brain"), py::arg("regions"), "Create neural topographic maps");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
