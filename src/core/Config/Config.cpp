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
    // Detect file format based on extension
    std::string extension = detectFileFormat(filepath);
    
    if (isJSONFormat(extension)) {
        return loadFromJSON(filepath);
    } else if (isYAMLFormat(extension)) {
        return loadFromYAML(filepath);
    } else if (isLegacyFormat(extension)) {
        return loadFromLegacy(filepath);
    } else {
        // Default to legacy format for unknown extensions
        return loadFromLegacy(filepath);
    }
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

// File format detection
std::string Config::detectFileFormat(const std::string& filepath) {
    std::string extension = "";
    size_t pos = filepath.find_last_of('.');
    if (pos != std::string::npos) {
        extension = toLower(filepath.substr(pos + 1));
    }
    return extension;
}

bool Config::isJSONFormat(const std::string& extension) {
    return extension == "json" || extension == "jsonc";
}

bool Config::isYAMLFormat(const std::string& extension) {
    return extension == "yaml" || extension == "yml";
}

bool Config::isLegacyFormat(const std::string& extension) {
    return extension == "cfg" || extension.empty();
}

// JSON/YAML parsing helpers
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        if (!j.is_object()) {
            return handleParseError("JSON file does not contain an object");
        }
        
        clear();
        parseNestedConfig(j);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return handleParseError("JSON parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        return handleParseError("JSON read error: " + std::string(e.what()));
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc;
        file >> doc;
        
        if (doc.IsNull()) {
            return handleParseError("YAML file is empty or null");
        }
        
        clear();
        parseNestedConfig(doc);
        return true;
    } catch (const YAML::Exception& e) {
        return handleParseError("YAML parse error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        return handleParseError("YAML read error: " + std::string(e.what()));
    }
}

bool Config::loadFromLegacy(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        clear();
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
    } catch (const std::exception& e) {
        return handleParseError("Legacy format error: " + std::string(e.what()));
    }
}

// JSON/YAML to ConfigValue conversion
ConfigValue Config::jsonToConfigValue(const nlohmann::json& j, const std::string& key) {
    if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number()) {
        // Check if it's an integer type
        if (j.is_number_integer()) {
            return j.get<int64_t>();
        } else {
            return j.get<double>();
        }
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Determine array type
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : j) {
                    intArray.push_back(item.get<int>());
                }
                return intArray;
            } else if (j[0].is_number()) {
                std::vector<double> doubleArray;
                for (const auto& item : j) {
                    doubleArray.push_back(item.get<double>());
                }
                return doubleArray;
            } else if (j[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : j) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        // Fallback to empty int array
        return std::vector<int>{};
    } else if (j.is_null()) {
        return std::string("") ;
    } else {
        // For complex objects, convert to JSON string
        return j.dump();
    }
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& node, const std::string& key) {
    if (node.IsScalar()) {
        if (node.IsBoolean()) {
            return node.as<bool>();
        } else if (node.IsInteger()) {
            return node.as<int64_t>();
        } else if (node.IsDouble()) {
            return node.as<double>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        // Determine sequence type
        if (node.size() > 0) {
            if (node[0].IsInteger()) {
                std::vector<int> intArray;
                for (const auto& item : node) {
                    intArray.push_back(item.as<int>());
                }
                return intArray;
            } else if (node[0].IsDouble()) {
                std::vector<double> doubleArray;
                for (const auto& item : node) {
                    doubleArray.push_back(item.as<double>());
                }
                return doubleArray;
            } else if (node[0].IsScalar()) {
                std::vector<std::string> stringArray;
                for (const auto& item : node) {
                    stringArray.push_back(item.as<std::string>());
                }
                return stringArray;
            }
        }
        // Fallback to empty int array
        return std::vector<int>{};
    } else if (node.IsMap()) {
        // For nested objects, convert to YAML string
        std::stringstream ss;
        ss << node;
        return ss.str();
    } else {
        return std::string("") ;
    }
}

// Type checking and conversion
template<typename T>
bool Config::valueMatchesType(const ConfigValue& value) {
    return std::holds_alternative<T>(value);
}

bool Config::tryConvertToType(const ConfigValue& from, ConfigValue& to) {
    // Try various conversions
    if (std::holds_alternative<int>(from)) {
        int val = std::get<int>(from);
        to = val;
        return true;
    } else if (std::holds_alternative<int64_t>(from)) {
        int64_t val = std::get<int64_t>(from);
        to = static_cast<int>(val);
        return true;
    } else if (std::holds_alternative<double>(from)) {
        double val = std::get<double>(from);
        to = static_cast<int>(val);
        return true;
    }
    return false;
}

// Error handling
bool Config::handleParseError(const std::string& error) {
    // Log error and return false
    return false;
}

// Configuration parsing
void Config::parseNestedConfig(const nlohmann::json& j, const std::string& prefix) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it.value().is_object()) {
            parseNestedConfig(it.value(), key);
        } else {
            ConfigValue value = jsonToConfigValue(it.value(), it.key());
            set(key, value, ConfigSource::File);
        }
    }
}

void Config::parseNestedConfig(const YAML::Node& node, const std::string& prefix) {
    if (!node.IsMap()) {
        return;
    }
    
    for (auto it = node.begin(); it != node.end(); ++it) {
        std::string key = it->first.as<std::string>();
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
        
        if (it->second.IsMap()) {
            parseNestedConfig(it->second, fullKey);
        } else {
            ConfigValue value = yamlToConfigValue(it->second, key);
            set(fullKey, value, ConfigSource::File);
        }
    }
}
