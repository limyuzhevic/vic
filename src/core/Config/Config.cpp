#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <iomanip>

// JSON and YAML support - include nlohmann/json and yaml-cpp if available
try_include nlohmann/json.hpp
ntry_include yaml-cpp/yaml.h

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// File format detection and loading
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Determine file type by extension
    std::string extension;
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }
    
    // Detect format and delegate to appropriate parser
    if (extension == "json" || extension == "json5" || extension == "jsonc") {
        // Read entire file for JSON parsing
        std::string content((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        return loadFromJSON(content);
    } else if (extension == "yaml" || extension == "yml") {
        std::string content((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        return loadFromYAML(content);
    } else {
        // Default to simple key=value format for backwards compatibility
        return loadFromSimpleFile(filepath);
    }
}

// JSON parsing
bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        // Check if nlohmann/json is available
#ifdef HAVE_NLOHMANN_JSON
        auto json = nlohmann::json::parse(jsonString);
        return parseJSON(json, pImpl->entries);
#else
        NLM_LOG_ERROR("JSON parsing requested but nlohmann/json library not available");
        return false;
#endif
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON: " + std::string(e.what()));
        return false;
    }
}

// YAML parsing
bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        // Check if yaml-cpp is available
#ifdef HAVE_YAML_CPP
        auto yaml = YAML::Load(yamlString);
        return parseYAML(yaml, pImpl->entries);
#else
        NLM_LOG_ERROR("YAML parsing requested but yaml-cpp library not available");
        return false;
#endif
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML: " + std::string(e.what()));
        return false;
    }
}

// Simple key=value file format (backwards compatibility)
bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert to appropriate type
            ConfigValue configValue = stringToValue(value);
            set(key, configValue, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    return true;
}

// Command line argument parsing
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

// Save to file (supports JSON, YAML, and simple formats)
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Determine file type by extension
    std::string extension;
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    }
    
    // Write appropriate format
    if (extension == "json" || extension == "json5" || extension == "jsonc") {
        std::string jsonContent = toJSON();
        file << jsonContent;
    } else if (extension == "yaml" || extension == "yml") {
        std::string yamlContent = toYAML();
        file << yamlContent;
    } else {
        // Default to simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << valueToString(entry.value) << "\n";
        }
    }
    
    return true;
}

// Get values with type safety
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

// Type-checked get method
template<typename T>
std::optional<T> Config::getChecked(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Check type
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(it->value)) {
            return std::get<int>(it->value);
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int64_t>(it->value)) {
            return std::get<int64_t>(it->value);
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<double>(it->value)) {
            return std::get<double>(it->value);
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(it->value)) {
            return std::get<bool>(it->value);
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(it->value)) {
            return std::get<std::string>(it->value);
        }
    }
    
    return std::nullopt;
}

// Fallback getOr method (maintains backward compatibility)
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Set value methods with source parameter
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Vector value setters
void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Existence check
bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

// Remove key
void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Clear all
void Config::clear() {
    pImpl->entries.clear();
}

// Configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Sort by key for consistent output
    std::vector<ConfigEntry> sortedEntries = pImpl->entries;
    std::sort(sortedEntries.begin(), sortedEntries.end(),
              [](const ConfigEntry& a, const ConfigEntry& b) { return a.key < b.key; });
    
    for (const auto& entry : sortedEntries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
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
                    oss << std::fixed << std::setprecision(6) << arg[i];
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
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    return oss.str();
}

// Configuration validation
bool Config::validate() const {
    return validateAgainstSchema(pImpl->entries);
}

// Convert string to ConfigValue
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

bool Config::isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

