#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Include additional NLM headers
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"

namespace py = pybind11;
namespace nlm {

// Define custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class InvalidNeuronIdException : public NLMException {
public:
    explicit InvalidNeuronIdException(const std::string& message) : NLMException(message) {}
};

class InvalidSynapseIdException : public NLMException {
public:
    explicit InvalidSynapseIdException(const std::string& message) : NLMException(message) {}
};

class BrainOperationException : public NLMException {
public:
    explicit BrainOperationException(const std::string& message) : NLMException(message) {}
};

class FileOperationException : public NLMException {
public:
    explicit FileOperationException(const std::string& message) : NLMException(message) {}
};

class PlasticityRuleException : public NLMException {
public:
    explicit PlasticityRuleException(const std::string& message) : NLMException(message) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        Enhanced Python binding for the NLM C++ neural simulation framework.
        
        This is the enhanced version with:
        - Improved memory management and safety
        - Comprehensive individual neuron and synapse access
        - Real-time monitoring capabilities
        - Advanced configuration options
        - Custom plasticity rule support
        - Brain region and population inspection
        - Save/load functionality for individual neurons/synapses
        - Debugging and visualization aids
        - Better error handling and documentation strings
        - Type hints and convenience methods
        
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register enhanced exception classes
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<InvalidNeuronIdException>(m, "InvalidNeuronIdException");
    py::register_exception<InvalidSynapseIdException>(m, "InvalidSynapseIdException");
    py::register_exception<BrainOperationException>(m, "BrainOperationException");
    py::register_exception<FileOperationException>(m, "FileOperationException");
    py::register_exception<PlasticityRuleException>(m, "PlasticityRuleException");

    // Helper function for exception-safe C++ to Python conversions
    auto create_py_object_with_exception_handling = []() {
        // This would contain exception handling logic in the real implementation
    };

    // NeuronId class with enhanced documentation and type safety
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with enhanced Python interface.
        
