#include "ConfigManager.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>

namespace nlm {

ConfigManager ConfigManager::instance_;

ConfigManager& ConfigManager::getInstance() {
    return instance_;
}

void ConfigManager::initialize(const std::string& configFile, int argc, char** argv) {
    // Load from file if specified
    if (!configFile.empty()) {
        if (!config_->loadFromFile(configFile)) {
            std::cerr << "Warning: Could not load configuration from " 
                      << configFile << ". Using defaults." << std::endl;
        } else {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        }
    }
    
    // Override with command line arguments
    if (argc > 0 && argv != nullptr) {
        config_->loadFromArgs(argc, argv);
    }
}

bool ConfigManager::setValidated(const std::string& key, const std::string& value, 
                                const std::string& description) {
    auto schema = createConfigurationSchema();
    auto it = schema.find(key);
    
    if (it == schema.end()) {
        std::cerr << "Warning: Unknown configuration key: " << key << std::endl;
        // Still set it but without validation
        config_->set(key, value, ConfigSource::Runtime);
        config_->set(key, description, ConfigSource::Runtime);
        return true;
    }
    
    const auto& schemaEntry = it->second;
    
    // Basic type validation
    bool valid = false;
    if (schemaEntry.type == "int") {
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.length()) {
                // Check constraints
                if (schemaEntry.constraints.count("min")) {
                    int minVal = std::stoi(schemaEntry.constraints.at("min"));
                    if (intVal < minVal) {
                        std::cerr << "Error: " << key << " must be >= " 
                                  << minVal << ", got " << intVal << std::endl;
                        return false;
                    }
                }
                if (schemaEntry.constraints.count("max")) {
                    int maxVal = std::stoi(schemaEntry.constraints.at("max"));
                    if (intVal > maxVal) {
                        std::cerr << "Error: " << key << " must be <= " 
                                  << maxVal << ", got " << intVal << std::endl;
                        return false;
                    }
                }
                config_->set(key, intVal, ConfigSource::Runtime);
                valid = true;
            }
        } catch (const std::exception&) {
            std::cerr << "Error: Invalid integer value for " << key 
                      << ": " << value << std::endl;
        }
    } else if (schemaEntry.type == "double") {
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.length()) {
                // Check constraints
                if (schemaEntry.constraints.count("min")) {
                    double minVal = std::stod(schemaEntry.constraints.at("min"));
                    if (doubleVal < minVal) {
                        std::cerr << "Error: " << key << " must be >= " 
                                  << minVal << ", got " << doubleVal << std::endl;
                        return false;
                    }
                }
                if (schemaEntry.constraints.count("max")) {
                    double maxVal = std::stod(schemaEntry.constraints.at("max"));
                    if (doubleVal > maxVal) {
                        std::cerr << "Error: " << key << " must be <= " 
                                  << maxVal << ", got " << doubleVal << std::endl;
                        return false;
                    }
                }
                config_->set(key, doubleVal, ConfigSource::Runtime);
                valid = true;
            }
        } catch (const std::exception&) {
            std::cerr << "Error: Invalid double value for " << key 
                      << ": " << value << std::endl;
        }
    } else if (schemaEntry.type == "bool") {
        if (value == "true" || value == "1" || value == "yes") {
            config_->set(key, true, ConfigSource::Runtime);
            valid = true;
        } else if (value == "false" || value == "0" || value == "no") {
            config_->set(key, false, ConfigSource::Runtime);
            valid = true;
        } else {
            std::cerr << "Error: Invalid boolean value for " << key 
                      << ": " << value << ". Use true/false, 1/0, or yes/no" << std::endl;
        }
    } else if (schemaEntry.type == "string") {
        // String validation - could add length or pattern checks
        config_->set(key, value, ConfigSource::Runtime);
        valid = true;
    }
    
    if (valid) {
        config_->set(key, description, ConfigSource::Runtime);
    }
    
    return valid;
}

