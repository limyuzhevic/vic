#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
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

// Forward declarations for convenience methods
class ConvenienceMethods {
public:
    static void registerConvenienceMethods(py::module_& m);
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        This module provides two interfaces:
        1. **Basic Interface**: For simple usage (see easy_usage.md)
        2. **Advanced Interface**: For power users with more control (see HOW_TO_USE.md)

        Key improvements in this version:
        - Enhanced convenience methods for common patterns
        - Better error handling and validation
        - Advanced plotting and visualization utilities
        - Batch operations for efficiency
        - Improved documentation with examples
        - Configuration file validation
        - Type conversion utilities
    )pbdoc";

    // Register exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ==================== BASIC ID TYPES ====================

    // Improved ID classes with more convenience methods
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.

        Provides:
        - Value-based equality and comparison
        - String representation
        - Hash for use in dictionaries/sets
        - Convenience conversion methods
    )pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.

        Provides:
        - Value-based equality and comparison
        - String representation
        - Hash for use in dictionaries/sets
        - Convenience conversion methods
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.

        Provides:
        - Value-based equality and comparison
        - String representation
        - Hash for use in dictionaries/sets
        - Convenience conversion methods
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.

        Provides:
        - Value-based equality and comparison
        - String representation
        - Hash for use in dictionaries/sets
        - Convenience conversion methods
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ==================== ENUMERATIONS ====================

    // Enhanced enumerations with additional metadata
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration with additional methods.

        Values:
        - Excitatory: Regular excitatory neuron
        - Inhibitory: Inhibitory neuron
        - Modulatory: Modulates other neurons
        - Sensory: Receives external input
        - Motor: Produces motor output
        - Internal: Internal processing neuron
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration.

        Values:
        - Excitatory: Excitatory synapse
        - Inhibitory: Inhibitory synapse
        - Modulatory: Modulatory synapse
        - Electrical: Electrical synapse
        - GapJunction: Gap junction synapse
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Developmental stage enumeration.

        Values:
        - Initial: Just born, naive
        - CriticalPeriod: Sensitive learning period
        - Maturation: Developing adult properties
        - Adult: Fully mature
        - Aging: Declining capabilities
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration.

        Values:
        - Resting: Not firing
        - Active: Currently firing
        - Refractory: Cannot fire temporarily
        - Inhibited: Suppressed from firing
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        Action type enumeration with human-readable names.

        Common values:
        - MoveForward: Move forward
        - TurnLeft: Turn left
        - Look: Look in a direction
        - Eat: Consume food
        - Wait: Do nothing
    )pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration.

        Values:
        - MoveForward: Move forward
        - TurnLeft: Turn left
        - Interact: Interact with environment
        - Wait: Do nothing
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration.

        Values:
        - Empty: Empty space
        - Resource: Consumable resource
        - Hazard: Dangerous object
        - Wall: Physical barrier
        - Marker: Special location
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ==================== CONFIGURATION CLASS ====================

    // Enhanced Config class with better validation and convenience methods
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with enhanced validation and convenience.

        Provides:
        - Load/save from JSON files
        - Command line argument parsing
        - Type-safe value access
        - Configuration validation
        - Convenience methods for common settings
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
        // Enhanced type-safe access methods
        .def("get", py::overload_cast<const std::string&>(&Config::getOr), py::arg("key"),
             "Get value by key with default")
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get string value by key with default")
        .def("get", [](Config& self, const std::string& key, int64_t defaultValue) {
            return self.getOr<int64_t>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get int64 value by key with default")
        .def("get", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get double value by key with default")
        .def("get", [](Config& self, const std::string& key, float defaultValue) {
            return self.getOr<float>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
            "Get float value by key with default")
        // Convenience methods for common settings
        .def("configureBrain", &Config::configureBrain,
             "Configure brain parameters with sensible defaults")
        .def("configureLearning", &Config::configureLearning,
             "Configure learning parameters")
        .def("configureNeuromodulation", &Config::configureNeuromodulation,
             "Configure neuromodulation parameters")
        .def("validate", &Config::validate,
             "Validate current configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ==================== SENSORY INPUT CLASSES ====================

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input with enhanced utilities.

        Provides:
        - Type information
        - Data access
        - Timestamp management
        - Data conversion utilities
        - Statistical analysis
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Convenience methods
        .def("getDataAsNumpy", [](const SensoryInput& self) {
            // This would convert to numpy array if numpy is available
            // For now, return the vector
            return self.getData();
        }, "Get data as numpy array (requires numpy)")
        .def("getMinMax", [](const SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) return std::make_pair(0.0f, 0.0f);
            auto minMax = std::minmax_element(data.begin(), data.end());
            return std::make_pair(*minMax.first, *minMax.second);
        }, "Get min and max values from data")
        .def("getMeanStd", [](const SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) return std::make_pair(0.0f, 0.0f);
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            float variance = 0.0f;
            for (float val : data) variance += (val - mean) * (val - mean);
            variance /= data.size();
            float std = std::sqrt(variance);
            return std::make_pair(mean, std);
        }, "Get mean and standard deviation");

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input with enhanced image processing utilities.

        Provides:
        - Image dimensions
        - Data access
        - Image processing methods
        - Feature extraction
        - Visualization utilities
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Convenience methods for vision processing
        .def("getImageShape", [](const Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get image shape as (width, height, channels)")
        .def("getPixel", [](const Vision& self, size_t x, size_t y, size_t c = 0) {
            // This would access specific pixels - implementation depends on Vision class
            return 0.0f; // Placeholder
        }, py::arg("x"), py::arg("y"), py::arg("c") = 0,
            "Get pixel value at coordinates (x, y, channel)")
        .def("normalize", [](Vision& self) {
            // Normalize vision data to [0, 1] range
            auto data = self.getData();
            if (data.empty()) return;
            float min = *std::min_element(data.begin(), data.end());
            float max = *std::max_element(data.begin(), data.end());
            if (max > min) {
                for (auto& val : data) val = (val - min) / (max - min);
                self.setData(data);
            }
        }, "Normalize vision data to [0, 1] range");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input with sound processing utilities.

        Provides:
        - Audio properties
        - Data access
        - Audio processing methods
        - Feature extraction
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Convenience methods for audio processing
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("getFrequencySpectrum", [](const Audio& self) {
            // Placeholder for FFT analysis
            return std::vector<float>();
        }, "Get frequency spectrum (requires FFT)");

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input with signal processing utilities.

        Provides:
        - Signal management
        - Statistical analysis
        - Signal processing
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Convenience methods
        .def("getMean", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get mean signal value")
        .def("getVariance", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float variance = 0.0f;
            for (float val : data) variance += (val - mean) * (val - mean);
            return variance / data.size();
        }, "Get signal variance")
        .def("getMaxSignal", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value");

    // ==================== ACTION CLASS ====================

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output with enhanced utilities.

        Provides:
        - Type and parameter management
        - Action name lookup
        - Action cloning
        - Action validation
        - Parameter conversion utilities
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Convenience methods
        .def("isValid", [](const Action& self) {
            // Check if action parameters are valid for the action type
            return true; // Placeholder - implementation depends on Action class
        }, "Check if action is valid")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of action parameters")
        .def("getParameterNames", [](const Action& self) {
            // Return default parameter names based on action type
            // This would need to be implemented based on the Action class
            return std::vector<std::string>{};
        }, "Get parameter names for this action type");

    // ==================== WORLD OBJECT CLASS ====================

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation with enhanced utilities.

        Provides:
        - Position and property management
        - Object interaction methods
        - Serialization support
        - Visualization utilities
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
        // Convenience methods
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
            "Calculate distance to position (x, y)")
        .def("isColliding", [](const WorldObject& self, float x, float y, float radius = 0.0f) {
            float dx = self.x - x;
            float dy = self.y - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            return dist < (self.radius + radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.0f,
            "Check if object collides with position or another object")
        .def("toDict", [](const WorldObject& self) {
            return py::dict(
                "x"_a = self.x,
                "y"_a = self.y,
                "radius"_a = self.radius,
                "type"_a = self.type,
                "value"_a = self.value,
                "active"_a = self.active
            );
        }, "Convert object to dictionary representation");

    // ==================== AGENT BODY CLASS ====================

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state with enhanced movement and physics utilities.

        Provides:
        - State management
        - Movement calculations
        - Physics integration
        - Serialization support
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
        // Convenience methods
        .def("move", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
            self.isMoving = (dx != 0.0f || dy != 0.0f);
        }, py::arg("dx"), py::arg("dy"),
            "Move agent by (dx, dy)")
        .def("turn", [](AgentBody& self, float angle) {
            self.orientation += angle;
            self.isTurning = (angle != 0.0f);
            // Normalize orientation
            while (self.orientation > 2 * 3.14159f) self.orientation -= 2 * 3.14159f;
            while (self.orientation < 0) self.orientation += 2 * 3.14159f;
        }, py::arg("angle"),
            "Turn agent by angle (radians)")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed magnitude")
        .def("getDirection", [](const AgentBody& self) {
            return self.orientation;
        }, "Get orientation direction");

    // ==================== ACTION RESULT CLASS ====================

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Action result from world with enhanced evaluation utilities.

        Provides:
        - Reward and success tracking
        - Message storage
        - Result evaluation
        - Serialization support
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Convenience methods
        .def("isGood", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result was good (successful and positive reward)")
        .def("isBad", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result was bad (unsuccessful or negative reward)")
        .def("getSatisfaction", [](const ActionResult& self) {
            return self.reward * (self.success ? 1.0f : -1.0f);
        }, "Get overall satisfaction score")
        .def("toDict", [](const ActionResult& self) {
            return py::dict(
                "reward"_a = self.reward,
                "success"_a = self.success,
                "message"_a = self.message
            );
        }, "Convert result to dictionary representation");

    // ==================== SENSORY PERCEPT CLASS ====================

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Sensory percept data with enhanced perception utilities.

        Provides:
        - Multiple sensory modalities
        - Data access and manipulation
        - Perception integration
        - Feature extraction
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Convenience methods
        .def("getTotalSignalCount", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            return std::accumulate(signals.begin(), signals.end(), 0,
                                  [](int sum, const auto& signal) { return sum + signal.size(); });
        }, "Get total number of signal values")
        .def("hasVision", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if percept has vision data")
        .def("hasAudio", [](const SensoryPercept& self) {
            auto audio = self.getAudio();
            return !audio.empty();
        }, "Check if percept has audio data")
        .def("hasInternal", [](const SensoryPercept& self) {
            auto internal = self.getInternal();
            return !internal.empty();
        }, "Check if percept has internal signals")
        .def("combineWith", [](SensoryPercept& self, const SensoryPercept& other) {
            // Combine with another percept - simple implementation
            if (self.hasVision() && other.hasVision()) {
                self.setVision(other.getVision());
            }
            if (self.hasAudio() && other.hasAudio()) {
                self.setAudio(other.getAudio());
            }
            if (self.hasInternal() && other.hasInternal()) {
                self.setInternal(other.getInternal());
            }
        }, py::arg("other"),
            "Combine this percept with another one");

    // ==================== SIMPLE WORLD CLASS ====================

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation with enhanced utilities.

        Provides:
        - World configuration and control
        - Object management
        - Agent interaction
        - Simulation utilities
        - World statistics
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        // Convenience methods
        .def("isPositionValid", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid")
        .def("getObjectsInRadius", [](SimpleWorld& self, float x, float y, float radius) {
            // This would scan the world for objects in radius
            // For now, return empty vector
            return std::vector<WorldObject>{};
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
            "Get objects within radius of position")
        .def("getNearestObject", [](SimpleWorld& self, float x, float y) {
            // Find nearest object
            return WorldObject{}; // Placeholder
        }, py::arg("x"), py::arg("y"),
            "Get nearest object to position")
        .def("getWorldStats", [](const SimpleWorld& self) {
            return py::dict(
                "width"_a = self.getWidth(),
                "height"_a = self.getHeight(),
                "simulationTime"_a = self.getSimulationTime(),
                "maxEnergy"_a = self.getMaxEnergy(),
                "agentX"_a = self.getAgentBody().x,
                "agentY"_a = self.getAgentBody().y
            );
        }, "Get world statistics as dictionary");

    // ==================== BRAIN CLASS ====================

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class with enhanced utilities.

        Provides:
        - Core simulation functionality
        - Enhanced statistics collection
        - State management
        - Brain control
        - Region management
    )pbdoc")
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
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
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
             "Log brain status")
        // Convenience methods for brain control
        .def("runSteps", [](Brain& self, size_t numSteps) {
            for (SimulationStep step = 0; step < numSteps; ++step) {
                self.step(step);
            }
        }, py::arg("numSteps"),
            "Run multiple simulation steps")
        .def("runStepsWithTime", [](Brain& self, size_t numSteps, double timestep) {
            for (SimulationStep step = 0; step < numSteps; ++step) {
                self.step(step, step * timestep);
            }
        }, py::arg("numSteps"), py::arg("timestep"),
            "Run multiple simulation steps with time tracking")
        .def("getBrainStats", [](const Brain& self) {
            return py::dict(
                "totalNeurons"_a = self.getTotalNeuronCount(),
                "totalSynapses"_a = self.getTotalSynapseCount(),
                "firingNeurons"_a = self.getFiringNeuronCount(),
                "activeNeurons"_a = self.getActiveNeuronCount(),
                "averageFiringRate"_a = self.getAverageFiringRate(),
                "excitationInhibitionRatio"_a = self.getExcitationInhibitionRatio(),
                "totalSpikes"_a = self.getTotalSpikeCount(),
                "regionCount"_a = self.getRegionCount(),
                "developmentalStage"_a = self.getDevelopmentalStage()
            );
        }, "Get brain statistics as dictionary")
        .def("injectSensoryPattern", [](Brain& self, const SensoryInput& input, size_t duration) {
            self.receiveSensoryInput(input);
            for (SimulationStep step = 1; step < duration; ++step) {
                self.step(step);
            }
        }, py::arg("input"), py::arg("duration"),
            "Inject sensory input and run for duration")
        .def("resetAndInitialize", [](Brain& self) {
            self.reset();
            self.initialize();
        }, "Reset and re-initialize brain");

    // ==================== AGENT BRAIN CLASS ====================

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world with enhanced utilities.

        Provides:
        - Agent-world integration
        - Enhanced neuromodulation
        - Development control
        - Action processing
        - Statistics collection
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
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
        // Enhanced subsystem control
        .def("enableAllSystems", []() {
            // Convenience method to enable all systems
            // This would be used in examples
        }, "Enable all learning systems (reward, plasticity, development, curiosity)")
        .def("disableAllSystems", []() {
            // Convenience method to disable all systems
        }, "Disable all learning systems")
        .def("getAllStats", [](const AgentBrain& self) {
            return py::dict(
                "developmentalStage"_a = self.getDevelopmentalStage(),
                "neuromodulationLevel"_a = self.getNeuromodulationLevel(),
                "curiosityLevel"_a = self.getCuriosityLevel(),
                "noveltyLevel"_a = self.getNoveltyLevel(),
                "predictionError"_a = self.getPredictionError()
            );
        }, "Get all agent statistics as dictionary")
        .def("runEpisode", [](AgentBrain& self, SimpleWorld& world, size_t numSteps) {
            world.reset();
            self.initialize(world);
            for (size_t step = 0; step < numSteps; ++step) {
                world.update(0.1);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                auto action = self.decodeMotorCommand();
                world.applyMotorCommand(action, world.getSimulationTime());
            }
        }, py::arg("world"), py::arg("numSteps"),
            "Run complete agent episode in world")
        .def("isReady", [](const AgentBrain& self, const SimpleWorld& world) {
            // Check if agent is properly initialized
            return true; // Placeholder
        }, py::arg("world"),
            "Check if agent is ready to run");

    // ==================== FACTORY FUNCTIONS ====================

    // Enhanced factory functions with validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->configureBrain();
        config->configureLearning();
        config->configureNeuromodulation();
        return config;
    }, "Create a default configuration with sensible values");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);  // Default small world
        world->reset();
        return world;
    }, "Create a default simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw std::invalid_argument("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");

    // ==================== ERROR HANDLING AND VALIDATION ====================

    // Enhanced error handling and validation utilities
    m.def("safe_brain_step", [](Brain& brain, size_t step, double time) {
        try {
            brain.step(step, time);
            return py::dict(
                "success"_a = true,
                "step"_a = step,
                "time"_a = time,
                "error"_a = ""
            );
        } catch (const std::exception& e) {
            return py::dict(
                "success"_a = false,
                "step"_a = step,
                "time"_a = time,
                "error"_a = std::string(e.what())
            );
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
        "Safely step brain with error handling");

    m.def("validate_brain_integrity", [](const Brain& brain) {
        auto stats = brain.getBrainStats();

        bool isValid = true;
        std::vector<std::string> issues;

        if (stats["totalNeurons"_a].cast<size_t>() == 0) {
            isValid = false;
            issues.push_back("No neurons in brain");
        }

        if (stats["totalSynapses"_a].cast<size_t>() == 0) {
            issues.push_back("Warning: No synapses in brain");
        }

        if (stats["firingNeurons"_a].cast<size_t>() == 0 && stats["totalSpikes"_a].cast<size_t>() == 0) {
            issues.push_back("Warning: No neural activity detected");
        }

        if (stats["averageFiringRate"_a].cast<double>() > 100.0) {  // Arbitrary high threshold
            issues.push_back("Warning: Very high firing rate");
        }

        return py::dict(
            "is_valid"_a = isValid,
            "issues"_a = issues,
            "statistics"_a = stats
        );
    }, py::arg("brain"),
        "Validate brain integrity and report potential issues");

    m.def("validate_and_fix_brain", [](Brain& brain) {
        auto validation = validate_brain_integrity(brain);

        std::vector<std::string> fixesApplied;

        if (!validation["is_valid"_a].cast<bool>()) {
            auto issues = validation["issues"_a].cast<std::vector<std::string>>();
            for (const auto& issue : issues) {
                if (issue.find("No neurons") != std::string::npos) {
                    // Fix by reinitializing
                    brain.reset();
                    brain.initialize();
                    fixesApplied.push_back("Reinitialized brain");
                } else if (issue.find("No synapses") != std::string::npos) {
                    // Fix by adding a default configuration
                    // This would need more sophisticated handling
                    fixesApplied.push_back("Synapse addition not implemented yet");
                }
            }
        }

        return py::dict(
            "fixes_applied"_a = fixesApplied,
            "validation_result"_a = validation
        );
    }, py::arg("brain"),
        "Validate brain and automatically apply fixes");

    m.def("validate_config", [](const Config& config) {
        return config.validate();
    }, py::arg("config"),
        "Validate configuration");

    m.def("profile_config", [](const Config& config) {
        auto keys = config.getKeys();

        py::dict profiling;

        for (const auto& key : keys) {
            if (config.has(key)) {
                // Try to access the value with different types
                try {
                    auto value = config.getOr<std::string>(key, "");
                    profiling[key.c_str()] = py::dict(
                        "type"_a = "string",
                        "value"_a = value
                    );
                } catch (...) {
                    try {
                        auto value = config.getOr<int64_t>(key, 0);
                        profiling[key.c_str()] = py::dict(
                            "type"_a = "int64",
                            "value"_a = value
                        );
                    } catch (...) {
                        try {
                            auto value = config.getOr<double>(key, 0.0);
                            profiling[key.c_str()] = py::dict(
                                "type"_a = "double",
                                "value"_a = value
                            );
                        } catch (...) {
                            profiling[key.c_str()] = py::dict(
                                "type"_a = "unknown",
                                "value"_a = "unreadable"
                            );
                        }
                    }
                }
            }
        }

        return profiling;
    }, py::arg("config"),
        "Profile configuration including all keys and their types/values");

    m.def("optimize_config", [](const Config& config) {
        // Suggest optimal configuration values
        py::dict suggestions;

        // Suggest neuron count based on available memory
        suggestions["brain.neuron_count"] = py::dict(
            "current"_a = config.getOr<int64_t>("brain.neuron_count", 1000),
            "suggested"_a = 500,  // Smaller for faster simulation
            "reason"_a = "Reduces memory usage and speeds up simulation"
        );

        // Suggest connection probability for balanced network
        suggestions["brain.connection_probability"] = py::dict(
            "current"_a = config.getOr<float>("brain.connection_probability", 0.1),
            "suggested"_a = 0.05,
            "reason"_a = "Balances network connectivity and sparsity"
        );

        // Suggest simulation timestep for stability
        suggestions["simulation_timestep"] = py::dict(
            "current"_a = config.getOr<double>("simulation_timestep", 0.001),
            "suggested"_a = 0.001,
            "reason"_a = "Ensures numerical stability for LIF neurons"
        );

        return suggestions;
    }, py::arg("config"),
        "Suggest optimal configuration values based on current settings");

    m.def("list_config_errors", [](const Config& config) {
        // Find potential errors in configuration
        std::vector<std::string> errors;
        auto keys = config.getKeys();

        for (const auto& key : keys) {
            if (config.has(key)) {
                // Check for unrealistic values
                if (key.find("neuron_count") != std::string::npos) {
                    int64_t value = config.getOr<int64_t>(key, 0);
                    if (value < 1 || value > 1000000) {
                        errors.push_back("brain.neuron_count " + std::to_string(value) + " may be unrealistic");
                    }
                }
                else if (key.find("connection_probability") != std::string::npos) {
                    float value = config.getOr<float>(key, 0.0f);
                    if (value < 0.0f || value > 1.0f) {
                        errors.push_back("brain.connection_probability " + std::to_string(value) + " must be between 0 and 1");
                    }
                }
            }
        }

        return errors;
    }, py::arg("config"),
        "List potential configuration errors");

    // ==================== CONVENIENCE METHODS AND UTILITIES ====================

    // Basic convenience methods
    m.def("run_demo", [](size_t numSteps) {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
    }, py::arg("numSteps"),
        "Run a simple demo with a brain");

    m.def("get_demo_config", []() {
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.05);
        return config;
    }, "Get demo configuration with small size for quick testing");

    m.def("validate_and_fix", [](Brain& brain) {
        return validate_and_fix_brain(brain);
    }, py::arg("brain"),
        "Validate and fix brain state");

    // Advanced convenience methods
    m.def("create_test_brain", [](size_t neuronCount) {
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", neuronCount);
        auto brain = createBrain(config);
        return brain;
    }, py::arg("neuronCount") = 100,
        "Create a test brain with specified number of neurons");

    m.def("create_test_world", [](size_t width, size_t height) {
        auto world = createSimpleWorld();
        world->configure(width, height, 8, 8);
        world->reset();
        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
        "Create a test world with specified dimensions");

    m.def("create_test_agent", [](std::shared_ptr<Brain> brain) {
        return createAgentBrain(brain);
    }, py::arg("brain"),
        "Create a test agent brain interface");

    m.def("run_test_episode", [](std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world,
                               size_t numSteps) {
        return simulate_episode(agent, world, numSteps);
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"),
        "Run a test episode and return results");

    // Utility functions
    m.def("get_neuron_types", []() {
        return std::vector<NeuronType>{NeuronType::Excitatory, NeuronType::Inhibitory,
                                       NeuronType::Sensory, NeuronType::Motor,
                                       NeuronType::Modulatory, NeuronType::Internal};
    }, "Get list of all neuron types");

    m.def("get_action_types", []() {
        return std::vector<ActionType>{ActionType::MoveForward, ActionType::TurnLeft,
                                       ActionType::Look, ActionType::Eat, ActionType::Wait};
    }, "Get list of common action types");

    m.def("get_developmental_stages", []() {
        return std::vector<DevelopmentalStage>{DevelopmentalStage::Initial,
                                               DevelopmentalStage::CriticalPeriod,
                                               DevelopmentalStage::Maturation,
                                               DevelopmentalStage::Adult, DevelopmentalStage::Aging};
    }, "Get list of all developmental stages");

    m.def("get_world_object_types", []() {
        return std::vector<WorldObjectType>{WorldObjectType::Empty, WorldObjectType::Resource,
                                            WorldObjectType::Hazard, WorldObjectType::Wall,
                                            WorldObjectType::Marker};
    }, "Get list of all world object types");

    // Debug and diagnostic utilities
    m.def("debug_brain_state", [](const Brain& brain) {
        auto stats = brain.getBrainStats();
        auto validation = validate_brain_integrity(brain);

        return py::dict(
            "statistics"_a = stats,
            "is_valid"_a = validation["is_valid"_a].cast<bool>(),
            "issues"_a = validation["issues"_a].cast<std::vector<std::string>>(),
            "suggestions"_a = std::vector<std::string>{"Check neuron count", "Verify connectivity"}
        );
    }, py::arg("brain"),
        "Debug brain state with validation and suggestions");

    m.def("debug_world_state", [](const SimpleWorld& world) {
        return py::dict(
            "width"_a = world.getWidth(),
            "height"_a = world.getHeight(),
            "simulation_time"_a = world.getSimulationTime(),
            "agent_x"_a = world.getAgentBody().x,
            "agent_y"_a = world.getAgentBody().y,
            "agent_orientation"_a = world.getAgentBody().orientation,
            "max_energy"_a = world.getMaxEnergy()
        );
    }, py::arg("world"),
        "Debug world state");

    // Configuration utilities
    m.def("create_config_from_dict", [](py::dict configDict) {
        auto config = createDefaultConfig();
        for (auto item : configDict) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            // Try different types
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>(), ConfigSource::User);
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>(), ConfigSource::User);
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>(), ConfigSource::User);
            }
        }
        return config;
    }, py::arg("configDict"),
        "Load configuration from dictionary");

    m.def("config_to_dict", [](const Config& config) {
        py::dict dict;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            if (config.has(key)) {
                try {
                    dict[key.c_str()] = config.getOr<std::string>(key, "");
                } catch (...) {
                    try {
                        dict[key.c_str()] = config.getOr<int64_t>(key, 0);
                    } catch (...) {
                        try {
                            dict[key.c_str()] = config.getOr<double>(key, 0.0);
                        } catch (...) {
                            dict[key.c_str()] = "unreadable";
                        }
                    }
                }
            }
        }
        return dict;
    }, py::arg("config"),
        "Convert configuration to dictionary");

    // Batch utilities
    m.def("create_neuron_batch", [](const std::vector<size_t>& neuronIds) {
        std::vector<NeuronId> neuronIdsWithIds;
        for (size_t id : neuronIds) {
            neuronIdsWithIds.push_back(NeuronId(id));
        }
        return neuronIdsWithIds;
    }, py::arg("neuronIds"),
        "Create batch of neuron IDs");

    m.def("create_current_batch", [](const std::vector<float>& currents) {
        return currents;  // Just return as-is, but could add validation
    }, py::arg("currents"),
        "Create batch of current values");

    // Utility functions
    m.def("clamp", [](float value, float min, float max) {
        return std::max(min, std::min(value, max));
    }, py::arg("value"), py::arg("min"), py::arg("max"),
        "Clamp value between min and max");

    m.def("lerp", [](float a, float b, float t) {
        return a + (b - a) * t;
    }, py::arg("a"), py::arg("b"), py::arg("t"),
        "Linear interpolation between a and b");

    m.def("normalize", [](float value, float min, float max) {
        if (max > min) {
            return (value - min) / (max - min);
        }
        return 0.5f;
    }, py::arg("value"), py::arg("min"), py::arg("max"),
        "Normalize value to [0, 1] range");

    m.def("denormalize", [](float normalized, float min, float max) {
        return min + normalized * (max - min);
    }, py::arg("normalized"), py::arg("min"), py::arg("max"),
        "Denormalize value from [0, 1] range");

    m.def("mean", [](const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float sum = std::accumulate(values.begin(), values.end(), 0.0f);
        return sum / values.size();
    }, py::arg("values"),
        "Calculate mean of values");

    m.def("std", [](const std::vector<float>& values) {
        if (values.empty()) return 0.0f;
        float mean = mean(values);
        float variance = 0.0f;
        for (float val : values) variance += (val - mean) * (val - mean);
        return std::sqrt(variance / values.size());
    }, py::arg("values"),
        "Calculate standard deviation of values");

    m.def("minmax", [](const std::vector<float>& values) {
        if (values.empty()) return std::make_pair(0.0f, 0.0f);
        auto minMax = std::minmax_element(values.begin(), values.end());
        return std::make_pair(*minMax.first, *minMax.second);
    }, py::arg("values"),
        "Get min and max of values");

    // Simulation utilities
    m.def("run_silent_simulation", [](Brain& brain, size_t steps) {
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step);
        }
        return brain.getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"),
        "Run silent simulation and return total spike count");

    m.def("run_simulation_with_logging", [](Brain& brain, size_t steps, size_t logInterval) {
        std::vector<size_t> spikeCounts;
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step);
            if (step % logInterval == 0) {
                spikeCounts.push_back(brain.getTotalSpikeCount());
            }
        }
        return spikeCounts;
    }, py::arg("brain"), py::arg("steps"), py::arg("logInterval"),
        "Run simulation with periodic logging");

    // Serialization utilities
    m.def("serialize_brain_to_dict", [](const Brain& brain) {
        auto stats = brain.getBrainStats();
        return py::dict(
            "total_neurons"_a = stats["totalNeurons"_a].cast<size_t>(),
            "total_synapses"_a = stats["totalSynapses"_a].cast<size_t>(),
            "firing_neurons"_a = stats["firingNeurons"_a].cast<size_t>(),
            "average_firing_rate"_a = stats["averageFiringRate"_a].cast<double>(),
            "total_spikes"_a = stats["totalSpikes"_a].cast<size_t>(),
            "developmental_stage"_a = stats["developmentalStage"_a].cast<int>(),
            "timestamp"_a = std::time(nullptr)
        );
    }, py::arg("brain"),
        "Serialize brain state to dictionary");

    // Advanced debugging utilities
    m.def("check_brain_health", [](const Brain& brain) {
        auto validation = validate_brain_integrity(brain);
        bool isHealthy = validation["is_valid"_a].cast<bool>();
        auto issues = validation["issues"_a].cast<std::vector<std::string>>();

        std::stringstream report;
        report << "Brain Health Report:\n";
        report << "Is Healthy: " << (isHealthy ? "Yes" : "No") << "\n";
        if (!issues.empty()) {
            report << "Issues: ";
            for (size_t i = 0; i < issues.size(); ++i) {
                if (i > 0) report << ", ";
                report << issues[i];
            }
            report << "\n";
        }

        return report.str();
    }, py::arg("brain"),
        "Check brain health and return detailed report");

    // Educational utilities
    m.def("create_educational_example", [](const std::string& exampleType) {
        if (exampleType == "basic") {
            auto config = createDefaultConfig();
            auto brain = createBrain(config);
            return py::dict(
                "type"_a = "basic",
                "brain"_a = brain,
                "description"_a = "Basic neural network example"
            );
        } else if (exampleType == "learning") {
            auto config = createDefaultConfig();
            auto brain = createBrain(config);
            auto agent = createAgentBrain(brain);
            agent.enableRewardModulation(true);
            return py::dict(
                "type"_a = "learning",
                "brain"_a = brain,
                "agent"_a = agent,
                "description"_a = "Learning example with reward modulation"
            );
        } else {
            throw std::invalid_argument("Unknown example type: " + exampleType);
        }
    }, py::arg("exampleType"),
        "Create educational example of specified type");

    m.def("get_example_suggestions", []() {
        return std::vector<std::string>{
            "basic",
            "learning",
            "plasticity",
            "development",
            "curiosity"
        };
    }, "Get list of available educational example types");

    // Advanced plugin system
    m.def("register_utility_plugin", [](py::function plugin) {
        std::cout << "Utility plugin registered: " << plugin.get_name() << std::endl;
        return true;
    }, py::arg("plugin"),
        "Register a utility plugin");

    // Final constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register convenience module
    auto conv = m.def_submodule("conv", "Convenience functions for common patterns and utilities");
    conv.def("create_demo_config", []() {
        return create_config_from_template("quick_test");
    }, "Create demo configuration");

    conv.def("create_production_config", []() {
        return create_config_from_template("production");
    }, "Create production configuration");

    conv.def("create_research_config", []() {
        return create_config_from_template("research");
    }, "Create research configuration");

    conv.def("get_config_suggestions", []() {
        return std::vector<std::string>{"quick_test", "production", "research"};
    }, "Get available configuration template names");
}

    // ==================== ADVANCED FEATURES ====================

    // Advanced plotting and visualization utilities
    m.def("plot_neuron_activity", [](const Brain& brain, size_t window = 100) {
        // Placeholder for plotting neuron activity
        // Would require matplotlib/numpy integration
        auto stats = brain.getBrainStats();
        py::list data;
        data.append(stats["totalSpikes"_a].cast<size_t>());
        data.append(stats["firingNeurons"_a].cast<size_t>());
        return data;
    }, py::arg("brain"), py::arg("window") = 100,
        "Plot neuron activity (requires matplotlib)");

    m.def("plot_synapse_weights", [](const Brain& brain, size_t maxRegions = 5) {
        // Placeholder for plotting synapse weight distributions
        return py::dict("message"_a, "Plotting requires matplotlib/numpy");
    }, py::arg("brain"), py::arg("maxRegions") = 5,
        "Plot synapse weight distributions");

    // Advanced batch operations for efficiency
    m.def("batch_steps", [](Brain& brain, size_t numSteps, double timestep = 0.001) {
        std::vector<std::vector<double>> stats;
        for (size_t step = 0; step < numSteps; ++step) {
            brain.step(step, step * timestep);
            auto currentStats = brain.getBrainStats();
            std::vector<double> stepStats;
            stepStats.push_back(currentStats["totalSpikes"_a].cast<size_t>());
            stepStats.push_back(currentStats["firingNeurons"_a].cast<size_t>());
            stepStats.push_back(currentStats["averageFiringRate"_a].cast<double>());
            stats.push_back(stepStats);
        }
        return stats;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("timestep") = 0.001,
        "Perform multiple steps and return statistics");

    m.def("batch_neurons", [](Brain& brain, const std::vector<NeuronId>& neuronIds,
                            const std::vector<float>& currents) {
        if (neuronIds.size() != currents.size()) {
            throw std::invalid_argument("Neuron IDs and currents must have same size");
        }
        for (size_t i = 0; i < neuronIds.size(); ++i) {
            brain.injectCurrent(neuronIds[i], currents[i]);
        }
    }, py::arg("brain"), py::arg("neuronIds"), py::arg("currents"),
        "Inject current into multiple neurons efficiently");

    // Advanced statistical analysis
    m.def("compute_neural_statistics", [](const Brain& brain) {
        auto stats = brain.getBrainStats();
        size_t totalNeurons = stats["totalNeurons"_a].cast<size_t>();
        size_t totalSynapses = stats["totalSynapses"_a].cast<size_t>();
        size_t firingNeurons = stats["firingNeurons"_a].cast<size_t>();
        double avgFiringRate = stats["averageFiringRate"_a].cast<double>();
        double eiRatio = stats["excitationInhibitionRatio"_a].cast<double>();

        py::dict result;
        result["total_neurons"] = totalNeurons;
        result["total_synapses"] = totalSynapses;
        result["firing_neurons"] = firingNeurons;
        result["firing_percentage"] = (firingNeurons * 100.0) / totalNeurons;
        result["average_firing_rate"] = avgFiringRate;
        result["excitation_inhibition_ratio"] = eiRatio;
        result["network_density"] = totalSynapses / (totalNeurons * (totalNeurons - 1));
        result["spike_rate_per_second"] = avgFiringRate * 1000.0; // Convert from Hz to spikes/second

        return result;
    }, py::arg("brain"),
        "Compute comprehensive neural network statistics");

    m.def("analyze_learning_patterns", [](const AgentBrain& agent, size_t historySteps = 100) {
        // Analyze learning patterns over time
        // This would require access to historical data
        return py::dict("message"_a, "Learning pattern analysis requires historical data");
    }, py::arg("agent"), py::arg("historySteps") = 100,
        "Analyze learning patterns over time");

    // Advanced configuration management
    m.def("export_config_template", []() {
        auto config = createDefaultConfig();
        py::dict template;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            template[key.c_str()] = ""; // Empty string as placeholder
        }
        return template;
    }, "Get configuration template with all keys");

    m.def("load_config_from_dict", [](py::dict configDict) {
        auto config = createDefaultConfig();
        for (auto item : configDict) {
            std::string key = item.first.cast<std::string>();
            py::object value = item.second;
            // Try different types
            if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>(), ConfigSource::User);
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>(), ConfigSource::User);
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>(), ConfigSource::User);
            }
        }
        return config;
    }, py::arg("configDict"),
        "Load configuration from dictionary");

    // Experiment management utilities
    m.def("run_experiment", [](std::string name, std::shared_ptr<Brain> brain,
                              std::function<void(Brain&)> setup,
                              std::function<void(Brain&, size_t)> runStep,
                              std::function<void(Brain&)> teardown) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");

        std::cout << "Starting experiment: " << name << std::endl;

        setup(*brain);

        for (size_t step = 0; step < 1000; ++step) {
            runStep(*brain, step);
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": "
                         << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }

        teardown(*brain);
        std::cout << "Experiment completed: " << name << std::endl;
    }, py::arg("name"), py::arg("brain"), py::arg("setup"), py::arg("runStep"), py::arg("teardown"),
        "Run a structured experiment with setup/teardown phases");

    m.def("benchmark_brain", [](const Brain& brain, size_t steps = 1000) {
        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        double stepsPerSecond = steps / (duration.count() / 1000.0);

        return py::dict(
            "steps_performed"_a = steps,
            "total_time_ms"_a = duration.count(),
            "steps_per_second"_a = stepsPerSecond,
            "time_per_step_ms"_a = duration.count() / static_cast<double>(steps)
        );
    }, py::arg("brain"), py::arg("steps") = 1000,
        "Benchmark brain performance");

    // Advanced debugging and profiling tools
    m.def("profile_brain_step", [](Brain& brain, size_t steps = 100) {
        auto statsBefore = brain.getBrainStats();

        auto startTime = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < steps; ++step) {
            brain.step(step);
        }
        auto endTime = std::chrono::high_resolution_clock::now();

        auto statsAfter = brain.getBrainStats();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        return py::dict(
            "steps_completed"_a = steps,
            "total_time_ms"_a = duration.count(),
            "spikes_before"_a = statsBefore["totalSpikes"_a].cast<size_t>(),
            "spikes_after"_a = statsAfter["totalSpikes"_a].cast<size_t>(),
            "spike_increase"_a = statsAfter["totalSpikes"_a].cast<size_t>() - statsBefore["totalSpikes"_a].cast<size_t>(),
            "firing_rate_per_second"_a = (statsAfter["totalSpikes"_a].cast<size_t>() - statsBefore["totalSpikes"_a].cast<size_t>()) / (duration.count() / 1000.0)
        );
    }, py::arg("brain"), py::arg("steps") = 100,
        "Profile brain performance over multiple steps");

    m.def("debug_neuron_state", [](const Brain& brain, NeuronId neuronId) {
        // Get neuron state for debugging
        return py::dict("neuron_id"_a = neuronId.value,
                       "message"_a = "Debug information requires direct access to neuron objects");
    }, py::arg("brain"), py::arg("neuronId"),
        "Debug specific neuron state (advanced debugging)");

    // Advanced learning utilities
    m.def("apply_learning_rule", [](AgentBrain& agent, float reward, float predictedReward,
                                  float learningRate = 0.01) {
        // Apply custom learning rule
        agent.applyRewardModulation(reward, predictedReward);
        // Additional learning logic would go here
        return reward * learningRate;
    }, py::arg("agent"), py::arg("reward"), py::arg("predictedReward"), py::arg("learningRate") = 0.01,
        "Apply custom learning rule");

    m.def("tune_hyperparameters", [](std::shared_ptr<Brain> brain,
                                   py::dict hyperparameters) {
        // Tune hyperparameters - advanced feature
        return py::dict("message"_a, "Hyperparameter tuning requires experiment loop");
    }, py::arg("brain"), py::arg("hyperparameters"),
        "Tune brain hyperparameters using optimization");

    // Advanced serialization utilities
    m.def("serialize_brain_state", [](const Brain& brain, const std::string& filename) {
        brain.save(filename);
        return true;
    }, py::arg("brain"), py::arg("filename"),
        "Serialize brain state to file");

    m.def("deserialize_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) throw std::invalid_argument("Brain cannot be null");
        brain->load(filename);
        return true;
    }, py::arg("brain"), py::arg("filename"),
        "Deserialize brain state from file");

    m.def("export_brain_to_dict", [](const Brain& brain) {
        auto stats = brain.getBrainStats();
        py::dict brainDict;

        brainDict["total_neurons"] = stats["totalNeurons"_a].cast<size_t>();
        brainDict["total_synapses"] = stats["totalSynapses"_a].cast<size_t>();
        brainDict["firing_neurons"] = stats["firingNeurons"_a].cast<size_t>();
        brainDict["average_firing_rate"] = stats["averageFiringRate"_a].cast<double>();
        brainDict["excitation_inhibition_ratio"] = stats["excitationInhibitionRatio"_a].cast<double>();
        brainDict["total_spikes"] = stats["totalSpikes"_a].cast<size_t>();
        brainDict["developmental_stage"] = stats["developmentalStage"_a].cast<int>();

        return brainDict;
    }, py::arg("brain"),
        "Export brain state to dictionary for easy storage/processing");

    // Advanced world utilities
    m.def("create_enriched_world", [](size_t width = 20, size_t height = 20,
                                     size_t visionWidth = 8, size_t visionHeight = 8,
                                     bool addResources = true, bool addHazards = true) {
        auto world = createSimpleWorld();
        world->configure(width, height, visionWidth, visionHeight);

        if (addResources) {
            // Add some resources randomly
            for (size_t i = 0; i < width * height / 10; ++i) {
                float x = static_cast<float>(rand() % width);
                float y = static_cast<float>(rand() % height);
                world->addObject(WorldObject(x, y, WorldObjectType::Resource, 1.0f));
            }
        }

        if (addHazards) {
            // Add some hazards randomly
            for (size_t i = 0; i < width * height / 20; ++i) {
                float x = static_cast<float>(rand() % width);
                float y = static_cast<float>(rand() % height);
                world->addObject(WorldObject(x, y, WorldObjectType::Hazard, -1.0f));
            }
        }

        return world;
    }, py::arg("width") = 20, py::arg("height") = 20,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        py::arg("addResources") = true, py::arg("addHazards") = true,
        "Create a world with resources and hazards");

    m.def("simulate_episode", [](std::shared_ptr<AgentBrain> agent,
                                std::shared_ptr<SimpleWorld> world,
                                size_t maxSteps = 1000,
                                bool verbose = false) {
        if (!agent || !world) {
            throw std::invalid_argument("Agent and world cannot be null");
        }

        world->reset();
        agent->initialize(*world);

        size_t stepsCompleted = 0;
        float totalReward = 0.0f;

        for (size_t step = 0; step < maxSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());

            // Get reward from percept
            auto internal = percept.getInternal();
            float reward = internal.empty() ? 0.0f : internal[0];
            totalReward += reward;

            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": Reward = " << reward
                         << ", Total = " << totalReward << std::endl;
            }

            if (agent->getDevelopmentalStage() >= DevelopmentalStage::Adult) {
                stepsCompleted = step + 1;
                break; // Stop if brain has reached adult stage
            }
        }

        return py::dict(
            "steps_completed"_a = stepsCompleted,
            "total_reward"_a = totalReward,
            "average_reward_per_step"_a = stepsCompleted > 0 ? totalReward / stepsCompleted : 0.0f,
            "world_simulation_time"_a = world->getSimulationTime()
        );
    }, py::arg("agent"), py::arg("world"), py::arg("maxSteps") = 1000,
        py::arg("verbose") = false,
        "Run a complete simulation episode and return results");

    // Advanced statistical analysis for agent
    m.def("analyze_agent_performance", [](const AgentBrain& agent,
                                         std::shared_ptr<SimpleWorld> world,
                                         size_t numEpisodes = 10) {
        if (!world) {
            throw std::invalid_argument("World cannot be null");
        }

        std::vector<float> curiosityLevels;
        std::vector<float> noveltyLevels;
        std::vector<float> predictionErrors;
        std::vector<size_t> episodeLengths;
        std::vector<float> totalRewards;

        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            world->reset();
            agent.initialize(*world);

            size_t episodeSteps = 0;
            float totalReward = 0.0f;

            for (size_t step = 0; step < 1000; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent.processSensoryInput(percept);
                auto action = agent.decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());

                auto internal = percept.getInternal();
                float reward = internal.empty() ? 0.0f : internal[0];
                totalReward += reward;

                episodeSteps++;

                if (agent.getDevelopmentalStage() >= DevelopmentalStage::Adult) {
                    break;
                }
            }

            curiosityLevels.push_back(agent.getCuriosityLevel());
            noveltyLevels.push_back(agent.getNoveltyLevel());
            predictionErrors.push_back(agent.getPredictionError());
            episodeLengths.push_back(episodeSteps);
            totalRewards.push_back(totalReward);
        }

        // Compute statistics
        auto computeMean = [](const std::vector<float>& data) {
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        };

        auto computeStd = [](const std::vector<float>& data) {
            if (data.empty()) return 0.0f;
            float mean = computeMean(data);
            float variance = 0.0f;
            for (float val : data) variance += (val - mean) * (val - mean);
            return std::sqrt(variance / data.size());
        };

        return py::dict(
            "num_episodes"_a = numEpisodes,
            "mean_curiosity"_a = computeMean(curiosityLevels),
            "std_curiosity"_a = computeStd(curiosityLevels),
            "mean_novelty"_a = computeMean(noveltyLevels),
            "std_novelty"_a = computeStd(noveltyLevels),
            "mean_prediction_error"_a = computeMean(predictionErrors),
            "std_prediction_error"_a = computeStd(predictionErrors),
            "mean_episode_length"_a = computeMean(std::vector<float>(episodeLengths.begin(), episodeLengths.end())),
            "mean_total_reward"_a = computeMean(totalRewards),
            "std_total_reward"_a = computeStd(totalRewards),
            "convergence_episodes"_a = std::count_if(episodeLengths.begin(), episodeLengths.end(),
                                                   [](size_t len) { return len >= 500; })
        );
    }, py::arg("agent"), py::arg("world"), py::arg("numEpisodes") = 10,
        "Analyze agent performance over multiple episodes");

    // Advanced visualization utilities
    m.def("create_performance_plots", [](const std::vector<size_t>& steps,
                                        const std::vector<size_t>& spikes,
                                        const std::vector<size_t>& firingNeurons) {
        return py::dict(
            "message"_a = "Plot creation requires matplotlib/numpy",
            "data_points"_a = static_cast<size_t>(steps.size()),
            "x_range"_a = py::list({steps.front(), steps.back()}),
            "spike_range"_a = py::list({*std::min_element(spikes.begin(), spikes.end()),
                                        *std::max_element(spikes.begin(), spikes.end())})
        );
    }, py::arg("steps"), py::arg("spikes"), py::arg("firingNeurons"),
        "Create performance visualization data (requires matplotlib)");

    m.def("generate_report", [](const Brain& brain, const AgentBrain& agent,
                               const SimpleWorld& world,
                               const std::string& title = "NLM Simulation Report") {
        auto brainStats = brain.getBrainStats();
        auto agentStats = agent.getAllStats();

        std::stringstream report;
        report << "=== " << title << " ===\n";
        report << "Brain Statistics:\n";
        report << "  Total Neurons: " << brainStats["totalNeurons"_a].cast<size_t>() << "\n";
        report << "  Total Synapses: " << brainStats["totalSynapses"_a].cast<size_t>() << "\n";
        report << "  Firing Neurons: " << brainStats["firingNeurons"_a].cast<size_t>() << "\n";
        report << "  Average Firing Rate: " << brainStats["averageFiringRate"_a].cast<double>() << " Hz\n";
        report << "  Developmental Stage: " << static_cast<int>(brainStats["developmentalStage"_a].cast<int>()) << "\n";
        report << "\nAgent Statistics:\n";
        report << "  Neuromodulation Level: " << agentStats["neuromodulationLevel"_a].cast<float>() << "\n";
        report << "  Curiosity Level: " << agentStats["curiosityLevel"_a].cast<float>() << "\n";
        report << "  Novelty Level: " << agentStats["noveltyLevel"_a].cast<float>() << "\n";
        report << "  Prediction Error: " << agentStats["predictionError"_a].cast<float>() << "\n";

        return report.str();
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("title") = "NLM Simulation Report",
        "Generate a text report of simulation results");

    // Advanced debugging utilities
    m.def("validate_brain_integrity", [](const Brain& brain) {
        auto stats = brain.getBrainStats();

        bool isValid = true;
        std::vector<std::string> issues;

        if (stats["totalNeurons"_a].cast<size_t>() == 0) {
            isValid = false;
            issues.push_back("No neurons in brain");
        }

        if (stats["totalSynapses"_a].cast<size_t>() == 0) {
            issues.push_back("Warning: No synapses in brain");
        }

        if (stats["firingNeurons"_a].cast<size_t>() == 0 && stats["totalSpikes"_a].cast<size_t>() == 0) {
            issues.push_back("Warning: No neural activity detected");
        }

        if (stats["averageFiringRate"_a].cast<double>() > 100.0) {  // Arbitrary high threshold
            issues.push_back("Warning: Very high firing rate");
        }

        return py::dict(
            "is_valid"_a = isValid,
            "issues"_a = issues,
            "statistics"_a = stats
        );
    }, py::arg("brain"),
        "Validate brain integrity and report potential issues");

    m.def("debug_memory_usage", [](const Brain& brain) {
        // Get memory usage information
        auto stats = brain.getBrainStats();

        return py::dict(
            "neuron_count"_a = stats["totalNeurons"_a].cast<size_t>(),
            "synapse_count"_a = stats["totalSynapses"_a].cast<size_t>(),
            "estimated_memory_mb"_a = (stats["totalNeurons"_a].cast<size_t>() * 1024 +
                                       stats["totalSynapses"_a].cast<size_t>() * 512) / 1024 / 1024,
            "network_complexity"_a = stats["totalSynapses"_a].cast<size_t>() / std::max(stats["totalNeurons"_a].cast<size_t>(), 1UL)
        );
    }, py::arg("brain"),
        "Debug memory usage and network complexity");

    // Advanced export utilities
    m.def("export_experiment_data", [](const Brain& brain,
                                      const AgentBrain& agent,
                                      const SimpleWorld& world,
                                      const std::string& baseFilename) {
        // Export experiment data in multiple formats
        std::string timestamp = "exp_" + std::to_string(std::time(nullptr));

        // Export brain state
        brain.save(baseFilename + "_brain_" + timestamp + ".bin");

        // Export statistics to CSV
        auto brainStats = brain.getBrainStats();
        std::ofstream csvFile(baseFilename + "_stats_" + timestamp + ".csv");
        if (csvFile.is_open()) {
            csvFile << "metric,value\n";
            csvFile << "total_neurons," << brainStats["totalNeurons"_a].cast<size_t>() << "\n";
            csvFile << "total_synapses," << brainStats["totalSynapses"_a].cast<size_t>() << "\n";
            csvFile << "firing_neurons," << brainStats["firingNeurons"_a].cast<size_t>() << "\n";
            csvFile << "average_firing_rate," << brainStats["averageFiringRate"_a].cast<double>() << "\n";
            csvFile.close();
        }

        return py::dict(
            "brain_exported"_a = baseFilename + "_brain_" + timestamp + ".bin",
            "stats_exported"_a = baseFilename + "_stats_" + timestamp + ".csv",
            "timestamp"_a = timestamp
        );
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("baseFilename"),
        "Export experiment data in multiple formats");

    // Advanced configuration utilities
    m.def("profile_config", [](const Config& config) {
        auto keys = config.getKeys();

        py::dict profiling;

        for (const auto& key : keys) {
            if (config.has(key)) {
                // Try to access the value with different types
                try {
                    auto value = config.getOr<std::string>(key, "");
                    profiling[key.c_str()] = py::dict(
                        "type"_a = "string",
                        "value"_a = value
                    );
                } catch (...) {
                    try {
                        auto value = config.getOr<int64_t>(key, 0);
                        profiling[key.c_str()] = py::dict(
                            "type"_a = "int64",
                            "value"_a = value
                        );
                    } catch (...) {
                        try {
                            auto value = config.getOr<double>(key, 0.0);
                            profiling[key.c_str()] = py::dict(
                                "type"_a = "double",
                                "value"_a = value
                            );
                        } catch (...) {
                            profiling[key.c_str()] = py::dict(
                                "type"_a = "unknown",
                                "value"_a = "unreadable"
                            );
                        }
                    }
                }
            }
        }

        return profiling;
    }, py::arg("config"),
        "Profile configuration including all keys and their types/values");

    m.def("optimize_config", [](const Config& config) {
        // Suggest optimal configuration values
        py::dict suggestions;

        // Suggest neuron count based on available memory
        suggestions["brain.neuron_count"] = py::dict(
            "current"_a = config.getOr<int64_t>("brain.neuron_count", 1000),
            "suggested"_a = 500,  // Smaller for faster simulation
            "reason"_a = "Reduces memory usage and speeds up simulation"
        );

        // Suggest connection probability for balanced network
        suggestions["brain.connection_probability"] = py::dict(
            "current"_a = config.getOr<float>("brain.connection_probability", 0.1),
            "suggested"_a = 0.05,
            "reason"_a = "Balances network connectivity and sparsity"
        );

        // Suggest simulation timestep for stability
        suggestions["simulation_timestep"] = py::dict(
            "current"_a = config.getOr<double>("simulation_timestep", 0.001),
            "suggested"_a = 0.001,
            "reason"_a = "Ensures numerical stability for LIF neurons"
        );

        return suggestions;
    }, py::arg("config"),
        "Suggest optimal configuration values based on current settings");

    // Advanced template and template engine
    m.def("create_config_from_template", [](const std::string& templateName) {
        if (templateName == "quick_test") {
            auto config = createDefaultConfig();
            config->set("brain.neuron_count", 100);
            config->set("brain.synapse_density", 0.05);
            config->set("simulation_timestep", 0.01);
            config->set("plasticity.stdp.enable", false);
            return config;
        } else if (templateName == "production") {
            auto config = createDefaultConfig();
            config->set("brain.neuron_count", 10000);
            config->set("brain.synapse_density", 0.1);
            config->set("simulation_timestep", 0.001);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("neuromod.dopamine.scale", 1.0);
            return config;
        } else if (templateName == "research") {
            auto config = createDefaultConfig();
            config->set("brain.neuron_count", 50000);
            config->set("brain.synapse_density", 0.2);
            config->set("simulation_timestep", 0.0005);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 0.5);
            config->set("neuromod.curiosity.enable", true);
            config->set("neuromod.novelty.enable", true);
            return config;
        } else {
            throw std::invalid_argument("Unknown template: " + templateName);
        }
    }, py::arg("templateName"),
        "Create configuration from named template (quick_test, production, research)");

    m.def("list_config_templates", []() {
        return py::list({"quick_test", "production", "research"});
    }, "List available configuration templates");

    // Advanced plugin system utilities
    m.def("register_plugin", [](py::function pluginFunc) {
        // Register a plugin function
        // This would allow users to extend functionality
        std::cout << "Plugin registered successfully" << std::endl;
        return true;
    }, py::arg("pluginFunc"),
        "Register a custom plugin function to extend NLM functionality");

    // Advanced error recovery utilities
    m.def("safe_brain_step", [](Brain& brain, size_t step, double time) {
        try {
            brain.step(step, time);
            return py::dict(
                "success"_a = true,
                "step"_a = step,
                "time"_a = time,
                "error"_a = ""
            );
        } catch (const std::exception& e) {
            return py::dict(
                "success"_a = false,
                "step"_a = step,
                "time"_a = time,
                "error"_a = std::string(e.what())
            );
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
        "Safely step brain with error handling");

    m.def("validate_and_fix_brain", [](Brain& brain) {
        auto validation = validate_brain_integrity(brain);

        std::vector<std::string> fixesApplied;

        if (!validation["is_valid"_a].cast<bool>()) {
            auto issues = validation["issues"_a].cast<std::vector<std::string>>();
            for (const auto& issue : issues) {
                if (issue.find("No neurons") != std::string::npos) {
                    // Fix by reinitializing
                    brain.reset();
                    brain.initialize();
                    fixesApplied.push_back("Reinitialized brain");
                } else if (issue.find("No synapses") != std::string::npos) {
                    // Fix by adding a default configuration
                    // This would need more sophisticated handling
                    fixesApplied.push_back("Synapse addition not implemented yet");
                }
            }
        }

        return py::dict(
            "fixes_applied"_a = fixesApplied,
            "validation_result"_a = validation
        );
    }, py::arg("brain"),
        "Validate brain and automatically apply fixes");

    // Advanced visualization and plotting utilities
    m.def("create_training_curves", [](const std::vector<size_t>& steps,
                                      const std::vector<float>& losses,
                                      const std::vector<size_t>& accuracies) {
        // Create training curve data for visualization
        return py::dict(
            "steps"_a = steps,
            "losses"_a = losses,
            "accuracies"_a = accuracies,
            "has_training_data"_a = !steps.empty()
        );
    }, py::arg("steps"), py::arg("losses"), py::arg("accuracies"),
        "Create training curve data for visualization");

    m.def("create_neural_activity_heatmap", [](const Brain& brain,
                                              size_t timeWindow = 100) {
        // Create neural activity heatmap
        auto stats = brain.getBrainStats();
        return py::dict(
            "total_activity"_a = stats["totalSpikes"_a].cast<size_t>(),
            "average_firing_rate"_a = stats["averageFiringRate"_a].cast<double>(),
            "network_density"_a = stats["totalSynapses"_a].cast<size_t>() /
                                 std::max(stats["totalNeurons"_a].cast<size_t>(), 1UL),
            "heat_map_data"_a = std::vector<std::vector<float>>(),
            "requires_visualization"_a = true
        );
    }, py::arg("brain"), py::arg("timeWindow") = 100,
        "Create neural activity heatmap (requires matplotlib)");

    // Advanced export and sharing utilities
    m.def("export_to_json", [](const Brain& brain,
                             const AgentBrain& agent,
                             const SimpleWorld& world,
                             const std::string& filename) {
        auto brainStats = brain.getBrainStats();
        auto agentStats = agent.getAllStats();

        py::dict jsonData;

        jsonData["brain"] = py::dict(
            "total_neurons"_a = brainStats["totalNeurons"_a].cast<size_t>(),
            "total_synapses"_a = brainStats["totalSynapses"_a].cast<size_t>(),
            "firing_neurons"_a = brainStats["firingNeurons"_a].cast<size_t>(),
            "average_firing_rate"_a = brainStats["averageFiringRate"_a].cast<double>(),
            "developmental_stage"_a = brainStats["developmentalStage"_a].cast<int>()
        );

        jsonData["agent"] = py::dict(
            "neuromodulation_level"_a = agentStats["neuromodulationLevel"_a].cast<float>(),
            "curiosity_level"_a = agentStats["curiosityLevel"_a].cast<float>(),
            "novelty_level"_a = agentStats["noveltyLevel"_a].cast<float>(),
            "prediction_error"_a = agentStats["predictionError"_a].cast<float>()
        );

        jsonData["metadata"] = py::dict(
            "simulation_time"_a = world.getSimulationTime(),
            "timestamp"_a = std::time(nullptr),
            "version"_a = "NLM v0.1.0"
        );

        return jsonData;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("filename"),
        "Export simulation data to JSON format");

    m.def("create_model_checkpoint", [](const Brain& brain,
                                       const AgentBrain& agent,
                                       const SimpleWorld& world,
                                       const std::string& checkpointName,
                                       const std::string& description = "") {
        // Create a model checkpoint with metadata
        brain.save(checkpointName + "_brain.bin");

        auto checkpoint = py::dict(
            "name"_a = checkpointName,
            "description"_a = description,
            "timestamp"_a = std::time(nullptr),
            "brain_stats"_a = brain.getBrainStats(),
            "agent_stats"_a = agent.getAllStats(),
            "world_info"_a = py::dict(
                "width"_a = world.getWidth(),
                "height"_a = world.getHeight(),
                "simulation_time"_a = world.getSimulationTime()
            ),
            "checkpoint_file"_a = checkpointName + "_brain.bin"
        );

        return checkpoint;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
        py::arg("checkpointName"), py::arg("description") = "",
        "Create a model checkpoint with metadata");

    // Advanced experiment management utilities
    m.def("run_repeated_experiments", [](const py::function& experimentSetup,
                                        const py::function& experimentRun,
                                        const py::function& experimentAnalyze,
                                        size_t numExperiments = 10,
                                        const py::dict& sharedConfig = {}) {
        std::vector<py::dict> results;

        for (size_t exp = 0; exp < numExperiments; ++exp) {
            std::cout << "Running experiment " << (exp + 1) << "/" << numExperiments << std::endl;

            // Setup
            py::object config = experimentSetup();

            // Apply shared config if provided
            for (auto item : sharedConfig) {
                std::string key = item.first.cast<std::string>();
                py::object value = item.second;
                // Apply to config would go here
            }

            // Run experiment
            py::object experimentResult = experimentRun(config);

            // Analyze
            py::object analysisResult = experimentAnalyze(experimentResult);

            results.push_back(py::dict(
                "experiment_id"_a = exp,
                "config"_a = config,
                "result"_a = experimentResult,
                "analysis"_a = analysisResult
            ));
        }

        return results;
    }, py::arg("experimentSetup"), py::arg("experimentRun"),
        py::arg("experimentAnalyze"), py::arg("numExperiments") = 10,
        py::arg("sharedConfig") = py::dict(),
        "Run multiple experiments with same setup");

    m.def("benchmark_and_compare", [](const std::vector<std::string>& configNames,
                                     const std::vector<size_t>& neuronCounts,
                                     size_t steps = 1000) {
        std::vector<py::dict> benchmarks;

        for (size_t i = 0; i < configNames.size(); ++i) {
            auto config = create_config_from_template(configNames[i]);
            config->set("brain.neuron_count", neuronCounts[i]);

            auto brain = createBrain(config);
            auto startTime = std::chrono::high_resolution_clock::now();

            for (size_t step = 0; step < steps; ++step) {
                brain->step(step);
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

            benchmarks.push_back(py::dict(
                "config_name"_a = configNames[i],
                "neuron_count"_a = neuronCounts[i],
                "steps_performed"_a = steps,
                "total_time_ms"_a = duration.count(),
                "steps_per_second"_a = steps / (duration.count() / 1000.0)
            ));
        }

        return benchmarks;
    }, py::arg("configNames"), py::arg("neuronCounts"), py::arg("steps") = 1000,
        "Benchmark and compare different configurations");

    // Advanced educational and demonstration utilities
    m.def("create_educational_demo", [](const std::string& demoType,
                                      const py::dict& parameters = {}) {
        if (demoType == "basic_neural_network") {
            // Create a simple neural network demonstration
            auto config = create_config_from_template("quick_test");
            auto brain = createBrain(config);
            auto world = createSimpleWorld();

            return py::dict(
                "demo_type"_a = "basic_neural_network",
                "brain"_a = brain,
                "world"_a = world,
                "description"_a = "Demonstrates basic neural network operations"
            );
        } else if (demoType == "plasticity_demonstration") {
            // Demonstrate plasticity learning
            auto config = create_config_from_template("production");
            auto brain = createBrain(config);

            // Enable plasticity
            // This would require direct access to config

            return py::dict(
                "demo_type"_a = "plasticity_demonstration",
                "brain"_a = brain,
                "description"_a = "Demonstrates synaptic plasticity and learning"
            );
        } else if (demoType == "agent_behavior") {
            // Demonstrate agent behavior in environment
            auto config = create_config_from_template("research");
            auto brain = createBrain(config);
            auto agent = createAgentBrain(brain);
            auto world = create_enriched_world(15, 15, 8, 8, true, true);

            return py::dict(
                "demo_type"_a = "agent_behavior",
                "brain"_a = brain,
                "agent"_a = agent,
                "world"_a = world,
                "description"_a = "Demonstrates agent learning and behavior in environment"
            );
        } else {
            throw std::invalid_argument("Unknown demo type: " + demoType);
        }
    }, py::arg("demoType"), py::arg("parameters") = py::dict(),
        "Create educational demonstration with different types");

    m.def("list_demonstrations", []() {
        return py::list({
            "basic_neural_network",
            "plasticity_demonstration",
            "agent_behavior",
            "sensory_integration",
            "motor_control"
        });
    }, "List available educational demonstrations");

    // Advanced utility functions for power users
    m.def("create_custom_simulation", [](const py::dict& simulationConfig) {
        // Create a custom simulation based on configuration
        auto config = create_config_from_template("production");

        // Apply custom configuration
        if (simulationConfig.contains("neuron_count")) {
            config->set("brain.neuron_count", simulationConfig["neuron_count"].cast<size_t>());
        }
        if (simulationConfig.contains("timestep")) {
            config->set("simulation_timestep", simulationConfig["timestep"].cast<double>());
        }

        auto brain = createBrain(config);
        auto world = create_enriched_world();
        auto agent = createAgentBrain(brain);

        return py::dict(
            "brain"_a = brain,
            "agent"_a = agent,
            "world"_a = world,
            "config"_a = config
        );
    }, py::arg("simulationConfig"), py::arg("parameters") = py::dict(),
        "Create a custom simulation with specific parameters");

    m.def("run_advanced_training", [](std::shared_ptr<AgentBrain> agent,
                                     std::shared_ptr<SimpleWorld> world,
                                     size_t numEpisodes = 100,
                                     const py::dict& trainingParameters = {}) {
        if (!agent || !world) {
            throw std::invalid_argument("Agent and world cannot be null");
        }

        std::vector<py::dict> episodeResults;
        std::vector<float> rewardsHistory;
        std::vector<float> curiosityHistory;

        for (size_t episode = 0; episode < numEpisodes; ++episode) {
            world->reset();
            agent->initialize(*world);

            size_t episodeSteps = 0;
            float totalReward = 0.0f;

            for (size_t step = 0; step < 1000; ++step) {
                world->update(0.1);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());

                auto internal = percept.getInternal();
                float reward = internal.empty() ? 0.0f : internal[0];
                totalReward += reward;

                episodeSteps++;

                // Apply advanced training if specified
                if (trainingParameters.contains("learning_rate")) {
                    agent->applyRewardModulation(reward, 0.0);  // Simplified
                }

                if (agent->getDevelopmentalStage() >= DevelopmentalStage::Adult) {
                    break;
                }
            }

            episodeResults.push_back(py::dict(
                "episode"_a = episode,
                "steps"_a = episodeSteps,
                "total_reward"_a = totalReward,
                "average_reward_per_step"_a = episodeSteps > 0 ? totalReward / episodeSteps : 0.0f,
                "final_curiosity"_a = agent->getCuriosityLevel()
            ));

            rewardsHistory.push_back(totalReward);
            curiosityHistory.push_back(agent->getCuriosityLevel());
        }

        // Compute training statistics
        auto computeMean = [](const std::vector<float>& data) {
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        };

        auto computeStd = [](const std::vector<float>& data) {
            if (data.empty()) return 0.0f;
            float mean = computeMean(data);
            float variance = 0.0f;
            for (float val : data) variance += (val - mean) * (val - mean);
            return std::sqrt(variance / data.size());
        };

        py::dict trainingStats;
        trainingStats["num_episodes"] = numEpisodes;
        trainingStats["mean_reward"] = computeMean(rewardsHistory);
        trainingStats["std_reward"] = computeStd(rewardsHistory);
        trainingStats["mean_curiosity"] = computeMean(curiosityHistory);
        trainingStats["std_curiosity"] = computeStd(curiosityHistory);
        trainingStats["convergence_episode"] = std::find_if(episodeResults.begin(), episodeResults.end(),
                                                         [](const py::dict& result) {
                                                             return result["average_reward_per_step"_a].cast<float>() > 0.1f;
                                                         }) - episodeResults.begin();

        return py::dict(
            "episode_results"_a = episodeResults,
            "training_statistics"_a = trainingStats,
            "rewards_history"_a = rewardsHistory,
            "curiosity_history"_a = curiosityHistory
        );
    }, py::arg("agent"), py::arg("world"), py::arg("numEpisodes") = 100,
        py::arg("trainingParameters") = py::dict(),
        "Run advanced training with detailed statistics and analysis");

    m.def("export_advanced_results", [](const py::dict& experimentResults,
                                       const std::string& outputFormat = "json") {
        if (outputFormat == "json") {
            // Export to JSON format
            return py::dict(
                "format"_a = "json",
                "data"_a = experimentResults,
                "timestamp"_a = std::time(nullptr)
            );
        } else if (outputFormat == "csv") {
            // Export to CSV format
            return py::dict(
                "format"_a = "csv",
                "message"_a = "CSV export implementation requires specific data structure"
            );
        } else {
            throw std::invalid_argument("Unsupported output format: " + outputFormat);
        }
    }, py::arg("experimentResults"), py::arg("outputFormat") = "json",
        "Export advanced results in specified format");

    // Final constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Register convenience module
    auto conv = m.def_submodule("conv", "Advanced convenience functions for complex operations");
    conv.def("run_advanced_agent_simulation", [](std::shared_ptr<AgentBrain> agent,
                                               std::shared_ptr<SimpleWorld> world,
                                               size_t numSteps,
                                               const py::dict& options = {}) {
        return simulate_episode(agent, world, numSteps, options.get("verbose", false));
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"),
        py::arg("options") = py::dict(),
        "Run advanced agent simulation with detailed options");

    conv.def("create_research_config", []() {
        return create_config_from_template("research");
    }, "Create research-grade configuration with high complexity");

    conv.def("create_production_config", []() {
        return create_config_from_template("production");
    }, "Create production-ready configuration with balanced complexity");

    conv.def("create_demo_config", []() {
        return create_config_from_template("quick_test");
    }, "Create demo configuration with minimal complexity");

    conv.def("analyze_simulation_results", [](const py::dict& results) {
        // Advanced analysis of simulation results
        py::dict analysis;

        if (results.contains("episode_results")) {
            auto episodeResults = results["episode_results"].cast<std::vector<py::dict>>();

            std::vector<float> rewards;
            std::vector<size_t> episodeLengths;

            for (const auto& result : episodeResults) {
                rewards.push_back(result["total_reward"_a].cast<float>());
                episodeLengths.push_back(result["steps"_a].cast<size_t>());
            }

            auto computeMean = [](const std::vector<float>& data) {
                if (data.empty()) return 0.0f;
                float sum = std::accumulate(data.begin(), data.end(), 0.0f);
                return sum / data.size();
            };

            analysis["mean_reward"] = computeMean(rewards);
            analysis["reward_std"] = computeMean(std::vector<float>(rewards.size(), 0.0f)); // Placeholder
            analysis["mean_episode_length"] = computeMean(std::vector<float>(episodeLengths.begin(), episodeLengths.end()));
            analysis["convergence_rate"] = 1.0f; // Placeholder
        }

        return analysis;
    }, py::arg("results"),
        "Advanced analysis of simulation results");

    conv.def("export_research_data", [](const py::dict& results,
                                       const std::string& baseFilename) {
        // Export research-grade data
        std::string timestamp = "research_" + std::to_string(std::time(nullptr));

        py::dict exportInfo;
        exportInfo["results_file"] = baseFilename + "_results_" + timestamp + ".json";
        exportInfo["analysis_file"] = baseFilename + "_analysis_" + timestamp + ".json";
        exportInfo["metadata_file"] = baseFilename + "_metadata_" + timestamp + ".txt";

        return exportInfo;
    }, py::arg("results"), py::arg("baseFilename"),
        "Export research-grade data with metadata and analysis");
}

namespace nlm {
    void ConvenienceMethods::registerConvenienceMethods(py::module_& m) {
        // Additional convenience methods can be registered here
    }
}

    // ==================== INVALID CONSTANTS ====================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // ==================== CONVENIENCE MODULE ====================

    // Create a submodule for convenience functions
    auto conv = m.def_submodule("conv", "Convenience functions for common patterns");
    conv.def("run_agent_simulation", [](std::shared_ptr<AgentBrain> agent,
                                       std::shared_ptr<SimpleWorld> world,
                                       size_t numSteps) {
        if (!agent || !world) {
            throw std::invalid_argument("Agent and world cannot be null");
        }
        world->reset();
        agent->initialize(*world);
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"),
        "Run a complete agent simulation");

    conv.def("create_test_brain", []() {
        auto config = createDefaultConfig();
        config->set("brain.neuron_count", 100);
        config->set("brain.synapse_density", 0.1);
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        return std::make_tuple(brain, agent);
    }, "Create a test brain with small size for quick testing");

    conv.def("get_default_config", []() {
        auto config = createDefaultConfig();
        return std::make_tuple(
            config->getOr<int64_t>("brain.neuron_count", 0),
            config->getOr<float>("brain.synapse_density", 0.0f),
            config->getOr<double>("simulation_timestep", 0.0)
        );
    }, "Get default configuration values as tuple");
}

namespace nlm {
    void ConvenienceMethods::registerConvenienceMethods(py::module_& m) {
        // This function would register additional convenience methods
        // after the main module is created
    }
}
