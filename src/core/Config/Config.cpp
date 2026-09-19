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
    // FIXED: Handle duplicate keys - later keys overwrite earlier ones
    
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
                
                // IMPROVEMENT: Validate command line arguments
                if (key == "neuron_count" || key == "random_seed" || 
                    key == "connection_probability" || key == "synaptogenesis_rate" || 
                    key == "pruning_rate" || key == "stdp_ltp_weight" || 
                    key == "stdp_ltd_weight" || key == "stdp_tau") {
                    try {
                        // Try to convert to float to validate it's a number
                        float floatValue = std::stof(value);
                        set(key, value, ConfigSource::CommandLine);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Config: Invalid numeric value for key '" + key + "': " + value);
                    }
                } else {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // IMPROVEMENT: Validate command line arguments
            if (key == "neuron_count" || key == "random_seed" || 
                key == "connection_probability" || key == "synaptogenesis_rate" || 
                key == "pruning_rate" || key == "stdp_ltp_weight" || 
                key == "stdp_ltd_weight" || key == "stdp_tau") {
                try {
                    float floatValue = std::stof(value);
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Config: Invalid numeric value for key '" + key + "': " + value);
                }
            } else {
                set(key, value, ConfigSource::CommandLine);
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
    // IMPROVEMENT: Add basic validation for critical keys
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) <= 0) {
            NLM_LOG_WARNING("Config: neuron_count must be positive, got " + std::to_string(std::get<int64_t>(value)));
        }
    } else if (key == "random_seed") {
        if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) < 0) {
            NLM_LOG_WARNING("Config: random_seed should be non-negative, got " + std::to_string(std::get<int64_t>(value)));
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    size_t beforeCount = pImpl->entries.size();
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    if (pImpl->entries.size() < beforeCount) {
        NLM_LOG_INFO("Config: Removed key '" + key + "'");
    } else {
        NLM_LOG_WARNING("Config: Key '" + key + "' not found for removal");
    }
}

// IMPROVEMENT: Add logging for config validation
void Config::logValidationReport() const {
    NLM_LOG_INFO("=== Configuration Validation Report ===");
    
    for (const auto& entry : pImpl->entries) {
        std::string typeStr;
        std::string valueStr;
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                typeStr = "string";
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                typeStr = "int64_t";
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                typeStr = "double";
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                typeStr = "bool";
                valueStr = arg ? "true" : "false";
            } else {
                typeStr = "unknown";
                valueStr = "...";
            }
        }, entry.value);
        
        NLM_LOG_INFO("  " + entry.key + ": " + valueStr + " (type: " + typeStr + 
                    ", source: " + std::to_string(static_cast<int>(entry.source)) + ")");
    }
    
    NLM_LOG_INFO("=== End Validation Report ===");
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

// IMPROVEMENT: Add configuration validation method
bool Config::validate() const {
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        const std::string& key = entry.key;
        const ConfigValue& value = entry.value;
        
        try {
            if (key == "neuron_count") {
                if (std::holds_alternative<int64_t>(value)) {
                    int64_t neuronCount = std::get<int64_t>(value);
                    if (neuronCount <= 0) {
                        NLM_LOG_ERROR("Config validation error: neuron_count must be positive, got " + 
                                    std::to_string(neuronCount));
                        isValid = false;
                    }
                }
            } else if (key == "random_seed") {
                if (std::holds_alternative<int64_t>(value)) {
                    int64_t seed = std::get<int64_t>(value);
                    if (seed < 0) {
                        NLM_LOG_ERROR("Config validation error: random_seed must be non-negative, got " + 
                                    std::to_string(seed));
                        isValid = false;
                    }
                }
            } else if (key == "connection_probability") {
                if (std::holds_alternative<double>(value)) {
                    double prob = std::get<double>(value);
                    if (prob < 0.0 || prob > 1.0) {
                        NLM_LOG_ERROR("Config validation error: connection_probability must be between 0 and 1, got " + 
                                    std::to_string(prob));
                        isValid = false;
                    }
                }
            } else if (key == "synaptogenesis_rate" || key == "pruning_rate" || 
                       key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || 
                       key == "stdp_tau") {
                if (std::holds_alternative<double>(value)) {
                    double param = std::get<double>(value);
                    if (param < 0.0) {
                        NLM_LOG_ERROR("Config validation error: " + key + " must be non-negative, got " + 
                                    std::to_string(param));
                        isValid = false;
                    }
                }
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Config validation error for key '" + key + "': " + e.what());
            isValid = false;
        }
    }
    
    if (isValid) {
        NLM_LOG_INFO("Config validation passed: All configuration values are valid");
    }
    
    return isValid;
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
