#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <optional>
#include <map>
#include <vector>
#include <cmath>

// Optional JSON library - if not available, we'll use simple parsing
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first (if available)
    #ifdef HAVE_NLOHMANN_JSON
    std::string jsonFile = filepath;
    if (jsonFile.find(".json") == std::string::npos && 
        jsonFile.find(".cfg") == std::string::npos) {
        jsonFile = jsonFile.substr(0, jsonFile.find_last_of('.')) + ".json";
    }
    
    std::ifstream jsonFileStream(jsonFile);
    if (jsonFileStream.good()) {
        jsonFileStream.close();
        // Attempt to parse JSON file
        std::ifstream jsonIn(jsonFile);
        std::stringstream buffer;
        buffer << jsonIn.rdbuf();
        try {
            loadFromJSON(buffer.str());
            NLM_LOG_INFO("Successfully loaded JSON configuration from: " + jsonFile);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to parse JSON config, falling back to text format: " + std::string(e.what()));
        }
    }
    #endif
    
    // Fallback to text format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
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
            
            if (key.empty()) {
                NLM_LOG_WARNING("Configuration file " + filepath + ", line " + 
                               std::to_string(lineNumber) + ": Empty key, skipping");
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
                NLM_LOG_ERROR("Error parsing configuration at " + filepath + ", line " + 
                             std::to_string(lineNumber) + ": " + std::string(e.what()));
                continue;
            }
        } else {
            NLM_LOG_WARNING("Configuration file " + filepath + ", line " + 
                           std::to_string(lineNumber) + ": Invalid format (expected key=value), line: " + line);
        }
    }
    
    // Validate configuration after loading
    std::vector<std::string> validationErrors;
    validate(*this, validationErrors);
    
    if (!validationErrors.empty()) {
        NLM_LOG_WARNING("Configuration validation completed with warnings:");
        for (const auto& error : validationErrors) {
            NLM_LOG_WARNING("  - " + error);
        }
    }
    
    NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
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

#ifdef HAVE_NLOHMANN_JSON

// JSON serialization/deserialization support
void Config::loadFromJSON(const std::string& jsonString) {
    try {
        #ifndef HAVE_NLOHMANN_JSON
        throw std::runtime_error("nlohmann/json not available");
        #endif
        
        auto json = json::parse(jsonString);
        
        // Convert JSON to key-value pairs
        clear();
        
        for (const auto& [key, value] : json.items()) {
            ConfigValue configValue;
            
            if (value.is_number_integer()) {
                if (value.get<int64_t>() <= std::numeric_limits<int>::max()) {
                    configValue = value.get<int>();
                } else {
                    configValue = value.get<int64_t>();
                }
            } else if (value.is_number_unsigned()) {
                configValue = static_cast<int64_t>(value.get<uint64_t>());
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else {
                configValue = value.get<std::string>();
            }
            
            set(key, configValue, ConfigSource::File);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to parse JSON config: ") + e.what());
    }
}

std::string Config::saveToJSON() const {
    try {
        #ifndef HAVE_NLOHMANN_JSON
        throw std::runtime_error("nlohmann/json not available");
        #endif
        
        json jsonObj;
        
        for (const auto& entry : pImpl->entries) {
            if (std::holds_alternative<int>(entry.value)) {
                jsonObj[entry.key] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                jsonObj[entry.key] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                jsonObj[entry.key] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                jsonObj[entry.key] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                jsonObj[entry.key] = std::get<std::string>(entry.value);
            }
        }
        
        return jsonObj.dump(2);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to serialize to JSON: ") + e.what());
    }
}

// Configuration validation
class ConfigValidator {
public:
    static bool validate(const Config& config, std::vector<std::string>& errors) {
        const auto& schema = ConfigSchema::getDefaultSchema();
        errors.clear();
        
        // Check required fields
        for (const auto& field : schema) {
            if (field.required && !config.has(field.name)) {
                errors.push_back("Missing required field: " + field.name);
            }
        }
        
        if (!errors.empty()) {
            return false;
        }
        
        // Validate field types
        for (const auto& field : schema) {
            if (config.has(field.name)) {
                auto value = config.get(field.name);
                if (value.has_value() && !field.isValidType(value.value())) {
                    errors.push_back("Invalid type for field " + field.name + ": expected " + field.type);
                }
            }
        }
        
        return errors.empty();
    }
    
    static std::string getValidationErrors(const Config& config) {
        std::vector<std::string> errors;
        validate(config, errors);
        
        if (errors.empty()) {
            return "Configuration is valid";
        }
        
        std::ostringstream oss;
        oss << "Configuration validation errors:";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
        return oss.str();
    }
};

#endif
