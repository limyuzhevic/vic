#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <ctime>

namespace nlm {

// Helper functions for JSON serialization
namespace {

// Convert ConfigValue to JSON
define_json_serializers_for_variant(ConfigValue) {
    std::visit(overloaded {
        [](int value) { return json(value); },
        [](int64_t value) { return json(value); },
        [](double value) { return json(value); },
        [](bool value) { return json(value); },
        [](const std::string& value) { return json(value); },
        [](const std::vector<int>& value) { return json(value); },
        [](const std::vector<double>& value) { return json(value); },
        [](const std::vector<std::string>& value) { return json(value); }
    }, value);
}

// Convert JSON to ConfigValue
ConfigValue define_json_deserializers_for_variant(json j) {
    if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Try to infer array type
        if (!j.empty()) {
            auto first = j[0];
            if (first.is_number_integer()) {
                std::vector<int> vec = j.get<std::vector<int>>();
                return vec;
            } else if (first.is_number()) {
                std::vector<double> vec = j.get<std::vector<double>>();
                return vec;
            } else if (first.is_string()) {
                std::vector<std::string> vec = j.get<std::vector<std::string>>();
                return vec;
            }
        }
        return std::vector<int>{};
    }
    return 0;
}

// Validate configuration value
bool validateConfigValue(const std::string& key, const ConfigValue& value, const ConfigValidation& validation) {
    // Implementation for value validation
    return true;
}

// Get current timestamp as string
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<ConfigSchema> schema;
    std::unordered_map<std::string, bool> validationCache;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->schema = std::make_shared<ConfigSchema>();
    // Initialize with default schema for common configuration options
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Define common configuration parameters with validation
    pImpl->schema->addEntry("random_seed", ConfigValidation{
        "random_seed", "Random seed for reproducible experiments", false, "42", "int"
    });
    
    pImpl->schema->addEntry("simulation_timestep", ConfigValidation{
        "simulation_timestep", "Simulation timestep in seconds", false, "0.001", "double", "0.0 < value <= 1.0"
    });
    
    pImpl->schema->addEntry("neuron_count", ConfigValidation{
        "neuron_count", "Number of neurons in the brain", false, "1000", "int", "value > 0"
    });
    
    pImpl->schema->addEntry("region_count", ConfigValidation{
        "region_count", "Number of brain regions", false, "2", "int", "value >= 1"
    });
    
    pImpl->schema->addEntry("connection_probability", ConfigValidation{
        "connection_probability", "Connection probability for random connectivity", false, "0.1", "double", "0.0 <= value <= 1.0"
    });
    
    pImpl->schema->addEntry("stdp_ltp_weight", ConfigValidation{
        "stdp_ltp_weight", "STDP long-term potentiation weight", false, "0.01", "double", "value > 0"
    });
    
    pImpl->schema->addEntry("stdp_ltd_weight", ConfigValidation{
        "stdp_ltd_weight", "STDP long-term depression weight", false, "0.012", "double", "value > 0"
    });
    
    pImpl->schema->addEntry("stdp_tau", ConfigValidation{
        "stdp_tau", "STDP time constant", false, "20.0", "double", "value > 0"
    });
    
    pImpl->schema->addEntry("synaptogenesis_rate", ConfigValidation{
        "synaptogenesis_rate", "Synaptogenesis rate", false, "0.0001", "double", "value >= 0"
    });
    
    pImpl->schema->addEntry("pruning_rate", ConfigValidation{
        "pruning_rate", "Pruning rate", false, "0.00001", "double", "value >= 0"
    });
    
    pImpl->schema->addEntry("replay_interval", ConfigValidation{
        "replay_interval", "Memory replay interval (steps)", false, "100", "int", "value >= 0"
    });
    
    pImpl->schema->addEntry("consolidation_interval", ConfigValidation{
        "consolidation_interval", "Memory consolidation interval (steps)", false, "1000", "int", "value >= 0"
    });
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then fallback to key=value format
    auto ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json" || ext == "yaml" || ext == "yml") {
        return loadFromJSON(filepath);
    } else {
        return loadFromKeyValue(filepath);
    }
}

