#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load configuration from file with automatic format detection
    // Supports both simple key=value format and JSON/YAML formats
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
        return false;
    }
    
    // Detect file format based on content
    bool isJSON = isJSONFormat(content);
    bool isYAML = isYAMLFormat(content);
    
    if (isJSON) {
        NLM_LOG_INFO("Loading JSON configuration from: " + filepath);
        return loadFromJSON(content);
    } else if (isYAML) {
        NLM_LOG_INFO("Loading YAML configuration from: " + filepath);
        return loadFromYAML(content);
    } else {
        // Fall back to simple key=value format for backward compatibility
        NLM_LOG_INFO("Loading simple key=value format from: " + filepath);
        return loadFromSimpleFormat(content);
    }
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

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    // Load configuration from file with automatic format detection
    // Supports both simple key=value format and JSON/YAML formats
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
        return false;
    }
    
    // Detect file format based on content
    bool isJSON = isJSONFormat(content);
    bool isYAML = isYAMLFormat(content);
    
    if (isJSON) {
        NLM_LOG_INFO("Loading JSON configuration from: " + filepath);
        return loadFromJSON(content);
    } else if (isYAML) {
        NLM_LOG_INFO("Loading YAML configuration from: " + filepath);
        return loadFromYAML(content);
    } else {
        // Fall back to simple key=value format for backward compatibility
        NLM_LOG_INFO("Loading simple key=value format from: " + filepath);
        return loadFromSimpleFormat(content);
    }
}

// Helper function to detect JSON format
bool Config::isJSONFormat(const std::string& content) {
    // Simple JSON detection - looks for { } or [ ] or "key": value patterns
    if (content.empty()) return false;
    
    // Remove whitespace for faster checking
    std::string normalized;
    for (char c : content) {
        if (!std::isspace(c)) {
            normalized += c;
        }
    }
    
    // Check for JSON array format
    if (normalized.front() == '[' && normalized.back() == ']') {
        return true;
    }
    
    // Check for JSON object format with key-value pairs
    if (normalized.front() == '{' && normalized.back() == '}') {
        // Look for "key": value pattern
        size_t colonPos = normalized.find(':');
        if (colonPos != std::string::npos) {
            // Find a quote before the colon
            size_t quotePos = normalized.rfind('"', colonPos);
            if (quotePos != std::string::npos) {
                return true;
            }
        }
    }
    
    return false;
}

// Helper function to detect YAML format
bool Config::isYAMLFormat(const std::string& content) {
    // Simple YAML detection - looks for key: value patterns with indentation
    if (content.empty()) return false;
    
    std::istringstream stream(content);
    std::string line;
    int indentLevel = -1;
    int nonEmptyLines = 0;
    
    while (std::getline(stream, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed.front() == '#' || trimmed.front() == '%') {
            continue; // Skip comments and directives
        }
        
        // Count leading spaces for indentation
        size_t spaces = 0;
        while (spaces < line.length() && line[spaces] == ' ') {
            spaces++;
        }
        
        // First non-empty, non-comment line determines base indent
        if (indentLevel == -1) {
            indentLevel = spaces;
        }
        
        // Check for key: value pattern (no quotes around key)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > spaces) {
            // Found key: value pattern
            nonEmptyLines++;
            
            // Ensure indentation is consistent (YAML uses indentation)
            if (indentLevel > 0 && spaces % indentLevel != 0) {
                return false; // Not consistent YAML indentation
            }
        }
    }
    
    return nonEmptyLines > 0; // Consider it YAML if we found key-value patterns
}

// Load configuration from simple key=value format (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    bool success = true;
    
    while (std::getline(stream, line)) {
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
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing config line: " + line + " - " + e.what());
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Skipping invalid config line: " + line);
            success = false;
        }
    }
    
    return success;
}

