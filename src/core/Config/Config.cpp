#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <chrono>
#include <regex>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    uint64_t creationTimestamp;
    bool strictValidation;
    std::vector<std::string> validationErrors;
    std::vector<ValidationRule> validationRules;
    
    Impl() : creationTimestamp(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()),
             strictValidation(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json jsonData;
        file >> jsonData;
        
        // Recursively load configuration from JSON
        loadFromJSON(jsonData, ConfigSource::File);
        
        return !pImpl->entries.empty();
    } catch (const json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    #ifdef YAML_SUPPORT
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Try to use YAML parser if available
        // This is a placeholder - implement proper YAML parsing
        return loadFromJSONFile(filepath); // Fallback to JSON
    } catch (const std::exception& e) {
        return false;
    }
    #else
    return false;
    #endif
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        json jsonData = json::parse(jsonString);
        loadFromJSON(jsonData, ConfigSource::Runtime);
        return !pImpl->entries.empty();
    } catch (const json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    // Placeholder for YAML parsing
    // Implement proper YAML parsing if available
    return loadFromJSON(yamlString); // Fallback to JSON
}

void Config::loadFromJSON(const json& j, ConfigSource source) {
    if (j.is_object()) {
        for (const auto& [key, value] : j.items()) {
            ConfigValue configValue;
            
            if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Convert array to appropriate vector type
                if (!value.empty()) {
                    // Check element type
                    if (value[0].is_boolean()) {
                        std::vector<bool> vec;
                        vec.reserve(value.size());
                        for (const auto& item : value) {
                            vec.push_back(item.get<bool>());
                        }
                        configValue = vec;
                    } else if (value[0].is_number_integer()) {
                        std::vector<int64_t> vec;
                        vec.reserve(value.size());
                        for (const auto& item : value) {
                            vec.push_back(item.get<int64_t>());
                        }
                        configValue = vec;
                    } else if (value[0].is_number_float()) {
                        std::vector<double> vec;
                        vec.reserve(value.size());
                        for (const auto& item : value) {
                            vec.push_back(item.get<double>());
                        }
                        configValue = vec;
                    } else if (value[0].is_string()) {
                        std::vector<std::string> vec;
                        vec.reserve(value.size());
                        for (const auto& item : value) {
                            vec.push_back(item.get<std::string>());
                        }
                        configValue = vec;
                    }
                }
            }
            
            // Set the configuration value if we have a valid one
            if (configValue.index() != 0) {
                set(key, configValue, source);
            }
        }
    }
}

std::string Config::exportToJSON() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = arg;
            } else {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return j.dump(4); // Pretty print with 4 spaces indentation
}

std::string Config::exportToYAML() const {
    // Placeholder for YAML export
    // Implement proper YAML export if available
    return exportToJSON(); // Fallback to JSON
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // If key doesn't exist in this config, add it
        if (!has(entry.key)) {
            pImpl->entries.push_back(entry);
            pImpl->entries.back().source = source;
        } else {
            // If key exists, update value if source is higher priority
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&entry](const ConfigEntry& e) { return e.key == entry.key; });n            
            if (it != pImpl->entries.end() && static_cast<int>(entry.source) > static_cast<int>(it->source)) {
                it->value = entry.value;
                it->source = source;
            }
        }
    }
}

std::vector<std::string> Config::validate() const {
    pImpl->validationErrors.clear();
    
    // Validate against validation rules
    for (const auto& rule : pImpl->validationRules) {
        if (has(rule.key)) {
            auto valueOpt = getVariant(rule.key);
            if (valueOpt.has_value()) {
                if (!std::visit([rule](auto&& arg) -> bool { return rule.validator(arg); }, valueOpt.value())) {
                    pImpl->validationErrors.push_back("Key \"" + rule.key + "\": " + rule.errorMessage);
                }
            } else {
                pImpl->validationErrors.push_back("Key \"" + rule.key + "\": Key not found");
            }
        }
    }
    
    // Check for duplicate keys (though this shouldn't happen)
    // and other validation issues
    
    return pImpl->validationErrors;
}

std::vector<std::string> Config::getKeysBySection(const std::string& section) const {
    std::vector<std::string> keys;
    
    // Extract section from key (e.g., "brain.neuron_count" -> section "brain")
    std::string sectionWithDot = section + ".";
    size_t dotPos = section.find('.');
    while (dotPos != std::string::npos) {
        std::string sectionPart = section.substr(0, dotPos + 1);
        
        for (const auto& entry : pImpl->entries) {
            if (entry.key.find(sectionPart) == 0 && (entry.key.size() == sectionPart.size() || entry.key[sectionPart.size()] == '.')) {
                keys.push_back(entry.key);
            }
        }
        
        section = section.substr(0, dotPos);
        dotPos = section.find('.');
    }
    
    return keys;
}

std::vector<std::string> Config::getSections() const {
    std::vector<std::string> sections;
    
    for (const auto& entry : pImpl->entries) {
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            std::string section = entry.key.substr(0, dotPos);
            if (std::find(sections.begin(), sections.end(), section) == sections.end()) {
                sections.push_back(section);
            }
        }
    }
    
    return sections;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    return "";
}

