#ifndef NLM_IMPROVED_CONFIG_H
#define NLM_IMPROVED_CONFIG_H

#include "core/Utilities.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <variant>
#include <cassert>

using json = nlohmann::json;

namespace nlm {

class ImprovedConfig {
public:
    // Configuration schema validation
    struct ConfigSchema {
        std::string name;
        std::string description;
        std::map<std::string, ConfigEntrySchema> entries;
        
        bool validate(const json& config) const {
            for (const auto& [key, schema] : entries) {
                if (config.contains(key)) {
                    if (!schema.validate(key, config[key])) {
                        Logger::error("Invalid configuration value for key: " + key);
                        return false;
                    }
                } else if (schema.required) {
                    Logger::warning("Required configuration key missing: " + key);
                }
            }
            return true;
        }
    };
    
    struct ConfigEntrySchema {
        std::string description;
        std::string type; // "int", "double", "string", "bool"
        bool required = false;
        std::variant<int, double, std::string, bool> defaultValue;
        std::optional<std::pair<double, double>> intRange;
        std::optional<std::pair<double, double>> doubleRange;
        std::vector<std::string> allowedValues;
        
        bool validate(const std::string& key, const json& value) const {
            try {
                switch (typeMap.at(type)) {
                    case Type::INT:
                        return validateInt(key, value.get<int>());
                    case Type::INT64:
                        return validateInt64(key, value.get<int64_t>());
                    case Type::DOUBLE:
                        return validateDouble(key, value.get<double>());
                    case Type::STRING:
                        return validateString(key, value.get<std::string>());
                    case Type::BOOL:
                        return validateBool(key, value.get<bool>());
                    default:
                        Logger::error("Unknown type for config key: " + key);
                        return false;
                }
            } catch (const json::type_error& e) {
                Logger::error("Invalid type for config key " + key + ": " + e.what());
                return false;
            } catch (const std::exception& e) {
                Logger::error("Validation error for key " + key + ": " + e.what());
                return false;
            }
        }
        
    private:
        enum class Type { INT, INT64, DOUBLE, STRING, BOOL };
        
        static const std::unordered_map<std::string, Type>& typeMap() {
            static const std::unordered_map<std::string, Type> map = {
                {"int", Type::INT}, {"int64", Type::INT64},
                {"double", Type::DOUBLE}, {"string", Type::STRING},
                {"bool", Type::BOOL}
            };
            return map;
        }
        
        bool validateInt(const std::string& key, int value) const {
            if (intRange && (value < intRange->first || value > intRange->second)) {
                Logger::error("Value " + std::to_string(value) + " for key " + key + 
                             " is out of range [" + std::to_string(intRange->first) +
                             ", " + std::to_string(intRange->second) + "]");
                return false;
            }
            if (!allowedValues.empty() && !std::count(allowedValues.begin(), allowedValues.end(), std::to_string(value))) {
                Logger::error("Invalid value " + std::to_string(value) + " for key " + key);
                return false;
            }
            return true;
        }
        
        bool validateInt64(const std::string& key, int64_t value) const {
            double dblVal = static_cast<double>(value);
            if (intRange && (dblVal < intRange->first || dblVal > intRange->second)) {
                Logger::error("Value " + std::to_string(value) + " for key " + key + 
                             " is out of range [" + std::to_string(intRange->first) +
                             ", " + std::to_string(intRange->second) + "]");
                return false;
            }
            if (!allowedValues.empty() && !std::count(allowedValues.begin(), allowedValues.end(), std::to_string(value))) {
                Logger::error("Invalid value " + std::to_string(value) + " for key " + key);
                return false;
            }
            return true;
        }
        