// Load configuration from JSON format
bool Config::loadFromJSON(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        
        if (json.is_object()) {
            for (auto it = json.begin(); it != json.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue = jsonValueToConfigValue(*it);
                set(key, configValue, ConfigSource::File);
            }
            return true;
        } else {
            NLM_LOG_ERROR("JSON root element is not an object");
            return false;
        }
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

// Load configuration from YAML format
bool Config::loadFromYAML(const std::string& content) {
    try {
        auto yaml = YAML::Load(content);
        
        if (yaml.IsMap()) {
            for (const auto& yamlPair : yaml) {
                std::string key = yamlPair.first.as<std::string>();
                
                // Convert YAML value to ConfigValue
                ConfigValue configValue = yamlValueToConfigValue(yamlPair.second);
                set(key, configValue, ConfigSource::File);
            }
            return true;
        } else {
            NLM_LOG_ERROR("YAML root element is not a mapping");
            return false;
        }
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

// Convert JSON value to ConfigValue
ConfigValue Config::jsonValueToConfigValue(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        std::vector<ConfigValue> result;
        for (const auto& element : json) {
            result.push_back(jsonValueToConfigValue(element));
        }
        // Convert vector<ConfigValue> to appropriate type
        // For now, handle specific array types
        if (allElementsAreType<std::vector<int>, int>(json)) {
            std::vector<int> intVec;
            for (const auto& element : json) {
                intVec.push_back(element.get<int>());
            }
            return intVec;
        } else if (allElementsAreType<std::vector<double>, double>(json)) {
            std::vector<double> doubleVec;
            for (const auto& element : json) {
                doubleVec.push_back(element.get<double>());
            }
            return doubleVec;
        } else if (allElementsAreType<std::vector<std::string>, std::string>(json)) {
            std::vector<std::string> stringVec;
            for (const auto& element : json) {
                stringVec.push_back(element.get<std::string>());
            }
            return stringVec;
        }
        // Fall back to generic vector
        return result;
    } else {
        // Null or unknown type - return empty string
        return std::string("");
    }
}

// Convert YAML value to ConfigValue
ConfigValue Config::yamlValueToConfigValue(const YAML::Node& yaml) {
    if (yaml.IsScalar()) {
        // Try different scalar types
        if (yaml.IsInt()) {
            return yaml.as<int64_t>();
        } else if (yaml.IsFloat()) {
            return yaml.as<double>();
        } else if (yaml.IsBool()) {
            return yaml.as<bool>();
        } else {
            return yaml.as<std::string>();
        }
    } else if (yaml.IsSequence()) {
        std::vector<ConfigValue> result;
        for (const auto& element : yaml) {
            result.push_back(yamlValueToConfigValue(element));
        }
        
        // Try to determine the specific vector type
        bool allInt = true;
        bool allDouble = true;
        bool allString = true;
        
        for (const auto& element : yaml) {
            if (!element.IsInt()) allInt = false;
            if (!element.IsFloat()) allDouble = false;
            if (!element.IsScalar() || (element.as<std::string>().empty() && !std::isdigit(element.as<std::string>()[0]))) {
                allString = false;
            }
        }
        
        if (allInt) {
            std::vector<int> intVec;
            for (const auto& element : yaml) {
                intVec.push_back(element.as<int>());
            }
            return intVec;
        } else if (allDouble) {
            std::vector<double> doubleVec;
            for (const auto& element : yaml) {
                doubleVec.push_back(element.as<double>());
            }
            return doubleVec;
        } else if (allString) {
            std::vector<std::string> stringVec;
            for (const auto& element : yaml) {
                stringVec.push_back(element.as<std::string>());
            }
            return stringVec;
        }
        
        return result;
    } else if (yaml.IsMap()) {
        // YAML maps are not directly supported in ConfigValue
        // Convert to string representation
        std::stringstream ss;
        ss << yaml;
        return ss.str();
    } else {
        // Null or unknown type
        return std::string("");
    }
}

// Helper to check if all JSON array elements are of a specific type
template<typename T, typename ElementType>
bool Config::allElementsAreType(const nlohmann::json& json) {
    for (const auto& element : json) {
        if (!element.is<ElementType>()) {
            return false;
        }
    }
    return true;
}

// Improved saveToFile with proper value storage
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM on " << std::time(nullptr) << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write the key with proper formatting based on value type
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
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
