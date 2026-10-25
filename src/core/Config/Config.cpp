#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format based on extension
    std::string extension = getFileExtension(filepath);
    
    if (isJSONFile(filepath)) {
        // JSON parsing
        return loadFromJSON(filepath);
    } else if (isYAMLFile(filepath)) {
        // YAML parsing  
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFile(filepath);
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

// Internal helper function for simple key=value file parsing
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

// Load configuration from JSON file
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Check if nlohmann/json is available
        #ifdef NLOHMANN_JSON_VERSION_MAJOR
        nlohmann::json j;
        file >> j;
        
        // Process JSON object
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue configValue;
            
            // Convert JSON value to appropriate ConfigValue type
            if (it->is_string()) {
                configValue = it->get<std::string>();
            } else if (it->is_boolean()) {
                configValue = it->get<bool>();
            } else if (it->is_number_integer()) {
                configValue = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                configValue = static_cast<int64_t>(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                configValue = it->get<double>();
            } else if (it->is_array()) {
                // Handle array types
                const auto& arr = it->get<std::vector<nlohmann::json>>();
                std::vector<int> intArr;
                bool isIntArray = true;
                
                for (const auto& item : arr) {
                    if (item.is_number_integer()) {
                        intArr.push_back(item.get<int>());
                    } else if (item.is_number_float()) {
                        isIntArray = false;
                        break;
                    } else {
                        isIntArray = false;
                        break;
                    }
                }
                
                if (isIntArray) {
                    configValue = intArr;
                } else {
                    // Try as double array
                    std::vector<double> doubleArr;
                    bool isDoubleArray = true;
                    
                    for (const auto& item : arr) {
                        if (item.is_number()) {
                            doubleArr.push_back(item.get<double>());
                        } else {
                            isDoubleArray = false;
                            break;
                        }
                    }
                    
                    if (isDoubleArray) {
                        configValue = doubleArr;
                    } else {
                        // Try as string array
                        std::vector<std::string> stringArr;
                        bool isStringArray = true;
                        
                        for (const auto& item : arr) {
                            if (item.is_string()) {
                                stringArr.push_back(item.get<std::string>());
                            } else {
                                isStringArray = false;
                                break;
                            }
                        }
                        
                        if (isStringArray) {
                            configValue = stringArr;
                        } else {
                            // Fallback to string representation
                            configValue = it->dump();
                        }
                    }
                }
            } else {
                // Fallback to string representation
                configValue = it->dump();
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    #else
        // JSON library not available, fallback to simple format
        NLM_LOG_WARNING("JSON parsing not available, falling back to simple format");
        return loadFromSimpleFile(filepath);
    #endif
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Load configuration from YAML file
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Note: This requires yaml-cpp library to be linked
        // For now, fallback to simple format
        NLM_LOG_WARNING("YAML parsing not implemented yet, falling back to simple format");
        return loadFromSimpleFile(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save as simple key=value format for backward compatibility
    // TODO: Implement JSON/YAML output format option
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Helper function to write value based on type
        auto writeValue = [&file](const ConfigValue& value) {
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, value);
        };
        
        writeValue(entry.value);
        file << "\n";
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

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters
    static const std::regex invalidKeyChars("[\\s\\.#\\$\\@\\*\\?\\/\\|\\<\\>\\&\\^\\%\\=\\'\\\\]");
    return !std::regex_search(key, invalidKeyChars);
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

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
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

std::string Config::summaryBySource(ConfigSource source) const {
    std::ostringstream oss;
    oss << "Configuration from " << static_cast<int>(source) << ":\n";
    
    bool found = false;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "]\n";
            found = true;
        }
    }
    
    if (!found) {
        oss << "  (no entries)\n";
    }
    
    return oss.str();
}

bool Config::fileExists(const std::string& filepath) const {
    return std::filesystem::exists(filepath);
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filepath.substr(dotPos + 1);
    }
    return "";
}

bool Config::isJSONFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "json";
}

bool Config::isYAMLFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "yaml" || ext == "yml";
}

bool Config::isSimpleFormatFile(const std::string& filepath) {
    return !isJSONFile(filepath) && !isYAMLFile(filepath);
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