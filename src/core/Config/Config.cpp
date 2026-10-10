#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Cache for quick lookup
    std::unordered_map<std::string, size_t> keyIndex;
    // Validation rules for known configuration keys
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeValidationRules();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeValidationRules() {
    // Add validation rules for known configuration keys
    validationRules["brain.neuron_count"] = [](const ConfigValue& v) {
        if (std::holds_alternative<int64_t>(v)) {
            int64_t val = std::get<int64_t>(v);
            return val > 0 && val <= 100000;
        } else if (std::holds_alternative<int>(v)) {
            int val = std::get<int>(v);
            return val > 0 && val <= 100000;
        }
        return false;
    };
    
    validationRules["brain.synapse_density"] = [](const ConfigValue& v) {
        if (std::holds_alternative<double>(v)) {
            double val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;
        } else if (std::holds_alternative<float>(v)) {
            float val = std::get<float>(v);
            return val >= 0.0f && val <= 1.0f;
        } else if (std::holds_alternative<int>(v)) {
            int val = std::get<int>(v);
            return val >= 0 && val <= 1;
        }
        return false;
    };
    
    validationRules["brain.connection_probability"] = [](const ConfigValue& v) {
        if (std::holds_alternative<double>(v)) {
            double val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;
        } else if (std::holds_alternative<float>(v)) {
            float val = std::get<float>(v);
            return val >= 0.0f && val <= 1.0f;
        } else if (std::holds_alternative<int>(v)) {
            int val = std::get<int>(v);
            return val >= 0 && val <= 1;
        }
        return false;
    };
    
    validationRules["simulation_timestep"] = [](const ConfigValue& v) {
        if (std::holds_alternative<double>(v)) {
            double val = std::get<double>(v);
            return val > 0.0 && val < 1.0; // Reasonable timestep
        } else if (std::holds_alternative<float>(v)) {
            float val = std::get<float>(v);
            return val > 0.0f && val < 1.0f;
        }
        return false;
    };
    
    validationRules["random_seed"] = [](const ConfigValue& v) {
        return std::holds_alternative<int64_t>(v) || std::holds_alternative<int>(v);
    };
    
    validationRules["plasticity.stdp.learning_rate"] = [](const ConfigValue& v) {
        if (std::holds_alternative<double>(v)) {
            double val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;
        } else if (std::holds_alternative<float>(v)) {
            float val = std::get<float>(v);
            return val >= 0.0f && val <= 1.0f;
        } else if (std::holds_alternative<int>(v)) {
            int val = std::get<int>(v);
            return val >= 0 && val <= 1;
        }
        return false;
    };
    
    validationRules["plasticity.hebbian.learning_rate"] = [](const ConfigValue& v) {
        if (std::holds_alternative<double>(v)) {
            double val = std::get<double>(v);
            return val >= 0.0 && val <= 1.0;
        } else if (std::holds_alternative<float>(v)) {
            float val = std::get<float>(v);
            return val >= 0.0f && val <= 1.0f;
        } else if (std::holds_alternative<int>(v)) {
            int val = std::get<int>(v);
            return val >= 0 && val <= 1;
        }
        return false;
    };
}

void Config::validateValue(const std::string& key, const ConfigValue& value) {
    auto it = pImpl->validationRules.find(key);
    if (it != pImpl->validationRules.end()) {
        if (!it->second(value)) {
            throw std::invalid_argument("Invalid value for configuration key '" + key + "': " + getValueString(value));
        }
    }
}

bool Config::validateValueType(const std::string& key, const ConfigValue& value) {
    // Check if the value type is acceptable for this key
    // For now, just check that the value is a valid variant type
    switch (value.index()) {
        case 0: // int
        case 1: // int64_t
        case 2: // double
        case 3: // bool
        case 4: // string
        case 5: // vector<int>
        case 6: // vector<double>
        case 7: // vector<string>
            return true;
        default:
            return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try JSON format first (preferred)
        auto jsonResult = loadFromJson(filepath);
        if (jsonResult) {
            return true;
        }
        
        // Fall back to simple key=value format (for backward compatibility)
        return loadFromSimpleFile(filepath);
    } catch (const std::exception& e) {
        // Log error if logging is available
        return false;
    }
}

