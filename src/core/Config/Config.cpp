#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <limits>
#include "../Logger/Logger.hpp"

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
        Logger::getGlobal().error("Failed to open configuration file: " + filepath, "", 0, "loadFromFile");
        return false;
    }
    
    int lineNumber = 0;
    int successCount = 0;
    int errorCount = 0;
    
    std::string line;
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
            Logger::getGlobal().warning("Invalid configuration format (no '=' found) in " + filepath + " at line " + std::to_string(lineNumber), "", 0, "loadFromFile");
            ++errorCount;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            Logger::getGlobal().warning("Empty key found in " + filepath + " at line " + std::to_string(lineNumber), "", 0, "loadFromFile");
            ++errorCount;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
            ++successCount;
        } catch (const std::exception& e) {
            Logger::getGlobal().error("Error setting configuration key '" + key + "' in " + filepath + " at line " + std::to_string(lineNumber) + ": " + e.what(), "", 0, "loadFromFile");
            ++errorCount;
        }
    }
    
    file.close();
    
    // Log summary
    std::ostringstream summary;
    summary << "Loaded configuration from " + filepath + ": ";
    if (successCount > 0) {
        summary << "Successfully loaded " + std::to_string(successCount) + " entries";
    }
    if (errorCount > 0) {
        if (successCount > 0) summary << ", ";
        summary << "Failed to load " + std::to_string(errorCount) + " entries";
    }
    summary << ".";
    
    if (successCount == 0) {
        Logger::getGlobal().warning(summary.str(), "", 0, "loadFromFile");
    } else {
        Logger::getGlobal().info(summary.str(), "", 0, "loadFromFile");
    }
    
    return successCount > 0 || errorCount > 0;  // Return true if we attempted to parse (even with errors)
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        Logger::getGlobal().debug("No command line arguments provided for configuration", "", 0, "loadFromArgs");
        return true;  // Not an error, just nothing to process
    }
    
    int successCount = 0;
    int errorCount = 0;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    Logger::getGlobal().warning("Empty key found in command line argument: " + arg, "", 0, "loadFromArgs");
                    ++errorCount;
                    continue;
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                    ++successCount;
                } catch (const std::exception& e) {
                    Logger::getGlobal().error("Error setting configuration key '" + key + "' from command line argument '" + arg + "': " + e.what(), "", 0, "loadFromArgs");
                    ++errorCount;
                }
            } else {
                Logger::getGlobal().warning("Invalid command line argument format (no '=' found): " + arg, "", 0, "loadFromArgs");
                ++errorCount;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                Logger::getGlobal().warning("Empty key found in command line argument: -" + value, "", 0, "loadFromArgs");
                ++errorCount;
                continue;
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
                ++successCount;
            } catch (const std::exception& e) {
                Logger::getGlobal().error("Error setting configuration key '" + key + "' from command line argument '-" + value + "': " + e.what(), "", 0, "loadFromArgs");
                ++errorCount;
            }
        } else {
            // Ignore unrecognized arguments (they might be program args, not config)
            continue;
        }
    }
    
    // Log summary
    std::ostringstream summary;
    summary << "Processed " + std::to_string(argc - 1) + " command line argument(s): ";
    if (successCount > 0) {
        summary << "Processed " + std::to_string(successCount) + " successfully";
    }
    if (errorCount > 0) {
        if (successCount > 0) summary << ", ";
        summary << "Failed to process " + std::to_string(errorCount) + " arguments";
    }
    summary << ".";
    
    Logger::getGlobal().info(summary.str(), "", 0, "loadFromArgs");
    
    return true;  // Always return true as we successfully processed (even if with errors)
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        Logger::getGlobal().error("Failed to open configuration file for writing: " + filepath, "", 0, "saveToFile");
        return false;
    }
    
    int successCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        // Use proper value serialization based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::setprecision(6) << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        ++successCount;
    }
    
    if (successCount == 0) {
        Logger::getGlobal().warning("No configuration entries to save to " + filepath, "", 0, "saveToFile");
    } else {
        Logger::getGlobal().info("Saved " + std::to_string(successCount) + " configuration entries to " + filepath, "", 0, "saveToFile");
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        Logger::getGlobal().debug("Configuration key not found: " + key, "", 0, "get");
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        // Validate the value based on type
        if constexpr (std::is_same_v<T, int>) {
            if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
                Logger::getGlobal().warning("Configuration value out of range for key " + key + ": " + std::to_string(value), "", 0, "get");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(value) || std::isinf(value)) {
                Logger::getGlobal().warning("Invalid configuration value for key " + key + ": " + std::to_string(value), "", 0, "get");
            }
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        Logger::getGlobal().error("Configuration value type mismatch for key " + key + " (expected " + typeid(T).name() + ")", "", 0, "get");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Configuration validation methods
bool Config::validate() const {
    bool isValid = true;
    
    // Check for required keys
    const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& requiredKey : requiredKeys) {
        if (!has(requiredKey)) {
            Logger::getGlobal().error("Missing required configuration key: " + requiredKey, "", 0, "validate");
            isValid = false;
        }
    }
    
    // Validate value ranges for known keys
    auto validateRange = [this](const std::string& key, double min, double max) {
        if (has(key)) {
            std::optional<double> value = get<double>(key);
            if (value.has_value()) {
                if (value.value() < min || value.value() > max) {
                    Logger::getGlobal().warning("Configuration key " + key + " has out-of-range value: " + 
                        std::to_string(value.value()) + " (expected " + std::to_string(min) + " - " + std::to_string(max) + ")", 
                        "", 0, "validate");
                    return false;
                }
            }
        }
        return true;
    };
    
    // Validate specific key ranges
    validateRange("random_seed", 0, std::numeric_limits<int>::max());
    validateRange("simulation_timestep", 0.0, 1.0);
    validateRange("neuron_count", 1, std::numeric_limits<int>::max());
    validateRange("region_count", 1, std::numeric_limits<int>::max());
    validateRange("connection_probability", 0.0, 1.0);
    validateRange("plasticity_learning_rate", 0.0, 1.0);
    validateRange("stdp_ltp_weight", -1.0, 1.0);
    validateRange("stdp_ltd_weight", -1.0, 1.0);
    validateRange("dopamine_baseline", 0.0, 10.0);
    validateRange("reward_discount_factor", 0.0, 1.0);
    validateRange("environment_width", 1, 1000);
    validateRange("environment_height", 1, 1000);
    validateRange("log_level", 0, 4);  // Debug=0, Critical=4
    validateRange("max_simulation_steps", 0, std::numeric_limits<int>::max());
    validateRange("visualization_update_rate", 1.0, 1000.0);
    
    if (isValid) {
        Logger::getGlobal().info("Configuration validation passed", "", 0, "validate");
    }
    
    return isValid;
}

