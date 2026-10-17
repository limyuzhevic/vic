#include <pybind11/pybind11.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json cachedJson;  // Cache for JSON serialization
    bool jsonDirty;             // Flag to indicate if JSON cache needs update
    
    Impl() : jsonDirty(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (Phase 2: proper JSON/YAML parser)
    if (loadFromJSON(filepath)) {
        NLM_LOG_INFO("Configuration loaded from JSON file: " + filepath);
        return true;
    }
    
    // Try YAML (Phase 2: support YAML files)
    if (loadFromYAML(filepath)) {
        NLM_LOG_INFO("Configuration loaded from YAML file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format (Phase 1: backward compatibility)
    NLM_LOG_INFO("Falling back to simple key=value format for: " + filepath);
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Clear existing config
        pImpl->entries.clear();
        pImpl->jsonDirty = true;
        
        // Parse JSON and populate config entries
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            ConfigSource source = ConfigSource::File;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            const auto& value = *it;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                // Try to determine if it's int or int64_t
                if (value.get<int64_t>() <= std::numeric_limits<int>::max() &&
                    value.get<int64_t>() >= std::numeric_limits<int>::min()) {
                    configValue = value.get<int>();
                } else {
                    configValue = value.get<int64_t>();
                }
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle array types - convert to vector
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    if (item.is_string()) {
                        stringArray.push_back(item.get<std::string>());
                    } else if (item.is_number_integer()) {
                        stringArray.push_back(std::to_string(item.get<int64_t>()));
                    } else if (item.is_number_float()) {
                        stringArray.push_back(std::to_string(item.get<double>()));
                    } else {
                        // Unsupported type in array
                        return false;
                    }
                }
                configValue = stringArray;
            } else {
                // Unsupported type
                NLM_LOG_WARNING("Unsupported JSON value type for key '" + key + "'");
                continue;
            }
            
            pImpl->entries.emplace_back(key, configValue, source, "");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration file '" + filepath + "': " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Try to load YAML if libyaml-cpp is available
    // This is a placeholder for Phase 2 YAML support
    NLM_LOG_INFO("YAML parsing not yet implemented - using JSON fallback for: " + filepath);
    return false;
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
    
    pImpl->jsonDirty = true;
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
    
    pImpl->jsonDirty = true;
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveToJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to JSON
            nlohmann::json jsonValue;
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, int>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    // Convert int vector to string vector for JSON
                    std::vector<std::string> stringVec;
                    for (int v : val) {
                        stringVec.push_back(std::to_string(v));
                    }
                    jsonValue = stringVec;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    // Convert double vector to string vector for JSON
                    std::vector<std::string> stringVec;
                    for (double v : val) {
                        stringVec.push_back(std::to_string(v));
                    }
                    jsonValue = stringVec;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue = val;
                }
            }, entry.value);
            
            jsonConfig[entry.key] = jsonValue;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write pretty-printed JSON
        file << std::setw(2) << jsonConfig << std::endl;
        
        NLM_LOG_INFO("Configuration saved to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save configuration to JSON file '" + filepath + "': " + e.what());
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    // Placeholder for YAML support
    NLM_LOG_WARNING("YAML saving not yet implemented - using JSON format for: " + filepath);
    return saveToJSON(filepath);
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, int> ||
                                std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << "["; // Simple representation for arrays
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
        NLM_LOG_WARNING("Type mismatch for key '" + key + "': requested " +
                       std::string(typeid(T).name()) + " but stored different type");
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
    
    pImpl->jsonDirty = true;
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
    
    pImpl->jsonDirty = true;
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
    pImpl->jsonDirty = true;
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
