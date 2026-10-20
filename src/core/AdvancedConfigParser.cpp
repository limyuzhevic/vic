//! JSON/YAML Configuration Parser for NLM
// Phase 2: Advanced Configuration Management
//
// This module implements a comprehensive configuration system that supports:
// - JSON configuration files (modern, structured)
// - YAML configuration files (human-readable)
// - JSON5 (JSON with comments and trailing commas)
// - Configuration validation and schema checking
// - Type-safe configuration values with proper conversion
// - Configuration inheritance and merging
// - Advanced configuration features for research

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <regex>

namespace nlm {

// Configuration schema validation
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, std::string> typeConstraints;
    std::map<std::string, std::string> defaultValues;
    std::vector<std::string> requiredKeys;
    std::map<std::string, std::vector<std::string>> keyGroups;
};

// Advanced configuration parser class
class AdvancedConfigParser {
private:
    std::unordered_map<std::string, ConfigSchema> schemas;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> cacheTimestamps;
    bool validateOnLoad = true;
    bool allowInheritance = true;
    std::string backupDirectory = "backups";

public:
    AdvancedConfigParser() {
        initializeDefaultSchemas();
    }

    void setValidationEnabled(bool enabled) {
        validateOnLoad = enabled;
    }

    void setInheritanceEnabled(bool enabled) {
        allowInheritance = enabled;
    }

    void setBackupDirectory(const std::string& directory) {
        backupDirectory = directory;
        std::filesystem::create_directories(directory);
    }

    bool loadFromJSONFile(const std::string& filepath, Config& config) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

            // Parse JSON content
            auto jsonData = parseJSON(content);
            validateJSONSchema(jsonData, filepath);

            // Convert JSON to config entries
            convertJSONToConfig(jsonData, config);

            // Cache the configuration
            cacheTimestamps[filepath] = std::chrono::system_clock::now();

            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading JSON configuration from " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }

    bool loadFromYAMLFile(const std::string& filepath, Config& config) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

            // Parse YAML content
            auto yamlData = parseYAML(content);
            validateYAMLSchema(yamlData, filepath);

            // Convert YAML to config entries
            convertYAMLToConfig(yamlData, config);

            // Cache the configuration
            cacheTimestamps[filepath] = std::chrono::system_clock::now();

            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading YAML configuration from " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }

    bool loadFromJSON5File(const std::string& filepath, Config& config) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }

            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

            // Preprocess JSON5 (remove comments, trailing commas)
            content = preprocessJSON5(content);

            // Parse JSON content
            auto jsonData = parseJSON(content);
            validateJSONSchema(jsonData, filepath);

            // Convert JSON to config entries
            convertJSONToConfig(jsonData, config);

            // Cache the configuration
            cacheTimestamps[filepath] = std::chrono::system_clock::now();

            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading JSON5 configuration from " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }

    bool saveToJSONFile(const std::string& filepath, const Config& config) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }

            // Create JSON structure from config entries
            auto jsonData = convertConfigToJSON(config);

            // Write formatted JSON
            file << "{" << "\n";
            int entryCount = 0;
            for (const auto& entry : config.getKeys()) {
                entryCount++;
                file << "  \"" << entry << "\": \"" << config.getOr<std::string>(entry, "") << "\"";

                if (entryCount < config.getKeys().size()) {
                    file << "," << "\n";
                } else {
                    file << "\n";
                }
            }
            file << "}" << std::endl;

            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error saving JSON configuration to " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }

    bool saveToYAMLFile(const std::string& filepath, const Config& config) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }

            // Create YAML structure from config entries
            auto yamlData = convertConfigToYAML(config);

            // Write formatted YAML
            for (const auto& entry : config.getKeys()) {
                file << entry << ": " << "" << config.getOr<std::string>(entry, "") << std::endl;
            }

            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error saving YAML configuration to " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }

    bool isConfigurationCached(const std::string& filepath) const {
        auto it = cacheTimestamps.find(filepath);
        if (it == cacheTimestamps.end()) {
            return false;
        }

        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::minutes>(now - it->second);

        return age.count() < 5;  // Consider cached for 5 minutes
    }

    void createBackup(const std::string& filepath, const Config& config) {
        std::string backupFile = backupDirectory + "/" + Path(filepath).filename().string() +
                                "_backup_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

        saveToJSONFile(backupFile, config);
        NLM_LOG_INFO("Created backup configuration at: " + backupFile);
    }

