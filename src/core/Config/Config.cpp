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
    // Enhanced file loading with comprehensive error handling
    
    if (filepath.empty()) {
        NLM_LOG_WARNING("loadFromFile called with empty filepath");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Unable to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasValidContent = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = trimmedLine.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + " in " + filepath + ": missing '='");
            continue;
        }
        
        std::string key = trim(trimmedLine.substr(0, pos));
        std::string value = trim(trimmedLine.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + " in " + filepath + ": empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
            hasValidContent = true;
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Error parsing config line " + std::to_string(lineNumber) + " in " + filepath + ": " + std::string(e.what()));
            continue;
        }
    }
    
    if (lineNumber == 0) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
    } else if (!hasValidContent) {
        NLM_LOG_WARNING("Config file contains no valid entries: " + filepath);
    }
    
    return hasValidContent;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced argument loading with validation
    if (argc <= 0 || argv == nullptr) {
        NLM_LOG_WARNING("loadFromArgs called with invalid arguments (argc: " + std::to_string(argc) + ")");
        return false;
    }
    
    bool hasValidArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.empty()) {
            NLM_LOG_WARNING("Empty argument at position " + std::to_string(i));
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Invalid argument format at position " + std::to_string(i) + ": missing '=' in --key=value");
                continue;
            }
            
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            if (key.empty()) {
                NLM_LOG_WARNING("Invalid argument at position " + std::to_string(i) + ": empty key");
                continue;
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
                hasValidArgs = true;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Error parsing argument --" + key + "=" + value + ": " + std::string(e.what()));
                continue;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_LOG_WARNING("Invalid argument at position " + std::to_string(i) + ": empty key");
                continue;
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
                hasValidArgs = true;
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Error parsing argument -" + key + " " + value + ": " + std::string(e.what()));
                continue;
            }
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized argument at position " + std::to_string(i) + ": " + arg);
        }
    }
    
    if (!hasValidArgs) {
        NLM_LOG_WARNING("No valid command line arguments parsed");
    }
    
    return hasValidArgs;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Enhanced file saving with validation
    
    if (filepath.empty()) {
        NLM_LOG_WARNING("saveToFile called with empty filepath");
        return false;
    }
    
    // Ensure directory exists
    std::filesystem::path fsPath(filepath);
    std::filesystem::path parentDir = fsPath.parent_path();
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        NLM_LOG_WARNING("Cannot save config to file: directory does not exist: " + filepath);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Unable to open config file for writing: " + filepath);
        return false;
    }
    
    // Write configuration with comments
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert value to string
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]";
            } else {
                file << "UNSUPPORTED_TYPE";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file << "# End of configuration\n";
    
    if (file.fail()) {
        NLM_LOG_WARNING("Error writing config file: " + filepath);
        return false;
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
    if (key.empty()) {
        NLM_LOG_WARNING("Config::get called with empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        // Not found - this is not necessarily an error, just return nullopt
        return std::nullopt;
    }
    
    try {
        // Numeric values convert between int/int64/double/float/etc, so a
        // value stored as double can be read as float and vice versa.
        return std::visit([](auto&& v) -> std::optional<T> {
            using V = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<V, T>) {
                return v;
            } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
                return static_cast<T>(v);
            } else {
                NLM_LOG_WARNING("Config::get type mismatch for key '" + key + "': expected " + typeid(T).name() + ", got " + typeid(V).name());
                return std::nullopt;
            }
        }, it->value);
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Error getting config value for key '" + key + "': " + std::string(e.what()));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    if (key.empty()) {
        NLM_LOG_WARNING("Config::getOr called with empty key");
        return defaultValue;
    }
    
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Config::getOr: key '" + key + "' not found, returning default value");
        return defaultValue;
    }
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
