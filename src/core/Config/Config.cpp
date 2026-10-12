#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine parser
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing config
        clear();
        
        // Parse JSON recursively
        parseJSONValue(jsonData, "");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error parsing JSON file ") + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, fall back to simple format with a warning
    // TODO: Implement proper YAML support when YAML library is available
    NLM_LOG_WARNING("YAML parsing not yet implemented, using simple format for " + filepath);
    return loadFromSimpleFormat(filepath);
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

void Config::parseJSONValue(const nlohmann::json& jsonValue, const std::string& prefix) {
    if (jsonValue.is_object()) {
        for (const auto& item : jsonValue.items()) {
            std::string key = prefix.empty() ? item.key() : (prefix + "." + item.key());
            parseJSONValue(item.value(), key);
        }
    } else if (jsonValue.is_array()) {
        // For arrays, store as comma-separated string
        std::string arrayStr;
        for (size_t i = 0; i < jsonValue.size(); ++i) {
            if (i > 0) arrayStr += ",";
            if (jsonValue[i].is_string()) {
                arrayStr += "\"" + jsonValue[i].get<std::string>() + "\"";
            } else {
                arrayStr += std::to_string(jsonValue[i].get<double>());
            }
        }
        set(prefix, arrayStr, ConfigSource::File);
    } else {
        // Store scalar values
        ConfigValue configValue;
        if (jsonValue.is_string()) {
            configValue = jsonValue.get<std::string>();
        } else if (jsonValue.is_number_integer()) {
            configValue = jsonValue.get<int64_t>();
        } else if (jsonValue.is_number_unsigned()) {
            configValue = jsonValue.get<uint64_t>();
        } else if (jsonValue.is_number_float()) {
            configValue = jsonValue.get<double>();
        } else if (jsonValue.is_boolean()) {
            configValue = jsonValue.get<bool>();
        } else {
            // Fallback to string representation
            configValue = jsonValue.dump();
        }
        set(prefix, configValue, ConfigSource::File);
    }
}

std::string Config::toJSON() const {
    nlohmann::json jsonValue;
    
    // Convert all config entries to JSON
    for (const auto& entry : pImpl->entries) {
        // Split key by dots for nested structure
        std::string currentPath = entry.key;
        nlohmann::json* currentJson = &jsonValue;
        
        size_t dotPos = currentPath.find('.');
        while (dotPos != std::string::npos) {
            std::string keyPart = currentPath.substr(0, dotPos);
            currentPath = currentPath.substr(dotPos + 1);
            
            if (!currentJson->contains(keyPart)) {
                currentJson->emplace(keyPart, nlohmann::json::object());
            }
            currentJson = &((*currentJson)[keyPart]);
            
            dotPos = currentPath.find('.');
        }
        
        // Set the final value
        visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                (*currentJson)[currentPath] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                (*currentJson)[currentPath] = arg;
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int> ||
                                 std::is_same_v<T, uint64_t>) {
                (*currentJson)[currentPath] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                (*currentJson)[currentPath] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                (*currentJson)[currentPath] = nlohmann::json::array();
                for (const auto& item : arg) {
                    (*currentJson)[currentPath].push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                (*currentJson)[currentPath] = nlohmann::json::array();
                for (const auto& item : arg) {
                    (*currentJson)[currentPath].push_back(item);
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                (*currentJson)[currentPath] = nlohmann::json::array();
                for (const auto& item : arg) {
                    (*currentJson)[currentPath].push_back(item);
                }
            }
        }, entry.value);
    }
    
    return jsonValue.dump(2); // Pretty print with 2-space indentation
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format based on file extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        file << toJSON();
    } else {
        // Default to simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Visit the variant value and output appropriately
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n";
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
        file << entry.key << " = ";
        
        // Visit the variant value and output appropriately
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
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
