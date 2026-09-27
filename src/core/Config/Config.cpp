#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <memory>

// Include JSON and YAML libraries if available
try {
    #include <nlohmann/json.hpp>
    #define HAS_JSON 1
} catch (...) {
    #define HAS_JSON 0
}

try {
    #include <yaml-cpp/yaml.h>
    #define HAS_YAML 1
} catch (...) {
    #define HAS_YAML 0
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try different file formats based on extension
    std::string ext = "." + std::string(
        filepath.substr(filepath.find_last_of('.') + 1)
    );
    
    if (ext == ".json" || ext == ".JSON") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format for unknown extensions
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#if HAS_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing configuration
        pImpl->entries.clear();
        
        // Parse JSON recursively
        parseJSON(jsonData, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
#else
    (void)filepath; // Avoid unused parameter warning
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#if HAS_YAML
    try {
        YAML::Loader loader;
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing configuration
        pImpl->entries.clear();
        
        // Parse YAML recursively
        parseYAML(yamlData, "", ConfigSource::File);
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
#else
    (void)filepath; // Avoid unused parameter warning
    return false;
#endif
}

void Config::parseJSON(const nlohmann::json& json, const std::string& prefix, ConfigSource source) {
#if HAS_JSON
    if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            
            if (value.is_number_integer()) {
                set(fullKey, value.get<int64_t>(), source);
            } else if (value.is_number_unsigned()) {
                set(fullKey, static_cast<int64_t>(value.get<uint64_t>()), source);
            } else if (value.is_number_float()) {
                set(fullKey, value.get<double>(), source);
            } else if (value.is_boolean()) {
                set(fullKey, value.get<bool>(), source);
            } else if (value.is_string()) {
                set(fullKey, value.get<std::string>(), source);
            } else if (value.is_array()) {
                // Handle arrays
                std::vector<double> arrayData;
                for (const auto& item : value) {
                    if (item.is_number()) {
                        arrayData.push_back(item.get<double>());
                    } else {
                        arrayData.clear();
                        break;
                    }
                }
                if (!arrayData.empty()) {
                    // Convert to appropriate vector type based on content
                    set(fullKey, arrayData, source);
                }
            } else if (value.is_object()) {
                // Recursively parse nested objects
                parseJSON(value, fullKey, source);
            }
        }
    }
#endif
}

void Config::parseYAML(const YAML::Node& yaml, const std::string& prefix, ConfigSource source) {
#if HAS_YAML
    if (yaml.IsMap()) {
        for (const auto& it : yaml) {
            std::string key = it.first.IsScalar() ? it.first.Scalar() : "";
            if (key.empty()) continue;
            
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            
            if (it.second.IsScalar()) {
                std::string value = it.second.Scalar();
                // Try to convert to appropriate type
                if (isNumeric(value)) {
                    double numValue = std::stod(value);
                    // Check if it's an integer
                    if (value.find('.') == std::string::npos && value.find('e') == std::string::npos) {
                        set(fullKey, static_cast<int64_t>(numValue), source);
                    } else {
                        set(fullKey, numValue, source);
                    }
                } else if (value == "true" || value == "false") {
                    set(fullKey, value == "true", source);
                } else {
                    set(fullKey, value, source);
                }
            } else if (it.second.IsSequence()) {
                // Handle arrays
                std::vector<double> arrayData;
                for (const auto& item : it.second) {
                    if (item.IsScalar()) {
                        try {
                            double numValue = std::stod(item.Scalar());
                            arrayData.push_back(numValue);
                        } catch (...) {
                            arrayData.clear();
                            break;
                        }
                    } else {
                        arrayData.clear();
                        break;
                    }
                }
                if (!arrayData.empty()) {
                    set(fullKey, arrayData, source);
                }
            } else if (it.second.IsMap()) {
                // Recursively parse nested objects
                parseYAML(it.second, fullKey, source);
            }
        }
    }
#endif
}

bool Config::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
    }
    bool hasDecimal = false;
    bool hasExponent = false;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (std::isdigit(str[i])) continue;
        if (str[i] == '.' && !hasDecimal && !hasExponent) {
            hasDecimal = true;
            continue;
        }
        if ((str[i] == 'e' || str[i] == 'E') && i > start && !hasExponent) {
            hasExponent = true;
            // Check for optional sign after exponent
            if (i + 1 < str.size() && (str[i + 1] == '+' || str[i + 1] == '-')) {
                i++;
            }
            continue;
        }
        return false;
    }
    return true;
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
        file << entry.key << " = ";
        
        // Convert value to string representation
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
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
                    file << std::to_string(arg[i]);
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
                file << "UNSUPPORTED_TYPE";
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
namespace {
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
}

} // namespace nlm
