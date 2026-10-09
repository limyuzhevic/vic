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

// Implement proper JSON/YAML parser for Config
    bool Config::loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            // Parse JSON file
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Clear existing entries
            clear();
            
            // Recursively parse JSON to Config entries
            parseJsonToConfig(jsonData, "");
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    
    bool Config::saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json jsonData;
            
            // Convert Config entries to JSON
            for (const auto& entry : pImpl->entries) {
                // Navigate to nested object if key contains dots
                size_t dotPos = entry.key.find('.');
                if (dotPos != std::string::npos) {
                    std::string prefix = entry.key.substr(0, dotPos);
                    std::string suffix = entry.key.substr(dotPos + 1);
                    
                    // Create nested objects recursively
                    nlohmann::json* current = &jsonData;
                    size_t start = 0;
                    while (true) {
                        size_t nextDot = suffix.find('.', start);
                        std::string part = suffix.substr(start, nextDot - start);
                        
                        if (nextDot == std::string::npos) {
                            // Last part - set the value
                            (*current)[part] = getValueAsJson(entry.value);
                            break;
                        }
                        
                        if (!(*current).contains(part)) {
                            (*current)[part] = nlohmann::json::object();
                        }
                        
                        current = &((*current)[part]);
                        start = nextDot + 1;
                    }
                } else {
                    // Simple key at top level
                    jsonData[entry.key] = getValueAsJson(entry.value);
                }
            }
            
            // Write formatted JSON
            file << std::setw(2) << jsonData;
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to write JSON config file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    }
    
    nlohmann::json Config::getValueAsJson(const ConfigValue& value) const {
        nlohmann::json jsonValue;
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonValue = arg;
            }
        }, value);
        
        return jsonValue;
    }
    
    void Config::parseJsonToConfig(const nlohmann::json& jsonData, const std::string& prefix) {
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it->empty() ? "" : it->get<std::string>();
                std::string fullKey = prefix.empty() ? key : (prefix + "." + key);
                
                if (it->is_object()) {
                    parseJsonToConfig(*it, fullKey);
                } else if (it->is_array()) {
                    setArrayValue(fullKey, *it);
                } else {
                    setValueFromJson(fullKey, *it);
                }
            }
        }
    }
    
    void Config::setValueFromJson(const std::string& key, const nlohmann::json& jsonValue) {
        if (jsonValue.is_string()) {
            set(key, jsonValue.get<std::string>(), ConfigSource::File);
        } else if (jsonValue.is_number_integer()) {
            set(key, jsonValue.get<int>(), ConfigSource::File);
        } else if (jsonValue.is_number_unsigned()) {
            set(key, jsonValue.get<uint64_t>(), ConfigSource::File);
        } else if (jsonValue.is_number_float()) {
            set(key, jsonValue.get<double>(), ConfigSource::File);
        } else if (jsonValue.is_boolean()) {
            set(key, jsonValue.get<bool>(), ConfigSource::File);
        } else if (jsonValue.is_null()) {
            set(key, std::string("null"), ConfigSource::File);
        }
    }
    
    void Config::setArrayValue(const std::string& key, const nlohmann::json& jsonArray) {
        if (jsonArray.empty()) return;
        
        // Try to detect array type from first element
        auto firstIt = jsonArray.begin();
        if (firstIt->is_string()) {
            std::vector<std::string> vec;
            vec.reserve(jsonArray.size());
            for (const auto& item : jsonArray) {
                vec.push_back(item.get<std::string>());
            }
            set(key, vec, ConfigSource::File);
        } else if (firstIt->is_number_integer()) {
            std::vector<int> vec;
            vec.reserve(jsonArray.size());
            for (const auto& item : jsonArray) {
                vec.push_back(item.get<int>());
            }
            set(key, vec, ConfigSource::File);
        } else if (firstIt->is_number_float()) {
            std::vector<double> vec;
            vec.reserve(jsonArray.size());
            for (const auto& item : jsonArray) {
                vec.push_back(item.get<double>());
            }
            set(key, vec, ConfigSource::File);
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
