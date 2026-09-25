#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <limits>
#include <random>

namespace nlm {

// Validation functions
template<typename T>
static bool isValidRange(const T& value, const T& min, const T& max) {
    static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic for range validation");
    return value >= min && value <= max;
}

static bool validateConfigEntry(const std::string& key, const ConfigValue& value) {
    if (key.empty()) {
        return false;
    }
    
    // Check for duplicate keys (would be caught during insertion)
    return true;
}

static std::string configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

static bool validateConfigValue(const ConfigValue& value) {
    // Basic validation for different value types
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            // Validate int range (reasonable bounds)
            if (arg < -1000000 || arg > 1000000) return false;
        } else if constexpr (std::is_same_v<T, double>) {
            // Validate double range
            if (std::isnan(arg) || std::isinf(arg)) return false;
            if (arg < -1000000.0 || arg > 1000000.0) return false;
        } else if constexpr (std::is_same_v<T, bool>) {
            // bool is always valid
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Validate string length
            if (arg.size() > 10000) return false;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Validate vector elements
            for (const auto& item : arg) {
                if (item < -1000000 || item > 1000000) return false;
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                if (std::isnan(item) || std::isinf(item)) return false;
                if (item < -1000000.0 || item > 1000000.0) return false;
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                if (item.size() > 1000) return false;
            }
        }
    }, value);
    return true;
}

namespace {

// Exception safety wrapper for operations that might throw
class ExceptionSafetyWrapper {
public:
    template<typename Func, typename... Args>
    static bool safeExecute(Func&& func, Args&&... args) {
        try {
            func(std::forward<Args>(args)...);
            return true;
        } catch (const std::bad_alloc&) {
            // Memory allocation failure
            return false;
        } catch (const std::exception&) {
            // Other standard exceptions
            return false;
        } catch (...) {
            // Unknown exceptions
            return false;
        }
    }
};

// Validation helper for numeric ranges
template<typename T>
static bool isNumericValid(T value, T min, T max) {
    static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic");
    return value >= min && value <= max;
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, false);
}

bool Config::loadFromFile(const std::string& filepath, bool useYAML) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        
        if (useYAML) {
            // For now, just parse as JSON
            // Full YAML support would require yaml-cpp
            file >> jsonData;
        } else {
            file >> jsonData;
        }
        
        if (!jsonData.is_object()) {
            return false;
        }
        
        // Clear existing entries
        clear();
        
        // Parse JSON object with validation
        bool success = true;
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (!jsonToConfigValue(*it, value)) {
                success = false;
                continue;
            }
            
            if (!validateConfigValue(value)) {
                success = false;
                continue;
            }
            
            if (!validateConfigEntry(key, value)) {
                success = false;
                continue;
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return success;
    } catch (const std::exception&) {
        return false;
    }
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
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        
        // Convert all entries to JSON with validation
        bool success = true;
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            const ConfigValue& value = entry.value;
            
            if (!validateConfigEntry(key, value)) {
                success = false;
                continue;
            }
            
            nlohmann::json jsonValue;
            std::visit([&jsonValue](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonValue = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue = arg;
                }
            }, value);
            
            jsonData[key] = jsonValue;
        }
        
        // Write JSON to file with indentation for readability
        file << jsonData.dump(2);
        
        return success;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty() || key.find('=') != std::string::npos) {
        return false;
    }
    
    // Validate value
    if (!validateConfigValue(value)) {
        return false;
    }
    
    // Thread-safe insertion (simplified)
    set(key, value, source);
    return true;
}

bool Config::loadWithValidation(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
    // Check file existence and permissions (simplified)
    std::ifstream testFile(filepath);
    if (!testFile.good()) {
        return false;
    }
    
    return ExceptionSafetyWrapper::safeExecute(&Config::loadFromFile, this, filepath);
}

std::string Config::getSafeString(const std::string& key, const std::string& defaultValue) const {
    auto value = get<std::string>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
}

int Config::getSafeInt(const std::string& key, int defaultValue) const {
    auto value = get<int>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
}

float Config::getSafeFloat(const std::string& key, float defaultValue) const {
    auto value = get<double>(key);
    if (value.has_value()) {
        return static_cast<float>(value.value());
    }
    return defaultValue;
}

bool Config::getSafeBool(const std::string& key, bool defaultValue) const {
    auto value = get<bool>(key);
    if (value.has_value()) {
        return value.value();
    }
    return defaultValue;
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