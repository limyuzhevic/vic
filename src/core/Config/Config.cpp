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
    // TODO PHASE 3: Implement proper JSON/YAML parser using a library like nlohmann::json
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded config from file: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        if (entry.value.index() == 0) {  // int
            file << std::get<int>(entry.value);
        } else if (entry.value.index() == 1) {  // int64_t
            file << std::get<int64_t>(entry.value);
        } else if (entry.value.index() == 2) {  // double
            file << std::get<double>(entry.value);
        } else if (entry.value.index() == 3) {  // bool
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (entry.value.index() == 4) {  // std::string
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (entry.value.index() == 5) {  // std::vector<int>
            const auto& vec = std::get<std::vector<int>>(entry.value);
            file << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << vec[i];
            }
            file << "]";
        } else if (entry.value.index() == 6) {  // std::vector<double>
            const auto& vec = std::get<std::vector<double>>(entry.value);
            file << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << vec[i];
            }
            file << "]";
        } else if (entry.value.index() == 7) {  // std::vector<std::string>
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            file << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << "\"" << vec[i] << "\"";
            }
            file << "]";
        }
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved config to file: " + filepath);
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

    // Get configuration summary
    std::string summary() const {
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
    
    // Brain configuration convenience methods
    void setBrainDefaults() {
        set("neuron_count", 1000);
        set("region_count", 1);
        set("connection_probability", 0.1f);
        set("simulation_timestep", 0.001);
        set("random_seed", 42);
        
        // Memory system configuration
        set("working_memory_capacity", 100);
        set("episodic_memory_max_episodes", 1000);
        set("associative_memory_capacity", 500);
        
        // Plasticity configuration
        set("stdp_ltp_weight", 0.01f);
        set("stdp_ltd_weight", 0.012f);
        set("stdp_tau", 20.0f);
        set("synaptogenesis_rate", 0.0001f);
        set("pruning_rate", 0.00001f);
        
        // Development configuration
        set("developmental_stage", 0);  // 0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult, 4=Aging
        set("plasticity_decay_rate", 0.05f);
        
        // Neuromodulation configuration
        set("dopamine_baseline", 0.1f);
        set("curiosity_rate", 0.05f);
        set("novelty_threshold", 0.5f);
        
        // Cognition configuration
        set("planner_depth", 5);
        set("concept_learning_rate", 0.01f);
        set("attention_inhibition_strength", 0.5f);
        set("attention_excitation_strength", 1.5f);
        
        // Memory integration
        set("replay_interval", 100);
        set("consolidation_interval", 1000);
        
        // Checkpoint configuration
        set("checkpoint_dir", "./checkpoints");
        set("checkpoint_interval", 10000);
        set("max_checkpoints", 10);
        
        NLM_LOG_INFO("Applied brain default configuration");
    }
    
    // Get brain configuration as a human-readable string
    std::string getBrainConfigSummary() const {
        std::stringstream ss;
        ss << "=== NLM Brain Configuration Summary ===\n";
        ss << "Neurons: " << getOr<int>("neuron_count", 0) << "\n";
        ss << "Regions: " << getOr<int>("region_count", 0) << "\n";
        ss << "Connections: ~" << getOr<int>("neuron_count", 0) * getOr<int>("neuron_count", 0) * 
            getOr<float>("connection_probability", 0.0f) / 2.0f << "\n";
        ss << "Memory: Working(" << getOr<int>("working_memory_capacity", 0) << ") + "
            << "Episodic(" << getOr<int>("episodic_memory_max_episodes", 0) << ") + "
            << "Associative(" << getOr<int>("associative_memory_capacity", 0) << ")\n";
        ss << "Plasticity: STDP, Hebbian, Structural\n";
        ss << "Development: " << getOr<int>("developmental_stage", 0) << "\n";
        ss << "Neuromodulation: Dopamine, Curiosity, Novelty, Prediction Error\n";
        ss << "Cognition: Planning, Concepts, Attention\n";
        
        return ss.str();
    }
    
    // Set brain-specific configuration values
    void setNeuronCount(size_t count) {
        set("neuron_count", static_cast<int>(count));
    }
    
    void setRegionCount(size_t count) {
        set("region_count", static_cast<int>(count));
    }
    
    void setConnectionProbability(float prob) {
        set("connection_probability", prob);
    }
    
    void setSimulationTimestep(double timestep) {
        set("simulation_timestep", timestep);
    }
    
    void setRandomSeed(uint64_t seed) {
        set("random_seed", static_cast<int64_t>(seed));
    }
    
    // Get brain-specific configuration values with defaults
    size_t getNeuronCount() const {
        return static_cast<size_t>(getOr<int>("neuron_count", 1000));
    }
    
    size_t getRegionCount() const {
        return static_cast<size_t>(getOr<int>("region_count", 1));
    }
    
    float getConnectionProbability() const {
        return getOr<float>("connection_probability", 0.1f);
    }
    
    double getSimulationTimestep() const {
        return getOr<double>("simulation_timestep", 0.001);
    }
    
    uint64_t getRandomSeed() const {
        return static_cast<uint64_t>(getOr<int64_t>("random_seed", 42));
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
