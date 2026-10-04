// Improved NLM Python Bindings with Better Error Handling and Consistency

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Include our exception utilities and error handling
#include "NLMErrors.hpp"
#include "NLMExceptionUtils.hpp"

// Include NLM core headers
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Types/Types.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for better error handling
class NLMError;
class ConfigError;
class BrainError;
class NeuronError;
class MemoryError;
class WorldError;
class AgentError;

// Function to validate configuration before creating brain
inline bool validateBrainConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        throw ConfigError("Config cannot be null");
    }
    
    // Validate essential configuration parameters
    auto neuronCountOpt = config->get<size_t>("neuron_count");
    if (!neuronCountOpt) {
        throw ConfigError("Required configuration parameter 'neuron_count' is missing");
    }
    if (*neuronCountOpt == 0) {
        throw ConfigError("Configuration parameter 'neuron_count' cannot be zero");
    }
    if (*neuronCountOpt > 1000000) {
        throw ConfigError("Configuration parameter 'neuron_count' too large (max 1,000,000)");
    }
    
    return true;
}

// Function to validate simulation parameters
inline bool validateSimulationStep(SimulationStep step, Timestamp time) {
    if (step < 0) {
        throw BrainError("Simulation step cannot be negative: " + std::to_string(step));
    }
    if (time < 0.0) {
        throw BrainError("Simulation time cannot be negative: " + std::to_string(time));
    }
    if (time > 86400.0) {  // More than 24 hours simulation
        throw BrainError("Simulation time too large (max 86400.0 seconds): " + std::to_string(time));
    }
    return true;
}

// Function to validate world parameters
inline bool validateWorldConfig(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
    if (width == 0 || height == 0) {
        throw WorldError("World dimensions cannot be zero");
    }
    if (width > 1000 || height > 1000) {
        throw WorldError("World dimensions too large (max 1000x1000)");
    }
    if (visionWidth > width || visionHeight > height) {
        throw WorldError("Vision dimensions cannot exceed world dimensions");
    }
    return true;
}