Config Config::copy() const {
    Config copyConfig;
    copyConfig.pImpl->entries = pImpl->entries;
    return copyConfig;
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::clearValidationRules() {
    pImpl->validationRules.clear();
}

void Config::setStrictValidation(bool enabled) {
    pImpl->strictValidation = enabled;
}

bool Config::isStrictValidation() const {
    return pImpl->strictValidation;
}

std::vector<std::string> Config::getLastValidationErrors() const {
    return pImpl->validationErrors;
}

size_t Config::size() const {
    return pImpl->entries.size();
}

bool Config::empty() const {
    return pImpl->entries.empty();
}

uint64_t Config::getAge() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() - pImpl->creationTimestamp;
}

void Config::refresh() {
    pImpl->creationTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalKeys = pImpl->entries.size();
    stats.ageMs = getAge();
    stats.isValid = pImpl->validationErrors.empty();
    stats.validationErrors = pImpl->validationErrors;
    stats.lastModified = "unknown"; // In a real implementation, track last modification time
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::Default) stats.keysBySource[0]++;
        else if (entry.source == ConfigSource::File) stats.keysBySource[1]++;
        else if (entry.source == ConfigSource::CommandLine) stats.keysBySource[2]++;
        else if (entry.source == ConfigSource::Runtime) stats.keysBySource[3]++;
    }
    
    return stats;
}

bool Config::loadFromAdvancedArgs(int argc, char** argv) {
    bool hasValidArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --config=file.json format
        if (arg.substr(0, 8) == "--config") {
            std::string configFile = "config.json";
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            
            if (loadFromJSONFile(configFile)) {
                hasValidArgs = true;
            }
        }
        // Handle --section.key=value format for advanced config
        else if (arg.find('=') != std::string::npos && arg.find('.') != std::string::npos) {
            size_t eqPos = arg.find('=');
            size_t dotPos = arg.find('.');
            std::string key = arg.substr(dotPos + 1);
            std::string value = arg.substr(eqPos + 1);
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::CommandLine);
            hasValidArgs = true;
        }
        // Handle simple --key=value format
        else if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            size_t eqPos = arg.find('=');
            std::string key = arg.substr(2, eqPos - 2);
            std::string value = arg.substr(eqPos + 1);
            
            set(key, value, ConfigSource::CommandLine);
            hasValidArgs = true;
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc && arg.find('.') == std::string::npos) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            set(key, value, ConfigSource::CommandLine);
            hasValidArgs = true;
        }
    }
    
    return hasValidArgs;
}

void Config::resetWithSource(ConfigSource source) {
    // Remove all entries with the specified source
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&source](const ConfigEntry& e) { return e.source == source; }),
        pImpl->entries.end()
    );
}

std::vector<std::pair<std::string, ConfigSource>> Config::getKeysWithSource() const {
    std::vector<std::pair<std::string, ConfigSource>> result;
    
    for (const auto& entry : pImpl->entries) {
        result.push_back(std::make_pair(entry.key, entry.source));
    }
    
    return result;
}

std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> Config::diff(const Config& other) const {
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> result;
    
    // Check keys in this config
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end()) {
            result.push_back(std::make_pair(entry.key, std::make_pair(entry.value, ConfigValue())));
        } else if (it->value != entry.value) {
            result.push_back(std::make_pair(entry.key, std::make_pair(entry.value, it->value)));
        }
    }
    
    // Check keys only in other config
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            result.push_back(std::make_pair(entry.key, std::make_pair(ConfigValue(), entry.value)));
        }
    }
    
    return result;
}

void Config::applyDelta(const std::vector<std::pair<std::string, ConfigValue>>& delta) {
    for (const auto& item : delta) {
        set(item.first, item.second, ConfigSource::Runtime);
    }
}

std::string Config::getDefaultJSONSchema() {
    // Return a basic JSON schema for NLM configuration
    return R"({
        "type": "object",
        "properties": {
            "brain": {
                "type": "object",
                "properties": {
                    "neuron_count": {"type": "integer", "minimum": 1},
                    "synapse_density": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                    "connection_probability": {"type": "number", "minimum": 0.0, "maximum": 1.0},
                    "v_thresh": {"type": "number"},
                    "v_rest": {"type": "number"},
                    "v_reset": {"type": "number"},
                    "tau_mem": {"type": "number", "minimum": 0.0},
                    "tau_ref": {"type": "number", "minimum": 0.0}
                }
            },
            "plasticity": {
                "type": "object",
                "properties": {
                    "stdp": {
                        "type": "object",
                        "properties": {
                            "enable": {"type": "boolean"},
                            "learning_rate": {"type": "number", "minimum": 0.0},
                            "tau_plus": {"type": "number", "minimum": 0.0},
                            "tau_minus": {"type": "number", "minimum": 0.0}
                        }
                    },
                    "hebbian": {
                        "type": "object",
                        "properties": {
                            "enable": {"type": "boolean"}
                        }
                    },
                    "structural": {
                        "type": "object",
                        "properties": {
                            "enable": {"type": "boolean"}
                        }
                    }
                }
            }
        }
    })";
}

bool Config::validateAgainstSchema(const std::string& schema) const {
    // Placeholder for JSON schema validation
    // In a real implementation, use a JSON schema validation library
    return true; // For now, always return true
}

// Helper function to get variant value
template<typename T>
std::optional<ConfigValue> Config::getVariant(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
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
