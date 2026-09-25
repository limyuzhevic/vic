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

void Config::set(const std::string& key, float value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, size_t value, ConfigSource source) {
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

// Get commonly used types
std::optional<bool> Config::getBool(const std::string& key) const {
    auto val = get<bool>(key);
    return val;
}

std::optional<int> Config::getInt(const std::string& key) const {
    auto val = get<int>(key);
    return val;
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto val = get<double>(key);
    return val;
}

std::optional<float> Config::getFloat(const std::string& key) const {
    auto val = get<float>(key);
    return val;
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto val = get<std::string>(key);
    return val;
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setFloat(const std::string& key, float value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::validateConfig() const {
    // Basic validation: check that required keys exist and have valid values
    // This is a placeholder - actual validation would depend on application needs
    
    // Check that neuron_count is a positive integer
    auto neuronCount = getInt("brain.neuron_count");
    if (neuronCount && *neuronCount <= 0) {
        return false;
    }
    
    // Check that simulation_timestep is positive
    auto timestep = getDouble("simulation_timestep");
    if (timestep && *timestep <= 0.0) {
        return false;
    }
    
    // Add more validation rules as needed
    return true;
}

Config Config::getDefaultConfig() {
    Config config;
    
    // Neuron parameters
    config.setFloat(ConfigKeys::NEURON_MEMBRANE_CAPACITANCE, 1.0);
    config.setDouble(ConfigKeys::NEURON_TIME_CONSTANT, 20.0);
    config.setSize_t(ConfigKeys::NEURON_MAX_SPIKE_HISTORY, 100);
    config.setDouble(ConfigKeys::NEURON_RESTING_POTENTIAL, -70.0);
    config.setDouble(ConfigKeys::NEURON_THRESHOLD, -55.0);
    config.setDouble(ConfigKeys::NEURON_RESET_POTENTIAL, -70.0);
    config.setInt(ConfigKeys::NEURON_REFACTORY_PERIOD, 5);
    config.setDouble(ConfigKeys::NEURON_ADAPTATION_STRENGTH, 1.0);
    config.setSize_t(ConfigKeys::NEURON_SPIKE_HISTORY_SIZE, 100);
    
    // Plasticity parameters
    config.setDouble(ConfigKeys::PLASTICITY_STDP_LTP_WEIGHT, 0.02);
    config.setDouble(ConfigKeys::PLASTICITY_STDP_LTD_WEIGHT, 0.015);
    config.setDouble(ConfigKeys::PLASTICITY_STDP_TIME_CONSTANT, 20.0);
    
    // Development parameters
    config.setDouble(ConfigKeys::DEVELOPMENT_BASE_PLASTICITY_RATE, 0.0001);
    config.setDouble(ConfigKeys::DEVELOPMENT_CRITICAL_PERIOD_DURATION, 100.0);
    config.setDouble(ConfigKeys::DEVELOPMENT_MATURATION_RATE, 0.01);
    
    // Memory parameters
    config.setSize_t(ConfigKeys::MEMORY_Episodic_CAPACITY, 1000);
    config.setDouble(ConfigKeys::MEMORY_WORKING_DURATION, 5000.0);
    
    // Neuromodulation parameters
    config.setDouble(ConfigKeys::NEUROMODULATION_DOPAMINE_SCALE, 1.0);
    config.setDouble(ConfigKeys::NEUROMODULATION_NOVELTY_THRESHOLD, 0.5);
    
    // Brain parameters
    config.setInt("brain.neuron_count", 1000);
    config.setInt("brain.region_count", 1);
    config.setDouble("brain.connection_probability", 0.1);
    config.setDouble("brain.initial_weight_mean", 0.5);
    config.setDouble("brain.initial_weight_std", 0.1);
    config.setDouble("brain.v_thresh", -50.0);
    config.setDouble("brain.v_rest", -70.0);
    config.setDouble("brain.v_reset", -75.0);
    config.setDouble("brain.tau_mem", 20.0);
    config.setDouble("brain.tau_ref", 2.0);
    
    // Plasticity configuration
    config.setBool("plasticity.stdp.enable", true);
    config.setDouble("plasticity.stdp.learning_rate", 0.001);
    config.setDouble("plasticity.stdp.tau_plus", 20.0);
    config.setDouble("plasticity.stdp.tau_minus", 20.0);
    config.setBool("plasticity.hebbian.enable", true);
    config.setBool("plasticity.structural.enable", true);
    
    // Neuromodulation configuration
    config.setDouble("neuromod.dopamine.scale", 1.0);
    config.setBool("neuromod.curiosity.enable", true);
    config.setBool("neuromod.novelty.enable", true);
    
    // Development configuration
    config.setBool("development.enable", true);
    config.setDouble("development.maturation_rate", 0.01);
    config.setDouble("development.critical_period_end", 100.0);
    
    return config;
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