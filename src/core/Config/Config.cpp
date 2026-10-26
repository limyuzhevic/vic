#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <stdexcept>

// JSON/YAML parsing support
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
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
    // Determine file format based on extension
    std::string ext = getFileExtension(filepath);
    
#ifdef HAVE_NLOHMANN_JSON
    if (ext == ".json") {
        return parseJsonFile(filepath);
    }
#endif
    
#ifdef HAVE_YAML_CPP
    if (ext == ".yaml" || ext == ".yml") {
        return parseYamlFile(filepath);
    }
#endif
    
    // Default to simple key=value format for backward compatibility
    return parseSimpleKeyValueFile(filepath);
}

// Helper to get file extension
std::string Config::getFileExtension(const std::string& filepath) {
    size_t pos = filepath.rfind('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filepath.substr(pos);
}

// Backward compatibility: parse simple key=value format
bool Config::parseSimpleKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    // Add JSON/YAML parsing implementations
#ifdef HAVE_NLOHMANN_JSON

// Convert ConfigValue to JSON value
nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else {
            jsonValue = "UNKNOWN_TYPE";
        }
    }, value);
    return jsonValue;
}

// Parse JSON file
bool Config::parseJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Recursively parse JSON structure
        parseJsonObject(jsonData, ConfigSource::File, "");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Parse JSON file
bool Config::parseJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Recursively parse JSON structure
        parseJsonObject(jsonData, ConfigSource::File, "");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Helper to parse JSON object recursively
void Config::parseJsonObject(const nlohmann::json& jsonObj, ConfigSource source, const std::string& prefix) {
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        std::string key = prefix + it.key();
        
        if (it->is_object()) {
            // Nested object - append "/" to key for nested structure
            parseJsonObject(*it, source, key + "/");
        } else if (it->is_array()) {
            // Array - convert to vector<string> for flexibility
            std::vector<std::string> strArray;
            for (const auto& item : *it) {
                strArray.push_back(item.dump());
            }
            set(key, strArray, source);
        } else {
            // Simple value - convert based on JSON type
            ConfigValue value;
            
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else {
                value = it->dump();
            }
            
            set(key, value, source);
        }
    }
}

#endif

#ifdef HAVE_YAML_CPP

// Parse YAML file  
bool Config::parseYamlFile(const std::string& filepath) {
    try {
        YAML::Loader loader;
        YAML::Node doc = loader.LoadFile(filepath);
        if (!doc) {
            return false;
        }
        
        // Clear existing entries
        clear();
        
        // Recursively parse YAML structure
        parseYamlNode(doc, ConfigSource::File, "");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Helper to parse YAML node recursively
void Config::parseYamlNode(const YAML::Node& node, ConfigSource source, const std::string& prefix) {
    if (node.IsScalar()) {
        // Simple scalar value - convert to ConfigValue based on content
        std::string key = prefix;
        ConfigValue value;
        
        if (node.IsSequence()) {
            // YAML sequence (array) - convert to vector<string>
            std::vector<std::string> strArray;
            for (const auto& item : node) {
                strArray.push_back(item.Scalar());
            }
            value = strArray;
        } else {
            // YAML scalar - store as string
            value = node.Scalar();
        }
        
        set(key, value, source);
    } else if (node.IsMap()) {
        // YAML map (dictionary) - process recursively with key separation
        for (const auto& it : node) {
            std::string key = prefix + "/" + it.first.as<std::string>();
            parseYamlNode(it.second, source, key);
        }
    } else if (node.IsSequence()) {
        // YAML sequence as root element - treat as empty array
        set(prefix, std::vector<std::string>(), source);
    }
}

#endif

// Add saveToJson and saveToYaml functions for future implementation
void Config::saveToJson(const std::string& filepath) const {
#ifdef HAVE_NLOHMANN_JSON
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for JSON output: " + filepath);
            return;
        }
        
        nlohmann::json jsonData;
        
        // Add all entries to JSON using a recursive helper
        for (const auto& entry : pImpl->entries) {
            // Convert nested key (e.g., "brain/neuron_count") to JSON structure
            std::string key = entry.key;
            nlohmann::json* current = &jsonData;
            
            size_t pos = 0;
            while (true) {
                size_t slashPos = key.find('/', pos);
                if (slashPos == std::string::npos) {
                    // Last part - set value
                    (*current)[key.substr(pos)] = configValueToJson(entry.value);
                    break;
                }
                
                std::string part = key.substr(pos, slashPos - pos);
                if (!current->contains(part)) {
                    (*current)[part] = nlohmann::json::object();
                }
                
                current = &((*current)[part]);
                pos = slashPos + 1;
            }
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indentation
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save JSON file " + filepath + ": " + std::string(e.what()));
    }
#endif
}

void Config::saveToYaml(const std::string& filepath) const {
#ifdef HAVE_YAML_CPP
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for YAML output: " + filepath);
            return;
        }
        
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        // Add all entries to YAML using recursive helper
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            
            size_t pos = 0;
            while (true) {
                size_t slashPos = key.find('/', pos);
                if (slashPos == std::string::npos) {
                    // Last part - set value with type conversion
                    ConfigValue value = entry.value;
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            emitter << YAML::Key << key.substr(pos) << YAML::Value << arg;
                        } else if constexpr (std::is_same_v<T, int64_t> || 
                                            std::is_same_v<T, uint64_t>) {
                            emitter << YAML::Key << key.substr(pos) << YAML::Value << static_cast<int64_t>(arg);
                        } else if constexpr (std::is_same_v<T, double>) {
                            emitter << YAML::Key << key.substr(pos) << YAML::Value << arg;
                        } else if constexpr (std::is_same_v<T, bool>) {
                            emitter << YAML::Key << key.substr(pos) << YAML::Value << arg;
                        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            emitter << YAML::Key << key.substr(pos);
                            emitter << YAML::BeginSeq;
                            for (const auto& item : arg) {
                                emitter << item;
                            }
                            emitter << YAML::EndSeq;
                        } else {
                            emitter << YAML::Key << key.substr(pos) << YAML::Value << "UNKNOWN";
                        }
                    }, value);
                    break;
                }
                
                std::string part = key.substr(pos, slashPos - pos);
                emitter << YAML::Key << part << YAML::BeginMap;
                pos = slashPos + 1;
            }
        }
        
        emitter << YAML::EndMap;
        file << emitter.c_str();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save YAML file " + filepath + ": " + std::string(e.what()));
    }
