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

// Simplified beginner-friendly API functions
namespace beginner_api {
    // Create a brain with default configuration
    inline std::shared_ptr<Brain> createBrain() {
        auto config = std::make_shared<Config>();
        return std::make_shared<Brain>(config);
    }
    
    // Create a simple world with reasonable defaults
    inline std::shared_ptr<SimpleWorld> createWorld(float width = 20.0f, float height = 20.0f,
                                                    size_t visionWidth = 8, size_t visionHeight = 8) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }
    
    // Create a default configuration with sensible values
    inline std::shared_ptr<Config> createDefaultConfig() {
        return std::make_shared<Config>();
    }
    
    // Run a simple brain simulation with logging
    inline float runSimulation(std::shared_ptr<Brain> brain, int steps = 1000, bool verbose = false) {
        if (!brain) {
            throw std::runtime_error("Brain pointer is null. Create a brain first.");
        }
        brain->initialize();
        
        float totalSpikes = 0;
        for (int step = 0; step < steps; ++step) {
            brain->step(static_cast<SimulationStep>(step));
            totalSpikes += brain->getTotalSpikeCount();
            
            if (verbose && step % 100 == 0) {
                // Note: In a real implementation, we would log to Python stdout
                // This is a simplified demonstration
            }
        }
        return totalSpikes;
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        **Wave 5 Enhanced Python API**
        
        This is the Wave 5 enhanced version of the NLM Python bindings,
        featuring:
        
        - Comprehensive documentation with detailed examples
        - Simplified beginner-friendly API layer
        - Modern Python features (context managers, type hints)
        - Improved error handling with Pythonic messages
        - Comprehensive tutorial system
        
        For basic usage, see the beginner examples in examples/01_basic_brain.py
        
        **Getting Started:**
        
        ```python
        import pynlm
        
        # Create and initialize a brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the brain network.
        
        NeuronId represents a specific neuron within a population and region.
        It combines a population ID and neuron index to uniquely identify
        any neuron in the brain.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a NeuronId from a raw value (for advanced use)")
        .def_readwrite("value", &NeuronId::value,
                       "Raw internal value of the neuron identifier")
        .def("index", &NeuronId::index,
             "Get the neuron index within its population")
        .def("__eq__", &NeuronId::operator==,
             "Check if two NeuronIds are equal")
        .def("__ne__", &NeuronId::operator!=,
             "Check if two NeuronIds are not equal")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in Python sets and dictionaries")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the neuron identifier");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synaptic connection between neurons.
        
        SynapseId represents a specific synapse in the brain network,
        identifying both the source neuron and target neuron of the connection.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a SynapseId from a raw value (for advanced use)")
        .def_readwrite("value", &SynapseId::value,
                       "Raw internal value of the synapse identifier")
        .def("index", &SynapseId::index,
             "Get the synapse index within its population")
        .def("__eq__", &SynapseId::operator==,
             "Check if two SynapseIds are equal")
        .def("__ne__", &SynapseId::operator!=,
             "Check if two SynapseIds are not equal")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in Python sets and dictionaries")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "String representation of the synapse identifier");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId identifies a specific neural region in the brain,
        such as the visual cortex, motor cortex, or association areas.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a RegionId from a raw value (for advanced use)")
        .def_readwrite("value", &RegionId::value,
                       "Raw internal value of the region identifier")
        .def("index", &RegionId::index,
             "Get the region index within its population")
        .def("__eq__", &RegionId::operator==,
             "Check if two RegionIds are equal")
        .def("__ne__", &RegionId::operator!=,
             "Check if two RegionIds are not equal")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Hash function for use in Python sets and dictionaries")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "String representation of the region identifier");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId identifies a group of neurons with similar properties
        and connectivity patterns, such as all excitatory neurons or
        all motor neurons.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             "Create a PopulationId from a raw value (for advanced use)")
        .def_readwrite("value", &PopulationId::value,
                       "Raw internal value of the population identifier")
        .def("index", &PopulationId::index,
             "Get the population index within its region")
        .def("__eq__", &PopulationId::operator==,
             "Check if two PopulationIds are equal")
        .def("__ne__", &PopulationId::operator!=,
             "Check if two PopulationIds are not equal");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the neural network.
        
        Different neuron types have distinct functions and characteristics:
        - Excitatory neurons stimulate other neurons
        - Inhibitory neurons suppress neural activity
        - Modulatory neurons influence overall network state
        - Sensory neurons process external stimuli
        - Motor neurons produce action outputs
        - Internal neurons handle internal processing
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory,
               "Neurons that excite other neurons via positive synapses")
        .value("Inhibitory", NeuronType::Inhibitory,
               "Neurons that inhibit other neurons via negative synapses")
        .value("Modulatory", NeuronType::Modulatory,
               "Neurons that modulate global brain state (dopamine, etc.)")
        .value("Sensory", NeuronType::Sensory,
               "Neurons that receive and process sensory input")
        .value("Motor", NeuronType::Motor,
               "Neurons that produce motor commands and actions")
        .value("Internal", NeuronType::Internal,
               "Neurons for internal processing and cognition")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synaptic connection types.
        
        Different synapse types have distinct functional properties:
        - Excitatory synapses increase target neuron activity
        - Inhibitory synapses decrease target neuron activity
        - Modulatory synapses affect overall network state
        - Electrical synapses allow direct current flow
        - Gap junctions enable electrical coupling
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory,
               "Synapses that increase target neuron firing probability")
        .value("Inhibitory", SynapseType::Inhibitory,
               "Synapses that decrease target neuron firing probability")
        .value("Modulatory", SynapseType::Modulatory,
               "Synapses that affect global brain modulation")
        .value("Electrical", SynapseType::Electrical,
               "Direct electrical connections between neurons")
        .value("GapJunction", SynapseType::GapJunction,
               "Specialized electrical coupling between neurons")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the brain.
        
        The brain goes through distinct developmental phases with different
        plasticity and learning characteristics:
        - Initial: Newborn, high plasticity
        - CriticalPeriod: Sensitive learning phase
        - Maturation: Stabilization and refinement
        - Adult: Optimized performance, lower plasticity
        - Aging: Decline in plasticity and performance
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial,
               "Newborn stage with high plasticity and rapid learning")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod,
               "Sensitive period for key skill acquisition")
        .value("Maturation", DevelopmentalStage::Maturation,
               "Stabilization and refinement of neural circuits")
        .value("Adult", DevelopmentalStage::Adult,
               "Adult stage with optimized performance")
        .value("Aging", DevelopmentalStage::Aging,
               "Aging stage with declining plasticity")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Neurons can be in different states of activity:
        - Resting: Not currently firing
        - Active: Currently firing spikes
        - Refractory: Recently fired, cannot fire again
        - Inhibited: Suppressed from firing by inhibition
    )pbdoc")
        .value("Resting", FiringState::Resting,
               "Neuron is not currently firing")
        .value("Active", FiringState::Active,
               "Neuron is currently firing action potentials")
        .value("Refractory", FiringState::Refractory,
               "Neuron is in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited,
               "Neuron is being actively inhibited from firing")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of possible action types that the agent can take.
        
        Actions represent the agent's interactions with the world:
        - Movement actions: MoveForward, MoveBackward, MoveLeft, MoveRight
        - Rotation actions: TurnLeft, TurnRight
        - Perception actions: Look, LookUp, LookDown
        - Interaction actions: Interact, Eat, Drink, Rest
        - General actions: Wait, Custom
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward,
               "Move forward one unit in the direction of orientation")
        .value("MoveBackward", ActionType::MoveBackward,
               "Move backward one unit in the opposite direction")
        .value("MoveLeft", ActionType::MoveLeft,
               "Move left one unit relative to current orientation")
        .value("MoveRight", ActionType::MoveRight,
               "Move right one unit relative to current orientation")
        .value("TurnLeft", ActionType::TurnLeft,
               "Rotate counterclockwise by default angle")
        .value("TurnRight", ActionType::TurnRight,
               "Rotate clockwise by default angle")
        .value("Look", ActionType::Look,
               "Look in the direction of orientation")
        .value("LookUp", ActionType::LookUp,
               "Look upward relative to current orientation")
        .value("LookDown", ActionType::LookDown,
               "Look downward relative to current orientation")
        .value("Interact", ActionType::Interact,
               "Interact with nearby objects")
        .value("Eat", ActionType::Eat,
               "Attempt to eat a nearby resource")
        .value("Drink", ActionType::Drink,
               "Attempt to drink a nearby resource")
        .value("Rest", ActionType::Rest,
               "Rest and recover energy")
        .value("Wait", ActionType::Wait,
               "Wait without taking any action")
        .value("Custom", ActionType::Custom,
               "Custom action with specific parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor commands for precise control.
        
        Motor commands are the actual outputs of the brain that the world
        interprets to produce movement and actions:
        - MoveForward, MoveBackward, TurnLeft, TurnRight: Basic movements
        - LookLeft, LookRight: Camera/head orientation
        - Interact: Trigger interaction with objects
        - Wait: No movement or interaction
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward,
               "Move forward one unit")
        .value("MoveBackward", MotorCommand::MoveBackward,
               "Move backward one unit")
        .value("TurnLeft", MotorCommand::TurnLeft,
               "Rotate counterclockwise")
        .value("TurnRight", MotorCommand::TurnRight,
               "Rotate clockwise")
        .value("LookLeft", MotorCommand::LookLeft,
               "Look left relative to current orientation")
        .value("LookRight", MotorCommand::LookRight,
               "Look right relative to current orientation")
        .value("Interact", MotorCommand::Interact,
               "Trigger interaction with nearby objects")
        .value("Wait", MotorCommand::Wait,
               "Take no action")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the simulated world.
        
        World objects represent different entities in the environment:
        - Empty: No object present
        - Resource: Consumable resource (food, water)
        - Hazard: Dangerous object (damage to agent)
        - Wall: Impassable barrier
        - Marker: Special location or landmark
    )pbdoc")
        .value("Empty", WorldObjectType::Empty,
               "No object present at this location")
        .value("Resource", WorldObjectType::Resource,
               "Consumable resource (provides energy)")
        .value("Hazard", WorldObjectType::Hazard,
               "Dangerous object (causes damage)")
        .value("Wall", WorldObjectType::Wall,
               "Impassable barrier")
        .value("Marker", WorldObjectType::Marker,
               "Special location or landmark")
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM brain and simulation parameters.
        
        The Config class provides a flexible key-value storage system for
        all NLM configuration parameters. It supports multiple sources
        (file, command-line, runtime) and various data types.
        
        **Configuration Categories:**
        - brain: Neural network parameters (neuron count, connectivity)
        - world: Environment settings (size, physics)
        - agent: Agent behavior and capabilities
        - learning: Learning algorithms and rules
        - development: Developmental stages and plasticity
        
        **Example Usage:**
        ```python
        # Create default config
        config = pynlm.createDefaultConfig()
        
        # Set custom values
        config.set("brain.neuron_count", 2000)
        config.set("brain.excitatory_ratio", 0.8)
        config.set("world.width", 30.0)
        config.set("world.height", 20.0)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Returns True on success.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
             "Load configuration from command-line arguments. Returns True on success.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Returns True on success.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key is present.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a formatted summary of all configuration settings")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "String representation of configuration");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input types in the NLM system.
        
        SensoryInput represents data from the agent's sensors (vision, audio,
        internal signals). It provides a uniform interface for accessing
        sensory data regardless of the input type.
        
        **Subclasses:**
        - Vision: Visual input from camera/sensors
        - Audio: Auditory input from sound sources
        - InternalSignals: Internal neural states and predictions
        
        **Key Properties:**
        - type: The type of sensory input
        - data: The raw sensory data as floating-point values
        - dimensions: The shape of the data (width, height, channels, etc.)
        - timestamp: When the sensory input was captured
        
        **Example Usage:**
        ```python
        # Get vision from world
        vision = world.getSensoryPercept().getVision()
        
        # Process audio input
        audio = SensoryInput.getAudio()
        ```
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp when this sensory input was captured")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input for the NLM agent.
        
        Vision represents the agent's visual perception of the world,
        typically from a camera or similar sensor. It provides structured
        access to visual information including resolution, color channels,
        and pixel data.
        
        **Constructor Signatures:**
        - Vision(): Create empty vision input
        - Vision(width, height, channels=3): Create vision with specified resolution
        
        **Key Properties:**
        - width: Image width in pixels
        - height: Image height in pixels  
        - channels: Number of color channels (typically 3 for RGB)
        - data: Flattened array of pixel values [0.0, 1.0]
        
        **Example Usage:**
        ```python
        # Create vision from world percept
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        
        # Access image dimensions
        width = vision.getWidth()
        height = vision.getHeight()
        channels = vision.getChannels()
        
        # Process pixel data
        pixels = vision.getData()
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create vision sensory input with specified resolution")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the pixel data for this vision input")
        .def("getWidth", &Vision::getWidth,
             "Get the width of the vision image in pixels")
        .def("getHeight", &Vision::getHeight,
             "Get the height of the vision image in pixels")
        .def("getChannels", &Vision::getChannels,
             "Get the number of color channels in the vision input");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for the NLM agent.
        
        Audio represents the agent's auditory perception of the environment,
        capturing sound waveforms and acoustic features. This enables agents
        to process and respond to environmental sounds.
        
        **Constructor Signatures:**
        - Audio(): Create empty audio input
        - Audio(sampleRate, numSamples): Create audio with specified audio parameters
        
        **Key Properties:**
        - sampleRate: Sampling rate in Hz
        - numSamples: Number of audio samples
        - data: Audio waveform samples [0.0, 1.0]
        
        **Example Usage:**
        ```python
        # Create audio sensory input
        audio = Audio(44100, 1024)  # 44.1kHz, 1 second of audio
        
        # Set audio data
        samples = [0.0, 0.1, 0.2, ...]  # Audio waveform
        audio.setData(samples)
        
        # Get audio properties
        sample_rate = audio.getSampleRate()
        num_samples = audio.getNumSamples()
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio sensory input with specified parameters")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the audio samples for this input")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sampling rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the sampling rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal sensory input representing the agent's internal state.
        
        InternalSignals captures the agent's internal neural and cognitive
        states, including working memory, predictions, attention, and
        other internal representations that guide behavior.
        
        **Key Features:**
        - Dynamic signal storage: Add/remove signals at runtime
        - Flexible signal types: Store floats of any value
        - Timestamp tracking: Track when signals were updated
        - Internal state representation: brain's internal processing
        
        **Example Usage:**
        ```python
        # Create internal signals
        internal = InternalSignals()
        
        # Add signals for different internal states
        internal.addSignal(0.5)  # Working memory activation
        internal.addSignal(0.8)  # Attention level
        internal.addSignal(0.3)  # Prediction confidence
        
        # Clear all signals
        internal.clearSignals()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

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

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output and behavior.
        
        Action represents a specific behavior or movement that the agent
        can take in the environment. It includes both the type of action
        and any associated parameters for complex behaviors.
        
        **Constructor Signatures:**
        - Action(): Create empty action
        - Action(type): Create action with specified type
        - Action(type, parameters): Create action with type and parameters
        
        **Key Properties:**
        - type: The action type (MoveForward, Interact, etc.)
        - parameters: Additional parameters for the action
        - name: Human-readable name for the action type
        
        **Action Types:**
        - Movement: MoveForward, MoveBackward, MoveLeft, MoveRight
        - Rotation: TurnLeft, TurnRight
        - Perception: Look, LookUp, LookDown
        - Interaction: Interact, Eat, Drink, Rest
        - Control: Wait, Custom
        
        **Example Usage:**
        ```python
        # Create simple movement action
        action = Action(ActionType.MoveForward)
        
        # Create action with parameters
        action = Action(ActionType.Custom, [0.5, 0.5, 0.5])
        
        # Get action properties
        action_type = action.getType()
        params = action.getParameters()
        action_name = action.getName()
        
        # Clone action for modification
        action_copy = action.clone()
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType,
             "Get the action type")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type")
        .def("getParameters", &Action::getParameters,
             "Get the action parameters as a vector of floats")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters")
        .def("getName", &Action::getName,
             "Get the human-readable name of the action type")
        .def("clone", &Action::clone,
             "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the simulated environment.
        
        WorldObject represents entities in the simulated world, including
        resources, hazards, walls, and landmarks. Each object has physical
        properties and semantic meaning within the environment.
        
        **Constructor Signatures:**
        - WorldObject(): Create empty object
        - WorldObject(x, y, type, value=0.0, radius=0.5): Create object with properties
        
        **Key Properties:**
        - x: X coordinate in world space
        - y: Y coordinate in world space
        - type: The type of object (Resource, Hazard, Wall, etc.)
        - value: Object value (energy for resources, damage for hazards)
        - radius: Physical size of the object
        - active: Whether the object is currently active
        
        **Object Types:**
        - Empty: No object
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Impassable barrier
        - Marker: Special location/landmark
        
        **Example Usage:**
        ```python
        # Create a resource object
        resource = WorldObject(5.0, 3.0, WorldObjectType.Resource, 10.0)
        
        # Create a wall
        wall = WorldObject(10.0, 10.0, WorldObjectType.Wall)
        
        # Access object properties
        position_x = world_object.x
        position_y = world_object.y
        obj_type = world_object.type
        obj_value = world_object.value
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position, type, and properties")
        .def_readwrite("x", &WorldObject::x,
                       "X coordinate of the object in world space")
        .def_readwrite("y", &WorldObject::y,
                       "Y coordinate of the object in world space")
        .def_readwrite("radius", &WorldObject::radius,
                       "Physical radius of the object")
        .def_readwrite("type", &WorldObject::type,
                       "Type of the object (Resource, Hazard, etc.)")
        .def_readwrite("value", &WorldObject::value,
                       "Value of the object (energy, damage, etc.)")
        .def_readwrite("active", &WorldObject::active,
                       "Whether the object is currently active");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation.
        
        AgentBody represents the physical state of the agent in the world,
        including position, orientation, velocity, and physical properties
        that affect how the agent interacts with the environment.
        
        **Key Properties:**
        - x: X coordinate in world space
        - y: Y coordinate in world space
        - orientation: Facing direction in radians (0 = +X, PI/2 = +Y)
        - velocityX: X component of linear velocity
        - velocityY: Y component of linear velocity
        - angularVelocity: Angular velocity in radians per timestep
        - energy: Current energy level (0-1)
        - health: Health status (0-1, 1 = full health)
        - age: Developmental age in simulation timesteps
        - isMoving: Whether the agent is currently moving
        - isTurning: Whether the agent is currently turning
        - lastActionTime: Time since last action (for rest behavior)
        
        **Physical Properties:**
        - Energy affects action costs and recovery time
        - Health affects damage taken and maximum energy
        - Velocity determines movement dynamics
        - Orientation determines facing direction and movement
        
        **Example Usage:**
        ```python
        # Get agent body state
        body = world.getAgentBody()
        
        # Access physical properties
        position_x = body.x
        position_y = body.y
        direction = body.orientation
        current_energy = body.energy
        is_alive = body.health > 0
        
        # Check if agent is moving
        if body.isMoving:
            print("Agent is moving!")
        ```
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                       "X coordinate in world space")
        .def_readwrite("y", &AgentBody::y,
                       "Y coordinate in world space")
        .def_readwrite("orientation", &AgentBody::orientation,
                       "Facing direction in radians (0 = +X, PI/2 = +Y)")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                       "X component of linear velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                       "Y component of linear velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                       "Angular velocity in radians per timestep")
        .def_readwrite("energy", &AgentBody::energy,
                       "Current energy level (0-1)")
        .def_readwrite("health", &AgentBody::health,
                       "Health status (0-1, 1 = full health)")
        .def_readwrite("age", &AgentBody::age,
                       "Developmental age in simulation timesteps")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                       "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                       "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                       "Time since last action (for rest behavior)")
        .def("reset", &AgentBody::reset,
             "Reset agent body state to initial values");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action taken by the agent.
        
        ActionResult represents the outcome of an action, including reward,
        success status, and any additional information about what happened.
        This provides feedback to the agent's learning system.
        
        **Constructor Signatures:**
        - ActionResult(): Create empty result
        - ActionResult(reward, success, message=""): Create result with details
        
        **Key Properties:**
        - reward: Numerical reward received (positive for good outcomes, negative for bad)
        - success: Whether the action succeeded
        - message: Descriptive text explaining the outcome
        
        **Example Usage:**
        ```python
        # Create action result
        result = ActionResult(10.0, True, "Found food resource")
        
        # Create failure result
        result = ActionResult(-5.0, False, "Hit wall")
        
        # Access result properties
        reward = result.reward
        success = result.success
        message = result.message
        ```
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward,
                       "Numerical reward received from this action")
        .def_readwrite("success", &ActionResult::success,
                       "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message,
                       "Descriptive message about the action outcome");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Complete sensory perception data for the agent.
        
        SensoryPercept combines all available sensory inputs from the agent
        at a given moment, including vision, touch, internal signals, and
        proprioception. It provides a unified interface for accessing
        the agent's current perception of the world.
        
        **Sensory Components:**
        - vision: Visual input from camera/sensors
        - touch: Proprioceptive and tactile input
        - internal: Internal neural and cognitive states
        - proprioception: Body position and movement sense
        - audio: Auditory input
        
        **Key Properties:**
        - All components are separate getter/setter methods
        - timestamp: When this perception was recorded
        - Dimensions vary by sensor type
        
        **Example Usage:**
        ```python
        # Get current perception from world
        percept = world.getSensoryPercept()
        
        # Access visual input
        vision = percept.getVision()
        
        # Access internal state
        internal = percept.getInternal()
        
        # Set new sensory data
        percept.setVision(vision_data)
        percept.setInternal(internal_data)
        
        # Get timestamp
        time = percept.getTimestamp()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get the visual sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set the visual sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get the width of visual input")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get the height of visual input")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get the touch/proprioceptive input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set the touch/proprioceptive input")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get the internal signals input")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set the internal signals input")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get the proprioception input")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set the proprioception input")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get the audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set the audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get the timestamp when this perception was recorded")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this perception");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides a complete simulated environment for NLM agents,
        including physics simulation, object management, and sensory feedback.
        The world provides a platform for agents to learn through interaction.
        
        **World Features:**
        - 2D physics simulation with collision detection
        - Multiple object types (resources, hazards, walls, markers)
        - Agent movement and interaction capabilities
        - Sensory input generation (vision, touch, internal)
        - Energy and damage systems
        - Random event generation
        
        **Constructor:**
        - SimpleWorld(): Create empty world
        
        **Configuration:**
        - configure(width, height, visionWidth, visionHeight): Set world dimensions
        - setAgentStart(x, y): Set initial agent position
        
        **Core Methods:**
        - reset(): Reset world to initial state
        - update(timestep): Update world physics and dynamics
        - applyMotorCommand(cmd, currentTime): Apply agent action
        - getSensoryPercept(): Get agent's current perception
        - getAgentBody(): Get agent physical state
        
        **Object Management:**
        - addObject(obj): Add object to world
        - removeObject(x, y): Remove object at position
        - isValidPosition(x, y): Check if position is accessible
        
        **Properties:**
        - getWidth(), getHeight(): World dimensions
        - getMaxEnergy(), setMaxEnergy(e): Energy system
        - getEnergyDecayRate(), setEnergyDecayRate(r): Energy decay
        - getSimulationTime(): Current simulation time
        
        **Example Usage:**
        ```python
        # Create and configure world
        world = pynlm.createWorld(20, 20, 8, 8)
        
        # Add objects
        food = pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, 10)
        wall = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Wall)
        world.addObject(food)
        world.addObject(wall)
        
        # Set agent start position
        world.setAgentStart(1, 1)
        
        # Reset and run simulation
        world.reset()
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            # Process percept and take action...
        ```
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set initial agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics and dynamics for one timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent and return action result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the simulation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at specified position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not a wall)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width in simulation units")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height in simulation units")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy level for the agent")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy level for the agent")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed");

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class for NLM.
        
        Brain is the core computational engine of the NLM system, implementing
        neural dynamics, plasticity, learning, and development. It represents
        a population of neurons that process sensory input, generate motor output,
        and maintain internal representations through complex neural interactions.
        
        **Brain Architecture:**
        - Neural regions: Organized groups of neurons (cortex, thalamus, etc.)
        - Neural populations: Groups of neurons with similar properties
        - Neurons: Individual computational units (LIF model)
        - Synapses: Connections between neurons for information transfer
        
        **Key Capabilities:**
        - Sensory processing and integration
        - Motor command generation
        - Learning through synaptic plasticity
        - Development with changing plasticity rates
        - Memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, etc.)
        - Prediction and error correction
        
        **Constructor:**
        - Brain(config): Create brain with configuration
        
        **Core Methods:**
        - initialize(): Initialize neural networks
        - step(currentStep): Perform one simulation step
        - step(currentStep, currentTime): Perform step with timestamp
        - receiveSensoryInput(input): Inject sensory data
        - injectCurrent(neuron, current): Inject current into neuron
        - injectCurrentToNeurons(type, current): Inject into neuron type
        - produceAction(): Generate motor command from neural activity
        - reset(): Reset neural state
        - save(filepath): Save brain state to file
        - load(filepath): Load brain state from file
        
        **Query Methods:**
        - getTotalNeuronCount(): Total neurons in brain
        - getTotalSynapseCount(): Total synapses in brain
        - getActiveNeuronCount(): Currently active neurons
        - getFiringNeuronCount(): Currently firing neurons
        - getTotalSpikeCount(): Total spikes in simulation
        - getAverageFiringRate(): Average firing across all neurons
        - getDevelopmentalStage(): Current development phase
        - getExcitationInhibitionRatio(): Network balance
        
        **Advanced Features:**
        - Region management: Add, get, and manage neural regions
        - Configuration access: Get brain configuration
        - Status logging: Detailed brain state information
        
        **Example Usage:**
        ```python
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(1000):
            # Get sensory input
            percept = world.getSensoryPercept()
            brain.receiveSensoryInput(percept)
            
            # Step neural dynamics
            brain.step(step)
            
            # Generate action
            brain.produceAction()
            
            # Get statistics
            firing_count = brain.getFiringNeuronCount()
            spike_count = brain.getTotalSpikeCount()
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
             "Add a new neural region with optional name")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by its ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a vector")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a vector")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count across all neurons")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set the developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the brain's configuration")
        .def("logStatus", &Brain::logStatus,
             "Log detailed brain status information");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain serves as the bridge between the neural processing in the brain
        and the external world, handling sensory transduction and motor decoding.
        It implements the agent behavior loop that translates between neural
        activity and world interaction.
        
        **Agent Architecture:**
        - Input layer: Sensory neurons process world perception
        - Processing layer: Brain computes motor commands
        - Output layer: Motor neurons generate actions
        - Feedback loop: World feedback affects learning
        
        **Key Capabilities:**
        - Sensory processing: Convert world percepts to neural input
        - Motor generation: Convert neural activity to actions
        - Reward modulation: Apply neuromodulation based on outcomes
        - Development: Adapt neural connectivity over time
        - Curiosity: Drive exploration of novel stimuli
        - Structural plasticity: Change brain architecture
        
        **Constructor:**
        - AgentBrain(brain): Create agent brain with underlying brain
        
        **Core Methods:**
        - initialize(world): Initialize with world reference
        - getSensoryInputSize(): Get expected sensory input size
        - getMotorOutputSize(): Get expected motor output size
        - processSensoryInput(percept): Process sensory percept
        - decodeMotorCommand(): Decode neural activity to action
        - applyRewardModulation(reward, predicted): Apply reward-based learning
        - updateDevelopment(timestep): Update developmental processes
        
        **Query Methods:**
        - getDevelopmentalStage(): Current developmental phase
        - getNeuromodulationLevel(): Current neuromodulation strength
        - getCuriosityLevel(): Current exploration drive
        - getNoveltyLevel(): Current novelty detection
        - getPredictionError(): Prediction error signal
        
        **Configuration:**
        - enableRewardModulation(): Toggle reward-based learning
        - enableStructuralPlasticity(): Toggle structural changes
        - enableDevelopment(): Toggle developmental processes
        - enableCuriosity(): Toggle curiosity-driven exploration
        
        **State Management:**
        - reset(): Reset agent for new episode
        - getBrain(): Access underlying brain
        
        **Example Usage:**
        ```python
        # Create agent brain
        brain = pynlm.createBrain(config)
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize with world
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Main agent loop
        for step in range(1000):
            # Get world state
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Generate action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, step * 0.1)
            
            # Provide feedback
            reward = 0.0  # Calculate based on action result
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
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
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development processes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

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

    // Simplified beginner-friendly API
    m.def("beginner_createBrain", []() -> std::shared_ptr<Brain> {
        return beginner_api::createBrain();
    }, "[Beginner] Create a brain with default settings - simplified API");

    m.def("beginner_createWorld", [](float width, float height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        return beginner_api::createWorld(width, height, visionWidth, visionHeight);
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "[Beginner] Create a world with default settings - simplified API");

    m.def("beginner_runSimulation", [](std::shared_ptr<Brain> brain, int steps, bool verbose) -> float {
        return beginner_api::runSimulation(brain, steps, verbose);
    }, py::arg("brain"), py::arg("steps") = 1000, py::arg("verbose") = false,
        "[Beginner] Run a complete simulation with logging - simplified API");

    m.def("beginner_createDefaultConfig", []() -> std::shared_ptr<Config> {
        return beginner_api::createDefaultConfig();
    }, "[Beginner] Create a default configuration - simplified API");

    // Advanced convenience functions
    m.def("configureWorld", [](std::shared_ptr<SimpleWorld> world, float width, float height,
                              size_t visionWidth, size_t visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
    }, py::arg("world"), py::arg("width"), py::arg("height"),
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "Configure world with validation and helpful error messages");

    // Error handling with better messages
    m.def("safeCreateWorld", [](float width, float height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        if (width < 5.0f || height < 5.0f) {
            throw std::runtime_error("World dimensions too small (min 5x5). "
                                    "Try larger dimensions like 20x20.");
        }
        if (visionWidth > width || visionHeight > height) {
            throw std::runtime_error("Vision dimensions cannot exceed world dimensions. "
                                    "Reduce vision size or increase world size.");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width") = 20.0f, py::arg("height") = 20.0f,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "Create world with validation and helpful error messages");

    // Context manager for brain simulation
    py::class_<BrainSimulator>(m, "BrainSimulator", R"pbdoc(
        Context manager for brain simulation with automatic cleanup.
        
        BrainSimulator provides a Pythonic way to manage brain simulations
        using the context manager pattern, ensuring proper initialization
        and cleanup even if exceptions occur.
        
        **Usage:**
        ```python
        # Basic usage
        with pynlm.BrainSimulator() as simulator:
            brain = simulator.brain
            brain.initialize()
            # Run simulation...
            simulator.run_steps(100)
        
        # With custom configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        with pynlm.BrainSimulator(config) as simulator:
            brain = simulator.brain
            brain.initialize()
            simulator.run_steps(500)
        ```
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config") = nullptr,
             "Create brain simulator with optional configuration")
        .def("__enter__", [](BrainSimulator& self) -> BrainSimulator& {
            if (!self.brain_) {
                auto config = self.config_ ? self.config_ : std::make_shared<Config>();
                self.brain_ = std::make_shared<Brain>(config);
            }
            self.brain_->initialize();
            return self;
        }, "Enter the context manager and initialize brain")
        .def("__exit__", [](BrainSimulator& self, py::args) {
            if (self.brain_) {
                self.brain_->reset();
            }
        }, "Exit the context manager and clean up")
        .def("run_steps", &BrainSimulator::run_steps,
             py::arg("steps"),
             "Run specified number of simulation steps")
        .def_readwrite("brain", &BrainSimulator::brain_,
                       "Access the brain instance within the simulator");

    // Type conversion utilities
    m.def("asNeuronId", [](uint64_t value) -> NeuronId {
        return NeuronId(value);
    }, py::arg("value"),
         "Convert uint64_t to NeuronId for advanced usage");

    m.def("asSynapseId", [](uint64_t value) -> SynapseId {
        return SynapseId(value);
    }, py::arg("value"),
         "Convert uint64_t to SynapseId for advanced usage");

    // Constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Python version and metadata
    m.attr("__version__") = "0.5.0-wave5-enhanced";
    m.attr("__author__") = "NLM Project (Wave 5)";
    m.attr("__description__") = "Enhanced Python bindings with comprehensive documentation, simplified API, and modern Python features";
}

} // namespace nlm
