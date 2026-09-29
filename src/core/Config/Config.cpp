#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <cmath>

// Try to include nlohmann/json for better JSON support
try {
    #include "nlohmann/json.hpp"
    using json = nlohmann::json;
    #define HAS_JSON_SUPPORT 1
} catch (...) {
    #define HAS_JSON_SUPPORT 0
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Optional JSON document for better parsing
    #if HAS_JSON_SUPPORT
    std::shared_ptr<json> jsonDoc;
    #endif
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to parse JSON value to ConfigValue
static ConfigValue jsonToConfigValue(const json& j) {
    ConfigValue result;
    
    if (j.is_null()) {
        result = "";
    } else if (j.is_boolean()) {
        result = j.get<bool>();
    } else if (j.is_number_integer()) {
        int64_t intVal = j.get<int64_t>();
        // Determine if it fits in int
        if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
            result = static_cast<int>(intVal);
        } else {
            result = intVal;
        }
    } else if (j.is_number_unsigned()) {
        uint64_t uintVal = j.get<uint64_t>();
        if (uintVal <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
            result = static_cast<int64_t>(uintVal);
        } else {
            result = static_cast<int64_t>(uintVal);
        }
    } else if (j.is_number()) {
        double doubleVal = j.get<double>();
        // Check if it's actually an integer
        if (std::floor(doubleVal) == doubleVal && 
            doubleVal >= std::numeric_limits<int64_t>::min() && 
            doubleVal <= std::numeric_limits<int64_t>::max()) {
            result = static_cast<int64_t>(doubleVal);
        } else {
            result = doubleVal;
        }
    } else if (j.is_string()) {
        result = j.get<std::string>();
    } else if (j.is_array()) {
        // Try to infer array type
        bool allInts = std::all_of(j.begin(), j.end(), [](const json& elem) { return elem.is_number_integer(); });
        bool allDoubles = std::all_of(j.begin(), j.end(), [](const json& elem) { return elem.is_number(); });
        bool allStrings = std::all_of(j.begin(), j.end(), [](const json& elem) { return elem.is_string(); });
        
        if (allInts) {
            std::vector<int> intVec;
            intVec.reserve(j.size());
            for (const auto& elem : j) {
                intVec.push_back(elem.get<int>());
            }
            result = intVec;
        } else if (allDoubles) {
            std::vector<double> doubleVec;
            doubleVec.reserve(j.size());
            for (const auto& elem : j) {
                doubleVec.push_back(elem.get<double>());
            }
            result = doubleVec;
        } else if (allStrings) {
            std::vector<std::string> stringVec;
            stringVec.reserve(j.size());
            for (const auto& elem : j) {
                stringVec.push_back(elem.get<std::string>());
            }
            result = stringVec;
        } else {
            // Mixed array - store as JSON string for now
            result = j.dump();
        }
    } else {
        // Fallback to string representation
        result = j.dump();
    }
    
    return result;
}

// Helper function to convert ConfigValue to JSON
static json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return json(arg);
        } else {
            return "";
        }
    }, value);
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                         std::istreambuf_iterator<char>());
    
    #if HAS_JSON_SUPPORT
    // Try to parse as JSON first
    try {
        auto jsonDoc = std::make_shared<json>(json::parse(content));
        pImpl->jsonDoc = jsonDoc;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Flatten JSON into key-value pairs
        for (const auto& [key, value] : *jsonDoc) {
            ConfigValue configValue = jsonToConfigValue(value);
            pImpl->entries.emplace_back(key, configValue, ConfigSource::File, "");
        }
        
        return true;
    } catch (const json::parse_error& e) {
        // Fall back to simple key=value format
        pImpl->jsonDoc.reset();
    }
    #endif
    
    // Fallback to simple key=value format for backward compatibility
    pImpl->entries.clear();
    pImpl->jsonDoc.reset();
    
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
    
    #if HAS_JSON_SUPPORT
    // If we have a JSON document, save it as JSON
    if (pImpl->jsonDoc) {
        // Add all entries to the JSON document
        json jsonDoc = *pImpl->jsonDoc;
        for (const auto& entry : pImpl->entries) {
            jsonDoc[entry.key] = configValueToJson(entry.value);
        }
        
        // Write pretty JSON
        file << jsonDoc.dump(2);
        return true;
    }
    #endif
    
    // Fallback to simple key=value format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format based on value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Check if it's an integer to avoid decimal point
                if (std::floor(arg) == arg && std::abs(arg) < 1e6) {
                    file << static_cast<int64_t>(arg);
                } else {
                    file << arg;
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    // Format double
                    if (std::floor(arg[i]) == arg[i] && std::abs(arg[i]) < 1e6) {
                        file << static_cast<int64_t>(arg[i]);
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << "";
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
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
    pImpl->jsonDoc.reset();
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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
#define INSTANTIATE_GET(T) template std::optional<T> Config::get<T>(const std::string&) const;

INSTANTIATE_GET(int)
INSTANTIATE_GET(int64_t)
INSTANTIATE_GET(double)
INSTANTIATE_GET(bool)
INSTANTIATE_GET(std::string)

#define INSTANTIATE_GETOR(T) template T Config::getOr<T>(const std::string&, const T&) const;

INSTANTIATE_GETOR(int)
INSTANTIATE_GETOR(int64_t)
INSTANTIATE_GETOR(double)
INSTANTIATE_GETOR(bool)
INSTANTIATE_GETOR(std::string)

#undef INSTANTIATE_GET
#undef INSTANTIATE_GETOR

} // namespace nlm
