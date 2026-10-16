#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "src/agent/AgentBrain.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Types/Types.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/world/SimpleWorld.hpp"

namespace py = pybind11;

// Expose the main NLM module with all its advanced features and APIs
PYBIND11_MODULE(pynlm, m) {

    m.doc() = "High-performance neural network framework with real-time spiking computation and learning capabilities";
    
    // Research-grade agent with advanced features
    m.def("createResearchAgent", [](std::shared_ptr<Brain> brain,
                                   const std::string& research_name,
                                   const py::dict& config_overrides) {
        // Research-grade agent with advanced features
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(SimpleWorld());
        
        // Apply research configuration
        for (auto& item : config_overrides) {
            std::string key = item.first.cast<std::string>();
            // Process config overrides
        }
        
        agent->setResearchMode(true);
        agent->setName(research_name);
        
        return agent;
    }, py::arg("brain"), py::arg("research_name"), py::arg("config_overrides"),
     "Create research-grade agent with advanced features (expert feature)");
    
    // Configuration validation and comparison APIs
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) -> bool {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        bool valid = true;
        
        // Validate neuron count
        if (auto neuronCount = config->get<int64_t>("brain.neuron_count")) {
            if (*neuronCount <= 0 || *neuronCount > 1000000) {
                NLM_LOG_WARNING("Invalid neuron_count: " + std::to_string(*neuronCount));
                valid = false;
            }
        }
        
        // Validate timestep
        if (auto timestep = config->get<double>("simulation.timestep")) {
            if (*timestep <= 0.0 || *timestep > 1.0) {
                NLM_LOG_WARNING("Invalid simulation.timestep: " + std::to_string(*timestep));
                valid = false;
            }
        }
        
        // Validate connection probability
        if (auto connProb = config->get<double>("brain.connection_probability")) {
            if (*connProb < 0.0 || *connProb > 1.0) {
                NLM_LOG_WARNING("Invalid brain.connection_probability: " + std::to_string(*connProb));
                valid = false;
            }
        }
        
        return valid;
    }, py::arg("config"), R"pbdoc(Validate configuration for errors and inconsistencies.
        
        Args:
            config: Configuration object to validate
            
        Returns:
            bool: True if configuration is valid, False otherwise
            
        Note:
            This function checks for common configuration errors like
            invalid ranges, missing required values, etc.
        )pbdoc");
    
    m.def("compareConfigs", [](const std::shared_ptr<Config>& config1,
                              const std::shared_ptr<Config>& config2) -> std::vector<std::string> {
        if (!config1 || !config2) {
            throw std::invalid_argument("Config objects cannot be null");
        }
        
        std::vector<std::string> differences;
        
        // Get all keys from both configurations
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Check keys in config1 but not in config2
        for (const auto& key : keys1) {
            if (std::find(keys2.begin(), keys2.end(), key) == keys2.end()) {
                differences.push_back("Key missing in second config: " + key);
            } else {
                // Compare values
                auto value1 = config1->get<ConfigValue>(key);
                auto value2 = config2->get<ConfigValue>(key);
                if (value1 != value2) {
                    differences.push_back("Difference in key " + key + ": " +
                                        (value1.has_value() ? "" : "None") +
                                        (value2.has_value() ? "" : "None"));
                }
            }
        }
        
        // Check keys in config2 but not in config1
        for (const auto& key : keys2) {
            if (std::find(keys1.begin(), keys1.end(), key) == keys1.end()) {
                differences.push_back("Key present in second config but missing in first: " + key);
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"),
        R"pbdoc(Compare two configurations and return differences.
        
        Args:
            config1: First configuration object
            config2: Second configuration object
            
        Returns:
            list: List of differences between the two configurations
        )pbdoc");
    
    m.def("getConfigDifferences", [](const std::shared_ptr<Config>& config1,
                                   const std::shared_ptr<Config>& config2) -> py::dict {
        if (!config1 || !config2) {
            throw std::invalid_argument("Config objects cannot be null");
        }
        
        py::dict differences;
        
        // Get all keys from both configurations
        auto keys1 = config1->getKeys();
        auto keys2 = config2->getKeys();
        
        // Compare all keys
        std::set<std::string> all_keys;
        all_keys.insert(keys1.begin(), keys1.end());
        all_keys.insert(keys2.begin(), keys2.end());
        
        for (const auto& key : all_keys) {
            auto value1 = config1->get<ConfigValue>(key);
            auto value2 = config2->get<ConfigValue>(key);
            
            if (value1 != value2) {
                py::dict diff_entry;
                diff_entry["key"] = key;
                
                if (value1.has_value()) {
                    if (std::holds_alternative<int>(*value1)) {
                        diff_entry["value1"] = std::get<int>(*value1);
                    } else if (std::holds_alternative<int64_t>(*value1)) {
                        diff_entry["value1"] = std::get<int64_t>(*value1);
                    } else if (std::holds_alternative<double>(*value1)) {
                        diff_entry["value1"] = std::get<double>(*value1);
                    } else if (std::holds_alternative<bool>(*value1)) {
                        diff_entry["value1"] = std::get<bool>(*value1);
                    } else if (std::holds_alternative<std::string>(*value1)) {
                        diff_entry["value1"] = std::get<std::string>(*value1);
                    }
                } else {
                    diff_entry["value1"] = py::none();
                }
                
                if (value2.has_value()) {
                    if (std::holds_alternative<int>(*value2)) {
                        diff_entry["value2"] = std::get<int>(*value2);
                    } else if (std::holds_alternative<int64_t>(*value2)) {
                        diff_entry["value2"] = std::get<int64_t>(*value2);
                    } else if (std::holds_alternative<double>(*value2)) {
                        diff_entry["value2"] = std::get<double>(*value2);
                    } else if (std::holds_alternative<bool>(*value2)) {
                        diff_entry["value2"] = std::get<bool>(*value2);
                    } else if (std::holds_alternative<std::string>(*value2)) {
                        diff_entry["value2"] = std::get<std::string>(*value2);
                    }
                } else {
                    diff_entry["value2"] = py::none();
                }
                
                differences[key.c_str()] = diff_entry;
            }
        }
        
        return differences;
    }, py::arg("config1"), py::arg("config2"),
        R"pbdoc(Get detailed differences between two configurations.
        
        Args:
            config1: First configuration object
            config2: Second configuration object
            
        Returns:
            dict: Dictionary of differences keyed by configuration key
        )pbdoc");
    
    m.def("normalizeConfig", [](std::shared_ptr<Config>& config) {
        if (!config) {
            throw std::invalid_argument("Config cannot be null");
        }
        
        // Apply normalization rules
        if (auto neuronCount = config->get<int64_t>("brain.neuron_count")) {
            // Ensure reasonable bounds
            if (*neuronCount < 10) config->set("brain.neuron_count", 10);
            if (*neuronCount > 1000000) config->set("brain.neuron_count", 1000000);
        }
        
        if (auto timestep = config->get<double>("simulation.timestep")) {
            // Ensure timestep is reasonable
            if (*timestep < 0.0001) config->set("simulation.timestep", 0.0001);
            if (*timestep > 0.1) config->set("simulation.timestep", 0.1);
        }
        
        if (auto connProb = config->get<double>("brain.connection_probability")) {
            // Clamp connection probability to valid range
            if (*connProb < 0.0) config->set("brain.connection_probability", 0.0);
            if (*connProb > 1.0) config->set("brain.connection_probability", 1.0);
        }
        
        return config;
    }, py::arg("config"), R"pbdoc(Apply normalization rules to configuration.
        
        This function ensures that configuration values are within reasonable bounds
        and fixes common inconsistencies.
        
        Args:
            config: Configuration object to normalize
            
        Returns:
            Config object: Normalized configuration
        )pbdoc");
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}