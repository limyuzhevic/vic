#include "ValidatedConfig.hpp"
#include "Config.hpp"
#include <sstream>
#include <iomanip>
#include <limits>

namespace nlm {

// ConfigValidator implementation

bool ConfigValidator::validateKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    // Check for invalid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            return false;
        }
    }
    return true;
}

bool ConfigValidator::validateNeuronCount(int64_t count) {
    return count > 0 && count <= std::numeric_limits<int64_t>::max() / 100;
}

bool ConfigValidator::validateRegionCount(int64_t count) {
    return count > 0 && count <= 100;
}

bool ConfigValidator::validateConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;
}

bool ConfigValidator::validatePlasticityParams(float ltp, float ltd) {
    return ltp >= 0.0f && ltp <= 10.0f &&
           ltd >= 0.0f && ltd <= 10.0f &&
           std::abs(ltp - ltd) < 5.0f; // Prevent extreme imbalance
}

bool ConfigValidator::validateTimestep(double timestep) {
    return timestep > 0.0 && timestep <= 0.1; // Reasonable timestep limits
}

bool ConfigValidator::validateMaxSimulationSteps(int64_t steps) {
    return steps >= 0 && steps <= std::numeric_limits<int64_t>::max() / 10;
}

bool ConfigValidator::validateLogLevel(const std::string& level) {
    static const std::vector<std::string> validLevels = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"};
    return std::find(validLevels.begin(), validLevels.end(), level) != validLevels.end();
}

std::string ConfigValidator::getValidationError(const std::string& key, const ConfigValue& value) {
    std::ostringstream oss;
    oss << "Validation failed for key '" << key << "' with value '";
    oss << std::visit([](const auto& val) { return std::to_string(val); }, value) << "'";
    return oss.str();
}

ConfigValue ConfigValidator::getDefaultValue(const std::string& key) {
    if (key == "random_seed") return 42;
    if (key == "simulation_timestep") return 0.001;
    if (key == "neuron_count") return 1000ll;
    if (key == "region_count") return 2ll;
    if (key == "connection_probability") return 0.1f;
    if (key == "plasticity_learning_rate") return 0.01f;
    if (key == "stdp_ltp_weight") return 0.01f;
    if (key == "stdp_ltd_weight") return 0.012f;
    if (key == "stdp_time_constant") return 20.0f;
    if (key == "development_synaptogenesis_rate") return 0.001f;
    if (key == "development_pruning_rate") return 0.0001f;
    if (key == "dopamine_baseline") return 0.1f;
    if (key == "reward_discount_factor") return 0.99f;
    if (key == "environment_name") return std::string("GridWorld");
    if (key == "environment_width") return 10ll;
    if (key == "environment_height") return 10ll;
    if (key == "log_level") return std::string("INFO");
    if (key == "log_to_file") return false;
    if (key == "log_filename") return std::string("nlm.log");
    if (key == "max_simulation_steps") return 10000ll;
    if (key == "simulation_time_limit") return 0.0;
    if (key == "visualization_enabled") return false;
    if (key == "visualization_update_rate") return 30.0f;
    return ConfigValue(); // Default empty variant
}

std::string ConfigValidator::getDefaultDescription(const std::string& key) {
    if (key == "random_seed") return "Random seed for reproducible experiments";
    if (key == "simulation_timestep") return "Simulation timestep in seconds";
    if (key == "neuron_count") return "Number of neurons in the brain";
    if (key == "region_count") return "Number of brain regions";
    if (key == "connection_probability") return "Connection probability for random connectivity";
    if (key == "plasticity_learning_rate") return "Plasticity learning rate";
    if (key == "stdp_ltp_weight") return "STDP LTP weight";
    if (key == "stdp_ltd_weight") return "STDP LTD weight";
    if (key == "stdp_time_constant") return "STDP time constant";
    if (key == "development_synaptogenesis_rate") return "Development synaptogenesis rate";
    if (key == "development_pruning_rate") return "Development pruning rate";
    if (key == "dopamine_baseline") return "Baseline dopamine level";
    if (key == "reward_discount_factor") return "Reward discount factor";
    if (key == "environment_name") return "Environment name";
    if (key == "environment_width") return "Environment width";
    if (key == "environment_height") return "Environment height";
    if (key == "log_level") return "Logging level";
    if (key == "log_to_file") return "Log to file flag";
    if (key == "log_filename") return "Log filename";
    if (key == "max_simulation_steps") return "Maximum simulation steps";
    if (key == "simulation_time_limit") return "Simulation time limit";
    if (key == "visualization_enabled") return "Visualization enabled flag";
    if (key == "visualization_update_rate") return "Visualization update rate";
    return "Unknown configuration parameter";
}

// ConfigFileFormat implementation

bool ConfigFileFormat::parseFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = Config::trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string&::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (!ConfigValidator::validateKey(key)) {
                continue; // Skip invalid keys
            }
            
            ConfigEntry entry(key, ConfigValue(value), ConfigSource::File, 
                            ConfigValidator::getDefaultDescription(key));
            entries.push_back(entry);
        }
    }
    
    return true;
}

bool ConfigFileFormat::writeFile(const std::string& filepath, const std::vector<ConfigEntry>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Enhanced with validation and type safety\n\n";
    
    for (const auto& entry : entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << ConfigFileFormat::formatValue(entry.value) << "\n\n";
    }
    
    return file.good();
}

bool ConfigFileFormat::validateFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        line = Config::trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            return false; // Invalid line format
        }
        
        std::string key = Config::trim(line.substr(0, pos));
        std::string value = Config::trim(line.substr(pos + 1));
        
        if (!ConfigValidator::validateKey(key)) {
            return false; // Invalid key
        }
        
        // Basic value validation
        if (value.empty()) {
            return false; // Empty value
        }
        
        if ((value.front() == '"' && value.back() != '"') ||
            (value.front() == '\'' && value.back() != '\'')) {
            return false; // Unmatched quotes
        }
    }
    
    return true;
}

std::string ConfigFileFormat::formatValue(const ConfigValue& value) {
    std::ostringstream oss;
    oss << std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + val + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else {
            return std::to_string(val);
        }
    }, value);
    return oss.str();
}

} // namespace nlm
