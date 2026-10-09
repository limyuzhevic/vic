#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

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

// Python-specific exception classes
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& msg) : NLMException("Validation Error: " + msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& msg) : NLMException("Resource Error: " + msg) {}
};

// Helper functions for validation
template<typename T>
T validatePositive(const T& value, const std::string& paramName) {
    if (value <= 0) {
        throw ValidationError(paramName + " must be positive, got " + std::to_string(value));
    }
    return value;
}

template<typename T>
T validateRange(const T& value, const T& min, const T& max, const std::string& paramName) {
    if (value < min || value > max) {
        throw ValidationError(paramName + " must be between " + std::to_string(min) + 
                            " and " + std::to_string(max) + ", got " + std::to_string(value));
    }
    return value;
}

// Create a generic ID class template to reduce code duplication
template<typename IDType>
void bindGenericId(py::module_& m, const std::string& className) {
    std::string doc = "Unique identifier for " + className;
    
    py::class_<IDType>(m, className.c_str(), py::doc(doc.c_str()))
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const IDType& id) {
            return "<" + className + ": " + std::to_string(id.value) + ">";
        });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        This module provides a comprehensive Python interface for neural simulation,
        including brain modeling, sensory processing, motor control, and learning systems.
        
        Key features:
        - Pythonic API with type checking and validation
        - Comprehensive error handling with detailed error messages
        - Convenience factory methods for common use cases
        - Resource management with context managers
        - Extensive documentation and examples
        
        For detailed documentation, see the NLM project documentation.
    )pbdoc";

    // Register Python exception classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<ResourceError>(m, "ResourceError");

    // Bind generic ID classes to reduce code duplication
    bindGenericId<NeuronId>(m, "NeuronId");
    bindGenericId<SynapseId>(m, "SynapseId");
    bindGenericId<RegionId>(m, "RegionId");
    bindGenericId<PopulationId>(m, "PopulationId");

    // Enhanced NeuronType enum with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration.
        
        Defines the functional types of neurons in the NLM system.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that promotes activity in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that reduces activity in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that affects other neurons' responsiveness")
        .value("Sensory", NeuronType::Sensory, "Neuron that receives external sensory input")
        .value("Motor", NeuronType::Motor, "Neuron that generates motor output")
        .value("Internal", NeuronType::Internal, "Neuron that processes internal signals")
        .export_values();

    // Enhanced SynapseType enum with better documentation
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration.
        
        Defines the connection types between neurons.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Connection that increases postsynaptic activity")
        .value("Inhibitory", SynapseType::Inhibitory, "Connection that decreases postsynaptic activity")
        .value("Modulatory", SynapseType::Modulatory, "Connection that affects neuromodulation")
        .value("Electrical", SynapseType::Electrical, "Electrical gap junction connection")
        .value("GapJunction", SynapseType::GapJunction, "Chemical synaptic connection")
        .export_values();

    // Enhanced DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration.
        
        Defines the current developmental phase of the brain.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental state")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical learning period")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult developmental state")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase")
        .export_values();

    // Enhanced FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration.
        
        Defines the current firing state of a neuron.
    )pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is not firing")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    // Enhanced ActionType enum
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration for motor outputs.
        
        Defines the types of actions an agent can take.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward")
        .value("MoveLeft", ActionType::MoveLeft, "Move left")
        .value("MoveRight", ActionType::MoveRight, "Move right")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left")
        .value("TurnRight", ActionType::TurnRight, "Turn right")
        .value("Look", ActionType::Look, "Look (default direction)")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat food")
        .value("Drink", ActionType::Drink, "Drink water")
        .value("Rest", ActionType::Rest, "Rest/recharge")
        .value("Wait", ActionType::Wait, "Wait without action")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    // Enhanced MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration.
        
        Defines the basic motor commands for movement.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait without action")
        .export_values();

    // Enhanced WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration.
        
        Defines the types of objects in the world simulation.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource (food, water, etc.)")
        .value("Hazard", WorldObjectType::Hazard, "Hazard (dangerous object)")
        .value("Wall", WorldObjectType::Wall, "Wall/obstacle")
        .value("Marker", WorldObjectType::Marker, "Marker/target")
        .export_values();

    // Enhanced Config class with Python builder pattern and comprehensive validation
    py::class_<ConfigBuilder> (m, "ConfigBuilder", R"pbdoc(
        Configuration builder class for NLM system.
        
        Provides a Pythonic builder pattern for constructing configuration objects
        with method chaining and validation.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.ConfigBuilder() \
            ...     .set("brain.neuron_count", 1000) \
            ...     .set("world.width", 100) \
            ...     .set("world.height", 100) \
            ...     .set("development.stage", "Adult") \
            ...     .validate()
    )pbdoc")
        .def(py::init<>())
        .def("set", [](ConfigBuilder& self, const std::string& key, const std::string& value) {
            self.config_->set(key, value);
            return self;
        }, py::arg("key"), py::arg("value"), "Set configuration value (string).")
        .def("set", [](ConfigBuilder& self, const std::string& key, int value) {
            self.config_->set(key, value);
            return self;
        }, py::arg("key"), py::arg("value"), "Set configuration value (int).")
        .def("set", [](ConfigBuilder& self, const std::string& key, double value) {
            self.config_->set(key, value);
            return self;
        }, py::arg("key"), py::arg("value"), "Set configuration value (double).")
        .def("setBrainNeuronCount", [](ConfigBuilder& self, size_t count) {
            self.config_->set("brain.neuron_count", count);
            return self;
        }, py::arg("count"), "Set brain neuron count.")
        .def("setWorldSize", [](ConfigBuilder& self, size_t width, size_t height) {
            self.config_->set("world.width", width);
            self.config_->set("world.height", height);
            return self;
        }, py::arg("width"), py::arg("height"), "Set world dimensions.")
        .def("setVision", [](ConfigBuilder& self, size_t width, size_t height) {
            self.config_->set("world.vision_width", width);
            self.config_->set("world.vision_height", height);
            return self;
        }, py::arg("width"), py::arg("height"), "Set vision dimensions.")
        .def("setDevelopmentStage", [](ConfigBuilder& self, const std::string& stage) {
            self.config_->set("development.stage", stage);
            return self;
        }, py::arg("stage"), "Set developmental stage.")
        .def("enableStructuralPlasticity", [](ConfigBuilder& self, bool enable) {
            self.config_->set("plasticity.structural_enabled", enable);
            return self;
        }, py::arg("enable"), "Enable/disable structural plasticity.")
        .def("enableRewardModulation", [](ConfigBuilder& self, bool enable) {
            self.config_->set("neuromodulation.reward_enabled", enable);
            return self;
        }, py::arg("enable"), "Enable/disable reward modulation.")
        .def("enableDevelopment", [](ConfigBuilder& self, bool enable) {
            self.config_->set("neuromodulation.development_enabled", enable);
            return self;
        }, py::arg("enable"), "Enable/disable development.")
        .def("getConfig", &ConfigBuilder::getConfig, "Get the final Config object.")
        .def("validate", [](ConfigBuilder& self) {
            auto config = self.getConfig();
            config->validate();
            return self;
        }, "Validate configuration and return builder for chaining.")
        .def("__call__", &ConfigBuilder::getConfig, "Get the final Config object.")
        .def("__repr__", [](const ConfigBuilder& self) {
            return "<ConfigBuilder>";
        });

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Configuration object for the NLM neural simulation framework.
        Provides methods for loading, saving, and managing configuration settings.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.Config()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("world.width", 100)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Throws ValidationError if file is invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
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
        // Add convenience methods for common configuration tasks
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string).")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (int).")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (double).")
        .def("getString", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get configuration value as string.")
        .def("getInt", [](const Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"), "Get configuration value as integer.")
        .def("getDouble", [](const Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"), "Get configuration value as double.")
        .def("validate", &Config::validate,
             "Validate configuration settings. Throws ValidationError if invalid.");

    // Helper function to create ConfigBuilder
    m.def("createConfigBuilder", []() -> ConfigBuilder {
        return ConfigBuilder();
    }, "Create a new ConfigBuilder instance for Pythonic configuration building.")
        .def("createConfigBuilder", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        return config;
    }, "Create a basic configuration object.")
        .def("createConfigBuilder", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Create a configuration object from a file.")
        .def("createConfigBuilder", [](const std::string& filepath, const std::string& defaults) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        // Apply defaults
        config->set("brain.neuron_count", 1000);
        return config;
    }, py::arg("filepath"), py::arg("defaults"), "Create a configuration object from a file with defaults.")
        .def("createConfigBuilder", [](const std::string& jsonString) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // TODO: Implement from string parsing
        return config;
    }, py::arg("jsonString"), "Create a configuration object from a JSON string.");

    // Enhanced Vision class with validation and context manager support
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input class.
        
        Represents visual sensory input with width, height, and color channels.
        
        Args:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (default: 3 for RGB)
        
        Example:
            >>> import pynlm
            >>> vision = pynlm.Vision(640, 480, 3)
            >>> vision.setData([255, 0, 0] * 640 * 480)  # Red image
            >>> with vision:
            ...     # Vision resource is available
            ...     pass
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValidationError("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Validates input.")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("validate", [](const Vision& self) {
            if (self.getWidth() == 0 || self.getHeight() == 0) {
                throw ValidationError("Vision dimensions must be positive");
            }
            if (self.getChannels() == 0) {
                throw ValidationError("Vision channels must be positive");
            }
        }, "Validate vision parameters. Throws ValidationError if invalid.")
        // Context manager support
        .def("__enter__", [](Vision& self) -> Vision& {
            return self;
        }, "Enter context manager, returns self.")
        .def("__exit__", [](Vision& self, py::object exc_type, py::object exc_val, py::object exc_tb) {
            // Clean up vision resources if any
            if (exc_type) {
                throw;
            }
        }, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"), "Exit context manager, cleanup resources.");

    // Enhanced Audio class with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input class.
        
        Represents audio sensory input with sample rate and samples.
        
        Args:
            sampleRate: Audio sample rate in Hz
            numSamples: Number of audio samples
        
        Example:
            >>> import pynlm
            >>> audio = pynlm.Audio(44100, 1024)
            >>> audio.setData([0.1, -0.2, 0.3] * 341)  # Audio samples
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValidationError("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data. Validates input.")
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            self.setSampleRate(validatePositive(sampleRate, "sampleRate"));
        }, py::arg("sampleRate"), "Set sample rate. Validates input.")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("validate", [](const Audio& self) {
            if (self.getSampleRate() == 0) {
                throw ValidationError("Sample rate must be positive");
            }
            if (self.getNumSamples() == 0) {
                throw ValidationError("Number of samples must be positive");
            }
        }, "Validate audio parameters. Throws ValidationError if invalid.");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input class.
        
        Represents internal neural signals and processing.
        
        Example:
            >>> import pynlm
            >>> signals = pynlm.InternalSignals()
            >>> signals.addSignal(0.5)
            >>> signals.addSignal(-0.3)
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, double value) {
            self.addSignal(value);
        }, py::arg("value"), "Add an internal signal value.")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals.")
        .def("__enter__", [](InternalSignals& self) -> InternalSignals& {
            return self;
        }, "Enter context manager, returns self.")
        .def("__exit__", [](InternalSignals& self, py::object exc_type, py::object exc_val, py::object exc_tb) {
            if (exc_type) {
                throw;
            }
        }, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"), "Exit context manager.");

    // Enhanced Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Represents an action that can be taken by an agent.
        
        Args:
            action_type: Type of action to perform
            parameters: Optional parameters for the action
        
        Example:
            >>> import pynlm
            >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
            >>> action.setType(pynlm.ActionType.TurnLeft)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type.")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type.")
        .def("getParameters", &Action::getParameters, "Get the action parameters.")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters.")
        .def("getName", &Action::getName, "Get the action name.")
        .def("clone", &Action::clone, "Create a copy of this action.")
        .def("validate", [](const Action& self) {
            if (self.getType() == ActionType::Custom && self.getParameters().empty()) {
                throw ValidationError("Custom actions must have parameters");
            }
        }, "Validate action parameters. Throws ValidationError if invalid.");

    // Enhanced WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Represents an object in the world simulation.
        
        Args:
            x: X coordinate
            y: Y coordinate
            type: Object type
            value: Object value (default: 0.0)
            radius: Object radius (default: 0.5)
        
        Example:
            >>> import pynlm
            >>> obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 100.0)
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
        .def_readwrite("active", &WorldObject::active)
        .def("validate", [](const WorldObject& self) {
            if (self.radius < 0) {
                throw ValidationError("Object radius must be non-negative");
            }
        }, "Validate object parameters. Throws ValidationError if invalid.");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state class.
        
        Represents the physical state of an agent in the world.
        
        Example:
            >>> import pynlm
            >>> body = pynlm.AgentBody()
            >>> body.x = 50.0
            >>> body.y = 30.0
            >>> body.orientation = 90.0  # degrees
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
        .def("reset", &AgentBody::reset, "Reset agent body state to defaults.")
        .def("validate", [](const AgentBody& self) {
            if (self.health < 0 || self.health > 100) {
                throw ValidationError("Health must be between 0 and 100");
            }
            if (self.energy < 0 || self.energy > 100) {
                throw ValidationError("Energy must be between 0 and 100");
            }
        }, "Validate agent body parameters. Throws ValidationError if invalid.");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world class.
        
        Represents the result of an action taken by an agent.
        
        Args:
            reward: Reward value
            success: Whether action was successful
            message: Optional message
        
        Example:
            >>> import pynlm
            >>> result = pynlm.ActionResult(10.0, True, "Goal reached!")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("validate", [](const ActionResult& self) {
            if (self.reward < -1000 || self.reward > 1000) {
                throw ValidationError("Reward value out of reasonable range");
            }
        }, "Validate action result parameters. Throws ValidationError if invalid.");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data class.
        
        Represents sensory percepts from an agent.
        
        Example:
            >>> import pynlm
            >>> percept = pynlm.SensoryPercept()
            >>> vision = pynlm.Vision(80, 60)
            >>> percept.setVision(vision)
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision percept.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch percept.")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal percept.")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception percept.")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio percept.")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp for percept.")
        .def("validate", [](const SensoryPercept& self) {
            // Add validation logic if needed
        }, "Validate sensory percept. Throws ValidationError if invalid.");

    // Enhanced SimpleWorld class with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Represents a 2D world where agents exist and interact.
        
        Args:
            width: World width
            height: World height
            visionWidth: Agent vision width
            visionHeight: Agent vision height
        
        Example:
            >>> import pynlm
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(100, 100, 80, 60)
    )pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                           size_t visionWidth, size_t visionHeight) {
            width = validatePositive(width, "width");
            height = validatePositive(height, "height");
            visionWidth = validatePositive(visionWidth, "visionWidth");
            visionHeight = validatePositive(visionHeight, "visionHeight");
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
            "Configure world with validation. Throws ValidationError if parameters invalid.")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.")
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (x < 0 || x > self.getWidth() || y < 0 || y > self.getHeight()) {
                throw ValidationError("Agent start position out of bounds");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent start position. Validates bounds.")
        .def("update", [](SimpleWorld& self, float timestep) {
            if (timestep <= 0) {
                throw ValidationError("Timestep must be positive");
            }
            self.update(timestep);
        }, py::arg("timestep"), "Update world with timestep. Validates input.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::copy,
             "Get current sensory percept.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy,
             "Get current agent body state.")
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            obj.validate(); // Validate before adding
            self.addObject(obj);
        }, py::arg("obj"), "Add world object with validation.")
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            if (x < 0 || x > self.getWidth() || y < 0 || y > self.getHeight()) {
                throw ValidationError("Object position out of bounds");
            }
            self.removeObject(x, y);
        }, py::arg("x"), py::arg("y"), "Remove object at position. Validates bounds.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world.")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            if (e <= 0) {
                throw ValidationError("Max energy must be positive");
            }
            self.setMaxEnergy(e);
        }, py::arg("e"), "Set max energy. Validates input.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            if (r < 0 || r > 1) {
                throw ValidationError("Energy decay rate must be between 0 and 1");
            }
            self.setEnergyDecayRate(r);
        }, py::arg("r"), "Set energy decay rate. Validates input.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("validate", [](const SimpleWorld& self) {
            if (self.getWidth() == 0 || self.getHeight() == 0) {
                throw ValidationError("World dimensions must be positive");
            }
        }, "Validate world parameters. Throws ValidationError if invalid.")
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) -> SimpleWorld& {
            return self;
        }, "Enter context manager, returns self.")
        .def("__exit__", [](SimpleWorld& self, py::object exc_type, py::object exc_val, py::object exc_tb) {
            // Clean up world resources if any
            if (exc_type) {
                throw;
            }
        }, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"), "Exit context manager, cleanup resources.");

    // Enhanced Brain class with validation and context manager support
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The central neural simulation brain with neurons, synapses, and learning.
        
        Args:
            config: Configuration object
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize brain with configuration.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.")
        .def("reset", &Brain::reset,
             "Reset brain state.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::copy,
             "Get a region by ID.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::copy,
             "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", [](Brain& self, DevelopmentalStage stage) {
            if (stage == DevelopmentalStage::Initial) {
                throw ValidationError("Cannot set developmental stage to Initial after initialization");
            }
            self.setDevelopmentalStage(stage);
        }, py::arg("stage"), "Set developmental stage. Validates input.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::copy,
             "Get the configuration.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status.")
        .def("validate", [](const Brain& self) {
            if (self.getTotalNeuronCount() == 0) {
                throw ValidationError("Brain must have at least one neuron");
            }
        }, "Validate brain state. Throws ValidationError if invalid.")
        // Context manager support
        .def("__enter__", [](Brain& self) -> Brain& {
            return self;
        }, "Enter context manager, returns self.")
        .def("__exit__", [](Brain& self, py::object exc_type, py::object exc_val, py::object exc_tb) {
            // Clean up brain resources if any
            if (exc_type) {
                throw;
            }
        }, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"), "Exit context manager, cleanup resources.");

    // Enhanced AgentBrain class with validation and context manager support
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Interface that connects the NLM brain to the simulation world.
        
        Args:
            brain: The brain to control
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::copy,
             "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Context manager support
        .def("__enter__", [](AgentBrain& self) -> AgentBrain& {
            return self;
        }, "Enter context manager, returns self.")
        .def("__exit__", [](AgentBrain& self, py::object exc_type, py::object exc_val, py::object exc_tb) {
            // Clean up agent brain resources if any
            if (exc_type) {
                throw;
            }
        }, py::arg("exc_type"), py::arg("exc_val"), py::arg("exc_tb"), "Exit context manager, cleanup resources.");

    // Enhanced MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration.
        
        Defines the basic motor commands for movement.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait without action")
        .export_values();

    // Enhanced WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration.
        
        Defines the types of objects in the world simulation.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource (food, water, etc.)")
        .value("Hazard", WorldObjectType::Hazard, "Hazard (dangerous object)")
        .value("Wall", WorldObjectType::Wall, "Wall/obstacle")
        .value("Marker", WorldObjectType::Marker, "Marker/target")
        .export_values();

    // Enhanced Config class with validation and Pythonic interface
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Configuration object for the NLM neural simulation framework.
        Provides methods for loading, saving, and managing configuration settings.
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.neuron_count", 1000)
            >>> config.set("world.width", 100)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Throws ValidationError if file is invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
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
        // Add convenience methods for common configuration tasks
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (string).")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (int).")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set configuration value (double).")
        .def("getString", [](const Config& self, const std::string& key) {
            return self.get<std::string>(key);
        }, py::arg("key"), "Get configuration value as string.")
        .def("getInt", [](const Config& self, const std::string& key) {
            return self.get<int>(key);
        }, py::arg("key"), "Get configuration value as integer.")
        .def("getDouble", [](const Config& self, const std::string& key) {
            return self.get<double>(key);
        }, py::arg("key"), "Get configuration value as double.")
        .def("validate", &Config::validate,
             "Validate configuration settings. Throws ValidationError if invalid.");

    // Enhanced Vision class with validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input class.
        
        Represents visual sensory input with width, height, and color channels.
        
        Args:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (default: 3 for RGB)
        
        Example:
            >>> import pynlm
            >>> vision = pynlm.Vision(640, 480, 3)
            >>> vision.setData([255, 0, 0] * 640 * 480)  # Red image
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValidationError("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Validates input.")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("validate", [](const Vision& self) {
            if (self.getWidth() == 0 || self.getHeight() == 0) {
                throw ValidationError("Vision dimensions must be positive");
            }
            if (self.getChannels() == 0) {
                throw ValidationError("Vision channels must be positive");
            }
        }, "Validate vision parameters. Throws ValidationError if invalid.");

    // Enhanced Audio class with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input class.
        
        Represents audio sensory input with sample rate and samples.
        
        Args:
            sampleRate: Audio sample rate in Hz
            numSamples: Number of audio samples
        
        Example:
            >>> import pynlm
            >>> audio = pynlm.Audio(44100, 1024)
            >>> audio.setData([0.1, -0.2, 0.3] * 341)  # Audio samples
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValidationError("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data. Validates input.")
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            self.setSampleRate(validatePositive(sampleRate, "sampleRate"));
        }, py::arg("sampleRate"), "Set sample rate. Validates input.")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("validate", [](const Audio& self) {
            if (self.getSampleRate() == 0) {
                throw ValidationError("Sample rate must be positive");
            }
            if (self.getNumSamples() == 0) {
                throw ValidationError("Number of samples must be positive");
            }
        }, "Validate audio parameters. Throws ValidationError if invalid.");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input class.
        
        Represents internal neural signals and processing.
        
        Example:
            >>> import pynlm
            >>> signals = pynlm.InternalSignals()
            >>> signals.addSignal(0.5)
            >>> signals.addSignal(-0.3)
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, double value) {
            self.addSignal(value);
        }, py::arg("value"), "Add an internal signal value.")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals.");

    // Enhanced Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Represents an action that can be taken by an agent.
        
        Args:
            action_type: Type of action to perform
            parameters: Optional parameters for the action
        
        Example:
            >>> import pynlm
            >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
            >>> action.setType(pynlm.ActionType.TurnLeft)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type.")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type.")
        .def("getParameters", &Action::getParameters, "Get the action parameters.")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters.")
        .def("getName", &Action::getName, "Get the action name.")
        .def("clone", &Action::clone, "Create a copy of this action.")
        .def("validate", [](const Action& self) {
            if (self.getType() == ActionType::Custom && self.getParameters().empty()) {
                throw ValidationError("Custom actions must have parameters");
            }
        }, "Validate action parameters. Throws ValidationError if invalid.");

    // Enhanced WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation.
        
        Represents an object in the world simulation.
        
        Args:
            x: X coordinate
            y: Y coordinate
            type: Object type
            value: Object value (default: 0.0)
            radius: Object radius (default: 0.5)
        
        Example:
            >>> import pynlm
            >>> obj = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Resource, 100.0)
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
        .def_readwrite("active", &WorldObject::active)
        .def("validate", [](const WorldObject& self) {
            if (self.radius < 0) {
                throw ValidationError("Object radius must be non-negative");
            }
        }, "Validate object parameters. Throws ValidationError if invalid.");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state class.
        
        Represents the physical state of an agent in the world.
        
        Example:
            >>> import pynlm
            >>> body = pynlm.AgentBody()
            >>> body.x = 50.0
            >>> body.y = 30.0
            >>> body.orientation = 90.0  # degrees
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
        .def("reset", &AgentBody::reset, "Reset agent body state to defaults.")
        .def("validate", [](const AgentBody& self) {
            if (self.health < 0 || self.health > 100) {
                throw ValidationError("Health must be between 0 and 100");
            }
            if (self.energy < 0 || self.energy > 100) {
                throw ValidationError("Energy must be between 0 and 100");
            }
        }, "Validate agent body parameters. Throws ValidationError if invalid.");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world class.
        
        Represents the result of an action taken by an agent.
        
        Args:
            reward: Reward value
            success: Whether action was successful
            message: Optional message
        
        Example:
            >>> import pynlm
            >>> result = pynlm.ActionResult(10.0, True, "Goal reached!")
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("validate", [](const ActionResult& self) {
            if (self.reward < -1000 || self.reward > 1000) {
                throw ValidationError("Reward value out of reasonable range");
            }
        }, "Validate action result parameters. Throws ValidationError if invalid.");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data class.
        
        Represents sensory percepts from an agent.
        
        Example:
            >>> import pynlm
            >>> percept = pynlm.SensoryPercept()
            >>> vision = pynlm.Vision(80, 60)
            >>> percept.setVision(vision)
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision percept.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch percept.")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal percept.")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception percept.")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio percept.")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp for percept.")
        .def("validate", [](const SensoryPercept& self) {
            // Add validation logic if needed
        }, "Validate sensory percept. Throws ValidationError if invalid.");

    // Enhanced SimpleWorld class with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Represents a 2D world where agents exist and interact.
        
        Args:
            width: World width
            height: World height
            visionWidth: Agent vision width
            visionHeight: Agent vision height
        
        Example:
            >>> import pynlm
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(100, 100, 80, 60)
    )pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                           size_t visionWidth, size_t visionHeight) {
            width = validatePositive(width, "width");
            height = validatePositive(height, "height");
            visionWidth = validatePositive(visionWidth, "visionWidth");
            visionHeight = validatePositive(visionHeight, "visionHeight");
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
            "Configure world with validation. Throws ValidationError if parameters invalid.")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.")
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (x < 0 || x > self.getWidth() || y < 0 || y > self.getHeight()) {
                throw ValidationError("Agent start position out of bounds");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent start position. Validates bounds.")
        .def("update", [](SimpleWorld& self, float timestep) {
            if (timestep <= 0) {
                throw ValidationError("Timestep must be positive");
            }
            self.update(timestep);
        }, py::arg("timestep"), "Update world with timestep. Validates input.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::copy,
             "Get current sensory percept.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::copy,
             "Get current agent body state.")
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            obj.validate(); // Validate before adding
            self.addObject(obj);
        }, py::arg("obj"), "Add world object with validation.")
        .def("removeObject", [](SimpleWorld& self, float x, float y) {
            if (x < 0 || x > self.getWidth() || y < 0 || y > self.getHeight()) {
                throw ValidationError("Object position out of bounds");
            }
            self.removeObject(x, y);
        }, py::arg("x"), py::arg("y"), "Remove object at position. Validates bounds.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world.")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", [](SimpleWorld& self, float e) {
            if (e <= 0) {
                throw ValidationError("Max energy must be positive");
            }
            self.setMaxEnergy(e);
        }, py::arg("e"), "Set max energy. Validates input.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", [](SimpleWorld& self, float r) {
            if (r < 0 || r > 1) {
                throw ValidationError("Energy decay rate must be between 0 and 1");
            }
            self.setEnergyDecayRate(r);
        }, py::arg("r"), "Set energy decay rate. Validates input.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("validate", [](const SimpleWorld& self) {
            if (self.getWidth() == 0 || self.getHeight() == 0) {
                throw ValidationError("World dimensions must be positive");
            }
        }, "Validate world parameters. Throws ValidationError if invalid.");

    // Enhanced Brain class with validation
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The central neural simulation brain with neurons, synapses, and learning.
        
        Args:
            config: Configuration object
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.")
        .def("reset", &Brain::reset,
             "Reset brain state.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::copy,
             "Get a region by ID.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::copy,
             "Get all regions.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("setDevelopmentalStage", [](Brain& self, DevelopmentalStage stage) {
            if (stage == DevelopmentalStage::Initial) {
                throw ValidationError("Cannot set developmental stage to Initial after initialization");
            }
            self.setDevelopmentalStage(stage);
        }, py::arg("stage"), "Set developmental stage. Validates input.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::copy,
             "Get the configuration.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status.")
        .def("validate", [](const Brain& self) {
            if (self.getTotalNeuronCount() == 0) {
                throw ValidationError("Brain must have at least one neuron");
            }
        }, "Validate brain state. Throws ValidationError if invalid.");

    // Enhanced AgentBrain class with validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Interface that connects the NLM brain to the simulation world.
        
        Args:
            brain: The brain to control
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> world = pynlm.createSimpleWorld()
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::copy,
             "Get the underlying brain.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        return config;
    }, "Create a default configuration. Returns a shared_ptr.")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw ValidationError("Configuration cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("config"), "Create a new brain with configuration. Returns a shared_ptr.")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        return world;
    }, "Create a new simple world. Returns a shared_ptr.")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw ValidationError("Brain cannot be null");
        }
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, py::arg("brain"), "Create a new agent brain interface. Returns a shared_ptr.")

    // Module-level constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add Python-specific convenience functions
    m.def("validate_positive", [](double value, const std::string& name) {
        return validatePositive(value, name);
    }, py::arg("value"), py::arg("name"), "Validate that a value is positive.");

    m.def("validate_range", [](double value, double min, double max, const std::string& name) {
        return validateRange(value, min, max, name);
    }, py::arg("value"), py::arg("min"), py::arg("max"), py::arg("name"), "Validate that a value is within a range.");

    // Create Pythonic convenience functions
    m.def("create_smart_config", []() {
        auto config = std::make_shared<Config>();
        // Set sensible defaults
        config->set("brain.neuron_count", 1000);
        config->set("world.width", 100);
        config->set("world.height", 100);
        config->set("world.vision_width", 80);
        config->set("world.vision_height", 60);
        config->set("development.stage", "Adult");
        return config;
    }, "Create a smart configuration with sensible defaults for quick testing.");

    m.def("create_agent_brain_smart", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 500);
        auto brain = std::make_shared<Brain>(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        return agent;
    }, "Create a complete agent brain with smart configuration for quick testing.");

    m.def("run_brain_episode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                std::shared_ptr<AgentBrain> agent, size_t steps) {
        if (!brain || !world || !agent) {
            throw ValidationError("Brain, world, and agent cannot be null");
        }
        if (steps == 0) {
            throw ValidationError("Number of steps must be positive");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(0.1f);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain->getFiringNeuronCount(), brain->getAverageFiringRate(),
                               agent->getNeuromodulationLevel());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        "Run a complete brain episode. Returns (firing_neurons, firing_rate, neuromodulation_level).");

    // Constants for common values
    m.attr("DEFAULT_BRAIN_STEPS_PER_UPDATE") = py::int_(10);
    m.attr("DEFAULT_WORLD_WIDTH") = py::int_(100);
    m.attr("DEFAULT_WORLD_HEIGHT") = py::int_(100);
    m.attr("DEFAULT_VISION_WIDTH") = py::int_(80);
    m.attr("DEFAULT_VISION_HEIGHT") = py::int_(60);
}

} // namespace nlm