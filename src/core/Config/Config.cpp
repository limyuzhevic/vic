#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace nlm {

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

    // Validate configuration with comprehensive checks
    bool validate() const {
        bool valid = true;
        
        // Check required keys
        std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.connection_probability",
            "plasticity.stdp.enable",
            "neuromod.curiosity.enable"
        };
        
        for (const auto& key : requiredKeys) {
            if (!has(key)) {
                NLM_LOG_WARNING("Missing required configuration key: " + key);
                valid = false;
            }
        }
        
        // Validate neuron count
        if (auto neuronCount = get<int64_t>("brain.neuron_count")) {
            if (*neuronCount < 10 || *neuronCount > 100000) {
                NLM_LOG_ERROR("brain.neuron_count out of range: must be 10-100000");
                valid = false;
            }
        }
        
        // Validate connection probability
        if (auto connProb = get<double>("brain.connection_probability")) {
            if (*connProb < 0.0 || *connProb > 1.0) {
                NLM_LOG_ERROR("brain.connection_probability must be 0.0-1.0");
                valid = false;
            }
        }
        
        return valid;
    }
    
    // Get configuration validation errors
    std::vector<std::string> getValidationErrors() const {
        std::vector<std::string> errors;
        
        // Check required keys
        std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.connection_probability",
            "plasticity.stdp.enable",
            "neuromod.curiosity.enable"
        };
        
        for (const auto& key : requiredKeys) {
            if (!has(key)) {
                errors.push_back("Missing required configuration key: " + key);
            }
        }
        
        // Validate neuron count
        if (auto neuronCount = get<int64_t>("brain.neuron_count")) {
            if (*neuronCount < 10 || *neuronCount > 100000) {
                errors.push_back("brain.neuron_count out of range: must be 10-100000");
            }
        } else {
            errors.push_back("brain.neuron_count must be specified");
        }
        
        // Validate connection probability
        if (auto connProb = get<double>("brain.connection_probability")) {
            if (*connProb < 0.0 || *connProb > 1.0) {
                errors.push_back("brain.connection_probability must be 0.0-1.0");
            }
        } else {
            errors.push_back("brain.connection_probability must be specified");
        }
        
        // Validate consistency
        if (has("plasticity.stdp.enable") && has("plasticity.stdp.learning_rate")) {
            auto learningRate = get<double>("plasticity.stdp.learning_rate");
            if (learningRate && *learningRate < 0.0) {
                errors.push_back("plasticity.stdp.learning_rate must be positive");
            }
        }
        
        return errors;
    }
    
    // Get configuration validation warnings
    std::vector<std::string> getValidationWarnings() const {
        std::vector<std::string> warnings;
        
        // Check for unusual values
        if (has("brain.neuron_count")) {
            auto neuronCount = get<int64_t>("brain.neuron_count");
            if (neuronCount && *neuronCount > 50000) {
                warnings.push_back("Large neuron count (>50,000) may impact performance");
            }
        }
        
        if (has("brain.synapse_density")) {
            auto synapseDensity = get<double>("brain.synapse_density");
            if (synapseDensity && *synapseDensity > 0.5) {
                warnings.push_back("High synapse density (>0.5) may impact performance");
            }
        }
        
        return warnings;
    }
    
    // Get configuration as YAML string
    std::string toYAML() const {
        std::ostringstream oss;
        oss << "brain:\n";
        oss << "  neuron_count: " << getOr<int64_t>("brain.neuron_count", 1000) << "\n";
        oss << "  connection_probability: " << getOr<double>("brain.connection_probability", 0.05) << "\n";
        oss << "  synapse_density: " << getOr<double>("brain.synapse_density", 0.1) << "\n";
        oss << "  v_thresh: " << getOr<double>("brain.v_thresh", -50.0) << "\n";
        oss << "  v_rest: " << getOr<double>("brain.v_rest", -70.0) << "\n";
        oss << "\nplasticity:\n";
        oss << "  stdp.enable: " << getOr<bool>("plasticity.stdp.enable", true) << "\n";
        oss << "  hebbian.enable: " << getOr<bool>("plasticity.hebbian.enable", true) << "\n";
        oss << "  structural.enable: " << getOr<bool>("plasticity.structural.enable", true) << "\n";
        oss << "\nneuromodulation:\n";
        oss << "  dopamine.scale: " << getOr<double>("neuromod.dopamine.scale", 1.0) << "\n";
        oss << "  curiosity.enable: " << getOr<bool>("neuromod.curiosity.enable", true) << "\n";
        oss << "  novelty.enable: " << getOr<bool>("neuromod.novelty.enable", true) << "\n";
        oss << "\ndevelopment:\n";
        oss << "  initial_plasticity: " << getOr<double>("development.initial_plasticity", 1.0) << "\n";
        oss << "  critical_period: " << getOr<int64_t>("development.critical_period", 1000) << "\n";
        return oss.str();
    }
    
    // Merge with another config (runtime overrides)
    void merge(const Config& other) {
        for (const auto& entry : other.pImpl->entries) {
            set(entry.key, entry.value, ConfigSource::Runtime);
        }
    }
    
    // Compare two configurations for equality
    bool equals(const Config& other) const {
        if (pImpl->entries.size() != other.pImpl->entries.size()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            auto otherValue = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                          [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            if (otherValue == other.pImpl->entries.end()) {
                return false;
            }
            if (entry.value != otherValue->value) {
                return false;
            }
        }
        return true;
    }
    
    // Get configuration differences
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> diff(const Config& other) const {
        std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> differences;
        
        // Check keys in this config
        for (const auto& entry : pImpl->entries) {
            auto otherValue = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                          [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            if (otherValue != other.pImpl->entries.end()) {
                if (entry.value != otherValue->value) {
                    differences.emplace_back(entry.key, std::make_pair(entry.value, otherValue->value));
                }
            } else {
                differences.emplace_back(entry.key, std::make_pair(entry.value, ConfigValue(std::monostate{})));
            }
        }
        
        // Check keys only in other config
        for (const auto& entry : other.pImpl->entries) {
            auto thisValue = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                         [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            if (thisValue == pImpl->entries.end()) {
                differences.emplace_back(entry.key, std::make_pair(ConfigValue(std::monostate{}), entry.value));
            }
        }
        
        return differences;
    }
    
    // Apply configuration template
    void applyTemplate(const std::string& templateName) {
        static const std::unordered_map<std::string, std::vector<std::pair<std::string, ConfigValue>>> templates = {
            {"default", {
                {"brain.neuron_count", 1000LL},
                {"brain.synapse_density", 0.1},
                {"brain.connection_probability", 0.05},
                {"plasticity.stdp.enable", true},
                {"plasticity.stdp.learning_rate", 0.001},
                {"plasticity.hebbian.enable", true},
                {"plasticity.structural.enable", true},
                {"neuromod.dopamine.scale", 1.0},
                {"neuromod.curiosity.enable", true},
                {"neuromod.novelty.enable", true},
                {"development.initial_plasticity", 1.0},
                {"development.critical_period", 1000}
            }},
            {"high_performance", {
                {"brain.neuron_count", 50000LL},
                {"brain.synapse_density", 0.12},
                {"brain.connection_probability", 0.06},
                {"plasticity.stdp.enable", true},
                {"plasticity.stdp.learning_rate", 0.0008},
                {"plasticity.structural.enable", true},
                {"neuromod.curiosity.enable", true},
                {"neuromod.novelty.enable", true},
                {"development.initial_plasticity", 1.5},
                {"development.critical_period", 2000},
                {"memory.working_memory.capacity", 5000},
                {"memory.episodic_memory.capacity", 500}
            }},
            {"debug", {
                {"brain.neuron_count", 100LL},
                {"brain.synapse_density", 0.05},
                {"brain.connection_probability", 0.03},
                {"plasticity.stdp.enable", true},
                {"plasticity.stdp.learning_rate", 0.01},
                {"neuromod.curiosity.enable", true},
                {"simulation.dt", 0.001},
                {"simulation.max_steps", 500},
                {"logging.level", "debug"}
            }}
        };
        
        auto it = templates.find(templateName);
        if (it == templates.end()) {
            NLM_LOG_WARNING("Unknown configuration template: " + templateName);
            return;
        }
        
        // Clear existing configuration (except source)
        std::vector<ConfigEntry> newEntries;
        for (const auto& entry : it->second) {
            newEntries.emplace_back(entry.first, entry.second, ConfigSource::Default);
        }
        pImpl->entries = std::move(newEntries);
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
