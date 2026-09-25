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

// Load from file (supports both simple key=value and JSON formats)
    bool Config::loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open config file: " + filepath);
            return false;
        }
        
        // Try to detect file format by extension and content
        std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
        
        if (extension == "json" || extension == "yaml" || extension == "yml") {
            return loadFromJsonFile(filepath);
        } else if (extension == "cfg" || extension == "ini" || filepath.find("config") != std::string::npos) {
            // For backward compatibility - simple key=value format
            return loadFromSimpleFile(filepath);
        } else {
            // Auto-detect based on first line
            std::string firstLine;
            std::getline(file, firstLine);
            file.clear();
            file.seekg(0);
            
            if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
                return loadFromJsonFile(filepath);
            } else {
                return loadFromSimpleFile(filepath);
            }
        }
    }
    
    // Load from simple key=value format (backward compatibility)
    bool Config::loadFromSimpleFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        int lineNum = 0;
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/' || line.substr(0, 2) == "//") {
                continue;
            }
            
            // Parse key=value pairs
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
        return true;
    }
    
    // Load from JSON format (Phase 2 implementation)
    bool Config::loadFromJsonFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json jsonConfig;
            file >> jsonConfig;
            
            parseJsonConfig(jsonConfig);
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }

// Load from command line arguments (new map-based interface)
    bool Config::loadFromArgs(const std::map<std::string, std::string>& cliArgs) {
        for (const auto& pair : cliArgs) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            set(key, value, ConfigSource::CommandLine);
        }
        return true;
    }
    
    // Load from command line arguments (original interface)
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

std::string Config::getValueTypeName(const ConfigValue& value) {
    std::string type;
    std::visit([&type](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) type = "int";
        else if constexpr (std::is_same_v<T, int64_t>) type = "int64_t";
        else if constexpr (std::is_same_v<T, double>) type = "double";
        else if constexpr (std::is_same_v<T, bool>) type = "bool";
        else if constexpr (std::is_same_v<T, std::string>) type = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) type = "std::vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) type = "std::vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) type = "std::vector<string>";
        else type = "unknown";
    }, value);
    return type;
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

// Parse JSON configuration
    void Config::parseJsonConfig(const nlohmann::json& jsonConfig) {
        if (!jsonConfig.is_object()) {
            return;
        }
        
        for (const auto& it : jsonConfig.items()) {
            std::string key = it.key();
            
            try {
                if (it.value().is_boolean()) {
                    set(key, it.value().get<bool>(), ConfigSource::File);
                } else if (it.value().is_number_integer()) {
                    set(key, it.value().get<int64_t>(), ConfigSource::File);
                } else if (it.value().is_number_float()) {
                    set(key, it.value().get<double>(), ConfigSource::File);
                } else if (it.value().is_string()) {
                    set(key, it.value().get<std::string>(), ConfigSource::File);
                } else if (it.value().is_array()) {
                    // Handle arrays
                    nlohmann::json array = it.value();
                    if (!array.empty()) {
                        std::string firstType = array[0].type_name();
                        if (firstType == "number_integer") {
                            std::vector<int64_t> intArray;
                            for (const auto& item : array) {
                                intArray.push_back(item.get<int64_t>());
                            }
                            set(key, intArray, ConfigSource::File);
                        } else if (firstType == "number_float") {
                            std::vector<double> floatArray;
                            for (const auto& item : array) {
                                floatArray.push_back(item.get<double>());
                            }
                            set(key, floatArray, ConfigSource::File);
                        } else if (firstType == "string") {
                            std::vector<std::string> stringArray;
                            for (const auto& item : array) {
                                stringArray.push_back(item.get<std::string>());
                            }
                            set(key, stringArray, ConfigSource::File);
                        }
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse config key '" + key + "': " + std::string(e.what()));
            }
        }
    }
