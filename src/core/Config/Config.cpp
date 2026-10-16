#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_set>

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
        nlohmann::json jsonData;
        file >> jsonData;
        return importFromJson(jsonData);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        return importFromJson(jsonData);
    } catch (const std::exception& e) {
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
    
    nlohmann::json jsonData = exportToJson();
    file << jsonData.dump(4); // Pretty print with 4-space indentation
    
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

nlohmann::json Config::exportToJson() const {
    nlohmann::json jsonArray = nlohmann::json::array();
    for (const auto& entry : pImpl->entries) {
        jsonArray.push_back(entry.toJson());
    }
    return jsonArray;
}

bool Config::importFromJson(const nlohmann::json& jsonData) {
    clear();
    
    if (!jsonData.is_array()) {
        return false;
    }
    
    for (const auto& item : jsonData) {
        ConfigEntry entry = ConfigEntry::fromJson(item);
        pImpl->entries.push_back(entry);
    }
    
    return true;
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    std::unordered_set<std::string> seenKeys;
    
    for (const auto& entry : pImpl->entries) {
        // Check for duplicate keys
        if (seenKeys.count(entry.key) > 0) {
            errors.push_back("Duplicate configuration key: " + entry.key);
        }
        seenKeys.insert(entry.key);
        
        // Validate key format
        if (!isValidKey(entry.key)) {
            errors.push_back("Invalid configuration key format: " + entry.key);
        }
        
        // Validate value
        if (!isValidValue(entry.value)) {
            errors.push_back("Invalid configuration value for key: " + entry.key);
        }
    }
    
    return errors;
}

bool Config::apply(const std::string& filepath) {
    // Try JSON first, then fallback to simple key=value
    if (loadFromFile(filepath)) {
        // Validate configuration
        auto errors = validate();
        if (!errors.empty()) {
            for (const auto& error : errors) {
                NLM_LOG_WARNING("Configuration validation warning: " + error);
            }
        }
        return true;
    }
    
    // Fallback to old format
    return loadFromFile(filepath);
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_number_integer()) {
        int64_t val = j.get<int64_t>();
        if (val <= std::numeric_limits<int>::max() && val >= std::numeric_limits<int>::min()) {
            return static_cast<int>(val);
        }
        return val;
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                return j.get<std::vector<int>>();
            } else if (j[0].is_number_float()) {
                return j.get<std::vector<double>>();
            } else if (j[0].is_string()) {
                return j.get<std::vector<std::string>>();
            }
        }
    }
    return std::string(); // Default empty string
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        return nlohmann::json(arg);
    }, value);
}

bool Config::isValidKey(const std::string& key) {
    // Key should be alphanumeric with underscores and dashes
    return !key.empty() && std::all_of(key.begin(), key.end(),
        [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

bool Config::isValidValue(const ConfigValue& value) {
    return !std::holds_alternative<std::monostate>(value);
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
