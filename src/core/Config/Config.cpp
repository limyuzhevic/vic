#include "Config.hpp"
#include "ErrorHandling/ErrorHandling.hpp"
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
    try {
        // Try to parse as JSON first
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_CONFIG_ERROR(ConfigFileNotFound, "loadFromFile:" + filepath);
            return false;
        }
        
        // Parse JSON
        nlohmann::json jsonData;
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_THROW_CONFIG_ERROR(ConfigInvalidFormat, "loadFromFile:" + filepath + " - " + e.what());
            return false;
        }
        
        // Clear existing entries
        clear();
        
        // Process JSON entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            try {
                // Convert JSON value to ConfigValue
                if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_number_integer()) {
                    value = it->get<int64_t>();
                } else if (it->is_number_unsigned()) {
                    value = static_cast<int64_t>(it->get<uint64_t>());
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_array()) {
                    // For arrays, we need to handle the element type
                    if (!it->empty()) {
                        auto firstElem = it->front();
                        if (firstElem.is_string()) {
                            value = it->get<std::vector<std::string>>();
                        } else if (firstElem.is_number_integer()) {
                            value = it->get<std::vector<int64_t>>();
                        } else if (firstElem.is_number_float()) {
                            value = it->get<std::vector<double>>();
                        } else if (firstElem.is_number_unsigned()) {
                            value = it->get<std::vector<uint64_t>>();
                        } else {
                            // For other types, just store as empty vector
                            value = std::vector<double>();
                        }
                    } else {
                        value = std::vector<double>();
                    }
                } else {
                    NLM_THROW_CONFIG_ERROR(ConfigInvalidFormat, "loadFromFile:" + filepath + " - unsupported JSON type for key: " + key);
                    return false;
                }
                
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_THROW_CONFIG_ERROR(ConfigInvalidValue, "loadFromFile:" + filepath + " - error processing key " + key + ": " + e.what());
                return false;
            }
        }
        
        return true;
    } catch (const NLMException& e) {
        // Re-throw NLM exceptions
        throw;
    } catch (const std::exception& e) {
        // Convert other exceptions to NLM exceptions
        NLM_THROW_CONFIG_ERROR(ConfigError, "loadFromFile:" + filepath + " - unexpected error: " + e.what());
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
                } else {
                    NLM_THROW_CONFIG_ERROR(ConfigInvalidFormat, "loadFromArgs: invalid argument format: " + arg);
                    return false;
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
    } catch (const NLMException& e) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG_ERROR(ConfigError, "loadFromArgs: unexpected error: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Create parent directories if they don't exist
        std::filesystem::path path(filepath);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_CONFIG_ERROR(ConfigFileNotFound, "saveToFile:" + filepath);
            return false;
        }
        
        // Write JSON configuration
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            try {
                std::string key = entry.key;
                nlohmann::json jsonValue;
                
                // Convert ConfigValue to JSON
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int>) {
                        jsonValue = arg;
                    } else if constexpr (std::is_same_v<T, int64_t>) {
                        jsonValue = arg;
                    } else if constexpr (std::is_same_v<T, double>) {
                        jsonValue = arg;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        jsonValue = arg;
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        jsonValue = arg;
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        jsonValue = arg;
                    } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                        // Convert int64_t vector to JSON array
                        nlohmann::json jsonArray = nlohmann::json::array();
                        for (auto val : arg) {
                            jsonArray.push_back(val);
                        }
                        jsonValue = jsonArray;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        jsonValue = arg;
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        jsonValue = arg;
                    }
                }, entry.value);
                
                jsonData[key] = jsonValue;
            } catch (const std::exception& e) {
                NLM_THROW_CONFIG_ERROR(ConfigError, "saveToFile:" + filepath + " - error processing key " + entry.key + ": " + e.what());
                return false;
            }
        }
        
        // Write JSON to file with pretty formatting
        file << jsonData.dump(2);
        file << std::endl;
        
        return true;
    } catch (const NLMException& e) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG_ERROR(ConfigError, "saveToFile:" + filepath + " - unexpected error: " + std::string(e.what()));
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
        NLM_THROW_CONFIG_ERROR(ConfigInvalidValue, "get:" + key + " - value type mismatch");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate input
    nlm_validate_param(!key.empty(), "key", "non-empty string");
    
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
    nlm_validate_param(!key.empty(), "key", "non-empty string");
    
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