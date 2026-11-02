#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <variant>
#include <optional>

// JSON/YAML support - use nlohmann::json if available, otherwise implement simple parser
#ifdef NLM_HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file (JSON format)
    bool Config::loadFromFile(const std::string& filepath) {
        // Try to determine file format by extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        bool success = false;
        
        // Try JSON first (if nlohmann::json is available)
#ifdef NLM_HAS_NLOHMANN_JSON
        if (ext == ".json" || ext == ".cfg" || filepath.find('.') == std::string::npos) {
            success = loadFromJSON(filepath);
            if (success) {
                return true;
            }
        }
#endif
        
        // Try YAML (if yaml-cpp is available)
#ifdef NLM_HAS_YAML_CPP
        if (ext == ".yaml" || ext == ".yml") {
            success = loadFromYAML(filepath);
            if (success) {
                return true;
            }
        }
#endif
        
        // Fall back to simple key=value format (Phase 1)
        if (!success) {
            success = loadFromSimpleFormat(filepath);
        }
        
        return success;
    }

    // Load from JSON file
#ifdef NLM_HAS_NLOHMANN_JSON
    bool Config::loadFromJSON(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json j;
            file >> j;
            
            // Clear existing entries
            clear();
            
            // Recursively parse JSON object
            parseJSONObject(j, ConfigSource::File);
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to parse JSON config: ") + e.what());
            return false;
        }
    }
#endif

    // Load from YAML file
#ifdef NLM_HAS_YAML_CPP
    bool Config::loadFromYAML(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            YAML::Node doc;
            file >> doc;
            
            // Clear existing entries
            clear();
            
            // Recursively parse YAML node
            parseYAMLNode(doc, ConfigSource::File);
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to parse YAML config: ") + e.what());
            return false;
        }
    }
#endif

    // Load from simple key=value format (Phase 1 compatibility)
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

    // Recursively parse JSON object
#ifdef NLM_HAS_NLOHMANN_JSON
    void Config::parseJSONObject(const nlohmann::json& j, ConfigSource source) {
        if (j.is_object()) {
            for (const auto& item : j.items()) {
                std::string key = item.key();
                
                // Determine value type and set appropriately
                if (item.value().is_number_integer()) {
                    set(key, item.value().get<int64_t>(), source);
                } else if (item.value().is_number_float()) {
                    set(key, item.value().get<double>(), source);
                } else if (item.value().is_boolean()) {
                    set(key, item.value().get<bool>(), source);
                } else if (item.value().is_string()) {
                    set(key, item.value().get<std::string>(), source);
                } else if (item.value().is_array()) {
                    // Parse array - convert to appropriate vector type
                    parseJSONArray(item.value(), key, source);
                } else if (item.value().is_object()) {
                    // Nested object - could be complex, for now skip
                    NLM_LOG_WARNING(std::string("Skipping nested object in config key: ") + key);
                } else {
                    NLM_LOG_WARNING(std::string("Unknown JSON type for key: ") + key);
                }
            }
        }
    }

    // Recursively parse JSON array
    void Config::parseJSONArray(const nlohmann::json& j, const std::string& baseKey, ConfigSource source) {
        // Try to determine element type from first element
        if (j.empty()) {
            return;
        }
        
        // Check if all elements are of the same type
        bool allInts = true, allDoubles = true, allStrings = true, allBools = true;
        
        for (const auto& item : j) {
            if (!item.is_number_integer()) allInts = false;
            if (!item.is_number_float()) allDoubles = false;
            if (!item.is_string()) allStrings = false;
            if (!item.is_boolean()) allBools = false;
        }
        
        std::string key = baseKey + "[]";  // Indicate it's an array
        
        if (allInts) {
            std::vector<int64_t> values;
            values.reserve(j.size());
            for (const auto& item : j) {
                values.push_back(item.get<int64_t>());
            }
            set(key, values, source);
        } else if (allDoubles) {
            std::vector<double> values;
            values.reserve(j.size());
            for (const auto& item : j) {
                values.push_back(item.get<double>());
            }
            set(key, values, source);
        } else if (allStrings) {
            std::vector<std::string> values;
            values.reserve(j.size());
            for (const auto& item : j) {
                values.push_back(item.get<std::string>());
            }
            set(key, values, source);
        } else if (allBools) {
            std::vector<bool> values;
            values.reserve(j.size());
            for (const auto& item : j) {
                values.push_back(item.get<bool>());
            }
            set(key, values, source);
        } else {
            NLM_LOG_WARNING(std::string("Cannot parse mixed-type JSON array for key: ") + baseKey);
        }
    }
