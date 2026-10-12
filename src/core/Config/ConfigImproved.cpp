#include "ConfigImproved.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>

// Include JSON library if available
try {
    #include <nlohmann/json.hpp>
} catch (...) {
    // JSON library not available, will use basic implementation
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::vector<std::pair<std::string, ConfigValue>> defaultValues;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set up default validators
    pImpl->validators["random_seed"] = [](const ConfigValue& v) {
        return std::holds_alternative<int64_t>(v);
    };
    
    pImpl->validators["simulation_timestep"] = [](const ConfigValue& v) {
        return std::holds_alternative<double>(v);
    };
    
    pImpl->validators["neuron_count"] = [](const ConfigValue& v) {
        return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
    };
    
    pImpl->validators["region_count"] = [](const ConfigValue& v) {
        return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
    };
    
    pImpl->validators["connection_probability"] = [](const ConfigValue& v) {
        if (!std::holds_alternative<double>(v)) return false;
        double val = std::get<double>(v);
        return val >= 0.0 && val <= 1.0;
    };
    
    pImpl->validators["plasticity_learning_rate"] = [](const ConfigValue& v) {
        return std::holds_alternative<double>(v);
    };
    
    pImpl->validators["log_level"] = [](const ConfigValue& v) {
        return std::holds_alternative<std::string>(v);
    };
    
    pImpl->validators["log_to_file"] = [](const ConfigValue& v) {
        return std::holds_alternative<bool>(v);
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

std::string Config::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool Config::parseLine(const std::string& line, std::string& key, std::string& value) const {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return true;
}

bool Config::loadFromStream(std::istream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        std::string key, value;
        if (parseLine(line, key, value)) {
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try YAML first, then fall back to simple format
    if (loadFromYamlFile(filepath)) {
        return true;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    return loadFromStream(file);
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    // Check if file exists and has YAML extension or content
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Simple YAML detection - check for key: value format
    std::string line;
    bool hasYamlFormat = false;
    while (std::getline(file, line) && !hasYamlFormat) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        // Check for YAML format (key: value)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > 0) {
            hasYamlFormat = true;
        }
    }
    
    if (hasYamlFormat) {
        // For now, fall back to simple parser for both YAML and JSON
        // In production, you'd integrate a proper YAML parser
        return loadFromStream(file);
    }
    
    return false;
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json;
        file >> json;
        return loadFromJson(json);
    } catch (const std::exception& e) {
        // JSON parsing failed, fall back to simple format
        return false;
    }
}

bool Config::loadFromJson(const nlohmann::json& json) {
    clear();
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        const nlohmann::json& value = it.value();
        
        ConfigSource source = ConfigSource::File;
        
        if (value.is_string()) {
            set(key, value.get<std::string>(), source);
        } else if (value.is_number_integer()) {
            if (value.get<int64_t>() > std::numeric_limits<int>::max()) {
                set(key, value.get<int64_t>(), source);
            } else {
                set(key, value.get<int>(), source);
            }
        } else if (value.is_number_unsigned()) {
            set(key, value.get<uint64_t>(), source);
        } else if (value.is_number_float()) {
            set(key, value.get<double>(), source);
        } else if (value.is_boolean()) {
            set(key, value.get<bool>(), source);
        } else if (value.is_array()) {
            // Convert array to vector of appropriate type
            if (!value.empty()) {
                if (value[0].is_number_integer()) {
                    std::vector<int64_t> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.get<int64_t>());
                    }
                    set(key, vec, source);
                } else if (value[0].is_number_float()) {
                    std::vector<double> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.get<double>());
                    }
                    set(key, vec, source);
                } else if (value[0].is_string()) {
                    std::vector<std::string> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.get<std::string>());
                    }
                    set(key, vec, source);
                }
            }
        }
        // Skip other JSON types (objects, null)
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
            } else {
                // Handle --key value format
                if (i + 1 < argc) {
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format (short format)
        else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    // This would read from environment variables with NLM_ prefix
    // For now, just return false to indicate not implemented
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Try JSON format first
    bool success = saveToJsonFile(filepath);
    if (!success) {
        // Fall back to simple format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << entry.toString() << "\n\n";
        }
    }
    
    return true;
}

