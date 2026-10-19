#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ValidatorFunc> validators;
    std::vector<ConfigSchemaEntry> schema;
    std::unordered_map<std::string, ConfigValue> defaults;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigParseException("Cannot open file: " + filepath);
    }
    
    try {
        if (format == "json" || format == "unknown") {
            // Try JSON first
            try {
                nlohmann::json jsonData;
                file >> jsonData;
                
                // Clear existing entries
                clear();
                
                // Parse JSON object
                if (jsonData.is_object()) {
                    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                        std::string key = it.key();
                        
                        // Handle nested keys (e.g., "brain.neuron_count" -> "brain.neuron_count")
                        // For now, use the key as-is
                        ConfigValue value;
                        
                        if (it.value().is_string()) {
                            value = it.value().get<std::string>();
                        } else if (it.value().is_number_integer()) {
                            value = it.value().get<int>();
                        } else if (it.value().is_number_unsigned()) {
                            value = it.value().get<uint64_t>();
                        } else if (it.value().is_number_float()) {
                            value = it.value().get<double>();
                        } else if (it.value().is_boolean()) {
                            value = it.value().get<bool>();
                        } else if (it.value().is_array()) {
                            // Handle arrays
                            nlohmann::json arr = it.value();
                            if (!arr.empty()) {
                                // Determine array type based on first element
                                if (arr[0].is_string()) {
                                    std::vector<std::string> strArray;
                                    for (const auto& item : arr) {
                                        strArray.push_back(item.get<std::string>());
                                    }
                                    value = strArray;
                                } else if (arr[0].is_number_integer()) {
                                    std::vector<int> intArray;
                                    for (const auto& item : arr) {
                                        intArray.push_back(item.get<int>());
                                    }
                                    value = intArray;
                                } else if (arr[0].is_number_float()) {
                                    std::vector<double> doubleArray;
                                    for (const auto& item : arr) {
                                        doubleArray.push_back(item.get<double>());
                                    }
                                    value = doubleArray;
                                }
                            }
                        }
                        
                        set(key, value, ConfigSource::File);
                    }
                }
                
                return true;
            } catch (const nlohmann::json::parse_error& e) {
                // If JSON parsing fails, try YAML or simple format
                if (format != "yaml") {
                    // Reset file stream
                    file.clear();
                    file.seekg(0, std::ios::beg);
                    
                    // Try YAML parsing
                    try {
                        YAML::Node yamlData = YAML::Load(file);
                        
                        clear();
                        
                        // Parse YAML node recursively
                        if (yamlData.IsMap()) {
                            for (const auto& yamlEntry : yamlData) {
                                std::string key = yamlEntry.first.as<std::string>();
                                
                                ConfigValue value;
                                if (yamlEntry.second.IsScalar()) {
                                    std::string strValue = yamlEntry.second.as<std::string>();
                                    // Try to infer type
                                    if (strValue == "true" || strValue == "false") {
                                        value = (strValue == "true");
                                    } else {
                                        try {
                                            size_t pos;
                                            int intVal = std::stoi(strValue, &pos);
                                            if (pos == strValue.size()) {
                                                value = intVal;
                                            } else {
                                                double doubleVal = std::stod(strValue, &pos);
                                                if (pos == strValue.size()) {
                                                    value = doubleVal;
                                                } else {
                                                    value = strValue;
                                                }
                                            }
                                        } catch (...) {
                                            value = strValue;
                                        }
                                    }
                                } else if (yamlEntry.second.IsSequence()) {
                                    // Handle sequences (arrays)
                                    const auto& yamlArray = yamlEntry.second;
                                    if (!yamlArray.empty()) {
                                        // Determine array type
                                        if (yamlArray[0].IsScalar()) {
                                            std::string firstVal = yamlArray[0].as<std::string>();
                                            if (firstVal == "true" || firstVal == "false") {
                                                std::vector<bool> boolArray;
                                                for (const auto& item : yamlArray) {
                                                    boolArray.push_back(item.as<bool>());
                                                }
                                                value = boolArray;
                                            } else {
                                                try {
                                                    std::vector<int> intArray;
                                                    for (const auto& item : yamlArray) {
                                                        intArray.push_back(item.as<int>());
                                                    }
                                                    value = intArray;
                                                } catch (...) {
                                                    std::vector<std::string> strArray;
                                                    for (const auto& item : yamlArray) {
                                                        strArray.push_back(item.as<std::string>());
                                                    }
                                                    value = strArray;
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                set(key, value, ConfigSource::File);
                            }
                        }
                    } catch (const YAML::Exception& e) {
                        throw ConfigParseException("Failed to parse file as JSON or YAML: " + std::string(e.what()));
                    }
                }
            }
        } else if (format == "yaml") {
            // Try YAML parsing
            try {
                YAML::Node yamlData;
                file >> yamlData;
                
                clear();
                
                // Parse YAML node recursively
                if (yamlData.IsMap()) {
                    for (const auto& yamlEntry : yamlData) {
                        std::string key = yamlEntry.first.as<std::string>();
                        
                        ConfigValue value;
                        if (yamlEntry.second.IsScalar()) {
                            std::string strValue = yamlEntry.second.as<std::string>();
                            // Try to infer type
                            if (strValue == "true" || strValue == "false") {
                                value = (strValue == "true");
                            } else {
                                try {
                                    size_t pos;
                                    int intVal = std::stoi(strValue, &pos);
                                    if (pos == strValue.size()) {
                                        value = intVal;
                                    } else {
                                        double doubleVal = std::stod(strValue, &pos);
                                        if (pos == strValue.size()) {
                                            value = doubleVal;
                                        } else {
                                            value = strValue;
                                        }
                                    }
                                } catch (...) {
                                    value = strValue;
                                }
                            }
                        } else if (yamlEntry.second.IsSequence()) {
                            // Handle sequences (arrays)
                            const auto& yamlArray = yamlEntry.second;
                            if (!yamlArray.empty()) {
                                // Determine array type
                                if (yamlArray[0].IsScalar()) {
                                    std::string firstVal = yamlArray[0].as<std::string>();
                                    if (firstVal == "true" || firstVal == "false") {
                                        std::vector<bool> boolArray;
                                        for (const auto& item : yamlArray) {
                                            boolArray.push_back(item.as<bool>());
                                        }
                                        value = boolArray;
                                    } else {
                                        try {
                                            std::vector<int> intArray;
                                            for (const auto& item : yamlArray) {
                                                intArray.push_back(item.as<int>());
                                            }
                                            value = intArray;
                                        } catch (...) {
                                            std::vector<std::string> strArray;
                                            for (const auto& item : yamlArray) {
                                                strArray.push_back(item.as<std::string>());
                                            }
                                            value = strArray;
                                        }
                                    }
                                }
                            }
                        }
                        
                        set(key, value, ConfigSource::File);
                    }
                }
                
                return true;
            } catch (const YAML::Exception& e) {
                throw ConfigParseException("Failed to parse YAML file: " + std::string(e.what()));
            }
        } else {
            // Fall back to simple key=value format
            file.clear();
            file.seekg(0, std::ios::beg);
            
            return loadFromFileSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        throw ConfigParseException("Error loading config file " + filepath + ": " + std::string(e.what()));
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

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigParseException("Cannot open file: " + filepath);
    }
    
    try {
        if (format == "json" || format == "unknown") {
            // Try JSON first
            try {
                nlohmann::json jsonData;
                file >> jsonData;
                
                // Clear existing entries
                clear();
                
                // Parse JSON object
                if (jsonData.is_object()) {
                    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                        std::string key = it.key();
                        
                        // Handle nested keys (e.g., "brain.neuron_count" -> "brain.neuron_count")
                        // For now, use the key as-is
                        ConfigValue value;
                        
                        if (it.value().is_string()) {
                            value = it.value().get<std::string>();
                        } else if (it.value().is_number_integer()) {
                            value = it.value().get<int>();
                        } else if (it.value().is_number_unsigned()) {
                            value = it.value().get<uint64_t>();
                        } else if (it.value().is_number_float()) {
                            value = it.value().get<double>();
                        } else if (it.value().is_boolean()) {
                            value = it.value().get<bool>();
                        } else if (it.value().is_array()) {
                            // Handle arrays
                            nlohmann::json arr = it.value();
                            if (!arr.empty()) {
                                // Determine array type based on first element
                                if (arr[0].is_string()) {
                                    std::vector<std::string> strArray;
                                    for (const auto& item : arr) {
                                        strArray.push_back(item.get<std::string>());
                                    }
                                    value = strArray;
                                } else if (arr[0].is_number_integer()) {
                                    std::vector<int> intArray;
                                    for (const auto& item : arr) {
                                        intArray.push_back(item.get<int>());
                                    }
                                    value = intArray;
                                } else if (arr[0].is_number_float()) {
                                    std::vector<double> doubleArray;
                                    for (const auto& item : arr) {
                                        doubleArray.push_back(item.get<double>());
                                    }
                                    value = doubleArray;
                                }
                            }
                        }
                        
                        set(key, value, ConfigSource::File);
                    }
                }
                
                return true;
            } catch (const nlohmann::json::parse_error& e) {
                // If JSON parsing fails, try YAML or simple format
                if (format != "yaml") {
                    // Reset file stream
                    file.clear();
                    file.seekg(0, std::ios::beg);
                    
                    // Try YAML parsing
                    try {
                        YAML::Node yamlData = YAML::Load(file);
                        
                        clear();
                        
                        // Parse YAML node recursively
                        if (yamlData.IsMap()) {
                            for (const auto& yamlEntry : yamlData) {
                                std::string key = yamlEntry.first.as<std::string>();
                                
                                ConfigValue value;
                                if (yamlEntry.second.IsScalar()) {
                                    std::string strValue = yamlEntry.second.as<std::string>();
                                    // Try to infer type
                                    if (strValue == "true" || strValue == "false") {
                                        value = (strValue == "true");
                                    } else {
                                        try {
                                            size_t pos;
                                            int intVal = std::stoi(strValue, &pos);
                                            if (pos == strValue.size()) {
                                                value = intVal;
                                            } else {
                                                double doubleVal = std::stod(strValue, &pos);
                                                if (pos == strValue.size()) {
                                                    value = doubleVal;
                                                } else {
                                                    value = strValue;
                                                }
                                            }
                                        } catch (...) {
                                            value = strValue;
                                        }
                                    }
                                } else if (yamlEntry.second.IsSequence()) {
                                    // Handle sequences (arrays)
                                    const auto& yamlArray = yamlEntry.second;
                                    if (!yamlArray.empty()) {
                                        // Determine array type
                                        if (yamlArray[0].IsScalar()) {
                                            std::string firstVal = yamlArray[0].as<std::string>();
                                            if (firstVal == "true" || firstVal == "false") {
                                                std::vector<bool> boolArray;
                                                for (const auto& item : yamlArray) {
                                                    boolArray.push_back(item.as<bool>());
                                                }
                                                value = boolArray;
                                            } else {
                                                try {
                                                    std::vector<int> intArray;
                                                    for (const auto& item : yamlArray) {
                                                        intArray.push_back(item.as<int>());
                                                    }
                                                    value = intArray;
                                                } catch (...) {
                                                    std::vector<std::string> strArray;
                                                    for (const auto& item : yamlArray) {
                                                        strArray.push_back(item.as<std::string>());
                                                    }
                                                    value = strArray;
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                set(key, value, ConfigSource::File);
                            }
                        }
                    } catch (const YAML::Exception& e) {
                        throw ConfigParseException("Failed to parse file as JSON or YAML: " + std::string(e.what()));
                    }
                }
            }
        } else if (format == "yaml") {
            // Try YAML parsing
            try {
                YAML::Node yamlData;
                file >> yamlData;
                
                clear();
                
                // Parse YAML node recursively
                if (yamlData.IsMap()) {
                    for (const auto& yamlEntry : yamlData) {
                        std::string key = yamlEntry.first.as<std::string>();
                        
                        ConfigValue value;
                        if (yamlEntry.second.IsScalar()) {
                            std::string strValue = yamlEntry.second.as<std::string>();
                            // Try to infer type
                            if (strValue == "true" || strValue == "false") {
                                value = (strValue == "true");
                            } else {
                                try {
                                    size_t pos;
                                    int intVal = std::stoi(strValue, &pos);
                                    if (pos == strValue.size()) {
                                        value = intVal;
                                    } else {
                                        double doubleVal = std::stod(strValue, &pos);
                                        if (pos == strValue.size()) {
                                            value = doubleVal;
                                        } else {
                                            value = strValue;
                                        }
                                    }
                                } catch (...) {
                                    value = strValue;
                                }
                            }
                        } else if (yamlEntry.second.IsSequence()) {
                            // Handle sequences (arrays)
                            const auto& yamlArray = yamlEntry.second;
                            if (!yamlArray.empty()) {
                                // Determine array type
                                if (yamlArray[0].IsScalar()) {
                                    std::string firstVal = yamlArray[0].as<std::string>();
                                    if (firstVal == "true" || firstVal == "false") {
                                        std::vector<bool> boolArray;
                                        for (const auto& item : yamlArray) {
                                            boolArray.push_back(item.as<bool>());
                                        }
                                        value = boolArray;
                                    } else {
                                        try {
                                            std::vector<int> intArray;
                                            for (const auto& item : yamlArray) {
                                                intArray.push_back(item.as<int>());
                                            }
                                            value = intArray;
                                        } catch (...) {
                                            std::vector<std::string> strArray;
                                            for (const auto& item : yamlArray) {
                                                strArray.push_back(item.as<std::string>());
                                            }
                                            value = strArray;
                                        }
                                    }
                                }
                            }
                        }
                        
                        set(key, value, ConfigSource::File);
                    }
                }
                
                return true;
            } catch (const YAML::Exception& e) {
                throw ConfigParseException("Failed to parse YAML file: " + std::string(e.what()));
            }
        } else {
            // Fall back to simple key=value format
            file.clear();
            file.seekg(0, std::ios::beg);
            
            return loadFromFileSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        throw ConfigParseException("Error loading config file " + filepath + ": " + std::string(e.what()));
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

bool Config::loadFromFileSimpleFormat(const std::string& filepath) {
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
