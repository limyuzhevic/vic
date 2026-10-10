#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try to parse as JSON first
        std::ifstream json_file(filepath);
        if (json_file.is_open()) {
            nlohmann::json json_data = nlohmann::json::parse(json_file);
            pImpl->entries.clear();
            
            for (const auto& [key, value] : json_data.items()) {
                ConfigEntry entry;
                entry.key = key;
                entry.source = ConfigSource::File;
                entry.description = "";
                
                // Convert JSON value to ConfigValue
                if (value.is_string()) {
                    entry.value = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    entry.value = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    entry.value = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    entry.value = value.get<double>();
                } else if (value.is_boolean()) {
                    entry.value = value.get<bool>();
                } else if (value.is_array()) {
                    // Convert array to vector
                    std::vector<double> double_array;
                    for (const auto& item : value) {
                        double_array.push_back(item.get<double>());
                    }
                    entry.value = double_array;
                }
                
                pImpl->entries.push_back(entry);
            }
            
            return true;
        }
        
        // If JSON parsing fails, try YAML or fallback to simple key=value format
        // For now, fallback to simple format
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
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from args: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Save in JSON format
        nlohmann::json json_data;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&json_data, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    json_data[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    json_data[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    json_data[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    json_data[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    json_data[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    json_data[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    json_data[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    json_data[entry.key] = arg;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << json_data.dump(4);  // Pretty print with indentation
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config to file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "Error getting config value for key " << key << ": " << e.what() << std::endl;
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
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
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