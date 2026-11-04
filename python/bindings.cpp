#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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

    // Add convenience methods for configuration
    .def("getOr", [](Config& self, const std::string& key, auto defaultValue) {
        return self.getOr<decltype(defaultValue)>(key, defaultValue);
    }, py::arg("key"), py::arg("defaultValue"),
         "Get a configuration value or return a default if not present")
    
    // Add configuration validation
    .def("validate", &Config::validate,
         "Validate the configuration and return True if valid")
    
    // Add validation error reporting
    .def("getValidationErrors", &Config::getValidationErrors,
         "Get validation errors as a list of strings")
    
    // Add strict mode control
    .def("setStrictMode", &Config::setStrictMode, py::arg("strict"),
         "Enable or disable strict validation mode")
    .def("getStrictMode", &Config::getStrictMode,
         "Get strict validation mode setting")
    
    // Add validation control
    .def("setValidateOnSet", &Config::setValidateOnSet, py::arg("validate"),
         "Enable or disable validation on configuration set operations")
    .def("getValidateOnSet", &Config::getValidateOnSet,
         "Get validation on set setting")
    
    // Add JSON/YAML support
    .def("saveToJSON", &Config::saveToJSON,
         "Save configuration as JSON string")
    .def("saveToYAML", &Config::saveToYAML,
         "Save configuration as YAML string")
    
    // Add configuration comparison
    .def("merge", [](Config& self, const Config& other) {
        // Add all entries from other config to self
        for (const auto& entry : other.getKeys()) {
            auto value = other.get<ConfigValue>(entry);
            if (value) {
                self.set(entry, *value, ConfigSource::Runtime);
            }
        }
    }, py::arg("other"),
         "Merge another configuration into this one (other overrides)")
    
    // Add configuration reset
    .def("resetToDefaults", &Config::setDefaults,
         "Reset configuration to default values")
    
    // Add configuration statistics
    .def("getStats", [](const Config& self) {
        py::dict stats;
        stats["entry_count"] = self.getKeys().size();
        stats["source_distribution"] = py::dict();
        // Could add more detailed statistics here
        return stats;
    }, "Get configuration statistics")
    
    // Add configuration export/import
    .def("exportAsDict", [](const Config& self) {
        py::dict dict;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                dict[key] = *value;
            }
        }
        return dict;
    }, "Export configuration as Python dictionary")
    
    .def("importFromDict", [](Config& self, const py::dict& dict) {
        for (auto item : dict) {
            std::string key = py::cast<std::string>(item.first);
            try {
                auto value = py::cast<ConfigValue>(item.second);
                self.set(key, value, ConfigSource::Runtime);
            } catch (const std::exception& e) {
                throw py::cast<std::runtime_error>("Error importing from dict: " + std::string(e.what()));
            }
        }
    }, py::arg("dict"),
         "Import configuration from Python dictionary")
    
    // Add configuration comparison operators
    .def("equals", [](const Config& self, const Config& other) {
        return self.getKeys().size() == other.getKeys().size() &&
               std::all_of(self.getKeys().begin(), self.getKeys().end(),
                          [&other](const std::string& key) {
                              return self.get<ConfigValue>(key) == other.get<ConfigValue>(key);
                          });
    }, py::arg("other"),
         "Check if two configurations are equal")
    
    .def("diff", [](const Config& self, const Config& other) {
        py::dict diff;
        for (const auto& key : self.getKeys()) {
            auto selfVal = self.get<ConfigValue>(key);
            auto otherVal = other.get<ConfigValue>(key);
            if (!selfVal.has_value() || !otherVal.has_value() || *selfVal != *otherVal) {
                diff[key] = std::make_pair(selfVal, otherVal);
            }
        }
        for (const auto& key : other.getKeys()) {
            if (!self.has(key)) {
                diff[key] = std::make_pair(nullptr, other.get<ConfigValue>(key));
            }
        }
        return diff;
    }, py::arg("other"),
         "Get difference between this and another configuration")
    
    // Add configuration validation report
    .def("getValidationReport", [](const Config& self) {
        std::stringstream report;
        report << "Configuration Validation Report:" << std::endl;
        report << "===============================" << std::endl;
        report << "Entry count: " << self.getKeys().size() << std::endl;
        report << "Is valid: " << (self.validate() ? "Yes" : "No") << std::endl;
        
        auto errors = self.getValidationErrors();
        if (!errors.empty()) {
            report << "Errors:" << std::endl;
            for (const auto& error : errors) {
                report << "  - " << error << std::endl;
            }
        }
        
        report << "Sources:" << std::endl;
        for (const auto& source : {ConfigSource::Default, ConfigSource::File, 
                                  ConfigSource::CommandLine, ConfigSource::Runtime}) {
            int count = 0;
            for (const auto& key : self.getKeys()) {
                auto entry = self.get<ConfigValue>(key);
                if (entry) {
                    // We can't easily get the source without modifying ConfigEntry
                    // For now, just count entries
                }
            }
            report << "  " << static_cast<int>(source) << ": " << count << std::endl;
        }
        
        return report.str();
    }, "Get detailed validation report")
    
    // Add configuration comparison utilities
    .def("filterKeys", [](const Config& self, const std::function<bool(const std::string&)>& predicate) {
        std::vector<std::string> filtered;
        for (const auto& key : self.getKeys()) {
            if (predicate(key)) {
                filtered.push_back(key);
            }
        }
        return filtered;
    }, py::arg("predicate"),
         "Filter keys based on a predicate function")
    
    .def("getKeysByPrefix", [](const Config& self, const std::string& prefix) {
        std::vector<std::string> prefixed;
        for (const auto& key : self.getKeys()) {
            if (key.find(prefix) == 0) {
                prefixed.push_back(key);
            }
        }
        return prefixed;
    }, py::arg("prefix"),
         "Get keys that start with a given prefix")
    
    .def("getKeysBySuffix", [](const Config& self, const std::string& suffix) {
        std::vector<std::string> suffixed;
        for (const auto& key : self.getKeys()) {
            if (key.size() >= suffix.size() && 
                key.compare(key.size() - suffix.size(), suffix.size(), suffix) == 0) {
                suffixed.push_back(key);
            }
        }
        return suffixed;
    }, py::arg("suffix"),
         "Get keys that end with a given suffix")
    
    // Add configuration backup/restore
    .def("backup", [](Config& self, const std::string& filepath) {
        // Create a copy of the configuration and save it
        Config backup;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                backup.set(key, *value, ConfigSource::File);
            }
        }
        backup.saveToFile(filepath);
    }, py::arg("filepath"),
         "Create a backup of the current configuration to file")
    
    .def("restoreFromBackup", [](Config& self, const std::string& filepath) {
        if (self.loadFromFile(filepath)) {
            NLM_LOG_INFO("Configuration restored from backup: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to restore configuration from backup: " + filepath);
            return false;
        }
    }, py::arg("filepath"),
         "Restore configuration from backup file")
    
    // Add configuration statistics and metrics
    .def("getConfigurationHealth", [](const Config& self) {
        py::dict health;
        health["is_valid"] = self.validate();
        health["error_count"] = static_cast<int>(self.getValidationErrors().size());
        health["entry_count"] = self.getKeys().size();
        
        // Count entries by type
        py::dict typeCount;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::string type;
                if (std::holds_alternative<int>(value.value())) type = "int";
                else if (std::holds_alternative<int64_t>(value.value())) type = "int64";
                else if (std::holds_alternative<double>(value.value())) type = "double";
                else if (std::holds_alternative<bool>(value.value())) type = "bool";
                else if (std::holds_alternative<std::string>(value.value())) type = "string";
                else if (std::holds_alternative<std::vector<int>>(value.value())) type = "vector<int>";
                else if (std::holds_alternative<std::vector<double>>(value.value())) type = "vector<double>";
                else if (std::holds_alternative<std::vector<std::string>>(value.value())) type = "vector<string>";
                
                if (typeCount.contains(type)) {
                    typeCount[type] = typeCount[type].cast<int>() + 1;
                } else {
                    typeCount[type] = 1;
                }
            }
        }
        
        health["type_distribution"] = typeCount;
        return health;
    }, "Get configuration health statistics")
    
    // Add configuration comparison utilities
    .def("findCommonKeys", [](const Config& self, const Config& other) {
        std::vector<std::string> common;
        for (const auto& key : self.getKeys()) {
            if (other.has(key)) {
                common.push_back(key);
            }
        }
        return common;
    }, py::arg("other"),
         "Find keys that exist in both configurations")
    
    .def("findUniqueKeys", [](const Config& self, const Config& other) {
        std::vector<std::string> unique;
        for (const auto& key : self.getKeys()) {
            if (!other.has(key)) {
                unique.push_back(key);
            }
        }
        return unique;
    }, py::arg("other"),
         "Find keys that are unique to this configuration (not in other)")
    
    // Add configuration merging utilities
    .def("mergeWithPriority", [](Config& self, const Config& other, const std::string& prioritySource) {
        // Apply other's entries with higher priority than self's
        for (const auto& key : other.getKeys()) {
            auto value = other.get<ConfigValue>(key);
            if (value) {
                ConfigSource source;
                if (prioritySource == "other") source = ConfigSource::CommandLine;
                else if (prioritySource == "self") source = ConfigSource::File;
                else source = ConfigSource::Runtime;
                self.set(key, *value, source);
            }
        }
    }, py::arg("other"), py::arg("prioritySource"),
         "Merge configuration with priority (other/self/runtime)")
    
    .def("mergeShallow", [](Config& self, const Config& other) {
        // Add all entries from other to self, overwriting existing keys
        for (const auto& key : other.getKeys()) {
            auto value = other.get<ConfigValue>(key);
            if (value) {
                self.set(key, *value, ConfigSource::File);
            }
        }
    }, py::arg("other"),
         "Shallow merge (other overwrites)")
    
    // Add configuration validation rules
    .def("addValidationRule", [](Config& self, const std::function<bool(const Config&)>& rule, const std::string& description) {
        // Note: This is a placeholder - actual validation rules would need
        // to be implemented differently in the C++ API
        NLM_LOG_INFO("Added validation rule: " + description);
    }, py::arg("rule"), py::arg("description"),
         "Add a custom validation rule")
    
    .def("getValidationRules", []() {
        // Return empty list for now - actual rules would need to be managed differently
        return std::vector<std::string>{};
    }, "Get list of validation rules")
    
    // Add configuration diff and patch
    .def("createPatch", [](const Config& self, const Config& other) {
        py::dict patch;
        for (const auto& key : other.getKeys()) {
            auto selfVal = self.get<ConfigValue>(key);
            auto otherVal = other.get<ConfigValue>(key);
            if (!selfVal.has_value()) {
                patch[key] = std::make_pair("add", otherVal);
            } else if (selfVal != otherVal) {
                patch[key] = std::make_pair("modify", otherVal);
            }
        }
        for (const auto& key : self.getKeys()) {
            if (!other.has(key)) {
                patch[key] = std::make_pair("remove", nullptr);
            }
        }
        return patch;
    }, py::arg("other"),
         "Create a patch that transforms this config into other")
    
    .def("applyPatch", [](Config& self, const py::dict& patch) {
        for (auto item : patch) {
            std::string key = py::cast<std::string>(item.first);
            auto patchData = py::cast<std::pair<std::string, std::optional<ConfigValue>>>(item.second);
            std::string operation = patchData.first;
            auto value = patchData.second;
            
            if (operation == "add" && value.has_value()) {
                self.set(key, *value, ConfigSource::Runtime);
            } else if (operation == "modify" && value.has_value()) {
                self.set(key, *value, ConfigSource::Runtime);
            } else if (operation == "remove") {
                self.remove(key);
            }
        }
    }, py::arg("patch"),
         "Apply a patch to this configuration")
    
    // Add configuration persistence
    .def("persist", [](Config& self, const std::string& filepath) {
        return self.saveToFile(filepath);
    }, py::arg("filepath"),
         "Persist configuration to file")
    
    .def("load", [](Config& self, const std::string& filepath) {
        return self.loadFromFile(filepath);
    }, py::arg("filepath"),
         "Load configuration from file")
    
    // Add configuration snapshots
    .def("snapshot", [](const Config& self) {
        // Return a copy of the current configuration
        Config snapshot;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                snapshot.set(key, *value, ConfigSource::File);
            }
        }
        return snapshot;
    }, "Create a snapshot of the current configuration")
    
    .def("restoreFromSnapshot", [](Config& self, const Config& snapshot) {
        // Replace current configuration with snapshot
        self.clear();
        for (const auto& key : snapshot.getKeys()) {
            auto value = snapshot.get<ConfigValue>(key);
            if (value) {
                self.set(key, *value, ConfigSource::File);
            }
        }
    }, py::arg("snapshot"),
         "Restore configuration from snapshot")
    
    // Add configuration statistics
    .def("getSize", &Config::getKeys().size,
         "Get the number of entries in the configuration")
    
    .def("isEmpty", [](const Config& self) {
        return self.getKeys().empty();
    }, "Check if configuration is empty")
    
    .def("getKeysSorted", [](const Config& self) {
        std::vector<std::string> keys = self.getKeys();
        std::sort(keys.begin(), keys.end());
        return keys;
    }, "Get configuration keys sorted alphabetically")
    
    .def("getKeysReverseSorted", [](const Config& self) {
        std::vector<std::string> keys = self.getKeys();
        std::sort(keys.begin(), keys.end(), std::greater<std::string>());
        return keys;
    }, "Get configuration keys sorted in reverse alphabetical order")
    
    // Add configuration pattern matching
    .def("matchPattern", [](const Config& self, const std::string& pattern) {
        // Simple wildcard pattern matching
        std::vector<std::string> matches;
        for (const auto& key : self.getKeys()) {
            bool match = true;
            size_t keyIdx = 0, patternIdx = 0;
            
            while (keyIdx < key.size() && patternIdx < pattern.size()) {
                if (pattern[patternIdx] == '*') {
                    // Skip any sequence of characters
                    patternIdx++;
                    while (patternIdx < pattern.size() && pattern[patternIdx] == '*') patternIdx++;
                    if (patternIdx == pattern.size()) break; // Pattern ends with *
                } else if (pattern[patternIdx] == '?') {
                    // Match exactly one character
                    keyIdx++;
                    patternIdx++;
                } else if (key[keyIdx] == pattern[patternIdx]) {
                    // Match exact character
                    keyIdx++;
                    patternIdx++;
                } else {
                    match = false;
                    break;
                }
            }
            
            if (match && keyIdx == key.size() && patternIdx == pattern.size()) {
                matches.push_back(key);
            }
        }
        return matches;
    }, py::arg("pattern"),
         "Match configuration keys against a wildcard pattern (* and ?)")
    
    // Add configuration transformation utilities
    .def("transformKeys", [](Config& self, const std::function<std::string(const std::string&)>& transformer) {
        // Apply transformer to all keys
        std::vector<std::string> newKeys;
        for (const auto& key : self.getKeys()) {
            std::string newKey = transformer(key);
            if (newKey != key) {
                auto value = self.get<ConfigValue>(key);
                if (value) {
                    self.remove(key);
                    self.set(newKey, *value, ConfigSource::Runtime);
                    newKeys.push_back(newKey);
                }
            }
        }
        return newKeys;
    }, py::arg("transformer"),
         "Transform configuration keys using a function")
    
    .def("renameKey", [](Config& self, const std::string& oldName, const std::string& newName) {
        auto value = self.get<ConfigValue>(oldName);
        if (value) {
            self.set(newName, *value, ConfigSource::Runtime);
            self.remove(oldName);
            return true;
        }
        return false;
    }, py::arg("oldName"), py::arg("newName"),
         "Rename a configuration key")
    
    // Add configuration comparison utilities
    .def("areIdentical", [](const Config& self, const Config& other) {
        return self.getKeys().size() == other.getKeys().size() &&
               std::all_of(self.getKeys().begin(), self.getKeys().end(),
                          [&other](const std::string& key) {
                              return self.get<ConfigValue>(key) == other.get<ConfigValue>(key);
                          });
    }, py::arg("other"),
         "Check if two configurations are identical")
    
    .def("hasCommonKeys", [](const Config& self, const Config& other) {
        for (const auto& key : self.getKeys()) {
            if (other.has(key)) {
                return true;
            }
        }
        return false;
    }, py::arg("other"),
         "Check if configurations share any common keys")
    
    .def("getExclusiveKeys", [](const Config& self, const Config& other) {
        std::vector<std::string> exclusive;
        for (const auto& key : self.getKeys()) {
            if (!other.has(key)) {
                exclusive.push_back(key);
            }
        }
        return exclusive;
    }, py::arg("other"),
         "Get keys that are exclusive to this configuration")
    
    // Add configuration metadata
    .def("setMetadata", [](Config& self, const std::string& key, const std::string& description) {
        // Note: This would require modifying the ConfigEntry structure
        // For now, we'll log it
        NLM_LOG_INFO("Set metadata for key '" + key + "': " + description);
    }, py::arg("key"), py::arg("description"),
         "Set metadata/description for a configuration key")
    
    .def("getMetadata", [](const Config& self, const std::string& key) {
        // Note: This would require modifying the ConfigEntry structure
        // For now, return empty string
        return std::string("");
    }, py::arg("key"),
         "Get metadata/description for a configuration key")
    
    // Add configuration validation and health checks
    .def("runHealthCheck", [](Config& self) {
        py::dict results;
        results["is_valid"] = self.validate();
        
        auto errors = self.getValidationErrors();
        results["error_count"] = static_cast<int>(errors.size());
        
        if (!errors.empty()) {
            results["errors"] = errors;
        }
        
        // Check for potential issues
        std::vector<std::string> warnings;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                // Add basic validation rules
                if (key == "neuron_count") {
                    auto neuronCount = self.get<int64_t>(key);
                    if (neuronCount && (*neuronCount <= 0 || *neuronCount > 1000000)) {
                        warnings.push_back("neuron_count may be outside recommended range");
                    }
                } else if (key == "region_count") {
                    auto regionCount = self.get<int64_t>(key);
                    if (regionCount && (*regionCount <= 0 || *regionCount > 100)) {
                        warnings.push_back("region_count may be outside recommended range");
                    }
                }
            }
        }
        
        results["warnings"] = warnings;
        results["warning_count"] = static_cast<int>(warnings.size());
        
        return results;
    }, "Run comprehensive health check on configuration")
    
    // Add configuration comparison utilities
    .def("calculateSimilarity", [](const Config& self, const Config& other) {
        std::vector<std::string> selfKeys = self.getKeys();
        std::vector<std::string> otherKeys = other.getKeys();
        
        if (selfKeys.empty() && otherKeys.empty()) return 1.0f;
        if (selfKeys.empty() || otherKeys.empty()) return 0.0f;
        
        int common = 0;
        for (const auto& key : selfKeys) {
            if (other.has(key)) {
                auto selfVal = self.get<ConfigValue>(key);
                auto otherVal = other.get<ConfigValue>(key);
                if (selfVal == otherVal) {
                    common++;
                }
            }
        }
        
        return static_cast<float>(common) / static_cast<float>(std::max(selfKeys.size(), otherKeys.size()));
    }, py::arg("other"),
         "Calculate similarity score between two configurations (0.0 to 1.0)")
    
    .def("isSubsetOf", [](const Config& self, const Config& other) {
        for (const auto& key : self.getKeys()) {
            if (!other.has(key)) {
                return false;
            }
            if (self.get<ConfigValue>(key) != other.get<ConfigValue>(key)) {
                return false;
            }
        }
        return true;
    }, py::arg("other"),
         "Check if this configuration is a subset of another")
    
    // Add configuration lifecycle management
    .def("cleanup", [](Config& self) {
        // Remove entries with empty values
        std::vector<std::string> emptyKeys;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                if (std::holds_alternative<std::string>(value.value()) && 
                    std::get<std::string>(value.value()).empty()) {
                    emptyKeys.push_back(key);
                }
            }
        }
        for (const auto& key : emptyKeys) {
            self.remove(key);
        }
        return static_cast<int>(emptyKeys.size());
    }, "Remove entries with empty string values")
    
    .def("countEntriesByType", [](const Config& self) {
        py::dict typeCount;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::string type;
                if (std::holds_alternative<int>(value.value())) type = "int";
                else if (std::holds_alternative<int64_t>(value.value())) type = "int64";
                else if (std::holds_alternative<double>(value.value())) type = "double";
                else if (std::holds_alternative<bool>(value.value())) type = "bool";
                else if (std::holds_alternative<std::string>(value.value())) type = "string";
                else if (std::holds_alternative<std::vector<int>>(value.value())) type = "vector<int>";
                else if (std::holds_alternative<std::vector<double>>(value.value())) type = "vector<double>";
                else if (std::holds_alternative<std::vector<std::string>>(value.value())) type = "vector<string>";
                
                typeCount[type] = typeCount[type].cast<int>() + 1;
            }
        }
        return typeCount;
    }, "Count entries by type (int, double, bool, string, vectors)")
    
    .def("findEntriesByType", [](const Config& self, const std::string& typeName) {
        std::vector<std::string> matchingKeys;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::string type;
                if (std::holds_alternative<int>(value.value())) type = "int";
                else if (std::holds_alternative<int64_t>(value.value())) type = "int64";
                else if (std::holds_alternative<double>(value.value())) type = "double";
                else if (std::holds_alternative<bool>(value.value())) type = "bool";
                else if (std::holds_alternative<std::string>(value.value())) type = "string";
                else if (std::holds_alternative<std::vector<int>>(value.value())) type = "vector<int>";
                else if (std::holds_alternative<std::vector<double>>(value.value())) type = "vector<double>";
                else if (std::holds_alternative<std::vector<std::string>>(value.value())) type = "vector<string>";
                
                if (type == typeName) {
                    matchingKeys.push_back(key);
                }
            }
        }
        return matchingKeys;
    }, py::arg("typeName"),
         "Find all keys with a specific value type")
    
    .def("exportAsStringMap", [](const Config& self) {
        py::dict stringMap;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                // Convert value to string representation
                std::stringstream ss;
                std::visit([&ss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        ss << arg;
                    } else if constexpr (std::is_same_v<T, int> ||
                                         std::is_same_v<T, int64_t> ||
                                         std::is_same_v<T, double> ||
                                         std::is_same_v<T, bool>) {
                        ss << arg;
                    } else {
                        ss << "<" << typeid(T).name() << ">";
                    }
                }, value.value());
                
                stringMap[key] = ss.str();
            }
        }
        return stringMap;
    }, "Export configuration as string map (key->string(value))")
    
    .def("importFromStringMap", [](Config& self, const py::dict& stringMap) {
        for (auto item : stringMap) {
            std::string key = py::cast<std::string>(item.first);
            std::string valueStr = py::cast<std::string>(item.second);
            
            // Try to parse string value back to appropriate type
            try {
                // Try int
                try {
                    int intVal = std::stoi(valueStr);
                    self.set(key, intVal, ConfigSource::Runtime);
                    continue;
                } catch (...) {}
                
                // Try int64
                try {
                    int64_t int64Val = std::stoll(valueStr);
                    self.set(key, int64Val, ConfigSource::Runtime);
                    continue;
                } catch (...) {}
                
                // Try double
                try {
                    double doubleVal = std::stod(valueStr);
                    self.set(key, doubleVal, ConfigSource::Runtime);
                    continue;
                } catch (...) {}
                
                // Try bool
                if (valueStr == "true") {
                    self.set(key, true, ConfigSource::Runtime);
                    continue;
                } else if (valueStr == "false") {
                    self.set(key, false, ConfigSource::Runtime);
                    continue;
                }
                
                // Default to string
                self.set(key, valueStr, ConfigSource::Runtime);
                
            } catch (const std::exception& e) {
                throw py::cast<std::runtime_error>("Error importing from string map: " + std::string(e.what()));
            }
        }
    }, py::arg("stringMap"),
         "Import configuration from string map (string(key)->string(value))")
    
    // Add configuration cloning
    .def("clone", [](const Config& self) {
        Config clone;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                clone.set(key, *value, ConfigSource::File);
            }
        }
        return clone;
    }, "Create a deep copy of the configuration")
    
    .def("copyTo", [](const Config& self, Config& other) {
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                other.set(key, *value, ConfigSource::File);
            }
        }
    }, py::arg("other"),
         "Copy configuration to another Config object")
    
    // Add configuration metadata
    .def("setDescription", [](Config& self, const std::string& key, const std::string& description) {
        // Note: This would require modifying ConfigEntry to store descriptions
        // For now, log it
        NLM_LOG_INFO("Description for key '" + key + "': " + description);
    }, py::arg("key"), py::arg("description"),
         "Set human-readable description for a configuration key")
    
    .def("getDescription", [](const Config& self, const std::string& key) {
        // Note: This would require modifying ConfigEntry to store descriptions
        // For now, return empty string
        return std::string("");
    }, py::arg("key"),
         "Get human-readable description for a configuration key")
    
    // Add configuration grouping
    .def("groupBySource", [](const Config& self) {
        py::dict sourceGroups;
        for (const auto& source : {ConfigSource::Default, ConfigSource::File, 
                                  ConfigSource::CommandLine, ConfigSource::Runtime}) {
            std::vector<std::string> keys;
            for (const auto& key : self.getKeys()) {
                // Note: We can't easily get the source without modifying ConfigEntry
                // For now, we'll just group by a hypothetical source
                keys.push_back(key);
            }
            sourceGroups[std::to_string(static_cast<int>(source))] = keys;
        }
        return sourceGroups;
    }, "Group configuration keys by source (not fully implemented)")
    
    .def("getSourceSummary", [](const Config& self) {
        // Note: This would require modifying ConfigEntry to store source information
        // For now, return basic information
        std::stringstream summary;
        summary << "Configuration sources:" << std::endl;
        summary << "  - Default: ? keys" << std::endl;
        summary << "  - File: ? keys" << std::endl;
        summary << "  - CommandLine: ? keys" << std::endl;
        summary << "  - Runtime: ? keys" << std::endl;
        return summary.str();
    }, "Get source summary (not fully implemented)")
    
    // Add configuration analysis
    .def("analyze", [](const Config& self) {
        py::dict analysis;
        analysis["total_keys"] = self.getKeys().size();
        
        // Count by value types
        py::dict typeAnalysis;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::string type;
                if (std::holds_alternative<int>(value.value())) type = "int";
                else if (std::holds_alternative<int64_t>(value.value())) type = "int64";
                else if (std::holds_alternative<double>(value.value())) type = "double";
                else if (std::holds_alternative<bool>(value.value())) type = "bool";
                else if (std::holds_alternative<std::string>(value.value())) type = "string";
                else if (std::holds_alternative<std::vector<int>>(value.value())) type = "vector<int>";
                else if (std::holds_alternative<std::vector<double>>(value.value())) type = "vector<double>";
                else if (std::holds_alternative<std::vector<std::string>>(value.value())) type = "vector<string>";
                
                if (typeAnalysis.contains(type)) {
                    typeAnalysis[type] = typeAnalysis[type].cast<int>() + 1;
                } else {
                    typeAnalysis[type] = 1;
                }
            }
        }
        
        analysis["type_distribution"] = typeAnalysis;
        analysis["is_valid"] = self.validate();
        
        return analysis;
    }, "Analyze configuration structure and content")
    
    // Add configuration utilities
    .def("getFirstKey", [](const Config& self) {
        auto keys = self.getKeys();
        if (!keys.empty()) {
            return keys[0];
        }
        return std::string("");
    }, "Get the first key in the configuration")
    
    .def("getLastKey", [](const Config& self) {
        auto keys = self.getKeys();
        if (!keys.empty()) {
            return keys.back();
        }
        return std::string("");
    }, "Get the last key in the configuration")
    
    .def("getKeyCount", &Config::getKeys().size,
         "Get the number of keys in the configuration")
    
    .def("hasKey", &Config::has, py::arg("key"),
         "Check if configuration has a specific key")
    
    .def("getAllKeys", &Config::getKeys,
         "Get all keys in the configuration")
    
    .def("clearAll", &Config::clear,
         "Clear all configuration entries")
    
    .def("getSize", &Config::getKeys().size,
         "Get the size of the configuration (number of entries)")
    
    .def("isEmpty", [](const Config& self) {
        return self.getKeys().empty();
    }, "Check if configuration is empty")
    
    .def("getKeysSortedAlphabetically", [](const Config& self) {
        std::vector<std::string> keys = self.getKeys();
        std::sort(keys.begin(), keys.end());
        return keys;
    }, "Get keys sorted alphabetically")
    
    .def("getKeysSortedReverse", [](const Config& self) {
        std::vector<std::string> keys = self.getKeys();
        std::sort(keys.begin(), keys.end(), std::greater<std::string>());
        return keys;
    }, "Get keys sorted in reverse alphabetical order")
    
    .def("getKeysByPrefix", [](const Config& self, const std::string& prefix) {
        std::vector<std::string> keys;
        for (const auto& key : self.getKeys()) {
            if (key.find(prefix) == 0) {
                keys.push_back(key);
            }
        }
        return keys;
    }, py::arg("prefix"),
         "Get keys that start with a given prefix")
    
    .def("getKeysBySuffix", [](const Config& self, const std::string& suffix) {
        std::vector<std::string> keys;
        for (const auto& key : self.getKeys()) {
            if (key.size() >= suffix.size() && 
                key.compare(key.size() - suffix.size(), suffix.size(), suffix) == 0) {
                keys.push_back(key);
            }
        }
        return keys;
    }, py::arg("suffix"),
         "Get keys that end with a given suffix")
    
    .def("matchPattern", [](const Config& self, const std::string& pattern) {
        // Simple wildcard pattern matching
        std::vector<std::string> matches;
        for (const auto& key : self.getKeys()) {
            bool match = true;
            size_t keyIdx = 0, patternIdx = 0;
            
            while (keyIdx < key.size() && patternIdx < pattern.size()) {
                if (pattern[patternIdx] == '*') {
                    // Skip any sequence of characters
                    patternIdx++;
                    while (patternIdx < pattern.size() && pattern[patternIdx] == '*') patternIdx++;
                    if (patternIdx == pattern.size()) break; // Pattern ends with *
                } else if (pattern[patternIdx] == '?') {
                    // Match exactly one character
                    keyIdx++;
                    patternIdx++;
                } else if (key[keyIdx] == pattern[patternIdx]) {
                    // Match exact character
                    keyIdx++;
                    patternIdx++;
                } else {
                    match = false;
                    break;
                }
            }
            
            if (match && keyIdx == key.size() && patternIdx == pattern.size()) {
                matches.push_back(key);
            }
        }
        return matches;
    }, py::arg("pattern"),
         "Match keys against a wildcard pattern (* and ?)")
    
    .def("getKeyCountByType", [](const Config& self, const std::string& typeName) {
        int count = 0;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::string type;
                if (std::holds_alternative<int>(value.value())) type = "int";
                else if (std::holds_alternative<int64_t>(value.value())) type = "int64";
                else if (std::holds_alternative<double>(value.value())) type = "double";
                else if (std::holds_alternative<bool>(value.value())) type = "bool";
                else if (std::holds_alternative<std::string>(value.value())) type = "string";
                else if (std::holds_alternative<std::vector<int>>(value.value())) type = "vector<int>";
                else if (std::holds_alternative<std::vector<double>>(value.value())) type = "vector<double>";
                else if (std::holds_alternative<std::vector<std::string>>(value.value())) type = "vector<string>";
                
                if (type == typeName) {
                    count++;
                }
            }
        }
        return count;
    }, py::arg("typeName"),
         "Get count of keys with a specific value type")
    
    .def("findKeysByValue", [](const Config& self, const std::string& valueStr) {
        std::vector<std::string> matchingKeys;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::stringstream ss;
                std::visit([&ss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        ss << arg;
                    } else if constexpr (std::is_same_v<T, int> ||
                                         std::is_same_v<T, int64_t> ||
                                         std::is_same_v<T, double> ||
                                         std::is_same_v<T, bool>) {
                        ss << arg;
                    } else {
                        ss << "<" << typeid(T).name() << ">";
                    }
                }, value.value());
                
                if (ss.str() == valueStr) {
                    matchingKeys.push_back(key);
                }
            }
        }
        return matchingKeys;
    }, py::arg("valueStr"),
         "Find keys where the string representation matches exactly")
    
    .def("containsValue", [](const Config& self, const std::string& valueStr) {
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::stringstream ss;
                std::visit([&ss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        ss << arg;
                    } else if constexpr (std::is_same_v<T, int> ||
                                         std::is_same_v<T, int64_t> ||
                                         std::is_same_v<T, double> ||
                                         std::is_same_v<T, bool>) {
                        ss << arg;
                    } else {
                        ss << "<" << typeid(T).name() << ">";
                    }
                }, value.value());
                
                if (ss.str() == valueStr) {
                    return true;
                }
            }
        }
        return false;
    }, py::arg("valueStr"),
         "Check if configuration contains a specific value")
    
    .def("getValueType", [](const Config& self, const std::string& key) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) {
            return std::string("none");
        }
        
        if (std::holds_alternative<int>(value.value())) return "int";
        else if (std::holds_alternative<int64_t>(value.value())) return "int64";
        else if (std::holds_alternative<double>(value.value())) return "double";
        else if (std::holds_alternative<bool>(value.value())) return "bool";
        else if (std::holds_alternative<std::string>(value.value())) return "string";
        else if (std::holds_alternative<std::vector<int>>(value.value())) return "vector<int>";
        else if (std::holds_alternative<std::vector<double>>(value.value())) return "vector<double>";
        else if (std::holds_alternative<std::vector<std::string>>(value.value())) return "vector<string>";
        
        return "unknown";
    }, py::arg("key"),
         "Get the type of value stored at a key")
    
    .def("isNumeric", [](const Config& self, const std::string& key) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) return false;
        
        return std::holds_alternative<int>(value.value()) ||
               std::holds_alternative<int64_t>(value.value()) ||
               std::holds_alternative<double>(value.value());
    }, py::arg("key"),
         "Check if value at key is numeric (int, int64, or double)")
    
    .def("isString", [](const Config& self, const std::string& key) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) return false;
        
        return std::holds_alternative<std::string>(value.value());
    }, py::arg("key"),
         "Check if value at key is a string")
    
    .def("isBoolean", [](const Config& self, const std::string& key) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) return false;
        
        return std::holds_alternative<bool>(value.value());
    }, py::arg("key"),
         "Check if value at key is a boolean")
    
    .def("isVector", [](const Config& self, const std::string& key) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) return false;
        
        return std::holds_alternative<std::vector<int>>(value.value()) ||
               std::holds_alternative<std::vector<double>>(value.value()) ||
               std::holds_alternative<std::vector<std::string>>(value.value());
    }, py::arg("key"),
         "Check if value at key is a vector (int, double, or string)")
    
    .def("getVectorSize", [](const Config& self, const std::string& key) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) return 0;
        
        if (std::holds_alternative<std::vector<int>>(value.value())) {
            return static_cast<int>(std::get<std::vector<int>>(value.value()).size());
        } else if (std::holds_alternative<std::vector<double>>(value.value())) {
            return static_cast<int>(std::get<std::vector<double>>(value.value()).size());
        } else if (std::holds_alternative<std::vector<std::string>>(value.value())) {
            return static_cast<int>(std::get<std::vector<std::string>>(value.value()).size());
        }
        return 0;
    }, py::arg("key"),
         "Get size of vector value at key (0 if not a vector)")
    
    .def("getVectorElement", [](const Config& self, const std::string& key, int index) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) {
            return std::string("");
        }
        
        if (std::holds_alternative<std::vector<int>>(value.value())) {
            auto& vec = std::get<std::vector<int>>(value.value());
            if (index >= 0 && index < static_cast<int>(vec.size())) {
                return std::to_string(vec[index]);
            }
        } else if (std::holds_alternative<std::vector<double>>(value.value())) {
            auto& vec = std::get<std::vector<double>>(value.value());
            if (index >= 0 && index < static_cast<int>(vec.size())) {
                return std::to_string(vec[index]);
            }
        } else if (std::holds_alternative<std::vector<std::string>>(value.value())) {
            auto& vec = std::get<std::vector<std::string>>(value.value());
            if (index >= 0 && index < static_cast<int>(vec.size())) {
                return vec[index];
            }
        }
        return std::string("");
    }, py::arg("key"), py::arg("index"),
         "Get element at index from vector value at key")
    
    .def("getVectorElements", [](const Config& self, const std::string& key) {
        auto value = self.get<ConfigValue>(key);
        if (!value.has_value()) {
            return std::vector<std::string>{};
        }
        
        std::vector<std::string> elements;
        if (std::holds_alternative<std::vector<int>>(value.value())) {
            auto& vec = std::get<std::vector<int>>(value.value());
            for (const auto& item : vec) {
                elements.push_back(std::to_string(item));
            }
        } else if (std::holds_alternative<std::vector<double>>(value.value())) {
            auto& vec = std::get<std::vector<double>>(value.value());
            for (const auto& item : vec) {
                elements.push_back(std::to_string(item));
            }
        } else if (std::holds_alternative<std::vector<std::string>>(value.value())) {
            auto& vec = std::get<std::vector<std::string>>(value.value());
            for (const auto& item : vec) {
                elements.push_back(item);
            }
        }
        return elements;
    }, py::arg("key"),
         "Get all elements from vector value at key as strings")
    
    // Add configuration printing and formatting
    .def("printToStream", [](const Config& self, std::ostream& stream) {
        stream << self.summary();
    }, py::arg("stream"),
         "Print configuration summary to output stream")
    
    .def("printCompact", [](const Config& self) {
        std::cout << "Config keys: [";
        auto keys = self.getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << keys[i];
        }
        std::cout << "] (" << keys.size() << " entries)" << std::endl;
    }, "Print configuration keys in compact format")
    
    .def("printStats", [](const Config& self) {
        auto stats = self.getStats();
        std::cout << "Configuration Statistics:" << std::endl;
        std::cout << "  Total entries: " << stats["entry_count"].cast<int>() << std::endl;
        
        if (stats.contains("type_distribution")) {
            py::dict typeDist = stats["type_distribution"].cast<py::dict>();
            std::cout << "  Type distribution:" << std::endl;
            for (auto item : typeDist) {
                std::string type = py::cast<std::string>(item.first);
                int count = item.second.cast<int>();
                std::cout << "    " << type << ": " << count << std::endl;
            }
        }
        
        std::cout << "  Is valid: " << (self.validate() ? "Yes" : "No") << std::endl;
    }, "Print detailed configuration statistics")
    
    // Add configuration lifecycle
    .def("clearAll", &Config::clear,
         "Clear all configuration entries")
    
    .def("removeEntry", &Config::remove, py::arg("key"),
         "Remove configuration entry by key")
    
    .def("hasEntry", &Config::has, py::arg("key"),
         "Check if configuration has entry with given key")
    
    .def("getEntryCount", &Config::getKeys().size,
         "Get number of entries in configuration")
    
    .def("isEmpty", [](const Config& self) {
        return self.getKeys().empty();
    }, "Check if configuration is empty")
    
    .def("getFirstKey", [](const Config& self) {
        auto keys = self.getKeys();
        if (!keys.empty()) {
            return keys[0];
        }
        return std::string("");
    }, "Get first key in configuration")
    
    .def("getLastKey", [](const Config& self) {
        auto keys = self.getKeys();
        if (!keys.empty()) {
            return keys.back();
        }
        return std::string("");
    }, "Get last key in configuration")
    
    // Add configuration serialization
    .def("serialize", [](const Config& self) {
        // Simple serialization: key=value pairs
        std::stringstream ss;
        for (const auto& key : self.getKeys()) {
            ss << key << " = ";
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::visit([&ss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        ss << "\"" << arg << "\"";
                    } else if constexpr (std::is_same_v<T, int> ||
                                         std::is_same_v<T, int64_t> ||
                                         std::is_same_v<T, double> ||
                                         std::is_same_v<T, bool>) {
                        ss << arg;
                    } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                         std::is_same_v<T, std::vector<double>> ||
                                         std::is_same_v<T, std::vector<std::string>>) {
                        ss << "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            if (i > 0) ss << ", ";
                            if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                                ss << "\"" << arg[i] << "\"";
                            } else {
                                ss << arg[i];
                            }
                        }
                        ss << "]";
                    }
                }, value.value());
            }
            ss << "\n";
        }
        return ss.str();
    }, "Serialize configuration to string")
    
    .def("deserialize", [](Config& self, const std::string& data) {
        // Simple deserialization from key=value pairs
        std::istringstream iss(data);
        std::string line;
        while (std::getline(iss, line)) {
            // Skip comments and empty lines
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) continue;
            
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                
                std::string value = line.substr(eqPos + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                // Remove quotes
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                self.set(key, value, ConfigSource::File);
            }
        }
    }, py::arg("data"),
         "Deserialize configuration from string (key=value format)")
    
    // Add configuration management utilities
    .def("exportAsJSON", &Config::saveToJSON,
         "Export configuration as JSON string")
    
    .def("exportAsYAML", &Config::saveToYAML,
         "Export configuration as YAML string")
    
    .def("importFromJSON", [](Config& self, const std::string& jsonData) {
        // For now, just create a simple config from JSON-like string
        // A proper JSON parser would be better
        self.clear();
        // TODO: Implement proper JSON parsing
        NLM_LOG_WARNING("Full JSON parsing not yet implemented in Python bindings");
        return false;
    }, py::arg("jsonData"),
         "Import configuration from JSON string")
    
    .def("importFromYAML", [](Config& self, const std::string& yamlData) {
        // For now, just create a simple config from YAML-like string
        // A proper YAML parser would be better
        self.clear();
        // TODO: Implement proper YAML parsing
        NLM_LOG_WARNING("Full YAML parsing not yet implemented in Python bindings");
        return false;
    }, py::arg("yamlData"),
         "Import configuration from YAML string")
    
    // Add configuration debugging
    .def("debug", [](Config& self) {
        std::cout << "=== Configuration Debug Info ===" << std::endl;
        std::cout << "Entry count: " << self.getKeys().size() << std::endl;
        std::cout << "Is valid: " << (self.validate() ? "Yes" : "No") << std::endl;
        
        auto errors = self.getValidationErrors();
        if (!errors.empty()) {
            std::cout << "Validation errors:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << "Entries:" << std::endl;
        for (const auto& key : self.getKeys()) {
            auto value = self.get<ConfigValue>(key);
            if (value) {
                std::cout << "  " << key << " = ";
                std::visit([](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        std::cout << "\"" << arg << "\"";
                    } else {
                        std::cout << arg;
                    }
                }, value.value());
                std::cout << std::endl;
            }
        }
    }, "Print detailed debug information about configuration")
    
    .def("validateWithDetails", [](const Config& self) {
        py::dict result;
        result["is_valid"] = self.validate();
        
        auto errors = self.getValidationErrors();
        result["error_count"] = static_cast<int>(errors.size());
        
        if (!errors.empty()) {
            py::list errorList;
            for (const auto& error : errors) {
                errorList.append(error);
            }
            result["errors"] = errorList;
        }
        
        return result;
    }, "Validate configuration and return detailed results")
    
    .def("getValidationSummary", [](const Config& self) {
        if (self.validate()) {
            return std::string("Configuration is valid");
        } else {
            auto errors = self.getValidationErrors();
            std::stringstream ss;
            ss << "Configuration is invalid with " << errors.size() << " errors:";
            for (const auto& error : errors) {
                ss << "\n  - " << error;
            }
            return ss.str();
        }
    }, "Get human-readable validation summary")
    
    // Add configuration testing
    .def("testLoadFromFile", [](Config& self, const std::string& filepath) {
        return self.loadFromFile(filepath);
    }, py::arg("filepath"),
         "Test loading configuration from file")
    
    .def("testSaveToFile", [](const Config& self, const std::string& filepath) {
        return self.saveToFile(filepath);
    }, py::arg("filepath"),
         "Test saving configuration to file")
    
    .def("testLoadFromArgs", [](Config& self, const std::vector<std::string>& args) {
        return self.loadFromArgs(args);
    }, py::arg("args"),
         "Test loading configuration from arguments")
    
    .def("testValidate", [](const Config& self) {
        return self.validate();
    }, "Test configuration validation")
    
    // Add configuration documentation
    .def("getDocumentation", []() {
        return std::string(
            "NLM Configuration System\n"
            "==========================\n\n"
            "Configuration is a key-value store for NLM parameters.\n\n"
            "Common keys include:\n"
            "  - neuron_count: Number of neurons (default: 1000)\n"
            "  - region_count: Number of neural regions (default: 1)\n"
            "  - connection_probability: Synapse connectivity (default: 0.1)\n"
            "  - simulation_timestep: Simulation time step (default: 0.001)\n"
            "  - random_seed: Random number generator seed (default: 42)\n\n"
            "Values can be int, int64, double, bool, or string.\n"
            "Lists are represented as [1, 2, 3] for int or [1.0, 2.0, 3.0] for double.\n\n"
            "Use loadFromFile() to load from JSON, YAML, or key=value files.\n"
            "Use saveToFile() to save to JSON, YAML, or key=value files.\n"
            "Use getOr() to get a value with a default.\n"
            "Use validate() to check configuration validity.\n"
            "\nFor more information, see the NLM documentation."
        );
    }, "Get configuration documentation")
    
    .def("getHelp", []() {
        return std::string(
            "NLM Configuration Help\n"
            "======================\n\n"
            "Configuration System Commands:\n"
            "  loadFromFile(filepath) - Load from JSON/YAML/key=value file\n"
            "  saveToFile(filepath) - Save to JSON/YAML/key=value file\n"
            "  loadFromArgs(args) - Load from command line arguments\n"
            "  getOr(key, default) - Get value or return default\n"
            "  has(key) - Check if key exists\n"
            "  getKeys() - Get all keys\n"
            "  validate() - Validate configuration\n"
            "  getValidationErrors() - Get list of validation errors\n"
            "  setStrictMode(strict) - Enable/disable strict validation\n"
            "  setValidateOnSet(validate) - Enable/disable validation on set\n"
            "  saveToJSON() - Export as JSON string\n"
            "  saveToYAML() - Export as YAML string\n"
            "  merge(other) - Merge another configuration\n"
            "  clear() - Clear all entries\n"
            "  remove(key) - Remove entry by key\n"
            "  summary() - Get configuration summary\n"
            "\nFile format support:\n"
            "  JSON: .json files\n"
            "  YAML: .yaml, .yml files\n"
            "  Plain text: .cfg, .txt files (key=value format)\n\n"
            "Value types:\n"
            "  int: 42\n"
            "  int64: 9223372036854775807\n"
            "  double: 3.14\n"
            "  bool: true or false\n"
            "  string: \"hello world\"\n"
            "  vector: [1, 2, 3] or [1.0, 2.0, 3.0] or [\"a\", \"b\", \"c\"]\n"
            "\nFor more help, see the NLM documentation or examples."
        );
    }, "Get configuration help and usage information")
];

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

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

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
             "Log brain status");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

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
}

} // namespace nlm
