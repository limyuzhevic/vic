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
// Validation helpers
    static bool isValidKey(const std::string& key);
    static bool isValidFilePath(const std::string& filepath);
    static bool isValidParameterValue(const ConfigValue& value, const std::string& key = "");
    
    // Validation methods implementation
    bool Config::validateKey(const std::string& key) const {
        return isValidKey(key);
    }
    
    bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
        return isValidParameterValue(value, key);
    }
    
    bool Config::validateFilePath(const std::string& filepath) const {
        return isValidFilePath(filepath);
    }
    
    std::string Config::formatError(const std::string& operation, const std::string& details) const {
        return "Config " + operation + ": " + details;
    }
    
    // Validation helper implementations
    bool Config::isValidKey(const std::string& key) {
        if (key.empty() || key.length() > 100) {
            return false;
        }
        
        // Check for invalid characters
        for (char c : key) {
            if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
                return false;
            }
        }
        
        // Check for reserved keywords (basic ones)
        std::string lowerKey = toLower(key);
        static const std::vector<std::string> reservedKeywords = {
            "help", "version", "verbose", "quiet", "debug", "release", "test"
        };
        
        return std::find(reservedKeywords.begin(), reservedKeywords.end(), lowerKey) == reservedKeywords.end();
    }
    
    bool Config::isValidFilePath(const std::string& filepath) {
        if (filepath.empty() || filepath.length() > 500) {
            return false;
        }
        
        // Basic path validation
        // Check for directory traversal attempts
        if (filepath.find("../") != std::string::npos || 
            filepath.find("..\\") != std::string::npos) {
            return false;
        }
        
        return true;
    }
    
    bool Config::isValidParameterValue(const ConfigValue& value, const std::string& key) {
        // Type-specific validation
        bool result = std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>) {
                // Validate integer ranges
                return arg >= std::numeric_limits<int>::min() && arg <= std::numeric_limits<int>::max();
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                // Validate int64 ranges
                return arg >= std::numeric_limits<int64_t>::min() && arg <= std::numeric_limits<int64_t>::max();
            }
            else if constexpr (std::is_same_v<T, double>) {
                // Validate double ranges
                return !std::isnan(arg) && !std::isinf(arg);
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return true; // Boolean values are always valid
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                // Validate string constraints
                return !arg.empty() && arg.length() <= 1000 && arg.find('\0') == std::string::npos;
            }
            else if constexpr (std::is_same_v<T, std::vector<int>>) {
                // Validate integer vector
                for (const auto& item : arg) {
                    if (item < std::numeric_limits<int>::min() || item > std::numeric_limits<int>::max()) {
                        return false;
                    }
                }
                return true;
            }
            else if constexpr (std::is_same_v<T, std::vector<double>>) {
                // Validate double vector
                for (const auto& item : arg) {
                    if (std::isnan(item) || std::isinf(item)) {
                        return false;
                    }
                }
                return true;
            }
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Validate string vector
                for (const auto& item : arg) {
                    if (item.empty() || item.length() > 1000 || item.find('\0') != std::string::npos) {
                        return false;
                    }
                }
                return true;
            }
            
            return true; // Default case
        }, value);
        
        if (!result) {
            return false;
        }
        
        // Key-specific validation
        if (!key.empty()) {
            std::string lowerKey = toLower(key);
            
            // Validate specific key constraints
            if (lowerKey == "neuron_count" || lowerKey == "region_count" || lowerKey == "thread_count") {
                // These should be positive integers
                if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
                    return false;
                }
                int64_t intValue = std::visit([](auto&& arg) { return static_cast<int64_t>(arg); }, value);
                if (intValue <= 0) return false;
            }
            else if (lowerKey == "connection_probability" || lowerKey == "synaptogenesis_rate" || 
                     lowerKey == "pruning_rate" || lowerKey == "stdp_ltp_weight" || lowerKey == "stdp_ltd_weight") {
                // These should be between 0.0 and 1.0
                if (!std::holds_alternative<double>(value)) {
                    return false;
                }
                double doubleValue = std::visit([](auto&& arg) { return static_cast<double>(arg); }, value);
                if (doubleValue < 0.0 || doubleValue > 1.0) return false;
            }
            else if (lowerKey == "simulation_timestep" || lowerKey == "stdp_tau") {
                // These should be positive numbers
                if (!std::holds_alternative<double>(value)) {
                    return false;
                }
                double doubleValue = std::visit([](auto&& arg) { return static_cast<double>(arg); }, value);
                if (doubleValue <= 0.0) return false;
            }
        }
        
        return true;
    }
    
    // Update set methods to include validation
    void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
        // Validate key and value
        if (!isValidKey(key)) {
            throw std::invalid_argument("Invalid configuration key: '" + key + "'");
        }
        
        if (!isValidParameterValue(value, key)) {
            throw std::invalid_argument("Invalid configuration value for key '" + key + "'");
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
        if (!isValidKey(key)) {
            throw std::invalid_argument("Invalid configuration key: '" + key + "'");
        }
        
        set(key, ConfigValue(value), source);
    }
    
    void Config::set(const std::string& key, int value, ConfigSource source) {
        if (!isValidKey(key)) {
            throw std::invalid_argument("Invalid configuration key: '" + key + "'");
        }
        
        set(key, ConfigValue(value), source);
    }
    
    void Config::set(const std::string& key, double value, ConfigSource source) {
        if (!isValidKey(key)) {
            throw std::invalid_argument("Invalid configuration key: '" + key + "'");
        }
        
        set(key, ConfigValue(value), source);
    }
    
    void Config::set(const std::string& key, bool value, ConfigSource source) {
        if (!isValidKey(key)) {
            throw std::invalid_argument("Invalid configuration key: '" + key + "'");
        }
        
        set(key, ConfigValue(value), source);
    }
    
    bool Config::loadFromFile(const std::string& filepath) {
        if (!isValidFilePath(filepath)) {
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string valueStr = trim(line.substr(pos + 1));
                
                if (valueStr.size() >= 2 && 
                    ((valueStr.front() == '"' && valueStr.back() == '"') ||
                     (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                try {
                    set(key, valueStr, ConfigSource::File);
                } catch (const std::exception& e) {
                    // Log error and continue with other entries
                    continue;
                }
            }
        }
        
        return true;
    }
    
    bool Config::saveToFile(const std::string& filepath) const {
        if (!isValidFilePath(filepath)) {
            return false;
        }
        
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
    
    std::vector<std::string> Config::getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        
        for (const auto& entry : pImpl->entries) {
            // Filter out invalid keys in the result
            if (isValidKey(entry.key)) {
                keys.push_back(entry.key);
            }
        }
        
        return keys;
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
