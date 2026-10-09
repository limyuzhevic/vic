#include "Config.hpp"
#include "ConfigConstants.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastLoadedFile;
    bool isValid;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->isValid = true;
    // Initialize with default configuration values
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Set default configuration values using constants
    set("brain.neuron_count", ConfigConstants::NEURON_COUNT_DEFAULT, ConfigSource::Default);
    set("brain.synapse_density", ConfigConstants::SYNAPSE_DENSITY_DEFAULT, ConfigSource::Default);
    set("brain.connection_probability", ConfigConstants::CONNECTION_PROBABILITY_DEFAULT, ConfigSource::Default);
    set("brain.initial_weight_mean", ConfigConstants::INITIAL_WEIGHT_MEAN, ConfigSource::Default);
    set("brain.initial_weight_std", ConfigConstants::INITIAL_WEIGHT_STD, ConfigSource::Default);
    set("brain.v_thresh", ConfigConstants::V_THRESH_DEFAULT, ConfigSource::Default);
    set("brain.v_rest", ConfigConstants::V_REST_DEFAULT, ConfigSource::Default);
    set("brain.v_reset", ConfigConstants::V_RESET_DEFAULT, ConfigSource::Default);
    set("brain.tau_mem", ConfigConstants::TAU_MEM_DEFAULT, ConfigSource::Default);
    set("brain.tau_ref", ConfigConstants::TAU_REF_DEFAULT, ConfigSource::Default);
    
    // Plasticity defaults
    set("plasticity.stdp.enable", ConfigConstants::STDP_ENABLE_DEFAULT, ConfigSource::Default);
    set("plasticity.stdp.learning_rate", ConfigConstants::STDP_LEARNING_RATE_DEFAULT, ConfigSource::Default);
    set("plasticity.stdp.tau_plus", ConfigConstants::STDP_TAU_PLUS_DEFAULT, ConfigSource::Default);
    set("plasticity.stdp.tau_minus", ConfigConstants::STDP_TAU_MINUS_DEFAULT, ConfigSource::Default);
    set("plasticity.hebbian.enable", ConfigConstants::HEBBIAN_ENABLE_DEFAULT, ConfigSource::Default);
    set("plasticity.structural.enable", ConfigConstants::STRUCTURAL_PLASTICITY_ENABLE_DEFAULT, ConfigSource::Default);
    
    // Neuromodulation defaults
    set("neuromod.dopamine.scale", ConfigConstants::DOPAMINE_SCALE_DEFAULT, ConfigSource::Default);
    set("neuromod.curiosity.enable", ConfigConstants::CURIOSITY_ENABLE_DEFAULT, ConfigSource::Default);
    set("neuromod.novelty.enable", ConfigConstants::NOVELTY_ENABLE_DEFAULT, ConfigSource::Default);
    
    // Development defaults
    set("development.stage.initial_age", ConfigConstants::DEVELOPMENT_STAGE_INITIAL, ConfigSource::Default);
    set("development.stage.critical_age", ConfigConstants::DEVELOPMENT_STAGE_CRITICAL, ConfigSource::Default);
    set("development.stage.maturation_age", ConfigConstants::DEVELOPMENT_STAGE_MATURATION, ConfigSource::Default);
    set("development.stage.adult_age", ConfigConstants::DEVELOPMENT_STAGE_ADULT, ConfigSource::Default);
    
    // World defaults
    set("world.width", ConfigConstants::WORLD_WIDTH_DEFAULT, ConfigSource::Default);
    set("world.height", ConfigConstants::WORLD_HEIGHT_DEFAULT, ConfigSource::Default);
    set("world.vision_width", ConfigConstants::VISION_WIDTH_DEFAULT, ConfigSource::Default);
    set("world.vision_height", ConfigConstants::VISION_HEIGHT_DEFAULT, ConfigSource::Default);
    set("world.simulation_timestep", ConfigConstants::SIMULATION_TIMESTEP_DEFAULT, ConfigSource::Default);
    
    // Performance defaults
    set("performance.target_fps", ConfigConstants::PERFORMANCE_TARGET_FPS, ConfigSource::Default);
    set("performance.neuron_pool_size", ConfigConstants::NEURON_POOL_SIZE_DEFAULT, ConfigSource::Default);
    set("performance.synapse_pool_size", ConfigConstants::SYNAPSE_POOL_SIZE_DEFAULT, ConfigSource::Default);
    
    // Checkpoint defaults
    set("checkpoint.enable", ConfigConstants::CHECKPOINT_ENABLE_DEFAULT, ConfigSource::Default);
    set("checkpoint.interval", ConfigConstants::CHECKPOINT_INTERVAL_DEFAULT, ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Invalid file path: empty string");
        pImpl->isValid = false;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        pImpl->isValid = false;
        return false;
    }
    
    // Try to parse as JSON first (recommended)
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Validate JSON structure
        if (!jsonData.is_object()) {
            NLM_LOG_ERROR("Invalid JSON: root element must be an object");
            pImpl->isValid = false;
            return false;
        }
        
        // Clear existing entries and load from JSON
        pImpl->entries.clear();
        
        // Load configuration from JSON
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            try {
                // Try to determine type and load appropriately
                if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), ConfigSource::File);
                } else if (it->is_number_unsigned()) {
                    set(key, it->get<uint64_t>(), ConfigSource::File);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), ConfigSource::File);
                } else if (it->is_boolean()) {
                    set(key, it->get<bool>(), ConfigSource::File);
                } else if (it->is_string()) {
                    set(key, it->get<std::string>(), ConfigSource::File);
                } else if (it->is_array()) {
                    // For arrays, we need to preserve type information
                    // For now, store as string representation
                    std::ostringstream oss;
                    oss << it->dump();
                    set(key, oss.str(), ConfigSource::File);
                } else {
                    NLM_LOG_WARN("Unsupported JSON type for key " + key);
                }
            } catch (const std::exception& e) {
                NLM_LOG_WARN("Failed to parse value for key " + key + ": " + std::string(e.what()));
            }
        }
        
        pImpl->lastLoadedFile = filepath;
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        // Fallback to simple key=value format
        NLM_LOG_WARN("JSON parsing failed, falling back to simple format: " + std::string(e.what()));
        file.clear();
        file.seekg(0);
        
        std::string line;
        size_t lineNumber = 0;
        bool hasErrors = false;
        
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
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
                
                try {
                    // Try to infer type and parse accordingly
                    bool parsed = false;
                    
                    // Try boolean
                    if (value == "true" || value == "false") {
                        set(key, value == "true", ConfigSource::File);
                        parsed = true;
                    }
                    // Try integer
                    else if (std::all_of(value.begin(), value.end(), ::isdigit) ||
                             (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                        try {
                            set(key, std::stoll(value), ConfigSource::File);
                            parsed = true;
                        } catch (...) {}
                    }
                    // Try floating point
                    else {
                        try {
                            size_t pos2;
                            double doubleValue = std::stod(value, &pos2);
                            if (pos2 == value.size()) {
                                set(key, doubleValue, ConfigSource::File);
                                parsed = true;
                            }
                        } catch (...) {}
                    }
                    
                    if (!parsed) {
                        // Store as string
                        set(key, value, ConfigSource::File);
                    }
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing config line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
                    hasErrors = true;
                }
            } else {
                NLM_LOG_WARN("Invalid config line " + std::to_string(lineNumber) + ": missing '='");
                hasErrors = true;
            }
        }
        
        pImpl->lastLoadedFile = filepath;
        pImpl->isValid = !hasErrors;
        return !hasErrors;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments provided
    }
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Failed to parse command line argument: " + arg);
                    success = false;
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Failed to parse command line argument: " + arg + " " + value);
                success = false;
            }
        } else {
            NLM_LOG_ERROR("Invalid command line argument: " + arg);
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Invalid file path: empty string");
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write JSON format
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to appropriate JSON type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // For arrays, store as JSON array
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            } else {
                // Fallback to string representation
                std::ostringstream oss;
                oss << arg;
                jsonData[entry.key] = oss.str();
            }
        }, entry.value);
    }
    
    // Write JSON with formatting
    file << jsonData.dump(2);
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (!pImpl->isValid) {
        NLM_LOG_ERROR("Cannot get config value: configuration is in invalid state");
        return std::nullopt;
    }
    
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot get config value: empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Key not found - check if it's a default value that should be available
        if (isDefaultKey(key)) {
            // Return default value for known keys
            return getDefaultValue<T>(key);
        }
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config: empty key");
        return;
    }
    
    // Validate value based on key type
    if (!validateConfigValue(key, value)) {
        NLM_LOG_ERROR("Invalid configuration value for key: " + key);
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
    if (key.empty()) {
        return false;
    }
    
    bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    // Also check if it's a default key
    if (!exists && isDefaultKey(key)) {
        return true;
    }
    
    return exists;
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        return;
    }
    
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
    pImpl->isValid = true;
    initializeDefaults(); // Restore defaults after clear
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, valid: " << (pImpl->isValid ? "yes" : "no") << "):\n";
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

