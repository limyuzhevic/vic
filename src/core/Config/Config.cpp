#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set default configuration values
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", 1000, ConfigSource::Default);
    set("region_count", 1, ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("random_seed", uint64_t(42), ConfigSource::Default);
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    set("replay_interval", 100, ConfigSource::Default);
    set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Memory system defaults
    set("working_memory_capacity", 100, ConfigSource::Default);
    set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    
    // Neuromodulation defaults
    set("dopamine_level", 0.5f, ConfigSource::Default);
    set("curiosity_strength", 0.5f, ConfigSource::Default);
    
    // Performance and limits
    set("max_neurons_per_region", 1000, ConfigSource::Default);
    set("max_synapses_per_neuron", 1000, ConfigSource::Default);
    set("max_total_synapses", 1000000, ConfigSource::Default);
    
    // Processing thresholds
    set("spike_history_max_length", 100, ConfigSource::Default);
    set("eligibility_trace_decay_rate", 0.001f, ConfigSource::Default);
    set("efficacy_decay_rate", 0.01f, ConfigSource::Default);
    
    // Developmental stages
    set("stage_initial_duration", 60, ConfigSource::Default);
    set("stage_critical_period_duration", 300, ConfigSource::Default);
    set("stage_maturation_duration", 600, ConfigSource::Default);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
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
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
        NLM_LOG_DEBUG("Loaded config: " + key + " = " + value);
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
        file << entry.key << " = ";
        
        // Handle different value types
        try {
            // Try int
            file << std::get<int>(entry.value);
        } catch (const std::bad_variant_access&) {
            try {
                // Try int64_t
                file << std::get<int64_t>(entry.value);
            } catch (const std::bad_variant_access&) {
                try {
                    // Try double
                    file << std::get<double>(entry.value);
                } catch (const std::bad_variant_access&) {
                    try {
                        // Try bool
                        file << (std::get<bool>(entry.value) ? "true" : "false");
                    } catch (const std::bad_variant_access&) {
                        try {
                            // Try std::string
                            file << "\"" << std::get<std::string>(entry.value) << "\"";
                        } catch (const std::bad_variant_access&) {
                            try {
                                // Try std::vector<int>
                                const auto& vec = std::get<std::vector<int>>(entry.value);
                                file << "[";
                                for (size_t i = 0; i < vec.size(); ++i) {
                                    if (i > 0) file << ", ";
                                    file << vec[i];
                                }
                                file << "]";
                            } catch (const std::bad_variant_access&) {
                                try {
                                    // Try std::vector<double>
                                    const auto& vec = std::get<std::vector<double>>(entry.value);
                                    file << "[";
                                    for (size_t i = 0; i < vec.size(); ++i) {
                                        if (i > 0) file << ", ";
                                        file << vec[i];
                                    }
                                    file << "]";
                                } catch (const std::bad_variant_access&) {
                                    // Try std::vector<std::string>
                                    const auto& vec = std::get<std::vector<std::string>>(entry.value);
                                    file << "[";
                                    for (size_t i = 0; i < vec.size(); ++i) {
                                        if (i > 0) file << ", ";
                                        file << "\"" << vec[i] << "\"";
                                    }
                                    file << "]";
                                }
                            }
                        }
                    }
                }
            }
        }
        
        file << "\n\n";
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
