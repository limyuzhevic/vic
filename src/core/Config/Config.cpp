// Enhanced JSON/YAML parser for Config.cpp
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

// JSON helper functions
std::string Config::parseJSONFile(const std::string& filepath) {
    // Try to parse as JSON first (preferred)
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "";
        }
        
        nlohmann::json json_data;
        file >> json_data;
        
        // Convert JSON to key=value format
        std::ostringstream oss;
        for (auto it = json_data.begin(); it != json_data.end(); ++it) {
            std::string key = it.key();
            std::string value;
            
            // Convert JSON value to string representation
            if (it->is_string()) {
                value = "\"" + it->get<std::string>() + "\"";
            } else if (it->is_number_integer()) {
                value = std::to_string(it->get<int64_t>());
            } else if (it->is_number_float()) {
                value = std::to_string(it->get<double>());
            } else if (it->is_boolean()) {
                value = it->get<bool>() ? "true" : "false";
            } else if (it->is_null()) {
                value = "null";
            } else if (it->is_array()) {
                // Handle arrays (convert to JSON string)
                value = it->dump();
            } else if (it->is_object()) {
                // Handle nested objects (convert to JSON string)
                value = it->dump();
            }
            
            oss << key << " = " << value << "\n";
        }
        
        return oss.str();
    } catch (const std::exception&) {
        // Not valid JSON, try YAML
    }
    
    return "";
}

std::string Config::parseYAMLFile(const std::string& filepath) {
    try {
        YAML::Node yaml_data = YAML::LoadFile(filepath);
        
        // Convert YAML to key=value format
        std::ostringstream oss;
        for (auto it = yaml_data.begin(); it != yaml_data.end(); ++it) {
            std::string key = it->first.as<std::string>();
            std::string value;
            
            // Convert YAML value to string representation
            if (it->second.IsScalar()) {
                value = it->second.as<std::string>();
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
            } else if (it->second.IsSequence()) {
                value = "[";
                for (size_t i = 0; i < it->second.size(); ++i) {
                    if (i > 0) value += ", ";
                    if (it->second[i].IsScalar()) {
                        value += it->second[i].as<std::string>();
                    } else {
                        value += it->second[i].as<std::string>();
                    }
                }
                value += "]";
            } else if (it->second.IsMap()) {
                value = it->second.as<std::string>();
            }
            
            oss << key << " = " << value << "\n";
        }
        
        return oss.str();
    } catch (const std::exception&) {
        // Not valid YAML either
    }
    
    return "";
}

bool Config::loadFromFile(const std::string& filepath) {
    std::string content;
    
    // Try JSON first
    content = parseJSONFile(filepath);
    if (!content.empty()) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return parseFromContent(content, ConfigSource::File);
    }
    
    // Try YAML second
    content = parseYAMLFile(filepath);
    if (!content.empty()) {
        NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath);
        return parseFromContent(content, ConfigSource::File);
    }
    
    // Fall back to simple key=value format for backward compatibility
    return loadFromSimpleFormat(filepath);
}

bool Config::parseFromContent(const std::string& content, ConfigSource source) {
    clear();
    
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
            continue;
        }
        
        // Parse key=value pairs with enhanced support
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Handle quoted strings
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
                // Unescape quotes
                size_t escapePos = value.find('\\"');
                while (escapePos != std::string::npos) {
                    value.replace(escapePos, 2, "\"");
                    escapePos = value.find('\\"', escapePos + 1);
                }
            }
            
            // Parse boolean values
            if (value == "true" || value == "false") {
                set(key, value == "true", source);
            }
            // Parse numeric values
            else if (std::all_of(value.begin(), value.end(), ::isdigit) ||
                     (value.front() == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                try {
                    int64_t intVal = std::stoll(value);
                    set(key, intVal, source);
                } catch (...) {
                    set(key, value, source);
                }
            }
            // Parse floating point values
            else {
                bool isFloat = false;
                size_t dotPos = value.find('.');
                if (dotPos != std::string::npos && dotPos < value.size() - 1) {
                    std::string beforeDot = value.substr(0, dotPos);
                    std::string afterDot = value.substr(dotPos + 1);
                    isFloat = std::all_of(beforeDot.begin(), beforeDot.end(), ::isdigit) &&
                              std::all_of(afterDot.begin(), afterDot.end(), ::isdigit);
                }
                
                if (isFloat) {
                    try {
                        double floatVal = std::stod(value);
                        set(key, floatVal, source);
                    } catch (...) {
                        set(key, value, source);
                    }
                } else {
                    set(key, value, source);
                }
            }
        } else {
            // Log parsing error
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return !pImpl->entries.empty();
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    // Original simple key=value parser (backward compatibility)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    return parseFromContent(std::string(std::istreambuf_iterator<char>(file), 
                                       std::istreambuf_iterator<char>()), ConfigSource::File);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write as JSON format (modern, readable)
    nlohmann::json json_data;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                json_data[entry.key] = arg;
            } else {
                // For other types, convert to string
                json_data[entry.key] = "(" + std::to_string(static_cast<int>(entry.source)) + ") " + std::to_string(arg);
            }
        }, entry.value);
    }
    
    // Write JSON with pretty formatting
    file << json_data.dump(2);
    return true;
}

