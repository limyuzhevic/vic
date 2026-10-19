#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <cctype>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <cmath>
#include <cfloat>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Index for faster key lookup
    std::unordered_map<std::string, size_t> keyIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with known configuration parameters
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Set default configuration parameters for Phase 2
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    set("dopamine_baseline", 0.1f, ConfigSource::Default);  // Fixed: Use final value (was 0.0 and 0.1)
    set("reward_discount_factor", 0.99f, ConfigSource::Default);
    set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    set("environment_width", static_cast<int64_t>(10), ConfigSource::Default);
    set("environment_height", static_cast<int64_t>(10), ConfigSource::Default);
    set("log_level", std::string("INFO"), ConfigSource::Default);
    set("log_to_file", false, ConfigSource::Default);
    set("log_filename", std::string("nlm.log"), ConfigSource::Default);
    set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    set("simulation_time_limit", 0.0f, ConfigSource::Default);
    set("visualization_enabled", false, ConfigSource::Default);
    set("visualization_update_rate", 30.0f, ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading configuration from: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    // Clear existing entries before loading new ones
    clear();
    
    std::string line;
    int lineNum = 0;
    size_t totalLines = std::count(std::istreambuf_iterator<char>(file), 
                                 std::istreambuf_iterator<char>(), '\n');
    
    file.clear();
    file.seekg(0);
    
    while (std::getline(file, line)) {
        ++lineNum;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": missing '='");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        if (!set(key, value, ConfigSource::File)) {
            NLM_LOG_WARNING("Failed to parse config key-value: " + key + " = " + value);
        }
        
        // Progress reporting for large files
        if (totalLines > 100 && lineNum % 50 == 0) {
            NLM_LOG_INFO("Parsing config... " + std::to_string((lineNum * 100) / totalLines) + "%");
        }
    }
    
    NLM_LOG_INFO("Configuration loaded successfully: " + std::to_string(pImpl->entries.size()) + " entries");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        NLM_LOG_DEBUG("No command line arguments provided");
        return true;
    }
    
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!set(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Failed to set config from command line: " + key + " = " + value);
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!set(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Failed to set config from command line: " + key + " = " + value);
            }
        }
        else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Saved on " << getCurrentTimeString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        switch (entry.value.index()) {
            case 0: case 1: // integer types
                file << entry.key << " = " << std::get<int64_t>(entry.value) << "\n";
                break;
            case 2: // double
                file << entry.key << " = " << std::get<double>(entry.value) << "\n";
                break;
            case 3: // bool
                file << entry.key << " = " << (std::get<bool>(entry.value) ? "true" : "false") << "\n";
                break;
            case 4: // string
                file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"\n";
                break;
            case 5: case 6: // vectors - save as JSON-like format
                file << entry.key << " = [";
                if (entry.value.index() == 5) {
                    const auto& vec = std::get<std::vector<int>>(entry.value);
                    for (size_t j = 0; j < vec.size(); ++j) {
                        file << vec[j] << (j < vec.size() - 1 ? ", " : "");
                    }
                } else {
                    const auto& vec = std::get<std::vector<double>>(entry.value);
                    for (size_t j = 0; j < vec.size(); ++j) {
                        file << vec[j] << (j < vec.size() - 1 ? ", " : "");
                    }
                }
                file << "]\n";
                break;
            case 7: // vector of strings
                file << entry.key << " = [";
                const auto& vec = std::get<std::vector<std::string>>(entry.value);
                for (size_t j = 0; j < vec.size(); ++j) {
                    file << "\"" << vec[j] << "\"" << (j < vec.size() - 1 ? ", " : "");
                }
                file << "]\n";
                break;
            default:
                file << entry.key << " = " << "UNKNOWN_TYPE\n";
                break;
        }
        file << "\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

bool Config::validateAndNormalize() const {
    // Validate configuration parameters
    bool isValid = true;
    
    // Validate numeric ranges
    auto validateRange = [this](const std::string& key, double value, double min, double max, 
                                 const std::string& name) {
        if (value < min || value > max) {
            NLM_LOG_WARNING("Config " + key + " = " + std::to_string(value) + 
                          " outside recommended range [" + std::to_string(min) + ", " + 
                          std::to_string(max) + "] for " + name);
            isValid = false;
        }
    };
    
    try {
        if (has("simulation_timestep")) {
            double timestep = getOr<double>("simulation_timestep", 0.001);
            validateRange("simulation_timestep", timestep, 1e-6, 1.0, "simulation timestep");
        }
        
        if (has("neuron_count")) {
            int64_t count = getOr<int64_t>("neuron_count", 500);
            validateRange("neuron_count", static_cast<double>(count), 1.0, 1e6, "neuron count");
        }
        
        if (has("connection_probability")) {
            double prob = getOr<double>("connection_probability", 0.15);
            validateRange("connection_probability", prob, 0.0, 1.0, "connection probability");
        }
        
        if (has("reward_discount_factor")) {
            double discount = getOr<double>("reward_discount_factor", 0.99);
            validateRange("reward_discount_factor", discount, 0.0, 1.0, "discount factor");
        }
        
        if (has("environment_width") && has("environment_height")) {
            int64_t width = getOr<int64_t>("environment_width", 10);
            int64_t height = getOr<int64_t>("environment_height", 10);
            validateRange("environment_width", static_cast<double>(width), 1.0, 1000, "environment width");
            validateRange("environment_height", static_cast<double>(height), 1.0, 1000, "environment height");
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during configuration validation: " + std::string(e.what()));
        isValid = false;
    }
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_WARNING("Configuration validation completed with warnings");
    }
    
    return isValid;
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
        NLM_LOG_ERROR("Config type mismatch for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    NLM_LOG_DEBUG("Using default value for key '" + key + "': " + std::to_string(defaultValue));
    return defaultValue;
}

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set empty configuration key");
        return false;
    }
    
    // Validate value based on key type
    bool isValid = validateKeyType(key, value);
    if (!isValid) {
        NLM_LOG_ERROR("Invalid value type for config key: " + key);
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // Update index
        if (source != ConfigSource::Default) {
            pImpl->keyIndex[key] = std::distance(pImpl->entries.begin(), it);
        }
        
        NLM_LOG_DEBUG("Updated config: " + key + " = " + getValueString(value));
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyIndex[key] = pImpl->entries.size() - 1;
        
        NLM_LOG_DEBUG("Added config: " + key + " = " + getValueString(value));
    }
    
    return true;
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    // Use index for O(1) lookup if available
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        return it->second < pImpl->entries.size();
    }
    
    // Fallback to linear search
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it, pImpl->entries.end());
        
        // Update index
        pImpl->keyIndex.erase(key);
        
        // Rebuild index for remaining entries
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            pImpl->keyIndex[pImpl->entries[i].key] = i;
        }
        
        NLM_LOG_DEBUG("Removed config key: " + key);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    // Use index to get keys in order
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
    
    // Reinitialize with defaults
    initializeDefaults();
    NLM_LOG_DEBUG("Configuration cleared and reinitialized with defaults");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "=" * 60 << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << std::left << std::setw(20) << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (Source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    oss << "=" * 60 << "\n";
    
    // Validate and report any issues
    validateAndNormalize();
    
    return oss.str();
}

bool Config::validateKeyType(const std::string& key, const ConfigValue& value) const {
    // Basic validation - can be extended for specific key-value type checking
    return true;
}

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

} // namespace nlm
