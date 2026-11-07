#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced file loading with multiple format support
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    // Try to detect file format by reading first line
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    bool success = false;
    
    // Try JSON format first (starts with { or [
    if (firstLine.find('{') == 0 || firstLine.find('[') == 0) {
        // TODO: Implement JSON parsing (would require nlohmann/json or similar)
        NLM_LOG_WARNING("JSON config files not yet implemented, using simple key=value format");
    }
    
    // Fall back to simple key=value format
    success = loadSimpleConfig(file);
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    } else {
        NLM_LOG_WARNING("Failed to load configuration from: " + filepath);
    }
    
    return success;
}

bool Config::loadSimpleConfig(std::ifstream& file) {
    clear(); // Clear any existing entries
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNum++;
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
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
            success = false;
        }
    }
    
    return success;
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
        NLM_LOG_ERROR("Cannot save config to file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File (Generated)\n";
    file << "# Format: key = value\n\n";
    
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
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added config key: " + key);
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
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it, pImpl->entries.end());
        NLM_LOG_DEBUG("Removed config key: " + key);
    }
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
    NLM_LOG_DEBUG("Cleared all configuration entries");
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

// Validate configuration parameters
bool Config::validate() const {
    NLM_LOG_INFO("Validating configuration...");
    
    bool isValid = true;
    
    // Check required parameters
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "simulation_timestep",
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_ERROR("Missing required configuration key: " + key);
            isValid = false;
        }
    }
    
    // Validate parameter ranges
    if (has("neuron_count")) {
        auto neuronCount = get<size_t>("neuron_count");
        if (neuronCount.has_value()) {
            if (neuronCount.value() < 1 || neuronCount.value() > 100000) {
                NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount.value()) + ". Must be between 1 and 100000");
                isValid = false;
            }
        }
    }
    
    if (has("region_count")) {
        auto regionCount = get<size_t>("region_count");
        if (regionCount.has_value()) {
            if (regionCount.value() < 1 || regionCount.value() > 1000) {
                NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount.value()) + ". Must be between 1 and 1000");
                isValid = false;
            }
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep.has_value()) {
            if (timestep.value() <= 0.0 || timestep.value() > 1.0) {
                NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep.value()) + ". Must be between 0.0001 and 1.0");
                isValid = false;
            }
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = get<double>("connection_probability");
        if (connProb.has_value()) {
            if (connProb.value() < 0.0 || connProb.value() > 1.0) {
                NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connProb.value()) + ". Must be between 0.0 and 1.0");
                isValid = false;
            }
        }
    }
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    return isValid;
}

// Get configuration parameter info for help
std::string Config::getParameterInfo(const std::string& key) const {
    static const std::unordered_map<std::string, std::string> parameterInfo = {
        {"neuron_count", "Number of neurons in the brain (int, 1-100000)"},
        {"region_count", "Number of brain regions (int, 1-1000)"},
        {"simulation_timestep", "Time step size in seconds (double, 0.0001-1.0)"},
        {"connection_probability", "Probability of forming random connections (float, 0.0-1.0)"},
        {"random_seed", "Random seed for reproducible experiments (int64)"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight (float)"},
        {"stdp_ltd_weight", "STDP long-term depression weight (float)"},
        {"stdp_tau", "STDP time constant (float, ms)"},
        {"hebbian_learning_rate", "Hebbian plasticity learning rate (float)"},
        {"hebbian_max_weight", "Maximum synaptic weight for Hebbian (float, 0.0-1.0)"},
        {"hebbian_min_weight", "Minimum synaptic weight for Hebbian (float, 0.0-1.0)"},
        {"synaptogenesis_rate", "Structural plasticity synaptogenesis rate (float)"},
        {"pruning_rate", "Structural plasticity pruning rate (float)"},
        {"dopamine_baseline", "Neuromodulation dopamine baseline level (float, -1.0-1.0)"},
        {"dopamine_decay_rate", "Neuromodulation dopamine decay rate (float, 0.0-1.0)"},
        {"reward_discount_factor", "Reward discount factor for future rewards (float, 0.0-1.0)"},
        {"working_memory_capacity", "Working memory capacity in slots (int, 1-1000)"},
        {"episodic_memory_max_duration", "Episodic memory maximum duration in steps (int, 1-1000000)"},
        {"prediction_system_horizon", "Prediction system lookahead horizon (int, 1-1000)"},
        {"concept_formation_min_examples", "Minimum examples for concept formation (int, 1-100)"},
        {"novelty_threshold", "Novelty detection threshold (float, 0.0-1.0)"},
        {"curiosity_baseline", "Curiosity baseline signal (float, 0.0-1.0)"},
        {"development_max_iterations", "Development system maximum iterations (int, 1-1000000)"},
        {"pruning_threshold", "Structural plasticity pruning threshold (float, 0.0-1.0)"},
        {"maturation_rate", "Neural maturation rate (float, 0.0-1.0)"},
        {"synaptogenesis_max_connections", "Maximum connections per neuron (int, 1-10000)"},
        {"lif_resting_potential", "LIF neuron resting potential (float, -1.0-1.0)"},
        {"lif_threshold_potential", "LIF neuron threshold potential (float, -1.0-1.0)"},
        {"lif_refractory_period", "LIF neuron refractory period (float, 0.0-0.1)"},
        {"synaptic_delay_min", "Synaptic transmission minimum delay (float, 0.0-0.1)"},
        {"synaptic_delay_max", "Synaptic transmission maximum delay (float, 0.0-0.1)"},
        {"stdp_max_weight_change", "Maximum STDP weight change (float)"},
        {"plasticity_stddev", "Plasticity update standard deviation (float)"},
        {"novelty_decay_rate", "Novelty signal decay rate (float, 0.0-1.0)"},
        {"curiosity_decay_rate", "Curiosity signal decay rate (float, 0.0-1.0)"},
        {"reward_prediction_error_decay", "RPE signal decay rate (float, 0.0-1.0)"}
    };
    
    auto it = parameterInfo.find(key);
    if (it != parameterInfo.end()) {
        return it->second;
    }
    return "Unknown parameter: " + key;
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