bool Config::loadFromJson(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        clear();
        
        // Recursively load JSON into configuration
        loadJsonToConfig(jsonData, "");
        
        return true;
    } catch (const std::exception&) {
        // JSON parsing failed, fall back to simple format
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
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
                
                try {
                    // Try to infer type and validate
                    setValueFromString(key, value, ConfigSource::File);
                } catch (const std::exception& e) {
                    // Log warning about invalid line
                    continue;
                }
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void Config::loadJsonToConfig(const nlohmann::json& jsonData, const std::string& prefix) {
    if (jsonData.is_object()) {
        for (const auto& [key, value] : jsonData.items()) {
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            loadJsonValue(fullKey, value);
        }
    } else if (jsonData.is_array()) {
        // Handle arrays as lists
        for (size_t i = 0; i < jsonData.size(); ++i) {
            std::string fullKey = prefix.empty() ? std::to_string(i) : prefix + ".[" + std::to_string(i) + "]";
            loadJsonValue(fullKey, jsonData[i]);
        }
    } else {
        loadJsonValue(prefix, jsonData);
    }
}

void Config::loadJsonValue(const std::string& key, const nlohmann::json& value) {
    ConfigSource source = ConfigSource::File;
    
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_unsigned()) {
        set(key, static_cast<int64_t>(value.get<uint64_t>()), source);
    } else if (value.is_number()) {
        set(key, value.get<double>(), source);
    } else if (value.is_array()) {
        // Handle different array types
        if (!value.empty()) {
            if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_number_integer(); })) {
                std::vector<int64_t> intArray;
                for (const auto& v : value) {
                    intArray.push_back(v.get<int64_t>());
                }
                set(key, intArray, source);
            } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_number(); })) {
                std::vector<double> doubleArray;
                for (const auto& v : value) {
                    doubleArray.push_back(v.get<double>());
                }
                set(key, doubleArray, source);
            } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_string(); })) {
                std::vector<std::string> stringArray;
                for (const auto& v : value) {
                    stringArray.push_back(v.get<std::string>());
                }
                set(key, stringArray, source);
            }
        }
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    try {
                        setValueFromString(key, value, ConfigSource::CommandLine);
                    } catch (const std::exception&) {
                        // Ignore invalid command line args
                        continue;
                    }
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                try {
                    setValueFromString(key, value, ConfigSource::CommandLine);
                } catch (const std::exception&) {
                    // Ignore invalid command line args
                    continue;
                }
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::string format = "json"; // Default format
        
        // Try to determine format from file extension
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos != std::string::npos) {
            std::string ext = filepath.substr(dotPos + 1);
            if (ext == "yaml" || ext == "yml") {
                format = "yaml";
            } else if (ext == "cfg" || ext == "txt") {
                format = "simple";
            }
        }
        
        if (format == "json") {
            return saveToJson(filepath);
        } else if (format == "yaml") {
            return saveToYaml(filepath);
        } else {
            return saveToSimpleFile(filepath);
        }
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToJson(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            setJsonValue(jsonData, entry.key, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << jsonData << std::endl;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToYaml(const std::string& filepath) const {
    // For now, fall back to JSON (YAML would require a YAML library)
    // In a full implementation, this would use a YAML library
    return saveToJson(filepath);
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << getValueString(entry.value) << "\n";
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void Config::setValueFromString(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to infer type
    if (value == "true" || value == "false") {
        set(key, value == "true", source);
    } else {
        // Try to parse as number
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.size()) {
                set(key, intVal, source);
                return;
            }
            
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleVal, source);
                return;
            }
            
            // If both fail, treat as string
            set(key, value, source);
        } catch (const std::exception&) {
            // Fall back to string
            set(key, value, source);
        }
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
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
    try {
        // Validate value type
        if (!validateValueType(key, value)) {
            throw std::invalid_argument("Invalid value type for configuration key '" + key + "'");
        }
        
        // Apply validation rules if any
        validateValue(key, value);
        
        // Update index cache
        if (pImpl->keyIndex.find(key) != pImpl->keyIndex.end()) {
            // Update existing entry
            size_t index = pImpl->keyIndex[key];
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
        } else {
            // Add new entry
            pImpl->entries.emplace_back(key, value, source);
            pImpl->keyIndex[key] = pImpl->entries.size() - 1;
        }
    } catch (const std::exception&) {
        throw;
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
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return;
    }
    
    size_t index = it->second;
    
    // Remove from vector
    pImpl->entries.erase(pImpl->entries.begin() + index);
    
    // Update indices for remaining entries
    for (auto& pair : pImpl->keyIndex) {
        if (pair.second > index) {
            pair.second--;
        }
    }
    
    // Remove from index
    pImpl->keyIndex.erase(it);
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
    pImpl->keyIndex.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << getValueString(entry.value);
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

void Config::setJsonValue(nlohmann::json& jsonData, const std::string& key, const ConfigValue& value) {
    if (value.index() == 0) { // int
        jsonData[key] = std::get<int>(value);
    } else if (value.index() == 1) { // int64_t
        jsonData[key] = std::get<int64_t>(value);
    } else if (value.index() == 2) { // double
        jsonData[key] = std::get<double>(value);
    } else if (value.index() == 3) { // bool
        jsonData[key] = std::get<bool>(value);
    } else if (value.index() == 4) { // string
        jsonData[key] = std::get<std::string>(value);
    } else if (value.index() == 5) { // vector<int>
        jsonData[key] = std::get<std::vector<int>>(value);
    } else if (value.index() == 6) { // vector<double>
        jsonData[key] = std::get<std::vector<double>>(value);
    } else if (value.index() == 7) { // vector<string>
        jsonData[key] = std::get<std::vector<std::string>>(value);
    }
}

std::string Config::getValueString(const ConfigValue& value) {
    if (value.index() == 0) return std::to_string(std::get<int>(value));
    if (value.index() == 1) return std::to_string(std::get<int64_t>(value));
    if (value.index() == 2) return std::to_string(std::get<double>(value));
    if (value.index() == 3) return std::get<bool>(value) ? "true" : "false";
    if (value.index() == 4) return "\"" + std::get<std::string>(value) + "\"";
    if (value.index() == 5) {
        std::string result = "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            result += std::to_string(vec[i]);
            if (i < vec.size() - 1) result += ", ";
        }
        result += "]";
        return result;
    }
    if (value.index() == 6) {
        std::string result = "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            result += std::to_string(vec[i]);
            if (i < vec.size() - 1) result += ", ";
        }
        result += "]";
        return result;
    }
    if (value.index() == 7) {
        std::string result = "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            result += "\"" + vec[i] + "\"";
            if (i < vec.size() - 1) result += ", ";
        }
        result += "]";
        return result;
    }
    return "";
}

// Explicit template instantiations
// Note: These are needed for the compiler to generate code for these template functions
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