bool ConfigManager::validateAll() const {
    auto schema = createConfigurationSchema();
    bool allValid = true;
    
    for (const auto& entry : config_->getKeys()) {
        auto it = schema.find(entry);
        if (it == schema.end()) {
            continue; // Unknown key, skip validation
        }
        
        const auto& schemaEntry = it->second;
        auto value = config_->get<std::string>(entry);
        
        if (!value) {
            // Key exists but has no value, try to get based on type
            bool valid = false;
            if (schemaEntry.type == "int") {
                auto intVal = config_->get<int>(entry);
                valid = intVal.has_value();
            } else if (schemaEntry.type == "double") {
                auto doubleVal = config_->get<double>(entry);
                valid = doubleVal.has_value();
            } else if (schemaEntry.type == "bool") {
                auto boolVal = config_->get<bool>(entry);
                valid = boolVal.has_value();
            } else if (schemaEntry.type == "string") {
                auto stringVal = config_->get<std::string>(entry);
                valid = stringVal.has_value();
            }
            
            if (!valid) {
                std::cerr << "Warning: Configuration key " << entry 
                          << " is missing or invalid" << std::endl;
                allValid = false;
            }
        } else {
            // Key has a string value, already validated by setValidated
        }
    }
    
    return allValid;
}

void ConfigManager::loadPhaseDefaults(int phase) {
    reset();
    
    if (phase == 2) {
        // Phase 2 defaults (real neural computation)
        setValidated("neuron_count", "500", "Number of neurons in the network");
        setValidated("region_count", "2", "Number of brain regions");
        setValidated("connection_probability", "0.1", "Probability of synapse creation");
        setValidated("random_seed", "42", "Random number generator seed");
        setValidated("simulation_timestep", "0.001", "Simulation time step in seconds");
        setValidated("stdp_ltp_weight", "0.01", "STDP long-term potentiation weight");
        setValidated("stdp_ltd_weight", "0.012", "STDP long-term depression weight");
        setValidated("stdp_tau", "20", "STDP time constant in ms");
        setValidated("synaptogenesis_rate", "0.0001", "Rate of new synapse creation");
        setValidated("pruning_rate", "0.00001", "Rate of synapse removal");
        setValidated("lateral_inhibition", "0.1", "Strength of lateral inhibition");
        setValidated("adaptation_rate", "0.05", "Spike-frequency adaptation rate");
    } else if (phase == 3) {
        // Phase 3 defaults (world interaction)
        setValidated("environment_width", "20", "World width in meters");
        setValidated("environment_height", "20", "World height in meters");
        setValidated("vision_width", "16", "Vision sensor width in pixels");
        setValidated("vision_height", "16", "Vision sensor height in pixels");
        setValidated("vision_range", "10", "Maximum vision range in meters");
        setValidated("audio_range", "5", "Maximum audio range in meters");
        setValidated("move_speed", "3.0", "Agent movement speed in m/s");
        setValidated("turn_speed", "2.0", "Agent turning speed in degrees/s");
        setValidated("energy_decay_rate", "0.01", "Energy consumption rate per second");
        setValidated("energy_regen_rate", "0.05", "Energy regeneration rate per second");
        setValidated("max_energy", "100", "Maximum agent energy");
    } else if (phase == 4) {
        // Phase 4 defaults (emerging cognition)
        setValidated("working_memory_size", "100", "Working memory capacity in items");
        setValidated("working_memory_decay", "0.9", "Working memory decay factor");
        setValidated("concept_formulation_threshold", "10", "Number of experiences before concept formation");
        setValidated("concept_stability_threshold", "0.8", "Concept stability threshold for consolidation");
        setValidated("attention_selectivity_threshold", "0.6", "Attention selectivity threshold");
        setValidated("planning_horizon", "5", "Number of steps for planning");
        setValidated("self_model_update_rate", "0.1", "Rate of self-model updates");
        setValidated("social_learning_rate", "0.2", "Social learning rate");
    } else if (phase == 5) {
        // Phase 5 defaults (lifetime learning)
        setValidated("max_neurons", "10000", "Maximum number of neurons");
        setValidated("synapse_formation_steps", "1000", "Steps for synapse formation phase");
        setValidated("sensory_development_steps", "2000", "Steps for sensory development");
        setValidated("motor_development_steps", "2000", "Steps for motor development");
        setValidated("associative_development_steps", "2000", "Steps for associative development");
        setValidated("memory_consolidation_steps", "2000", "Steps for memory consolidation");
        setValidated("social_development_steps", "2000", "Steps for social development");
        setValidated("continual_learning_cycles", "3", "Number of continual learning cycles");
        setValidated("damage_recovery_threshold", "0.5", "Damage threshold for recovery experiment");
    } else if (phase == 6) {
        // Phase 6 defaults (final integration)
        setValidated("integration_test_episodes", "5", "Number of integration test episodes");
        setValidated("integration_test_steps", "200", "Steps per integration test episode");
        setValidated("brain_states_to_save", "10", "Number of brain states to save for checkpointing");
        setValidated("replay_frequency", "0.1", "Frequency of memory replay (0-1)");
        setValidated("consolidation_strength", "0.8", "Memory consolidation strength");
        setValidated("neuroplasticity_rate", "0.05", "Overall neuroplasticity rate");
        setValidated("developmental_plasticity_bonus", "2.0", "Plasticity bonus during development");
        setValidated("age_related_decay_rate", "0.01", "Rate of age-related plasticity decay");
    }
}

