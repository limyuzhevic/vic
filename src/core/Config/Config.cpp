#include "Config.hpp"
#include "../Exceptions/Exceptions.hpp"
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
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
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
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            validateKeyFormat(key);
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const ConfigException& e) {
                // Log warning but continue processing
                return false;
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
                
                // Validate key format
                validateKeyFormat(key);
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const ConfigException& e) {
                    // Log error and continue
                    return false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            validateKeyFormat(key);
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigException& e) {
                // Log error and continue
                return false;
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
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " at " << __TIME__ << "\n";
    file << "# Format: key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
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
    // Validate key format
    validateKeyFormat(key);
    
    // Validate value type matches expected type
    validateNumericType(key, value);
    validateStringType(key, value);
    validateVectorType(key, value);
    
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

void Config::validateKey(const std::string& key) const {
    validateKeyFormat(key);
}

template<typename T>
void Config::validateValue(const std::string& key, const T& value) const {
    // Template instantiation for supported types
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getDefaultEntryCount() const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::Default; });
}

size_t Config::getFileEntryCount() const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::File; });
}

size_t Config::getCommandLineEntryCount() const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::CommandLine; });
}

size_t Config::getRuntimeEntryCount() const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::Runtime; });
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

void Config::validateKeyFormat(const std::string& key) {
    if (key.empty()) {
        throw ConfigValidationException("Invalid key format", "Key cannot be empty");
    }
    if (key.size() > 100) {
        throw ConfigValidationException("Invalid key format", "Key too long (max 100 characters)");
    }
    // Check for valid characters (alphanumeric, underscore, hyphen)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            throw ConfigValidationException("Invalid key format", 
                "Key contains invalid characters. Only alphanumeric, underscore, and hyphen allowed");
        }
    }
}

void Config::validateNumericType(const std::string& key, const ConfigValue& value) {
    std::visit([&key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            if (arg < -1000000 || arg > 1000000) {
                throw ConfigValidationException("Invalid int value", 
                    "Value out of range for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (arg < -1000000000000LL || arg > 1000000000000LL) {
                throw ConfigValidationException("Invalid int64_t value", 
                    "Value out of range for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                throw ConfigValidationException("Invalid double value", 
                    "NaN or infinity not allowed for key: " + key);
            }
            if (arg < -1e10 || arg > 1e10) {
                throw ConfigValidationException("Invalid double value", 
                    "Value out of range for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Booleans are always valid
        }
    }, value);
}

void Config::validateStringType(const std::string& key, const ConfigValue& value) {
    std::visit([&key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.size() > 1000) {
                throw ConfigValidationException("Invalid string value", 
                    "String too long (max 1000 characters) for key: " + key);
            }
        }
    }, value);
}

void Config::validateVectorType(const std::string& key, const ConfigValue& value) {
    std::visit([&key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.size() > 1000) {
                throw ConfigValidationException("Invalid vector value", 
                    "Vector too large (max 1000 elements) for key: " + key);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.size() > 1000) {
                throw ConfigValidationException("Invalid vector value", 
                    "Vector too large (max 1000 elements) for key: " + key);
            }
            for (size_t i = 0; i < arg.size(); ++i) {
                if (std::isnan(arg[i]) || std::isinf(arg[i])) {
                    throw ConfigValidationException("Invalid vector value", 
                        "NaN or infinity not allowed in vector for key: " + key);
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.size() > 1000) {
                throw ConfigValidationException("Invalid vector value", 
                    "Vector too large (max 1000 elements) for key: " + key);
            }
            for (const auto& item : arg) {
                if (item.size() > 100) {
                    throw ConfigValidationException("Invalid vector value", 
                        "String too long in vector (max 100 characters) for key: " + key);
                }
            }
        }
    }, value);
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
