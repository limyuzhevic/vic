#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

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
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Try JSON format first (modern standard)
#ifdef HAVE_NLOHMANN_JSON
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        clear();
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string description = "Loaded from JSON";
            
            // Set appropriate type and value
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File, description);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File, description);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File, description);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File, description);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File, description);
            } else if (it->is_array()) {
                // Try to determine array element type
                std::vector<std::string> stringArray;
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                
                for (const auto& item : *it) {
                    if (item.is_string()) stringArray.push_back(item.get<std::string>());
                    else if (item.is_number_integer()) intArray.push_back(item.get<int>());
                    else if (item.is_number_float()) doubleArray.push_back(item.get<double>());
                    else {
                        // For mixed or unknown types, use string representation
                        stringArray.push_back(item.dump());
                    }
                }
                
                // Prefer most specific type
                if (!stringArray.empty()) {
                    set(key, stringArray, ConfigSource::File, description);
                } else if (!intArray.empty()) {
                    set(key, intArray, ConfigSource::File, description);
                } else if (!doubleArray.empty()) {
                    set(key, doubleArray, ConfigSource::File, description);
                }
            } else if (it->is_null()) {
                set(key, std::string("null"), ConfigSource::File, description);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Failed to parse JSON config, falling back to simple format: " + std::string(e.what()));
        file.clear();
        file.seekg(0);
    }
#endif
    
    // Fallback to simple key=value format
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            set(key, value, ConfigSource::File, "Loaded from config file line " + std::to_string(lineNum));
        } else {
            NLM_LOG_WARN("Invalid config line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple config file: " + filepath);
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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

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
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Try JSON format first (modern standard)
#ifdef HAVE_NLOHMANN_JSON
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        clear();
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string description = "Loaded from JSON";
            
            // Set appropriate type and value
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File, description);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File, description);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File, description);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File, description);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File, description);
            } else if (it->is_array()) {
                // Try to determine array element type
                std::vector<std::string> stringArray;
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                
                for (const auto& item : *it) {
                    if (item.is_string()) stringArray.push_back(item.get<std::string>());
                    else if (item.is_number_integer()) intArray.push_back(item.get<int>());
                    else if (item.is_number_float()) doubleArray.push_back(item.get<double>());
                    else {
                        // For mixed or unknown types, use string representation
                        stringArray.push_back(item.dump());
                    }
                }
                
                // Prefer most specific type
                if (!stringArray.empty()) {
                    set(key, stringArray, ConfigSource::File, description);
                } else if (!intArray.empty()) {
                    set(key, intArray, ConfigSource::File, description);
                } else if (!doubleArray.empty()) {
                    set(key, doubleArray, ConfigSource::File, description);
                }
            } else if (it->is_null()) {
                set(key, std::string("null"), ConfigSource::File, description);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Failed to parse JSON config, falling back to simple format: " + std::string(e.what()));
        file.clear();
        file.seekg(0);
    }
#endif
    
    // Fallback to simple key=value format
    std::string line;
    size_t lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
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
            
            set(key, value, ConfigSource::File, "Loaded from config file line " + std::to_string(lineNum));
        } else {
            NLM_LOG_WARN("Invalid config line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple config file: " + filepath);
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save config to file: " + filepath);
        return false;
    }
    
    // Try JSON format first
#ifdef HAVE_NLOHMANN_JSON
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&jsonData, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t> || 
                                   std::is_same_v<T, int> ||
                                   std::is_same_v<T, uint64_t> ||
                                   std::is_same_v<T, uint32_t>) {
                    jsonData[entry.key] = static_cast<int64_t>(arg);
                } else if constexpr (std::is_same_v<T, double> ||
                                   std::is_same_v<T, float>) {
                    jsonData[entry.key] = static_cast<double>(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    std::vector<int64_t> temp(arg.begin(), arg.end());
                    jsonData[entry.key] = temp;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    std::vector<double> temp(arg.begin(), arg.end());
                    jsonData[entry.key] = temp;
                }
            }, entry.value);
        }
        
        file << jsonData.dump(2); // Pretty print with 2-space indentation
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_WARN("Failed to save JSON config, falling back to simple format: " + std::string(e.what()));
    }
#endif
    
    // Fallback to simple key=value format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n\n";
    }
    
    NLM_LOG_INFO("Saved configuration to simple config file: " + filepath);
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
