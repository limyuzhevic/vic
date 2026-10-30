#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json schema;  // JSON schema for validation
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with basic schema
    pImpl->schema = nlohmann::json::object();
    pImpl->schema["type"] = "object";
    pImpl->schema["additionalProperties"] = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](const auto& v) -> nlohmann::json {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, double>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : v) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : v) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : v) {
                arr.push_back(item);
            }
            return arr;
        }
        return nullptr;
    }, value);
}

ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_number_integer()) {
        return jsonValue.get<int>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        if (!jsonValue.empty()) {
            const auto& first = jsonValue[0];
            if (first.is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (first.is_number()) {
                std::vector<double> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            } else if (first.is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            }
        }
        // Default to empty vector
        return std::vector<int>();
    }
    // Default to empty string
    return std::string();
}

std::string configValueToString(const ConfigValue& value) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            return "[...]";
        }
        return "";
    }, value);
}

bool configValueToBool(const ConfigValue& value) {
    return std::visit([](const auto& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v == "true" || v == "1" || v == "yes" || v == "on";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return v != 0;
        } else if constexpr (std::is_same_v<T, double>) {
            return v != 0.0;
        }
        return false;
    }, value);
}

int configValueToInt(const ConfigValue& value) {
    return std::visit([](const auto& v) -> int {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            return v;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<int>(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return static_cast<int>(v);
        } else if constexpr (std::is_same_v<T, std::string>) {
            try {
                return std::stoi(v);
            } catch (...) {
                return 0;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? 1 : 0;
        }
        return 0;
    }, value);
}

double configValueToDouble(const ConfigValue& value) {
    return std::visit([](const auto& v) -> double {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, double>) {
            return v;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return static_cast<double>(v);
        } else if constexpr (std::is_same_v<T, std::string>) {
            try {
                return std::stod(v);
            } catch (...) {
                return 0.0;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? 1.0 : 0.0;
        }
        return 0.0;
    }, value);
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Clear existing entries
        clear();
        
        // Parse JSON and populate entries
        if (jsonConfig.is_object()) {
            for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(it.value());
                
                // Try to get description from schema if available
                std::string description = "";
                if (pImpl->schema.is_object() && pImpl->schema.contains(key)) {
                    auto& keySchema = pImpl->schema[key];
                    if (keySchema.contains("description")) {
                        description = keySchema["description"].get<std::string>();
                    }
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error
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
        nlohmann::json jsonConfig = nlohmann::json::object();
        
        for (const auto& entry : pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
        
        // Write JSON with pretty formatting
        file << std::setw(2) << jsonConfig << std::endl;
        return true;
    } catch (const std::exception& e) {
        // Log error
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
        // Try to convert to requested type
        const ConfigValue& value = it->value;
        if constexpr (std::is_same_v<T, int>) {
            return configValueToInt(value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::visit([](const auto& v) -> std::optional<int64_t> {
                using U = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<U, int64_t>) {
                    return v;
                } else if constexpr (std::is_same_v<U, int>) {
                    return static_cast<int64_t>(v);
                } else if constexpr (std::is_same_v<U, double>) {
                    return static_cast<int64_t>(v);
                } else if constexpr (std::is_same_v<U, bool>) {
                    return v ? 1 : 0;
                } else if constexpr (std::is_same_v<U, std::string>) {
                    try {
                        return std::stoll(v);
                    } catch (...) {
                        return std::nullopt;
                    }
                }
                return std::nullopt;
            }, value);
        } else if constexpr (std::is_same_v<T, double>) {
            return configValueToDouble(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return configValueToBool(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::visit([](const auto& v) -> std::optional<std::string> {
                using U = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<U, std::string>) {
                    return v;
                }
                return std::nullopt;
            }, value);
        }
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
    return std::nullopt;
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
        oss << "  " << entry.key << " = " << configValueToString(entry.value) << " (" << static_cast<int>(entry.source) << ")\n";
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
