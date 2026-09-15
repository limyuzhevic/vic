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
    // Try to load as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Convert JSON to config entries
        pImpl->entries.clear();
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Handle different JSON types
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Handle arrays
                if (!it->empty() && it->front().is_number_integer()) {
                    std::vector<int64_t> intArray;
                    for (const auto& item : *it) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    value = intArray;
                } else if (!it->empty() && it->front().is_number_float()) {
                    std::vector<double> floatArray;
                    for (const auto& item : *it) {
                        floatArray.push_back(item.get<double>());
                    }
                    value = floatArray;
                } else if (!it->empty() && it->front().is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : *it) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    value = stringArray;
                }
            }
            
            pImpl->entries.emplace_back(key, value, ConfigSource::File, "Loaded from JSON");
        }
        
        return true;
    } catch (const std::exception&) {
        // If JSON parsing fails, fall back to simple key=value format
        file.clear();
        file.seekg(0);
        
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
                std::string valueStr = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (valueStr.size() >= 2 && 
                    ((valueStr.front() == '"' && valueStr.back() == '"') ||
                     (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                // Try to convert value to appropriate type
                ConfigValue value;
                bool converted = false;
                
                // Try int64_t
                try {
                    size_t posEnd;
                    int64_t intVal = std::stoll(valueStr, &posEnd);
                    if (posEnd == valueStr.size()) {
                        value = intVal;
                        converted = true;
                    }
                } catch (...) {}
                
                // Try double
                if (!converted) {
                    try {
                        size_t posEnd;
                        double doubleVal = std::stod(valueStr, &posEnd);
                        if (posEnd == valueStr.size()) {
                            value = doubleVal;
                            converted = true;
                        }
                    } catch (...) {}
                }
                
                // Try bool
                if (!converted) {
                    if (valueStr == "true") {
                        value = true;
                        converted = true;
                    } else if (valueStr == "false") {
                        value = false;
                        converted = true;
                    }
                }
                
                // Fall back to string
                if (!converted) {
                    value = valueStr;
                }
                
                set(key, value, ConfigSource::File);
            }
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
    
    // Try to save as JSON for better readability
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Add description as comment
        if (!entry.description.empty()) {
            jsonData[entry.key] = entry.description;
        } else {
            // Use std::visit to get the value
            std::visit([&](const auto& val) {
                jsonData[entry.key] = val;
            }, entry.value);
        }
    }
    
    file << jsonData.dump(2); // Pretty print with 2-space indentation
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
        
        // Use direct type index to avoid nlohmann::json compatibility issues
        switch (entry.value.index()) {
            case 0: // int64_t
                oss << std::get<0>(entry.value);
                break;
            case 1: // uint64_t
                oss << std::get<1>(entry.value);
                break;
            case 2: // double
                oss << std::get<2>(entry.value);
                break;
            case 3: // bool
                oss << (std::get<3>(entry.value) ? "true" : "false");
                break;
            case 4: // std::string
                oss << "\"" << std::get<4>(entry.value) << "\"";
                break;
            case 5: // std::vector<int64_t>
                oss << "[";
                for (size_t i = 0; i < std::get<5>(entry.value).size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::get<5>(entry.value)[i];
                }
                oss << "]";
                break;
            case 6: // std::vector<double>
                oss << "[";
                for (size_t i = 0; i < std::get<6>(entry.value).size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::get<6>(entry.value)[i];
                }
                oss << "]";
                break;
            case 7: // std::vector<std::string>
                oss << "[";
                for (size_t i = 0; i < std::get<7>(entry.value).size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << std::get<7>(entry.value)[i] << "\"";
                }
                oss << "]";
                break;
        }
        oss << "[] (" << static_cast<int>(entry.source) << ")\n";
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
