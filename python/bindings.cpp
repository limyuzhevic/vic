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
        # NLM (Neural Learning Machine) Python Bindings
        
        ## Overview
        NLM is a comprehensive neural simulation framework that provides a realistic spiking
        neural network implementation with integrated cognitive systems. These Python bindings
        enable you to create, configure, and simulate neural brains in Python with access to
        advanced features like memory systems, prediction, development, and neuromodulation.
        
        ## Module Structure
        - **Core Neural Types**: NeuronId, SynapseId, RegionId, PopulationId
        - **Brain Simulation**: Brain class for neural computation
        - **Configuration**: Config class for system settings
        - **Sensory Systems**: Vision, Audio, InternalSignals for input processing
        - **Motor Systems**: Action, MotorCommand for output generation
        - **World Simulation**: SimpleWorld for environmental interaction
        - **Agent Integration**: AgentBrain connecting brain to world
        - **Factory Functions**: createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
        
        ## Quick Start Guide
        ```python
        import pynlm
        
        # Create a default configuration
        config = pynlm.createDefaultConfig()
        
        # Create a brain with the configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create a world for simulation
        world = pynlm.createSimpleWorld()
        
        # Create an agent brain to connect brain to world
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        ```
        
        ## Best Practices
        - Always initialize brains and agents before use
        - Configure worlds before adding agents
        - Use proper error handling for file I/O operations
        - Monitor brain statistics during simulation
        - Clean up resources properly when done
        
        ## Error Handling
        - Configuration errors raise RuntimeError with descriptive messages
        - Memory errors include details about allocation failures
        - File I/O errors include the file path and operation details
        - Validation errors specify the failing constraint and valid values
        - Neuromodulation errors provide context about the modulation operation
        
        ## Performance Tips
        - Pre-allocate memory for large simulations
        - Use batch operations where possible
        - Configure appropriate neuron and synapse counts
        - Monitor and adjust simulation time steps
        - Use the development system for efficient training
        
        ## Installation Instructions
        1. Build the NLM C++ library first
        2. Install pybind11 and required dependencies
        3. Build Python bindings: `python3 setup.py build_ext --inplace`
        4. Test installation with a simple example
        
        ## Basic Examples
        See the examples directory for beginner, intermediate, and advanced usage patterns.
        
        ---
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
            Unique identifier for a neuron in the neural network.
            
            NeuronId represents a specific neuron within the brain simulation and is used
            to reference individual neurons for operations like current injection, stimulation,
            or accessing neuron properties. Each neuron has a unique 64-bit identifier that
            allows efficient lookup and management within the neural system.
            
            Examples:
                ```python
                # Create a default neuron ID (invalid)
                neuron_id = NeuronId()
                
                # Create a neuron ID with specific value
                neuron_id = NeuronId(42)
                print(neuron_id.value)  # Output: 42
                print(neuron_id.index)  # Output: 42
                
                # Compare neuron IDs
                id1 = NeuronId(100)
                id2 = NeuronId(100)
                print(id1 == id2)  # Output: True
                ```
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
            Unique identifier for a synapse connecting two neurons.
            
            SynapseId represents a synaptic connection between neurons in the neural network.
            Synapses are the fundamental communication channels between neurons, carrying
            excitatory, inhibitory, or modulatory signals. Each synapse has a unique identifier
            that enables precise manipulation of synaptic strength and plasticity.
            
            Examples:
                ```python
                # Create a default synapse ID
                synapse_id = SynapseId()
                
                # Create a synapse ID with specific value
                synapse_id = SynapseId(123)
                print(synapse_id.value)  # Output: 123
                
                # Use in brain operations
                # brain.injectCurrentToNeurons(NeuronType.Excitatory, 1.0)
                ```
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
            
            RegionId represents a distinct anatomical or functional region of the brain.
            Different regions typically contain specialized types of neurons and are
            organized to perform specific cognitive functions. Regions enable modular
            organization of neural computation and support complex cognitive architectures.
            
            Examples:
                ```python
                # Create a default region ID
                region_id = RegionId()
                
                # Create a region ID for a specific region
                region_id = RegionId(1)
                
                # Add regions to brain
                # region_id = brain.addRegion("Visual Cortex")
                # motor_region = brain.addRegion("Motor Cortex")
                # visual_region = brain.addRegion("Sensory Cortex")
                ```
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
            Unique identifier for a neuron population within a brain region.
            
            PopulationId represents a group of neurons with similar properties or functions
            within a specific brain region. Populations enable efficient management of
            homogeneous neuron groups, such as all excitatory neurons in a region or
            neurons with specific computational roles. This abstraction simplifies neuron
            management in large-scale neural simulations.
            
            Examples:
                ```python
                # Create a default population ID
                pop_id = PopulationId()
                
                # Create a population ID for a specific neuron group
                pop_id = PopulationId(42)
                
                # Create populations in brain regions
                # region = brain.addRegion("Cortex")
                # excitatory_pop = brain.addPopulation(region, "Excitatory")
                ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
            Enumeration of neuron types in the neural network.
            
            Neuron types categorize neurons based on their functional properties and
            connectivity patterns. Each type has distinct computational roles and
            is associated with specific learning rules and neuromodulation effects.
            
            NeuronType Values:
                - Excitatory: Promotes firing in connected neurons (positive synaptic weight)
                - Inhibitory: Reduces firing in connected neurons (negative synaptic weight)
                - Modulatory: Releases neuromodulators affecting multiple neurons
                - Sensory: Converts sensory input to neural activity
                - Motor: Initiates motor actions based on neural computation
                - Internal: Maintains homeostasis and internal state
            
            Examples:
                ```python
                # Access neuron types
                neuron_type = NeuronType.Excitatory
                print(neuron_type)  # Output: NeuronType.Excitatory
                
                # Inject current to specific neuron types
                # brain.injectCurrentToNeurons(NeuronType.Inhibitory, 0.5)
                ```
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Promotes firing in connected neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Reduces firing in connected neurons")
        .value("Modulatory", NeuronType::Modulatory, "Releases neuromodulators affecting multiple neurons")
        .value("Sensory", NeuronType::Sensory, "Converts sensory input to neural activity")
        .value("Motor", NeuronType::Motor, "Initiates motor actions based on neural computation")
        .value("Internal", NeuronType::Internal, "Maintains homeostasis and internal state")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
            Enumeration of synapse connection types in the neural network.
            
            Synapse types define the nature of connections between neurons, determining
            the communication mechanism and functional properties of synaptic transmission.
            Different synapse types support different plasticity rules and neuromodulation
            effects, enabling diverse computational capabilities in the neural system.
            
            SynapseType Values:
                - Excitatory: Increases postsynaptic neuron firing probability
                - Inhibitory: Decreases postsynaptic neuron firing probability
                - Modulatory: Releases neuromodulators affecting multiple neurons
                - Electrical: Direct electrical coupling between neurons
                - GapJunction: Electrical synapse via gap junctions
            
            Examples:
                ```python
                # Access synapse types
                synapse_type = SynapseType.Excitatory
                print(synapse_type)  # Output: SynapseType.Excitatory
                
                # Create different synapse types
                # excitatory_synapse = SynapseType.Excitatory
                # inhibitory_synapse = SynapseType.Inhibitory
                ```
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Increases postsynaptic neuron firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Decreases postsynaptic neuron firing")
        .value("Modulatory", SynapseType::Modulatory, "Releases neuromodulators")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Electrical synapse via gap junctions")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
            Enumeration of brain developmental stages.
            
            Developmental stages describe the maturation process of the neural system,
            from initial formation through critical learning periods to adult stability
            and aging. Each stage is associated with specific plasticity mechanisms and
            learning capabilities.
            
            DevelopmentalStage Values:
                - Initial: Initial formation and basic wiring
                - CriticalPeriod: High plasticity for rapid learning
                - Maturation: Fine-tuning and stabilization
                - Adult: Stable adult function and performance
                - Aging: Age-related changes and decline
            
            Examples:
                ```python
                # Access developmental stages
                stage = DevelopmentalStage.CriticalPeriod
                print(stage)  # Output: DevelopmentalStage.CriticalPeriod
                
                # Set developmental stage
                # brain.setDevelopmentalStage(DevelopmentalStage.CriticalPeriod)
                ```
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

py::class_<Config>(m, "Config", R"pbdoc(
            Configuration class for NLM system settings and parameters.
            
            Config provides a flexible key-value storage system for managing neural
            network parameters, simulation settings, and behavioral controls. It
            enables customization of brain architecture, simulation dynamics, learning
            mechanisms, and environmental interaction. Configuration can be loaded from
            JSON files, command-line arguments, or created programmatically.
            
            Configuration Categories:
                - Brain Architecture: neuron counts, region definitions, connectivity
                - Simulation Dynamics: time steps, thresholds, plasticity rules
                - Memory Systems: memory capacity, learning rates
                - Neuromodulation: reward signals, curiosity, prediction error
                - Development: developmental stages, plasticity windows
                - Environment: world dimensions, agent properties
            
            Examples:
                ```python
                # Create default configuration
                config = pynlm.createDefaultConfig()
                
                # Load from JSON file
                config = Config()
                config.loadFromFile("my_config.json")
                
                # Set configuration values
                config.set("brain.neuron_counts.excitatory", 1000)
                config.set("simulation.time_step", 0.01)
                
                # Create brain with configuration
                brain = pynlm.createBrain(config)
                ```
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises RuntimeError if file cannot be read or parsed.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Returns True if successful, False otherwise.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises RuntimeError if file cannot be written.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Returns True if key exists, False otherwise.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list of strings. Returns keys for all configured parameters.")
        .def("clear", &Config::clear,
             "Clear all configuration entries. Use with caution as this will reset all settings.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration. Useful for debugging and logging.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
            Base class for sensory input in the neural system.
            
            SensoryInput represents external or internal signals that the brain receives
            and processes to understand the environment and generate appropriate actions.
            Sensory inputs are converted into neural representations that drive perception,
            decision-making, and behavior. Each sensory modality provides different types
            of information with varying temporal and spatial characteristics.
            
            Sensory Input Categories:
                - Vision: Visual information from cameras or simulated vision
                - Audio: Sound information from audio sensors
                - Internal: Internal body state signals (proprioception, interoception)
                - Touch: Physical contact and pressure information
            
            Examples:
                ```python
                # Create vision input
                vision = Vision(64, 64, 3)
                vision.setData([...])  # 64x64 RGB image data
                
                # Create audio input
                audio = Audio(44100, 44100)  # 1 second of audio at 44.1kHz
                audio.setData([...])  # Audio samples
                
                # Create internal signals
                internal = InternalSignals()
                internal.addSignal(0.5)  # Add a signal value
                internal.addSignal(0.8)  # Add another signal
                ```
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input as string")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp when data was captured")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for when this sensory input was generated");

py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
            Vision sensory input for visual information processing.
            
            Vision represents visual data such as images or video frames that are
            processed by the neural system for object recognition, scene understanding,
            and navigation. Visual information is typically high-dimensional and requires
            preprocessing (e.g., feature extraction, normalization) before being injected
            into the neural network.
            
            Vision System Parameters:
                - width: Horizontal resolution in pixels
                - height: Vertical resolution in pixels
                - channels: Color channels (1=grayscale, 3=RGB, 4=RGBA)
            
            Visual Processing Pipeline:
                1. Raw image capture → 2. Feature extraction → 3. Neural representation
            
            Examples:
                ```python
                # Create a grayscale vision sensor (28x28, common for MNIST)
                vision = Vision(28, 28, 1)
                # vision.setData(list_of_784_floats)  # Flatten 28x28 image
                
                # Create an RGB vision sensor (e.g., for CIFAR-10)
                vision = Vision(32, 32, 3)
                # vision.setData(list_of_3072_floats)  # 32x32x3 RGB values
                
                # Access vision properties
                print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
                ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the visual data as a flattened vector of pixel values")
        .def("getWidth", &Vision::getWidth, "Get the width of the visual field in pixels")
        .def("getHeight", &Vision::getHeight, "Get the height of the visual field in pixels")
        .def("getChannels", &Vision::getChannels, "Get the number of color channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
            Audio sensory input for sound information processing.
            
            Audio represents sound data such as music, speech, or environmental sounds
            that are processed by the neural system for auditory scene analysis, speech
            recognition, and music processing. Audio information is time-series data that
            requires temporal processing and often involves spectral analysis.
            
            Audio System Parameters:
                - sampleRate: Sampling frequency in Hertz (Hz)
                - numSamples: Number of audio samples in the buffer
            
            Audio Processing Pipeline:
                1. Raw audio capture → 2. Feature extraction → 3. Neural representation
            
            Examples:
                ```python
                # Create mono audio at standard CD quality
                audio = Audio(44100, 44100 * 2)  # 2 seconds of audio
                # audio.setData(list_of_88200_floats)  # 2 seconds @ 44.1kHz
                
                # Create stereo audio for music processing
                audio = Audio(22050, 44100 * 5)  # 5 seconds of audio
                # audio.setData(list_of_44100_floats)  # 5 seconds @ 22.1kHz
                
                # Adjust sample rate for speech processing
                audio.setSampleRate(16000)  # Lower rate for speech recognition
                print(f"Audio duration: {audio.getNumSamples() / audio.getSampleRate():.2f} seconds")
                ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data as a vector of sound samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sampling rate in Hertz (Hz)")
        .def("getSampleRate", &Audio::getSampleRate, "Get the sampling rate in Hertz (Hz)")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
            Internal signals sensory input for body state and interoception.
            
            InternalSignals represents internal physiological and cognitive states
            such as hunger, fatigue, emotional states, proprioception, and other
            homeostatic signals that influence behavior and decision-making. These
            internal signals are crucial for survival motivation and adaptive behavior.
            
            Internal Signal Categories:
                - Physiological: hunger, thirst, fatigue, energy levels
                - Cognitive: attention, confidence, surprise
                - Proprioceptive: body position, movement awareness
                - Interoceptive: visceral sensations, emotional states
            
            Integration with External World:
                Internal signals modulate external behavior and learning processes,
                creating adaptive responses to environmental demands.
            
            Examples:
                ```python
                # Create internal signals processor
                internal = InternalSignals()
                
                # Add various internal states
                internal.addSignal(0.8)  # High hunger level (0.0-1.0)
                internal.addSignal(0.3)  # Low energy level
                internal.addSignal(0.6)  # Moderate attention level
                
                # Clear all internal signals (e.g., after eating)
                internal.clearSignals()
                
                # Access internal signals through agent brain
                # percept = agent.getInternalSignals()
                ```
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value (typically 0.0-1.0 scale). Use values between 0.0 (low) and 1.0 (high).")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals. Use this when the underlying physiological state changes (e.g., after eating or resting).");

    py::class_<Action>(m, "Action", R"pbdoc(
            Action representation for motor output from the brain.
            
            Action represents the output of the neural system, specifying what motor
            command the agent should execute in the environment. Actions are produced
            by the brain's motor system based on neural activity patterns and are
            executed by the agent's body in the world. Each action includes a type
            (e.g., move forward, turn left) and optional parameters for fine control.
            
            Action Types:
                - Movement: MoveForward, MoveBackward, MoveLeft, MoveRight
                - Rotation: TurnLeft, TurnRight
                - Sensing: Look, LookUp, LookDown
                - Interaction: Interact, Eat, Drink
                - State: Rest, Wait, Custom
            
            Action Parameters:
                - Continuous: Speed, direction, intensity
                - Discrete: Binary flags, categorical values
                - Custom: User-defined parameter sets
            
            Action Execution:
                1. Brain produces action based on neural computation
                2. AgentBrain decodes neural activity into motor command
                3. SimpleWorld applies motor command to agent body
                4. Agent executes physical action in the world
            
            Examples:
                ```python
                # Create a simple move action
                action = Action(ActionType.MoveForward)
                
                # Create a move action with parameters
                action = Action(ActionType.MoveForward, [0.8])  # Speed 0.8
                
                # Access action properties
                print(f"Action type: {action.getType()}")
                print(f"Action parameters: {action.getParameters()}")
                print(f"Action name: {action.getName()}")
                
                # Clone an action for modification
                new_action = action.clone()
                ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type (ActionType)")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters as a list of floats")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable name of the action type")
        .def("clone", &Action::clone, "Create a copy of this action. Useful for action modification and replay.");

