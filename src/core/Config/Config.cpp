#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationError> validationErrors;
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

// Configuration validation
void Config::validate() {
    pImpl->validationErrors.clear();
    
    // Validate specific known configuration parameters
    if (has("neuron_count")) {
        auto neuronCountOpt = get<size_t>("neuron_count");
        if (neuronCountOpt && *neuronCountOpt == 0) {
            pImpl->validationErrors.emplace_back("neuron_count", "Neuron count must be greater than 0");
        }
    }
    
    if (has("region_count")) {
        auto regionCountOpt = get<size_t>("region_count");
        if (regionCountOpt && *regionCountOpt == 0) {
            pImpl->validationErrors.emplace_back("region_count", "Region count must be greater than 0");
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestepOpt = get<double>("simulation_timestep");
        if (timestepOpt) {
            if (*timestepOpt <= 0.0) {
                pImpl->validationErrors.emplace_back("simulation_timestep", "Simulation timestep must be positive");
            } else if (*timestepOpt > 1.0) {
                pImpl->validationErrors.emplace_back("simulation_timestep", "Simulation timestep too large (max 1.0)");
            }
        }
    }
    
    if (has("connection_probability")) {
        auto probOpt = get<double>("connection_probability");
        if (probOpt) {
            if (*probOpt < 0.0 || *probOpt > 1.0) {
                pImpl->validationErrors.emplace_back("connection_probability", "Connection probability must be between 0.0 and 1.0");
            }
        }
    }
    
    if (has("stdp_ltp_weight")) {
        auto ltpOpt = get<double>("stdp_ltp_weight");
        if (ltpOpt && *ltpOpt < 0.0) {
            pImpl->validationErrors.emplace_back("stdp_ltp_weight", "STDP LTP weight must be non-negative");
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto ltdOpt = get<double>("stdp_ltd_weight");
        if (ltdOpt && *ltdOpt < 0.0) {
            pImpl->validationErrors.emplace_back("stdp_ltd_weight", "STDP LTD weight must be non-negative");
        }
    }
    
    if (has("stdp_tau")) {
        auto tauOpt = get<double>("stdp_tau");
        if (tauOpt && *tauOpt <= 0.0) {
            pImpl->validationErrors.emplace_back("stdp_tau", "STDP tau must be positive");
        }
    }
    
    if (has("synaptogenesis_rate")) {
        auto rateOpt = get<double>("synaptogenesis_rate");
        if (rateOpt) {
            if (*rateOpt < 0.0 || *rateOpt > 0.1) {
                pImpl->validationErrors.emplace_back("synaptogenesis_rate", "Synaptogenesis rate must be between 0.0 and 0.1");
            }
        }
    }
    
    if (has("pruning_rate")) {
        auto rateOpt = get<double>("pruning_rate");
        if (rateOpt) {
            if (*rateOpt < 0.0 || *rateOpt > 0.1) {
                pImpl->validationErrors.emplace_back("pruning_rate", "Pruning rate must be between 0.0 and 0.1");
            }
        }
    }
    
    // Check that required keys have values
    const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "simulation_timestep",
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            pImpl->validationErrors.emplace_back(key, "Required configuration key is missing");
        }
    }
}

bool Config::hasErrors() const {
    return !pImpl->validationErrors.empty();
}

const std::vector<ValidationError>& Config::getErrors() const {
    return pImpl->validationErrors;
}

} // namespace nlm
