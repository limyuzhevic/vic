#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string configStats;
    bool validationEnabled;
    std::vector<std::string> validationErrors;
    std::vector<std::string> validationWarnings;
    std::string statistics;
    
    // Configuration history
    std::chrono::system_clock::time_point creationTime;
    std::chrono::system_clock::time_point lastModifiedTime;
    std::string modificationHistory;
    
    Impl() : configStats(), validationEnabled(false), 
             creationTime(std::chrono::system_clock::now()),
             lastModifiedTime(std::chrono::system_clock::now()) {}
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
        if (arg.find("--") == 0) {
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

bool Config::validate(const std::vector<std::string>& requiredKeys) const {
    return validateConfig(requiredKeys);
}

bool Config::validateConfig(const std::vector<std::string>& requiredKeys) const {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Check required keys
    for (const auto& key : requiredKeys) {
        if (!hasKey(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate parameter ranges based on common NLM parameters
    auto validateParameter = [&errors, &warnings](const std::string& key, 
                                                   double value, double min, double max, 
                                                   const std::string& paramName) {
        if (value < min) {
            warnings.push_back("Parameter " + paramName + " (" + key + ") is below recommended minimum: " + 
                             std::to_string(value) + " < " + std::to_string(min));
        }
        if (value > max) {
            errors.push_back("Parameter " + paramName + " (" + key + ") exceeds maximum allowed value: " + 
                           std::to_string(value) + " > " + std::to_string(max));
        }
    };
    
    // Validate common NLM configuration parameters
    if (hasKey("simulation_timestep")) {
        double timestep = getOr<double>("simulation_timestep", 0.001);
        validateParameter("simulation_timestep", timestep, 0.000001, 0.1, 
                         "simulation timestep");
    }
    
    if (hasKey("neuron_count")) {
        double neuronCount = getOr<double>("neuron_count", 1000);
        validateParameter("neuron_count", neuronCount, 10, 100000, 
                         "neuron count");
    }
    
    if (hasKey("connection_probability")) {
        double connProb = getOr<double>("connection_probability", 0.1);
        validateParameter("connection_probability", connProb, 0.0, 1.0, 
                         "connection probability");
    }
    
    if (hasKey("random_seed")) {
        double seed = getOr<double>("random_seed", 42);
        if (seed < 0) {
            errors.push_back("Random seed must be non-negative: " + std::to_string(seed));
        }
    }
    
    // Validate plasticity parameters
    if (hasKey("plasticity_learning_rate")) {
        double learningRate = getOr<double>("plasticity_learning_rate", 0.01);
        validateParameter("plasticity_learning_rate", learningRate, 0.0, 1.0, 
                         "plasticity learning rate");
    }
    
    if (hasKey("stdp_ltp_weight")) {
        double ltpWeight = getOr<double>("stdp_ltp_weight", 0.01);
        validateParameter("stdp_ltp_weight", ltpWeight, -1.0, 1.0, 
                         "STDP LTP weight");
    }
    
    if (hasKey("stdp_ltd_weight")) {
        double ltdWeight = getOr<double>("stdp_ltd_weight", 0.012);
        validateParameter("stdp_ltd_weight", ltdWeight, -1.0, 1.0, 
                         "STDP LTD weight");
    }
    
    // Generate statistics
    std::ostringstream stats;
    stats << "Configuration Analysis:" << std::endl;
    stats << "  Total parameters: " << getKeys().size() << std::endl;
    stats << "  Source distribution: ";
    // Count by source type (simplified)
    size_t defaultCount = 0, fileCount = 0, cliCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: cliCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    stats << "Default: " << defaultCount << ", File: " << fileCount 
          << ", CLI: " << cliCount << ", Runtime: " << runtimeCount << std::endl;
    
    // Store statistics
    setStatistics(stats.str());
    
    // Clear existing validation results
    clearValidationErrors();
    clearValidationWarnings();
    
    // Set new validation results
    for (const auto& error : errors) {
        setValidationEnabled(true);
        validationErrors_.push_back(error);
    }
    
    for (const auto& warning : warnings) {
        setValidationEnabled(true);
        validationWarnings_.push_back(warning);
    }
    
    // Return true if no errors
    return errors.empty();
}