#endif

    // Recursively parse YAML node
#ifdef NLM_HAS_YAML_CPP
    void Config::parseYAMLNode(const YAML::Node& node, ConfigSource source) {
        if (node.IsMap()) {
            for (const auto& it : node) {
                std::string key = it.first.as<std::string>();
                
                // Determine value type and set appropriately
                if (it.second.IsScalar()) {
                    if (it.second.IsIntegral()) {
                        set(key, it.second.as<int64_t>(), source);
                    } else if (it.second.IsFloat()) {
                        set(key, it.second.as<double>(), source);
                    } else if (it.second.IsBoolean()) {
                        set(key, it.second.as<bool>(), source);
                    } else {
                        set(key, it.second.as<std::string>(), source);
                    }
                } else if (it.second.IsSequence()) {
                    // Parse sequence - convert to appropriate vector type
                    parseYAMLSequence(it.second, key, source);
                } else if (it.second.IsMap()) {
                    // Nested map - for now skip
                    NLM_LOG_WARNING(std::string("Skipping nested map in YAML key: ") + key);
                }
            }
        }
    }

    // Recursively parse YAML sequence
    void Config::parseYAMLSequence(const YAML::Node& node, const std::string& baseKey, ConfigSource source) {
        // Try to determine element type from first element
        if (node.size() == 0) {
            return;
        }
        
        // Check if all elements are of the same type
        bool allInts = true, allDoubles = true, allStrings = true, allBools = true;
        
        for (const auto& item : node) {
            if (item.IsIntegral()) allInts = false;
            if (item.IsFloat()) allDoubles = false;
            if (item.IsScalar() && !item.IsIntegral() && !item.IsFloat()) allStrings = false;
            if (item.IsBoolean()) allBools = false;
        }
        
        std::string key = baseKey + "[]";  // Indicate it's a sequence
        
        if (allInts) {
            std::vector<int64_t> values;
            values.reserve(node.size());
            for (const auto& item : node) {
                values.push_back(item.as<int64_t>());
            }
            set(key, values, source);
        } else if (allDoubles) {
            std::vector<double> values;
            values.reserve(node.size());
            for (const auto& item : node) {
                values.push_back(item.as<double>());
            }
            set(key, values, source);
        } else if (allStrings) {
            std::vector<std::string> values;
            values.reserve(node.size());
            for (const auto& item : node) {
                values.push_back(item.as<std::string>());
            }
            set(key, values, source);
        } else if (allBools) {
            std::vector<bool> values;
            values.reserve(node.size());
            for (const auto& item : node) {
                values.push_back(item.as<bool>());
            }
            set(key, values, source);
        } else {
            NLM_LOG_WARNING(std::string("Cannot parse mixed-type YAML sequence for key: ") + baseKey);
        }
    }
#endif

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

// Save to file (JSON format)
    bool Config::saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Create JSON object
#ifdef NLM_HAS_NLOHMANN_JSON
        nlohmann::json j;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            std::visit([&j, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                    j[entry.key] = static_cast<int64_t>(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    // Convert to JSON array
                    nlohmann::json arr = arg;
                    j[entry.key] = arr;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    nlohmann::json arr = arg;
                    j[entry.key] = arr;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    nlohmann::json arr = arg;
                    j[entry.key] = arr;
                }
            }, entry.value);
        }
        
        file << j.dump(2);  // Pretty print with 2-space indentation
#else
        // Fallback to simple format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = PLACEHOLDER_VALUE\n";
        }
#endif
        
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
