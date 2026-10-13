#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <regex>
#include <cctype>

namespace nlm {

// Helper function to convert string to lowercase
static std::string toLowercase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Global registry of configuration schema definitions
static std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> g_configValidators;

// Register configuration schema (internal use)
static void registerConfigValidator(const std::string& key, 
                                   std::function<bool(const ConfigValue&)> validator) {
    g_configValidators[key] = std::move(validator);
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if nlohmann_json is available
#ifdef NLOHMANN_JSON_VERSION_MAJOR
    // Use JSON parser if available
    return loadFromJsonFile(filepath);
#else
    // Fallback to simple key=value format
    return loadFromSimpleFile(filepath);
#endif
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
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
    
    NLM_LOG_INFO("Loaded simple config from: " + filepath);
    return true;
}

#ifdef NLOHMANN_JSON_VERSION_MAJOR
bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        // Parse JSON file
        auto json = nlohmann::json::parse(file);
        
        // Clear existing entries
        clear();
        
        // Load from JSON
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            
            // Try to determine type and set value
            const auto& value = it.value();
            ConfigSource source = ConfigSource::File;
            
            // Set value based on type
            if (value.is_string()) {
                set(key, value.get<std::string>(), source);
            } else if (value.is_number_integer()) {
                set(key, value.get<int64_t>(), source);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), source);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), source);
            } else if (value.is_array()) {
                // Handle arrays
                std::vector<int64_t> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& item : value) {
                    if (item.is_number_integer()) {
                        intArray.push_back(item.get<int64_t>());
                    } else if (item.is_number_float()) {
                        doubleArray.push_back(item.get<double>());
                    } else if (item.is_string()) {
                        stringArray.push_back(item.get<std::string>());
                    }
                }
                
                // Choose the most appropriate vector type
                if (!intArray.empty()) {
                    set(key, intArray, source);
                } else if (!doubleArray.empty()) {
                    set(key, doubleArray, source);
                } else if (!stringArray.empty()) {
                    set(key, stringArray, source);
                } else {
                    NLM_LOG_WARNING("Empty array for key: " + key);
                    std::vector<int> emptyIntArray;
                    set(key, emptyIntArray, source);
                }
            }
            
// Validate if validator exists
            if (g_configValidators.find(key) != g_configValidators.end()) {
                // Convert json value to ConfigValue for validation
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
                    // Convert array to appropriate vector type
                    std::vector<int64_t> intArray;
                    std::vector<double> doubleArray;
                    std::vector<std::string> stringArray;
                    
                    for (const auto& item : value) {
                        if (item.is_number_integer()) {
                            intArray.push_back(item.get<int64_t>());
                        } else if (item.is_number_float()) {
                            doubleArray.push_back(item.get<double>());
                        } else if (item.is_string()) {
                            stringArray.push_back(item.get<std::string>());
                        }
                    }
                    
                    if (!intArray.empty()) {
                        configValue = intArray;
                    } else if (!doubleArray.empty()) {
                        configValue = doubleArray;
                    } else if (!stringArray.empty()) {
                        configValue = stringArray;
                    } else {
                        // Empty array - use int vector
                        configValue = std::vector<int>();
                    }
                } else {
                    // Handle null or other unsupported types
                    // For validation purposes, use an empty string
                    configValue = std::string();
                }
                
                if (!g_configValidators[key](configValue)) {
                    NLM_LOG_WARNING("Invalid value for key " + key + ", using default");
                    remove(key);
                }
            }
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}
#endif

std::string Config::generateJson(const std::string& filepath) const {
#ifdef NLOHMANN_JSON_VERSION_MAJOR
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        // Add to JSON based on type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return json.dump(2); // Pretty print with 2-space indentation
#else
    return "{}"; // Placeholder if no JSON support
#endif
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
#ifdef NLOHMANN_JSON_VERSION_MAJOR
    // Save as JSON file if available
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for saving: " + filepath);
        return false;
    }
    
    try {
        std::string jsonContent = generateJson(filepath);
        file << jsonContent;
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    // Fallback to simple key=value format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for saving: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string
        std::ostringstream valueStream;
        std::visit([&valueStream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStream << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStream << (arg ? "true" : "false");
            } else {
                valueStream << arg;
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStream.str() << "\n";
    }
    
    NLM_LOG_INFO("Saved configuration to simple file: " + filepath);
    return true;
#endif
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
    return toLowercase(str);
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