void Config::setAdvancedOptions(const std::string& options) {
    // Parse advanced options string
    std::istringstream stream(options);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse option=value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            set(key, value, ConfigSource::Advanced);
        }
    }
}

std::string Config::exportToJSON() const {
    nlohmann::json json_data;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                json_data[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return json_data.dump(2);
}

void Config::importFromJSON(const std::string& jsonStr) {
    try {
        auto json_data = nlohmann::json::parse(jsonStr);
        clear();
        
        for (auto it = json_data.begin(); it != json_data.end(); ++it) {
            set(it.key(), it.value(), ConfigSource::File);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import from JSON: " + std::string(e.what()));
        throw;
    }
}

std::string Config::exportToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                emitter << static_cast<int>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << arg;
            }
        }, entry.value);
    }
    
    emitter << YAML::EndMap;
    
    std::stringstream ss;
    ss << emitter.c_str();
    return ss.str();
}

void Config::importFromYAML(const std::string& yamlStr) {
    try {
        auto yaml_data = YAML::Load(yamlStr);
        clear();
        
        for (auto it = yaml_data.begin(); it != yaml_data.end(); ++it) {
            set(it->first.as<std::string>(), it->second, ConfigSource::File);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to import from YAML: " + std::string(e.what()));
        throw;
    }
}

// Advanced validation and error checking
bool Config::validate() const {
    // Define required configuration keys
    std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.region_count",
        "simulation_timestep"
    };
    
    std::vector<std::string> missingKeys;
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            missingKeys.push_back(key);
        }
    }
    
    if (!missingKeys.empty()) {
        NLM_LOG_ERROR("Configuration validation failed - missing keys:");
        for (const auto& key : missingKeys) {
            NLM_LOG_ERROR("  " + key);
        }
        return false;
    }
    
    // Validate numeric ranges
    if (has("brain.neuron_count")) {
        auto neuronCount = get<int64_t>("brain.neuron_count");
        if (neuronCount && (*neuronCount < 10 || *neuronCount > 1000000)) {
            NLM_LOG_WARNING("Unusual neuron count: " + std::to_string(*neuronCount));
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && (*timestep <= 0.0 || *timestep > 1.0)) {
            NLM_LOG_ERROR("Invalid simulation timestep: " + std::to_string(*timestep));
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.region_count",
        "simulation_timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Check value ranges
    if (has("brain.neuron_count")) {
        auto neuronCount = get<int64_t>("brain.neuron_count");
        if (neuronCount) {
            if (*neuronCount < 10) {
                errors.push_back("Neuron count too small: " + std::to_string(*neuronCount) +
                               ". Minimum is 10.");
            } else if (*neuronCount > 1000000) {
                errors.push_back("Neuron count too large: " + std::to_string(*neuronCount) +
                               ". Maximum is 1000000.");
            }
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep) {
            if (*timestep <= 0.0) {
                errors.push_back("Simulation timestep must be positive, got: " + std::to_string(*timestep));
            } else if (*timestep > 1.0) {
                errors.push_back("Simulation timestep too large: " + std::to_string(*timestep) +
                               ". Maximum is 1.0.");
            }
        }
    }
    
    return errors;
}

// Performance optimizations
void Config::optimizeForSimulation() {
    // Set sensible defaults for performance
    if (!has("brain.neuron_count")) {
        set("brain.neuron_count", 1000, ConfigSource::Optimization);
    }
    
    if (!has("brain.synapse_density")) {
        set("brain.synapse_density", 0.05, ConfigSource::Optimization);
    }
    
    if (!has("simulation_timestep")) {
        set("simulation_timestep", 0.001, ConfigSource::Optimization);
    }
    
    // Enable performance-related optimizations
    set("performance.use_sparse_connectivity", true, ConfigSource::Optimization);
    set("performance.use_simd", true, ConfigSource::Optimization);
    set("performance.enable_checkpointing", true, ConfigSource::Optimization);
}

void Config::enableDebugMode() {
    set("debug.enable_debug_output", true, ConfigSource::Advanced);
    set("debug.log_level", "DEBUG", ConfigSource::Advanced);
    set("debug.validate_every_step", true, ConfigSource::Advanced);
    set("debug.enable_profiling", true, ConfigSource::Advanced);
}

void Config::disableDebugMode() {
    set("debug.enable_debug_output", false, ConfigSource::Advanced);
    set("debug.log_level", "INFO", ConfigSource::Advanced);
    set("debug.validate_every_step", false, ConfigSource::Advanced);
    set("debug.enable_profiling", false, ConfigSource::Advanced);
}

// Configuration comparison and merging
bool Config::merge(const Config& other, bool overwriteExisting) {
    bool changesMade = false;
    
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key) || overwriteExisting) {
            set(entry.key, entry.value, entry.source);
            changesMade = true;
        }
    }
    
    return changesMade;
}

