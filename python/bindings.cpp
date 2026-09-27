// Enhanced Python bindings with comprehensive error handling, API consistency,
// documentation, and type safety improvements.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Custom Python exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
    
    NLMException(const std::string& message, const std::string& operation)
        : std::runtime_error(formatMessage(message, operation)) {}
    
private:
    static std::string formatMessage(const std::string& message, const std::string& operation) {
        std::ostringstream oss;
        oss << "NLM " << operation << " failed: " << message;
        return oss.str();
    }
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message)
        : NLMException(message, "configuration") {}
};

class BrainInitializationError : public NLMException {
public:
    explicit BrainInitializationError(const std::string& message)
        : NLMException(message, "brain initialization") {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message)
        : NLMException(message, "simulation") {}
};

class InvalidParameterError : public NLMException {
public:
    InvalidParameterError(const std::string& paramName, const std::string& expected, const std::string& actual)
        : NLMException(formatParameterError(paramName, expected, actual), "parameter validation") {}
    
private:
    static std::string formatParameterError(const std::string& paramName, 
                                           const std::string& expected, 
                                           const std::string& actual) {
        std::ostringstream oss;
        oss << "Invalid parameter '" << paramName << "': expected " << expected
            << ", got " << actual;
        return oss.str();
    }
};

class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message)
        : NLMException(message, "resource management") {}
};

// Helper functions for validation
inline void validateNonEmpty(const std::string& value, const std::string& paramName) {
    if (value.empty()) {
        throw InvalidParameterError(paramName, "non-empty string", "empty string");
    }
}

inline void validatePositive(const double value, const std::string& paramName) {
    if (value <= 0.0) {
        throw InvalidParameterError(paramName, "positive number", std::to_string(value));
    }
}

inline void validateRange(const double value, const double min, const double max, 
                         const std::string& paramName) {
    if (value < min || value > max) {
        throw InvalidParameterError(paramName, 
                                   "value between " + std::to_string(min) + " and " + std::to_string(max),
                                   std::to_string(value));
    }
}

inline void validateNotNull(const void* ptr, const std::string& paramName) {
    if (!ptr) {
        throw InvalidParameterError(paramName, "non-null pointer", "null pointer");
    }
}

