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

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for JSON/YAML conversion
std::optional<ConfigValue> ConfigConverter::jsonValueToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            const auto& firstElement = j[0];
            if (firstElement.is_string()) {
                std::vector<std::string> result;
                result.reserve(j.size());
                for (const auto& item : j) {
                    result.push_back(item.get<std::string>());
                }
                return result;
            } else if (firstElement.is_number_integer()) {
                std::vector<int> result;
                result.reserve(j.size());
                for (const auto& item : j) {
                    result.push_back(item.get<int>());
                }
                return result;
            } else if (firstElement.is_number_float()) {
                std::vector<double> result;
                result.reserve(j.size());
                for (const auto& item : j) {
                    result.push_back(item.get<double>());
                }
                return result;
            } else if (firstElement.is_boolean()) {
                std::vector<bool> result;
                result.reserve(j.size());
                for (const auto& item : j) {
                    result.push_back(item.get<bool>());
                }
                return result;
            }
        }
    }
    return std::nullopt;
}

nlohmann::json ConfigConverter::configValueToJsonValue(const ConfigValue& value) {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            j = nlohmann::json(arg);
        }
    }, value);
    return j;
}

std::optional<ConfigValue> ConfigConverter::yamlValueToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        const std::string& value = node.Scalar();
        
        try {
            bool boolVal = std::stoi(value);
            return boolVal;
        } catch (...) {}
        
        try {
            int intVal = std::stoi(value);
            return intVal;
        } catch (...) {}
        
        try {
            double doubleVal = std::stod(value);
            return doubleVal;
        } catch (...) {}
        
        return value;
    } else if (node.IsSequence()) {
        if (!node.empty()) {
            const auto& firstElement = node[0];
            if (firstElement.IsScalar()) {
                if (firstElement.IsBoolean()) {
                    std::vector<bool> result;
                    result.reserve(node.size());
                    for (const auto& item : node) {
                        result.push_back(item.as<bool>());
                    }
                    return result;
                } else if (firstElement.IsInteger()) {
                    std::vector<int> result;
                    result.reserve(node.size());
                    for (const auto& item : node) {
                        result.push_back(item.as<int>());
                    }
                    return result;
                } else if (firstElement.IsDouble()) {
                    std::vector<double> result;
                    result.reserve(node.size());
                    for (const auto& item : node) {
                        result.push_back(item.as<double>());
                    }
                    return result;
                } else {
                    std::vector<std::string> result;
                    result.reserve(node.size());
                    for (const auto& item : node) {
                        result.push_back(item.as<std::string>());
                    }
                    return result;
                }
            }
        }
    }
    return std::nullopt;
}

void ConfigConverter::configValueToYamlNode(const ConfigValue& value, YAML::Node& node) {
    std::visit([&node](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
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
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        }
    }, value);
}

std::string ConfigConverter::getFileExtension(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

bool ConfigConverter::isJSONFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    return ext == ".json";
}

bool ConfigConverter::isYAMLFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    return ext == ".yaml" || ext == ".yml";
}

bool ConfigConverter::isSimpleFormatFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    return ext == ".cfg" || ext == ".ini" || ext == ".properties";
}

FileFormat ConfigConverter::detectFileFormat(const std::string& filepath) {
    if (isJSONFile(filepath)) {
        return FileFormat::JSON;
    } else if (isYAMLFile(filepath)) {
        return FileFormat::YAML;
    } else if (isSimpleFormatFile(filepath)) {
        return FileFormat::SimpleKeyValue;
    }
    return FileFormat::Unknown;
}

bool Config::loadFromFile(const std::string& filepath) {
    FileFormat format = ConfigConverter::detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case FileFormat::JSON:
            try {
                nlohmann::json j;
                file >> j;
                clear();
                for (const auto& [key, value] : j.items()) {
                    auto configValue = ConfigConverter::jsonValueToConfigValue(value);
                    if (configValue.has_value()) {
                        set(key, configValue.value(), ConfigSource::File);
                    }
                }
                return true;
            } catch (const std::exception& e) {
                return false;
            }
            break;
            
        case FileFormat::YAML:
            try {
                YAML::Node doc = YAML::Load(file);
                clear();
                for (const auto& it : doc) {
                    auto configValue = ConfigConverter::yamlValueToConfigValue(it.second);
                    if (configValue.has_value()) {
                        set(it.first.as<std::string>(), configValue.value(), ConfigSource::File);
                    }
                }
                return true;
            } catch (const std::exception& e) {
                return false;
            }
            break;
            
        default:
            clear();
            return loadFromFileSimple(filepath);
            break;
    }
    return false;
}

bool Config::loadFromFileSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
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
    FileFormat format = ConfigConverter::detectFileFormat(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case FileFormat::JSON:
        {
            nlohmann::json j;
            for (const auto& entry : pImpl->entries) {
                if (!entry.description.empty()) {
                    j[entry.key]["__description__"] = entry.description;
                }
                j[entry.key] = ConfigConverter::configValueToJsonValue(entry.value);
            }
            file << j.dump(4);
            break;
        }
        
        case FileFormat::YAML:
        {
            YAML::Node doc;
            for (const auto& entry : pImpl->entries) {
                if (!entry.description.empty()) {
                    doc[entry.key]["__description__"] = entry.description;
                }
                ConfigConverter::configValueToYamlNode(entry.value, doc[entry.key]);
            }
            file << doc;
            break;
        }
        
        default:
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                // Convert config value to string
                std::string valueStr;
                std::visit([&valueStr](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        valueStr = arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        valueStr = arg ? "true" : "false";
                    } else if constexpr (std::is_same_v<T, int> || 
                                        std::is_same_v<T, int64_t> || 
                                        std::is_same_v<T, double>) {
                        std::ostringstream oss;
                        oss << arg;
                        valueStr = oss.str();
                    } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                        std::is_same_v<T, std::vector<double>> || 
                                        std::is_same_v<T, std::vector<std::string>> || 
                                        std::is_same_v<T, std::vector<bool>>) {
                        std::ostringstream oss;
                        oss << "[";
                        bool first = true;
                        for (const auto& item : arg) {
                            if (!first) oss << ", ";
                            if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                                oss << "\"" << item << "\"";
                            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                                oss << (item ? "true" : "false");
                            } else {
                                oss << item;
                            }
                            first = false;
                        }
                        oss << "]";
                        valueStr = oss.str();
                    }
                }, entry.value);
                
                file << entry.key << " = " << valueStr << "\n";
            }
            break;
    }
    
    return true;
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