#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 4: Implement proper JSON/YAML parser
    // Current implementation uses a simple key=value format
    // For now, just try the existing format and log a warning if parsing fails
    
    // Try JSON parser first
    try {
        auto jsonConfig = std::make_shared<JsonConfig>(filepath);
        if (jsonConfig->isValid()) {
            // Convert JSON to Config entries
            auto entries = jsonConfig->getEntries();
            for (const auto& entry : entries) {
                set(entry.key, entry.value, ConfigSource::File);
            }
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("JSON parser failed: ") + e.what() + ", falling back to simple parser");
    }
    
    // Try YAML parser as second option
    try {
        auto yamlConfig = std::make_shared<YamlConfig>(filepath);
        if (yamlConfig->isValid()) {
            auto entries = yamlConfig->getEntries();
            for (const auto& entry : entries) {
                set(entry.key, entry.value, ConfigSource::File);
            }
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("YAML parser failed: ") + e.what() + ", falling back to simple parser");
    }
    
    // Fall back to simple key=value parser (original implementation)
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
        
        // Store actual value instead of PLACEHOLDER_VALUE
        std::visit([&file](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << val[i] << "\"";
                    } else {
                        file << val[i];
                    }
                }
                file << "]";
            } else {
                // Handle numeric and boolean values
                if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else {
                    file << val;
                }
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// JSON configuration parser (Phase 4)
namespace nlm {

JsonConfig::JsonConfig(const std::string& filepath) : valid(false) {
    // Try to parse as JSON (nlohmann::json format)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    try {
        data = nlohmann::json::parse(file);
        valid = true;
    } catch (const std::exception&) {
        // Not valid JSON
    }
}

bool JsonConfig::isValid() const {
    return valid;
}

std::vector<ConfigEntry> JsonConfig::getEntries() const {
    std::vector<ConfigEntry> entries;
    
    if (!valid) {
        return entries;
    }
    
    for (const auto& item : data.items()) {
        std::string key = item.key();
        ConfigValue value;
        
        // Convert JSON value to ConfigValue
        if (item.value().is_string()) {
            value = item.value().get<std::string>();
        } else if (item.value().is_number_integer()) {
            value = item.value().get<int64_t>();
        } else if (item.value().is_number_float()) {
            value = item.value().get<double>();
        } else if (item.value().is_boolean()) {
            value = item.value().get<bool>();
        } else if (item.value().is_array()) {
            // For arrays, use vector<double> as default
            std::vector<double> vec = item.value().get<std::vector<double>>();
            value = vec;
        }
        
        entries.emplace_back(key, value, ConfigSource::File);
    }
    
    return entries;
}

// YAML configuration parser (Phase 4)
YamlConfig::YamlConfig(const std::string& filepath) : valid(false) {
    // Try to parse as YAML (YAML::Node format)
    try {
        data = YAML::LoadFile(filepath);
        valid = true;
    } catch (const std::exception&) {
        // Not valid YAML
    }
}

bool YamlConfig::isValid() const {
    return valid;
}

std::vector<ConfigEntry> YamlConfig::getEntries() const {
    std::vector<ConfigEntry> entries;
    
    if (!valid) {
        return entries;
    }
    
    for (const auto& item : data) {
        std::string key = item.first.as<std::string>();
        ConfigValue value;
        
        // Convert YAML value to ConfigValue
        if (item.second.IsScalar()) {
            if (item.second.IsString()) {
                value = item.second.as<std::string>();
            } else if (item.second.IsInteger()) {
                value = static_cast<int64_t>(item.second.as<int64_t>());
            } else if (item.second.IsFloat()) {
                value = item.second.as<double>();
            } else if (item.second.IsBoolean()) {
                value = item.second.as<bool>();
            }
        } else if (item.second.IsSequence()) {
            // For sequences, use vector<double> as default
            std::vector<double> vec;
            for (const auto& elem : item.second) {
                if (elem.IsScalar()) {
                    if (elem.IsInteger()) {
                        vec.push_back(static_cast<double>(elem.as<int64_t>()));
                    } else if (elem.IsFloat()) {
                        vec.push_back(elem.as<double>());
                    }
                }
            }
            value = vec;
        }
        
        entries.emplace_back(key, value, ConfigSource::File);
    }
    
    return entries;
}

} // namespace nlm