Config Config::getSubset(const std::vector<std::string>& keys) const {
    Config subset;
    
    for (const auto& key : keys) {
        auto val = get(key);
        if (val.has_value()) {
            set(key, val.value(), ConfigSource::Subset);
        }
    }
    
    return subset;
}

std::string Config::generateConfigReport() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Report ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "\n";
    
    // Group entries by source
    std::map<ConfigSource, std::vector<ConfigEntry>> entriesBySource;
    for (const auto& entry : pImpl->entries) {
        entriesBySource[entry.source].push_back(entry);
    }
    
    oss << "Entries by source:\n";
    for (const auto& [source, entries] : entriesBySource) {
        oss << "  " << static_cast<int>(source) << ": " << entries.size() << "\n";
    }
    
    oss << "\n";
    oss << "Validation status: ";
    if (validate()) {
        oss << "PASSED\n";
    } else {
        oss << "FAILED\n";
        auto errors = getValidationErrors();
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
    }
    
    return oss.str();
}

// Enhanced error handling and recovery
void Config::recoveryFromCorrupted() {
    // Attempt to recover from corrupted configuration
    NLM_LOG_WARNING("Attempting to recover from corrupted configuration");
    
    // Restore essential defaults
    clear();
    set("brain.neuron_count", 100, ConfigSource::Recovery);
    set("brain.region_count", 1, ConfigSource::Recovery);
    set("simulation_timestep", 0.001, ConfigSource::Recovery);
    
    NLM_LOG_INFO("Configuration recovered with essential defaults");
}

// Configuration history and versioning
void Config::addComment(const std::string& comment) {
    // Store comments with entries (simplified)
    NLM_LOG_INFO("Config comment: " + comment);
}

void Config::exportToFileWithMetadata(const std::string& filepath, const std::string& metadata) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    // Write metadata header
    file << "# " << metadata << "\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# Timestamp: " << std::chrono::system_clock::now() << "\n";
    file << "\n";
    
    // Write configuration entries
    saveToFile(filepath);
}

// Serialization/deserialization utilities
std::vector<uint8_t> Config::serialize() const {
    std::vector<uint8_t> buffer;
    
    // Simple binary serialization
    for (const auto& entry : pImpl->entries) {
        // Write key length
        uint32_t keyLen = static_cast<uint32_t>(entry.key.length());
        buffer.insert(buffer.end(), 
                     reinterpret_cast<const uint8_t*>(&keyLen),
                     reinterpret_cast<const uint8_t*>(&keyLen) + sizeof(keyLen));
        
        // Write key
        buffer.insert(buffer.end(), entry.key.begin(), entry.key.end());
        
        // Write value (simplified - would need more sophisticated serialization for variant types)
        // For now, just write as string
        std::string valueStr;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else {
                valueStr = "unknown_type";
            }
        }, entry.value);
        
        uint32_t valueLen = static_cast<uint32_t>(valueStr.length());
        buffer.insert(buffer.end(),
                     reinterpret_cast<const uint8_t*>(&valueLen),
                     reinterpret_cast<const uint8_t*>(&valueLen) + sizeof(valueLen));
        
        buffer.insert(buffer.end(), valueStr.begin(), valueStr.end());
    }
    
    return buffer;
}

bool Config::deserialize(const std::vector<uint8_t>& data) {
    clear();
    
    size_t offset = 0;
    
    while (offset < data.size()) {
        if (offset + sizeof(uint32_t) > data.size()) {
            break; // Invalid data
        }
        
        // Read key length
        uint32_t keyLen;
        std::memcpy(&keyLen, &data[offset], sizeof(keyLen));
        offset += sizeof(keyLen);
        
        if (offset + keyLen > data.size()) {
            break; // Invalid data
        }
        
        // Read key
        std::string key(&data[offset], keyLen);
        offset += keyLen;
        
        if (offset + sizeof(uint32_t) > data.size()) {
            break; // Invalid data
        }
        
        // Read value length
        uint32_t valueLen;
        std::memcpy(&valueLen, &data[offset], sizeof(valueLen));
        offset += sizeof(valueLen);
        
        if (offset + valueLen > data.size()) {
            break; // Invalid data
        }
        
        // Read value
        std::string valueStr(&data[offset], valueLen);
        offset += valueLen;
        
        // Parse value and set
        set(key, valueStr, ConfigSource::File);
    }
    
    return !pImpl->entries.empty();
}

// Configuration comparison
bool Config::operator==(const Config& other) const {
    return pImpl->entries.size() == other.pImpl->entries.size() &&
           std::is_permutation(pImpl->entries.begin(), pImpl->entries.end(),
                              other.pImpl->entries.begin(),
                              [](const ConfigEntry& a, const ConfigEntry& b) {
                                  return a.key == b.key && a.value == b.value && a.source == b.source;
                              });
}

bool Config::operator!=(const Config& other) const {
    return !(*this == other);
}

} // namespace nlm
