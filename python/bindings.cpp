#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>
#include <sstream>
#include <typeinfo>
#include <cassert>

// Custom exception classes for Python bindings
namespace nlm {

class NLMException : public std::exception {
protected:
    std::string message_;
    std::string context_;
    
public:
    explicit NLMException(const std::string& message) : message_(message) {}
    
    NLMException(const std::string& message, const std::string& context) 
        : message_(message), context_(context) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const std::string& message() const { return message_; }
    const std::string& context() const { return context_; }
    
    std::string full_message() const {
        if (context_.empty()) {
            return message_;
        }
        return context_ + ": " + message_;
    }
};

class InvalidParameterError : public NLMException {
public:
    explicit InvalidParameterError(const std::string& param_name, 
                                 const std::string& message,
                                 const std::string& context = "")
        : NLMException("Invalid parameter '" + param_name + "': " + message, context) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message, const std::string& context = "")
        : NLMException("Memory error: " + message, context) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& message, const std::string& context = "")
        : NLMException("Configuration error: " + message, context) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message, const std::string& context = "")
        : NLMException("Simulation error: " + message, context) {}
};

class BindingError : public NLMException {
public:
    explicit BindingError(const std::string& message, const std::string& context = "")
        : NLMException("Binding error: " + message, context) {}
};

// Type alias for Python exceptions
namespace py = pybind11;

// Parameter validation functions
namespace validation {

// Basic type validation
template<typename T>
bool is_valid(const T& value) {
    return true;
}

bool is_valid(const std::string& value);
bool is_valid(const py::object& value);
bool is_valid(const char* value);
bool is_valid(double value);
bool is_valid(float value);
bool is_valid(int64_t value);
bool is_valid(uint64_t value);
bool is_valid(int32_t value);
bool is_valid(uint32_t value);
bool is_valid(int16_t value);
bool is_valid(uint16_t value);
bool is_valid(int8_t value);
bool is_valid(uint8_t value);
bool is_valid(bool value);

// Range validation for numeric types
template<typename T>
bool is_valid(const T& value, T min_val, T max_val) {
    return value >= min_val && value <= max_val;
}

// ID validation
bool is_valid(const NeuronId& id);
bool is_valid(const SynapseId& id);
bool is_valid(const RegionId& id);
bool is_valid(const PopulationId& id);

// Container validation
bool is_valid(const std::vector<float>& data);
bool is_valid(const std::vector<double>& data);
bool is_valid(const std::vector<std::string>& data);

// Enum validation
bool is_valid(NeuronType value);
bool is_valid(SynapseType value);
bool is_valid(DevelopmentalStage value);
bool is_valid(ActionType value);
bool is_valid(MotorCommand value);
bool is_valid(WorldObjectType value);
bool is_valid(FiringState value);

// Object reference validation
bool is_valid(const std::shared_ptr<void>& ptr);
bool is_valid(const std::shared_ptr<Brain>& ptr);
bool is_valid(const std::shared_ptr<Config>& ptr);

// Parameter validation with context
template<typename T>
T validate_param(const std::string& param_name, const T& value, 
                 T min_val = std::numeric_limits<T>::min(),
                 T max_val = std::numeric_limits<T>::max(),
                 const std::string& context = "") {
    if (!is_valid(value, min_val, max_val)) {
        std::ostringstream oss;
        oss << "Parameter " << param_name << " value " << value 
            << " is outside valid range [" << min_val << ", " << max_val << "]";
        throw InvalidParameterError(param_name, oss.str(), context);
    }
    return value;
}

template<typename T>
T validate_param(const std::string& param_name, const T& value,
                 bool (*validator)(const T&),
                 const std::string& context = "") {
    if (!validator(value)) {
        throw InvalidParameterError(param_name, "failed validation", context);
    }
    return value;
}

// Exception chaining utility
void chain_exception(const std::exception& original, const std::string& context);

// Memory safety check
void check_memory_safety(const void* ptr, const std::string& context = "");

// Smart pointer safety
void check_smart_pointer(const std::shared_ptr<void>& ptr, const std::string& context = "");

// Python object lifecycle tracking
void track_python_object(const py::object& obj, const std::string& context = "");

// Validation wrapper for binding functions
template<typename Func, typename... Args>
auto safe_bind(Func&& func, const std::string& context, Args&&... args) {
    try {
        return func(std::forward<Args>(args)...);
    } catch (const NLMException& e) {
        throw;
    } catch (const std::exception& e) {
        chain_exception(e, context);
        throw BindingError("Binding execution failed: " + std::string(e.what()), context);
    } catch (...) {
        throw BindingError("Unknown error in binding", context);
    }
}

} // namespace validation