// Get configuration issues
std::vector<std::string> Config::getIssues() const {
    std::vector<std::string> issues;
    
    // Check for required keys
    const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& requiredKey : requiredKeys) {
        if (!has(requiredKey)) {
            issues.push_back("Missing required configuration key: " + requiredKey);
        }
    }
    
    // Check value ranges for known keys
    auto checkRange = [this, &issues](const std::string& key, double min, double max) {
        if (has(key)) {
            std::optional<double> value = get<double>(key);
            if (value.has_value()) {
                if (value.value() < min || value.value() > max) {
                    issues.push_back("Configuration key " + key + " has out-of-range value: " + 
                        std::to_string(value.value()) + " (expected " + std::to_string(min) + " - " + std::to_string(max) + ")");
                }
            }
        }
    };
    
    checkRange("random_seed", 0, std::numeric_limits<int>::max());
    checkRange("simulation_timestep", 0.0, 1.0);
    checkRange("neuron_count", 1, std::numeric_limits<int>::max());
    checkRange("region_count", 1, std::numeric_limits<int>::max());
    checkRange("connection_probability", 0.0, 1.0);
    checkRange("plasticity_learning_rate", 0.0, 1.0);
    checkRange("stdp_ltp_weight", -1.0, 1.0);
    checkRange("stdp_ltd_weight", -1.0, 1.0);
    checkRange("dopamine_baseline", 0.0, 10.0);
    checkRange("reward_discount_factor", 0.0, 1.0);
    checkRange("environment_width", 1, 1000);
    checkRange("environment_height", 1, 1000);
    checkRange("log_level", 0, 4);  // Debug=0, Critical=4
    checkRange("max_simulation_steps", 0, std::numeric_limits<int>::max());
    checkRange("visualization_update_rate", 1.0, 1000.0);
    
    return issues;
}

// Auto-correct configuration issues
bool Config::autoCorrect() {
    bool corrected = false;
    
    // Auto-correct common issues
    if (has("simulation_timestep") && getOr<double>("simulation_timestep", 0.001) <= 0.0) {
        set("simulation_timestep", 0.001);
        Logger::getGlobal().info("Auto-corrected simulation_timestep to 0.001", "", 0, "autoCorrect");
        corrected = true;
    }
    
    if (has("neuron_count") && getOr<int>("neuron_count", 1000) <= 0) {
        set("neuron_count", 1000);
        Logger::getGlobal().info("Auto-corrected neuron_count to 1000", "", 0, "autoCorrect");
        corrected = true;
    }
    
    if (has("region_count") && getOr<int>("region_count", 2) <= 0) {
        set("region_count", 2);
        Logger::getGlobal().info("Auto-corrected region_count to 2", "", 0, "autoCorrect");
        corrected = true;
    }
    
    return corrected;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate key format (basic validation)
    if (key.find('=') != std::string::npos || key.find('\n') != std::string::npos || key.find('\r') != std::string::npos) {
        throw std::invalid_argument("Configuration key contains invalid characters: " + key);
    }
    
    // Value validation based on type
    std::visit([this, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        // Type-specific validation
        if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                throw std::invalid_argument("Invalid double value for configuration key " + key + ": " + std::to_string(arg));
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                if (item < std::numeric_limits<int>::min() || item > std::numeric_limits<int>::max()) {
                    throw std::invalid_argument("Integer out of range in vector for configuration key " + key);
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                if (std::isnan(item) || std::isinf(item)) {
                    throw std::invalid_argument("Invalid double value in vector for configuration key " + key);
                }
            }
        }
    }, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        Logger::getGlobal().debug("Updated configuration key " + key + " to new value", "", 0, "set");
    } else {
        pImpl->entries.emplace_back(key, value, source);
        Logger::getGlobal().debug("Added new configuration key " + key + " with value", "", 0, "set");
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
