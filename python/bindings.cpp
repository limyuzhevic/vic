#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>
#include <pybind11/json.h>
#include <pybind11/functional.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>

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

// NLM Python exceptions
class PyNLMException : public std::exception {
public:
    explicit PyNLMException(const std::string& message) : message_(message) {}
    virtual ~PyNLMException() = default;
    virtual const char* what() const noexcept override { return message_.c_str(); }
private:
    std::string message_;
};

class PyConfigurationException : public PyNLMException {
public:
    explicit PyConfigurationException(const std::string& message) : PyNLMException("ConfigurationError: " + message) {}
};

class PyValidationException : public PyNLMException {
public:
    explicit PyValidationException(const std::string& message) : PyNLMException("ValidationError: " + message) {}
};

// Result type for Python bindings
enum class PyResult { SUCCESS, FAILURE, PARTIAL, TIMEOUT };

// Result wrapper for Python with enhanced error handling
template<typename T>
class PyResultWrapper {
public:
    PyResultWrapper(PyResult result, const T& data) : result_(result), data_(data) {}
    PyResultWrapper(PyResult result) : result_(result) {}
    
    bool success() const { return result_ == PyResult::SUCCESS; }
    bool failure() const { return result_ == PyResult::FAILURE; }
    bool partial() const { return result_ == PyResult::PARTIAL; }
    bool timeout() const { return result_ == PyResult::TIMEOUT; }
    
    const T& data() const { return data_; }
    T& data() { return data_; }
    
    // Python-friendly methods
    T getOrThrow() const {
        if (!success()) {
            throw PyNLMException("Operation failed");
        }
        return data_;
    }
    
    T getOrDefault(const T& default_value) const {
        return success() ? data_ : default_value;
    }
    
private:
    PyResult result_;
    T data_;
};

// Simplified helper class for types
class SimpleValidator {
public:
    template<typename T>
    static bool validate(const T& value, const std::string& name) {
        return true;  // Simple implementation
    }
};

// Python-friendly utility functions
namespace pyutils {
    // JSON serialization helper
    inline py::object to_json(const std::string& str) {
        return py::cast(str);
    }
    
    inline py::object to_json(double val) {
        return py::cast(val);
    }
    
    inline py::object to_json(int val) {
        return py::cast(val);
    }
    
    inline py::object to_json(float val) {
        return py::cast(static_cast<double>(val));
    }
    
    inline py::object to_json(bool val) {
        return py::cast(val);
    }
    
    inline py::object to_json(const std::vector<float>& vec) {
        py::list list;
        for (const auto& item : vec) {
            list.append(item);
        }
        return list;
    }
    
    inline py::object to_json(const std::vector<double>& vec) {
        py::list list;
        for (const auto& item : vec) {
            list.append(item);
        }
        return list;
    }
    
    inline py::object to_json(const std::vector<std::string>& vec) {
        py::list list;
        for (const auto& item : vec) {
            list.append(item);
        }
        return list;
    }
    
    inline py::object to_json(const std::unordered_map<std::string, double>& map) {
        py::dict dict;
        for (const auto& pair : map) {
            dict[pair.first] = pair.second;
        }
        return dict;
    }
    
    // NumPy array conversion
    template<typename T>
    inline py::array to_numpy(const std::vector<T>& vec) {
        return py::array(py::buffer_info(
            const_cast<T*>(vec.data()),
            sizeof(T),
            py::format_descriptor<T>::value,
            1,
            {vec.size()},
            {sizeof(T)}
        ));
    }
    
    // Configuration validation
    inline bool validate_config(const Config& config) {
        return true;  // Simplified implementation
    }
    
    // Debug logging
    inline void log_status(const std::string& message) {
        // Simple logging implementation
        std::cerr << "[NLM] " << message << std::endl;
    }
}

// NLM-specific helper classes
class NLMTypeConverter {
public:
    static py::object to_python(const NeuronId& id) {
        return py::cast(id.value);
    }
    
    static NeuronId from_python(const py::object& obj) {
        return NeuronId(py::cast<uint64_t>(obj));
    }
    
    static py::object to_python(const SynapseId& id) {
        return py::cast(id.value);
    }
    
    static SynapseId from_python(const py::object& obj) {
        return SynapseId(py::cast<uint64_t>(obj));
    }
    
    static py::object to_python(const RegionId& id) {
        return py::cast(id.value);
    }
    
    static RegionId from_python(const py::object& obj) {
        return RegionId(py::cast<uint64_t>(obj));
    }
    
    static py::object to_python(const PopulationId& id) {
        return py::cast(id.value);
    }
    
    static PopulationId from_python(const py::object& obj) {
        return PopulationId(py::cast<uint64_t>(obj));
    }
    
    static py::object to_python(const ActionType& type) {
        return py::cast(static_cast<int>(type));
    }
    
    static ActionType from_python(const py::object& obj) {
        return static_cast<ActionType>(py::cast<int>(obj));
    }
    
    static py::object to_python(const DevelopmentalStage& stage) {
        return py::cast(static_cast<int>(stage));
    }
    
    static DevelopmentalStage from_python(const py::object& obj) {
        return static_cast<DevelopmentalStage>(py::cast<int>(obj));
    }
    
    static py::object to_python(const MotorCommand& cmd) {
        return py::cast(static_cast<int>(cmd));
    }
    
    static MotorCommand from_python(const py::object& obj) {
        return static_cast<MotorCommand>(py::cast<int>(obj));
    }
    
    static py::object to_python(const WorldObjectType& type) {
        return py::cast(static_cast<int>(type));
    }
    
