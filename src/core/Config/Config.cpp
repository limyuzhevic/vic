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

bool Config::loadFromString(const std::string& configStr) {
    // Parse JSON format configuration
    // Simple JSON parser for basic key-value pairs
    size_t pos = 0;
    while (pos < configStr.size()) {
        // Find opening brace or quote
        if (configStr[pos] == '{' || configStr[pos] == '"') {
            // Find closing brace or quote
            size_t endPos = configStr.find_first_of('}', '"', pos + 1);
            if (endPos == std::string::npos) break;
            
            // Extract content between braces/quotes
            std::string content = configStr.substr(pos + 1, endPos - pos - 1);
            
            // Try to parse as key=value pair
            size_t equalPos = content.find('=');
            if (equalPos != std::string::npos) {
                std::string key = trim(content.substr(0, equalPos));
                std::string value = trim(content.substr(equalPos + 1));
                
                // Remove surrounding quotes from value
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        pos = configStr.find_first_not_of(' ', endPos + 1);
        if (pos == std::string::npos) break;
    }
    
    return true;
}

std::unordered_map<std::string, std::string> Config::to_dict() const {
    std::unordered_map<std::string, std::string> result;
    for (const auto& entry : pImpl->entries) {
        result[entry.key] = entry.value;
    }
    return result;
}

bool Config::from_dict(const std::unordered_map<std::string, std::string>& configDict) {
    clear();
    for (const auto& pair : configDict) {
        set(pair.first, pair.second, ConfigSource::Default);
    }
    return true;
}

bool Config::validate(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    return it != pImpl->entries.end();
}

std::vector<std::string> Config::getValidKeys() const {
    std::vector<std::string> validKeys;
    
    // Define valid configuration keys based on system requirements
    std::vector<std::string> systemKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
        "synaptogenesis_rate", "pruning_rate", "dopamine_baseline", "curiosity_baseline",
        "reward_discount_factor", "environment_name", "environment_width", "environment_height",
        "log_level", "log_to_file", "log_filename", "max_simulation_steps",
        "simulation_time_limit", "visualization_enabled", "visualization_update_rate"
    };
    
    // Phase 6 specific keys
    std::vector<std::string> phase6Keys = {
        "max_steps", "neuron_count", "region_count", "connection_probability",
        "enable_checkpointing", "enable_replay", "enable_development",
        "checkpoint_path", "novelty_threshold", "curiosity_threshold",
        "prediction_confidence", "memory_decay_rate"
    };
    
    // Add all valid keys
    validKeys.insert(validKeys.end(), systemKeys.begin(), systemKeys.end());
    validKeys.insert(validKeys.end(), phase6Keys.begin(), phase6Keys.end());
    
    // Sort and remove duplicates
    std::sort(validKeys.begin(), validKeys.end());
    validKeys.erase(std::unique(validKeys.begin(), validKeys.end()), validKeys.end());
    
    return validKeys;
}

std::string Config::generateReport() const {
    std::stringstream report;
    report << "=== NLM Configuration Report ===\n\n";
    report << "Total configuration entries: " << pImpl->entries.size() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        report << "Key: " << entry.key << "\n";
        report << "Value: " << entry.value << "\n";
        report << "Source: " << configSourceToString(entry.source) << "\n";
        if (!entry.description.empty()) {
            report << "Description: " << entry.description << "\n";
        }
        report << "\n";
    }
    
    report << "=== End Configuration Report ===\n";
    return report.str();
}

// Helper function to trim whitespace
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// Helper function to convert ConfigSource to string
std::string Config::configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        default: return "Unknown";
    }
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