// Helper implementations
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

// Private helper methods
bool Config::setFromString(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to infer type and parse
    try {
        // Check for boolean values
        if (value == "true" || value == "false") {
            set(key, value == "true", source);
            return true;
        }
        
        // Check for integer values
        size_t pos;
        try {
            int64_t intValue = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(key, intValue, source);
                return true;
            }
        } catch (...) {}
        
        // Check for floating point values
        try {
            double doubleValue = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleValue, source);
                return true;
            }
        } catch (...) {}
        
        // Default to string
        set(key, value, source);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing string value for key " + key + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::validateConfigValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation - can be extended with more specific validation
    // For now, just ensure key format is reasonable
    if (key.empty() || key.find(' ') != std::string::npos) {
        return false;
    }
    
    // Key format validation
    if (key.find('.') != std::string::npos) {
        // Nested key (section.key)
        size_t dotPos = key.find('.');
        std::string section = key.substr(0, dotPos);
        std::string subfield = key.substr(dotPos + 1);
        
        if (section.empty() || subfield.empty()) {
            return false;
        }
    }
    
    return true;
}

bool Config::isDefaultKey(const std::string& key) const {
    // Check if key corresponds to a default configuration
    // This can be extended as needed
    static const std::vector<std::string> defaultKeys = {
        "brain.neuron_count", "brain.synapse_density", "brain.connection_probability",
        "brain.initial_weight_mean", "brain.initial_weight_std", "brain.v_thresh",
        "brain.v_rest", "brain.v_reset", "brain.tau_mem", "brain.tau_ref",
        "plasticity.stdp.enable", "plasticity.stdp.learning_rate", "plasticity.stdp.tau_plus",
        "plasticity.stdp.tau_minus", "plasticity.hebbian.enable", "plasticity.structural.enable",
        "neuromod.dopamine.scale", "neuromod.curiosity.enable", "neuromod.novelty.enable",
        "development.stage.initial_age", "development.stage.critical_age",
        "development.stage.maturation_age", "development.stage.adult_age",
        "world.width", "world.height", "world.vision_width", "world.vision_height",
        "world.simulation_timestep", "performance.target_fps",
        "performance.neuron_pool_size", "performance.synapse_pool_size",
        "checkpoint.enable", "checkpoint.interval"
    };
    
    return std::find(defaultKeys.begin(), defaultKeys.end(), key) != defaultKeys.end();
}

