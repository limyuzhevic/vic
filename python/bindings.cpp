#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/eigen.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a high-level interface to the neural simulation framework,
        enabling users to create and simulate artificial brains with advanced cognitive
        capabilities including memory, learning, and decision-making.
        
        Key Features:
        - Spiking neural networks with biologically realistic dynamics
        - Multiple memory systems (working, episodic, semantic, procedural)
        - Advanced plasticity rules (STDP, Hebbian, structural)
        - Neuromodulation systems (dopamine, curiosity, novelty)
        - Sensorimotor integration and motor control
        - Developmental processes and learning
        - Visualization and monitoring tools
        
        Usage Example:
            >>> from pynlm import *
            >>> config = createDefaultConfig()
            >>> # Configure brain parameters
            >>> config.set("brain.neuron.count", 100)
            >>> config.set("brain.region.count", 5)
            >>> # Create brain
            >>> brain = createBrain(config)
            >>> brain.initialize()
            >>> # Create environment
            >>> world = createSimpleWorld()
            >>> world.configure(100, 100, 32, 32)
            >>> # Run simulation loop
            >>> for step in range(1000):
            ...     # Get sensory input
            ...     percept = world.getSensoryPercept()
            ...     # Process sensory input
            ...     agent.processSensoryInput(percept)
            ...     # Produce action
            ...     action = brain.produceAction()
            ...     # Apply action in world
            ...     result = world.applyMotorCommand(action, step)
            ...     # Update brain based on reward
            ...     if result.success:
            ...         agent.applyRewardModulation(result.reward, predicted_reward)
    )pbdoc";

    // Register standard exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // NeuronId class with enhanced documentation and type safety
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId provides a stable, unique identifier for neurons that persists
        throughout the simulation, even when neurons are created or destroyed.
        Used for synaptic connections and neuron references.
        
        Attributes:
            value (int): Internal integer value representing the neuron ID
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value,
            R"pbdoc(Internal integer value representing the neuron ID)pbdoc")
        .def("index", &NeuronId::index,
            R"pbdoc(Return the index component of the neuron ID)pbdoc")
        .def("__eq__", &NeuronId::operator==,
            R"pbdoc(Check if two NeuronIds are equal)pbdoc")
        .def("__ne__", &NeuronId::operator!=,
            R"pbdoc(Check if two NeuronIds are not equal)pbdoc")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
            R"pbdoc(Return hash of the NeuronId for use in dictionaries and sets)pbdoc")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        },
            R"pbdoc(String representation of the NeuronId)pbdoc")
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        },
            R"pbdoc(String representation of the NeuronId)pbdoc");

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId provides a stable, unique identifier for synapses that persists
        throughout the simulation. Used for synaptic connections and plasticity.
        
        Attributes:
            value (int): Internal integer value representing the synapse ID
        "pbdoc")
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

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId provides a stable, unique identifier for brain regions that
        persist throughout the simulation. Regions are containers for populations
        and handle local connectivity.
        
        Attributes:
            value (int): Internal integer value representing the region ID
        "pbdoc")
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId provides a stable, unique identifier for neuron populations
        within a brain region. Populations contain groups of neurons of the same type.
        
        Attributes:
            value (int): Internal integer value representing the population ID
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types.
        
        Different neuron types have different functional properties and roles
        in the neural network. The type determines:
        - Synaptic connections (who they connect to)
        - Input/output characteristics
        - Plasticity rules applied
        - Computational functions
        
        Types:
            Excitatory: Neurons that increase firing in postsynaptic neurons
            Inhibitory: Neurons that decrease firing in postsynaptic neurons
            Modulatory: Neurons that influence plasticity and neuromodulation
            Sensory: Neurons that receive external sensory input
            Motor: Neurons that generate motor outputs
            Internal: Internal processing neurons
        "pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types.
        
        Different synapse types have different functional properties and dynamics.
        The type determines:
        - Synaptic transmission speed
        - Reversal potential
        - Plasticity rules
        - Modulation effects
        
        Types:
            Excitatory: AMPA-type synapses (depolarizing)
            Inhibitory: GABA-type synapses (hyperpolarizing)
            Modulatory: GPCR-type synapses (slow, modulatory)
            Electrical: Gap junctions (direct current flow)
            GapJunction: Electrical coupling between neurons
        "pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages.
        
        The brain progresses through distinct developmental stages, each with
        different characteristics and capabilities. This models critical periods
        and developmental transitions observed in biological systems.
        
        Stages:
            Initial: Newborn stage with basic connectivity
            CriticalPeriod: Enhanced plasticity and learning
            Maturation: Refinement and specialization
            Adult: Mature, stable functionality
            Aging: Age-related changes and decline
        "pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Neurons can be in different firing states that determine their behavior:
        
        States:
            Resting: Neuron not firing, membrane potential at resting level
            Active: Neuron currently firing action potentials
            Refractory: Neuron just fired, cannot fire again immediately
            Inhibited: Neuron is inhibited from firing
        "pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enumeration with detailed documentation
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of possible action types.
        
        Actions represent motor outputs or behavioral responses. Each type has
        specific parameters and characteristics:
        
        Movement Actions:
            MoveForward: Move forward in the environment
            MoveBackward: Move backward in the environment
            MoveLeft: Turn and move left
            MoveRight: Turn and move right
            TurnLeft: Rotate counterclockwise
            TurnRight: Rotate clockwise
        
        Sensing Actions:
            Look: Look in a specific direction
            LookUp: Look upward
            LookDown: Look downward
        
        Interaction Actions:
            Interact: Interact with nearby objects
            Eat: Consume food/resources
            Drink: Consume liquids
        
        State Actions:
            Rest: Stop moving and rest
            Wait: Pause action
            Custom: User-defined action
        "pbdoc")
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

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
        
        These are the basic motor commands that can be executed by the agent.
        They are more abstract than ActionType and represent higher-level
        motor intentions.
        
        Commands:
            MoveForward: Move forward
            MoveBackward: Move backward
            TurnLeft: Turn left
            TurnRight: Turn right
            LookLeft: Look left
            LookRight: Look right
            Interact: Interact with environment
            Wait: Do nothing
        "pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
        
        Objects in the simulation environment have different types with different
        properties and behaviors:
        
        Types:
            Empty: No object (empty space)
            Resource: Consumable resource (food, water)
            Hazard: Dangerous object (poison, trap)
            Wall: Physical barrier
            Marker: Reference point or target
        "pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Config class with enhanced documentation
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class manages configuration parameters for the entire NLM system.
        It provides a flexible key-value store for all types of configuration data,
        from neural network parameters to simulation settings.
        
        Key Features:
        - Type-safe configuration with automatic type conversion
        - Multiple configuration sources (file, command line, runtime)
        - Validation and error checking
        - Default configuration management
        - Summary and diagnostic information
        
        Configuration Categories:
        - Brain parameters: neuron count, region count, connectivity
        - Neuron parameters: membrane properties, firing dynamics
        - Plasticity parameters: learning rates, synaptic rules
        - Memory parameters: capacity and characteristics
        - Simulation parameters: time steps, limits
        - Neuromodulation parameters: reward, curiosity, novelty
        - Developmental parameters: stages and rates
        - Visualization parameters: display and recording
        
        Example Usage:
            >>> config = createDefaultConfig()
            >>> config.set("brain.neuron.count", 100)
            >>> config.set("neuron.membrane.threshold", -55.0)
            >>> value = config.getOr("simulation.dt", 0.001)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises ValueError if file not found or invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command line arguments.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises ValueError if save fails.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a detailed summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input.
        
        SensoryInput represents input from the environment or internal state.
        All sensory modalities inherit from this base class and provide
        standardized interfaces for accessing sensory data.
        
        The class provides:
        - Type information for the sensory modality
        - Raw data access as arrays
        - Dimensional information
        - Timestamps for temporal alignment
        - Data normalization and scaling
        
        Subclasses:
            Vision: Visual input (images, pixels)
            Audio: Audio input (sound samples)
            Internal: Internal state signals
        "pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with detailed documentation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input representing images or pixel data.
        
        Vision captures visual information from the environment, including
        object shapes, colors, and spatial relationships. It's used by the
        visual processing system to understand the environment.
        
        Attributes:
            width (int): Image width in pixels
            height (int): Image height in pixels
            channels (int): Number of color channels (1=grayscale, 3=RGB)
        
        Methods:
            setData: Set raw pixel data
            getWidth: Get image width
            getHeight: Get image height
            getChannels: Get number of channels
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Initialize vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set raw pixel data as a flat vector of floats")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Audio captures auditory information including frequency, amplitude, and
        temporal patterns. Used by the auditory processing system.
        
        Attributes:
            sampleRate (int): Sampling rate in Hz
            numSamples (int): Number of audio samples
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal state signals representing neural and physiological states.
        
        InternalSignals represents internal states of the brain and body,
        including hunger, thirst, energy levels, and other homeostatic variables.
        These signals drive motivation and behavioral needs.
        
        Methods:
            addSignal: Add a signal value
            clearSignals: Clear all signals
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value (e.g., hunger level, energy level)")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    // Action class with comprehensive documentation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Action represents a behavioral response or motor output generated by
        the brain. It includes both the type of action and the parameters needed
        to execute it. Actions are produced by the brain's motor system and
        executed by the agent's body in the environment.
        
        Action Types:
            Movement actions have position/orientation parameters
            Sensing actions have direction parameters
            Interaction actions have target/object parameters
            State actions have duration or intensity parameters
        
        Attributes:
            type (ActionType): Type of action to perform
            parameters (list): Action-specific parameters
        
        Methods:
            getType: Get action type
            setType: Set action type
            getParameters: Get action parameters
            setParameters: Set action parameters
            getName: Get human-readable action name
            clone: Create a copy of this action
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the type of action to perform")
        .def("setType", &Action::setType, py::arg("type"), "Set the type of action")
        .def("getParameters", &Action::getParameters, "Get action parameters as a list")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable name for this action")
        .def("clone", &Action::clone, "Create a copy of this action");

    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        WorldObject represents objects in the simulation environment, including
        agents, resources, hazards, and landmarks. Each object has a position,
        type, and properties that determine how it interacts with the agent.
        
        Attributes:
            x (float): X position in world coordinates
            y (float): Y position in world coordinates
            type (WorldObjectType): Type of object
            value (float): Object value (e.g., nutritional value)
            radius (float): Collision radius
            active (bool): Whether object is active
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position in world coordinates")
        .def_readwrite("y", &WorldObject::y, "Y position in world coordinates")
        .def_readwrite("radius", &WorldObject::radius, "Collision radius")
        .def_readwrite("type", &WorldObject::type, "Type of object")
        .def_readwrite("value", &WorldObject::value, "Object value (e.g., nutritional value)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // AgentBody class with detailed documentation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representing the physical agent in the environment.
        
        AgentBody represents the physical characteristics and state of the agent,
        including its position, orientation, movement, and physiological state.
        It serves as the interface between the neural system and the environment.
        
        State Variables:
            x, y: Position coordinates
            orientation: Heading angle (radians)
            velocityX, velocityY: Linear velocity components
            angularVelocity: Angular velocity (rad/s)
            energy: Current energy level
            health: Health status (0.0-1.0)
            age: Biological age
            isMoving: Whether agent is currently moving
            isTurning: Whether agent is currently turning
            lastActionTime: Time since last action
        
        Methods:
            reset: Reset body to initial state
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position in world coordinates")
        .def_readwrite("y", &AgentBody::y, "Y position in world coordinates")
        .def_readwrite("orientation", &AgentBody::orientation, "Heading angle (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health status (0.0-1.0)")
        .def_readwrite("age", &AgentBody::age, "Biological age")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action executed in the world.
        
        ActionResult represents the outcome of an action, including whether it
        was successful, the reward received, and any additional information.
        This is used for learning and decision-making.
        
        Attributes:
            reward (float): Reward value (positive = good, negative = bad)
            success (bool): Whether action was successful
            message (str): Additional information about action result
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Additional information");

    // SensoryPercept class with comprehensive documentation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data from environment processing.
        
        SensoryPercept contains processed sensory information ready for use by
        the neural system. It combines multiple sensory modalities into a unified
        representation that can be used for perception and decision-making.
        
        Sensory Modalities:
            Vision: Visual information (images, object positions)
            Touch: Physical contact and proximity
            Internal: Internal state signals
            Proprioception: Body position and movement
            Audio: Auditory information
        
        The percept provides a unified interface for all sensory input, making
        it easier for the brain to integrate and process environmental information.
        
        Methods:
            getVision: Get visual information
            setVision: Set visual information
            getTouch: Get tactile information
            setTouch: Set tactile information
            getInternal: Get internal signals
            setInternal: Set internal signals
            getProprioception: Get proprioceptive information
            setProprioception: Set proprioceptive information
            getAudio: Get auditory information
            setAudio: Set auditory information
            getAllSignals: Get all signals as a dictionary
            getTimestamp: Get percept timestamp
            setTimestamp: Set percept timestamp
        "pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get visual information")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set visual information")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width in pixels")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height in pixels")
        .def("getTouch", &SensoryPercept::getTouch, "Get tactile information")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set tactile information")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioceptive information")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioceptive information")
        .def("getAudio", &SensoryPercept::getAudio, "Get auditory information")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set auditory information")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals as a dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp");

    // SimpleWorld class with extensive documentation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld represents a 2D simulation environment where agents interact
        with objects and navigate through space. It provides the interface
        between the neural system and the physical environment.
        
        World Features:
        - 2D coordinate system with X,Y positions
        - Dynamic objects (agents, resources, hazards)
        - Sensorimotor integration
        - Physics simulation (collision detection, movement)
        - Action execution and feedback
        
        Key Methods:
            configure: Set world dimensions and vision parameters
            reset: Reset world to initial state
            setAgentStart: Set agent starting position
            update: Advance world state
            applyMotorCommand: Execute agent action
            getSensoryPercept: Get agent's sensory input
            getAgentBody: Get agent physical state
        
        Configuration Parameters:
            width: World width in units
            height: World height in units
            visionWidth: Vision sensor width (pixels)
            visionHeight: Vision sensor height (pixels)
            maxEnergy: Maximum agent energy
            energyDecayRate: Energy loss rate per timestep
        
        Usage Example:
            >>> world = createSimpleWorld()
            >>> world.configure(100, 100, 32, 32)
            >>> world.setAgentStart(50, 50)
            >>> for step in range(1000):
            ...     percept = world.getSensoryPercept()
            ...     action = brain.produceAction()
            ...     result = world.applyMotorCommand(action, step)
            ...     if result.success:
            ...     agent.applyRewardModulation(result.reward, predicted_reward)
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensor parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Advance world state by one timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command and execute action")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's physical body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Brain class with comprehensive documentation
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Brain is the main neural computation engine in the NLM system. It implements
        real spiking neural computation with event-driven dynamics and integrates
        multiple cognitive and memory systems. The brain processes sensory input,
        generates motor output, and learns from experience.
        
        Brain Architecture:
        - Sensory Processing: Receives and interprets environmental input
        - Neural Core: Spiking neural networks with plasticity
        - Memory Systems: Working, episodic, semantic, and procedural memory
        - Prediction Systems: Forward models and prediction errors
        - Cognitive Systems: Planning, attention, concept formation
        - Neuromodulation: Dopamine, curiosity, novelty
        - Development System: Growth and maturation processes
        
        Key Capabilities:
        - Real-time simulation of large-scale neural networks
        - Multiple learning mechanisms (STDP, Hebbian, reward-modulated)
        - Advanced memory management and retrieval
        - Sensorimotor integration and control
        - Developmental learning and adaptation
        - Neuromodulatory control and motivation
        
        Core Methods:
            initialize: Initialize brain with configuration
            step: Advance simulation (with or without timestamp)
            receiveSensoryInput: Inject sensory input
            injectCurrent: Inject current into specific neurons
            produceAction: Generate motor output
            reset: Reset brain state
            logStatus: Log current brain status
        
        Configuration Options:
            brain.neuron.count: Number of neurons
            brain.region.count: Number of brain regions
            neuron.membrane.properties: Neuron membrane parameters
            plasticity.rules: Plasticity learning rates
            memory.capacity: Memory system capacities
            simulation.parameters: Time steps and limits
        
        Usage Example:
            >>> config = createDefaultConfig()
            >>> config.set("brain.neuron.count", 1000)
            >>> brain = createBrain(config)
            >>> brain.initialize()
            >>> # Main simulation loop
            >>> for step in range(10000):
            ...     percept = world.getSensoryPercept()
            ...     brain.receiveSensoryInput(percept)
            ...     action = brain.produceAction()
            ...     result = world.applyMotorCommand(action, step)
            ...     if result.success:
            ...         brain.applyNeuromodulation(reward, predicted_reward)
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True if successful, False otherwise.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with just step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with step number and timestamp")
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
             "Produce motor action based on neural activity. Returns NoneAction if no action.")
        .def("reset", &Brain::reset, "Reset brain state to initial conditions")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Returns True if successful, False otherwise.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Returns True if successful, False otherwise.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount, "Get number of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status and statistics");

    // AgentBrain class with detailed documentation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain provides a high-level interface for integrating the neural brain
        with the environment through an agent. It handles the conversion between
        neural activity and motor commands, as well as reward processing and
        developmental updates.
        
        AgentBrain Architecture:
        - Sensory Processing: Converts world percepts to neural input
        - Motor Control: Converts neural activity to world actions
        - Reward Processing: Handles reinforcement learning signals
        - Development: Manages brain development over time
        - Monitoring: Tracks performance and state metrics
        
        Key Features:
        - Sensorimotor integration and control
        - Reward-based learning and adaptation
        - Developmental learning and maturation
        - Performance monitoring and evaluation
        - State management and checkpointing
        
        Core Methods:
            initialize: Initialize with world and brain
            processSensoryInput: Convert percept to neural input
            decodeMotorCommand: Convert neural activity to motor command
            applyRewardModulation: Apply reward signals
            updateDevelopment: Update developmental state
            reset: Reset agent for new episode
        
        Usage Example:
            >>> agent = AgentBrain(brain)
            >>> agent.initialize(world)
            >>> for step in range(1000):
            ...     percept = world.getSensoryPercept()
            ...     agent.processSensoryInput(percept)
            ...     action = world.applyMotorCommand(agent.decodeMotorCommand(), step)
            ...     if action.success:
            ...         agent.applyRewardModulation(action.reward, predicted_reward)
            ...         agent.updateDevelopment(step)
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent brain with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled");

    // Factory functions with comprehensive documentation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration for the NLM system.
        
        Returns a Config object with sensible default values for all major
        system parameters. This configuration includes:
        - Neural network parameters (neuron count, region count, etc.)
        - Neuron membrane properties (resting potential, threshold, etc.)
        - Plasticity rule parameters (learning rates)
        - Memory system capacities
        - Simulation settings (time steps, limits)
        - Neuromodulation parameters
        - Developmental parameters
        - Visualization settings
        
        Returns:
            Config: Default configuration object
        "pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with the specified configuration.
        
        Args:
            config (Config): Configuration object for the brain
            
        Returns:
            Brain: Brain object initialized with the configuration
        "pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world for NLM simulation.
        
        Returns:
            SimpleWorld: New world object
        "pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface with the specified brain.
        
        Args:
            brain (Brain): Underlying brain object
            
        Returns:
            AgentBrain: Agent brain interface
        "pbdoc");

    // Convenience constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    }

} // namespace nlm
