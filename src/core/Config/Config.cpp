#include "Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cerrno>
#include <cstring>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Known configuration keys with descriptions and default values for validation
    std::unordered_map<std::string, std::string> keyDescriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize known configuration keys with descriptions
    pImpl->keyDescriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Plasticity learning rate"},
        {"stdp_ltp_weight", "STDP LTP weight"},
        {"stdp_ltd_weight", "STDP LTD weight"},
        {"stdp_time_constant", "STDP time constant"},
        {"dopamine_baseline", "Baseline dopamine level"},
        {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
        {"development_pruning_rate", "Development pruning rate"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level (DEBUG, INFO, WARNING, ERROR)"},
        {"log_to_file", "Log to file (true/false)"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Simulation time limit"},
        {"visualization_enabled", "Visualization enabled (true/false)"},
        {"visualization_update_rate", "Visualization update rate"},
        {"synaptogenesis_rate", "Synaptogenesis rate (Phase 2+)"},
        {"pruning_rate", "Pruning rate (Phase 2+)"},
        {"replay_interval", "Memory replay interval (Phase 2+)"},
        {"consolidation_interval", "Memory consolidation interval (Phase 2+)"},
        {"checkpoint_dir", "Checkpoint directory"}
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists first
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    // Check if file is readable
    if (!std::filesystem::is_regular_file(filepath) || !std::filesystem::status(filepath).permissions() & std::filesystem::perms::owner_read) {
        NLM_LOG_ERROR("Configuration file is not readable: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath + " (" + std::strerror(errno) + ")");
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration format at line " + std::to_string(lineNumber) + ": missing '='");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Empty configuration key at line " + std::to_string(lineNumber));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Check for unknown keys (warning, not error)
        if (pImpl->keyDescriptions.find(key) == pImpl->keyDescriptions.end()) {
            NLM_LOG_WARNING("Unknown configuration key \"" + key + "\" at line " + std::to_string(lineNumber));
        }
        
        set(key, value, ConfigSource::File);
        NLM_LOG_DEBUG("Loaded config \"" + key + "\" = \"" + value + "\" from file (line " + std::to_string(lineNumber) + ")");
    }
    
    int entryCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::File) {
            ++entryCount;
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(entryCount) + " configuration entries from file: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Known argument keys for validation
    std::unordered_set<std::string> knownKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "plasticity_learning_rate", "stdp_ltp_weight",
        "stdp_ltd_weight", "stdp_time_constant", "dopamine_baseline",
        "development_synaptogenesis_rate", "development_pruning_rate",
        "reward_discount_factor", "environment_name", "environment_width",
        "environment_height", "log_level", "log_to_file", "log_filename",
        "max_simulation_steps", "simulation_time_limit", "visualization_enabled",
        "visualization_update_rate", "synaptogenesis_rate", "pruning_rate",
        "replay_interval", "consolidation_interval", "checkpoint_dir"
    };
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty command line key in argument: " + arg);
                    continue;
                }
                
                // Validate key
                if (knownKeys.find(key) == knownKeys.end()) {
                    NLM_LOG_WARNING("Unknown command line key \"" + key + "\": \"" + value + "\"");
                }
                
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Set config \"" + key + "\" = \"" + value + "\" from command line");
            } else {
                NLM_LOG_WARNING("Invalid command line argument format (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty command line key in argument: -" + arg);
                continue;
            }
            
            // Validate key
            if (knownKeys.find(key) == knownKeys.end()) {
                NLM_LOG_WARNING("Unknown command line key \"" + key + "\": \"" + value + "\"");
            }
            
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_DEBUG("Set config \"" + key + "\" = \"" + value + "\" from command line");
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Check if we can write to the target location
    std::error_code ec;
    if (std::filesystem::exists(filepath) && !std::filesystem::is_regular_file(filepath)) {
        NLM_LOG_ERROR("Cannot save configuration: path exists but is not a regular file: " + filepath);
        return false;
    }
    
    // Try to create or overwrite the file
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath + " (" + std::strerror(errno) + ")");
        return false;
    }
    
    // Check if we have write permissions
    if (!file.good()) {
        NLM_LOG_ERROR("Configuration file is not writable: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        std::string description = entry.description.empty() ? 
            (pImpl->keyDescriptions.find(entry.key) != pImpl->keyDescriptions.end() ? 
                pImpl->keyDescriptions.at(entry.key) : "Configuration parameter") :
            entry.description;
        
        file << "# " << description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    if (!file.good()) {
        NLM_LOG_ERROR("Failed to write configuration data to file: " + filepath);
        return false;
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
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
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Count entries by source
    int fileCount = 0, commandLineCount = 0, defaultCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    oss << "Sources:\n";
    oss << "  File: " << fileCount << "\n";
    oss << "  Command Line: " << commandLineCount << "\n";
    oss << "  Default: " << defaultCount << "\n";
    oss << "  Runtime: " << runtimeCount << "\n\n";
    
    oss << "Entries by key:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        // Add description if available
        if (pImpl->keyDescriptions.find(entry.key) != pImpl->keyDescriptions.end()) {
            oss << " - " << pImpl->keyDescriptions.at(entry.key);
        } else if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        
        oss << "\n";
    }
    
    // Check for overrides
    oss << "\nConfiguration validation:\n";
    oss << "  File entries: " << fileCount << " (highest priority)\n";
    oss << "  Command line: " << commandLineCount << " (medium priority)\n";
    oss << "  Default: " << defaultCount << " (low priority)\n";
    oss << "  Runtime: " << runtimeCount << " (overridable)\n";
    
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
