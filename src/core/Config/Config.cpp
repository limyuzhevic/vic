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
    // TODO PHASE 3: Implement proper JSON/YAML parser with schema validation
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    // Apply Phase 6 defaults for advanced configuration
    applyPhase6Defaults();
    
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

void Config::applyPhase6Defaults() {
    // Memory system configuration
    set("working_memory_capacity", 1000, ConfigSource::Default);
    set("working_memory_decay_rate", 0.05f, ConfigSource::Default);
    set("episodic_memory_max_episodes", 5000, ConfigSource::Default);
    set("episodic_memory_replay_enabled", true, ConfigSource::Default);
    set("associative_memory_strength_threshold", 0.5f, ConfigSource::Default);
    
    // Prediction system configuration
    set("prediction_enabled", true, ConfigSource::Default);
    set("prediction_confidence_threshold", 0.3f, ConfigSource::Default);
    set("prediction_memory_window", 100, ConfigSource::Default);
    set("prediction_learning_rate", 0.05f, ConfigSource::Default);
    
    // Neuromodulation configuration
    set("dopamine_enabled", true, ConfigSource::Default);
    set("dopamine_plasticity_factor", 1.0f, ConfigSource::Default);
    set("curiosity_enabled", true, ConfigSource::Default);
    set("curiosity_exploration_strength", 0.4f, ConfigSource::Default);
    set("novelty_enabled", true, ConfigSource::Default);
    set("novelty_sensitivity", 0.5f, ConfigSource::Default);
    set("prediction_error_enabled", true, ConfigSource::Default);
    set("prediction_error_gain", 1.0f, ConfigSource::Default);
    
    // Cognition system configuration
    set("cognition_enabled", true, ConfigSource::Default);
    set("attention_enabled", true, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    set("attention_focus_duration", 20, ConfigSource::Default);
    set("concept_formation_enabled", true, ConfigSource::Default);
    set("concept_formation_threshold", 0.7f, ConfigSource::Default);
    set("planner_enabled", true, ConfigSource::Default);
    set("planner_planning_depth", 5, ConfigSource::Default);
    
    // Development system configuration
    set("development_enabled", true, ConfigSource::Default);
    set("development_critical_period_start", 60.0, ConfigSource::Default);  // seconds
    set("development_critical_period_end", 300.0, ConfigSource::Default);
    set("development_maturation_duration", 900.0, ConfigSource::Default);
    set("development_plasticity_decay", 0.9f, ConfigSource::Default);
    
    // Checkpointing configuration
    set("checkpoint_enabled", true, ConfigSource::Default);
    set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    set("checkpoint_interval", 10000, ConfigSource::Default);
    set("checkpoint_max_files", 10, ConfigSource::Default);
    set("checkpoint_compression", true, ConfigSource::Default);
    set("checkpoint_version", "v1.0", ConfigSource::Default);
    
    // Advanced simulation parameters
    set("simulation_time_step", 0.001, ConfigSource::Default);
    set("simulation_stabilization_threshold", 0.01f, ConfigSource::Default);
    set("simulation_noise_level", 0.05f, ConfigSource::Default);
    set("simulation_max_steps", 100000, ConfigSource::Default);
    
    // Integration configuration
    set("sensory_memory_integration", true, ConfigSource::Default);
    set("memory_prediction_integration", true, ConfigSource::Default);
    set("prediction_action_integration", true, ConfigSource::Default);
    set("action_memory_integration", true, ConfigSource::Default);
    set("memory_development_integration", true, ConfigSource::Default);
    
    // Performance optimization
    set("performance_optimization_level", 2, ConfigSource::Default);  // 1=low, 2=balanced, 3=high
    set("simd_optimization", true, ConfigSource::Default);
    set("parallel_processing", false, ConfigSource::Default);  // Single-threaded for stability
    set("memory_pool_size", 1024, ConfigSource::Default);
    
    // Experimental features
    set("experimental_plasticity_rules", false, ConfigSource::Default);
    set("experimental_neuromodulators", false, ConfigSource::Default);
    set("experimental_memory_theories", false, ConfigSource::Default);
    
    // Logging and monitoring
    set("verbose_logging", false, ConfigSource::Default);
    set("performance_monitoring", true, ConfigSource::Default);
    set("memory_usage_tracking", true, ConfigSource::Default);
    set("spike_history_tracking", true, ConfigSource::Default);
    set("checkpoint_logging", true, ConfigSource::Default);
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
