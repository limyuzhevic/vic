#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Helper function to extract file extension
static std::string getFileExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    if (!extension.empty() && extension[0] == '.') {
        extension = extension.substr(1);
    }
    return extension;
}

// Convert YAML node to ConfigValue
static ConfigValue yamlNodeToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        const std::string& value = node.Scalar();
        // Try to determine type
        if (value == "true" || value == "false") {
            return value == "true";
        } else if (value.find('.') != std::string::npos || value.find('e') != std::string::npos) {
            // Try to parse as double first
            try {
                return std::stod(value);
            } catch (...) {
                return value;
            }
        } else {
            // Try to parse as integer
            try {
                return std::stoll(value);
            } catch (...) {
                return value;
            }
        }
    } else if (node.IsSequence()) {
        if (!node.IsNull() && node.size() > 0) {
            // Check type of first element
            if (node[0].IsScalar()) {
                const std::string& firstVal = node[0].Scalar();
                if (firstVal == "true" || firstVal == "false") {
                    std::vector<bool> vec;
                    for (const auto& item : node) {
                        vec.push_back(item.as<bool>());
                    }
                    return vec;
                } else if (firstVal.find('.') != std::string::npos || firstVal.find('e') != std::string::npos) {
                    std::vector<double> vec;
                    for (const auto& item : node) {
                        vec.push_back(item.as<double>());
                    }
                    return vec;
                } else {
                    try {
                        std::stoll(firstVal);
                        std::vector<int64_t> vec;
                        for (const auto& item : node) {
                            vec.push_back(item.as<int64_t>());
                        }
                        return vec;
                    } catch (...) {
                        std::vector<std::string> vec;
                        for (const auto& item : node) {
                            vec.push_back(item.Scalar());
                        }
                        return vec;
                    }
                }
            }
        }
        // Fallback to empty vector
        return std::vector<std::string>{};
    } else if (node.IsMap()) {
        // YAML maps are not directly supported in ConfigValue, treat as string
        return node.as<std::string>();
    } else {
        // Default to string representation
        return node.as<std::string>();
    }
}

// Convert ConfigValue to YAML node
static YAML::Node configValueToYamlNode(const ConfigValue& value) {
    YAML::Node node;
    std::visit([&node](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        }
    }, value);
    return node;
}

// Convert JSON to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number()) {
        return json.get<double>();
    } else if (json.is_array()) {
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int64_t> vec = json.get<std::vector<int64_t>>();
                return vec;
            } else if (json[0].is_number()) {
                std::vector<double> vec = json.get<std::vector<double>>();
                return vec;
            } else if (json[0].is_string()) {
                std::vector<std::string> vec = json.get<std::vector<std::string>>();
                return vec;
            } else if (json[0].is_boolean()) {
                std::vector<bool> vec = json.get<std::vector<bool>>();
                return vec;
            }
        }
        return std::vector<std::string>{};
    }
    return std::string("");
}

// Convert ConfigValue to JSON
static nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    std::visit([&jsonValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, value);
    return jsonValue;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string extension = getFileExtension(filepath);
        
        if (extension == "json") {
            nlohmann::json jsonData;
            file >> jsonData;
            
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                const std::string& key = it.key();
                const nlohmann::json& value = it.value();
                
                ConfigValue configValue = jsonToConfigValue(value);
                set(key, configValue, ConfigSource::File);
            }
            return true;
        } else if (extension == "yaml" || extension == "yml") {
            YAML::Node yamlData = YAML::Load(file);
            
            for (const auto& pair : yamlData) {
                const std::string& key = pair.first.as<std::string>();
                ConfigValue configValue = yamlNodeToConfigValue(pair.second);
                set(key, configValue, ConfigSource::File);
            }
            return true;
        } else {
            // Fall back to simple key=value format
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
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
    } catch (const std::exception& e) {
        return false;
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
    
    try {
        std::string extension = getFileExtension(filepath);
        
        if (extension == "json") {
            nlohmann::json jsonData;
            
            for (const auto& entry : pImpl->entries) {
                jsonData[entry.key] = configValueToJson(entry.value);
            }
            
            file << jsonData.dump(4);
            return true;
        } else if (extension == "yaml" || extension == "yml") {
            YAML::Emitter emitter;
            
            for (const auto& entry : pImpl->entries) {
                emitter << YAML::Key << entry.key;
                emitter << YAML::Value << configValueToYamlNode(entry.value);
            }
            
            file << emitter.c_str();
            return true;
        } else {
            // Fall back to simple key=value format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << arg << "\"\n";
                    } else {
                        file << entry.key << " = " << arg << "\n";
                    }
                }, entry.value);
            }
            return true;
        }
    } catch (const std::exception& e) {
        return false;
    }
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
