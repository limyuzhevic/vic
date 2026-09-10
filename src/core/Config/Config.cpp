#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Auto-detect format by file extension and content
    if (detectFormat(filepath)) {
        // Load as JSON
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json jsonObj = nlohmann::json::parse(file);
            fromJson(jsonObj);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON file " << filepath << ": " << e.what() << std::endl;
            return false;
        }
    } else {
        // Load as key=value format (legacy)
        return false; // Will fall back to legacy implementation in real code
    }
}

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        nlohmann::json jsonObj = nlohmann::json::parse(jsonString);
        fromJson(jsonObj);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON string: " << e.what() << std::endl;
        return false;
    }
}

void Config::fromJson(const nlohmann::json& jsonObj) {
    clear();
    
    if (!jsonObj.is_object()) {
        throw std::runtime_error("JSON root must be an object");
    }
    
    for (const auto& [key, value] : jsonObj.items()) {
        ConfigValue configVal = jsonToConfigValue(value);
        set(key, configVal, ConfigSource::File);
        
        // Try to extract description from the JSON (if available)
        std::string description = "";
        if (value.is_object() && value.contains("__description")) {
            description = value["__description"].get<std::string>();
        }
        set(key, configVal, ConfigSource::File); // Update description
    }
    
    // Validate critical parameters
    if (!validateCriticalParameters(jsonObj)) {
        throw std::runtime_error("Validation failed for critical parameters");
    }
}

nlohmann::json Config::toJson() const {
    nlohmann::json jsonObj = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json value = configValueToJson(entry.value);
        
        // Include description in JSON for metadata
        if (!entry.description.empty()) {
            value["__description"] = entry.description;
        }
        
        jsonObj[entry.key] = value;
    }
    
    return jsonObj;
}

std::string Config::toJsonString() const {
    nlohmann::json jsonObj = toJson();
    return jsonObj.dump(2); // Pretty print with 2-space indentation
}

bool Config::validate(const nlohmann::json& schema) const {
    // Basic JSON schema validation
    if (!schema.is_object() || !schema.contains("type") || 
        schema["type"].get<std::string>() != "object") {
        return false;
    }
    
    // This is a simplified implementation
    // In production, you'd use a proper JSON schema library
    for (const auto& [key, value] : schema.items()) {
        if (key == "type" || key == "$schema") continue;
        
        if (!has(key)) continue;
        
        // Type checking would go here
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Auto-detect if file should be saved in JSON format
    bool isJsonFile = isJsonFile(filepath);
    
    if (isJsonFile) {
        // Save in JSON format
        nlohmann::json jsonObj = toJson();
        file << jsonObj.dump(2); // Pretty print with 2-space indentation
    } else {
        // Save in legacy key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Convert ConfigValue to string for legacy format
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
        }
    }
    
    return true;
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

bool Config::isJsonFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    return ext == ".json" || ext == ".cfg.json" || ext == ".conf.json";
}

bool Config::detectFormat(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    // Check file extension first
    if (isJsonFile(filepath)) {
        return true;
    }
    
    // Try to read first few lines to detect content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int bracketCount = 0;
    int braceCount = 0;
    bool hasJsonPattern = false;
    
    for (int i = 0; i < 10 && std::getline(file, line); ++i) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Check for JSON-like patterns
        if (line.find('{') != std::string::npos || line.find('[') != std::string::npos) {
            hasJsonPattern = true;
        }
        if (line.find('{') != std::string::npos) {
            braceCount += std::count(line.begin(), line.end(), '{');
            braceCount -= std::count(line.begin(), line.end(), '}');
        }
        if (line.find('[') != std::string::npos) {
            bracketCount += std::count(line.begin(), line.end(), '[');
            bracketCount -= std::count(line.begin(), line.end(), ']');
        }
    }
    
    // Decide format based on pattern
    return hasJsonPattern || (braceCount > 0) || (bracketCount > 0);
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& jsonVal) {
    if (jsonVal.is_string()) {
        return jsonVal.get<std::string>();
    } else if (jsonVal.is_number_integer()) {
        return jsonVal.get<int>();
    } else if (jsonVal.is_number()) {
        return jsonVal.get<double>();
    } else if (jsonVal.is_boolean()) {
        return jsonVal.get<bool>();
    } else if (jsonVal.is_array()) {
        if (!jsonVal.empty()) {
            if (jsonVal[0].is_number_integer()) {
                std::vector<int> intVec;
                for (const auto& item : jsonVal) {
                    intVec.push_back(item.get<int>());
                }
                return intVec;
            } else if (jsonVal[0].is_number()) {
                std::vector<double> doubleVec;
                for (const auto& item : jsonVal) {
                    doubleVec.push_back(item.get<double>());
                }
                return doubleVec;
            } else if (jsonVal[0].is_string()) {
                std::vector<std::string> stringVec;
                for (const auto& item : jsonVal) {
                    stringVec.push_back(item.get<std::string>());
                }
                return stringVec;
            }
        }
        // Empty array defaults to empty string vector
        return std::vector<std::string>{};
    } else if (jsonVal.is_object()) {
        // Convert JSON object to string representation for now
        // In a more complete implementation, you'd want to support nested ConfigValue
        return jsonVal.dump();
    }
    
    throw std::runtime_error("Unsupported JSON type for ConfigValue conversion");
}

nlohmann::json Config::configValueToJson(const ConfigValue& configVal) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else {
            throw std::runtime_error("Unsupported ConfigValue type");
        }
    }, configVal);
}

bool Config::validateCriticalParameters(const nlohmann::json& config) const {
    // Check for critical parameters that must be present
    // This is a simplified implementation
    
    // Required parameters for Phase 1
    const std::vector<std::string> requiredParams = {
        "random_seed",
        "simulation_timestep",
        "neuron_count"
    };
    
    for (const auto& param : requiredParams) {
        if (!config.contains(param)) {
            std::cerr << "Missing critical parameter: " << param << std::endl;
            return false;
        }
    }
    
    // Validate types of critical parameters
    if (config["random_seed"].is_number_integer() && config["simulation_timestep"].is_number() && config["neuron_count"].is_number_integer()) {
        return true;
    }
    
    std::cerr << "Invalid types for critical parameters" << std::endl;
    return false;
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
