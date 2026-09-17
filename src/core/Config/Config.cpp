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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Determine file format by extension
    std::string extension = "." + filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success;
    if (extension == ".json") {
        success = loadFromJSON(file, ConfigSource::File);
    } else if (extension == ".yaml" || extension == ".yml") {
        success = loadFromYAML(file, ConfigSource::File);
    } else {
        // Default to simple key=value format
        success = loadFromSimpleFormat(file, ConfigSource::File);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
    }
    
    return success;
}

bool Config::loadFromJSON(std::istream& stream, ConfigSource source) {
    try {
        nlohmann::json jsonData;
        stream >> jsonData;
        
        // Flatten JSON structure for backward compatibility
        // Handle both nested and flat JSON structures
        flattenAndLoadJSON(jsonData, source);
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(std::istream& stream, ConfigSource source) {
    try {
        // Try to include yaml-cpp if available
        // For now, fallback to simple format with warning
        NLM_LOG_WARNING("YAML config loading not yet implemented, falling back to simple format");
        // Reset stream position - this is tricky, so we need a different approach
        stream.clear();
        stream.seekg(0);
        return loadFromSimpleFormat(stream, source);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML: " + std::string(e.what()));
        return false;
    }
}

void Config::flattenAndLoadJSON(const nlohmann::json& json, ConfigSource source) {
    if (json.is_object()) {
        // Handle nested object structure
        for (const auto& [key, value] : json.items()) {
            // Convert nested path to dot notation (e.g., "brain.neuron_count")
            std::string flatKey = flattenKey(key);
            setFromJSONValue(flatKey, value, source);
            
            // Recursively process nested objects
            if (value.is_object()) {
                flattenAndLoadJSON(value, source);
            }
        }
    } else if (json.is_array()) {
        // Handle array values - convert to JSON string for now
        std::string jsonStr = json.dump();
        set("__json_array__", ConfigValue(jsonStr), source);
    } else {
        // Root-level scalar value
        std::string flatKey = "root_value";
        setFromJSONValue(flatKey, json, source);
    }
}

std::string Config::flattenKey(const std::string& key, const nlohmann::json& parent) const {
    if (!parent.is_object() || parent.empty()) {
        return key;
    }
    
    // For now, just return the key - could be enhanced to handle nested paths
    return key;
}

void Config::setFromJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // Convert array to vector based on element type
        if (!value.empty()) {
            const auto& first = value[0];
            if (first.is_string()) {
                std::vector<std::string> vec;
                vec.reserve(value.size());
                for (const auto& item : value) {
                    vec.push_back(item.get<std::string>());
                }
                set(key, vec, source);
            } else if (first.is_number_integer()) {
                std::vector<int> vec;
                vec.reserve(value.size());
                for (const auto& item : value) {
                    vec.push_back(item.get<int>());
                }
                set(key, vec, source);
            } else if (first.is_number_float()) {
                std::vector<double> vec;
                vec.reserve(value.size());
                for (const auto& item : value) {
                    vec.push_back(item.get<double>());
                }
                set(key, vec, source);
            } else if (first.is_boolean()) {
                std::vector<bool> vec;
                vec.reserve(value.size());
                for (const auto& item : value) {
                    vec.push_back(item.get<bool>());
                }
                set(key, vec, source);
            } else {
                // Unknown type - store as JSON string
                set(key, value.dump(), source);
            }
        } else {
            // Empty array - store as empty vector
            set(key, std::vector<std::string>(), source);
        }
    } else {
        // Null or unknown type - store as string
        set(key, value.dump(), source);
    }
}

bool Config::loadFromSimpleFormat(std::istream& stream, ConfigSource source) {
    std::string line;
    size_t lineNum = 0;
    bool success = true;
    
    while (std::getline(stream, line)) {
        ++lineNum;
        
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, source);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Error parsing config line " + std::to_string(lineNum) + ": " + e.what()));
                success = false;
            }
        } else {
            // Invalid format, but don't fail - could be a section header
            NLM_LOG_WARNING(std::string("Skipping malformed config line " + std::to_string(lineNum) + ": " + line));
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Save in human-readable INI format for compatibility
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << getConfigValueAsString(entry.value) << "\n";
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
    return true;
}

std::string Config::getConfigValueAsString(const ConfigValue& value) const {
    std::string result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += arg[i] ? "true" : "false";
            }
            result += "]";
        } else {
            // For scalar types (int, double, bool)
            if constexpr (std::is_same_v<T, bool>) {
                result = arg ? "true" : "false";
            } else {
                result = std::to_string(arg);
            }
        }
    }, value);
    return result;
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