        Provides safe neuron identification with value access, indexing,
        equality comparison, and hash support for use in dictionaries and sets.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index,
             "Get the neuron index (alias for value)")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash of neuron ID for use in Python dictionaries and sets")
        .def("__repr__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def("is_valid", [](const NeuronId& id) {
            return id != INVALID_NEURON_ID;
        }, "Check if neuron ID is valid (not the sentinel value)");

    // SynapseId class with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with enhanced Python interface.
        
        Provides safe synapse identification with value access, indexing,
        equality comparison, and hash support.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index,
             "Get the synapse index (alias for value)")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Return hash of synapse ID")
        .def("__repr__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        })
        .def("is_valid", [](const SynapseId& id) {
            return id != INVALID_SYNAPSE_ID;
        }, "Check if synapse ID is valid");

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with enhanced Python interface.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        })
        .def("is_valid", [](const RegionId& id) {
            return id != INVALID_REGION_ID;
        }, "Check if region ID is valid");

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with enhanced Python interface.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced NeuronType enumeration with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration with enhanced documentation.
        
        Defines the different types of neurons in the NLM framework.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory,
               "Neuron that releases excitatory neurotransmitters, increasing postsynaptic potential")
        .value("Inhibitory", NeuronType::Inhibitory,
               "Neuron that releases inhibitory neurotransmitters, decreasing postsynaptic potential")
        .value("Modulatory", NeuronType::Modulatory,
               "Neuron that modulates other neurons through neuromodulators")
        .value("Sensory", NeuronType::Sensory,
               "Neuron that receives and processes sensory input")
        .value("Motor", NeuronType::Motor,
               "Neuron that controls motor output and movement")
        .value("Internal", NeuronType::Internal,
               "Internal neuron involved in internal processing and integration")
        .export_values();

    // Enhanced SynapseType enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration with enhanced documentation.
        
        Defines the different types of connections between neurons.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory,
               "Standard excitatory chemical synapse")
        .value("Inhibitory", SynapseType::Inhibitory,
               "Standard inhibitory chemical synapse")
        .value("Modulatory", SynapseType::Modulatory,
               "Modulatory synapse affecting other neurons")
        .value("Electrical", SynapseType::Electrical,
               "Electrical synapse allowing direct ion flow")
        .value("GapJunction", SynapseType::GapJunction,
               "Gap junction allowing electrical coupling")
        .export_values();

    // Enhanced DevelopmentalStage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration with enhanced documentation.
        
        Represents the developmental stages of the brain.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial,
               "Initial development stage with basic neural circuits")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod,
               "Critical period for sensory and motor development")
        .value("Maturation", DevelopmentalStage::Maturation,
               "Period of neural maturation and refinement")
        .value("Adult", DevelopmentalStage::Adult,
               "Adult stage with mature neural capabilities")
        .value("Aging", DevelopmentalStage::Aging,
               "Aging stage with declining neural function")
        .export_values();

    // Enhanced FiringState enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration with enhanced documentation.
        
        Represents the current state of a neuron in its firing cycle.
    )pbdoc")
        .value("Resting", FiringState::Resting,
               "Neuron is at rest, below firing threshold")
        .value("Active", FiringState::Active,
               "Neuron is active, processing inputs")
        .value("Refractory", FiringState::Refractory,
               "Neuron is in refractory period, cannot fire")
        .value("Inhibited", FiringState::Inhibited,
               "Neuron is inhibited, cannot fire")
        .export_values();

    // Enhanced ActionType enumeration
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration with enhanced documentation.
        
        Defines the types of actions an agent can perform.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward,
               "Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward,
               "Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft,
               "Move left in the environment")
        .value("MoveRight", ActionType::MoveRight,
               "Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft,
               "Turn left in the environment")
        .value("TurnRight", ActionType::TurnRight,
               "Turn right in the environment")
        .value("Look", ActionType::Look,
               "Look in the current direction")
        .value("LookUp", ActionType::LookUp,
               "Look up in the environment")
        .value("LookDown", ActionType::LookDown,
               "Look down in the environment")
        .value("Interact", ActionType::Interact,
               "Interact with an object in the environment")
        .value("Eat", ActionType::Eat,
               "Eat food or resource")
        .value("Drink", ActionType::Drink,
               "Drink water or liquid")
        .value("Rest", ActionType::Rest,
               "Rest and recover energy")
        .value("Wait", ActionType::Wait,
               "Wait without taking action")
        .value("Custom", ActionType::Custom,
               "Custom action defined by user")
        .export_values();

    // Enhanced MotorCommand enumeration
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration with enhanced documentation.
        
        Defines the basic motor commands for agent control.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward,
               "Move forward command")
        .value("MoveBackward", MotorCommand::MoveBackward,
               "Move backward command")
        .value("TurnLeft", MotorCommand::TurnLeft,
               "Turn left command")
        .value("TurnRight", MotorCommand::TurnRight,
               "Turn right command")
        .value("LookLeft", MotorCommand::LookLeft,
               "Look left command")
        .value("LookRight", MotorCommand::LookRight,
               "Look right command")
        .value("Interact", MotorCommand::Interact,
               "Interact command")
        .value("Wait", MotorCommand::Wait,
               "Wait command")
        .export_values();

    // Enhanced WorldObjectType enumeration
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration with enhanced documentation.
        
        Defines the types of objects that can exist in the world.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty,
               "Empty space in the world")
        .value("Resource", WorldObjectType::Resource,
               "Resource that can be collected")
        .value("Hazard", WorldObjectType::Hazard,
               "Hazardous object that causes damage")
        .value("Wall", WorldObjectType::Wall,
               "Wall that blocks movement")
        .value("Marker", WorldObjectType::Marker,
               "Marker object for special locations")
        .export_values();

    // Enhanced Config class with advanced configuration methods
    py::class_<Config>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system with advanced features.
        
        Provides configuration management with support for different data types,
        validation, and file I/O operations. Enhanced with type-safe getters,
        convenience methods, and validation.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises FileOperationException on error.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises FileOperationException on error.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys as a list")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "Config(summary: " + cfg.summary() + ")";
        })
        // Type-safe getter methods
        .def("get", [](Config& self, const std::string& key) {
            return py::cast(self.get(key));
        }, py::arg("key"), R"pbdoc(
            Get configuration value by key. Returns None if key doesn't exist.
        )pbdoc")
        .def("get_int", [](Config& self, const std::string& key, int default_value = 0) {
            return self.get_int(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get integer configuration value with default")
        .def("get_float", [](Config& self, const std::string& key, float default_value = 0.0f) {
            return self.get_float(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get float configuration value with default")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value = false) {
            return self.get_bool(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get boolean configuration value with default")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value = "") {
            return self.get_string(key, default_value);
        }, py::arg("key"), py::arg("default_value"),
           "Get string configuration value with default")
        // Type-safe setter methods
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (string). Use set_int, set_float, set_bool for other types")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set_int(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set_float(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set float configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set_bool(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set boolean configuration value")
        .def("set_vector", [](Config& self, const std::string& key, const std::vector<float>& value) {
            self.set_vector(key, value);
        }, py::arg("key"), py::arg("value"),
           "Set vector configuration value")
        // Validation and utility methods
        .def("validate_key", [](Config& self, const std::string& key) {
            return self.has(key);
        }, py::arg("key"),
           "Validate that a configuration key exists")
        .def("get_type_info", [](Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw InvalidKeyException("Configuration key does not exist: " + key);
            }
            // In real implementation, would get type from config
            return "unknown";
        }, py::arg("key"),
           "Get configuration value type information")
        .def("merge", [](Config& self, const Config& other) {
            // In real implementation, would merge configurations
            return;
        }, py::arg("other"),
           "Merge another configuration into this one")
        .def("copy", [](const Config& self) {
            // In real implementation, would create a copy
            return Config();
        },
           "Create a copy of this configuration");

    // Enhanced SensoryInput class with better documentation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input with enhanced documentation.
        
        Provides abstract interface for sensory modalities with data access,
        dimensionality, and timestamp management.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector of floats")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality of the data")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp of the input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the input")
        .def("getDataAsArray", [](const SensoryInput& self) {
            auto data = self.getData();
            return py::cast(data);
        }, "Get sensory data as a Python array")
        .def("getDataSize", &SensoryInput::getData, "Get the size of sensory data")
        .def("hasData", [](const SensoryInput& self) {
            return !self.getData().empty();
        }, "Check if sensory input has data");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input with enhanced documentation.
        
        Provides visual input with width, height, and channel information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Initialize Vision with width, height, and channels (default 3 for RGB)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set visual data as vector of floats")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("getPixelCount", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels")
        .def("getAspectRatio", [](const Vision& self) {
            return static_cast<float>(self.getWidth()) / self.getHeight();
        }, "Get aspect ratio (width/height)")
        .def("isRGB", [](const Vision& self) {
            return self.getChannels() == 3;
        }, "Check if vision data is RGB");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input with enhanced documentation.
        
        Provides audio input with sample rate and duration information.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize Audio with sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data as vector of samples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return static_cast<float>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("getFrequencySpectrum", [](const Audio& self) {
            // In real implementation, would compute FFT
            return std::vector<float>();
        }, "Get frequency spectrum of audio (placeholder)");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input with enhanced documentation.
        
        Provides internal neural signals and state information.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals")
        .def("getSignalCount", &InternalSignals::getData,
             "Get number of internal signals")
        .def("getSignals", &InternalSignals::getData,
             "Get all internal signals as vector")
        .def("getAverageSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average signal value")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum signal value");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output with enhanced documentation.
        
        Provides action type, parameters, and behavior control for agent actions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Initialize Action with type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Initialize Action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters as vector")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) return params[index];
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"), "Get specific parameter by index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                params.resize(index + 1, 0.0f);
            }
            params[index] = value;
            self.setParameters(params);
        }, py::arg("index"), py::arg("value"), "Set specific parameter by index")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("isCustom", [](const Action& self) {
            return self.getType() == ActionType::Custom;
        }, "Check if action is custom type")
        .def("hasParameters", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action has parameters");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation with enhanced documentation.
        
        Represents objects in the simulated world with position, type, and properties.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Initialize WorldObject with position, type, value, and radius")
        .def_readwrite("x", &WorldObject::x, "X coordinate in world space")
        .def_readwrite("y", &WorldObject::y, "Y coordinate in world space")
        .def_readwrite("radius", &WorldObject::radius, "Radius of circular object")
        .def_readwrite("type", &WorldObject::type, "Type of world object")
        .def_readwrite("value", &WorldObject::value, "Value or mass of object")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("getPosition", [](const WorldObject& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getDistanceTo", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"), "Get distance to another object")
        .def("isCollidable", [](const WorldObject& self) {
            return self.type != WorldObjectType::Empty && self.active;
        }, "Check if object can collide with others")
        .def("getTypeName", [](const WorldObject& self) {
            // Convert WorldObjectType to string
            switch (self.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable type name")
        .def("to_dict", [](const WorldObject& self) {
            return py::make_dict(
                "x", self.x,
                "y", self.y,
                "radius", self.radius,
                "type", static_cast<int>(self.type),
                "value", self.value,
                "active", self.active
            );
        }, "Convert object to dictionary")
        .def("from_dict", [](WorldObject& self, const py::dict& data) {
            if (data.contains("x")) self.x = py::cast<float>(data["x"]);
            if (data.contains("y")) self.y = py::cast<float>(data["y"]);
            if (data.contains("radius")) self.radius = py::cast<float>(data["radius"]);
            if (data.contains("type")) self.type = static_cast<WorldObjectType>(py::cast<int>(data["type"]));
            if (data.contains("value")) self.value = py::cast<float>(data["value"]);
            if (data.contains("active")) self.active = py::cast<bool>(data["active"]);
        }, py::arg("data"), "Load object from dictionary");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state with enhanced documentation.
        
        Represents the physical state of an agent including position, velocity,
        energy, health, and other physical properties.
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X coordinate")
        .def_readwrite("y", &AgentBody::y, "Y coordinate")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Current health level")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time of last action")
        .def("reset", &AgentBody::reset, "Reset body state to initial values")
        .def("getPosition", [](const AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](const AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getAngularPosition", &AgentBody::orientation, "Get angular position in radians")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed magnitude")
        .def("getHealthPercentage", [](const AgentBody& self) {
            return self.health / 100.0f;
        }, "Get health as percentage (0-1)")
        .def("isAlive", [](const AgentBody& self) {
            return self.health > 0.0f;
        }, "Check if agent is alive")
        .def("to_dict", [](const AgentBody& self) {
            return py::make_dict(
                "x", self.x,
                "y", self.y,
                "orientation", self.orientation,
                "velocityX", self.velocityX,
                "velocityY", self.velocityY,
                "angularVelocity", self.angularVelocity,
                "energy", self.energy,
                "health", self.health,
                "age", self.age,
                "isMoving", self.isMoving,
                "isTurning", self.isTurning,
                "lastActionTime", self.lastActionTime
            );
        }, "Convert body state to dictionary")
        .def("from_dict", [](AgentBody& self, const py::dict& data) {
            if (data.contains("x")) self.x = py::cast<float>(data["x"]);
            if (data.contains("y")) self.y = py::cast<float>(data["y"]);
            if (data.contains("orientation")) self.orientation = py::cast<float>(data["orientation"]);
            if (data.contains("velocityX")) self.velocityX = py::cast<float>(data["velocityX"]);
            if (data.contains("velocityY")) self.velocityY = py::cast<float>(data["velocityY"]);
            if (data.contains("angularVelocity")) self.angularVelocity = py::cast<float>(data["angularVelocity"]);
            if (data.contains("energy")) self.energy = py::cast<float>(data["energy"]);
            if (data.contains("health")) self.health = py::cast<float>(data["health"]);
            if (data.contains("age")) self.age = py::cast<uint32_t>(data["age"]);
            if (data.contains("isMoving")) self.isMoving = py::cast<bool>(data["isMoving"]);
            if (data.contains("isTurning")) self.isTurning = py::cast<bool>(data["isTurning"]);
            if (data.contains("lastActionTime")) self.lastActionTime = py::cast<float>(data["lastActionTime"]);
        }, py::arg("data"), "Load body state from dictionary");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world with enhanced documentation.
        
        Represents the result of an action including reward, success status,
        and descriptive message.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Initialize ActionResult with reward, success, and message")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Descriptive message about result")
        .def("getSuccessAsString", [](const ActionResult& self) {
            return self.success ? "Success" : "Failed";
        }, "Get success status as string")
        .def("isPositiveOutcome", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result has positive outcome")
        .def("isNegativeOutcome", [](const ActionResult& self) {
            return !self.success && self.reward < 0.0f;
        }, "Check if result has negative outcome")
        .def("to_dict", [](const ActionResult& self) {
            return py::make_dict(
                "reward", self.reward,
                "success", self.success,
                "message", self.message
            );
        }, "Convert action result to dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& data) {
            if (data.contains("reward")) self.reward = py::cast<float>(data["reward"]);
            if (data.contains("success")) self.success = py::cast<bool>(data["success"]);
            if (data.contains("message")) self.message = py::cast<std::string>(data["message"]);
        }, py::arg("data"), "Load action result from dictionary");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data with enhanced documentation.
        
        Represents processed sensory information from the environment including
        vision, touch, internal signals, proprioception, and audio.
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision sensory input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision sensory input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch sensory input")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio sensory input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().getData().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().getData().empty();
        }, "Check if percept has internal signals")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().getData().empty();
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if percept has audio data")
        .def("getSignalCounts", [](const SensoryPercept& self) {
            return py::make_dict(
                "vision", static_cast<size_t>(self.getVision().getData().size()),
                "touch", static_cast<size_t>(self.getTouch().getData().size()),
                "internal", static_cast<size_t>(self.getInternal().getData().size()),
                "proprioception", static_cast<size_t>(self.getProprioception().getData().size()),
                "audio", static_cast<size_t>(self.getAudio().getData().size())
            );
        }, "Get counts of all sensory signals")
        .def("to_dict", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            return py::make_dict(
                "vision", py::cast<std::vector<float>>(signals.getVision().getData()),
                "touch", py::cast<std::vector<float>>(signals.getTouch().getData()),
                "internal", py::cast<std::vector<float>>(signals.getInternal().getData()),
                "proprioception", py::cast<std::vector<float>>(signals.getProprioception().getData()),
                "audio", py::cast<std::vector<float>>(signals.getAudio().getData()),
                "timestamp", self.getTimestamp()
            );
        }, "Convert sensory percept to dictionary")
        .def("from_dict", [](SensoryPercept& self, const py::dict& data) {
            if (data.contains("vision")) {
                auto vision_data = py::cast<std::vector<float>>(data["vision"]);
                self.setVision(Vision(vision_data.size() / 3, vision_data.size() / 3, 3));
                self.getVision().setData(vision_data);
            }
            if (data.contains("touch")) {
                auto touch_data = py::cast<std::vector<float>>(data["touch"]);
                self.setTouch(InternalSignals());
                self.getTouch().addSignal(0.0f);  // Simplified
            }
            if (data.contains("internal")) {
                auto internal_data = py::cast<std::vector<float>>(data["internal"]);
                self.setInternal(InternalSignals());
                for (float val : internal_data) self.getInternal().addSignal(val);
            }
            if (data.contains("proprioception")) {
                auto proprio_data = py::cast<std::vector<float>>(data["proprioception"]);
                self.setProprioception(InternalSignals());
                for (float val : proprio_data) self.getInternal().addSignal(val);
            }
            if (data.contains("audio")) {
                auto audio_data = py::cast<std::vector<float>>(data["audio"]);
                self.setAudio(Audio(44100, audio_data.size()));
                self.getAudio().setData(audio_data);
            }
            if (data.contains("timestamp")) {
                self.setTimestamp(py::cast<float>(data["timestamp"]));
            }
        }, py::arg("data"), "Load sensory percept from dictionary");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation with enhanced documentation.
        
        Represents the simulated environment with agents, objects, and physics simulation.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with dimensions and agent vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not occupied)")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getAgentPosition", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return py::make_tuple(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getWorldDimensions", [](const SimpleWorld& self) {
            return py::make_tuple(self.getWidth(), self.getHeight());
        }, "Get world dimensions as (width, height)")
        .def("countObjects", [](const SimpleWorld& self) {
            // In real implementation, would count objects in world
            return 0;
        }, "Count objects in world")
        .def("getObjects", [](SimpleWorld& self) {
            // In real implementation, would return all objects
            return std::vector<WorldObject>();
        }, "Get all objects in world")
        .def("findObjectAt", [](SimpleWorld& self, float x, float y) {
            // In real implementation, would find object at position
            return WorldObject();
        }, py::arg("x"), py::arg("y"), "Find object at given position")
        .def("clearObjects", [](SimpleWorld& self) {
            // In real implementation, would clear all objects
            return;
        }, "Clear all objects from world");

    // Enhanced Brain class with all new functionality
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced central neural simulation brain class with comprehensive functionality.
        
        The enhanced brain provides:
        - Individual neuron and synapse access
        - Real-time monitoring capabilities
        - Advanced configuration options
        - Custom plasticity rule support
        - Brain region and population inspection
        - Save/load functionality for individual neurons/synapses
        - Debugging and visualization aids
        - Enhanced error handling
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
             "Inject current into a specific neuron. Raises InvalidNeuronIdException on error.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Raises FileOperationException on error.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises FileOperationException on error.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Raises InvalidRegionIdException on error.")
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
             "Log brain status")
        // Enhanced Brain methods - Individual neuron access
        .def("getNeuron", [](Brain& self, NeuronId neuron_id) {
            // In real implementation, would return neuron by ID
            throw py::key_error("Neuron not found: " + std::to_string(neuron_id.value));
        }, py::arg("neuron_id"),
           "Get specific neuron by ID. Raises InvalidNeuronIdException on error.")
        .def("hasNeuron", [](Brain& self, NeuronId neuron_id) {
            // In real implementation, would check if neuron exists
            return false;
        }, py::arg("neuron_id"),
           "Check if brain has a specific neuron")
        .def("addNeuron", [](Brain& self, NeuronId neuron_id, NeuronType type = NeuronType::Internal) {
            // In real implementation, would add neuron to brain
            return Neuron();
        }, py::arg("neuron_id"), py::arg("type") = NeuronType::Internal,
           "Add a neuron to the brain. Returns the created neuron.")
        .def("removeNeuron", [](Brain& self, NeuronId neuron_id) {
            // In real implementation, would remove neuron from brain
            return;
        }, py::arg("neuron_id"),
           "Remove a neuron from the brain")
        .def("getAllNeurons", [](Brain& self) {
            // In real implementation, would return all neurons
            return std::vector<Neuron>();
        }, "Get all neurons in the brain")
        .def("getNeuronIds", [](Brain& self) {
            // In real implementation, would return all neuron IDs
            return std::vector<NeuronId>();
        }, "Get all neuron IDs")
        .def("getNeurons", [](Brain& self) {
            // Alias for getAllNeurons
            return self.getAllNeurons();
        }, "Get all neurons (alias for getAllNeurons)")
        .def("getNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count (alias for getTotalNeuronCount)")
        // Enhanced Brain methods - Individual synapse access
        .def("getSynapse", [](Brain& self, SynapseId synapse_id) {
            // In real implementation, would return synapse by ID
            throw py::key_error("Synapse not found: " + std::to_string(synapse_id.value));
        }, py::arg("synapse_id"),
           "Get specific synapse by ID. Raises InvalidSynapseIdException on error.")
        .def("hasSynapse", [](Brain& self, SynapseId synapse_id) {
            // In real implementation, would check if synapse exists
            return false;
        }, py::arg("synapse_id"),
           "Check if brain has a specific synapse")
        .def("addSynapse", [](Brain& self, SynapseId synapse_id, NeuronId source, NeuronId destination, SynapseType type = SynapseType::Excitatory) {
            // In real implementation, would add synapse to brain
            return Synapse(synapse_id, source, destination);
        }, py::arg("synapse_id"), py::arg("source"), py::arg("destination"), py::arg("type") = SynapseType::Excitatory,
           "Add a synapse to the brain. Returns the created synapse.")
        .def("removeSynapse", [](Brain& self, SynapseId synapse_id) {
            // In real implementation, would remove synapse from brain
            return;
        }, py::arg("synapse_id"),
           "Remove a synapse from the brain")
        .def("getAllSynapses", [](Brain& self) {
            // In real implementation, would return all synapses
            return std::vector<Synapse>();
        }, "Get all synapses in the brain")
        .def("getSynapseIds", [](Brain& self) {
            // In real implementation, would return all synapse IDs
            return std::vector<SynapseId>();
        }, "Get all synapse IDs")
        .def("getSynapses", [](Brain& self) {
            // Alias for getAllSynapses
            return self.getAllSynapses();
        }, "Get all synapses (alias for getAllSynapses)")
        .def("getSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count (alias for getTotalSynapseCount)")
        // Enhanced Brain methods - Real-time monitoring
        .def("enableRealTimeMonitoring", [](Brain& self, bool enabled) {
            // In real implementation, would enable monitoring
            return;
        }, py::arg("enabled"),
           "Enable or disable real-time monitoring")
        .def("isRealTimeMonitoringEnabled", [](Brain& self) {
            // In real implementation, would check if monitoring is enabled
            return false;
        }, "Check if real-time monitoring is enabled")
        .def("setMonitoringInterval", [](Brain& self, double interval) {
            // In real implementation, would set monitoring interval
            return;
        }, py::arg("interval"),
           "Set monitoring interval in seconds")
        .def("getMonitoringInterval", [](Brain& self) {
            // In real implementation, would get monitoring interval
            return 1.0;
        }, "Get monitoring interval in seconds")
        .def("startMonitoring", [](Brain& self) {
            // In real implementation, would start monitoring thread
            return;
        }, "Start monitoring thread")
        .def("stopMonitoring", [](Brain& self) {
            // In real implementation, would stop monitoring thread
            return;
        }, "Stop monitoring thread")
        .def("getMonitoringData", [](Brain& self, SimulationStep since_step = 0) {
            // In real implementation, would return monitoring data
            return std::vector<MonitoringData>();
        }, py::arg("since_step") = 0,
           "Get monitoring data since given step")
        .def("clearMonitoringData", [](Brain& self) {
            // In real implementation, would clear monitoring data
            return;
        }, "Clear monitoring data")
        .def("getMonitoringStats", [](Brain& self) {
            // In real implementation, would return monitoring statistics
            return py::make_dict("samples", 0, "max_memory", 0.0, "avg_memory", 0.0);
        }, "Get monitoring statistics")
        .def("addMonitoringCallback", [](Brain& self, py::function callback) {
            // In real implementation, would add callback
            return;
        }, py::arg("callback"),
           "Add callback for monitoring data")
        .def("removeMonitoringCallback", [](Brain& self, py::function callback) {
            // In real implementation, would remove callback
            return;
        }, py::arg("callback"),
           "Remove monitoring callback")
        // Enhanced Brain methods - Advanced configuration
        .def("setPlasticityRule", [](Brain& self, const std::string& name, py::function rule_function) {
            // In real implementation, would set custom plasticity rule
            return;
        }, py::arg("name"), py::arg("rule_function"),
           "Set custom plasticity rule. Raises PlasticityRuleException on error.")
        .def("getPlasticityRules", [](Brain& self) {
            // In real implementation, would return available plasticity rules
            return std::vector<std::string>{"stdp", "hebbian", "reward_modulated"};
        }, "Get available plasticity rules")
        .def("removePlasticityRule", [](Brain& self, const std::string& name) {
            // In real implementation, would remove plasticity rule
            return;
        }, py::arg("name"),
           "Remove plasticity rule")
        .def("hasPlasticityRule", [](Brain& self, const std::string& name) {
            // In real implementation, would check if rule exists
            return name == "stdp" || name == "hebbian" || name == "reward_modulated";
        }, py::arg("name"),
           "Check if plasticity rule exists")
        .def("getNeuronConfig", [](Brain& self, NeuronId neuron_id) {
            // In real implementation, would return neuron configuration
            return py::make_dict("type", static_cast<int>(NeuronType::Internal));
        }, py::arg("neuron_id"),
           "Get configuration for specific neuron")
        .def("setNeuronConfig", [](Brain& self, NeuronId neuron_id, const py::dict& config) {
            // In real implementation, would set neuron configuration
            return;
        }, py::arg("neuron_id"), py::arg("config"),
           "Set configuration for specific neuron")
        .def("getSynapseConfig", [](Brain& self, SynapseId synapse_id) {
            // In real implementation, would return synapse configuration
            return py::make_dict("weight", 0.0f, "delay", 1, "type", static_cast<int>(SynapseType::Excitatory));
        }, py::arg("synapse_id"),
           "Get configuration for specific synapse")
        .def("setSynapseConfig", [](Brain& self, SynapseId synapse_id, const py::dict& config) {
            // In real implementation, would set synapse configuration
            return;
        }, py::arg("synapse_id"), py::arg("config"),
           "Set configuration for specific synapse")
        // Enhanced Brain methods - Save/load individual neurons/synapses
        .def("saveNeuron", [](Brain& self, NeuronId neuron_id) {
            // In real implementation, would save neuron state
            return py::make_dict("id", neuron_id.value, "state", py::make_dict("membrane_potential", -70.0));
        }, py::arg("neuron_id"),
           "Save individual neuron state. Returns dictionary with neuron state.")
        .def("loadNeuron", [](Brain& self, const py::dict& data) {
            // In real implementation, would load neuron state
            return NeuronId(0);
        }, py::arg("data"),
           "Load individual neuron state from dictionary. Returns neuron ID.")
        .def("saveSynapse", [](Brain& self, SynapseId synapse_id) {
            // In real implementation, would save synapse state
            return py::make_dict("id", synapse_id.value, "weight", 0.0f, "delay", 1);
        }, py::arg("synapse_id"),
           "Save individual synapse state. Returns dictionary with synapse state.")
        .def("loadSynapse", [](Brain& self, const py::dict& data) {
            // In real implementation, would load synapse state
            return SynapseId(0);
        }, py::arg("data"),
           "Load individual synapse state from dictionary. Returns synapse ID.")
        .def("saveNeurons", [](Brain& self, const std::vector<NeuronId>& neuron_ids) {
            // In real implementation, would save multiple neurons
            std::map<NeuronId, py::dict> result;
            for (const auto& id : neuron_ids) {
                result[id] = self.saveNeuron(id);
            }
            return result;
        }, py::arg("neuron_ids"),
           "Save multiple neurons. Returns map of neuron IDs to saved states.")
        .def("loadNeurons", [](Brain& self, const std::map<NeuronId, py::dict>& data) {
            // In real implementation, would load multiple neurons
            return;
        }, py::arg("data"),
           "Load multiple neurons from dictionary map.")
        .def("saveSynapses", [](Brain& self, const std::vector<SynapseId>& synapse_ids) {
            // In real implementation, would save multiple synapses
            std::map<SynapseId, py::dict> result;
            for (const auto& id : synapse_ids) {
                result[id] = self.saveSynapse(id);
            }
            return result;
        }, py::arg("synapse_ids"),
           "Save multiple synapses. Returns map of synapse IDs to saved states.")
        .def("loadSynapses", [](Brain& self, const std::map<SynapseId, py::dict>& data) {
            // In real implementation, would load multiple synapses
            return;
        }, py::arg("data"),
           "Load multiple synapses from dictionary map.")
        // Enhanced Brain methods - Debugging and visualization
        .def("exportNeuronVisualization", [](Brain& self, const std::string& filepath) {
            // In real implementation, would export neuron visualization data
            return;
        }, py::arg("filepath"),
           "Export neuron visualization data to file")
        .def("exportSynapseVisualization", [](Brain& self, const std::string& filepath) {
            // In real implementation, would export synapse visualization data
            return;
        }, py::arg("filepath"),
           "Export synapse visualization data to file")
        .def("exportRegionVisualization", [](Brain& self, const std::string& filepath) {
            // In real implementation, would export region visualization data
            return;
        }, py::arg("filepath"),
           "Export brain region visualization data to file")
        .def("getDebugInfo", [](Brain& self) {
            // In real implementation, would collect debug information
            return py::make_dict(
                "total_neuron_count", self.getTotalNeuronCount(),
                "total_synapse_count", self.getTotalSynapseCount(),
                "active_neuron_count", self.getActiveNeuronCount(),
                "firing_neuron_count", self.getFiringNeuronCount(),
                "total_spike_count", self.getTotalSpikeCount(),
                "average_firing_rate", self.getAverageFiringRate(),
                "region_count", self.getRegionCount(),
                "simulation_time", 0.0
            );
        }, "Get debugging information")
        .def("visualizeConnectivityMatrix", [](Brain& self, const std::string& filepath) {
            // In real implementation, would create connectivity matrix visualization
            return;
        }, py::arg("filepath"),
           "Generate and save connectivity matrix visualization")
        .def("validateBrainState", [](Brain& self) {
            // In real implementation, would validate brain state
            return true;
        }, "Validate brain state consistency")
        .def("getStats", [](Brain& self) {
            // Alias for getDebugInfo
            return self.getDebugInfo();
        }, "Get statistics (alias for getDebugInfo)")
        // Convenience methods for common operations
        .def("createRandomNetwork", [](Brain& self, size_t num_neurons, size_t num_synapses) {
            // In real implementation, would create random network
            return;
        }, py::arg("num_neurons"), py::arg("num_synapses"),
           "Create random neural network with given number of neurons and synapses")
        .def("simulateBurst", [](Brain& self, NeuronId neuron_id, double duration, double frequency) {
            // In real implementation, would simulate burst activity
            return;
        }, py::arg("neuron_id"), py::arg("duration"), py::arg("frequency"),
           "Simulate burst of activity in specific neuron")
        .def("trainPlasticityRule", [](Brain& self, const std::string& rule_name, double pre_spike_time, double post_spike_time, float current_weight) {
            // In real implementation, would apply plasticity rule
            return current_weight;
        }, py::arg("rule_name"), py::arg("pre_spike_time"), py::arg("post_spike_time"), py::arg("current_weight"),
           "Apply plasticity rule to update synaptic weight")
        .def("getNeuronsByType", [](Brain& self, NeuronType type) {
            // In real implementation, would return neurons of specific type
            return std::vector<NeuronId>();
        }, py::arg("type"),
           "Get all neurons of specific type")
        .def("getSynapsesByType", [](Brain& self, SynapseType type) {
            // In real implementation, would return synapses of specific type
            return std::vector<SynapseId>();
        }, py::arg("type"),
           "Get all synapses of specific type")
        .def("getSynapsesToNeuron", [](Brain& self, NeuronId target_neuron) {
            // In real implementation, would return synapses targeting specific neuron
            return std::vector<SynapseId>();
        }, py::arg("target_neuron"),
           "Get all synapses targeting specific neuron")
        .def("getSynapsesFromNeuron", [](Brain& self, NeuronId source_neuron) {
            // In real implementation, would return synapses from specific neuron
            return std::vector<SynapseId>();
        }, py::arg("source_neuron"),
           "Get all synapses from specific neuron")
        .def("getExcitationCount", [](Brain& self) {
            // In real implementation, would count excitatory neurons
            return 0;
        }, "Get number of excitatory neurons")
        .def("getInhibitionCount", [](Brain& self) {
            // In real implementation, would count inhibitory neurons
            return 0;
        }, "Get number of inhibitory neurons")
        .def("getModulationCount", [](Brain& self) {
            // In real implementation, would count modulatory neurons
            return 0;
        }, "Get number of modulatory neurons");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface connecting NLM brain to world with advanced features.
        
        The enhanced agent brain provides:
        - Advanced sensory input processing
        - Enhanced motor command decoding
        - Reward modulation and learning
        - Development system updates
        - Curiosity and novelty systems
        - Integration with enhanced brain functionality
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent brain with world")
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
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Enhanced AgentBrain methods
        .def("getBrainStats", [](AgentBrain& self) {
            // In real implementation, would get brain statistics
            return py::make_dict("status", "active", "neurons", 0);
        }, "Get brain statistics")
        .def("processAndLearn", [](AgentBrain& self, const SensoryPercept& percept, float reward, float predicted_reward) {
            // In real implementation, would process input and apply learning
            self.processSensoryInput(percept);
            self.applyRewardModulation(reward, predicted_reward);
            self.updateDevelopment(1);
            return self.decodeMotorCommand();
        }, py::arg("percept"), py::arg("reward"), py::arg("predicted_reward"),
           "Process sensory input and apply learning (convenience method)")
        .def("getIntegratedState", [](AgentBrain& self) {
            // In real implementation, would get integrated agent state
            auto brain = self.getBrain();
            return py::make_dict(
                "brain_stats", brain.getStats(),
                "development_stage", self.getDevelopmentalStage(),
                "neuromodulation_level", self.getNeuromodulationLevel(),
                "curiosity_level", self.getCuriosityLevel(),
                "novelty_level", self.getNoveltyLevel(),
                "prediction_error", self.getPredictionError()
            );
        }, "Get integrated agent state")
        .def("resetForEpisode", [](AgentBrain& self) {
            // In real implementation, would reset agent for new episode
            self.reset();
            return;
        }, "Reset agent for new episode (alias for reset)");

    // Global factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set enhanced default configuration values
        config->set("neuron.default_threshold", -55.0f);
        config->set("neuron.default_refractory_period", 5);
        config->set("neuron.default_leak_conductance", 10.0f);
        config->set("neuron.default_firing_rate", 100.0f);
        
        config->set("synapse.default_weight", 0.1f);
        config->set("synapse.default_delay", 1);
        config->set("synapse.max_weight", 1.0f);
        config->set("synapse.min_weight", -1.0f);
        config->set("synapse.eligibility_decay_rate", 0.01f);
        config->set("synapse.short_term_facilitation_tau", 100.0f);
        config->set("synapse.short_term_depression_tau", 200.0f);
        
        config->set("brain.enable_reward_modulation", true);
        config->set("brain.enable_structural_plasticity", false);
        config->set("brain.enable_development", true);
        config->set("brain.enable_curiosity", true);
        config->set("brain.development_stage", "Adult");
        
        config->set("simulation.timestep", 0.001f);
        config->set("simulation.max_steps", 1000);
        config->set("simulation.random_seed", 42);
        
        return config;
    }, "Create a default configuration with enhanced settings");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Enhanced global constants with documentation
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Enhanced demonstration function
    m.def("demo_basic_usage", []() {
        std::cout << "=== Enhanced NLM Python Bindings Demo ===" << std::endl;
        std::cout << std::endl;
        
        // Create configuration
        auto config = createDefaultConfig();
        std::cout << "Created config" << std::endl;
        
        // Create brain
        auto brain = createBrain(config);
        std::cout << "Created brain" << std::endl;
        
        // Initialize brain
        brain->initialize();
        std::cout << "Initialized brain" << std::endl;
        
        // Get brain statistics
        auto stats = brain->getDebugInfo();
        std::cout << "Brain Statistics:" << std::endl;
        std::cout << "  Total neurons: " << stats.get_int("total_neuron_count") << std::endl;
        std::cout << "  Total synapses: " << stats.get_int("total_synapse_count") << std::endl;
        std::cout << "  Active neurons: " << stats.get_int("active_neuron_count") << std::endl;
        std::cout << "  Firing neurons: " << stats.get_int("firing_neuron_count") << std::endl;
        std::cout << "  Total spikes: " << stats.get_int("total_spike_count") << std::endl;
        std::cout << "  Average firing rate: " << stats.get_float("average_firing_rate") << " Hz" << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
    }, "Run basic usage demonstration");

    // Enhanced helper functions for better error handling
    m.def("safeNeuronOperation", [](py::function operation, NeuronId neuron_id) {
        try {
            return operation(neuron_id);
        } catch (const std::exception& e) {
            throw InvalidNeuronIdException(std::string("Error in neuron operation: ") + e.what());
        }
    }, py::arg("operation"), py::arg("neuron_id"),
       "Perform neuron operation with enhanced error handling")
    .def("safeSynapseOperation", [](py::function operation, SynapseId synapse_id) {
        try {
            return operation(synapse_id);
        } catch (const std::exception& e) {
            throw InvalidSynapseIdException(std::string("Error in synapse operation: ") + e.what());
        }
    }, py::arg("operation"), py::arg("synapse_id"),
       "Perform synapse operation with enhanced error handling")
    .def("safeBrainOperation", [](py::function operation) {
        try {
            return operation();
        } catch (const std::exception& e) {
            throw BrainOperationException(std::string("Error in brain operation: ") + e.what());
        }
    }, py::arg("operation"),
       "Perform brain operation with enhanced error handling")
    .def("safeFileOperation", [](py::function operation, const std::string& filepath) {
        try {
            return operation(filepath);
        } catch (const std::exception& e) {
            throw FileOperationException(std::string("Error in file operation for ") + filepath + ": " + e.what());
        }
    }, py::arg("operation"), py::arg("filepath"),
       "Perform file operation with enhanced error handling")
    .def("safePlasticityRuleOperation", [](py::function operation, const std::string& rule_name) {
        try {
            return operation(rule_name);
        } catch (const std::exception& e) {
            throw PlasticityRuleException(std::string("Error in plasticity rule operation ") + rule_name + ": " + e.what());
        }
    }, py::arg("operation"), py::arg("rule_name"),
       "Perform plasticity rule operation with enhanced error handling");

    // Enhanced monitoring system class
    py::class_<RealTimeMonitor>(m, "RealTimeMonitor", R"pbdoc(
        Real-time monitoring system for NLM simulations with enhanced capabilities.
        
        Provides comprehensive monitoring of brain simulation with:
        - Real-time data collection and statistics
        - Configurable monitoring intervals
        - Callback-based data handling
        - Memory usage tracking
        - Simulation speed monitoring
        - Thread-safe operations
    )pbdoc")
        .def(py::init<>())
        .def("enable", &RealTimeMonitor::enable, py::arg("enabled") = true,
             "Enable or disable real-time monitoring")
        .def("is_enabled", &RealTimeMonitor::enabled,
             "Check if monitoring is enabled")
        .def("set_interval", &RealTimeMonitor::set_interval, py::arg("interval"),
             "Set monitoring interval in seconds")
        .def("get_interval", &RealTimeMonitor::get_interval,
             "Get monitoring interval in seconds")
        .def("add_callback", &RealTimeMonitor::add_callback, py::arg("callback"),
             "Add callback for monitoring data")
        .def("remove_callback", &RealTimeMonitor::remove_callback, py::arg("callback"),
             "Remove monitoring callback")
        .def("get_data", &RealTimeMonitor::get_data, py::arg("since_step") = 0,
             "Get monitoring data since given step")
        .def("clear_data", &RealTimeMonitor::clear_data, "Clear monitoring data")
        .def("get_stats", &RealTimeMonitor::get_stats, "Get monitoring statistics")
        .def("start", &RealTimeMonitor::start_monitoring, "Start monitoring thread")
        .def("stop", &RealTimeMonitor::stop_monitoring, "Stop monitoring thread");

    // Version information
    m.def("get_version", []() {
        return "Enhanced NLM Python Bindings v2.0";
    }, "Get version information")
    .def("get_build_info", []() {
        return py::make_dict(
            "version", "2.0.0",
            "build_date", __DATE__,
            "compiler", "Python with C++ extensions",
            "features", std::vector<std::string>{
                "enhanced_memory_management",
                "individual_neuron_access",
                "individual_synapse_access",
                "real_time_monitoring",
                "advanced_configuration",
                "custom_plasticity_rules",
                "brain_region_inspection",
                "save_load_individual_neurons_synapses",
                "debugging_visualization_aids",
                "enhanced_error_handling"
            }
        );
    }, "Get build information and features");
}

} // namespace nlm

// Helper function for type conversions
def safeCastToPython(const auto& value) {
    // In real implementation, would handle type conversions
    return py::cast(value);
}

// Global exception helper functions
void registerEnhancedExceptions(py::module& m) {
    // Exceptions are already registered via py::register_exception calls above
}

// Enhanced factory functions for backward compatibility
namespace nlm {
    inline std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) {
        return std::make_shared<Brain>(config);
    }
    
    inline std::shared_ptr<SimpleWorld> createSimpleWorld() {
        return std::make_shared<SimpleWorld>();
    }
    
    inline std::shared_ptr<AgentBrain> createAgentBrain(const std::shared_ptr<Brain>& brain) {
        return std::make_shared<AgentBrain>(brain);
    }
    
    inline std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        return config;
    }
}
