#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
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
    // Auto-detect file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJsonFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYamlFile(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

void Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
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
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing config
        clear();
        
        // Convert JSON to Config entries
        for (const auto& [key, value] : jsonData.items()) {
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
                // Try to infer array type
                const auto& arr = value;
                if (!arr.empty() && arr[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : arr) {
                        intArray.push_back(item.get<int>());
                    }
                    configValue = intArray;
                } else if (!arr.empty() && arr[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : arr) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                } else if (!arr.empty() && arr[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : arr) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    configValue = stringArray;
                } else {
                    // Default to double array
                    std::vector<double> doubleArray;
                    for (const auto& item : arr) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                }
            } else {
                // For other types, convert to string
                configValue = value.dump();
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error and return false
        return false;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing config
        clear();
        
        // Convert YAML to Config entries
        if (yamlData.IsMap()) {
            for (const auto& pair : yamlData) {
                std::string key = pair.first.as<std::string>();
                YAML::Node value = pair.second;
                
                ConfigValue configValue;
                
                if (value.IsScalar()) {
                    // Try to infer scalar type
                    if (value.IsInteger()) {
                        configValue = value.as<int64_t>();
                    } else if (value.IsFloat()) {
                        configValue = value.as<double>();
                    } else if (value.IsBoolean()) {
                        configValue = value.as<bool>();
                    } else {
                        configValue = value.as<std::string>();
                    }
                } else if (value.IsSequence()) {
                    // Try to infer sequence type
                    const auto& seq = value;
                    if (!seq.empty() && seq[0].IsInteger()) {
                        std::vector<int> intArray;
                        for (const auto& item : seq) {
                            intArray.push_back(item.as<int>());
                        }
                        configValue = intArray;
                    } else if (!seq.empty() && seq[0].IsFloat()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : seq) {
                            doubleArray.push_back(item.as<double>());
                        }
                        configValue = doubleArray;
                    } else if (!seq.empty() && seq[0].IsScalar()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : seq) {
                            stringArray.push_back(item.as<std::string>());
                        }
                        configValue = stringArray;
                    } else {
                        // Default to double array
                        std::vector<double> doubleArray;
                        for (const auto& item : seq) {
                            doubleArray.push_back(item.as<double>());
                        }
                        configValue = doubleArray;
                    }
                } else {
                    configValue = value.as<std::string>();
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error and return false
        return false;
    }
}
