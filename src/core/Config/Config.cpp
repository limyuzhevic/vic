#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <optional>

#ifdef USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef USE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert string to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string format = "simple";  // "simple", "json", "yaml"
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->format = "simple"; // Default to simple key=value format
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string extension = ".";
    if (!filepath.empty()) {
        size_t lastDot = filepath.find_last_of('.');
        if (lastDot != std::string::npos) {
            extension = filepath.substr(lastDot);
        }
    }
    
    pImpl->format = toLower(extension);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Parse based on format
    bool success = false;
    if (extension == ".json" || extension == ".json") {
        success = loadFromJSON(file);
    } else if (extension == ".yaml" || extension == ".yml") {
        success = loadFromYAML(file);
    } else {
        // Try to detect format from content
        std::string firstLine;
        std::getline(file, firstLine);
        file.clear();
        file.seekg(0, std::ios::beg);
        
        // Check if it looks like JSON (starts with { or [)
        if (!firstLine.empty() && (firstLine.find('{') != std::string::npos || 
            firstLine.find('[') != std::string::npos)) {
            success = loadFromJSON(file);
            pImpl->format = "json";
        }
        // Check if it looks like YAML (has colon or dash at start of lines)
        else if (!firstLine.empty() && (firstLine.find(':') != std::string::npos || 
            (firstLine.find(' ') != std::string::npos && 
             (std::isalpha(firstLine[0]) || firstLine[0] == '-')))) {
            success = loadFromYAML(file);
            pImpl->format = "yaml";
        } else {
            // Fall back to simple format
            success = loadFromSimple(file);
            pImpl->format = "simple";
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from " + filepath + " (format: " + pImpl->format + ")");
    } else {
        NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
    }
    return success;
}

bool Config::loadFromSimple(std::istream& stream) {
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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
            
            // Try to parse value as different types
            setFromString(key, value);
        }
    }
    
    return true;
}

#ifdef USE_NLOHMANN_JSON
bool Config::loadFromJSON(std::istream& stream) {
    try {
        nlohmann::json jsonData;
        stream >> jsonData;
        
        // Convert JSON to config entries
        convertJSONToConfig(jsonData);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
        return false;
    }
}

void Config::convertJSONToConfig(const nlohmann::json& jsonData) {
    clear();
    
    if (jsonData.is_object()) {
        for (const auto& item : jsonData.items()) {
            std::string key = item.key();
            const auto& value = item.value();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue = convertJSONValue(value);
            set(key, configValue, ConfigSource::File);
        }
    }
}

Config::ConfigValue Config::convertJSONValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_null()) {
        return std::string("null");
    } else if (jsonValue.is_array()) {
        std::vector<double> array;
        for (const auto& item : jsonValue) {
            if (item.is_number()) {
                array.push_back(item.get<double>());
            } else {
                array.push_back(0.0); // Default for non-numeric values
            }
        }
        return array;
    } else if (jsonValue.is_object()) {
        // For nested objects, convert to JSON string representation
        return jsonValue.dump();
    } else {
        return std::string(""); // Empty string as fallback
    }
}
#endif

#ifdef USE_YAML_CPP
bool Config::loadFromYAML(std::istream& stream) {
    try {
        YAML::Node yamlData = YAML::Load(stream);
        
        // Convert YAML to config entries
        convertYAMLToConfig(yamlData);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML parsing error: ") + e.what());
        return false;
    }
}

void Config::convertYAMLToConfig(const YAML::Node& yamlData) {
    clear();
    
    if (yamlData.IsMap()) {
        for (const auto& item : yamlData) {
            std::string key = item.first.as<std::string>();
            YAML::Node value = item.second;
            
            // Convert YAML value to ConfigValue
            ConfigValue configValue = convertYAMLValue(value);
            set(key, configValue, ConfigSource::File);
        }
    }
}

