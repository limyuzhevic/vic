#include "Config.hpp"
#include "NLMError.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <system_error>
#include <cerrno>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    try {
        // Validate format parameter
        if (format != "auto" && format != "simple" && format != "json" && format != "yaml") {
            setError("Invalid format: " + format);
            return false;
        }
        
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            setError("File does not exist: " + filepath);
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            setError("Path is not a regular file: " + filepath);
            return false;
        }
        
        if (!std::filesystem::is_readable(filepath)) {
            setError("File is not readable: " + filepath);
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            setError(std::string("Cannot open file: ") + filepath + ". " + 
                    std::strerror(errno));
            return false;
        }
        
        // Determine format if auto-detected
        std::string actualFormat = format;
        if (actualFormat == "auto") {
            actualFormat = detectFormat(filepath);
        }
        
        // Load based on format
        if (actualFormat == "simple") {
            return parseSimpleFormat(filepath);
        } else if (actualFormat == "json") {
            #ifdef HAS_NLOHMANN_JSON
            return parseJSONFormat(filepath);
            #else
            setError("JSON support not compiled into this build");
            return false;
            #endif
        } else if (actualFormat == "yaml") {
            #ifdef HAS_YAML_CPP
            return parseYAMLFormat(filepath);
            #else
            setError("YAML support not compiled into this build");
            return false;
            #endif
        }
        
        setError("Unsupported format: " + actualFormat);
        return false;
        
    } catch (const ConfigError& e) {
        setError(std::string("Config error: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        setError(std::string("Unexpected error: ") + e.what());
        return false;
    }
}

bool Config::loadFromString(const std::string& content, const std::string& format) {
    try {
        // Validate format parameter
        if (format != "auto" && format != "simple" && format != "json" && format != "yaml") {
            setError("Invalid format: " + format);
            return false;
        }
        
        if (content.empty()) {
            setError("Empty content provided");
            return false;
        }
        
        // Determine format if auto-detected
        std::string actualFormat = format;
        if (actualFormat == "auto") {
            actualFormat = detectFormatFromContent(content);
        }
        
        // Load based on format
        if (actualFormat == "simple") {
            clear();
            return parseSimpleFormat(content);
        } else if (actualFormat == "json") {
            #ifdef HAS_NLOHMANN_JSON
            clear();
            return parseJSONFormat(content);
            #else
            setError("JSON support not compiled into this build");
            return false;
            #endif
        } else if (actualFormat == "yaml") {
            #ifdef HAS_YAML_CPP
            clear();
            return parseYAMLFormat(content);
            #else
            setError("YAML support not compiled into this build");
            return false;
            #endif
        }
        
        setError("Unsupported format: " + actualFormat);
        return false;
        
    } catch (const ConfigError& e) {
        setError(std::string("Config error: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        setError(std::string("Unexpected error: ") + e.what());
        return false;
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// File format detection methods
std::string Config::detectFormat(const std::string& filename) {
    if (filename.empty()) return "simple";
    
    std::string ext = filename;
    size_t dot = ext.find_last_of('.');
    if (dot != std::string::npos) {
        ext = ext.substr(dot + 1);
    }
    
    if (ext == "json" || ext == "JSON") return "json";
    if (ext == "yaml" || ext == "yml" || ext == "YAML") return "yaml";
    if (ext == "cfg" || ext == "conf" || ext == "config") return "simple";
    
    return "simple";  // Default
}

std::string Config::detectFormatFromContent(const std::string& content) {
    if (content.empty()) return "simple";
    
    // Trim whitespace
    std::string trimmed = trim(content);
    
    // Check for JSON (starts with { or [)
    if (trimmed.empty()) return "simple";
    
    if (trimmed.front() == '{' && trimmed.back() == '}') {
        return "json";
    }
    
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        return "json";
    }
    
    // Check for YAML (contains colons at start of lines)
    bool hasColonAfterTab = false;
    size_t pos = 0;
    while ((pos = trimmed.find('\n', pos)) != std::string::npos) {
        pos++;
        size_t end = trimmed.find('\n', pos);
        if (end == std::string::npos) end = trimmed.size();
        
        std::string line = trimmed.substr(pos, end - pos);
        // Trim line
        line = trim(line);
        
        // Check for YAML-like pattern
        if (!line.empty() && line.front() == ':' && line.size() > 1 && line[1] != ':') {
            hasColonAfterTab = true;
            break;
        }
    }
    
    if (hasColonAfterTab) {
        return "yaml";
    }
    
    // Default to simple format
    return "simple";
}

// File parsing implementations
bool Config::parseSimpleFormat(const std::string& content) {
    try {
        clear();
        setError(""); // Clear any previous error
        
        std::istringstream stream(content);
        std::string line;
        size_t lineNum = 0;
        
        while (std::getline(stream, line)) {
            lineNum++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key
                if (key.empty()) {
                    setError(std::string("Empty key at line ") + std::to_string(lineNum));
                    continue; // Skip invalid lines
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate and convert value
                ConfigValue configValue = config_detail::validateValue(key, value);
                
                // Store the entry
                set(key, configValue, ConfigSource::File);
            } else {
                setError(std::string("Invalid format at line ") + std::to_string(lineNum) + ": ");
            }
        }
        
        return true;
        
    } catch (const ConfigError& e) {
        setError(std::string("Config error in parseSimpleFormat: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        setError(std::string("Unexpected error in parseSimpleFormat: ") + e.what());
        return false;
    }
}

bool Config::parseJSONFormat(const std::string& content) {
    // This is a placeholder - would need nlohmann/json to actually implement
    // For now, just set an error indicating the feature needs to be implemented
    setError("JSON parsing not yet implemented - requires nlohmann/json library");
    return false;
}

bool Config::parseYAMLFormat(const std::string& content) {
    // This is a placeholder - would need yaml-cpp to actually implement
    // For now, just set an error indicating the feature needs to be implemented
    setError("YAML parsing not yet implemented - requires yaml-cpp library");
    return false;
}

// Helper function to create error context
ErrorContext Config::createConfigErrorContext(const std::string& key, const std::string& value, 
                                             const std::string& operation) {
    ErrorContext ctx("Config operation failed: " + operation + " for key '" + key + "' with value '" + value + "'");
    ctx.addDetail("key", key);
    ctx.addDetail("value", value);
    ctx.addDetail("operation", operation);
    return ctx;
}

// Helper function to validate value types
ConfigValue Config::validateValue(const std::string& key, const std::string& value) {
    // Try to detect type and convert
    ConfigValue result;
    
    // Check for boolean
    if (value == "true" || value == "false") {
        result = (value == "true");
        return result;
    }
    
    // Check for integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            result = intVal;
            return result;
        }
    } catch (...) {}
    
    // Check for floating point
    try {
        size_t pos;
        double floatVal = std::stod(value, &pos);
        if (pos == value.size()) {
            result = floatVal;
            return result;
        }
    } catch (...) {}
    
    // Check for string
    if (!value.empty()) {
        result = value;
        return result;
    }
    
    // Check for list format: [val1,val2,val3]
    if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
        std::string inner = value.substr(1, value.size() - 2);
        // Simple list parsing - would need more robust implementation for production
        std::vector<double> list;
        size_t pos = 0;
        while (pos < inner.size()) {
            size_t end = inner.find(',', pos);
            if (end == std::string::npos) end = inner.size();
            
            std::string item = inner.substr(pos, end - pos);
            try {
                double val = std::stod(item);
                list.push_back(val);
            } catch (...) {
                // Skip invalid items
            }
            
            pos = end + 1;
        }
        
        if (!list.empty()) {
            // Create appropriate list type - use double for now
            result = list;
            return result;
        }
    }
    
    // Default to string
    result = value;
    return result;
}

// Helper function to convert value back to string for display
std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ",";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ",";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ",";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// JSON parsing implementations
#ifdef HAS_NLOHMANN_JSON
nlohmann::json Config::parseJSON(const std::string& content) {
    try {
        return nlohmann::json::parse(content);
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError("JSON parse error: " + std::string(e.what()));
    }
}

bool Config::parseJSONFormat(const std::string& content) {
    try {
        clear();
        setError(""); // Clear any previous error
        
        auto json = parseJSON(content);
        
        if (!json.is_object()) {
            setError("JSON root element must be an object");
            return false;
        }
        
        // Recursively parse JSON object
        parseJSONObject(json, "");
        
        return true;
        
    } catch (const ConfigError& e) {
        setError(std::string("Config error in parseJSONFormat: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        setError(std::string("Unexpected error in parseJSONFormat: ") + e.what());
        return false;
    }
}

void Config::parseJSONObject(const nlohmann::json& json, const std::string& prefix) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it->is_object()) {
            // Recursively parse nested object
            parseJSONObject(*it, key);
        } else if (it->is_array()) {
            // Parse array
            std::vector<ConfigValue> array;
            for (const auto& item : *it) {
                array.push_back(jsonValueToConfigValue(item));
            }
            set(key, array, ConfigSource::File);
        } else {
            // Simple value
            set(key, jsonValueToConfigValue(*it), ConfigSource::File);
        }
    }
}

ConfigValue Config::jsonValueToConfigValue(const nlohmann::json& json) {
    if (json.is_null()) {
        return std::string("null"); // Represent null as string for now
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number()) {
        return json.get<double>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else {
        // Fallback to string representation
        return json.dump();
    }
}

bool Config::parseJSONFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setError(std::string("Cannot open JSON file: ") + filepath);
        return false;
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        return parseJSONFormat(json.dump());  // Convert back to string
    } catch (const nlohmann::json::parse_error& e) {
        setError(std::string("JSON parse error: ") + e.what());
        return false;
    }
}
#endif // HAS_NLOHMANN_JSON

// YAML parsing implementations  
#ifdef HAS_YAML_CPP
YAML::Node Config::parseYAML(const std::string& content) {
    std::stringstream stream(content);
    YAML::Node doc;
    try {
        doc = YAML::Load(stream);
    } catch (const YAML::Exception& e) {
        throw ConfigError("YAML parse error: " + std::string(e.what()));
    }
    return doc;
}

bool Config::parseYAMLFormat(const std::string& content) {
    try {
        clear();
        setError(""); // Clear any previous error
        
        auto yaml = parseYAML(content);
        
        if (!yaml.IsMap()) {
            setError("YAML root element must be a map");
            return false;
        }
        
        // Recursively parse YAML map
        parseYAMLMap(yaml, "");
        
        return true;
        
    } catch (const ConfigError& e) {
        setError(std::string("Config error in parseYAMLFormat: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        setError(std::string("Unexpected error in parseYAMLFormat: ") + e.what());
        return false;
    }
}

void Config::parseYAMLMap(const YAML::Node& yaml, const std::string& prefix) {
    for (const auto& pair : yaml) {
        std::string key = pair.first.as<std::string>();
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
        
        if (pair.second.IsMap()) {
            // Recursively parse nested map
            parseYAMLMap(pair.second, fullKey);
        } else if (pair.second.IsSequence()) {
            // Parse sequence
            std::vector<ConfigValue> sequence;
            for (const auto& item : pair.second) {
                sequence.push_back(yamlValueToConfigValue(item));
            }
            set(fullKey, sequence, ConfigSource::File);
        } else {
            // Simple value
            set(fullKey, yamlValueToConfigValue(pair.second), ConfigSource::File);
        }
    }
}

ConfigValue Config::yamlValueToConfigValue(const YAML::Node& yaml) {
    if (yaml.IsNull()) {
        return std::string("null"); // Represent null as string for now
    } else if (yaml.IsBoolean()) {
        return yaml.as<bool>();
    } else if (yaml.IsInt()) {
        return yaml.as<int64_t>();
    } else if (yaml.IsDouble()) {
        return yaml.as<double>();
    } else if (yaml.IsString()) {
        return yaml.as<std::string>();
    } else {
        // Fallback to string representation
        return yaml.as<std::string>();
    }
}

bool Config::parseYAMLFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setError(std::string("Cannot open YAML file: ") + filepath);
        return false;
    }
    
    try {
        auto yaml = parseYAML(file);
        return parseYAMLFormat(yaml);
    } catch (const YAML::Exception& e) {
        setError(std::string("YAML parse error: ") + e.what());
        return false;
    }
}
#endif // HAS_YAML_CPP

// Validation implementations
bool Config::validate(const ConfigSchema& schema) const {
    try {
        bool valid = true;
        std::string validationErrors;
        
        for (const auto& [key, constraint] : schema.constraints) {
            if (has(key)) {
                auto value = getValue(key);
                if (!validateConstraint(constraint, value)) {
                    valid = false;
                    validationErrors += "Key '" + key + "' failed validation. ";
                }
            } else if (schema.requireAll) {
                valid = false;
                validationErrors += "Required key '" + key + "' is missing. ";
            }
        }
        
        if (!valid) {
            setError("Configuration validation failed: " + validationErrors);
        }
        
        return valid;
        
    } catch (const std::exception& e) {
        setError(std::string("Error during validation: ") + e.what());
        return false;
    }
}

bool Config::validateConstraint(const ConfigConstraint& constraint, const ConfigValue& value) const {
    switch (constraint.type) {
        case "range": {
            if (std::holds_alternative<int>(value)) {
                int val = std::get<int>(value);
                if (constraint.min.has_value() && val < std::any_cast<int>(constraint.min)) return false;
                if (constraint.max.has_value() && val > std::any_cast<int>(constraint.max)) return false;
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                if (constraint.min.has_value() && val < std::any_cast<double>(constraint.min)) return false;
                if (constraint.max.has_value() && val > std::any_cast<double>(constraint.max)) return false;
            }
            break;
        }
        case "enum": {
            if (!constraint.allowedValues.empty()) {
                bool found = false;
                for (const auto& allowed : constraint.allowedValues) {
                    if (allowed == value) {
                        found = true;
                        break;
                    }
                }
                if (!found) return false;
            }
            break;
        }
        case "regex": {
            if (std::holds_alternative<std::string>(value)) {
                // Would need regex library - simplified for now
                std::string val = std::get<std::string>(value);
                if (!constraint.pattern.empty()) {
                    // Simplified regex check
                    if (val.find(constraint.pattern) == std::string::npos) return false;
                }
            }
            break;
        }
        case "length": {
            if (std::holds_alternative<std::string>(value)) {
                std::string val = std::get<std::string>(value);
                if (constraint.minLength >= 0 && static_cast<size_t>(constraint.minLength) > val.size()) return false;
                if (constraint.maxLength >= 0 && static_cast<size_t>(constraint.maxLength) < val.size()) return false;
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                auto val = std::get<std::vector<int>>(value);
                if (constraint.minLength >= 0 && static_cast<size_t>(constraint.minLength) > val.size()) return false;
                if (constraint.maxLength >= 0 && static_cast<size_t>(constraint.maxLength) < val.size()) return false;
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                auto val = std::get<std::vector<double>>(value);
                if (constraint.minLength >= 0 && static_cast<size_t>(constraint.minLength) > val.size()) return false;
                if (constraint.maxLength >= 0 && static_cast<size_t>(constraint.maxLength) < val.size()) return false;
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                auto val = std::get<std::vector<std::string>>(value);
                if (constraint.minLength >= 0 && static_cast<size_t>(constraint.minLength) > val.size()) return false;
                if (constraint.maxLength >= 0 && static_cast<size_t>(constraint.maxLength) < val.size()) return false;
            }
            break;
        }
        default:
            // Unknown constraint type
            break;
    }
    
    return true;
}

// Initialization and cleanup
Config& getDefaultConfig() {
    static Config defaultConfig;
    return defaultConfig;
}
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
