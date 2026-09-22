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
    // Load configuration from various formats
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file into string
    std::string content((std::istreambuf_iterator<char>(file)), 
                         std::istreambuf_iterator<char>());
    
    // Try to detect file format and parse accordingly
    // First check if it's YAML (contains ':' followed by space and has indented structure)
    bool isYaml = false;
    size_t yamlColon = content.find(": ");
    size_t yamlIndent = content.find('\n ');
    if (yamlColon != std::string::npos && yamlIndent != std::string::npos) {
        isYaml = true;
    }
    
    if (isYaml) {
        // Simple YAML parsing (supports key-value pairs with basic indentation)
        size_t pos = 0;
        while (pos < content.length()) {
            // Find next line with key-value pair
            size_t newlinePos = content.find('\n', pos);
            if (newlinePos == std::string::npos) {
                newlinePos = content.length();
            }
            
            std::string line = content.substr(pos, newlinePos - pos);
            pos = newlinePos + 1;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for key-value pair with colon
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                // Remove quotes from value
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
    } else {
        // Parse as key=value format (original format)
        size_t pos = 0;
        while (pos < content.length()) {
            size_t newlinePos = content.find('\n', pos);
            if (newlinePos == std::string::npos) {
                newlinePos = content.length();
            }
            
            std::string line = content.substr(pos, newlinePos - pos);
            pos = newlinePos + 1;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
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
    // Validate configuration value based on key
    if (!validateKeyValue(key, value)) {
        throw std::runtime_error("Invalid configuration value for key: " + key);
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

// Validation function
bool Config::validateKeyValue(const std::string& key, const ConfigValue& value) {
    // Normalize key for case-insensitive comparison
    std::string normalizedKey = toLower(key);
    
    // Dispatch based on value type
    return std::visit([](const auto& val) -> bool {
        using T = std::decay_t<decltype(val)>;
        return validateByType(normalizedKey, val);
    }, value);
}

// Template specialization for validation
bool Config::validateByType(const std::string& key, const std::string& value) {
    // String values - generic check
    return !value.empty() || key.find("description") != std::string::npos;
}

bool Config::validateByType(const std::string& key, int value) {
    if (key == "random_seed") return true;  // Any seed is valid
    if (key == "neuron_count" || key == "region_count" || 
        key == "max_simulation_steps" || key == "environment_width" || key == "environment_height") {
        return value > 0;
    }
    return true;  // Other int values accepted
}

bool Config::validateByType(const std::string& key, int64_t value) {
    if (key == "neuron_count" || key == "region_count" || 
        key == "max_simulation_steps") {
        return value > 0;
    }
    return true;
}

bool Config::validateByType(const std::string& key, double value) {
    if (key == "simulation_timestep") {
        return value > 0.0;  // Timestep must be positive
    }
    if (key == "reward_discount_factor") {
        return value >= 0.0 && value <= 1.0;  // Valid discount factor
    }
    if (key == "visualization_update_rate") {
        return value > 0.0;  // Update rate must be positive
    }
    if (key == "simulation_time_limit") {
        return value >= 0.0;  // Time limit can be 0 (unlimited)
    }
    return true;
}

bool Config::validateByType(const std::string& key, bool value) {
    return true;  // All boolean values are valid
}

bool Config::validateByType(const std::string& key, const std::vector<int>& values) {
    return !values.empty();
}

bool Config::validateByType(const std::string& key, const std::vector<double>& values) {
    return !values.empty();
}

bool Config::validateByType(const std::string& key, const std::vector<std::string>& values) {
    return !values.empty();
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
