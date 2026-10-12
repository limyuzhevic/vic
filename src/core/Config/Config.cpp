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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Parse JSON file
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries
        clear();
        
        // Process JSON entries
        for (const auto& [key, value] : j.items()) {
            ConfigSource source = ConfigSource::File;
            std::string description = "";
            
            // Handle different value types
            if (value.is_string()) {
                set(key, value.get<std::string>(), source, description);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), source, description);
            } else if (value.is_number_integer()) {
                // Check if it fits in int64_t
                set(key, value.get<int64_t>(), source, description);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), source, description);
            } else if (value.is_array()) {
                // Handle array types
                if (!value.empty() && value.front().is_number_integer()) {
                    std::vector<int64_t> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    set(key, intArray, source, description);
                } else if (!value.empty() && value.front().is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    set(key, doubleArray, source, description);
                } else if (!value.empty() && value.front().is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : value) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    set(key, stringArray, source, description);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + e.what());
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
        nlohmann::json j;
        
        // Add configuration entries to JSON
        for (const auto& entry : pImpl->entries) {
            std::string description = entry.description.empty() ? 
                "Configuration parameter" : entry.description;
            
            // Convert value based on type
            std::visit([&j, &entry, &description](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, int> || 
                                   std::is_same_v<T, int64_t>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    j[entry.key] = val;
                }
            }, entry.value);
            
            // Add metadata comment (JSON doesn't support inline comments)
            // Instead, we could create a separate metadata section or use a different format
        }
        
        // Write JSON with pretty formatting
        file << j.dump(2);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write JSON config file " + filepath + ": " + e.what());
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
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
