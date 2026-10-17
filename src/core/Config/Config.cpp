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
    // Try JSON parsing first (modern approach)
    auto jsonResult = loadFromJSON(filepath);
    if (jsonResult) {
        return true;
    }
    
    // Fall back to simple key=value format (legacy)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
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

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        
        // Clear existing entries
        clear();
        
        // Parse all configuration entries from JSON
        if (json.is_object()) {
            for (auto& [key, value] : json.items()) {
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Handle arrays
                    if (!value.empty()) {
                        // Determine array element type
                        auto firstElement = value[0];
                        if (firstElement.is_string()) {
                            std::vector<std::string> strArray;
                            strArray.reserve(value.size());
                            for (const auto& item : value) {
                                strArray.push_back(item.get<std::string>());
                            }
                            configValue = strArray;
                        } else if (firstElement.is_number_integer()) {
                            std::vector<int> intArray;
                            intArray.reserve(value.size());
                            for (const auto& item : value) {
                                intArray.push_back(item.get<int>());
                            }
                            configValue = intArray;
                        } else if (firstElement.is_number_float()) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(value.size());
                            for (const auto& item : value) {
                                doubleArray.push_back(item.get<double>());
                            }
                            configValue = doubleArray;
                        }
                    }
                }
                
                if (!value.is_null()) {
                    set(key, configValue, ConfigSource::File);
                }
            }
            
            NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
            return true;
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_DEBUG("JSON parse error (falling back to simple format): " + std::string(e.what()));
        return false;
    } catch (const nlohmann::json::type_error& e) {
        NLM_LOG_DEBUG("JSON type error (falling back to simple format): " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_DEBUG("Error loading JSON file (falling back to simple format): " + std::string(e.what()));
        return false;
    }
    
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            continue;
        }
        
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
        else if (arg[0] == '-' && i + 1 < argc && argv[i + 1]) {
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
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Create JSON object
    nlohmann::json jsonConfig;
    
    // Add all configuration entries to JSON
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to JSON
        nlohmann::json jsonValue;
        std::visit([&jsonValue](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonValue = arg;
            }
        }, entry.value);
        
        jsonConfig[entry.key] = jsonValue;
    }
    
    // Write JSON to file with pretty formatting
    file << jsonConfig.dump(2);
    
    NLM_LOG_INFO("Successfully saved configuration to JSON file: " + filepath);
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

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_DEBUG("Could not open JSON file: " + filepath);
        return false;
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        
        // Clear existing entries
        clear();
        
        // Parse all configuration entries from JSON
        if (json.is_object()) {
            for (auto& [key, value] : json.items()) {
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Handle arrays
                    if (!value.empty()) {
                        // Determine array element type
                        auto firstElement = value[0];
                        if (firstElement.is_string()) {
                            std::vector<std::string> strArray;
                            strArray.reserve(value.size());
                            for (const auto& item : value) {
                                strArray.push_back(item.get<std::string>());
                            }
                            configValue = strArray;
                        } else if (firstElement.is_number_integer()) {
                            std::vector<int> intArray;
                            intArray.reserve(value.size());
                            for (const auto& item : value) {
                                intArray.push_back(item.get<int>());
                            }
                            configValue = intArray;
                        } else if (firstElement.is_number_float()) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(value.size());
                            for (const auto& item : value) {
                                doubleArray.push_back(item.get<double>());
                            }
                            configValue = doubleArray;
                        }
                    }
                }
                
                if (!value.is_null()) {
                    set(key, configValue, ConfigSource::File);
                }
            }
            
            NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
            return true;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_DEBUG("Error loading JSON file: " + std::string(e.what()));
        return false;
    }
    
    return false;
}
