#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <limits>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> paramDescriptions;
    std::unordered_map<std::string, ConfigValue> defaultValues;
};

// Configuration parameter documentation and defaults
static void initializeConfigSchema(Config::Impl* impl) {
    // Define all available configuration parameters with descriptions and defaults
    const std::vector<std::tuple<std::string, std::string, ConfigValue>> configParams = {
        // Simulation parameters
        {"random_seed", "Random seed for reproducible results", 42},
        {"simulation_timestep", "Time step for simulation in seconds", 0.001},
        {"neuron_count", "Total number of neurons to create", 1000},
        {"region_count", "Number of neural regions to create", 1},
        {"connection_probability", "Probability of synaptic connection between neurons", 0.1f},
        
        // Plasticity parameters
        {"stdp_ltp_weight", "STDP Long-Term Potentiation weight", 0.01f},
        {"stdp_ltd_weight", "STDP Long-Term Depression weight", 0.012f},
        {"stdp_tau", "STDP time constant in steps", 20},
        {"synaptogenesis_rate", "Rate of new synapse formation per step", 0.0001f},
        {"pruning_rate", "Rate of synapse elimination per step", 0.00001f},
        
        // Neuromodulation parameters
        {"dopamine_baseline", "Baseline dopamine level", 0.1f},
        {"dopamine_scale", "Dopamine effect scale factor", 1.0f},
        {"curiosity_baseline", "Baseline curiosity level", 0.1f},
        {"novelty_threshold", "Novelty detection threshold", 0.5f},
        
        // Memory system parameters
        {"working_memory_capacity", "Capacity of working memory in neuron traces", 100},
        {"episodic_memory_max_episodes", "Maximum number of episodic memory episodes", 1000},
        {"replay_interval", "Memory replay interval in steps", 100},
        {"consolidation_interval", "Memory consolidation interval in steps", 1000},
        
        // Cognitive parameters
        {"attention_inhibition_strength", "Strength of attentional inhibition", 0.5f},
        {"attention_excitation_strength", "Strength of attentional excitation", 1.5f},
        {"planning_depth", "Depth of planning horizon for neural planner", 5},
        
        // Development parameters
        {"development_rate", "Rate of developmental changes per step", 0.01f},
        {"plasticity_modulation", "Plasticity modulation factor during development", 1.0f},
        
        // I/O parameters
        {"checkpoint_dir", "Directory for saving brain checkpoints", "./checkpoints"},
        {"checkpoint_interval", "Interval for automatic checkpointing in steps", 10000},
        {"compression_level", "Checkpoint compression level (0=fast, 1=balanced, 2=best)", 1},
    };
    
    for (const auto& [key, desc, defaultVal] : configParams) {
        impl->paramDescriptions[key] = desc;
        impl->defaultValues[key] = defaultVal;
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeConfigSchema(pImpl.get());
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Parse configuration file in multiple formats (JSON, YAML, INI, simple key=value)
    // Implement progressive format detection and parsing
    
    // First, check file extension and determine format
    std::string ext = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.size() - 1) {
        ext = filepath.substr(dotPos);
    }
    
    // Try JSON parsing first (preferred format)
    if (ext == ".json" || ext == ".JSON") {
        return loadFromJSON(filepath);
    }
    // Try YAML parsing
    else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        return loadFromYAML(filepath);
    }
    // Try INI format
    else if (ext == ".ini" || ext == ".INI") {
        return loadFromINI(filepath);
    }
    // Default to simple key=value format (original implementation)
    else {
        return loadFromSimpleFormat(filepath);
    }
}

