#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Helper for quick lookups
    std::unordered_map<std::string, size_t> keyToIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Static helper function implementations
Config Config::createDefaultConfig() {
    Config config;
    
    // Core brain parameters
    config.set("brain.neuron_count", 1000, ConfigSource::Default);
    config.set("brain.region_count", 1, ConfigSource::Default);
    config.set("brain.connection_probability", 0.1f, ConfigSource::Default);
    config.set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
    config.set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
    config.set("brain.v_thresh", -50.0f, ConfigSource::Default);
    config.set("brain.v_rest", -70.0f, ConfigSource::Default);
    config.set("brain.v_reset", -75.0f, ConfigSource::Default);
    config.set("brain.tau_mem", 20.0f, ConfigSource::Default);
    config.set("brain.tau_ref", 2.0f, ConfigSource::Default);
    
    // Plasticity parameters
    config.set("plasticity.stdp.enable", true, ConfigSource::Default);
    config.set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config.set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config.set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    config.set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config.set("plasticity.structural.enable", true, ConfigSource::Default);
    config.set("plasticity.synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("plasticity.pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation parameters
    config.set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config.set("neuromod.curiosity.noise_level", 0.1f, ConfigSource::Default);
    config.set("neuromod.novelty.enable", true, ConfigSource::Default);
    config.set("neuromod.novelty.sensitivity", 0.5f, ConfigSource::Default);
    
    // Prediction system parameters
    config.set("prediction.error_threshold", 0.1f, ConfigSource::Default);
    config.set("prediction.confidence_decay", 0.99f, ConfigSource::Default);
    
    // Memory system parameters
    config.set("memory.working.capacity", 1000, ConfigSource::Default);
    config.set("memory.working.decay_rate", 0.01f, ConfigSource::Default);
    config.set("memory.episodic.max_episodes", 1000, ConfigSource::Default);
    config.set("memory.associative.max_associations", 10000, ConfigSource::Default);
    
    // Development parameters
    config.set("development.initial_stage_age", 60.0f, ConfigSource::Default);
    config.set("development.critical_period_length", 300.0f, ConfigSource::Default);
    config.set("development.maturation_length", 600.0f, ConfigSource::Default);
    config.set("development.plasticity_decrease_rate", 0.1f, ConfigSource::Default);
    
    // Simulation parameters
    config.set("simulation.timestep", 0.001, ConfigSource::Default);
    config.set("simulation.max_steps", 100000, ConfigSource::Default);
    config.set("simulation.random_seed", 42, ConfigSource::Default);
    
    // Curiosity and exploration parameters
    config.set("curiosity.base_level", 0.1f, ConfigSource::Default);
    config.set("curiosity.exploration_bonus", 0.5f, ConfigSource::Default);
    config.set("curiosity.exploration_decay", 0.95f, ConfigSource::Default);
    
    // Agent behavior parameters
    config.set("agent.action_selection.exploration_rate", 0.1f, ConfigSource::Default);
    config.set("agent.action_selection.epsilon", 0.01f, ConfigSource::Default);
    config.set("agent.reward.clip_min", -10.0f, ConfigSource::Default);
    config.set("agent.reward.clip_max", 10.0f, ConfigSource::Default);
    
    // World parameters
    config.set("world.width", 20.0f, ConfigSource::Default);
    config.set("world.height", 20.0f, ConfigSource::Default);
    config.set("world.vision_range", 8.0f, ConfigSource::Default);
    config.set("world.max_energy", 100.0f, ConfigSource::Default);
    config.set("world.energy_decay_rate", 0.01f, ConfigSource::Default);
    
    // Checkpoint parameters
    config.set("checkpoint.enabled", true, ConfigSource::Default);
    config.set("checkpoint.save_interval", 10000, ConfigSource::Default);
    config.set("checkpoint.max_checkpoints", 10, ConfigSource::Default);
    config.set("checkpoint.compress", true, ConfigSource::Default);
    
    return config;
}

std::string Config::getConfigSchema() {
    return R"(
# NLM Configuration Schema
# ---------------------------

# Core Brain Configuration
brain.neuron_count = 1000                    # Total number of neurons (int)
brain.region_count = 1                     # Number of brain regions (int)
brain.connection_probability = 0.1          # Probability of synaptic connections (float)
brain.initial_weight_mean = 0.5             # Mean initial synaptic weight (float)
brain.initial_weight_std = 0.1              # Standard deviation of initial weights (float)

# Neuron Dynamics
brain.v_thresh = -50.0                      # Neuron threshold potential (mV)
brain.v_rest = -70.0                        # Resting potential (mV)
brain.v_reset = -75.0                       # Reset potential after spike (mV)
brain.tau_mem = 20.0                        # Membrane time constant (ms)
brain.tau_ref = 2.0                         # Refractory period (ms)

# Plasticity Configuration
plasticity.stdp.enable = true                # Enable STDP plasticity (bool)
plasticity.stdp.learning_rate = 0.001        # STDP learning rate (float)
plasticity.stdp.tau_plus = 20.0              # STDP potentiation time constant (ms)
plasticity.stdp.tau_minus = 20.0             # STDP depression time constant (ms)
plasticity.hebbian.enable = true             # Enable Hebbian plasticity (bool)
plasticity.structural.enable = true          # Enable structural plasticity (bool)
plasticity.synaptogenesis_rate = 0.0001      # Rate of new synapse formation (float)
plasticity.pruning_rate = 0.00001            # Rate of synapse pruning (float)

# Neuromodulation Configuration
neuromod.dopamine.scale = 1.0                # Dopamine modulation scale (float)
neuromod.curiosity.enable = true             # Enable curiosity-driven exploration (bool)
neuromod.curiosity.noise_level = 0.1         # Noise level in curiosity system (float)
neuromod.novelty.enable = true               # Enable novelty detection (bool)
neuromod.novelty.sensitivity = 0.5           # Novelty sensitivity (float)

# Prediction System
prediction.error_threshold = 0.1             # Prediction error threshold (float)
prediction.confidence_decay = 0.99            # Confidence decay rate (float)

# Memory Systems
memory.working.capacity = 1000               # Working memory capacity (int)
memory.working.decay_rate = 0.01             # Working memory decay rate (float)
memory.episodic.max_episodes = 1000          # Maximum number of episodic memories (int)
memory.associative.max_associations = 10000  # Maximum number of associations (int)

# Development System
development.initial_stage_age = 60.0         # Initial stage duration (float)
development.critical_period_length = 300.0   # Critical period length (float)
development.maturation_length = 600.0        # Maturation stage length (float)
development.plasticity_decrease_rate = 0.1  # Rate of plasticity decrease (float)

# Simulation Parameters
simulation.timestep = 0.001                  # Simulation timestep (float)
simulation.max_steps = 100000                # Maximum simulation steps (int)
simulation.random_seed = 42                  # Random seed for reproducibility (int)

# Agent Behavior
agent.action_selection.exploration_rate = 0.1 # Exploration rate (float)
agent.action_selection.epsilon = 0.01       # Epsilon for action selection (float)
agent.reward.clip_min = -10.0                # Minimum reward value (float)
agent.reward.clip_max = 10.0                 # Maximum reward value (float)

# World Parameters
world.width = 20.0                           # World width (float)
world.height = 20.0                          # World height (float)
world.vision_range = 8.0                    # Agent vision range (float)
world.max_energy = 100.0                    # Maximum agent energy (float)
world.energy_decay_rate = 0.01              # Energy decay rate (float)

# Checkpoint System
checkpoint.enabled = true                    # Enable checkpointing (bool)
checkpoint.save_interval = 10000            # Checkpoint save interval (int)
checkpoint.max_checkpoints = 10              # Maximum number of checkpoints (int)
checkpoint.compress = true                   # Compress checkpoints (bool)

# Example config file format:
# neuron_count = 1000
# region_count = 1
# connection_probability = 0.1
# stdp.enable = true
# stdp.learning_rate = 0.001
# ...
";
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear existing entries
    clear();
    
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
        file << entry.key << " = ";
        
        // Convert value to string based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
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
    auto it = pImpl->keyToIndex.find(key);
    
    if (it != pImpl->keyToIndex.end()) {
        // Update existing entry
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
        }
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
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
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it == pImpl->keyToIndex.end()) {
        return;
    }
    
    size_t index = it->second;
    
    // Remove from entries vector
    pImpl->entries.erase(pImpl->entries.begin() + index);
    
    // Update indices in keyToIndex
    for (auto& pair : pImpl->keyToIndex) {
        if (pair.second > index) {
            pair.second--;
        }
    }
    
    // Remove from keyToIndex
    pImpl->keyToIndex.erase(it);
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
    pImpl->keyToIndex.clear();
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