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

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced JSON/YAML parser with full support for all configuration types
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Parse as JSON
        auto json = nlohmann::json::parse(file);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process all key-value pairs in the JSON
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            std::string description = "Configuration parameter: " + key;
            
            // Determine the value type and set accordingly
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Try to determine array type
                if (!it->empty()) {
                    if (it->front().is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : *it) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        set(key, ConfigValue(intArray), ConfigSource::File);
                    } else if (it->front().is_number_float()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : *it) {
                            doubleArray.push_back(item.get<double>());
                        }
                        set(key, ConfigValue(doubleArray), ConfigSource::File);
                    } else if (it->front().is_string()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : *it) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        set(key, ConfigValue(stringArray), ConfigSource::File);
                    }
                }
            }
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        // Fall back to simple key=value format
        NLM_LOG_ERROR("JSON parsing failed, falling back to simple format: " + std::string(e.what()));
        return loadFromSimpleFile(filepath);
    }
    
    return true;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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
    // Write configuration in JSON format with enhanced support
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    
    // Add metadata
    json["metadata"] = {
        {"version", "2.0"},
        {"description", "NLM Neural Learning Machine Configuration"},
        {"generated_by", "NLM Phase 2"},
        {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
    };
    
    // Process all configuration entries
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<int>(entry.value)) {
            json[entry.key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            json[entry.key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            json[entry.key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            json[entry.key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            json[entry.key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            json[entry.key] = std::get<std::vector<int>>(entry.value);
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            json[entry.key] = std::get<std::vector<double>>(entry.value);
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            json[entry.key] = std::get<std::vector<std::string>>(entry.value);
        }
    }
    
    // Write JSON to file with pretty formatting
    file << json.dump(2);
    
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
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    oss << "\nJSON Export Available:\n";
    oss << "  saveToFile('config.json') - Save as JSON\n";
    oss << "  loadFromFile('config.json') - Load from JSON\n";
    
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