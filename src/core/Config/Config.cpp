#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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
        NLM_LOG_ERROR("Could not open configuration file: " + filepath);
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json" || ext == ".json5" || ext == ".jsonc") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Default to simple key=value format for backward compatibility
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open JSON configuration file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            const auto& value = *it;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle arrays
                if (value.empty()) {
                    continue;
                }
                // Determine array type based on first element
                if (value[0].is_number_integer()) {
                    std::vector<int64_t> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int64_t>());
                    }
                    configValue = intArray;
                } else if (value[0].is_number_float()) {
                    std::vector<double> floatArray;
                    for (const auto& item : value) {
                        floatArray.push_back(item.get<double>());
                    }
                    configValue = floatArray;
                } else if (value[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : value) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    configValue = stringArray;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open YAML configuration file: " + filepath);
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        for (const auto& node : yamlData) {
            std::string key = node.first.as<std::string>();
            
            ConfigValue configValue;
            try {
                const auto& value = node.second;
                
                if (value.IsScalar()) {
                    std::string scalarValue = value.as<std::string>();
                    // Try to parse as different types
                    if (scalarValue == "true" || scalarValue == "false") {
                        configValue = scalarValue == "true";
                    } else {
                        try {
                            // Try integer
                            int64_t intVal = std::stoll(scalarValue);
                            configValue = intVal;
                        } catch (...) {
                            try {
                                // Try double
                                double doubleVal = std::stod(scalarValue);
                                configValue = doubleVal;
                            } catch (...) {
                                // Fall back to string
                                configValue = scalarValue;
                            }
                        }
                    }
                } else if (value.IsSequence()) {
                    // Handle YAML sequences (arrays)
                    std::vector<int64_t> intArray;
                    std::vector<double> floatArray;
                    std::vector<std::string> stringArray;
                    bool allInt = true;
                    bool allFloat = true;
                    bool allString = true;
                    
                    for (const auto& item : value) {
                        if (item.IsScalar()) {
                            std::string strValue = item.as<std::string>();
                            if (allInt) {
                                try {
                                    int64_t intVal = std::stoll(strValue);
                                    intArray.push_back(intVal);
                                } catch (...) {
                                    allInt = false;
                                }
                            }
                            if (allFloat && allInt) {
                                try {
                                    double floatVal = std::stod(strValue);
                                    floatArray.push_back(floatVal);
                                } catch (...) {
                                    allFloat = false;
                                }
                            }
                            if (allString) {
                                stringArray.push_back(strValue);
                            }
                        }
                    }
                    
                    if (allInt && !intArray.empty()) {
                        configValue = intArray;
                    } else if (allFloat && !floatArray.empty()) {
                        configValue = floatArray;
                    } else if (!stringArray.empty()) {
                        configValue = stringArray;
                    }
                } else if (value.IsMap()) {
                    // Handle nested maps - convert to JSON string for now
                    // Could be enhanced to handle nested structures
                    YAML::Emitter emitter;
                    emitter << value;
                    std::string yamlStr;
                    std::stringstream ss;
                    ss << emitter.c_str();
                    configValue = ss.str();
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Skipping config key '" + key + "' due to error: " + e.what());
                continue;
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded YAML configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error in " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    // Original simple key=value format for backward compatibility
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + " in " + filepath + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded simple format configuration from: " + filepath);
    return true;
}

void Config::loadFromArgs(int argc, char** argv) {
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
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to JSON value
            std::string key = entry.key;
            
            if (std::holds_alternative<int>(entry.value)) {
                jsonData[key] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                jsonData[key] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                jsonData[key] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                jsonData[key] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                jsonData[key] = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                jsonData[key] = std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                jsonData[key] = std::get<std::vector<double>>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                jsonData[key] = std::get<std::vector<std::string>>(entry.value);
            }
        }
        
        // Write JSON with indentation for readability
        file << std::setw(2) << jsonData;
        
        NLM_LOG_INFO("Saved JSON configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to " + filepath + ": " + e.what());
        return false;
    }
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
