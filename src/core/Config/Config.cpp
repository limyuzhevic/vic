#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>  // Include nlohmann JSON library

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json jsonCache;  // Cache for JSON serialization
    bool dirty = false;        // Track if changes need to be reflected
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser (Phase 3+)
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

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        pImpl->entries.clear();
        
        // Convert JSON to Config entries
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            
            // Handle different JSON value types
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_array()) {
                // For arrays, convert to string representation
                std::vector<std::string> strArray;
                for (const auto& item : *it) {
                    strArray.push_back(item.dump());
                }
                set(key, strArray, ConfigSource::File);
            } else {
                // Default to string representation for other types
                set(key, it->dump(), ConfigSource::File);
            }
        }
        
        pImpl->dirty = false;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    // TODO: Implement YAML parsing (Phase 3+)
    // For now, treat YAML as JSON
    return loadFromJSON(yamlStr);
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigEntry back to JSON
        std::visit([&json, &entry](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json[entry.key] = value;
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, uint64_t>) {
                json[entry.key] = static_cast<int64_t>(value);
            } else if constexpr (std::is_same_v<T, float> || 
                                 std::is_same_v<T, double>) {
                json[entry.key] = static_cast<double>(value);
            } else if constexpr (std::is_same_v<T, bool>) {
                json[entry.key] = value;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json arr;
                for (const auto& item : value) {
                    arr.push_back(item);
                }
                json[entry.key] = arr;
            } else {
                json[entry.key] = "UNKNOWN_TYPE";
            }
        }, entry.value);
    }
    
    file << json.dump(4);  // Pretty print with 4-space indentation
    return true;
}

std::string Config::toJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&json, &entry](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json[entry.key] = value;
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, uint64_t>) {
                json[entry.key] = static_cast<int64_t>(value);
            } else if constexpr (std::is_same_v<T, float> || 
                                 std::is_same_v<T, double>) {
                json[entry.key] = static_cast<double>(value);
            } else if constexpr (std::is_same_v<T, bool>) {
                json[entry.key] = value;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json arr;
                for (const auto& item : value) {
                    arr.push_back(item);
                }
                json[entry.key] = arr;
            } else {
                json[entry.key] = "UNKNOWN_TYPE";
            }
        }, entry.value);
    }
    
    return json.dump(4);  // Pretty print
}

std::string Config::toYAML() const {
    // For now, return JSON representation as YAML
    // TODO: Implement proper YAML serialization (Phase 3+)
    return toJSON();
}

bool Config::validate() const {
    // Basic validation - check for required keys
    for (const auto& entry : pImpl->entries) {
        // Log validation warnings for certain keys
        if (entry.key == "neuron_count" && entry.value.index() == 0) {  // int64_t
            int64_t count = std::get<int64_t>(entry.value);
            if (count <= 0) {
                NLM_LOG_WARNING("neuron_count should be positive: " + std::to_string(count));
                return false;
            }
        } else if (entry.key == "connection_probability" && entry.value.index() == 2) {  // double
            double prob = std::get<double>(entry.value);
            if (prob < 0.0 || prob > 1.0) {
                NLM_LOG_WARNING("connection_probability should be between 0 and 1: " + std::to_string(prob));
                return false;
            }
        }
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
    
    pImpl->dirty = true;
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
