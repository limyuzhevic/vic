#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Usage Example:
        ```python
        import pynlm
        
        # Create a configuration
        config = pynlm.Config()
        config.set("simulation_steps", 1000)
        
        # Create a brain with the configuration
        brain = pynlm.Brain(config)
        brain.initialize()
        
        # Create a simple world for the brain
        world = pynlm.SimpleWorld()
        world.configure(100, 100, 11, 11)
        
        # Create an agent brain
        agent_brain = pynlm.AgentBrain(brain)
        agent_brain.initialize(world)
        ```
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ============================================================================
    // ID TYPES
    // ============================================================================
    // Core identification types for neural simulation components
    // These provide unique identifiers for all major entities in the simulation
    
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the index representation of the ID")
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
        .def("index", &SynapseId::index, "Get the index representation of the ID")
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
        .def("index", &RegionId::index, "Get the index representation of the ID")
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

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================
    // Enumeration types for various neural and simulation parameters
    // Note: These are read-only in Python bindings
    
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron that promotes firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron that suppresses firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron that influences plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron that processes input")
        .value("Motor", NeuronType::Motor, "Motor neuron that produces output")
        .value("Internal", NeuronType::Internal, "Internal regulatory neuron")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synaptic connection")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synaptic connection")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synaptic connection")
        .value("Electrical", SynapseType::Electrical, "Electrical coupling via gap junctions")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction connection")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation phase")
        .value("Adult", DevelopmentalStage::Adult, "Adult stable phase")
        .value("Aging", DevelopmentalStage::Aging, "Aging phase")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron at rest (no spiking)")
        .value("Active", FiringState::Active, "Neuron actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in world")
        .value("MoveRight", ActionType::MoveRight, "Move right in world")
        .value("TurnLeft", ActionType::TurnLeft, "Rotate left")
        .value("TurnRight", ActionType::TurnRight, "Rotate right")
        .value("Look", ActionType::Look, "Look direction")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with object")
        .value("Eat", ActionType::Eat, "Eat resource")
        .value("Drink", ActionType::Drink, "Drink resource")
        .value("Rest", ActionType::Rest, "Rest/idle")
        .value("Wait", ActionType::Wait, "Wait")
        .value("Custom", ActionType::Custom, "Custom action with parameters")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Low-level forward movement")
        .value("MoveBackward", MotorCommand::MoveBackward, "Low-level backward movement")
        .value("TurnLeft", MotorCommand::TurnLeft, "Low-level left turn")
        .value("TurnRight", MotorCommand::TurnRight, "Low-level right turn")
        .value("LookLeft", MotorCommand::LookLeft, "Low-level sensor pan left")
        .value("LookRight", MotorCommand::LookRight, "Low-level sensor pan right")
        .value("Interact", MotorCommand::Interact, "Low-level interaction")
        .value("Wait", MotorCommand::Wait, "Low-level wait")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ============================================================================
    // CONFIGURATION CLASS
    // ============================================================================
    // Handles simulation configuration with JSON support
    // Thread-safe for read operations, exclusive write access recommended
    
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        Stores key-value pairs for controlling simulation behavior.
        Supports JSON file I/O, runtime setting, and command-line loading.
        Thread-safe for read operations.
        
        Usage:
            >>> config = pynlm.Config()
            >>> config.set("simulation_steps", 1000)
            >>> config.set("vision_width", 11)
            >>> brain = pynlm.Brain(config)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file (throws on error)")
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
        
        // Enhanced type-safe getters with validation
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (!value) {
                throw std::runtime_error("Key \"" + key + "\" not found or wrong type");
            }
            return *value;
        }, py::arg("key"), R"pbdoc(Get an integer value, throws if key not found or wrong type)pbdoc")
        
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value) {
                throw std::runtime_error("Key \"" + key + "\" not found or wrong type");
            }
            return *value;
        }, py::arg("key"), R"pbdoc(Get a float value, throws if key not found or wrong type)pbdoc")
        
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (!value) {
                throw std::runtime_error("Key \"" + key + "\" not found or wrong type");
            }
            return *value;
        }, py::arg("key"), R"pbdoc(Get a boolean value, throws if key not found or wrong type)pbdoc")
        
        .def("get_string", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) {
                throw std::runtime_error("Key \"" + key + "\" not found or wrong type");
            }
            return *value;
        }, py::arg("key"), R"pbdoc(Get a string value, throws if key not found or wrong type)pbdoc")
        
        // Type-safe setters with validation
        .def("set_int", [](Config& self, const std::string& key, int value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set an integer value")
        
        .def("set_float", [](Config& self, const std::string& key, double value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a float value")
        
        .def("set_bool", [](Config& self, const std::string& key, bool value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a boolean value")
        
        .def("set_string", [](Config& self, const std::string& key, const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a string value")
        
        // Convenience methods for common configuration keys
        .def("set_simulation_steps", [](Config& self, size_t steps) {
            self.set("simulation_steps", steps);
        }, py::arg("steps"), "Set number of simulation steps")
        
        .def("get_simulation_steps", [](const Config& self) {
            auto steps = self.get<size_t>("simulation_steps");
            return steps ? *steps : 1000; // Default
        }, "Get number of simulation steps")
        
        .def("set_vision_size", [](Config& self, size_t width, size_t height) {
            self.set("vision_width", width);
            self.set("vision_height", height);
        }, py::arg("width"), py::arg("height"), "Set vision dimensions")
        
        .def("get_vision_size", [](const Config& self) {
            auto width = self.get<size_t>("vision_width");
            auto height = self.get<size_t>("vision_height");
            return std::make_pair(width ? *width : 11, height ? *height : 11);
        }, "Get vision dimensions (width, height)")
        
        .def("validate", [](const Config& self) {
            // Basic validation - check for required keys
            std::vector<std::string> errors;
            
            auto steps = self.get<size_t>("simulation_steps");
            if (!steps || *steps == 0) {
                errors.push_back("simulation_steps must be positive");
            }
            
            auto width = self.get<size_t>("vision_width");
            if (!width || *width == 0) {
                errors.push_back("vision_width must be positive");
            }
            
            auto height = self.get<size_t>("vision_height");
            if (!height || *height == 0) {
                errors.push_back("vision_height must be positive");
            }
            
            if (!errors.empty()) {
                throw std::runtime_error("Configuration validation failed: " + 
                    std::accumulate(errors.begin() + 1, errors.end(), errors[0],
                        [](const std::string& a, const std::string& b) {
                            return a + "; " + b;
                        }));
            }
            
            return true;
        }, "Validate configuration, throws if invalid")
        
        .def("clone", []() {
            return std::make_shared<Config>();
        }, "Create a new default configuration")
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ============================================================================
    // SENSORY INPUT CLASSES
    // ============================================================================
    // Base classes and implementations for sensory input
    // Thread-safe for concurrent access
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input
        
        Abstract base class representing sensory input from the environment.
        All sensory modalities inherit from this class.
        Thread-safe for read operations, not thread-safe for write operations.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for the sensory input")
        .def("clone", &SensoryInput::clone, "Clone this sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input - visual data from agent's camera
        
        Represents 2D visual input as a grayscale intensity grid.
        Format: row-major, values 0.0-1.0 (0=black, 1=white).
        Thread-safe for read operations.
        
        Usage:
            >>> vision = pynlm.Vision(11, 11)
            >>> vision.setData(pixel_values)  # 121 values for 11x11 grid
            >>> width = vision.getWidth()
            >>> height = vision.getHeight()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Initialize vision with dimensions (width, height, channels)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() < self.getWidth() * self.getHeight()) {
                throw std::runtime_error("Data size too small for vision dimensions");
            }
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set image data as vector of floats)")
        .def("setData", [](Vision& self, const std::vector<int>& data) {
            std::vector<float> float_data(data.begin(), data.end());
            self.setData(float_data);
        }, py::arg("data"), R"pbdoc(Set image data as vector of integers (0-255)")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("normalizeData", [](Vision& self) {
            auto& data = self.getData();
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > min_val) {
                float scale = 1.0f / (max_val - min_val);
                for (auto& val : data) {
                    val = (val - min_val) * scale;
                }
            }
        }, "Normalize image data to range [0, 1]");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input - sound data from agent's microphone
        
        Represents audio signals as time-series data.
        Thread-safe for read operations.
        
        Usage:
            >>> audio = pynlm.Audio(44100, 1024)
            >>> audio.setSampleRate(44100)
            >>> audio.setData(sound_samples)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize audio with sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() != self.getNumSamples()) {
                throw std::runtime_error("Data size must match number of samples");
            }
            self.setData(data);
        }, py::arg("data"), R"pbdoc(Set audio data as vector of float samples)")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("normalizeData", [](Audio& self) {
            auto& data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end(),
                [](float a, float b) { return std::abs(a) < std::abs(b); });
            if (max_val != 0.0f) {
                for (auto& val : data) {
                    val /= max_val;
                }
            }
        }, "Normalize audio data to range [-1, 1]");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input - brain's internal state
        
        Represents internal signals from the brain such as homeostatic
        signals, neuromodulation, and regulatory signals.
        Thread-safe for read operations.
        
        Usage:
            >>> signals = pynlm.InternalSignals()
            >>> signals.addSignal(0.5)  # Add a signal value
            >>> data = signals.getData()  # Get all signals
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("getSignalCount", &InternalSignals::getSignalCount,
             "Get number of signals stored")
        .def("getSignal", [](const InternalSignals& self, size_t index) {
            const auto& data = self.getData();
            if (index >= data.size()) {
                throw std::runtime_error("Signal index out of range");
            }
            return data[index];
        }, py::arg("index"), "Get signal at specific index")
        .def("getMean", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get mean of all signals")
        .def("getStdDev", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.size() <= 1) return 0.0f;
            
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            return std::sqrt(variance);
        }, "Get standard deviation of all signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // ============================================================================
    // ACTION CLASS
    // ============================================================================
    // Represents agent motor actions with type and parameters
    // Thread-safe for read operations, not thread-safe for write operations
    
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output
        
        Represents a motor action with type and optional parameters.
        Thread-safe for read operations, not thread-safe for write operations.
        
        Usage:
            >>> action = pynlm.Action(pynlm.ActionType.MoveForward, [1.0, 0.0])
            >>> action_type = action.getType()
            >>> params = action.getParameters()
            >>> name = action.getName()
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("isCustom", [](const Action& self) {
            return self.getType() == ActionType::Custom;
        }, "Check if this is a custom action")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to this action")
        .def("removeParameter", [](Action& self, size_t index) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw std::runtime_error("Parameter index out of range");
            }
            params.erase(params.begin() + index);
            self.setParameters(params);
        }, py::arg("index"), "Remove parameter at specific index")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            const auto& params = self.getParameters();
            if (index >= params.size()) {
                throw std::runtime_error("Parameter index out of range");
            }
            return params[index];
        }, py::arg("index"), "Get parameter at specific index");

    // ============================================================================
    // WORLD OBJECT CLASS
    // ============================================================================
    // Represents objects in the simulation world
    // Thread-safe for concurrent access
    
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation
        
        Represents objects in the simulation world with position, type, and properties.
        Thread-safe for concurrent access.
        
        Usage:
            >>> wall = pynlm.WorldObject(10.0, 20.0, pynlm.WorldObjectType.Wall)
            >>> wall.radius = 1.0
            >>> wall.value = 0.0  # No reward/damage
            >>> world.addObject(wall)
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
        .def("getPosition", [](const WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get object position as (x, y)")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if this is a resource (positive reward)")
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if this is a hazard (negative reward)")
        .def("getDistanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Get distance to point (x, y)")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            return dist < (self.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("clone", [](const WorldObject& self) {
            return WorldObject(self.x, self.y, self.type, self.value, self.radius);
        }, "Create a copy of this object");

    // ============================================================================
    // AGENT BODY CLASS
    // ============================================================================
    // Contains physical properties and internal state of the agent
    // Thread-safe for concurrent access
    
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state
        
        Contains physical properties and internal state of the agent.
        This is part of the environment, not the brain.
        Thread-safe for concurrent access.
        
        Usage:
            >>> body = pynlm.AgentBody()
            >>> body.x = 10.0
            >>> body.y = 20.0
            >>> body.orientation = 0.5  # radians
            >>> body.energy = 0.8
            >>> body.isMoving = True
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
        .def("reset", &AgentBody::reset)
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y)")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy)")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set velocity")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get speed (magnitude of velocity)")
        .def("getEnergyLevel", [](const AgentBody& self) {
            return self.energy;  // Already 0-1 range
        }, "Get energy level (0-1)")
        .def("setEnergyLevel", [](AgentBody& self, float level) {
            self.energy = std::max(0.0f, std::min(1.0f, level));
        }, py::arg("level"), "Set energy level (clamped to 0-1)")
        .def("getHealthLevel", [](const AgentBody& self) {
            return self.health;  // Already 0-1 range
        }, "Get health level (0-1)")
        .def("setHealthLevel", [](AgentBody& self, float level) {
            self.health = std::max(0.0f, std::min(1.0f, level));
        }, py::arg("level"), "Set health level (clamped to 0-1)")
        .def("getAge", [](const AgentBody& self) {
            return self.age;
        }, "Get agent age in simulation steps")
        .def("getMovementState", [](const AgentBody& self) {
            return std::make_pair(self.isMoving, self.isTurning);
        }, "Get movement state as (is_moving, is_turning)")
        .def("updatePosition", [](AgentBody& self, float x, float y, float dt) {
            // Simple kinematic update
            self.x = x;
            self.y = y;
            self.velocityX = 0.0f; // Would normally integrate from forces
            self.velocityY = 0.0f;
            self.lastActionTime += dt;
        }, py::arg("x"), py::arg("y"), py::arg("dt"), "Update position (placeholder)");

    // ============================================================================
    // ACTION RESULT CLASS
    // ============================================================================
    // Result from applying a motor command to the world
    // Thread-safe for concurrent access
    
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world
        
        Contains the result of applying a motor command to the world.
        Thread-safe for concurrent access.
        
        Usage:
            >>> result = pynlm.ActionResult(reward=1.0, success=True, message="Collected")
            >>> reward = result.reward
            >>> success = result.success
            >>> message = result.message
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("getReward", [](const ActionResult& self) {
            return self.reward;
        }, "Get reward value")
        .def("setReward", [](ActionResult& self, float reward) {
            self.reward = reward;
        }, py::arg("reward"), "Set reward value")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if reward is negative")
        .def("isZero", [](const ActionResult& self) {
            return self.reward == 0.0f;
        }, "Check if reward is zero")
        .def("getSuccessMessage", [](const ActionResult& self) {
            return self.success ? self.message : "Failed: " + self.message;
        }, "Get appropriate success message")
        .def("clone", [](const ActionResult& self) {
            return ActionResult(self.reward, self.success, self.message);
        }, "Create a copy of this result");

    // ============================================================================
    // SENSORY PERCEPT CLASS
    // ============================================================================
    // Sensory data packet - what the agent perceives
    // Contains only low-level sensory signals, NO semantic labels
    // Thread-safe for read operations, not thread-safe for write operations
    
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data
        
        Contains low-level sensory signals from the environment.
        Thread-safe for read operations, not thread-safe for write operations.
        
        Usage:
            >>> percept = pynlm.SensoryPercept()
            >>> vision_data = percept.getVision()  # 11x11 grid flattened
            >>> width = percept.getVisionWidth()
            >>> height = percept.getVisionHeight()
            >>> touch_data = percept.getTouch()
            >>> internal_data = percept.getInternal()
            >>> audio_data = percept.getAudio()
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getVisionDataAsMatrix", [](const SensoryPercept& self) {
            auto data = self.getVision();
            size_t width = self.getVisionWidth();
            size_t height = self.getVisionHeight();
            
            std::vector<std::vector<float>> matrix(height, std::vector<float>(width));
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    matrix[y][x] = data[y * width + x];
                }
            }
            return matrix;
        }, "Get vision data as 2D matrix")
        .def("setVisionFromMatrix", [](SensoryPercept& self, const std::vector<std::vector<float>>& matrix) {
            if (matrix.empty()) {
                throw std::runtime_error("Vision matrix cannot be empty");
            }
            
            size_t height = matrix.size();
            size_t width = matrix[0].size();
            
            std::vector<float> data(width * height);
            for (size_t y = 0; y < height; ++y) {
                if (matrix[y].size() != width) {
                    throw std::runtime_error("Vision matrix rows have inconsistent lengths");
                }
                for (size_t x = 0; x < width; ++x) {
                    data[y * width + x] = matrix[y][x];
                }
            }
            
            self.setVision(data);
            // Note: Vision dimensions should be set elsewhere (by SimpleWorld)
        }, py::arg("matrix"), "Set vision data from 2D matrix")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals concatenated")
        .def("getSignalCount", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get total number of signals")
        .def("getSignalAt", [](const SensoryPercept& self, size_t index) {
            const auto& signals = self.getAllSignals();
            if (index >= signals.size()) {
                throw std::runtime_error("Signal index out of range");
            }
            return signals[index];
        }, py::arg("index"), "Get signal at specific index")
        .def("normalizeVision", [](SensoryPercept& self) {
            auto& vision = self.getVision();
            float min_val = *std::min_element(vision.begin(), vision.end());
            float max_val = *std::max_element(vision.begin(), vision.end());
            if (max_val > min_val) {
                float scale = 1.0f / (max_val - min_val);
                for (auto& val : vision) {
                    val = (val - min_val) * scale;
                }
            }
        }, "Normalize vision data to range [0, 1]")
        .def("getVisionStats", [](const SensoryPercept& self) {
            auto data = self.getVision();
            if (data.empty()) {
                return std::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
            }
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            float stddev = std::sqrt(variance);
            
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            
            return std::make_tuple(mean, stddev, min_val, max_val);
        }, "Get vision statistics (mean, stddev, min, max)");

        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getSignalCount", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get total number of signals")
        .def("getSignalAt", [](const SensoryPercept& self, size_t index) {
            const auto& signals = self.getAllSignals();
            if (index >= signals.size()) {
                throw std::runtime_error("Signal index out of range");
            }
            return signals[index];
        }, py::arg("index"), "Get signal at specific index")
        .def("normalizeVision", [](SensoryPercept& self) {
            auto& vision = self.getVision();
            float min_val = *std::min_element(vision.begin(), vision.end());
            float max_val = *std::max_element(vision.begin(), vision.end());
            if (max_val > min_val) {
                float scale = 1.0f / (max_val - min_val);
                for (auto& val : vision) {
                    val = (val - min_val) * scale;
                }
            }
        }, "Normalize vision data to range [0, 1]")
        .def("getVisionStats", [](const SensoryPercept& self) {
            auto data = self.getVision();
            if (data.empty()) {
                return std::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
            }
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            float stddev = std::sqrt(variance);
            
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            
            return std::make_tuple(mean, stddev, min_val, max_val);
        }, "Get vision statistics (mean, stddev, min, max)");

    // ============================================================================
    // SIMPLE WORLD CLASS
    // ============================================================================
    // Simple 2D world for NLM simulation
    // Thread-safe for concurrent access
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation
        
        Provides sensory experience without semantic labels.
        Thread-safe for concurrent access.
        
        Usage:
            >>> world = pynlm.SimpleWorld()
            >>> world.configure(100, 100, 11, 11)
            >>> world.reset()
            >>> percept = world.getSensoryPercept()
            >>> body = world.getAgentBody()
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision resolution")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent, returns ActionResult")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        
        // World statistics and utilities
        .def("getObjectCount", [](const SimpleWorld& self) {
            return self.getSensoryPercept().getAllSignals().size();
        }, "Get number of objects in world")
        .def("getAgentPosition", [](const SimpleWorld& self) {
            const auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y)")
        .def("setAgentPosition", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getAgentEnergy", [](const SimpleWorld& self) {
            return self.getAgentBody().energy;
        }, "Get agent energy (0-1)")
        .def("isAgentAlive", [](const SimpleWorld& self) {
            return self.getAgentBody().health > 0.0f;
        }, "Check if agent is alive (health > 0)")
        .def("getWorldBounds", [](const SimpleWorld& self) {
            return std::make_tuple(0.0f, 0.0f, self.getWidth(), self.getHeight());
        }, "Get world bounds as (x_min, y_min, x_max, y_max)");

        .def("getWorldBounds", [](const SimpleWorld& self) {
            return std::make_tuple(0.0f, 0.0f, self.getWidth(), self.getHeight());
        }, "Get world bounds as (x_min, y_min, x_max, y_max)");

    // ============================================================================
    // BRAIN CLASS
    // ============================================================================
    // Central neural simulation brain class
    // Thread-safe for read operations, not thread-safe for write operations
    
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        Thread-safe for read operations, not thread-safe for write operations.
        
        Usage:
            >>> config = pynlm.Config()
            >>> config.set("simulation_steps", 1000)
            >>> brain = pynlm.Brain(config)
            >>> brain.initialize()
            >>> brain.step(0)
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get active neuron count")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getActiveSpikeCount", &Brain::getPendingSpikeEventCount, "Get pending spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        
        // Enhanced Brain methods with validation and error handling
        .def("injectCurrentToNeuron", [](Brain& self, uint64_t neuron_id, MembranePotential current) {
            self.injectCurrent(NeuronId(neuron_id), current);
        }, py::arg("neuron_id"), py::arg("current"), "Inject current to neuron by ID")
        .def("injectCurrentToNeuronsByType", [](Brain& self, NeuronType type, MembranePotential current) {
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"), "Inject current to neurons of specific type")
        .def("getNeuronCountByType", [](const Brain& self, NeuronType type) {
            // This would need a method in the C++ Brain class to get neurons by type
            // For now, we'll throw a not implemented error
            throw std::runtime_error("getNeuronCountByType not yet implemented in C++ Brain");
        }, py::arg("type"), "Get neuron count by type")
        .def("getRegionByName", [](Brain& self, const std::string& name) {
            size_t count = self.getRegionCount();
            auto region_ids = self.getRegionIds();
            for (size_t i = 0; i < count; ++i) {
                auto region = self.getRegion(region_ids[i]);
                if (region) {
                    // Would need a method to get region name
                    // For now, throw not implemented
                    throw std::runtime_error("getRegionByName not yet implemented in C++ Brain");
                }
            }
            throw std::runtime_error("Region not found: " + name);
        }, py::arg("name"), "Get region by name (not yet implemented)")
        .def("getBrainStats", [](const Brain& self) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = static_cast<double>(self.getTotalNeuronCount());
            stats["total_synapses"] = static_cast<double>(self.getTotalSynapseCount());
            stats["active_neurons"] = static_cast<double>(self.getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<double>(self.getFiringNeuronCount());
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = static_cast<double>(self.getTotalSpikeCount());
            return stats;
        }, "Get brain statistics as dictionary")
        .def("validateBrainState", [](const Brain& self) {
            bool valid = true;
            std::string errors;
            
            if (self.getTotalNeuronCount() == 0) {
                valid = false;
                errors += "No neurons initialized; ";
            }
            
            if (self.getTotalSynapseCount() == 0) {
                errors += "No synapses present; ";
            }
            
            auto stage = self.getDevelopmentalStage();
            if (stage == DevelopmentalStage::Initial) {
                errors += "Brain is still in Initial developmental stage; ";
            }
            
            if (!errors.empty() && valid) {
                throw std::runtime_error("Brain validation warnings: " + errors);
            }
            
            return valid;
        }, "Validate brain state, returns True if valid, throws if invalid with warnings")
        .def("simulateEpisode", [](Brain& self, const SimpleWorld& world, size_t steps) {
            self.reset();
            world.reset();
            
            for (size_t i = 0; i < steps; ++i) {
                world.update(static_cast<double>(i));
                self.step(static_cast<SimulationStep>(i), static_cast<Timestamp>(i));
            }
        }, py::arg("world"), py::arg("steps"), "Simulate a complete episode");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        Handles sensory transduction and motor decoding.
        Thread-safe for read operations, not thread-safe for write operations.
        
        Usage:
            >>> brain = pynlm.Brain(config)
            >>> agent_brain = pynlm.AgentBrain(brain)
            >>> agent_brain.initialize(world)
            >>> percept = agent_brain.processSensoryInput(percept)
            >>> command = agent_brain.decodeMotorCommand()
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain activity to motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"), "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"), "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"), "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"), "Enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled, "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled, "Check if curiosity is enabled")
        
        // Enhanced AgentBrain methods with validation
        .def("processSensoryData", [](AgentBrain& self, const std::vector<float>& data) {
            // Create a percept from raw data
            SensoryPercept percept;
            percept.setVision(data); // Note: this assumes vision data format
            self.processSensoryInput(percept);
        }, py::arg("data"), "Process raw sensory data directly")
        .def("getNeuromodulationStats", [](const AgentBrain& self) {
            std::map<std::string, double> stats;
            stats["dopamine_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            return stats;
        }, "Get neuromodulation statistics")
        .def("simulateWithWorld", [](AgentBrain& self, SimpleWorld& world, const SensoryPercept& percept, size_t steps) {
            self.initialize(world);
            self.processSensoryInput(percept);
            
            for (size_t i = 0; i < steps; ++i) {
                auto action = self.decodeMotorCommand();
                auto result = world.applyMotorCommand(action, static_cast<double>(i));
                world.update(static_cast<double>(i));
                
                if (result.success) {
                    self.applyRewardModulation(result.reward, 0.0f); // Using 0 for predicted reward
                }
                self.updateDevelopment(static_cast<double>(i));
            }
        }, py::arg("world"), py::arg("percept"), py::arg("steps"), "Simulate with world using agent brain")
        .def("enableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all neuromodulation features")
        .def("disableAllFeatures", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all neuromodulation features");

    // ============================================================================
    // FACTORY FUNCTIONS
    // ============================================================================
    // Convenience factory methods for common objects
    // Thread-safe for concurrent creation
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world object");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // ============================================================================
    // UTILITY FUNCTIONS
    // ============================================================================
    // Global utility functions
    
    m.def("createVision", [](size_t width, size_t height, size_t channels) {
        auto vision = std::make_unique<Vision>(width, height, channels);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a Vision object");

    m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
        auto audio = std::make_unique<Audio>(sampleRate, numSamples);
        return audio;
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create an Audio object");

    m.def("createInternalSignals", []() {
        auto signals = std::make_unique<InternalSignals>();
        return signals;
    }, "Create an InternalSignals object");

    // ============================================================================
    // GLOBAL CONSTANTS
    // ============================================================================
    // Global constants and invalid IDs
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
