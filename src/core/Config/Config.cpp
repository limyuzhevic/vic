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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + 
                           " in " + filepath + ": " + line);
            continue;
        }
        
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
    
    NLM_LOG_INFO("Loaded configuration from " + filepath + ": " + 
                 std::to_string(pImpl->entries.size()) + " entries");
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

// Implement new advanced configuration features

bool Config::validate() const {
    // Basic validation - check for required parameters
    static const std::vector<std::string> requiredParams = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep"
    };
    
    std::string errors = validateAndGetErrors();
    return errors.empty();
}

std::string Config::validateAndGetErrors() const {
    std::string errors;
    
    // Check for required parameters
    static const std::vector<std::string> requiredParams = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep"
    };
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            if (!errors.empty()) errors += "\n";
            errors += "Missing required parameter: '" + param + "'";
        } else {
            // Validate parameter values
            if (!isParameterValid(param, pImpl->entries)) {
                if (!errors.empty()) errors += "\n";
                errors += "Invalid value for parameter: '" + param + "'";
            }
        }
    }
    
    return errors;
}

Config Config::createQuickTestPreset() {
    Config config;
    config.set("neuron_count", 100, ConfigSource::Default);
    config.set("region_count", 1, ConfigSource::Default);
    config.set("connection_probability", 0.15, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("random_seed", 42, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.02, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015, ConfigSource::Default);
    config.set("stdp_tau", 20.0, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
    config.set("pruning_rate", 0.00001, ConfigSource::Default);
    
    return config;
}

Config Config::createPerformancePreset() {
    Config config;
    config.set("neuron_count", 500, ConfigSource::Default);
    config.set("region_count", 1, ConfigSource::Default);
    config.set("connection_probability", 0.1, ConfigSource::Default);
    config.set("simulation_timestep", 0.01, ConfigSource::Default);
    config.set("random_seed", 42, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012, ConfigSource::Default);
    config.set("stdp_tau", 20.0, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.00001, ConfigSource::Default);
    config.set("pruning_rate", 0.000001, ConfigSource::Default);
    config.set("working_memory_capacity", 50, ConfigSource::Default);
    config.set("episodic_memory_limit", 500, ConfigSource::Default);
    
    return config;
}

Config Config::createResearchPreset() {
    Config config;
    config.set("neuron_count", 5000, ConfigSource::Default);
    config.set("region_count", 2, ConfigSource::Default);
    config.set("connection_probability", 0.05, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("random_seed", 12345, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.03, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.025, ConfigSource::Default);
    config.set("stdp_tau", 25.0, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.00005, ConfigSource::Default);
    config.set("pruning_rate", 0.000005, ConfigSource::Default);
    config.set("working_memory_capacity", 500, ConfigSource::Default);
    config.set("episodic_memory_limit", 5000, ConfigSource::Default);
    config.set("enable_curiosity", true, ConfigSource::Default);
    config.set("enable_novelty", true, ConfigSource::Default);
    config.set("enable_prediction", true, ConfigSource::Default);
    config.set("enable_planning", true, ConfigSource::Default);
    
    return config;
}

Config Config::createVisualizationPreset() {
    Config config;
    config.set("neuron_count", 200, ConfigSource::Default);
    config.set("region_count", 1, ConfigSource::Default);
    config.set("connection_probability", 0.12, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("random_seed", 999, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.015, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.018, ConfigSource::Default);
    config.set("stdp_tau", 15.0, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.00002, ConfigSource::Default);
    config.set("pruning_rate", 0.000002, ConfigSource::Default);
    config.set("enable_checkpointing", true, ConfigSource::Default);
    config.set("checkpoint_path", "./checkpoints", ConfigSource::Default);
    config.set("enable_replay", true, ConfigSource::Default);
    config.set("replay_interval", 100, ConfigSource::Default);
    
    return config;
}

bool Config::validateNeuronCount(size_t count) {
    return count >= 10 && count <= 100000;
}

bool Config::validateSynapseDensity(double density) {
    return density >= 0.0 && density <= 1.0;
}

bool Config::validateLearningRate(double rate) {
    return rate >= 0.0 && rate <= 1.0;
}

bool Config::validateMemoryCapacity(size_t capacity) {
    return capacity <= 1000000; // Reasonable upper bound
}

std::pair<size_t, size_t> Config::getNeuronCountRange() {
    return std::make_pair(10, 100000);
}

std::pair<double, double> Config::getSynapseDensityRange() {
    return std::make_pair(0.0, 1.0);
}

std::pair<double, double> Config::getLearningRateRange() {
    return std::make_pair(0.0, 1.0);
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

bool Config::isComplete() const {
    return validate();
}

std::vector<std::string> Config::getMissingParameters() const {
    std::vector<std::string> missing;
    
    static const std::vector<std::string> requiredParams = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep"
    };
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            missing.push_back(param);
        }
    }
    
    return missing;
}

void Config::initializeValidationRules() {
    // Initialize any validation rules or constraints
}

bool Config::validateParameter(const ConfigEntry& entry) const {
    return isParameterValid(entry.key, entry.value);
}

bool Config::isParameterValid(const std::string& key, const ConfigValue& value) {
    // Validate based on key name
    std::string lowerKey = toLower(key);
    
    if (lowerKey.find("neuron_count") != std::string::npos) {
        if (std::holds_alternative<size_t>(value)) {
            return validateNeuronCount(std::get<size_t>(value));
        } else if (std::holds_alternative<int>(value)) {
            return validateNeuronCount(static_cast<size_t>(std::get<int>(value)));
        }
    } else if (lowerKey.find("synapse_density") != std::string::npos || 
               lowerKey.find("connection_probability") != std::string::npos) {
        if (std::holds_alternative<double>(value)) {
            return validateSynapseDensity(std::get<double>(value));
        }
    } else if (lowerKey.find("learning_rate") != std::string::npos || 
               lowerKey.find("plasticity") != std::string::npos) {
        if (std::holds_alternative<double>(value)) {
            return validateLearningRate(std::get<double>(value));
        }
    } else if (lowerKey.find("memory") != std::string::npos || 
               lowerKey.find("capacity") != std::string::npos) {
        if (std::holds_alternative<size_t>(value)) {
            return validateMemoryCapacity(std::get<size_t>(value));
        } else if (std::holds_alternative<int>(value)) {
            return validateMemoryCapacity(static_cast<size_t>(std::get<int>(value)));
        }
    }
    
    // Default: parameter is valid if it can be retrieved without error
    return true;
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