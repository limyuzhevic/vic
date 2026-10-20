#include "brain_bindings.hpp"

#include <pybind11/pybind11.h>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"

namespace py = pybind11;
namespace nlm {

void bindBrainModule(pybind11::module_ &m) {
    m.doc() = R"pbdoc(
        NLM Brain Python Bindings
        ---------------------------
        Bindings for the NLM brain neural simulation system.
        Provides Brain class with neuron and synapse management.
    )pbdoc";

    // NeuronId
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, R"pbdoc(Get the index value of this neuron ID)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    // SynapseId
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, R"pbdoc(Get the index value of this synapse ID)pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    // RegionId
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, R"pbdoc(Get the index value of this region ID)pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, R"pbdoc(Get the index value of this population ID)pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });

    // Enumerations
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

    // Config class with enhanced Python API and type safety
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("loadFromEnv", [](Config& self) {
            return self.loadFromEnv();
        }, "Load configuration from environment variables")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        
        // Enhanced type-safe get methods with automatic conversions
        .def("get", py::overload_cast<const std::string&>(&Config::template get<std::string>, 
                   py::const_cast_key(&Config::get<std::string>)),
             py::arg("key"), 
             "Get a configuration value as string (returns None if key doesn't exist)")
        .def("get", py::overload_cast<const std::string&>(&Config::template get<int>,
                   py::const_cast_key(&Config::get<int>)),
             py::arg("key"),
             "Get a configuration value as int (returns None if key doesn't exist)")
        .def("get", py::overload_cast<const std::string&>(&Config::template get<double>,
                   py::const_cast_key(&Config::get<double>)),
             py::arg("key"),
             "Get a configuration value as float (returns None if key doesn't exist)")
        .def("get", py::overload_cast<const std::string&>(&Config::template get<bool>,
                   py::const_cast_key(&Config::get<bool>)),
             py::arg("key"),
             "Get a configuration value as bool (returns None if key doesn't exist)")
        
        // Type-safe getter with automatic conversion to Python types
        .def("getInt", &Config::template get<int>, py::arg("key"),
             "Get a configuration value as int (returns None if key doesn't exist)")
        .def("getFloat", &Config::template get<double>, py::arg("key"),
             "Get a configuration value as float (returns None if key doesn't exist)")
        .def("getBool", &Config::template get<bool>, py::arg("key"),
             "Get a configuration value as bool (returns None if key doesn't exist)")
        .def("getString", &Config::template get<std::string>, py::arg("key"),
             "Get a configuration value as string (returns None if key doesn't exist)")
        
        // Type-safe getter with default values
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::template getOr<std::string>),
             py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value with fallback default (type-safe)")
        .def("getOr", py::overload_cast<const std::string&, int>(&Config::template getOr<int>),
             py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value with fallback default (type-safe)")
        .def("getOr", py::overload_cast<const std::string&, double>(&Config::template getOr<double>),
             py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value with fallback default (type-safe)")
        .def("getOr", py::overload_cast<const std::string&, bool>(&Config::template getOr<bool>),
             py::arg("key"), py::arg("defaultValue"),
             "Get a configuration value with fallback default (type-safe)")
        
        // Enhanced set methods with better type safety
        .def("set", py::overload_cast<const std::string&, const std::string&, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value from string (type-safe)")
        .def("set", py::overload_cast<const std::string&, int, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value from int (type-safe)")
        .def("set", py::overload_cast<const std::string&, double, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value from float (type-safe)")
        .def("set", py::overload_cast<const std::string&, bool, ConfigSource>(&Config::set),
             py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value from bool (type-safe)")
        
        // Type-safe vector getter methods
        .def("getIntVector", &Config::template get<std::vector<int>>, py::arg("key"),
             "Get a configuration value as int vector (returns None if key doesn't exist)")
        .def("getFloatVector", &Config::template get<std::vector<double>>, py::arg("key"),
             "Get a configuration value as float vector (returns None if key doesn't exist)")
        .def("getStringVector", &Config::template get<std::vector<std::string>>, py::arg("key"),
             "Get a configuration value as string vector (returns None if key doesn't exist)")
        
        // Enhanced validation methods
        .def("validateKey", [](const Config& self, const std::string& key) {
            bool exists = self.has(key);
            if (!exists) {
                throw py::key_error("Configuration key does not exist: " + key);
            }
            return exists;
        }, py::arg("key"),
           "Validate that a configuration key exists (raises KeyError if not found)")
        
        .def("validateType", [](const Config& self, const std::string& key, 
                               const std::string& expectedType) {
            bool exists = self.has(key);
            if (!exists) {
                throw py::key_error("Configuration key does not exist: " + key);
            }
            
            auto value = self.get<std::string>(key);
            if (!value.has_value()) {
                throw py::type_error("Configuration key exists but value cannot be converted to string: " + key);
            }
            
            return expectedType;
        }, py::arg("key"), py::arg("expectedType"),
           "Validate that a configuration key exists and can be converted to specified type")
        
        .def("getType", [](const Config& self, const std::string& key) {
            bool exists = self.has(key);
            if (!exists) {
                throw py::key_error("Configuration key does not exist: " + key);
            }
            
            // Try different types to determine actual type
            if (self.get<int>(key).has_value()) return std::string("int");
            if (self.get<double>(key).has_value()) return std::string("float");
            if (self.get<bool>(key).has_value()) return std::string("bool");
            if (self.get<std::string>(key).has_value()) return std::string("string");
            if (self.get<std::vector<int>>(key).has_value()) return std::string("int_vector");
            if (self.get<std::vector<double>>(key).has_value()) return std::string("float_vector");
            if (self.get<std::vector<std::string>>(key).has_value()) return std::string("string_vector");
            
            return std::string("unknown");
        }, py::arg("key"),
           "Get the type of a configuration value as string")
        
        // Configuration introspection
        .def("getConfigInfo", [](const Config& self) {
            std::vector<std::pair<std::string, std::string>> info;
            auto keys = self.getKeys();
            
            for (const auto& key : keys) {
                auto type = self.getType(key);
                info.emplace_back(key, type);
            }
            
            return info;
        }, "Get configuration information with key-type pairs")
        
        // Type-safe configuration loading from environment
        .def("loadFromEnvVars", [](Config& self, const std::string& prefix) {
            return self.loadFromEnvVars(prefix);
        }, py::arg("prefix"),
             "Load configuration from environment variables with given prefix")
        
        // Safe conversion utilities
        .def("safeGetInt", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value.value_or(defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Safely get an integer configuration value with fallback")
        
        .def("safeGetFloat", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value.value_or(defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Safely get a float configuration value with fallback")
        
        .def("safeGetBool", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value.value_or(defaultValue);
        }, py::arg("key"), py::arg("defaultValue"),
           "Safely get a bool configuration value with fallback")
        
        // Configuration validation and cleanup
        .def("removeInvalidKeys", [](Config& self) {
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                auto type = self.getType(key);
                if (type == "unknown") {
                    self.remove(key);
                }
            }
            return keys.size();
        }, "Remove configuration keys with invalid types")
        
        .def("validateAll", [](const Config& self) {
            auto keys = self.getKeys();
            std::vector<std::string> errors;
            
            for (const auto& key : keys) {
                auto type = self.getType(key);
                if (type == "unknown") {
                    errors.push_back("Key '" + key + "' has invalid type");
                }
            }
            
            if (!errors.empty()) {
                std::string errorMsg = "Configuration validation failed:\n";
                for (const auto& error : errors) {
                    errorMsg += "  - " + error + "\n";
                }
                throw std::runtime_error(errorMsg);
            }
            
            return std::string("Configuration is valid");
        }, "Validate all configuration keys and types")
        
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