bool Config::loadFromKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNum;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
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
            
            if (!setFromString(key, value, ConfigSource::File)) {
                NLM_LOG_WARNING("Failed to parse config key=value pair at line " + std::to_string(lineNum) +
                               ": " + key + " = " + value);
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Invalid config format at line " + std::to_string(lineNum) + ": " + line);
            success = false;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully loaded configuration from key=value file: " + filepath);
    } else {
        NLM_LOG_ERROR("Partial configuration loaded from key=value file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        if (!jsonData.is_object()) {
            NLM_LOG_ERROR("Invalid JSON format in config file: " + filepath + " - expected object at root level");
            return false;
        }
        
        bool success = true;
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            try {
                configValue = jsonToConfigValue(it.value());
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse JSON value for key '" + key + "': " + std::string(e.what()));
                success = false;
                continue;
            }
            
            // Validate and set
            if (pImpl->schema->validate(key, configValue)) {
                set(key, configValue, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Config validation failed for key '" + key + "' in JSON file");
                success = false;
            }
        }
        
        if (success) {
            NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
        } else {
            NLM_LOG_ERROR("Partial configuration loaded from JSON file: " + filepath);
        }
        
        return success;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const nlohmann::json::type_error& e) {
        NLM_LOG_ERROR("JSON type error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Infer array type from first element
        if (!j.empty()) {
            auto first = j[0];
            if (first.is_number_integer()) {
                return j.get<std::vector<int>>();
            } else if (first.is_number()) {
                return j.get<std::vector<double>>();
            } else if (first.is_string()) {
                return j.get<std::vector<std::string>>();
            }
        }
        return std::vector<int>{};
    }
    return 0;
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](const auto& val) -> nlohmann::json {
        return val;
    }, value);
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc == 0 || argv == nullptr) {
        NLM_LOG_ERROR("Invalid command line arguments");
        return false;
    }
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_WARNING("Failed to parse command line argument: --" + key + "=" + value);
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_WARNING("Failed to parse command line argument: -" + key + " " + value);
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Missing value for command line argument: -" + key);
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
        }
    }
    
    return success;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    try {
        // Determine type based on validation schema
        auto validation = pImpl->schema->getValidation(key);
        std::string type = "auto";
        if (validation) {
            type = validation->type;
        }
        
        ConfigValue value;
        
        if (type == "int") {
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos != valueStr.length()) throw std::invalid_argument("Extra characters");
            value = intVal;
        } else if (type == "double") {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos != valueStr.length()) throw std::invalid_argument("Extra characters");
            value = doubleVal;
        } else if (type == "bool") {
            std::string lowerValue = valueStr;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes") {
                value = true;
            } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no") {
                value = false;
            } else {
                throw std::invalid_argument("Invalid boolean value: " + valueStr);
            }
        } else {
            // Auto-detection based on content
            if (valueStr.find('.') != std::string::npos) {
                size_t pos;
                double doubleVal = std::stod(valueStr, &pos);
                if (pos != valueStr.length()) throw std::invalid_argument("Invalid double format");
                value = doubleVal;
            } else if (valueStr.find_first_not_of("0123456789") == std::string::npos) {
                size_t pos;
                int intVal = std::stoi(valueStr, &pos);
                if (pos != valueStr.length()) throw std::invalid_argument("Invalid integer format");
                value = intVal;
            } else if (valueStr == "true" || valueStr == "false" || 
                       valueStr == "1" || valueStr == "0" ||
                       valueStr == "yes" || valueStr == "no") {
                std::string lowerValue = valueStr;
                std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
                value = (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes");
            } else {
                // String value
                value = valueStr;
            }
        }
        
        set(key, value, source);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to convert config value '" + valueStr + "' for key '" + key + "': " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Try to preserve original format based on extension
    auto ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    try {
        if (ext == "json") {
            nlohmann::json jsonData;
            for (const auto& entry : pImpl->entries) {
                jsonData[entry.key] = configValueToJson(entry.value);
            }
            file << std::setw(2) << jsonData << std::endl;
        } else {
            // Default to key=value format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
            }
        }
        
        NLM_LOG_INFO("Successfully saved configuration to file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        if constexpr (std::is_same_v<decltype(val), int>) {
            oss << val;
        } else if constexpr (std::is_same_v<decltype(val), int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<decltype(val), double>) {
            oss << val;
        } else if constexpr (std::is_same_v<decltype(val), bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<decltype(val), std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<decltype(val), std::vector<int>>) {
            oss << "["; 
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
            oss << "["; 
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
            oss << "["; 
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << val[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
