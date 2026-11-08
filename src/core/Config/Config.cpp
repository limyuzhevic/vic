#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<json> configSchema; // Optional schema for validation
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced JSON/YAML parser support
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    try {
        // Try to parse as JSON first
        json configData;
        file >> configData;
        
        // Clear existing entries
        clear();
        
        // Process JSON entries
        if (configData.is_object()) {
            for (auto it = configData.begin(); it != configData.end(); ++it) {
                std::string key = it.key();
                
                // Handle different value types
                if (it->is_string()) {
                    set(key, it->get<std::string>(), ConfigSource::File);
                } else if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), ConfigSource::File);
                } else if (it->is_number_unsigned()) {
                    set(key, it->get<uint64_t>(), ConfigSource::File);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), ConfigSource::File);
                } else if (it->is_boolean()) {
                    set(key, it->get<bool>(), ConfigSource::File);
                } else if (it->is_array()) {
                    // Handle arrays (convert to string representation)
                    std::vector<std::string> arrayStr;
                    for (const auto& item : *it) {
                        if (item.is_string()) {
                            arrayStr.push_back(item.get<std::string>());
                        } else if (item.is_number()) {
                            arrayStr.push_back(std::to_string(item.get<double>()));
                        } else {
                            arrayStr.push_back("[complex]");
                        }
                    }
                    set(key, arrayStr, ConfigSource::File);
                }
            }
            
            NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
            return true;
        }
        
        // If not valid JSON, try YAML (using nlohmann/json which also supports YAML)
        return loadFromYamlFile(filepath);
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parsing error in config file " + filepath + ": " + std::string(e.what()));
        return loadFromLegacyFormat(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return loadFromLegacyFormat(filepath);
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    // YAML support using nlohmann/json which also parses YAML
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json configData;
        file >> configData;
        
        clear();
        
        if (configData.is_object()) {
            for (auto it = configData.begin(); it != configData.end(); ++it) {
                std::string key = it.key();
                
                if (it->is_string()) {
                    set(key, it->get<std::string>(), ConfigSource::File);
                } else if (it->is_number_integer()) {
                    set(key, it->get<int64_t>(), ConfigSource::File);
                } else if (it->is_number_unsigned()) {
                    set(key, it->get<uint64_t>(), ConfigSource::File);
                } else if (it->is_number_float()) {
                    set(key, it->get<double>(), ConfigSource::File);
                } else if (it->is_boolean()) {
                    set(key, it->get<bool>(), ConfigSource::File);
                } else if (it->is_array()) {
                    std::vector<std::string> arrayStr;
                    for (const auto& item : *it) {
                        if (item.is_string()) {
                            arrayStr.push_back(item.get<std::string>());
                        } else if (item.is_number()) {
                            arrayStr.push_back(std::to_string(item.get<double>()));
                        } else {
                            arrayStr.push_back("[complex]");
                        }
                    }
                    set(key, arrayStr, ConfigSource::File);
                }
            }
            
            NLM_LOG_INFO("Successfully loaded configuration from YAML file: " + filepath);
            return true;
        }
        
    } catch (...) {
        // YAML parsing failed
    }
    
    return false;
}

bool Config::loadFromLegacyFormat(const std::string& filepath) {
    // Original simple key=value format for backward compatibility
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
    
    NLM_LOG_INFO("Successfully loaded configuration from legacy format: " + filepath);
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
        // Handle boolean flags
        else if (arg.substr(0, 2) == "-" && arg.size() > 2 && arg.find('=') == std::string::npos) {
            // Convert --flag to key=flag with true value
            std::string key = arg.substr(2);
            if (key == "help" || key == "h") {
                set(key, "true", ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    // Save in JSON format
    json configData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                configData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, uint64_t>) {
                configData[entry.key] = static_cast<int64_t>(arg);
            } else if constexpr (std::is_same_v<T, double> ||
                                 std::is_same_v<T, float>) {
                configData[entry.key] = static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                configData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                // Convert array to JSON array
                json arrayJson = json::array();
                for (const auto& item : arg) {
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        arrayJson.push_back(item);
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        arrayJson.push_back(static_cast<int64_t>(item));
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        arrayJson.push_back(static_cast<double>(item));
                    }
                }
                configData[entry.key] = arrayJson;
            } else {
                configData[entry.key] = "[complex]";
            }
        }, entry.value);
    }
    
    // Write JSON to file with pretty formatting
    file << configData.dump(2);
    
    NLM_LOG_INFO("Configuration saved to file: " + filepath);
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
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
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