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

// Add validation methods to Config.cpp

// Validate the configuration
bool Config::validate() const {
    bool isValid = true;
    
    // Validate required fields
    if (!has("random_seed")) {
        NLM_LOG_WARNING("Configuration missing required field: random_seed");
        isValid = false;
    } else {
        auto seedOpt = get<int64_t>("random_seed");
        if (seedOpt && *seedOpt < 0) {
            NLM_LOG_ERROR("Invalid random_seed: must be non-negative");
            isValid = false;
        }
    }
    
    if (!has("simulation_timestep")) {
        NLM_LOG_WARNING("Configuration missing required field: simulation_timestep");
        isValid = false;
    } else {
        auto timestepOpt = get<double>("simulation_timestep");
        if (timestepOpt && *timestepOpt <= 0.0) {
            NLM_LOG_ERROR("Invalid simulation_timestep: must be positive");
            isValid = false;
        }
    }
    
    if (!has("neuron_count")) {
        NLM_LOG_WARNING("Configuration missing required field: neuron_count");
        isValid = false;
    } else {
        auto neuronsOpt = get<size_t>("neuron_count");
        if (neuronsOpt && *neuronsOpt < 1) {
            NLM_LOG_ERROR("Invalid neuron_count: must be at least 1");
            isValid = false;
        }
    }
    
    if (!has("region_count")) {
        NLM_LOG_WARNING("Configuration missing required field: region_count");
        isValid = false;
    } else {
        auto regionsOpt = get<size_t>("region_count");
        if (regionsOpt && *regionsOpt < 1) {
            NLM_LOG_ERROR("Invalid region_count: must be at least 1");
            isValid = false;
        }
    }
    
    if (!has("connection_probability")) {
        NLM_LOG_WARNING("Configuration missing required field: connection_probability");
        isValid = false;
    } else {
        auto probOpt = get<float>("connection_probability");
        if (probOpt && (*probOpt < 0.0f || *probOpt > 1.0f)) {
            NLM_LOG_ERROR("Invalid connection_probability: must be between 0.0 and 1.0");
            isValid = false;
        }
    }
    
    // Validate consistency between neurons and regions
    auto neuronsOpt = get<size_t>("neuron_count");
    auto regionsOpt = get<size_t>("region_count");
    if (neuronsOpt && regionsOpt && *neuronsOpt < *regionsOpt) {
        NLM_LOG_ERROR("Invalid configuration: neuron_count (%zu) must be >= region_count (%zu)", 
                     *neuronsOpt, *regionsOpt);
        isValid = false;
    }
    
    // Validate optional fields with reasonable ranges
    if (has("stdp_ltp_weight")) {
        auto ltpOpt = get<float>("stdp_ltp_weight");
        if (ltpOpt && *ltpOpt < 0.0f) {
            NLM_LOG_ERROR("Invalid stdp_ltp_weight: must be non-negative");
            isValid = false;
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto ltdOpt = get<float>("stdp_ltd_weight");
        if (ltdOpt && *ltdOpt < 0.0f) {
            NLM_LOG_ERROR("Invalid stdp_ltd_weight: must be non-negative");
            isValid = false;
        }
    }
    
    if (has("stdp_time_constant")) {
        auto tauOpt = get<float>("stdp_time_constant");
        if (tauOpt && *tauOpt <= 0.0f) {
            NLM_LOG_ERROR("Invalid stdp_time_constant: must be positive");
            isValid = false;
        }
    }
    
    if (has("environment_width")) {
        auto widthOpt = get<size_t>("environment_width");
        if (widthOpt && *widthOpt < 1) {
            NLM_LOG_ERROR("Invalid environment_width: must be at least 1");
            isValid = false;
        }
    }
    
    if (has("environment_height")) {
        auto heightOpt = get<size_t>("environment_height");
        if (heightOpt && *heightOpt < 1) {
            NLM_LOG_ERROR("Invalid environment_height: must be at least 1");
            isValid = false;
        }
    }
    
    // Validate that development rates are non-negative
    if (has("development_synaptogenesis_rate")) {
        auto synOpt = get<float>("development_synaptogenesis_rate");
        if (synOpt && *synOpt < 0.0f) {
            NLM_LOG_ERROR("Invalid development_synaptogenesis_rate: must be non-negative");
            isValid = false;
        }
    }
    
    if (has("development_pruning_rate")) {
        auto pruneOpt = get<float>("development_pruning_rate");
        if (pruneOpt && *pruneOpt < 0.0f) {
            NLM_LOG_ERROR("Invalid development_pruning_rate: must be non-negative");
            isValid = false;
        }
    }
    
    return isValid;
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

    // Apply validation callback if set
    bool Config::validateWithCallback() const {
        if (validationCallback) {
            return validationCallback(*this);
        }
        return validate();
    }

} // namespace nlm
