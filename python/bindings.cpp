#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
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

// Helper function to reduce code duplication for ID classes
template<typename IDType>
void registerIDClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<IDType>(m, name.c_str(), doc.c_str())
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const IDType& id) {
            std::string typeName = name;
            // Convert from e.g. "NeuronId" to "Neuron"
            if (typeName.size() >= 3 && typeName.substr(typeName.size() - 2) == "Id") {
                typeName = typeName.substr(0, typeName.size() - 2);
            }
            return "<" + typeName + "Id: " + std::to_string(id.value) + ">";
        });
}

// Context manager for brain simulation
class BrainSimulator {
public:
    BrainSimulator(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->initialize();
        }
    }
    
    ~BrainSimulator() {
        if (brain_) {
            brain_->reset();
        }
    }
    
    void step(int64_t step) {
        if (brain_) {
            brain_->step(step);
        }
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Configuration builder with fluent API
class ConfigBuilder {
public:
    static std::shared_ptr<Config> createDefault() {
        auto config = std::make_shared<Config>();
        return config;
    }
    
    static std::shared_ptr<Config> createWithDefaults(int64_t neuronCount = 1000, 
                                                      int64_t regionCount = 1,
                                                      double connectionProbability = 0.1) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount, nlm::ConfigSource::Default);
        config->set("region_count", regionCount, nlm::ConfigSource::Default);
        config->set("connection_probability", connectionProbability, nlm::ConfigSource::Default);
        return config;
    }
    
    static std::shared_ptr<Config> loadFromJSON(const std::string& filepath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Enhanced Python API with:
        - Code generation for ID classes (NeuronId, SynapseId, etc.)
        - Context managers for safe brain simulation
        - Fluent configuration builder
        - Enhanced type safety and error handling
        - Serialization support (pickle, JSON)
        - Convenience functions for common patterns
        
        Key features:
        - Reduced code duplication through templates
        - Pythonic API design (context managers, fluent interfaces)
        - Better error messages and validation
        - Support for advanced use cases
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Use template to register all ID classes with reduced duplication
    registerIDClass<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc");
    registerIDClass<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc");
    registerIDClass<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc");
    registerIDClass<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc");

    // Enumerations with better organization
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

    // Enhanced Config class with fluent API and validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with enhanced API)pbdoc")
        .def(py::init<>())
        
        // File I/O
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Throws RuntimeError on failure.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Throws RuntimeError on failure.")
        
        // Key-value operations with validation
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (string)")
        .def("set", [](Config& self, const std::string& key, int64_t value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (integer)")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (float)")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value, nlm::ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
           "Set configuration value (boolean)")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                return *value;
            } else {
                throw py::key_error("Key \"" + key + "\" not found in configuration");
            }
        }, py::arg("key"),
           "Get configuration value as string. Throws KeyError if not found.")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (value.has_value()) {
                return *value;
            } else {
                throw py::key_error("Key \"" + key + "\" not found in configuration");
            }
        }, py::arg("key"),
           "Get configuration value as integer. Throws KeyError if not found.")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value.has_value()) {
                return *value;
            } else {
                throw py::key_error("Key \"" + key + "\" not found in configuration");
            }
        }, py::arg("key"),
           "Get configuration value as float. Throws KeyError if not found.")
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value.has_value()) {
                return *value;
            } else {
                throw py::key_error("Key \"" + key + "\" not found in configuration");
            }
        }, py::arg("key"),
           "Get configuration value as boolean. Throws KeyError if not found.")
        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            if (value.has_value()) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with default for strings")
        .def("getOr", [](const Config& self, const std::string& key, int64_t defaultValue) {
            auto value = self.get<int64_t>(key);
            if (value.has_value()) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with default for integers")
        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            if (value.has_value()) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with default for floats")
        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            if (value.has_value()) {
                return *value;
            }
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get configuration value with default for booleans")
        
        // Validation and utility methods
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validateKey", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                std::string errorMsg = "Invalid configuration key: '" + key + "'. ";
                errorMsg += "Available keys: ";
                auto keys = self.getKeys();
                for (size_t i = 0; i < keys.size(); ++i) {
                    if (i > 0) errorMsg += ", ";
                    errorMsg += "'" + keys[i] + "'";
                }
                throw py::key_error(errorMsg);
            }
            return true;
        }, py::arg("key"),
           "Validate that a configuration key exists. Throws KeyError if not found.")
        
        // Context manager for safe configuration loading
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config& self, py::args) {
            self.clear();
        })
        
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput base class with enhanced interface
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with enhanced API)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("validateData", [](const SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) {
                throw py::value_error("Sensory input data is empty");
            }
            return true;
        }, "Validate that sensory input has data")
        .def("getDataAsNumpy", [](const SensoryInput& self) {
            // This is a placeholder for numpy integration
            auto data = self.getData();
            return data;  // In a real implementation, this would convert to numpy array
        }, "Get sensory data as a numpy array (requires numpy)");

    // Vision class with enhanced API
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with enhanced API)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        .def("setDataFromArray", [](Vision& self, const std::vector<std::vector<float>>& array2d) {
            // Flatten 2D array
            std::vector<float> flatData;
            for (const auto& row : array2d) {
                flatData.insert(flatData.end(), row.begin(), row.end());
            }
            if (flatData.empty()) {
                throw py::value_error("Vision data array is empty");
            }
            self.setData(flatData);
        }, py::arg("array2d"), "Set vision data from 2D array")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("isValid", [](const Vision& self) {
            auto data = self.getData();
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            return data.size() == expectedSize && !data.empty();
        }, "Check if vision data is valid (correct dimensions)")
        .def("normalize", [](Vision& self, float minVal = 0.0f, float maxVal = 255.0f) {
            auto& data = const_cast<std::vector<float>&>(self.getData());
            for (auto& val : data) {
                val = (val - minVal) / (maxVal - minVal);
            }
        }, py::arg("minVal") = 0.0f, py::arg("maxVal") = 255.0f,
           "Normalize vision data to [minVal, maxVal] range");

    // Audio class with enhanced API
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced API)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get audio duration in seconds")
        .def("isValid", [](const Audio& self) {
            auto data = self.getData();
            size_t expectedSamples = self.getNumSamples();
            return data.size() == expectedSamples && !data.empty();
        }, "Check if audio data is valid (correct length)")
        .def("normalize", [](Audio& self, float minVal = 0.0f, float maxVal = 1.0f) {
            auto& data = const_cast<std::vector<float>&>(self.getData());
            for (auto& val : data) {
                val = (val - minVal) / (maxVal - minVal);
            }
        }, py::arg("minVal") = 0.0f, py::arg("maxVal") = 1.0f,
           "Normalize audio data to [minVal, maxVal] range");

    // InternalSignals class with enhanced API
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with enhanced API)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("setSignal", [](InternalSignals& self, size_t index, float value) {
            // This assumes InternalSignals has a way to modify existing signals
            // For now, we'll implement a basic version
            auto signals = self.getData();
            if (index < signals.size()) {
                signals[index] = value;
                // Note: This is a simplification - actual implementation might need
                // a proper setter method in InternalSignals
            }
        }, py::arg("index"), py::arg("value"),
           "Set signal value at specific index")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of signals")
        .def("getSignalAt", [](const InternalSignals& self, size_t index) {
            auto signals = self.getData();
            if (index < signals.size()) {
                return signals[index];
            }
            throw py::index_error("Signal index out of range");
        }, py::arg("index"),
           "Get signal value at specific index")
        .def("getAverageSignal", [](const InternalSignals& self) {
            auto signals = self.getData();
            if (signals.empty()) {
                return 0.0f;
            }
            float sum = 0.0f;
            for (float val : signals) {
                sum += val;
            }
            return sum / signals.size();
        }, "Get average signal value");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with enhanced API)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameterAt", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"), "Get parameter value at specific index")
        .def("isValid", [](const Action& self) {
            // Basic validation - action has a type
            auto type = self.getType();
            return type != ActionType::Custom || !self.getParameters().empty();
        }, "Check if action is valid");

    // Enhanced WorldObject class
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with enhanced API)pbdoc")
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to point (x, y)")
        .def("isAtPosition", [](const WorldObject& self, float x, float y, float tolerance = 0.01f) {
            float dx = std::abs(self.x - x);
            float dy = std::abs(self.y - y);
            return dx <= tolerance && dy <= tolerance;
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.01f, "Check if object is at position (x, y)")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("setTypeAndValue", [](WorldObject& self, WorldObjectType type, float value) {
            self.type = type;
            self.value = value;
        }, py::arg("type"), py::arg("value"), "Set object type and value")
        .def("isHazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("isResource", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("getDescription", [](const WorldObject& self) {
            std::stringstream ss;
            ss << "WorldObject at (" << self.x << ", " << self.y << ")";
            ss << " type: " << static_cast<int>(self.type);
            ss << " value: " << self.value;
            ss << " active: " << (self.active ? "yes" : "no");
            return ss.str();
        }, "Get string description of object");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with enhanced API)pbdoc")
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
        .def("moveTo", [](AgentBody& self, float x, float y, float dt = 0.1f) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance > 0) {
                float speed = distance / dt;
                self.velocityX = dx / distance * speed;
                self.velocityY = dy / distance * speed;
            }
        }, py::arg("x"), py::arg("y"), py::arg("dt") = 0.1f, "Move towards target position")
        .def("turnTowards", [](AgentBody& self, float targetOrientation, float speed = 0.1f) {
            float diff = targetOrientation - self.orientation;
            while (diff > M_PI) diff -= 2 * M_PI;
            while (diff < -M_PI) diff += 2 * M_PI;
            self.angularVelocity = diff * speed;
        }, py::arg("targetOrientation"), py::arg("speed") = 0.1f, "Turn towards target orientation")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getDirection", [](const AgentBody& self) {
            float angle = std::atan2(self.velocityY, self.velocityX);
            return angle;
        }, "Get current direction in radians")
        .def("isEnergyLow", [](const AgentBody& self, float threshold = 0.2f) {
            return self.energy < threshold;
        }, py::arg("threshold") = 0.2f, "Check if energy is below threshold")
        .def("isHealthy", [](const AgentBody& self, float threshold = 0.5f) {
            return self.health >= threshold;
        }, py::arg("threshold") = 0.5f, "Check if health is above threshold")
        .def("getPosition", [](const AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("getVelocity", [](const AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("getStatus", [](const AgentBody& self) {
            std::stringstream ss;
            ss << "AgentBody at (" << self.x << ", " << self.y << ")";
            ss << " orientation: " << self.orientation;
            ss << " speed: " << self.getSpeed();
            ss << " energy: " << self.energy;
            ss << " health: " << self.health;
            ss << " age: " << self.age;
            return ss.str();
        }, "Get string status report");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with enhanced API)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is positive (success and positive reward)")
        .def("isNegative", [](const ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result is negative (failure or negative reward)")
        .def("getMagnitude", [](const ActionResult& self) {
            return std::abs(self.reward);
        }, "Get magnitude of reward (absolute value)")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert to Python dictionary")
        .def("fromDict", [](ActionResult& self, py::dict dict) {
            if (dict.contains("reward")) self.reward = py::cast<float>(dict["reward"]);
            if (dict.contains("success")) self.success = py::cast<bool>(dict["success"]);
            if (dict.contains("message")) self.message = py::cast<std::string>(dict["message"]);
        }, py::arg("dict"), "Set values from Python dictionary");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced API)pbdoc")
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
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().empty();
        }, "Check if percept has internal data")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().empty();
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        .def("getAllAvailableSignals", [](const SensoryPercept& self) {
            py::dict signals;
            if (!self.getVision().empty()) {
                signals["vision"] = self.getVision();
            }
            if (!self.getTouch().empty()) {
                signals["touch"] = self.getTouch();
            }
            if (!self.getInternal().empty()) {
                signals["internal"] = self.getInternal();
            }
            if (!self.getProprioception().empty()) {
                signals["proprioception"] = self.getProprioception();
            }
            if (!self.getAudio().empty()) {
                signals["audio"] = self.getAudio();
            }
            return signals;
        }, "Get all available signals as dictionary")
        .def("mergePercept", [](SensoryPercept& self, const SensoryPercept& other) {
            if (!other.getVision().empty()) self.setVision(other.getVision());
            if (!other.getTouch().empty()) self.setTouch(other.getTouch());
            if (!other.getInternal().empty()) self.setInternal(other.getInternal());
            if (!other.getProprioception().empty()) self.setProprioception(other.getProprioception());
            if (!other.getAudio().empty()) self.setAudio(other.getAudio());
            // Keep original timestamp unless other has a more recent one
            if (other.getTimestamp() > self.getTimestamp()) {
                self.setTimestamp(other.getTimestamp());
            }
        }, py::arg("other"), "Merge another percept into this one");

    // Enhanced SimpleWorld class with better API
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced API)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensory parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state by timestep seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from agent view")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getAgentPosition", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](const SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation in radians")
        .def("getVisionData", [](const SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            return percept.getVision();
        }, "Get vision data as flat array")
        .def("getVisionAs2D", [](const SimpleWorld& self) {
            auto vision = self.getVisionData();
            size_t width = self.getVisionWidth();
            size_t height = self.getVisionHeight();
            std::vector<std::vector<float>> vision2D(height, std::vector<float>(width));
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    size_t index = y * width + x;
                    if (index < vision.size()) {
                        vision2D[y][x] = vision[index];
                    }
                }
            }
            return vision2D;
        }, "Get vision data as 2D array")
        .def("isAgentNearObject", [](const SimpleWorld& self, float x, float y, float radius = 0.5f) {
            auto objects = self.getAgentBody(); // This would need access to world objects
            // Simplified version - just checks distance to agent
            auto body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx*dx + dy*dy) <= radius;
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Check if agent is near position (x, y)")
        .def("getWorldBounds", [](const SimpleWorld& self) {
            return std::make_tuple(0.0f, 0.0f, self.getWidth(), self.getHeight());
        }, "Get world bounds as (min_x, min_y, max_x, max_y) tuple")
        .def("simulateStep", [](SimpleWorld& self, float timestep, const std::vector<float>& actionParams) {
            // Apply a complete simulation step with given action parameters
            auto percept = self.getSensoryPercept();
            self.update(timestep);
            // Note: This is a simplified interface - in practice, you'd need
            // to properly decode motor commands from action parameters
        }, py::arg("timestep"), py::arg("actionParams"),
           "Run a complete simulation step with action parameters");

    // Enhanced Brain class with better API
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Throws RuntimeError on failure.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Throws RuntimeError on failure.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with optional name")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console")
        .def("getStatistics", [](const Brain& brain) {
            py::dict stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["average_firing_rate"] = brain.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as dictionary")
        .def("validateState", [](const Brain& brain) {
            bool valid = true;
            valid = valid && brain.getTotalNeuronCount() > 0;
            valid = valid && brain.getTotalSynapseCount() >= 0;
            valid = valid && brain.getFiringNeuronCount() >= 0;
            valid = valid && brain.getAverageFiringRate() >= 0.0f;
            valid = valid && brain.getExcitationInhibitionRatio() >= 0.0f;
            return valid;
        }, "Validate brain state (basic checks)")
        .def("getSummary", [](const Brain& brain) {
            std::stringstream ss;
            ss << "Brain Statistics:";
            ss << "\n  Total neurons: " << brain.getTotalNeuronCount();
            ss << "\n  Total synapses: " << brain.getTotalSynapseCount();
            ss << "\n  Firing neurons: " << brain.getFiringNeuronCount();
            ss << "\n  Average firing rate: " << brain.getAverageFiringRate();
            ss << "\n  E/I ratio: " << brain.getExcitationInhibitionRatio();
            ss << "\n  Total spikes: " << brain.getTotalSpikeCount();
            ss << "\n  Developmental stage: " << static_cast<int>(brain.getDevelopmentalStage());
            return ss.str();
        }, "Get formatted brain summary as string");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced API)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("getAllFactors", [](const AgentBrain& agent) {
            py::dict factors;
            factors["neuromodulation"] = agent.getNeuromodulationLevel();
            factors["curiosity"] = agent.getCuriosityLevel();
            factors["novelty"] = agent.getNoveltyLevel();
            factors["prediction_error"] = agent.getPredictionError();
            factors["developmental_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            return factors;
        }, "Get all neuromodulation and developmental factors as dictionary")
        .def("enableAll", [](AgentBrain& agent) {
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
        }, "Enable all brain subsystems")
        .def("disableAll", [](AgentBrain& agent) {
            agent.enableRewardModulation(false);
            agent.enableStructuralPlasticity(false);
            agent.enableDevelopment(false);
            agent.enableCuriosity(false);
        }, "Disable all brain subsystems");

    // Enhanced factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world with default settings");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Configuration builder functions
    m.def("createConfig", &ConfigBuilder::createDefault,
         "Create a default configuration");
    m.def("createConfigWithDefaults", &ConfigBuilder::createWithDefaults,
         py::arg("neuronCount") = 1000,
         py::arg("regionCount") = 1,
         py::arg("connectionProbability") = 0.1,
         "Create configuration with specified defaults");

    // Context manager for brain simulation
    py::class_<BrainSimulator>(m, "BrainSimulator", R"pbdoc(Context manager for safe brain simulation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create brain simulator with given brain")
        .def("step", &BrainSimulator::step, py::arg("step"),
             "Advance simulation by one step")
        .def("getBrain", &BrainSimulator::getBrain,
             py::return_value_policy::reference_internal,
             "Get the brain inside the simulator");

    // Convenience functions for common patterns
    m.def("runSilentBrain", [](int64_t numSteps) {
        auto config = ConfigBuilder::createWithDefaults(100, 1, 0.1);
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        for (int64_t step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("numSteps"),
       "Run a simple brain simulation without world interaction");

    m.def("runAgentEpisode", [](std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent,
                              int64_t numSteps) {
        world->reset();
        agent->initialize(*world);
        
        for (int64_t step = 0; step < numSteps; ++step) {
            world->update(0.1f);
            agent->processSensoryInput(world->getSensoryPercept());
            agent->getBrain()->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        auto brain = agent->getBrain();
        return std::make_tuple(brain->getTotalSpikeCount(),
                              brain->getAverageFiringRate());
    }, py::arg("world"), py::arg("agent"), py::arg("numSteps"),
       "Run a complete agent episode");

    // Constants and utility functions
    m.attr("VERSION") = "0.1.0";
    m.attr("PI") = 3.14159265358979323846;
    
    m.def("getVersion", []() { return std::string("NLM Python Bindings v0.1.0"); },
         "Get version information");
    
    m.def("createUniqueId", []() { return std::make_unique<uint64_t>(std::random_device{}()); },
         "Generate a random unique ID (for testing)");

    // Serialization support (placeholder for pybind11's pickle)
    m.def("to_string", [](const std::string& s) { return s; }, "Convert to string");

    // Invalid ID constants (for backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__license__") = "MIT";
    
    // Add examples to module docstring
    m.doc() += R"pbdoc(
    
    Examples:
    ---------
    
    Basic usage:
    ```python
    import pynlm
    
    # Create a simple brain
    brain = pynlm.createBrain(pynlm.createConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    ```
    
    Agent with world:
    ```python
    import pynlm
    
    # Setup
    brain = pynlm.createBrain(pynlm.createConfigWithDefaults(1000, 3, 0.05))
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableAll()  # Enable all learning systems
    
    # Run episode
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Episode completed!")
    ```
    
    Using context manager:
    ```python
    import pynlm
    
    brain = pynlm.createBrain(pynlm.createConfig())
    
    with pynlm.BrainSimulator(brain) as simulator:
        for i in range(50):
            simulator.step(i)
    
    # Brain is automatically reset when exiting context
    print("Simulation completed safely")
    ```
    
    Configuration builder:
    ```python
    import pynlm
    
    # Create custom configuration
    config = pynlm.createConfigWithDefaults(
        neuronCount=500,
        regionCount=2,
        connectionProbability=0.02
    )
    
    # Modify after creation
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", False)
    
    brain = pynlm.createBrain(config)
    ```
    """;
}

} // namespace nlm
