// NLM (熙然) - Neural Learning Machine
// JSON configuration utilities implementation
//

#include "JSONConfigUtils.hpp"
#include <fstream>
#include <filesystem>
#include <stdexcept>

using namespace nlm;

bool JSONConfigUtils::loadJSONFromFile(std::shared_ptr<Config> config, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json;
        file >> json;
        fromJSON(config, json);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool JSONConfigUtils::saveJSONToFile(const std::shared_ptr<Config>& config, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json = toJSON(config);
        file << json.dump(2); // Pretty print with 2-space indentation
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool JSONConfigUtils::isJSONFile(const std::string& filepath) {
    // Check file extension
    if (filepath.size() < 5 || filepath.substr(filepath.size() - 5) != ".json") {
        return false;
    }
    
    // Check content (optional - can be expensive for large files)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.close();
    
    // JSON typically starts with { or [
    return (!firstLine.empty() && (firstLine[0] == '{' || firstLine[0] == '['));
}

nlohmann::json JSONConfigUtils::toJSON(const std::shared_ptr<Config>& config) {
    nlohmann::json json = nlohmann::json::object();
    
    // Add a comment about format version
    json["_format_version"] = "nlm_2_0";
    json["_config_source"] = "generated";
    
    for (const auto& entry : config->pImpl->entries) {
        std::string key = entry.key;
        
        // Handle nested keys (e.g., "brain.neuron_count" -> { "brain": { "neuron_count": 1000 } })
        size_t dotPos = key.find('.');
        if (dotPos != std::string::npos) {
            std::string prefix = key.substr(0, dotPos);
            std::string suffix = key.substr(dotPos + 1);
            
            if (!json.contains(prefix)) {
                json[prefix] = nlohmann::json::object();
            }
            
            // Recursively set nested values
            auto& nested = json[prefix];
            setJSONValue(nested, suffix, entry.value);
        } else {
            // Top-level key
            setJSONValue(json, key, entry.value);
        }
    }
    
    return json;
}

void JSONConfigUtils::fromJSON(std::shared_ptr<Config> config, const nlohmann::json& json) {
    // Clear existing entries
    config->clear();
    
    // Process JSON object
    if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            // Skip metadata keys
            if (key == "_format_version" || key == "_config_source") {
                continue;
            }
            
            // Check if key is nested (contains dot)
            size_t dotPos = key.find('.');
            if (dotPos != std::string::npos) {
                // For nested keys, store as-is (flat structure)
                // Could be expanded to support nested structures
                config->set(key, value.get<ConfigValue>(), ConfigSource::File);
            } else {
                config->set(key, value.get<ConfigValue>(), ConfigSource::File);
            }
        }
    }
}

void JSONConfigUtils::setJSONValue(nlohmann::json& json, const std::string& key, const ConfigValue& value) {
    std::visit([&json, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json[key] = arg;
        }
    }, value);
}

// ConfigValidation implementation
bool ConfigValidation::validateEntry(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Basic validation rules
    if (key.empty()) {
        return false;
    }
    
    // Check for valid key format
    if (key.find(' ') != std::string::npos) {
        return false;
    }
    
    // Validate specific keys based on their expected types and ranges
    if (key == "random_seed") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
            return false;
        }
        auto val = std::get<int64_t>(value);
        if (val < 0 || val > 2147483647) {
            return false;
        }
    } else if (key == "neuron_count") {
        if (!std::holds_alternative<int64_t>(value)) {
            return false;
        }
        auto val = std::get<int64_t>(value);
        if (val < 1 || val > 100000) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        auto val = std::get<double>(value);
        if (val <= 0.0 || val > 1.0) {
            return false;
        }
    } else if (key == "connection_probability") {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) {
            return false;
        }
    } else if (key == "dopamine_baseline") {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 10.0) {
            return false;
        }
    }
    
    return true;
}

bool ConfigValidation::validateAll(const std::shared_ptr<Config>& config) {
    for (const auto& entry : config->pImpl->entries) {
        if (!validateEntry(entry.key, entry.value, entry.source)) {
            return false;
        }
    }
    return true;
}

std::string ConfigValidation::getValidationError(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (validateEntry(key, value, source)) {
        return "";
    }
    
    std::string error = "Invalid value for key '" + key + "': ";
    
    if (key == "random_seed") {
        error += "random_seed must be an integer between 0 and 2147483647";
    } else if (key == "neuron_count") {
        error += "neuron_count must be an integer between 1 and 100000";
    } else if (key == "simulation_timestep") {
        error += "simulation_timestep must be a positive number less than or equal to 1.0";
    } else if (key == "connection_probability") {
        error += "connection_probability must be between 0.0 and 1.0";
    } else if (key == "dopamine_baseline") {
        error += "dopamine_baseline must be between 0.0 and 10.0";
    } else {
        error += "Value does not meet validation requirements";
    }
    
    return error;
}

std::vector<std::string> ConfigValidation::getValidKeys() {
    return {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate"
    };
}

std::string ConfigValidation::getKeyDescription(const std::string& key) {
    static const std::map<std::string, std::string> descriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "General plasticity learning rate"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_time_constant", "STDP time constant"},
        {"development_synaptogenesis_rate", "Developmental synaptogenesis rate"},
        {"development_pruning_rate", "Developmental pruning rate"},
        {"dopamine_baseline", "Baseline dopamine level for neuromodulation"},
        {"reward_discount_factor", "Reward discount factor for reinforcement learning"},
        {"environment_name", "Name of the environment simulation"},
        {"environment_width", "Width of the environment"},
        {"environment_height", "Height of the environment"},
        {"log_level", "Logging level (ERROR, WARNING, INFO, DEBUG)"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Name of log file"},
        {"max_simulation_steps", "Maximum number of simulation steps"},
        {"simulation_time_limit", "Maximum simulation time in seconds"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate in Hz"}
    };
    
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->second : "";
}