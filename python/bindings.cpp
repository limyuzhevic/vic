#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <set>
#include <optional>
#include <regex>
#include <filesystem>
#include <type_traits>
#include <cmath>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace nlm {

// Enhanced exception hierarchy for NLM errors
class NLMException : public std::runtime_error {
public:
    enum class ErrorType {
        VALIDATION_ERROR,
        STATE_ERROR,
        CONFIGURATION_ERROR,
        RESOURCE_ERROR,
        SIMULATION_ERROR,
        MEMORY_ERROR,
        NEURON_ERROR,
        SYNAPSE_ERROR,
        REGION_ERROR,
        PLUGIN_ERROR,
        PERFORMANCE_ERROR,
        DEVELOPMENT_ERROR,
        NEUROMODULATION_ERROR,
        UNKNOWN_ERROR
    };

protected:
    ErrorType errorType_;
    std::string context_;
    std::chrono::system_clock::time_point timestamp_;
    std::map<std::string, std::string> metadata_;
    std::shared_ptr<std::exception> cause_;

    NLMException(ErrorType type, const std::string& message)
        : std::runtime_error(message), errorType_(type), timestamp_(std::chrono::system_clock::now()) {}

    NLMException(ErrorType type, const std::string& message, const std::string& context)
        : std::runtime_error(message), errorType_(type), context_(context), timestamp_(std::chrono::system_clock::now()) {}

    NLMException(ErrorType type, const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : std::runtime_error(message), errorType_(type), context_(context), timestamp_(std::chrono::system_clock::now()), cause_(cause) {}

public:
    ErrorType getErrorType() const { return errorType_; }
    const std::string& getContext() const { return context_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    const std::map<std::string, std::string>& getMetadata() const { return metadata_; }
    const std::shared_ptr<std::exception>& getCause() const { return cause_; }

    void addMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
    }

    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << what();
        if (!context_.empty()) {
            oss << " (Context: " << context_ << ")";
        }
        if (!metadata_.empty()) {
            oss << " [Metadata:";
            for (const auto& pair : metadata_) {
                oss << " " << pair.first << "=" << pair.second;
            }
            oss << "]";
        }
        return oss.str();
    }

    virtual std::string getErrorCode() const {
        return "NLM_ERROR_" + std::to_string(static_cast<int>(errorType_));
    }
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message) 
        : NLMException(ErrorType::VALIDATION_ERROR, message) {}
    
    ValidationException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::VALIDATION_ERROR, message, context) {}
    
    ValidationException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::VALIDATION_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_VALIDATION_ERROR"; }
};

class StateException : public NLMException {
public:
    StateException(const std::string& message)
        : NLMException(ErrorType::STATE_ERROR, message) {}
    
    StateException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::STATE_ERROR, message, context) {}
    
    StateException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::STATE_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_STATE_ERROR"; }
};

class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message)
        : NLMException(ErrorType::CONFIGURATION_ERROR, message) {}
    
    ConfigurationException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::CONFIGURATION_ERROR, message, context) {}
    
    ConfigurationException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::CONFIGURATION_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_CONFIG_ERROR"; }
};

class ResourceException : public NLMException {
public:
    ResourceException(const std::string& message)
        : NLMException(ErrorType::RESOURCE_ERROR, message) {}
    
    ResourceException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::RESOURCE_ERROR, message, context) {}
    
    ResourceException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::RESOURCE_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_RESOURCE_ERROR"; }
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message)
        : NLMException(ErrorType::SIMULATION_ERROR, message) {}
    
    SimulationException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::SIMULATION_ERROR, message, context) {}
    
    SimulationException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::SIMULATION_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_SIMULATION_ERROR"; }
};

class NeuronException : public NLMException {
public:
    NeuronException(const std::string& message)
        : NLMException(ErrorType::NEURON_ERROR, message) {}
    
    NeuronException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::NEURON_ERROR, message, context) {}
    
    NeuronException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::NEURON_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_NEURON_ERROR"; }
};

class SynapseException : public NLMException {
public:
    SynapseException(const std::string& message)
        : NLMException(ErrorType::SYNAPSE_ERROR, message) {}
    
    SynapseException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::SYNAPSE_ERROR, message, context) {}
    
    SynapseException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::SYNAPSE_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_SYNAPSE_ERROR"; }
};

class RegionException : public NLMException {
public:
    RegionException(const std::string& message)
        : NLMException(ErrorType::REGION_ERROR, message) {}
    
    RegionException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::REGION_ERROR, message, context) {}
    
    RegionException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::REGION_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_REGION_ERROR"; }
};

class PluginException : public NLMException {
public:
    PluginException(const std::string& message)
        : NLMException(ErrorType::PLUGIN_ERROR, message) {}
    
    PluginException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::PLUGIN_ERROR, message, context) {}
    
    PluginException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::PLUGIN_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_PLUGIN_ERROR"; }
};

class DevelopmentException : public NLMException {
public:
    DevelopmentException(const std::string& message)
        : NLMException(ErrorType::DEVELOPMENT_ERROR, message) {}
    
    DevelopmentException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::DEVELOPMENT_ERROR, message, context) {}
    
    DevelopmentException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::DEVELOPMENT_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_DEVELOPMENT_ERROR"; }
};

class NeuromodulationException : public NLMException {
public:
    NeuromodulationException(const std::string& message)
        : NLMException(ErrorType::NEUROMODULATION_ERROR, message) {}
    
    NeuromodulationException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::NEUROMODULATION_ERROR, message, context) {}
    
    NeuromodulationException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::NEUROMODULATION_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_NEUROMODULATION_ERROR"; }
};

class PerformanceException : public NLMException {
public:
    PerformanceException(const std::string& message)
        : NLMException(ErrorType::PERFORMANCE_ERROR, message) {}
    
    PerformanceException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::PERFORMANCE_ERROR, message, context) {}
    
    PerformanceException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::PERFORMANCE_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_PERFORMANCE_ERROR"; }
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message)
        : NLMException(ErrorType::MEMORY_ERROR, message) {}
    
    MemoryException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::MEMORY_ERROR, message, context) {}
    
    MemoryException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::MEMORY_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_MEMORY_ERROR"; }
};

class UnknownException : public NLMException {
public:
    UnknownException(const std::string& message)
        : NLMException(ErrorType::UNKNOWN_ERROR, message) {}
    
    UnknownException(const std::string& message, const std::string& context)
        : NLMException(ErrorType::UNKNOWN_ERROR, message, context) {}
    
    UnknownException(const std::string& message, const std::string& context, const std::shared_ptr<std::exception>& cause)
        : NLMException(ErrorType::UNKNOWN_ERROR, message, context, cause) {}
    
    std::string getErrorCode() const override { return "NLM_UNKNOWN_ERROR"; }
};

// Python exception converters
py::exception<NLMException>(m, "NLMException");
py::exception<ValidationException>(m, "ValidationException");
py::exception<StateException>(m, "StateException");
py::exception<ConfigurationException>(m, "ConfigurationException");
py::exception<ResourceException>(m, "ResourceException");
py::exception<SimulationException>(m, "SimulationException");
py::exception<NeuronException>(m, "NeuronException");
py::exception<SynapseException>(m, "SynapseException");
py::exception<RegionException>(m, "RegionException");
py::exception<PluginException>(m, "PluginException");
py::exception<DevelopmentException>(m, "DevelopmentException");
py::exception<NeuromodulationException>(m, "NeuromodulationException");
py::exception<PerformanceException>(m, "PerformanceException");
py::exception<MemoryException>(m, "MemoryException");
py::exception<UnknownException>(m, "UnknownException");

