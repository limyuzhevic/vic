#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>

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
    // First try to open file to check if it exists and is readable
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Config::loadFromFile: File does not exist: " << filepath << std::endl;
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        std::cerr << "Config::loadFromFile: Path is not a regular file: " << filepath << std::endl;
        return false;
    }
    
    // Try JSON parsing first (modern config format)
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Config::loadFromFile: Cannot open file for reading: " << filepath << std::endl;
            return false;
        }
        
        json j;
        file >> j;
        file.close();
        
        // Clear existing entries before loading new ones
        clear();
        
        // Parse JSON into Config entries
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                std::string description = "Configuration parameter: " + key;
                
                // Handle different value types
                ConfigValue configValue;
                const auto& val = it.value();
                
                if (val.is_number_integer()) {
                    configValue = val.get<int64_t>();
                } else if (val.is_number_float()) {
                    configValue = val.get<double>();
                } else if (val.is_boolean()) {
                    configValue = val.get<bool>();
                } else if (val.is_string()) {
                    configValue = val.get<std::string>();
                } else if (val.is_array()) {
                    // Convert JSON array to std::vector based on element type
                    if (!val.empty()) {
                        if (val[0].is_number_integer()) {
                            std::vector<int> intArray;
                            for (const auto& item : val) {
                                intArray.push_back(item.get<int>());
                            }
                            configValue = intArray;
                        } else if (val[0].is_number_float()) {
                            std::vector<double> doubleArray;
                            for (const auto& item : val) {
                                doubleArray.push_back(item.get<double>());
                            }
                            configValue = doubleArray;
                        } else if (val[0].is_string()) {
                            std::vector<std::string> stringArray;
                            for (const auto& item : val) {
                                stringArray.push_back(item.get<std::string>());
                            }
                            configValue = stringArray;
                        }
                    }
                }
                
                set(key, configValue, ConfigSource::File, description);
            }
        }
        
        std::cout << "Config::loadFromFile: Successfully loaded JSON configuration from: " << filepath << std::endl;
        return true;
        
    } catch (const json::parse_error& e) {
        // JSON parsing failed, fall back to simple format
        std::cout << "Config::loadFromFile: JSON parse error (" << e.what() << "), falling back to simple format: " << filepath << std::endl;
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
    }
    
    // Fallback to simple key=value format with better error handling
    clear();
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Config::loadFromFile: Cannot open file for reading: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with better validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Config::loadFromFile: Invalid format at line " << lineNumber 
                     << " (missing '='): " << line << std::endl;
            continue; // Skip malformed lines but continue parsing
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            std::cerr << "Config::loadFromFile: Empty key at line " << lineNumber << std::endl;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File, "Loaded from simple format");
    }
    
    std::cout << "Config::loadFromFile: Successfully loaded simple configuration from: " << filepath << std::endl;
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
