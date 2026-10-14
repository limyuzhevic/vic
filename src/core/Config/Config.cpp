#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<std::string>> keySynonyms;
    
    // Configuration validation and defaults
    static constexpr size_t MAX_ENTRIES = 10000;
    static constexpr size_t MAX_KEY_LENGTH = 256;
    static constexpr size_t MAX_VALUE_LENGTH = 4096;
    
    // Default configuration values
    static const std::unordered_map<std::string, std::pair<ConfigValue, ConfigSource>> DEFAULTS;
};

// Initialize default configuration values
const std::unordered_map<std::string, std::pair<ConfigValue, ConfigSource>> Config::Impl::DEFAULTS = {
    // Brain configuration
    {"brain.neuron_count", {int64_t(1000), ConfigSource::Default}},
    {"brain.synapse_density", {0.1f, ConfigSource::Default}},
    {"brain.connection_probability", {0.05f, ConfigSource::Default}},
    {"brain.initial_weight_mean", {0.5f, ConfigSource::Default}},
    {"brain.initial_weight_std", {0.1f, ConfigSource::Default}},
    {"brain.v_thresh", {-50.0f, ConfigSource::Default}},
    {"brain.v_rest", {-70.0f, ConfigSource::Default}},
    {"brain.v_reset", {-75.0f, ConfigSource::Default}},
    {"brain.tau_mem", {20.0, ConfigSource::Default}},
    {"brain.tau_ref", {2.0, ConfigSource::Default}},
    {"brain.refractory_period", {5, ConfigSource::Default}},
    
    // Simulation configuration
    {"simulation.timestep", {0.001, ConfigSource::Default}},
    {"simulation.max_steps", {int64_t(10000), ConfigSource::Default}},
    {"simulation.enable_profiling", {false, ConfigSource::Default}},
    {"simulation.log_level", {std::string("INFO"), ConfigSource::Default}},
    
    // Plasticity configuration
    {"plasticity.stdp.enable", {true, ConfigSource::Default}},
    {"plasticity.stdp.learning_rate", {0.001, ConfigSource::Default}},
    {"plasticity.stdp.tau_plus", {20.0, ConfigSource::Default}},
    {"plasticity.stdp.tau_minus", {20.0, ConfigSource::Default}},
    {"plasticity.stdp.ltp_weight", {0.02f, ConfigSource::Default}},
    {"plasticity.stdp.ltd_weight", {0.015f, ConfigSource::Default}},
    {"plasticity.stdp.min_weight", {-1.0f, ConfigSource::Default}},
    {"plasticity.stdp.max_weight", {1.0f, ConfigSource::Default}},
    
    {"plasticity.hebbian.enable", {true, ConfigSource::Default}},
    {"plasticity.hebbian.learning_rate", {0.001, ConfigSource::Default}},
    {"plasticity.hebbian.max_weight", {1.0f, ConfigSource::Default}},
    
    {"plasticity.structural.enable", {true, ConfigSource::Default}},
    {"plasticity.structural.synaptogenesis_rate", {0.0001f, ConfigSource::Default}},
    {"plasticity.structural.pruning_rate", {0.00001f, ConfigSource::Default}},
    {"plasticity.structural.min_synaptic_weight", {0.01f, ConfigSource::Default}},
    {"plasticity.structural.max_synaptic_age", {int64_t(10000), ConfigSource::Default}},
    
    // Neuromodulation configuration
    {"neuromod.dopamine.scale", {1.0, ConfigSource::Default}},
    {"neuromod.dopamine.baseline", {0.0, ConfigSource::Default}},
    {"neuromod.curiosity.enable", {true, ConfigSource::Default}},
    {"neuromod.curiosity.sensitivity", {0.5, ConfigSource::Default}},
    {"neuromod.novelty.threshold", {0.1, ConfigSource::Default}},
    {"neuromod.novelty.smoothing", {0.95, ConfigSource::Default}},
    
    // Memory system configuration
    {"memory.working.capacity", {int64_t(1000), ConfigSource::Default}},
    {"memory.working.decay_rate", {0.01, ConfigSource::Default}},
    {"memory.episodic.capacity", {int64_t(1000), ConfigSource::Default}},
    {"memory.associative.capacity", {int64_t(10000), ConfigSource::Default}},
    
    // Prediction system configuration
    {"prediction.window_size", {int64_t(100), ConfigSource::Default}},
    {"prediction.horizon", {int64_t(10), ConfigSource::Default}},
    {"prediction.confidence_threshold", {0.8, ConfigSource::Default}},
    
    // Development configuration
    {"development.initial_stage", {0, ConfigSource::Default}},
    {"development.stage_duration", {int64_t(1000), ConfigSource::Default}},
    {"development.plasticity_modulation", {0.1, ConfigSource::Default}},
    {"development.reproduction_rate", {0.01, ConfigSource::Default}},
    
    // Output and logging configuration
    {"output.enable_logging", {true, ConfigSource::Default}},
    {"output.log_file", {std::string("nlm.log"), ConfigSource::Default}},
    {"output.log_level", {std::string("INFO"), ConfigSource::Default}},
    {"output.enable_profiling", {false, ConfigSource::Default}},
    {"output.profile_file", {std::string("profile.json"), ConfigSource::Default}},
    
    // Performance optimization
    {"performance.use_thread_pool", {true, ConfigSource::Default}},
    {"performance.thread_count", {int64_t(0), ConfigSource::Default}},  // 0 = auto-detect
    {"performance.enable_simd", {true, ConfigSource::Default}},
    {"performance.use_memory_pools", {true, ConfigSource::Default}},
    
    // Advanced features
    {"advanced.enable_replay", {true, ConfigSource::Default}},
    {"advanced.replay_interval", {int64_t(100), ConfigSource::Default}},
    {"advanced.consolidation_interval", {int64_t(1000), ConfigSource::Default}},
    {"advanced.enable_tracing", {false, ConfigSource::Default}},
    {"advanced.trace_file", {std::string("trace.json"), ConfigSource::Default}},
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize synonyms for common configuration keys
    initializeSynonyms();
    
    // Set up default values
    setupDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::Impl::initializeSynonyms() {
    // Add common synonyms for frequently used configuration keys
    keySynonyms = {
        {"neuron_count", {"brain.neuron_count", "neurons", "n"}},
        {"synapse_density", {"brain.synapse_density", "synapse.density"}},
        {"connection_probability", {"brain.connection_probability", "connect_prob", "p"}},
        {"learning_rate", {"plasticity.learning_rate", "learning_rate"}},
        {"simulation_timestep", {"simulation.timestep", "timestep", "dt"}},
        {"random_seed", {"seed", "random.seed"}},
        {"enable_reward_modulation", {"reward_modulation", "reward", "dopamine"}},
        {"enable_structural_plasticity", {"structural_plasticity", "structural", "synaptogenesis"}},
        {"enable_development", {"development", "brain_development"}},
        {"enable_curiosity", {"curiosity", "exploration"}},
        {"log_level", {"logging.level", "log_level", "verbosity"}},
        {"output_file", {"output.file", "log_file", "results_file"}}
    };
}

void Config::setupDefaults() {
    // Apply all default values
    for (const auto& [key, value_source] : Impl::DEFAULTS) {
        const auto& [default_value, default_source] = value_source;
        set(key, default_value, default_source);
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first (if available)
    if (loadFromJSON(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    NLM_LOG_INFO("Falling back to simple config file format: " + filepath);
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    // This is a placeholder for JSON parsing functionality
    // In a real implementation, you would use a JSON library like nlohmann::json
    
    // Check if file extension suggests JSON
    if (filepath.size() < 5 || filepath.substr(filepath.size() - 5) != ".json") {
        return false;
    }
    
    // For now, return false to fall back to simple format
    // TODO: Implement proper JSON parsing
    return false;
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
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
            
            // Validate key length
            if (key.length() > Impl::MAX_KEY_LENGTH) {
                NLM_LOG_WARNING("Config line " + std::to_string(lineNumber) + ": " +
                              "Key \"" + key + "\" is too long (max " +
                              std::to_string(Impl::MAX_KEY_LENGTH) + " characters)");
                continue;
            }
            
            // Validate value length
            if (value.length() > Impl::MAX_VALUE_LENGTH) {
                NLM_LOG_WARNING("Config line " + std::to_string(lineNumber) + ": " +
                              "Value for key \"" + key + "\" is too long (max " +
                              std::to_string(Impl::MAX_VALUE_LENGTH) + " characters)");
                continue;
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config line " + std::to_string(lineNumber) + ": " +
                             "Error setting key \"" + key + "\": " + std::string(e.what()));
            }
        } else {
            NLM_LOG_WARNING("Config line " + std::to_string(lineNumber) + ": " +
                          "Invalid format (expected key=value): \"" + line + "\"");
        }
    }
    
    return true;
}

// Continue implementing the rest of the Config class methods

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
        // Handle --key (boolean flag)
        else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            set(key, ConfigValue(true), ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format if possible, otherwise fall back to simple format
    if (saveToJSON(filepath)) {
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
    }
    
    NLM_LOG_INFO("Falling back to simple config file format: " + filepath);
    return saveToSimpleFormat(filepath);
}

bool Config::saveToJSON(const std::string& filepath) const {
    // This is a placeholder for JSON saving functionality
    // In a real implementation, you would use a JSON library like nlohmann::json
    
    // For now, return false to fall back to simple format
    return false;
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM " << __DATE__ << " " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Format the value as string
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // First try direct key lookup
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // Try key synonyms
    auto synonymIt = pImpl->keySynonyms.find(key);
    if (synonymIt != pImpl->keySynonyms.end()) {
        for (const auto& synonym : synonymIt->second) {
            auto synonymResult = get<T>(synonym);
            if (synonymResult.has_value()) {
                return synonymResult;
            }
        }
    }
    
    // Try default values
    auto defaultIt = Impl::DEFAULTS.find(key);
    if (defaultIt != Impl::DEFAULTS.end()) {
        try {
            return std::get<T>(defaultIt->second.first);
        } catch (const std::bad_variant_access&) {
            // Type mismatch, not a problem
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.length() > Impl::MAX_KEY_LENGTH) {
        throw std::invalid_argument("Key \"" + key + "\" is too long (max " +
                                   std::to_string(Impl::MAX_KEY_LENGTH) + " characters)");
    }
    
    // Validate value length if it's a string
    if (std::holds_alternative<std::string>(value)) {
        const std::string& strValue = std::get<std::string>(value);
        if (strValue.length() > Impl::MAX_VALUE_LENGTH) {
            throw std::invalid_argument("Value for key \"" + key + "\" is too long (max " +
                                       std::to_string(Impl::MAX_VALUE_LENGTH) + " characters)");
        }
    }
    
    // Check entry count
    if (pImpl->entries.size() >= Impl::MAX_ENTRIES) {
        throw std::runtime_error("Configuration has too many entries (max " +
                               std::to_string(Impl::MAX_ENTRIES) + ")");
    }
    
    // Try to get existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        
        // Set description if not provided
        if (pImpl->entries.back().description.empty()) {
            pImpl->entries.back().description = "Configuration parameter: " + key;
        }
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
    // Check direct key
    if (std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; })) {
        return true;
    }
    
    // Check synonyms
    auto synonymIt = pImpl->keySynonyms.find(key);
    if (synonymIt != pImpl->keySynonyms.end()) {
        for (const auto& synonym : synonymIt->second) {
            if (has(synonym)) {
                return true;
            }
        }
    }
    
    // Check default values
    return Impl::DEFAULTS.find(key) != Impl::DEFAULTS.end();
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
    
    // Add all configured keys
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    // Add default keys that haven't been overridden
    for (const auto& [key, _] : Impl::DEFAULTS) {
        if (!has(key)) {
            keys.push_back(key);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    // Re-apply defaults after clearing
    setupDefaults();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "NLM Configuration Summary\n";
    oss << "==========================\n\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Active source: " << static_cast<int>(getOr<ConfigSource>("active_source", ConfigSource::Default)) << "\n\n";
    
    oss << "Brain Configuration:\n";
    oss << "  - Neurons: " << getOr<int64_t>("brain.neuron_count", 1000) << "\n";
    oss << "  - Synapse Density: " << getOr<float>("brain.synapse_density", 0.1f) << "\n";
    oss << "  - Connection Probability: " << getOr<float>("brain.connection_probability", 0.05f) << "\n\n";
    
    oss << "Plasticity Configuration:\n";
    oss << "  - STDP Enabled: " << (getOr<bool>("plasticity.stdp.enable", false) ? "Yes" : "No") << "\n";
    oss << "  - Hebbian Enabled: " << (getOr<bool>("plasticity.hebbian.enable", false) ? "Yes" : "No") << "\n";
    oss << "  - Structural Plasticity: " << (getOr<bool>("plasticity.structural.enable", false) ? "Yes" : "No") << "\n\n";
    
    oss << "Neuromodulation:\n";
    oss << "  - Dopamine Scale: " << getOr<float>("neuromod.dopamine.scale", 1.0f) << "\n";
    oss << "  - Curiosity Enabled: " << (getOr<bool>("neuromod.curiosity.enable", false) ? "Yes" : "No") << "\n";
    oss << "  - Novelty Detection: " << (getOr<bool>("neuromod.novelty.enable", false) ? "Yes" : "No") << "\n\n";
    
    oss << "Development:\n";
    oss << "  - Stage: " << getOr<int64_t>("development.initial_stage", 0) << "\n";
    oss << "  - Reproduction Rate: " << getOr<float>("development.reproduction_rate", 0.01f) << "\n\n";
    
    oss << "System Configuration:\n";
    oss << "  - Simulation Timestep: " << getOr<double>("simulation.timestep", 0.001) << "s\n";
    oss << "  - Max Steps: " << getOr<int64_t>("simulation.max_steps", 10000) << "\n";
    oss << "  - Profiling Enabled: " << (getOr<bool>("simulation.enable_profiling", false) ? "Yes" : "No") << "\n";
    
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

// Add validation helper methods
bool Config::validate(bool checkDefaults) const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Check for required configuration
        if (checkDefaults && Impl::DEFAULTS.find(key) == Impl::DEFAULTS.end()) {
            errors.push_back("Unknown configuration key: " + key);
        }
        
        // Validate value ranges based on key
        if (!validateValue(key, entry.value)) {
            errors.push_back("Invalid value for key: " + key);
        }
    }
    
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation failed:\n";
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
        NLM_LOG_ERROR(oss.str());
        return false;
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Placeholder for value validation
    // In a real implementation, this would validate based on key
    return true;
}

// Helper to get configuration in various formats
std::string Config::exportToYAML() const {
    // This would use a YAML library
    // For now, return a simplified representation
    return "# YAML export not yet implemented\n";
}

std::string Config::exportToJSON() const {
    // This would use a JSON library
    // For now, return a simplified representation
    return "{\n  // JSON export not yet implemented\n}";
}

std::vector<std::string> Config::getConfigFiles() const {
    std::vector<std::string> configFiles;
    
    // Get all configuration files from the configs directory
    std::string configDir = "configs/";
    
    if (std::filesystem::exists(configDir) && std::filesystem::is_directory(configDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(configDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".json") {
                    configFiles.push_back(filename);
                } else if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".cfg") {
                    configFiles.push_back(filename);
                } else if (filename == "default.cfg" || filename == "default.json") {
                    configFiles.push_back(filename);
                }
            }
        }
    }
    
    return configFiles;
}

std::vector<ConfigEntry> Config::getConfigHistory() const {
    // This would track configuration changes over time
    // For now, return all current entries
    return pImpl->entries;
}

bool Config::importFromBackup(const std::string& backupFile) {
    // Import configuration from backup file
    // This would typically save/load the entire configuration state
    return loadFromFile(backupFile);
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
