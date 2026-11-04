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
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        NeuronId is used throughout NLM to reference specific neurons within the brain.
        Each neuron has a unique numerical identifier that persists across simulation steps.
        
        Example:
            >>> import pynlm
            >>> from pynlm import NeuronId
            >>> neuron_id = NeuronId(42)
            >>> print(neuron_id)
            <NeuronId: 42>
            >>> neuron_id.value
            42
            >>> neuron_id.index()
            42
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Construct a NeuronId with the given value.
            
            Args:
                value: The unique identifier value for this neuron.
        )pbdoc")
        .def_readwrite("value", &NeuronId::value, "The unique identifier value")
        .def("index", &NeuronId::index, "Get the neuron index (alias for value)")
        .def("__eq__", &NeuronId::operator==, "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=, "Check inequality with another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        SynapseId is used throughout NLM to reference specific synaptic connections
        between neurons. Each synapse has a unique numerical identifier that persists
        across simulation steps and plasticity events.
        
        Example:
            >>> import pynlm
            >>> from pynlm import SynapseId
            >>> synapse_id = SynapseId(123)
            >>> print(synapse_id)
            <SynapseId: 123>
            >>> synapse_id.value
            123
            >>> synapse_id.index()
            123
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Construct a SynapseId with the given value.
            
            Args:
                value: The unique identifier value for this synapse.
        )pbdoc")
        .def_readwrite("value", &SynapseId::value, "The unique identifier value")
        .def("index", &SynapseId::index, "Get the synapse index (alias for value)")
        .def("__eq__", &SynapseId::operator==, "Check equality with another SynapseId")
        .def("__ne__", &SynapseId::operator!=, "Check inequality with another SynapseId")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        RegionId identifies a specific neural region within the brain. Each region
        contains populations of neurons that perform specific functions or computations.
        Region IDs persist across simulation steps and can be used to reference specific
        regions for targeted operations.
        
        Example:
            >>> import pynlm
            >>> from pynlm import RegionId
            >>> region_id = RegionId(5)
            >>> print(region_id)
            <RegionId: 5>
            >>> region_id.value
            5
            >>> region_id.index()
            5
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Construct a RegionId with the given value.
            
            Args:
                value: The unique identifier value for this brain region.
        )pbdoc")
        .def_readwrite("value", &RegionId::value, "The unique identifier value")
        .def("index", &RegionId::index, "Get the region index (alias for value)")
        .def("__eq__", &RegionId::operator==, "Check equality with another RegionId")
        .def("__ne__", &RegionId::operator!=, "Check inequality with another RegionId")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        PopulationId identifies a specific population of neurons within a brain region.
        Each population contains neurons of the same type that perform related functions
        or represent specific features. Population IDs help organize neurons by function
        and make it easier to target operations to specific groups of neurons.
        
        Example:
            >>> import pynlm
            >>> from pynlm import PopulationId
            >>> pop_id = PopulationId(10)
            >>> print(pop_id)
            <PopulationId: 10>
            >>> pop_id.value
            10
            >>> pop_id.index()
            10
        )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Construct a PopulationId with the given value.
            
            Args:
                value: The unique identifier value for this neuron population.
        )pbdoc")
        .def_readwrite("value", &PopulationId::value, "The unique identifier value")
        .def("index", &PopulationId::index, "Get the population index (alias for value)")
        .def("__eq__", &PopulationId::operator==, "Check equality with another PopulationId")
        .def("__ne__", &PopulationId::operator!=, "Check inequality with another PopulationId");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Type of neuron in the NLM system.
        
        NeuronType enumerates the different categories of neurons that can exist
        in a computational brain. Each type has distinct physiological properties
        and functional roles in neural computation.
        
        Values:
            - Excitatory: Neurons that increase the likelihood of other neurons firing
            - Inhibitory: Neurons that decrease the likelihood of other neurons firing  
            - Modulatory: Neurons that release neuromodulators affecting large-scale brain state
            - Sensory: Neurons that receive input from the environment
            - Motor: Neurons that generate output actions
            - Internal: Neurons involved in internal processing and memory
            
        Example:
            >>> import pynlm
            >>> from pynlm import NeuronType
            >>> neuron_type = NeuronType.Excitatory
            >>> print(neuron_type)
            <NeuronType.Excitatory>
        )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that increases firing probability")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that decreases firing probability")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that releases neuromodulators")
        .value("Sensory", NeuronType::Sensory, "Neuron that receives sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that generates motor output")
        .value("Internal", NeuronType::Internal, "Neuron involved in internal processing");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Type of synaptic connection between neurons.
        
        SynapseType enumerates the different categories of synaptic connections
        that can exist in a computational brain. Each type has distinct
        physiological properties and functional characteristics.
        
        Values:
            - Excitatory: Synapse that increases postsynaptic neuron firing probability
            - Inhibitory: Synapse that decreases postsynaptic neuron firing probability
            - Modulatory: Synapse that releases neuromodulators affecting brain state
            - Electrical: Direct electrical coupling between neurons
            - GapJunction: Chemical synaptic connection with rapid signaling
            
        Example:
            >>> import pynlm
            >>> from pynlm import SynapseType
            >>> synapse_type = SynapseType.Excitatory
            >>> print(synapse_type)
            <SynapseType.Excitatory>
        )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Synapse that increases firing probability")
        .value("Inhibitory", SynapseType::Inhibitory, "Synapse that decreases firing probability")
        .value("Modulatory", SynapseType::Modulatory, "Synapse that releases neuromodulators")
        .value("Electrical", SynapseType::Electrical, "Direct electrical coupling")
        .value("GapJunction", SynapseType::GapJunction, "Rapid chemical synaptic connection");

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage of a brain or neuron population.
        
        DevelopmentalStage enumerates the different phases of brain development
        that a computational brain can be in. Each stage represents a distinct
        period with specific plasticity rates, excitability patterns, and functional
        capabilities.
        
        Values:
            - Initial: Newborn stage with high plasticity and immature connections
            - CriticalPeriod: Period of enhanced learning and refinement
            - Maturation: Stabilization and optimization of neural circuits
            - Adult: Mature brain with optimized performance and stable connections
            - Aging: Gradual decline in plasticity and performance
            
        Example:
            >>> import pynlm
            >>> from pynlm import DevelopmentalStage
            >>> stage = DevelopmentalStage.CriticalPeriod
            >>> print(stage)
            <DevelopmentalStage.CriticalPeriod>
        )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Newborn stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Enhanced learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Stabilization and optimization")
        .value("Adult", DevelopmentalStage::Adult, "Mature brain with stable connections")
        .value("Aging", DevelopmentalStage::Aging, "Declining plasticity and performance");

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Firing state of a neuron in the simulation.
        
        FiringState enumerates the different electrical states a neuron can be in
        during its life cycle. These states reflect the membrane potential dynamics
        and determine the neuron's readiness to fire or its recovery after firing.
        
        Values:
            - Resting: Neuron at its resting membrane potential
            - Active: Neuron is actively firing or will soon fire
            - Refractory: Neuron recovering from a spike, unable to fire immediately
            - Inhibited: Neuron actively suppressed by inhibitory inputs
            
        Example:
            >>> import pynlm
            >>> from pynlm import FiringState
            >>> state = FiringState.Active
            >>> print(state)
            <FiringState.Active>
        )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at resting membrane potential")
        .value("Active", FiringState::Active, "Neuron actively firing or about to fire")
        .value("Refractory", FiringState::Refractory, "Neuron recovering from spike")
        .value("Inhibited", FiringState::Inhibited, "Neuron actively suppressed by inhibition");

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Type of action that can be produced by a neuron.
        
        ActionType enumerates the different categories of actions that a brain
        can produce to interact with its environment. Each action type represents
        a specific behavioral response to sensory input or internal state.
        
        Values:
            - MoveForward: Move forward in the environment
            - MoveBackward: Move backward in the environment
            - MoveLeft: Move left in the environment
            - MoveRight: Move right in the environment
            - TurnLeft: Rotate counterclockwise
            - TurnRight: Rotate clockwise
            - Look: Change gaze direction to a specific point
            - LookUp: Look upward
            - LookDown: Look downward
            - Interact: Interact with an object
            - Eat: Consume a resource
            - Drink: Consume a liquid
            - Rest: Stop activity and recover
            - Wait: Do nothing for one time step
            - Custom: User-defined action
            
        Example:
            >>> import pynlm
            >>> from pynlm import ActionType
            >>> action = ActionType.MoveForward
            >>> print(action)
            <ActionType.MoveForward>
        )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in environment")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Change gaze direction")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Consume resource")
        .value("Drink", ActionType::Drink, "Consume liquid")
        .value("Rest", ActionType::Rest, "Stop activity and recover")
        .value("Wait", ActionType::Wait, "Do nothing for one time step")
        .value("Custom", ActionType::Custom, "User-defined action");

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command for agent control.
        
        MotorCommand enumerates the fundamental motor actions that can be directly
        executed by the agent's body. These are more abstract than ActionType
        and represent the basic building blocks of motor behavior.
        
        Values:
            - MoveForward: Move forward one unit of distance
            - MoveBackward: Move backward one unit of distance
            - TurnLeft: Rotate counterclockwise by a small angle
            - TurnRight: Rotate clockwise by a small angle
            - LookLeft: Turn gaze left
            - LookRight: Turn gaze right
            - Interact: Attempt to interact with current target
            - Wait: Do nothing this time step
            
        Example:
            >>> import pynlm
            >>> from pynlm import MotorCommand
            >>> cmd = MotorCommand.TurnLeft
            >>> print(cmd)
            <MotorCommand.TurnLeft>
        )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward one unit")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward one unit")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate clockwise")
        .value("LookLeft", MotorCommand::LookLeft, "Turn gaze left")
        .value("LookRight", MotorCommand::LookRight, "Turn gaze right")
        .value("Interact", MotorCommand::Interact, "Interact with target")
        .value("Wait", MotorCommand::Wait, "Do nothing this time step");

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Type of object in the simple 2D world simulation.
        
        WorldObjectType enumerates the different categories of objects that can
        exist in the SimpleWorld environment. Each type has specific properties
        and behaviors in the simulated environment.
        
        Values:
            - Empty: No object (vacant space)
            - Resource: Consumable resource that provides energy
            - Hazard: Dangerous object that damages the agent
            - Wall: Impassable barrier
            - Marker: Special point of interest or goal
            
        Example:
            >>> import pynlm
            >>> from pynlm import WorldObjectType
            >>> obj_type = WorldObjectType.Resource
            >>> print(obj_type)
            <WorldObjectType.Resource>
        )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object (vacant space)")
        .value("Resource", WorldObjectType::Resource, "Consumable resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object")
        .value("Wall", WorldObjectType::Wall, "Impassable barrier")
        .value("Marker", WorldObjectType::Marker, "Special point of interest");

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration system for NLM brain simulation.
        
        Config provides a key-value storage system for brain configuration parameters.
        It supports multiple data types, persistence to file, and command-line argument
        loading. Configuration is essential for setting up brain behavior, learning
        rates, environmental parameters, and simulation settings.
        
        The Config class is designed to be flexible and type-safe, supporting:
        - Basic types: int, float, bool, string
        - Complex types: arrays of integers, floats, and strings
        - Multiple sources: defaults, files, command line, and runtime
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("learning_rate", 0.01)
            >>> config.set("brain_size", 1000)
            >>> config.set("environment.width", 20)
            >>> brain = pynlm.createBrain(config)
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file.")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
           "Load configuration from command line arguments.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.")
        .def("clear", &Config::clear,
             "Clear all configuration entries.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("get", [](const Config& self, const std::string& key) {
            // Try different types and return the first match
            auto value = self.get<int>(key);
            if (value) return *value;
            auto fvalue = self.get<double>(key);
            if (fvalue) return *fvalue;
            auto bvalue = self.get<bool>(key);
            if (bvalue) return *bvalue;
            auto svalue = self.get<std::string>(key);
            if (svalue) return *svalue;
            return std::string(); // Return empty string if not found
        }, py::arg("key"), R"pbdoc(
            Get a configuration value with automatic type detection.
            
            Args:
                key: The configuration key to retrieve.
                
            Returns:
                The configuration value, or empty string if not found.
                
            Example:
                >>> config.get("learning_rate")
                0.01
                >>> config.get("brain_size")
                1000
        )pbdoc");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory inputs to the brain.
        
        SensoryInput represents data from the environment or internal brain state
        that the brain can process to understand its current situation. All sensory
        modalities (vision, audio, touch, internal signals) inherit from this base class.
        
        Key properties:
        - Provides raw data as float vectors
        - Includes dimensionality information
        - Has timestamps for temporal alignment
        - Supports multiple sensory modalities
        
        Example:
            >>> import pynlm
            >>> from pynlm import SensoryInput
            >>> # Vision sensory input
            >>> vision = pynlm.Vision(80, 60)
            >>> vision.setData([0.1, 0.5, 0.9, ...])  # Image data
            >>> vision.getType()
            'Vision'
            >>> vision.getDimensions()
            (80, 60, 3)
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input from the environment.
        
        Vision represents visual information captured by the agent's sensory systems.
        It contains pixel intensity values (typically 0-1) representing the visual scene.
        
        Vision is the primary sensory modality in the SimpleWorld simulation, providing
        information about objects, terrain, and spatial relationships.
        
        Attributes:
            - width: Horizontal resolution in pixels
            - height: Vertical resolution in pixels  
            - channels: Number of color channels (1=grayscale, 3=RGB)
            
        Example:
            >>> import pynlm
            >>> # Create vision input
            >>> vision = pynlm.Vision(64, 48, 3)  # 64x48 RGB image
            >>> # Simulate seeing a red wall
            >>> vision.setData([0.1] * 64*48*3)  # Dark scene
            >>> width = vision.getWidth()
            >>> height = vision.getHeight()
            >>> channels = vision.getChannels()
            >>> print(f"Vision size: {width}x{height}x{channels}")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, R"pbdoc(
                Initialize Vision sensory input.
                
                Args:
                    width: Image width in pixels.
                    height: Image height in pixels.
                    channels: Number of color channels (1 for grayscale, 3 for RGB).
                )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(
            Set visual data.
            
            Args:
                data: Flattened array of pixel values (0-1). Order is row-major:
                      [R,G,B] for RGB or single value for grayscale, repeated
                      for each pixel.
                      
            Example:
                >>> # Create a 2x2 grayscale image: [0, 0.5, 0.8, 1.0]
                >>> vision.setData([0.0, 0.5, 0.8, 1.0])  # 2x2 image
            )pbdoc")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input from the environment.
        
        Audio represents sound information captured by the agent's sensory systems.
        It contains amplitude samples over time representing audio waveforms.
        
        Audio input is used in more complex environments where sound provides
        additional information about the world, such as location of sources or
        environmental characteristics.
        
        Attributes:
            - sampleRate: Number of samples per second (Hz)
            - numSamples: Number of samples in the buffer
            
        Example:
            >>> import pynlm
            >>> # Create audio input
            >>> audio = pynlm.Audio(44100, 1024)  # 1 second at 44.1kHz
            >>> audio.setSampleRate(44100)
            >>> audio.setData([0.1, 0.5, -0.3, ...])  # Audio waveform
            >>> sample_rate = audio.getSampleRate()
            >>> num_samples = audio.getNumSamples()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"), R"pbdoc(
            Initialize Audio sensory input.
            
            Args:
                sampleRate: Number of samples per second (Hz).
                numSamples: Number of samples in the buffer.
            )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), R"pbdoc(
            Set audio data.
            
            Args:
                data: Array of audio sample amplitudes (-1.0 to 1.0).
                      Order is chronological: [sample0, sample1, ..., sampleN].
            )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal brain state signals as sensory input.
        
        InternalSignals represents the brain's own internal state as if it were
        sensory data. This allows the brain to "feel" its own neural activity,
        creating a form of self-awareness and closed-loop processing.
        
        Internal signals include:
        - Neural firing rates
        - Membrane potentials
        - Neuromodulator levels
        - Working memory states
        
        This modality enables the brain to process its own activity, which is
        essential for self-regulation and complex cognitive functions.
        
        Example:
            >>> import pynlm
            >>> # Create internal signals
            >>> internal = pynlm.InternalSignals()
            >>> # Add neural activity signals
            >>> internal.addSignal(0.5)  # Moderate firing rate
            >>> internal.addSignal(0.8)  # High firing rate
            >>> internal.addSignal(0.1)  # Low firing rate
            >>> internal.getData()
            [0.5, 0.8, 0.1]
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Action represents a behavior that a brain can produce to interact with
        the environment. Each action has a type and optional parameters that
        specify how the action should be executed.
        
        Actions are the output of brain processing and are sent to the world
        to produce effects like movement, interaction, or environmental changes.
        
        Example:
            >>> import pynlm
            >>> from pynlm import Action, ActionType
            >>> # Create a simple move action
            >>> action = Action(ActionType.MoveForward)
            >>> print(action.getType())
            <ActionType.MoveForward>
            >>> # Create an action with parameters
            >>> action2 = Action(ActionType.Look, [0.5, 0.5])  # Look at center
            >>> params = action2.getParameters()
            [0.5, 0.5]
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), R"pbdoc(
            Construct an action with the specified type.
            
            Args:
                type: The type of action to perform.
        )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"), R"pbdoc(
            Construct an action with type and parameters.
            
            Args:
                type: The type of action to perform.
                parameters: List of action parameters (interpretation depends on type).
            )pbdoc")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable name of the action")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the simulation.
        
        WorldObject represents entities that exist in the SimpleWorld environment.
        Objects can be static (walls, hazards) or dynamic (resources, agents).
        Each object has position, type, and physical properties.
        
        Objects are the basic building blocks of the simulated environment and
        define what the agent can interact with and how.
        
        Example:
            >>> import pynlm
            >>> from pynlm import WorldObject, WorldObjectType
            >>> # Create a resource at position (5, 5)
            >>> resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, 10.0)
            >>> print(resource.x), print(resource.y)
            5.0 5.0
            >>> print(resource.type)
            <WorldObjectType.Resource>
            >>> print(resource.value)  # Energy content
            10.0
            >>> print(resource.active)  # Is it available?
            true
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, R"pbdoc(
                Initialize a world object.
                
                Args:
                    x: X coordinate position.
                    y: Y coordinate position.
                    type: Type of object (Resource, Hazard, Wall, etc.).
                    value: Object-specific value (energy for resources, danger for hazards).
                    radius: Physical size of the object.
            )pbdoc")
        .def_readwrite("x", &WorldObject::x, "X coordinate position")
        .def_readwrite("y", &WorldObject::y, "Y coordinate position")
        .def_readwrite("radius", &WorldObject::radius, "Physical size of object")
        .def_readwrite("type", &WorldObject::type, "Type of object")
        .def_readwrite("value", &WorldObject::value, "Object-specific value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active/available");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation.
        
        AgentBody represents the physical state of the agent in the simulated world.
        It contains position, velocity, orientation, and physiological properties
        that determine how the agent interacts with the environment.
        
        The body state is updated by the world when motor actions are applied,
        and is used by the agent to understand its current situation.
        
        Example:
            >>> import pynlm
            >>> from pynlm import AgentBody
            >>> # Create agent at position (0, 0)
            >>> body = AgentBody()
            >>> body.x = 0.0
            >>> body.y = 0.0
            >>> body.orientation = 0.0  # Facing right
            >>> body.velocityX = 0.1
            >>> body.velocityY = 0.0
            >>> body.energy = 100.0
            >>> body.health = 100.0
            >>> print(f"Position: ({body.x}, {body.y})")
            Position: (0.0, 0.0)
            >>> print(f"Orientation: {body.orientation} radians")
            Orientation: 0.0 radians
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate position")
        .def_readwrite("y", &AgentBody::y, "Y coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians (0=right, π=up)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X component of velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y component of velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (rad/s)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Current health level (0-100)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result from applying an action to the world.
        
        ActionResult represents the outcome of an agent's action in the environment.
        It contains feedback about whether the action succeeded, the reward received,
        and any descriptive message about the outcome.
        
        Action results are used by the brain to learn from experience through
        reward modulation and prediction error signals.
        
        Example:
            >>> import pynlm
            >>> from pynlm import ActionResult
            >>> # Create a successful action result
            >>> result = ActionResult(10.0, True, "Successfully ate resource")
            >>> print(f"Reward: {result.reward}")
            Reward: 10.0
            >>> print(f"Success: {result.success}")
            Success: true
            >>> print(f"Message: {result.message}")
            Message: Successfully ate resource
            >>> # Create a failed action
            >>> failed_result = ActionResult(-5.0, False, "Hit wall")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Initialize action result")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about outcome");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Combined sensory percept from all modalities.
        
        SensoryPercept represents the complete sensory input from all available
        modalities (vision, audio, touch, internal signals) as experienced by
        the agent in the current simulation step.
        
        This unified representation is what the brain receives and processes
        to understand the current state of the world and the agent's own state.
        
        Example:
            >>> import pynlm
            >>> from pynlm import SensoryPercept
            >>> # Create a sensory percept
            >>> percept = SensoryPercept()
            >>> # Set vision data (80x60 RGB image flattened)
            >>> vision_data = [0.1] * 80*60*3
            >>> percept.setVision(vision_data)
            >>> # Set touch data
            >>> touch_data = [0.0, 0.5, 0.0]  # Left, center, right
            >>> percept.setTouch(touch_data)
            >>> # Get combined data
            >>> all_signals = percept.getAllSignals()
            >>> print(f"Vision dimensions: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
            Vision dimensions: 80x60
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals combined")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp");

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld represents the simulated environment in which the NLM brain operates.
        It provides the physical space, objects, and dynamics that the agent interacts with.
        
        The world is a 2D grid-based environment with continuous physics, providing
        vision, touch, and other sensory inputs to the agent. It handles agent movement,
        collision detection, and environmental dynamics.
        
        Key features:
        - 2D grid with continuous coordinates
        - Vision sensor with configurable field of view
        - Touch/proximity sensor
        - Physics simulation (position, velocity, acceleration)
        - Object management (resources, hazards, walls, markers)
        - Time-based updates with fixed timestep
        - Random number generation for stochastic behavior
        
        Example:
            >>> import pynlm
            >>> # Create the world
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> world.reset()
            >>> # Place a resource
            >>> resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
            >>> world.addObject(resource)
            >>> # Place a wall
            >>> wall = pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Wall, 0.0, 1.0)
            >>> world.addObject(wall)
            >>> # Run simulation
            >>> for step in range(10):
            ...     world.update(0.1)  # Update physics
            ...     percept = world.getSensoryPercept()
            ...     # Process percept with brain...
            ...     # Apply motor command...
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and sensor fields of view")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics for the given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to move the agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept (vision, touch, etc.)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's current body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid (not occupied by wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in units")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in units")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed");

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Brain is the core computational component of NLM. It implements real spiking
        neural computation with event-driven dynamics and integrated memory, prediction,
        cognition, and neuromodulation systems.
        
        The Brain class orchestrates all neural processing, maintaining:
        - Millions of neurons with biologically plausible dynamics
        - Complex synaptic connectivity patterns
        - Multiple memory systems (working, episodic, associative)
        - Prediction and cognition systems
        - Neuromodulation for learning and behavior
        - Developmental stages affecting plasticity
        
        The brain operates as a closed-loop system: it receives sensory input,
        processes it through neural dynamics, produces motor output, and learns
        from outcomes through plasticity rules.
        
        Example:
            >>> import pynlm
            >>> # Create a brain with default configuration
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> # Process one step of thinking
            >>> brain.step(0)
            >>> # Get statistics
            >>> print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            >>> print(f"Total spikes: {brain.getTotalSpikeCount()}")
            >>> print(f"Development stage: {brain.getDevelopmentalStage()}")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain provides the interface between the neural brain and the simulated
        world. It handles sensory transduction (converting world percepts into brain
        inputs) and motor decoding (converting brain activity into world actions).
        
        AgentBrain is the "agent" in the agent-based learning paradigm - it's what
        perceives the world, makes decisions, and acts within it. The brain itself
        does the thinking, but the agent is the bridge to the environment.
        
        Key responsibilities:
        - Convert world state into sensory inputs for the brain
        - Process sensory information and inject it into neural populations
        - Decode neural activity into motor commands for the world
        - Apply reward signals to modulate brain learning and behavior
        - Manage developmental processes over time
        - Maintain curiosity and novelty-driven exploration
        
        Example:
            >>> import pynlm
            >>> # Create the complete agent system
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> # Enable learning features
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
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

    // Function to create a brain with a simple configuration optimized for beginners
    m.def("createBeginnerBrain", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.populationSize", 500);
        config->set("brain.regionCount", 5);
        config->set("brain.plasticity.enabled", true);
        config->set("brain.development.enabled", true);
        config->set("brain.reward.modulation.enabled", true);
        config->set("brain.curiosity.enabled", true);
        return std::make_shared<Brain>(config);
    }, R"pbdoc(
        Create a brain with configuration optimized for beginners and simple simulations.
        
        This convenience function creates a brain with sensible defaults for getting
        started with NLM. It includes:
        - 500 neurons across 5 regions
        - Enabled plasticity for learning
        - Enabled development for brain maturation
        - Enabled reward modulation for reward-based learning
        - Enabled curiosity for exploration
        
        Returns:
            A configured Brain instance ready for simulation.
            
        Example:
            >>> import pynlm
            >>> # Create a beginner-friendly brain
            >>> brain = pynlm.createBeginnerBrain()
            >>> brain.initialize()
        )pbdoc");
    
    // Function to create a complete agent simulation setup
    m.def("createSimulationSetup", []() {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Configure world
        world->configure(20, 20, 8, 8);
        
        return std::make_tuple(brain, world, agent);
    }, R"pbdoc(
        Create a complete agent simulation setup with brain, world, and agent.
        
        This convenience function creates all components needed for a complete
        simulation and returns them as a tuple for easy use.
        
        Returns:
            Tuple of (brain, world, agent) - all initialized and ready for use.
            
        Example:
            >>> import pynlm
            >>> brain, world, agent = pynlm.createSimulationSetup()
            >>> brain.initialize()
            >>> agent.initialize(world)
            >>> 
            >>> # Run a simple simulation
            >>> for step in range(100):
            ...     world.update(0.1)
            ...     agent.processSensoryInput(world.getSensoryPercept())
            ...     brain.step(step)
            ...     action = agent.decodeMotorCommand()
            ...     world.applyMotorCommand(action, world.getSimulationTime())
        )pbdoc");
    
    // Function to run a complete simulation step
    m.def("runSimulationStep", [](
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        float timestep,
        bool verbose = false
    ) {
        // Update world
        world->update(timestep);
        
        // Get and process sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Brain processes
        static size_t step = 0;
        brain->step(step, world->getSimulationTime());
        step++;
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (verbose) {
            std::cout << "Step " << (step-1) << ": "
                     << "Firing neurons: " << brain->getFiringNeuronCount() << ", "
                     << "Total spikes: " << brain->getTotalSpikeCount() << ", "
                     << "Energy: " << world->getAgentBody()->energy
                     << std::endl;
        }
        
        return std::make_tuple(action, world->getSensoryPercept(), world->getAgentBody());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("timestep"), py::arg("verbose") = false,
       R"pbdoc(
        Run a complete simulation step (world update + brain processing + action execution).
        
        This convenience function handles the complete simulation loop for one timestep:
        1. Update world physics
        2. Get sensory percept
        3. Process percept with brain
        4. Execute brain step
        5. Decode motor command
        6. Apply action to world
        
        Args:
            brain: The neural brain to process with.
            world: The world containing the agent and environment.
            agent: The agent that bridges brain to world.
            timestep: Time step for world update.
            verbose: Whether to print detailed information.
            
        Returns:
            Tuple of (action, percept, body) from the simulation step.
            
        Example:
            >>> import pynlm
            >>> brain, world, agent = pynlm.createSimulationSetup()
            >>> brain.initialize()
            >>> agent.initialize(world)
            >>> 
            >>> for step in range(50):
            ...     action, percept, body = pynlm.runSimulationStep(
            ...         brain, world, agent, 0.1, verbose=False
            ...     )
            ...     if step % 10 == 0:
            ...         print(f"Step {step}: Energy={body.energy:.1f}")
        )pbdoc");
    
    // Function to run a complete simulation episode
    m.def("runSimulationEpisode", [](
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t num_steps,
        float timestep,
        bool verbose = false,
        bool enable_rendering = false
    ) {
        std::cout << "Starting simulation episode with " << num_steps << " steps" << std::endl;
        
        brain->initialize();
        agent->initialize(*world);
        
        std::vector<std::tuple<size_t, float, float, float>> history;
        
        for (size_t step = 0; step < num_steps; step++) {
            // Run simulation step
            auto result = runSimulationStep(brain, world, agent, timestep, false);
            auto action = std::get<0>(result);
            auto percept = std::get<1>(result);
            auto body = std::get<2>(result);
            
            // Record history
            history.push_back(std::make_tuple(
                step,
                body->energy,
                body->health,
                brain->getAverageFiringRate()
            ));
            
            if (verbose && (step % 10 == 0 || step == num_steps - 1)) {
                std::cout << "Step " << step << ": "
                         << "Energy=" << body->energy << ", "
                         << "Health=" << body->health << ", "
                         << "FiringRate=" << brain->getAverageFiringRate() << ", "
                         << "ActiveNeurons=" << brain->getActiveNeuronCount()
                         << std::endl;
            }
            
            if (enable_rendering) {
                // Simple visualization - could be enhanced
                std::cout << "Frame: ";
                for (size_t y = 0; y < percept->getVisionHeight(); y++) {
                    std::cout << " ";
                    for (size_t x = 0; x < percept->getVisionWidth(); x++) {
                        auto data = percept->getVision()->getData();
                        size_t idx = (y * percept->getVisionWidth() + x) * 3;
                        if (idx + 2 < data.size()) {
                            float brightness = (data[idx] + data[idx+1] + data[idx+2]) / 3.0f;
                            if (brightness > 0.7) std::cout << "*";
                            else if (brightness > 0.4) std::cout << ".";
                            else std::cout << " ";
                        }
                    }
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        std::cout << "Simulation episode completed" << std::endl;
        return history;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       py::arg("timestep"), py::arg("verbose") = false, py::arg("enable_rendering") = false,
       R"pbdoc(
        Run a complete simulation episode.
        
        This function runs a full simulation episode with all the agent systems
        working together. It handles initialization, the complete simulation loop,
        and optional logging.
        
        Args:
            brain: The neural brain.
            world: The world containing the agent and environment.
            agent: The agent that bridges brain to world.
            num_steps: Number of simulation steps to run.
            timestep: Time step for world updates.
            verbose: Whether to print detailed information.
            enable_rendering: Whether to print simple text-based visualization.
            
        Returns:
            History of simulation state (step, energy, health, firing rate).
            
        Example:
            >>> import pynlm
            >>> brain, world, agent = pynlm.createSimulationSetup()
            >>> # Run a 1000-step simulation
            >>> history = pynlm.runSimulationEpisode(
            ...     brain, world, agent, 1000, 0.1, verbose=True
            ... )
            >>> # Analyze the results
            >>> steps, energies, healths, firing_rates = zip(*history)
            >>> print(f"Final energy: {energies[-1]:.1f}")
            >>> print(f"Max firing rate: {max(firing_rates):.2f} Hz")
        )pbdoc");
    
    // Utility function to get brain state as a Python-friendly dictionary
    m.def("getBrainStateDict", [](std::shared_ptr<Brain> brain) {
        py::dict state;
        
        // Basic statistics
        state["total_neurons"] = brain->getTotalNeuronCount();
        state["total_synapses"] = brain->getTotalSynapseCount();
        state["active_neurons"] = brain->getActiveNeuronCount();
        state["firing_neurons"] = brain->getFiringNeuronCount();
        state["average_firing_rate"] = brain->getAverageFiringRate();
        state["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        state["total_spikes"] = brain->getTotalSpikeCount();
        
        // Developmental state
        state["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        state["learned_preference"] = brain->getLearnedPreference();
        
        // Memory systems (if available)
        try {
            auto working_memory = brain->getWorkingMemory();
            if (working_memory) {
                state["working_memory_active"] = working_memory->isActive();
            }
        } catch (...) {
            state["working_memory_available"] = false;
        }
        
        return state;
    }, py::arg("brain"),
       R"pbdoc(
        Get brain state as a Python-friendly dictionary.
        
        This utility function converts the brain state into a Python dictionary
        for easy access and analysis. It's particularly useful for monitoring
        brain activity and collecting statistics.
        
        Args:
            brain: The brain to query for state information.
            
        Returns:
            Dictionary containing brain state information including:
            - total_neurons: Total number of neurons
            - total_synapses: Total number of synapses
            - active_neurons: Number of active neurons
            - firing_neurons: Number of currently firing neurons
            - average_firing_rate: Average firing rate in Hz
            - excitation_inhibition_ratio: E/I balance ratio
            - total_spikes: Total spike count
            - developmental_stage: Current developmental stage (0=Initial, 4=Aging)
            - learned_preference: Learned preference trait (-1 to 1)
            
        Example:
            >>> import pynlm
            >>> brain = pynlm.createBrain(pynlm.createDefaultConfig())
            >>> brain.initialize()
            >>> state = pynlm.getBrainStateDict(brain)
            >>> print(f"Neurons: {state['total_neurons']}")
            >>> print(f"Firing rate: {state['average_firing_rate']:.2f} Hz")
            >>> print(f"Development stage: {state['developmental_stage']}")
        )pbdoc");
    
    // Utility function to get agent status as a Python-friendly dictionary
    m.def("getAgentStatusDict", [](std::shared_ptr<AgentBrain> agent) {
        py::dict status;
        
        // Get brain status
        auto brain = agent->getBrain();
        if (brain) {
            auto brain_state = getBrainStateDict(brain);
            status.update(brain_state);
        }
        
        // Agent-specific status
        status["neuromodulation_enabled"] = agent->isRewardModulationEnabled();
        status["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
        status["development_enabled"] = agent->isDevelopmentEnabled();
        status["curiosity_enabled"] = agent->isCuriosityEnabled();
        
        status["neuromodulation_level"] = agent->getNeuromodulationLevel();
        status["curiosity_level"] = agent->getCuriosityLevel();
        status["novelty_level"] = agent->getNoveltyLevel();
        status["prediction_error"] = agent->getPredictionError();
        status["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
        
        return status;
    }, py::arg("agent"),
       R"pbdoc(
        Get agent status as a Python-friendly dictionary.
        
        This utility function collects both brain state and agent configuration
        into a single dictionary for easy monitoring and analysis.
        
        Args:
            agent: The agent to query for status information.
            
        Returns:
            Dictionary containing agent status including brain state and
            agent-specific settings and levels.
            
        Example:
            >>> import pynlm
            >>> brain, world, agent = pynlm.createSimulationSetup()
            >>> brain.initialize()
            >>> agent.initialize(world)
            >>> # Enable learning features
            >>> agent.enableCuriosity(True)
            >>> agent.enableRewardModulation(True)
            >>> status = pynlm.getAgentStatusDict(agent)
            >>> print(f"Learning enabled: {status['neuromodulation_enabled']}")
            >>> print(f"Curiosity level: {status['curiosity_level']:.3f}")
            >>> print(f"Prediction error: {status['prediction_error']:.3f}")
        )pbdoc");

} // namespace nlm
