#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to validate config keys
bool isValidKey(const std::string& key) {
    // Basic validation: alphanumeric, underscore, hyphen, dot
    // First character cannot be a number
    if (key.empty() || std::isdigit(key[0])) {
        return false;
    }
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    
    return true;
}

// Helper function to validate config values based on key type
bool isValidConfigValue(const std::string& key, const ConfigValue& value) {
    // Basic type validation based on key
    std::string lowerKey = toLower(key);
    
    // Numeric ranges and validation
    if (lowerKey.find("neuron_count") != std::string::npos) {
        if (!std::holds_alternative<int>(value)) return false;
        return std::get<int>(value) > 0 && std::get<int>(value) <= 10000;
    }
    
    if (lowerKey.find("region_count") != std::string::npos) {
        if (!std::holds_alternative<int>(value)) return false;
        return std::get<int>(value) >= 1 && std::get<int>(value) <= 100;
    }
    
    if (lowerKey.find("connection_probability") != std::string::npos) {
        if (!std::holds_alternative<double>(value)) return false;
        return std::get<double>(value) >= 0.0 && std::get<double>(value) <= 1.0;
    }
    
    if (lowerKey.find("simulation_timestep") != std::string::npos) {
        if (!std::holds_alternative<double>(value)) return false;
        return std::get<double>(value) > 0.0 && std::get<double>(value) <= 1.0;
    }
    
    if (lowerKey.find("random_seed") != std::string::npos) {
        if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value)) return false;
        int64_t seed = std::holds_alternative<int>(value) ? static_cast<int64_t>(std::get<int>(value)) : std::get<int64_t>(value);
        return seed >= 0 && seed < static_cast<int64_t>(std::numeric_limits<int64_t>::max() / 2);
    }
    
    // Float value ranges
    if (lowerKey.find("weight") != std::string::npos || lowerKey.find("rate") != std::string::npos) {
        if (!std::holds_alternative<double>(value)) return false;
        double val = std::get<double>(value);
        return val >= -1.0 && val <= 1.0;
    }
    
    // Boolean validation
    if (lowerKey.find("enabled") != std::string::npos || lowerKey.find("active") != std::string::npos || 
        lowerKey.find("enabled") != std::string::npos || lowerKey.find("flag") != std::string::npos) {
        return std::holds_alternative<bool>(value);
    }
    
    // String validation (basic length check)
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value).length() <= 1024;
    }
    
    // Default: accept any value for unknown keys
    return true;
}

// Helper function to parse simple key=value format
void parseSimpleConfigLine(const std::string& line, Config* config) {
    // Parse simple key=value pairs
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        std::string key = config->trim(line.substr(0, pos));
        std::string value = config->trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        config->set(key, value, ConfigSource::File);
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (check extension or content)
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJson(filepath);
    }
    
    // Try YAML next (check extension or content)
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml" ||
        filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        return loadFromYaml(filepath);
    }
    
    // Fall back to simple key=value format
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
        
        // Skip INI-style sections
        if (line[0] == '[' && line.back() == ']') {
            continue;
        }
        
        // Try to parse as key=value
        parseSimpleConfigLine(line, this);
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.find("=") != std::string::npos && arg.find("--") == 0) {
            size_t equalsPos = arg.find("=");
            std::string key = arg.substr(2, equalsPos - 2);
            std::string value = arg.substr(equalsPos + 1);
            
            // Parse value based on key or type hints
            setConfigValueFromString(key, value, ConfigSource::CommandLine);
        }
        // Handle --key value format
        else if (arg.find("--") == 0 && arg.length() > 2) {
            std::string key = arg.substr(2);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                setConfigValueFromString(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format (short options)
        else if (arg.find("-") == 0 && arg.length() > 1 && arg[1] != '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                setConfigValueFromString(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

void Config::setConfigValueFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Validate key
    if (!isValidKey(key)) {
        // Log error but continue
        // In a real implementation, would use the logger
        return;
    }
    
    // Special handling for known config keys
    std::string lowerKey = toLower(key);
    
    // For known numeric parameters, try to parse as double first for precision
    if (lowerKey.find("neuron_count") != std::string::npos || 
        lowerKey.find("region_count") != std::string::npos ||
        lowerKey.find("random_seed") != std::string::npos) {
        try {
            size_t pos = 0;
            long long int_val = std::stoll(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, int_val, source);
                return;
            }
        } catch (...) {}
    }
    
    // Most parameters are double precision
    if (lowerKey.find("probability") != std::string::npos ||
        lowerKey.find("timestep") != std::string::npos ||
        lowerKey.find("weight") != std::string::npos ||
        lowerKey.find("rate") != std::string::npos ||
        lowerKey.find("learning_rate") != std::string::npos ||
        lowerKey.find("scale") != std::string::npos ||
        lowerKey.find("factor") != std::string::npos ||
        lowerKey.find("limit") != std::string::npos ||
        lowerKey.find("capacity") != std::string::npos) {
        try {
            size_t pos = 0;
            double double_val = std::stod(valueStr, &pos);
            if (pos == valueStr.length()) {
                set(key, double_val, source);
                return;
            }
        } catch (...) {}
    }
    
    // Boolean parameters
    if (lowerKey.find("enabled") != std::string::npos ||
        lowerKey.find("active") != std::string::npos ||
        lowerKey.find("enable") != std::string::npos ||
        lowerKey.find("flag") != std::string::npos ||
        lowerKey.find("enabled") != std::string::npos) {
        bool val = (valueStr == "true" || valueStr == "1" || valueStr == "yes" || valueStr == "on");
        set(key, val, source);
        return;
    }
    
    // Default to string
    set(key, valueStr, source);
}

bool Config::loadFromJson(const std::string& filepath) {
    // TODO: Implement JSON parser using standard C++ libraries
    // For now, return false to fall back to simple parser
    return false;
}

bool Config::loadFromYaml(const std::string& filepath) {
    // TODO: Implement YAML parser using standard C++ libraries
    // For now, return false to fall back to simple parser
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.key << " (from " << static_cast<int>(entry.source) << ")\n";
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
    
    return true;
}

// Implementation of the public methods

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

// Internal helpers
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

// Add validation functions to Config class
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!isValidConfigValue(entry.key, entry.value)) {
            // Log validation error
            // In a real implementation, would use the logger
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        if (!isValidConfigValue(entry.key, entry.value)) {
            errors.push_back("Invalid value for key \"" + entry.key + "\": " + getValueString(entry.value));
        }
    }
    return errors;
}

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        oss << arg;
    }, value);
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