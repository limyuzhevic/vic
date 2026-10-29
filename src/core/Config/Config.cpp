#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

// Include nlohmann/json for configuration file parsing
#include <nlohmann/json.hpp>

using json = nlohmann::json;

using json = nlohmann::json;

namespace nlm {

// Configuration schema validation
struct ConfigurationSchema {
    std::unordered_map<std::string, std::string> requiredKeys;
    std::unordered_map<std::string, std::string> optionalKeys;
    std::string schemaVersion;
    
    bool validate(const std::vector<ConfigEntry>& entries) const {
        // Validate required keys
        for (const auto& pair : requiredKeys) {
            bool found = false;
            for (const auto& entry : entries) {
                if (entry.key == pair.first) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
};

// Global configuration schemas
namespace ConfigSchemas {
    const ConfigurationSchema defaultSchema {
        .requiredKeys = {
            {"random_seed", "Random seed for reproducible simulations"},
            {"simulation_timestep", "Simulation time step in seconds"},
            {"neuron_count", "Number of neurons in the brain"},
            {"region_count", "Number of brain regions"}
        },
        .optionalKeys = {
            {"connection_probability", "Synaptic connection probability (0.0-1.0)"},
            {"stdp_ltp_weight", "STDP long-term potentiation weight"},
            {"stdp_ltd_weight", "STDP long-term depression weight"},
            {"stdp_tau", "STDP time constant"},
            {"synaptogenesis_rate", "Rate of new synapse formation"},
            {"pruning_rate", "Rate of synapse pruning"},
            {"working_memory_capacity", "Working memory capacity in neurons"},
            {"episodic_capacity", "Episodic memory capacity in episodes"},
            {"environment_width", "Width of the simulation world"},
            {"environment_height", "Height of the simulation world"},
            {"vision_width", "Vision sensor width"},
            {"vision_height", "Vision sensor height"},
            {"enable_learning", "Enable neuromodulation and plasticity"},
            {"enable_curiosity", "Enable curiosity-driven exploration"},
            {"enable_development", "Enable developmental processes"},
            {"enable_checkpointing", "Enable checkpoint saving"},
            {"enable_replay", "Enable memory replay"},
            {"checkpoint_dir", "Directory for checkpoint files"},
            {"checkpoint_path", "Path for checkpoint files"},
            {"log_level", "Logging level (TRACE, DEBUG, INFO, WARN, ERROR, QUIET)"},
            {"threads", "Number of CPU threads for parallel processing"},
            {"memory_limit", "Memory limit in MB (0 = unlimited)"}
        },
        .schemaVersion = "1.0.0"
    };
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigurationSchema> schemas;
    
    Impl() {
        // Register default schema
        schemas["default"] = ConfigSchemas::defaultSchema;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.size() - 1) {
        extension = filepath.substr(dotPos + 1);
    }
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return loadFromJSON(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        return loadFromYAML(filepath);
    } else if (extension == "ini" || extension == "cfg") {
        return loadFromINI(filepath);
    } else {
        // Default to key=value format for backward compatibility
        NLM_LOG_WARN("Unknown file format '" + extension + "' for configuration file: " + filepath);
        NLM_LOG_WARN("Falling back to key=value format.");
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON configuration file: " + filepath);
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Validate schema if present
        if (j.contains("_schema")) {
            std::string schemaName = j["_schema"];
            auto schemaIt = pImpl->schemas.find(schemaName);
            if (schemaIt != pImpl->schemas.end()) {
                const ConfigurationSchema& schema = schemaIt->second;
                
                // Convert JSON object to ConfigEntry vector
                std::vector<ConfigEntry> entries;
                for (auto it = j.begin(); it != j.end(); ++it) {
                    if (it.key() == "_schema") continue; // Skip schema marker
                    
                    std::string key = it.key();
                    std::string valueStr;
                    ConfigSource source = ConfigSource::File;
                    
                    if (it->is_boolean()) {
                        valueStr = it->get<bool>() ? "true" : "false";
                    } else if (it->is_number_integer()) {
                        valueStr = std::to_string(it->get<int64_t>());
                    } else if (it->is_number_unsigned()) {
                        valueStr = std::to_string(it->get<uint64_t>());
                    } else if (it->is_number_float()) {
                        valueStr = std::to_string(it->get<double>());
                    } else {
                        valueStr = it->get<std::string>();
                    }
                    
                    // Find description from schema
                    std::string description;
                    auto descIt = schema.optionalKeys.find(key);
                    if (descIt != schema.optionalKeys.end()) {
                        description = descIt->second;
                    }
                    
                    entries.emplace_back(key, ConfigValue(valueStr), source, description);
                }
                
                // Validate against schema
                if (!schema.validate(entries)) {
                    NLM_LOG_ERROR("Configuration validation failed against schema: " + schemaName);
                    return false;
                }
                
                pImpl->entries = std::move(entries);
                NLM_LOG_INFO("Successfully loaded and validated JSON configuration from: " + filepath);
                return true;
            }
        }
        
        // No schema validation, just load everything (excluding metadata)
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (it.key().empty() || it.key()[0] == '_') {
                continue; // Skip empty keys and metadata keys
            }
            
            std::string key = it.key();
            std::string valueStr;
            
            if (it->is_boolean()) {
                valueStr = it->get<bool>() ? "true" : "false";
            } else if (it->is_number_integer()) {
                valueStr = std::to_string(it->get<int64_t>());
            } else if (it->is_number_unsigned()) {
                valueStr = std::to_string(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                valueStr = std::to_string(it->get<double>());
            } else {
                valueStr = it->get<std::string>();
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration file: " + filepath);
        NLM_LOG_ERROR("Parse error: " + std::string(e.what()));
        return false;
    } catch (const json::type_error& e) {
        NLM_LOG_ERROR("Type error in JSON configuration file: " + filepath);
        NLM_LOG_ERROR("Type error: " + std::string(e.what()));
        return false;
    } catch (const json::out_of_range& e) {
        NLM_LOG_ERROR("Out of range error in JSON configuration file: " + filepath);
        NLM_LOG_ERROR("Error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error loading JSON configuration file: " + filepath);
        NLM_LOG_ERROR("Error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, treat YAML the same as JSON
    // TODO: Add proper YAML support when libyaml or equivalent is available
    NLM_LOG_WARN("YAML configuration support requires additional dependencies.");
    NLM_LOG_WARN("Falling back to JSON parsing for: " + filepath);
    return loadFromJSON(filepath);
}

bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open INI configuration file: " + filepath);
        return false;
    }
    
    try {
        json j;
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }
            
            // Section header
            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Key = value pair
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Create INI key with section prefix
                std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
                
                if (!j.contains(fullKey)) {
                    j[fullKey] = value;
                }
            }
        }
        
        // Convert JSON to Config entries (simplified INI parsing)
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            std::string value = it->get<std::string>();
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Successfully loaded INI configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse INI configuration file: " + filepath);
        NLM_LOG_ERROR("Error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
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
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.size() - 1) {
        extension = filepath.substr(dotPos + 1);
    }
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    if (extension == "json") {
        return saveToJSON(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        return saveToYAML(filepath);
    } else if (extension == "ini" || extension == "cfg") {
        return saveToINI(filepath);
    } else {
        // Default to key=value format for backward compatibility
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    json j;
    
    // Add schema version
    j["_schema"] = "default";
    
    // Add all configuration entries
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Convert vector to JSON array
                json array = json::array();
                for (const auto& item : arg) {
                    array.push_back(item);
                }
                j[entry.key] = array;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json array = json::array();
                for (const auto& item : arg) {
                    array.push_back(item);
                }
                j[entry.key] = array;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json array = json::array();
                for (const auto& item : arg) {
                    array.push_back(item);
                }
                j[entry.key] = array;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to write JSON configuration file: " + filepath);
        return false;
    }
    
    file << std::setw(2) << j;
    
    NLM_LOG_INFO("Successfully saved JSON configuration to: " + filepath);
    return true;
}

bool Config::saveToYAML(const std::string& filepath) const {
    // For now, treat YAML the same as JSON
    // TODO: Add proper YAML support when libyaml or equivalent is available
    NLM_LOG_WARN("YAML configuration support requires additional dependencies.");
    NLM_LOG_WARN("Falling back to JSON format for: " + filepath);
    return saveToJSON(filepath);
}

bool Config::saveToINI(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to write INI configuration file: " + filepath);
        return false;
    }
    
    // Write sections
    std::string currentSection;
    for (const auto& entry : pImpl->entries) {
        // Determine section based on key (split by dot)
        size_t dotPos = entry.key.find('.');
        if (dotPos != std::string::npos) {
            std::string section = entry.key.substr(0, dotPos);
            std::string key = entry.key.substr(dotPos + 1);
            
            if (section != currentSection) {
                currentSection = section;
                file << "[" << currentSection << "]" << std::endl;
            }
            
            file << key << " = " << getValueString(entry.value) << std::endl;
        } else {
            // No section, write key directly
            if (!currentSection.empty()) {
                file << "[" << currentSection << "]" << std::endl;
                currentSection.clear();
            }
            file << entry.key << " = " << getValueString(entry.value) << std::endl;
        }
    }
    
    NLM_LOG_INFO("Successfully saved INI configuration to: " + filepath);
    return true;
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to write configuration file: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << std::endl;
        file << entry.key << " = " << getValueString(entry.value) << std::endl;
        file << std::endl;
    }
    
    NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
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

std::string Config::getValueString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, value);
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