py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
            World object representation in the 2D simulation environment.
            
            WorldObject represents entities that exist in the simulated world, including
            both static obstacles (walls, hazards) and dynamic resources (food, items).
            World objects define the spatial structure and resource landscape that
            the agent interacts with. Each object has a position, type, and properties
            that determine how the agent can interact with it.
            
            World Object Types:
                - Empty: Empty space, no interaction
                - Resource: Consumable or collectible item
                - Hazard: Dangerous object that damages the agent
                - Wall: Static obstacle that cannot be passed
                - Marker: Location marker for navigation
            
            Object Properties:
                - position: (x, y) coordinates in world space
                - radius: Interaction/interaction radius
                - value: Resource value or hazard intensity
                - active: Whether the object is currently active
            
            World Interaction:
                - Resources can be collected by the agent
                - Hazards damage the agent's health
                - Walls block agent movement
                - Markers provide navigation targets
            
            Examples:
                ```python
                # Create a resource item
                resource = WorldObject(5.0, 3.0, WorldObjectType.Resource, 10.0)
                print(f"Resource at ({resource.x}, {resource.y}) with value {resource.value}")
                
                # Create a wall
                wall = WorldObject(2.0, 2.0, WorldObjectType.Wall)
                wall.radius = 0.5
                world.addObject(wall)
                
                # Create a hazard
                hazard = WorldObject(8.0, 8.0, WorldObjectType.Hazard, 5.0)
                hazard.active = True
                
                # Toggle object state
                resource.active = False  # No longer collectible
                ```
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate in world space")
        .def_readwrite("y", &WorldObject::y, "Y coordinate in world space")
        .def_readwrite("radius", &WorldObject::radius, "Interaction radius (collision detection)")
        .def_readwrite("type", &WorldObject::type, "Object type (WorldObjectType)")
        .def_readwrite("value", &WorldObject::value, "Object value or intensity (resource amount, hazard strength)")
        .def_readwrite("active", &WorldObject::active, "Whether the object is currently active (can be collected/damaged)");

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