        bool validateDouble(const std::string& key, double value) const {
            if (doubleRange && (value < doubleRange->first || value > doubleRange->second)) {
                Logger::error("Value " + std::to_string(value) + " for key " + key + 
                             " is out of range [" + std::to_string(doubleRange->first) +
                             ", " + std::to_string(doubleRange->second) + "]");
                return false;
            }
            if (!allowedValues.empty()) {
                bool found = false;
                for (const auto& allowed : allowedValues) {
                    try {
                        double allowedVal = std::stod(allowed);
                        if (isNear(allowedVal, value, 1e-6f)) {
                            found = true;
                            break;
                        }
                    } catch (...) {}
                }
                if (!found) {
                    Logger::error("Invalid value " + std::to_string(value) + " for key " + key);
                    return false;
                }
            }
            return true;
        }
        
        bool validateString(const std::string& key, const std::string& value) const {
            if (!allowedValues.empty() && !std::count(allowedValues.begin(), allowedValues.end(), value)) {
                Logger::error("Invalid value " + value + " for key " + key);
                return false;
            }
            return true;
        }
        
        bool validateBool(const std::string& key, bool value) const {
            return true;
        }
    };
    
    // Main configuration class
    class Config {
    public:
        Config() : schema_(createDefaultSchema()) {
            // Initialize with default values
            try {
                initializeDefaults();
            } catch (const std::exception& e) {
                Logger::warning("Failed to initialize default config: " + std::string(e.what()));
            }
        }
        
        bool loadFromFile(const std::string& filepath) {
            try {
                // Backup current config
                auto backup = configData_;
                
                // Try to load JSON file first
                std::ifstream file(filepath);
                if (file.is_open()) {
                    json jsonConfig;
                    try {
                        file >> jsonConfig;
                        configData_ = jsonConfig;
                    } catch (const json::parse_error& e) {
                        Logger::warning("Failed to parse JSON config: " + std::string(e.what()) + ". Falling back to simple format.");
                        // Restore backup
                        configData_ = backup;
                        return loadFromSimpleFormat(filepath);
                    }
                } else {
                    return loadFromSimpleFormat(filepath);
                }
                
                // Validate the loaded configuration
                if (!schema_.validate(configData_)) {
                    Logger::error("Configuration validation failed for: " + filepath);
                    return false;
                }
                
                return true;
                
            } catch (const std::exception& e) {
                Logger::error(std::string("Exception loading config from file: ") + e.what());
                return false;
            }
        }
        
        bool loadFromArgs(int argc, char** argv) {
            try {
                for (int i = 1; i < argc; ++i) {
                    std::string arg(argv[i]);
                    
                    // Handle --key=value format
                    if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
                        size_t pos = arg.find("=");
                        std::string key = arg.substr(2, pos - 2);
                        std::string value = arg.substr(pos + 1);
                        setFromString(key, value);
                    }
                    // Handle -key value format
                    else if (arg.find("-") == 0 && i + 1 < argc) {
                        std::string key = arg.substr(1);
                        std::string value = argv[++i];
                        setFromString(key, value);
                    }
                    // Handle boolean flags
                    else if (arg.find("--") == 0 && arg.find("=") == std::string::npos) {
                        std::string key = arg.substr(2);
                        setBool(key, true);
                    }
                }
                return true;
            } catch (const std::exception& e) {
                Logger::error(std::string("Exception loading config from args: ") + e.what());
                return false;
            }
        }
        
        template<typename T>
        std::optional<T> get(const std::string& key) const {
            try {
                if (!configData_.contains(key)) {
                    return std::nullopt;
                }
                
                const json& value = configData_[key];
                
                if constexpr (std::is_same_v<T, int>) {
                    if (value.is_number_integer()) return value.get<int>();
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    if (value.is_number_integer()) return value.get<int64_t>();
                    if (value.is_number()) return static_cast<int64_t>(value.get<double>());
                } else if constexpr (std::is_same_v<T, double>) {
                    if (value.is_number()) return value.get<double>();
                } else if constexpr (std::is_same_v<T, std::string>) {
                    if (value.is_string()) return value.get<std::string>();
                } else if constexpr (std::is_same_v<T, bool>) {
                    if (value.is_boolean()) return value.get<bool>();
                }
                
                return std::nullopt;
                
            } catch (const std::exception& e) {
                Logger::error(std::string("Exception getting config value for key: ") + key + ": " + e.what());
                return std::nullopt;
            }
        }
        
