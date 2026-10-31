#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <regex>
#include <cctype>

// Include JSON library if available
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

// Include YAML library if available
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<ValidationRule>> schemas;
    std::map<std::string, ConfigValue> defaults;
    std::vector<ValidationRule> globalValidationRules;
    bool validationEnabled;
    
    Impl() : validationEnabled(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (isJSON(content)) {
        return loadFromJSON(content);
    } else if (isYAML(content)) {
        return loadFromYAML(content);
    } else {
        return loadFromProperties(content);
    }
}

bool Config::loadFromJSON(const std::string& jsonContent) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        auto json = nlohmann::json::parse(jsonContent);
        jsonToConfig(json, *this);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON parsing not available - nlohmann/json library not found");
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& yamlContent) {
#ifdef HAVE_YAML_CPP
    try {
        auto yaml = YAML::Load(yamlContent);
        // Convert YAML to config (simplified implementation)
        for (const auto& pair : yaml) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            if (pair.second.IsScalar()) {
                std::string scalarValue = pair.second.as<std::string>();
                // Try to infer type
                if (scalarValue == "true" || scalarValue == "false") {
                    value = scalarValue == "true";
                } else {
                    try {
                        size_t pos;
                        int intVal = std::stoi(scalarValue, &pos);
                        if (pos == scalarValue.size()) {
                            value = intVal;
                        } else {
                            float floatVal = std::stof(scalarValue);
                            if (std::abs(floatVal - std::stoi(floatVal)) < 0.001f) {
                                value = static_cast<int>(floatVal);
                            } else {
                                value = floatVal;
                            }
                        }
                    } catch (...) {
                        value = scalarValue;
                    }
                }
            } else if (pair.second.IsSequence()) {
                std::vector<std::any> arrayValues;
                for (const auto& item : pair.second) {
                    if (item.IsScalar()) {
                        arrayValues.push_back(item.as<std::string>());
                    }
                }
                value = arrayValues;
            } else {
                NLM_LOG_WARNING("Unsupported YAML structure for key: " + key);
                continue;
            }
            
            set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML: " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML parsing not available - yaml-cpp library not found");
    return false;
#endif
}

bool Config::loadFromProperties(const std::string& propertiesContent) {
    std::istringstream stream(propertiesContent);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '!') {
            continue;
        }
        
        // Handle Java properties format: key=value or key value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Handle escaped characters in values
            value = replaceAll(value, "\\n", "\n");
            value = replaceAll(value, "\\t", "\t");
            value = replaceAll(value, "\\\\", "\\");
            
            set(key, value, ConfigSource::File);
        } else if (line.find(' ') != std::string::npos) {
            // Handle space-separated format
            size_t spacePos = line.find(' ');
            std::string key = trim(line.substr(0, spacePos));
            std::string value = trim(line.substr(spacePos + 1));
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid line in properties file: " + std::to_string(lineNumber) + ": " + line);
        }
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

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to determine format from file extension
    std::string extension = "";
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos && dotPos < filepath.size() - 1) {
        extension = filepath.substr(dotPos + 1);
    }
    
    if (extension == "json" || extension.empty()) {
        std::string jsonContent = toJSON(prettyPrint);
        file << jsonContent;
    } else if (extension == "yaml" || extension == "yml") {
        std::string yamlContent = toYAML();
        file << yamlContent;
    } else {
        // Default to properties format
        std::string propertiesContent = toProperties();
        file << propertiesContent;
    }
    
    return true;
}

std::string Config::toJSON(bool prettyPrint) const {
#ifdef HAVE_NLOHMANN_JSON
    auto json = configToJson(*this);
    if (prettyPrint) {
        return json.dump(2);
    } else {
        return json.dump();
    }
#else
    NLM_LOG_ERROR("JSON serialization not available - nlohmann/json library not found");
    return "{\"error\": \"JSON library not available\"}";
#endif
}