template<typename T>
T Config::getDefaultValue(const std::string& key) const {
    // Return default value for known keys based on constants
    if (key == "brain.neuron_count") return static_cast<T>(ConfigConstants::NEURON_COUNT_DEFAULT);
    if (key == "brain.synapse_density") return static_cast<T>(ConfigConstants::SYNAPSE_DENSITY_DEFAULT);
    if (key == "brain.connection_probability") return static_cast<T>(ConfigConstants::CONNECTION_PROBABILITY_DEFAULT);
    if (key == "brain.initial_weight_mean") return static_cast<T>(ConfigConstants::INITIAL_WEIGHT_MEAN);
    if (key == "brain.initial_weight_std") return static_cast<T>(ConfigConstants::INITIAL_WEIGHT_STD);
    if (key == "brain.v_thresh") return static_cast<T>(ConfigConstants::V_THRESH_DEFAULT);
    if (key == "brain.v_rest") return static_cast<T>(ConfigConstants::V_REST_DEFAULT);
    if (key == "brain.v_reset") return static_cast<T>(ConfigConstants::V_RESET_DEFAULT);
    if (key == "brain.tau_mem") return static_cast<T>(ConfigConstants::TAU_MEM_DEFAULT);
    if (key == "brain.tau_ref") return static_cast<T>(ConfigConstants::TAU_REF_DEFAULT);
    
    if (key == "plasticity.stdp.enable") return static_cast<T>(ConfigConstants::STDP_ENABLE_DEFAULT);
    if (key == "plasticity.stdp.learning_rate") return static_cast<T>(ConfigConstants::STDP_LEARNING_RATE_DEFAULT);
    if (key == "plasticity.stdp.tau_plus") return static_cast<T>(ConfigConstants::STDP_TAU_PLUS_DEFAULT);
    if (key == "plasticity.stdp.tau_minus") return static_cast<T>(ConfigConstants::STDP_TAU_MINUS_DEFAULT);
    if (key == "plasticity.hebbian.enable") return static_cast<T>(ConfigConstants::HEBBIAN_ENABLE_DEFAULT);
    if (key == "plasticity.structural.enable") return static_cast<T>(ConfigConstants::STRUCTURAL_PLASTICITY_ENABLE_DEFAULT);
    
    if (key == "neuromod.dopamine.scale") return static_cast<T>(ConfigConstants::DOPAMINE_SCALE_DEFAULT);
    if (key == "neuromod.curiosity.enable") return static_cast<T>(ConfigConstants::CURIOSITY_ENABLE_DEFAULT);
    if (key == "neuromod.novelty.enable") return static_cast<T>(ConfigConstants::NOVELTY_ENABLE_DEFAULT);
    
    if (key == "development.stage.initial_age") return static_cast<T>(ConfigConstants::DEVELOPMENT_STAGE_INITIAL);
    if (key == "development.stage.critical_age") return static_cast<T>(ConfigConstants::DEVELOPMENT_STAGE_CRITICAL);
    if (key == "development.stage.maturation_age") return static_cast<T>(ConfigConstants::DEVELOPMENT_STAGE_MATURATION);
    if (key == "development.stage.adult_age") return static_cast<T>(ConfigConstants::DEVELOPMENT_STAGE_ADULT);
    
    if (key == "world.width") return static_cast<T>(ConfigConstants::WORLD_WIDTH_DEFAULT);
    if (key == "world.height") return static_cast<T>(ConfigConstants::WORLD_HEIGHT_DEFAULT);
    if (key == "world.vision_width") return static_cast<T>(ConfigConstants::VISION_WIDTH_DEFAULT);
    if (key == "world.vision_height") return static_cast<T>(ConfigConstants::VISION_HEIGHT_DEFAULT);
    if (key == "world.simulation_timestep") return static_cast<T>(ConfigConstants::SIMULATION_TIMESTEP_DEFAULT);
    
    if (key == "performance.target_fps") return static_cast<T>(ConfigConstants::PERFORMANCE_TARGET_FPS);
    if (key == "performance.neuron_pool_size") return static_cast<T>(ConfigConstants::NEURON_POOL_SIZE_DEFAULT);
    if (key == "performance.synapse_pool_size") return static_cast<T>(ConfigConstants::SYNAPSE_POOL_SIZE_DEFAULT);
    
    if (key == "checkpoint.enable") return static_cast<T>(ConfigConstants::CHECKPOINT_ENABLE_DEFAULT);
    if (key == "checkpoint.interval") return static_cast<T>(ConfigConstants::CHECKPOINT_INTERVAL_DEFAULT);
    
    // Default fallback
    if constexpr (std::is_same_v<T, int>) return 0;
    if constexpr (std::is_same_v<T, int64_t>) return 0;
    if constexpr (std::is_same_v<T, double>) return 0.0;
    if constexpr (std::is_same_v<T, bool>) return false;
    if constexpr (std::is_same_v<T, std::string>) return "";
    
    return T{};
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
