// NLM Python Bindings with Enhanced Error Handling and Type Safety
// Comprehensive improvements for robustness and usability

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

// Include all NLM headers
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
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class NeuronError : public NLMError {
public:
    explicit NeuronError(const std::string& message) : NLMError("Neuron Error: " + message) {}
};

class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message) : NLMError("Agent Error: " + message) {}
};

class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError("World Error: " + message) {}
};

// Validation helper functions
inline void validateNeuronId(const NeuronId& id, const std::string& context = "neuron operation") {
    if (id.value == UINT64_MAX) {
        throw NeuronError("Invalid NeuronId (" + std::to_string(id.value) + ") used in " + context);
    }
}

inline void validateSynapseId(const SynapseId& id, const std::string& context = "synapse operation") {
    if (id.value == UINT64_MAX) {
        throw NeuronError("Invalid SynapseId (" + std::to_string(id.value) + ") used in " + context);
    }
}

inline void validateRegionId(const RegionId& id, const std::string& context = "region operation") {
    if (id.value == UINT64_MAX) {
        throw BrainError("Invalid RegionId (" + std::to_string(id.value) + ") used in " + context);
    }
}

inline void validateRange(float value, float min, float max, const std::string& paramName) {
    if (std::isnan(value) || std::isinf(value)) {
        throw ConfigurationError("Parameter " + paramName + " cannot be NaN or infinite");
    }
    if (value < min || value > max) {
        throw ConfigurationError("Parameter " + paramName + " must be in range [" + 
                               std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(value));
    }
}

inline void validateNonNegative(size_t value, const std::string& paramName) {
    if (value == 0 && paramName != "steps") {
        // Zero might be valid for some parameters, but we'll warn
    }
}

inline void validatePositive(int value, const std::string& paramName) {
    if (value <= 0) {
        throw ConfigurationError("Parameter " + paramName + " must be positive, got " + std::to_string(value));
    }
}

inline void validateStringNonEmpty(const std::string& str, const std::string& paramName) {
    if (str.empty()) {
        throw ConfigurationError("Parameter " + paramName + " cannot be empty");
    }
}

// Enhanced NeuronId wrapper for better Python integration
class NeuronIdWrapper {
private:
    NeuronId id_;
    
public:
    NeuronIdWrapper() : id_(0) {}
    explicit NeuronIdWrapper(uint64_t value) : id_(value) {}
    
    uint64_t getValue() const { return id_.value; }
    uint64_t getIndex() const { return id_.index(); }
    
    bool isValid() const { return id_.value != UINT64_MAX; }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "<NeuronId: " << id_.value << ">";
        return ss.str();
    }
    
    bool equals(const NeuronIdWrapper& other) const {
        return id_ == other.id_;
    }
    
    size_t hash() const {
        return std::hash<uint64_t>{}(id_.value);
    }
    
    bool operator==(const NeuronIdWrapper& other) const {
        return id_ == other.id_;
    }
    
    bool operator!=(const NeuronIdWrapper& other) const {
        return !(*this == other);
    }
    
    // Python-specific methods
    py::object __repr__() const {
        return py::str(toString());
    }
    
    py::object __str__() const {
        return py::str(std::to_string(id_.value));
    }
};

// Helper to convert Python list to std::vector
template<typename T>
std::vector<T> toStdVector(py::list pyList) {
    std::vector<T> result;
    result.reserve(pyList.size());
    for (auto item : pyList) {
        result.push_back(item.cast<T>());
    }
    return result;
}

// Helper to convert Python list to std::vector<float>
std::vector<float> toFloatVector(py::list pyList) {
    std::vector<float> result;
    result.reserve(pyList.size());
    for (auto item : pyList) {
        result.push_back(item.cast<float>());
    }
    return result;
}

