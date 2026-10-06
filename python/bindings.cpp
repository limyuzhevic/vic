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
        
        This module provides a Python interface to the NLM neural simulation framework,
        enabling brain simulation, sensory processing, motor control, and learning behaviors.
    )pbdoc";

    // Enhanced error handling with multiple exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError", "Base runtime error from NLM");
    py::register_exception<std::invalid_argument>(m, "ValueError", "Invalid argument or value provided");
    py::register_exception<std::out_of_range>(m, "IndexError", "Index out of valid range");
    py::register_exception<std::logic_error>(m, "LogicError", "Logical error in operation");
    
    // NLM-specific custom exceptions
    py::register_exception<>(m, "NLMError", "Base exception for NLM-specific errors");
    py::register_exception<>(m, "ConfigError", "Configuration error (invalid config file, missing keys, etc.)");
    py::register_exception<>(m, "BrainError", "Brain simulation error (not initialized, invalid operation, etc.)");
    py::register_exception<>(m, "WorldError", "World simulation error (invalid position, collision, etc.)");
    py::register_exception<>(m, "MemoryError", "Memory system error (full capacity, access error, etc.)");
    py::register_exception<>(m, "DevelopmentError", "Development system error (invalid stage, plasticity error, etc.)");
    py::register_exception<>(m, "NeuromodulationError", "Neuromodulation error (signal issue, balance error, etc.)");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)

    Represents a neuron identifier with a unique numerical value. Used to reference
    specific neurons within the brain. Provides comparison, hashing, and string representation.
    
    Examples:
        >>> neuron_id = NeuronId(42)
        >>> print(neuron_id)
        <NeuronId: 42>
        >>> neuron_id.value
        42
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), "Create a NeuronId with the given value")
        .def_readwrite("value", &NeuronId::value, "The numeric value of this neuron ID")
        .def("index", &NeuronId::index, "Get the 1-based index representation")
        .def("__eq__", &NeuronId::operator==, "Check if two NeuronIds are equal")
        .def("__ne__", &NeuronId::operator!=, "Check if two NeuronIds are not equal")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "Return string representation");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)

    Represents a synapse identifier with a unique numerical value. Used to reference
    specific synapses within the brain. Provides comparison, hashing, and string representation.
    
    Examples:
        >>> synapse_id = SynapseId(123)
        >>> print(synapse_id)
        <SynapseId: 123>
        >>> synapse_id.value
        123
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), "Create a SynapseId with the given value")
        .def_readwrite("value", &SynapseId::value, "The numeric value of this synapse ID")
        .def("index", &SynapseId::index, "Get the 1-based index representation")
        .def("__eq__", &SynapseId::operator==, "Check if two SynapseIds are equal")
        .def("__ne__", &SynapseId::operator!=, "Check if two SynapseIds are not equal")
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        }, "Return string representation");

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)

    Represents a brain region identifier with a unique numerical value. Used to reference
    specific neural regions within the brain. Provides comparison, hashing, and string representation.
    
    Examples:
        >>> region_id = RegionId(5)
        >>> print(region_id)
        <RegionId: 5>
        >>> region_id.value
        5
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), "Create a RegionId with the given value")
        .def_readwrite("value", &RegionId::value, "The numeric value of this region ID")
        .def("index", &RegionId::index, "Get the 1-based index representation")
        .def("__eq__", &RegionId::operator==, "Check if two RegionIds are equal")
        .def("__ne__", &RegionId::operator!=, "Check if two RegionIds are not equal")
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash value for use in dictionaries and sets")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        }, "Return string representation");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)

    Represents a neuron population identifier with a unique numerical value. Used to reference
    specific groups of neurons within the brain. Provides comparison, hashing.
    
    Examples:
        >>> population_id = PopulationId(10)
        >>> population_id.value
        10
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), "Create a PopulationId with the given value")
        .def_readwrite("value", &PopulationId::value, "The numeric value of this population ID")
        .def("index", &PopulationId::index, "Get the 1-based index representation")
        .def("__eq__", &PopulationId::operator==, "Check if two PopulationIds are equal")
        .def("__ne__", &PopulationId::operator!=, "Check if two PopulationIds are not equal");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)

    Defines the types of neurons in the NLM brain system, representing different functional
    roles and properties. Each type has distinct characteristics and behaviors.
    
    Examples:
        >>> neuron_type = NeuronType.Excitatory
        >>> print(neuron_type)
        NeuronType.Excitatory
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Exciting neuron that promotes neural activity")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibiting neuron that suppresses neural activity")
        .value("Modulatory", NeuronType::Modulatory, "Modulating neuron that affects plasticity and learning")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron that processes external input")
        .value("Motor", NeuronType::Motor, "Motor neuron that controls movement and actions")
        .value("Internal", NeuronType::Internal, "Internal neuron for internal representation and processing")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)

    Defines the types of synapses in the NLM brain system, representing different
    connection properties and functional roles. Each type has distinct characteristics
    and transmission properties.
    
    Examples:
        >>> synapse_type = SynapseType.Excitatory
        >>> print(synapse_type)
        SynapseType.Excitatory
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse that promotes neural activity")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse that suppresses neural activity")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse that affects plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse with fast direct coupling")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse allowing ion flow")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)

    Defines the developmental stages of the NLM brain system, representing different
    phases of growth, learning, and maturation. Each stage has characteristic plasticity
    rates and behavioral capabilities.
    
    Examples:
        >>> stage = DevelopmentalStage.CriticalPeriod
        >>> print(stage)
        DevelopmentalStage.CriticalPeriod
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial/embryonic stage with high plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical learning period with enhanced plasticity")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage with reduced plasticity")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage with stable performance")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage with declining performance")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)

    Defines the possible firing states of neurons in the NLM brain system, representing
    different phases of neural activity and electrical behavior.
    
    Examples:
        >>> state = FiringState.Active
        >>> print(state)
        FiringState.Active
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest, not firing")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period after firing")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited and not firing")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)

    Defines the types of motor actions that can be produced by the NLM system,
    representing different behavioral and motor functions.
    
    Examples:
        >>> action_type = ActionType.MoveForward
        >>> print(action_type)
        ActionType.MoveForward
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the world")
        .value("MoveRight", ActionType::MoveRight, "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left in the world")
        .value("TurnRight", ActionType::TurnRight, "Turn right in the world")
        .value("Look", ActionType::Look, "Look forward")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with nearby objects")
        .value("Eat", ActionType::Eat, "Eat/consume objects")
        .value("Drink", ActionType::Drink, "Drink/consume liquid")
        .value("Rest", ActionType::Rest, "Rest and recover energy")
        .value("Wait", ActionType::Wait, "Wait without movement")
        .value("Custom", ActionType::Custom, "Custom action type")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)

    Defines the low-level motor commands that can be directly applied to the world,
    representing specific physical actions and movements.
    
    Examples:
        >>> command = MotorCommand.MoveForward
        >>> print(command)
        MotorCommand.MoveForward
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward in the world")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward in the world")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left in the world")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right in the world")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with nearby objects")
        .value("Wait", MotorCommand::Wait, "Wait without movement")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)

    Defines the types of objects that can exist in the NLM world simulation,
    representing different functional roles and properties of objects.
    
    Examples:
        >>> obj_type = WorldObjectType.Resource
        >>> print(obj_type)
        WorldObjectType.Resource
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space (no object)")
        .value("Resource", WorldObjectType::Resource, "Resource object (provides energy)")
        .value("Hazard", WorldObjectType::Hazard, "Hazard object (causes damage)")
        .value("Wall", WorldObjectType::Wall, "Wall object (blocks movement)")
        .value("Marker", WorldObjectType::Marker, "Marker object (goal/target)")
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)

    Provides configuration management for the NLM neural simulation framework.
    Supports loading and saving configuration from JSON files, command line arguments,
    and runtime settings. Manages configuration sources (default, file, command line, runtime)
    and provides type-safe value access.
    
    Examples:
        >>> config = Config.createDefaultConfig()
        >>> config.set("learning_rate", 0.01)
        >>> learning_rate = config.get("learning_rate", 0.1)
        >>> print(config.summary())
        "Config with 1 entries"
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw ValueError("File path cannot be empty");
            }
            if (!self.loadFromFile(filepath)) {
                throw ConfigError("Failed to load configuration from file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), R"pbdoc(Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file. Cannot be empty.
            
        Returns:
            bool: True if configuration loaded successfully.
            
        Raises:
            ValueError: If filepath is empty.
            ConfigError: If file loading fails.
        )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (argc <= 0) {
                throw ValueError("Argument count must be positive");
            }
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw ValueError("File path cannot be empty");
            }
            if (!self.saveToFile(filepath)) {
                throw ConfigError("Failed to save configuration to file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), "Save configuration to a JSON file")
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
        }, "Return string representation of the configuration");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)

    Abstract base class representing sensory input to the NLM brain system.
    Provides interface for accessing sensory data, dimensions, and timestamps.
    Subclasses include Vision, Audio, and InternalSignals.
    
    Examples:
        >>> # Create vision input
        >>> vision = Vision(32, 32)
        >>> vision.setData([0.1, 0.2, 0.3, ...])  # flattened array
        >>> data = vision.getData()
        >>> timestamp = vision.getTimestamp()
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)

    Represents visual sensory input as a 2D array of pixel values.
    Used by the NLM brain to process visual information from the environment.
    
    Examples:
        >>> vision = Vision(64, 48, 3)  # 64x48 RGB image
        >>> vision.configure(64, 48, 3)
        >>> vision.setData(pixel_array)  # flattened RGB array
        >>> width, height, channels = vision.getWidth(), vision.getHeight(), vision.getChannels()
        >>> data = vision.getData()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create a vision sensory input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValueError("Vision data cannot be empty");
            }
            const size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expectedSize) {
                throw ValueError("Vision data size mismatch. Expected: " + std::to_string(expectedSize) +
                               ", Got: " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set vision data with validation.
        
        Args:
            data: Flattened array of pixel values. Size must match width x height x channels.
            
        Raises:
            ValueError: If data is empty or size doesn't match expected dimensions.
        )pbdoc")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels (typically 1-3)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)

    Represents audio sensory input as a time series of sound samples.
    Used by the NLM brain to process auditory information from the environment.
    
    Examples:
        >>> audio = Audio(44100, 1024)  # 1 second of audio at 44.1kHz
        >>> audio.setSampleRate(48000)
        >>> audio.setData(audio_samples)  # array of float samples
        >>> sample_rate = audio.getSampleRate()
        >>> num_samples = audio.getNumSamples()
        >>> data = audio.getData()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an audio sensory input with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValueError("Audio data cannot be empty");
            }
            const size_t expectedSize = self.getNumSamples();
            if (data.size() != expectedSize) {
                throw ValueError("Audio data size mismatch. Expected: " + std::to_string(expectedSize) +
                               ", Got: " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data with validation.
        
        Args:
            data: Array of audio samples. Size must match number of samples.
            
        Raises:
            ValueError: If data is empty or size doesn't match expected number of samples.
        )pbdoc")
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            if (sampleRate == 0) {
                throw ValueError("Sample rate must be positive");
            }
            if (sampleRate > 384000) {
                throw ValueError("Sample rate too high: " + std::to_string(sampleRate));
            }
            self.setSampleRate(sampleRate);
        }, py::arg("sampleRate"), "Set audio sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate (Hz)")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)

    Represents internal neural signals as sensory input for the NLM brain.
    Used to monitor and integrate internal brain state into the perception-action loop.
    
    Examples:
        >>> internal = InternalSignals()
        >>> internal.addSignal(0.5)  # Add a signal value
        >>> internal.addSignal(-0.2)
        >>> internal.addSignal(0.8)
        >>> # Access signals through getData() method
        >>> signals = internal.getData()
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)

    Represents a motor action that can be produced by the NLM brain system.
    Contains action type, parameters, and utility methods for accessing action properties.
    Used in the perception-action loop to control agent behavior.
    
    Examples:
        >>> action = Action(ActionType.MoveForward)
        >>> action.setType(ActionType.MoveLeft)
        >>> action.setParameters([0.5, 0.3])  # movement parameters
        >>> action_type = action.getType()
        >>> params = action.getParameters()
        >>> name = action.getName()
        >>> cloned_action = action.clone()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create an action with the specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create an action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters as a vector")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable name of the action")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)

    Represents an object in the NLM world simulation.
    Contains position, type, and properties that define the object's role and behavior.
    Used in the world state to represent resources, hazards, walls, markers, etc.
    
    Examples:
        >>> obj = WorldObject(10.0, 20.0, WorldObjectType.Resource, 50.0)
        >>> obj.x, obj.y = 15.0, 25.0  # Set position
        >>> obj.type = WorldObjectType.Hazard  # Change object type
        >>> obj.value = -10.0  # Set damage value
        >>> obj.radius = 1.0  # Set object radius
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with position, type, and optional properties")
        .def_readwrite("x", &WorldObject::x, "X coordinate position")
        .def_readwrite("y", &WorldObject::y, "Y coordinate position")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of the object (see WorldObjectType enum)")
        .def_readwrite("value", &WorldObject::value, "Value (energy gain/damage) associated with the object")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active in the world");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)

    Represents the physical state of an agent in the NLM world simulation.
    Contains position, movement, energy, health, and age information.
    Used by the agent brain interface to track and control agent behavior.
    
    Examples:
        >>> agent = AgentBody()
        >>> agent.x, agent.y = 50.0, 50.0
        >>> agent.orientation = 90.0  # radians
        >>> agent.energy = 75.0
        >>> agent.health = 100.0
        >>> agent.age = 10
        >>> agent.isMoving = True
        >>> agent.isTurning = False
        >>> agent.reset()  # Reset to initial state
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate position")
        .def_readwrite("y", &AgentBody::y, "Y coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians (0 = east, π = west)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity (radians per timestep)")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-100+)")
        .def_readwrite("health", &AgentBody::health, "Current health level (0-100+)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset agent to initial state");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)

    Represents the result of applying a motor command in the NLM world simulation.
    Contains reward signal, success status, and optional message for logging.
    Used by the world simulation to provide feedback to the agent brain.
    
    Examples:
        >>> result = ActionResult(10.0, True, "Successfully moved to resource")
        >>> reward = result.reward
        >>> success = result.success
        >>> message = result.message
        >>> result2 = ActionResult(-5.0, False, "Hit wall")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result with reward, success status, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward signal (+positive reward, -negative penalty)")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about the action result");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)

    Represents sensory percepts as input to the NLM brain system.
    Contains vision, touch, internal signals, and proprioception data.
    Used by the agent brain interface to process sensory information.
    
    Examples:
        >>> percept = SensoryPercept()
        >>> vision = Vision(32, 32)
        >>> percept.setVision(vision)
        >>> percept.setTouch([0.1, 0.5, 0.9])
        >>> percept.setInternal([0.3, 0.7])
        >>> percept.setProprioception([0.5, 0.5])
        >>> audio = Audio(44100, 1024)
        >>> percept.setAudio(audio)
        >>> vision_data = percept.getVision()
        >>> touch_data = percept.getTouch()
        >>> internal_data = percept.getInternal()
        >>> proprioception_data = percept.getProprioception()
        >>> audio_data = percept.getAudio()
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision sensory data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision sensory data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch sensory data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal sensory data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal sensory data")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception sensory data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception sensory data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio sensory data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a combined vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, float width, float height, size_t visionWidth, size_t visionHeight) {
            // Validate configuration parameters
            if (width <= 0.0f) {
                throw ValueError("World width must be positive, got: " + std::to_string(width));
            }
            if (height <= 0.0f) {
                throw ValueError("World height must be positive, got: " + std::to_string(height));
            }
            if (visionWidth == 0 || visionWidth > 10000) {
                throw ValueError("Invalid vision width: " + std::to_string(visionWidth));
            }
            if (visionHeight == 0 || visionHeight > 10000) {
                throw ValueError("Invalid vision height: " + std::to_string(visionHeight));
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters with validation")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            // Validate position
            if (x < 0.0f || x > self.getWidth()) {
                throw ValueError("Invalid agent start x position: " + std::to_string(x) + ". Must be between 0 and " + std::to_string(self.getWidth()));
            }
            if (y < 0.0f || y > self.getHeight()) {
                throw ValueError("Invalid agent start y position: " + std::to_string(y) + ". Must be between 0 and " + std::to_string(self.getHeight()));
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent starting position with validation")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"), "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            // Validate object
            if (obj.radius <= 0.0f) {
                throw ValueError("World object radius must be positive, got: " + std::to_string(obj.radius));
            }
            if (obj.x < 0.0f || obj.x > self.getWidth()) {
                throw ValueError("Invalid object x position: " + std::to_string(obj.x) + ". Must be between 0 and " + std::to_string(self.getWidth()));
            }
            if (obj.y < 0.0f || obj.y > self.getHeight()) {
                throw ValueError("Invalid object y position: " + std::to_string(obj.y) + ". Must be between 0 and " + std::to_string(self.getHeight()));
            }
            self.addObject(obj);
        }, py::arg("obj"), "Add object to world with validation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"), "Get object at position (if any)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed for reproducibility")
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
        .def("injectCurrent", [](Brain& self, NeuronId neuron, MembranePotential current) {
            // Validate inputs
            if (!neuron || neuron.value == 0) {
                throw ValueError("Invalid neuron ID: " + std::to_string(neuron.value));
            }
            if (current < -1000.0f || current > 1000.0f) {
                throw ValueError("Current value out of reasonable range: " + std::to_string(current));
            }
            self.injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron with validation")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, MembranePotential current) {
            // Validate inputs
            if (current < -1000.0f || current > 1000.0f) {
                throw ValueError("Current value out of reasonable range: " + std::to_string(current));
            }
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type with validation")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", [](Brain& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw ValueError("File path cannot be empty");
            }
            if (!self.save(filepath)) {
                throw BrainError("Failed to save brain state to file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), "Save brain state to file with validation")
        .def("load", [](Brain& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw ValueError("File path cannot be empty");
            }
            if (!self.load(filepath)) {
                throw BrainError("Failed to load brain state from file: " + filepath);
            }
            return true;
        }, py::arg("filepath"), "Load brain state from file with validation")
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
        .def("initialize", [](AgentBrain& self, const SimpleWorld& world) {
            // Validate world
            if (world.getWidth() <= 0.0f || world.getHeight() <= 0.0f) {
                throw WorldError("Invalid world dimensions: width=" + std::to_string(world.getWidth()) + ", height=" + std::to_string(world.getHeight()));
            }
            self.initialize(world);
        }, py::arg("world"), "Initialize with world with validation")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            // Validate reward values
            if (std::isnan(reward) || std::isinf(reward)) {
                throw ValueError("Reward value must be a finite number, got: " + std::to_string(reward));
            }
            if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
                throw ValueError("Predicted reward must be a finite number, got: " + std::to_string(predictedReward));
            }
            if (std::abs(reward) > 1000.0f || std::abs(predictedReward) > 1000.0f) {
                throw ValueError("Reward values too large. Must be between -1000 and 1000");
            }
            self.applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation with validation")
        .def("updateDevelopment", [](AgentBrain& self, double timestep) {
            // Validate timestep
            if (timestep <= 0.0) {
                throw ValueError("Timestep must be positive, got: " + std::to_string(timestep));
            }
            if (timestep > 1.0) {
                throw ValueError("Timestep too large. Maximum allowed: 1.0, got: " + std::to_string(timestep));
            }
            self.updateDevelopment(timestep);
        }, py::arg("timestep"), "Update development system with validation")
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
    }, "Create a default configuration with all default settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        // Apply common default configuration for easier setup
        world->configure(100.0f, 100.0f, 32, 32);
        return world;
    }, "Create a simple world with default dimensions (100x100) and vision (32x32)");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface for the given brain");

    // Enhanced convenience functions for common patterns
    
    m.def("createAgentSetup", []() -> std::tuple<std::shared_ptr<Config>, std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100.0f, 100.0f, 32, 32);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        brain->initialize();
        return std::make_tuple(config, brain, world, agentBrain);
    }, "Create a complete agent setup with brain, world, and agent brain initialized");

    m.def("createDevelopmentBrain", [](std::shared_ptr<Config> config, bool enableDevelopment = true,
                                    bool enablePlasticity = true, bool enableCuriosity = true) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (enableDevelopment) {
            // Enable development-related plasticity rules
            if (auto stdp = brain->getSTDP()) {
                stdp->enableDevelopment(true);
            }
        }
        if (enablePlasticity) {
            if (auto structPlast = brain->getStructuralPlasticity()) {
                structPlast->enable(true);
            }
        }
        if (enableCuriosity) {
            if (auto curiosity = brain->getCuriosity()) {
                curiosity->enable(true);
            }
        }
        return brain;
    }, py::arg("config"), py::arg("enableDevelopment") = true, py::arg("enablePlasticity") = true, py::arg("enableCuriosity") = true,
       "Create a brain with development features enabled for learning and adaptation");

    m.def("createWorldForAgent", [](float width, float height, size_t visionWidth, size_t visionHeight,
                                   float maxEnergy = 100.0f, float energyDecayRate = 0.01f,
                                   uint64_t seed = std::random_device{}()) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setMaxEnergy(maxEnergy);
        world->setEnergyDecayRate(energyDecayRate);
        world->setRandomSeed(seed);
        return world;
    }, py::arg("width") = 100.0f, py::arg("height") = 100.0f, py::arg("visionWidth") = 32, py::arg("visionHeight") = 32,
       py::arg("maxEnergy") = 100.0f, py::arg("energyDecayRate") = 0.01f, py::arg("seed") = std::random_device{}(),
       "Create a configurable world with specified dimensions and energy parameters");

    m.def("createVisionSensory", [](size_t width, size_t height, size_t channels = 3) -> std::shared_ptr<Vision> {
        return std::make_unique<Vision>(width, height, channels);
    }, py::arg("width") = 32, py::arg("height") = 32, py::arg("channels") = 3,
       "Create a vision sensory input with specified dimensions and channels");

    m.def("createAudioSensory", [](size_t sampleRate = 44100, size_t numSamples = 1024) -> std::shared_ptr<Audio> {
        return std::make_unique<Audio>(sampleRate, numSamples);
    }, py::arg("sampleRate") = 44100, py::arg("numSamples") = 1024,
       "Create an audio sensory input with specified sample rate and duration");

    m.def("createInternalSignalsSensory", []() -> std::shared_ptr<InternalSignals> {
        return std::make_unique<InternalSignals>();
    }, "Create internal signals sensory input for brain state monitoring");

    // Helper function to create brain with configuration from file
    m.def("createBrainFromFile", [](const std::string& configFile) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            throw ConfigError("Failed to load configuration from file: " + configFile);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("configFile"), "Create a brain with configuration loaded from a file");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