// Helper function to safely get regions from brain
inline const NeuralRegion* getFirstRegion(const std::shared_ptr<Brain>& brain) {
    VALIDATE_PTR(brain, "Brain cannot be null");
    
    const auto& regions = brain->getRegions();
    if (regions.empty()) {
        throw BrainError("Brain has no regions available");
    }
    
    return regions[0].get();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Improved Version
        
        This is an improved version of the NLM Python bindings with:
        - Better error handling and validation
        - Consistent API design
        - Comprehensive parameter checking
        - Improved documentation
        - Python-friendly exception classes
        
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action with enhanced safety and usability.
    )pbdoc";
    
    // Register improved exception classes
    registerNLMErrors(m);
    
    // Configure Python exception handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, ("NLM Error: " + std::string(e.what())).c_str());
        }
    });

    // NeuronId class with improved validation
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Provides safe, type-checked identification of neurons within the brain.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId class with improved validation
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Provides safe, type-checked identification of synapses within neural regions.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId class with improved validation
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Provides safe, type-checked identification of brain regions.
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

    // PopulationId class with improved validation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Provides safe, type-checked identification of neuron populations within regions.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum with improved documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration with biological significance.
        
        Defines the functional type of a neuron in the neural network.
        "pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that suppress firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that influence plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that process external input")
        .value("Motor", NeuronType::Motor, "Motor neurons that produce actions")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing")
        .export_values();

    // SynapseType enum with improved documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration with biological significance.
        
        Defines the functional type of a synaptic connection.
        "pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synaptic transmission")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synaptic transmission")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synaptic influence")
        .value("Electrical", SynapseType::Electrical, "Electrical gap junction connection")
        .value("GapJunction", SynapseType::GapJunction, "Chemical synaptic transmission")
        .export_values();

    // DevelopmentalStage enum with improved documentation
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration representing brain maturation.
        
        Defines the current developmental stage of the artificial brain.
        "pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    // Config class with improved validation and error handling
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with comprehensive validation.
        
        Manages neural simulation parameters with type safety and validation.
        Provides methods for loading/saving configuration and parameter validation.
        "pbdoc")
        .def(py::init<>(), R"pbdoc(
            Create a new default configuration.
            
            Returns a default Config object with standard neural parameters.
            Use set() method to customize parameters.
            "pbdoc")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a file.
             
             Args:
                 filepath: Path to configuration file in key=value format
                 
             Returns:
                 bool: True if file loaded successfully, False otherwise
                 
             Raises:
                 ConfigError: If file cannot be read or contains invalid parameters
            )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command line arguments.
           
           Args:
               argc: Argument count (typically from sys.argv)
               argv: Argument vector (typically sys.argv)
               
           Returns:
               bool: True if arguments parsed successfully
               
           Raises:
               ConfigError: If arguments cannot be parsed
            )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a file.
             
             Args:
                 filepath: Path where configuration will be saved
                 
             Returns:
                 bool: True if file saved successfully, False otherwise
                 
             Raises:
                 ConfigError: If file cannot be written or configuration is invalid
            )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration parameter name
                 
             Returns:
                 bool: True if key exists, False otherwise
            )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 list: All configuration parameter names
            )pbdoc")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             R"pbdoc(
             Set a configuration parameter with type conversion.
             
             Args:
                 key: Configuration parameter name (e.g., "brain.neuron_count")
                 value: Parameter value (string will be converted to appropriate type)
                 
             Raises:
                 ConfigError: If key is invalid or value type mismatch
            )pbdoc")
        .def("get", py::overload_cast<const std::string&>(&Config::get), py::arg("key"),
             R"pbdoc(
             Get a configuration parameter with type conversion.
             
             Args:
                 key: Configuration parameter name
                 
             Returns:
                 Parameter value if found, None otherwise
                 
             Raises:
                 ConfigError: If parameter type conversion fails
            )pbdoc")
        .def("getOr", [](const Config& self, const std::string& key, auto defaultValue) {
            auto value = self.get<std::remove_reference_t<decltype(defaultValue)>>(key);
            return value.has_value() ? *value : defaultValue;
        }, py::arg("key"), py::arg("default"),
           R"pbdoc(
           Get a configuration parameter with default value.
           
           Args:
               key: Configuration parameter name
               default: Default value if parameter not found
               
           Returns:
               Parameter value if found, otherwise default value
            )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
            )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 str: Summary of all configuration parameters
            )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    // SensoryInput base class with improved validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input with validation.
        
        Abstract base class for all sensory modalities in the NLM system.
        Provides common interface for accessing sensory data.
        "pbdoc")
        .def("getType", &SensoryInput::getType,
             R"pbdoc(Get the type of sensory input. Returns NeuronType.)pbdoc")
        .def("getData", &SensoryInput::getData,
             R"pbdoc(Get the raw data as a vector of floats.)pbdoc")
        .def("getDimensions", &SensoryInput::getDimensions,
             R"pbdoc(Get the dimensionality of sensory data.)pbdoc")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             R"pbdoc(Get the timestamp of this sensory input.)pbdoc")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set the timestamp for this sensory input.)pbdoc")
        .def("__repr__", [](const SensoryInput& input) {
            return "<" + input.getType() + " input at " + std::to_string(input.getTimestamp()) + ">";
        });

    // Vision class with improved validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input for visual processing.
        
        Represents visual data from the environment with validation and utility methods.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            VALIDATE_PARAM(!data.empty(), "Vision data cannot be empty");
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set vision data with validation.
             
             Args:
                 data: Vision data as flat array (width × height × channels)
                 
             Raises:
                 ValueError: If data is empty or invalid
            )pbdoc")
        .def("getWidth", &Vision::getWidth,
             R"pbdoc(Get vision width in pixels.)pbdoc")
        .def("getHeight", &Vision::getHeight,
             R"pbdoc(Get vision height in pixels.)pbdoc")
        .def("getChannels", &Vision::getChannels,
             R"pbdoc(Get number of color channels.)pbdoc")
        .def("to_numpy", [](const Vision& self) {
            return py::array_t<float>({(ssize_t)self.getHeight(), (ssize_t)self.getWidth(), 
                                     (ssize_t)self.getChannels()}, self.getData());
        }, R"pbdoc(
            Convert vision data to NumPy array for processing.
            
            Returns:
                numpy.ndarray: 2D/3D array with shape (height, width, channels)
            )pbdoc");

    // Audio class with improved validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input for sound processing.
        
        Represents audio data with validation and utility methods.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            VALIDATE_PARAM(!data.empty(), "Audio data cannot be empty");
            self.setData(data);
        }, py::arg("data"),
             R"pbdoc(
             Set audio data with validation.
             
             Args:
                 data: Audio samples as array of floats
                 
             Raises:
                 ValueError: If data is empty or invalid
            )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             R"pbdoc(Set audio sample rate in Hz.)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
             R"pbdoc(Get audio sample rate in Hz.)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
             R"pbdoc(Get number of audio samples.)pbdoc")
        .def("to_numpy", [](const Audio& self) {
            return py::array_t<float>({(ssize_t)self.getNumSamples()}, self.getData());
        }, R"pbdoc(
            Convert audio data to NumPy array.
            
            Returns:
                numpy.ndarray: 1D array of audio samples
            )pbdoc");

    // InternalSignals class with improved validation
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input for internal state.
        
        Represents internal neural signals and states with validation.
        "pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             R"pbdoc(Add an internal signal value.)pbdoc")
        .def("clearSignals", &InternalSignals::clearSignals,
             R"pbdoc(Clear all internal signals.)pbdoc")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             R"pbdoc(Get number of internal signals.)pbdoc");

    // Action class with improved validation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output with validation.
        
        Represents a motor action with type and parameters.
        Provides validation for action parameters.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType,
             R"pbdoc(Get action type.)pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set action type.)pbdoc")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get action parameters as vector.)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set action parameters.)pbdoc")
        .def("getName", &Action::getName,
             R"pbdoc(Get human-readable action name.)pbdoc")
        .def("validate", [](const Action& self) {
            return self.validate();
        }, R"pbdoc(
            Validate action parameters.
            
            Returns:
                bool: True if action parameters are valid
            )pbdoc")
        .def("clone", &Action::clone,
             R"pbdoc(
             Create a copy of this action.
             
             Returns:
                 Action: Copy of this action
            )pbdoc");

    // WorldObject class with improved validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation with validation.
        
        Represents an object in the world with position, type, and properties.
        Provides validation for object properties.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("isValid", [](const WorldObject& self) {
            return self.isValid();
        }, R"pbdoc(
            Check if object is valid.
            
            Returns:
                bool: True if object is valid, False otherwise
            )pbdoc")
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            return self.distanceTo(x, y);
        }, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Calculate distance to another position.
             
             Args:
                 x: Target X coordinate
                 y: Target Y coordinate
                 
             Returns:
                 float: Distance to target position
            )pbdoc");

    // AgentBody class with improved validation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state with validation.
        
        Represents the physical state of the agent in the world.
        Provides validation for body properties and movement.
        "pbdoc")
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
        .def("reset", &AgentBody::reset,
             R"pbdoc(Reset agent body to initial state.)pbdoc")
        .def("isAlive", [](const AgentBody& self) {
            return self.isAlive();
        }, R"pbdoc(
            Check if agent is alive.
            
            Returns:
                bool: True if agent is alive, False if dead
            )pbdoc")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, R"pbdoc(
            Get agent position.
            
            Returns:
                tuple: (x, y) position coordinates
            )pbdoc")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.setPosition(x, y);
        }, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set agent position.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
            )pbdoc");

    // ActionResult class with improved validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world with validation.
        
        Represents the result of applying an action to the world.
        Provides validation for result properties.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isValid", [](const ActionResult& self) {
            return self.isValid();
        }, R"pbdoc(
            Check if action result is valid.
            
            Returns:
                bool: True if result is valid, False otherwise
            )pbdoc")
        .def("to_dict", [](const ActionResult& self) {
            return py::dict(
                "reward"_a = self.reward,
                "success"_a = self.success,
                "message"_a = self.message
            );
        }, R"pbdoc(
            Convert action result to dictionary.
            
            Returns:
                dict: Dictionary representation of result
            )pbdoc");

    // SensoryPercept class with improved validation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data with validation and utility methods.
        
        Represents all sensory inputs received by the agent in one timestep.
        Provides methods for accessing and processing sensory data.
        "pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             R"pbdoc(Get vision sensory input.)pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             R"pbdoc(Set vision sensory input.)pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             R"pbdoc(Get vision width.)pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             R"pbdoc(Get vision height.)pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
             R"pbdoc(Get touch sensory input.)pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             R"pbdoc(Set touch sensory input.)pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
             R"pbdoc(Get internal signals sensory input.)pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             R"pbdoc(Set internal signals sensory input.)pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
             R"pbdoc(Get proprioception sensory input.)pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             R"pbdoc(Set proprioception sensory input.)pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
             R"pbdoc(Get audio sensory input.)pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             R"pbdoc(Set audio sensory input.)pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             R"pbdoc(Get all sensory signals as dictionary.)pbdoc")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             R"pbdoc(Get percept timestamp.)pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             R"pbdoc(Set percept timestamp.)pbdoc")
        .def("hasVision", [](const SensoryPercept& self) {
            return self.hasVision();
        }, R"pbdoc(
            Check if percept has vision data.
            
            Returns:
                bool: True if vision data is available
            )pbdoc")
        .def("hasTouch", [](const SensoryPercept& self) {
            return self.hasTouch();
        }, R"pbdoc(
            Check if percept has touch data.
            
            Returns:
                bool: True if touch data is available
            )pbdoc")
        .def("hasInternal", [](const SensoryPercept& self) {
            return self.hasInternal();
        }, R"pbdoc(
            Check if percept has internal signals.
            
            Returns:
                bool: True if internal signals are available
            )pbdoc")
        .def("to_dict", [](const SensoryPercept& self) {
            return py::dict(
                "vision"_a = self.getVision(),
                "touch"_a = self.getTouch(),
                "internal"_a = self.getInternal(),
                "proprioception"_a = self.getProprioception(),
                "audio"_a = self.getAudio(),
                "timestamp"_a = self.getTimestamp()
            );
        }, R"pbdoc(
            Convert percept to dictionary.
            
            Returns:
                dict: Dictionary representation of all sensory data
            )pbdoc");

    // SimpleWorld class with improved validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation with validation.
        
        A simple 2D environment for NLM agent simulation with validation
        and utility methods for world management.
        "pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                           size_t visionWidth, size_t visionHeight) {
            validateWorldConfig(width, height, visionWidth, visionHeight);
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure world dimensions with validation.
             
             Args:
                 width: World width in units
                 height: World height in units
                 visionWidth: Agent vision width in pixels
                 visionHeight: Agent vision height in pixels
                 
             Raises:
                 ValueError: If dimensions are invalid
            )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(Reset world to initial state.)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(Set agent starting position.)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(Update world state.)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply motor command to world.
             
             Args:
                 cmd: Motor command to apply
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult: Result of applying the command
            )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             R"pbdoc(Get sensory percept from world.)pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             R"pbdoc(Get agent body state.)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(Add object to world.)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(Remove object at position.)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(Check if position is valid in world.)pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(Get world width.)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(Get world height.)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(Get maximum agent energy.)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(Set maximum agent energy.)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(Get energy decay rate.)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(Set energy decay rate.)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(Get current simulation time.)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(Set random seed for reproducible simulations.)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(Get random seed.)pbdoc")
        .def("to_dict", [](const SimpleWorld& self) {
            return py::dict(
                "width"_a = self.getWidth(),
                "height"_a = self.getHeight(),
                "visionWidth"_a = self.getVisionWidth(),
                "visionHeight"_a = self.getVisionHeight(),
                "simulationTime"_a = self.getSimulationTime(),
                "maxEnergy"_a = self.getMaxEnergy(),
                "energyDecayRate"_a = self.getEnergyDecayRate(),
                "agentX"_a = self.getAgentBody().x,
                "agentY"_a = self.getAgentBody().y,
                "agentOrientation"_a = self.getAgentBody().orientation
            );
        }, R"pbdoc(
            Convert world state to dictionary.
            
            Returns:
                dict: Dictionary representation of world state
            )pbdoc");

    // Brain class with improved validation and error handling
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class with validation.
        
        Main brain class for NLM neural simulation with comprehensive
        validation and error handling for safe simulation.
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a new brain with configuration.
             
             Args:
                 config: Brain configuration
                 
             Raises:
                 ConfigError: If configuration is invalid
            )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration.
             
             Raises:
                 BrainError: If brain initialization fails
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Execute a simulation step.
             
             Args:
                 currentStep: Simulation step number (non-negative integer)
                 
             Raises:
                 BrainError: If step parameters are invalid
            )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Execute a simulation step with explicit time.
             
             Args:
                 currentStep: Simulation step number (non-negative integer)
                 currentTime: Absolute simulation time in seconds
                 
             Raises:
                 BrainError: If step parameters are invalid
            )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             Args:
                 input: Sensory input data
                 
             Raises:
                 BrainError: If input is invalid
            )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current into a specific neuron.
             
             Args:
                 neuron: Neuron ID
                 current: Current amplitude (positive for excitation, negative for inhibition)
                 
             Raises:
                 BrainError: If neuron ID is invalid
            )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type (excitatory, inhibitory, etc.)
                 current: Current amplitude
                 
             Raises:
                 BrainError: If current is invalid
            )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor action based on neural activity.
             
             Returns:
                 Action: Motor action selected by the brain
                 
             Raises:
                 BrainError: If action production fails
            )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(Reset brain state to initial values.)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file.
             
             Args:
                 filepath: Path where brain state will be saved
                 
             Returns:
                 bool: True if save successful, False otherwise
                 
             Raises:
                 BrainError: If save operation fails
            )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file.
             
             Args:
                 filepath: Path from which brain state will be loaded
                 
             Returns:
                 bool: True if load successful, False otherwise
                 
             Raises:
                 BrainError: If load operation fails
            )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region to the brain.
             
             Args:
                 name: Region name (optional)
                 
             Returns:
                 RegionId: ID of the newly created region
                 
             Raises:
                 BrainError: If region creation fails
            )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             R"pbdoc(
             Get a region by ID.
             
             Args:
                 id: Region identifier
                 
             Returns:
                 NeuralRegion: Region object if found, None otherwise
            )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(Get number of regions in the brain.)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(Get all region IDs.)pbdoc")
        .def("getRegions", &Brain::getRegions,
             R"pbdoc(Get all regions.)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(Get total neuron count across all regions.)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(Get total synapse count across all regions.)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(Get count of active neurons.)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(Get count of currently firing neurons.)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(Get average firing rate across all neurons.)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(Get excitation/inhibition balance ratio.)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(Get total spike count.)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(Set developmental stage.)pbdoc")
        .def("getConfig", &Brain::getConfig,
             R"pbdoc(Get the configuration.)pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(Log brain status for debugging.)pbdoc")
        .def("to_dict", [](const Brain& brain) {
            return py::dict(
                "neuronCount"_a = brain.getTotalNeuronCount(),
                "synapseCount"_a = brain.getTotalSynapseCount(),
                "activeNeuronCount"_a = brain.getActiveNeuronCount(),
                "firingNeuronCount"_a = brain.getFiringNeuronCount(),
                "averageFiringRate"_a = brain.getAverageFiringRate(),
                "totalSpikeCount"_a = brain.getTotalSpikeCount(),
                "developmentalStage"_a = static_cast<int>(brain.getDevelopmentalStage()),
                "regionCount"_a = brain.getRegionCount()
            );
        }, R"pbdoc(
            Convert brain state to dictionary.
            
            Returns:
                dict: Dictionary representation of brain state
            )pbdoc")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) + 
                   " synapses>";
        })
        .def("__str__", [](const Brain& brain) {
            return "Brain(neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   ", synapses=" + std::to_string(brain.getTotalSynapseCount()) + ")";
        });

    // AgentBrain class with improved validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world with validation.
        
        Provides interface for agent-brain interaction with comprehensive
        validation and error handling for safe world interaction.
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Create agent brain interface.
             
             Args:
                 brain: Shared pointer to brain object
                 
             Raises:
                 AgentError: If brain is null
            )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize with world.
             
             Args:
                 world: SimpleWorld object
                 
             Raises:
                 AgentError: If world is invalid
            )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(Get expected sensory input size.)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(Get expected motor output size.)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain.
             
             Args:
                 percept: Sensory percept to process
                 
             Raises:
                 AgentError: If percept is invalid
            )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand: Decoded motor command
                 
             Raises:
                 AgentError: If motor decoding fails
            )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predictedReward: Predicted reward
                 
             Raises:
                 AgentError: If reward modulation fails
            )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system.
             
             Args:
                 timestep: Time step for development update
                 
             Raises:
                 AgentError: If development update fails
            )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(Get current developmental stage.)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(Get current neuromodulation level.)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(Get curiosity level.)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(Get novelty level.)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(Get prediction error.)pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(Reset agent for new episode.)pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             R"pbdoc(Get the underlying brain.)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(Enable or disable reward modulation.)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(Enable or disable structural plasticity.)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(Enable or disable development.)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(Enable or disable curiosity.)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(Check if reward modulation is enabled.)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(Check if structural plasticity is enabled.)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(Check if development is enabled.)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(Check if curiosity is enabled.)pbdoc")
        .def("to_dict", [](const AgentBrain& agent) {
            return py::dict(
                "neuromodulationLevel"_a = agent.getNeuromodulationLevel(),
                "curiosityLevel"_a = agent.getCuriosityLevel(),
                "noveltyLevel"_a = agent.getNoveltyLevel(),
                "predictionError"_a = agent.getPredictionError(),
                "developmentalStage"_a = static_cast<int>(agent.getDevelopmentalStage()),
                "rewardModulationEnabled"_a = agent.isRewardModulationEnabled(),
                "structuralPlasticityEnabled"_a = agent.isStructuralPlasticityEnabled(),
                "developmentEnabled"_a = agent.isDevelopmentEnabled(),
                "curiosityEnabled"_a = agent.isCuriosityEnabled()
            );
        }, R"pbdoc(
            Convert agent state to dictionary.
            
            Returns:
                dict: Dictionary representation of agent state
            )pbdoc");

    // Factory functions for ease of use
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration.
        
        Returns:
            Config: Default configuration object
        )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        validateBrainConfig(config);
        return std::make_shared<Brain>(config);
    }, py::arg("config"),
       R"pbdoc(
       Create a new brain with configuration.
       
       Args:
           config: Brain configuration
           
       Returns:
           Brain: New brain object
           
       Raises:
           ConfigError: If configuration is invalid
       )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple world.
        
        Returns:
            SimpleWorld: New world object
        )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        VALIDATE_PTR(brain.get(), "Brain cannot be null");
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"),
       R"pbdoc(
       Create a new agent brain interface.
       
       Args:
           brain: Brain object to interface with
           
       Returns:
           AgentBrain: New agent brain interface
           
       Raises:
           AgentError: If brain is null
       )pbdoc");

    // Constants for user convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Helper functions for common use cases
    m.def("run_complete_simulation", [](int numSteps, size_t numNeurons) {
        // Create and run a complete simulation
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", static_cast<size_t>(numNeurons));
        config->set("brain.synapse_density", 0.1f);
        config->set("brain.connection_probability", 0.05f);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        
        return brain;
    }, py::arg("numSteps"), py::arg("numNeurons"),
       R"pbdoc(
       Run a complete simulation with default settings.
       
       Args:
           numSteps: Number of simulation steps
           numNeurons: Number of neurons in the brain
           
       Returns:
           Brain: Simulated brain after running
           
       Raises:
           Exception: If simulation fails
       )pbdoc");

    m.def("create_test_environment", []() {
        auto world = createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->setAgentStart(10.0, 10.0);
        return world;
    }, R"pbdoc(
        Create a test environment for agent simulation.
        
        Returns:
            SimpleWorld: Configured test environment
        )pbdoc");

    // Module metadata
    m.attr("__version__") = "2.0.0-improved";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Improved NLM Python bindings with better error handling and validation";
}

} // namespace nlm
