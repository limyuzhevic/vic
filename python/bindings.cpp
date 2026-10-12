#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>

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

// Python-style custom exceptions
class NLMException : public std::exception {
public:
    explicit NLMException(const std::string& message) : message_(message) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    std::string message() const {
        return message_;
    }
    
private:
    std::string message_;
};

class NLMConfigError : public NLMException {
public:
    explicit NLMConfigError(const std::string& message) : NLMException(message) {}
};

class NLMBrainError : public NLMException {
public:
    explicit NLMBrainError(const std::string& message) : NLMException(message) {}
};

class NLMWorldError : public NLMException {
public:
    explicit NLMWorldError(const std::string& message) : NLMException(message) {}
};

class NLMSimulationError : public NLMException {
public:
    explicit NLMSimulationError(const std::string& message) : NLMException(message) {}
};

class NLMMemoryError : public NLMException {
public:
    explicit NLMMemoryError(const std::string& message) : NLMException(message) {}
};

// Progress tracking callback system
class ProgressTracker {
public:
    using ProgressCallback = std::function<void(float progress, const std::string& message)>;
    
    ProgressTracker() : progress_(0.0f), max_steps_(1), current_step_(0) {}
    
    void setProgress(float progress) {
        progress_ = std::min(1.0f, std::max(0.0f, progress));
        if (callback_) {
            callback_(progress_, std::string("Step ") + std::to_string(current_step_));
        }
    }
    
    void incrementStep(const std::string& message = "") {
        current_step_++;
        if (max_steps_ > 0) {
            setProgress(static_cast<float>(current_step_) / max_steps_);
        }
        if (callback_) {
            callback_(progress_, message.empty() ? std::string("Completed step ") + std::to_string(current_step_) : message);
        }
    }
    
    void setCallback(ProgressCallback cb) {
        callback_ = cb;
        if (callback_) {
            callback_(progress_, "Progress tracker initialized");
        }
    }
    
    void setMaxSteps(int max) {
        max_steps_ = max;
    }
    
    float getProgress() const { return progress_; }
    int getCurrentStep() const { return current_step_; }
    int getMaxSteps() const { return max_steps_; }
    
    void runWithProgress(std::function<void()> task) {
        if (max_steps_ <= 0) {
            task();
            return;
        }
        
        std::atomic<int> completed{0};
        auto worker = [&]() {
            task();
            completed = max_steps_;
            setProgress(1.0f);
        };
        
        std::thread thread(worker);
        while (completed < max_steps_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        thread.join();
    }
    
private:
    float progress_;
    int max_steps_;
    int current_step_;
    ProgressCallback callback_;
    std::atomic<int> completed{0};
};

// Version constants
const std::string NLM_VERSION = "0.1.0";
const std::string NLM_VERSION_NAME = "NLM Python Bindings";

// Type definitions for better Python integration
using float_array = std::vector<float>;
using str_array = std::vector<std::string>;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action along with utilities for simulation management,
        progress tracking, and experiment control.
        
        Example usage:
        
        .. code-block:: python
        
            import pynlm
            
            # Create a default configuration
            config = pynlm.Config()
            config.set("learning_rate", 0.01)
            config.set("num_neurons", 1000)
            
            # Create brain and world
            brain = pynlm.Brain(config)
            world = pynlm.SimpleWorld()
            
            # Initialize with progress tracking
            def progress_callback(progress, message):
                print(f"Progress: {progress:.1%}: {message}")
            
            tracker = pynlm.ProgressTracker()
            tracker.setCallback(progress_callback)
            tracker.setMaxSteps(100)
            
            brain.initialize()
            
            # Run simulation
            for step in range(100):
                tracker.incrementStep(f"Step {step}")
                brain.step(step)
            
            # Save results
            brain.save("brain_checkpoint.nml")
        