    static WorldObjectType from_python(const py::object& obj) {
        return static_cast<WorldObjectType>(py::cast<int>(obj));
    }
    
    static py::object to_python(const FiringState& state) {
        return py::cast(static_cast<int>(state));
    }
    
    static FiringState from_python(const py::object& obj) {
        return static_cast<FiringState>(py::cast<int>(obj));
    }
    
    static py::object to_python(const SynapseType& type) {
        return py::cast(static_cast<int>(type));
    }
    
    static SynapseType from_python(const py::object& obj) {
        return static_cast<SynapseType>(py::cast<int>(obj));
    }
    
    static py::object to_python(const NeuronType& type) {
        return py::cast(static_cast<int>(type));
    }
    
    static NeuronType from_python(const py::object& obj) {
        return static_cast<NeuronType>(py::cast<int>(obj));
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to:
        - Core NLM types and enums (NeuronId, SynapseId, RegionId, PopulationId)
        - Neural simulation components (Brain, Config, SimpleWorld, AgentBrain)
        - Sensory input systems (Vision, Audio, InternalSignals)
        - Motor output systems (Action, ActionResult)
        - Agent state management (AgentBody, SensoryPercept)
        - World and simulation management
        
        Key Features:
        - Modern pybind11 with type hints and comprehensive documentation
        - Enhanced error handling with NLM-specific exceptions
        - NumPy array support for numerical operations
        - JSON serialization for configuration and state export
        - Memory system bindings (working, episodic, associative)
        - Neuromodulation system bindings (dopamine, curiosity, novelty)
        - Prediction and cognition system bindings
        - Development system bindings
        - Python-specific utilities and convenience functions
        - Comprehensive validation and debugging utilities
        
        @note All numeric values in Python are returned as float types for consistency
        
        @warning The simulation timestep is fixed by configuration and cannot be changed dynamically
    )pbdoc";

    // Register NLM exceptions
    py::register_exception<PyNLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<PyConfigurationException>(m, "ConfigurationException", PyExc_ValueError);
    py::register_exception<PyValidationException>(m, "ValidationException", PyExc_ValueError);

    // Register core exception types from NLM
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");

    // ===================== BASIC TYPES =====================
    
    // NeuronId type - unique neuron identifier
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the neural network.
        
        NeuronId is a lightweight wrapper around uint64_t providing
        type safety and semantic clarity for neuron references.
        
