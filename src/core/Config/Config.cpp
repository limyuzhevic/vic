#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp> // JSON parsing

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser with nlohmann/json
    // Use JSON format for better configuration management
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    try {
        // Parse JSON configuration file
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process JSON entries
        if (jsonConfig.contains("config")) {
            // Handle nested config section
            const auto& configSection = jsonConfig["config"];
            processJSONSection(configSection);
        } else {
            // Handle flat config (legacy support)
            processJSONSection(jsonConfig);
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON: " + filepath);
        return true;
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in config file " + filepath + ": " + std::string(e.what()));
        // Fall back to legacy key=value format
        file.clear();
        file.seekg(0);
        return loadLegacyFormat(file, filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::processJSONSection(const nlohmann::json& jsonConfig) {
    for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
        std::string key = it.key();
        
        // Convert JSON value to ConfigValue
        ConfigValue value;
        ConfigSource source = ConfigSource::File;
        
        if (it->is_string()) {
            value = it->get<std::string>();
        } else if (it->is_number_integer()) {
            value = it->get<int64_t>();
        } else if (it->is_number_unsigned()) {
            value = static_cast<int64_t>(it->get<uint64_t>());
        } else if (it->is_number_float()) {
            value = it->get<double>();
        } else if (it->is_boolean()) {
            value = it->get<bool>();
        } else if (it->is_array()) {
            // Handle array values (e.g., lists of integers)
            const auto& array = *it;
            if (!array.empty()) {
                if (array[0].is_number_integer()) {
                    std::vector<int64_t> intArray;
                    for (const auto& item : array) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    value = intArray;
                } else if (array[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : array) {
                        doubleArray.push_back(item.get<double>());
                    }
                    value = doubleArray;
                } else if (array[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : array) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    value = stringArray;
                }
            }
        }
        
        // Add entry if we successfully parsed it
        if (std::holds_alternative<std::monostate>(value)) {
            // Failed to parse, skip this entry
            NLM_LOG_WARNING("Could not parse config value for key: " + key);
            continue;
        }
        
        set(key, value, source);
    }
}

bool Config::loadLegacyFormat(std::ifstream& file, const std::string& filepath) {
    // Fall back to legacy key=value format
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
    
    NLM_LOG_INFO("Loaded legacy format from config file: " + filepath);
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
        NLM_LOG_ERROR("Cannot write to config file: " + filepath);
        return false;
    }
    
    // Create JSON structure
    nlohmann::json jsonConfig;
    nlohmann::json configSection;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON
        std::string description = "Configuration parameter: " + entry.key;
        
        if (std::holds_alternative<int>(entry.value)) {
            configSection[entry.key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            configSection[entry.key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            configSection[entry.key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            configSection[entry.key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            configSection[entry.key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            configSection[entry.key] = std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            configSection[entry.key] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            configSection[entry.key] = std::get<std::vector<std::string>>(entry.value);
        }
    }
    
    // Add config metadata
    jsonConfig["config"] = configSection;
    jsonConfig["version"] = "2.0";
    jsonConfig["description"] = "NLM Neural Learning Machine Configuration";
    
    // Write JSON to file with pretty formatting
    file << std::setw(2) << jsonConfig;
    
    NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
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