bool Config::saveToYamlFile(const std::string& filepath) const {
    try {
        nlohmann::json json;
        
        for (const auto& entry : pImpl->entries) {
            json[entry.key] = entry.toString();
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << json.dump(2); // Pretty print with 2-space indentation
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    try {
        nlohmann::json json;
        
        for (const auto& entry : pImpl->entries) {
            if (std::holds_alternative<int>(entry.value)) {
                json[entry.key] = entry.as<int>();
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                json[entry.key] = entry.as<int64_t>();
            } else if (std::holds_alternative<double>(entry.value)) {
                json[entry.key] = entry.as<double>();
            } else if (std::holds_alternative<bool>(entry.value)) {
                json[entry.key] = entry.as<bool>();
            } else if (std::holds_alternative<std::string>(entry.value)) {
                json[entry.key] = entry.as<std::string>();
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                // For arrays, use the first type present
                json[entry.key] = std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                json[entry.key] = std::get<std::vector<double>>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                json[entry.key] = std::get<std::vector<std::string>>(entry.value);
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << json.dump(2);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, bool required) {
    set(key, value, source, description, required, nullptr);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, bool required) {
    set(key, ConfigValue(value), source, description, required, nullptr);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, bool required) {
    set(key, ConfigValue(value), source, description, required, nullptr);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, bool required) {
    set(key, ConfigValue(value), source, description, required, nullptr);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, bool required) {
    set(key, ConfigValue(value), source, description, required, nullptr);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, bool required,
                 std::function<bool(const ConfigValue&)> validator) {
    ConfigEntry entry(key, value, source, description, required);
    
    // Set custom validator if provided
    if (validator) {
        entry.validator = validator;
    } else {
        // Use default validator from map if available
        auto it = pImpl->validators.find(key);
        if (it != pImpl->validators.end()) {
            entry.validator = it->second;
        }
    }
    
    // Check if key exists
    auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                   [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (existingIt != pImpl->entries.end()) {
        *existingIt = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key] = validator;
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
    pImpl->validators.erase(key);
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
    pImpl->validators.clear();
    pImpl->defaultValues.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Sort by source priority: CommandLine > File > Default > Runtime > Environment
    std::map<ConfigSource, std::vector<const ConfigEntry*>> grouped;
    for (const auto& entry : pImpl->entries) {
        grouped[entry.source].push_back(&entry);
    }
    
    for (const auto& sourceGroup : grouped) {
        oss << "  [" << static_cast<int>(sourceGroup.first) << "]\n";
        for (const auto* entry : sourceGroup.second) {
            oss << "    " << entry->key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry->value);
            oss << "]" << (entry->required ? " *" : "") << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            errors.push_back(getValidationError(entry));
        }
    }
    
    return errors;
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Check required fields
    if (entry.required && entry.value.index() == std::variant_npos) {
        return false;
    }
    
    // Use custom validator if present
    if (entry.validator) {
        return entry.validator(entry.value);
    }
    
    // Use default validator if available
    auto it = pImpl->validators.find(entry.key);
    if (it != pImpl->validators.end()) {
        return it->second(entry.value);
    }
    
    return true; // No validation rules defined, assume valid
}

std::string Config::getValidationError(const ConfigEntry& entry) const {
    if (entry.validationErrorMessage.empty()) {
        return "Validation failed for key: " + entry.key;
    }
    return entry.validationErrorMessage;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                           [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

bool Config::isOverridden(const std::string& key) const {
    ConfigSource source = getSource(key);
    return source == ConfigSource::CommandLine || source == ConfigSource::File || 
           source == ConfigSource::Environment;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.getEntries()) {
        // Only merge if not already set with higher priority
        if (!has(entry.key) || 
            (getSource(entry.key) != ConfigSource::CommandLine && 
             entry.source == ConfigSource::CommandLine)) {
            // Create a copy with the entry's source
            ConfigEntry newEntry(entry);
            auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                           [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (existingIt != pImpl->entries.end()) {
                *existingIt = newEntry;
            } else {
                pImpl->entries.push_back(newEntry);
            }
        }
    }
}

void Config::applyDefaults(const std::vector<std::pair<std::string, ConfigValue>>& defaults) {
    pImpl->defaultValues = defaults;
    
    for (const auto& defaultPair : defaults) {
        if (!has(defaultPair.first)) {
            set(defaultPair.first, defaultPair.second, ConfigSource::Default,
                "Default value", false);
        }
    }
}

} // namespace nlm