Config::ConfigValue Config::convertYAMLValue(const YAML::Node& yamlValue) {
    if (yamlValue.IsScalar()) {
        std::string scalar = yamlValue.as<std::string>();
        
        // Try to parse as different types
        if (scalar == "true" || scalar == "false") {
            return scalar == "true";
        }
        
        // Try integer
        try {
            if (scalar.find('.') == std::string::npos && scalar.find('e') == std::string::npos) {
                // Check if it's within int64_t range
                int64_t intValue = std::stoll(scalar);
                return intValue;
            }
        } catch (...) {}
        
        // Try double
        try {
            double doubleValue = std::stod(scalar);
            return doubleValue;
        } catch (...) {}
        
        // Default to string
        return scalar;
    } else if (yamlValue.IsSequence()) {
        std::vector<double> array;
        for (const auto& item : yamlValue) {
            if (item.IsScalar()) {
                try {
                    double value = std::stod(item.as<std::string>());
                    array.push_back(value);
                } catch (...) {
                    array.push_back(0.0);
                }
            }
        }
        return array;
    } else {
        return std::string("[unknown]"); // Fallback for complex YAML
    }
}
#endif

bool Config::loadFromSimple(const std::string& content) {
    std::istringstream stream(content);
    return loadFromSimple(stream);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Use format to determine output format
    bool success = false;
    if (pImpl->format == "json") {
        success = saveToJSON(file);
    } else if (pImpl->format == "yaml" || pImpl->format == ".yaml" || pImpl->format == ".yml") {
        success = saveToYAML(file);
    } else {
        success = saveToSimple(file);
    }
    
    if (success) {
        NLM_LOG_INFO("Saved configuration to " + filepath + " (format: " + pImpl->format + ")");
    }
    return success;
}

bool Config::saveToSimple(std::ostream& stream) const {
    for (const auto& entry : pImpl->entries) {
        stream << "# " << entry.description << "\n";
        
        // Format value based on its type
        std::visit([&stream](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings with special characters
                if (value.find(' ') != std::string::npos || value.find('=') != std::string::npos) {
                    stream << entry.key << " = \"" << value << "\"\n";
                } else {
                    stream << entry.key << " = " << value << "\n";
                }
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, uint64_t> ||
                                std::is_same_v<T, double>) {
                stream << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                stream << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else {
                stream << entry.key << " = " << "PLACEHOLDER_VALUE\n";
            }
        }, entry.value);
        stream << "\n";
    }
    return true;
}

#ifdef USE_NLOHMANN_JSON
bool Config::saveToJSON(std::ostream& stream) const {
    try {
        nlohmann::json jsonData;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            std::visit([&jsonData, &entry](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = value;
                } else {
                    jsonData[entry.key] = "PLACEHOLDER_VALUE";
                }
            }, entry.value);
        }
        
        // Write formatted JSON
        stream << jsonData.dump(2);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON writing error: ") + e.what());
        return false;
    }
}
#endif

#ifdef USE_YAML_CPP
bool Config::saveToYAML(std::ostream& stream) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value;
            
            std::visit([&emitter](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << value;
                } else if constexpr (std::is_same_v<T, int64_t> || 
                                    std::is_same_v<T, uint64_t>) {
                    emitter << static_cast<int64_t>(value);
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << (value ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::FlowSeq;
                    for (const auto& item : value) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else {
                    emitter << "PLACEHOLDER_VALUE";
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        stream << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("YAML writing error: ") + e.what());
        return false;
    }
}
#endif

void Config::setFromString(const std::string& key, const std::string& valueStr) {
    // Try to parse as different types in order of preference
    bool boolSuccess = false;
    int64_t intSuccess = 0;
    double doubleSuccess = 0.0;
    
    // Boolean
    if (toLower(valueStr) == "true") {
        set(key, true, ConfigSource::File);
        return;
    } else if (toLower(valueStr) == "false") {
        set(key, false, ConfigSource::File);
        return;
    }
    
    // Integer
    try {
        size_t pos;
        int64_t intValue = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, intValue, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Double
    try {
        size_t pos;
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            set(key, doubleValue, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Default to string
    set(key, valueStr, ConfigSource::File);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, format: " << pImpl->format << "):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, uint64_t>) {
                oss << static_cast<int64_t>(value);
            } else if constexpr (std::is_same_v<T, double>) {
                oss << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : value) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else {
                oss << "UNKNOWN";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Explicit template instantiations
// [Keep the existing template instantiations from Config.cpp]

} // namespace nlm

// Template function definitions (simplified for brevity)
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

void Config::clear() {
    pImpl->entries.clear();
    pImpl->format = "simple";
}

// Existing template instantiations (keep these from original)
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
