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

bool Config::validateConfiguration() const {
    auto errors = validateConfigurationDetailed();
    return errors.empty();
}

std::vector<std::string> Config::validateConfigurationDetailed() const {
    std::vector<std::string> errors;
    
    // Validate neuron count
    if (has("neuron_count")) {
        if (auto neuronCountOpt = get<size_t>("neuron_count")) {
            if (!validateNeuronCount(*neuronCountOpt)) {
                errors.push_back("Invalid neuron_count: " + std::to_string(*neuronCountOpt) + ". Must be a positive integer.");
            }
        }
    }
    
    // Validate region count
    if (has("region_count")) {
        if (auto regionCountOpt = get<size_t>("region_count")) {
            if (!validateRegionCount(*regionCountOpt)) {
                errors.push_back("Invalid region_count: " + std::to_string(*regionCountOpt) + ". Must be a positive integer.");
            }
        }
    }
    
    // Validate connection probability
    if (has("connection_probability")) {
        if (auto probOpt = get<double>("connection_probability")) {
            if (!validateConnectionProbability(static_cast<float>(*probOpt))) {
                errors.push_back("Invalid connection_probability: " + std::to_string(*probOpt) + ". Must be between 0.0 and 1.0.");
            }
        }
    }
    
    // Validate simulation timestep
    if (has("simulation_timestep")) {
        if (auto timestepOpt = get<double>("simulation_timestep")) {
            if (!validateTimestep(*timestepOpt)) {
                errors.push_back("Invalid simulation_timestep: " + std::to_string(*timestepOpt) + ". Must be a positive number.");
            }
        }
    }
    
    // Validate plasticity parameters
    if (has("stdp_ltp_weight")) {
        if (auto ltpOpt = get<double>("stdp_ltp_weight")) {
            if (!validatePositiveValue(*ltpOpt)) {
                errors.push_back("Invalid stdp_ltp_weight: " + std::to_string(*ltpOpt) + ". Must be positive.");
            }
        }
    }
    
    if (has("stdp_ltd_weight")) {
        if (auto ltdOpt = get<double>("stdp_ltd_weight")) {
            if (!validatePositiveValue(*ltdOpt)) {
                errors.push_back("Invalid stdp_ltd_weight: " + std::to_string(*ltdOpt) + ". Must be positive.");
            }
        }
    }
    
    // Validate structural plasticity rates
    if (has("synaptogenesis_rate")) {
        if (auto synOpt = get<double>("synaptogenesis_rate")) {
            if (!validatePositiveValue(*synOpt, 0.0)) {
                errors.push_back("Invalid synaptogenesis_rate: " + std::to_string(*synOpt) + ". Must be non-negative.");
            }
        }
    }
    
    if (has("pruning_rate")) {
        if (auto pruneOpt = get<double>("pruning_rate")) {
            if (!validatePositiveValue(*pruneOpt, 0.0)) {
                errors.push_back("Invalid pruning_rate: " + std::to_string(*pruneOpt) + ". Must be non-negative.");
            }
        }
    }
    
    return errors;
}

bool Config::validateNeuronCount(size_t neuronCount) const {
    return neuronCount > 0 && neuronCount <= 1000000; // Reasonable limits
}

bool Config::validateRegionCount(size_t regionCount) const {
    return regionCount > 0 && regionCount <= 1000; // Reasonable limits
}

bool Config::validateConnectionProbability(float probability) const {
    return probability >= 0.0f && probability <= 1.0f;
}

bool Config::validateTimestep(double timestep) const {
    return timestep > 0.0 && timestep <= 1.0; // Should be less than 1 second
}

bool Config::validatePositiveValue(double value, double tolerance) const {
    return value >= -tolerance;
}

std::vector<Config::ConfigurationInfo> Config::getConfigurationSchema() {
    return {
        {"neuron_count", "size_t", "Number of neurons in the brain", "1000", true, "Must be a positive integer (1-1000000)"},
        {"region_count", "size_t", "Number of brain regions", "1", true, "Must be a positive integer (1-1000)"},
        {"connection_probability", "float", "Probability of connection between neurons", "0.1", true, "Must be between 0.0 and 1.0"},
        {"simulation_timestep", "double", "Simulation time step in seconds", "0.001", true, "Must be positive (0.0-1.0)"},
        {"random_seed", "int64_t", "Random seed for reproducible simulations", "42", false, "Any integer value"},
        {"stdp_ltp_weight", "float", "STDP LTP weight", "0.01", false, "Must be positive"},
        {"stdp_ltd_weight", "float", "STDP LTD weight", "0.012", false, "Must be positive"},
        {"stdp_tau", "float", "STDP time constant", "20.0", false, "Must be positive"},
        {"synaptogenesis_rate", "float", "Structural plasticity synaptogenesis rate", "0.0001", false, "Must be non-negative"},
        {"pruning_rate", "float", "Structural plasticity pruning rate", "0.00001", false, "Must be non-negative"},
        {"replay_interval", "size_t", "Memory replay interval in steps", "100", false, "Must be positive"},
        {"consolidation_interval", "size_t", "Memory consolidation interval in steps", "1000", false, "Must be positive"},
        {"checkpoint_dir", "std::string", "Directory for checkpoint files", "./checkpoints", false, "Any valid path"},
        {"max_checkpoints", "size_t", "Maximum number of checkpoints to retain", "10000", false, "Must be positive"},
        {"checkpoint_retention_days", "size_t", "Days to retain checkpoints", "5", false, "Must be positive"},
        {"auto_save", "bool", "Automatically save checkpoints", "true", false, "Any boolean value"},
        {"planning_depth", "size_t", "Neural planner depth", "5", false, "Must be positive"},
        {"max_episodes", "size_t", "Maximum number of episodic memory entries", "1000", false, "Must be positive"},
        {"working_memory_capacity_divisor", "size_t", "Working memory capacity divisor", "10", false, "Must be positive"},
        {"episode_interval", "size_t", "Episodic memory storage interval", "10", false, "Must be positive"},
        {"novelty_decay", "float", "Novelty detection decay factor", "0.99", false, "Must be between 0.0 and 1.0"},
        {"curiosity_multiplier", "float", "Curiosity calculation multiplier", "2.0", false, "Must be positive"},
        {"prediction_error_weight", "float", "Prediction error weighting factor", "0.5", false, "Must be positive"},
        {"exploration_threshold", "float", "Curiosity-based exploration threshold", "0.3", false, "Must be between 0.0 and 1.0"},
        {"dopamine_excitability", "float", "Dopamine effect on neural excitability", "0.5", false, "Must be positive"},
        {"plasticity_factor_adjustment", "float", "Plasticity factor adjustment factor", "0.001", false, "Must be positive"}
    };
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
