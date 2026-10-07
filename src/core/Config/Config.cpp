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

// Convert JSON value to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) return j.get<std::string>();
    if (j.is_number_integer()) return j.get<int>();
    if (j.is_number_unsigned()) return j.get<int64_t>();
    if (j.is_number()) {
        // Check if it's a float or double
        if (j.is_number_float()) return j.get<double>();
        return j.get<int64_t>();
    }
    if (j.is_boolean()) return j.get<bool>();
    if (j.is_array()) {
        // Try to infer array type
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                return j.get<std::vector<int>>();
            } else if (j[0].is_number_float()) {
                return j.get<std::vector<double>>();
            } else if (j[0].is_string()) {
                return j.get<std::vector<std::string>>();
            }
        }
        // Default to empty vector of ints
        return std::vector<int>();
    }
    // Fallback
    return std::string();
}

// Convert ConfigValue to JSON value
static nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return arg;
        if constexpr (std::is_same_v<T, int64_t>) return arg;
        if constexpr (std::is_same_v<T, double>) return arg;
        if constexpr (std::is_same_v<T, bool>) return arg;
        if constexpr (std::is_same_v<T, std::string>) return arg;
        if constexpr (std::is_same_v<T, std::vector<int>>) return arg;
        if constexpr (std::is_same_v<T, std::vector<double>>) return arg;
        if constexpr (std::is_same_v<T, std::vector<std::string>>) return arg;
        return nlohmann::json();
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
        
        // Parse JSON object
        if (jsonConfig.is_object()) {
            for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(it.value());
                
                // Try to find description in the JSON value (if it's an object)
                std::string description = "";
                if (it.value().is_object() && it.value().contains("_description")) {
                    description = it.value()["_description"].get<std::string>();
                    // Create a simplified JSON without the description field for the actual value
                    nlohmann::json simplifiedValue = it.value();
                    simplifiedValue.erase("_description");
                    value = jsonToConfigValue(simplifiedValue);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + std::string(e.what()));
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
    
    nlohmann::json jsonConfig = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        jsonConfig[entry.key] = configValueToJson(entry.value);
    }
    
    // Add pretty printing
    file << jsonConfig.dump(2);
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
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

// Template instantiations for JSON support
#include <type_traits>

// Instantiate common template types
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