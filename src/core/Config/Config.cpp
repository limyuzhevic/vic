#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <yaml-cpp/yaml.h>
#include <stdexcept>

namespace nlm {

// Helper function to convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Try to determine if it's int or double array
        if (!j.empty() && j[0].is_number_integer()) {
            std::vector<int64_t> intArray;
            for (const auto& item : j) {
                intArray.push_back(item.get<int64_t>());
            }
            return intArray;
        } else {
            std::vector<double> doubleArray;
            for (const auto& item : j) {
                doubleArray.push_back(item.get<double>());
            }
            return doubleArray;
        }
    } else if (j.is_null()) {
        return std::string("");
    } else {
        // Fallback to string representation
        return j.dump();
    }
}

// Helper function to convert ConfigValue to JSON value
nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](const auto& val) -> nlohmann::json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return static_cast<int64_t>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else {
            return std::string("");
        }
    }, value);
}

#include <chrono>
#include <ctime>

namespace nlm {

// Helper function to convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Try to determine if it's int or double array
        if (!j.empty() && j[0].is_number_integer()) {
            std::vector<int64_t> intArray;
            for (const auto& item : j) {
                intArray.push_back(item.get<int64_t>());
            }
            return intArray;
        } else {
            std::vector<double> doubleArray;
            for (const auto& item : j) {
                doubleArray.push_back(item.get<double>());
            }
            return doubleArray;
        }
    } else if (j.is_null()) {
        return std::string("");
    } else {
        // Fallback to string representation
        return j.dump();
    }
}

// Helper function to convert ConfigValue to JSON value
nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](const auto& val) -> nlohmann::json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return static_cast<int64_t>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else {
            return std::string("");
        }
    }, value);
}

// Helper function to determine file extension and load accordingly
bool loadConfigFromFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension;
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    try {
        if (extension == "json" || extension == "JSON") {
            nlohmann::json j;
            file >> j;
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(it.value());
                config.set(key, value, ConfigSource::File);
            }
        } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            YAML::Node yaml;
            file >> yaml;
            
            for (const auto& it : yaml) {
                std::string key = it.first.as<std::string>();
                ConfigValue value = jsonToConfigValue(nlohmann::json::parse(it.second.as<std::string>()));
                config.set(key, value, ConfigSource::File);
            }
        } else {
            // Fallback to simple key=value format for backward compatibility
            std::string line;
            while (std::getline(file, line)) {
                line = Config::trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = Config::trim(line.substr(0, pos));
                    std::string value = Config::trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    config.set(key, value, ConfigSource::File);
                }
            }
        }
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

#include <chrono>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Helper function to convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Try to determine if it's int or double array
        if (!j.empty() && j[0].is_number_integer()) {
            std::vector<int64_t> intArray;
            for (const auto& item : j) {
                intArray.push_back(item.get<int64_t>());
            }
            return intArray;
        } else {
            std::vector<double> doubleArray;
            for (const auto& item : j) {
                doubleArray.push_back(item.get<double>());
            }
            return doubleArray;
        }
    } else if (j.is_null()) {
        return std::string("");
    } else {
        // Fallback to string representation
        return j.dump();
    }
}

// Helper function to convert ConfigValue to JSON value
nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](const auto& val) -> nlohmann::json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return static_cast<int64_t>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto& item : val) {
                j.push_back(item);
            }
            return j;
        } else {
            return std::string("");
        }
    }, value);
}

// Helper function to determine file extension and load accordingly
bool loadConfigFromFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension;
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    try {
        if (extension == "json" || extension == "JSON") {
            nlohmann::json j;
            file >> j;
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(it.value());
                config.set(key, value, ConfigSource::File);
            }
        } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            YAML::Node yaml;
            file >> yaml;
            
            for (const auto& it : yaml) {
                std::string key = it.first.as<std::string>();
                ConfigValue value = jsonToConfigValue(nlohmann::json::parse(it.second.as<std::string>()));
                config.set(key, value, ConfigSource::File);
            }
        } else {
            // Fallback to simple key=value format for backward compatibility
            std::string line;
            while (std::getline(file, line)) {
                line = Config::trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = Config::trim(line.substr(0, pos));
                    std::string value = Config::trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    config.set(key, value, ConfigSource::File);
                }
            }
        }
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastError;
    bool autoSaveEnabled = false;
    std::string autoSavePath;
    int autoSaveIntervalSeconds = 60;
    std::chrono::system_clock::time_point lastModified;
    std::chrono::system_clock::time_point lastAutoSave;
    
    // Configuration templates
    struct Template {
        std::string name;
        std::string description;
        nlohmann::json schema;
        nlohmann::json defaultValues;
    };
    
    std::vector<Template> templates;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to determine file extension and load accordingly
bool loadConfigFromFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension;
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    try {
        if (extension == "json" || extension == "JSON") {
            nlohmann::json j;
            file >> j;
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(it.value());
                config.set(key, value, ConfigSource::File);
            }
        } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            YAML::Node yaml;
            file >> yaml;
            
            for (const auto& it : yaml) {
                std::string key = it.first.as<std::string>();
                ConfigValue value = jsonToConfigValue(nlohmann::json::parse(it.second.as<std::string>()));
                config.set(key, value, ConfigSource::File);
            }
        } else {
            // Fallback to simple key=value format for backward compatibility
            std::string line;
            while (std::getline(file, line)) {
                line = Config::trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = Config::trim(line.substr(0, pos));
                    std::string value = Config::trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    config.set(key, value, ConfigSource::File);
                }
            }
        }
    } catch (const std::exception& e) {
        return false;
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

// Save configuration to file with automatic format detection based on file extension
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension;
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
    }
    
    try {
        if (extension == "json" || extension == "JSON") {
            file << std::setw(2) << j << std::endl;
        } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
            // Convert to YAML by dumping JSON to string and then parsing
            std::string jsonStr = j.dump();
            nlohmann::json temp = nlohmann::json::parse(jsonStr);
            file << temp.dump(2);
        } else {
            // Default to simple key=value format for backward compatibility
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                
                // Convert value to string representation
                std::visit([&file](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << "\"" << val << "\"";
                    } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                        file << val;
                    } else if constexpr (std::is_same_v<T, double>) {
                        file << val;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << (val ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, std::vector<int64_t>> ||
                                      std::is_same_v<T, std::vector<double>> ||
                                      std::is_same_v<T, std::vector<std::string>>) {
                        file << "["; // Simplified - would need to output actual array values
                        file << "]";
                    } else {
                        file << "\"\"";
                    }
                }, entry.value);
                
                file << "\n";
            }
        }
    } catch (const std::exception& e) {
        return false;
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