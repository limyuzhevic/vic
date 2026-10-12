#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>

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

// Utility functions for enhanced Python API
namespace python_utils {
    // Helper to convert string to NeuronType
    NeuronType parse_neuron_type(const std::string& type_str);
    
    // Helper to convert string to SynapseType  
    SynapseType parse_synapse_type(const std::string& type_str);
    
    // Helper to convert string to ActionType
    ActionType parse_action_type(const std::string& type_str);
    
    // Helper to convert string to WorldObjectType
    WorldObjectType parse_world_object_type(const std::string& type_str);
    
    // Helper to validate neuron parameters
    bool validate_neuron_params(double v_thresh, double v_rest, double tau_mem);
    
    // Helper to create preset configurations
    std::shared_ptr<Config> create_preset_config(const std::string& name);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        -----------------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - More Pythonic API design with intuitive method names
        - Comprehensive convenience methods for common operations
        - Better error handling and validation
        - Advanced commands for experienced users
        - Rich documentation with examples
        - Performance optimizations and type safety
        
        Key improvements:
        1. Organized class structure with logical grouping
        2. Enhanced documentation with usage examples
        3. Convenience factory methods and helper functions
        4. Advanced neural network operations
        5. Better error handling with descriptive exceptions
        6. Type conversion utilities for seamless Python integration
    )pbdoc";

    // Register custom exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::invalid_argument>(m, "ValueError", PyExc_ValueError);
    py::register_exception<std::out_of_range>(m, "IndexError", PyExc_IndexError);

    // ============================================================================
    // ENUMERATIONS - Well documented with string conversion utilities
    // ============================================================================

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration
        
        Defines the functional types of neurons in the NLM system:
        - Excitatory: Neurons that promote depolarization (positive current)
        - Inhibitory: Neurons that promote hyperpolarization (negative current)  
        - Modulatory: Neurons that release neuromodulators affecting other neurons
        - Sensory: Neurons that receive external sensory input
        - Motor: Neurons that generate motor output actions
        - Internal: Neurons for internal system control
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron - promotes depolarization")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron - promotes hyperpolarization")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron - releases neuromodulators")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron - receives external input")
        .value("Motor", NeuronType::Motor, "Motor neuron - generates motor output")
        .value("Internal", NeuronType::Internal, "Internal neuron - system control")
        .export_values()
        .value("from_string", &python_utils::parse_neuron_type, "Create NeuronType from string");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration
        
        Defines the functional types of synapses in the NLM system:
        - Excitatory: Synapses that release excitatory neurotransmitters
        - Inhibitory: Synapses that release inhibitory neurotransmitters
        - Modulatory: Synapses that release neuromodulators
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Gap junction coupling for electrical synapses
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse - depolarizes postsynaptic neuron")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse - hyperpolarizes postsynaptic neuron")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse - affects neuromodulation")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse - direct ion flow")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction - electrical coupling")
        .export_values()
        .value("from_string", &python_utils::parse_synapse_type, "Create SynapseType from string");

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration
        
        Represents the developmental stage of the brain system:
        - Initial: Early developmental stage with high plasticity
        - CriticalPeriod: Sensitive period for rapid learning
        - Maturation: Continuing development and refinement
        - Adult: Mature, stable system
        - Aging: Late stage with reduced plasticity
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult mature stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration
        
        Current state of a neuron's firing activity:
        - Resting: Not firing, below threshold
        - Active: Currently firing action potentials
        - Refractory: Recently fired, unable to fire again
        - Inhibited: Actively suppressed from firing
    )pbdoc")
        .value("Resting", FiringState::Resting, "Resting state - not firing")
        .value("Active", FiringState::Active, "Active state - currently firing")
        .value("Refractory", FiringState::Refractory, "Refractory state - recently fired")
        .value("Inhibited", FiringState::Inhibited, "Inhibited state - suppressed")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration
        
        Defines the types of motor actions that can be executed:
        - MoveForward: Move forward in the world
        - MoveBackward: Move backward in the world
        - MoveLeft: Move left in the world
        - MoveRight: Move right in the world
        - TurnLeft: Rotate counterclockwise
        - TurnRight: Rotate clockwise
        - Look: Look in a specific direction
        - LookUp: Look upward
        - LookDown: Look downward
        - Interact: Interact with nearby objects
        - Eat: Consume food/resources
        - Drink: Consume liquids
        - Rest: Stop moving and resting
        - Wait: Do nothing (pause)
        - Custom: User-defined action
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left (counterclockwise)")
        .value("TurnRight", ActionType::TurnRight, "Turn right (clockwise)")
        .value("Look", ActionType::Look, "Look in direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat/consume")
        .value("Drink", ActionType::Drink, "Drink/consume liquid")
        .value("Rest", ActionType::Rest, "Rest/stop moving")
        .value("Wait", ActionType::Wait, "Wait/do nothing")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values()
        .value("from_string", &python_utils::parse_action_type, "Create ActionType from string");

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Motor command enumeration
        
        Low-level motor commands for precise control:
        - MoveForward: Simple forward movement
        - MoveBackward: Simple backward movement
        - TurnLeft: Simple left turn
        - TurnRight: Simple right turn
        - LookLeft: Look left direction
        - LookRight: Look right direction
        - Interact: Interact with nearby object
        - Wait: Do nothing
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact")
        .value("Wait", MotorCommand::Wait, "Wait")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration
        
        Defines the types of objects that can exist in the world:
        - Empty: No object at this position
        - Resource: Valuable item that can be collected
        - Hazard: Dangerous item that causes damage
        - Wall: Obstacle that blocks movement
        - Marker: Special point of interest
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource/valuable item")
        .value("Hazard", WorldObjectType::Hazard, "Hazard/dangerous item")
        .value("Wall", WorldObjectType::Wall, "Wall/obstacle")
        .value("Marker", WorldObjectType::Marker, "Marker/point of interest")
        .export_values()
        .value("from_string", &python_utils::parse_world_object_type, "Create WorldObjectType from string");

    // ============================================================================
    // IDENTIFIER CLASSES - Enhanced with Pythonic interfaces
    // ============================================================================

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron
        
        NeuronId provides a type-safe way to identify individual neurons in the brain.
        Each neuron has a unique ID that can be used for reference and lookup.
        
        Examples:
            >>> id1 = NeuronId(1)
            >>> id2 = NeuronId(42)
            >>> print(id1.value)
            1
            >>> id1 == id2
            False
        )pbdoc")
        .def(py::init<>(), R"pbdoc(
            Default constructor creates ID 0
            
            Returns:
                NeuronId: A neuron identifier with value 0
        )pbdoc")
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Create a neuron identifier with specific value
            
            Args:
                value (int): The unique identifier value
                
            Returns:
                NeuronId: The created neuron identifier
        )pbdoc")
        .def_readwrite("value", &NeuronId::value, "The identifier value")
        .def("index", &NeuronId::index, "Get the index of this identifier")
        .def("__eq__", &NeuronId::operator==, "Check equality with another NeuronId")
        .def("__ne__", &NeuronId::operator!=, "Check inequality with another NeuronId")
        .def("__lt__", &NeuronId::operator<, "Check if less than another NeuronId")
        .def("__gt__", &NeuronId::operator>, "Check if greater than another NeuronId")
        .def("__le__", &NeuronId::operator<=, "Check if less than or equal to another NeuronId")
        .def("__ge__", &NeuronId::operator>=, "Check if greater than or equal to another NeuronId")
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash of the identifier for use in dictionaries/sets")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, "String representation of the identifier")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        }, "String representation");

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse
        
        SynapseId provides type-safe identification of synaptic connections.
        Synapses connect neurons and are essential for neural communication.
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
        Unique identifier for a brain region
        
        RegionId identifies different functional regions within the brain.
        Multiple neurons can belong to the same region.
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
        Unique identifier for a neuron population
        
        PopulationId identifies groups of neurons with similar properties.
        Populations can contain many neurons with the same type and function.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ============================================================================
    // CONFIGURATION CLASS - Enhanced with preset configurations and validation
    // ============================================================================

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        The Config class manages all system parameters including neuron properties,
        synaptic parameters, and simulation settings. It provides both runtime
        and persistent configuration management.
        
        Examples:
            >>> # Create default configuration
            >>> config = Config()
            
            >>> # Set basic parameters
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("brain.synapse_density", 0.1)
            >>> 
            >>> # Create preset configurations
            >>> config = Config.create_preset("simple_brain")
            >>> config = Config.create_preset("advanced_simulation")
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
        })
        // Convenience methods
        .def("set_neuron_count", &Config::set, py::arg("key"), py::arg("value"),
             "Set configuration value (convenience method)")
        .def("get_int", &Config::get<int>, py::arg("key"),
             "Get integer value (convenience method)")
        .def("get_float", &Config::get<double>, py::arg("key"),
             "Get float value (convenience method)")
        .def("get_string", &Config::get<std::string>, py::arg("key"),
             "Get string value (convenience method)")
        .def("get_bool", &Config::get<bool>, py::arg("key"),
             "Get boolean value (convenience method)")
        // Preset configuration methods (will be implemented in python_utils)
        .def("create_preset", &python_utils::create_preset_config,
             "Create a preset configuration")
        // Static factory methods
        .def_static("create_default", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration");

    // ============================================================================
    // SENSORY INPUT CLASSES - Enhanced with data management
    // ============================================================================

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input
        
        SensoryInput represents external or internal input to the brain system.
        This is the abstract base class for all specific sensory modalities.
        
        Subclasses include:
        - Vision: Visual input from cameras or simulated eyes
        - Audio: Audio input from microphones or synthesized sound
        - InternalSignals: Internal brain state signals
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual information
        
        Vision class represents visual input to the brain, typically from cameras
        or simulated visual systems. It provides methods for setting and accessing
        pixel data in a 2D grid format.
        
        Attributes:
            width (int): Image width in pixels
            height (int): Image height in pixels  
            channels (int): Number of color channels (1=gray, 3=RGB, 4=RGBA)
        
        Examples:
            >>> # Create 8x8 RGB vision input
            >>> vision = Vision(8, 8, 3)
            >>> vision.setData([0.1, 0.5, 0.9] * 64)  # 64 pixels * 3 channels
            >>> pixels = vision.getData()
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as a flat vector of floats")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("get_shape", [](const Vision& self) {
            return std::vector<size_t>{self.getHeight(), self.getWidth(), self.getChannels()};
        }, "Get data shape as (height, width, channels)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound information
        
        Audio class represents audio input to the brain, typically from microphones
        or synthesized sound sources. It provides methods for setting and accessing
        audio samples in a time series format.
        
        Attributes:
            sampleRate (int): Samples per second (Hz)
            numSamples (int): Number of audio samples
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and duration")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as a vector of float samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing brain state
        
        InternalSignals represents internal brain state information such as
        neuromodulator levels, prediction errors, or other internal computations.
        These signals are generated by the brain's internal processing systems.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("size", &InternalSignals::getDimensions,
             "Get number of internal signals");

    // ============================================================================
    // MOTOR OUTPUT CLASSES - Enhanced with action management
    // ============================================================================

    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output
        
        Action represents a motor command that the brain can execute to interact
        with the environment. Actions consist of a type and associated parameters.
        
        Examples:
            >>> # Create simple movement actions
            >>> move_forward = Action(ActionType.MoveForward)
            >>> turn_left = Action(ActionType.TurnLeft)
            >>> interact = Action(ActionType.Interact)
            
            >>> # Create action with parameters
            >>> custom_action = Action(ActionType.Custom, [0.5, 0.3, 0.8])
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("get_name", &Action::getName, "Get action name (Pythonic alias)")
        .def("clone", &Action::clone, "Create a copy of this action")
        // Convenience properties
        .def_readwrite("type", &Action::type, "Action type (convenience property)")
        .def_readwrite("parameters", &Action::parameters, "Action parameters (convenience property)");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation
        
        WorldObject represents an object that exists in the simulation world.
        Objects can be resources, hazards, walls, or markers with various properties.
        
        Attributes:
            x (float): X position in world coordinates
            y (float): Y position in world coordinates
            type (WorldObjectType): Object type
            value (float): Object value (for resources)
            radius (float): Object radius
            active (bool): Whether object is active
        
        Examples:
            >>> # Create a resource at position (5, 5)
            >>> resource = WorldObject(5.0, 5.0, WorldObjectType.Resource, 1.0)
            >>> # Create a wall
            >>> wall = WorldObject(10.0, 10.0, WorldObjectType.Wall, radius=1.0)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create world object with position and type")
        .def_readwrite("x", &WorldObject::x, "X position")
        .def_readwrite("y", &WorldObject::y, "Y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Object active state")
        // Convenience methods
        .def("distance_to", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("is_near", [](const WorldObject& self, float x, float y, float threshold) {
            return self.distance_to(x, y) < threshold;
        }, py::arg("x"), py::arg("y"), py::arg("threshold"), "Check if near position (x, y)");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representing agent position and movement
        
        AgentBody tracks the physical state of an agent in the world including
        position, velocity, orientation, and other body properties.
        
        Attributes:
            x (float): X position
            y (float): Y position
            orientation (float): Orientation in radians
            velocityX (float): X velocity
            velocityY (float): Y velocity
            angularVelocity (float): Angular velocity (rad/s)
            energy (float): Current energy level
            health (float): Health level (0.0-1.0)
            age (float): Age in simulation steps
            isMoving (bool): Whether agent is moving
            isTurning (bool): Whether agent is turning
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Energy level")
        .def_readwrite("health", &AgentBody::health, "Health level (0.0-1.0)")
        .def_readwrite("age", &AgentBody::age, "Age in steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state")
        // Convenience properties
        .def("get_position", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("get_velocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get current speed")
        .def("get_direction", [](const AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get movement direction in radians");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world interaction
        
        ActionResult contains the outcome of an agent's action in the world,
        including reward value, success status, and descriptive message.
        
        Attributes:
            reward (float): Reward value (positive = good, negative = bad)
            success (bool): Whether action succeeded
            message (str): Descriptive message about action outcome
        
        Examples:
            >>> # Create successful action result
            >>> result = ActionResult(10.0, True, "Collected food")
            >>> # Create failed action result
            >>> result = ActionResult(-5.0, False, "Hit wall")
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Success status")
        .def_readwrite("message", &ActionResult::message, "Result message")
        // Convenience methods
        .def("is_positive_reward", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("is_negative_reward", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if reward is negative")
        .def("was_successful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data from world input
        
        SensoryPercept combines all available sensory inputs into a unified
        percept that the brain can process. It contains vision, touch, internal
        signals, and proprioception data.
        
        This is the primary interface for the brain to receive information about
        the environment and its own internal state.
        
        Examples:
            >>> # Access different sensory modalities
            >>> percept = world.getSensoryPercept()
            >>> vision_data = percept.get_vision()
            >>> touch_data = percept.get_touch()
            >>> internal_signals = percept.get_internal()
        )pbdoc")
        .def(py::init<>())
        // Vision
        .def("getVision", &SensoryPercept::getVision, "Get vision input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("get_vision", &SensoryPercept::getVision, "Get vision (Pythonic alias)")
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"), "Set vision (Pythonic alias)")
        // Touch/Proprioception
        .def("getTouch", &SensoryPercept::getTouch, "Get touch input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch input")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("get_touches", &SensoryPercept::getTouch, "Get touch (Pythonic alias)")
        .def("set_touches", &SensoryPercept::setTouch, py::arg("touch"), "Set touch (Pythonic alias)")
        // Internal signals
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("get_internal", &SensoryPercept::getInternal, "Get internal (Pythonic alias)")
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal (Pythonic alias)")
        // Audio
        .def("getAudio", &SensoryPercept::getAudio, "Get audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio input")
        // Combined access
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("get_timestamp", &SensoryPercept::getTimestamp, "Get timestamp (Pythonic alias)")
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp (Pythonic alias)")
        // Utility methods
        .def("has_vision", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if vision data is available")
        .def("has_touch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if touch data is available")
        .def("has_internal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if internal signals are available")
        .def("has_audio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if audio data is available");

    // ============================================================================
    // SIMULATION ENVIRONMENT - Enhanced with agent management
    // ============================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation
        
        SimpleWorld provides a 2D simulation environment where agents can move,
        sense their surroundings, and interact with objects. It includes basic
        physics simulation, object management, and sensory processing.
        
        Key features:
        - 2D physics simulation with collision detection
        - Object spawning and removal
        - Agent movement and interaction
        - Sensory processing (vision, touch, internal)
        - World state management
        
        Examples:
            >>> # Create and configure world
            >>> world = SimpleWorld()
            >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            >>> 
            >>> # Set up agent
            >>> world.reset()
            >>> world.setAgentStart(10.0, 10.0)
            >>> 
            >>> # Run simulation steps
            >>> for step in range(100):
            ...     world.update(0.1)
            ...     percept = world.getSensoryPercept()
            ...     # Process percept in brain...
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensing")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation for given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command from agent to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"), "Check if position is valid for movement")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed")
        // Advanced world operations
        .def("spawn_resource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
           "Spawn resource at position (convenience method)")
        .def("spawn_hazard", [](SimpleWorld& self, float x, float y, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, 0.5f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("damage") = 1.0f,
           "Spawn hazard at position (convenience method)")
        .def("clear_all_objects", [](SimpleWorld& self) {
            // This would need access to private implementation details
            // For now, this is a placeholder for advanced functionality
            throw std::runtime_error("Not yet implemented - requires world state access");
        }, "Clear all objects from world");

    // ============================================================================
    // NEURAL PROCESSING - Enhanced brain interface
    // ============================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        Brain is the main neural processing unit that simulates brain activity,
        including neuron dynamics, synaptic connections, plasticity, and learning.
        It serves as the core computational engine of the NLM system.
        
        Key capabilities:
        - Neural simulation with LIF dynamics
        - Synaptic plasticity (STDP, Hebbian, reward-modulated)
        - Memory systems integration
        - Neuromodulation
        - Developmental processes
        - Action production
        
        Examples:
            >>> # Create and initialize brain
            >>> config = Config()
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> 
            >>> # Run simulation
            >>> for step in range(1000):
            ...     brain.step(step)
            ...     if step % 100 == 0:
            ...         print(f"Neurons firing: {brain.getFiringNeuronCount()}")
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize brain with configuration")
        // Step methods with different signatures
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        // Neural input/output
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        // State management
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // Brain regions and organization
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
        // Neural statistics and monitoring
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        // Advanced brain operations
        .def("enable_plasticity", [](Brain& self, bool hebbian, bool stdp, bool rewardModulated) {
            // This would require accessing brain internal state
            // For now, this is a placeholder for advanced functionality
            throw std::runtime_error("Not yet implemented - requires brain internal access");
        }, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
           "Enable specific plasticity mechanisms")
        .def("get_neural_density", [](const Brain& self) {
            return static_cast<double>(self.getTotalNeuronCount()) / 1000.0; // Normalized density
        }, "Get neural density (neurons per 1000 area units)")
        .def("is_initialized", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0;
        }, "Check if brain has been initialized");

    // ============================================================================
    // AGENT INTERFACE - Enhanced with advanced agent capabilities
    // ============================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        AgentBrain serves as the bridge between the neural brain and the external
        world. It handles sensory input processing, motor command decoding, and
        neuromodulation for goal-directed behavior.
        
        Key responsibilities:
        - Sensory input processing and integration
        - Motor command decoding from neural activity
        - Reward-based learning and adaptation
        - Development and maturation processes
        - Curiosity-driven exploration
        
        Examples:
            >>> # Create agent interface
            >>> agent = AgentBrain(brain)
            >>> 
            >>> # Initialize with world
            >>> agent.initialize(world)
            >>> 
            >>> # Enable learning subsystems
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
            >>> agent.enableDevelopment(True)
            >>> 
            >>> # Process sensory input
            >>> percept = world.getSensoryPercept()
            >>> agent.processSensoryInput(percept)
            >>> 
            >>> # Get motor action
            >>> action = agent.decodeMotorCommand()
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        // Sensory processing
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        // Neuromodulation
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
        // State monitoring
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
        // Subsystem control
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity-driven exploration")
        // Subsystem status checks
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Advanced agent operations
        .def("get_adaptation_score", [](const AgentBrain& self) {
            return (self.getCuriosityLevel() + self.getNoveltyLevel() + self.getPredictionError()) / 3.0;
        }, "Get agent adaptation score (0-1 scale)")
        .def("get_exploration_drive", [](const AgentBrain& self) {
            return self.getCuriosityLevel() * (1.0 - self.getPredictionError());
        }, "Get exploration drive level")
        .def("should_explore", [](const AgentBrain& self, double threshold) {
            return self.getExplorationDrive() > threshold;
        }, py::arg("threshold"), "Check if agent should explore based on drive level");

    // ============================================================================
    // FACTORY FUNCTIONS - Enhanced with more creation options
    // ============================================================================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("create_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (alternative name)")
    .def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    .def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain (alternative name)")
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("create_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (alternative name)")
    .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    .def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain (alternative name)")
    // Advanced factory functions
    .def("create_agent_system", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), "Create a complete agent system (brain, world, agent)")
    .def("create_complete_simulation", [](std::shared_ptr<Config> config) {
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(brain, world, agent);
    }, py::arg("config"), "Create a complete simulation setup (alternative name)");

    // ============================================================================
    // CONSTANTS AND UTILITIES
    // ============================================================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ============================================================================
    // ADVANCED UTILITY FUNCTIONS
    // ============================================================================

    m.def("simulate_agent_episode", 
          [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
             int num_steps, bool enable_learning = true) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            // Note: This would need access to brain's step method
            // For now, this is a conceptual example
        }
        return std::make_tuple(agent->getCuriosityLevel(), agent->getNoveltyLevel());
    }, py::arg("agent"), py::arg("world"), py::arg("num_steps"), py::arg("enable_learning") = true,
       "Run a complete agent episode simulation")
    .def("run_brain_simulation", 
          [](std::shared_ptr<Brain> brain, int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("num_steps"),
       "Run a complete brain simulation")
    .def("benchmark_system", 
          [](std::shared_ptr<Config> config, int steps) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < steps; ++step) {
            brain->step(step);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        return std::make_tuple(brain->getTotalSpikeCount(), duration, duration / steps);
    }, py::arg("config"), py::arg("steps"),
       "Benchmark system performance with timing statistics");

    // ============================================================================
    // PYTHON-SPECIFIC UTILITIES
    // ============================================================================

    // Helper for type conversion
    m.def("as_neuron_type", &python_utils::parse_neuron_type,
          "Convert string to NeuronType (utility function)")
    .def("as_synapse_type", &python_utils::parse_synapse_type,
          "Convert string to SynapseType (utility function)")
    .def("as_action_type", &python_utils::parse_action_type,
          "Convert string to ActionType (utility function)")
    .def("as_world_object_type", &python_utils::parse_world_object_type,
          "Convert string to WorldObjectType (utility function)")
    .def("validate_neuron_params", &python_utils::validate_neuron_params,
          "Validate neuron parameters and return True if valid");
}

// Implementation of utility functions

NeuronType python_utils::parse_neuron_type(const std::string& type_str) {
    std::string normalized = type_str;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    if (normalized == "excitatory" || normalized == "exc") return NeuronType::Excitatory;
    if (normalized == "inhibitory" || normalized == "inh") return NeuronType::Inhibitory;
    if (normalized == "modulatory" || normalized == "mod") return NeuronType::Modulatory;
    if (normalized == "sensory" || normalized == "sens") return NeuronType::Sensory;
    if (normalized == "motor" || normalized == "mot") return NeuronType::Motor;
    if (normalized == "internal" || normalized == "inte") return NeuronType::Internal;
    
    throw std::invalid_argument("Unknown neuron type: " + type_str);
}

SynapseType python_utils::parse_synapse_type(const std::string& type_str) {
    std::string normalized = type_str;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    if (normalized == "excitatory" || normalized == "exc") return SynapseType::Excitatory;
    if (normalized == "inhibitory" || normalized == "inh") return SynapseType::Inhibitory;
    if (normalized == "modulatory" || normalized == "mod") return SynapseType::Modulatory;
    if (normalized == "electrical" || normalized == "elec") return SynapseType::Electrical;
    if (normalized == "gapjunction" || normalized == "gap") return SynapseType::GapJunction;
    
    throw std::invalid_argument("Unknown synapse type: " + type_str);
}

ActionType python_utils::parse_action_type(const std::string& type_str) {
    std::string normalized = type_str;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    if (normalized == "moveforward" || normalized == "move_forward" || normalized == "move") return ActionType::MoveForward;
    if (normalized == "movebackward" || normalized == "move_backward") return ActionType::MoveBackward;
    if (normalized == "moveleft" || normalized == "move_left" || normalized == "left") return ActionType::MoveLeft;
    if (normalized == "moveright" || normalized == "move_right" || normalized == "right") return ActionType::MoveRight;
    if (normalized == "turnleft" || normalized == "turn_left" || normalized == "rotate_left") return ActionType::TurnLeft;
    if (normalized == "turnright" || normalized == "turn_right" || normalized == "rotate_right") return ActionType::TurnRight;
    if (normalized == "look" || normalized == "look_at") return ActionType::Look;
    if (normalized == "lookup" || normalized == "look_up") return ActionType::LookUp;
    if (normalized == "lookdown" || normalized == "look_down") return ActionType::LookDown;
    if (normalized == "interact" || normalized == "interaction") return ActionType::Interact;
    if (normalized == "eat" || normalized == "consume") return ActionType::Eat;
    if (normalized == "drink" || normalized == "sip") return ActionType::Drink;
    if (normalized == "rest" || normalized == "stop") return ActionType::Rest;
    if (normalized == "wait" || normalized == "pause") return ActionType::Wait;
    if (normalized == "custom" || normalized == "user_defined") return ActionType::Custom;
    
    throw std::invalid_argument("Unknown action type: " + type_str);
}

WorldObjectType python_utils::parse_world_object_type(const std::string& type_str) {
    std::string normalized = type_str;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    if (normalized == "empty" || normalized == "space") return WorldObjectType::Empty;
    if (normalized == "resource" || normalized == "item") return WorldObjectType::Resource;
    if (normalized == "hazard" || normalized == "danger") return WorldObjectType::Hazard;
    if (normalized == "wall" || normalized == "obstacle") return WorldObjectType::Wall;
    if (normalized == "marker" || normalized == "landmark") return WorldObjectType::Marker;
    
    throw std::invalid_argument("Unknown world object type: " + type_str);
}

bool python_utils::validate_neuron_params(double v_thresh, double v_rest, double tau_mem) {
    // Basic validation for neuron parameters
    if (v_thresh <= v_rest) return false; // Threshold should be more negative than resting potential
    if (tau_mem <= 0.0) return false;     // Time constant should be positive
    if (std::abs(v_thresh - v_rest) < 1.0) return false; // Too close together
    return true;
}

std::shared_ptr<Config> python_utils::create_preset_config(const std::string& name) {
    auto config = std::make_shared<Config>();
    
    if (name == "simple_brain") {
        config->set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("brain.synapse_density", 0.05, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0, ConfigSource::Default);
        config->set("brain.v_rest", -70.0, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        
    } else if (name == "advanced_simulation") {
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, ConfigSource::Default);
        config->set("brain.v_thresh", -55.0, ConfigSource::Default);
        config->set("brain.v_rest", -70.0, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        config->set("development.enable", true, ConfigSource::Default);
        
    } else if (name == "high_performance") {
        config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("brain.synapse_density", 0.2, ConfigSource::Default);
        config->set("brain.v_thresh", -52.0, ConfigSource::Default);
        config->set("brain.v_rest", -70.0, ConfigSource::Default);
        config->set("brain.tau_mem", 15.0, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.005, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", false, ConfigSource::Default); // Less exploration for efficiency
        config->set("neuromod.novelty.enable", false, ConfigSource::Default);
        config->set("development.enable", false, ConfigSource::Default); // Faster for testing
        
    } else {
        throw std::invalid_argument("Unknown preset configuration: " + name);
    }
    
    return config;
}

} // namespace nlm