#include <iomanip>
#include <sstream>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    setDefaultValues();
}

void Config::setDefaultValues() {
    // Neural system configuration
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default, 
        "Total number of neurons in the brain");
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default,
        "Number of neural regions (multiple regions enable specialized processing)");
    set("connection_probability", 0.1f, ConfigSource::Default,
        "Probability of synapse formation between neurons (0.0 to 1.0)");
    set("simulation_timestep", 0.001, ConfigSource::Default,
        "Time step duration for simulation in seconds");
    set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default,
        "Maximum number of simulation steps before auto-termination");
    
    // Neuron dynamics parameters
    set("neuron_threshold", 1.0f, ConfigSource::Default,
        "Membrane potential threshold for spike generation");
    set("neuron_leak_conductance", 0.01f, ConfigSource::Default,
        "Membrane leak conductance (how quickly potential returns to resting state)");
    set("neuron_reset_potential", 0.0f, ConfigSource::Default,
        "Membrane potential to which neuron resets after spike");
    set("neuron_refractory_period", 0.005, ConfigSource::Default,
        "Refractory period duration after spike in seconds");
    
    // Plasticity parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default,
        "STDP Long-Term Potentiation weight factor");
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default,
        "STDP Long-Term Depression weight factor");
    set("stdp_tau", 20.0f, ConfigSource::Default,
        "STDP trace decay time constant (seconds)");
    set("hebbian_learning_rate", 0.001f, ConfigSource::Default,
        "Hebbian learning rate for co-activation strengthening");
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default,
        "Rate of new synapse formation (synaptogenesis)");
    set("pruning_rate", 0.00001f, ConfigSource::Default,
        "Rate of synapse elimination (pruning)");
    set("max_synapses_per_neuron", static_cast<size_t>(100), ConfigSource::Default,
        "Maximum number of synapses a neuron can form");
    
    // Neuromodulation parameters
    set("dopamine_effect_strength", 0.5f, ConfigSource::Default,
        "Strength of dopamine neuromodulation effects");
    set("curiosity_threshold", 0.3f, ConfigSource::Default,
        "Novelty threshold for curiosity-driven exploration");
    set("novelty_decay_rate", 0.99f, ConfigSource::Default,
        "Rate at which novelty signals decay over time");
    
    // Memory system parameters
    set("working_memory_capacity", static_cast<size_t>(100), ConfigSource::Default,
        "Capacity of working memory (number of active traces)");
    set("episodic_memory_max_episodes", static_cast<size_t>(1000), ConfigSource::Default,
        "Maximum number of episodes stored in episodic memory");
    set("consolidation_threshold", 0.3f, ConfigSource::Default,
        "Threshold for memory consolidation (weaker memories are dropped)");
    
    // Development parameters
    set("development_enabled", true, ConfigSource::Default,
        "Whether the brain should undergo developmental changes over time");
    set("plasticity_modifier_initial", 1.0f, ConfigSource::Default,
        "Initial plasticity modifier (higher = more plastic)");
    set("plasticity_modifier_adult", 0.2f, ConfigSource::Default,
        "Adult plasticity modifier (lower = more stable)");
    
    // Agent behavior parameters
    set("exploration_rate", 0.1f, ConfigSource::Default,
        "Default exploration rate for action selection (0.0 to 1.0)");
    set("reward_discount_factor", 0.95, ConfigSource::Default,
        "Discount factor for future rewards in reinforcement learning");
    
    // Performance and checkpointing
    set("checkpoint_dir", "./checkpoints", ConfigSource::Default,
        "Directory for saving brain state checkpoints");
    set("checkpoint_interval", static_cast<size_t>(1000), ConfigSource::Default,
        "Number of simulation steps between automatic checkpoints");
    set("max_checkpoints", static_cast<size_t>(10), ConfigSource::Default,
        "Maximum number of checkpoints to retain");
    
    // Debugging and logging
    set("log_level", "Info", ConfigSource::Default,
        "Logging verbosity level: Debug, Info, Warning, Error, Critical");
    set("enable_profiling", false, ConfigSource::Default,
        "Whether to enable performance profiling and statistics");
    set("debug_mode", false, ConfigSource::Default,
        "Enable debug output and additional validation checks");
    
    // Environment and world simulation
    set("world_width", static_cast<size_t>(20), ConfigSource::Default,
        "Width of the simulated world in cells");
    set("world_height", static_cast<size_t>(20), ConfigSource::Default,
        "Height of the simulated world in cells");
    set("vision_range", static_cast<size_t>(8), ConfigSource::Default,
        "Visual perception range (how many cells an agent can see)");
    
    // Random seed
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default,
        "Random seed for reproducible simulations (use -1 for random)");
}

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Try to parse as JSON
    try {
        // For now, implement simple JSON parsing for common NLM config patterns
        // In a production system, we'd use a proper JSON library like nlohmann/json
        if (content.find("{") != std::string::npos && content.find("}") != std::string::npos) {
            // Simple JSON parsing for key-value pairs
            size_t pos = 0;
            while (pos < content.size()) {
                // Find next key
                size_t keyStart = content.find('"', pos);
                if (keyStart == std::string::npos) break;
                size_t keyEnd = content.find('"', keyStart + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
                size_t colonPos = content.find(':', keyEnd);
                if (colonPos == std::string::npos) break;
                
                // Find value (skip whitespace)
                size_t valueStart = content.find_first_not_of(" \t\n\r", colonPos + 1);
                if (valueStart == std::string::npos) break;
                
                // Handle different value types
                if (content[valueStart] == '\"') {
                    // String value
                    size_t valueEnd = content.find('\"', valueStart + 1);
                    if (valueEnd == std::string::npos) break;
                    std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                    pos = valueEnd + 1;
                    set(key, value, ConfigSource::File);
                } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
                    // Boolean value
                    size_t valueEnd = content.find_first_of(",}\n\r", valueStart);
                    std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
                    bool value = (valueStr == "true" || valueStr == "True" || valueStr == "TRUE");
                    pos = valueEnd;
                    set(key, value, ConfigSource::File);
                } else if (content[valueStart] == '-' || std::isdigit(content[valueStart])) {
                    // Number value
                    size_t valueEnd = content.find_first_of(",}\n\r", valueStart);
                    std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
                    
                    // Try double first, then int
                    try {
                        double doubleValue = std::stod(valueStr);
                        pos = valueEnd;
                        set(key, doubleValue, ConfigSource::File);
                    } catch (...) {
                        try {
                            int64_t intValue = std::stoll(valueStr);
                            pos = valueEnd;
                            set(key, intValue, ConfigSource::File);
                        } catch (...) {
                            break;
                        }
                    }
                }
            }
            return true;
        } else {
            // Fallback to simple key=value format for backward compatibility
            std::ifstream file2(filepath);
            std::string line;
            while (std::getline(file2, line)) {
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
                        ((value.front() == '\"' && value.back() == '\"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing config file " + filepath + ": " + std::string(e.what()));
        return false;
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
    
    // Use JSON format for modern config files
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Output value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double with reasonable precision
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg;
                file << oss.str();
            } else {
                file << "null";  // Default for unsupported types
            }
        }, entry.value);
        
        file << " ";
    }
    file << "\n}\n";
    
    return true;
}

