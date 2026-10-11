#include "nlohmann/json.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Determine file format by extension
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        // TODO: Implement YAML parsing (nlohmann/json supports YAML via include)
        return loadFromJSON(filepath);  // Temporary fallback
    } else {
        // Assume simple key=value format for backward compatibility
        NLM_LOG_WARNING("Unknown config file format: " + ext + ". Using simple key=value parsing.");
        return loadFromSimpleFormat(filepath);
    }
}

// Load from simple key=value format (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with better error handling
        size_t pos = trimmedLine.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(trimmedLine.substr(0, pos));
            std::string value = trim(trimmedLine.substr(pos + 1));
            
            if (key.empty()) {
                NLM_LOG_WARNING("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                              " has empty key, skipping");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                             " error parsing key '" + key + "': " + e.what());
                // Continue with other lines instead of failing completely
                continue;
            }
        } else {
            NLM_LOG_WARNING("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                          " has invalid format, expected key=value");
        }
    }
    
    return true;
}

// Load from JSON format
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to ConfigEntry objects
        convertJSONToConfig(jsonData);
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Convert JSON data to ConfigEntry objects
void Config::convertJSONToConfig(const nlohmann::json& jsonData) {
    if (!jsonData.is_object()) {
        throw std::runtime_error("JSON data is not an object");
    }
    
    for (const auto& [key, value] : jsonData.items()) {
        try {
            ConfigValue configValue;
            std::string description;
            
            // Handle different JSON types
            if (value.is_string()) {
                configValue = value.get<std::string>();
                description = "String value";
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
                description = "Integer value";
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
                description = "Unsigned integer value";
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
                description = "Float value";
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
                description = "Boolean value";
            } else if (value.is_array()) {
                // Try to infer array type
                if (!value.empty()) {
                    if (value[0].is_number_integer()) {
                        configValue = value.get<std::vector<int64_t>>();
                        description = "Integer array";
                    } else if (value[0].is_number_float()) {
                        configValue = value.get<std::vector<double>>();
                        description = "Float array";
                    } else if (value[0].is_string()) {
                        configValue = value.get<std::vector<std::string>>();
                        description = "String array";
                    } else {
                        throw std::runtime_error("Unsupported array type in JSON");
                    }
                } else {
                    configValue = std::vector<int64_t>{};
                    description = "Empty array (int)";
                }
            } else {
                throw std::runtime_error("Unsupported JSON type for key: " + key);
            }
            
            set(key, configValue, ConfigSource::File, description);
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error processing JSON key '" + key + "': " + std::string(e.what()));
            // Continue with other keys instead of failing completely
            continue;
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write actual value based on its type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else {
                file << val;
            }
        }, entry.value);
        file << "\n";
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description; // Update description
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
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