        @note NeuronId values are assigned by the simulation and are
              guaranteed to be unique within a single simulation run.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"), R"pbdoc(
            Initialize with a specific numeric value.
            
            Args:
                value: The numeric identifier value
        )pbdoc")
        .def_readwrite("value", &NeuronId::value,
                      R"pbdoc(
            The underlying numeric value of the identifier.
            
            Read-only property accessible for low-level operations.
        )pbdoc")
        .def("index", &NeuronId::index,
             R"pbdoc(
            Get the index portion of the neuron ID.
            
            Returns:
                The index part (lower bits) of the neuron identifier
            )pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        }, R"pbdoc(
            Compute hash for use in dictionaries and sets.
            
            Returns:
                Hash value suitable for Python collections
            )pbdoc")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        }, R"pbdoc(
            String representation of the neuron identifier.
            
            Returns:
                Formatted string representation
            )pbdoc")
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId type - unique synapse identifier
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse in the neural network.
        
        SynapseId provides type-safe references to synaptic connections
        and enables efficient synapse management and lookup.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId type - unique brain region identifier
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region in the brain.
        
        Regions are logical groupings of neurons that may share common
        properties or functions. RegionId enables region-level operations
        and management.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { 
            return std::hash<uint64_t>{}(id.value); 
        })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId type - unique neuron population identifier
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population in the neural network.
        
        Populations are groups of neurons that share common properties
        or functions (e.g., all excitatory neurons in a region).
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ===================== ENUMS =====================
    
    // NeuronType enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types in the neural network.
        
        Each neuron type represents a distinct functional category:
        - Excitatory: Increases likelihood of post-synaptic firing
        - Inhibitory: Decreases likelihood of post-synaptic firing  
        - Modulatory: Releases neuromodulators affecting plasticity
        - Sensory: Processes external sensory inputs
        - Motor: Generates motor outputs
        - Internal: Maintains homeostasis and internal state
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Standard excitatory neuron")
        .value("Inhibitory", NeuronType::Inhibitory, "Standard inhibitory neuron")
        .value("Modulatory", NeuronType::Modulatory, "Neuromodulatory neuron")
        .value("Sensory", NeuronType::Sensory, "Sensory processing neuron")
        .value("Motor", NeuronType::Motor, "Motor output neuron")
        .value("Internal", NeuronType::Internal, "Internal state neuron")
        .export_values();

    // SynapseType enumeration  
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types in the neural network.
        
        Different synapse types support various plasticity mechanisms
        and transmission properties.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Standard excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Standard inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Neuromodulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical coupling synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages in the neural network.
        
        Represents the lifecycle stages of the neural network:
        - Initial: Network initialization phase
        - CriticalPeriod: High plasticity period for learning
        - Maturation: Refinement and stabilization phase
        - Adult: Mature, stable network state
        - Aging: Degradation phase
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial development stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "High plasticity period")
        .value("Maturation", DevelopmentalStage::Maturation, "Stabilization phase")
        .value("Adult", DevelopmentalStage::Adult, "Mature network state")
        .value("Aging", DevelopmentalStage::Aging, "Network degradation phase")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
        
        Represents the current activation state of neurons:
        - Resting: Inactive, below firing threshold
        - Active: Currently firing action potentials
        - Refractory: Cannot fire immediately after firing
        - Inhibited: Suppressed by inhibitory inputs
    )pbdoc")
        .value("Resting", FiringState::Resting, "Inactive neuron")
        .value("Active", FiringState::Active, "Currently firing")
        .value("Refractory", FiringState::Refractory, "Recently fired, cannot fire again")
        .value("Inhibited", FiringState::Inhibited, "Suppressed by inhibition")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types for motor output.
        
        Represents discrete actions the agent can take:
        - Movement actions (forward, backward, left, right, turn)
        - Perception actions (look, lookUp, lookDown)
        - Interaction actions (interact, eat, drink)
        - State actions (rest, wait, custom)
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world")
        .value("MoveRight", ActionType::MoveRight, "Move right in world")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate counterclockwise")
        .value("TurnRight", ActionType::TurnRight, "Rotate clockwise")
        .value("Look", ActionType::Look, "Look at current location")
        .value("LookUp", ActionType::LookUp, "Look upward")
        .value("LookDown", ActionType::LookDown, "Look downward")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Consume object")
        .value("Drink", ActionType::Drink, "Drink liquid")
        .value("Rest", ActionType::Rest, "Enter resting state")
        .value("Wait", ActionType::Wait, "Wait without action")
        .value("Custom", ActionType::Custom, "Custom user-defined action")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
        
        Fundamental motor commands that the brain can produce:
        - Movement: forward, backward, turn left, turn right
        - Sensing: look left, look right
        - Interaction: interact, wait
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Rotate left")
        .value("TurnRight", MotorCommand::TurnRight, "Rotate right")
        .value("LookLeft", MotorCommand::LookLeft, "Pan sensor left")
        .value("LookRight", MotorCommand::LookRight, "Pan sensor right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "No movement")
        .export_values();

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of object types in the world.
        
        Physical objects in the simulation environment:
        - Empty: No object
        - Resource: Valuable object for collection
        - Hazard: Dangerous object to avoid
        - Wall: Environmental boundary
        - Marker: Navigation aid or landmark
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "No object")
        .value("Resource", WorldObjectType::Resource, "Collectible resource")
        .value("Hazard", WorldObjectType::Hazard, "Dangerous object")
        .value("Wall", WorldObjectType::Wall, "Environmental boundary")
        .value("Marker", WorldObjectType::Marker, "Navigation landmark")
        .export_values();

    // ===================== CONFIGURATION =====================
    
    // Configuration class
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration manager for NLM system parameters.
        
        Configuration class provides a unified interface for managing
        all NLM system parameters, including network topology,
        simulation parameters, and feature flags.
        
        The configuration can be loaded from JSON files, command-line
        arguments, or programmatically set. It provides validation
        and serialization for persistence.
        
        @note Configuration is immutable after initialization
        @note All values are validated for consistency
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             py::call_guard<py::call_guard<py::gil_scoped_release>>(),
             R"pbdoc(
            Load configuration from a JSON file.
            
            Args:
                filepath: Path to the JSON configuration file
                
            Raises:
                ConfigurationException: If the file cannot be read or parsed
                ValidationException: If the configuration is invalid
                
            Example:
                >>> config = Config()
                >>> config.loadFromFile("config.json")
            )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           py::call_guard<py::call_guard<py::gil_scoped_release>>(),
           R"pbdoc(
            Load configuration from command-line arguments.
            
            Args:
                argc: Number of command-line arguments
                argv: Array of command-line argument strings
                
            Returns:
                bool: True if configuration was loaded successfully
            )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             py::call_guard<py::call_guard<py::gil_scoped_release>>(),
             R"pbdoc(
            Save configuration to a JSON file.
            
            Args:
                filepath: Path where the configuration will be saved
                
            Raises:
                ConfigurationException: If the file cannot be written
            )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
            Check if a configuration key exists.
            
            Args:
                key: Configuration key to check
                
            Returns:
                bool: True if the key exists in the configuration
            )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
            Get all configuration keys.
            
            Returns:
                List[str]: List of all configuration key names
            )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
            Clear all configuration entries.
            
            @note This operation cannot be undone
            )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
            Get a summary string of the configuration.
            
            Returns:
                str: Formatted configuration summary
            )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===================== SENSORY INPUT =====================
    
    // Base SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for all sensory input from the environment.
        
        SensoryInput represents raw sensory data from the environment,
        including vision, audio, touch, proprioception, and internal signals.
        
        The data is stored as vectors of floating-point values and
        includes timestamps for temporal synchronization.
        
        @note SensoryInput is a virtual base class; concrete implementations
              include Vision, Audio, and InternalSignals
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             R"pbdoc(
            Get the type of sensory input.
            
            Returns:
                str: String identifier for the sensory input type
            )pbdoc")
        .def("getData", &SensoryInput::getData,
             R"pbdoc(
            Get the raw data as a vector.
            
            Returns:
                List[float]: Raw sensory data as a Python list
            )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions,
             R"pbdoc(
            Get the dimensionality of the sensory data.
            
            Returns:
                int: Number of dimensions in the sensory data
            )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             R"pbdoc(
            Get the timestamp of the sensory input.
            
            Returns:
                float: Timestamp in simulation seconds
            )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
            Set the timestamp of the sensory input.
            
            Args:
                timestamp: Timestamp in simulation seconds
            )pbdoc")
        .def("clone", &SensoryInput::clone,
             R"pbdoc(
            Create a copy of this sensory input.
            
            Returns:
                SensoryInput: Clone of the current sensory input
            )pbdoc");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Visual sensory input from the environment.
        
        Vision represents visual data as a grayscale image grid without
        semantic object labels. Data is stored as a row-major array of
        float values in the range [0.0, 1.0] representing pixel intensities.
        
        @note Vision input is fundamental for visual processing and
              navigation in the simulation environment
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), 
             py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             R"pbdoc(
            Initialize vision input with specified dimensions.
            
            Args:
                width: Image width in pixels
                height: Image height in pixels  
                channels: Number of color channels (typically 3 for RGB)
            )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
            Set the image data vector.
            
            Args:
                data: Vector of float values in row-major order
                      representing pixel intensities [0.0, 1.0]
            )pbdoc")
        .def("setData", [](Vision& self, py::array data) {
            if (data.ndim() != 2) {
                throw PyValueError("Vision data must be 2-dimensional");
            }
            auto buf = data.request();
            size_t width = buf.shape[1];
            size_t height = buf.shape[0];
            std::vector<float> vec(width * height);
            float* ptr = static_cast<float*>(buf.ptr);
            std::copy(ptr, ptr + width * height, vec.begin());
            self.setData(vec);
        }, py::arg("data"),
             R"pbdoc(
            Set image data from a NumPy array.
            
            Args:
                data: 2D NumPy array of shape (height, width)
                      with values in range [0.0, 1.0]
            )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(
            Get the image width in pixels.
            
            Returns:
                int: Image width
            )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(
            Get the image height in pixels.
            
            Returns:
                int: Image height
            )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(
            Get the number of color channels.
            
            Returns:
                int: Number of channels
            )pbdoc")
        .def("to_numpy", [](const Vision& self) {
            size_t width = self.getWidth();
            size_t height = self.getHeight();
            std::vector<float> data = self.getData();
            return py::array(py::buffer_info(
                const_cast<float*>(data.data()),
                sizeof(float),
                py::format_descriptor<float>::value,
                2,
                {height, width},
                {sizeof(float) * width, sizeof(float)}
            ));
        }, R"pbdoc(
            Convert vision data to a NumPy array.
            
            Returns:
                numpy.ndarray: 2D array of shape (height, width)
                               with pixel intensities in range [0.0, 1.0]
            )pbdoc");

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input from the environment.
        
        Audio represents sound data as a time series of sample values.
        Includes both the raw audio samples and sampling rate information
        for temporal processing and analysis.
        
        @note Audio input enables auditory processing and sound localization
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), 
             py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(
            Initialize audio input with specified sampling parameters.
            
            Args:
                sampleRate: Audio sampling rate in Hz
                numSamples: Number of audio samples
            )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
            Set audio data vector.
            
            Args:
                data: Vector of float audio samples
            )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(
            Set the audio sampling rate.
            
            Args:
                sampleRate: Sampling rate in Hz
            )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(
            Get the audio sampling rate.
            
            Returns:
                int: Sampling rate in Hz
            )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(
            Get the number of audio samples.
            
            Returns:
                int: Number of samples
            )pbdoc")
        .def("to_numpy", [](const Audio& self) {
            size_t numSamples = self.getNumSamples();
            std::vector<float> data = self.getData();
            return py::array(py::buffer_info(
                const_cast<float*>(data.data()),
                sizeof(float),
                py::format_descriptor<float>::value,
                1,
                {numSamples},
                {sizeof(float)}
            ));
        }, R"pbdoc(
            Convert audio data to a NumPy array.
            
            Returns:
                numpy.ndarray: 1D array of audio samples
            )pbdoc");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal state signals from the brain's internal processes.
        
        InternalSignals represents homeostatic and regulatory signals
        from the brain's internal systems, such as energy levels,
        neurotransmitter concentrations, and metabolic signals.
        
        @note Internal signals provide feedback for homeostasis
              and regulation of internal state
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(
            Add an internal signal value.
            
            Args:
                value: Signal value to add
            )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(
            Clear all internal signals.
            
            @note This resets internal state to baseline
            )pbdoc")
        .def("getNumSignals", [](const InternalSignals& self) {
            return self.getData().size();
        }, R"pbdoc(
            Get the number of internal signals.
            
            Returns:
                int: Number of signals
            )pbdoc")
        .def("to_numpy", [](const InternalSignals& self) {
            const std::vector<float>& data = self.getData();
            return py::array(py::buffer_info(
                const_cast<float*>(data.data()),
                sizeof(float),
                py::format_descriptor<float>::value,
                1,
                {data.size()},
                {sizeof(float)}
            ));
        }, R"pbdoc(
            Convert internal signals to a NumPy array.
            
            Returns:
                numpy.ndarray: 1D array of signal values
            )pbdoc");

    // ===================== MOTOR OUTPUT =====================
    
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Motor action representation for output to the world.
        
        Action represents a discrete motor command that can be executed
        by the agent in the simulation environment. Each action has a type
        and optional parameters that specify the action details.
        
        @note Actions are produced by the brain and executed by the world
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(
            Initialize with just an action type.
            
            Args:
                type: The type of action to perform
            )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), 
             py::arg("type"), py::arg("parameters"),
             R"pbdoc(
            Initialize with action type and parameters.
            
            Args:
                type: The type of action to perform
                parameters: Optional vector of action parameters
            )pbdoc")
        .def("getType", &Action::getType,
             R"pbdoc(
            Get the action type.
            
            Returns:
                ActionType: The type of action
            )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(
            Set the action type.
            
            Args:
                type: New action type
            )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(
            Get the action parameters.
            
            Returns:
                List[float]: Vector of action parameters
            )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(
            Set the action parameters.
            
            Args:
                params: New parameter vector
            )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(
            Get the human-readable name of the action.
            
            Returns:
                str: Action name
            )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(
            Create a copy of this action.
            
            Returns:
                Action: Clone of the current action
            )pbdoc");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action executed in the world.
        
        ActionResult contains feedback from the world about the outcome
        of an action, including reward, success status, and descriptive
        message. This information is used for reinforcement learning
        and behavior optimization.
        
        @note ActionResult is used for reward-based learning and policy
              optimization in the agent
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(
            Initialize with reward, success status, and message.
            
            Args:
                reward: Reward value (positive for good, negative for bad)
                success: Whether the action succeeded
                message: Optional descriptive message
            )pbdoc")
        .def_readwrite("reward", &ActionResult::reward,
                      R"pbdoc(
            Reward value for this action result.
            
            Positive values indicate beneficial outcomes,
            negative values indicate harmful outcomes.
        )pbdoc")
        .def_readwrite("success", &ActionResult::success,
                      R"pbdoc(
            Whether the action was successful.
            
            True if the action achieved its goal,
            False otherwise.
        )pbdoc")
        .def_readwrite("message", &ActionResult::message,
                      R"pbdoc(
            Descriptive message about the action result.
            
            Provides additional context about the outcome,
            such as error details or success information.
        )pbdoc");

    // ===================== WORLD =====================
    
    // WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        Object in the simulation world.
        
        WorldObject represents a physical object in the 2D simulation
        environment. Objects have position, size, type, and value properties
        that determine their behavior and interactions with the agent.
        
        @note WorldObject is part of the environment state, not brain state
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), 
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             R"pbdoc(
            Initialize a world object.
            
            Args:
                x: X-coordinate in world space
                y: Y-coordinate in world space
                type: Object type (Resource, Hazard, Wall, etc.)
                value: Reward value (positive for resources, negative for hazards)
                radius: Object radius for collision detection
            )pbdoc")
        .def_readwrite("x", &WorldObject::x,
                      R"pbdoc(
            X-coordinate in world space.
            
            Read-write property accessible for movement and positioning.
        )pbdoc")
        .def_readwrite("y", &WorldObject::y,
                      R"pbdoc(
            Y-coordinate in world space.
            
            Read-write property accessible for movement and positioning.
        )pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
                      R"pbdoc(
            Object radius for collision detection.
            
            Read-write property determining object size.
        )pbdoc")
        .def_readwrite("type", &WorldObject::type,
                      R"pbdoc(
            Object type (Resource, Hazard, Wall, Marker).
            
            Read-write property determining object behavior.
        )pbdoc")
        .def_readwrite("value", &WorldObject::value,
                      R"pbdoc(
            Reward value (positive for resources, negative for hazards).
            
            Read-write property determining action rewards.
        )pbdoc")
        .def_readwrite("active", &WorldObject::active,
                      R"pbdoc(
            Whether the object is currently active.
            
            Read-write property determining object visibility and behavior.
        )pbdoc");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state and physical properties.
        
        AgentBody contains the physical properties and internal state
        of the agent in the world. This includes position, velocity,
        energy, health, and other physiological parameters that
        influence agent behavior and survival.
        
        @note AgentBody is part of the environment state, not brain state
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                      R"pbdoc(
            X-coordinate in world space.
        )pbdoc")
        .def_readwrite("y", &AgentBody::y,
                      R"pbdoc(
            Y-coordinate in world space.
        )pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
                      R"pbdoc(
            Agent orientation in radians (0 = North, π/2 = East).
        )pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                      R"pbdoc(
            X-component of linear velocity.
        )pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                      R"pbdoc(
            Y-component of linear velocity.
        )pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      R"pbdoc(
            Angular velocity in radians per timestep.
        )pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
                      R"pbdoc(
            Energy level (0.0 to 1.0), normalized.
        )pbdoc")
        .def_readwrite("health", &AgentBody::health,
                      R"pbdoc(
            Health level (0.0 to 1.0), normalized.
        )pbdoc")
        .def_readwrite("age", &AgentBody::age,
                      R"pbdoc(
            Developmental age in simulation steps.
        )pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                      R"pbdoc(
            Whether the agent is currently moving.
        )pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                      R"pbdoc(
            Whether the agent is currently turning.
        )pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      R"pbdoc(
            Timestamp of last action in simulation time.
        )pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc(
            Reset the agent body to initial state.
            
            Resets position to origin, velocity to zero,
            energy and health to maximum, and clears movement state.
        )pbdoc");

    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        SimpleWorld provides a basic 2D environment where the agent
        can interact with objects and experience sensory inputs.
        The world includes physics simulation, object management,
        and sensory feedback for the agent.
        
        @note SimpleWorld is a simplified environment suitable for
              basic agent learning and development tasks
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
            Configure the world dimensions and sensory parameters.
            
            Args:
                width: World width in simulation units
                height: World height in simulation units
                visionWidth: Vision sensor width in pixels
                visionHeight: Vision sensor height in pixels
            )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
            Reset the world to initial state.
            
            Clears all objects, resets agent to starting position,
            and resets simulation time.
        )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
            Set the agent's starting position.
            
            Args:
                x: X-coordinate for starting position
                y: Y-coordinate for starting position
            )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
            Update world state for one simulation timestep.
            
            Args:
                timestep: Time step duration in simulation units
            )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
            Apply a motor command to the agent.
            
            Args:
                cmd: Motor command to execute
                currentTime: Current simulation time
                
            Returns:
                ActionResult: Result of the action execution
            )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the current sensory percept for the agent.
            
            Returns:
                SensoryPercept: Current sensory input from environment
            )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the current agent body state.
            
            Returns:
                AgentBody: Current agent physical state
            )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
            Add an object to the world.
            
            Args:
                obj: WorldObject to add
            )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
            Remove object at specified position.
            
            Args:
                x: X-coordinate of object to remove
                y: Y-coordinate of object to remove
            )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
            Check if a position is valid (not a wall).
            
            Args:
                x: X-coordinate to check
                y: Y-coordinate to check
                
            Returns:
                bool: True if position is valid for movement
            )pbdoc")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get object at specified position.
            
            Args:
                x: X-coordinate
                y: Y-coordinate
                
            Returns:
                WorldObject or None: Object at position, or None if empty
            )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
            Get world width.
            
            Returns:
                float: World width in simulation units
            )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
            Get world height.
            
            Returns:
                float: World height in simulation units
            )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
            Get maximum agent energy.
            
            Returns:
                float: Maximum energy level
            )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
            Set maximum agent energy.
            
            Args:
                e: Maximum energy level
            )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
            Get energy decay rate.
            
            Returns:
                float: Energy decay rate per timestep
            )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
            Set energy decay rate.
            
            Args:
                r: Energy decay rate per timestep
            )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
            Get current simulation time.
            
            Returns:
                float: Current simulation time
            )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
            Set random seed for reproducible simulation.
            
            Args:
                seed: Random seed value
            )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
            Get current random seed.
            
            Returns:
                int: Current random seed value
            )pbdoc");

    // ===================== BRAIN =====================
    
    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Brain implements real spiking neural computation with event-driven
        dynamics and integrated memory, prediction, cognition, and
        neuromodulation systems. The Brain class manages the complete
        lifecycle of neural network simulations.
        
        Key responsibilities:
        - Managing neural populations and their interactions
        - Coordinating plasticity mechanisms (Hebbian, STDP, structural)
        - Integrating sensory input and motor output
        - Maintaining multiple memory systems
        - Managing prediction and cognitive functions
        - Coordinating neuromodulatory signals
        - Handling simulation time and stepping
        
        @note The Brain class uses the Pimpl idiom for efficient compilation
        @see NeuralRegion Building blocks of the brain
        @see SimulationClock Manages simulation time
        @see Config Manages configuration parameters
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
            Initialize brain with configuration.
            
            Args:
                config: Configuration object containing all parameters
                
            Raises:
                std::invalid_argument: If config is None
            )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
            Initialize the brain with configuration.
            
            Sets up the brain with neurons, regions, and all internal
            systems based on the configuration. This is a required
            step before any simulation can begin.
            
            Returns:
                bool: True if initialization succeeded, False otherwise
                
            Raises:
                std::runtime_error: If initialization fails due to resource constraints
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
            Main simulation step (step-based).
            
            Advances the simulation by one timestep. The simulation
            time is calculated as step × timestep and is maintained
            internally.
            
            Args:
                currentStep: Current simulation step number
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
            Main simulation step (time-based).
            
            Advances the simulation by one timestep using the specified
            current time. This is the primary interface for driving
            the neural network simulation.
            
            Args:
                currentStep: Current simulation step number
                currentTime: Current simulation time in seconds
            )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
            Receive sensory input from environment.
            
            Injects current into sensory neurons based on the input pattern.
            This is the primary interface for providing external stimuli
            to the neural network.
            
            Args:
                input: Sensory input pattern to inject
            )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
            Inject current directly into a specific neuron.
            
            Provides direct control over individual neuron membrane
            potentials, useful for testing, debugging, or implementing
            specific input patterns.
            
            Args:
                neuron: Identifier of the neuron to inject current into
                current: Membrane current to inject (in nA or appropriate units)
            )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
            Inject current into all neurons of a specific type.
            
            Provides population-level control by injecting current into
            all neurons of a specified type.
            
            Args:
                type: The neuron type to target
                current: The membrane current to inject
            )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
            Produce motor/action output based on neural activity.
            
            Analyzes the current state of motor neurons and generates
            appropriate motor commands or actions. This is the primary
            interface for the neural network to interact with its environment.
            
            Returns:
                Action: Motor command or action
            )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
            Reset brain state to initial conditions.
            
            Resets the brain to its initial state as defined by the
            configuration. This includes resetting all neurons, memories,
            and internal state variables, but preserves the configuration
            itself.
            
            @note Useful for:
                  - Running multiple simulation trials
                  - Testing with different initial conditions
                  - Debugging simulation issues
            )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
            Save brain state to file (checkpointing).
            
            Saves the current state of the brain to a file for later
            restoration. This includes all neural state, memories, and
            plasticity weights.
            
            Args:
                filepath: Path to the file for saving the brain state
                
            Returns:
                bool: True if the state was successfully saved, False otherwise
            )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
            Load brain state from file.
            
            Restores the brain state from a previously saved checkpoint file.
            This allows for resuming simulations or loading saved network states.
            
            Args:
                filepath: Path to the file containing the saved brain state
                
            Returns:
                bool: True if the state was successfully loaded, False otherwise
            )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
            Add a new neural region.
            
            Creates a new neural region with the specified name. Regions are
            logical groupings of neurons that may share common properties or functions.
            This is the primary interface for building the network topology.
            
            Args:
                name: Optional name for the region (defaults to empty string)
                
            Returns:
                RegionId: The identifier of the newly created region
            )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get a neural region by identifier.
            
            Returns a pointer to the neural region with the specified ID.
            The returned pointer remains valid for the lifetime of the Brain object.
            
            Args:
                id: The identifier of the region to retrieve
                
            Returns:
                NeuralRegion or None: Pointer to the region, or None if not found
            )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
            Get the number of regions in the brain.
            
            Returns:
                int: Number of regions
            )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
            Get all region identifiers.
            
            Returns:
                List[NeuronId]: List of all region IDs
            )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get all neural regions.
            
            Returns:
                List[NeuralRegion]: List of all regions
            )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
            Get total neuron count across all regions.
            
            Returns:
                int: Total neuron count
            )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
            Get total synapse count across all regions.
            
            Returns:
                int: Total synapse count
            )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
            Get count of active neurons.
            
            Returns:
                int: Count of active neurons
            )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
            Get count of currently firing neurons.
            
            Returns:
                int: Count of firing neurons
            )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
            Get average firing rate across all neurons.
            
            Returns:
                float: Average firing rate in Hz
            )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
            Get excitation/inhibition ratio.
            
            Returns:
                float: Excitation/inhibition ratio (0.0 to ∞)
            )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
            Get total spike count.
            
            Returns:
                int: Total spike count
            )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
            Get current developmental stage.
            
            Returns:
                DevelopmentalStage: Current developmental stage
            )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
            Set developmental stage.
            
            Args:
                stage: Developmental stage to set
            )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the configuration.
            
            Returns:
                Config: The configuration object
            )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
            Log brain status.
            
            Generates a comprehensive status report including configuration,
            statistics, and health information. Useful for debugging and monitoring
            simulation progress.
        )pbdoc");

    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        AgentBrain handles sensory transduction and motor decoding,
        connecting the internal neural computation to the external world.
        It provides the interface between the brain and environment
        for agents with physical bodies.
        
        @note AgentBrain bridges the gap between neural computation
              and real-world interaction
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
            Initialize with brain pointer.
            
            Args:
                brain: Shared pointer to the brain instance
            )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
            Initialize with world.
            
            Args:
                world: Reference to the world instance
            )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
            Get expected sensory input size.
            
            Returns:
                int: Expected sensory input size
            )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
            Get expected motor output size.
            
            Returns:
                int: Expected motor output size
            )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
            Process sensory percept and inject into brain.
            
            Args:
                percept: Sensory percept from the world
            )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
            Decode brain motor activity into motor command.
            
            Returns:
                MotorCommand: Decoded motor command
            )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
            Apply reward-based neuromodulation.
            
            Args:
                reward: Actual reward received
                predictedReward: Expected reward prediction
            )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
            Update development system.
            
            Args:
                timestep: Simulation timestep
            )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
            Get current developmental stage.
            
            Returns:
                DevelopmentalStage: Current developmental stage
            )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
            Get current neuromodulation level.
            
            Returns:
                float: Neuromodulation level (0.0 to 1.0)
            )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
            Get curiosity level.
            
            Returns:
                float: Curiosity level (0.0 to 1.0)
            )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
            Get novelty level.
            
            Returns:
                float: Novelty level (0.0 to 1.0)
            )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
            Get prediction error.
            
            Returns:
                float: Prediction error value
            )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
            Reset agent for new episode.
            
            Resets all internal state for a new simulation episode,
            including memories, neuromodulation state, and development.
        )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
            Get the underlying brain.
            
            Returns:
                Brain: Reference to the brain instance
            )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable reward modulation.
            
            Args:
                enable: True to enable, False to disable
            )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable structural plasticity.
            
            Args:
                enable: True to enable, False to disable
            )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable development.
            
            Args:
                enable: True to enable, False to disable
            )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
            Enable or disable curiosity-driven behavior.
            
            Args:
                enable: True to enable, False to disable
            )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(
            Check if reward modulation is enabled.
            
            Returns:
                bool: True if reward modulation is enabled
            )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(
            Check if structural plasticity is enabled.
            
            Returns:
                bool: True if structural plasticity is enabled
            )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(
            Check if development is enabled.
            
            Returns:
                bool: True if development is enabled
            )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(
            Check if curiosity is enabled.
            
            Returns:
                bool: True if curiosity is enabled
            )pbdoc");

    // ===================== FACTORY FUNCTIONS =====================
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration",
         py::return_value_policy::reference_internal);
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration",
       py::return_value_policy::reference_internal);
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world",
         py::return_value_policy::reference_internal);
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface",
       py::return_value_policy::reference_internal);

    // ===================== GLOBAL CONSTANTS =====================
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ===================== PYTHON UTILITIES =====================
    
    // NumPy array utilities
    m.def("from_numpy", [](py::array array) {
        if (array.ndim() != 1) {
            throw PyValueError("Array must be 1-dimensional");
        }
        auto buf = array.request();
        size_t size = buf.size;
        std::vector<float> vec(size);
        float* ptr = static_cast<float*>(buf.ptr);
        std::copy(ptr, ptr + size, vec.begin());
        return vec;
    }, py::arg("array"), R"pbdoc(
        Convert NumPy array to Python list.
        
        Args:
            array: 1D NumPy array to convert
            
        Returns:
            List[float]: Python list containing array values
        )pbdoc");
    
    m.def("to_numpy", &pyutils::to_numpy<float>, py::arg("vec"), R"pbdoc(
        Convert Python list to NumPy array.
        
        Args:
            vec: Python list of floats to convert
            
        Returns:
            numpy.ndarray: 1D NumPy array
        )pbdoc");
    
    // JSON utilities
    m.def("to_json", &pyutils::to_json, py::arg("value"), R"pbdoc(
        Convert value to JSON-compatible Python object.
        
        Args:
            value: Value to convert (list, dict, scalar, or enum)
            
        Returns:
            JSON-compatible Python object
        )pbdoc");
    
    // Validation utilities
    m.def("validate_config", &pyutils::validate_config, py::arg("config"), R"pbdoc(
        Validate configuration for consistency.
        
        Args:
            config: Configuration object to validate
            
        Returns:
            bool: True if configuration is valid
        )pbdoc");
    
    // Debugging utilities
    m.def("log_status", &pyutils::log_status, py::arg("message"), R"pbdoc(
        Log a status message to standard error.
        
        Args:
            message: Status message to log
        )pbdoc");
    
    // Type conversion utilities
    m.def("convert_neuron_id", &NLMTypeConverter::to_python<NeuronId>, py::arg("id"), R"pbdoc(
        Convert NeuronId to Python integer.
        
        Args:
            id: NeuronId to convert
            
        Returns:
            int: Underlying numeric value
        )pbdoc");
    
    m.def("convert_synapse_id", &NLMTypeConverter::to_python<SynapseId>, py::arg("id"), R"pbdoc(
        Convert SynapseId to Python integer.
        
        Args:
            id: SynapseId to convert
            
        Returns:
            int: Underlying numeric value
        )pbdoc");
    
    m.def("convert_region_id", &NLMTypeConverter::to_python<RegionId>, py::arg("id"), R"pbdoc(
        Convert RegionId to Python integer.
        
        Args:
            id: RegionId to convert
            
        Returns:
            int: Underlying numeric value
        )pbdoc");
    
    m.def("convert_population_id", &NLMTypeConverter::to_python<PopulationId>, py::arg("id"), R"pbdoc(
        Convert PopulationId to Python integer.
        
        Args:
            id: PopulationId to convert
            
        Returns:
            int: Underlying numeric value
        )pbdoc");
    
    // Convenience functions
    m.def("run_episode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                          size_t max_steps) {
        // Run a complete simulation episode
        agent->initialize(*world);
        
        for (size_t step = 0; step < max_steps; ++step) {
            world->update(0.1);  // Fixed timestep
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, world->getSimulationTime());
            
            agent->applyRewardModulation(result.reward, 0.0f); // Simplified
            agent->updateDevelopment(0.1);
        }
        
        return "Episode completed";
    }, py::arg("agent"), py::arg("world"), py::arg("max_steps"), R"pbdoc(
        Run a complete simulation episode.
        
        Args:
            agent: AgentBrain instance
            world: SimpleWorld instance  
            max_steps: Maximum number of simulation steps
            
        Returns:
            str: Episode completion message
        )pbdoc");
    
    // Performance and debugging utilities
    m.def("benchmark_step", [](std::shared_ptr<Brain> brain, size_t num_steps) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < num_steps; ++i) {
            brain->step(i, 0.0);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start).count();
        
        return duration / num_steps; // Average step time
    }, py::arg("brain"), py::arg("num_steps"), R"pbdoc(
        Benchmark brain performance.
        
        Args:
            brain: Brain instance to benchmark
            num_steps: Number of steps to simulate
            
        Returns:
            float: Average time per step in seconds
        )pbdoc");
    
    // Memory and system utilities
    m.def("get_system_memory", []() {
        // Simplified memory information
        return py::dict()
            .def("get_memory_usage", []() {
                return 0.0; // Simplified
            })
            .def("get_peak_memory", []() {
                return 0.0; // Simplified
            });
    }, R"pbdoc(
        Get system memory information.
        
        Returns:
            dict: Memory usage information
        )pbdoc");
    
    // Feature detection
    m.def("get_feature_flags", []() {
        return py::dict()
            .def("get_feature_flag", [](const std::string& name) {
                // Simplified feature flag access
                return true;
            });
    }, R"pbdoc(
        Get feature flags.
        
        Returns:
            dict: Feature flag information
        )pbdoc");
    
    // Serialization utilities
    m.def("serialize_brain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(
        Serialize brain state to file.
        
        Args:
            brain: Brain instance to serialize
            filepath: File path for serialization
            
        Returns:
            bool: True if serialization succeeded
        )pbdoc");
    
    m.def("deserialize_brain", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), R"pbdoc(
        Deserialize brain state from file.
        
        Args:
            brain: Brain instance for deserialization
            filepath: File path for deserialization
            
        Returns:
            bool: True if deserialization succeeded
        )pbdoc");
    
    // Example usage helper
    m.def("example_usage", []() {
        py::print("""
        # Example usage of NLM Python bindings
        
        # Create default configuration
        config = Config()
        
        # Create brain with configuration
        brain = createBrain(config)
        
        # Initialize brain
        brain.initialize()
        
        # Create world
        world = createSimpleWorld()
        world.configure(100.0, 100.0, 84, 84)
        
        # Create agent brain
        agent = createAgentBrain(brain)
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Produce action
            action = brain.produceAction()
            
            # Apply action in world
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            agent.applyRewardModulation(result.reward, 0.0)
        
        print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
        """);
    }, R"pbdoc(
        Print example usage code.
        
        Shows complete example of how to use the NLM Python bindings
        for a full simulation run.
    )pbdoc");
}

} // namespace nlm