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
    
    file << "# Configuration saved by NLM\n";
    file << "# Format: key = value (value type depends on content)\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Save the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, float> || std::is_same_v<T, double> || 
                         std::is_same_v<T, bool>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << '"' << arg[i] << '"';
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
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

// Add advanced configuration implementation here

std::shared_ptr<Config> Config::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Core simulation parameters
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Development parameters
    config->set("developmental_level", 0, ConfigSource::Default);
    config->set("plasticity_rate", 1.0f, ConfigSource::Default);
    
    // Memory parameters
    config->set("working_memory_capacity", 100, ConfigSource::Default);
    config->set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    config->set("replay_interval", 100, ConfigSource::Default);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Neuromodulation parameters
    config->set("dopamine_initial_level", 0.5f, ConfigSource::Default);
    config->set("curiosity_initial_level", 0.5f, ConfigSource::Default);
    config->set("novelty_sensitivity", 0.5f, ConfigSource::Default);
    
    // Cognitive parameters
    config->set("planning_depth", 5, ConfigSource::Default);
    config->set("attention_strength", 0.5f, ConfigSource::Default);
    
    // Environment parameters
    config->set("action_space_size", 4, ConfigSource::Default);
    config->set("reward_function", "cumulative", ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> Config::createPhase2Config() {
    auto config = createDefaultConfig();
    
    // Phase 2 specific overrides
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> Config::createPhase6Config() {
    auto config = createDefaultConfig();
    
    // Phase 6 integration overrides
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.08f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.01f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
    config->set("pruning_rate", 0.00005f, ConfigSource::Default);
    
    // Enable Phase 6 specific features
    config->set("enable_checkpointing", true, ConfigSource::Default);
    config->set("enable_replay", true, ConfigSource::Default);
    config->set("enable_development", true, ConfigSource::Default);
    
    return config;
}

std::string Config::getAvailableConfigKeys() {
    std::string keys = "Available configuration keys:\n";
    keys += "Core simulation:\n";
    keys += "  neuron_count, region_count, connection_probability, simulation_timestep, random_seed\n";
    keys += "Plasticity:\n";
    keys += "  stdp_ltp_weight, stdp_ltd_weight, stdp_tau, synaptogenesis_rate, pruning_rate\n";
    keys += "Development:\n";
    keys += "  developmental_level, plasticity_rate\n";
    keys += "Memory:\n";
    keys += "  working_memory_capacity, episodic_memory_max_episodes, replay_interval, consolidation_interval\n";
    keys += "Neuromodulation:\n";
    keys += "  dopamine_initial_level, curiosity_initial_level, novelty_sensitivity\n";
    keys += "Cognition:\n";
    keys += "  planning_depth, attention_strength\n";
    keys += "Environment:\n";
    keys += "  action_space_size, reward_function\n";
    keys += "\nUse --help for more details on command-line arguments.\n";
    return keys;
}

std::string Config::getConfigHelp() const {
    std::string help = "NLM Configuration Help:\n";
    help += "===================\n\n";
    help += "The NLM framework uses a key=value configuration format.\n\n";
    help += "Basic format:\n";
    help += "  --key=value        Command line argument\n";
    help += "  config file:\n";
    help += "    key = value      Configuration file line\n\n";
    help += "Data types:\n";
    help += "  int, int64, float, double, bool, string\n";
    help += "  vectors: [1,2,3], \"value1\", \"value2\", ...\n\n";
    help += "Common keys:\n";
    help += "  --neuron_count=N    Number of neurons (default: 1000)\n";
    help += "  --region_count=N    Number of brain regions (default: 1)\n";
    help += "  --config=file.cfg   Load configuration from file\n";
    help += "  --help              Show this help\n\n";
    help += "Examples:\n";
    help += "  --neuron_count=500 --region_count=2\n";
    help += "  --stdp_ltp_weight=0.02 --stdp_ltd_weight= veterinarian\n\n";
    help += "To see all available keys, use --keys.\n";
    return help;
}

} // namespace nlm
