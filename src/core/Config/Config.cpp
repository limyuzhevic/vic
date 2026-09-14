#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <set>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigEntry> entryIndex;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::string> categories;
    std::vector<std::string> profiles;
    
    // Schema validation
    bool strictMode = false;
    std::vector<std::string> requiredKeys;
    std::unordered_map<std::string, ConfigValue> defaults;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr;
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        other.pImpl = nullptr;
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check for empty or invalid filepath
    if (filepath.empty()) {
        NLM_LOG_ERROR("Empty filepath provided to Config::loadFromFile");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check if file is readable and valid
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        NLM_LOG_ERROR("Config file is empty: " + filepath);
        return false;
    }
    file.seekg(0, std::ios::beg);
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_ERROR("Invalid line format in config file " + filepath + ": line " + std::to_string(lineNum) + " - missing '='");
            continue;  // Skip malformed lines but continue processing
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_ERROR("Empty key found in config file " + filepath + ": line " + std::to_string(lineNum));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    // Check if file is corrupted by reading it back
    if (file.bad() || file.fail()) {
        NLM_LOG_ERROR("File read error occurred while processing: " + filepath);
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
            return false;
        }
        
        nlohmann::json configJson;
        file >> configJson;
        
        // Validate structure
        if (!configJson.is_object()) {
            NLM_LOG_ERROR("Invalid JSON format in config file: " + filepath);
            return false;
        }
        
        clear();
        
        // Process JSON data
        for (auto& [key, value] : configJson.items()) {
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            
            // Convert JSON value to ConfigValue
            if (value.is_object() && value.contains("__type")) {
                // Check for type-specific values
                std::string type = value["__type"].get<std::string>();
                if (type == "int") {
                    entry.value = value["value"].get<int>();
                } else if (type == "int64") {
                    entry.value = value["value"].get<int64_t>();
                } else if (type == "double") {
                    entry.value = value["value"].get<double>();
                } else if (type == "bool") {
                    entry.value = value["value"].get<bool>();
                } else if (type == "string") {
                    entry.value = value["value"].get<std::string>();
                } else if (type == "vector<int>") {
                    entry.value = value["value"].get<std::vector<int>>();
                } else if (type == "vector<double>") {
                    entry.value = value["value"].get<std::vector<double>>();
                } else if (type == "vector<string>") {
                    entry.value = value["value"].get<std::vector<std::string>>();
                } else {
                    NLM_LOG_WARNING("Unknown type in JSON config: " + type + " for key: " + key);
                    continue;
                }
            } else {
                // Simple JSON value
                if (value.is_number_integer()) {
                    entry.value = value.get<int>();
                } else if (value.is_number_unsigned()) {
                    entry.value = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    entry.value = value.get<double>();
                } else if (value.is_boolean()) {
                    entry.value = value.get<bool>();
                } else if (value.is_string()) {
                    entry.value = value.get<std::string>();
                } else if (value.is_array()) {
                    // Try to infer array type
                    if (!value.empty()) {
                        if (value[0].is_number_integer()) {
                            entry.value = value.get<std::vector<int>>();
                        } else if (value[0].is_number_float()) {
                            entry.value = value.get<std::vector<double>>();
                        } else {
                            entry.value = value.get<std::vector<std::string>>();
                        }
                    }
                }
            }
            
            // Get description if present
            if (value.contains("description")) {
                entry.description = value["description"].get<std::string>();
                pImpl->descriptions[key] = entry.description;
            }
            
            pImpl->entries.push_back(entry);
            pImpl->entryIndex[key] = entry;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Get configuration schema
std::string Config::getSchema() const {
    std::ostringstream oss;
    oss << "Config Schema:";
    oss << "\nStrict mode: " << (pImpl->strictMode ? "ON" : "OFF");
    oss << "\nRequired keys: " << pImpl->requiredKeys.size();
    oss << "\nDefault values: " << pImpl->defaults.size();
    oss << "\nValidators: " << pImpl->validators.size();
    oss << "\nProfiles: " << pImpl->profiles.size();
    return oss.str();
}

// Export configuration to JSON with schema
bool Config::saveToJSONWithSchema(const std::string& filepath) const {
    nlohmann::json schemaJson;
    
    // Add schema information
    schemaJson["schema_version"] = "1.0";
    schemaJson["strict_mode"] = pImpl->strictMode;
    
    // Add required keys
    if (!pImpl->requiredKeys.empty()) {
        schemaJson["required_keys"] = pImpl->requiredKeys;
    }
    
    // Add default values
    if (!pImpl->defaults.empty()) {
        nlohmann::json defaultsJson;
        for (const auto& [key, value] : pImpl->defaults) {
            nlohmann::json valueJson;
            if (std::holds_alternative<int>(value)) {
                valueJson["__type"] = "int";
                valueJson["value"] = std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                valueJson["__type"] = "int64";
                valueJson["value"] = std::get<int64_t>(value);
            } else if (std::holds_alternative<double>(value)) {
                valueJson["__type"] = "double";
                valueJson["value"] = std::get<double>(value);
            } else if (std::holds_alternative<bool>(value)) {
                valueJson["__type"] = "bool";
                valueJson["value"] = std::get<bool>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                valueJson["__type"] = "string";
                valueJson["value"] = std::get<std::string>(value);
            }
            defaultsJson[key] = valueJson;
        }
        schemaJson["default_values"] = defaultsJson;
    }
    
    // Add current configuration
    nlohmann::json configJson;
    for (const auto& entry : pImpl->entries) {
        nlohmann::json valueJson;
        if (std::holds_alternative<int>(entry.value)) {
            valueJson["__type"] = "int";
            valueJson["value"] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            valueJson["__type"] = "int64";
            valueJson["value"] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            valueJson["__type"] = "double";
            valueJson["value"] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            valueJson["__type"] = "bool";
            valueJson["value"] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            valueJson["__type"] = "string";
            valueJson["value"] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            valueJson["__type"] = "vector<int>";
            valueJson["value"] = std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            valueJson["__type"] = "vector<double>";
            valueJson["value"] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            valueJson["__type"] = "vector<string>";
            valueJson["value"] = std::get<std::vector<std::string>>(entry.value);
        }
        
        if (pImpl->descriptions.find(entry.key) != pImpl->descriptions.end()) {
            valueJson["description"] = pImpl->descriptions[entry.key];
        }
        
        if (pImpl->categories.find(entry.key) != pImpl->categories.end()) {
            valueJson["category"] = pImpl->categories[entry.key];
        }
        
        configJson[entry.key] = valueJson;
    }
    
    schemaJson["configuration"] = configJson;
    
    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << schemaJson.dump(2);
    return true;
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
