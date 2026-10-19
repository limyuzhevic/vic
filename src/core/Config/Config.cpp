#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json.hpp>
#include <yaml-cpp/yaml.h>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Try JSON first
    if (ext == "json" || ext == "JSON" || ext.empty()) {
        try {
            return loadFromJSON(filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("JSON parsing failed: ") + e.what());
        }
    }
    
    // Try YAML
    if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        try {
            return loadFromYAML(filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("YAML parsing failed: ") + e.what());
        }
    }
    
    // Fallback to simple key=value format for backward compatibility
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }
    
    parseJSON(j, ConfigSource::File);
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node y;
        y = YAML::LoadFile(filepath);
        parseYAML(y, ConfigSource::File);
    } catch (const YAML::ParserException& e) {
        throw std::runtime_error(std::string("YAML parse error: ") + e.what());
    }
    
    return true;
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
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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
            
            // Try to infer type from value
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::parseJSON(const json& j, ConfigSource source) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        const json& value = it.value();
        
        if (value.is_number_integer()) {
            set(key, value.get<int64_t>(), source);
        } else if (value.is_number_unsigned()) {
            set(key, value.get<uint64_t>(), source);
        } else if (value.is_number_float()) {
            set(key, value.get<double>(), source);
        } else if (value.is_boolean()) {
            set(key, value.get<bool>(), source);
        } else if (value.is_string()) {
            set(key, value.get<std::string>(), source);
        } else if (value.is_array()) {
            // Handle array types
            if (!value.empty()) {
                if (std::all_of(value.begin(), value.end(), [](const json& v) { return v.is_number_integer(); })) {
                    std::vector<int64_t> arr;
                    for (const auto& v : value) {
                        arr.push_back(v.get<int64_t>());
                    }
                    set(key, arr, source);
                } else if (std::all_of(value.begin(), value.end(), [](const json& v) { return v.is_number_float(); })) {
                    std::vector<double> arr;
                    for (const auto& v : value) {
                        arr.push_back(v.get<double>());
                    }
                    set(key, arr, source);
                } else if (std::all_of(value.begin(), value.end(), [](const json& v) { return v.is_boolean(); })) {
                    std::vector<bool> arr;
                    for (const auto& v : value) {
                        arr.push_back(v.get<bool>());
                    }
                    set(key, arr, source);
                } else if (std::all_of(value.begin(), value.end(), [](const json& v) { return v.is_string(); })) {
                    std::vector<std::string> arr;
                    for (const auto& v : value) {
                        arr.push_back(v.get<std::string>());
                    }
                    set(key, arr, source);
                } else if (value[0].is_array()) {
                    // Matrix case
                    std::vector<std::vector<double>> matrix;
                    for (const auto& row : value) {
                        std::vector<double> row_data;
                        for (const auto& elem : row) {
                            row_data.push_back(elem.get<double>());
                        }
                        matrix.push_back(row_data);
                    }
                    set(key, matrix, source);
                }
            }
        } else if (value.is_object()) {
            // Check if it's a parameter map
            bool has_string_keys = true;
            for (const auto& elem : value.items()) {
                if (!elem.key().is_string()) {
                    has_string_keys = false;
                    break;
                }
            }
            
            if (has_string_keys) {
                // Try to determine value type
                auto first_value = value.begin();
                if (first_value.value().is_number()) {
                    std::map<std::string, double> param_map;
                    for (const auto& elem : value.items()) {
                        param_map[elem.key()] = elem.value().get<double>();
                    }
                    set(key, param_map, source);
                } else if (first_value.value().is_string()) {
                    std::map<std::string, std::string> param_map;
                    for (const auto& elem : value.items()) {
                        param_map[elem.key()] = elem.value().get<std::string>();
                    }
                    set(key, param_map, source);
                }
            }
        }
    }
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
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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
            
            // Try to infer type from value
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::parseYAML(const YAML::Node& y, ConfigSource source) {
    for (const auto& it : y) {
        std::string key = it.first.as<std::string>();
        const YAML::Node& value = it.second;
        
        if (value.IsScalar()) {
            std::string str_value = value.as<std::string>();
            // Try to convert to appropriate type
            if (str_value == "true" || str_value == "false") {
                set(key, str_value == "true", source);
            } else if (str_value.find('.') != std::string::npos) {
                try {
                    set(key, std::stod(str_value), source);
                } catch (...) {
                    set(key, str_value, source);
                }
            } else {
                try {
                    set(key, std::stoll(str_value), source);
                } catch (...) {
                    set(key, str_value, source);
                }
            }
        } else if (value.IsSequence()) {
            // Handle arrays
            std::vector<double> arr;
            for (const auto& elem : value) {
                arr.push_back(elem.as<double>());
            }
            set(key, arr, source);
        } else if (value.IsMap()) {
            // Handle nested maps
            std::map<std::string, double> param_map;
            for (const auto& elem : value) {
                param_map[elem.first.as<std::string>()] = elem.second.as<double>();
            }
            set(key, param_map, source);
        }
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