        See the documentation for detailed API reference.
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](int exc_type, PyObject* exc_value, PyObject* exc_traceback) {
        if (PyErr_GivenExceptionMatches(exc_value, PyExc_RuntimeError)) {
            std::string message = PyUnicode_AsUTF8(PyObject_Str(exc_value));
            // Convert runtime errors to NLM exceptions
            PyErr_SetString(PyExc_RuntimeError, ("NLM Runtime Error: " + message).c_str());
        }
    });

    // Register Python-style exceptions
    py::register_exception<NLMException, std::runtime_error>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<NLMConfigError, NLMException>(m, "NLMConfigError");
    py::register_exception<NLMBrainError, NLMException>(m, "NLMBrainError");
    py::register_exception<NLMWorldError, NLMException>(m, "NLMWorldError");
    py::register_exception<NLMSimulationError, NLMException>(m, "NLMSimulationError");
    py::register_exception<NLMMemoryError, NLMException>(m, "NLMMemoryError");

    // Register version attributes
    m.attr("__version__") = NLM_VERSION;
    m.attr("__version_name__") = NLM_VERSION_NAME;
    m.attr("__author__") = "NLM Authors";
    m.attr("__license__") = "MIT";

    // ===== TYPE DEFINITIONS =====
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
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

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ===== CONFIGURATION MODULE =====
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system.
        
        The Config class manages key-value pairs for simulation parameters.
        It supports various data types including int, float, bool, string,
        and arrays of these types.
        
        Example:
        
        .. code-block:: python
        
            config = pynlm.Config()
            config.set("learning_rate", 0.01)
            config.set("num_neurons", 1000)
            config.set("population_size", [10, 20, 30])
            
            # Get values with defaults
            lr = config.getOr("learning_rate", 0.001)
            neurons = config.getOr("num_neurons", 100)
            
        The configuration can be saved to and loaded from JSON files.
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises NLMConfigError on failure.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Example:
             
             .. code-block:: python
             
                 config.loadFromFile("config.json")
                 
             Raises:
                 NLMConfigError: If the file cannot be read or is invalid JSON
             ")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments.
           
           Args:
               argc: Number of arguments
               argv: Argument array (typically sys.argv)
               
           Example:
           
           .. code-block:: python
           
               import sys
               config.loadFromArgs(len(sys.argv), sys.argv)
           ")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises NLMConfigError on failure.
             
             Args:
                 filepath: Path where the configuration should be saved
                 
             Example:
             
             .. code-block:: python
             
                 config.saveToFile("config.json")
                 
             Raises:
                 NLMConfigError: If the file cannot be written
             ")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 bool: True if the key exists, False otherwise
                 
             Example:
             
             .. code-block:: python
             
                 if config.has("learning_rate"):
                     rate = config.get("learning_rate")
             ")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys.
             
             Returns:
                 list: List of all configuration key strings
                 
             Example:
             
             .. code-block:: python
             
                 keys = config.getKeys()
                 for key in keys:
                     print(f"{key}: {config.get(key)}")
             ")
        .def("clear", &Config::clear,
             "Clear all configuration entries. Removes all key-value pairs.
             
             Example:
             
             .. code-block:: python
             
                 config.clear()
             ")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration.
             
             Returns:
                 str: Human-readable summary of all configuration entries
                 
             Example:
             
             .. code-block:: python
             
                 print(config.summary())
             ")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ===== SENSORY INPUT MODULE =====
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input.
        
        SensoryInput represents the raw input from external or internal sensors
        that the brain uses to interpret the environment.
        
        Subclasses include Vision, Audio, and InternalSignals.
        
        Example:
        
        .. code-block:: python
        
            # Create a vision sensor
            vision = pynlm.Vision(width=128, height=128, channels=3)
            
            # Set vision data
            data = [0.1] * 128 * 128 * 3
            vision.setData(data)
            
            # Get dimensions
            width = vision.getWidth()
            height = vision.getHeight()
            
        Note:
            The actual data format depends on the specific sensor type.
        )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input.
            
            Returns:
                str: Sensor type name (e.g., "Vision", "Audio", "InternalSignals")
        ")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector.
            
            Returns:
                list: Raw sensor data as a list of floats
        ")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality.
            
            Returns:
                int: Number of data dimensions
        ")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp.
            
            Returns:
                int: Simulation timestamp
        ")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp.
             
             Args:
                 timestamp: Simulation timestamp value
        ");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input.
        
        Vision represents visual input from the environment, typically as
        pixel data. It can be used to simulate different visual conditions.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels
            channels: Number of color channels (1=grayscale, 3=RGB)
        
        Example:
        
        .. code-block:: python
        
            # Create 64x64 grayscale image
            vision = pynlm.Vision(64, 64, 1)
            
            # Create 128x128 RGB image
            vision_rgb = pynlm.Vision(128, 128, 3)
            
            # Generate a simple pattern
            data = []
            for y in range(128):
                for x in range(128):
                    # Create a gradient pattern
                    intensity = (x + y) / 256.0
                    data.extend([intensity] * 3)  # RGB
            vision_rgb.setData(data)
        
        See Also:
            SensoryInput: Base class for all sensory inputs
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision pixel data.
            
            Args:
                data: List of pixel values, ordered as [R,G,B,R,G,B,...] or grayscale [v1,v2,...]
                
            Example:
            
            .. code-block:: python
            
                data = [1.0, 0.5, 0.0] * 100  # Red, green, blue pattern
                vision.setData(data)
            ")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels.
            
            Returns:
                int: Image width
        ")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels.
            
            Returns:
                int: Image height
        ")
        .def("getChannels", &Vision::getChannels, "Get number of color channels.
            
            Returns:
                int: Number of channels (1 for grayscale, 3 for RGB)
        ");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input.
        
        Audio represents audio input, typically as a time series of samples.
        This can simulate different sound environments or input patterns.
        
        Attributes:
            sampleRate: Samples per second
            numSamples: Number of audio samples
        
        Example:
        
        .. code-block:: python
        
            # Create 44.1kHz audio with 1-second duration
            audio = pynlm.Audio(44100, 44100)
            
            # Generate a sine wave tone
            import math
            data = []
            for i in range(44100):
                t = i / 44100.0
                data.append(math.sin(2 * math.pi * 440 * t))  # 440Hz tone
            audio.setData(data)
        
        See Also:
            SensoryInput: Base class for all sensory inputs
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio sample data.
            
            Args:
                data: List of audio samples, typically -1.0 to 1.0 range
                
            Example:
            
            .. code-block:: python
            
                # White noise
                import random
                data = [random.uniform(-1, 1) for _ in range(44100)]
                audio.setData(data)
            ")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate in Hz.
             
             Args:
                 sampleRate: Samples per second (e.g., 44100 for CD quality)
            ")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate in Hz.
            
            Returns:
                int: Sample rate in Hz
            ")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples.
            
            Returns:
                int: Number of samples in the audio buffer
            ");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input.
        
        InternalSignals represents internal brain states such as hunger,
        thirst, energy levels, or other physiological signals that affect
        behavior and decision-making.
        
        Example:
        
        .. code-block:: python
        
            # Create internal signals
            internal = pynlm.InternalSignals()
            
            # Add various signals
            internal.addSignal(0.8)  # High energy
            internal.addSignal(0.3)  # Moderate hunger
            internal.addSignal(0.1)  # Low stress
            
            # Clear all signals
            internal.clearSignals()
        
        See Also:
            SensoryInput: Base class for all sensory inputs
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals buffer.
             
             Args:
                 value: Signal strength (typically 0.0 to 1.0)
            ")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals.
            
            Removes all stored signal values from the buffer.
            
            Example:
            
            .. code-block:: python
            
                internal.clearSignals()  # Reset all internal states
            ");

    // ===== MOTOR MODULE =====
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output.
        
        Action represents a motor command or behavioral output from the brain
        to the environment. Actions can have different types and parameter values.
        
        Attributes:
            type: Action type (e.g., MoveForward, Look, Eat)
            parameters: List of action-specific parameters (e.g., angle, distance)
            name: Human-readable action name
        
        Example:
        
        .. code-block:: python
        
            # Create a simple move action
            action = pynlm.Action(pynlm.ActionType.MoveForward)
            
            # Create an action with parameters
            action = pynlm.Action(
                pynlm.ActionType.Look,
                [45.0]  # Look 45 degrees to the right
            )
            
            # Get action details
            print(f"Action type: {action.getType()}")
            print(f"Action name: {action.getName()}")
            
            # Clone an action
            action_copy = action.clone()
        
        See Also:
            ActionType: Enumeration of possible action types
            MotorCommand: Low-level motor commands
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type.
            
            Returns:
                ActionType: The action type enum value
        ")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type.
             
             Args:
                 type: ActionType enum value
        ")
        .def("getParameters", &Action::getParameters, "Get action parameters.
            
            Returns:
                list: List of parameter values (typically floats)
        ")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters.
             
             Args:
                 params: List of parameter values
        ")
        .def("getName", &Action::getName, "Get human-readable action name.
            
            Returns:
                str: Descriptive name of the action
        ")
        .def("clone", &Action::clone, "Create a copy of this action.
            
            Returns:
                Action: A new Action object with the same type and parameters
        ");

    // ===== WORLD MODULE =====
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation.
        
        WorldObject represents an entity or feature in the simulation world.
        Objects can be resources, hazards, walls, markers, or empty spaces.
        
        Attributes:
            x: X position in the world (0.0 to 1.0)
            y: Y position in the world (0.0 to 1.0)
            type: Object type (Empty, Resource, Hazard, Wall, Marker)
            value: Object-specific value (e.g., resource quantity, hazard danger level)
            radius: Object influence radius
            active: Whether the object is currently active
        
        Example:
        
        .. code-block:: python
        
            # Create a resource object
            resource = pynlm.WorldObject(0.5, 0.5, pynlm.WorldObjectType.Resource, 100.0)
            
            # Create a hazard zone
            hazard = pynlm.WorldObject(0.8, 0.8, pynlm.WorldObjectType.Hazard, 50.0, radius=0.2)
            
            # Add to world
            world.addObject(resource)
            
            # Check if position is valid
            is_valid = world.isValidPosition(0.3, 0.3)
        
        See Also:
            SimpleWorld: The 2D simulation world
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X position in the world")
        .def_readwrite("y", &WorldObject::y, "Y position in the world")
        .def_readwrite("radius", &WorldObject::radius, "Object influence radius")
        .def_readwrite("type", &WorldObject::type, "Object type (WorldObjectType enum)")
        .def_readwrite("value", &WorldObject::value, "Object-specific value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state.
        
        AgentBody represents the physical state of an agent in the world,
        including position, orientation, velocity, energy, and other physiological
        attributes that affect behavior.
        
        Attributes:
            x: X position in the world
            y: Y position in the world
            orientation: Heading direction in radians (0 to 2π)
            velocityX: X velocity component
            velocityY: Y velocity component
            angularVelocity: Rotational velocity
            energy: Current energy level (0.0 to 1.0)
            health: Health status (0.0 to 1.0)
            age: Age in simulation steps
            isMoving: Whether the agent is currently moving
            isTurning: Whether the agent is currently turning
            lastActionTime: Time since last action was taken
        
        Example:
        
        .. code-block:: python
        
            # Create an agent body
            body = pynlm.AgentBody()
            body.x = 0.5
            body.y = 0.5
            body.orientation = 0.0  # Facing right
            
            # Update movement
            body.velocityX = 0.1
            body.isMoving = True
            
            # Check status
            energy = body.energy
            if energy < 0.2:
                # Need to rest
                body.setType(pynlm.ActionType.Rest)
        
        See Also:
            SimpleWorld: World simulation environment
            Action: Motor commands
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position in the world")
        .def_readwrite("y", &AgentBody::y, "Y position in the world")
        .def_readwrite("orientation", &AgentBody::orientation, "Heading direction in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Rotational velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0.0 to 1.0)")
        .def_readwrite("health", &AgentBody::health, "Health status (0.0 to 1.0)")
        .def_readwrite("age", &AgentBody::age, "Age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state.
            
            Example:
            
            .. code-block:: python
            
                body.reset()  # Reset to fresh state
            ");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world interaction.
        
        ActionResult represents the outcome of an action taken by an agent,
        including reward, success status, and descriptive message.
        
        Attributes:
            reward: Numerical reward value (positive for good outcomes)
            success: Whether the action succeeded
            message: Descriptive message about the outcome
        
        Example:
        
        .. code-block:: python
        
            # Create a successful action result
            result = pynlm.ActionResult(10.0, True, "Found food")
            
            # Handle unsuccessful action
            failure = pynlm.ActionResult(-5.0, False, "Hit wall")
            
            # Check result
            if result.success:
                print(f"Reward: {result.reward}")
        
        See Also:
            Action: Actions performed by the brain
            AgentBrain: Agent decision-making system
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Numerical reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Descriptive outcome message");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data.
        
        SensoryPercept aggregates all sensory inputs received in a single
        simulation step. It contains vision, touch, internal signals,
        proprioception, and audio data.
        
        This is the interface between the world and the brain, providing
        a unified view of the agent's current sensory state.
        
        Example:
        
        .. code-block:: python
        
            # Get vision data
            vision_data = percept.getVision()
            width = percept.getVisionWidth()
            height = percept.getVisionHeight()
            
            # Get touch data
            touch_data = percept.getTouch()
            
            # Get internal states
            internal_data = percept.getInternal()
            
            # Get audio data
            audio_data = percept.getAudio()
        
        See Also:
            SensoryInput: Individual sensory modalities
            Brain: Neural processing system
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision sensory data.
            
            Returns:
                Vision: Vision sensory input object
            ")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory data.
             
             Args:
                 vision: Vision sensory input object
            ")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision image width.
            
            Returns:
                int: Vision width in pixels
            ")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision image height.
            
            Returns:
                int: Vision height in pixels
            ")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory data.
            
            Returns:
                list: Touch sensory input data
            ")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch sensory data.
             
             Args:
                 touch: Touch sensory input data
            ")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal sensory data.
            
            Returns:
                list: Internal signals data
            ")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal sensory data.
             
             Args:
                 internal: Internal signals data
            ")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data.
            
            Returns:
                list: Proprioception (body position) data
            ")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data.
             
             Args:
                 proprioception: Proprioception data
            ")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory data.
            
            Returns:
                list: Audio sensory input data
            ")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory data.
             
             Args:
                 audio: Audio sensory input data
            ")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals.
            
            Returns:
                dict: Dictionary containing all sensory modalities
            ")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get perception timestamp.
            
            Returns:
                int: Simulation timestamp
            ")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set perception timestamp.
             
             Args:
                 timestamp: Simulation timestamp
            ");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation.
        
        SimpleWorld provides a basic 2D simulation environment where agents
        interact with objects and the environment. It handles physics,
        collision detection, and state management.
        
        The world has configurable dimensions and vision parameters for
        agents operating within it.
        
        Attributes:
            width: World width (0.0 to 1.0 normalized)
            height: World height (0.0 to 1.0 normalized)
            maxEnergy: Maximum agent energy capacity
            energyDecayRate: Energy decay per timestep
        
        Example:
        
        .. code-block:: python
        
            # Create and configure a world
            world = pynlm.SimpleWorld()
            world.configure(100, 100, 128, 128)
            
            # Set agent start position
            world.setAgentStart(0.5, 0.5)
            
            # Add objects
            resource = pynlm.WorldObject(0.8, 0.8, pynlm.WorldObjectType.Resource, 50.0)
            world.addObject(resource)
            
            # Check valid positions
            if world.isValidPosition(0.3, 0.3):
                # Safe to move here
                pass
        
        See Also:
            WorldObject: Objects in the world
            AgentBrain: Agent control system
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision.
             
             Args:
                 width: World width in simulation units
                 height: World height in simulation units
                 visionWidth: Agent vision width in pixels
                 visionHeight: Agent vision height in pixels
                 
             Example:
             
             .. code-block:: python
             
                 world.configure(100, 100, 128, 128)  # 100x100m world
             ")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state.
            
            Resets all objects, agent position, and simulation time.
            
            Example:
            
            .. code-block:: python
            
                world.reset()  # Start fresh
            ")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set initial agent start position.
             
             Args:
                 x: Initial X position (0.0 to 1.0)
                 y: Initial Y position (0.0 to 1.0)
            ")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for one simulation timestep.
             
             Args:
                 timestep: Time step duration
                 
             Example:
             
             .. code-block:: python
             
                 world.update(0.1)  # Update for 0.1 seconds
            ")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world.
             
             Args:
                 cmd: Motor command (ActionType)
                 currentTime: Current simulation time
            ")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from the world.
             
             Returns:
                 SensoryPercept: Current sensory observations
            ")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state.
             
             Returns:
                 AgentBody: Current agent physical state
            ")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world.
             
             Args:
                 obj: WorldObject to add
                 
             Example:
             
             .. code-block:: python
             
                 world.addObject(resource)  # Place resource in world
            ")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position.
             
             Args:
                 x: X position
                 y: Y position
            ")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not colliding with obstacles).
             
             Args:
                 x: X position
                 y: Y position
                 
             Returns:
                 bool: True if position is valid, False otherwise
            ")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width.
            
            Returns:
                int: World width in simulation units
            ")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height.
            
            Returns:
                int: World height in simulation units
            ")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy.
            
            Returns:
                float: Maximum energy capacity
            ")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy.
             
             Args:
                 e: Maximum energy value
            ")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate.
            
            Returns:
                float: Energy decay per timestep
            ")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate.
             
             Args:
                 r: Energy decay rate per timestep
            ")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time.
            
            Returns:
                float: Current simulation time
            ")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations.
             
             Args:
                 seed: Random seed value
            ")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed.
            
            Returns:
                int: Current random seed
            ");

    // ===== BRAIN MODULE =====
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class.
        
        Brain is the core component of the NLM system, implementing
        real spiking neural computation with event-driven dynamics and
        integrated memory, prediction, cognition, and neuromodulation systems.
        
        The brain processes sensory input, produces motor output, and maintains
        internal neural states through complex interactions between neurons
        and synapses.
        
        Example:
        
        .. code-block:: python
        
            # Create configuration
            config = pynlm.Config()
            config.set("num_neurons", 1000)
            config.set("learning_rate", 0.01)
            
            # Create brain
            brain = pynlm.Brain(config)
            
            # Initialize with configuration
            brain.initialize()
            
            # Create region for sensory processing
            region_id = brain.addRegion("sensory")
            
            # Run simulation
            for step in range(100):
                brain.step(step)
            
            # Save brain state
            brain.save("brain_state.nml")
        
        See Also:
            Config: Configuration system
            SimpleWorld: World simulation environment
            AgentBrain: Agent interface
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration.
             
             Raises:
                 NLMBrainError: If initialization fails
                 
             Example:
             
             .. code-block:: python
             
                 brain.initialize()  # Prepare neural networks
             ")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step.
             
             Args:
                 currentStep: Current simulation step number
                 
             Example:
             
             .. code-block:: python
             
                 brain.step(step)  # Advance neural simulation
             ")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time
                 
             Example:
             
             .. code-block:: python
             
                 brain.step(step, time)  # Advance with time tracking
             ")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain.
             
             Args:
                 input: Sensory input data (Vision, Audio, or InternalSignals)
                 
             Example:
             
             .. code-block:: python
             
                 brain.receiveSensoryInput(vision_data)  # Process visual input
             ")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron.
             
             Args:
                 neuron: Target neuron ID
                 current: Current to inject (positive or negative)
                 
             Example:
             
             .. code-block:: python
             
                 brain.injectCurrent(neuron_id, 0.1)  # Stimulate neuron
             ")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type (NeuronType enum)
                 current: Current to inject
                 
             Example:
             
             .. code-block:: python
             
                 brain.injectCurrentToNeurons(NeuronType.Excitatory, 0.01)
             ")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity.
             
             Returns:
                 Action: Motor action produced by the brain
                 
             Example:
             
             .. code-block:: python
             
                 action = brain.produceAction()  # Get motor output
             ")
        .def("reset", &Brain::reset,
             "Reset brain state.
             
             Resets all neural states, synapses, and memory systems.
             
             Example:
             
             .. code-block:: python
             
                 brain.reset()  # Clear all neural activity
             ")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file (checkpointing).
             
             Args:
                 filepath: Path where brain state should be saved
                 
             Returns:
                 bool: True if save was successful
                 
             Raises:
                 NLMMemoryError: If memory allocation fails
                 NLMException: If file cannot be written
                 
             Example:
             
             .. code-block:: python
             
                 brain.save("checkpoint.nml")  # Save neural state
             ")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file.
             
             Args:
                 filepath: Path to brain state file
                 
             Returns:
                 bool: True if load was successful
                 
             Raises:
                 NLMMemoryError: If memory allocation fails
                 NLMException: If file cannot be read or is corrupted
                 
             Example:
             
             .. code-block:: python
             
                 brain.load("checkpoint.nml")  # Restore neural state
             ")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region to the brain.
             
             Args:
                 name: Optional name for the region
                 
             Returns:
                 RegionId: Unique identifier for the new region
                 
             Example:
             
             .. code-block:: python
             
                 region_id = brain.addRegion("motor")  # Create motor region
             ")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID.
             
             Args:
                 id: Region ID
                 
             Returns:
                 NeuralRegion: The requested region
                 
             Example:
             
             .. code-block:: python
             
                 region = brain.getRegion(region_id)
             ")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions.
             
             Returns:
                 int: Number of neural regions
                 
             Example:
             
             .. code-block:: python
             
                 count = brain.getRegionCount()  # Get region count
             ")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs.
             
             Returns:
                 list: List of RegionId objects
                 
             Example:
             
             .. code-block:: python
             
                 region_ids = brain.getRegionIds()
             ")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions.
             
             Returns:
                 list: List of all neural regions
                 
             Example:
             
             .. code-block:: python
             
                 regions = brain.getRegions()  # Get all regions
             ")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions.
             
             Returns:
                 int: Total number of neurons
                 
             Example:
             
             .. code-block:: python
             
                 neurons = brain.getTotalNeuronCount()  # Get neuron count
             ")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions.
             
             Returns:
                 int: Total number of synapses
                 
             Example:
             
             .. code-block:: python
             
                 synapses = brain.getTotalSynapseCount()  # Get synapse count
             ")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons.
             
             Returns:
                 int: Number of active neurons
                 
             Example:
             
             .. code-block:: python
             
                 active = brain.getActiveNeuronCount()  # Get active neuron count
             ")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons.
             
             Returns:
                 int: Number of firing neurons
                 
             Example:
             
             .. code-block:: python
             
                 firing = brain.getFiringNeuronCount()  # Get firing neuron count
             ")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons.
             
             Returns:
                 float: Average firing rate (Hz)
                 
             Example:
             
             .. code-block:: python
             
                 rate = brain.getAverageFiringRate()  # Get firing rate
             ")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio.
             
             Returns:
                 float: E/I ratio (typically 1.0 for balanced network)
                 
             Example:
             
             .. code-block:: python
             
                 ei_ratio = brain.getExcitationInhibitionRatio()
             ")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count.
             
             Returns:
                 int: Total number of spikes across all neurons
                 
             Example:
             
             .. code-block:: python
             
                 spikes = brain.getTotalSpikeCount()  # Get spike count
             ")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
                 
             Example:
             
             .. code-block:: python
             
                 stage = brain.getDevelopmentalStage()
             ")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage.
             
             Args:
                 stage: DevelopmentalStage enum value
                 
             Example:
             
             .. code-block:: python
             
                 brain.setDevelopmentalStage(DevelopmentalStage.Adult)
             ")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration.
             
             Returns:
                 Config: The brain's configuration
                 
             Example:
             
             .. code-block:: python
             
                 config = brain.getConfig()  # Get brain config
             ")
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics.
             
             Example:
             
             .. code-block:: python
             
                 brain.logStatus()  # Print diagnostic info
             ");

    // ===== AGENT MODULE =====
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world.
        
        AgentBrain provides a high-level interface between the neural brain
        and the simulation world, handling sensory processing, motor decoding,
        and neuromodulation applications.
        
        It translates neural activity into observable behaviors and world
        interactions, implementing the full sensory-motor loop.
        
        Example:
        
        .. code-block:: python
        
            # Create brain and world
            brain = pynlm.Brain(config)
            world = pynlm.SimpleWorld()
            agent_brain = pynlm.AgentBrain(brain)
            
            # Initialize with world
            agent_brain.initialize(world)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            
            # Get action
            action = agent_brain.decodeMotorCommand()
            
            # Apply reward modulation
            result = world.applyMotorCommand(action.type, time)
            agent_brain.applyRewardModulation(result.reward, result.predicted_reward)
        
        See Also:
            Brain: Underlying neural simulation
            SimpleWorld: Simulation environment
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent brain with world.
             
             Args:
                 world: Simulation world
                 
             Example:
             
             .. code-block:: python
             
                 agent_brain.initialize(world)  # Connect to world
             ")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size.
             
             Returns:
                 int: Expected sensory input dimension
            ")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size.
             
             Returns:
                 int: Expected motor output dimension
            ")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain.
             
             Args:
                 percept: Sensory percept data
                 
             Example:
             
             .. code-block:: python
             
                 agent_brain.processSensoryInput(percept)  # Process vision
            ")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command.
             
             Returns:
                 Action: Motor command/Action object
                 
             Example:
             
             .. code-block:: python
             
                 action = agent_brain.decodeMotorCommand()
            ")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation.
             
             Args:
                 reward: Actual reward received
                 predictedReward: Predicted reward
                 
             Example:
             
             .. code-block:: python
             
                 agent_brain.applyRewardModulation(10.0, 8.0)  # Reward learning
            ")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system.
             
             Args:
                 timestep: Simulation timestep
                 
             Example:
             
             .. code-block:: python
             
                 agent_brain.updateDevelopment(0.1)  # Update development
            ")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
                 
             Example:
             
             .. code-block:: python
             
                 stage = agent_brain.getDevelopmentalStage()
            ")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level.
             
             Returns:
                 float: Current neuromodulation level
                 
             Example:
             
             .. code-block:: python
             
                 level = agent_brain.getNeuromodulationLevel()
            ")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level.
             
             Returns:
                 float: Current curiosity level
                 
             Example:
             
                 .. code-block:: python
                 
                     curiosity = agent_brain.getCuriosityLevel()
            ")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level.
             
             Returns:
                 float: Current novelty level
                 
             Example:
             
                 .. code-block:: python
                 
                     novelty = agent_brain.getNoveltyLevel()
            ")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error.
             
             Returns:
                 float: Current prediction error
                 
             Example:
             
                 .. code-block:: python
                 
                     error = agent_brain.getPredictionError()
            ")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode.
             
             Resets agent state for a new learning episode.
             
             Example:
             
                 .. code-block:: python
                 
                     agent_brain.reset()  # Start new episode
            ")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain.
             
             Returns:
                 Brain: The underlying neural brain
                 
             Example:
             
                 .. code-block:: python
                 
                     brain = agent_brain.getBrain()
            ")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation.
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
             
                 .. code-block:: python
                 
                     agent_brain.enableRewardModulation(True)
            ")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity.
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
             
                 .. code-block:: python
                 
                     agent_brain.enableStructuralPlasticity(True)
            ")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development system.
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
             
                 .. code-block:: python
                 
                     agent_brain.enableDevelopment(True)
            ")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity system.
             
             Args:
                 enable: True to enable, False to disable
                 
             Example:
             
                 .. code-block:: python
                 
                     agent_brain.enableCuriosity(True)
            ")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled.
             
             Returns:
                 bool: True if reward modulation is enabled
                 
             Example:
             
                 .. code-block:: python
                 
                     enabled = agent_brain.isRewardModulationEnabled()
            ")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled.
             
             Returns:
                 bool: True if structural plasticity is enabled
                 
             Example:
             
                 ".. code-block:: python
                 
                     enabled = agent_brain.isStructuralPlasticityEnabled()
            ")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development system is enabled.
             
             Returns:
                 bool: True if development system is enabled
                 
             Example:
             
                 .. code-block:: python
                 
                     enabled = agent_brain.isDevelopmentEnabled()
            ")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity system is enabled.
             
             Returns:
                 bool: True if curiosity system is enabled
                 
             Example:
             
                 .. code-block:: python
                 
                     enabled = agent_brain.isCuriosityEnabled()
            ");

    // ===== FACTORY FUNCTIONS =====
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object.
        
        Returns:
            Config: New default configuration
            
        Example:
        
        .. code-block:: python
        
            config = pynlm.createDefaultConfig()
        ");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration.
        
        Args:
            config: Configuration object for the brain
            
        Returns:
            Brain: New brain instance
            
        Example:
        
        .. code-block:: python
        
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
        ");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world.
        
        Returns:
            SimpleWorld: New world instance
            
        Example:
        
        .. code-block:: python
        
            world = pynlm.createSimpleWorld()
        ");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface.
        
        Args:
            brain: Brain instance to wrap
            
        Returns:
            AgentBrain: New agent brain interface
            
        Example:
        
        .. code-block:: python
        
            brain = pynlm.createBrain(config)
            agent_brain = pynlm.createAgentBrain(brain)
        ");

    // ===== UTILITY FUNCTIONS =====
    m.def("createProgressTracker", []() -> std::unique_ptr<ProgressTracker> {
        return std::make_unique<ProgressTracker>();
    }, "Create a progress tracker for monitoring long operations.
        
        Returns:
            ProgressTracker: New progress tracker instance
            
        Example:
        
        .. code-block:: python
        
            tracker = pynlm.createProgressTracker()
            tracker.setCallback(lambda progress, msg: print(f"Progress: {progress:.1%}"))
            tracker.setMaxSteps(100)
        ");

    m.def("runSimulationWithProgress", [](std::shared_ptr<Brain> brain, int steps, 
                                          std::shared_ptr<SimpleWorld> world, 
                                          ProgressTracker::ProgressCallback callback) {
        auto tracker = std::make_unique<ProgressTracker>();
        tracker->setCallback(callback);
        tracker->setMaxSteps(steps);
        
        if (world) {
            world->reset();
        }
        brain->reset();
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            tracker->incrementStep(std::string("Step ") + std::to_string(step + 1));
            brain->step(step);
            if (world) {
                world->update(1.0f);
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("steps"), py::arg("world") = nullptr, py::arg("callback"),
       "Run a simulation with progress tracking.
        
        Args:
            brain: Brain instance to run
            steps: Number of simulation steps
            world: Optional world instance (can be None)
            callback: Progress callback function
            
        Returns:
            bool: True if simulation completed successfully
            
        Example:
        
        .. code-block:: python
        
            def progress_callback(progress, message):
                print(f"Progress: {progress:.1%}: {message}")
            
            brain = pynlm.createBrain(config)
            world = pynlm.createSimpleWorld()
            pynlm.runSimulationWithProgress(brain, 100, world, progress_callback)
        ");

    m.def("saveBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Save brain state to file with error handling.
        
        Args:
            brain: Brain instance to save
            filepath: Path to save file
            
        Returns:
            bool: True if save was successful
            
        Raises:
            NLMMemoryError: If memory allocation fails
            NLMException: If file cannot be written
            
        Example:
        
        .. code-block:: python
        
            brain = pynlm.createBrain(config)
            if brain.save("checkpoint.nml"):
                print("Brain saved successfully")
        ");

    m.def("loadBrainState", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from file with error handling.
        
        Args:
            brain: Brain instance to load into
            filepath: Path to load file
            
        Returns:
            bool: True if load was successful
            
        Raises:
            NLMMemoryError: If memory allocation fails
            NLMException: If file cannot be read or is corrupted
            
        Example:
        
        .. code-block:: python
        
            brain = pynlm.createBrain(config)
            if brain.load("checkpoint.nml"):
                print("Brain loaded successfully")
        ");

    // ===== MEMORY AND PERFORMANCE UTILITIES =====
    m.def("getMemoryUsage", []() {
        return "Memory monitoring not yet implemented for this platform";
    }, "Get current memory usage information.
        
        Returns:
            str: Memory usage information string
            
        Example:
        
        .. code-block:: python
        
            usage = pynlm.getMemoryUsage()
            print(usage)
        ");

    m.def("gcCollect", []() {
        std::cout << "Garbage collection requested" << std::endl;
        return "Garbage collection completed (manual cleanup recommended for C++ objects)";
    }, "Request garbage collection.
        
        Note:
            This is a hint for manual cleanup. Python's garbage collector
            will manage Python objects automatically.
            
        Example:
        
        .. code-block:: python
        
            pynlm.gcCollect()  # Suggest cleanup
        ");

    m.def("getSystemInfo", []() {
        return "System information not yet implemented for this platform";
    }, "Get system information.
        
        Returns:
            str: System information string
            
        Example:
        
        .. code-block:: python
        
            info = pynlm.getSystemInfo()
            print(info)
        ");

    // ===== INVALID IDs (Backward Compatibility) =====
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm