#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Config/Constants.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception for NLM-specific errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

// Input validation utilities
namespace validation {
    template<typename T>
    T validatePositive(const T& value, const std::string& paramName) {
        if (value <= 0) {
            throw NLMException(paramName + " must be positive, got: " + std::to_string(value));
        }
        return value;
    }
    
    template<typename T>
    T validateNonNegative(const T& value, const std::string& paramName) {
        if (value < 0) {
            throw NLMException(paramName + " must be non-negative, got: " + std::to_string(value));
        }
        return value;
    }
    
    float validateCurrent(const float& current, const std::string& paramName) {
        if (std::isnan(current) || std::isinf(current)) {
            throw NLMException(paramName + " must be a finite number, got: " + std::to_string(current));
        }
        return current;
    }
    
    double validateTimestep(const double& timestep) {
        if (timestep <= 0.0) {
            throw NLMException("Simulation timestep must be positive, got: " + std::to_string(timestep));
        }
        if (timestep > config::MAX_SIMULATION_WALL_TIME) {
            throw NLMException("Simulation timestep too large, maximum: " + std::to_string(config::MAX_SIMULATION_WALL_TIME) + ", got: " + std::to_string(timestep));
        }
        return timestep;
    }
    
    size_t validateNeuronCount(const size_t& count) {
        if (count == 0) {
            throw NLMException("Neuron count cannot be zero");
        }
        if (count > config::PHASE_6_DEMO_NEURON_COUNT * 10) { // Allow up to 10x the demo count
            throw NLMException("Neuron count too large, maximum: " + std::to_string(config::PHASE_6_DEMO_NEURON_COUNT * 10) + ", got: " + std::to_string(count));
        }
        return count;
    }
    
    size_t validateSteps(const size_t& steps) {
        if (steps == 0) {
            throw NLMException("Step count cannot be zero");
        }
        if (steps > config::MAX_PARALLEL_SIMULATIONS * 1000) { // Reasonable upper limit
            throw NLMException("Step count too large, maximum: " + std::to_string(config::MAX_PARALLEL_SIMULATIONS * 1000) + ", got: " + std::to_string(steps));
        }
        return steps;
    }
    
    template<typename T>
    T validateRange(const T& value, const T& min, const T& max, const std::string& paramName) {
        if (value < min || value > max) {
            throw NLMException(paramName + " must be between " + std::to_string(min) + " and " + std::to_string(max) + ", got: " + std::to_string(value));
        }
        return value;
    }
    
    // Convert exception to Python exception
    void convertException(py::object exc) {
        try {
            if (PyErr_Occurred()) {
                // Convert any existing Python exception
                PyErr_Print();
            }
        } catch (...) {
            // If we can't convert, just let it be
        }
    }
} // namespace validation

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This module provides access to advanced neural simulation capabilities including:
        - Brain simulation with spiking neurons
        - Real-time learning and plasticity
        - Memory systems (working, episodic, semantic)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction systems
        - Cognitive mechanisms (planning, attention, concept formation)
        - Developmental systems
        - Agent interfaces for world interaction
        
        Features:
        - Comprehensive error handling with descriptive exceptions
        - Input validation for all parameters
        - Type checking and safety
        - Enhanced documentation with examples
        - Performance optimizations
        
        Usage:
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> action = brain.produceAction()
    )pbdoc";

    // Register custom exception
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred in NLM");
        }
    });

    // NeuronId with enhanced validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)
    
    NeuronId represents a unique identifier for individual neurons in the neural network.
    Each neuron has a unique ID that allows precise targeting and manipulation.
    
    Attributes:
        value (int): The numeric identifier value
    
    Examples:
        >>> id1 = pynlm.NeuronId()
        >>> id2 = pynlm.NeuronId(42)
        >>> print(id2.value)
        42
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value,
            R"pbdoc(The numeric identifier value of the neuron)pbdoc")
        .def("index", &NeuronId::index,
            R"pbdoc(Get the index of the neuron identifier)
            
            Returns:
                int: The index component of the identifier
            )pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            std::stringstream ss;
            ss << "<NeuronId: " << id.value << ">";
            return ss.str();
        });

    // SynapseId with enhanced validation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)
    
    SynapseId represents a unique identifier for synaptic connections between neurons.
    Each synapse has a unique ID that allows precise targeting and manipulation.
    
    Attributes:
        value (int): The numeric identifier value
    
    Examples:
        >>> syn_id = pynlm.SynapseId(123)
        >>> print(syn_id.value)
        123
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value,
            R"pbdoc(The numeric identifier value of the synapse)pbdoc")
        .def("index", &SynapseId::index,
            R"pbdoc(Get the index of the synapse identifier)
            
            Returns:
                int: The index component of the identifier
            )pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            std::stringstream ss;
            ss << "<SynapseId: " << id.value << ">";
            return ss.str();
        });

    // RegionId with enhanced validation
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)
    
    RegionId represents a unique identifier for neural regions or areas in the brain.
    Each region has a unique ID that allows precise targeting and manipulation.
    
    Attributes:
        value (int): The numeric identifier value
    
    Examples:
        >>> region_id = pynlm.RegionId(5)
        >>> print(region_id.value)
        5
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value,
            R"pbdoc(The numeric identifier value of the region)pbdoc")
        .def("index", &RegionId::index,
            R"pbdoc(Get the index of the region identifier)
            
            Returns:
                int: The index component of the identifier
            )pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            std::stringstream ss;
            ss << "<RegionId: " << id.value << ">";
            return ss.str();
        });

    // PopulationId with enhanced validation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)
    
    PopulationId represents a unique identifier for groups of neurons that share
    similar characteristics or functions within the brain.
    
    Attributes:
        value (int): The numeric identifier value
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value,
            R"pbdoc(The numeric identifier value of the population)pbdoc")
        .def("index", &PopulationId::index,
            R"pbdoc(Get the index of the population identifier)
            
            Returns:
                int: The index component of the identifier
            )pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
    
    NeuronType defines the different categories of neurons in the NLM system,
    each with distinct functions and properties.
    
    Enum values:
        - Excitatory: Neurons that increase the likelihood of postsynaptic firing
        - Inhibitory: Neurons that decrease the likelihood of postsynaptic firing
        - Modulatory: Neurons that affect synaptic efficacy broadly
        - Sensory: Neurons that receive external input
        - Motor: Neurons that initiate actions
        - Internal: Neurons that maintain internal states
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    // SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
    
    SynapseType defines the different categories of synaptic connections in the NLM system,
    each with distinct mechanisms and effects.
    
    Enum values:
        - Excitatory: Synaptic connections that increase postsynaptic firing
        - Inhibitory: Synaptic connections that decrease postsynaptic firing
        - Modulatory: Synaptic connections that broadly affect neuronal activity
        - Electrical: Direct electrical coupling between neurons
        - GapJunction: Specialized electrical synapses
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse type")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse type")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse type")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse type")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse type")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
    
    DevelopmentalStage represents the different phases of brain development in the NLM system,
    reflecting how neural properties change over time.
    
    Enum values:
        - Initial: Newborn/initial state
        - CriticalPeriod: Period of heightened plasticity
        - Maturation: Refinement and specialization
        - Adult: Mature, stable state
        - Aging: Age-related changes
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
    
    FiringState represents the current activity level of neurons in the NLM system,
    reflecting their dynamic state during simulation.
    
    Enum values:
        - Resting: Not currently firing
        - Active: Currently firing action potentials
        - Refractory: Unable to fire due to recovery period
        - Inhibited: Actively suppressed from firing
    )pbdoc")
        .value("Resting", FiringState::Resting, "Resting firing state")
        .value("Active", FiringState::Active, "Active firing state")
        .value("Refractory", FiringState::Refractory, "Refractory firing state")
        .value("Inhibited", FiringState::Inhibited, "Inhibited firing state")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
    
    ActionType defines the different categories of actions that agents can perform
    in the NLM world simulation, from basic movements to complex interactions.
    
    Enum values:
        - MoveForward: Move forward in the world
        - MoveBackward: Move backward in the world
        - MoveLeft: Move left in the world
        - MoveRight: Move right in the world
        - TurnLeft: Rotate counterclockwise
        - TurnRight: Rotate clockwise
        - Look: Look in a direction
        - LookUp: Look upward
        - LookDown: Look downward
        - Interact: Interact with nearby objects
        - Eat: Consume food resources
        - Drink: Consume water resources
        - Rest: Stop moving and recover energy
        - Wait: Do nothing for a timestep
        - Custom: Custom action defined by user
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)
    
    MotorCommand defines the basic motor commands that can be executed by agents
    in the NLM world, representing the interface between neural activity and
    physical actions.
    
    Enum values:
        - MoveForward: Move forward
        - MoveBackward: Move backward
        - TurnLeft: Turn left
        - TurnRight: Turn right
        - LookLeft: Look left
        - LookRight: Look right
        - Interact: Interact with environment
        - Wait: Do nothing
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward motor command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward motor command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left motor command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right motor command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left motor command")
        .value("LookRight", MotorCommand::LookRight, "Look right motor command")
        .value("Interact", MotorCommand::Interact, "Interact motor command")
        .value("Wait", MotorCommand::Wait, "Wait motor command")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
    
    WorldObjectType defines the different categories of objects that exist in the NLM world,
    each with distinct properties and behaviors.
    
    Enum values:
        - Empty: Empty space
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Impassable barrier
        - Marker: Reference point or landmark
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty world object")
        .value("Resource", WorldObjectType::Resource, "Resource world object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard world object")
        .value("Wall", WorldObjectType::Wall, "Wall world object")
        .value("Marker", WorldObjectType::Marker, "Marker world object")
        .export_values();

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
    
    Config is the central configuration container for the NLM system, providing
    a flexible key-value storage mechanism for all simulation parameters.
    
    Features:
    - Type-safe configuration values
    - File-based configuration loading
    - Command-line override support
    - Validation and error handling
    - Serialization support
    
    The configuration system supports the following value types:
    - int64_t: 64-bit integer values
    - double: 64-bit floating point values  
    - float: 32-bit floating point values
    
    Common configuration keys include:
    - random_seed: Random number generator seed
    - simulation_timestep: Time step for simulation
    - neuron_count: Number of neurons in the network
    - region_count: Number of neural regions
    - connection_probability: Probability of synaptic connections
    
    Examples:
        >>> config = pynlm.createDefaultConfig()
        >>> config.set("neuron_count", 1000)
        >>> config.set("simulation_timestep", 0.001)
        >>> brain = pynlm.createBrain(config)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file\n\n"
             "Args:\n"
             "    filepath (str): Path to the configuration file\n\n"
             "Raises:\n"
             "    RuntimeError: If file cannot be read or parsed\n\n"
             "Examples:\n"
             "    >>> config = pynlm.createDefaultConfig()\n"
             "    >>> config.loadFromFile('configs/my_config.json')")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments\n\n"
           "Args:\n"
           "    argc (int): Number of command line arguments\n"
           "    argv (list): List of command line arguments\n\n"
           "Command line arguments support the format:\n"
           "    --config=file.json    Load configuration from file\n"
           "    --key=value           Set configuration value\n"
           "    --key value           Set configuration value\n\n"
           "Examples:\n"
           "    >>> config = pynlm.createDefaultConfig()\n"
           "    >>> config.loadFromArgs(5, ['--neurons=1000', '--timestep=0.001'])")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file\n\n"
             "Args:\n"
             "    filepath (str): Path where the configuration will be saved\n\n"
             "Raises:\n"
             "    RuntimeError: If file cannot be written\n\n"
             "Examples:\n"
             "    >>> config.saveToFile('my_config.json')")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists\n\n"
             "Args:\n"
             "    key (str): Configuration key to check\n\n"
             "Returns:\n"
             "    bool: True if the key exists, False otherwise\n\n"
             "Examples:\n"
             "    >>> config.has('neuron_count')\n"
             "    True")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys\n\n"
             "Returns:\n"
             "    list: List of all configuration keys\n\n"
             "Examples:\n"
             "    >>> keys = config.getKeys()\n"
             "    ['neuron_count', 'simulation_timestep', ...]")
        .def("clear", &Config::clear,
             "Clear all configuration entries\n\n"
             "This removes all configuration values, resetting to default state.\n\n"
             "Examples:\n"
             "    >>> config.clear()")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration\n\n"
             "Returns:\n"
             "    str: Human-readable summary of configuration values\n\n"
             "Examples:\n"
             "    >>> print(config.summary())")
        .def("__repr__", [](const Config& cfg) {
            std::stringstream ss;
            ss << "<Config: " << cfg.summary() << ">";
            return ss.str();
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
    
    SensoryInput represents sensory data received by agents from the environment,
    forming the primary input to the NLM cognitive system.
    
    This abstract base class defines the interface for all sensory modalities,
    including vision, audio, touch, and internal signals.
    
    Attributes:
        type (NeuronType): The type of sensory input
        timestamp (float): Time when the input was generated
        data (list): Raw sensory data as a list of floats
    
    Examples:
        >>> vision = pynlm.Vision(10, 10)
        >>> vision.setData([0.1, 0.5, 0.9, ...])
        >>> print(vision.getWidth())
        10
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input\n\n"
             "Returns:\n"
             "    NeuronType: The type of this sensory input\n\n"
             "Examples:\n"
             "    >>> input_type = sensory.getType()")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector\n\n"
             "Returns:\n"
             "    list: Raw sensory data as a list of floats\n\n"
             "Examples:\n"
             "    >>> data = sensory.getData()")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data\n\n"
             "Returns:\n"
             "    int: Number of dimensions in the sensory data\n\n"
             "Examples:\n"
             "    >>> dims = sensory.getDimensions()")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of the sensory input\n\n"
             "Returns:\n"
             "    float: Time when the input was generated\n\n"
             "Examples:\n"
             "    >>> time = sensory.getTimestamp()")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the sensory input\n\n"
             "Args:\n"
             "    timestamp (float): Time when the input was generated\n\n"
             "Raises:\n"
             "    ValueError: If timestamp is invalid\n\n"
             "Examples:\n"
             "    >>> sensory.setTimestamp(123.456)");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
    
    Vision represents visual sensory input, capturing the 2D visual field
    experienced by agents in the NLM world.
    
    Attributes:
        width (int): Horizontal resolution of the vision
        height (int): Vertical resolution of the vision
        channels (int): Number of color channels (typically 3 for RGB)
    
    Examples:
        >>> vision = pynlm.Vision(320, 240, 3)
        >>> print(vision.getWidth(), vision.getHeight(), vision.getChannels())
        (320, 240, 3)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create a vision sensory input with specified dimensions\n\n"
             "Args:\n"
             "    width (int): Horizontal resolution in pixels\n"
             "    height (int): Vertical resolution in pixels\n"
             "    channels (int): Number of color channels (default: 3)\n\n"
             "Raises:\n"
             "    ValueError: If dimensions are invalid\n\n"
             "Examples:\n"
             "    >>> vision = pynlm.Vision(640, 480)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            validation::validateRange(data.size(), size_t(1), config::PYTHON_MAX_SAMPLES_FOR_VALIDATION,
                "Vision data size");
            self.setData(data);
        }, py::arg("data"),
             "Set the visual data for this vision input\n\n"
             "Args:\n"
             "    data (list): List of pixel values as floats\n\n"
             "Raises:\n"
             "    ValueError: If data size is invalid or contains invalid values\n\n"
             "Examples:\n"
             "    >>> vision.setData([0.1, 0.5, 0.9, ...])")
        .def("getWidth", &Vision::getWidth,
             "Get the width of the vision in pixels\n\n"
             "Returns:\n"
             "    int: Horizontal resolution\n\n"
             "Examples:\n"
             "    >>> width = vision.getWidth()")
        .def("getHeight", &Vision::getHeight,
             "Get the height of the vision in pixels\n\n"
             "Returns:\n"
             "    int: Vertical resolution\n\n"
             "Examples:\n"
             "    >>> height = vision.getHeight()")
        .def("getChannels", &Vision::getChannels,
             "Get the number of color channels\n\n"
             "Returns:\n"
             "    int: Number of channels (typically 3 for RGB)\n\n"
             "Examples:\n"
             "    >>> channels = vision.getChannels()");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
    
    Audio represents auditory sensory input, capturing the sound environment
    experienced by agents in the NLM world.
    
    Attributes:
        sampleRate (int): Number of samples per second
        numSamples (int): Number of audio samples
    
    Examples:
        >>> audio = pynlm.Audio(44100, 1024)
        >>> print(audio.getSampleRate(), audio.getNumSamples())
        (44100, 1024)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create an audio sensory input with specified parameters\n\n"
             "Args:\n"
             "    sampleRate (int): Samples per second\n"
             "    numSamples (int): Number of audio samples\n\n"
             "Raises:\n"
             "    ValueError: If parameters are invalid\n\n"
             "Examples:\n"
             "    >>> audio = pynlm.Audio(48000, 2048)")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            validation::validateRange(data.size(), size_t(1), config::PYTHON_MAX_SAMPLES_FOR_VALIDATION,
                "Audio data size");
            self.setData(data);
        }, py::arg("data"),
             "Set the audio data for this audio input\n\n"
             "Args:\n"
             "    data (list): List of audio samples as floats\n\n"
             "Raises:\n"
             "    ValueError: If data size is invalid or contains invalid values\n\n"
             "Examples:\n"
             "    >>> audio.setData([0.1, 0.2, 0.3, ...])")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the sample rate of the audio\n\n"
             "Args:\n"
             "    sampleRate (int): Samples per second\n\n"
             "Raises:\n"
             "    ValueError: If sample rate is invalid\n\n"
             "Examples:\n"
             "    >>> audio.setSampleRate(44100)")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the sample rate of the audio\n\n"
             "Returns:\n"
             "    int: Samples per second\n\n"
             "Examples:\n"
             "    >>> rate = audio.getSampleRate()")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples\n\n"
             "Returns:\n"
             "    int: Number of samples\n\n"
             "Examples:\n"
             "    >>> samples = audio.getNumSamples()");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
    
    InternalSignals represents the internal state signals processed by agents,
    including proprioception, emotional states, and other internal variables.
    
    This sensory modality captures the agent's internal physiological and
    cognitive states, which are crucial for self-regulation and decision-making.
    
    Examples:
        >>> signals = pynlm.InternalSignals()
        >>> signals.addSignal(0.5)  # Add a signal value
        >>> signals.addSignal(0.8)  # Add another signal value
        >>> print(len(signals.getData()))
        2
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals\n\n"
             "Args:\n"
             "    value (float): Signal value to add\n\n"
             "Raises:\n"
             "    ValueError: If value is invalid\n\n"
             "Examples:\n"
             "    >>> signals.addSignal(0.7)")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals\n\n"
             "This removes all stored signal values.\n\n"
             "Examples:\n"
             "    >>> signals.clearSignals()");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
    
    Action represents a motor command that can be executed by agents,
    translating neural activity into physical actions in the environment.
    
    Actions consist of a type and parameters that define the specific
    movement or interaction to be performed.
    
    Attributes:
        type (ActionType): The type of action to perform
        parameters (list): Action parameters as a list of floats
    
    Examples:
        >>> move_action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0])
        >>> print(move_action.getType())
        ActionType.MoveForward
        >>> print(move_action.getParameters())
        [1.0]
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create an action with only type\n\n"
             "Args:\n"
             "    type (ActionType): The type of action\n\n"
             "Examples:\n"
             "    >>> action = pynlm.Action(pynlm.ActionType.Rest)")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create a fully specified action\n\n"
             "Args:\n"
             "    type (ActionType): The type of action\n"
             "    parameters (list): Action parameters\n\n"
             "Examples:\n"
             "    >>> action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0])")
        .def("getType", &Action::getType,
             "Get the type of this action\n\n"
             "Returns:\n"
             "    ActionType: The type of the action\n\n"
             "Examples:\n"
             "    >>> action_type = action.getType()")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the type of this action\n\n"
             "Args:\n"
             "    type (ActionType): The new action type\n\n"
             "Examples:\n"
             "    >>> action.setType(pynlm.ActionType.MoveLeft)")
        .def("getParameters", &Action::getParameters,
             "Get the parameters of this action\n\n"
             "Returns:\n"
             "    list: Action parameters as a list of floats\n\n"
             "Examples:\n"
             "    >>> params = action.getParameters()")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the parameters of this action\n\n"
             "Args:\n"
             "    params (list): New action parameters\n\n"
             "Examples:\n"
             "    >>> action.setParameters([0.5, 0.2])")
        .def("getName", &Action::getName,
             "Get the human-readable name of this action type\n\n"
             "Returns:\n"
             "    str: Human-readable action name\n\n"
             "Examples:\n"
             "    >>> name = action.getName()")
        .def("clone", &Action::clone,
             "Create a copy of this action\n\n"
             "Returns:\n"
             "    Action: A copy of this action\n\n"
             "Examples:\n"
             "    >>> action_copy = action.clone()");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
    
    WorldObject represents an object in the NLM world environment, such as
    resources, hazards, walls, or markers.
    
    Each world object has a position, type, and properties that define
    how it interacts with agents in the world.
    
    Attributes:
        x (float): X-coordinate position
        y (float): Y-coordinate position
        radius (float): Size of the object
        type (WorldObjectType): Type of the object
        value (float): Additional value (e.g., resource amount)
        active (bool): Whether the object is currently active
    
    Examples:
        >>> resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
        >>> print(resource.x, resource.y, resource.type)
        5.0 5.0 WorldObjectType.Resource
    )pbdoc")
        """
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with specified properties\n\n"
             "Args:\n"
             "    x (float): X-coordinate position\n"
             "    y (float): Y-coordinate position\n"
             "    type (WorldObjectType): Type of the object\n"
             "    value (float): Additional value (default: 0.0)\n"
             "    radius (float): Size of the object (default: 0.5)\n\n"
             "Examples:\n"
             "    >>> wall = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Wall)")
        .def_readwrite("x", &WorldObject::x,
            "X-coordinate position of the object\n\n"
            "Set/Get the x-coordinate of the object's position.\n\n"
            "Examples:\n"
            "    >>> obj.x = 10.0\n"
            "    >>> print(obj.x)")
        .def_readwrite("y", &WorldObject::y,
            "Y-coordinate position of the object\n\n"
            "Set/Get the y-coordinate of the object's position.\n\n"
            "Examples:\n"
            "    >>> obj.y = 10.0\n"
            "    >>> print(obj.y)")
        .def_readwrite("radius", &WorldObject::radius,
            "Radius of the object\n\n"
            "Set/Get the radius of the object.\n\n"
            "Examples:\n"
            "    >>> obj.radius = 1.0\n"
            "    >>> print(obj.radius)")
        .def_readwrite("type", &WorldObject::type,
            "Type of the object\n\n"
            "Set/Get the type of the object.\n\n"
            "Examples:\n"
            "    >>> obj.type = pynlm.WorldObjectType.Resource\n"
            "    >>> print(obj.type)")
        .def_readwrite("value", &WorldObject::value,
            "Additional value of the object\n\n"
            "Set/Get the additional value of the object (e.g., resource amount).\n\n"
            "Examples:\n"
            "    >>> obj.value = 100.0\n"
            "    >>> print(obj.value)")
        .def_readwrite("active", &WorldObject::active,
            "Active status of the object\n\n"
            "Set/Get whether the object is currently active.\n\n"
            "Examples:\n"
            "    >>> obj.active = True\n"
            "    >>> print(obj.active)");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
    
    AgentBody represents the physical state of an agent in the NLM world,
    including position, orientation, velocity, and physiological parameters.
    
    This class tracks the agent's complete body state, including movement,
    orientation, and energy levels, which are used for action execution
    and reward calculation.
    
    Attributes:
        x (float): X-coordinate position
        y (float): Y-coordinate position
        orientation (float): Heading angle in radians
        velocityX (float): X-component of velocity
        velocityY (float): Y-component of velocity
        angularVelocity (float): Angular velocity in radians per second
        energy (float): Current energy level
        health (float): Current health level
        age (float): Age in simulation steps
        isMoving (bool): Whether the agent is currently moving
        isTurning (bool): Whether the agent is currently turning
        lastActionTime (float): Time since last action
    
    Examples:
        >>> body = pynlm.AgentBody()
        >>> print(body.x, body.y, body.orientation)
        0.0 0.0 0.0
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
            "X-coordinate position\n\n"
            "Set/Get the x-coordinate position of the agent.\n\n"
            "Examples:\n"
            "    >>> body.x = 5.0\n"
            "    >>> print(body.x)")
        .def_readwrite("y", &AgentBody::y,
            "Y-coordinate position\n\n"
            "Set/Get the y-coordinate position of the agent.\n\n"
            "Examples:\n"
            "    >>> body.y = 5.0\n"
            "    >>> print(body.y)")
        .def_readwrite("orientation", &AgentBody::orientation,
            "Orientation angle in radians\n\n"
            "Set/Get the agent's heading angle (0 = +X axis, π/2 = +Y axis).\n\n"
            "Examples:\n"
            "    >>> body.orientation = 1.57\n"
            "    >>> print(body.orientation)")
        .def_readwrite("velocityX", &AgentBody::velocityX,
            "X-velocity component\n\n"
            "Set/Get the x-component of the agent's velocity.\n\n"
            "Examples:\n"
            "    >>> body.velocityX = 1.0\n"
            "    >>> print(body.velocityX)")
        .def_readwrite("velocityY", &AgentBody::velocityY,
            "Y-velocity component\n\n"
            "Set/Get the y-component of the agent's velocity.\n\n"
            "Examples:\n"
            "    >>> body.velocityY = 1.0\n"
            "    >>> print(body.velocityY)")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
            "Angular velocity in radians per second\n\n"
            "Set/Get the agent's angular velocity.\n\n"
            "Examples:\n"
            "    >>> body.angularVelocity = 0.5\n"
            "    >>> print(body.angularVelocity)")
        .def_readwrite("energy", &AgentBody::energy,
            "Energy level\n\n"
            "Set/Get the agent's current energy level (0-1).\n\n"
            "Examples:\n"
            "    >>> body.energy = 0.8\n"
            "    >>> print(body.energy)")
        .def_readwrite("health", &AgentBody::health,
            "Health level\n\n"
            "Set/Get the agent's current health level (0-1).\n\n"
            "Examples:\n"
            "    >>> body.health = 1.0\n"
            "    >>> print(body.health)")
        .def_readwrite("age", &AgentBody::age,
            "Age in simulation steps\n\n"
            "Set/Get the agent's age in simulation steps.\n\n"
            "Examples:\n"
            "    >>> body.age = 100\n"
            "    >>> print(body.age)")
        .def_readwrite("isMoving", &AgentBody::isMoving,
            "Whether the agent is moving\n\n"
            "Set/Get whether the agent is currently moving.\n\n"
            "Examples:\n"
            "    >>> body.isMoving = True\n"
            "    >>> print(body.isMoving)")
        .def_readwrite("isTurning", &AgentBody::isTurning,
            "Whether the agent is turning\n\n"
            "Set/Get whether the agent is currently turning.\n\n"
            "Examples:\n"
            "    >>> body.isTurning = True\n"
            "    >>> print(body.isTurning)")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
            "Time since last action\n\n"
            "Set/Get the time elapsed since the agent's last action.\n\n"
            "Examples:\n"
            "    >>> body.lastActionTime = 10.0\n"
            "    >>> print(body.lastActionTime)")
        .def("reset", &AgentBody::reset,
             "Reset the agent body to initial state\n\n"
             "This resets all body state variables to their default values.\n\n"
             "Examples:\n"
             "    >>> body.reset()");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
    
    ActionResult represents the outcome of executing an action in the NLM world,
    including reward information and success status.
    
    This class captures the feedback an agent receives from the environment
    after performing an action, which is used for learning and decision-making.
    
    Attributes:
        reward (float): Reward received from the action
        success (bool): Whether the action was successful
        message (str): Human-readable description of the result
    
    Examples:
        >>> result = pynlm.ActionResult(10.0, True, "Successfully moved forward")
        >>> print(result.reward, result.success)
        10.0 True
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result\n\n"
             "Args:\n"
             "    reward (float): Reward received from the action\n"
             "    success (bool): Whether the action was successful\n"
             "    message (str): Description of the result (default: \"\")\n\n"
             "Examples:\n"
             "    >>> result = pynlm.ActionResult(5.0, True)")
        .def_readwrite("reward", &ActionResult::reward,
            "Reward received from the action\n\n"
            "Set/Get the reward value.\n\n"
            "Examples:\n"
            "    >>> result.reward = 10.0\n"
            "    >>> print(result.reward)")
        .def_readwrite("success", &ActionResult::success,
            "Success status of the action\n\n"
            "Set/Get whether the action was successful.\n\n"
            "Examples:\n"
            "    >>> result.success = True\n"
            "    >>> print(result.success)")
        .def_readwrite("message", &ActionResult::message,
            "Description of the result\n\n"
            "Set/Get the human-readable description.\n\n"
            "Examples:\n"
            "    >>> result.message = \"Action successful!\"\n"
            "    >>> print(result.message)");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
    
    SensoryPercept represents the complete sensory input received by an agent,
    combining all sensory modalities into a single percept object.
    
    This class integrates vision, touch, internal signals, and audio data
    into a comprehensive percept that can be processed by the agent's cognitive system.
    
    Methods:
        getVision(): Get vision data
        getTouch(): Get touch data
        getInternal(): Get internal signals
        getAudio(): Get audio data
        getAllSignals(): Get all sensory signals as a single vector
    
    Examples:
        >>> percept = pynlm.SensoryPercept()
        >>> vision = pynlm.Vision(10, 10)
        >>> percept.setVision(vision)
        >>> print(percept.getVisionWidth(), percept.getVisionHeight())
        (10, 10)
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get vision sensory input\n\n"
             "Returns:\n"
             "    Vision: Vision sensory input object\n\n"
             "Examples:\n"
             "    >>> vision = percept.getVision()")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory input\n\n"
             "Args:\n"
             "    vision (Vision): Vision sensory input to set\n\n"
             "Examples:\n"
             "    >>> percept.setVision(vision)")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get the width of vision data\n\n"
             "Returns:\n"
             "    int: Width of vision data\n\n"
             "Examples:\n"
             "    >>> width = percept.getVisionWidth()")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get the height of vision data\n\n"
             "Returns:\n"
             "    int: Height of vision data\n\n"
             "Examples:\n"
             "    >>> height = percept.getVisionHeight()")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch sensory input\n\n"
             "Returns:\n"
             "    InternalSignals: Touch sensory input object\n\n"
             "Examples:\n"
             "    >>> touch = percept.getTouch()")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory input\n\n"
             "Args:\n"
             "    touch (InternalSignals): Touch sensory input to set\n\n"
             "Examples:\n"
             "    >>> percept.setTouch(touch)")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal signals sensory input\n\n"
             "Returns:\n"
             "    InternalSignals: Internal signals sensory input object\n\n"
             "Examples:\n"
             "    >>> internal = percept.getInternal()")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals sensory input\n\n"
             "Args:\n"
             "    internal (InternalSignals): Internal signals to set\n\n"
             "Examples:\n"
             "    >>> percept.setInternal(internal)")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception sensory input\n\n"
             "Returns:\n"
             "    InternalSignals: Proprioception sensory input object\n\n"
             "Examples:\n"
             "    >>> proprioception = percept.getProprioception()")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception sensory input\n\n"
             "Args:\n"
             "    proprioception (InternalSignals): Proprioception to set\n\n"
             "Examples:\n"
             "    >>> percept.setProprioception(proprioception)")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio sensory input\n\n"
             "Returns:\n"
             "    Audio: Audio sensory input object\n\n"
             "Examples:\n"
             "    >>> audio = percept.getAudio()")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory input\n\n"
             "Args:\n"
             "    audio (Audio): Audio sensory input to set\n\n"
             "Examples:\n"
             "    >>> percept.setAudio(audio)")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a single vector\n\n"
             "Returns:\n"
             "    list: All sensory signals as a flat list of floats\n\n"
             "Examples:\n"
             "    >>> signals = percept.getAllSignals()")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get the timestamp of the sensory percept\n\n"
             "Returns:\n"
             "    float: Time when the percept was created\n\n"
             "Examples:\n"
             "    >>> time = percept.getTimestamp()")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the sensory percept\n\n"
             "Args:\n"
             "    timestamp (float): Time when the percept was created\n\n"
             "Examples:\n"
             "    >>> percept.setTimestamp(123.456)");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
    
    SimpleWorld represents a 2D simulation environment where NLM agents interact
    with the world and each other. The world supports basic physics, object
    management, and agent interactions.
    
    The world provides methods for configuration, object management, and
    simulation updates, enabling realistic agent-environment interaction.
    
    Attributes:
        width (int): World width in units
        height (int): World height in units
        maxEnergy (float): Maximum energy level for agents
        energyDecayRate (float): Energy decay rate per time step
        randomSeed (int): Random seed for reproducible simulations
    
    Examples:
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(100, 100, 20, 20)
        >>> world.setAgentStart(50, 50)
        >>> print(world.getWidth(), world.getHeight())
        (100, 100)
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world environment\n\n"
             "Args:\n"
             "    width (int): World width in units\n"
             "    height (int): World height in units\n"
             "    visionWidth (int): Width of agent's vision field\n"
             "    visionHeight (int): Height of agent's vision field\n\n"
             "Examples:\n"
             "    >>> world.configure(200, 200, 40, 40)")
        .def("reset", &SimpleWorld::reset,
             "Reset the world to initial state\n\n"
             "This removes all objects and resets agent state.\n\n"
             "Examples:\n"
             "    >>> world.reset()")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position\n\n"
             "Args:\n"
             "    x (float): X-coordinate of starting position\n"
             "    y (float): Y-coordinate of starting position\n\n"
             "Examples:\n"
             "    >>> world.setAgentStart(10, 10)")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world state\n\n"
             "Args:\n"
             "    timestep (float): Time step for update\n\n"
             "Examples:\n"
             "    >>> world.update(0.001)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world\n\n"
             "Args:\n"
             "    cmd (MotorCommand): Motor command to apply\n"
             "    currentTime (float): Current simulation time\n\n"
             "Returns:\n"
             "    ActionResult: Result of the action\n\n"
             "Examples:\n"
             "    >>> result = world.applyMotorCommand(pynlm.MotorCommand.MoveForward, 1.0)")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the current sensory percept from the world\n\n"
             "Returns:\n"
             "    SensoryPercept: Current sensory percept\n\n"
             "Examples:\n"
             "    >>> percept = world.getSensoryPercept()")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state\n\n"
             "Returns:\n"
             "    AgentBody: Current agent body state\n\n"
             "Examples:\n"
             "    >>> body = world.getAgentBody()")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world\n\n"
             "Args:\n"
             "    obj (WorldObject): Object to add\n\n"
             "Examples:\n"
             "    >>> world.addObject(resource_obj)")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove an object from the world at specified position\n\n"
             "Args:\n"
             "    x (float): X-coordinate position\n"
             "    y (float): Y-coordinate position\n\n"
             "Examples:\n"
             "    >>> world.removeObject(5.0, 5.0)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid in the world\n\n"
             "Args:\n"
             "    x (float): X-coordinate position\n"
             "    y (float): Y-coordinate position\n\n"
             "Returns:\n"
             "    bool: True if position is valid, False otherwise\n\n"
             "Examples:\n"
             "    >>> valid = world.isValidPosition(10.0, 10.0)")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get the world width\n\n"
             "Returns:\n"
             "    int: World width in units\n\n"
             "Examples:\n"
             "    >>> width = world.getWidth()")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get the world height\n\n"
             "Returns:\n"
             "    int: World height in units\n\n"
             "Examples:\n"
             "    >>> height = world.getHeight()")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get the maximum energy level\n\n"
             "Returns:\n"
             "    float: Maximum energy level\n\n"
             "Examples:\n"
             "    >>> max_energy = world.getMaxEnergy()")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum energy level\n\n"
             "Args:\n"
             "    e (float): Maximum energy level\n\n"
             "Examples:\n"
             "    >>> world.setMaxEnergy(100.0)")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get the energy decay rate\n\n"
             "Returns:\n"
             "    float: Energy decay rate per time step\n\n"
             "Examples:\n"
             "    >>> decay_rate = world.getEnergyDecayRate()")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the energy decay rate\n\n"
             "Args:\n"
             "    r (float): Energy decay rate per time step\n\n"
             "Examples:\n"
             "    >>> world.setEnergyDecayRate(0.01)")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get the current simulation time\n\n"
             "Returns:\n"
             "    float: Current simulation time\n\n"
             "Examples:\n"
             "    >>> time = world.getSimulationTime()")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random seed\n\n"
             "Args:\n"
             "    seed (int): Random seed for reproducible simulations\n\n"
             "Examples:\n"
             "    >>> world.setRandomSeed(42)")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get the random seed\n\n"
             "Returns:\n"
             "    int: Current random seed\n\n"
             "Examples:\n"
             "    >>> seed = world.getRandomSeed()");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
    
    Brain represents the central processing unit of the NLM system, implementing
    the complete neural architecture including sensory processing, motor control,
    learning, memory, and cognitive functions.
    
    The Brain class orchestrates all neural subsystems and provides the primary
    interface for neural simulation, learning, and interaction with the environment.
    
    Key capabilities:
    - Real-time neural computation with spiking neurons
    - Memory systems (working, episodic, semantic, procedural)
    - Neuromodulation (dopamine, curiosity, novelty)
    - Prediction systems
    - Cognitive mechanisms (planning, attention, concept formation)
    - Developmental systems
    - Plasticity (STDP, Hebbian, structural)
    
    Examples:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> action = brain.produceAction()
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain with configuration\n\n"
             "Args:\n"
             "    config (Config): Configuration for the brain\n\n"
             "Examples:\n"
             "    >>> config = pynlm.createDefaultConfig()\n"
             "    >>> brain = pynlm.createBrain(config)")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration\n\n"
             "This method sets up the neural architecture and initializes all\n"
             "subsystems including memory, prediction, cognition, and development.\n\n"
             "Examples:\n"
             "    >>> brain.initialize()")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step\n\n"
             "Args:\n"
             "    currentStep (int): Current simulation step\n\n"
             "Examples:\n"
             "    >>> brain.step(1000)")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp\n\n"
             "Args:\n"
             "    currentStep (int): Current simulation step\n"
             "    currentTime (float): Current simulation time\n\n"
             "Examples:\n"
             "    >>> brain.step(1000, 1.0)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain\n\n"
             "Args:\n"
             "    input (SensoryInput): Sensory input to inject\n\n"
             "Examples:\n"
             "    >>> brain.receiveSensoryInput(percept)")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron\n\n"
             "Args:\n"
             "    neuron (NeuronId): Target neuron\n"
             "    current (float): Current to inject in pA\n\n"
             "Examples:\n"
             "    >>> brain.injectCurrent(neuron_id, 50.0)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type\n\n"
             "Args:\n"
             "    type (NeuronType): Type of neurons to target\n"
             "    current (float): Current to inject in pA\n\n"
             "Examples:\n"
             "    >>> brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity\n\n"
             "Returns:\n"
             "    Action: Motor action produced by the brain\n\n"
             "Examples:\n"
             "    >>> action = brain.produceAction()")
        .def("reset", &Brain::reset,
             "Reset brain state\n\n"
             "This resets all brain state to initial values.\n\n"
             "Examples:\n"
             "    >>> brain.reset()")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file\n\n"
             "Args:\n"
             "    filepath (str): Path to save brain state\n\n"
             "Examples:\n"
             "    >>> brain.save('brain_state.nlm')")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file\n\n"
             "Args:\n"
             "    filepath (str): Path to load brain state from\n\n"
             "Examples:\n"
             "    >>> brain.load('brain_state.nlm')")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region\n\n"
             "Args:\n"
             "    name (str): Name of the region (optional)\n\n"
             "Returns:\n"
             "    RegionId: ID of the newly created region\n\n"
             "Examples:\n"
             "    >>> region_id = brain.addRegion('visual')")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID\n\n"
             "Args:\n"
             "    id (RegionId): ID of the region to retrieve\n\n"
             "Returns:\n"
             "    Region: The requested region\n\n"
             "Examples:\n"
             "    >>> region = brain.getRegion(region_id)")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions\n\n"
             "Returns:\n"
             "    int: Number of regions\n\n"
             "Examples:\n"
             "    >>> count = brain.getRegionCount()")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs\n\n"
             "Returns:\n"
             "    list: List of all region IDs\n\n"
             "Examples:\n"
             "    >>> region_ids = brain.getRegionIds()")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions\n\n"
             "Returns:\n"
             "    list: List of all regions\n\n"
             "Examples:\n"
             "    >>> regions = brain.getRegions()")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions\n\n"
             "Returns:\n"
             "    int: Total number of neurons\n\n"
             "Examples:\n"
             "    >>> count = brain.getTotalNeuronCount()")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions\n\n"
             "Returns:\n"
             "    int: Total number of synapses\n\n"
             "Examples:\n"
             "    >>> count = brain.getTotalSynapseCount()")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons\n\n"
             "Returns:\n"
             "    int: Number of active neurons\n\n"
             "Examples:\n"
             "    >>> count = brain.getActiveNeuronCount()")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons\n\n"
             "Returns:\n"
             "    int: Number of firing neurons\n\n"
             "Examples:\n"
             "    >>> count = brain.getFiringNeuronCount()")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons\n\n"
             "Returns:\n"
             "    float: Average firing rate in Hz\n\n"
             "Examples:\n"
             "    >>> rate = brain.getAverageFiringRate()")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio\n\n"
             "Returns:\n"
             "    float: Excitation/inhibition ratio\n\n"
             "Examples:\n"
             "    >>> ratio = brain.getExcitationInhibitionRatio()")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count\n\n"
             "Returns:\n"
             "    int: Total number of spikes\n\n"
             "Examples:\n"
             "    >>> count = brain.getTotalSpikeCount()")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage\n\n"
             "Returns:\n"
             "    DevelopmentalStage: Current developmental stage\n\n"
             "Examples:\n"
             "    >>> stage = brain.getDevelopmentalStage()")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage\n\n"
             "Args:\n"
             "    stage (DevelopmentalStage): New developmental stage\n\n"
             "Examples:\n"
             "    >>> brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration\n\n"
             "Returns:\n"
             "    Config: The brain's configuration\n\n"
             "Examples:\n"
             "    >>> config = brain.getConfig()")
        .def("logStatus", &Brain::logStatus,
             "Log brain status\n\n"
             "This logs the current status of the brain including statistics\n"
             "and performance metrics.\n\n"
             "Examples:\n"
             "    >>> brain.logStatus()");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
    
    AgentBrain provides a high-level interface for connecting NLM brain to the
    external world, enabling agents to perceive, act, and learn in dynamic
    environments.
    
    This interface abstracts the complexity of direct brain-world interaction,
    providing convenient methods for common agent operations.
    
    Key capabilities:
    - Processing sensory input from the environment
    - Decoding neural activity into motor commands
    - Applying reward-based neuromodulation
    - Managing development and maturation
    - Monitoring neuromodulation levels
    
    Examples:
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> percept = world.getSensoryPercept()
        >>> agent.processSensoryInput(percept)
        >>> action = agent.decodeMotorCommand()
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface\n\n"
             "Args:\n"
             "    brain (Brain): Underlying brain to interface with\n\n"
             "Examples:\n"
             "    >>> agent = pynlm.createAgentBrain(brain)")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with world\n\n"
             "Args:\n"
             "    world (SimpleWorld): World to interact with\n\n"
             "Examples:\n"
             "    >>> agent.initialize(world)")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size\n\n"
             "Returns:\n"
             "    int: Expected size of sensory input\n\n"
             "Examples:\n"
             "    >>> size = agent.getSensoryInputSize()")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size\n\n"
             "Returns:\n"
             "    int: Expected size of motor output\n\n"
             "Examples:\n"
             "    >>> size = agent.getMotorOutputSize()")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain\n\n"
             "Args:\n"
             "    percept (SensoryPercept): Sensory input to process\n\n"
             "Examples:\n"
             "    >>> agent.processSensoryInput(percept)")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command\n\n"
             "Returns:\n"
             "    MotorCommand: Decoded motor command\n\n"
             "Examples:\n"
             "    >>> command = agent.decodeMotorCommand()")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation\n\n"
             "Args:\n"
             "    reward (float): Actual reward received\n"
             "    predictedReward (float): Expected reward\n\n"
             "Examples:\n"
             "    >>> agent.applyRewardModulation(10.0, 8.0)")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system\n\n"
             "Args:\n"
             "    timestep (float): Time step for development update\n\n"
             "Examples:\n"
             "    >>> agent.updateDevelopment(0.001)")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage\n\n"
             "Returns:\n"
             "    DevelopmentalStage: Current developmental stage\n\n"
             "Examples:\n"
             "    >>> stage = agent.getDevelopmentalStage()")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level\n\n"
             "Returns:\n"
             "    float: Current neuromodulation level\n\n"
             "Examples:\n"
             "    >>> level = agent.getNeuromodulationLevel()")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level\n\n"
             "Returns:\n"
             "    float: Current curiosity level\n\n"
             "Examples:\n"
             "    >>> level = agent.getCuriosityLevel()")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level\n\n"
             "Returns:\n"
             "    float: Current novelty level\n\n"
             "Examples:\n"
             "    >>> level = agent.getNoveltyLevel()")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error\n\n"
             "Returns:\n"
             "    float: Current prediction error\n\n"
             "Examples:\n"
             "    >>> error = agent.getPredictionError()")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode\n\n"
             "This resets the agent's state for a new episode.\n\n"
             "Examples:\n"
             "    >>> agent.reset()")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain\n\n"
             "Returns:\n"
             "    Brain: The underlying brain object\n\n"
             "Examples:\n"
             "    >>> brain = agent.getBrain()")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation\n\n"
             "Args:\n"
             "    enable (bool): Whether to enable reward modulation\n\n"
             "Examples:\n"
             "    >>> agent.enableRewardModulation(True)")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity\n\n"
             "Args:\n"
             "    enable (bool): Whether to enable structural plasticity\n\n"
             "Examples:\n"
             "    >>> agent.enableStructuralPlasticity(True)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development\n\n"
             "Args:\n"
             "    enable (bool): Whether to enable development\n\n"
             "Examples:\n"
             "    >>> agent.enableDevelopment(True)")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity\n\n"
             "Args:\n"
             "    enable (bool): Whether to enable curiosity\n\n"
             "Examples:\n"
             "    >>> agent.enableCuriosity(True)")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled\n\n"
             "Returns:\n"
             "    bool: True if reward modulation is enabled\n\n"
             "Examples:\n"
             "    >>> enabled = agent.isRewardModulationEnabled()")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled\n\n"
             "Returns:\n"
             "    bool: True if structural plasticity is enabled\n\n"
             "Examples:\n"
             "    >>> enabled = agent.isStructuralPlasticityEnabled()")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled\n\n"
             "Returns:\n"
             "    bool: True if development is enabled\n\n"
             "Examples:\n"
             "    >>> enabled = agent.isDevelopmentEnabled()")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled\n\n"
             "Returns:\n"
             "    bool: True if curiosity is enabled\n\n"
             "Examples:\n"
             "    >>> enabled = agent.isCuriosityEnabled()");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration\n\n"
         "Returns:\n"
         "    Config: A new default configuration object\n\n"
         "Examples:\n"
         "    >>> config = pynlm.createDefaultConfig()")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration\n\n"
         "Args:\n"
         "    config (Config): Configuration for the brain\n\n"
         "Returns:\n"
         "    Brain: A new brain object\n\n"
         "Examples:\n"
         "    >>> config = pynlm.createDefaultConfig()\n"
         "    >>> brain = pynlm.createBrain(config)")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world\n\n"
         "Returns:\n"
         "    SimpleWorld: A new world object\n\n"
         "Examples:\n"
         "    >>> world = pynlm.createSimpleWorld()")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface\n\n"
         "Args:\n"
         "    brain (Brain): Brain to interface with\n\n"
         "Returns:\n"
         "    AgentBrain: A new agent brain interface\n\n"
         "Examples:\n"
         "    >>> agent = pynlm.createAgentBrain(brain)")

    // Constants and type aliases
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
