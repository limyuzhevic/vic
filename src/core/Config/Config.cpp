#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Configuration validation functions
namespace {
    bool validateNeuronCount(int64_t count) {
        return count > 0 && count <= 10000;
    }
    
    bool validateRegionCount(int64_t count) {
        return count > 0 && count <= 100;
    }
    
    bool validateConnectionProbability(float prob) {
        return prob >= 0.0f && prob <= 1.0f;
    }
    
    bool validateTimestep(double timestep) {
        return timestep > 0.0 && timestep <= 1.0;
    }
    
    bool validatePlasticityLearningRate(float rate) {
        return rate >= 0.0f && rate <= 1.0f;
    }
    
    bool validateWeight(float weight) {
        return weight >= -10.0f && weight <= 10.0f;
    }
    
    bool validateMaxSteps(uint64_t steps) {
        return steps > 0 && steps <= 1000000;
    }
    
    bool validateLogLevel(const std::string& level) {
        static const std::vector<std::string> validLevels = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
        return std::find(validLevels.begin(), validLevels.end(), level) != validLevels.end();
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    int validEntries = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
            validEntries++;
            
            // Validate configuration after loading
            validateConfiguration();
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath + " (" + std::to_string(validEntries) + " entries)");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                
                // Validate the argument was parsed correctly
                if (!has(key)) {
                    NLM_LOG_WARNING("Failed to parse command line argument: " + arg);
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Invalid command line argument format (missing '='): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            
            // Validate the argument was parsed correctly
            if (!has(key)) {
                NLM_LOG_WARNING("Failed to parse command line argument: -" + key + " " + value);
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            success = false;
        }
    }
    
    // Validate configuration after loading from command line
    validateConfiguration();
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config to file: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "[SAVED_VALUE]\n";
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
    return true;
}

// Configuration validation functions
void Config::validateConfiguration() {
    NLM_LOG_DEBUG("Validating configuration...");
    
    // Validate numeric ranges
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (neuronCount.has_value()) {
            if (!validateNeuronCount(neuronCount.value())) {
                NLM_LOG_WARNING("Invalid neuron_count: " + std::to_string(neuronCount.value()) + ". Must be between 1 and 10000");
            }
        }
    }
    
    if (has("region_count")) {
        auto regionCount = get<int64_t>("region_count");
        if (regionCount.has_value()) {
            if (!validateRegionCount(regionCount.value())) {
                NLM_LOG_WARNING("Invalid region_count: " + std::to_string(regionCount.value()) + ". Must be between 1 and 100");
            }
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = get<float>("connection_probability");
        if (connProb.has_value()) {
            if (!validateConnectionProbability(connProb.value())) {
                NLM_LOG_WARNING("Invalid connection_probability: " + std::to_string(connProb.value()) + ". Must be between 0.0 and 1.0");
            }
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep.has_value()) {
            if (!validateTimestep(timestep.value())) {
                NLM_LOG_WARNING("Invalid simulation_timestep: " + std::to_string(timestep.value()) + ". Must be between 0.0 and 1.0");
            }
        }
    }
    
    if (has("max_simulation_steps")) {
        auto maxSteps = get<uint64_t>("max_simulation_steps");
        if (maxSteps.has_value()) {
            if (!validateMaxSteps(maxSteps.value())) {
                NLM_LOG_WARNING("Invalid max_simulation_steps: " + std::to_string(maxSteps.value()) + ". Must be between 1 and 1000000");
            }
        }
    }
    
    if (has("log_level")) {
        auto logLevel = get<std::string>("log_level");
        if (logLevel.has_value()) {
            if (!validateLogLevel(logLevel.value())) {
                NLM_LOG_WARNING("Invalid log_level: " + logLevel.value() + ". Must be one of: DEBUG, INFO, WARNING, ERROR, CRITICAL");
            }
        }
    }
    
    NLM_LOG_INFO("Configuration validation completed");
}

// Enhanced get methods with validation
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        
        // Validate the value if it's a known configuration parameter
        validateValueForKey(key, value);
        
        return value;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Configuration value validation based on key
template<typename T>
void Config::validateValueForKey(const std::string& key, const T& value) const {
    if (key == "neuron_count") {
        if (!validateNeuronCount(std::get<int64_t>(value))) {
            NLM_LOG_WARNING("Invalid neuron_count value in variant: " + std::to_string(std::get<int64_t>(value)));
        }
    } else if (key == "region_count") {
        if (!validateRegionCount(std::get<int64_t>(value))) {
            NLM_LOG_WARNING("Invalid region_count value in variant: " + std::to_string(std::get<int64_t>(value)));
        }
    } else if (key == "connection_probability") {
        if (!validateConnectionProbability(std::get<float>(value))) {
            NLM_LOG_WARNING("Invalid connection_probability value in variant: " + std::to_string(std::get<float>(value)));
        }
    } else if (key == "simulation_timestep") {
        if (!validateTimestep(std::get<double>(value))) {
            NLM_LOG_WARNING("Invalid simulation_timestep value in variant: " + std::to_string(std::get<double>(value)));
        }
    } else if (key == "max_simulation_steps") {
        if (!validateMaxSteps(std::get<uint64_t>(value))) {
            NLM_LOG_WARNING("Invalid max_simulation_steps value in variant: " + std::to_string(std::get<uint64_t>(value)));
        }
    } else if (key == "log_level") {
        if (!validateLogLevel(std::get<std::string>(value))) {
            NLM_LOG_WARNING("Invalid log_level value in variant: " + std::get<std::string>(value));
        }
    }
    // Add more validations as needed
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
