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

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Try YAML next
    if (loadFromYAML(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json;
        file >> json;
        
        for (const auto& [key, value] : json.items()) {
            // Handle different JSON types
            if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_array()) {
                // Handle arrays - determine element type
                if (!value.empty()) {
                    if (value[0].is_string()) {
                        std::vector<std::string> strArray;
                        for (const auto& item : value) {
                            strArray.push_back(item.get<std::string>());
                        }
                        set(key, ConfigValue(strArray), ConfigSource::File);
                    } else if (value[0].is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : value) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        set(key, ConfigValue(intArray), ConfigSource::File);
                    } else if (value[0].is_number_float()) {
                        std::vector<double> floatArray;
                        for (const auto& item : value) {
                            floatArray.push_back(item.get<double>());
                        }
                        set(key, ConfigValue(floatArray), ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // JSON parsing failed
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (const auto& node : doc) {
            std::string key = node.first.as<std::string>();
            
            // Determine YAML type and handle appropriately
            if (node.second.IsScalar()) {
                // Get scalar type
                YAML::NodeType type = node.second.Type();
                if (type == YAML::NodeType::SCALAR) {
                    std::string value = node.second.as<std::string>();
                    // Try to convert to numeric
                    try {
                        if (value.find('.') != std::string::npos) {
                            double num = std::stod(value);
                            set(key, num, ConfigSource::File);
                        } else {
                            int64_t num = std::stoll(value);
                            set(key, num, ConfigSource::File);
                        }
                    } catch (...) {
                        // Not a number, store as string
                        set(key, value, ConfigSource::File);
                    }
                    // Handle boolean
                    if (value == "true" || value == "false") {
                        set(key, value == "true", ConfigSource::File);
                    }
                }
            } else if (node.second.IsSequence()) {
                // Handle arrays - determine element type
                YAML::NodeType childType = YAML::NodeType::NONE;
                if (!node.second.empty()) {
                    childType = node.second[0].Type();
                }
                
                if (childType == YAML::NodeType::SCALAR) {
                    std::vector<std::string> array;
                    for (const auto& item : node.second) {
                        array.push_back(item.as<std::string>());
                    }
                    set(key, ConfigValue(array), ConfigSource::File);
                } else if (childType == YAML::NodeType::INT) {
                    std::vector<int64_t> array;
                    for (const auto& item : node.second) {
                        array.push_back(item.as<int64_t>());
                    }
                    set(key, ConfigValue(array), ConfigSource::File);
                } else if (childType == YAML::NodeType::DOUBLE) {
                    std::vector<double> array;
                    for (const auto& item : node.second) {
                        array.push_back(item.as<double>());
                    }
                    set(key, ConfigValue(array), ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // YAML parsing failed
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Save the actual value based on its type
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else {
                // Handle numeric types
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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