        template<typename T>
        T getOr(const std::string& key, const T& defaultValue) const {
            auto val = get<T>(key);
            return val.has_value() ? val.value() : defaultValue;
        }
        
        bool has(const std::string& key) const {
            return configData_.contains(key);
        }
        
        void set(const std::string& key, const json& value) {
            configData_[key] = value;
        }
        
        void setInt(const std::string& key, int value) {
            set(key, value);
        }
        
        void setInt64(const std::string& key, int64_t value) {
            set(key, value);
        }
        
        void setDouble(const std::string& key, double value) {
            set(key, value);
        }
        
        void setString(const std::string& key, const std::string& value) {
            set(key, value);
        }
        
        void setBool(const std::string& key, bool value) {
            set(key, value);
        }
        
        void setFromString(const std::string& key, const std::string& value) {
            try {
                // Try to infer type from string
                if (value.empty()) {
                    setString(key, value);
                    return;
                }
                
                // Try int first
                if (value.find('.') == std::string::npos && value.find('e') == std::string::npos) {
                    try {
                        int intVal = std::stoi(value);
                        setInt(key, intVal);
                        return;
                    } catch (...) {}
                }
                
                // Try double
                try {
                    double doubleVal = std::stod(value);
                    setDouble(key, doubleVal);
                    return;
                } catch (...) {}
                
                // Try bool
                if (value == "true" || value == "false") {
                    bool boolVal = (value == "true");
                    setBool(key, boolVal);
                    return;
                }
                
                // Default to string
                setString(key, value);
                
            } catch (const std::exception& e) {
                Logger::error(std::string("Exception parsing config string for key: ") + key + ": " + e.what());
            }
        }
        
        std::string summary() const {
            std::ostringstream oss;
            oss << "Improved Configuration (JSON format):\n";
            oss << configData_.dump(2) << "\n";
            oss << "Total keys: " << configData_.size();
            return oss.str();
        }
        
        bool validate() const {
            return schema_.validate(configData_);
        }
        
        void exportToSimple(const std::string& filepath) const {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw ConfigurationException("Failed to open file for export: " + filepath);
            }
            
            for (const auto& [key, value] : configData_.items()) {
                file << "# " << getDescription(key) << "\n";
                
                if (value.is_string()) {
                    file << key << " = \"" << value.get<std::string>() << "\"\n";
                } else if (value.is_boolean()) {
                    file << key << " = " << (value.get<bool>() ? "true" : "false") << "\n";
                } else if (value.is_number_integer()) {
                    file << key << " = " << value.get<int64_t>() << "\n";
                } else if (value.is_number()) {
                    file << key << " = " << value.get<double>() << "\n";
                } else {
                    file << key << " = \"" << value.dump() << "\"\n";
                }
                file << "\n";
            }
        }
        
    private:
        json configData_;
        ConfigSchema schema_;
        
