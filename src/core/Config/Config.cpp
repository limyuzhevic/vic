#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration schema for validation
    std::unordered_map<std::string, std::string> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default schema for validation
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Define known configuration keys with their expected types
    // This helps with validation and documentation
    pImpl->schema = {
        // Simulation parameters
        {"random_seed", "int64"},
        {"simulation_timestep", "double"},
        {"neuron_count", "size_t"},
        {"region_count", "size_t"},
        {"connection_probability", "float"},
        {"max_simulation_steps", "size_t"},
        {"simulation_time_limit", "double"},
        
        // Plasticity settings
        {"plasticity_learning_rate", "float"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_time_constant", "float"},
        
        // Development settings
        {"development_synaptogenesis_rate", "float"},
        {"development_pruning_rate", "float"},
        
        // Neuromodulation settings
        {"dopamine_baseline", "float"},
        {"reward_discount_factor", "float"},
        
        // Environment settings
        {"environment_name", "string"},
        {"environment_width", "size_t"},
        {"environment_height", "size_t"},
        
        // Logging settings
        {"log_level", "string"},
        {"log_to_file", "bool"},
        {"log_filename", "string"},
        
        // Visualization settings
        {"visualization_enabled", "bool"},
        {"visualization_update_rate", "float"}
    };
}

bool Config::loadFromFile(const std::string& filepath) {
    // First try to parse as JSON (new format)
    if (tryLoadFromJSON(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARN("Configuration file not found: " + filepath);
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
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate configuration before setting
            if (validateKeyValue(key, value)) {
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARN("Invalid configuration in file " + filepath + 
                           " line " + std::to_string(lineNum) + ": " + key + " = " + value);
            }
        } else {
            NLM_LOG_WARN("Malformed configuration line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from file: " + filepath);
    return true;
}

bool Config::tryLoadFromJSON(const std::string& filepath) {
    // Simple JSON parser implementation
    // In a production environment, this would use nlohmann/json or similar library
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Basic JSON parsing for object format
    if (content.find('{') == std::string::npos) {
        return false; // Not a JSON object
    }
    
    // Simple JSON key-value extraction
    // This is a simplified JSON parser for demonstration
    size_t pos = 0;
    while ((pos = content.find('"', pos)) != std::string::npos) {
        size_t keyStart = pos + 1;
        pos = content.find('"', keyStart);
        if (pos == std::string::npos) break;
        
        std::string key = content.substr(keyStart, pos - keyStart);
        
        // Find colon after key
        pos = content.find(':', pos + 1);
        if (pos == std::string::npos) break;
        
        // Find value (could be string, number, or boolean)
        size_t valueStart = content.find_first_not_of(' ', pos + 1);
        if (valueStart == std::string::npos) break;
        
        // Handle different value types
        if (content[valueStart] == '"') {
            // String value
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            if (validateKeyValue(key, value)) {
                set(key, value, ConfigSource::File);
            }
            pos = valueEnd + 1;
        } else if (std::isdigit(content[valueStart]) || content[valueStart] == '-' || content[valueStart] == '+') {
            // Number value
            size_t valueEnd = content.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) break;
            std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
            if (validateKeyValue(key, valueStr)) {
                set(key, valueStr, ConfigSource::File);
            }
            pos = valueEnd;
        } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
            // Boolean value
            if (content.substr(valueStart, 4) == "true" || content.substr(valueStart, 5) == "false") {
                size_t valueEnd = content.find_first_of(",}", valueStart);
                if (valueEnd == std::string::npos) break;
                std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
                if (validateKeyValue(key, valueStr)) {
                    set(key, valueStr, ConfigSource::File);
                }
                pos = valueEnd;
            }
        }
    }
    
    return true;
}

bool Config::validateKeyValue(const std::string& key, const std::string& value) {
    // Check if key is in schema
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt == pImpl->schema.end()) {
        // Unknown key - warn but allow it (for extensibility)
        NLM_LOG_DEBUG("Unknown configuration key: " + key);
        return true;
    }
    
    // Validate based on expected type
    std::string expectedType = schemaIt->second;
    
    if (expectedType == "int64") {
        try {
            std::stoll(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (expectedType == "double") {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (expectedType == "float") {
        try {
            std::stof(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (expectedType == "size_t") {
        try {
            std::stoull(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (expectedType == "bool") {
        return value == "true" || value == "false";
    } else if (expectedType == "string") {
        return !value.empty();
    }
    
    return true; // Unknown type, assume valid
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
                if (validateKeyValue(key, value)) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (validateKeyValue(key, value)) {
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated from runtime configuration\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << entry.value << "\n\n";
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
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.source = source;
        entry.description = getDescriptionForKey(key);
        pImpl->entries.push_back(entry);
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

std::string Config::getDescriptionForKey(const std::string& key) const {
    // Provide descriptions for known configuration keys
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Plasticity learning rate"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_time_constant", "STDP time constant"},
        {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
        {"development_pruning_rate", "Development pruning rate"},
        {"dopamine_baseline", "Dopamine baseline level"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level (DEBUG, INFO, WARN, ERROR)"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Log filename"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate"}
    };
    
    auto it = descriptions.find(key);
    return (it != descriptions.end()) ? it->second : "Unknown configuration parameter";
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
