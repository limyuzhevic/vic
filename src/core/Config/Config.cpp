// Config.cpp - Improved configuration system for NLM
// Provides robust configuration management with JSON support,
// validation, environment variables, and comprehensive error handling.

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <exception>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Pre-computed case-insensitive lookup for faster key access
    std::unordered_map<std::string, size_t> caseInsensitiveIndex;
    
    Impl() : caseInsensitiveIndex() {}
    
    void rebuildIndex() {
        caseInsensitiveIndex.clear();
        for (size_t i = 0; i < entries.size(); ++i) {
            caseInsensitiveIndex[entries[i].key] = i;
            // Also add lowercase version for case-insensitive lookup
            std::string lowerKey = toLower(entries[i].key);
            caseInsensitiveIndex[lowerKey] = i;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format
    std::string ext = "." + std::string(std::filesystem::path(filepath).extension().string());
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json" || ext == ".json5" || ext == ".jsonc") {
        success = loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(filepath);
    } else {
        success = loadFromSimpleFormat(filepath);
    }
    
    if (success) {
        pImpl->rebuildIndex();
    }
    return success;
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAS_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert JSON to config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Store the description if present
            std::string description = "";
            if (jsonData[key].contains("_description")) {
                description = jsonData[key]["_description"];
            }
            
            // Store the value based on JSON type
            ConfigSource source = ConfigSource::File;
            
            if (it->is_string()) {
                set(key, it->get<std::string>(), source);
                if (!description.empty()) {
                    setDescription(key, description);
                }
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), source);
                if (!description.empty()) {
                    setDescription(key, description);
                }
            } else if (it->is_number_unsigned()) {
                set(key, it->get<uint64_t>(), source);
                if (!description.empty()) {
                    setDescription(key, description);
                }
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), source);
                if (!description.empty()) {
                    setDescription(key, description);
                }
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), source);
                if (!description.empty()) {
                    setDescription(key, description);
                }
            } else if (it->is_array()) {
                // Handle arrays - determine element type
                if (!it->empty() && it->front().is_number_integer()) {
                    std::vector<int64_t> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<int64_t>());
                    }
                    set(key, vec, source);
                } else if (!it->empty() && it->front().is_number_float()) {
                    std::vector<double> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<double>());
                    }
                    set(key, vec, source);
                } else if (!it->empty() && it->front().is_string()) {
                    std::vector<std::string> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<std::string>());
                    }
                    set(key, vec, source);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    // Fall back to simple format if nlohmann::json is not available
    return loadFromSimpleFormat(filepath);
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAS_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node yamlData = YAML::Load(file);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert YAML to config entries
        for (const auto& pair : yamlData) {
            std::string key = pair.first.as<std::string>();
            
            // Store the description if present
            std::string description = "";
            if (yamlData[key]["_description"]) {
                description = yamlData[key]["_description"].as<std::string>();
            }
            
            ConfigSource source = ConfigSource::File;
            
            if (pair.second.IsScalar()) {
                if (yamlData[key].IsString()) {
                    set(key, pair.second.as<std::string>(), source);
                } else if (yamlData[key].IsInteger()) {
                    set(key, pair.second.as<int64_t>(), source);
                } else if (yamlData[key].IsFloat()) {
                    set(key, pair.second.as<double>(), source);
                } else if (yamlData[key].IsBoolean()) {
                    set(key, pair.second.as<bool>(), source);
                }
            } else if (pair.second.IsSequence()) {
                // Handle sequences - determine element type
                if (!pair.second.empty() && pair.second[0].IsInteger()) {
                    std::vector<int64_t> vec;
                    for (const auto& item : pair.second) {
                        vec.push_back(item.as<int64_t>());
                    }
                    set(key, vec, source);
                } else if (!pair.second.empty() && pair.second[0].IsFloat()) {
                    std::vector<double> vec;
                    for (const auto& item : pair.second) {
                        vec.push_back(item.as<double>());
                    }
                    set(key, vec, source);
                } else if (!pair.second.empty() && pair.second[0].IsScalar()) {
                    std::vector<std::string> vec;
                    for (const auto& item : pair.second) {
                        vec.push_back(item.as<std::string>());
                    }
                    set(key, vec, source);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
#else
    // Fall back to simple format if yaml-cpp is not available
    return loadFromSimpleFormat(filepath);
#endif
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line.find("#") == std::string::npos) {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Try to parse key: value format (for more structured data)
            pos = line.find(':');
            if (pos == std::string::npos) {
                // Invalid line format
                hasErrors = true;
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Determine value type
            if (value.empty()) {
                set(key, std::string(""), ConfigSource::File);
            } else if (value[0] == '"' && value.back() == '"') {
                // String value
                set(key, value.substr(1, value.size() - 2), ConfigSource::File);
            } else if (value[0] == '\'' && value.back() == '\'') {
                // String value with single quotes
                set(key, value.substr(1, value.size() - 2), ConfigSource::File);
            } else {
                // Try to detect number type
                try {
                    if (value.find('.') != std::string::npos) {
                        // Double value
                        double doubleVal = std::stod(value);
                        set(key, doubleVal, ConfigSource::File);
                    } else {
                        // Integer value
                        int64_t intVal = std::stoll(value);
                        set(key, intVal, ConfigSource::File);
                    }
                } catch (...) {
                    // Boolean value
                    if (value == "true" || value == "1") {
                        set(key, true, ConfigSource::File);
                    } else if (value == "false" || value == "0") {
                        set(key, false, ConfigSource::File);
                    } else {
                        // String value
                        set(key, value, ConfigSource::File);
                    }
                }
            }
        } else {
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
    
    return !hasErrors;
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
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated on " << getCurrentTimestamp() << std::endl;
    file << "# Format: key = value (simple format)" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        // Skip internal keys that shouldn't be saved
        if (entry.key == "" || entry.key.find("_internal") == 0) {
            continue;
        }
        
        file << "# " << entry.description << std::endl;
        file << entry.key << " = " << formatValueForFile(entry.value) << std::endl;
        file << std::endl;
    }
    
    return file.good();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Try exact match first
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it == pImpl->entries.end()) {
        // Try case-insensitive match
        std::string lowerKey = toLower(key);
        auto it2 = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&lowerKey](const ConfigEntry& e) { return toLower(e.key) == lowerKey; });
        if (it2 != pImpl->entries.end()) {
            it = it2;
        }
    }
    
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
    // Remove existing entry if present
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, "");
    }
    
    pImpl->rebuildIndex();
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

void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

bool Config::has(const std::string& key) const {
    // Try exact match first
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        return true;
    }
    
    // Try case-insensitive match
    std::string lowerKey = toLower(key);
    auto it2 = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&lowerKey](const ConfigEntry& e) { return toLower(e.key) == lowerKey; });
    return it2 != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
        pImpl->rebuildIndex();
    }
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
    pImpl->rebuildIndex();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << std::endl;
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

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Config::formatValueForFile(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
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