        ConfigSchema createDefaultSchema() {
            ConfigSchema schema;
            schema.name = "NLM Configuration";
            schema.description = "Default configuration schema for NLM neural simulation";
            
            // Add all known configuration parameters
            auto& entries = schema.entries;
            
            // Basic configuration
            entries["random_seed"] = ConfigEntrySchema{
                "Random seed for reproducible simulations",
                "int", true, 42,
                std::make_pair(0, static_cast<double>(std::numeric_limits<int64_t>::max())),
                {"42"}
            };
            
            entries["simulation_timestep"] = ConfigEntrySchema{
                "Time step for simulation (in seconds)",
                "double", true, 0.001,
                std::make_pair(1e-6, 1.0),
                {"0.001"}
            };
            
            entries["neuron_count"] = ConfigEntrySchema{
                "Number of neurons in the network",
                "int", true, 1000,
                std::make_pair(10, 100000)
            };
            
            entries["region_count"] = ConfigEntrySchema{
                "Number of neural regions",
                "int", true, 1,
                std::make_pair(1, 10)
            };
            
            entries["connection_probability"] = ConfigEntrySchema{
                "Probability of synapse formation between neurons",
                "double", true, 0.1,
                std::make_pair(0.0, 1.0)
            };
            
            // Plasticity parameters
            entries["stdp_ltp_weight"] = ConfigEntrySchema{
                "STDP Long-Term Potentiation weight",
                "double", true, 0.01,
                std::make_pair(0.0, 1.0)
            };
            
            entries["stdp_ltd_weight"] = ConfigEntrySchema{
                "STDP Long-Term Depression weight",
                "double", true, 0.012,
                std::make_pair(0.0, 1.0)
            };
            
            entries["stdp_tau"] = ConfigEntrySchema{
                "STDP time constant (in ms)",
                "double", true, 20.0,
                std::make_pair(1.0, 100.0)
            };
            
            entries["synaptogenesis_rate"] = ConfigEntrySchema{
                "Rate of new synapse formation",
                "double", true, 0.0001,
                std::make_pair(0.0, 0.1)
            };
            
            entries["pruning_rate"] = ConfigEntrySchema{
                "Rate of synapse elimination",
                "double", true, 0.00001,
                std::make_pair(0.0, 0.01)
            };
            
            // Advanced configuration
            entries["working_memory_capacity"] = ConfigEntrySchema{
                "Capacity of working memory (number of traces)",
                "int", false, 100,
                std::make_pair(0, 10000)
            };
            
            entries["max_episodes"] = ConfigEntrySchema{
                "Maximum number of episodic memory entries",
                "int", false, 1000,
                std::make_pair(100, 100000)
            };
            
            entries["replay_interval"] = ConfigEntrySchema{
                "Number of steps between memory replay cycles",
                "int", false, 100,
                std::make_pair(1, 10000)
            };
            
            entries["consolidation_interval"] = ConfigEntrySchema{
                "Number of steps between memory consolidation cycles",
                "int", false, 1000,
                std::make_pair(100, 100000)
            };
            
            entries["checkpoint_dir"] = ConfigEntrySchema{
                "Directory for checkpoint saves",
                "string", false, "./checkpoints"
            };
            
            entries["checkpoint_interval"] = ConfigEntrySchema{
                "Number of steps between checkpoint saves",
                "int", false, 10000,
                std::make_pair(100, 100000)
            };
            
            entries["enable_visualization"] = ConfigEntrySchema{
                "Enable visualization output",
                "bool", false, false
            };
            
            return schema;
        }
        
        std::string getDescription(const std::string& key) const {
            auto it = schema_.entries.find(key);
            if (it != schema_.entries.end()) {
                return it->second.description;
            }
            return "Unknown configuration parameter";
        }
        
        void initializeDefaults() {
            // Set default values from schema
            for (const auto& [key, schema] : schema_.entries) {
                if (std::holds_alternative<int>(schema.defaultValue)) {
                    setInt(key, std::get<int>(schema.defaultValue));
                } else if (std::holds_alternative<int64_t>(schema.defaultValue)) {
                    setInt64(key, std::get<int64_t>(schema.defaultValue));
                } else if (std::holds_alternative<double>(schema.defaultValue)) {
                    setDouble(key, std::get<double>(schema.defaultValue));
                } else if (std::holds_alternative<std::string>(schema.defaultValue)) {
                    setString(key, std::get<std::string>(schema.defaultValue));
                } else if (std::holds_alternative<bool>(schema.defaultValue)) {
                    setBool(key, std::get<bool>(schema.defaultValue));
                }
            }
        }
        
        bool loadFromSimpleFormat(const std::string& filepath) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    // Remove quotes if present
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    setFromString(key, value);
                }
            }
            
            return true;
        }
        
        static std::string trim(const std::string& str) {
            size_t start = str.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) return "";
            size_t end = str.find_last_not_of(" \t\r\n");
            return str.substr(start, end - start + 1);
        }
    };
}; // namespace nlm

#endif // NLM_IMPROVED_CONFIG_H
