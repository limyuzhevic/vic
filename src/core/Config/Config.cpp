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

// Validation implementation
Config::ValidationResult Config::validate(const std::vector<std::pair<std::string, std::string>>& rules) const {
    ValidationResult result;
    
    for (const auto& rule : rules) {
        const std::string& key = rule.first;
        const std::string& ruleType = rule.second;
        
        if (!has(key)) {
            result.addError("Missing configuration: " + key);
            continue;
        }
        
        if (ruleType == "positive") {
            auto val = get<double>(key);
            if (val.has_value() && val.value() <= 0.0) {
                result.addError("Configuration " + key + " must be positive");
            }
        } else if (ruleType == "non_negative") {
            auto val = get<double>(key);
            if (val.has_value() && val.value() < 0.0) {
                result.addError("Configuration " + key + " must be non-negative");
            }
        } else if (ruleType == "int_range") {
            // Parse range from rule key (format: key,min,max)
            // This is a simple implementation - in practice, you'd want better parsing
        }
    }
    
    return result;
}

Config::ValidationResult Config::validateValue(const std::string& key, const std::string& typeName) const {
    ValidationResult result;
    
    // Check if key exists
    if (!has(key)) {
        result.addError("Missing required configuration: " + key);
        return result;
    }
    
    // Type-specific validation would go here
    if (typeName == "int" || typeName == "int64_t") {
        auto val = get<int64_t>(key);
        if (val.has_value() && val.value() < 0) {
            result.addError("Invalid value for " + key + ": must be non-negative");
        }
    } else if (typeName == "double" || typeName == "float") {
        auto val = get<double>(key);
        if (val.has_value() && val.value() < 0.0) {
            result.addError("Invalid value for " + key + ": must be non-negative");
        }
    } else if (typeName == "bool") {
        // Boolean values are always valid
    }
    
    return result;
}

Config::ValidationResult Config::validateCommonConfigs(const std::vector<std::string>& configKeys) {
    ValidationResult result;
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "random_seed", "simulation_timestep", "neuron_count",
        "region_count", "connection_probability"
    };
    
    Config tempConfig;
    for (const auto& key : requiredKeys) {
        if (!tempConfig.has(key)) {
            result.addError("Missing required configuration: " + key);
        }
    }
    
    // Validate ranges for specific keys
    auto validateIntRange = [&result](const std::string& key, int64_t min, int64_t max) {
        // This is a simplified validation - in practice you'd want more robust checking
        Config tempConfig;
        tempConfig.set(key, int64_t(100), ConfigSource::Default); // Set a test value
        auto val = tempConfig.get<int64_t>(key);
        if (val.has_value() && (val.value() < min || val.value() > max)) {
            result.addError("Invalid value for " + key + ": must be between " + 
                           std::to_string(min) + " and " + std::to_string(max));
        }
    };
    
    validateIntRange("neuron_count", 1, 1000000);
    validateIntRange("region_count", 1, 1000);
    
    return result;
}

bool Config::isComplete() const {
    return validateCommonConfigs(getKeys()).valid;
}

std::vector<std::string> Config::getRequiredConfigKeys() {
    return {
        "random_seed", "simulation_timestep", "neuron_count",
        "region_count", "connection_probability"
    };
}

std::vector<std::string> Config::getValidationWarnings() const {
    std::vector<std::string> warnings;
    
    // Check for common configuration issues
    auto val = get<double>("simulation_timestep");
    if (val.has_value() && val.value() <= 0.0) {
        warnings.push_back("simulation_timestep should be positive");
    }
    
    val = get<int64_t>("neuron_count");
    if (val.has_value() && val.value() < 100) {
        warnings.push_back("neuron_count is very small, may affect learning");
    }
    
    return warnings;
}

