#include "Config.hpp"
#include "../Logger/Logger.hpp"
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
    // Determine file type based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    
    if (ext == "json" || ext == "JSON") {
        return loadFromJson(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return loadFromYaml(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
    }
}

// Load from JSON file
bool Config::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open JSON file: " + filepath);
        return false;
    }
    
    try {
        // Use nlohmann/json if available
        // For now, implement a simple JSON parser
        // TODO: Add nlohmann/json dependency
        return loadFromSimpleFormat(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Load from YAML file
bool Config::loadFromYaml(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open YAML file: " + filepath);
        return false;
    }
    
    // TODO: Add YAML parsing support
    NLM_LOG_WARNING("YAML parsing not yet implemented, using simple format");
    return loadFromSimpleFormat(filepath);
}

// Load from simple key=value format (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

// Validate configuration values
bool Config::validateConfig() const {
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep"
    };
    
    bool allValid = true;
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required config key: " + key);
            allValid = false;
        }
    }
    
    // Validate numeric ranges
    auto checkRange = [this](const std::string& key, double min, double max) {
        if (has(key)) {
            auto val = get<double>(key);
            if (val.has_value() && (val.value() < min || val.value() > max)) {
                NLM_LOG_WARNING("Config key " + key + " value " + std::to_string(val.value()) + 
                               " outside expected range [" + std::to_string(min) + ", " + 
                               std::to_string(max) + "]");
                return false;
            }
        }
        return true;
    };
    
    checkRange("simulation_timestep", 0.0, 1.0);
    checkRange("connection_probability", 0.0, 1.0);
    
    return allValid;
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

// Save to file with proper formatting
    bool Config::saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Could not open configuration file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated on " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            // Write description if available
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            // Write key=value pair with proper formatting based on value type
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
            
            // Add comment about source
            std::string sourceStr;
            switch (entry.source) {
                case ConfigSource::Default: sourceStr = "default"; break;
                case ConfigSource::File: sourceStr = "file"; break;
                case ConfigSource::CommandLine: sourceStr = "command-line"; break;
                case ConfigSource::Runtime: sourceStr = "runtime"; break;
            }
            file << "  # " << sourceStr << "\n";
        }
        
        file << "\n# End of configuration\n";
        return true;
    }
    
    std::string Config::getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time_t, &tm_buf);
        char timestamp[100];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_buf);
        return std::string(timestamp);
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