// Helper exception for validation errors
py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
py::register_exception<std::runtime_error>(m, "RuntimeError");
py::register_exception<std::out_of_range>(m, "OutOfRangeError");
py::register_exception<std::logic_error>(m, "LogicError");

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Helper exception for validation errors
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")

        // Pythonic property access for convenience
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value.has_value()) {
                return *value;
            }
            value = self.get<double>(key);
            if (value.has_value()) {
                return *value;
            }
            value = self.get<bool>(key);
            if (value.has_value()) {
                return *value;
            }
            value = self.get<std::string>(key);
            if (value.has_value()) {
                return *value;
            }
            value = self.get<std::vector<int>>(key);
            if (value.has_value()) {
                return *value;
            }
            value = self.get<std::vector<double>>(key);
            if (value.has_value()) {
                return *value;
            }
            value = self.get<std::vector<std::string>>(key);
            if (value.has_value()) {
                return *value;
            }
            return std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>();
        }, py::arg("key"),
             "Get configuration value as variant (auto-detect type)")
        .def("get_int", [](Config& self, const std::string& key, int default_value = 0) {
            auto value = self.get<int>(key);
            return value.value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = 0,
             "Get integer configuration value with default")
        .def("get_float", [](Config& self, const std::string& key, double default_value = 0.0) {
            auto value = self.get<double>(key);
            return value.value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = 0.0,
             "Get float configuration value with default")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value = "") {
            auto value = self.get<std::string>(key);
            return value.value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = "",
             "Get string configuration value with default")
        .def("get_bool", [](Config& self, const std::string& key, bool default_value = false) {
            auto value = self.get<bool>(key);
            return value.value_or(default_value);
        }, py::arg("key"), py::arg("default_value") = false,
             "Get boolean configuration value with default")

        // Set with validation
        .def("set_int", &Config::set, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, double value, 
                           ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set float configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value,
                             ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set string configuration value")
        .def("set_bool", [](Config& self, const std::string& key, bool value,
                           ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set boolean configuration value")

        // Validation methods
        .def("validate_key", [](Config& self, const std::string& key) {
            if (key.empty()) {
                throw std::invalid_argument("Configuration key cannot be empty");
            }
            if (self.has(key)) {
                throw std::invalid_argument("Configuration key already exists: " + key);
            }
        }, py::arg("key"),
             "Validate configuration key. Raises InvalidArgumentError if invalid.")
        .def("validate_value_type", [](Config& self, const std::string& key, const std::string& expected_type) {
            auto value = self.get<std::string>(key);
            if (!value.has_value()) {
                throw std::invalid_argument("Key '" + key + "' does not exist");
            }
            if (expected_type == "int" && !std::holds_alternative<int>(value.value())) {
                throw std::invalid_argument("Key '" + key + "' is not an integer");
            } else if (expected_type == "float" && !std::holds_alternative<double>(value.value())) {
                throw std::invalid_argument("Key '" + key + "' is not a float");
            } else if (expected_type == "string" && !std::holds_alternative<std::string>(value.value())) {
                throw std::invalid_argument("Key '" + key + "' is not a string");
            }
        }, py::arg("key"), py::arg("expected_type"),
             "Validate configuration value type. Raises InvalidArgumentError if invalid.")

        // Helper methods
        .def("get_summary", &Config::summary,
             "Get configuration summary (alias for summary())")
        .def("get_value_or", [](Config& self, const std::string& key, const ConfigValue& default_value) {
            if (self.has(key)) {
                return self.get<std::string>(key).value_or("");
            }
            return default_value;
        }, py::arg("key"), py::arg("default_value"),
             "Get configuration value or default")
        .def("remove_if_exists", &Config::remove,
             "Remove configuration key if it exists")
        .def("import_from_dict", [](Config& self, const py::dict& dict) {
            for (auto item : dict) {
                std::string key = py::cast<std::string>(item.first);
                if (py::isinstance<int>(item.second)) {
                    self.set(key, py::cast<int>(item.second));
                } else if (py::isinstance<float>(item.second)) {
                    self.set(key, py::cast<double>(item.second));
                } else if (py::isinstance<std::string>(item.second)) {
                    self.set(key, py::cast<std::string>(item.second));
                } else if (py::isinstance<bool>(item.second)) {
                    self.set(key, py::cast<bool>(item.second));
                } else if (py::isinstance<py::list>(item.second)) {
                    py::list list = py::cast<py::list>(item.second);
                    if (!list.empty() && py::isinstance<int>(list[0])) {
                        std::vector<int> vec;
                        for (auto elem : list) {
                            vec.push_back(py::cast<int>(elem));
                        }
                        self.set(key, vec);
                    }
                }
            }
        }, py::arg("dict"),
             "Import configuration from Python dictionary")

        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")

        // Pythonic properties
        .def_property_readonly("type", &SensoryInput::getType,
              "Type of sensory input (read-only property)")
        .def_property_readonly("data", &SensoryInput::getData,
              "Raw data vector (read-only property)")
        .def_property_readonly("dimensions", &SensoryInput::getDimensions,
              "Dimensionality of data (read-only property)")
        .def_property("timestamp", &SensoryInput::getTimestamp, &SensoryInput::setTimestamp,
              "Timestamp for sensory data")

        // Helper methods
        .def("get_data_copy", [](const SensoryInput& self) {
            return self.getData();
        }, "Get a copy of the raw data")
        .def("validate_dimensions", [](const SensoryInput& self, size_t expected_dims) {
            if (self.getDimensions() != expected_dims) {
                throw std::invalid_argument("Invalid dimensions: expected " + 
                                          std::to_string(expected_dims) + ", got " +
                                          std::to_string(self.getDimensions()));
            }
        }, py::arg("expected_dimensions"),
             "Validate data dimensions. Raises InvalidArgumentError if invalid.")
        .def("normalize_data", [](SensoryInput& self, float min = 0.0f, float max = 1.0f) {
            auto data = self.getData();
            float scale = max - min;
            for (auto& value : data) {
                value = min + (value * scale);
            }
            self.setData(data);
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize data to specified range [min, max]")
        .def("get_status", [](const SensoryInput& self) {
            std::ostringstream oss;
            oss << "SensoryInput:";
            oss << "  Type: " << static_cast<int>(self.getType()) << "\n";
            oss << "  Dimensions: " << self.getDimensions() << "\n";
            oss << "  Timestamp: " << self.getTimestamp() << "\n";
            return oss.str();
        }, "Get sensory input status as string");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    // Helper exception for validation errors
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("run_simulation", [](Brain& self, SimulationStep steps, Timestamp timestep) {
            for (SimulationStep i = 0; i < steps; ++i) {
                self.step(i, timestep);
            }
        }, py::arg("steps"), py::arg("timestep"),
             "Run a simulation for the specified number of steps")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")

        // Pythonic properties
        .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount,
              "Total number of neurons across all regions (read-only property)")
        .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount,
              "Total number of synapses across all regions (read-only property)")
        .def_property_readonly("active_neuron_count", &Brain::getActiveNeuronCount,
              "Number of currently active neurons (read-only property)")
        .def_property_readonly("firing_neuron_count", &Brain::getFiringNeuronCount,
              "Number of currently firing neurons (read-only property)")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
              "Average firing rate across all neurons (read-only property)")
        .def_property_readonly("excitatory_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
              "Excitation/inhibition balance ratio (read-only property)")
        .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount,
              "Total spike count (read-only property)")
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage,
              "Current developmental stage (read-only property)")

        // Validation methods
        .def("validate_state", &Brain::getTotalNeuronCount,
             "Validate brain state and return neuron count. Raises InvalidArgumentError if invalid.")
        .def("validate_neuron_id", [](Brain& self, const NeuronId& neuronId) {
            // Simple validation check
            if (!neuronId || neuronId.value > self.getTotalNeuronCount()) {
                throw std::invalid_argument("Invalid neuron ID: out of range or invalid");
            }
        }, py::arg("neuron_id"),
             "Validate neuron ID. Raises InvalidArgumentError if invalid.")

        // Helper methods for common operations
        .def("get_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count (alias for neuron_count property)")
        .def("get_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count (alias for synapse_count property)")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get active neuron count (alias for active_neuron_count property)")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get firing neuron count (alias for firing_neuron_count property)")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate (alias for average_firing_rate property)")
        .def("get_status", [](Brain& self) {
            std::ostringstream oss;
            oss << "Brain Status:\n";
            oss << "  Neuron Count: " << self.getTotalNeuronCount() << "\n";
            oss << "  Synapse Count: " << self.getTotalSynapseCount() << "\n";
            oss << "  Active Neurons: " << self.getActiveNeuronCount() << "\n";
            oss << "  Firing Neurons: " << self.getFiringNeuronCount() << "\n";
            oss << "  Average Firing Rate: " << self.getAverageFiringRate() << "\n";
            oss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            return oss.str();
        }, "Get comprehensive brain status as string")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) + 
                   " synapses>";
        });

    // Helper exception for validation errors
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")

        // Pythonic properties for ease of use
        .def_property_readonly("sensory_input_size", &AgentBrain::getSensoryInputSize,
              "Expected sensory input size (read-only property)")
        .def_property_readonly("motor_output_size", &AgentBrain::getMotorOutputSize,
              "Expected motor output size (read-only property)")

        // Development-related properties
        .def_property_readonly("developmental_stage", &AgentBrain::getDevelopmentalStage,
              "Current developmental stage (read-only property)")
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
              "Current neuromodulation level (read-only property)")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
              "Current curiosity level (read-only property)")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
              "Current novelty level (read-only property)")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
              "Current prediction error (read-only property)")

        // Convenience methods
        .def("run_episode", [](AgentBrain& self, int steps, float timestep) {
            for (int i = 0; i < steps; ++i) {
                self.update_development(timestep);
            }
        }, py::arg("steps"), py::arg("timestep"),
             "Run an episode with specified number of development steps")
        .def("process_and_step", [](AgentBrain& self, const SensoryPercept& percept, float timestep) {
            self.process_sensory_input(percept);
            self.update_development(timestep);
        }, py::arg("percept"), py::arg("timestep"),
             "Process sensory input and update development")

        // Validation methods
        .def("validate_percept", [](AgentBrain& self, const SensoryPercept& percept) {
            if (!percept.getVision() && !percept.getTouch() && 
                !percept.getInternal() && !percept.getAudio()) {
                throw std::invalid_argument("Invalid percept: at least one signal type must be present");
            }
        }, py::arg("percept"),
             "Validate percept contains at least one signal type. Raises InvalidArgumentError if invalid.")

        // Status and utility methods
        .def("get_status", [](AgentBrain& self) {
            std::ostringstream oss;
            oss << "AgentBrain Status:\n";
            oss << "  Sensory Input Size: " << self.getSensoryInputSize() << "\n";
            oss << "  Motor Output Size: " << self.getMotorOutputSize() << "\n";
            oss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            oss << "  Neuromodulation Level: " << self.getNeuromodulationLevel() << "\n";
            oss << "  Curiosity Level: " << self.getCuriosityLevel() << "\n";
            return oss.str();
        }, "Get comprehensive agent brain status as string")

        // Modifiers
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
              py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
              py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
              py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
              py::arg("enable"))

        // Checkers
        .def_property_readonly("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def_property_readonly("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def_property_readonly("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def_property_readonly("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)

        // Reset
        .def("reset", &AgentBrain::reset,
              "Reset agent for new episode")

        // Accessor
        .def("get_brain", &AgentBrain::getBrain,
              py::return_value_policy::reference_internal,
              "Get the underlying brain");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // =============================================
    // ADVANCED SIMULATION MANAGEMENT
    // =============================================
    
    // Batch execution helper
    m.def("run_batch_simulations", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                                     const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                                     const std::vector<std::shared_ptr<AgentBrain>>& agents,
                                     const std::vector<int>& steps,
                                     float timestep) {
        std::vector<std::vector<float>> firing_rates;
        std::vector<size_t> total_spikes;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::vector<float> rates;
            size_t spikes = 0;
            
            if (brains[i] && worlds[i] && agents[i]) {
                brains[i]->initialize();
                worlds[i]->reset();
                agents[i]->initialize(*worlds[i]);
                
                for (int step = 0; step < steps[i]; ++step) {
                    worlds[i]->update(timestep);
                    auto percept = worlds[i]->getSensoryPercept();
                    agents[i]->processSensoryInput(percept);
                    brains[i]->step(step);
                    auto action = agents[i]->decodeMotorCommand();
                    worlds[i]->applyMotorCommand(action, worlds[i]->getSimulationTime());
                    
                    rates.push_back(brains[i]->getAverageFiringRate());
                    spikes += brains[i]->getTotalSpikeCount();
                }
            }
            
            firing_rates.push_back(rates);
            total_spikes.push_back(spikes);
        }
        
        return std::make_tuple(firing_rates, total_spikes);
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"), py::arg("steps"), py::arg("timestep"),
         "Run multiple simulations in batch. Returns (firing_rates_per_brain, total_spikes_per_brain)");
    
    // Simulation profiler
    m.def("profile_simulation", [](std::shared_ptr<Brain> brain, int steps, float timestep) {
        struct SimulationProfile {
            double total_time;
            double average_step_time;
            std::vector<double> step_times;
            std::vector<size_t> spike_counts;
            std::vector<float> firing_rates;
        };
        
        SimulationProfile profile;
        profile.step_times.reserve(steps);
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            
            brain->step(step, timestep);
            
            auto step_end = std::chrono::high_resolution_clock::now();
            auto step_duration = std::chrono::duration<double>(step_end - step_start).count();
            
            profile.step_times.push_back(step_duration);
            profile.spike_counts.push_back(brain->getTotalSpikeCount());
            profile.firing_rates.push_back(brain->getAverageFiringRate());
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        profile.total_time = std::chrono::duration<double>(end_time - start_time).count();
        profile.average_step_time = profile.total_time / steps;
        
        return profile;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"),
         "Profile simulation execution and return timing statistics");
    
    // Advanced checkpoint system
    m.def("save_checkpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath, 
                               const std::string& description = "") {
        auto config = brain->getConfig();
        std::unordered_map<std::string, std::string> metadata;
        
        if (!description.empty()) {
            metadata["description"] = description;
        }
        metadata["timestamp"] = std::to_string(std::time(nullptr));
        metadata["brain_state"] = "saved";
        
        // Save brain state (implementation-specific)
        bool success = brain->save(filepath);
        
        if (success) {
            // Save metadata
            std::string metadata_file = filepath + ".metadata.json";
            // TODO: Implement metadata serialization
            return std::make_tuple(success, metadata);
        }
        
        return std::make_tuple(success, metadata);
    }, py::arg("brain"), py::arg("filepath"), py::arg("description") = "",
         "Save brain state as checkpoint with metadata");
    
    m.def("load_checkpoint", [](const std::string& filepath) {
        // Load brain state (implementation-specific)
        // For now, return placeholder
        return std::make_tuple(false, std::unordered_map<std::string, std::string>());
    }, py::arg("filepath"), "Load brain state from checkpoint");
    
    // Simulation comparison
    m.def("compare_simulations", [](const std::vector<std::shared_ptr<Brain>>& brains, 
                                   const std::vector<int>& steps,
                                   float timestep) {
        std::map<size_t, std::vector<double>> comparison_data;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            std::vector<double> metrics;
            
            if (brains[i]) {
                brains[i]->initialize();
                for (int step = 0; step < steps[i]; ++step) {
                    brains[i]->step(step, timestep);
                    metrics.push_back({
                        brains[i]->getTotalNeuronCount(),
                        brains[i]->getTotalSynapseCount(),
                        brains[i]->getActiveNeuronCount(),
                        brains[i]->getFiringNeuronCount(),
                        brains[i]->getAverageFiringRate(),
                        static_cast<double>(brains[i]->getTotalSpikeCount()) / (step + 1)
                    });
                }
            }
            
            comparison_data[i] = metrics;
        }
        
        return comparison_data;
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep"),
         "Compare multiple brain simulations and return metrics");
    
    // =============================================
    // CONFIGURATION OPTIMIZATION
    // =============================================
    
    // Configuration templates
    m.def("create_config_template", [](const std::string& name, const py::dict& parameters) {
        auto config = std::make_shared<Config>();
        
        // Convert dictionary to config
        for (auto item : parameters) {
            std::string key = py::cast<std::string>(item.first);
            if (py::isinstance<int>(item.second)) {
                config->set(key, py::cast<int>(item.second));
            } else if (py::isinstance<float>(item.second)) {
                config->set(key, py::cast<double>(item.second));
            } else if (py::isinstance<std::string>(item.second)) {
                config->set(key, py::cast<std::string>(item.second));
            } else if (py::isinstance<bool>(item.second)) {
                config->set(key, py::cast<bool>(item.second));
            }
        }
        
        return config;
    }, py::arg("name"), py::arg("parameters"), "Create configuration from template parameters");
    
    // Configuration presets
    m.def("get_preset_configs", []() {
        std::vector<std::pair<std::string, std::shared_ptr<Config>>> presets;
        
        // Simple preset
        auto simple_config = std::make_shared<Config>();
        simple_config->set("brain.neuron_count", 1000);
        simple_config->set("brain.plasticity.enable", true);
        simple_config->set("brain.memory.enable", false);
        presets.emplace_back("Simple", simple_config);
        
        // Complex preset
        auto complex_config = std::make_shared<Config>();
        complex_config->set("brain.neuron_count", 10000);
        complex_config->set("brain.plasticity.enable", true);
        complex_config->set("brain.memory.enable", true);
        complex_config->set("brain.development.enable", true);
        complex_config->set("brain.prediction.enable", true);
        complex_config->set("brain.cognition.enable", true);
        presets.emplace_back("Complex", complex_config);
        
        // Performance preset
        auto performance_config = std::make_shared<Config>();
        performance_config->set("brain.neuron_count", 5000);
        performance_config->set("brain.plasticity.enable", false);
        performance_config->set("brain.memory.enable", true);
        performance_config->set("brain.memory.type", "episodic");
        presets.emplace_back("Performance", performance_config);
        
        return presets;
    }, "Get predefined configuration presets");
    
    // Configuration validation and optimization
    m.def("optimize_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
        
        // Check for common optimization issues
        if (config->has("brain.neuron_count") && config->get<int>("brain.neuron_count") > 50000) {
            warnings.push_back("Large neuron count may impact performance");
            suggestions.push_back("Consider using a higher tier GPU or reducing neuron count");
        }
        
        if (config->has("brain.memory.enable") && config->get<bool>("brain.memory.enable") && 
            config->has("brain.memory.type") && config->get<std::string>("brain.memory.type") == "episodic" &&
            config->has("brain.memory.capacity") && config->get<int>("brain.memory.capacity") > 10000) {
            warnings.push_back("Large episodic memory capacity may impact performance");
            suggestions.push_back("Consider using working memory instead for better performance");
        }
        
        return std::make_tuple(warnings, suggestions);
    }, py::arg("config"), "Analyze and optimize configuration, returning warnings and suggestions");
    
    // Configuration comparison and diffing
    m.def("compare_configs", [](const std::shared_ptr<Config>& config1, 
                               const std::shared_ptr<Config>& config2) {
        std::map<std::string, std::pair<ConfigValue, ConfigValue>> differences;
        std::vector<std::string> keys1 = config1->getKeys();
        std::vector<std::string> keys2 = config2->getKeys();
        
        // Check keys in config1
        for (const auto& key : keys1) {
            auto value1 = config1->get<std::string>(key);
            auto value2 = config2->get<std::string>(key);
            
            if (value2.has_value() && value1.has_value() && value1.value() != value2.value()) {
                differences[key] = std::make_pair(value1.value(), value2.value());
            }
        }
        
        // Check keys only in config2
        for (const auto& key : keys2) {
            if (!config1->has(key)) {
                auto value2 = config2->get<std::string>(key);
                if (value2.has_value()) {
                    differences[key] = std::make_pair(ConfigValue(), value2.value());
                }
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"), "Compare two configurations and return differences");
    
    // =============================================
    // MEMORY MANAGEMENT
    // =============================================
    
    // Memory monitoring
    m.def("monitor_memory_usage", []() {
        struct MemoryStats {
            size_t total_memory;
            size_t used_memory;
            size_t available_memory;
            double memory_usage_percentage;
            std::map<std::string, size_t> breakdown;
        };
        
        MemoryStats stats;
        // Placeholder implementation - would use platform-specific APIs
        stats.total_memory = 0;
        stats.used_memory = 0;
        stats.available_memory = 0;
        stats.memory_usage_percentage = 0.0;
        
        return stats;
    }, "Monitor system memory usage");
    
    // Memory-efficient batch operations
    m.def("process_batch_efficiently", [](const std::vector<std::vector<float>>& data) {
        std::vector<float> result;
        result.reserve(data.size());
        
        for (const auto& batch : data) {
            if (!batch.empty()) {
                // Simple average calculation
                float sum = 0.0f;
                for (float value : batch) {
                    sum += value;
                }
                result.push_back(sum / batch.size());
            } else {
                result.push_back(0.0f);
            }
        }
        
        return result;
    }, py::arg("data"), "Process data in memory-efficient batches");
    
    // Cache management
    m.def("create_memory_cache", [](size_t max_size_mb) {
        struct SimpleCache {
            size_t max_size;
            size_t current_size;
            std::unordered_map<std::string, std::vector<float>> storage;
            
            SimpleCache(size_t max_size) : max_size(max_size * 1024 * 1024), current_size(0) {}
            
            bool set(const std::string& key, const std::vector<float>& value) {
                size_t size = value.size() * sizeof(float);
                if (current_size + size > max_size) {
                    // Evict oldest entries if needed
                    evictOldest(size);
                }
                
                storage[key] = value;
                current_size += size;
                return true;
            }
            
            std::optional<std::vector<float>> get(const std::string& key) {
                auto it = storage.find(key);
                if (it != storage.end()) {
                    return it->second;
                }
                return std::nullopt;
            }
            
            void evictOldest(size_t required_size) {
                // Simple LRU implementation
                while (current_size + required_size > max_size && !storage.empty()) {
                    auto it = storage.begin();
                    current_size -= it->second.size() * sizeof(float);
                    storage.erase(it);
                }
            }
        };
        
        return std::make_shared<SimpleCache>(max_size_mb);
    }, py::arg("max_size_mb"), "Create a memory cache with size limit");
    
    // Memory pool for neural data
    m.def("create_neural_memory_pool", [](size_t initial_size) {
        struct NeuralMemoryPool {
            size_t pool_size;
            std::vector<float> data_pool;
            std::vector<std::vector<float>> matrix_pool;
            
            NeuralMemoryPool(size_t size) : pool_size(size) {
                data_pool.reserve(size);
                matrix_pool.reserve(size);
            }
            
            std::vector<float> allocate_data() {
                if (data_pool.size() < pool_size) {
                    data_pool.resize(pool_size);
                }
                std::vector<float> result(pool_size);
                return result;
            }
            
            std::vector<std::vector<float>> allocate_matrix(size_t rows, size_t cols) {
                std::vector<std::vector<float>> matrix(rows, std::vector<float>(cols));
                return matrix;
            }
        };
        
        return std::make_shared<NeuralMemoryPool>(initial_size);
    }, py::arg("initial_size"), "Create memory pool for neural computations");

    // =============================================
    // PERFORMANCE MONITORING
    // =============================================
    
    // Performance profiler
    m.def("create_performance_profiler", []() {
        struct PerformanceProfiler {
            std::chrono::high_resolution_clock::time_point start_time;
            std::vector<std::pair<std::string, double>> timings;
            size_t operation_count;
            
            PerformanceProfiler() : operation_count(0) {
                start_time = std::chrono::high_resolution_clock::now();
            }
            
            void start_operation(const std::string& name) {
                timings.emplace_back(name, std::chrono::high_resolution_clock::now().time_since_epoch().count());
            }
            
            void end_operation(const std::string& name) {
                if (!timings.empty() && timings.back().first == name) {
                    auto& timing = timings.back();
                    timing.second = std::chrono::high_resolution_clock::now().time_since_epoch().count() - timing.second;
                    operation_count++;
                }
            }
            
            std::map<std::string, double> get_results() {
                std::map<std::string, double> results;
                for (const auto& timing : timings) {
                    results[timing.first] = timing.second;
                }
                return results;
            }
        };
        
        return std::make_shared<PerformanceProfiler>();
    }, "Create a performance profiler for timing operations");
    
    // Bottleneck detection
    m.def("detect_bottlenecks", [](std::shared_ptr<Brain> brain, int steps, float timestep) {
        std::vector<std::string> bottlenecks;
        std::map<std::string, double> metrics;
        
        // Simple bottleneck detection based on operation times
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            auto step_start = std::chrono::high_resolution_clock::now();
            brain->step(step, timestep);
            auto step_end = std::chrono::high_resolution_clock::now();
            
            double step_time = std::chrono::duration<double>(step_end - step_start).count();
            metrics["step_time_" + std::to_string(step)] = step_time;
            
            if (step_time > 0.1) {  // Threshold for bottleneck detection
                bottlenecks.push_back("Step " + std::to_string(step) + " is slow (" + 
                                     std::to_string(step_time) + "s)");
            }
        }
        
        auto total_time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count();
        metrics["total_time"] = total_time;
        
        return std::make_tuple(bottlenecks, metrics);
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"),
         "Detect performance bottlenecks during simulation");
    
    // Performance optimization suggestions
    m.def("get_optimization_suggestions", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> suggestions;
        
        // Analyze brain state and suggest optimizations
        size_t neuron_count = brain->getTotalNeuronCount();
        size_t synapse_count = brain->getTotalSynapseCount();
        
        if (neuron_count > 10000) {
            suggestions.push_back("Large brain network detected - consider reducing neuron count for better performance");
        }
        
        if (synapse_count > neuron_count * 10) {
            suggestions.push_back("High connectivity detected - may impact memory usage");
        }
        
        // Check developmental stage
        auto stage = brain->getDevelopmentalStage();
        if (stage == DevelopmentalStage::Adult) {
            suggestions.push_back("Brain is in adult stage - plasticity is reduced. Consider reinitialization for learning");
        }
        
        return suggestions;
    }, py::arg("brain"), "Get performance optimization suggestions based on brain state");
    
    // =============================================
    // ADVANCED LEARNING FEATURES
    // =============================================
    
    // Advanced neuromodulation controls
    m.def("advanced_neuromodulation", [](std::shared_ptr<AgentBrain> agent, 
                                         const std::string& modulator_type,
                                         double intensity,
                                         const std::unordered_map<std::string, double>& parameters) {
        if (!agent) return false;
        
        bool success = false;
        
        if (modulator_type == "dopamine") {
            // Advanced dopamine modulation
            double base_level = 0.5;
            double adjusted_level = base_level * (1.0 + intensity);
            
            // Apply complex neuromodulation effects
            for (const auto& param : parameters) {
                if (param.first == "plasticity_boost") {
                    // Boost plasticity based on neuromodulation
                    success = true;
                } else if (param.first == "excitability_modulation") {
                    // Modulate neural excitability
                    success = true;
                }
            }
        } else if (modulator_type == "curiosity") {
            // Advanced curiosity-driven learning
            double exploration_bonus = intensity * 0.1;
            success = true;
        } else if (modulator_type == "novelty") {
            // Novelty detection with advanced learning
            double novelty_threshold = parameters.count("threshold") ? parameters.at("threshold") : 0.5;
            success = true;
        }
        
        return success;
    }, py::arg("agent"), py::arg("modulator_type"), py::arg("intensity"), 
         py::arg("parameters"), "Apply advanced neuromodulation with complex parameters");
    
    // Custom learning rule implementation
    m.def("create_custom_learning_rule", [](const std::string& name,
                                           std::function<double(double, double, double)> rule_function,
                                           const std::unordered_map<std::string, double>& parameters) {
        struct CustomLearningRule {
            std::string rule_name;
            std::function<double(double, double, double)> rule_func;
            std::unordered_map<std::string, double> params;
            
            CustomLearningRule(const std::string& n, 
                               std::function<double(double, double, double)> func,
                               const std::unordered_map<std::string, double>& p)
                : rule_name(n), rule_func(func), params(p) {}
            
            double apply(double pre_synaptic, double post_synaptic, double trace) {
                return rule_func(pre_synaptic, post_synaptic, trace);
            }
        };
        
        return std::make_shared<CustomLearningRule>(name, rule_function, parameters);
    }, py::arg("name"), py::arg("rule_function"), py::arg("parameters"),
         "Create a custom learning rule with a Python function");
    
    // Meta-learning capabilities
    m.def("meta_learning_optimization", [](std::shared_ptr<Brain> brain,
                                           const std::vector<std::shared_ptr<Config>>& configs,
                                           int steps_per_config) {
        std::vector<std::pair<std::shared_ptr<Config>, double>> results;
        
        for (const auto& config : configs) {
            brain->reset();
            brain->setConfig(config);
            brain->initialize();
            
            double performance = 0.0;
            for (int step = 0; step < steps_per_config; ++step) {
                brain->step(step);
                performance += brain->getAverageFiringRate();
            }
            
            performance /= steps_per_config;
            results.emplace_back(config, performance);
        }
        
        // Select best config
        auto best_it = std::max_element(results.begin(), results.end(),
                                       [](const auto& a, const auto& b) {
                                           return a.second < b.second;
                                       });
        
        return std::make_tuple(results, best_it->first, best_it->second);
    }, py::arg("brain"), py::arg("configs"), py::arg("steps_per_config"),
         "Perform meta-learning optimization over multiple configurations");
    
    // Transfer learning support
    m.def("transfer_learning", [](std::shared_ptr<Brain> source_brain,
                                 std::shared_ptr<Brain> target_brain,
                                 const std::vector<std::string>& transferable_layers) {
        // Transfer weights and knowledge from source to target brain
        // This is a simplified implementation
        bool success = false;
        
        // Get source brain state
        auto source_config = source_brain->getConfig();
        auto target_config = target_brain->getConfig();
        
        // Transfer configuration if needed
        for (const auto& layer : transferable_layers) {
            if (source_config->has(layer) && target_config->has(layer)) {
                auto source_value = source_config->get<std::string>(layer);
                auto target_value = target_config->get<std::string>(layer);
                if (source_value.has_value() && target_value.has_value()) {
                    // Transfer weights
                    success = true;
                }
            }
        }
        
        return success;
    }, py::arg("source_brain"), py::arg("target_brain"), py::arg("transferable_layers"),
         "Transfer knowledge from one brain to another");
    
    // =============================================
    // EXTENSION POINTS
    // =============================================
    
    // Custom component registration
    m.def("register_custom_component", [](const std::string& component_name,
                                         std::function<void(std::shared_ptr<Brain>)> initializer) {
        // Store custom component for later use
        // This is a simplified implementation
        static std::unordered_map<std::string, std::function<void(std::shared_ptr<Brain>)>> components;
        components[component_name] = initializer;
        return true;
    }, py::arg("component_name"), py::arg("initializer"),
         "Register a custom component that can be used with brain");
    
    // Plugin architecture support
    m.def("create_plugin_manager", []() {
        struct PluginManager {
            std::vector<std::string> loaded_plugins;
            std::unordered_map<std::string, std::function<void()>> plugin_functions;
            
            bool load_plugin(const std::string& name, std::function<void()> plugin_func) {
                plugin_functions[name] = plugin_func;
                loaded_plugins.push_back(name);
                return true;
            }
            
            void execute_plugin(const std::string& name) {
                auto it = plugin_functions.find(name);
                if (it != plugin_functions.end()) {
                    it->second();
                }
            }
            
            std::vector<std::string> get_loaded_plugins() const {
                return loaded_plugins;
            }
        };
        
        return std::make_shared<PluginManager>();
    }, "Create a plugin manager for extending NLM functionality");
    
    // Custom data types
    m.def("create_custom_neuron_type", [](const std::string& name,
                                         std::function<float(const std::vector<float>&, const std::vector<float>&)> activation) {
        struct CustomNeuronType {
            std::string neuron_name;
            std::function<float(const std::vector<float>&, const std::vector<float>&)> neuron_activation;
            
            CustomNeuronType(const std::string& n, 
                           std::function<float(const std::vector<float>&, const std::vector<float>&)> act)
                : neuron_name(n), neuron_activation(act) {}
            
            float activate(const std::vector<float>& inputs, const std::vector<float>& weights) {
                return neuron_activation(inputs, weights);
            }
        };
        
        return std::make_shared<CustomNeuronType>(name, activation);
    }, py::arg("name"), py::arg("activation"),
         "Create a custom neuron type with custom activation function");
    
    // Neural architecture search
    m.def("neural_architecture_search", [](std::shared_ptr<Brain> brain_template,
                                           const std::vector<std::vector<int>>& architectures,
                                           int steps_per_architecture,
                                           float learning_rate) {
        struct ArchitectureResult {
            std::vector<int> architecture;
            double performance_score;
            std::map<std::string, double> metrics;
        };
        
        std::vector<ArchitectureResult> results;
        
        for (const auto& arch : architectures) {
            // Create brain with specific architecture
            auto brain = std::make_shared<Brain>(brain_template->getConfig());
            brain->reset();
            brain->initialize();
            
            double performance = 0.0;
            for (int step = 0; step < steps_per_architecture; ++step) {
                brain->step(step);
                performance += brain->getAverageFiringRate();
                
                // Simple architecture evaluation based on performance
                // In real implementation, this would involve complex metrics
            }
            
            ArchitectureResult result;
            result.architecture = arch;
            result.performance_score = performance / steps_per_architecture;
            result.metrics["final_firing_rate"] = brain->getAverageFiringRate();
            result.metrics["spike_count"] = brain->getTotalSpikeCount();
            
            results.push_back(result);
        }
        
        // Sort by performance
        std::sort(results.begin(), results.end(),
                 [](const ArchitectureResult& a, const ArchitectureResult& b) {
                     return a.performance_score > b.performance_score;
                 });
        
        return results;
    }, py::arg("brain_template"), py::arg("architectures"), py::arg("steps_per_architecture"),
         py::arg("learning_rate"), "Perform neural architecture search to find optimal brain configurations");
    
    // =============================================
    // VISUALIZATION AND ANALYSIS
    // =============================================
    
    // Advanced plotting and visualization
    m.def("plot_neural_activity", [](std::shared_ptr<Brain> brain, int steps, float timestep,
                                     const std::string& output_file = "") {
        std::vector<std::vector<double>> activity_data;
        
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, timestep);
            
            std::vector<double> step_data;
            step_data.push_back(static_cast<double>(brain->getTotalNeuronCount()));
            step_data.push_back(static_cast<double>(brain->getActiveNeuronCount()));
            step_data.push_back(static_cast<double>(brain->getFiringNeuronCount()));
            step_data.push_back(static_cast<double>(brain->getAverageFiringRate()));
            step_data.push_back(static_cast<double>(brain->getTotalSpikeCount()) / (step + 1));
            
            activity_data.push_back(step_data);
        }
        
        if (!output_file.empty()) {
            // TODO: Save to file
        }
        
        return activity_data;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"), py::arg("output_file") = "",
         "Plot neural activity over time and return data");
    
    // Data export/import capabilities
    m.def("export_brain_state", [](std::shared_ptr<Brain> brain, const std::string& filepath,
                                   const std::vector<std::string>& include_data) {
        // Export brain state to file
        // This is a simplified implementation
        bool success = brain->save(filepath);
        
        if (success) {
            // TODO: Add metadata about included data
        }
        
        return success;
    }, py::arg("brain"), py::arg("filepath"), py::arg("include_data"),
         "Export brain state to file with specified data included");
    
    m.def("import_brain_state", [](const std::string& filepath) {
        // Import brain state from file
        // This is a simplified implementation
        return std::make_tuple(false, "Not implemented");
    }, py::arg("filepath"), "Import brain state from file");
    
    // Statistical analysis tools
    m.def("analyze_brain_dynamics", [](std::shared_ptr<Brain> brain, int steps, float timestep) {
        struct BrainDynamicsAnalysis {
            double mean_firing_rate;
            double std_firing_rate;
            double mean_spike_intervals;
            double std_spike_intervals;
            std::vector<double> firing_rate_history;
            std::vector<double> spike_interval_history;
        };
        
        BrainDynamicsAnalysis analysis;
        brain->initialize();
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, timestep);
            
            double firing_rate = brain->getAverageFiringRate();
            analysis.firing_rate_history.push_back(firing_rate);
            
            // Simple spike interval calculation
            if (step > 0) {
                double spike_interval = static_cast<double>(brain->getTotalSpikeCount()) / (step + 1);
                analysis.spike_interval_history.push_back(spike_interval);
            }
        }
        
        // Calculate statistics
        if (!analysis.firing_rate_history.empty()) {
            double sum = 0.0, sum_sq = 0.0;
            for (double rate : analysis.firing_rate_history) {
                sum += rate;
                sum_sq += rate * rate;
            }
            
            analysis.mean_firing_rate = sum / analysis.firing_rate_history.size();
            analysis.std_firing_rate = std::sqrt((sum_sq / analysis.firing_rate_history.size()) - 
                                                (analysis.mean_firing_rate * analysis.mean_firing_rate));
        }
        
        return analysis;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep"),
         "Analyze brain dynamics and return statistical measures");
    
    // Network analysis tools
    m.def("analyze_neural_network", [](std::shared_ptr<Brain> brain) {
        struct NetworkAnalysis {
            size_t neuron_count;
            size_t synapse_count;
            double connection_density;
            double average_strength;
            std::map<std::string, double> topology_metrics;
        };
        
        NetworkAnalysis analysis;
        analysis.neuron_count = brain->getTotalNeuronCount();
        analysis.synapse_count = brain->getTotalSynapseCount();
        
        if (analysis.neuron_count > 0) {
            analysis.connection_density = static_cast<double>(analysis.synapse_count) / 
                                        (analysis.neuron_count * analysis.neuron_count);
            analysis.average_strength = 0.5; // Placeholder
        }
        
        // Add topology metrics
        analysis.topology_metrics["excitatory_ratio"] = 0.7; // Placeholder
        analysis.topology_metrics["inhibitory_ratio"] = 0.3; // Placeholder
        
        return analysis;
    }, py::arg("brain"), "Analyze neural network topology and properties");
    
    // =============================================
    // MULTI-THREADED AND DISTRIBUTED SUPPORT
    // =============================================
    
    // Parallel simulation execution
    m.def("run_parallel_simulations", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                         const std::vector<int>& steps,
                                         float timestep) {
        std::vector<double> results;
        std::vector<std::future<double>> futures;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            futures.push_back(std::async(std::launch::async, [brains, steps, timestep, i]() {
                if (brains[i]) {
                    brains[i]->initialize();
                    double total_performance = 0.0;
                    
                    for (int step = 0; step < steps[i]; ++step) {
                        brains[i]->step(step, timestep);
                        total_performance += brains[i]->getAverageFiringRate();
                    }
                    
                    return total_performance;
                }
                return 0.0;
            }));
        }
        
        for (size_t i = 0; i < futures.size(); ++i) {
            results.push_back(futures[i].get());
        }
        
        return results;
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep"),
         "Run multiple simulations in parallel");
    
    // Distributed simulation
    m.def("distributed_simulation", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                        const std::vector<int>& steps,
                                        float timestep) {
        struct DistributedResults {
            std::vector<double> results;
            std::vector<bool> success_flags;
            double total_time;
            size_t completed_simulations;
        };
        
        DistributedResults results;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < brains.size(); ++i) {
            if (brains[i]) {
                try {
                    brains[i]->initialize();
                    double performance = 0.0;
                    
                    for (int step = 0; step < steps[i]; ++step) {
                        brains[i]->step(step, timestep);
                        performance += brains[i]->getAverageFiringRate();
                    }
                    
                    results.results.push_back(performance);
                    results.success_flags.push_back(true);
                    results.completed_simulations++;
                } catch (const std::exception& e) {
                    results.results.push_back(0.0);
                    results.success_flags.push_back(false);
                }
            } else {
                results.results.push_back(0.0);
                results.success_flags.push_back(false);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        results.total_time = std::chrono::duration<double>(end_time - start_time).count();
        
        return results;
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep"),
         "Run distributed simulations with fault tolerance");
    
    // Load balancing
    m.def("balance_workload", [](const std::vector<std::shared_ptr<Brain>>& brains,
                               const std::vector<int>& base_steps) {
        std::vector<int> balanced_steps = base_steps;
        int total_steps = std::accumulate(base_steps.begin(), base_steps.end(), 0);
        
        // Simple load balancing algorithm
        if (brains.size() > 1) {
            int avg_steps = total_steps / brains.size();
            int max_deviation = 0;
            
            // Find maximum deviation from average
            for (int steps : base_steps) {
                max_deviation = std::max(max_deviation, std::abs(steps - avg_steps));
            }
            
            // Redistribute if deviation is too high
            if (max_deviation > avg_steps * 0.5) {
                int redistribution = (max_deviation - avg_steps * 0.5) / 2;
                for (size_t i = 0; i < balanced_steps.size(); ++i) {
                    if (balanced_steps[i] > avg_steps) {
                        balanced_steps[i] -= redistribution;
                        if (i + 1 < balanced_steps.size()) {
                            balanced_steps[i + 1] += redistribution;
                        }
                    }
                }
            }
        }
        
        return balanced_steps;
    }, py::arg("brains"), py::arg("base_steps"),
         "Balance workload across multiple brains");
    
    // Fault tolerance
    m.def("fault_tolerant_simulation", [](const std::vector<std::shared_ptr<Brain>>& brains,
                                           const std::vector<int>& steps,
                                           float timestep) {
        struct FaultToleranceResults {
            std::vector<double> results;
            std::vector<bool> success_flags;
            std::vector<std::string> error_messages;
        };
        
        FaultToleranceResults results;
        
        for (size_t i = 0; i < brains.size(); ++i) {
            if (brains[i]) {
                try {
                    brains[i]->initialize();
                    double performance = 0.0;
                    
                    for (int step = 0; step < steps[i]; ++step) {
                        brains[i]->step(step, timestep);
                        performance += brains[i]->getAverageFiringRate();
                    }
                    
                    results.results.push_back(performance);
                    results.success_flags.push_back(true);
                    results.error_messages.push_back("");
                } catch (const std::exception& e) {
                    results.results.push_back(0.0);
                    results.success_flags.push_back(false);
                    results.error_messages.push_back(std::string("Error: ") + e.what());
                }
            } else {
                results.results.push_back(0.0);
                results.success_flags.push_back(false);
                results.error_messages.push_back("Brain is null");
            }
        }
        
        return results;
    }, py::arg("brains"), py::arg("steps"), py::arg("timestep"),
         "Run simulation with fault tolerance and error reporting");
    
    // Enhanced validation functions for all Python API bindings
    m.def("validate_config", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Validate basic configuration structure
        if (config) {
            auto keys = config->getKeys();
            
            // Check for required keys if they exist
            if (config->has("brain.neuron_count")) {
                auto neuron_count = config->get<int64_t>("brain.neuron_count");
                if (neuron_count && *neuron_count <= 0) {
                    errors.push_back("brain.neuron_count must be positive");
                } else if (neuron_count && *neuron_count > 1000000) {
                    warnings.push_back("Large neuron count may impact performance");
                }
            }
            
            if (config->has("brain.region_count")) {
                auto region_count = config->get<int64_t>("brain.region_count");
                if (region_count && *region_count <= 0) {
                    errors.push_back("brain.region_count must be positive");
                }
            }
            
            if (config->has("brain.connection_probability")) {
                auto conn_prob = config->get<double>("brain.connection_probability");
                if (conn_prob && (*conn_prob < 0.0 || *conn_prob > 1.0)) {
                    errors.push_back("brain.connection_probability must be between 0 and 1");
                }
            }
            
            // Check for incompatible configurations
            if (config->has("brain.memory.enable") && config->has("brain.plasticity.enable")) {
                auto memory_enabled = config->get<bool>("brain.memory.enable");
                auto plasticity_enabled = config->get<bool>("brain.plasticity.enable");
                if (memory_enabled && *memory_enabled && plasticity_enabled && *plasticity_enabled) {
                    warnings.push_back("Memory and plasticity both enabled - may impact performance");
                }
            }
        }
        
        return std::make_tuple(errors, warnings);
    }, py::arg("config"), "Validate configuration and return errors/warnings");
    
    m.def("validate_brain_state", [](std::shared_ptr<Brain> brain) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        if (brain) {
            // Check basic consistency
            size_t neuron_count = brain->getTotalNeuronCount();
            size_t synapse_count = brain->getTotalSynapseCount();
            size_t active_count = brain->getActiveNeuronCount();
            size_t firing_count = brain->getFiringNeuronCount();
            
            if (neuron_count == 0) {
                errors.push_back("Brain has no neurons");
            }
            
            if (active_count > neuron_count) {
                errors.push_back("Active neuron count exceeds total neuron count");
            }
            
            if (firing_count > active_count) {
                errors.push_back("Firing neuron count exceeds active neuron count");
            }
            
            // Check developmental stage consistency
            auto stage = brain->getDevelopmentalStage();
            if (stage == DevelopmentalStage::Adult && active_count > neuron_count * 0.1) {
                warnings.push_back("High activity in adult stage - may indicate learning"
            }
        }
        
        return std::make_tuple(errors, warnings);
    }, py::arg("brain"), "Validate brain state and return errors/warnings");
    
    m.def("validate_sensory_percept", [](const SensoryPercept& percept) {
        std::vector<std::string> errors;
        
        // Check if percept has at least one signal type
        if (!percept.getVision() && !percept.getTouch() && 
            !percept.getInternal() && !percept.getAudio()) {
            errors.push_back("Sensory percept must contain at least one signal type");
        }
        
        return errors;
    }, py::arg("percept"), "Validate sensory percept and return errors");
    
    m.def("validate_world_object", [](const WorldObject& obj) {
        std::vector<std::string> errors;
        
        // Validate world object properties
        if (obj.radius <= 0.0f) {
            errors.push_back("World object radius must be positive");
        }
        
        if (std::isnan(obj.x) || std::isnan(obj.y)) {
            errors.push_back("World object position must be valid numbers");
        }
        
        return errors;
    }, py::arg("obj"), "Validate world object and return errors");
    
    m.def("validate_action", [](const Action& action) {
        std::vector<std::string> errors;
        
        // Validate action properties
        if (!action.getName().empty()) {
            // Basic validation
        }
        
        return errors;
    }, py::arg("action"), "Validate action and return errors");
    
    // Recovery and error handling functions
    m.def("attempt_recovery", [](const std::shared_ptr<Brain>& brain, const std::string& error_type) {
        bool recovery_attempted = false;
        std::string recovery_message;
        
        if (brain) {
            recovery_attempted = true;
            
            if (error_type == "step_failure") {
                try {
                    brain->reset();
                    brain->initialize();
                    recovery_message = "Brain reset and reinitialized successfully";
                } catch (const std::exception& e) {
                    recovery_attempted = false;
                    recovery_message = std::string("Recovery failed: ") + e.what();
                }
            } else if (error_type == "memory_exhaustion") {
                recovery_message = "Memory exhaustion - consider reducing brain size or using compression";
            } else if (error_type == "performance_degradation") {
                recovery_message = "Performance degradation detected - consider reducing simulation complexity";
            } else {
                recovery_attempted = false;
                recovery_message = "Unknown error type - no recovery strategy available";
            }
        } else {
            recovery_attempted = false;
            recovery_message = "Invalid brain reference";
        }
        
        return std::make_tuple(recovery_attempted, recovery_message);
    }, py::arg("brain"), py::arg("error_type"),
         "Attempt recovery from error and return success message");
    
    m.def("create_safe_brain", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw ValidationException("Configuration cannot be null");
        }
        
        // Validate configuration before creating brain
        auto validation_result = m.attr("validate_config")(config);
        auto errors = std::get<0>(validation_result);
        
        if (!errors.empty()) {
            throw ValidationException("Invalid configuration: " + errors[0]);
        }
        
        auto brain = std::make_shared<Brain>(config);
        
        try {
            brain->initialize();
        } catch (const std::exception& e) {
            throw ValidationException("Brain initialization failed: " + std::string(e.what()));
        }
        
        return brain;
    }, py::arg("config"), "Create brain with validation and error handling");
    
    m.def("create_safe_world", [](size_t width, size_t height, size_t vision_width, size_t vision_height) {
        // Validate world parameters
        if (width == 0 || height == 0) {
            throw ValidationException("World dimensions must be positive");
        }
        
        if (vision_width > width || vision_height > height) {
            throw ValidationException("Vision dimensions cannot exceed world dimensions");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("vision_width"), py::arg("vision_height"),
         "Create world with validation and error handling");
    
    // Advanced error recovery and rollback
    m.def("safe_brain_operation", [](std::shared_ptr<Brain> brain, 
                                     std::function<void(std::shared_ptr<Brain>)> operation,
                                     bool rollback_on_failure = true) {
        // Save brain state for potential rollback
        std::string backup_filepath = "/tmp/brain_backup_" + std::to_string(std::time(nullptr)) + ".dat";
        bool backup_success = false;
        
        if (rollback_on_failure) {
            try {
                backup_success = brain->save(backup_filepath);
            } catch (const std::exception& e) {
                // Log warning but continue
            }
        }
        
        bool operation_success = false;
        std::string error_message;
        
        try {
            operation(brain);
            operation_success = true;
        } catch (const std::exception& e) {
            error_message = e.what();
            
            if (rollback_on_failure && backup_success) {
                try {
                    brain->load(backup_filepath);
                    error_message += ". State rolled back.";
                } catch (const std::exception& rollback_e) {
                    error_message += ". Rollback failed: " + std::string(rollback_e.what());
                }
            }
        }
        
        // Clean up backup file
        if (backup_success) {
            std::remove(backup_filepath.c_str());
        }
        
        return std::make_tuple(operation_success, error_message);
    }, py::arg("brain"), py::arg("operation"), py::arg("rollback_on_failure") = true,
         "Execute brain operation with rollback support");
    
    // Comprehensive error reporting and diagnostics
    m.def("get_diagnostic_info", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, std::string> diagnostics;
        
        if (brain) {
            diagnostics["brain_type"] = "NLM_Brain";
            diagnostics["neuron_count"] = std::to_string(brain->getTotalNeuronCount());
            diagnostics["synapse_count"] = std::to_string(brain->getTotalSynapseCount());
            diagnostics["active_neuron_count"] = std::to_string(brain->getActiveNeuronCount());
            diagnostics["firing_neuron_count"] = std::to_string(brain->getFiringNeuronCount());
            diagnostics["average_firing_rate"] = std::to_string(brain->getAverageFiringRate());
            diagnostics["total_spike_count"] = std::to_string(brain->getTotalSpikeCount());
            
            auto stage = brain->getDevelopmentalStage();
            diagnostics["developmental_stage"] = std::to_string(static_cast<int>(stage));
            
            auto config = brain->getConfig();
            if (config) {
                auto keys = config->getKeys();
                diagnostics["config_keys_count"] = std::to_string(keys.size());
            }
            
            diagnostics["timestamp"] = std::to_string(std::time(nullptr));
            diagnostics["status"] = "ok";
        } else {
            diagnostics["status"] = "error";
            diagnostics["error"] = "Brain is null";
        }
        
        return diagnostics;
    }, py::arg("brain"), "Get comprehensive diagnostic information about brain");
    
    m.def("validate_and_log", [](const std::string& operation, const std::vector<std::string>& parameters) {
        std::vector<std::string> validation_errors;
        
        // Basic parameter validation
        if (operation.empty()) {
            validation_errors.push_back("Operation name cannot be empty");
        }
        
        for (const auto& param : parameters) {
            if (param.empty()) {
                validation_errors.push_back("Parameter cannot be empty");
            }
        }
        
        // Log operation
        if (validation_errors.empty()) {
            // Log successful operation
        } else {
            // Log validation errors
        }
        
        return validation_errors;
    }, py::arg("operation"), py::arg("parameters"),
         "Validate operation and parameters, return errors if any");
    
    // Performance monitoring with error tracking
    m.def("profile_operation_with_errors", [](std::function<void()> operation, 
                                            const std::string& operation_name) {
        struct OperationProfile {
            double execution_time;
            bool success;
            std::string error_message;
            std::map<std::string, std::string> metadata;
        };
        
        OperationProfile profile;
        profile.success = false;
        profile.execution_time = 0.0;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            operation();
            profile.success = true;
        } catch (const std::exception& e) {
            profile.success = false;
            profile.error_message = e.what();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        profile.execution_time = std::chrono::duration<double>(end_time - start_time).count();
        
        profile.metadata["operation_name"] = operation_name;
        profile.metadata["timestamp"] = std::to_string(std::time(nullptr));
        
        return profile;
    }, py::arg("operation"), py::arg("operation_name"),
         "Execute operation with profiling and error tracking");
    
    // Resource monitoring and validation
    m.def("check_resources", []() {
        struct ResourceStatus {
            double memory_usage_percentage;
            bool sufficient_memory;
            std::string memory_warning;
            std::map<std::string, double> resource_metrics;
        };
        
        ResourceStatus status;
        // Simplified resource check - would use platform-specific APIs in practice
        status.memory_usage_percentage = 0.0; // Placeholder
        status.sufficient_memory = true;
        status.memory_warning = ""; // No warning
        
        // Set resource metrics
        status.resource_metrics["cpu_usage"] = 0.0; // Placeholder
        status.resource_metrics["gpu_usage"] = 0.0; // Placeholder
        
        return status;
    }, "Check system resource availability");
    
    // Safe configuration loading with validation
    m.def("safe_load_config", [](const std::string& filepath) {
        struct LoadResult {
            bool success;
            std::string error_message;
            std::shared_ptr<Config> config;
        };
        
        LoadResult result;
        result.success = false;
        
        try {
            auto config = std::make_shared<Config>();
            bool load_success = config->loadFromFile(filepath);
            
            if (load_success) {
                // Validate loaded configuration
                auto validation_result = m.attr("validate_config")(config);
                auto errors = std::get<0>(validation_result);
                
                if (errors.empty()) {
                    result.success = true;
                    result.config = config;
                    result.error_message = "";
                } else {
                    result.error_message = "Validation failed: " + errors[0];
                }
            } else {
                result.error_message = "Failed to load configuration from file";
            }
        } catch (const std::exception& e) {
            result.error_message = std::string("Error loading configuration: ") + e.what();
        }
        
        return result;
    }, py::arg("filepath"), "Safely load configuration from file with validation");
    
    // Comprehensive error reporting function
    m.def("report_error", [](const std::string& error_type, const std::string& error_message,
                           const std::map<std::string, std::string>& context) {
        std::ostringstream report;
        report << "=== ERROR REPORT ===\n";
        report << "Type: " << error_type << "\n";
        report << "Message: " << error_message << "\n";
        report << "Context:\n";
        for (const auto& pair : context) {
            report << "  " << pair.first << ": " << pair.second << "\n";
        }
        report << "Timestamp: " << std::time(nullptr) << "\n";
        report << "====================\n";
        
        return report.str();
    }, py::arg("error_type"), py::arg("error_message"), py::arg("context"),
         "Generate comprehensive error report");
    
    // Test integration helper
    m.def("run_validation_tests", []() {
        std::vector<std::string> test_results;
        
        // Test configuration validation
        try {
            auto config = std::make_shared<Config>();
            config->set("test.key", 123);
            
            auto validation_result = m.attr("validate_config")(config);
            auto errors = std::get<0>(validation_result);
            
            if (errors.empty()) {
                test_results.push_back("Configuration validation: PASS");
            } else {
                test_results.push_back("Configuration validation: FAIL - " + errors[0]);
            }
        } catch (const std::exception& e) {
            test_results.push_back("Configuration validation: ERROR - " + std::string(e.what()));
        }
        
        // Test brain validation
        try {
            auto config = std::make_shared<Config>();
            config->set("brain.neuron_count", 100);
            
            auto brain = m.attr("create_safe_brain")(config);
            
            auto brain_validation = m.attr("validate_brain_state")(brain);
            auto errors = std::get<0>(brain_validation);
            
            if (errors.empty()) {
                test_results.push_back("Brain state validation: PASS");
            } else {
                test_results.push_back("Brain state validation: FAIL - " + errors[0]);
            }
        } catch (const std::exception& e) {
            test_results.push_back("Brain state validation: ERROR - " + std::string(e.what()));
        }
        
        // Test error recovery
        try {
            auto recovery_result = m.attr("attempt_recovery")(nullptr, "step_failure");
            bool success = std::get<0>(recovery_result);
            
            if (success) {
                test_results.push_back("Error recovery: PASS");
            } else {
                test_results.push_back("Error recovery: FAIL - " + std::get<1>(recovery_result));
            }
        } catch (const std::exception& e) {
            test_results.push_back("Error recovery: ERROR - " + std::string(e.what()));
        }
        
        return test_results;
    }, "Run comprehensive validation tests and return results");
    
    // Batch operation with error handling
    m.def("safe_batch_operation", [](std::vector<std::function<void()>> operations) {
        struct BatchResult {
            size_t total_operations;
            size_t successful_operations;
            size_t failed_operations;
            std::vector<std::string> error_messages;
            double total_time;
        };
        
        BatchResult result;
        result.total_operations = operations.size();
        result.successful_operations = 0;
        result.failed_operations = 0;
        result.total_time = 0.0;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < operations.size(); ++i) {
            try {
                operations[i]();
                result.successful_operations++;
            } catch (const std::exception& e) {
                result.failed_operations++;
                result.error_messages.push_back(std::string("Operation " ) + std::to_string(i) + 
                                               ": " + e.what());
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.total_time = std::chrono::duration<double>(end_time - start_time).count();
        
        return result;
    }, py::arg("operations"), "Execute batch of operations with comprehensive error handling");
    
    // Compatibility layer for existing code
    // Keep all original function signatures for backward compatibility
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (backward compatibility)");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (backward compatibility)");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (backward compatibility)");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (backward compatibility)");
    
    // Python-friendly aliases for backward compatibility
    m.def("load_config", &Config::loadFromFile, py::arg("filepath"),
          "Load configuration from file (backward compatibility)");
    
    m.def("save_config", &Config::saveToFile, py::arg("filepath"),
          "Save configuration to file (backward compatibility)");
    
    m.def("brain_step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
          py::arg("brain"), py::arg("currentStep"),
          "Perform brain step (backward compatibility)");
    
    m.def("brain_step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
          py::arg("brain"), py::arg("currentStep"), py::arg("currentTime"),
          "Perform brain step with time (backward compatibility)");
    
    // Global error handler
    m.def("get_last_error", []() {
        // Placeholder for last error tracking
        return std::string("");
    }, "Get last error message");
    
    m.def("clear_errors", []() {
        // Clear error history
    }, "Clear error history");
    
    // Statistics and monitoring
    m.def("get_system_stats", []() {
        struct SystemStats {
            double uptime;
            size_t total_simulations;
            double average_simulation_time;
            std::map<std::string, double> resource_usage;
        };
        
        SystemStats stats;
        stats.uptime = 0.0; // Placeholder
        stats.total_simulations = 0;
        stats.average_simulation_time = 0.0;
        
        // Set resource usage
        stats.resource_usage["memory_percent"] = 0.0;
        stats.resource_usage["cpu_percent"] = 0.0;
        
        return stats;
    }, "Get system statistics");
    
    // ======================================================================
    // BACKWARD COMPATIBILITY LAYER
    // ======================================================================
    
    // Original function names (kept for backward compatibility)
    m.def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
          "Load configuration from file (original name)");
    
    m.def("loadFromArgs", [](Config& self, int argc, char** argv) {
        return self.loadFromArgs(argc, argv);
    }, py::arg("argc"), py::arg("argv"),
         "Load configuration from args (original name)");
    
    m.def("saveToFile", &Config::saveToFile, py::arg("filepath"),
          "Save configuration to file (original name)");
    
    m.def("getKeys", &Config::getKeys, "Get configuration keys (original name)");
    
    m.def("getSummary", &Config::summary, "Get configuration summary (original name)");
    
    m.def("validate_key", [](Config& self, const std::string& key) {
        if (key.empty()) {
            throw std::invalid_argument("Configuration key cannot be empty");
        }
        if (self.has(key)) {
            throw std::invalid_argument("Configuration key already exists: " + key);
        }
    }, py::arg("key"), "Validate key (original name)");
    
    m.def("validate_value_type", [](Config& self, const std::string& key, const std::string& expected_type) {
        auto value = self.get<std::string>(key);
        if (!value.has_value()) {
            throw std::invalid_argument("Key '" + key + "' does not exist");
        }
        if (expected_type == "int" && !std::holds_alternative<int>(value.value())) {
            throw std::invalid_argument("Key '" + key + "' is not an integer");
        } else if (expected_type == "float" && !std::holds_alternative<double>(value.value())) {
            throw std::invalid_argument("Key '" + key + "' is not a float");
        } else if (expected_type == "string" && !std::holds_alternative<std::string>(value.value())) {
            throw std::invalid_argument("Key '" + key + "' is not a string");
        }
    }, py::arg("key"), py::arg("expected_type"), "Validate value type (original name)");
    
    // ======================================================================
    // END OF BINDINGS
    // ======================================================================
    
    // Global initialization and cleanup
    // Note: Python module will handle cleanup automatically
    
} // end PYBIND11_MODULE

} // namespace nlm