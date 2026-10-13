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
            
    // Add more sophisticated key=value parsing with type conversion
    if (key == "neuron_count" || key == "region_count" || key == "random_seed") {
        try {
            int64_t intVal = std::stoll(value);
            set(key, intVal, ConfigSource::File);
        } catch (...) {
            set(key, value, ConfigSource::File);
        }
    } else if (key == "connection_probability" || key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "stdp_tau" || key == "synaptogenesis_rate" || key == "pruning_rate" || key == "dopamine_baseline" || key == "plasticity_learning_rate" || key == "stdp_time_constant" || key == "development_synaptogenesis_rate" || key == "development_pruning_rate" || key == "dopamine_baseline" || key == "reward_discount_factor" || key == "visualization_update_rate") {
        try {
            float floatVal = std::stof(value);
            set(key, floatVal, ConfigSource::File);
        } catch (...) {
            set(key, value, ConfigSource::File);
        }
    } else if (key == "simulation_timestep") {
        try {
            double doubleVal = std::stod(value);
            set(key, doubleVal, ConfigSource::File);
        } catch (...) {
            set(key, value, ConfigSource::File);
        }
    } else if (key == "log_to_file" || key == "visualization_enabled") {
        try {
            bool boolVal = (value == "true" || value == "1" || value == "yes");
            set(key, boolVal, ConfigSource::File);
        } catch (...) {
            set(key, value, ConfigSource::File);
        }
    }
        }
    }
    
    return true;
}

        return true;
    }
    
    // Check --help, --version flags first
    if (arg == "--help" || arg == "-h" || arg == "/?" || arg == "--version" || arg == "-V") {
        continue;
    }
    
    // Check for advanced configuration flags
    if (arg == "--benchmark" || arg == "-b" || arg == "--visualization" || arg == "-v" || 
        arg == "--no-colors" || arg == "--output" || arg == "-o" || arg == "--log-level" || 
        arg == "-l" || arg == "--log-file" || arg == "--benchmark-duration") {
        // Skip advanced flags
        if (arg == "--output" || arg == "-o" || arg == "--log-level" || arg == "-l" || 
            arg == "--log-file" || arg == "--benchmark-duration") {
            i += 2; // Skip flag and value
        }
        continue;
    }
    
    // Check for --set= overrides
    if (arg.substr(0, 7) == "--set=") {
        continue;
    }
    
    // Check for config file flag
    if (arg.substr(0, 8) == "--config=") {
        i++; // Skip value
        continue;
    } else if (arg == "--config" || arg == "-c") {
        if (i + 1 < argc) {
            i++; // Skip value
        }
        continue;
    }
    
    // All other arguments are treated as config overrides
    if (arg.substr(0, 2) == "--" || (arg.size() > 0 && arg[0] == '-')) {
        set(arg, "true", ConfigSource::CommandLine);
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

// Validation system for configuration
ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Check required configuration keys
    std::vector<std::string> requiredKeys = {
        "random_seed", "simulation_timestep", "neuron_count", 
        "region_count", "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            result.warnings.push_back("Required configuration key missing: '" + key + "' (using default)");
        }
    }
    
    // Validate all configuration entries
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryResult;
        
        // Basic type validation based on key name
        if (entry.key == "simulation_timestep") {
            if (std::holds_alternative<double>(entry.value)) {
                double timestep = std::get<double>(entry.value);
                if (timestep <= 0) {
                    entryResult.errors.push_back("simulation_timestep must be positive");
                } else if (timestep > 0.1) {
                    entryResult.warnings.push_back("simulation_timestep is large (" + std::to_string(timestep) + ") - may cause numerical issues");
                }
            }
        } else if (entry.key == "neuron_count" || entry.key == "region_count") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                int64_t val = std::get<int64_t>(entry.value);
                if (val <= 0) {
                    entryResult.errors.push_back(entry.key + " must be positive");
                } else if (val > 1000000) {
                    entryResult.warnings.push_back(entry.key + " is very large (" + std::to_string(val) + ") - may impact performance");
                }
            }
        } else if (entry.key == "connection_probability") {
            if (std::holds_alternative<float>(entry.value)) {
                float prob = std::get<float>(entry.value);
                if (prob < 0 || prob > 1) {
                    entryResult.errors.push_back("connection_probability must be between 0 and 1");
                }
            }
        }
        
        // Combine entry results
        result.errors.insert(result.errors.end(), entryResult.errors.begin(), entryResult.errors.end());
        result.warnings.insert(result.warnings.end(), entryResult.warnings.begin(), entryResult.warnings.end());
    }
    
    result.valid = result.errors.empty();
    return result;
}

// Configuration export for debugging
std::string Config::exportConfig() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Export ===" << std::endl;
    oss << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
    oss << std::endl;
    
    oss << "Configuration Summary:" << std::endl;
    oss << summary() << std::endl;
    
    oss << "Validation Status:" << std::endl;
    ValidationResult validation = validate();
    oss << "Valid: " << (validation.valid ? "YES" : "NO") << std::endl;
    
    if (!validation.errors.empty()) {
        oss << "Errors:" << std::endl;
        for (const auto& error : validation.errors) {
            oss << "  - " << error << std::endl;
        }
    }
    
    if (!validation.warnings.empty()) {
        oss << "Warnings:" << std::endl;
        for (const auto& warning : validation.warnings) {
            oss << "  - " << warning << std::endl;
        }
    }
    
    oss << "=== End of Configuration Export ===" << std::endl;
    return oss.str();
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
