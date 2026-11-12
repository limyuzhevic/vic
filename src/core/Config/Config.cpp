#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <regex>
#include <map>
#include "core/Logger/Logger.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, std::vector<std::string>> synonyms;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with improved error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": missing '='");
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": empty key");
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Check for duplicate key with better error message
        if (has(key)) {
            NLM_LOG_WARNING("Duplicate config key '" + key + "' in file " + filepath + " at line " + std::to_string(lineNumber));
            hasErrors = true;
        }
        
        // Parse and validate value format
        if (!parseAndValidateValue(key, value)) {
            NLM_LOG_WARNING("Invalid value for key '" + key + "' at line " + std::to_string(lineNumber));
            hasErrors = true;
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return !hasErrors;
}

bool Config::parseAndValidateValue(const std::string& key, const std::string& value) {
    // Check for environment variable reference
    if (value.find('$') != std::string::npos) {
        // Handle environment variables: ${VAR} or $VAR
        static const std::regex envRegex(R"(\$(?:\{([A-Za-z_][A-Za-z0-9_]*)\}|([A-Za-z_][A-Za-z0-9]*)))");
        std::smatch match;
        std::string testValue = value;
        bool hasEnvVar = false;
        
        while (std::regex_search(testValue, match, envRegex)) {
            hasEnvVar = true;
            // Check if variable exists
            const char* envVar = nullptr;
            if (match[1].matched) {
                envVar = std::getenv(match[1].str().c_str());
            } else if (match[2].matched) {
                envVar = std::getenv(match[2].str().c_str());
            }
            
            if (!envVar) {
                NLM_LOG_WARNING("Environment variable referenced in key '" + key + "' does not exist");
                return false;
            }
            
            // Replace with actual value
            testValue = std::regex_replace(testValue, envRegex, hasEnvVar ? "" : envVar);
        }
        
        // If we have environment variables but didn't replace them, it's an error
        if (hasEnvVar && testValue != value) {
            NLM_LOG_WARNING("Failed to parse environment variables in key '" + key + "'");
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with improved parsing
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Remove quotes from value
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Expand environment variables in command line args
                value = expandEnvironmentVariables(value);
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument: missing value for " + arg);
            }
        }
        // Handle -key value format with improved error checking
        else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                
                // Remove quotes from value
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Expand environment variables
                value = expandEnvironmentVariables(value);
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Missing value for command line argument: " + arg);
            }
        }
        // Handle --help and --version flags
        else if (arg == "--help") {
            printHelp();
            return true;
        } else if (arg == "--version") {
            printVersion();
            return true;
        } else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    return true;
}

std::string Config::expandEnvironmentVariables(const std::string& input) {
    std::string result = input;
    static const std::regex envRegex(R"(\$(?:\{([A-Za-z_][A-Za-z0-9_]*)\}|([A-Za-z_][A-Za-z0-9]*)))");
    
    // Replace all environment variables
    auto replaceFunc = [&](std::smatch match) {
        const char* envVar = nullptr;
        if (match[1].matched) {
            envVar = std::getenv(match[1].str().c_str());
        } else if (match[2].matched) {
            envVar = std::getenv(match[2].str().c_str());
        }
        
        if (envVar) {
            return std::string(envVar);
        } else {
            NLM_LOG_WARNING("Environment variable referenced does not exist: "$"" + (match[1].matched ? match[1].str() : match[2].str()) + "\"");
            return std::string("");
        }
    };
    
    result = std::regex_replace(result, envRegex, replaceFunc);
    return result;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Config System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Write description if available
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write the key=value pair
        file << entry.key << " = ";
        
        // Convert value to string based on type
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes and backslashes in string values
                std::string escaped = val;
                replace(escaped.begin(), escaped.end(), '\\', "\\\\");
                replace(escaped.begin(), escaped.end(), '"', "\\\"");
                file << "\"" << escaped << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << val[i] << "\"";
                    } else {
                        file << val[i];
                    }
                }
                file << "]";
            } else {
                file << val;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

std::string Config::expandEnvironmentVariables(const std::string& input) {
    std::string result = input;
    static const std::regex envRegex(R"(\$(?:\{([A-Za-z_][A-Za-z0-9_]*)\}|([A-Za-z_][A-Za-z0-9]*)))");
    
    // Replace all environment variables
    auto replaceFunc = [&](std::smatch match) {
        const char* envVar = nullptr;
        if (match[1].matched) {
            envVar = std::getenv(match[1].str().c_str());
        } else if (match[2].matched) {
            envVar = std::getenv(match[2].str().c_str());
        }
        
        if (envVar) {
            return std::string(envVar);
        } else {
            NLM_LOG_WARNING("Environment variable referenced does not exist: "$"" + (match[1].matched ? match[1].str() : match[2].str()) + "\"");
            return std::string("");
        }
    };
    
    result = std::regex_replace(result, envRegex, replaceFunc);
    return result;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_WARNING("Attempt to set empty config key");
        return;
    }
    
    // Check for reserved keys
    if (key == "help" || key == "version") {
        NLM_LOG_WARNING("Reserved key name used: '" + key + "'");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Check if source has higher priority than existing source
        if (source > it->source) {
            it->value = value;
            it->source = source;
        } else if (source == it->source) {
            it->value = value;
            NLM_LOG_DEBUG("Overwriting config value for key '" + key + "' with same source priority");
        } else {
            NLM_LOG_DEBUG("Ignoring config value for key '" + key + "' (lower source priority)");
        }
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
    if (key.empty()) {
        NLM_LOG_WARNING("Attempt to remove empty config key");
        return;
    }
    
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

void Config::printHelp() const {
    std::cout << "NLM Configuration Options:\n";
    std::cout << "=========================\n\n";
    std::cout << "Common options:\n";
    std::cout << "  --help                    Show this help message\n";
    std::cout << "  --version                 Show version information\n\n";
    
    if (!pImpl->entries.empty()) {
        std::cout << "Available configuration keys:\n";
        for (const auto& entry : pImpl->entries) {
            std::cout << "  " << entry.key;
            if (!entry.description.empty()) {
                std::cout << " - " << entry.description;
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        std::cout << "Usage examples:\n";
        std::cout << "  --neuron_count=1000\n";
        std::cout << "  --connection_probability=0.15\n";
        std::cout << "  --random_seed=$RANDOM_SEED\n";
        std::cout << "  --log_level=DEBUG\n";
    }
}

void Config::printVersion() const {
    std::cout << "NLM (熙然) - Neural Learning Machine v0.1.0\n";
    std::cout << "An experimental artificial developmental brain project\n";
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm\n";
}

std::string Config::expandEnvironmentVariables(const std::string& input) {
    std::string result = input;
    static const std::regex envRegex(R"(\$(?:\{([A-Za-z_][A-Za-z0-9_]*)\}|([A-Za-z_][A-Za-z0-9]*)))");
    
    // Replace all environment variables
    auto replaceFunc = [&](std::smatch match) {
        const char* envVar = nullptr;
        if (match[1].matched) {
            envVar = std::getenv(match[1].str().c_str());
        } else if (match[2].matched) {
            envVar = std::getenv(match[2].str().c_str());
        }
        
        if (envVar) {
            return std::string(envVar);
        } else {
            NLM_LOG_WARNING("Environment variable referenced does not exist: "$"" + (match[1].matched ? match[1].str() : match[2].str()) + "\"");
            return std::string("");
        }
    };
    
    result = std::regex_replace(result, envRegex, replaceFunc);
    return result;
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
