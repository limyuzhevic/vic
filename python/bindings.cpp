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
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides Python access to the NLM neural simulation framework, enabling
        creation and simulation of artificial brains that can learn, remember, and interact
        with dynamic environments.
        
        Key features:
        - Real-time spiking neural network simulation
        - Multiple learning mechanisms (STDP, Hebbian, structural plasticity)
        - Neuromodulation (dopamine, curiosity, novelty detection)
        - Memory systems (working, episodic, associative memory)
        - Prediction and planning capabilities
        - Developmental processes and maturation
        - Embodied agent interaction with virtual environments
        
        For detailed documentation and examples, see:
        - easy_usage.md: Quick start guide for beginners
        - HOW_TO_USE.md: Comprehensive API documentation
        - docs/ARCHITECTURE.md: System architecture overview
        
        Installation: pip install pynlm
        Building from source: See pyproject.toml for build instructions
        
        Examples are available in the tests/ directory and in the documentation.
    )pbdoc";

    // Register standard exceptions for better error handling
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::domain_error>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);
    py::register_exception<std::logic_error>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::runtime_error>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ConfigError", PyExc_ValueError);
    py::register_exception<std::runtime_error>(m, "InitializationError", PyExc_RuntimeError);
    
    // Add error messages for better debugging
    auto set_error_message = [](py::object& exc, const std::string& msg) {
        exc.attr("args") = py::make_tuple(msg);
    };
    
    // Helper function to wrap C++ exceptions with Python-friendly messages
    auto wrap_cpp_exception = [](std::function<void()> cpp_func) {
        try {
            cpp_func();
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
            throw py::error_already_set();
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
            throw py::error_already_set();
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
            throw py::error_already_set();
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
            throw py::error_already_set();
        }
    };

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
            Unique identifier for a neuron. NeuronId represents an ID for neurons, synapses, regions, and populations.
            
            Each NeuronId contains a unique value that identifies a specific neural element.
            The index() method provides a human-readable index representation.
            
            Example usage:
                neuron_id = pynlm.NeuronId(42)
                index = neuron_id.index()
                print(neuron_id)  # <NeuronId: 42>
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
            Unique identifier for a synapse. SynapseId represents an ID for synaptic connections between neurons.
            
            Each SynapseId contains a unique value that identifies a specific synapse.
            The index() method provides a human-readable index representation.
            
            Example usage:
                synapse_id = pynlm.SynapseId(123)
                index = synapse_id.index()
                print(synapse_id)  # <SynapseId: 123>
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
            Unique identifier for a brain region. RegionId represents an ID for neural regions that contain populations of neurons.
            
            Each RegionId contains a unique value that identifies a specific brain region.
            The index() method provides a human-readable index representation.
            
            Example usage:
                region_id = pynlm.RegionId(1)
                index = region_id.index()
                print(region_id)  # <RegionId: 1>
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
            Unique identifier for a neuron population. PopulationId represents an ID for groups of neurons
            that share similar properties or functions (e.g., excitatory neurons, sensory neurons).
            
            Each PopulationId contains a unique value that identifies a specific neuron population.
            The index() method provides a human-readable index representation.
            
            Example usage:
                population_id = pynlm.PopulationId(5)
                index = population_id.index()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
            Enumeration of neuron types in the NLM brain simulation.
            
            Different neuron types have distinct functions and characteristics:
            - Excitatory: Releases excitatory neurotransmitters, increases firing probability
            - Inhibitory: Releases inhibitory neurotransmitters, decreases firing probability  
            - Modulatory: Releases neuromodulators, regulates overall brain state
            - Sensory: Detects external/internal stimuli, processes sensory input
            - Motor: Controls actions and movements, sends motor commands
            - Internal: Processes internal signals, regulates homeostasis
            
            Example usage:
                neuron_type = pynlm.NeuronType.Excitatory
                print(neuron_type)  # NeuronType.Excitatory
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
            Enumeration of synapse types in the NLM brain simulation.
            
            Different synapse types have distinct transmission mechanisms:
            - Excitatory: Releases excitatory neurotransmitters, increases postsynaptic firing
            - Inhibitory: Releases inhibitory neurotransmitters, decreases postsynaptic firing
            - Modulatory: Releases neuromodulators, regulates synaptic plasticity
            - Electrical: Direct electrical coupling through gap junctions
            - GapJunction: Specialized electrical synapses for synchronized activity
            
            Example usage:
                synapse_type = pynlm.SynapseType.Excitatory
                print(synapse_type)  # SynapseType.Excitatory
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
            Enumeration of developmental stages for brain maturation and plasticity.
            
            Each stage represents a different phase of brain development with specific characteristics:
            - Initial: Brain is at birth/initial state, high plasticity, basic connectivity
            - CriticalPeriod: Sensitive period for learning, rapid synaptic changes
            - Maturation: Continued refinement, strengthening of important connections
            - Adult: Stable brain state, balanced plasticity and stability
            - Aging: Declining plasticity, age-related changes in neural dynamics
            
            Example usage:
                stage = pynlm.DevelopmentalStage.Adult
                print(stage)  # DevelopmentalStage.Adult
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
            Enumeration of action types for agent behavior in the NLM simulation.
            
            Action types represent possible behaviors an agent can perform:
            - MoveForward: Move forward in the direction of orientation
            - MoveBackward: Move backward opposite to orientation
            - MoveLeft: Move left relative to orientation
            - MoveRight: Move right relative to orientation
            - TurnLeft: Rotate counterclockwise
            - TurnRight: Rotate clockwise
            - Look: Look in a specific direction
            - LookUp: Look upward
            - LookDown: Look downward
            - Interact: Interact with nearby objects
            - Eat: Consume resources
            - Drink: Consume water
            - Rest: Stop moving and conserve energy
            - Wait: Stay in place for specified time
            - Custom: User-defined action with parameters
            
            Example usage:
                action = pynlm.ActionType.MoveForward
                print(action)  # ActionType.MoveForward
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
            Enumeration of motor commands for controlling agent movement.
            
            Motor commands are low-level commands that translate neural activity into physical actions:
            - MoveForward: Move forward in current direction
            - MoveBackward: Move backward opposite to direction
            - TurnLeft: Rotate counterclockwise
            - TurnRight: Rotate clockwise
            - LookLeft: Turn head/view left
            - LookRight: Turn head/view right
            - Interact: Attempt to interact with nearby objects
            - Wait: No movement, stay in place
            
            Example usage:
                command = pynlm.MotorCommand.MoveForward
                print(command)  # MotorCommand.MoveForward
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
            Enumeration of world object types in the NLM simulation environment.
            
            Object types represent different kinds of entities in the simulation world:
            - Empty: No object, empty space
            - Resource: Valuable item that can be consumed (food, water, etc.)
            - Hazard: Dangerous item that causes damage (predators, traps, etc.)
            - Wall: Impassable barrier that blocks movement
            - Marker: Special location with significance (goal, spawn point, etc.)
            
            Example usage:
                obj_type = pynlm.WorldObjectType.Resource
                print(obj_type)  # WorldObjectType.Resource
        )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(
            Configuration class for NLM system. The Config class manages simulation parameters
            and settings for the neural brain simulation.
            
            Config provides a key-value store for simulation parameters that control:
            - Brain configuration (neuron count, connectivity, plasticity)
            - Simulation parameters (timestep, random seed, development)
            - Neuromodulation settings (dopamine, curiosity, novelty)
            - Environment configuration (world size, physics)
            - Performance and logging settings
            
            Configuration can be loaded from files, command line arguments, or created programmatically.
            
            Example usage:
                # Create default configuration
                config = pynlm.createDefaultConfig()
                
                # Modify configuration
                config.set("brain.neuron_count", 2000)
                config.set("plasticity.stdp.enable", True)
                
                # Create brain with custom config
                brain = pynlm.createBrain(config)
                
                # Save configuration
                config.saveToFile("my_config.json")
            
            Available configuration keys include:
            - brain.neuron_count, brain.region_count, brain.connection_probability
            - plasticity.stdp.enable, plasticity.stdp.learning_rate
            - neuromod.dopamine.scale, neuromod.curiosity.enable
            - simulation.timestep, random_seed, etc.
        )pbdoc")
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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
            Base class for sensory input in the NLM brain simulation.
            
            SensoryInput represents data from the environment that the brain processes:
            - Vision: Visual input from the agent's camera/sensors
            - Audio: Sound input from the environment
            - Internal: Internal physiological signals
            - Proprioception: Body position and movement information
            
            The sensory system provides the brain with information about the external
            and internal world, enabling the agent to perceive and respond to its environment.
            
            Methods:
            - getType(): Returns the type of sensory input
            - getData(): Returns the raw sensor data as a vector
            - getDimensions(): Returns the dimensionality of the data
            - getTimestamp(): Returns when the input was received
            - setTimestamp(): Sets the timestamp for the sensory data
            
            Subclasses include Vision, Audio, and InternalSignals.
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
            Vision sensory input representing visual information from the agent's sensors.
            
            Vision data is typically a 2D grid of values representing:
            - Light intensity at different positions
            - Color information (if RGB channels)
            - Depth information (if available)
            - Object detection features
            
            The vision system enables the agent to see its environment and detect objects,
            obstacles, resources, and other agents.
            
            Attributes:
            - width: Number of pixels horizontally
            - height: Number of pixels vertically  
            - channels: Number of color channels (typically 1 for grayscale, 3 for RGB)
            
            Methods:
            - setData(data): Set the vision data as a flattened vector
            - getWidth(): Get image width in pixels
            - getHeight(): Get image height in pixels
            - getChannels(): Get number of color channels
            
            Example usage:
                # Create vision input with 8x8 grayscale image
                vision = pynlm.Vision(8, 8, 1)
                vision.setData([0.1, 0.2, 0.3, ...])  # 64 values
                width = vision.getWidth()  # 8
                height = vision.getHeight()  # 8
            
            Note: Vision data is typically normalized between 0.0 (dark) and 1.0 (bright)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
            Audio sensory input representing sound information from the NLM simulation environment.
            
            Audio data represents acoustic signals that the agent can hear:
            - Sound pressure levels from different sources
            - Frequency spectrum information
            - Spatial audio cues for sound localization
            - Environmental acoustics
            
            The audio system enables the agent to detect and respond to sound sources,
            which can indicate the presence of other agents, hazards, or resources.
            
            Attributes:
            - sampleRate: Audio sampling rate in Hz
            - numSamples: Number of audio samples in the recording
            
            Methods:
            - setData(data): Set audio data as a vector of sample values
            - setSampleRate(rate): Set the audio sampling rate
            - getSampleRate(): Get the audio sampling rate in Hz
            - getNumSamples(): Get the number of audio samples
            
            Example usage:
                # Create audio input with 44.1kHz sample rate and 1 second of audio
                audio = pynlm.Audio(44100, 44100)
                audio.setSampleRate(44100)
                audio.setData([0.1, 0.2, -0.1, ...])  # 44100 samples
                sample_rate = audio.getSampleRate()  # 44100
                samples = audio.getNumSamples()  # 44100
            
            Note: Audio data is typically normalized between -1.0 (minimum) and 1.0 (maximum)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
            Internal signals sensory input representing the agent's internal physiological state.
            
            Internal signals represent the agent's internal condition and homeostatic needs:
            - Hunger/satiety levels
            - Thirst/hydration levels
            - Energy levels
            - Health status
            - Temperature
            - Other metabolic signals
            
            The internal signals system allows the agent to monitor and regulate its
            physiological needs, influencing motivation and behavior.
            
            Methods:
            - addSignal(value): Add a new internal signal value
            - clearSignals(): Remove all internal signals
            
            Example usage:
                # Create internal signals for an agent
                internal = pynlm.InternalSignals()
                internal.addSignal(0.8)  # High energy level
                internal.addSignal(0.3)  # Low hunger
                internal.addSignal(0.9)  # High satiety
                
                signals = internal.getSignals()  # Returns [0.8, 0.3, 0.9]
                internal.clearSignals()  # Remove all signals
            
            Note: Internal signal values are typically normalized between 0.0 (low) and 1.0 (high)
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(
            Action representation for motor output in the NLM brain simulation.
            
            An Action represents a potential behavior that an agent can execute:
            - Specifies what to do (type)
            - Provides parameters for the action (e.g., movement speed, direction)
            - Includes metadata for execution and effects
            
            Actions bridge the gap between neural activity and physical behavior,
            allowing the brain to control an agent's interactions with the environment.
            
            Constructors:
            - Action(): Default constructor, creates empty action
            - Action(type): Create action with specified type
            - Action(type, parameters): Create action with type and parameters
            
            Attributes:
            - type: The ActionType specifying what to do
            - parameters: Vector of float parameters for action execution
            
            Methods:
            - getType(): Get the action type
            - setType(type): Set the action type
            - getParameters(): Get the action parameters vector
            - setParameters(params): Set the action parameters
            - getName(): Get human-readable name of the action
            - clone(): Create a copy of this action
            
            Example usage:
                # Create a simple movement action
                move_action = pynlm.Action(pynlm.ActionType.MoveForward)
                
                # Create an action with parameters
                custom_action = pynlm.Action(
                    pynlm.ActionType.Custom, 
                    [1.0, 0.5, 0.0]  # direction, speed, rotation
                )
                
                # Modify action
                move_action.setType(pynlm.ActionType.TurnLeft)
                move_action.setParameters([0.5])  # turn angle
                
                # Get action info
                action_name = move_action.getName()  # "TurnLeft"
                params = move_action.getParameters()
            
            Common Action types include movement, interaction, and consumption actions.
            Custom actions allow for user-defined behaviors with specific parameters.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
            World object representation in the NLM simulation environment.
            
            WorldObject represents entities in the simulation world that the agent can interact with:
            - Resources: Collectible items that provide value (food, water, energy)
            - Hazards: Dangerous items that cause damage (poison, traps, predators)
            - Walls: Barriers that block movement (boundaries, obstacles)
            - Markers: Special locations with significance (goals, spawn points)
            
            Objects have physical properties and can be added to/remove from the world,
            allowing the agent to collect resources, avoid hazards, navigate around walls,
            and reach goal markers.
            
            Attributes:
            - x: X coordinate of object center (meters)
            - y: Y coordinate of object center (meters)
            - radius: Physical radius of object (meters)
            - type: Type of object (Resource, Hazard, Wall, etc.)
            - value: Numerical value of object (energy, danger level, etc.)
            - active: Whether the object is currently present in the world
            
            Example usage:
                # Create a resource object at position (5.0, 3.0) with value 100
                resource = pynlm.WorldObject(5.0, 3.0, pynlm.WorldObjectType.Resource, value=100)
                resource.radius = 0.5  # 1 meter diameter
                
                # Create a hazard object
                hazard = pynlm.WorldObject(2.0, 2.0, pynlm.WorldObjectType.Hazard, value=50)
                hazard.radius = 0.3
                
                # Create a wall
                wall = pynlm.WorldObject(10.0, 0.0, pynlm.WorldObjectType.Wall)
                wall.radius = 0.1  # Thin wall
                
                # Add objects to world
                world.addObject(resource)
                world.addObject(hazard)
                world.addObject(wall)
                
                # Check position
                is_valid = world.isValidPosition(5.5, 3.5)  # True (not in object)
                is_valid = world.isValidPosition(5.0, 3.0)  # False (in resource)
            
            Object values:
            - Resource: Positive value (energy, nutrition, etc.)
            - Hazard: Positive value (danger level, damage amount)
            - Wall: Typically 0.0 or -1.0 (impassable)
            - Marker: Special value indicating goal or importance
        )pbdoc")
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

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
            Agent body state representing the physical agent in the NLM simulation.
            
            AgentBody tracks the agent's physical properties and state:
            - Position: X, Y coordinates in the world
            - Orientation: Direction the agent is facing (radians)
            - Velocity: Linear speed and direction
            - Angular motion: Rotational speed
            - Physiological state: Energy, health, age
            - Activity state: Whether moving, turning, etc.
            - Time tracking: Last action time
            
            The agent body represents the physical embodiment of the agent in the world,
            allowing interaction with the environment and other agents.
            
            Attributes:
            - x: X coordinate in world space (meters)
            - y: Y coordinate in world space (meters)
            - orientation: Facing direction in radians (0 = +X axis)
            - velocityX: Linear velocity in X direction (m/s)
            - velocityY: Linear velocity in Y direction (m/s)
            - angularVelocity: Rotational speed (rad/s)
            - energy: Current energy level (0.0 = empty, 1.0 = full)
            - health: Health status (1.0 = healthy, 0.0 = dead)
            - age: Age in simulation steps or seconds
            - isMoving: Whether agent is currently moving
            - isTurning: Whether agent is currently turning
            - lastActionTime: Time of last action execution
            
            Methods:
            - reset(): Reset body to initial state
            
            Example usage:
                # Create agent body
                body = pynlm.AgentBody()
                body.x = 0.0
                body.y = 0.0
                body.orientation = 0.0  # Facing +X direction
                body.energy = 1.0
                body.health = 1.0
                
                # Move agent
                body.x = 1.0
                body.y = 2.0
                body.velocityX = 0.5
                body.velocityY = 0.3
                
                # Check state
                is_moving = body.isMoving
                current_energy = body.energy
                facing_direction = body.orientation
                
                # Reset body
                body.reset()
            
            Note: Body coordinates are in world units (typically meters)
            Orientation is in radians, with 0.0 pointing along positive X axis
            Energy and health are normalized between 0.0 and 1.0
        )pbdoc")
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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
            Action result from world in the NLM simulation.
            
            ActionResult represents the outcome of an agent's action:
            - Reward: Value received from the environment
            - Success: Whether the action was successful
            - Message: Descriptive text about the action result
            - Additional metadata for complex actions
            
            Action results provide feedback to the agent about the consequences of its
            behavior, enabling learning and reinforcement.
            
            Attributes:
            - reward: Numerical reward value (positive = good, negative = bad)
            - success: Boolean indicating if action completed successfully
            - message: Descriptive string explaining the result
            
            Constructors:
            - ActionResult(): Default constructor
            - ActionResult(reward, success, message): Full constructor
            
            Example usage:
                # Create a successful action with reward
                result = pynlm.ActionResult(10.0, True, "Collected food")
                print(result.reward)  # 10.0
                print(result.success)  # True
                print(result.message)  # "Collected food"
                
                # Create a failed action
                failed_result = pynlm.ActionResult(-5.0, False, "Hit wall")
                print(failed_result.reward)  # -5.0
                print(failed_result.success)  # False
            
            Note: Reward values should be meaningful for the agent's reinforcement learning
            system. Positive rewards encourage repetition of behavior, negative rewards
            discourage it.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
            Sensory percept data representing what the agent perceives from the environment.
            
            SensoryPercept combines multiple sensory modalities into a unified perception:
            - Vision: Visual input from agent's sensors
            - Touch: Tactile/physical contact information
            - Internal: Internal physiological signals
            - Proprioception: Body position and movement awareness
            - Audio: Sound input (if available)
            
            The percept serves as the bridge between the external world and the agent's
            internal representation, allowing the brain to process sensory information
            and generate appropriate responses.
            
            Attributes:
            - vision: 2D array of visual values (width x height)
            - touch: Vector of tactile contact points
            - internal: Vector of internal physiological signals
            - proprioception: Vector of body position/movement signals
            - audio: Vector of audio samples (if available)
            - timestamp: Time when this percept was generated
            
            Methods:
            - getVision(): Get visual input as 2D array
            - setVision(vision): Set visual input
            - getVisionWidth(): Get vision field width
            - getVisionHeight(): Get vision field height
            - getTouch(): Get tactile input
            - setTouch(touch): Set tactile input
            - getInternal(): Get internal signals
            - setInternal(internal): Set internal signals
            - getProprioception(): Get body position signals
            - setProprioception(proprioception): Set body signals
            - getAudio(): Get audio input
            - setAudio(audio): Set audio input
            - getAllSignals(): Get all sensory data concatenated
            - getTimestamp(): Get percept timestamp
            - setTimestamp(timestamp): Set percept timestamp
            
            Example usage:
                # Create empty percept
                percept = pynlm.SensoryPercept()
                
                # Set vision (8x8 grayscale image)
                vision_data = [[0.1, 0.2], [0.3, 0.4]]  # 2x2 example
                percept.setVision(vision_data)
                
                # Set internal signals
                internal_data = [0.8, 0.3, 0.9]  # hunger, energy, satiety
                percept.setInternal(internal_data)
                
                # Get all signals as a single vector
                all_signals = percept.getAllSignals()
                
                # Get vision dimensions
                width = percept.getVisionWidth()  # 2
                height = percept.getVisionHeight()  # 2
                
                # Set timestamp
                import time
                percept.setTimestamp(time.time())
            
            Note: Vision data is typically stored as a flat vector internally,
            but can be accessed as 2D arrays. All signal values are normalized
            between 0.0 (low) and 1.0 (high) for consistency.
        )pbdoc")
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
            Simple 2D world for NLM simulation. SimpleWorld represents the environment
            in which the agent operates, providing sensory input and handling actions.
            
            The world is a 2D grid where agents can move, interact with objects, and
            receive sensory input. It simulates physics, handles object interactions,
            and provides feedback to the agent's actions.
            
            World features:
            - 2D grid layout with configurable dimensions
            - Agent body state tracking (position, orientation, velocity)
            - Object management (resources, hazards, walls, markers)
            - Physics simulation (collision detection, movement)
            - Energy decay and resource consumption
            - Random number generation for stochastic behavior
            - Sensory percept generation for the agent
            
            Attributes:
            - width: World width in cells
            - height: World height in cells
            - maxEnergy: Maximum energy for agents
            - energyDecayRate: Rate at which energy naturally decays
            - simulationTime: Current simulation time
            - randomSeed: Random seed for reproducible behavior
            
            Methods:
            - configure(width, height, visionWidth, visionHeight): Set world dimensions
            - reset(): Reset world to initial state
            - setAgentStart(x, y): Set agent's starting position
            - update(timestep): Advance simulation by given time
            - applyMotorCommand(cmd, currentTime): Process agent's action
            - getSensoryPercept(): Get current sensory input for agent
            - getAgentBody(): Get current agent body state
            - addObject(obj): Add object to world
            - removeObject(x, y): Remove object at position
            - isValidPosition(x, y): Check if position is free
            - getWidth(), getHeight(): Get world dimensions
            - getMaxEnergy(), setMaxEnergy(e): Get/set max energy
            - getEnergyDecayRate(), setEnergyDecayRate(r): Get/set decay rate
            - getSimulationTime(): Get current simulation time
            - setRandomSeed(seed), getRandomSeed(): Manage random seed
            
            Example usage:
                # Create and configure world
                world = pynlm.createSimpleWorld()
                world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
                
                # Set agent starting position
                world.setAgentStart(10.0, 10.0)
                
                # Reset world to initial state
                world.reset()
                
                # Update world (agent moves for 0.1 seconds)
                world.update(0.1)
                
                # Get sensory percept for agent
                percept = world.getSensoryPercept()
                
                # Apply agent's action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Add objects to world
                resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=100)
                world.addObject(resource)
                
                # Check valid positions
                is_free = world.isValidPosition(5.5, 5.5)  # True (not in object)
                is_free = world.isValidPosition(5.0, 5.0)  # False (in resource)
            
            Note: World coordinates are in meters, with (0,0) at bottom-left corner.
            Vision fields are centered on the agent's position and provide a limited
            window into the environment for efficient processing.
        )pbdoc")
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

    py::class_<Brain>(m, "Brain", R"pbdoc(
            Central neural simulation brain class. Brain is the core computational engine
            of the NLM system, implementing spiking neural networks that process information,
            learn from experience, and generate behavior.
            
            The brain consists of multiple neural regions connected by synapses:
            - Neurons: Computational units that process inputs and produce outputs
            - Synapses: Connections between neurons that transmit signals
            - Regions: Groups of neurons with similar properties and functions
            - Populations: Subgroups of neurons within regions
            
            The brain implements complex neural dynamics including:
            - Leaky Integrate-and-Fire (LIF) neuron dynamics
            - Event-driven spike propagation with delays
            - Multiple plasticity mechanisms (STDP, Hebbian, structural)
            - Neuromodulation (dopamine, curiosity, novelty)
            - Memory systems (working, episodic, associative)
            - Prediction and planning systems
            - Cognitive functions (attention, concept formation)
            - Development and maturation processes
            
            Brain methods enable:
            - Initialization and configuration
            - Simulation steps and time evolution
            - Sensory input processing
            - Action generation and output
            - State management (save/load, reset)
            - Statistical analysis and monitoring
            - Region and population management
            
            Example usage:
                # Create configuration
                config = pynlm.createDefaultConfig()
                config.set("brain.neuron_count", 1000)
                config.set("brain.region_count", 2)
                
                # Create and initialize brain
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Run simulation
                for step in range(100):
                    # Inject sensory input
                    sensory_input = percept  # From environment
                    brain.receiveSensoryInput(sensory_input)
                    
                    # Process one step of neural activity
                    brain.step(step, step * 0.001)
                    
                    # Get motor output
                    action = brain.produceAction()
                
                # Get statistics
                neuron_count = brain.getTotalNeuronCount()
                spike_count = brain.getTotalSpikeCount()
                firing_rate = brain.getAverageFiringRate()
            
            The brain operates in real-time with millisecond precision, making it
            suitable for interactive applications and robotics simulations.
        )pbdoc")
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

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
            Agent brain interface connecting NLM brain to world. AgentBrain serves as the
            bridge between the neural brain and the external environment, translating between
            sensory input and motor output.
            
            AgentBrain provides the essential interface for embodied intelligence:
            - Receives sensory percepts from the environment
            - Processes sensory information and injects it into the brain
            - Decodes neural activity into motor commands
            - Applies reward and other neuromodulation signals
            - Updates developmental processes
            - Monitors and controls brain subsystems
            
            The agent brain enables the brain to interact with the real world through:
            - Sensory perception: Seeing, hearing, feeling internal state
            - Motor control: Acting in the environment
            - Learning: Receiving feedback through rewards
            - Development: Maturing over time and experience
            
            Methods:
            - initialize(world): Connect brain to specific world
            - getSensoryInputSize(): Get expected sensory input size
            - getMotorOutputSize(): Get expected motor output size
            - processSensoryInput(percept): Feed percept to brain
            - decodeMotorCommand(): Convert neural activity to action
            - applyRewardModulation(reward, predicted): Apply reinforcement learning
            - updateDevelopment(timestep): Advance developmental processes
            - getDevelopmentalStage(): Get current brain maturity level
            - getNeuromodulationLevel(): Get current neuromodulation strength
            - getCuriosityLevel(): Get curiosity-driven exploration drive
            - getNoveltyLevel(): Get sensitivity to novel stimuli
            - getPredictionError(): Get prediction accuracy measure
            - reset(): Reset agent for new episode
            - getBrain(): Access underlying brain object
            - enable/disable subsystems: Turn on/off specific functions
            
            Example usage:
                # Create agent brain
                agent = pynlm.createAgentBrain(brain)
                
                # Initialize with world
                agent.initialize(world)
                
                # Enable learning subsystems
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
                
                # Run perception-action loop
                for step in range(100):
                    # Get what the agent sees
                    percept = world.getSensoryPercept()
                    
                    # Tell brain what it sees
                    agent.processSensoryInput(percept)
                    
                    # Brain thinks
                    brain.step(step)
                    
                    # Get action from brain
                    action = agent.decodeMotorCommand()
                    
                    # Do action in world
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Learn from result
                    reward = world.computeReward()
                    agent.applyRewardModulation(reward, 0.0)
                    
                    # Update development
                    agent.updateDevelopment(0.1)
                
                # Get statistics
                curiosity = agent.getCuriosityLevel()
                novelty = agent.getNoveltyLevel()
                dev_stage = agent.getDevelopmentalStage()
            
            The AgentBrain is the primary interface for using NLM in embodied applications,
            combining neural computation with real-time interaction with dynamic environments.
        )pbdoc")
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

    m.def("createDemoConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set parameters for demonstration/simplicity
        config->set("brain.neuron_count", 500, ConfigSource::Default);
        config->set("brain.region_count", 1, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a demonstration configuration optimized for simple experiments");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set parameters for research/complex experiments
        config->set("brain.neuron_count", 5000, ConfigSource::Default);
        config->set("brain.region_count", 4, ConfigSource::Default);
        config->set("brain.connection_probability", 0.1, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.01, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("simulation_timestep", 0.0001, ConfigSource::Default);
        return config;
    }, "Create a research configuration optimized for complex experiments");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Minimal configuration for basic functionality
        config->set("brain.neuron_count", 100, ConfigSource::Default);
        config->set("brain.region_count", 1, ConfigSource::Default);
        config->set("brain.connection_probability", 0.02, ConfigSource::Default);
        config->set("plasticity.stdp.enable", false, ConfigSource::Default); // Disable for minimal
        return config;
    }, "Create a minimal configuration for basic testing");

    m.def("validateBrainState", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw py::value_error("Brain is null or not initialized");
        }
        if (!brain->getConfig()) {
            throw py::value_error("Brain configuration is not set");
        }
        return true;
    }, py::arg("brain"), "Validate brain state and configuration");

    m.def("validateWorldState", [](std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw py::value_error("World is null or not initialized");
        }
        return true;
    }, py::arg("world"), "Validate world state");

    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                        std::shared_ptr<AgentBrain> agent, uint64_t steps) {
        if (!validateBrainState(brain) || !validateWorldState(world) || !agent) {
            throw py::value_error("Invalid brain, world, or agent state");
        }
        
        float total_reward = 0.0f;
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.1);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Compute reward
            float reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0f;
            total_reward += reward;
            
            // Apply reward modulation
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        return total_reward / steps;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), 
         "Run a complete simulation episode and return average reward");

    m.def("runCompleteSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent, uint64_t steps,
                                     bool enable_reward_modulation, bool enable_curiosity,
                                     bool enable_development) {
        if (!validateBrainState(brain) || !validateWorldState(world) || !agent) {
            throw py::value_error("Invalid brain, world, or agent state");
        }
        
        // Configure agent subsystems
        agent->enableRewardModulation(enable_reward_modulation);
        agent->enableCuriosity(enable_curiosity);
        agent->enableDevelopment(enable_development);
        
        float total_reward = 0.0f;
        float total_firing_rate = 0.0f;
        uint64_t firing_count = 0;
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.1);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Compute reward
            float reward = world->getSensoryPercept().getInternal()[0] if world->getSensoryPercept().getInternal() else 0.0f;
            total_reward += reward;
            
            // Collect metrics
            total_firing_rate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firing_count++;
            
            // Update development
            agent->updateDevelopment(0.1);
        }
        
        return py::make_tuple(
            total_reward / steps,
            total_firing_rate / steps,
            (float)firing_count / steps,
            agent->getCuriosityLevel(),
            agent->getNoveltyLevel()
        );
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
         py::arg("enable_reward_modulation") = true,
         py::arg("enable_curiosity") = true,
         py::arg("enable_development") = true,
         "Run a complete simulation with all subsystems enabled");

    m.def("getSimulationStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                 std::shared_ptr<AgentBrain> agent) {
        if (!validateBrainState(brain) || !validateWorldState(world) || !agent) {
            throw py::value_error("Invalid brain, world, or agent state");
        }
        
        auto stats = py::dict();
        
        // Brain statistics
        stats["brain"] = py::dict(
            "neuron_count", brain->getTotalNeuronCount(),
            "synapse_count", brain->getTotalSynapseCount(),
            "firing_count", brain->getFiringNeuronCount(),
            "average_firing_rate", brain->getAverageFiringRate(),
            "total_spikes", brain->getTotalSpikeCount(),
            "development_stage", brain->getDevelopmentalStage(),
            "e_to_i_ratio", brain->getExcitationInhibitionRatio()
        );
        
        // World statistics
        stats["world"] = py::dict(
            "width", world->getWidth(),
            "height", world->getHeight(),
            "simulation_time", world->getSimulationTime(),
            "max_energy", world->getMaxEnergy(),
            "energy_decay_rate", world->getEnergyDecayRate()
        );
        
        // Agent statistics
        stats["agent"] = py::dict(
            "curiosity_level", agent->getCuriosityLevel(),
            "novelty_level", agent->getNoveltyLevel(),
            "prediction_error", agent->getPredictionError(),
            "neuromodulation_level", agent->getNeuromodulationLevel(),
            "developmental_stage", agent->getDevelopmentalStage()
        );
        
        return stats;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
         "Collect comprehensive statistics from brain, world, and agent");

    m.def("dumpBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!validateBrainState(brain)) {
            throw py::value_error("Invalid brain state");
        }
        
        if (brain->save(filename)) {
            return true;
        } else {
            throw py::runtime_error("Failed to save brain state to file: " + filename);
        }
    }, py::arg("brain"), py::arg("filename"),
         "Save brain state to file (returns True if successful)");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!validateBrainState(brain)) {
            throw py::value_error("Invalid brain state");
        }
        
        if (brain->load(filename)) {
            return true;
        } else {
            throw py::runtime_error("Failed to load brain state from file: " + filename);
        }
    }, py::arg("brain"), py::arg("filename"),
         "Load brain state from file (returns True if successful)");

    m.def("exportNeuronActivity", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!validateBrainState(brain)) {
            throw py::value_error("Invalid brain state");
        }
        
        // Create simple text file with neuron activity data
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw py::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "Neuron Activity Export" << std::endl;
        file << "=======================" << std::endl;
        file << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        file << "Firing neuron count: " << brain->getFiringNeuronCount() << std::endl;
        file << "Total spike count: " << brain->getTotalSpikeCount() << std::endl;
        
        file.close();
        return filename;
    }, py::arg("brain"), py::arg("filename"),
         "Export neuron activity statistics to file (returns filename)");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
