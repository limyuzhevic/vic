#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

namespace nlm {

// Validation functions
namespace {

// Validate and clamp simulation timestep
bool validateTimestep(double timestep) {
    return timestep > 0.0 && timestep <= 1.0; // Max 1 second timestep
}

// Validate neuron count
bool validateNeuronCount(int64_t count) {
    return count >= 10 && count <= 10000; // Reasonable range
}

// Validate connection probability
bool validateConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;
}

// Validate plasticity parameters
bool validatePlasticityParam(float param, float min, float max) {
    return param >= min && param <= max;
}

// Validate string key (should not be empty and contain valid characters)
bool validateKey(const std::string& key) {
    if (key.empty()) return false;
    // Check for whitespace or special characters that could cause parsing issues
    for (char c : key) {
        if (std::isspace(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

} // anonymous namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validationEnabled;  // Flag to enable/disable validation
    
    Impl() : entries(), validationEnabled(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with NLM-specific sensible defaults
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);  // 1ms timestep
    set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory parameters
    set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    set("episodic_memory_max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
    
    // Neuromodulation parameters
    set("dopamine_baseline", 0.0f, ConfigSource::Default);
    set("dopamine_peak", 1.0f, ConfigSource::Default);
    set("ach_baseline", 0.1f, ConfigSource::Default);
    set("ach_peak", 1.0f, ConfigSource::Default);
    set("ne_baseline", 0.05f, ConfigSource::Default);
    set("ne_peak", 1.0f, ConfigSource::Default);
    set("5ht_baseline", 0.1f, ConfigSource::Default);
    set("5ht_peak", 1.0f, ConfigSource::Default);
    
    NLM_LOG_INFO("Configuration initialized with NLM defaults");
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
            if (value.size() >= 2) {
                if ((value.front() == '\"' && value.back() == '\"') ||
                    (value.front() == '\'' && value.back() == '\'')) {
                    value = value.substr(1, value.size() - 2);
                }
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

void Config::visitAndSave(const ConfigEntry& entry, std::ostream& file) const {
    file << "# " << entry.description << "\n";
    file << entry.key << " = ";
    
    // Save configuration value with proper formatting
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
                if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "\"" << arg[i] << "\"";
                } else {
                    file << arg[i];
                }
            }
            file << "]";
        } else if constexpr (std::is_same_v<T, bool>) {
            file << (arg ? "true" : "false");
        } else {
            file << arg;
        }
    }, entry.value);
    
    file << "\n";
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (!validateKey(key)) {
        NLM_LOG_WARNING("Invalid configuration key: '" + key + "'. Keys must not be empty and must not contain whitespace.");
        return;
    }
    
    // Apply validation based on key
    if (pImpl->validationEnabled) {
        // Validate specific keys
        if (key == "simulation_timestep") {
            double timestep = std::get<double>(value);
            if (!validateTimestep(timestep)) {
                NLM_LOG_WARNING("Invalid simulation_timestep: " + std::to_string(timestep) + ". Must be > 0 and <= 1.0.");
                return;
            }
        } else if (key == "neuron_count") {
            int64_t count = std::get<int64_t>(value);
            if (!validateNeuronCount(count)) {
                NLM_LOG_WARNING("Invalid neuron_count: " + std::to_string(count) + ". Must be >= 10 and <= 10000.");
                return;
            }
        } else if (key == "connection_probability") {
            float prob = std::get<float>(value);
            if (!validateConnectionProbability(prob)) {
                NLM_LOG_WARNING("Invalid connection_probability: " + std::to_string(prob) + ". Must be >= 0.0 and <= 1.0.");
                return;
            }
        } else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
            float weight = std::get<float>(value);
            if (!validatePlasticityParam(weight, 0.0f, 1.0f)) {
                NLM_LOG_WARNING("Invalid plasticity parameter " + key + ": " + std::to_string(weight) + ". Must be >= 0.0 and <= 1.0.");
                return;
            }
        } else if (key == "stdp_tau") {
            float tau = std::get<float>(value);
            if (tau <= 0.0f) {
                NLM_LOG_WARNING("Invalid stdp_tau: " + std::to_string(tau) + ". Must be > 0.");
                return;
            }
        } else if (key == "synaptogenesis_rate" || key == "pruning_rate") {
            float rate = std::get<float>(value);
            if (rate < 0.0f || rate > 0.01f) {
                NLM_LOG_WARNING("Invalid plasticity rate " + key + ": " + std::to_string(rate) + ". Must be >= 0.0 and <= 0.01.");
                return;
            }
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated configuration: " + key + " = " + "[value]");
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added configuration: " + key + " = " + "[value]");
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