// Load configuration from JSON file (modern, robust format)
bool Config::loadFromJSON(const std::string& filepath) {
    // Use nlohmann/json library if available, otherwise implement lightweight JSON parser
    // For now, implement a simplified version that handles common JSON patterns
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing - look for "key": "value" patterns
    std::string line;
    while (std::getline(file, line)) {
        // Remove whitespace
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line.find("#") == 0 || line.find("//") == 0) {
            continue;
        }
        
        // Handle simple key-value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from key and value
            if (key.size() >= 2 && key.front() == '\"' && key.back() == '\"') {
                key = key.substr(1, key.size() - 2);
            }
            if (value.size() >= 2 && value.front() == '\"' && value.back() == '\"') {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Load configuration from YAML file (readable format)
bool Config::loadFromYAML(const std::string& filepath) {
    // Implement YAML parsing - look for "key: value" patterns with indentation
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t currentIndent = 0;
    std::string currentPrefix;
    
    while (std::getline(file, line)) {
        // Remove trailing whitespace
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line.find("#") == 0 || line.find("//") == 0) {
            continue;
        }
        
        // Calculate indentation
        size_t indent = line.find_first_not_of(" \t");
        
        // Handle new key-value pair
        if (indent == std::string::npos) {
            continue; // line with only whitespace
        }
        
        // Extract key and value
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > indent) {
            std::string key = trim(line.substr(indent, colonPos - indent));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Apply prefix for nested keys
            std::string fullKey = currentPrefix + key;
            set(fullKey, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Load configuration from INI file (traditional format)
bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Handle sections
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Handle key-value pairs
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
            
            // Apply section prefix
            std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
            set(fullKey, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Original simple key=value format parser (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with flexible parsing
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Handle --key value format (space separated)
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -k value format (short option)
        else if (arg.size() == 2 && arg[0] == '-') {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle boolean flags (e.g., --verbose)
        else if (arg.size() > 2 && arg[0] == '-' && arg[1] == '-') {
            std::string key = arg.substr(2);
            // Check if this key exists in config schema
            if (pImpl->paramDescriptions.find(key) != pImpl->paramDescriptions.end()) {
                set(key, true, ConfigSource::CommandLine);
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
    
    file << "# NLM Neural Learning Machine Configuration\n";
    file << "# Generated by NLM " << std::to_string(getConfigVersion()) << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Use parameter description if available, otherwise use entry description
        std::string desc = pImpl->paramDescriptions.find(entry.key) != pImpl->paramDescriptions.end() 
            ? pImpl->paramDescriptions.at(entry.key) 
            : entry.description;
        
        file << "# " << desc << "\n";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << "\", \";
                }
                file << "\"]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
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

// Configuration validation function
static bool validateConfigValue(const std::string& key, const ConfigValue& value) {
    // Check if key is valid and get expected type from schema
    // For now, implement basic validation that can be extended
    if (key.empty()) {
        return false;  // Invalid key
    }
    
    // Basic type validation based on key patterns
    // In a full implementation, this would check the schema for each key
    
    // Common patterns for validation
    if (key.find("seed") != std::string::npos) {
        // Validate that integer seeds are reasonable
        try {
            int seed = std::get<int>(value);
            return seed >= 0 && seed < std::numeric_limits<int>::max() / 2;
        } catch (...) {
            return false;
        }
    }
    
    if (key.find("rate") != std::string::npos || key.find("weight") != std::string::npos || 
        key.find("time") != std::string::npos) {
        // Validate floating point parameters are within reasonable bounds
        try {
            float fval = std::get<float>(value);
            return fval >= 0.0f && fval <= 100.0f;
        } catch (...) {
            // Try double
            try {
                double dval = std::get<double>(value);
                return dval >= 0.0 && dval <= 100.0;
            } catch (...) {
                return false;
            }
        }
    }
    
    if (key.find("count") != std::string::npos || key.find("steps") != std::string::npos) {
        // Validate integer parameters
        try {
            int64_t ival = std::get<int64_t>(value);
            return ival >= 0 && ival < std::numeric_limits<int64_t>::max() / 1000;
        } catch (...) {
            return false;
        }
    }
    
    // Default validation - accept any value
    return true;
}

// Validate configuration file content
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateConfigValue(entry.key, entry.value)) {
            NLM_LOG_ERROR("Configuration validation failed for key: " + entry.key);
            return false;
        }
    }
    return true;
}

// Sanitize configuration values (remove or fix problematic values)
void Config::sanitize() {
    for (auto& entry : pImpl->entries) {
        // Apply sanitization based on key patterns
        if (entry.key.find("weight") != std::string::npos) {
            // Clamp weight values to reasonable ranges
            try {
                float& weight = std::get<float>(entry.value);
                weight = std::clamp(weight, -1.0f, 1.0f);
            } catch (...) {
                try {
                    double& weight = std::get<double>(entry.value);
                    weight = std::clamp(weight, -1.0, 1.0);
                } catch (...) {
                    // Cannot sanitize this value, leave as-is
                }
            }
        }
        
        if (entry.key.find("rate") != std::string::npos) {
            // Ensure rate values are not excessively large
            try {
                float& rate = std::get<float>(entry.value);
                if (rate > 1.0f) rate = 1.0f;
            } catch (...) {
                try {
                    double& rate = std::get<double>(entry.value);
                    if (rate > 1.0) rate = 1.0;
                } catch (...) {
                    // Cannot sanitize this value, leave as-is
                }
            }
        }
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate configuration before setting
    if (!validateConfigValue(key, value)) {
        NLM_LOG_WARNING("Invalid configuration value for key: " + key + ", skipping");
        return;
    }
    
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
