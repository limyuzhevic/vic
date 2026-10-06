#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Set default configuration values
static std::vector<ConfigEntry> getDefaultConfigEntries() {
    std::vector<ConfigEntry> entries;
    
    // Core brain configuration
    entries.emplace_back("neuron_count", int64_t(1000), ConfigSource::Default, "Total number of neurons in the network");
    entries.emplace_back("region_count", int64_t(1), ConfigSource::Default, "Number of neural regions");
    entries.emplace_back("connection_probability", 0.1f, ConfigSource::Default, "Probability of connection between neurons (0.0-1.0)");
    entries.emplace_back("synapse_density", 0.1f, ConfigSource::Default, "Initial synapse density (0.0-1.0)");
    
    // Neuron dynamics
    entries.emplace_back("v_thresh", -50.0, ConfigSource::Default, "Neuron voltage threshold (mV)");
    entries.emplace_back("v_rest", -70.0, ConfigSource::Default, "Resting potential (mV)");
    entries.emplace_back("v_reset", -75.0, ConfigSource::Default, "Reset potential after spike (mV)");
    entries.emplace_back("tau_mem", 20.0, ConfigSource::Default, "Membrane time constant (ms)");
    entries.emplace_back("tau_ref", 2.0, ConfigSource::Default, "Refractory period (ms)");
    
    // Simulation parameters
    entries.emplace_back("simulation_timestep", 0.001, ConfigSource::Default, "Simulation time step (seconds)");
    entries.emplace_back("simulation_steps", int64_t(10000), ConfigSource::Default, "Total number of simulation steps");
    entries.emplace_back("random_seed", int64_t(42), ConfigSource::Default, "Random seed for reproducible results");
    
    // Plasticity parameters
    entries.emplace_back("plasticity_enabled", true, ConfigSource::Default, "Enable plasticity mechanisms");
    entries.emplace_back("plasticity_learning_rate", 0.01, ConfigSource::Default, "General plasticity learning rate");
    entries.emplace_back("plasticity_scale", 1.0, ConfigSource::Default, "Plasticity strength scaling factor");
    
    // STDP parameters
    entries.emplace_back("stdp_enabled", true, ConfigSource::Default, "Enable STDP plasticity");
    entries.emplace_back("stdp_ltp_weight", 0.02f, ConfigSource::Default, "STDP LTP weight");
    entries.emplace_back("stdp_ltd_weight", 0.015f, ConfigSource::Default, "STDP LTD weight");
    entries.emplace_back("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant (ms)");
    
    // Structural plasticity parameters
    entries.emplace_back("synaptogenesis_enabled", true, ConfigSource::Default, "Enable structural plasticity (new connections)");
    entries.emplace_back("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Rate of new synapse formation");
    entries.emplace_back("pruning_enabled", true, ConfigSource::Default, "Enable synaptic pruning");
    entries.emplace_back("pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse removal");
    
    // Memory system parameters
    entries.emplace_back("memory_working_memory_enabled", true, ConfigSource::Default, "Enable working memory system");
    entries.emplace_back("memory_episodic_enabled", true, ConfigSource::Default, "Enable episodic memory system");
    entries.emplace_back("memory_associative_enabled", true, ConfigSource::Default, "Enable associative memory system");
    entries.emplace_back("memory_trace_duration", 1000.0, ConfigSource::Default, "Duration of memory traces (steps)");
    entries.emplace_back("memory_replay_enabled", true, ConfigSource::Default, "Enable memory replay during rest");
    
    // Neuromodulation parameters
    entries.emplace_back("neuromodulation_enabled", true, ConfigSource::Default, "Enable neuromodulation systems");
    entries.emplace_back("dopamine_enabled", true, ConfigSource::Default, "Enable dopamine modulation");
    entries.emplace_back("curiosity_enabled", true, ConfigSource::Default, "Enable curiosity-driven exploration");
    entries.emplace_back("novelty_enabled", true, ConfigSource::Default, "Enable novelty detection");
    entries.emplace_back("reward_prediction_enabled", true, ConfigSource::Default, "Enable reward prediction error");
    
    // Prediction system parameters
    entries.emplace_back("prediction_enabled", true, ConfigSource::Default, "Enable predictive processing");
    entries.emplace_back("prediction_horizon", 10.0, ConfigSource::Default, "Prediction time horizon (steps)");
    entries.emplace_back("prediction_error_weight", 0.1, ConfigSource::Default, "Weight of prediction error signals");
    
    // Development parameters
    entries.emplace_back("development_enabled", true, ConfigSource::Default, "Enable developmental stages");
    entries.emplace_back("development_rate", 0.001, ConfigSource::Default, "Rate of developmental change");
    entries.emplace_back("critical_period_enabled", true, ConfigSource::Default, "Enable critical period for plasticity");
    entries.emplace_back("sensitive_period_start", 100.0, ConfigSource::Default, "Start of sensitive period (steps)");
    entries.emplace_back("sensitive_period_end", 1000.0, ConfigSource::Default, "End of sensitive period (steps)");
    
    // Cognitive system parameters
    entries.emplace_back("attention_enabled", true, ConfigSource::Default, "Enable attention mechanisms");
    entries.emplace_back("attention_strength", 0.5, ConfigSource::Default, "Strength of attentional focus");
    entries.emplace_back("planner_enabled", true, ConfigSource::Default, "Enable action planning");
    entries.emplace_back("concept_formation_enabled", true, ConfigSource::Default, "Enable concept formation");
    entries.emplace_back("self_model_enabled", true, ConfigSource::Default, "Enable self-model representation");
    
    // Environmental parameters
    entries.emplace_back("environment_size", 20.0, ConfigSource::Default, "Size of simulated environment (arbitrary units)");
    entries.emplace_back("vision_range", 5.0, ConfigSource::Default, "Visual perception range");
    entries.emplace_back("auditory_range", 5.0, ConfigSource::Default, "Auditory perception range");
    entries.emplace_back("reward_scale", 1.0, ConfigSource::Default, "Reward magnitude scaling factor");
    
    // Checkpoint and persistence
    entries.emplace_back("checkpoint_enabled", true, ConfigSource::Default, "Enable checkpoint saving");
    entries.emplace_back("checkpoint_path", std::string("./checkpoint.bin"), ConfigSource::Default, "Path for checkpoint files");
    entries.emplace_back("auto_save_interval", 1000, ConfigSource::Default, "Auto-save interval (steps)");
    
    // Logging and output
    entries.emplace_back("log_level", std::string("info"), ConfigSource::Default, "Logging verbosity: debug, info, warn, error, off");
    entries.emplace_back("log_file", std::string("nlm.log"), ConfigSource::Default, "Path for log file");
    entries.emplace_back("output_format", std::string("text"), ConfigSource::Default, "Output format: text, json, csv, binary");
    
    return entries;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Load default configuration
    for (const auto& entry : getDefaultConfigEntries()) {
        pImpl->entries.push_back(entry);
    }
}

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
    // Try to use JSON if available, otherwise use simple format
    std::string ext = "";
    if (filepath.size() >= 4) {
        ext = filepath.substr(filepath.size() - 4);
    }
    
    bool useJSON = (ext == ".json" || ext == ".JSON");
    
    if (useJSON) {
        return saveToJSON(filepath);
    } else {
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Neural Learning Machine\n";
    file << "# " << time(nullptr) << std::endl;
    file << "\n";
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::Default) continue; // Skip default values
        
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
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; bool first = true;
                for (auto val : arg) {
                    if (!first) file << ", "; first = false;
                    file << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; bool first = true;
                for (auto val : arg) {
                    if (!first) file << ", "; first = false;
                    file << std::fixed << std::setprecision(6) << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; bool first = true;
                for (auto& val : arg) {
                    if (!first) file << ", "; first = false;
                    file << "\"" << val << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file.close();
    return true;
}

bool Config::saveToJSON(const std::string& filepath) const {
    // Simplified JSON generation - could be enhanced with proper JSON library
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    bool firstEntry = true;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::Default) continue; // Skip default values
        
        if (!firstEntry) {
            file << ",\n";
        }
        firstEntry = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Convert value to JSON based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes in string
                std::string escaped = arg;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                file << "\"" << escaped << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                bool first = true;
                for (auto val : arg) {
                    if (!first) file << ", "; first = false;
                    file << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                bool first = true;
                for (auto val : arg) {
                    if (!first) file << ", "; first = false;
                    file << std::fixed << std::setprecision(6) << val;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (auto& val : arg) {
                    if (!first) file << ", "; first = false;
                    std::string escaped = val;
                    size_t pos = 0;
                    while ((pos = escaped.find('"', pos)) != std::string::npos) {
                        escaped.replace(pos, 1, "\\\"");
                        pos += 2;
                    }
                    file << "\"" << escaped << "\"";
                }
                file << "]";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    file.close();
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
