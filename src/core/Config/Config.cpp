#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

// JSON support
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

// YAML support
#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

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
        return false;
    }
    
    // Detect file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    // Try JSON if nlohmann/json is available
    #ifdef HAVE_NLOHMANN_JSON
    if (ext == ".json" || ext == ".jsonc") {
        try {
            auto data = nlohmann::json::parse(file);
            success = parseJSON(data);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    #endif
    
    // Try YAML if yaml-cpp is available
    #ifdef HAVE_YAML_CPP
    if (!success && (ext == ".yaml" || ext == ".yml")) {
        try {
            YAML::Node data = YAML::LoadFile(filepath);
            success = parseYAML(data);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse YAML file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    #endif
    
    // Default: try key=value format
    if (!success) {
        file.clear();
        file.seekg(0);
        
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
        success = true;
    }
    
    return success;
}

bool Config::parseJSON(const nlohmann::json& data) {
    if (!data.is_object()) {
        return false;
    }
    
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string key = it.key();
        const auto& value = it.value();
        
        ConfigValue configValue;
        if (value.is_number_integer()) {
            configValue = value.get<int64_t>();
        } else if (value.is_number_unsigned()) {
            configValue = value.get<uint64_t>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_array()) {
            // Handle arrays
            if (value.empty()) {
                // Empty array - choose appropriate type
                configValue = std::vector<double>{};
            } else {
                // Try to determine array type
                bool allInts = true;
                bool allDoubles = true;
                bool allStrings = true;
                
                for (const auto& item : value) {
                    if (!item.is_number_integer()) allInts = false;
                    if (!item.is_number()) allDoubles = false;
                    if (!item.is_string()) allStrings = false;
                }
                
                if (allInts) {
                    std::vector<int64_t> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.get<int64_t>());
                    }
                    configValue = vec;
                } else if (allDoubles) {
                    std::vector<double> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.get<double>());
                    }
                    configValue = vec;
                } else if (allStrings) {
                    std::vector<std::string> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.get<std::string>());
                    }
                    configValue = vec;
                } else {
                    // Mixed types - use strings for everything
                    std::vector<std::string> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.dump());
                    }
                    configValue = vec;
                }
            }
        } else {
            // Unknown type - convert to string
            configValue = value.dump();
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

bool Config::parseYAML(const YAML::Node& data) {
    if (!data.IsMap()) {
        return false;
    }
    
    for (const auto& item : data) {
        std::string key = item.first.as<std::string>();
        const YAML::Node& value = item.second;
        
        ConfigValue configValue;
        if (value.IsScalar()) {
            // Check if it's a boolean
            if (value.IsBoolean()) {
                configValue = value.as<bool>();
            }
            // Check if it's an integer
            else if (value.IsInt()) {
                configValue = value.as<int64_t>();
            }
            // Check if it's a double
            else if (value.IsDouble()) {
                configValue = value.as<double>();
            }
            // Otherwise treat as string
            else {
                configValue = value.as<std::string>();
            }
        } else if (value.IsSequence()) {
            // Handle arrays - determine type
            std::vector<ConfigValue> vec;
            for (const auto& item : value) {
                ConfigValue itemValue;
                if (item.IsScalar()) {
                    if (item.IsBoolean()) {
                        itemValue = item.as<bool>();
                    } else if (item.IsInt()) {
                        itemValue = item.as<int64_t>();
                    } else if (item.IsDouble()) {
                        itemValue = item.as<double>();
                    } else {
                        itemValue = item.as<std::string>();
                    }
                } else {
                    // Nested structure - convert to string
                    itemValue = "{";
                    // Convert to string representation
                    std::stringstream ss;
                    ss << item;
                    itemValue = ss.str();
                }
                vec.push_back(itemValue);
            }
            configValue = vec;
        } else {
            // Complex structure - convert to string
            std::stringstream ss;
            ss << value;
            configValue = ss.str();
        }
        
        set(key, configValue, ConfigSource::File);
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
        file << entry.key << " = ";
        
        // Convert ConfigValue to string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double>) {
                file << arg;
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
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n";
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
