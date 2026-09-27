// Config.cpp - Updated with JSON/YAML parser support
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Phase 2: Implement proper JSON/YAML parser with backward compatibility
    
    // Detect format based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    
    // Try JSON format first if extension is .json
    if (ext == ".json") {
        success = loadJsonFile(filepath);
    }
    // Try YAML format if extension is .yaml or .yml
    else if (ext == ".yaml" || ext == ".yml") {
        success = loadYamlFile(filepath);
    }
    // For .cfg files and others, try key=value format first for backward compatibility
    else if (ext == ".cfg") {
        success = loadKeyValueFile(filepath);
    }
    // Auto-detect format based on content
    else {
        // Try JSON first
        success = loadJsonFile(filepath);
        if (!success) {
            // Try YAML if JSON fails
            success = loadYamlFile(filepath);
            if (!success) {
                // Fall back to key=value format
                success = loadKeyValueFile(filepath);
            }
        }
    }
    
    return success;
}

bool Config::loadKeyValueFile(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json_data = nlohmann::json::parse(file);
        convertJsonToConfig(json_data);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yaml_data = YAML::Load(file);
        convertYamlToConfig(yaml_data);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::convertJsonToConfig(const nlohmann::json& json_data) {
    for (auto it = json_data.begin(); it != json_data.end(); ++it) {
        std::string key = it.key();
        std::optional<ConfigValue> value = convertJsonValue(*it);
        
        if (value.has_value()) {
            set(key, value.value(), ConfigSource::File);
        }
    }
}

void Config::convertYamlToConfig(const YAML::Node& yaml_data) {
    for (auto it = yaml_data.begin(); it != yaml_data.end(); ++it) {
        std::string key = it->first.as<std::string>();
        std::optional<ConfigValue> value = convertYamlValue(*it);
        
        if (value.has_value()) {
            set(key, value.value(), ConfigSource::File);
        }
    }
}

std::optional<ConfigValue> Config::convertJsonValue(const nlohmann::json& json_value) {
    if (json_value.is_string()) {
        return json_value.get<std::string>();
    } else if (json_value.is_number_integer()) {
        return json_value.get<int>();
    } else if (json_value.is_number_unsigned()) {
        return json_value.get<uint64_t>();
    } else if (json_value.is_number()) {
        return json_value.get<double>();
    } else if (json_value.is_boolean()) {
        return json_value.get<bool>();
    } else if (json_value.is_array()) {
        if (json_value.empty()) {
            return std::vector<std::string>{};
        }
        
        // Try to detect array type
        bool all_strings = std::all_of(json_value.begin(), json_value.end(),
            [](const nlohmann::json& item) { return item.is_string(); });
        bool all_numbers = std::all_of(json_value.begin(), json_value.end(),
            [](const nlohmann::json& item) { return item.is_number(); });
        
        if (all_strings) {
            std::vector<std::string> result;
            for (const auto& item : json_value) {
                result.push_back(item.get<std::string>());
            }
            return result;
        } else if (all_numbers) {
            // Check if all are integers
            bool all_integers = std::all_of(json_value.begin(), json_value.end(),
                [](const nlohmann::json& item) { return item.is_number_integer(); });
            
            if (all_integers) {
                std::vector<int> result;
                for (const auto& item : json_value) {
                    result.push_back(item.get<int>());
                }
                return result;
            } else {
                std::vector<double> result;
                for (const auto& item : json_value) {
                    result.push_back(item.get<double>());
                }
                return result;
            }
        }
    }
    
    return std::nullopt;
}

std::optional<ConfigValue> Config::convertYamlValue(const YAML::Node& yaml_value) {
    if (yaml_value.IsScalar()) {
        if (yaml_value.IsBoolean()) {
            return yaml_value.as<bool>();
        } else if (yaml_value.IsInteger()) {
            return yaml_value.as<int>();
        } else if (yaml_value.IsDouble()) {
            return yaml_value.as<double>();
        } else {
            return yaml_value.as<std::string>();
        }
    } else if (yaml_value.IsSequence()) {
        if (yaml_value.empty()) {
            return std::vector<std::string>{};
        }
        
        // Try to detect sequence type
        bool all_strings = std::all_of(yaml_value.begin(), yaml_value.end(),
            [](const YAML::Node& item) { return item.IsScalar() && item.IsString(); });
        bool all_integers = std::all_of(yaml_value.begin(), yaml_value.end(),
            [](const YAML::Node& item) { return item.IsScalar() && item.IsInteger(); });
        bool all_doubles = std::all_of(yaml_value.begin(), yaml_value.end(),
            [](const YAML::Node& item) { return item.IsScalar() && item.IsDouble(); });
        
        if (all_strings) {
            std::vector<std::string> result;
            for (const auto& item : yaml_value) {
                result.push_back(item.as<std::string>());
            }
            return result;
        } else if (all_integers) {
            std::vector<int> result;
            for (const auto& item : yaml_value) {
                result.push_back(item.as<int>());
            }
            return result;
        } else if (all_doubles) {
            std::vector<double> result;
            for (const auto& item : yaml_value) {
                result.push_back(item.as<double>());
            }
            return result;
        }
    }
    
    return std::nullopt;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect format based on file extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveJsonFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveYamlFile(filepath);
    } else {
        // Default to key=value format for backward compatibility
        return saveKeyValueFile(filepath);
    }
}

bool Config::saveKeyValueFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by Config class\n\n";
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << "\", \"";
                    file << arg[i];
                }
                file << "\"]\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

bool Config::saveJsonFile(const std::string& filepath) const {
    nlohmann::json json_data;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&json_data, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json_data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json_data[entry.key] = arg;
            } else {
                json_data[entry.key] = arg;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << json_data.dump(2);
    return true;
}

bool Config::saveYamlFile(const std::string& filepath) const {
    YAML::Emitter yaml_emitter;
    yaml_emitter.SetIndent(2);
    yaml_emitter.SetMapFormat(YAML::E_MultiDoc);
    
    yaml_emitter << YAML::BeginDoc;
    yaml_emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        yaml_emitter << YAML::Key << entry.key;
        std::visit([&yaml_emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                yaml_emitter << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                yaml_emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    yaml_emitter << item;
                }
                yaml_emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                yaml_emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    yaml_emitter << item;
                }
                yaml_emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                yaml_emitter << YAML::BeginSeq;
                for (const auto& item : arg) {
                    yaml_emitter << item;
                }
                yaml_emitter << YAML::EndSeq;
            }
        }, entry.value);
    }
    
    yaml_emitter << YAML::EndMap;
    yaml_emitter << YAML::EndDoc;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << yaml_emitter.c_str();
    return true;
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
namespace {

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

} // anonymous namespace

} // namespace nlm

// Include nlohmann/json.hpp and yaml-cpp headers from system includes
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>