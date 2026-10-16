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

// Apply configuration preset
void Config::applyPreset(const std::string& presetName) {
    clear();
    
    // Convert to lowercase for case-insensitive comparison
    std::string presetLower = toLower(presetName);
    
    if (presetLower == "quick") {
        // Quick test configuration
        set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.1f, ConfigSource::Default);
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        set("stdp_tau", 20.0f, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        set("pruning_rate", 0.00001f, ConfigSource::Default);
        set("enable_checkpointing", false, ConfigSource::Default);
        set("enable_replay", false, ConfigSource::Default);
        set("enable_development", false, ConfigSource::Default);
        set("enable_plasticity", false, ConfigSource::Default);
        set("enable_structural_plasticity", false, ConfigSource::Default);
    }
    else if (presetLower == "balanced") {
        // Balanced test configuration (Phase 6 standard)
        set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.15f, ConfigSource::Default);
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        set("stdp_tau", 20.0f, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        set("pruning_rate", 0.00001f, ConfigSource::Default);
        set("enable_checkpointing", true, ConfigSource::Default);
        set("enable_replay", true, ConfigSource::Default);
        set("enable_development", true, ConfigSource::Default);
        set("enable_plasticity", true, ConfigSource::Default);
        set("enable_structural_plasticity", true, ConfigSource::Default);
    }
    else if (presetLower == "advanced") {
        // Advanced test configuration
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        set("connection_probability", 0.1f, ConfigSource::Default);
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.02f, ConfigSource::Default);
        set("stdp_tau", 15.0f, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        set("pruning_rate", 0.00005f, ConfigSource::Default);
        set("enable_checkpointing", true, ConfigSource::Default);
        set("enable_replay", true, ConfigSource::Default);
        set("enable_development", true, ConfigSource::Default);
        set("enable_plasticity", true, ConfigSource::Default);
        set("enable_structural_plasticity", true, ConfigSource::Default);
        set("enable_curiosity", true, ConfigSource::Default);
        set("enable_novelty", true, ConfigSource::Default);
    }
    else if (presetLower == "extreme") {
        // Extreme test configuration for maximum coverage
        set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        set("connection_probability", 0.05f, ConfigSource::Default);
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.05f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.04f, ConfigSource::Default);
        set("stdp_tau", 10.0f, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0005f, ConfigSource::Default);
        set("pruning_rate", 0.0002f, ConfigSource::Default);
        set("enable_checkpointing", true, ConfigSource::Default);
        set("enable_replay", true, ConfigSource::Default);
        set("enable_development", true, ConfigSource::Default);
        set("enable_plasticity", true, ConfigSource::Default);
        set("enable_structural_plasticity", true, ConfigSource::Default);
        set("enable_curiosity", true, ConfigSource::Default);
        set("enable_novelty", true, ConfigSource::Default);
        set("enable_prediction", true, ConfigSource::Default);
        set("enable_planning", true, ConfigSource::Default);
        set("enable_concept_formation", true, ConfigSource::Default);
        set("enable_attention", true, ConfigSource::Default);
    }
    else {
        // Default configuration
        set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.1f, ConfigSource::Default);
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        set("simulation_timestep", 0.001, ConfigSource::Default);
        set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        set("stdp_tau", 20.0f, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        set("pruning_rate", 0.00001f, ConfigSource::Default);
        set("enable_checkpointing", true, ConfigSource::Default);
        set("enable_replay", true, ConfigSource::Default);
        set("enable_development", true, ConfigSource::Default);
        set("enable_plasticity", true, ConfigSource::Default);
        set("enable_structural_plasticity", true, ConfigSource::Default);
    }
}

// Validate configuration values
bool Config::validate() const {
    bool isValid = true;
    
    // Get values safely with validation
    auto neuronCountOpt = get<int64_t>("neuron_count");
    if (!neuronCountOpt) {
        std::cerr << "Error: neuron_count not set" << std::endl;
        isValid = false;
    } else {
        int64_t neuronCount = *neuronCountOpt;
        if (neuronCount < 10) {
            std::cerr << "Error: neuron_count too low (minimum 10)" << std::endl;
            isValid = false;
        } else if (neuronCount > 10000) {
            std::cerr << "Error: neuron_count too high (maximum 10000)" << std::endl;
            isValid = false;
        }
    }
    
    auto regionCountOpt = get<int64_t>("region_count");
    if (!regionCountOpt) {
        std::cerr << "Error: region_count not set" << std::endl;
        isValid = false;
    } else {
        int64_t regionCount = *regionCountOpt;
        if (regionCount < 1) {
            std::cerr << "Error: region_count must be at least 1" << std::endl;
            isValid = false;
        }
    }
    
    auto connectionProbOpt = get<double>("connection_probability");
    if (!connectionProbOpt) {
        std::cerr << "Error: connection_probability not set" << std::endl;
        isValid = false;
    } else {
        double connectionProb = *connectionProbOpt;
        if (connectionProb < 0.0 || connectionProb > 1.0) {
            std::cerr << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
            isValid = false;
        }
    }
    
    auto simulationTimestepOpt = get<double>("simulation_timestep");
    if (!simulationTimestepOpt) {
        std::cerr << "Error: simulation_timestep not set" << std::endl;
        isValid = false;
    } else {
        double timestep = *simulationTimestepOpt;
        if (timestep <= 0.0) {
            std::cerr << "Error: simulation_timestep must be positive" << std::endl;
            isValid = false;
        } else if (timestep > 0.1) {
            std::cerr << "Warning: simulation_timestep is large (max recommended 0.1)" << std::endl;
        }
    }
    
    return isValid;
}

// Enhanced save to file with actual values
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on: " << std::endl;
    file << "# Format: key = value (quoted strings)\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}
