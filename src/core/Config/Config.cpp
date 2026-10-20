#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Initialize the configuration with default values
Config::Config() : pImpl(std::make_unique<Impl>()) {
    registerDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Register default configuration values
void Config::registerDefaults() {
    // Brain parameters
    set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("brain.synapse_density", 0.1f, ConfigSource::Default);
    set("brain.connection_probability", 0.1f, ConfigSource::Default);
    set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
    set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
    set("brain.v_thresh", -50.0f, ConfigSource::Default);
    set("brain.v_rest", -70.0f, ConfigSource::Default);
    set("brain.v_reset", -75.0f, ConfigSource::Default);
    set("brain.tau_mem", 20.0f, ConfigSource::Default);
    set("brain.tau_ref", 2.0f, ConfigSource::Default);
    
    // Plasticity parameters
    set("plasticity.stdp.enable", true, ConfigSource::Default);
    set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    set("plasticity.hebbian.enable", true, ConfigSource::Default);
    set("plasticity.structural.enable", true, ConfigSource::Default);
    
    // Neuromodulation parameters
    set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    set("neuromod.curiosity.enable", true, ConfigSource::Default);
    set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    // Reward parameters
    set("reward.discount_factor", 0.99f, ConfigSource::Default);
    
    // Development parameters
    set("development.synaptogenesis_rate", 0.001f, ConfigSource::Default);
    set("development.pruning_rate", 0.0001f, ConfigSource::Default);
    
    // Environment parameters
    set("environment.name", "GridWorld", ConfigSource::Default);
    set("environment.width", 10.0f, ConfigSource::Default);
    set("environment.height", 10.0f, ConfigSource::Default);
    
    // Simulation parameters
    set("simulation.timestep", 0.001f, ConfigSource::Default);
    set("simulation.max_steps", 10000, ConfigSource::Default);
    
    // Logging parameters
    set("logging.level", "INFO", ConfigSource::Default);
    set("logging.to_file", false, ConfigSource::Default);
    set("logging.filename", "nlm.log", ConfigSource::Default);
    
    // Visualization parameters
    set("visualization.enabled", false, ConfigSource::Default);
    set("visualization.update_rate", 30.0f, ConfigSource::Default);
}

// Validate all configuration values
bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            // NLM_LOG_ERROR("Configuration validation error: " + error);
        }
        return false;
    }
    return true;
}

// Get validation errors
std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check for required configuration
    if (!has("brain.neuron_count")) {
        errors.push_back("Missing required configuration: brain.neuron_count");
    }
    
    if (!has("simulation.timestep")) {
        errors.push_back("Missing required configuration: simulation.timestep");
    }
    
    // Validate numeric ranges
    if (has("brain.neuron_count")) {
        int64_t neurons = getOr<int64_t>("brain.neuron_count", 1000);
        if (neurons < 10) {
            errors.push_back("brain.neuron_count must be at least 10");
        } else if (neurons > 1000000) {
            errors.push_back("brain.neuron_count must not exceed 1,000,000");
        }
    }
    
    if (has("brain.connection_probability")) {
        float prob = getOr<float>("brain.connection_probability", 0.1f);
        if (prob < 0.0f || prob > 1.0f) {
            errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
    }
    
    if (has("simulation.timestep")) {
        float dt = getOr<float>("simulation.timestep", 0.001f);
        if (dt <= 0.0f) {
            errors.push_back("simulation.timestep must be positive");
        } else if (dt > 1.0f) {
            errors.push_back("simulation.timestep must not exceed 1.0 second");
        }
    }
    
    return errors;
}

// Add validation for specific key
void Config::addValidator(const std::string& key, const ValidationCallback& validator) {
    // This would require modifying the ConfigEntry structure to include validators
    // For now, we'll add basic validation in getValidationErrors()
    (void)key;
    (void)validator;
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