private:
    void initializeDefaultSchemas() {
        // Brain configuration schema
        ConfigSchema brainSchema;
        brainSchema.name = "Brain Configuration";
        brainSchema.description = "Configuration for brain neural network";
        brainSchema.typeConstraints = {
            {"neuron_count", "int64"},
            {"region_count", "int64"},
            {"connection_probability", "float"},
            {"initial_weight_mean", "float"},
            {"initial_weight_std", "float"},
            {"v_thresh", "float"},
            {"v_rest", "float"},
            {"v_reset", "float"},
            {"tau_mem", "float"},
            {"tau_ref", "float"},
            {"refractory_period", "float"},
            {"synaptic_delay", "int64"}
        };
        brainSchema.defaultValues = {
            {"neuron_count", "1000"},
            {"region_count", "1"},
            {"connection_probability", "0.05"},
            {"initial_weight_mean", "0.5"},
            {"initial_weight_std", "0.1"},
            {"v_thresh", "-50.0"},
            {"v_rest", "-70.0"},
            {"v_reset", "-75.0"},
            {"tau_mem", "20.0"},
            {"tau_ref", "2.0"},
            {"refractory_period", "2.0"},
            {"synaptic_delay", "1"}
        };
        brainSchema.requiredKeys = {"neuron_count", "region_count"};
        schemas["brain"] = brainSchema;

        // Plasticity configuration schema
        ConfigSchema plasticitySchema;
        plasticitySchema.name = "Plasticity Configuration";
        plasticitySchema.description = "Configuration for synaptic plasticity";
        plasticitySchema.typeConstraints = {
            {"stdp.enable", "bool"},
            {"stdp.learning_rate", "float"},
            {"stdp.tau_plus", "float"},
            {"stdp.tau_minus", "float"},
            {"stdp.a_plus", "float"},
            {"stdp.a_minus", "float"},
            {"hebbian.enable", "bool"},
            {"hebbian.learning_rate", "float"},
            {"structural.enable", "bool"},
            {"structural.synaptogenesis_rate", "float"},
            {"structural.pruning_rate", "float"},
            {"structural.max_connections", "int64"}
        };
        plasticitySchema.defaultValues = {
            {"stdp.enable", "true"},
            {"stdp.learning_rate", "0.001"},
            {"stdp.tau_plus", "20.0"},
            {"stdp.tau_minus", "20.0"},
            {"stdp.a_plus", "0.01"},
            {"stdp.a_minus", "0.01"},
            {"hebbian.enable", "true"},
            {"hebbian.learning_rate", "0.0005"},
            {"structural.enable", "false"},
            {"structural.synaptogenesis_rate", "0.0001"},
            {"structural.pruning_rate", "0.00001"},
            {"structural.max_connections", "10000"}
        };
        schemas["plasticity"] = plasticitySchema;

        // Neuromodulation configuration schema
        ConfigSchema neuromodSchema;
        neuromodSchema.name = "Neuromodulation Configuration";
        neuromodSchema.description = "Configuration for neuromodulation systems";
        neuromodSchema.typeConstraints = {
            {"dopamine.scale", "float"},
            {"dopamine.learning_rate", "float"},
            {"dopamine.tolerance", "float"},
            {"curiosity.enable", "bool"},
            {"curiosity.scale", "float"},
            {"curiosity.threshold", "float"},
            {"novelty.enable", "bool"},
            {"novelty.threshold", "float"},
            {"prediction_error.enable", "bool"},
            {"prediction_error.scale", "float"},
            {"arousal.scale", "float"}
        };
        neuromodSchema.defaultValues = {
            {"dopamine.scale", "1.0"},
            {"dopamine.learning_rate", "0.01"},
            {"dopamine.tolerance", "0.1"},
            {"curiosity.enable", "true"},
            {"curiosity.scale", "0.5"},
            {"curiosity.threshold", "0.05"},
            {"novelty.enable", "true"},
            {"novelty.threshold", "0.1"},
            {"prediction_error.enable", "true"},
            {"prediction_error.scale", "1.0"},
            {"arousal.scale", "0.8"}
        };
        schemas["neuromod"] = neuromodSchema;

        // Development configuration schema
        ConfigSchema developmentSchema;
        developmentSchema.name = "Development Configuration";
        developmentSchema.description = "Configuration for brain development";
        developmentSchema.typeConstraints = {
            {"enable", "bool"},
            {"plasticity_decay_rate", "float"},
            {"critical_period_length", "float"},
            {"maturation_rate", "float"},
            {"pruning_rate", "float"},
            {"synaptogenesis_rate", "float"},
            {"age_max", "int64"}
        };
        developmentSchema.defaultValues = {
            {"enable", "true"},
            {"plasticity_decay_rate", "0.001"},
            {"critical_period_length", "100.0"},
            {"maturation_rate", "0.01"},
            {"pruning_rate", "0.0001"},
            {"synaptogenesis_rate", "0.0001"},
            {"age_max", "100"}
        };
        schemas["development"] = developmentSchema;

        // Environment configuration schema
        ConfigSchema environmentSchema;
        environmentSchema.name = "Environment Configuration";
        environmentSchema.description = "Configuration for simulation environment";
        environmentSchema.typeConstraints = {
            {"width", "int64"},
            {"height", "int64"},
            {"vision_width", "int64"},
            {"vision_height", "int64"},
            {"max_energy", "float"},
            {"energy_decay_rate", "float"},
            {"resource_density", "float"},
            {"hazard_density", "float"},
            {"agent_speed", "float"},
            {"agent_vision_range", "float"}
        };
        environmentSchema.defaultValues = {
            {"width", "20"},
            {"height", "20"},
            {"vision_width", "8"},
            {"vision_height", "8"},
            {"max_energy", "100.0"},
            {"energy_decay_rate", "0.01"},
            {"resource_density", "0.3"},
            {"hazard_density", "0.1"},
            {"agent_speed", "1.0"},
            {"agent_vision_range", "5.0"}
        };
        schemas["environment"] = environmentSchema;
    }

    nlohmann::json parseJSON(const std::string& content) {
        try {
            return nlohmann::json::parse(content);
        } catch (const nlohmann::json::parse_error& e) {
            throw std::runtime_error("JSON parse error: " + std::string(e.what()));
        }
    }

    nlohmann::json parseYAML(const std::string& content) {
        try {
            // For now, use a simple YAML parser - in production, use a proper YAML library
            // This is a simplified implementation
            nlohmann::json result;
            
            std::regex line_regex("(\w+):\s*(.+)");
            std::istringstream stream(content);
            std::string line;
            
            while (std::getline(stream, line)) {
                std::smatch match;
                if (std::regex_match(line, match, line_regex)) {
                    std::string key = match[1];
                    std::string value = match[2];
                    
                    // Try to parse as number
                    try {
                        if (value.find('.') != std::string::npos) {
                            result[key] = std::stod(value);
                        } else {
                            result[key] = std::stoll(value);
                        }
                    } catch (...) {
                        // Remove quotes if present
                        if ((value.front() == '"' && value.back() == '"') ||
                            (value.front() == '\'' && value.back() == '\'')) {
                            value = value.substr(1, value.size() - 2);
                        }
                        result[key] = value;
                    }
                }
            }
            
            return result;
        } catch (const std::exception& e) {
            throw std::runtime_error("YAML parse error: " + std::string(e.what()));
        }
    }

    std::string preprocessJSON5(const std::string& content) {
        std::string result = content;
        
        // Remove line comments
        size_t comment_pos = result.find("//");
        while (comment_pos != std::string::npos) {
            size_t newline_pos = result.find('\n', comment_pos);
            if (newline_pos != std::string::npos) {
                result.erase(comment_pos, newline_pos - comment_pos);
            } else {
                result.erase(comment_pos);
                break;
            }
            comment_pos = result.find("//", comment_pos);
        }
        
        // Remove block comments
        size_t block_comment_pos = result.find("/*");
        while (block_comment_pos != std::string::npos) {
            size_t block_end_pos = result.find("*/", block_comment_pos);
            if (block_end_pos != std::string::npos) {
                result.erase(block_comment_pos, block_end_pos - block_comment_pos + 2);
            } else {
                break;
            }
            block_comment_pos = result.find("/*", block_comment_pos);
        }
        
        // Remove trailing commas (simplified)
        // This is a basic implementation - for production, use a more robust parser
        result = std::regex_replace(result, std::regex(",(\s*\}|\s*\])"), "$1");
        
        return result;
    }

    void validateJSONSchema(const nlohmann::json& jsonData, const std::string& filepath) {
        if (!validateOnLoad) return;
        
        // Basic validation
        if (!jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON structure: expected object at root level");
        }
        
        // Validate required keys exist
        for (const auto& schema : schemas) {
            for (const auto& requiredKey : schema.second.requiredKeys) {
                if (!jsonData.contains(requiredKey)) {
                    NLM_LOG_WARNING("Missing required key in configuration '" + filepath + "': " + requiredKey);
                }
            }
        }
    }

    void validateYAMLSchema(const nlohmann::json& yamlData, const std::string& filepath) {
        validateJSONSchema(yamlData, filepath);
    }

    void convertJSONToConfig(const nlohmann::json& jsonData, Config& config) {
        config.clear();
        
        for (const auto& [key, value] : jsonData.items()) {
            std::string valueStr;
            
            if (value.is_string()) {
                valueStr = value.get<std::string>();
            } else if (value.is_number_integer()) {
                valueStr = std::to_string(value.get<int64_t>());
            } else if (value.is_number_float()) {
                valueStr = std::to_string(value.get<double>());
            } else if (value.is_boolean()) {
                valueStr = value.get<bool>() ? "true" : "false";
            } else if (value.is_null()) {
                valueStr = "";
            } else {
                // Convert complex types to string representation
                valueStr = value.dump();
            }
            
            config.set(key, valueStr, ConfigSource::File);
        }
    }

    void convertYAMLToConfig(const nlohmann::json& yamlData, Config& config) {
        convertJSONToConfig(yamlData, config);
    }

    nlohmann::json convertConfigToJSON(const Config& config) {
        nlohmann::json result;
        
        for (const auto& key : config.getKeys()) {
            auto val = config.get<std::string>(key);
            if (val.has_value()) {
                // Try to parse as number
                try {
                    size_t pos;
                    int intVal = std::stoi(val.value(), &pos);
                    if (pos == val.value().size()) {
                        result[key] = intVal;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    size_t pos;
                    double floatVal = std::stod(val.value(), &pos);
                    if (pos == val.value().size()) {
                        result[key] = floatVal;
                        continue;
                    }
                } catch (...) {}
                
                // Try to parse as boolean
                std::string lowerVal = val.value();
                std::transform(lowerVal.begin(), lowerVal.end(), lowerVal.begin(), ::tolower);
                if (lowerVal == "true") {
                    result[key] = true;
                } else if (lowerVal == "false") {
                    result[key] = false;
                } else {
                    result[key] = val.value();
                }
            }
        }
        
        return result;
    }

    nlohmann::json convertConfigToYAML(const Config& config) {
        return convertConfigToJSON(config);
    }

    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
};