void Config::setDefaultValues() {
    // Neural system configuration
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default, 
        "Total number of neurons in the brain");
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default,
        "Number of neural regions (multiple regions enable specialized processing)");
    set("connection_probability", 0.1f, ConfigSource::Default,
        "Probability of synapse formation between neurons (0.0 to 1.0)");
    set("simulation_timestep", 0.001, ConfigSource::Default,
        "Time step duration for simulation in seconds");
    set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default,
        "Maximum number of simulation steps before auto-termination");
    
    // Neuron dynamics parameters
    set("neuron_threshold", 1.0f, ConfigSource::Default,
        "Membrane potential threshold for spike generation");
    set("neuron_leak_conductance", 0.01f, ConfigSource::Default,
        "Membrane leak conductance (how quickly potential returns to resting state)");
    set("neuron_reset_potential", 0.0f, ConfigSource::Default,
        "Membrane potential to which neuron resets after spike");
    set("neuron_refractory_period", 0.005, ConfigSource::Default,
        "Refractory period duration after spike in seconds");
    
    // Plasticity parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default,
        "STDP Long-Term Potentiation weight factor");
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default,
        "STDP Long-Term Depression weight factor");
    set("stdp_tau", 20.0f, ConfigSource::Default,
        "STDP trace decay time constant (seconds)");
    set("hebbian_learning_rate", 0.001f, ConfigSource::Default,
        "Hebbian learning rate for co-activation strengthening");
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default,
        "Rate of new synapse formation (synaptogenesis)");
    set("pruning_rate", 0.00001f, ConfigSource::Default,
        "Rate of synapse elimination (pruning)");
    set("max_synapses_per_neuron", static_cast<size_t>(100), ConfigSource::Default,
        "Maximum number of synapses a neuron can form");
    
    // Neuromodulation parameters
    set("dopamine_effect_strength", 0.5f, ConfigSource::Default,
        "Strength of dopamine neuromodulation effects");
    set("curiosity_threshold", 0.3f, ConfigSource::Default,
        "Novelty threshold for curiosity-driven exploration");
    set("novelty_decay_rate", 0.99f, ConfigSource::Default,
        "Rate at which novelty signals decay over time");
    
    // Memory system parameters
    set("working_memory_capacity", static_cast<size_t>(100), ConfigSource::Default,
        "Capacity of working memory (number of active traces)");
    set("episodic_memory_max_episodes", static_cast<size_t>(1000), ConfigSource::Default,
        "Maximum number of episodes stored in episodic memory");
    set("consolidation_threshold", 0.3f, ConfigSource::Default,
        "Threshold for memory consolidation (weaker memories are dropped)");
    
    // Development parameters
    set("development_enabled", true, ConfigSource::Default,
        "Whether the brain should undergo developmental changes over time");
    set("plasticity_modifier_initial", 1.0f, ConfigSource::Default,
        "Initial plasticity modifier (higher = more plastic)");
    set("plasticity_modifier_adult", 0.2f, ConfigSource::Default,
        "Adult plasticity modifier (lower = more stable)");
    
    // Agent behavior parameters
    set("exploration_rate", 0.1f, ConfigSource::Default,
        "Default exploration rate for action selection (0.0 to 1.0)");
    set("reward_discount_factor", 0.95, ConfigSource::Default,
        "Discount factor for future rewards in reinforcement learning");
    
    // Performance and checkpointing
    set("checkpoint_dir", "./checkpoints", ConfigSource::Default,
        "Directory for saving brain state checkpoints");
    set("checkpoint_interval", static_cast<size_t>(1000), ConfigSource::Default,
        "Number of simulation steps between automatic checkpoints");
    set("max_checkpoints", static_cast<size_t>(10), ConfigSource::Default,
        "Maximum number of checkpoints to retain");
    
    // Debugging and logging
    set("log_level", "Info", ConfigSource::Default,
        "Logging verbosity level: Debug, Info, Warning, Error, Critical");
    set("enable_profiling", false, ConfigSource::Default,
        "Whether to enable performance profiling and statistics");
    set("debug_mode", false, ConfigSource::Default,
        "Enable debug output and additional validation checks");
    
    // Environment and world simulation
    set("world_width", static_cast<size_t>(20), ConfigSource::Default,
        "Width of the simulated world in cells");
    set("world_height", static_cast<size_t>(20), ConfigSource::Default,
        "Height of the simulated world in cells");
    set("vision_range", static_cast<size_t>(8), ConfigSource::Default,
        "Visual perception range (how many cells an agent can see)");
    
    // Random seed
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default,
        "Random seed for reproducible simulations (use -1 for random)");
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
