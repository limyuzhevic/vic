#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <chrono>

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
            } else if (i + 1 < argc) {
                // Handle --key value format (separate args)
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -k value format (short options)
        else if (arg.size() > 1 && arg[0] == '-' && arg[1] != '-') {
            if (i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
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
    
    file << "# NLM Configuration File\n";
    file << "# Saved from memory at " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Add comment if description exists
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write the key-value pair with proper formatting
        file << entry.key << " = ";
        
        // Format based on value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                // Use sufficient precision for floating point
                file << std::fixed << std::setprecision(6) << arg;
            } else {
                // For integers and other types, just output the value
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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

bool Config::validate() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        bool entryValid = true;
        std::string errorMsg;
        
        // Type-specific validation
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                if (arg.empty()) {
                    entryValid = false;
                    errorMsg = "String value cannot be empty";
                }
            } else if constexpr (std::is_same_v<T, int>) {
                if (arg < std::numeric_limits<int>::min() || arg > std::numeric_limits<int>::max()) {
                    entryValid = false;
                    errorMsg = "Integer value out of range";
                }
            } else if constexpr (std::is_same_v<T, int64_t>) {
                if (arg < std::numeric_limits<int64_t>::min() || arg > std::numeric_limits<int64_t>::max()) {
                    entryValid = false;
                    errorMsg = "64-bit integer value out of range";
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::isnan(arg) || std::isinf(arg)) {
                    entryValid = false;
                    errorMsg = "Double value is NaN or infinite";
                }
            } else if constexpr (std::is_same_v<T, float>) {
                if (std::isnan(arg) || std::isinf(arg)) {
                    entryValid = false;
                    errorMsg = "Float value is NaN or infinite";
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                // Boolean values are always valid
            }
        }, entry.value);
        
        if (!entryValid) {
            NLM_LOG_WARNING("Invalid configuration value for key '" + entry.key + "': " + errorMsg);
            allValid = false;
        }
    }
    
    // Validate specific configuration relationships
    // For example, timestep should be positive, neuron_count should be reasonable, etc.
    auto timestepOpt = get<double>("simulation_timestep");
    if (timestepOpt.has_value()) {
        if (timestepOpt.value() <= 0.0) {
            NLM_LOG_WARNING("Invalid simulation timestep: must be positive");
            allValid = false;
        }
    }
    
    auto neuronCountOpt = get<int64_t>("neuron_count");
    if (neuronCountOpt.has_value()) {
        if (neuronCountOpt.value() <= 0 || neuronCountOpt.value() > 1000000) {
            NLM_LOG_WARNING("Invalid neuron count: must be between 1 and 1,000,000");
            allValid = false;
        }
    }
    
    auto regionCountOpt = get<int64_t>("region_count");
    if (regionCountOpt.has_value()) {
        if (regionCountOpt.value() <= 0 || regionCountOpt.value() > 1000) {
            NLM_LOG_WARNING("Invalid region count: must be between 1 and 1,000");
            allValid = false;
        }
    }
    
    auto connProbOpt = get<float>("connection_probability");
    if (connProbOpt.has_value()) {
        if (connProbOpt.value() < 0.0f || connProbOpt.value() > 1.0f) {
            NLM_LOG_WARNING("Invalid connection probability: must be between 0.0 and 1.0");
            allValid = false;
        }
    }
    
    if (allValid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_WARNING("Configuration validation failed");
    }
    
    return allValid;
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
