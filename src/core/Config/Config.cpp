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

// Configuration validation and diagnostics utilities
// These functions provide validation of configuration parameters and diagnostic reporting
// to help users identify configuration issues before simulation runtime errors occur.

namespace nlm {

bool validateConfig(const Config& config) {
    bool isValid = true;
    
    // Validate neuron_count
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    if (neuronCount == 0) {
        NLM_LOG_WARNING("neuron_count is not set or is 0");
        isValid = false;
    } else if (neuronCount > 10000) {
        NLM_LOG_WARNING("neuron_count is very large (" + std::to_string(neuronCount) + ") - may impact performance");
    }
    
    // Validate region_count
    auto regionCount = config.getOr<size_t>("region_count", 0);
    if (regionCount == 0) {
        NLM_LOG_WARNING("region_count is not set or is 0");
        isValid = false;
    } else if (regionCount > 10) {
        NLM_LOG_WARNING("region_count is large (" + std::to_string(regionCount) + ") - may impact performance");
    }
    
    // Validate simulation_timestep
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        NLM_LOG_WARNING("simulation_timestep must be positive, using default 0.001");
        const_cast<Config&>(config).set("simulation_timestep", 0.001, ConfigSource::Default);
    } else if (timestep > 0.1) {
        NLM_LOG_WARNING("simulation_timestep is large (" + std::to_string(timestep) + ") - may impact stability");
    }
    
    // Validate connection_probability
    auto connProb = config.getOr<float>("connection_probability", 0.0f);
    if (connProb < 0.0f || connProb > 1.0f) {
        NLM_LOG_WARNING("connection_probability must be between 0.0 and 1.0, using default 0.15");
        const_cast<Config&>(config).set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    // Validate plasticity parameters
    auto learningRate = config.getOr<float>("plasticity_learning_rate", 0.0f);
    if (learningRate <= 0.0f) {
        NLM_LOG_WARNING("plasticity_learning_rate should be positive");
    }
    
    auto stdpLtp = config.getOr<float>("stdp_ltp_weight", 0.0f);
    auto stdpLtd = config.getOr<float>("stdp_ltd_weight", 0.0f);
    if (stdpLtp <= 0.0f) {
        NLM_LOG_WARNING("stdp_ltp_weight should be positive");
    }
    if (stdpLtd <= 0.0f) {
        NLM_LOG_WARNING("stdp_ltd_weight should be positive");
    }
    
    // Validate development parameters
    auto synaptogenesis = config.getOr<float>("development_synaptogenesis_rate", 0.0f);
    auto pruning = config.getOr<float>("development_pruning_rate", 0.0f);
    if (synaptogenesis < 0.0f) {
        NLM_LOG_WARNING("development_synaptogenesis_rate should be non-negative");
    }
    if (pruning < 0.0f) {
        NLM_LOG_WARNING("development_pruning_rate should be non-negative");
    }
    
    // Validate random_seed
    auto seed = config.getOr<int64_t>("random_seed", 0);
    if (seed < 0) {
        NLM_LOG_WARNING("random_seed should be non-negative");
    }
    
    // Validate max_simulation_steps
    auto maxSteps = config.getOr<size_t>("max_simulation_steps", 0);
    if (maxSteps == 0) {
        NLM_LOG_WARNING("max_simulation_steps should be positive");
    } else if (maxSteps > 1000000) {
        NLM_LOG_WARNING("max_simulation_steps is very large (" + std::to_string(maxSteps) + ") - may impact performance");
    }
    
    return isValid;
}

std::string getConfigValidationReport(const Config& config) {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    
    bool allValid = true;
    
    // Check key required parameters
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    if (neuronCount > 0) {
        oss << "✓ neuron_count: " << neuronCount << " (OK)\n";
    } else {
        oss << "✗ neuron_count: not set or zero\n";
        allValid = false;
    }
    
    auto regionCount = config.getOr<size_t>("region_count", 0);
    if (regionCount > 0) {
        oss << "✓ region_count: " << regionCount << " (OK)\n";
    } else {
        oss << "✗ region_count: not set or zero\n";
        allValid = false;
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep > 0.0) {
        oss << "✓ simulation_timestep: " << timestep << "s (OK)\n";
    } else {
        oss << "✗ simulation_timestep: invalid (" << timestep << ")\n";
        allValid = false;
    }
    
    auto connProb = config.getOr<float>("connection_probability", 0.0f);
    if (connProb >= 0.0f && connProb <= 1.0f) {
        oss << "✓ connection_probability: " << connProb << " (OK)\n";
    } else {
        oss << "✗ connection_probability: invalid (" << connProb << ")\n";
        allValid = false;
    }
    
    oss << "\n=== Configuration Summary ===\n";
    oss << "Total parameters loaded: " << config.getKeys().size() << "\n";
    oss << "Configuration is " << (allValid ? "valid" : "has issues") << "\n";
    
    return oss.str();
}

std::vector<std::string> getConfigErrors(const Config& config) {
    std::vector<std::string> errors;
    
    // Check for known configuration issues
    auto neuronCount = config.getOr<size_t>("neuron_count", 0);
    if (neuronCount == 0) {
        errors.push_back("neuron_count is not set or is 0");
    } else if (neuronCount > 10000) {
        errors.push_back("neuron_count is very large (" + std::to_string(neuronCount) + ") - may impact performance");
    }
    
    auto timestep = config.getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        errors.push_back("simulation_timestep must be positive");
    }
    
    auto connProb = config.getOr<float>("connection_probability", 0.0f);
    if (connProb < 0.0f || connProb > 1.0f) {
        errors.push_back("connection_probability must be between 0.0 and 1.0");
    }
    
    return errors;
}

} // namespace nlm
