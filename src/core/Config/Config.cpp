#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

// Include JSON and YAML libraries if available
// Check if nlohmann/json is available
#if __has_include(<nlohmann/json.hpp>)
    #include <nlohmann/json.hpp>
    #define HAS_NLOHMANN_JSON 1
#else
    #define HAS_NLOHMANN_JSON 0
#endif

// Check if yaml-cpp is available  
#if __has_include(<yaml-cpp/yaml.h>)
    #include <yaml-cpp/yaml.h>
    #define HAS_YAML_CPP 1
#else
    #define HAS_YAML_CPP 0
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function implementations
namespace detail {
    bool isJSONFile(const std::string& filepath) {
        const std::string ext = std::filesystem::path(filepath).extension().string();
        std::string lowerExt = ext;
        std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
        
        if (lowerExt == ".json") return true;
        
        // Also check by content for .conf files
        if (lowerExt == ".conf") {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            
            std::string firstLine;
            if (std::getline(file, firstLine)) {
                // Check if first non-empty, non-comment line looks like JSON
                std::string trimmed = trim(firstLine);
                return !trimmed.empty() && (trimmed.front() == '{' || trimmed.front() == '[');
            }
        }
        
        return false;
    }
    
    bool isYAMLFile(const std::string& filepath) {
        const std::string ext = std::filesystem::path(filepath).extension().string();
        std::string lowerExt = ext;
        std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), ::tolower);
        
        if (lowerExt == ".yaml" || lowerExt == ".yml") return true;
        
        // Also check by content
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::string content;
        char ch;
        int braceCount = 0;
        bool foundContent = false;
        
        while (file.get(ch)) {
            content.push_back(ch);
            if (std::isspace(ch)) continue;
            if (ch == '{' || ch == '[') {
                braceCount++;
                foundContent = true;
            } else if (ch == '}' || ch == ']') {
                braceCount--;
            } else if (foundContent && braceCount == 0) {
                // Found content that's not inside braces
                return false;
            }
            
            // Safety check
            if (content.length() > 10000) break;
        }
        
        return foundContent && braceCount == 0;
    }
    
    bool isSimpleFormatFile(const std::string& filepath) {
        // Check if file looks like simple key=value format
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        int keyValueLines = 0;
        int totalLines = 0;
        std::string line;
        
        while (std::getline(file, line) && totalLines < 1000) {
            totalLines++;
            line = trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos && pos > 0) {
                keyValueLines++;
            }
        }
        
        return totalLines > 0 && (double)keyValueLines / totalLines > 0.5;
    }
    
    bool parseSimpleFormat(const std::string& content, Config* config) {
        std::istringstream stream(content);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(stream, line)) {
            lineNum++;
            line = trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos && pos > 0) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to parse value to appropriate type
                try {
                    // Check if it's a number
                    size_t posInt;
                    int intVal = std::stoi(value, &posInt);
                    if (posInt == value.size()) {
                        config->set(key, intVal, ConfigSource::File);
                        continue;
                    }
                    
                    // Check if it's a double
                    size_t posDbl;
                    double dblVal = std::stod(value, &posDbl);
                    if (posDbl == value.size()) {
                        config->set(key, dblVal, ConfigSource::File);
                        continue;
                    }
                    
                    // Check if it's a boolean
                    if (value == "true" || value == "false") {
                        config->set(key, value == "true", ConfigSource::File);
                        continue;
                    }
                    
                    // Otherwise treat as string
                    config->set(key, value, ConfigSource::File);
                } catch (const std::exception&) {
                    // If parsing fails, treat as string
                    config->set(key, value, ConfigSource::File);
                }
            }
        }
        
        return true;
    }
    
    std::string configValueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    std::string configValueToYAML(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << '"' << item << '"';
                    first = false;
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    // Determine file format
    bool isJSON = detail::isJSONFile(filepath);
    bool isYAML = detail::isYAMLFile(filepath);
    bool isSimple = detail::isSimpleFormatFile(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        if (isJSON && HAS_NLOHMANN_JSON) {
            // Parse as JSON using nlohmann/json
            nlohmann::json jsonData;
            file >> jsonData;
            
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                const auto& value = it.value();
                
                ConfigValue configValue;
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Handle arrays
                    if (!value.empty()) {
                        const auto& first = value[0];
                        if (first.is_number_integer()) {
                            std::vector<int> intArray;
                            intArray.reserve(value.size());
                            for (const auto& item : value) {
                                intArray.push_back(item.get<int>());
                            }
                            configValue = intArray;
                        } else if (first.is_number_float()) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(value.size());
                            for (const auto& item : value) {
                                doubleArray.push_back(item.get<double>());
                            }
                            configValue = doubleArray;
                        } else if (first.is_string()) {
                            std::vector<std::string> stringArray;
                            stringArray.reserve(value.size());
                            for (const auto& item : value) {
                                stringArray.push_back(item.get<std::string>());
                            }
                            configValue = stringArray;
                        }
                    }
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            return true;
        } else if (isYAML && HAS_YAML_CPP) {
            // Parse as YAML using yaml-cpp
            YAML::Node yamlData = YAML::LoadFile(filepath);
            
            for (const auto& pair : yamlData) {
                std::string key = pair.first.as<std::string>();
                const YAML::Node& value = pair.second;
                
                ConfigValue configValue;
                
                if (value.IsScalar()) {
                    if (value.IsMap() || value.IsSequence()) {
                        // Complex type
                        return false;
                    }
                    
                    if (value.IsBoolean()) {
                        configValue = value.as<bool>();
                    } else if (value.IsInt()) {
                        configValue = value.as<int>();
                    } else if (value.IsDouble()) {
                        configValue = value.as<double>();
                    } else {
                        configValue = value.as<std::string>();
                    }
                } else if (value.IsSequence()) {
                    // Handle arrays
                    if (!value.IsNull() && value.size() > 0) {
                        const YAML::Node& first = value[0];
                        if (first.IsInt()) {
                            std::vector<int> intArray;
                            intArray.reserve(value.size());
                            for (const auto& item : value) {
                                intArray.push_back(item.as<int>());
                            }
                            configValue = intArray;
                        } else if (first.IsFloat()) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(value.size());
                            for (const auto& item : value) {
                                doubleArray.push_back(item.as<double>());
                            }
                            configValue = doubleArray;
                        } else if (first.IsScalar()) {
                            std::vector<std::string> stringArray;
                            stringArray.reserve(value.size());
                            for (const auto& item : value) {
                                stringArray.push_back(item.as<std::string>());
                            }
                            configValue = stringArray;
                        }
                    }
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
            return true;
        } else {
            // Fall back to simple format
            std::ifstream file2(filepath);
            if (!file2.is_open()) {
                return false;
            }
            
            std::string content((std::istreambuf_iterator<char>(file2)),
                                std::istreambuf_iterator<char>());
            
            return detail::parseSimpleFormat(content, this);
        }
    } catch (const std::exception& e) {
        // If JSON/YAML parsing fails, fall back to simple format
        std::ifstream file2(filepath);
        if (!file2.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file2)),
                            std::istreambuf_iterator<char>());
        
        return detail::parseSimpleFormat(content, this);
    }
}

// Rest of Config.cpp implementation

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
    
    // Write as JSON format (preferred)
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Add description as comment
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Store in JSON
        jsonData[entry.key] = detail::configValueToString(entry.value);
        
        // Also write simple format for backward compatibility
        file << entry.key << " = " << detail::configValueToString(entry.value) << "\n";
    }
    
    // Write JSON content to file as well (append or overwrite)
    std::string jsonFilepath = filepath;
    size_t dotPos = jsonFilepath.rfind('.');
    if (dotPos != std::string::npos) {
        jsonFilepath.replace(dotPos, std::string::npos, ".json");
    } else {
        jsonFilepath += ".json";
    }
    
    std::ofstream jsonFile(jsonFilepath);
    if (jsonFile.is_open()) {
        jsonFile << jsonData.dump(2);
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