std::string Config::toYAML() const {
#ifdef HAVE_YAML_CPP
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    emitter.SetSeqIndent('-');
    
    emitter << YAML::BeginMap;
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        if (std::holds_alternative<int>(entry.value)) {
            emitter << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            emitter << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            emitter << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            emitter << std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            emitter << std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            emitter << YAML::BeginSeq;
            for (const auto& item : std::get<std::vector<int>>(entry.value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            emitter << YAML::BeginSeq;
            for (const auto& item : std::get<std::vector<double>>(entry.value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            emitter << YAML::BeginSeq;
            for (const auto& item : std::get<std::vector<std::string>>(entry.value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else {
            emitter << "null";
        }
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
#else
    NLM_LOG_ERROR("YAML serialization not available - yaml-cpp library not found");
    return "# YAML generation not available - yaml-cpp library not found\n";
#endif
}

std::string Config::toProperties() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << " = ";
        
        if (std::holds_alternative<int>(entry.value)) {
            oss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            oss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            oss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            oss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            std::string str = std::get<std::string>(entry.value);
            // Escape special characters
            str = replaceAll(str, "\n", "\\n");
            str = replaceAll(str, "\t", "\\t");
            str = replaceAll(str, "\\", "\\\\");
            oss << "\"" << str << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<int>>(entry.value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<double>>(entry.value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                std::string str = vec[i];
                str = replaceAll(str, "\n", "\\n");
                str = replaceAll(str, "\t", "\\t");
                oss << "\"" << str << "\"";
            }
            oss << "]";
        } else {
            oss << "";
        }
        
        oss << "\n\n";
    }
    
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
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(it->value)) {
                return std::get<int>(it->value);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(it->value)) {
                return std::get<int64_t>(it->value);
            } else if (std::holds_alternative<int>(it->value)) {
                return static_cast<int64_t>(std::get<int>(it->value));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->value)) {
                return std::get<double>(it->value);
            } else if (std::holds_alternative<int>(it->value)) {
                return static_cast<double>(std::get<int>(it->value));
            } else if (std::holds_alternative<int64_t>(it->value)) {
                return static_cast<double>(std::get<int64_t>(it->value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->value)) {
                return std::get<bool>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->value)) {
                return std::get<std::string>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(it->value)) {
                return std::get<std::vector<int>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(it->value)) {
                return std::get<std::vector<double>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(it->value)) {
                return std::get<std::vector<std::string>>(it->value);
            }
        }
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::optional<ConfigValue> Config::getRaw(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate type consistency
    bool isValid = false;
    std::string type = "unknown";
    
    if (std::holds_alternative<int>(value)) {
        isValid = true;
        type = "int";
    } else if (std::holds_alternative<int64_t>(value)) {
        isValid = true;
        type = "int64";
    } else if (std::holds_alternative<double>(value)) {
        isValid = true;
        type = "double";
    } else if (std::holds_alternative<bool>(value)) {
        isValid = true;
        type = "bool";
    } else if (std::holds_alternative<std::string>(value)) {
        isValid = true;
        type = "string";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        isValid = true;
        type = "int_array";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        isValid = true;
        type = "double_array";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        isValid = true;
        type = "string_array";
    }
    
    if (!isValid) {
        NLM_LOG_WARNING("Unsupported configuration value type for key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry entry(key, value, source);
        entry.type = type;
        entry.description = "Configuration entry for " + key;
        pImpl->entries.push_back(std::move(entry));
    }
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key exists in schema
    bool keyFound = false;
    for (const auto& rule : pImpl->globalValidationRules) {
        if (rule.key == key) {
            keyFound = true;
            if (!rule.validator(value)) {
                NLM_LOG_ERROR("Validation failed for key " + key + ": " + rule.errorMessage);
                return false;
            }
            break;
        }
    }
    
    // Check if key is in defaults
    if (!keyFound && pImpl->defaults.find(key) != pImpl->defaults.end()) {
        keyFound = true;
    }
    
    // If not in schema, log warning but still set
    if (!keyFound) {
        NLM_LOG_WARNING("Setting configuration key outside defined schema: " + key);
    }
    
    set(key, value, source);
    return true;
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->globalValidationRules.push_back(rule);
}

void Config::addDefault(const std::string& key, const ConfigValue& defaultValue, 
                        const std::string& type, const std::string& description) {
    pImpl->defaults[key] = defaultValue;
    
    // Add a validation rule for required fields
    addValidationRule(ValidationRule(key, 
        [defaultValue](const std::any&) { return true; },  // Always valid if present
        "Missing required default value: " + key,
        false, type));
}

void Config::addSchema(const std::string& name, const std::vector<ValidationRule>& rules) {
    pImpl->schemas[name] = rules;
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return it != pImpl->entries.end();
}

bool Config::hasType(const std::string& key, const std::string& type) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->type == type;
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    pImpl->entries.erase(it, pImpl->entries.end());
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByPrefix(const std::string& prefix) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->schemas.clear();
    pImpl->defaults.clear();
    pImpl->globalValidationRules.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << "null";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

ValidationResult Config::validate() const {
    return validateAgainstRules(pImpl->globalValidationRules);
}

ValidationResult Config::validate(const std::string& schemaName) const {
    auto it = pImpl->schemas.find(schemaName);
    if (it == pImpl->schemas.end()) {
        return ValidationResult(false, {"Schema not found: " + schemaName});
    }
    
    return validateAgainstRules(it->second);
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Skip keys that already exist with higher priority sources
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            ConfigEntry newEntry = entry;
            newEntry.source = source;
            pImpl->entries.push_back(std::move(newEntry));
        } else if (static_cast<int>(source) > static_cast<int>(it->source)) {
            // Replace with higher priority source
            it->value = entry.value;
            it->source = source;
        }
    }
}

void Config::mergeFromFile(const std::string& filepath, ConfigSource source) {
    Config other;
    if (other.loadFromFile(filepath)) {
        merge(other, source);
    }
}

size_t Config::size() const {
    return pImpl->entries.size();
}

std::map<ConfigSource, size_t> Config::getSourceCounts() const {
    std::map<ConfigSource, size_t> counts;
    for (const auto& entry : pImpl->entries) {
        counts[entry.source]++;
    }
    return counts;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool success = false;
    
    // Simple environment variable parsing
    // In a real implementation, you'd use platform-specific functions
    // This is a simplified version
    
    std::string prefixUpper = prefix;
    std::transform(prefixUpper.begin(), prefixUpper.end(), prefixUpper.begin(), ::toupper);
    
    // For demonstration, we'll just set a sample environment variable
    // In practice, you'd use std::getenv() or similar
    const char* envVar = std::getenv((prefix + "TEST_VAR").c_str());
    if (envVar != nullptr) {
        set("test.environment_var", std::string(envVar), ConfigSource::Environment);
        success = true;
    }
    
    return success;
}

bool Config::isJSON(const std::string& content) {
    // Simple JSON detection
    if (content.empty()) return false;
    
    // Remove whitespace for checking
    std::string trimmed = content;
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
    
    if (trimmed.empty()) return false;
    
    // Check if it starts with { or [
    return (trimmed[0] == '{' || trimmed[0] == '[');
}

bool Config::isYAML(const std::string& content) {
    // Simple YAML detection
    if (content.empty()) return false;
    
    // Check for common YAML patterns
    size_t colonPos = content.find(':');
    if (colonPos != std::string::npos) {
        // Check if there's indentation (YAML uses spaces for indentation)
        size_t lineEnd = content.find('\n', colonPos);
        if (lineEnd != std::string::npos) {
            std::string line = content.substr(colonPos + 1, lineEnd - colonPos - 1);
            line = trim(line);
            return !line.empty() && (line[0] == ' ' || line[0] == '\t');
        }
    }
    
    return false;
}

bool Config::isProperties(const std::string& content) {
    // Simple properties file detection
    if (content.empty()) return false;
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '!') {
            continue;
        }
        
        // Check if line contains '=' or is a simple key-value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

// Internal helper implementations
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Config::replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

#ifdef HAVE_NLOHMANN_JSON
void Config::jsonToConfig(const nlohmann::json& j, Config& config) {
    if (j.is_object()) {
        for (const auto& item : j.items()) {
            std::string key = item.key();
            
            if (item.value().is_number_integer()) {
                config.set(key, item.value().get<int>(), ConfigSource::File);
            } else if (item.value().is_number_unsigned()) {
                config.set(key, item.value().get<uint64_t>(), ConfigSource::File);
            } else if (item.value().is_number_float()) {
                config.set(key, item.value().get<double>(), ConfigSource::File);
            } else if (item.value().is_boolean()) {
                config.set(key, item.value().get<bool>(), ConfigSource::File);
            } else if (item.value().is_string()) {
                config.set(key, item.value().get<std::string>(), ConfigSource::File);
            } else if (item.value().is_array()) {
                std::vector<int> intArray;
                bool isIntArray = true;
                
                for (const auto& element : item.value()) {
                    if (element.is_number_integer()) {
                        intArray.push_back(element.get<int>());
                    } else if (element.is_number_float()) {
                        intArray.push_back(static_cast<int>(element.get<double>()));
                        isIntArray = false;
                    } else {
                        isIntArray = false;
                        break;
                    }
                }
                
                if (isIntArray && !intArray.empty()) {
                    config.set(key, intArray, ConfigSource::File);
                } else {
                    std::vector<double> doubleArray;
                    for (const auto& element : item.value()) {
                        if (element.is_number()) {
                            doubleArray.push_back(element.get<double>());
                        } else {
                            doubleArray.push_back(0.0);
                        }
                    }
                    config.set(key, doubleArray, ConfigSource::File);
                }
            } else if (item.value().is_object()) {
                // For nested objects, create a string representation
                config.set(key, item.value().dump(), ConfigSource::File);
            }
        }
    }
}

nlohmann::json Config::configToJson(const Config& config) {
    nlohmann::json j;
    
    for (const auto& entry : config.pImpl->entries) {
        if (std::holds_alternative<int>(entry.value)) {
            j[entry.key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            j[entry.key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            j[entry.key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            j[entry.key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            j[entry.key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            j[entry.key] = std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            j[entry.key] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            j[entry.key] = std::get<std::vector<std::string>>(entry.value);
        }
    }
    
    return j;
}
#endif

ValidationResult Config::validateAgainstRules(const std::vector<ValidationRule>& rules) const {
    ValidationResult result;
    
    if (!pImpl->validationEnabled) {
        return result;
    }
    
    for (const auto& rule : rules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        
        if (!it) {
            if (rule.required) {
                result.addError("Required configuration key missing: " + rule.key);
            }
            continue;
        }
        
        // Type validation
        bool typeValid = false;
        if (rule.typeName == "int" && std::holds_alternative<int>(it->value)) {
            typeValid = true;
        } else if (rule.typeName == "int64" && (std::holds_alternative<int>(it->value) || std::holds_alternative<int64_t>(it->value))) {
            typeValid = true;
        } else if (rule.typeName == "double" && (std::holds_alternative<int>(it->value) || std::holds_alternative<int64_t>(it->value) || std::holds_alternative<double>(it->value))) {
            typeValid = true;
        } else if (rule.typeName == "bool" && std::holds_alternative<bool>(it->value)) {
            typeValid = true;
        } else if (rule.typeName == "string" && std::holds_alternative<std::string>(it->value)) {
            typeValid = true;
        } else if (rule.typeName == "int_array" && std::holds_alternative<std::vector<int>>(it->value)) {
            typeValid = true;
        } else if (rule.typeName == "double_array" && (std::holds_alternative<std::vector<int>>(it->value) || std::holds_alternative<std::vector<double>>(it->value))) {
            typeValid = true;
        } else if (rule.typeName == "string_array" && std::holds_alternative<std::vector<std::string>>(it->value)) {
            typeValid = true;
        }
        
        if (!typeValid) {
            result.addError("Type mismatch for key " + rule.key + ": expected " + rule.typeName);
        }
        
        // Value validation
        if (typeValid && rule.validator) {
            try {
                // Convert to any for validation
                std::any value;
                if (std::holds_alternative<int>(it->value)) {
                    value = std::get<int>(it->value);
                } else if (std::holds_alternative<int64_t>(it->value)) {
                    value = std::get<int64_t>(it->value);
                } else if (std::holds_alternative<double>(it->value)) {
                    value = std::get<double>(it->value);
                } else if (std::holds_alternative<bool>(it->value)) {
                    value = std::get<bool>(it->value);
                } else if (std::holds_alternative<std::string>(it->value)) {
                    value = std::get<std::string>(it->value);
                }
                
                if (!rule.validator(value)) {
                    result.addError(rule.errorMessage + " (key: " + rule.key + ")");
                }
            } catch (...) {
                result.addError("Error validating key " + rule.key);
            }
        }
    }
    
    return result;
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
} // namespace nlm