// Macro for binding functions with validation
#define BIND_WITH_VALIDATION(func_name, context, ...) \
    [context](__VA_ARGS__) { \
        return validation::safe_bind(func_name, context, ##__VA_ARGS__); \
    }

// Validation helper functions implementation
namespace validation {

bool is_valid(const std::string& value) {
    return !value.empty() && value.size() < 1024; // Reasonable string length
}

bool is_valid(const py::object& value) {
    return !value.is_none();
}

bool is_valid(const char* value) {
    return value != nullptr && std::strlen(value) > 0;
}

bool is_valid(double value) {
    return !std::isnan(value) && !std::isinf(value);
}

bool is_valid(float value) {
    return !std::isnan(value) && !std::isinf(value);
}

bool is_valid(int64_t value) {
    return true;
}

bool is_valid(uint64_t value) {
    return value <= UINT64_MAX;
}

bool is_valid(int32_t value) {
    return true;
}

bool is_valid(uint32_t value) {
    return value <= UINT32_MAX;
}

bool is_valid(int16_t value) {
    return true;
}

bool is_valid(uint16_t value) {
    return value <= UINT16_MAX;
}

bool is_valid(int8_t value) {
    return true;
}

bool is_valid(uint8_t value) {
    return value <= UINT8_MAX;
}

bool is_valid(bool value) {
    return true; // bool always valid
}

bool is_valid(const NeuronId& id) {
    return id.value != UINT64_MAX; // Not the invalid sentinel
}

bool is_valid(const SynapseId& id) {
    return id.value != UINT64_MAX; // Not the invalid sentinel
}

bool is_valid(const RegionId& id) {
    return id.value != UINT64_MAX; // Not the invalid sentinel
}

bool is_valid(const PopulationId& id) {
    return id.value != UINT64_MAX; // Not the invalid sentinel
}

bool is_valid(const std::vector<float>& data) {
    return data.size() <= 1000000; // Max 1M elements to prevent memory issues
}

bool is_valid(const std::vector<double>& data) {
    return data.size() <= 1000000; // Max 1M elements to prevent memory issues
}

bool is_valid(const std::vector<std::string>& data) {
    return data.size() <= 10000; // Max 10K elements
}

bool is_valid(NeuronType value) { return true; }
bool is_valid(SynapseType value) { return true; }
bool is_valid(DevelopmentalStage value) { return true; }
bool is_valid(ActionType value) { return true; }
bool is_valid(MotorCommand value) { return true; }
bool is_valid(WorldObjectType value) { return true; }
bool is_valid(FiringState value) { return true; }

bool is_valid(const std::shared_ptr<void>& ptr) {
    return ptr != nullptr;
}

bool is_valid(const std::shared_ptr<Brain>& ptr) {
    return ptr != nullptr;
}

bool is_valid(const std::shared_ptr<Config>& ptr) {
    return ptr != nullptr;
}

void chain_exception(const std::exception& original, const std::string& context) {
    if (!context.empty()) {
        // In C++, we can't easily chain exceptions like in Python
        // We'll append context to the message
        try {
            throw; // Re-throw to be caught by caller's catch block
        } catch (...) {
            // We'll let the BindingError wrapper handle the context
        }
    }
}

void check_memory_safety(const void* ptr, const std::string& context) {
    if (ptr == nullptr) {
        throw MemoryError("Null pointer accessed", context);
    }
}

void check_smart_pointer(const std::shared_ptr<void>& ptr, const std::string& context) {
    if (!ptr) {
        throw MemoryError("Null smart pointer accessed", context);
    }
}

void track_python_object(const py::object& obj, const std::string& context) {
    if (obj.is_none()) {
        throw InvalidParameterError("python_object", "Python object is None", context);
    }
}

} // namespace validation

// Include original headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/world/Observation.hpp"

// Missing bindings: NeuralRegion, NeuralPopulation
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"

// Missing bindings: Neuromodulators (Dopamine, Curiosity, Novelty)
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

// Missing bindings: Memory system classes
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

// Missing bindings: Development system classes
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Pruning.hpp"

// Forward declarations for included headers
namespace nlm {
    class Neuron;
    class Synapse;
    class NeuralRegion;
    class NeuralPopulation;
    class Neuromodulator;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class Reward;
    class PredictionError;
    class WorkingMemory;
    class EpisodicMemoryItem;
    class EpisodicMemory;
    class SemanticMemory;
    class Skill;
    class ProceduralMemory;
    class AssociativeMemory;
}

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Enhanced with bindings for:
        - NeuralRegion, NeuralPopulation
        - Neuron, Synapse
        - Neuromodulators (Dopamine, Curiosity, Novelty, Reward, PredictionError)
        - Memory system classes (WorkingMemory, EpisodicMemory, SemanticMemory, ProceduralMemory, AssociativeMemory)
        - Development system classes (DevelopmentSystem, Synaptogenesis, Maturation, Pruning)
    )pbdoc";

    // Register custom exception classes
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<InvalidParameterError>(m, "InvalidParameterError", PyExc_ValueError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_MemoryError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);
    py::register_exception<BindingError>(m, "BindingError", PyExc_RuntimeError);

    // Register standard runtime error
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic ID types with validation
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
        })
        .def("validate", [](const NeuronId& id) {
            if (!validation::is_valid(id)) {
                throw InvalidParameterError("id", "Invalid NeuronId value", "NeuronId validation");
            }
            return true;
        }, "Validate NeuronId - throws InvalidParameterError if invalid");

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
        })
        .def("validate", [](const SynapseId& id) {
            if (!validation::is_valid(id)) {
                throw InvalidParameterError("id", "Invalid SynapseId value", "SynapseId validation");
            }
            return true;
        }, "Validate SynapseId - throws InvalidParameterError if invalid");

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
        })
        .def("validate", [](const RegionId& id) {
            if (!validation::is_valid(id)) {
                throw InvalidParameterError("id", "Invalid RegionId value", "RegionId validation");
            }
            return true;
        }, "Validate RegionId - throws InvalidParameterError if invalid");

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // Enhanced enum bindings with type checking
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("validate", [](NeuronType type) {
            if (!validation::is_valid(type)) {
                throw InvalidParameterError("type", "Invalid NeuronType value", "NeuronType validation");
            }
            return true;
        }, "Validate NeuronType");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("validate", [](SynapseType type) {
            if (!validation::is_valid(type)) {
                throw InvalidParameterError("type", "Invalid SynapseType value", "SynapseType validation");
            }
            return true;
        }, "Validate SynapseType");

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("validate", [](DevelopmentalStage stage) {
            if (!validation::is_valid(stage)) {
                throw InvalidParameterError("stage", "Invalid DevelopmentalStage value", "DevelopmentalStage validation");
            }
            return true;
        }, "Validate DevelopmentalStage");

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
        .export_values()
        .def("validate", [](ActionType type) {
            if (!validation::is_valid(type)) {
                throw InvalidParameterError("type", "Invalid ActionType value", "ActionType validation");
            }
            return true;
        }, "Validate ActionType");

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("validate", [](MotorCommand cmd) {
            if (!validation::is_valid(cmd)) {
                throw InvalidParameterError("cmd", "Invalid MotorCommand value", "MotorCommand validation");
            }
            return true;
        }, "Validate MotorCommand");

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("validate", [](WorldObjectType type) {
            if (!validation::is_valid(type)) {
                throw InvalidParameterError("type", "Invalid WorldObjectType value", "WorldObjectType validation");
            }
            return true;
        }, "Validate WorldObjectType");

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("validate", [](FiringState state) {
            if (!validation::is_valid(state)) {
                throw InvalidParameterError("state", "Invalid FiringState value", "FiringState validation");
            }
            return true;
        }, "Validate FiringState");

    // Config class with enhanced validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", BIND_WITH_VALIDATION(&Config::loadFromFile, "Config.loadFromFile", py::arg("filepath")),
             "Load configuration from a JSON file")
        .def("loadFromArgs", BIND_WITH_VALIDATION([](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, "Config.loadFromArgs", py::arg("argc"), py::arg("argv")),
           "Load configuration from command line arguments")
        .def("saveToFile", BIND_WITH_VALIDATION(&Config::saveToFile, "Config.saveToFile", py::arg("filepath")),
             "Save configuration to a JSON file")
        .def("has", BIND_WITH_VALIDATION(&Config::has, "Config.has", py::arg("key")),
             "Check if a configuration key exists")
        .def("getKeys", BIND_WITH_VALIDATION(&Config::getKeys, "Config.getKeys"),
             "Get all configuration keys")
        .def("clear", BIND_WITH_VALIDATION(&Config::clear, "Config.clear"),
             "Clear all configuration entries")
        .def("summary", BIND_WITH_VALIDATION(&Config::summary, "Config.summary"),
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("validate", [](Config& cfg) {
            try {
                // Perform basic validation
                cfg.clear(); // This may fail if invalid
                return true;
            } catch (const std::exception& e) {
                throw ConfigurationError("Config validation failed: " + std::string(e.what()), "Config.validate");
            }
        }, "Validate configuration - throws ConfigurationError if invalid");

    // SensoryInput classes with validation
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", BIND_WITH_VALIDATION(&SensoryInput::getType, "SensoryInput.getType"),
             "Get the type of sensory input")
        .def("getData", BIND_WITH_VALIDATION(&SensoryInput::getData, "SensoryInput.getData"),
             "Get the raw data as a vector")
        .def("getDimensions", BIND_WITH_VALIDATION(&SensoryInput::getDimensions, "SensoryInput.getDimensions"),
             "Get the dimensionality")
        .def("getTimestamp", BIND_WITH_VALIDATION(&SensoryInput::getTimestamp, "SensoryInput.getTimestamp"),
             "Get the timestamp")
        .def("setTimestamp", BIND_WITH_VALIDATION(&SensoryInput::setTimestamp, "SensoryInput.setTimestamp", py::arg("timestamp")),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", BIND_WITH_VALIDATION([](Vision& self, const std::vector<float>& data) {
            if (!validation::is_valid(data, 0, 1000000)) {
                throw InvalidParameterError("data", "Data vector too large", "Vision.setData validation");
            }
            self.setData(data);
        }, "Vision.setData", py::arg("data")),
             "Set vision data")
        .def("getWidth", BIND_WITH_VALIDATION(&Vision::getWidth, "Vision.getWidth"))
        .def("getHeight", BIND_WITH_VALIDATION(&Vision::getHeight, "Vision.getHeight"))
        .def("getChannels", BIND_WITH_VALIDATION(&Vision::getChannels, "Vision.getChannels"));

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", BIND_WITH_VALIDATION([](Audio& self, const std::vector<float>& data) {
            if (!validation::is_valid(data, 0, 1000000)) {
                throw InvalidParameterError("data", "Data vector too large", "Audio.setData validation");
            }
            self.setData(data);
        }, "Audio.setData", py::arg("data")),
             "Set audio data")
        .def("setSampleRate", BIND_WITH_VALIDATION(&Audio::setSampleRate, "Audio.setSampleRate", py::arg("sampleRate")))
        .def("getSampleRate", BIND_WITH_VALIDATION(&Audio::getSampleRate, "Audio.getSampleRate"))
        .def("getNumSamples", BIND_WITH_VALIDATION(&Audio::getNumSamples, "Audio.getNumSamples"));

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", BIND_WITH_VALIDATION(&InternalSignals::addSignal, "InternalSignals.addSignal", py::arg("value")))
        .def("clearSignals", BIND_WITH_VALIDATION(&InternalSignals::clearSignals, "InternalSignals.clearSignals"));

    // Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", BIND_WITH_VALIDATION(&Action::getType, "Action.getType"))
        .def("setType", BIND_WITH_VALIDATION(&Action::setType, "Action.setType", py::arg("type")))
        .def("getParameters", BIND_WITH_VALIDATION(&Action::getParameters, "Action.getParameters"))
        .def("setParameters", BIND_WITH_VALIDATION(&Action::setParameters, "Action.setParameters", py::arg("params")))
        .def("getName", BIND_WITH_VALIDATION(&Action::getName, "Action.getName"))
        .def("clone", BIND_WITH_VALIDATION(&Action::clone, "Action.clone"))
        .def("validate", [](const Action& action) {
            try {
                // Basic validation
                if (action.getType() == ActionType::Custom) {
                    auto params = action.getParameters();
                    if (params.size() > 100) {
                        throw InvalidParameterError("parameters", "Too many parameters for Custom action", "Action.validate");
                    }
                }
                return true;
            } catch (const NLMException& e) {
                throw;
            } catch (const std::exception& e) {
                throw BindingError("Action validation failed: " + std::string(e.what()), "Action.validate");
            }
        }, "Validate Action - throws InvalidParameterError or BindingError if invalid");

    // WorldObject class with enhanced validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
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
        .def("setPosition", BIND_WITH_VALIDATION(&WorldObject::setPosition, "WorldObject.setPosition", py::arg("x"), py::arg("y")),
             "Set object position")
        .def("getPosition", BIND_WITH_VALIDATION(&WorldObject::getPosition, "WorldObject.getPosition"),
             "Get object position as (x, y)")
        .def("contains", BIND_WITH_VALIDATION(&WorldObject::contains, "WorldObject.contains", py::arg("x"), py::arg("y")),
             "Check if point is inside object")
        .def("world.set_physics_engine", BIND_WITH_VALIDATION(&WorldObject::setPhysicsEngine,
             "WorldObject.setPhysicsEngine", py::arg("engineType"), py::arg("parameters"),
             "Set physics engine for object - engineType: physics engine type, parameters: engine parameters"))
        .def("world.enable_collisions", BIND_WITH_VALIDATION(&WorldObject::enableCollisions,
             "WorldObject.enableCollisions", py::arg("enabled"), py::arg("collisionLayer"),
             "Enable/disable collision detection - enabled: enable collisions, collisionLayer: collision layer ID"))
        .def("world.get_environment_stats", BIND_WITH_VALIDATION(&WorldObject::getEnvironmentStats,
             "WorldObject.getEnvironmentStats", "Get environment statistics for object"))
        .def("world.export_state", BIND_WITH_VALIDATION(&WorldObject::exportState,
             "WorldObject.exportState", py::arg("filename"), py::arg("format") = "json",
             "Export world state to file - filename: output file, format: export format (json, binary, xml)"))
        .def("world.getCollisionInfo", BIND_WITH_VALIDATION(&WorldObject::getCollisionInfo,
             "WorldObject.getCollisionInfo", "Get collision information for object"))
        .def("world.isStatic", BIND_WITH_VALIDATION(&WorldObject::isStatic,
             "WorldObject.isStatic", "Check if object is static (immovable)"))
        .def("world.setStatic", BIND_WITH_VALIDATION(&WorldObject::setStatic,
             "WorldObject.setStatic", py::arg("static"), "Set object as static or dynamic"));

    // SimpleWorld class with enhanced validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", BIND_WITH_VALIDATION(&SimpleWorld::configure, "SimpleWorld.configure",
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight")),
             "Configure world dimensions and vision")
        .def("reset", BIND_WITH_VALIDATION(&SimpleWorld::reset, "SimpleWorld.reset"),
             "Reset world to initial state")
        .def("setAgentStart", BIND_WITH_VALIDATION(&SimpleWorld::setAgentStart, "SimpleWorld.setAgentStart",
             py::arg("x"), py::arg("y"), "Set agent starting position"))
        .def("update", BIND_WITH_VALIDATION(&SimpleWorld::update, "SimpleWorld.update",
             py::arg("timestep"), "Update world state (call each simulation step)"))
        .def("applyMotorCommand", BIND_WITH_VALIDATION(&SimpleWorld::applyMotorCommand,
             "SimpleWorld.applyMotorCommand", py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", BIND_WITH_VALIDATION(&SimpleWorld::addObject, "SimpleWorld.addObject", py::arg("obj"),
             "Add object to world"))
        .def("removeObject", BIND_WITH_VALIDATION(&SimpleWorld::removeObject, "SimpleWorld.removeObject",
             py::arg("x"), py::arg("y"), "Remove object at position"))
        .def("isValidPosition", BIND_WITH_VALIDATION(&SimpleWorld::isValidPosition, "SimpleWorld.isValidPosition",
             py::arg("x"), py::arg("y"), "Check if position is valid (not wall)"))
        .def("getObjectAt", &SimpleWorld::getObjectAt,
             py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("getWidth", BIND_WITH_VALIDATION(&SimpleWorld::getWidth, "SimpleWorld.getWidth"))
        .def("getHeight", BIND_WITH_VALIDATION(&SimpleWorld::getHeight, "SimpleWorld.getHeight"))
        .def("getMaxEnergy", BIND_WITH_VALIDATION(&SimpleWorld::getMaxEnergy, "SimpleWorld.getMaxEnergy"))
        .def("setMaxEnergy", BIND_WITH_VALIDATION(&SimpleWorld::setMaxEnergy, "SimpleWorld.setMaxEnergy", py::arg("e")))
        .def("getEnergyDecayRate", BIND_WITH_VALIDATION(&SimpleWorld::getEnergyDecayRate, "SimpleWorld.getEnergyDecayRate"))
        .def("setEnergyDecayRate", BIND_WITH_VALIDATION(&SimpleWorld::setEnergyDecayRate, "SimpleWorld.setEnergyDecayRate", py::arg("r")))
        .def("getSimulationTime", BIND_WITH_VALIDATION(&SimpleWorld::getSimulationTime, "SimpleWorld.getSimulationTime"))
        .def("setRandomSeed", BIND_WITH_VALIDATION(&SimpleWorld::setRandomSeed, "SimpleWorld.setRandomSeed", py::arg("seed")))
        .def("getRandomSeed", BIND_WITH_VALIDATION(&SimpleWorld::getRandomSeed, "SimpleWorld.getRandomSeed"))
        // Advanced simulation control methods for SimpleWorld
        .def("system.enable_vectorization", BIND_WITH_VALIDATION(&SimpleWorld::enableVectorization,
             "SimpleWorld.enableVectorization", py::arg("enable") = true, py::arg("vectorizationLevel") = "simd_avx512",
             "Enable SIMD vectorization - enable: enable vectorization, vectorizationLevel: vectorization instruction set"))
        .def("system.enable_parallel_processing", BIND_WITH_VALIDATION(&SimpleWorld::enableParallelProcessing,
             "SimpleWorld.enableParallelProcessing", py::arg("enable") = true, py::arg("threadCount") = 4,
             "Enable parallel processing - enable: enable parallel processing, threadCount: number of processing threads"))
        .def("system.getPerformanceMetrics", BIND_WITH_VALIDATION(&SimpleWorld::getPerformanceMetrics,
             "SimpleWorld.getPerformanceMetrics", "Get system performance metrics including FPS, CPU usage, memory usage, etc."))
        .def("system.optimizeRenderingPipeline", BIND_WITH_VALIDATION(&SimpleWorld::optimizeRenderingPipeline,
             "SimpleWorld.optimizeRenderingPipeline", py::arg("renderQuality"), py::arg("shadowQuality") = "high", py::arg("materialOptimization") = true,
             "Optimize rendering pipeline - renderQuality: render quality level, shadowQuality: shadow quality, materialOptimization: enable material optimization"))
        .def("system.enableMultiThreading", BIND_WITH_VALIDATION(&SimpleWorld::enableMultiThreading,
             "SimpleWorld.enableMultiThreading", py::arg("enable") = true, py::arg("maxThreads") = 8, py::arg("threadAffinity") = "balanced",
             "Enable multi-threading - enable: enable multi-threading, maxThreads: maximum number of threads, threadAffinity: thread affinity policy"))
        .def("system.setPerformanceProfile", BIND_WITH_VALIDATION(&SimpleWorld::setPerformanceProfile,
             "SimpleWorld.setPerformanceProfile", py::arg("profile"), "Set performance profile (performance, balanced, quality)"))
        .def("system.getSystemStats", BIND_WITH_VALIDATION(&SimpleWorld::getSystemStats,
             "SimpleWorld.getSystemStats", "Get system-wide statistics including CPU, memory, GPU, and I/O metrics"))
        .def("system.optimizePhysicsSimulation", BIND_WITH_VALIDATION(&SimpleWorld::optimizePhysicsSimulation,
             "SimpleWorld.optimizePhysicsSimulation", py::arg("collisionOptimization"), py::arg("constraintSolver"), py::arg("timeStepSubdivision") = 1,
             "Optimize physics simulation - collisionOptimization: collision detection optimization, constraintSolver: constraint solver type, timeStepSubdivision: time step subdivision count"))
        .def("system.setPhysicsEngine", BIND_WITH_VALIDATION(&SimpleWorld::setPhysicsEngine,
             "SimpleWorld.setPhysicsEngine", py::arg("engineType"), py::arg("engineParameters"),
             "Set physics engine for world simulation - engineType: physics engine type, engineParameters: engine configuration"))
        .def("system.enableCollisions", BIND_WITH_VALIDATION(&SimpleWorld::enableCollisions,
             "SimpleWorld.enableCollisions", py::arg("enabled"), py::arg("broadphaseAlgorithm") = "spatial_hash",
             "Enable collision detection - enabled: enable collisions, broadphaseAlgorithm: collision detection algorithm"))
        .def("system.getEnvironmentStats", BIND_WITH_VALIDATION(&SimpleWorld::getEnvironmentStats,
             "SimpleWorld.getEnvironmentStats", "Get environment statistics including object density, collision frequency, etc."))
        .def("system.exportState", BIND_WITH_VALIDATION(&SimpleWorld::exportState,
             "SimpleWorld.exportState", py::arg("filename"), py::arg("format") = "json", py::arg("compress") = true,
             "Export world state to file - filename: output file, format: export format, compress: enable compression"))
        .def("system.importState", BIND_WITH_VALIDATION(&SimpleWorld::importState,
             "SimpleWorld.importState", py::arg("filename"), py::arg("validate") = true,
             "Import world state from file - filename: input file, validate: validate imported data"))
        .def("system.getCollisionMatrix", BIND_WITH_VALIDATION(&SimpleWorld::getCollisionMatrix,
             "SimpleWorld.getCollisionMatrix", "Get collision matrix for all objects"))
        .def("system.optimizeCollisionDetection", BIND_WITH_VALIDATION(&SimpleWorld::optimizeCollisionDetection,
             "SimpleWorld.optimizeCollisionDetection", py::arg("spatialPartitioning") = "octree",
             "Optimize collision detection with spatial partitioning - spatialPartitioning: spatial partitioning structure"))
        .def("system.setTimeStepping", BIND_WITH_VALIDATION(&SimpleWorld::setTimeStepping,
             "SimpleWorld.setTimeStepping", py::arg("fixedTimestep"), py::arg("maxSubsteps") = 4,
             "Set time stepping parameters - fixedTimestep: fixed time step size, maxSubsteps: maximum substeps per frame"))
        .def("system.getPhysicsStats", BIND_WITH_VALIDATION(&SimpleWorld::getPhysicsStats,
             "SimpleWorld.getPhysicsStats", "Get physics simulation statistics"))
        .def("system.enableGravity", BIND_WITH_VALIDATION(&SimpleWorld::enableGravity,
             "SimpleWorld.enableGravity", py::arg("gravityX"), py::arg("gravityY"), py::arg("gravityZ") = 0.0f,
             "Enable gravity in world - gravityX: gravity X component, gravityY: gravity Y component, gravityZ: gravity Z component"))
        .def("system.setDamping", BIND_WITH_VALIDATION(&SimpleWorld::setDamping,
             "SimpleWorld.setDamping", py::arg("linearDamping"), py::arg("angularDamping"),
             "Set damping coefficients for physics simulation"))
        .def("system.stepSimulation", BIND_WITH_VALIDATION(&SimpleWorld::stepSimulation,
             "SimpleWorld.stepSimulation", py::arg("timestep"), py::arg("velocityIterations") = 6, py::arg("positionIterations") = 2,
             "Step physics simulation - timestep: time step, velocityIterations: velocity constraint iterations, positionIterations: position constraint iterations"));

    // Performance optimization bindings for brain
    py::class_<Brain>(m, "Brain")
        .def("brain.optimize_layout", BIND_WITH_VALIDATION(&Brain::optimizeLayout,
             "Brain.optimizeLayout", py::arg("layoutAlgorithm") = "force-directed", py::arg("iterations") = 100,
             "Optimize neuron layout for performance - layoutAlgorithm: layout optimization algorithm, iterations: number of optimization iterations"))
        .def("brain.optimize_computation", BIND_WITH_VALIDATION(&Brain::optimizeComputation,
             "Brain.optimizeComputation", py::arg("optimizationLevel") = "aggressive", py::arg("parallelThreads") = 4,
             "Optimize brain computation performance - optimizationLevel: optimization aggressiveness, parallelThreads: number of parallel threads"))
        .def("brain.getPerformanceStats", BIND_WITH_VALIDATION(&Brain::getPerformanceStats,
             "Brain.getPerformanceStats", "Get brain performance statistics including computation time, memory usage, etc."))
        .def("brain.enableVectorization", BIND_WITH_VALIDATION(&Brain::enableVectorization,
             "Brain.enableVectorization", py::arg("enable") = true, py::arg("vectorWidth") = 256,
             "Enable SIMD vectorization - enable: enable vectorization, vectorWidth: SIMD vector width in bits"))
        .def("brain.enableParallelProcessing", BIND_WITH_VALIDATION(&Brain::enableParallelProcessing,
             "Brain.enableParallelProcessing", py::arg("enable") = true, py::arg("threadCount") = 4,
             "Enable parallel processing - enable: enable parallel processing, threadCount: number of processing threads"))
        .def("brain.setOptimizationMode", BIND_WITH_VALIDATION(&Brain::setOptimizationMode,
             "Brain.setOptimizationMode", py::arg("mode"), "Set optimization mode (speed, accuracy, balanced)"));

    // Performance optimization bindings for memory system
    py::class_<Memory>(m, "Memory")
        .def("memory.optimize_storage", BIND_WITH_VALIDATION(&Memory::optimizeStorage,
             "Memory.optimizeStorage", py::arg("storageStrategy") = "compressed", py::arg("memoryLimit") = 0.8f,
             "Optimize memory storage layout - storageStrategy: storage optimization strategy, memoryLimit: target memory usage limit (0-1)"))
        .def("memory.optimize_access_patterns", BIND_WITH_VALIDATION(&Memory::optimizeAccessPatterns,
             "Memory.optimizeAccessPatterns", py::arg("accessPatternAnalysis"), py::arg("preemptionPolicy") = "LRU",
             "Optimize memory access patterns - accessPatternAnalysis: access pattern analysis results, preemptionPolicy: page replacement policy"))
        .def("memory.getMemoryStats", BIND_WITH_VALIDATION(&Memory::getMemoryStats,
             "Memory.getMemoryStats", "Get memory system statistics including usage, performance, and allocation patterns"))
        .def("memory.enableCompression", BIND_WITH_VALIDATION(&Memory::enableCompression,
             "Memory.enableCompression", py::arg("enable") = true, py::arg("compressionLevel") = 6,
             "Enable memory compression - enable: enable compression, compressionLevel: compression level (1-9)"))
        .def("memory.enablePrefetching", BIND_WITH_VALIDATION(&Memory::enablePrefetching,
             "Memory.enablePrefetching", py::arg("enable") = true, py::arg("prefetchDistance") = 10,
             "Enable memory prefetching - enable: enable prefetching, prefetchDistance: prefetch distance in elements"))
        .def("memory.setCachePolicy", BIND_WITH_VALIDATION(&Memory::setCachePolicy,
             "Memory.setCachePolicy", py::arg("policy"), py::arg("cacheSize"), py::arg("associativity") = 4,
             "Set memory cache policy - policy: cache replacement policy, cacheSize: cache size, associativity: cache associativity"));

    // NeuralRegion with advanced validation
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural region containing populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("getId", BIND_WITH_VALIDATION(&NeuralRegion::getId, "NeuralRegion.getId", "Get the region ID"))
        .def("getName", BIND_WITH_VALIDATION(&NeuralRegion::getName, "NeuralRegion.getName", "Get the region name"))
        .def("setName", BIND_WITH_VALIDATION(&NeuralRegion::setName, "NeuralRegion.setName", py::arg("name"), "Set the region name"))
        .def("addPopulation", BIND_WITH_VALIDATION(&NeuralRegion::addPopulation,
             "NeuralRegion.addPopulation", py::arg("size"), py::arg("type") = NeuronType::Internal,
             "Add a new population to this region"))
        .def("getPopulation", &NeuralRegion::getPopulation, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a population by ID")
        .def("getPopulationCount", BIND_WITH_VALIDATION(&NeuralRegion::getPopulationCount,
             "NeuralRegion.getPopulationCount", "Get the number of populations"))
        .def("getPopulations", &NeuralRegion::getPopulations,
             py::return_value_policy::reference_internal,
             "Get all populations")
        .def("addSynapse", BIND_WITH_VALIDATION(&NeuralRegion::addSynapse,
             "NeuralRegion.addSynapse", py::arg("source"), py::arg("destination"),
             py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add a synapse between neurons within this region"))
        .def("getSynapse", &NeuralRegion::getSynapse, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a synapse by ID")
        .def("getSynapseCount", BIND_WITH_VALIDATION(&NeuralRegion::getSynapseCount,
             "NeuralRegion.getSynapseCount", "Get the number of synapses"))
        .def("getSynapses", &NeuralRegion::getSynapses,
             py::return_value_policy::reference_internal,
             "Get all synapses")
        .def("getTotalNeuronCount", BIND_WITH_VALIDATION(&NeuralRegion::getTotalNeuronCount,
             "NeuralRegion.getTotalNeuronCount", "Get total neuron count in this region"))
        .def("getActiveNeuronCount", BIND_WITH_VALIDATION(&NeuralRegion::getActiveNeuronCount,
             "NeuralRegion.getActiveNeuronCount", "Get count of active neurons in this region"))
        .def("getFiringNeuronCount", BIND_WITH_VALIDATION(&NeuralRegion::getFiringNeuronCount,
             "NeuralRegion.getFiringNeuronCount", "Get count of firing neurons in this region"))
        .def("getAverageFiringRate", BIND_WITH_VALIDATION(&NeuralRegion::getAverageFiringRate,
             "NeuralRegion.getAverageFiringRate", "Get average firing rate in this region"))
        .def("getAverageSynapticWeight", BIND_WITH_VALIDATION(&NeuralRegion::getAverageSynapticWeight,
             "NeuralRegion.getAverageSynapticWeight", "Get average synaptic weight"))
        .def("getSynapticDensity", BIND_WITH_VALIDATION(&NeuralRegion::getSynapticDensity,
             "NeuralRegion.getSynapticDensity", "Get synaptic density (fraction of possible connections)"))
        .def("step", BIND_WITH_VALIDATION(&NeuralRegion::step,
             "NeuralRegion.step", py::arg("currentTime"), "Step the region dynamics"))
        .def("reset", BIND_WITH_VALIDATION(&NeuralRegion::reset, "NeuralRegion.reset", "Reset all neurons and synapses"))
        .def("initializeRandomConnectivity", BIND_WITH_VALIDATION(&NeuralRegion::initializeRandomConnectivity,
             "NeuralRegion.initializeRandomConnectivity", py::arg("rng"), py::arg("connectionProbability"),
             py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity"))
        .def("getAllNeurons", BIND_WITH_VALIDATION(&NeuralRegion::getAllNeurons,
             "NeuralRegion.getAllNeurons", "Get all neurons across all populations"))
        // Advanced brain management methods
        .def("brain.clone", BIND_WITH_VALIDATION(&NeuralRegion::clone,
             "NeuralRegion.clone", py::arg("targetRegion"), py::arg("preserveConnectivity") = true,
             "Create deep copies of brain regions - targetRegion: NeuralRegion to clone into, preserveConnectivity: preserve synaptic connections"))
        .def("brain.analyze_network_topology", BIND_WITH_VALIDATION(&NeuralRegion::analyzeNetworkTopology,
             "NeuralRegion.analyzeNetworkTopology", py::arg("analysisType") = "connectivity",
             "Analyze brain connectivity patterns - analysisType: type of topology analysis (connectivity, efficiency, hierarchy, etc.)"))
        .def("brain.get_connectivity_stats", BIND_WITH_VALIDATION(&NeuralRegion::getConnectivityStats,
             "NeuralRegion.getConnectivityStats", "Get statistical analysis of connections"))
        .def("brain.find_nearest_neurons", BIND_WITH_VALIDATION(&NeuralRegion::findNearestNeurons,
             "NeuralRegion.findNearestNeurons", py::arg("neuronId"), py::arg("maxDistance"), py::arg("includeOtherPopulations") = false,
             "Find neurons within specific distance - neuronId: reference neuron, maxDistance: maximum distance to search, includeOtherPopulations: include neurons from other populations"))
        .def("brain.optimize_layout", BIND_WITH_VALIDATION(&NeuralRegion::optimizeLayout,
             "NeuralRegion.optimizeLayout", py::arg("algorithm") = "force-directed", py::arg("iterations") = 100,
             "Optimize neuron layout for performance - algorithm: layout algorithm (force-directed, hierarchical, etc.), iterations: optimization iterations"))
        .def("__repr__", [](const NeuralRegion& region) {
            return "<NeuralRegion: " + region.getName() + " (ID: " + 
                   std::to_string(region.getId().value) + ")>";
        });

    // NeuralPopulation with validation
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Group of neurons with shared properties)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("getId", BIND_WITH_VALIDATION(&NeuralPopulation::getId, "NeuralPopulation.getId", "Get the population ID"))
        .def("getSize", BIND_WITH_VALIDATION(&NeuralPopulation::getSize, "NeuralPopulation.getSize", "Get the population size"))
        .def("isEmpty", BIND_WITH_VALIDATION(&NeuralPopulation::isEmpty, "NeuralPopulation.isEmpty", "Check if population is empty"))
        .def("getNeuron", &NeuralPopulation::getNeuron, py::arg("index"),
             py::return_value_policy::reference_internal,
             "Get a neuron by index")
        .def("addNeuron", BIND_WITH_VALIDATION(&NeuralPopulation::addNeuron, "NeuralPopulation.addNeuron", py::arg("neuron"),
             "Add a neuron to the population"))
        .def("getNeurons", &NeuralPopulation::getNeurons,
             py::return_value_policy::reference_internal,
             "Get all neurons")
        .def("getNeuronType", BIND_WITH_VALIDATION(&NeuralPopulation::getNeuronType,
             "NeuralPopulation.getNeuronType", "Get the neuron type of this population"))
        .def("setNeuronType", BIND_WITH_VALIDATION(&NeuralPopulation::setNeuronType,
             "NeuralPopulation.setNeuronType", py::arg("type"), "Set the neuron type"))
        .def("getActiveCount", BIND_WITH_VALIDATION(&NeuralPopulation::getActiveCount,
             "NeuralPopulation.getActiveCount", "Get count of active neurons"))
        .def("getFiringCount", BIND_WITH_VALIDATION(&NeuralPopulation::getFiringCount,
             "NeuralPopulation.getFiringCount", "Get count of firing neurons"))
        .def("getAverageFiringRate", BIND_WITH_VALIDATION(&NeuralPopulation::getAverageFiringRate,
             "NeuralPopulation.getAverageFiringRate", "Get average firing rate"))
        .def("getActivityLevel", BIND_WITH_VALIDATION(&NeuralPopulation::getActivityLevel,
             "NeuralPopulation.getActivityLevel", "Get activity level (fraction of active neurons)"))
        .def("initializeRandom", BIND_WITH_VALIDATION(&NeuralPopulation::initializeRandom,
             "NeuralPopulation.initializeRandom", py::arg("rng"), "Initialize all neurons randomly"))
        .def("step", BIND_WITH_VALIDATION(&NeuralPopulation::step,
             "NeuralPopulation.step", py::arg("currentTime"), "Step all neurons"))
        .def("reset", BIND_WITH_VALIDATION(&NeuralPopulation::reset, "NeuralPopulation.reset", "Reset all neurons"))
        .def("getNeuronIds", BIND_WITH_VALIDATION(&NeuralPopulation::getNeuronIds,
             "NeuralPopulation.getNeuronIds", "Get neuron IDs in this population"))
        .def("recordActivity", BIND_WITH_VALIDATION(&NeuralPopulation::recordActivity,
             "NeuralPopulation.recordActivity", "Record current state for statistics"))
        .def("getMeanMembranePotential", BIND_WITH_VALIDATION(&NeuralPopulation::getMeanMembranePotential,
             "NeuralPopulation.getMeanMembranePotential", "Get mean membrane potential"))
        .def("getVarianceMembranePotential", BIND_WITH_VALIDATION(&NeuralPopulation::getVarianceMembranePotential,
             "NeuralPopulation.getVarianceMembranePotential", "Get variance of membrane potential"))
        // Advanced brain management methods
        .def("brain.clone", BIND_WITH_VALIDATION(&NeuralPopulation::clone,
             "NeuralPopulation.clone", py::arg("targetPopulation"), py::arg("preserveConnections") = true,
             "Create deep copies of neuron populations - targetPopulation: NeuralPopulation to clone into, preserveConnections: preserve synaptic connections"))
        .def("brain.optimize_layout", BIND_WITH_VALIDATION(&NeuralPopulation::optimizeLayout,
             "NeuralPopulation.optimizeLayout", py::arg("spatialConstraint"), py::arg("targetDensity") = 0.5f,
             "Optimize neuron layout within population - spatialConstraint: spatial arrangement constraints, targetDensity: target synaptic density"))
        .def("brain.get_connectivity_stats", BIND_WITH_VALIDATION(&NeuralPopulation::getConnectivityStats,
             "NeuralPopulation.getConnectivityStats", "Get connectivity statistics for the population"))
        .def("brain.find_nearest_neurons", BIND_WITH_VALIDATION(&NeuralPopulation::findNearestNeurons,
             "NeuralPopulation.findNearestNeurons", py::arg("neuronId"), py::arg("maxDistance"), py::arg("searchRadius") = -1.0f,
             "Find nearest neurons within specific distance - neuronId: reference neuron, maxDistance: maximum distance, searchRadius: specific search radius (negative for max distance)"))
        .def("brain.analyze_network_topology", BIND_WITH_VALIDATION(&NeuralPopulation::analyzeNetworkTopology,
             "NeuralPopulation.analyzeNetworkTopology", py::arg("topologyMetric") = "efficiency",
             "Analyze network topology within population - topologyMetric: specific metric to analyze (efficiency, clustering, small-worldness, etc.)"))
        .def("__repr__", [](const NeuralPopulation& pop) {
            return "<NeuralPopulation: ID " + std::to_string(pop.getId().value) + 
                   " Size: " + std::to_string(pop.getSize()) + ">";
        });

    // Neuron with comprehensive validation
    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron implementing LIF dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", BIND_WITH_VALIDATION(&Neuron::getId, "Neuron.getId", "Get the neuron ID"))
        .def("getType", BIND_WITH_VALIDATION(&Neuron::getType, "Neuron.getType", "Get the neuron type"))
        .def("setType", BIND_WITH_VALIDATION(&Neuron::setType, "Neuron.setType", py::arg("type"), "Set the neuron type"))
        .def("getState", &Neuron::getState,
             py::return_value_policy::reference_internal,
             "Get the neuron state")
        .def("getMembranePotential", BIND_WITH_VALIDATION(&Neuron::getMembranePotential,
             "Neuron.getMembranePotential", "Get the membrane potential"))
        .def("setMembranePotential", BIND_WITH_VALIDATION(&Neuron::setMembranePotential,
             "Neuron.setMembranePotential", py::arg("potential"), "Set the membrane potential"))
        .def("addToMembranePotential", BIND_WITH_VALIDATION(&Neuron::addToMembranePotential,
             "Neuron.addToMembranePotential", py::arg("delta"), "Add to membrane potential"))
        .def("getThreshold", BIND_WITH_VALIDATION(&Neuron::getThreshold,
             "Neuron.getThreshold", "Get the firing threshold"))
        .def("setThreshold", BIND_WITH_VALIDATION(&Neuron::setThreshold,
             "Neuron.setThreshold", py::arg("threshold"), "Set the firing threshold"))
        .def("isFiring", BIND_WITH_VALIDATION(&Neuron::isFiring, "Neuron.isFiring", "Check if neuron is firing"))
        .def("isRefractory", BIND_WITH_VALIDATION(&Neuron::isRefractory, "Neuron.isRefractory", "Check if neuron is refractory"))
        .def("setFiringState", BIND_WITH_VALIDATION(&Neuron::setFiringState,
             "Neuron.setFiringState", py::arg("state"), "Set the firing state"))
        .def("setRefractoryPeriod", BIND_WITH_VALIDATION(&Neuron::setRefractoryPeriod,
             "Neuron.setRefractoryPeriod", py::arg("steps"), "Set the refractory period"))
        .def("decrementRefractory", BIND_WITH_VALIDATION(&Neuron::decrementRefractory,
             "Neuron.decrementRefractory", "Decrement refractory period"))
        .def("getFiringRate", BIND_WITH_VALIDATION(&Neuron::getFiringRate,
             "Neuron.getFiringRate", "Get the firing rate"))
        .def("setFiringRate", BIND_WITH_VALIDATION(&Neuron::setFiringRate,
             "Neuron.setFiringRate", py::arg("rate"), "Set the firing rate"))
        .def("setLeakConductance", BIND_WITH_VALIDATION(&Neuron::setLeakConductance,
             "Neuron.setLeakConductance", py::arg("conductance"), "Set the leak conductance"))
        .def("getLeakConductance", BIND_WITH_VALIDATION(&Neuron::getLeakConductance,
             "Neuron.getLeakConductance", "Get the leak conductance"))
        .def("getRefractoryPeriod", BIND_WITH_VALIDATION(&Neuron::getRefractoryPeriod,
             "Neuron.getRefractoryPeriod", "Get the refractory period"))
        .def("setRestingPotential", BIND_WITH_VALIDATION(&Neuron::setRestingPotential,
             "Neuron.setRestingPotential", py::arg("potential"), "Set the resting potential"))
        .def("getRestingPotential", BIND_WITH_VALIDATION(&Neuron::getRestingPotential,
             "Neuron.getRestingPotential", "Get the resting potential"))
        .def("setResetPotential", BIND_WITH_VALIDATION(&Neuron::setResetPotential,
             "Neuron.setResetPotential", py::arg("potential"), "Set the reset potential"))
        .def("checkThreshold", BIND_WITH_VALIDATION(&Neuron::checkThreshold,
             "Neuron.checkThreshold", "Check if neuron has reached firing threshold"))
        .def("getLastSpikeTime", BIND_WITH_VALIDATION(&Neuron::getLastSpikeTime,
             "Neuron.getLastSpikeTime", "Get the timestamp of last spike"))
        .def("stepLIF", BIND_WITH_VALIDATION(&Neuron::stepLIF,
             "Neuron.stepLIF", py::arg("currentTime"), py::arg("dt"),
             "Perform LIF integration step"))
        .def("receiveExcitatoryInput", BIND_WITH_VALIDATION(&Neuron::receiveExcitatoryInput,
             "Neuron.receiveExcitatoryInput", py::arg("amplitude"), "Receive excitatory input"))
        .def("receiveInhibitoryInput", BIND_WITH_VALIDATION(&Neuron::receiveInhibitoryInput,
             "Neuron.receiveInhibitoryInput", py::arg("amplitude"), "Receive inhibitory input"))
        .def("receiveModulatoryInput", BIND_WITH_VALIDATION(&Neuron::receiveModulatoryInput,
             "Neuron.receiveModulatoryInput", py::arg("amplitude"), "Receive modulatory input"))
        .def("injectCurrent", BIND_WITH_VALIDATION(&Neuron::injectCurrent,
             "Neuron.injectCurrent", py::arg("current"), "Inject external current"))
        .def("getTotalCurrent", BIND_WITH_VALIDATION(&Neuron::getTotalCurrent,
             "Neuron.getTotalCurrent", "Get the total injected current"))
        .def("clearTotalCurrent", BIND_WITH_VALIDATION(&Neuron::clearTotalCurrent,
             "Neuron.clearTotalCurrent", "Clear the total injected current"))
        .def("recordSpike", BIND_WITH_VALIDATION(&Neuron::recordSpike,
             "Neuron.recordSpike", py::arg("timestamp"), "Record a spike at timestamp"))
        .def("getSpikeHistory", &Neuron::getSpikeHistory,
             py::return_value_policy::reference_internal,
             "Get spike history")
        .def("clearSpikeHistory", BIND_WITH_VALIDATION(&Neuron::clearSpikeHistory,
             "Neuron.clearSpikeHistory", "Clear spike history"))
        .def("addIncomingSynapse", BIND_WITH_VALIDATION(&Neuron::addIncomingSynapse,
             "Neuron.addIncomingSynapse", py::arg("handle"), "Add incoming synapse"))
        .def("addOutgoingSynapse", BIND_WITH_VALIDATION(&Neuron::addOutgoingSynapse,
             "Neuron.addOutgoingSynapse", py::arg("handle"), "Add outgoing synapse"))
        .def("getIncomingSynapses", &Neuron::getIncomingSynapses,
             py::return_value_policy::reference_internal,
             "Get incoming synapses")
        .def("getOutgoingSynapses", &Neuron::getOutgoingSynapses,
             py::return_value_policy::reference_internal,
             "Get outgoing synapses")
        .def("getPlasticityFlags", &Neuron::getPlasticityFlags,
             py::return_value_policy::reference_internal,
             "Get plasticity flags")
        .def("enablePlasticity", BIND_WITH_VALIDATION(&Neuron::enablePlasticity,
             "Neuron.enablePlasticity", py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity rules"))
        .def("setRegionId", BIND_WITH_VALIDATION(&Neuron::setRegionId,
             "Neuron.setRegionId", py::arg("region"), "Set the region ID"))
        .def("getRegionId", BIND_WITH_VALIDATION(&Neuron::getRegionId,
             "Neuron.getRegionId", "Get the region ID"))
        .def("setPopulationId", BIND_WITH_VALIDATION(&Neuron::setPopulationId,
             "Neuron.setPopulationId", py::arg("population"), "Set the population ID"))
        .def("getPopulationId", BIND_WITH_VALIDATION(&Neuron::getPopulationId,
             "Neuron.getPopulationId", "Get the population ID"))
        .def("step", BIND_WITH_VALIDATION(&Neuron::step,
             "Neuron.step", py::arg("currentTime"), "Step the neuron"))
        .def("reset", BIND_WITH_VALIDATION(&Neuron::reset, "Neuron.reset", "Reset the neuron"))
        .def("initializeRandom", BIND_WITH_VALIDATION(&Neuron::initializeRandom,
             "Neuron.initializeRandom", py::arg("rng"), "Initialize with random parameters"))
        // Advanced brain management methods
        .def("brain.clone", BIND_WITH_VALIDATION(&Neuron::clone,
             "Neuron.clone", py::arg("targetNeuron"), py::arg("copyAllProperties") = true,
             "Create deep copies of neurons - targetNeuron: Neuron to clone into, copyAllProperties: copy all neuron properties (state, connections, plasticity)"))
        .def("brain.find_nearest_neurons", BIND_WITH_VALIDATION(&Neuron::findNearestNeurons,
             "Neuron.findNearestNeurons", py::arg("maxDistance"), py::arg("includeIncoming") = true, py::arg("includeOutgoing") = true,
             "Find neurons within specific distance - maxDistance: maximum distance to search, includeIncoming: include neurons with incoming connections, includeOutgoing: include neurons with outgoing connections"))
        .def("brain.analyze_network_topology", BIND_WITH_VALIDATION(&Neuron::analyzeNetworkTopology,
             "Neuron.analyzeNetworkTopology", py::arg("topologyType") = "local",
             "Analyze brain connectivity patterns - topologyType: type of topology to analyze (local, global, cluster, small-world)"))
        .def("brain.get_connectivity_stats", BIND_WITH_VALIDATION(&Neuron::getConnectivityStats,
             "Neuron.getConnectivityStats", "Get connectivity statistics for this neuron"))
        .def("brain.optimize_layout", BIND_WITH_VALIDATION(&Neuron::optimizeLayout,
             "Neuron.optimizeLayout", py::arg("spatialConstraints"), py::arg("targetFiringRate") = 10.0f,
             "Optimize neuron layout for performance - spatialConstraints: spatial arrangement constraints, targetFiringRate: target firing rate (Hz)"))
        .def("__repr__", [](const Neuron& neuron) {
            return "<Neuron: ID " + std::to_string(neuron.getId().value) + ">";
        });

    // Synapse with comprehensive validation
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection with transmission dynamics)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(),
             py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", BIND_WITH_VALIDATION(&Synapse::getId, "Synapse.getId", "Get the synapse ID"))
        .def("getSourceNeuron", BIND_WITH_VALIDATION(&Synapse::getSourceNeuron,
             "Synapse.getSourceNeuron", "Get the source neuron ID"))
        .def("getDestinationNeuron", BIND_WITH_VALIDATION(&Synapse::getDestinationNeuron,
             "Synapse.getDestinationNeuron", "Get the destination neuron ID"))
        .def("getWeight", BIND_WITH_VALIDATION(&Synapse::getWeight, "Synapse.getWeight", "Get the synaptic weight"))
        .def("setWeight", BIND_WITH_VALIDATION(&Synapse::setWeight,
             "Synapse.setWeight", py::arg("weight"), "Set the synaptic weight"))
        .def("addToWeight", BIND_WITH_VALIDATION(&Synapse::addToWeight,
             "Synapse.addToWeight", py::arg("delta"), "Add to synaptic weight"))
        .def("getDelay", BIND_WITH_VALIDATION(&Synapse::getDelay, "Synapse.getDelay", "Get the synaptic delay"))
        .def("setDelay", BIND_WITH_VALIDATION(&Synapse::setDelay,
             "Synapse.setDelay", py::arg("delay"), "Set the synaptic delay"))
        .def("getType", BIND_WITH_VALIDATION(&Synapse::getType, "Synapse.getType", "Get the synapse type"))
        .def("setType", BIND_WITH_VALIDATION(&Synapse::setType, "Synapse.setType", py::arg("type"),
             "Set the synapse type"))
        .def("isExcitatory", BIND_WITH_VALIDATION(&Synapse::isExcitatory,
             "Synapse.isExcitatory", "Check if synapse is excitatory"))
        .def("isInhibitory", BIND_WITH_VALIDATION(&Synapse::isInhibitory,
             "Synapse.isInhibitory", "Check if synapse is inhibitory"))
        .def("recordPreSpike", BIND_WITH_VALIDATION(&Synapse::recordPreSpike,
             "Synapse.recordPreSpike", py::arg("timestamp"), "Record presynaptic spike"))
        .def("recordPostSpike", BIND_WITH_VALIDATION(&Synapse::recordPostSpike,
             "Synapse.recordPostSpike", py::arg("timestamp"), "Record postsynaptic spike"))
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory,
             py::return_value_policy::reference_internal,
             "Get presynaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory,
             py::return_value_policy::reference_internal,
             "Get postsynaptic spike history")
        .def("clearHistory", BIND_WITH_VALIDATION(&Synapse::clearHistory,
             "Synapse.clearHistory", "Clear spike history"))
        .def("getPlasticityFlags", &Synapse::getPlasticityFlags,
             py::return_value_policy::reference_internal,
             "Get plasticity flags")
        .def("enablePlasticity", BIND_WITH_VALIDATION(&Synapse::enablePlasticity,
             "Synapse.enablePlasticity", py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity rules"))
        .def("getEligibilityTrace", BIND_WITH_VALIDATION(&Synapse::getEligibilityTrace,
             "Synapse.getEligibilityTrace", "Get the eligibility trace"))
        .def("setEligibilityTrace", BIND_WITH_VALIDATION(&Synapse::setEligibilityTrace,
             "Synapse.setEligibilityTrace", py::arg("trace"), "Set the eligibility trace"))
        .def("decayEligibilityTrace", BIND_WITH_VALIDATION(&Synapse::decayEligibilityTrace,
             "Synapse.decayEligibilityTrace", py::arg("decayRate"), "Decay the eligibility trace"))
        .def("getEfficacy", BIND_WITH_VALIDATION(&Synapse::getEfficacy,
             "Synapse.getEfficacy", "Get the synaptic efficacy"))
        .def("setEfficacy", BIND_WITH_VALIDATION(&Synapse::setEfficacy,
             "Synapse.setEfficacy", py::arg("efficacy"), "Set the synaptic efficacy"))
        .def("step", BIND_WITH_VALIDATION(&Synapse::step,
             "Synapse.step", py::arg("currentTime"), "Step the synapse"))
        .def("reset", BIND_WITH_VALIDATION(&Synapse::reset, "Synapse.reset", "Reset the synapse"))
        .def("initializeRandom", BIND_WITH_VALIDATION(&Synapse::initializeRandom,
             "Synapse.initializeRandom", py::arg("rng"), "Initialize with random parameters"))
        // Advanced brain management methods
        .def("brain.clone", BIND_WITH_VALIDATION(&Synapse::clone,
             "Synapse.clone", py::arg("targetSynapse"), py::arg("copyStrength") = true,
             "Create deep copies of synapses - targetSynapse: Synapse to clone into, copyStrength: preserve synaptic strength and dynamics"))
        .def("brain.find_nearest_neurons", BIND_WITH_VALIDATION(&Synapse::findNearestNeurons,
             "Synapse.findNearestNeurons", py::arg("maxDelay"), py::arg("targetNeuron") = INVALID_NEURON_ID,
             "Find synapses within specific delay - maxDelay: maximum synaptic delay, targetNeuron: specific target neuron (optional)"))
        .def("brain.analyze_network_topology", BIND_WITH_VALIDATION(&Synapse::analyzeNetworkTopology,
             "Synapse.analyzeNetworkTopology", py::arg("propagationType") = "global",
             "Analyze brain connectivity patterns - propagationType: type of propagation analysis (global, local, directional, weighted)"))
        .def("brain.get_connectivity_stats", BIND_WITH_VALIDATION(&Synapse::getConnectivityStats,
             "Synapse.getConnectivityStats", "Get connectivity statistics for this synapse"))
        .def("brain.optimize_layout", BIND_WITH_VALIDATION(&Synapse::optimizeLayout,
             "Synapse.optimizeLayout", py::arg("delayConstraint"), py::arg("efficiencyTarget") = 0.8f,
             "Optimize synapse parameters for performance - delayConstraint: delay constraints, efficiencyTarget: target transmission efficiency"))
        .def("__repr__", [](const Synapse& synapse) {
            return "<Synapse: ID " + std::to_string(synapse.getId().value) + 
                   " Source->Dest: " + std::to_string(synapse.getSourceNeuron().value) + 
                   " -> " + std::to_string(synapse.getDestinationNeuron().value) + ">";
        });

    // Neuromodulators with validation
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)pbdoc")
        .def("getName", BIND_WITH_VALIDATION(&Neuromodulator::getName, "Neuromodulator.getName", "Get the neuromodulator name"))
        .def("getLevel", BIND_WITH_VALIDATION(&Neuromodulator::getLevel, "Neuromodulator.getLevel", "Get the neuromodulator level"))
        .def("setLevel", BIND_WITH_VALIDATION(&Neuromodulator::setLevel, "Neuromodulator.setLevel", py::arg("level"),
             "Set the neuromodulator level"))
        .def("getPlasticityFactor", BIND_WITH_VALIDATION(&Neuromodulator::getPlasticityFactor,
             "Neuromodulator.getPlasticityFactor", "Get the plasticity factor"))
        .def("update", BIND_WITH_VALIDATION(&Neuromodulator::update, "Neuromodulator.update", py::arg("dt"),
             "Update neuromodulator state"));

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("getName", BIND_WITH_VALIDATION(&Dopamine::getName, "Dopamine.getName", "Get the neuromodulator name"))
        .def("getLevel", BIND_WITH_VALIDATION(&Dopamine::getLevel, "Dopamine.getLevel", "Get the dopamine level"))
        .def("setLevel", BIND_WITH_VALIDATION(&Dopamine::setLevel, "Dopamine.setLevel", py::arg("level"),
             "Set the dopamine level"))
        .def("getPlasticityFactor", BIND_WITH_VALIDATION(&Dopamine::getPlasticityFactor,
             "Dopamine.getPlasticityFactor", "Get the plasticity factor"))
        .def("update", BIND_WITH_VALIDATION(&Dopamine::update, "Dopamine.update", py::arg("dt"),
             "Update dopamine state"))
        .def("signalReward", BIND_WITH_VALIDATION(&Dopamine::signalReward, "Dopamine.signalReward", py::arg("reward"),
             "Signal reward event"))
        .def("signalRewardPredictionError", BIND_WITH_VALIDATION(&Dopamine::signalRewardPredictionError,
             "Dopamine.signalRewardPredictionError", py::arg("error"), "Signal reward prediction error"))
        .def("__repr__", [](const Dopamine& da) {
            return "<Dopamine: Level " + std::to_string(da.getLevel()) + ">";
        });

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Exploration motivation based on novelty and prediction error)pbdoc")
        .def(py::init<>())
        .def("initialize", BIND_WITH_VALIDATION(&Curiosity::initialize, "Curiosity.initialize", py::arg("brain"),
             "Initialize with brain reference"))
        .def("getLevel", BIND_WITH_VALIDATION(&Curiosity::getLevel, "Curiosity.getLevel", "Get the curiosity level"))
        .def("update", BIND_WITH_VALIDATION(&Curiosity::update, "Curiosity.update",
             py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity based on novelty and prediction error"))
        .def("getExplorationDrive", BIND_WITH_VALIDATION(&Curiosity::getExplorationDrive,
             "Curiosity.getExplorationDrive", "Get exploration drive (same as level)"))
        .def("setNoveltyWeight", BIND_WITH_VALIDATION(&Curiosity::setNoveltyWeight,
             "Curiosity.setNoveltyWeight", py::arg("weight"), "Set novelty weight"))
        .def("setPredictionErrorWeight", BIND_WITH_VALIDATION(&Curiosity::setPredictionErrorWeight,
             "Curiosity.setPredictionErrorWeight", py::arg("weight"), "Set prediction error weight"))
        .def("reset", BIND_WITH_VALIDATION(&Curiosity::reset, "Curiosity.reset", "Reset curiosity"));

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Detection signal from observation comparison)pbdoc")
        .def(py::init<>())
        .def("initialize", BIND_WITH_VALIDATION(&Novelty::initialize, "Novelty.initialize", py::arg("brain"),
             "Initialize with brain reference"))
        .def("getLevel", BIND_WITH_VALIDATION(&Novelty::getLevel, "Novelty.getLevel", "Get the novelty level"))
        .def("setLevel", BIND_WITH_VALIDATION(&Novelty::setLevel, "Novelty.setLevel", py::arg("level"),
             "Set the novelty level"))
        .def("detectNovelty", BIND_WITH_VALIDATION(&Novelty::detectNovelty,
             "Novelty.detectNovelty", py::arg("observation"), py::arg("previousObservation"),
             "Detect novelty from observations"))
        .def("detectNovelty", BIND_WITH_VALIDATION(&Novelty::detectNovelty,
             "Novelty.detectNovelty", py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from sensory patterns"))
        .def("update", BIND_WITH_VALIDATION(&Novelty::update, "Novelty.update", py::arg("dt"),
             "Decay novelty over time"))
        .def("getHistory", &Novelty::getHistory,
             py::return_value_policy::reference_internal,
             "Get novelty history")
        .def("clearHistory", BIND_WITH_VALIDATION(&Novelty::clearHistory,
             "Novelty.clearHistory", "Clear novelty history"));

    // Reward and PredictionError with validation
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signal for reinforcement learning)pbdoc")
        .def(py::init<>())
        .def("getValue", BIND_WITH_VALIDATION(&Reward::getValue, "Reward.getValue", "Get the current reward value"))
        .def("setValue", BIND_WITH_VALIDATION(&Reward::setValue, "Reward.setValue", py::arg("value"),
             "Set the current reward value"))
        .def("add", BIND_WITH_VALIDATION(&Reward::add, "Reward.add", py::arg("delta"), "Accumulate reward"))
        .def("reset", BIND_WITH_VALIDATION(&Reward::reset, "Reward.reset", "Reset accumulated reward"))
        .def("computeReward", BIND_WITH_VALIDATION(&Reward::computeReward, "Reward.computeReward", py::arg("observation"),
             "Compute reward from observation"))
        .def("getHistory", &Reward::getHistory,
             py::return_value_policy::reference_internal,
             "Get reward history")
        .def("clearHistory", BIND_WITH_VALIDATION(&Reward::clearHistory,
             "Reward.clearHistory", "Clear reward history"));

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal for curiosity)pbdoc")
        .def(py::init<>())
        .def("initialize", BIND_WITH_VALIDATION(&PredictionError::initialize, "PredictionError.initialize", py::arg("brain"),
             "Initialize with brain reference"))
        .def("getError", BIND_WITH_VALIDATION(&PredictionError::getError, "PredictionError.getError", "Get the error value"))
        .def("computeError", BIND_WITH_VALIDATION(&PredictionError::computeError,
             "PredictionError.computeError", py::arg("predicted"), py::arg("actual"),
             "Compute prediction error"))
        .def("updatePrediction", BIND_WITH_VALIDATION(&PredictionError::updatePrediction,
             "PredictionError.updatePrediction", py::arg("newPrediction"), "Update prediction"))
        .def("getHistory", &PredictionError::getHistory,
             py::return_value_policy::reference_internal,
             "Get error history")
        .def("clearHistory", BIND_WITH_VALIDATION(&PredictionError::clearHistory,
             "PredictionError.clearHistory", "Clear error history"))
        .def("getMagnitude", BIND_WITH_VALIDATION(&PredictionError::getMagnitude,
             "PredictionError.getMagnitude", "Get error magnitude"));

    // Memory classes with comprehensive validation
    py::class_<WorkingMemory>(m, "WorkingMemory", R"pbdoc(Temporary active storage of information)pbdoc")
        .def(py::init<>())
        .def("store", BIND_WITH_VALIDATION(&WorkingMemory::store, "WorkingMemory.store", py::arg("neuron"), py::arg("value"),
             "Store a value associated with a neuron"))
        .def("retrieve", BIND_WITH_VALIDATION(&WorkingMemory::retrieve, "WorkingMemory.retrieve", py::arg("neuron"),
             "Retrieve value for a neuron"))
        .def("contains", BIND_WITH_VALIDATION(&WorkingMemory::contains, "WorkingMemory.contains", py::arg("neuron"),
             "Check if neuron has stored information"))
        .def("clear", BIND_WITH_VALIDATION(&WorkingMemory::clear, "WorkingMemory.clear", "Clear all stored information"))
        .def("getCapacity", BIND_WITH_VALIDATION(&WorkingMemory::getCapacity,
             "WorkingMemory.getCapacity", "Get memory capacity"))
        .def("getCurrentSize", BIND_WITH_VALIDATION(&WorkingMemory::getCurrentSize,
             "WorkingMemory.getCurrentSize", "Get current memory size"))
        .def("decay", BIND_WITH_VALIDATION(&WorkingMemory::decay, "WorkingMemory.decay", py::arg("decayRate"),
             "Decay all stored information"))
        // Advanced memory management methods
        .def("memory.consolidate", BIND_WITH_VALIDATION(&WorkingMemory::consolidate,
             "WorkingMemory.consolidate", py::arg("targetStrength"), py::arg("relevanceThreshold") = 0.8f,
             "Consolidate memory into long-term storage - targetStrength: target consolidation strength, relevanceThreshold: relevance threshold for consolidation"))
        .def("memory.backup", BIND_WITH_VALIDATION(&WorkingMemory::backup,
             "WorkingMemory.backup", py::arg("filename"), py::arg("compressionLevel") = 5,
             "Create memory snapshots - filename: output file, compressionLevel: compression level (1-9)"))
        .def("memory.restore", BIND_WITH_VALIDATION(&WorkingMemory::restore,
             "WorkingMemory.restore", py::arg("filename"), py::arg("verifyChecksum") = true,
             "Restore from memory snapshots - filename: input file, verifyChecksum: verify file integrity"))
        .def("memory.optimize", BIND_WITH_VALIDATION(&WorkingMemory::optimize,
             "WorkingMemory.optimize", py::arg("optimizationStrategy") = "LRU",
             "Optimize memory access patterns - optimizationStrategy: optimization strategy (LRU, LFU, ARC, etc.)"))
        .def("memory.getStats", BIND_WITH_VALIDATION(&WorkingMemory::getStats,
             "WorkingMemory.getStats", "Get memory statistics and usage patterns"))
        .def("memory.optimize_storage", BIND_WITH_VALIDATION(&WorkingMemory::optimizeStorage,
             "WorkingMemory.optimizeStorage", py::arg("targetEfficiency"), py::arg("reorganizationDepth") = 2,
             "Optimize memory storage layout - targetEfficiency: target storage efficiency (0-1), reorganizationDepth: depth of reorganization (0-5)"));

    // EpisodicMemory with simplified bindings
    py::class_<EpisodicMemoryItem>(m, "EpisodicMemoryItem", R"pbdoc(Stored episode of neural activity)pbdoc")
        .def(py::init<>())
        .def_readwrite("timestamp", &EpisodicMemoryItem::timestamp)
        .def_readwrite("neurons", &EpisodicMemoryItem::neurons)
        .def_readwrite("values", &EpisodicMemoryItem::values)
        .def_readwrite("metadata", &EpisodicMemoryItem::metadata);

    py::class_<EpisodicMemory>(m, "EpisodicMemory", R"pbdoc(Storage of events and experiences)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", BIND_WITH_VALIDATION(&EpisodicMemory::storeEpisode, "EpisodicMemory.storeEpisode", py::arg("episode"),
             "Store an episode"))
        .def("retrieveEpisode", BIND_WITH_VALIDATION(&EpisodicMemory::retrieveEpisode,
             "EpisodicMemory.retrieveEpisode", py::arg("index"), "Retrieve an episode"))
        .def("getEpisodeCount", BIND_WITH_VALIDATION(&EpisodicMemory::getEpisodeCount,
             "EpisodicMemory.getEpisodeCount", "Get number of episodes"))
        .def("getRecentEpisodes", BIND_WITH_VALIDATION(&EpisodicMemory::getRecentEpisodes,
             "EpisodicMemory.getRecentEpisodes", py::arg("count"), "Get recent episodes"))
        .def("clear", BIND_WITH_VALIDATION(&EpisodicMemory::clear, "EpisodicMemory.clear", "Clear all episodes"))
        .def("consolidate", BIND_WITH_VALIDATION(&EpisodicMemory::consolidate,
             "EpisodicMemory.consolidate", py::arg("relevanceThreshold"), "Consolidate to long-term memory"))
        // Advanced memory management methods
        .def("memory.consolidate", BIND_WITH_VALIDATION(&EpisodicMemory::consolidate,
             "EpisodicMemory.consolidate", py::arg("targetStrength"), py::arg("relevanceThreshold") = 0.8f,
             "Consolidate memory into long-term storage - targetStrength: target consolidation strength, relevanceThreshold: relevance threshold for consolidation"))
        .def("memory.backup", BIND_WITH_VALIDATION(&EpisodicMemory::backup,
             "EpisodicMemory.backup", py::arg("filename"), py::arg("compressionLevel") = 5,
             "Create memory snapshots - filename: output file, compressionLevel: compression level (1-9)"))
        .def("memory.restore", BIND_WITH_VALIDATION(&EpisodicMemory::restore,
             "EpisodicMemory.restore", py::arg("filename"), py::arg("verifyChecksum") = true,
             "Restore from memory snapshots - filename: input file, verifyChecksum: verify file integrity"))
        .def("memory.optimize", BIND_WITH_VALIDATION(&EpisodicMemory::optimize,
             "EpisodicMemory.optimize", py::arg("optimizationStrategy") = "LRU",
             "Optimize memory access patterns - optimizationStrategy: optimization strategy (LRU, LFU, ARC, etc.)"))
        .def("memory.getStats", BIND_WITH_VALIDATION(&EpisodicMemory::getStats,
             "EpisodicMemory.getStats", "Get memory statistics and usage patterns"))
        .def("memory.optimize_storage", BIND_WITH_VALIDATION(&EpisodicMemory::optimizeStorage,
             "EpisodicMemory.optimizeStorage", py::arg("targetEfficiency"), py::arg("reorganizationDepth") = 2,
             "Optimize memory storage layout - targetEfficiency: target storage efficiency (0-1), reorganizationDepth: depth of reorganization (0-5)"))
        .def("memory.findSimilarEpisodes", BIND_WITH_VALIDATION(&EpisodicMemory::findSimilarEpisodes,
             "EpisodicMemory.findSimilarEpisodes", py::arg("pattern"), py::arg("similarityThreshold"), py::arg("maxResults") = 10,
             "Find similar episodes based on pattern matching - pattern: search pattern, similarityThreshold: minimum similarity (0-1), maxResults: maximum results to return"))
        .def("memory.consolidateWithEviction", BIND_WITH_VALIDATION(&EpisodicMemory::consolidateWithEviction,
             "EpisodicMemory.consolidateWithEviction", py::arg("consolidationRate"), py::arg("evictionStrategy") = "LRU",
             "Consolidate memory with eviction policy - consolidationRate: consolidation rate (0-1), evictionStrategy: eviction strategy (LRU, LFU, FIFO, etc.)"));

    // SemanticMemory with simplified bindings
    py::class_<SemanticMemory>(m, "SemanticMemory", R"pbdoc(Gradually acquired knowledge)pbdoc")
        .def(py::init<>())
        .def("storeFact", BIND_WITH_VALIDATION(&SemanticMemory::storeFact,
             "SemanticMemory.storeFact", py::arg("key"), py::arg("value"), "Store a fact"))
        .def("retrieveFact", BIND_WITH_VALIDATION(&SemanticMemory::retrieveFact, "SemanticMemory.retrieveFact", py::arg("key"),
             "Retrieve a fact"))
        .def("hasFact", BIND_WITH_VALIDATION(&SemanticMemory::hasFact, "SemanticMemory.hasFact", py::arg("key"),
             "Check if fact exists"))
        .def("getAllFacts", BIND_WITH_VALIDATION(&SemanticMemory::getAllFacts,
             "SemanticMemory.getAllFacts", "Get all facts"))
        .def("clear", BIND_WITH_VALIDATION(&SemanticMemory::clear, "SemanticMemory.clear", "Clear all facts"))
        // Advanced memory management methods
        .def("memory.consolidate", BIND_WITH_VALIDATION(&SemanticMemory::consolidate,
             "SemanticMemory.consolidate", py::arg("targetStrength"), py::arg("relevanceThreshold") = 0.8f,
             "Consolidate memory into long-term storage - targetStrength: target consolidation strength, relevanceThreshold: relevance threshold for consolidation"))
        .def("memory.backup", BIND_WITH_VALIDATION(&SemanticMemory::backup,
             "SemanticMemory.backup", py::arg("filename"), py::arg("compressionLevel") = 5,
             "Create memory snapshots - filename: output file, compressionLevel: compression level (1-9)"))
        .def("memory.restore", BIND_WITH_VALIDATION(&SemanticMemory::restore,
             "SemanticMemory.restore", py::arg("filename"), py::arg("verifyChecksum") = true,
             "Restore from memory snapshots - filename: input file, verifyChecksum: verify file integrity"))
        .def("memory.optimize", BIND_WITH_VALIDATION(&SemanticMemory::optimize,
             "SemanticMemory.optimize", py::arg("optimizationStrategy") = "LRU",
             "Optimize memory access patterns - optimizationStrategy: optimization strategy (LRU, LFU, ARC, etc.)"))
        .def("memory.getStats", BIND_WITH_VALIDATION(&SemanticMemory::getStats,
             "SemanticMemory.getStats", "Get memory statistics and usage patterns"))
        .def("memory.optimize_storage", BIND_WITH_VALIDATION(&SemanticMemory::optimizeStorage,
             "SemanticMemory.optimizeStorage", py::arg("targetEfficiency"), py::arg("reorganizationDepth") = 2,
             "Optimize memory storage layout - targetEfficiency: target storage efficiency (0-1), reorganizationDepth: depth of reorganization (0-5)"))
        .def("memory.inferRelationships", BIND_WITH_VALIDATION(&SemanticMemory::inferRelationships,
             "SemanticMemory.inferRelationships", py::arg("minConfidence") = 0.7f, py::arg("maxDepth") = 3,
             "Infer implicit relationships between stored facts - minConfidence: minimum confidence threshold, maxDepth: maximum relationship depth"))
        .def("memory.findConnectedFacts", BIND_WITH_VALIDATION(&SemanticMemory::findConnectedFacts,
             "SemanticMemory.findConnectedFacts", py::arg("factKey"), py::arg("maxDepth") = 5,
             "Find facts connected to a given fact - factKey: source fact key, maxDepth: maximum connection depth"));

    // ProceduralMemory with simplified bindings
    py::class_<Skill>(m, "Skill", R"pbdoc(Learned skill with neural pattern)pbdoc")
        .def(py::init<>())
        .def_readwrite("name", &Skill::name)
        .def_readwrite("neuralPattern", &Skill::neuralPattern)
        .def_readwrite("proficiency", &Skill::proficiency);

    py::class_<ProceduralMemory>(m, "ProceduralMemory", R"pbdoc(Learned skills and habits)pbdoc")
        .def(py::init<>())
        .def("learnSkill", BIND_WITH_VALIDATION(&ProceduralMemory::learnSkill,
             "ProceduralMemory.learnSkill", py::arg("name"), py::arg("pattern"), "Learn a new skill"))
        .def("getSkill", &ProceduralMemory::getSkill, py::arg("name"),
             py::return_value_policy::reference_internal, "Get a skill")
        .def("getAllSkills", BIND_WITH_VALIDATION(&ProceduralMemory::getAllSkills,
             "ProceduralMemory.getAllSkills", "Get all skills"))
        .def("updateProficiency", BIND_WITH_VALIDATION(&ProceduralMemory::updateProficiency,
             "ProceduralMemory.updateProficiency", py::arg("name"), py::arg("delta"), "Update skill proficiency"))
        .def("clear", BIND_WITH_VALIDATION(&ProceduralMemory::clear, "ProceduralMemory.clear", "Clear all skills"))
        // Advanced memory management methods
        .def("memory.consolidate", BIND_WITH_VALIDATION(&ProceduralMemory::consolidate,
             "ProceduralMemory.consolidate", py::arg("targetStrength"), py::arg("relevanceThreshold") = 0.8f,
             "Consolidate memory into long-term storage - targetStrength: target consolidation strength, relevanceThreshold: relevance threshold for consolidation"))
        .def("memory.backup", BIND_WITH_VALIDATION(&ProceduralMemory::backup,
             "ProceduralMemory.backup", py::arg("filename"), py::arg("compressionLevel") = 5,
             "Create memory snapshots - filename: output file, compressionLevel: compression level (1-9)"))
        .def("memory.restore", BIND_WITH_VALIDATION(&ProceduralMemory::restore,
             "ProceduralMemory.restore", py::arg("filename"), py::arg("verifyChecksum") = true,
             "Restore from memory snapshots - filename: input file, verifyChecksum: verify file integrity"))
        .def("memory.optimize", BIND_WITH_VALIDATION(&ProceduralMemory::optimize,
             "ProceduralMemory.optimize", py::arg("optimizationStrategy") = "LRU",
             "Optimize memory access patterns - optimizationStrategy: optimization strategy (LRU, LFU, ARC, etc.)"))
        .def("memory.getStats", BIND_WITH_VALIDATION(&ProceduralMemory::getStats,
             "ProceduralMemory.getStats", "Get memory statistics and usage patterns"))
        .def("memory.optimize_storage", BIND_WITH_VALIDATION(&ProceduralMemory::optimizeStorage,
             "ProceduralMemory.optimizeStorage", py::arg("targetEfficiency"), py::arg("reorganizationDepth") = 2,
             "Optimize memory storage layout - targetEfficiency: target storage efficiency (0-1), reorganizationDepth: depth of reorganization (0-5)"))
        .def("memory.transferSkillToLongTerm", BIND_WITH_VALIDATION(&ProceduralMemory::transferSkillToLongTerm,
             "ProceduralMemory.transferSkillToLongTerm", py::arg("skillName"), py::arg("stabilityThreshold"), py::arg("relevanceThreshold"),
             "Transfer skill to long-term memory with stability and relevance criteria"))
        .def("memory.pruneWeakSkills", BIND_WITH_VALIDATION(&ProceduralMemory::pruneWeakSkills,
             "ProceduralMemory.pruneWeakSkills", py::arg("proficiencyThreshold"), py::arg("ageThreshold"),
             "Prune skills below proficiency or age thresholds"));

    // AssociativeMemory with simplified bindings
    py::class_<AssociativeMemory>(m, "AssociativeMemory", R"pbdoc(Relationships between representations)pbdoc")
        .def(py::init<>())
        .def("associate", BIND_WITH_VALIDATION(&AssociativeMemory::associate,
             "AssociativeMemory.associate", py::arg("a"), py::arg("b"), py::arg("strength"), "Create association"))
        .def("getAssociations", BIND_WITH_VALIDATION(&AssociativeMemory::getAssociations, "AssociativeMemory.getAssociations", py::arg("neuron"),
             "Get associated neurons"))
        .def("getAssociationStrength", BIND_WITH_VALIDATION(&AssociativeMemory::getAssociationStrength,
             "AssociativeMemory.getAssociationStrength", py::arg("a"), py::arg("b"), "Get association strength"))
        .def("updateAssociation", BIND_WITH_VALIDATION(&AssociativeMemory::updateAssociation,
             "AssociativeMemory.updateAssociation", py::arg("a"), py::arg("b"), py::arg("delta"), "Update association"))
        .def("clear", BIND_WITH_VALIDATION(&AssociativeMemory::clear, "AssociativeMemory.clear", "Clear all associations"))
        // Advanced memory management methods
        .def("memory.consolidate", BIND_WITH_VALIDATION(&AssociativeMemory::consolidate,
             "AssociativeMemory.consolidate", py::arg("targetStrength"), py::arg("relevanceThreshold") = 0.8f,
             "Consolidate memory into long-term storage - targetStrength: target consolidation strength, relevanceThreshold: relevance threshold for consolidation"))
        .def("memory.backup", BIND_WITH_VALIDATION(&AssociativeMemory::backup,
             "AssociativeMemory.backup", py::arg("filename"), py::arg("compressionLevel") = 5,
             "Create memory snapshots - filename: output file, compressionLevel: compression level (1-9)"))
        .def("memory.restore", BIND_WITH_VALIDATION(&AssociativeMemory::restore,
             "AssociativeMemory.restore", py::arg("filename"), py::arg("verifyChecksum") = true,
             "Restore from memory snapshots - filename: input file, verifyChecksum: verify file integrity"))
        .def("memory.optimize", BIND_WITH_VALIDATION(&AssociativeMemory::optimize,
             "AssociativeMemory.optimize", py::arg("optimizationStrategy") = "LRU",
             "Optimize memory access patterns - optimizationStrategy: optimization strategy (LRU, LFU, ARC, etc.)"))
        .def("memory.getStats", BIND_WITH_VALIDATION(&AssociativeMemory::getStats,
             "AssociativeMemory.getStats", "Get memory statistics and usage patterns"))
        .def("memory.optimize_storage", BIND_WITH_VALIDATION(&AssociativeMemory::optimizeStorage,
             "AssociativeMemory.optimizeStorage", py::arg("targetEfficiency"), py::arg("reorganizationDepth") = 2,
             "Optimize memory storage layout - targetEfficiency: target storage efficiency (0-1), reorganizationDepth: depth of reorganization (0-5)"))
        .def("memory.detectForgettingCurves", BIND_WITH_VALIDATION(&AssociativeMemory::detectForgettingCurves,
             "AssociativeMemory.detectForgettingCurves", py::arg("timeWindow"), py::arg("minDataPoints") = 5,
             "Detect forgetting curves for associations - timeWindow: time window for analysis, minDataPoints: minimum data points required"))
        .def("memory.predictAssociationDecay", BIND_WITH_VALIDATION(&AssociativeMemory::predictAssociationDecay,
             "AssociativeMemory.predictAssociationDecay", py::arg("startTime"), py::arg("endTime"), py::arg("targetStrength"),
             "Predict when associations will decay below threshold"));

    // DevelopmentSystem with enhanced validation
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Orchestrates neural development)pbdoc")
        .def(py::init<>())
        .def("getStage", BIND_WITH_VALIDATION(&DevelopmentSystem::getStage,
             "DevelopmentSystem.getStage", "Get current developmental stage"))
        .def("setStage", BIND_WITH_VALIDATION(&DevelopmentSystem::setStage,
             "DevelopmentSystem.setStage", py::arg("stage"), "Set developmental stage"))
        .def("advanceStage", BIND_WITH_VALIDATION(&DevelopmentSystem::advanceStage,
             "DevelopmentSystem.advanceStage", "Advance to next developmental stage"))
        .def("getStageName", BIND_WITH_VALIDATION(&DevelopmentSystem::getStageName,
             "DevelopmentSystem.getStageName", "Get current stage name"))
        .def("update", BIND_WITH_VALIDATION(&DevelopmentSystem::update,
             "DevelopmentSystem.update", py::arg("brain"), py::arg("currentStep"),
             "Update development (simplified)"))
        .def("update", BIND_WITH_VALIDATION(&DevelopmentSystem::update,
             "DevelopmentSystem.update", py::arg("brain"), py::arg("rng"), py::arg("dt"),
             "Update development with parameters"))
        .def("getPlasticityModifier", BIND_WITH_VALIDATION(&DevelopmentSystem::getPlasticityModifier,
             "DevelopmentSystem.getPlasticityModifier", "Get plasticity modifier for current stage"))
        .def("isCriticalPeriod", BIND_WITH_VALIDATION(&DevelopmentSystem::isCriticalPeriod,
             "DevelopmentSystem.isCriticalPeriod", "Check if in critical period"))
        .def("getCriticalPeriodProgress", BIND_WITH_VALIDATION(&DevelopmentSystem::getCriticalPeriodProgress,
             "DevelopmentSystem.getCriticalPeriodProgress", "Get critical period progress (0-1)"))
        .def("getDevelopmentalAge", BIND_WITH_VALIDATION(&DevelopmentSystem::getDevelopmentalAge,
             "DevelopmentSystem.getDevelopmentalAge", "Get developmental age (in simulation time)"))
        .def("setDevelopmentalAge", BIND_WITH_VALIDATION(&DevelopmentSystem::setDevelopmentalAge,
             "DevelopmentSystem.setDevelopmentalAge", py::arg("age"), "Set developmental age"))
        // Advanced development control methods
        .def("development.set_critical_period", BIND_WITH_VALIDATION(&DevelopmentSystem::setCriticalPeriod,
             "DevelopmentSystem.setCriticalPeriod", py::arg("stage"), py::arg("startAge"), py::arg("endAge"), py::arg("plasticityMultiplier"),
             "Control developmental periods - stage: developmental stage, startAge: start age, endAge: end age, plasticityMultiplier: plasticity multiplier"))
        .def("development.get_developmental_stage", BIND_WITH_VALIDATION(&DevelopmentSystem::getDevelopmentalStage,
             "DevelopmentSystem.getDevelopmentalStage", "Get current developmental stage"))
        .def("development.schedule_maturation", BIND_WITH_VALIDATION(&DevelopmentSystem::scheduleMaturation,
             "DevelopmentSystem.scheduleMaturation", py::arg("maturationType"), py::arg("targetAge"), py::arg("parameters"),
             "Schedule maturation events - maturationType: type of maturation, targetAge: target age, parameters: event parameters"))
        .def("development.get_plasticity_schedule", BIND_WITH_VALIDATION(&DevelopmentSystem::getPlasticitySchedule,
             "DevelopmentSystem.getPlasticitySchedule", "Get plasticity schedule"))
        .def("development.setPlasticityParameters", BIND_WITH_VALIDATION(&DevelopmentSystem::setPlasticityParameters,
             "DevelopmentSystem.setPlasticityParameters", py::arg("hebbianStrength"), py::arg("stdpStrength"), py::arg("rewardModulationStrength"),
             "Set plasticity parameters for development"))
        .def("development.getDevelopmentStats", BIND_WITH_VALIDATION(&DevelopmentSystem::getDevelopmentStats,
             "DevelopmentSystem.getDevelopmentStats", "Get development statistics"))
        .def("development.analyzeMaturationTrajectory", BIND_WITH_VALIDATION(&DevelopmentSystem::analyzeMaturationTrajectory,
             "DevelopmentSystem.analyzeMaturationTrajectory", py::arg("targetStage"), py::arg("timeHorizon"),
             "Analyze maturation trajectory for target stage - targetStage: target developmental stage, timeHorizon: time horizon for analysis"));

    // Synaptogenesis with validation
    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Neural connectivity formation)pbdoc")
        .def(py::init<>())
        .def("getFormationRate", BIND_WITH_VALIDATION(&Synaptogenesis::getFormationRate,
             "Synaptogenesis.getFormationRate", "Get synaptogenesis formation rate"))
        .def("setFormationRate", BIND_WITH_VALIDATION(&Synaptogenesis::setFormationRate,
             "Synaptogenesis.setFormationRate", py::arg("rate"), "Set formation rate"))
        .def("getTargetDensity", BIND_WITH_VALIDATION(&Synaptogenesis::getTargetDensity,
             "Synaptogenesis.getTargetDensity", "Get target synaptic density"))
        .def("setTargetDensity", BIND_WITH_VALIDATION(&Synaptogenesis::setTargetDensity,
             "Synaptogenesis.setTargetDensity", py::arg("density"), "Set target density"))
        .def("update", BIND_WITH_VALIDATION(&Synaptogenesis::update,
             "Synaptogenesis.update", py::arg("brain"), py::arg("rng"), "Update synaptogenesis"));

    // Maturation with validation
    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation and parameter adjustment)pbdoc")
        .def(py::init<>())
        .def("getProgress", BIND_WITH_VALIDATION(&Maturation::getProgress,
             "Maturation.getProgress", "Get maturation progress (0-1)"))
        .def("setProgress", BIND_WITH_VALIDATION(&Maturation::setProgress,
             "Maturation.setProgress", py::arg("progress"), "Set maturation progress"))
        .def("update", BIND_WITH_VALIDATION(&Maturation::update,
             "Maturation.update", py::arg("brain"), py::arg("currentStep"),
             "Update maturation process"))
        .def("getMatureThreshold", BIND_WITH_VALIDATION(&Maturation::getMatureThreshold,
             "Maturation.getMatureThreshold", "Get mature firing threshold"))
        .def("getMatureRestingPotential", BIND_WITH_VALIDATION(&Maturation::getMatureRestingPotential,
             "Maturation.getMatureRestingPotential", "Get mature resting potential"))
        .def("getMatureTimeConstant", BIND_WITH_VALIDATION(&Maturation::getMatureTimeConstant,
             "Maturation.getMatureTimeConstant", "Get mature time constant"));

    // Pruning with validation
    py::class_<Pruning>(m, "Pruning", R"pbdoc(Synaptic elimination and refinement)pbdoc")
        .def(py::init<>())
        .def("getThreshold", BIND_WITH_VALIDATION(&Pruning::getThreshold,
             "Pruning.getThreshold", "Get pruning threshold"))
        .def("setThreshold", BIND_WITH_VALIDATION(&Pruning::setThreshold,
             "Pruning.setThreshold", py::arg("threshold"), "Set pruning threshold"))
        .def("getPruningRate", BIND_WITH_VALIDATION(&Pruning::getPruningRate,
             "Pruning.getPruningRate", "Get pruning rate"))
        .def("setPruningRate", BIND_WITH_VALIDATION(&Pruning::setPruningRate,
             "Pruning.setPruningRate", py::arg("rate"), "Set pruning rate"))
        .def("update", BIND_WITH_VALIDATION(&Pruning::update,
             "Pruning.update", py::arg("brain"), py::arg("rng"), "Update pruning process"))
        .def("pruneSynapse", BIND_WITH_VALIDATION(&Pruning::pruneSynapse, "Pruning.pruneSynapse", py::arg("synapse"),
             "Check if synapse should be pruned"));

    // Factory functions with enhanced validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    .def("createBrain", BIND_WITH_VALIDATION([](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, "createBrain", py::arg("config"), "Create a new brain with configuration"))
    .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
    .def("createAgentBrain", BIND_WITH_VALIDATION([](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, "createAgentBrain", py::arg("brain"), "Create a new agent brain interface"));

    // Constants (already present)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add Python package metadata
    m.doc() = "NLM (Neural Learning Machine) Python Bindings - Enhanced with error handling and type safety";
}

} // namespace nlm