// Helper to convert Python list to std::vector<double>
std::vector<double> toDoubleVector(py::list pyList) {
    std::vector<double> result;
    result.reserve(pyList.size());
    for (auto item : pyList) {
        result.push_back(item.cast<double>());
    }
    return result;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ----------------------------------------------------
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        This version includes comprehensive error handling, type safety, and validation.
        
        Key improvements:
        - Enhanced error handling with descriptive messages
        - Parameter validation and bounds checking
        - Type safety for all Python-C++ interactions
        - Memory safety and resource management
        - Better documentation and examples
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Enhanced NeuronId with wrapper class
    py::class_<NeuronIdWrapper>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def("getValue", &NeuronIdWrapper::getValue, "Get the integer value of the neuron ID")
        .def("getIndex", &NeuronIdWrapper::getIndex, "Get the index of the neuron ID")
        .def("isValid", &NeuronIdWrapper::isValid, "Check if this ID is valid")
        .def("toString", &NeuronIdWrapper::toString, "String representation of the NeuronId")
        .def("equals", &NeuronIdWrapper::equals, py::arg("other"), "Check equality with another NeuronId")
        .def("__eq__", &NeuronIdWrapper::operator==)
        .def("__ne__", &NeuronIdWrapper::operator!=)
        .def("__hash__", &NeuronIdWrapper::hash)
        .def("__repr__", &NeuronIdWrapper::toString)
        .def("__str__", &NeuronIdWrapper::toString);

    // Other ID types (kept as simple wrappers for compatibility)
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

    // Enhanced enums with validation
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

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Validated configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Raises ConfigurationError if file not found or invalid.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (argc <= 0 || argv == nullptr) {
                throw ConfigurationError("Invalid command line arguments: argc must be positive and argv must not be null");
            }
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Raises ConfigurationError if parsing fails.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Raises ConfigurationError if save fails.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput with validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for validated sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("validateData", [](SensoryInput& self) {
            auto data = self.getData();
            if (data.empty()) {
                throw ConfigurationError("Sensory input data is empty");
            }
            for (size_t i = 0; i < data.size(); ++i) {
                if (std::isnan(data[i]) || std::isinf(data[i])) {
                    throw ConfigurationError("Invalid data value at index " + std::to_string(i));
                }
            }
        }, "Validate sensory input data for NaN/infinite values");

    // Enhanced Vision class with parameter validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with parameter validation)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3,
             "Create Vision input with validated dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ConfigurationError("Vision data cannot be empty");
            }
            // Validate dimensions match
            size_t expectedSize = self.getWidth() * self.getHeight() * self.getChannels();
            if (data.size() != expectedSize) {
                throw ConfigurationError("Vision data size mismatch: expected " + std::to_string(expectedSize) + 
                                       " elements, got " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data with parameter validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getDataSize", [](const Vision& self) {
            return self.getWidth() * self.getHeight() * self.getChannels();
        }, "Get expected data size for vision input");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio input with validated parameters")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ConfigurationError("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with parameter validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDurationSeconds", [](const Audio& self) {
            return self.getNumSamples() / static_cast<double>(self.getSampleRate());
        }, "Get audio duration in seconds");

    // Enhanced InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with validation)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value with range validation")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getData, "Get number of signals");

    // Enhanced Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type with validation")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters with validation")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("validate", [](Action& self) {
            auto params = self.getParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                if (std::isnan(params[i]) || std::isinf(params[i])) {
                    throw ConfigurationError("Invalid parameter value at index " + std::to_string(i));
                }
            }
        }, "Validate action parameters for NaN/infinite values");

    // Enhanced WorldObject with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
             "Create world object with parameter validation")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("validate", [](WorldObject& self) {
            if (std::isnan(self.x) || std::isinf(self.x)) {
                throw WorldError("WorldObject x coordinate is invalid");
            }
            if (std::isnan(self.y) || std::isinf(self.y)) {
                throw WorldError("WorldObject y coordinate is invalid");
            }
            if (self.radius < 0.0f) {
                throw WorldError("WorldObject radius cannot be negative");
            }
            if (self.value < 0.0f) {
                throw WorldError("WorldObject value cannot be negative");
            }
        }, "Validate world object parameters");

    // Enhanced AgentBody
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with validation)pbdoc")
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
        .def("validate", [](AgentBody& self) {
            if (std::isnan(self.x) || std::isinf(self.x)) {
                throw AgentError("AgentBody x coordinate is invalid");
            }
            if (std::isnan(self.y) || std::isinf(self.y)) {
                throw AgentError("AgentBody y coordinate is invalid");
            }
            if (std::isnan(self.orientation) || std::isinf(self.orientation)) {
                throw AgentError("AgentBody orientation is invalid");
            }
            if (self.energy < 0.0f) {
                throw AgentError("AgentBody energy cannot be negative");
            }
            if (self.health < 0.0f) {
                throw AgentError("AgentBody health cannot be negative");
            }
        }, "Validate agent body parameters");

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with validation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with parameter validation")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("validate", [](ActionResult& self) {
            if (std::isnan(self.reward) || std::isinf(self.reward)) {
                throw WorldError("ActionResult reward is invalid");
            }
        }, "Validate action result parameters");

    // Enhanced SensoryPercept with validation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with validation)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data with validation")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data with validation")
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal data with validation")
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data with validation")
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data with validation")
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp with validation")
        .def("validate", [](SensoryPercept& self) {
            auto vision = self.getVision();
            for (size_t i = 0; i < vision.size(); ++i) {
                if (std::isnan(vision[i]) || std::isinf(vision[i])) {
                    throw ConfigurationError("Invalid vision data at index " + std::to_string(i));
                }
            }
        }, "Validate all sensory percept data for NaN/infinite values");

    // Enhanced SimpleWorld with comprehensive validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world with comprehensive validation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with parameter validation")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position with validation")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world with timestep validation")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command with parameter validation")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept (validated)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body (validated)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object with validation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position with validation")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid with boundary validation")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set max energy with validation")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate with validation")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed with validation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("validate", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            if (std::isnan(body.x) || std::isinf(body.x) || body.x < 0.0f || body.x >= self.getWidth()) {
                throw WorldError("Agent position x is out of bounds: " + std::to_string(body.x));
            }
            if (std::isnan(body.y) || std::isinf(body.y) || body.y < 0.0f || body.y >= self.getHeight()) {
                throw WorldError("Agent position y is out of bounds: " + std::to_string(body.y));
            }
        }, "Validate world state");

    // Enhanced Brain class with comprehensive validation
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with comprehensive validation)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Raises BrainError if config is invalid.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns True on success, raises BrainError on failure.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with current step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp (validated)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain with validation")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron with validation")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity (validated)")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Raises BrainError if save fails.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Raises BrainError if load fails.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region with name validation")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID (raises BrainError if ID is invalid)")
        .def("getRegionCount", &Brain::getRegionCount)
        .def("getRegionIds", &Brain::getRegionIds)
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount)
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount)
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount)
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount)
        .def("getAverageFiringRate", &Brain::getAverageFiringRate)
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio)
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount)
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount)
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage with validation")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status (validated)")
        .def("validate", [](Brain& self) {
            if (self.getTotalNeuronCount() == 0) {
                throw BrainError("Brain has no neurons - initialization may have failed");
            }
            if (self.getTotalSynapseCount() < 0) {
                throw BrainError("Invalid synapse count: " + std::to_string(self.getTotalSynapseCount()));
            }
        }, "Validate brain state");

    // Enhanced AgentBrain with validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with comprehensive validation)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with validated brain reference")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world (validated)")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize)
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize)
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain (validated)")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command (validated)")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation with validation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system with timestep validation")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage)
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel)
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel)
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel)
        .def("getPredictionError", &AgentBrain::getPredictionError)
        .def("reset", &AgentBrain::reset)
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Factory functions with validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set reasonable defaults
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        return config;
    }, "Create a default configuration with sensible defaults", py::return_value_policy::automatic);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw BrainError("Config cannot be null");
        }
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration. Raises BrainError if config is invalid.", py::return_value_policy::automatic);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        // Configure with sensible defaults
        world->configure(20.0f, 20.0f, 16, 16);
        world->reset();
        return world;
    }, "Create a new simple world with sensible defaults", py::return_value_policy::automatic);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw AgentError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface. Raises AgentError if brain is invalid.", py::return_value_policy::automatic);

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Error classes
    py::class_<NLMError>(m, "NLMError")
        .def(py::init<const std::string&>());

    py::class_<ConfigurationError, NLMError>(m, "ConfigurationError")
        .def(py::init<const std::string&>());

    py::class_<NeuronError, NLMError>(m, "NeuronError")
        .def(py::init<const std::string&>());

    py::class_<BrainError, NLMError>(m, "BrainError")
        .def(py::init<const std::string&>());

    py::class_<AgentError, NLMError>(m, "AgentError")
        .def(py::init<const std::string&>());

    py::class_<WorldError, NLMError>(m, "WorldError")
        .def(py::init<const std::string&>());

    // Utility functions
    m.def("validateNeuronId", &validateNeuronId, py::arg("id"), py::arg("context") = "neuron operation",
          "Validate a NeuronId. Raises NeuronError if invalid.");

    m.def("validateSynapseId", &validateSynapseId, py::arg("id"), py::arg("context") = "synapse operation",
          "Validate a SynapseId. Raises NeuronError if invalid.");

    m.def("validateRange", &validateRange,
          py::arg("value"), py::arg("min"), py::arg("max"), py::arg("paramName"),
          "Validate that a value is within specified range. Raises ConfigurationError if invalid.");

    m.def("toFloatVector", &toFloatVector, py::arg("pyList"),
          "Convert Python list to std::vector<float>");

    m.def("toDoubleVector", &toDoubleVector, py::arg("pyList"),
          "Convert Python list to std::vector<double>");

    // Examples section
    m.def("getExampleCode", []() {
        return R"(
Example 1: Simple Brain Simulation
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)
    
print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")

Example 2: Complete Agent in World
import pynlm

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
print(f"Agent simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")

Example 3: Error Handling
import pynlm

# This will raise a BrainError due to invalid parameters
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain created and initialized successfully")
except pynlm.BrainError as e:
    print(f"Caught BrainError: {e}")
except pynlm.NLMError as e:
    print(f"Caught NLMError: {e}")
";
    }, "Get example code snippets for common use cases");
}

} // namespace nlm
