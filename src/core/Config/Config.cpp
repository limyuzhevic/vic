#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool strictMode = false;
    bool validateOnSet = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration values
    setDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setDefaults() {
    // Set sensible default configuration values for NLM
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Hebbian learning parameters
    set("hebbian_learning_rate", 0.001f, ConfigSource::Default);
    set("hebbian_decay_time", 50.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation parameters
    set("dopamine_scale", 1.0f, ConfigSource::Default);
    set("curiosity_threshold", 0.5f, ConfigSource::Default);
    set("novelty_decay", 0.99f, ConfigSource::Default);
    
    // Development parameters
    set("critical_period_end", 300.0, ConfigSource::Default);
    set("maturation_rate", 0.8f, ConfigSource::Default);
    
    // Memory system parameters
    set("working_memory_capacity", 0.1f, ConfigSource::Default);
    set("episodic_memory_capacity", 1000, ConfigSource::Default);
    set("replay_interval", 100, ConfigSource::Default);
    
    // Checkpointing parameters
    set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    set("max_checkpoints", 10, ConfigSource::Default);
    set("checkpoint_compression", 1, ConfigSource::Default); // 1=balanced, 2=fast, 3=best
    
    // Logging parameters
    set("log_level", "INFO", ConfigSource::Default);
    set("enable_file_logging", true, ConfigSource::Default);
    set("log_file", "./nlm.log", ConfigSource::Default);
    
    // Cognitive parameters
    set("planning_depth", 5, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    
    // Sensory processing parameters
    set("vision_sensitivity", 1.0f, ConfigSource::Default);
    set("touch_sensitivity", 1.0f, ConfigSource::Default);
    set("internal_signal_sensitivity", 1.0f, ConfigSource::Default);
    
    // Motor output parameters
    set("motor_neuron_threshold", 5.0f, ConfigSource::Default);
    set("action_execution_time", 0.001, ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

void Config::setDefaults() {
    // Set sensible default configuration values for NLM
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Hebbian learning parameters
    set("hebbian_learning_rate", 0.001f, ConfigSource::Default);
    set("hebbian_decay_time", 50.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation parameters
    set("dopamine_scale", 1.0f, ConfigSource::Default);
    set("curiosity_threshold", 0.5f, ConfigSource::Default);
    set("novelty_decay", 0.99f, ConfigSource::Default);
    
    // Development parameters
    set("critical_period_end", 300.0, ConfigSource::Default);
    set("maturation_rate", 0.8f, ConfigSource::Default);
    
    // Memory system parameters
    set("working_memory_capacity", 0.1f, ConfigSource::Default);
    set("episodic_memory_capacity", 1000, ConfigSource::Default);
    set("replay_interval", 100, ConfigSource::Default);
    
    // Checkpointing parameters
    set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    set("max_checkpoints", 10, ConfigSource::Default);
    set("checkpoint_compression", 1, ConfigSource::Default); // 1=balanced, 2=fast, 3=best
    
    // Logging parameters
    set("log_level", "INFO", ConfigSource::Default);
    set("enable_file_logging", true, ConfigSource::Default);
    set("log_file", "./nlm.log", ConfigSource::Default);
    
    // Cognitive parameters
    set("planning_depth", 5, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    
    // Sensory processing parameters
    set("vision_sensitivity", 1.0f, ConfigSource::Default);
    set("touch_sensitivity", 1.0f, ConfigSource::Default);
    set("internal_signal_sensitivity", 1.0f, ConfigSource::Default);
    
    // Motor output parameters
    set("motor_neuron_threshold", 5.0f, ConfigSource::Default);
    set("action_execution_time", 0.001, ConfigSource::Default);
}

// Check if configuration is valid
bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

// Get validation errors
std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check required fields
    std::vector<std::string> requiredFields = {
        "neuron_count", "region_count", "connection_probability"
    };
    
    for (const auto& field : requiredFields) {
        if (!has(field)) {
            errors.push_back("Missing required configuration field: " + field);
        }
    }
    
    // Validate numeric ranges
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (neuronCount && (*neuronCount <= 0 || *neuronCount > 1000000)) {
            errors.push_back("neuron_count must be between 1 and 1000000");
        }
    }
    
    if (has("region_count")) {
        auto regionCount = get<int64_t>("region_count");
        if (regionCount && (*regionCount <= 0 || *regionCount > 100)) {
            errors.push_back("region_count must be between 1 and 100");
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = get<double>("connection_probability");
        if (connProb && (*connProb < 0.0 || *connProb > 1.0)) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    return errors;
}

// Enable/disable strict mode
void Config::setStrictMode(bool strict) {
    pImpl->strictMode = strict;
}

bool Config::getStrictMode() const {
    return pImpl->strictMode;
}

// Enable/disable validation on set
void Config::setValidateOnSet(bool validate) {
    pImpl->validateOnSet = validate;
}

bool Config::getValidateOnSet() const {
    return pImpl->validateOnSet;
}
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

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