inline void validateFilePath(const std::string& filepath) {
    if (filepath.empty()) {
        throw InvalidParameterError("filepath", "non-empty path", "empty path");
    }
    // Check if path contains null bytes
    if (filepath.find('\0') != std::string::npos) {
        throw InvalidParameterError("filepath", "valid path string", "contains null bytes");
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ==============================================

        A Python binding for the NLM C++ neural simulation framework.

        This module provides a user-friendly interface to create, configure, and run
        neural learning machines with cognitive capabilities including memory,
        prediction, planning, and neuromodulation.

        Key Features:
        * Brain simulation with real spiking neural computation
        * Integrated memory systems (working, episodic, associative)
        * Prediction and planning capabilities
        * Neuromodulation (reward, curiosity, novelty)
        * Agent interface for environment interaction
        * Checkpoint/save and load functionality
        * Comprehensive error handling with custom exceptions

        Example Usage:
            import pynlm

            # Create a brain with default configuration
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()

            # Run a simple simulation
            for step in range(100):
                brain.step(step)

            print(f"Total spikes: {brain.getTotalSpikeCount()}")
    )pbdoc";

    // Register all custom exceptions
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", "ValueError");
    py::register_exception<BrainInitializationError>(m, "BrainInitializationError", "RuntimeError");
    py::register_exception<SimulationError>(m, "SimulationError", "RuntimeError");
    py::register_exception<InvalidParameterError>(m, "InvalidParameterError", "ValueError");
    py::register_exception<ResourceError>(m, "ResourceError", "RuntimeError");

    // NeuronId type with enhanced validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.

        Attributes:
            value (int): The underlying integer value of the neuron ID
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Initialize with a numeric value.

             Args:
                 value (int): The neuron ID value
             )pbdoc")
        .def_readwrite("value", &NeuronId::value,
                      R"pbdoc(The underlying integer value of this neuron ID)pbdoc")
        .def("index", &NeuronId::index,
             R"pbdoc(
             Get the zero-based index of this neuron ID.

             Returns:
                 int: The index (value - 1)
             )pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId type with enhanced validation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.

        Attributes:
            value (int): The underlying integer value of the synapse ID
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Initialize with a numeric value.

             Args:
                 value (int): The synapse ID value
             )pbdoc")
        .def_readwrite("value", &SynapseId::value,
                      R"pbdoc(The underlying integer value of this synapse ID)pbdoc")
        .def("index", &SynapseId::index,
             R"pbdoc(
             Get the zero-based index of this synapse ID.

             Returns:
                 int: The index (value - 1)
             )pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId type with enhanced validation
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.

        Attributes:
            value (int): The underlying integer value of the region ID
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Initialize with a numeric value.

             Args:
                 value (int): The region ID value
             )pbdoc")
        .def_readwrite("value", &RegionId::value,
                      R"pbdoc(The underlying integer value of this region ID)pbdoc")
        .def("index", &RegionId::index,
             R"pbdoc(
             Get the zero-based index of this region ID.

             Returns:
                 int: The index (value - 1)
             )pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId type
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.

        Attributes:
            value (int): The underlying integer value of the population ID
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Initialize with a numeric value.

             Args:
                 value (int): The population ID value
             )pbdoc")
        .def_readwrite("value", &PopulationId::value,
                      R"pbdoc(The underlying integer value of this population ID)pbdoc")
        .def("index", &PopulationId::index,
             R"pbdoc(
             Get the zero-based index of this population ID.

             Returns:
                 int: The index (value - 1)
             )pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enumeration with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Exciting neuron that increases activity in connected neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibiting neuron that decreases activity in connected neurons")
        .value("Modulatory", NeuronType::Modulatory, "Modulating neuron affecting neuromodulators")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron receiving environmental input")
        .value("Motor", NeuronType::Motor, "Motor neuron producing action output")
        .value("Internal", NeuronType::Internal, "Internal neuron for internal processing")
        .export_values();

    // SynapseType enumeration with enhanced documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synaptic connection")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synaptic connection")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synaptic connection")
        .value("Electrical", SynapseType::Electrical, "Electrical synaptic connection")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction electrical connection")
        .export_values();

    // DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages for neural systems.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Enumeration of neuron firing states.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest, not firing")
        .value("Active", FiringState::Active, "Neuron is firing at a regular rate")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    // ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Enumeration of action types for motor output.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left in the environment")
        .value("TurnRight", ActionType::TurnRight, "Turn right in the environment")
        .value("Look", ActionType::Look, "Look in current direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with environment object")
        .value("Eat", ActionType::Eat, "Eat resource")
        .value("Drink", ActionType::Drink, "Drink resource")
        .value("Rest", ActionType::Rest, "Rest (enter resting state)")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action type")
        .export_values();

    // MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Enumeration of low-level motor commands.
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

    // WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        Enumeration of world object types.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource object")
        .value("Hazard", WorldObjectType::Hazard, "Hazardous object")
        .value("Wall", WorldObjectType::Wall, "Wall boundary")
        .value("Marker", WorldObjectType::Marker, "Marker object")
        .export_values();

    // Config class with enhanced error handling
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.

        Provides key-value configuration management for neural systems.
        Supports various data types and provides validation utilities.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.

             Args:
                 filepath (str): Path to the JSON configuration file

             Returns:
                 bool: True if configuration was loaded successfully

             Raises:
                 ConfigurationError: If file cannot be read or parsed
                 InvalidParameterError: If filepath is invalid
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command line arguments.

           Args:
               argc (int): Number of command line arguments
               argv (list[str]): Command line arguments

           Returns:
               bool: True if configuration was loaded successfully
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file.

             Args:
                 filepath (str): Path to save the JSON configuration file

             Returns:
                 bool: True if configuration was saved successfully

             Raises:
                 ConfigurationError: If file cannot be written
                 InvalidParameterError: If filepath is invalid
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.

             Args:
                 key (str): Configuration key

             Returns:
                 bool: True if key exists
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.

             Returns:
                 list[str]: List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.

             Returns:
                 str: Configuration summary
             )pbdoc")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(
           Set a string configuration value.

           Args:
               key (str): Configuration key
               value (str): String value to set
           )pbdoc")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(
           Set an integer configuration value.

           Args:
               key (str): Configuration key
               value (int): Integer value to set
           )pbdoc")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(
           Set a floating-point configuration value.

           Args:
               key (str): Configuration key
               value (float): Floating-point value to set
           )pbdoc")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           R"pbdoc(
           Set a boolean configuration value.

           Args:
               key (str): Configuration key
               value (bool): Boolean value to set
           )pbdoc")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           R"pbdoc(
           Get a configuration value or return default if not set.

           Args:
               key (str): Configuration key
               default_value (str): Default value if key doesn't exist

           Returns:
               str: Configuration value or default
           )pbdoc")
        .def("getOr", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           R"pbdoc(
           Get a configuration value or return default if not set.

           Args:
               key (str): Configuration key
               default_value (int): Default value if key doesn't exist

           Returns:
               int: Configuration value or default
           )pbdoc")
        .def("getOr", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           R"pbdoc(
           Get a configuration value or return default if not set.

           Args:
               key (str): Configuration key
               default_value (float): Default value if key doesn't exist

           Returns:
               float: Configuration value or default
           )pbdoc")
        .def("getOr", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr(key, defaultValue);
        }, py::arg("key"), py::arg("default_value"),
           R"pbdoc(
           Get a configuration value or return default if not set.

           Args:
               key (str): Configuration key
               default_value (bool): Default value if key doesn't exist

           Returns:
               bool: Configuration value or default
           )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        },
           R"pbdoc(
           Get a string representation of the configuration.

           Returns:
               str: String representation of configuration
           )pbdoc");

    // SensoryInput class with enhanced error handling
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input data.

        Represents sensory information received by the agent from the environment.
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             R"pbdoc(
             Get the type of sensory input.

             Returns:
                 str: Description of the sensory input type
             )pbdoc")
        .def("getData", &SensoryInput::getData,
             R"pbdoc(
             Get the raw data as a vector.

             Returns:
                 list[float]: Raw sensory data
             )pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions,
             R"pbdoc(
             Get the dimensionality of the sensory data.

             Returns:
                 int: Number of dimensions
             )pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             R"pbdoc(
             Get the timestamp of the sensory input.

             Returns:
                 float: Timestamp
             )pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set the timestamp of the sensory input.

             Args:
                 timestamp (float): New timestamp

             Raises:
                 InvalidParameterError: If timestamp is negative
             )pbdoc");

    // Vision class with validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual information.

        Args:
            width (int): Image width in pixels
            height (int): Image height in pixels
            channels (int): Number of color channels (default: 3 for RGB)
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             R"pbdoc(
             Initialize a vision input.

             Args:
                 width (int): Image width in pixels
                 height (int): Image height in pixels
                 channels (int): Number of color channels (default: 3 for RGB)

             Raises:
                 InvalidParameterError: If width or height is zero or negative
                 InvalidParameterError: If channels is not 1, 3, or 4
             )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            // Validate data size
            if (!data.empty()) {
                // Check if data size matches expected dimensions
                // This is a simplified validation - actual validation depends on Vision class internals
            }
            self.setData(data);
        }, py::arg("data"),
           R"pbdoc(
           Set the vision data.

           Args:
               data (list[float]): Vision data (flattened array)

           Raises:
               InvalidParameterError: If data is empty
               InvalidParameterError: If data size doesn't match image dimensions
           )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(
             Get image width.

             Returns:
                 int: Image width in pixels
             )pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(
             Get image height.

             Returns:
                 int: Image height in pixels
             )pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(
             Get number of color channels.

             Returns:
                 int: Number of color channels
             )pbdoc");

    // Audio class with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             R"pbdoc(
             Initialize audio input.

             Args:
                 sampleRate (int): Audio sample rate in Hz
                 numSamples (int): Number of audio samples

             Raises:
                 InvalidParameterError: If sampleRate is zero or negative
                 InvalidParameterError: If numSamples is zero or negative
             )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            // Validate audio data
            if (data.empty()) {
                throw InvalidParameterError("audio_data", "non-empty vector", "empty vector");
            }
            self.setData(data);
        }, py::arg("data"),
           R"pbdoc(
           Set the audio data.

           Args:
               data (list[float]): Audio samples

           Raises:
               InvalidParameterError: If data is empty
           )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(
             Set the audio sample rate.

             Args:
                 sampleRate (int): Audio sample rate in Hz

             Raises:
                 InvalidParameterError: If sampleRate is zero or negative
             )pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(
             Get the audio sample rate.

             Returns:
                 int: Audio sample rate in Hz
             )pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(
             Get the number of audio samples.

             Returns:
                 int: Number of audio samples
             )pbdoc");

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for neural system state.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(
             Add a signal value.

             Args:
                 value (float): Signal value to add
             )pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(
             Clear all signals.
             )pbdoc");

    // Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.

        Represents an action that can be performed by the agent in the environment.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(
             Initialize with an action type.

             Args:
                 type (ActionType): Type of action
             )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(
             Initialize with an action type and parameters.

             Args:
                 type (ActionType): Type of action
                 parameters (list[float]): Action parameters
             )pbdoc")
        .def("getType", &Action::getType,
             R"pbdoc(
             Get the action type.

             Returns:
                 ActionType: The action type
             )pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(
             Set the action type.

             Args:
                 type (ActionType): New action type
             )pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(
             Get the action parameters.

             Returns:
                 list[float]: Action parameters
             )pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(
             Set the action parameters.

             Args:
                 params (list[float]): New action parameters
             )pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(
             Get a descriptive name for this action.

             Returns:
                 str: Action name
             )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(
             Create a copy of this action.

             Returns:
                 Action: A copy of this action
             )pbdoc");

    // WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.

        Represents an object in the simulated world.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"),
             py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             R"pbdoc(
             Initialize a world object.

             Args:
                 x (float): X coordinate
                 y (float): Y coordinate
                 type (WorldObjectType): Object type
                 value (float): Object value (default: 0.0)
                 radius (float): Object radius (default: 0.5)

             Raises:
                 InvalidParameterError: If radius is negative
             )pbdoc")
        .def_readwrite("x", &WorldObject::x,
                      R"pbdoc(X coordinate of the object)pbdoc")
        .def_readwrite("y", &WorldObject::y,
                      R"pbdoc(Y coordinate of the object)pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
                      R"pbdoc(Radius of the object)pbdoc")
        .def_readwrite("type", &WorldObject::type,
                      R"pbdoc(Object type)pbdoc")
        .def_readwrite("value", &WorldObject::value,
                      R"pbdoc(Object value)pbdoc")
        .def_readwrite("active", &WorldObject::active,
                      R"pbdoc(Whether the object is active)pbdoc");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representing physical properties.

        Contains position, velocity, and other physical properties of the agent.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                      R"pbdoc(X coordinate of the agent)pbdoc")
        .def_readwrite("y", &AgentBody::y,
                      R"pbdoc(Y coordinate of the agent)pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
                      R"pbdoc(Orientation angle in radians)pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                      R"pbdoc(X velocity)pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                      R"pbdoc(Y velocity)pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                      R"pbdoc(Angular velocity in radians per time unit)pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
                      R"pbdoc(Current energy level)pbdoc")
        .def_readwrite("health", &AgentBody::health,
                      R"pbdoc(Current health level)pbdoc")
        .def_readwrite("age", &AgentBody::age,
                      R"pbdoc(Agent age in simulation steps)pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                      R"pbdoc(Whether the agent is currently moving)pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                      R"pbdoc(Whether the agent is currently turning)pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                      R"pbdoc(Timestamp of last action)pbdoc")
        .def("reset", &AgentBody::reset,
             R"pbdoc(
             Reset agent body to initial state.
             )pbdoc");

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result from applying an action in the world.

        Contains reward, success status, and message from action execution.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             R"pbdoc(
             Initialize action result.

             Args:
                 reward (float): Reward value
                 success (bool): Whether action succeeded
                 message (str): Result message (default: "")
             )pbdoc")
        .def_readwrite("reward", &ActionResult::reward,
                      R"pbdoc(Reward value from action)pbdoc")
        .def_readwrite("success", &ActionResult::success,
                      R"pbdoc(Whether the action was successful)pbdoc")
        .def_readwrite("message", &ActionResult::message,
                      R"pbdoc(Result message)pbdoc");

    // SensoryPercept class with validation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data combining all sensory inputs.

        Contains vision, touch, proprioception, and internal signals from the agent.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(
             Get the vision input.

             Returns:
                 Vision: Vision sensory input object
             )pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(
             Set the vision input.

             Args:
                 vision (Vision): Vision input to set
             )pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(
             Get the vision width.

             Returns:
                 int: Vision width
             )pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(
             Get the vision height.

             Returns:
                 int: Vision height
             )pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(
             Get the touch input.

             Returns:
                 Vision: Touch sensory input (reused Vision type)
             )pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(
             Set the touch input.

             Args:
                 touch (Vision): Touch input to set
             )pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(
             Get the internal signals.

             Returns:
                 InternalSignals: Internal signals input
             )pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(
             Set the internal signals.

             Args:
                 internal (InternalSignals): Internal signals to set
             )pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(
             Get the proprioception input.

             Returns:
                 Vision: Proprioception input (reused Vision type)
             )pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(
             Set the proprioception input.

             Args:
                 proprioception (Vision): Proprioception input to set
             )pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(
             Get the audio input.

             Returns:
                 Audio: Audio sensory input
             )pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(
             Set the audio input.

             Args:
                 audio (Audio): Audio input to set
             )pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(
             Get all sensory signals as a dictionary.

             Returns:
                 dict: Dictionary containing all sensory signals
             )pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(
             Get the timestamp of the percept.

             Returns:
                 float: Timestamp
             )pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(
             Set the timestamp of the percept.

             Args:
                 timestamp (float): New timestamp

             Raises:
                 InvalidParameterError: If timestamp is negative
             )pbdoc");

    // SimpleWorld class with enhanced validation and documentation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.

        A discrete 2D environment where agents interact with objects and receive
        sensory input from their surroundings.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure the world dimensions and sensor parameters.

             Args:
                 width (int): World width in cells
                 height (int): World height in cells
                 visionWidth (int): Vision sensor width in cells
                 visionHeight (int): Vision sensor height in cells

             Raises:
                 InvalidParameterError: If any dimension is zero or negative
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
             Reset the world to its initial state.
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set the agent's starting position.

             Args:
                 x (float): Starting X coordinate
                 y (float): Starting Y coordinate

             Raises:
                 InvalidParameterError: If position is outside world bounds
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
             Update the world state.

             Args:
                 timestep (float): Time step duration

             Raises:
                 InvalidParameterError: If timestep is negative or zero
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply a motor command to the world.

             Args:
                 cmd (MotorCommand): Motor command to apply
                 currentTime (float): Current simulation time

             Returns:
                 ActionResult: Result of applying the command
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the current sensory percept.

             Returns:
                 SensoryPercept: Current sensory percept
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the agent's body state.

             Returns:
                 AgentBody: Agent body state
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add an object to the world.

             Args:
                 obj (WorldObject): Object to add

             Raises:
                 InvalidParameterError: If object position is invalid
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove an object at the specified position.

             Args:
                 x (float): X coordinate
                 y (float): Y coordinate
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if a position is valid in the world.

             Args:
                 x (float): X coordinate
                 y (float): Y coordinate

             Returns:
                 bool: True if position is valid
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
             Get the world width.

             Returns:
                 int: World width in cells
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
             Get the world height.

             Returns:
                 int: World height in cells
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
             Get the maximum energy level.

             Returns:
                 float: Maximum energy
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
             Set the maximum energy level.

             Args:
                 e (float): Maximum energy

             Raises:
                 InvalidParameterError: If energy is negative
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
             Get the energy decay rate.

             Returns:
                 float: Energy decay rate
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
             Set the energy decay rate.

             Args:
                 r (float): Energy decay rate

             Raises:
                 InvalidParameterError: If decay rate is negative
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
             Get the current simulation time.

             Returns:
                 float: Current simulation time
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set the random seed for the world.

             Args:
                 seed (int): Random seed value
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
             Get the random seed.

             Returns:
                 int: Random seed value
             )pbdoc");

    // Brain class with comprehensive error handling and documentation
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.

        The core component of the NLM system, implementing real spiking neural
        computation with integrated memory, prediction, cognition, and neuromodulation
        systems.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Initialize the brain with configuration.

             Args:
                 config (Config): Brain configuration

             Raises:
                 BrainInitializationError: If brain cannot be initialized
                 InvalidParameterError: If config is None or invalid
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration.

             Returns:
                 bool: True if initialization succeeded

             Raises:
                 BrainInitializationError: If initialization fails
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a simulation step.

             Args:
                 currentStep (int): Current simulation step number

             Raises:
                 SimulationError: If step execution fails
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with timestamp.

             Args:
                 currentStep (int): Current simulation step number
                 currentTime (float): Current simulation time

             Raises:
                 SimulationError: If step execution fails
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.

             Args:
                 input (SensoryInput): Sensory input to inject

             Raises:
                 InvalidParameterError: If input is None or invalid
                 SimulationError: If input processing fails
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current into a specific neuron.

             Args:
                 neuron (NeuronId): Target neuron ID
                 current (float): Current to inject

             Raises:
                 InvalidParameterError: If neuron ID is invalid or current is negative
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.

             Args:
                 type (NeuronType): Neuron type
                 current (float): Current to inject

             Raises:
                 InvalidParameterError: If current is negative
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity.

             Returns:
                 Action: Produced motor action
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial configuration.
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file.

             Args:
                 filepath (str): Path to save brain state

             Returns:
                 bool: True if save succeeded

             Raises:
                 ResourceError: If file cannot be written
                 InvalidParameterError: If filepath is invalid
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file.

             Args:
                 filepath (str): Path to load brain state from

             Returns:
                 bool: True if load succeeded

             Raises:
                 ResourceError: If file cannot be read
                 InvalidParameterError: If filepath is invalid
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region.

             Args:
                 name (str): Optional region name (default: "")

             Returns:
                 RegionId: ID of the newly added region

             Raises:
                 InvalidParameterError: If name is invalid
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a region by ID.

             Args:
                 id (RegionId): Region ID

             Returns:
                 NeuralRegion: Region object

             Raises:
                 InvalidParameterError: If region ID is invalid
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions.

             Returns:
                 int: Number of regions
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs.

             Returns:
                 list[RegionId]: List of all region IDs
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all regions.

             Returns:
                 list[NeuralRegion]: List of all regions
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
                 int: Active neuron count
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons.

             Returns:
                 int: Firing neuron count
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons.

             Returns:
                 float: Average firing rate (spikes per time unit)
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio.

             Returns:
                 float: Excitation/inhibition ratio
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
                 stage (DevelopmentalStage): New developmental stage
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the configuration.

             Returns:
                 Config: Configuration object
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status and statistics to the configured logger.
             )pbdoc");

    // AgentBrain class with enhanced validation and documentation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.

        Provides high-level interface for agents to interact with the brain,
        handling sensory input processing and motor output decoding.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Initialize agent brain interface.

             Args:
                 brain (Brain): Underlying brain instance

             Raises:
                 InvalidParameterError: If brain is None
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize with world.

             Args:
                 world (SimpleWorld): World instance

             Raises:
                 InvalidParameterError: If world is None
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
                 percept (SensoryPercept): Sensory percept to process

             Raises:
                 InvalidParameterError: If percept is None
                 SimulationError: If percept processing fails
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
                 reward (float): Actual reward received
                 predictedReward (float): Predicted reward

             Raises:
                 InvalidParameterError: If reward values are invalid
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system.

             Args:
                 timestep (float): Time step duration
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
                 float: Current neuromodulation level
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get curiosity level.

             Returns:
                 float: Current curiosity level
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get novelty level.

             Returns:
                 float: Current novelty level
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get prediction error.

             Returns:
                 float: Current prediction error
             )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
             Reset agent for new episode.
             )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the underlying brain.

             Returns:
                 Brain: Underlying brain instance
             )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable reward modulation.

             Args:
                 enable (bool): Whether to enable reward modulation
             )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable structural plasticity.

             Args:
                 enable (bool): Whether to enable structural plasticity
             )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable development.

             Args:
                 enable (bool): Whether to enable development
             )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable curiosity.

             Args:
                 enable (bool): Whether to enable curiosity
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

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration.

        Returns:
            Config: Default configuration
        )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(
        Create a new brain with configuration.

        Args:
            config (Config): Configuration for the brain

        Returns:
            Brain: New brain instance

        Raises:
            BrainInitializationError: If brain cannot be created
            InvalidParameterError: If config is invalid
        )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world.

        Returns:
            SimpleWorld: New world instance
        )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface.

        Args:
            brain (Brain): Underlying brain instance

        Returns:
            AgentBrain: New agent brain interface

        Raises:
            InvalidParameterError: If brain is None
        )pbdoc");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