// Configuration validation utility functions
namespace nlm {

// Common validation rules for NLM configuration
ValidationResult validateNLMConfig(const Config& config) {
    ValidationResult result;
    
    // Check required keys
    auto requiredKeys = Config::getRequiredConfigKeys();
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            result.addError("Missing required configuration: " + key);
        }
    }
    
    // Validate simulation timestep
    auto timestep = config.get<double>("simulation_timestep");
    if (timestep.has_value()) {
        if (timestep.value() <= 0.0) {
            result.addError("simulation_timestep must be positive");
        } else if (timestep.value() > 1.0) {
            result.addWarning("simulation_timestep is large, may affect simulation stability");
        }
    }
    
    // Validate neuron count
    auto neuronCount = config.get<int64_t>("neuron_count");
    if (neuronCount.has_value()) {
        if (neuronCount.value() <= 0) {
            result.addError("neuron_count must be positive");
        } else if (neuronCount.value() > 1000000) {
            result.addWarning("neuron_count is very large, may affect performance");
        }
    }
    
    // Validate connection probability
    auto connProb = config.get<double>("connection_probability");
    if (connProb.has_value()) {
        if (connProb.value() < 0.0 || connProb.value() > 1.0) {
            result.addError("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    // Validate plasticity parameters
    auto stdpLTP = config.get<double>("stdp_ltp_weight");
    if (stdpLTP.has_value() && stdpLTP.value() <= 0.0) {
        result.addWarning("stdp_ltp_weight should be positive for LTP");
    }
    
    auto stdpLTD = config.get<double>("stdp_ltd_weight");
    if (stdpLTD.has_value() && stdpLTD.value() <= 0.0) {
        result.addWarning("stdp_ltd_weight should be positive for LTD");
    }
    
    return result;
}

// Configuration validation report
class ConfigValidationReport {
public:
    ValidationResult validationResult;
    std::string report;
    
    void generateReport(const Config& config) {
        std::ostringstream oss;
        
        oss << "=== NLM Configuration Validation Report ===\n";
        oss << "Generated at: " << std::time(nullptr) << "\n\n";
        
        // Summary
        oss << "Configuration Summary:";
        oss << "  Total entries: " << config.getKeys().size() << "\n";
        oss << "  Complete: " << (config.isComplete() ? "YES" : "NO") << "\n\n";
        
        // Errors
        if (!validationResult.errors.empty()) {
            oss << "ERRORS:";
            for (const auto& error : validationResult.errors) {
                oss << "  - " << error << "\n";
            }
            oss << "\n";
        } else {
            oss << "✓ No configuration errors found\n\n";
        }
        
        // Warnings
        if (!validationResult.warnings.empty()) {
            oss << "WARNINGS:";
            for (const auto& warning : validationResult.warnings) {
                oss << "  - " << warning << "\n";
            }
            oss << "\n";
        }
        
        // Recommendations
        oss << "Recommendations:";
        if (!config.has("environment_name")) {
            oss << "  - Consider setting 'environment_name' for world simulation\n";
        }
        if (!config.has("visualization_enabled")) {
            oss << "  - Consider enabling 'visualization_enabled' for debugging\n";
        }
        oss << "\n";
        
        // Current configuration values
        oss << "Current Configuration Values:";
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            oss << "  " << key << ": ";
            if (config.has(key)) {
                auto val = config.get<std::string>(key);
                if (val.has_value()) {
                    oss << val.value() << "\n";
                }
            }
        }
        
        report = oss.str();
    }
    
    void saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << report;
            file.close();
        }
    }
};

// Performance monitoring utilities
namespace nlm {

// Performance metrics tracking
struct PerformanceMetrics {
    double totalSimulationTime = 0.0;      // Total time spent in simulation steps
    double averageStepTime = 0.0;         // Average time per simulation step
    size_t totalSpikeCount = 0;            // Total spikes generated
    double spikeRate = 0.0;                // Spikes per second
    size_t totalNeuronCount = 0;          // Total neurons in system
    size_t activeNeuronCount = 0;         // Number of active neurons
    double averageFiringRate = 0.0;       // Average firing rate across all neurons
    size_t plasticityEvents = 0;          // Number of plasticity events
    double structuralPlasticityRate = 0.0; // Rate of structural changes
    size_t memoryUsage = 0;               // Approximate memory usage (bytes)
    
    PerformanceMetrics() {}
    
