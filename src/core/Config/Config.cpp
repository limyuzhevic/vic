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
    
    // Helper to get section from key
    static Config::Section getSectionFromKey(const std::string& key) {
        std::string lowerKey = toLower(key);
        if (lowerKey.find("neuron") != std::string::npos || lowerKey.find("region") != std::string::npos ||
            lowerKey.find("population") != std::string::npos) {
            return Config::Section::Brain;
        } else if (lowerKey.find("stdp") != std::string::npos || lowerKey.find("hebbian") != std::string::npos ||
                   lowerKey.find("plasticity") != std::string::npos) {
            return Config::Section::Plasticity;
        } else if (lowerKey.find("dopamine") != std::string::npos || lowerKey.find("curiosity") != std::string::npos ||
                   lowerKey.find("novelty") != std::string::npos || lowerKey.find("neuromod") != std::string::npos) {
            return Config::Section::Neuromodulation;
        } else if (lowerKey.find("working") != std::string::npos || lowerKey.find("episodic") != std::string::npos ||
                   lowerKey.find("associative") != std::string::npos || lowerKey.find("memory") != std::string::npos) {
            return Config::Section::Memory;
        } else if (lowerKey.find("developmental") != std::string::npos || lowerKey.find("age") != std::string::npos ||
                   lowerKey.find("maturation") != std::string::npos) {
            return Config::Section::Development;
        } else if (lowerKey.find("timestep") != std::string::npos || lowerKey.find("simulation") != std::string::npos ||
                   lowerKey.find("step") != std::string::npos) {
            return Config::Section::Simulation;
        } else if (lowerKey.find("checkpoint") != std::string::npos || lowerKey.find("save") != std::string::npos ||
                   lowerKey.find("load") != std::string::npos || lowerKey.find("performance") != std::string::npos) {
            return Config::Section::Performance;
        } else if (lowerKey.find("debug") != std::string::npos || lowerKey.find("log") != std::string::npos ||
                   lowerKey.find("verbose") != std::string::npos) {
            return Config::Section::Debug;
        }
        return Config::Section::Brain; // Default section
    }
    
    // Initialize with default values
    Config::Config() : pImpl(std::make_unique<Impl>()) {
        // Set up default configuration values
        setBrainParameters();
        setPlasticityParameters();
        setNeuromodulationParameters();
        setMemoryParameters();
        setDevelopmentParameters();
        setSimulationParameters();
        setPerformanceParameters();
        setDebugParameters();
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
                NLM_LOG_DEBUG("Loaded config from file: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Invalid config line in " + filepath + ": " + line);
            }
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from: " + filepath);
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
                    NLM_LOG_DEBUG("Loaded config from args: " + key + " = " + value);
                } else {
                    NLM_LOG_WARNING("Invalid command line argument: " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Loaded config from args: " + key + " = " + value);
            }
        }
        return true;
    }
    
    bool Config::saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save config to file: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated by NLM configuration system\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
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
                        file << arg[i];
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "  # " << static_cast<int>(entry.source) << "\n\n";
        }
        
        NLM_LOG_INFO("Saved " + std::to_string(pImpl->entries.size()) + " configuration entries to: " + filepath);
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
    
    std::vector<std::string> Config::getKeysBySection(Section section) const {
        std::vector<std::string> keys;
        for (const auto& entry : pImpl->entries) {
            Section entrySection = getSectionFromKey(entry.key);
            if (entrySection == section) {
                keys.push_back(entry.key);
            }
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
    
    std::string Config::summaryBySection(Section section) const {
        std::ostringstream oss;
        oss << "Configuration - " << static_cast<int>(section) << " (";
        oss << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
            [this, section](const ConfigEntry& entry) {
                return getSectionFromKey(entry.key) == section;
            }) << " entries):\n";
        
        for (const auto& entry : pImpl->entries) {
            Section entrySection = getSectionFromKey(entry.key);
            if (entrySection == section) {
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
                oss << "]\n";
            }
        }
        return oss.str();
    }
    
    std::vector<Config::ValidationError> Config::validate() const {
        std::vector<ValidationError> errors;
        
        for (const auto& entry : pImpl->entries) {
            if (!validate(entry.key, errors)) {
                errors.emplace_back(entry.key, "Validation failed", entry.source);
            }
        }
        
        return errors;
    }
    
    bool Config::validate(const std::string& key, std::string& error) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            error = "Key not found in configuration";
            return false;
        }
        
        // Validate based on key
        std::string lowerKey = toLower(key);
        
        if (lowerKey.find("neuron") != std::string::npos) {
            auto val = get<int64_t>(key);
            if (!val.has_value() || val.value() <= 0) {
                error = "neuron_count must be a positive integer";
                return false;
            }
        } else if (lowerKey.find("connection") != std::string::npos) {
            auto val = get<float>(key);
            if (!val.has_value() || val.value() < 0.0f || val.value() > 1.0f) {
                error = "connection_probability must be between 0.0 and 1.0";
                return false;
            }
        } else if (lowerKey.find("random_seed") != std::string::npos) {
            auto val = get<int64_t>(key);
            if (!val.has_value() || val.value() < 0) {
                error = "random_seed must be a non-negative integer";
                return false;
            }
        } else if (lowerKey.find("timestep") != std::string::npos) {
            auto val = get<double>(key);
            if (!val.has_value() || val.value() <= 0.0) {
                error = "simulation_timestep must be positive";
                return false;
            }
        }
        
        return true;
    }
    
    bool Config::validateRequired() const {
        // Check for essential configuration values
        std::vector<std::string> requiredKeys = {
            "neuron_count",
            "region_count",
            "connection_probability",
            "random_seed",
            "simulation_timestep"
        };
        
        for (const auto& key : requiredKeys) {
            if (!has(key)) {
                NLM_LOG_ERROR("Missing required configuration key: " + key);
                return false;
            }
        }
        
        return true;
    }
    
    Config::Stats Config::getStats() const {
        Stats stats;
        stats.totalEntries = pImpl->entries.size();
        
        // Initialize source counts
        for (size_t i = 0; i < 3; ++i) {
            stats.bySource[i] = 0;
        }
        for (size_t i = 0; i < 8; ++i) {
            stats.bySection[i] = 0;
        }
        
        // Count by source
        for (const auto& entry : pImpl->entries) {
            if (entry.source == ConfigSource::Default) {
                stats.bySource[0]++;
            } else if (entry.source == ConfigSource::File) {
                stats.bySource[1]++;
            } else if (entry.source == ConfigSource::CommandLine) {
                stats.bySource[2]++;
            }
        }
        
        // Count by section
        for (const auto& entry : pImpl->entries) {
            Section section = getSectionFromKey(entry.key);
            int sectionIndex = static_cast<int>(section);
            if (sectionIndex >= 0 && sectionIndex < 8) {
                stats.bySection[sectionIndex]++;
            }
        }
        
        return stats;
    }
    
    // Convenience methods for setting values
    void Config::setBrainParameters(const std::string& name) {
        set("brain.neuron_count", DEFAULT_NEURON_COUNT, ConfigSource::Default);
        set("brain.region_count", DEFAULT_REGION_COUNT, ConfigSource::Default);
        set("brain.connection_probability", DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    }
    
    void Config::setPlasticityParameters() {
        set("plasticity.stdp.enable", true, ConfigSource::Default);
        set("plasticity.stdp.learning_rate", DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
        set("plasticity.stdp.tau_plus", DEFAULT_STDP_TAU, ConfigSource::Default);
        set("plasticity.stdp.tau_minus", DEFAULT_STDP_TAU, ConfigSource::Default);
        set("plasticity.hebbian.enable", true, ConfigSource::Default);
        set("plasticity.structural.enable", true, ConfigSource::Default);
        set("plasticity.structural.synaptogenesis_rate", DEFAULT_SYNAPTONGENESIS_RATE, ConfigSource::Default);
        set("plasticity.structural.pruning_rate", DEFAULT_PRUNING_RATE, ConfigSource::Default);
    }
    
    void Config::setNeuromodulationParameters() {
        set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        set("neuromod.curiosity.enable", true, ConfigSource::Default);
        set("neuromod.novelty.enable", true, ConfigSource::Default);
    }
    
    void Config::setMemoryParameters() {
        set("memory.working.capacity", 100, ConfigSource::Default);
        set("memory.episodic.max_episodes", 1000, ConfigSource::Default);
        set("memory.associative.enabled", true, ConfigSource::Default);
    }
    
    void Config::setDevelopmentParameters() {
        set("development.initial_plasticity", 1.0f, ConfigSource::Default);
        set("development.critical_period_duration", 1000, ConfigSource::Default);
        set("development.maturation_rate", 0.5f, ConfigSource::Default);
    }
    
    void Config::setSimulationParameters() {
        set("simulation.timestep", DEFAULT_SIMULATION_TIMESTEP, ConfigSource::Default);
        set("simulation.random_seed", DEFAULT_RANDOM_SEED, ConfigSource::Default);
        set("simulation.replay_interval", DEFAULT_REPLAY_INTERVAL, ConfigSource::Default);
        set("simulation.consolidation_interval", DEFAULT_CONSOLIDATION_INTERVAL, ConfigSource::Default);
    }
    
    void Config::setPerformanceParameters() {
        set("performance.checkpoint.max_size", DEFAULT_CHECKPOINT_MAX_SIZE, ConfigSource::Default);
        set("performance.checkpoint.history", DEFAULT_CHECKPOINT_HISTORY, ConfigSource::Default);
        set("performance.checkpoint.compress", DEFAULT_CHECKPOINT_COMPRESS, ConfigSource::Default);
    }
    
    void Config::setDebugParameters() {
        set("debug.log_level", 2, ConfigSource::Default);
        set("debug.enable_profiling", false, ConfigSource::Default);
        set("debug.enable_validation", true, ConfigSource::Default);
    }
    
    // Template implementations
    template<typename T>
    std::optional<T> Config::get(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
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

