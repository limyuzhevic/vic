#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (key.empty()) {
                NLM_LOG_WARNING("Config file " + filepath + ", line " + std::to_string(lineNumber) + ": Empty key (ignored)");
                continue;
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config file " + filepath + ", line " + std::to_string(lineNumber) lepton << ": Error parsing value for key \"" << key << "\": " << e.what());
                // Continue processing other lines instead of failing completely
            }
        } else {
            NLM_LOG_WARNING("Config file " + filepath + ", line " + std::to_string(lineNumber) + ": No '=' found (ignored)");
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from file: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        NLM_LOG_WARNING("Config::loadFromArgs: No command line arguments provided");
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0,  Implement proper JSON/YAML parser
    PLACEHOLDER - Phase 1 uses a simple key=value format

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
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    if (pImpl->entries.empty()) {
        NLM_LOG_WARNING("Attempted to save empty configuration to file: " + filepath);
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to file: " + filepath);
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << getSourceString(entry.source) << ")\n";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    return it->source;
}

std::string Config::getSourceString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Memory: return "Memory";
        default: return "Unknown";
    }
}

}

// Configuration validation result structure
struct ConfigValidationResult {
    bool isValid;
    std::vector<std::string> errors;
};

ConfigValidationResult Config::validate() const {
    ConfigValidationResult result;
    std::vector<std::string> errors;
    bool isValid = ConfigValidator::validate(*this, errors);
    
    result.isValid = isValid;
    result.errors = errors;
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_WARNING("Configuration validation failed with " + std::to_string(errors.size()) + " errors:");
        for (const auto& error : errors) {
            NLM_LOG_WARNING("  - " + error);
        }
    }
    
    return result;
}

void Config::applyDefaults() {
    NLM_LOG_INFO("Applying configuration defaults...");
    
    // Set reasonable defaults if values are missing or invalid
    setIfMissingOrInvalid("neuron_count", 1000, ConfigSource::Default);
    setIfMissingOrInvalid("region_count", 2, ConfigSource::Default);
    setIfMissingOrInvalid("connection_probability", 0.1f, ConfigSource::Default);
    setIfMissingOrInvalid("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    setIfMissingOrInvalid("simulation_timestep", 0.001, ConfigSource::Default);
    setIfMissingOrInvalid("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    setIfMissingOrInvalid("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    setIfMissingOrInvalid("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    setIfMissingOrInvalid("stdp_time_constant", 20.0f, ConfigSource::Default);
    setIfMissingOrInvalid("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    setIfMissingOrInvalid("pruning_rate", 0.00001f, ConfigSource::Default);
    setIfMissingOrInvalid("dopamine_baseline", 0.1f, ConfigSource::Default);
    setIfMissingOrInvalid("reward_discount_factor", 0.99f, ConfigSource::Default);
    setIfMissingOrInvalid("environment_name", std::string("GridWorld"), ConfigSource::Default);
    setIfMissingOrInvalid("environment_width", 10, ConfigSource::Default);
    setIfMissingOrInvalid("environment_height", 10, ConfigSource::Default);
    setIfMissingOrInvalid("log_level", std::string("INFO"), ConfigSource::Default);
    setIfMissingOrInvalid("log_to_file", false, ConfigSource::Default);
    setIfMissingOrInvalid("log_filename", std::string("nlm.log"), ConfigSource::Default);
    setIfMissingOrInvalid("max_simulation_steps", 10000, ConfigSource::Default);
    setIfMissingOrInvalid("simulation_time_limit", 0.0, ConfigSource::Default);
    setIfMissingOrInvalid("visualization_enabled", false, ConfigSource::Default);
    setIfMissingOrInvalid("visualization_update_rate", 30.0f, ConfigSource::Default);
    
    NLM_LOG_INFO("Configuration defaults applied successfully");
}

void Config::setIfMissingOrInvalid(const std::string& key, const ConfigValue& defaultValue, ConfigSource source) {
    auto value = get<std::string>(key);
    
    // Check if key exists and if the value is valid
    if (!has(key)) {
        NLM_LOG_INFO("Setting default for \"" + key + "\": " + ConfigValueToString(defaultValue));
        set(key, defaultValue, source);
    } else {
        // For now, we'll just apply defaults regardless of current values
        // In a more sophisticated implementation, we could validate values and only set defaults if invalid
        NLM_LOG_INFO("Applying default for \"" + key + "\": " + ConfigValueToString(defaultValue));
        set(key, defaultValue, source);
    }
}

std::string Config::ConfigValueToString(const ConfigValue& value) {
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same<T, std::string>()) {
            return "\"" + val + "\"";
        } else if constexpr (std::is_same<T, int>() || std::is_same<T, int64_t>() ||
                             std::is_same<T, double>() || std::is_same<T, bool>() ||
                             std::is_same<T, float>() || std::is_same<T, std::vector<int>>() ||
                             std::is_same<T, std::vector<double>>() ||
                             std::is_same<T, std::vector<std::string>>()) {
            return std::to_string(val);
        }
    }, value);
    return "";
}
