#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validateOnSet = false;
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
        nlohmann::json jsonData;
        file >> jsonData;
        
        clear();
        
        for (auto& [key, value] : jsonData.items()) {
            ConfigSource source = ConfigSource::File;
            std::string description = "";
            
            if (key.find('.') != std::string::npos) {
                description = "Configuration parameter: " + key;
            }
            
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                std::vector<double> doubleArray;
                for (auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            }
            
            set(key, configValue, source);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from JSON file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
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
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::fixed << std::setprecision(6) << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
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
                        file << std::fixed << std::setprecision(6) << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config to file " << filepath << ": " << e.what() << std::endl;
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
    
    if (pImpl->validateOnSet) {
        validateEntry(key);
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

bool Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return validateEntry(key);
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry.key)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> entryErrors = validateEntryDetailed(entry.key);
        errors.insert(errors.end(), entryErrors.begin(), entryErrors.end());
    }
    return errors;
}

void Config::setValidationEnabled(bool enabled) {
    pImpl->validateOnSet = enabled;
}

bool Config::isValidationEnabled() const {
    return pImpl->validateOnSet;
}

std::string Config::toJSONString() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            jsonData[entry.key] = arg;
        }, entry.value);
    }
    
    return jsonData.dump(2);
}

bool Config::fromJSONString(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        clear();
        
        for (auto& [key, value] : jsonData.items()) {
            ConfigSource source = ConfigSource::Runtime;
            std::string description = "";
            
            set(key, value.get<ConfigValue>(), source);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON string: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> Config::getKeysByType(const std::string& typeName) const {
    std::vector<std::string> result;
    
    for (const auto& entry : pImpl->entries) {
        std::string keyType = getKeyType(entry.key);
        if (keyType == typeName) {
            result.push_back(entry.key);
        }
    }
    
    return result;
}

std::string Config::getKeyType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "unknown";
    }
    
    const ConfigValue& value = it->value;
    
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    
    return "unknown";
}

bool Config::validateEntry(const std::string& key) const {
    return std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; }) != pImpl->entries.end();
}

std::vector<std::string> Config::validateEntryDetailed(const std::string& key) const {
    std::vector<std::string> errors;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        errors.push_back("Key does not exist: " + key);
        return errors;
    }
    
    if (key == "random_seed") {
        auto value = it->value;
        if (std::holds_alternative<int64_t>(value)) {
            auto seed = std::get<int64_t>(value);
            if (seed < 0) {
                errors.push_back("random_seed must be non-negative");
            }
        }
    } else if (key == "simulation_timestep") {
        auto value = it->value;
        if (std::holds_alternative<double>(value)) {
            auto timestep = std::get<double>(value);
            if (timestep <= 0.0) {
                errors.push_back("simulation_timestep must be positive");
            }
        }
    } else if (key == "neuron_count") {
        auto value = it->value;
        if (std::holds_alternative<int64_t>(value)) {
            auto count = std::get<int64_t>(value);
            if (count <= 0) {
                errors.push_back("neuron_count must be positive");
            } else if (count > 1000000) {
                errors.push_back("neuron_count too large (max 1,000,000)");
            }
        }
    } else if (key == "region_count") {
        auto value = it->value;
        if (std::holds_alternative<int64_t>(value)) {
            auto count = std::get<int64_t>(value);
            if (count <= 0) {
                errors.push_back("region_count must be positive");
            } else if (count > 1000) {
                errors.push_back("region_count too large (max 1000)");
            }
        }
    }
    
    return errors;
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