std::map<std::string, ConfigManager::ConfigSchema> ConfigManager::createConfigurationSchema() {
    std::map<std::string, ConfigSchema> schema;
    
    // Core simulation parameters
    schema["neuron_count"] = {"int", "Number of neurons in the network", "500", {"min": "1", "max": "1000000"}};
    schema["region_count"] = {"int", "Number of brain regions", "2", {"min": "1", "max": "100"}};
    schema["connection_probability"] = {"double", "Probability of synapse creation", "0.1", {"min": "0.0", "max": "1.0"}};
    schema["random_seed"] = {"int", "Random number generator seed", "42", {"min": "0", "max": "2147483647"}};
    schema["simulation_timestep"] = {"double", "Simulation time step in seconds", "0.001", {"min": "0.0001", "max": "0.1"}};
    
    // Plasticity parameters
    schema["stdp_ltp_weight"] = {"double", "STDP long-term potentiation weight", "0.01", {"min": "0.0", "max": "1.0"}};
    schema["stdp_ltd_weight"] = {"double", "STDP long-term depression weight", "0.012", {"min": "0.0", "max": "1.0"}};
    schema["stdp_tau"] = {"double", "STDP time constant in ms", "20", {"min": "1.0", "max": "100.0"}};
    schema["synaptogenesis_rate"] = {"double", "Rate of new synapse creation", "0.0001", {"min": "0.0", "max": "0.01"}};
    schema["pruning_rate"] = {"double", "Rate of synapse removal", "0.00001", {"min": "0.0", "max": "0.001"}};
    
    // Environment parameters (Phase 3)
    schema["environment_width"] = {"double", "World width in meters", "20", {"min": "1.0", "max": "1000.0"}};
    schema["environment_height"] = {"double", "World height in meters", "20", {"min": "1.0", "max": "1000.0"}};
    schema["vision_width"] = {"int", "Vision sensor width in pixels", "16", {"min": "1", "max": "1024"}};
    schema["vision_height"] = {"int", "Vision sensor height in pixels", "16", {"min": "1", "max": "1024"}};
    schema["vision_range"] = {"double", "Maximum vision range in meters", "10", {"min": "0.1", "max": "100.0"}};
    schema["audio_range"] = {"double", "Maximum audio range in meters", "5", {"min": "0.1", "max": "50.0"}};
    
    // Agent parameters (Phase 3)
    schema["move_speed"] = {"double", "Agent movement speed in m/s", "3.0", {"min": "0.1", "max": "20.0"}};
    schema["turn_speed"] = {"double", "Agent turning speed in degrees/s", "2.0", {"min": "0.1", "max": "180.0"}};
    
    // Memory parameters (Phase 4)
    schema["working_memory_size"] = {"int", "Working memory capacity in items", "100", {"min": "10", "max": "10000"}};
    schema["working_memory_decay"] = {"double", "Working memory decay factor", "0.9", {"min": "0.0", "max": "1.0"}};
    
    // Lifetime learning parameters (Phase 5)
    schema["max_neurons"] = {"int", "Maximum number of neurons", "10000", {"min": "100", "max": "1000000"}};
    schema["synapse_formation_steps"] = {"int", "Steps for synapse formation phase", "1000", {"min": "100", "max": "100000"}};
    
    // Integration parameters (Phase 6)
    schema["integration_test_episodes"] = {"int", "Number of integration test episodes", "5", {"min": "1", "max": "100"}};
    schema["integration_test_steps"] = {"int", "Steps per integration test episode", "200", {"min": "10", "max": "10000"}};
    
    return schema;
}

} // namespace nlm
