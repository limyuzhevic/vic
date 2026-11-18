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
    // Enhanced error handling for file loading
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config: Empty filepath provided to loadFromFile");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config: Cannot open file for reading: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config: Line " + std::to_string(lineNumber) + 
                           " missing '=' separator: " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Config: Line " + std::to_string(lineNumber) + 
                           " has empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Basic validation for key characters
        if (!std::all_of(key.begin(), key.end(), [](char c) {
            return std::isalnum(c) || c == '_' || c == '-';
        })) {
            NLM_LOG_WARNING("Config: Line " + std::to_string(lineNumber) + 
                           " key contains invalid characters: " + key);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    NLM_LOG_INFO("Config: Loaded " + std::to_string(pImpl->entries.size()) + 
                " entries from file: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 0) {
        NLM_LOG_WARNING("Config: Invalid arguments for loadFromArgs");
        return false;
    }
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.empty()) {
            NLM_LOG_WARNING("Config: Empty argument at position " + std::to_string(i));
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Config: Empty key in argument at position " + std::to_string(i));
                    success = false;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Config: Invalid --key=value format in argument: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Config: Empty key in argument at position " + std::to_string(i - 1));
                    success = false;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Config: Missing value for argument: " + arg);
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Config: Unknown argument format: " + arg);
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config: Empty filepath provided to saveToFile");
        return false;
    }
    
    // Check if we have write permissions by attempting to create/open the file
    std::ofstream testFile(filepath, std::ios::trunc);
    if (!testFile.is_open()) {
        NLM_LOG_ERROR("Config: Cannot create/open file for writing: " + filepath);
        return false;
    }
    testFile.close();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config: Cannot open file for writing: " + filepath);
        return false;
    }
    
    // Set good formatting
    file.precision(6);
    file << std::fixed;
    
    for (const auto& entry : pImpl->entries) {
        // Add comment with description
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Format the value based on its type
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Config: Saved " + std::to_string(pImpl->entries.size()) + 
                " entries to file: " + filepath);
    return true;
}

// Helper macro for config validation
#define VALIDATE_CONFIG_KEY(key) \
    if ((key).empty()) { \
        NLM_LOG_WARNING("Config: Empty key provided to set method"); \
        return; \
    } \
    if ((key).find('=') != std::string::npos || (key).find(' ') != std::string::npos) { \
        NLM_LOG_WARNING("Config: Key contains invalid characters: " + (key)); \
    }

// Set methods with validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    VALIDATE_CONFIG_KEY(key);
    
    // Validate that the key doesn't conflict with reserved names
    if (key == "source" || key == "description" || key == "timestamp") {
        NLM_LOG_WARNING("Config: Using reserved key name: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Config: Updated entry: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Config: Added new entry: " + key);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    VALIDATE_CONFIG_KEY(key);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    VALIDATE_CONFIG_KEY(key);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    VALIDATE_CONFIG_KEY(key);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    VALIDATE_CONFIG_KEY(key);
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    VALIDATE_CONFIG_KEY(key);
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
