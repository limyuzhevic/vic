#include "../core/Logger/Logger.hpp"
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
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    size_t loadedCount = 0;
    size_t errorCount = 0;
    std::string line;
    size_t lineNumber = 0;
    
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
            
            try {
                set(key, value, ConfigSource::File);
                loadedCount++;
            } catch (const std::exception& e) {
                errorCount++;
                NLM_LOG_ERROR("Failed to parse configuration at line " + std::to_string(lineNumber) + ": " + e.what());
            }
        } else {
            errorCount++;
            NLM_LOG_ERROR("Invalid configuration format at line " + std::to_string(lineNumber) + ": missing '='");
        }
    }
    
    if (errorCount > 0) {
        NLM_LOG_WARNING("Configuration loaded with " + std::to_string(errorCount) + " errors. Loaded " + std::to_string(loadedCount) + " valid entries.");
    } else {
        NLM_LOG_INFO("Configuration loaded successfully: " + std::to_string(loadedCount) + " entries");
    }
    
    return loadedCount > 0 || errorCount == 0; // Return false only if there were errors and no entries loaded
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Processing command line arguments...");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                NLM_LOG_DEBUG("Command line: set key=\"" + key + "\" value=\"" + value + "\"");
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to set configuration key '" + key + "': " + e.what());
                }
            } else {
                NLM_LOG_WARNING("Invalid command line argument format (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            NLM_LOG_DEBUG("Command line: set key=\"" + key + "\" value=\"" + value + "\"");
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to set configuration key '" + key + "': " + e.what());
            }
        }
        // Handle special flags
        else if (arg == "--help" || arg == "-help") {
            NLM_LOG_INFO("NLM Help - Available configuration options:");
            NLM_LOG_INFO("  --help, -help: Show this help message");
            NLM_LOG_INFO("  --config=<file>: Load configuration from file");
            NLM_LOG_INFO("  --config <file>: Load configuration from file");
            NLM_LOG_INFO("  --set <key> <value>: Set configuration key");
            NLM_LOG_INFO("  --neuron_count <value>: Set number of neurons");
            NLM_LOG_INFO("  --region_count <value>: Set number of regions");
            NLM_LOG_INFO("  --log_level <level>: Set log level (Debug, Info, Warning, Error, Critical)");
            NLM_LOG_INFO("  --random_seed <seed>: Set random seed for reproducible experiments");
            NLM_LOG_INFO("  --simulation_timestep <value>: Set simulation timestep");
            NLM_LOG_INFO("  --help-all: Show all available configuration parameters");
        }
        else if (arg == "--help-all") {
            NLM_LOG_INFO("=== NLM Configuration Help (All Parameters) ===");
            NLM_LOG_INFO("Basic Parameters:");
            NLM_LOG_INFO("  neuron_count: Number of neurons in the brain");
            NLM_LOG_INFO("  region_count: Number of brain regions");
            NLM_LOG_INFO("  connection_probability: Probability of synaptic connections");
            NLM_LOG_INFO("  random_seed: Random seed for reproducible experiments");
            NLM_LOG_INFO("  simulation_timestep: Simulation timestep in seconds");
            
            NLM_LOG_INFO("\\nPlasticity Parameters:");
            NLM_LOG_INFO("  stdp_ltp_weight: STDP long-term potentiation weight");
            NLM_LOG_INFO("  stdp_ltd_weight: STDP long-term depression weight");
            NLM_LOG_INFO("  stdp_time_constant: STDP time constant");
            NLM_LOG_INFO("  synaptogenesis_rate: Rate of new synapse formation");
            NLM_LOG_INFO("  pruning_rate: Rate of synapse elimination");
            
            NLM_LOG_INFO("\\nDevelopment Parameters:");
            NLM_LOG_INFO("  development_synaptogenesis_rate: Developmental synaptogenesis rate");
            NLM_LOG_INFO("  development_pruning_rate: Developmental pruning rate");
            
            NLM_LOG_INFO("\\nNeuromodulation Parameters:");
            NLM_LOG_INFO("  dopamine_baseline: Baseline dopamine level");
            NLM_LOG_INFO("  reward_discount_factor: Reward discount factor");
            
            NLM_LOG_INFO("\\nSimulation Parameters:");
            NLM_LOG_INFO("  max_simulation_steps: Maximum simulation steps");
            NLM_LOG_INFO("  simulation_time_limit: Time limit for simulation (seconds)");
            NLM_LOG_INFO("  replay_interval: Memory replay interval (steps)");
            NLM_LOG_INFO("  consolidation_interval: Memory consolidation interval (steps)");
            
            NLM_LOG_INFO("\\nLogging Parameters:");
            NLM_LOG_INFO("  log_level: Logging level (Debug, Info, Warning, Error, Critical)");
            NLM_LOG_INFO("  log_to_file: Enable/disable logging to file");
            NLM_LOG_INFO("  log_filename: Log file name");
            
            NLM_LOG_INFO("\\nEnvironment Parameters:");
            NLM_LOG_INFO("  environment_name: Name of the environment (GridWorld, etc.)");
            NLM_LOG_INFO("  environment_width: Environment width");
            NLM_LOG_INFO("  environment_height: Environment height");
        }
        else if (arg == "--version") {
            NLM_LOG_INFO("NLM (Neural Learning Machine) Phase 6 - Version 1.0.0");
            NLM_LOG_INFO("Integrated Artificial Brain System");
        }
        else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Saving configuration to: " + filepath);
    
    size_t savedCount = 0;
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string
        std::string valueStr;
        try {
            if (std::holds_alternative<int>(entry.value)) {
                valueStr = std::to_string(std::get<int>(entry.value));
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                valueStr = std::to_string(std::get<int64_t>(entry.value));
            } else if (std::holds_alternative<double>(entry.value)) {
                valueStr = std::to_string(std::get<double>(entry.value));
            } else if (std::holds_alternative<bool>(entry.value)) {
                valueStr = std::get<bool>(entry.value) ? "true" : "false";
            } else if (std::holds_alternative<std::string>(entry.value)) {
                valueStr = "\"" + std::get<std::string>(entry.value) + "\"";
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                // For vector types, just show count
                const auto& vec = std::get<std::vector<int>>(entry.value);
                valueStr = "[" + std::to_string(vec.size()) + " integers]";
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                const auto& vec = std::get<std::vector<double>>(entry.value);
                valueStr = "[" + std::to_string(vec.size()) + " floats]";
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                const auto& vec = std::get<std::vector<std::string>>(entry.value);
                valueStr = "[" + std::to_string(vec.size()) + " strings]";
            } else {
                valueStr = "UNKNOWN_TYPE";
            }
            
            file << entry.key << " = " << valueStr << "\n";
            savedCount++;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to save configuration entry '" + entry.key + "': " + e.what());
            continue; // Skip this entry but continue saving others
        }
    }
    
    NLM_LOG_INFO("Configuration saved successfully: " + std::to_string(savedCount) + " entries");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Configuration type mismatch for key '" + key + "': expected " + typeid(T).name());
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
    bool result = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!result) {
        NLM_LOG_DEBUG("Configuration key not found: " + key);
    }
    
    return result;
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
    
    // Log summary if we have a logger
    NLM_LOG_INFO("Configuration summary: " + std::to_string(pImpl->entries.size()) + " entries loaded");
    
    return oss.str();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigValue();
    }
    
    return it->value;
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
// Update the getSource and getValue methods

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigValue();
    }
    
    return it->value;
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