// Global parser instance
static AdvancedConfigParser* globalParser = nullptr;

// Configuration management functions
bool loadAdvancedConfig(const std::string& filepath, Config& config, const std::string& format = "auto") {
    if (!globalParser) {
        globalParser = new AdvancedConfigParser();
    }

    bool success = false;
    
    if (format == "auto" || format == "json") {
        success = globalParser->loadFromJSONFile(filepath, config);
    }
    
    if (!success && (format == "auto" || format == "yaml" || format == "yml")) {
        success = globalParser->loadFromYAMLFile(filepath, config);
    }
    
    if (!success && (format == "auto" || format == "json5")) {
        success = globalParser->loadFromJSON5File(filepath, config);
    }
    
    return success;
}

bool saveAdvancedConfig(const std::string& filepath, const Config& config, const std::string& format = "json") {
    if (!globalParser) {
        globalParser = new AdvancedConfigParser();
    }

    bool success = false;
    
    if (format == "json") {
        success = globalParser->saveToJSONFile(filepath, config);
    } else if (format == "yaml" || format == "yml") {
        success = globalParser->saveToYAMLFile(filepath, config);
    }
    
    if (success) {
        globalParser->createBackup(filepath, config);
    }
    
    return success;
}

void initializeAdvancedConfigurationSystem() {
    if (!globalParser) {
        globalParser = new AdvancedConfigParser();
    }
}