#endif
}
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

// Add validation functions for configuration values
void Config::validate(const std::string& key, const ConfigValue& value) const {
    // Validate based on key prefix and value type
    if (key.find("neuron_count") != std::string::npos) {
        // Validate neuron count
        if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0) {
                throw std::invalid_argument("neuron_count must be positive");
            }
            if (count > 10000000) { // 10 million
                NLM_LOG_WARNING("Large neuron_count may impact performance");
            }
        }
    } else if (key.find("simulation_timestep") != std::string::npos) {
        // Validate timestep
        if (std::holds_alternative<double>(value)) {
            double timestep = std::get<double>(value);
            if (timestep <= 0.0) {
                throw std::invalid_argument("simulation_timestep must be positive");
            }
            if (timestep > 1.0) {
                NLM_LOG_WARNING("Large timestep may affect simulation accuracy");
            }
        }
    } else if (key.find("plasticity") != std::string::npos && 
               key.find("learning_rate") != std::string::npos) {
        // Validate learning rates
        if (std::holds_alternative<double>(value)) {
            double rate = std::get<double>(value);
            if (rate < 0.0 || rate > 1.0) {
                throw std::invalid_argument("plasticity learning_rate must be between 0 and 1");
            }
        }
    } else if (key.find("dopamine_baseline") != std::string::npos) {
        // Validate dopamine baseline
        if (std::holds_alternative<double>(value)) {
            double baseline = std::get<double>(value);
            if (baseline < 0.0 || baseline > 10.0) {
                throw std::invalid_argument("dopamine_baseline must be between 0 and 10");
            }
        }
    }
    
    // Generic validation for other keys
    // This can be extended with more specific validation rules
    
    // Log validation warnings for potential issues
    if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) > 1000000) {
        NLM_LOG_WARNING("Very large integer value in configuration key: " + key);
    } else if (std::holds_alternative<double>(value) && std::get<double>(value) > 1000.0) {
        NLM_LOG_WARNING("Very large double value in configuration key: " + key);
    }
}

// Add validation to set methods
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value before setting
    validate(key, value);
    
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
    validate(key, ConfigValue(value));
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    validate(key, ConfigValue(value));
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    validate(key, ConfigValue(value));
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    validate(key, ConfigValue(value));
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
