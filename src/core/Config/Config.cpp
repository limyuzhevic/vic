#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <stack>
#include <nlohmann/json.hpp>  // For proper JSON parsing

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

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

/**
 * @brief Loads configuration from a JSON file using the nlohmann::json library
 * 
 * This function reads a JSON file and parses it into the configuration system.
 * It supports both object notation {"key": "value"} and array notation [{"key": "value"}]
 * 
 * @param filepath Path to the JSON configuration file
 * @return true if file was successfully loaded and parsed, false otherwise
 */
bool Config::loadFromJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON configuration file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                // Convert JSON value to ConfigValue based on type
                if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_number_integer()) {
                    value = it->get<int64_t>();
                } else if (it->is_number_unsigned()) {
                    value = it->get<uint64_t>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_array()) {
                    // Handle arrays
                    auto array = it->get<std::vector<nlohmann::json>>();
                    if (!array.empty()) {
                        if (array[0].is_string()) {
                            std::vector<std::string> strArray;
                            strArray.reserve(array.size());
                            for (const auto& item : array) {
                                strArray.push_back(item.get<std::string>());
                            }
                            value = strArray;
                        } else if (array[0].is_number_integer()) {
                            std::vector<int> intArray;
                            intArray.reserve(array.size());
                            for (const auto& item : array) {
                                intArray.push_back(item.get<int>());
                            }
                            value = intArray;
                        } else if (array[0].is_number_float()) {
                            std::vector<double> doubleArray;
                            doubleArray.reserve(array.size());
                            for (const auto& item : array) {
                                doubleArray.push_back(item.get<double>());
                            }
                            value = doubleArray;
                        }
                    }
                }
                
                if (!value.valueless_by_exception()) {
                    set(key, value, ConfigSource::File);
                }
            }
        } else if (jsonData.is_array()) {
            // Handle array of objects
            for (const auto& item : jsonData) {
                if (item.is_object()) {
                    // For array format, prefix keys with "item_" to avoid conflicts
                    int index = 0;
                    for (auto it = item.begin(); it != item.end(); ++it, ++index) {
                        std::string key = "item_" + std::to_string(index) + "_" + it.key();
                        ConfigValue value;
                        
                        // Convert JSON value to ConfigValue based on type
                        if (it->is_string()) {
                            value = it->get<std::string>();
                        } else if (it->is_number_integer()) {
                            value = it->get<int64_t>();
                        } else if (it->is_number_unsigned()) {
                            value = it->get<uint64_t>();
                        } else if (it->is_number_float()) {
                            value = it->get<double>();
                        } else if (it->is_boolean()) {
                            value = it->get<bool>();
                        }
                        
                        if (!value.valueless_by_exception()) {
                            set(key, value, ConfigSource::File);
                        }
                    }
                }
            }
        }
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

/**
 * @brief Attempts to detect if a file is JSON format by checking its content
 * 
 * @param filepath Path to the file to check
 * @return true if file appears to be JSON, false otherwise
 */
bool Config::isJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Read first 1KB to check for JSON characteristics
        char buffer[1024];
        file.read(buffer, sizeof(buffer) - 1);
        buffer[file.gcount()] = '\0';
        
        std::string content(buffer);
        
        // Trim whitespace
        size_t start = content.find_first_not_of(" \t\r\n");
        size_t end = content.find_last_not_of(" \t\r\n");
        if (start == std::string::npos || end == std::string::npos) {
            return false;
        }
        
        std::string trimmed = content.substr(start, end - start + 1);
        
        // Check for JSON characteristics
        bool isJson = false;
        
        if (trimmed.empty()) {
            return false;
        }
        
        // JSON objects start with {
        if (trimmed[0] == '{') {
            isJson = true;
            
            // Check for matching braces (simplified)
            int braceCount = 0;
            for (char c : trimmed) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
            }
            isJson = (braceCount == 0);
        }
        // JSON arrays start with [
        else if (trimmed[0] == '[') {
            isJson = true;
            
            // Check for matching brackets (simplified)
            int bracketCount = 0;
            for (char c : trimmed) {
                if (c == '[') bracketCount++;
                else if (c == ']') bracketCount--;
            }
            isJson = (bracketCount == 0);
        }
        
        return isJson;
        
    } catch (const std::exception& e) {
        return false;
    }
}

/**
 * @brief Attempts to detect if a file is YAML format by checking its content
 * 
 * @param filepath Path to the file to check
 * @return true if file appears to be YAML, false otherwise
 */
bool Config::isYamlFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Read first 1KB to check for YAML characteristics
        char buffer[1024];
        file.read(buffer, sizeof(buffer) - 1);
        buffer[file.gcount()] = '\0';
        
        std::string content(buffer);
        
        // Look for YAML characteristics
        // YAML files often have:
        // 1. Indentation with spaces
        // 2. Key-value pairs with colons
        // 3. Lists starting with dashes
        bool hasIndentation = false;
        bool hasColonPair = false;
        bool hasDashList = false;
        
        size_t lineStart = 0;
        size_t lineEnd;
        while ((lineEnd = content.find('\n', lineStart)) != std::string::npos) {
            std::string line = content.substr(lineStart, lineEnd - lineStart);
            
            // Check for indentation (lines starting with spaces)
            if (!line.empty() && (line[0] == ' ' || line[0] == '\t')) {
                hasIndentation = true;
            }
            
            // Check for key-value pair with colon
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos && colonPos > 0) {
                // Check if there's text before the colon (indicating a key)
                std::string keyPart = line.substr(0, colonPos);
                if (!keyPart.empty() && keyPart.find_first_not_of(" \t") != std::string::npos) {
                    hasColonPair = true;
                }
            }
            
            // Check for dash list items
            size_t firstNonSpace = line.find_first_not_of(" \t");
            if (firstNonSpace != std::string::npos && line[firstNonSpace] == '-') {
                hasDashList = true;
            }
            
            lineStart = lineEnd + 1;
        }
        
        // Also check for YAML document start marker
        bool hasYamlStart = (content.find("---") != std::string::npos);
        
        // Consider it YAML if it has multiple YAML characteristics
        int yamlIndicators = 0;
        if (hasIndentation) yamlIndicators++;
        if (hasColonPair) yamlIndicators++;
        if (hasDashList) yamlIndicators++;
        if (hasYamlStart) yamlIndicators++;
        
        return yamlIndicators >= 2;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Helper function to convert ConfigValue to string
        auto valueToString = [](const ConfigValue& val) {
            std::ostringstream oss;
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << arg[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << arg[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << "\"" << arg[i] << "\"";
                    }
                    oss << "]";
                } else {
                    oss << arg;
                }
            }, val.value);
            return oss.str();
        };
        
        file << valueToString(entry.value) << "\n\n";
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

// Enhanced error handling and logging for config operations
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Log the update
        NLM_LOG_DEBUG("Updating configuration key: " + key + " from source " + 
                     std::to_string(static_cast<int>(it->source)) + " to source " +
                     std::to_string(static_cast<int>(source)));
        it->value = value;
        it->source = source;
    } else {
        // Log new entry
        NLM_LOG_DEBUG("Adding new configuration key: " + key);
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
    auto originalSize = pImpl->entries.size();
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    if (pImpl->entries.size() < originalSize) {
        NLM_LOG_INFO("Removed configuration key: " + key);
    } else {
        NLM_LOG_WARNING("Attempted to remove non-existent configuration key: " + key);
    }
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
    NLM_LOG_INFO("Clearing all configuration entries");
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
