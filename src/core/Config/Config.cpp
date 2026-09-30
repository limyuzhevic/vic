#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

// Enhanced configuration key enumeration
enum class ConfigKey {
    // Basic configuration
    RandomSeed,
    SimulationTimestep,
    NeuronCount,
    RegionCount,
    ConnectionProbability,
    
    // Plasticity configuration
    PlasticityLearningRate,
    StdpLtpWeight,
    StdpLtdWeight,
    StdpTimeConstant,
    SynaptogenesisRate,
    PruningRate,
    PlasticityRule,
    SynapticPruningRate,
    
    // Development configuration
    DevelopmentalStage,
    
    // Neuromodulation configuration
    NeuromodulationLevel,
    DopamineBaseline,
    
    // Memory system configuration
    MemorySystemType,
    
    // Output configuration
    OutputFormat,
    OutputInterval,
    StatsFile,
    PlotFile,
    
    // Simulation control
    Steps,
    Duration,
    SkipLearning,
    FastMode,
    
    // Monitoring and integration
    EnableMonitoring,
    CheckpointInterval,
    MonitorFile,
    
    // Benchmarking
    EnableBenchmarking,
    BenchmarkReportFile
};

// Configuration key to string mapping
inline const std::string configKeyToString(ConfigKey key) {
    switch (key) {
        case ConfigKey::RandomSeed: return "random_seed";
        case ConfigKey::SimulationTimestep: return "simulation_timestep";
        case ConfigKey::NeuronCount: return "neuron_count";
        case ConfigKey::RegionCount: return "region_count";
        case ConfigKey::ConnectionProbability: return "connection_probability";
        case ConfigKey::PlasticityLearningRate: return "plasticity_learning_rate";
        case ConfigKey::StdpLtpWeight: return "stdp_ltp_weight";
        case ConfigKey::StdpLtdWeight: return "stdp_ltd_weight";
        case ConfigKey::StdpTimeConstant: return "stdp_time_constant";
        case ConfigKey::SynaptogenesisRate: return "synaptogenesis_rate";
        case ConfigKey::PruningRate: return "pruning_rate";
        case ConfigKey::PlasticityRule: return "plasticity_rule";
        case ConfigKey::SynapticPruningRate: return "synaptic_pruning_rate";
        case ConfigKey::DevelopmentalStage: return "developmental_stage";
        case ConfigKey::NeuromodulationLevel: return "neuromodulation_level";
        case ConfigKey::DopamineBaseline: return "dopamine_baseline";
        case ConfigKey::MemorySystemType: return "memory_system_type";
        case ConfigKey::OutputFormat: return "output_format";
        case ConfigKey::OutputInterval: return "output_interval";
        case ConfigKey::StatsFile: return "stats_file";
        case ConfigKey::PlotFile: return "plot_file";
        case ConfigKey::Steps: return "steps";
        case ConfigKey::Duration: return "duration";
        case ConfigKey::SkipLearning: return "skip_learning";
        case ConfigKey::FastMode: return "fast_mode";
        case ConfigKey::EnableMonitoring: return "enable_monitoring";
        case ConfigKey::CheckpointInterval: return "checkpoint_interval";
        case ConfigKey::MonitorFile: return "monitor_file";
        case ConfigKey::EnableBenchmarking: return "enable_benchmarking";
        case ConfigKey::BenchmarkReportFile: return "benchmark_report_file";
        default: return "unknown";
    }
}

// String to configuration value parser
inline std::optional<ConfigValue> parseStringToConfigValue(const std::string& key, const std::string& value) {
    // Try to parse based on key patterns
    if (key == "random_seed" || key == "region_count" || key == "steps") {
        try {
            return std::stoi(value);
        } catch (...) {
            return std::nullopt;
        }
    } else if (key == "neuron_count") {
        try {
            return static_cast<int64_t>(std::stoll(value));
        } catch (...) {
            return std::nullopt;
        }
    } else if (key == "simulation_timestep" || key == "output_interval" || 
               key == "duration" || key == "checkpoint_interval" ||
               key == "synaptic_pruning_rate" || key == "plasticity_learning_rate" ||
               key == "stdp_ltp_weight" || key == "stdp_ltd_weight" ||
               key == "stdp_time_constant" || key == "synaptogenesis_rate" ||
               key == "pruning_rate" || key == "dopamine_baseline" ||
               key == "neuromodulation_level") {
        try {
            return std::stod(value);
        } catch (...) {
            return std::nullopt;
        }
    } else if (key == "connection_probability" || 
               key == "skip_learning" || key == "fast_mode" ||
               key == "enable_monitoring" || key == "enable_benchmarking") {
        // Convert string to boolean
        std::string lowerValue = value;
        std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
        if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes") {
            return true;
        } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no") {
            return false;
        }
        return std::nullopt;
    }
    
    // Default to string
    return value;
}

// Enhanced loadFromArgs implementation
bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse and set configuration
                auto config_value = parseStringToConfigValue(key, value);
                if (config_value.has_value()) {
                    set(key, config_value.value(), ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Special handling for boolean flags
            if (key == "skip-learning" || key == "fast-mode" || 
                key == "enable-monitoring") {
                set(key, true, ConfigSource::CommandLine);
            } else if (key == "plasticity-rule" || key == "memory-system" ||
                       key == "output-format" || key == "stats-file" ||
                       key == "plot-file" || key == "monitor-file" ||
                       key == "benchmark-report" || key == "brain-config" ||
                       key == "plasticity-config" || key == "developmental-stage") {
                set(key, value, ConfigSource::CommandLine);
            } else if (key == "steps" || key == "duration" ||
                       key == "checkpoint-interval" || key == "synaptic-pruning") {
                try {
                    if (key == "steps" || key == "checkpoint-interval") {
                        set(key, std::stoi(value), ConfigSource::CommandLine);
                    } else {
                        set(key, std::stod(value), ConfigSource::CommandLine);
                    }
                } catch (...) {
                    // Ignore conversion errors
                }
            }
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

} // namespace nlm