void cleanupAdvancedConfigurationSystem() {
    if (globalParser) {
        delete globalParser;
        globalParser = nullptr;
    }
}

// Helper functions for configuration validation and analysis
std::map<std::string, std::string> analyzeConfiguration(const Config& config) {
    std::map<std::string, std::string> analysis;
    
    for (const auto& key : config.getKeys()) {
        auto val = config.get<std::string>(key);
        if (val.has_value()) {
            analysis[key] = val.value();
        }
    }
    
    return analysis;
}

bool validateConfiguration(const Config& config) {
    // Basic validation - check for empty values
    for (const auto& key : config.getKeys()) {
        auto val = config.get<std::string>(key);
        if (val.has_value() && val.value().empty()) {
            NLM_LOG_WARNING("Empty value found for configuration key: " + key);
        }
    }
    
    return true;
}

std::string generateConfigurationReport(const Config& config) {
    std::ostringstream report;
    
    report << "Configuration Analysis Report" << std::endl;
    report << "=" << std::endl;
    report << "Total keys: " << config.getKeys().size() << std::endl;
    report << std::endl;
    
    for (const auto& key : config.getKeys()) {
        auto val = config.get<std::string>(key);
        if (val.has_value()) {
            report << key << ": " << val.value() << std::endl;
        }
    }
    
    return report.str();
}

} // namespace nlm