    void update(size_t stepCount, double stepTime, size_t spikes, 
                size_t totalNeurons, size_t activeNeurons,
                double firingRate, size_t plasticEvents) {
        totalSimulationTime += stepTime * stepCount;
        averageStepTime = (averageStepTime * (stepCount - 1) + stepTime) / stepCount;
        totalSpikeCount += spikes;
        spikeRate = (totalSpikeCount / totalSimulationTime) * 1000.0; // Spikes per ms
        totalNeuronCount = totalNeurons;
        activeNeuronCount = activeNeurons;
        averageFiringRate = firingRate;
        plasticityEvents += plasticEvents;
        // Estimate structural plasticity rate
        structuralPlasticityRate = (plasticityEvents / totalSimulationTime) * 1000.0;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Performance Metrics:\n";
        oss << "  Simulation:\n";
        oss << "    Total time: " << totalSimulationTime << "s\n";
        oss << "    Average step time: " << (averageStepTime * 1000) << "ms\n";
        oss << "    Steps per second: " << (1000.0 / (averageStepTime * 1000)) << "\n";
        oss << "  Spike activity:\n";
        oss << "    Total spikes: " << totalSpikeCount << "\n";
        oss << "    Spike rate: " << spikeRate << " spikes/ms\n";
        oss << "    Firing rate: " << averageFiringRate << " Hz\n";
        oss << "  System state:\n";
        oss << "    Total neurons: " << totalNeuronCount << "\n";
        oss << "    Active neurons: " << activeNeuronCount << "\n";
        oss << "    Utilization: " << (activeNeuronCount * 100.0 / std::max(totalNeuronCount, size_t(1))) << "%\n";
        oss << "  Plasticity:\n";
        oss << "    Events: " << plasticityEvents << "\n";
        oss << "    Rate: " << structuralPlasticityRate << " events/ms\n";
        oss << "  Memory:\n";
        oss << "    Estimated usage: " << (memoryUsage / 1024) << " KB\n";
        return oss.str();
    }
};

// Performance monitoring class for NLM brain
class PerformanceMonitor {
private:
    PerformanceMetrics metrics;
    bool enabled;
    size_t sampleInterval;
    size_t sampleCount;
    std::vector<double> stepTimeHistory;
    
public:
    PerformanceMonitor() : enabled(false), sampleInterval(100), sampleCount(0) {}
    
    void setEnabled(bool enable) {
        enabled = enable;
        if (!enable) {
            reset();
        }
    }
    
    void setSampleInterval(size_t interval) {
        sampleInterval = interval;
    }
    
    void recordStep(double stepTime, size_t spikes, size_t totalNeurons,
                    size_t activeNeurons, double firingRate,
                    size_t plasticEvents) {
        if (!enabled) return;
        
        metrics.update(1, stepTime, spikes, totalNeurons, activeNeurons,
                      firingRate, plasticEvents);
        
        sampleCount++;
        if (sampleCount % sampleInterval == 0) {
            stepTimeHistory.push_back(stepTime);
            if (stepTimeHistory.size() > 1000) {
                stepTimeHistory.erase(stepTimeHistory.begin());
            }
        }
    }
    
    PerformanceMetrics getMetrics() const {
        return metrics;
    }
    
    double getAverageStepTime() const {
        if (sampleCount == 0) return 0.0;
        double totalTime = 0.0;
        for (double time : stepTimeHistory) {
            totalTime += time;
        }
        return (stepTimeHistory.empty() ? 0.0 : totalTime / stepTimeHistory.size());
    }
    
    double getStabilityIndex() const {
        // Calculate coefficient of variation of step times
        if (stepTimeHistory.size() < 2) return 1.0;
        
        double mean = getAverageStepTime();
        double variance = 0.0;
        for (double time : stepTimeHistory) {
            double diff = time - mean;
            variance += diff * diff;
        }
        variance /= stepTimeHistory.size();
        
        double stddev = std::sqrt(variance);
        return (mean > 0.0) ? (stddev / mean) : 1.0;
    }
    
    std::string getPerformanceReport() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Monitor Report ===\n";
        oss << "Generated at: " << std::time(nullptr) << "\n\n";
        
        oss << "Performance Summary:";
        oss << "  Stability Index: " << getStabilityIndex() << " (lower is better)\n";
        oss << "  Step Time Variance: " << getAverageStepTime() * 1000 << "ms\n";
        oss << "  Samples Collected: " << sampleCount << "\n";
        oss << "  Monitoring Interval: " << sampleInterval << " steps\n\n";
        
        oss << metrics.toString() << "\n";
        
        // Performance recommendations
        oss << "Performance Recommendations:";
        if (getStabilityIndex() > 0.1) {
            oss << "  - Step time variance is high, consider optimizing plasticity calculations\n";
        }
        if (metrics.averageStepTime > 0.001) {  // > 1ms per step
            oss << "  - Step execution is slow, consider reducing plasticity event count\n";
        }
        if (metrics.spikeRate > 100.0) {  // > 100 spikes/ms
            oss << "  - High spike rate detected, may indicate excessive neuron excitation\n";
        }
        
        return oss.str();
    }
    
    void saveReportToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << getPerformanceReport();
            file.close();
        }
    }
    
    void reset() {
        metrics = PerformanceMetrics();
        sampleCount = 0;
        stepTimeHistory.clear();
    }
};

} // namespace nlm

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