ConfigValue Config::stringToValue(const std::string& str) {
    // Try different types
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try integer
    try {
        if (str.find('.') == std::string::npos && str.find('e') == std::string::npos) {
            // Try int first
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.length()) {
                return intVal;
            }
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.length()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Default to string
    return str;
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// JSON parsing implementation
bool Config::parseJSON(const nlohmann::json& json, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    if (!json.is_object()) {
        NLM_LOG_ERROR("JSON root must be an object");
        return false;
    }
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        ConfigValue value;
        std::string description;
        
        // Convert JSON value to ConfigValue
        if (it->is_string()) {
            value = it->get<std::string>();
            description = "String parameter: " + key;
        } else if (it->is_number_integer()) {
            value = it->get<int64_t>();
            description = "Integer parameter: " + key;
        } else if (it->is_number_float()) {
            value = it->get<double>();
            description = "Float parameter: " + key;
        } else if (it->is_boolean()) {
            value = it->get<bool>();
            description = "Boolean parameter: " + key;
        } else if (it->is_array()) {
            // Handle arrays
            if (it->empty()) {
                description = "Empty array: " + key;
                value = std::vector<int>{};
            } else {
                // Try to infer array type
                bool allInts = std::all_of(it->begin(), it->end(), 
                    [](const auto& v) { return v.is_number_integer(); });
                bool allDoubles = std::all_of(it->begin(), it->end(),
                    [](const auto& v) { return v.is_number(); });
                bool allStrings = std::all_of(it->begin(), it->end(),
                    [](const auto& v) { return v.is_string(); });
                
                if (allInts) {
                    std::vector<int64_t> intArray;
                    for (const auto& v : *it) {
                        intArray.push_back(v.get<int64_t>());
                    }
                    value = intArray;
                    description = "Integer array: " + key;
                } else if (allDoubles) {
                    std::vector<double> doubleArray;
                    for (const auto& v : *it) {
                        doubleArray.push_back(v.get<double>());
                    }
                    value = doubleArray;
                    description = "Float array: " + key;
                } else if (allStrings) {
                    std::vector<std::string> stringArray;
                    for (const auto& v : *it) {
                        stringArray.push_back(v.get<std::string>());
                    }
                    value = stringArray;
                    description = "String array: " + key;
                } else {
                    description = "Mixed-type array: " + key;
                    value = std::vector<int>{};
                }
            }
        } else if (it->is_null()) {
            description = "Null value: " + key;
            value = std::string("")
        } else {
            description = "Unknown JSON type for " + key;
            value = std::string("")
        }
        
        entries.emplace_back(key, value, ConfigSource::JSON, description);
    }
    
    return true;
}

// YAML parsing implementation
bool Config::parseYAML(const YAML::Node& yaml, std::vector<ConfigEntry>& entries) {
    entries.clear();
    
    if (!yaml.IsMap()) {
        NLM_LOG_ERROR("YAML root must be a map");
        return false;
    }
    
    for (const auto& it : yaml) {
        std::string key = it.first.as<std::string>();
        ConfigValue value;
        std::string description;
        
        // Convert YAML node to ConfigValue
        if (it.second.IsScalar()) {
            std::string scalarStr = it.second.Scalar();
            value = stringToValue(scalarStr);
            if (std::holds_alternative<int>(value)) {
                description = "Integer parameter: " + key;
            } else if (std::holds_alternative<double>(value)) {
                description = "Float parameter: " + key;
            } else if (std::holds_alternative<bool>(value)) {
                description = "Boolean parameter: " + key;
            } else {
                description = "String parameter: " + key;
            }
        } else if (it.second.IsSequence()) {
            // Handle sequences (arrays)
            description = "Array parameter: " + key;
            std::vector<int> intArray;
            std::vector<double> doubleArray;
            std::vector<std::string> stringArray;
            
            bool allInts = true, allDoubles = true, allStrings = true;
            
            for (const auto& item : it.second) {
                if (item.IsScalar()) {
                    std::string scalarStr = item.Scalar();
                    try {
                        // Try to detect type
                        if (allInts) {
                            size_t pos;
                            int64_t intVal = std::stoll(scalarStr, &pos);
                            if (pos == scalarStr.length()) {
                                intArray.push_back(static_cast<int>(intVal));
                            } else {
                                allInts = false;
                            }
                        }
                        
                        if (allDoubles) {
                            size_t pos;
                            double doubleVal = std::stod(scalarStr, &pos);
                            if (pos == scalarStr.length()) {
                                doubleArray.push_back(doubleVal);
                            } else {
                                allDoubles = false;
                            }
                        }
                        
                        if (allStrings) {
                            stringArray.push_back(scalarStr);
                        }
                    } catch (...) {
                        allInts = false;
                        allDoubles = false;
                        allStrings = false;
                    }
                } else {
                    allInts = false;
                    allDoubles = false;
                    allStrings = false;
                }
            }
            
            if (allInts && !intArray.empty()) {
                value = intArray;
            } else if (allDoubles && !doubleArray.empty()) {
                value = doubleArray;
            } else if (!stringArray.empty()) {
                value = stringArray;
            } else {
                value = intArray; // Default
            }
        } else {
            description = "Unknown YAML type for " + key;
            value = std::string("")
        }
        
        entries.emplace_back(key, value, ConfigSource::YAML, description);
    }
    
    return true;
}

// Serialize to JSON
std::string Config::toJSON() const {
#ifdef HAVE_NLOHMANN_JSON
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&json, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return json.dump(2); // Pretty print with 2-space indentation
#else
    NLM_LOG_ERROR("JSON serialization requested but nlohmann/json library not available");
    return "{}";
#endif
}

// Serialize to YAML
std::string Config::toYAML() const {
#ifdef HAVE_YAML_CPP
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << static_cast<int>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    return emitter.str();
#else
    NLM_LOG_ERROR("YAML serialization requested but yaml-cpp library not available");
    return "{}";
#endif
}

// Validate configuration against schema
bool Config::validateAgainstSchema(const std::vector<ConfigEntry>& entries) {
    const auto& schema = getDefaultSchema();
    
    // Check for required fields
    for (const auto& field : schema) {
        if (field.required) {
            bool found = false;
            for (const auto& entry : entries) {
                if (entry.key == field.key) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                NLM_LOG_ERROR("Missing required configuration field: " + field.key);
                return false;
            }
        }
    }
    
    // Validate types
    for (const auto& entry : entries) {
        bool typeValid = false;
        
        for (const auto& field : schema) {
            if (field.key == entry.key) {
                typeValid = validateConfigValue(entry.value, field.type);
                break;
            }
        }
        
        if (!typeValid) {
            NLM_LOG_WARNING("Unknown or invalid type for configuration field: " + entry.key);
        }
    }
    
    return true;
}

// Validate a single ConfigValue against type string
bool Config::validateConfigValue(const ConfigValue& value, const std::string& type) {
    if (type == "int") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (type == "double") {
        return std::holds_alternative<double>(value);
    } else if (type == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (type == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (type == "vector") {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    return false;
}

// Default configuration descriptions
const std::unordered_map<std::string, std::string> Config::getDefaultDescriptions() {
    return {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Plasticity learning rate"},
        {"stdp_ltp_weight", "STDP Long-Term Potentiation weight"},
        {"stdp_ltd_weight", "STDP Long-Term Depression weight"},
        {"stdp_time_constant", "STDP time constant"},
        {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
        {"development_pruning_rate", "Development pruning rate"},
        {"dopamine_baseline", "Dopamine baseline level"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level"},
        {"log_to_file", "Log to file"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Simulation time limit"},
        {"visualization_enabled", "Enable visualization"},
        {"visualization_update_rate", "Visualization update rate"}
    };
}

// Default configuration schema
const std::vector<Config::ConfigSchema>& Config::getDefaultSchema() {
    static const std::vector<ConfigSchema> defaultSchema = {
        {"random_seed", "int", int64_t(42), "Random seed for reproducible experiments", true},
        {"simulation_timestep", "double", 0.001, "Simulation timestep in seconds", true},
        {"neuron_count", "int", static_cast<int64_t>(1000), "Number of neurons in the brain", true},
        {"region_count", "int", static_cast<int64_t>(2), "Number of brain regions", true},
        {"connection_probability", "double", 0.1, "Connection probability for random connectivity", true},
        {"plasticity_learning_rate", "double", 0.01, "Plasticity learning rate", false},
        {"stdp_ltp_weight", "double", 0.01, "STDP Long-Term Potentiation weight", false},
        {"stdp_ltd_weight", "double", 0.012, "STDP Long-Term Depression weight", false},
        {"stdp_time_constant", "double", 20.0, "STDP time constant", false},
        {"development_synaptogenesis_rate", "double", 0.001, "Development synaptogenesis rate", false},
        {"development_pruning_rate", "double", 0.0001, "Development pruning rate", false},
        {"dopamine_baseline", "double", 0.1, "Dopamine baseline level", false},
        {"reward_discount_factor", "double", 0.99, "Reward discount factor", false},
        {"environment_name", "string", std::string("GridWorld"), "Environment name", false},
        {"environment_width", "int", static_cast<int64_t>(10), "Environment width", false},
        {"environment_height", "int", static_cast<int64_t>(10), "Environment height", false},
        {"log_level", "string", std::string("INFO"), "Log level", false},
        {"log_to_file", "bool", false, "Log to file", false},
        {"log_filename", "string", std::string("nlm.log"), "Log filename", false},
        {"max_simulation_steps", "int", static_cast<int64_t>(10000), "Maximum simulation steps", false},
        {"simulation_time_limit", "double", 0.0, "Simulation time limit", false},
        {"visualization_enabled", "bool", false, "Enable visualization", false},
        {"visualization_update_rate", "double", 30.0, "Visualization update rate", false}
    };
    return defaultSchema;
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
