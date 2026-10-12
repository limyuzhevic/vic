#include "ConfigSource.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <algorithm>

namespace nlm {

// JSONConfigSource implementation
JSONConfigSource::JSONConfigSource(const std::string& filepath, int priority)
    : filepath(filepath), priority(priority) {}

bool JSONConfigSource::load(Config& config) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json = nlohmann::json::parse(file);
        
        // Load configuration from JSON
        for (const auto& item : json.items()) {
            const std::string& key = item.key();
            const nlohmann::json& value = item.value();
            
            // Convert JSON value to ConfigValue
            if (value.is_number_integer()) {
                config.set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                config.set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                config.set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_string()) {
                config.set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_array()) {
                // Handle arrays
                if (!value.empty()) {
                    if (value[0].is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : value) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        config.set(key, intArray, ConfigSource::File);
                    } else if (value[0].is_number_float()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : value) {
                            doubleArray.push_back(item.get<double>());
                        }
                        config.set(key, doubleArray, ConfigSource::File);
                    } else if (value[0].is_string()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : value) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        config.set(key, stringArray, ConfigSource::File);
                    }
                }
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
    
    return true;
}

bool JSONConfigSource::save(const Config& config) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    
    // Save configuration to JSON
    for (const auto& entry : config.getKeys()) {
        std::optional<ConfigValue> valueOpt = config.get<ConfigValue>(entry);
        if (valueOpt) {
            const ConfigValue& value = *valueOpt;
            
            // Convert ConfigValue to JSON based on type
            if (const int64_t* intVal = std::get_if<int64_t>(&value)) {
                json[entry] = *intVal;
            } else if (const double* doubleVal = std::get_if<double>(&value)) {
                json[entry] = *doubleVal;
            } else if (const bool* boolVal = std::get_if<bool>(&value)) {
                json[entry] = *boolVal;
            } else if (const std::string* stringVal = std::get_if<std::string>(&value)) {
                json[entry] = *stringVal;
            } else if (const std::vector<int64_t>* intArrayVal = std::get_if<std::vector<int64_t>>(&value)) {
                json[entry] = *intArrayVal;
            } else if (const std::vector<double>* doubleArrayVal = std::get_if<std::vector<double>>(&value)) {
                json[entry] = *doubleArrayVal;
            } else if (const std::vector<std::string>* stringArrayVal = std::get_if<std::vector<std::string>>(&value)) {
                json[entry] = *stringArrayVal;
            }
        }
    }
    
    file << json.dump(2);
    return true;
}

// YAMLConfigSource implementation
YAMLConfigSource::YAMLConfigSource(const std::string& filepath, int priority)
    : filepath(filepath), priority(priority) {}

bool YAMLConfigSource::load(Config& config) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // For YAML, we'd use a YAML library like yaml-cpp
        // For now, we'll use the same approach as JSON
        nlohmann::json json = nlohmann::json::parse(file);
        
        // Load configuration from YAML (using JSON as fallback)
        for (const auto& item : json.items()) {
            const std::string& key = item.key();
            const nlohmann::json& value = item.value();
            
            if (value.is_number_integer()) {
                config.set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_float()) {
                config.set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                config.set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_string()) {
                config.set(key, value.get<std::string>(), ConfigSource::File);
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
    
    return true;
}

bool YAMLConfigSource::save(const Config& config) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    
    for (const auto& entry : config.getKeys()) {
        std::optional<ConfigValue> valueOpt = config.get<ConfigValue>(entry);
        if (valueOpt) {
            const ConfigValue& value = *valueOpt;
            
            if (const int64_t* intVal = std::get_if<int64_t>(&value)) {
                json[entry] = *intVal;
            } else if (const double* doubleVal = std::get_if<double>(&value)) {
                json[entry] = *doubleVal;
            } else if (const bool* boolVal = std::get_if<bool>(&value)) {
                json[entry] = *boolVal;
            } else if (const std::string* stringVal = std::get_if<std::string>(&value)) {
                json[entry] = *stringVal;
            }
        }
    }
    
    // Save as YAML (using nlohmann JSON as YAML for now)
    file << json.dump(2);
    return true;
}

// INIConfigSource implementation
INIConfigSource::INIConfigSource(const std::string& filepath, int priority)
    : filepath(filepath), priority(priority) {}

bool INIConfigSource::load(Config& config) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = Config::trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config.set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool INIConfigSource::save(const Config& config) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : config.getKeys()) {
        std::optional<ConfigValue> valueOpt = config.get<ConfigValue>(entry);
        if (valueOpt) {
            const ConfigValue& value = *valueOpt;
            
            file << entry << " = ";
            
            if (const int64_t* intVal = std::get_if<int64_t>(&value)) {
                file << *intVal;
            } else if (const double* doubleVal = std::get_if<double>(&value)) {
                file << *doubleVal;
            } else if (const bool* boolVal = std::get_if<bool>(&value)) {
                file << (*boolVal ? "true" : "false");
            } else if (const std::string* stringVal = std::get_if<std::string>(&value)) {
                file << "\"" << *stringVal << "\"";
            }
            
            file << "\n";
        }
    }
    
    return true;
}

// EnvironmentConfigSource implementation
EnvironmentConfigSource::EnvironmentConfigSource(int priority) : priority(priority) {}

bool EnvironmentConfigSource::load(Config& config) const {
    for (const auto& mapping : mappings) {
        const char* envValue = std::getenv(mapping.first.c_str());
        if (envValue != nullptr) {
            std::string value(envValue);
            config.set(mapping.second, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool EnvironmentConfigSource::save(const Config& config) const {
    // Environment variables are read-only, cannot be saved
    return false;
}

void EnvironmentConfigSource::addMapping(const std::string& envVar, const std::string& configKey) {
    mappings[envVar] = configKey;
}

void EnvironmentConfigSource::removeMapping(const std::string& envVar) {
    mappings.erase(envVar);
}

// CommandLineConfigSource implementation
CommandLineConfigSource::CommandLineConfigSource(int priority) : priority(priority) {}

bool CommandLineConfigSource::load(Config& config) const {
    // This would typically be called during initialization
    // For now, return true (actual loading happens during initialization)
    return true;
}

bool CommandLineConfigSource::save(const Config& config) const {
    // Command line arguments are not saved
    return false;
}

bool CommandLineConfigSource::parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                // Note: This would need access to the actual config manager
                // For now, this is a placeholder
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            // Note: This would need access to the actual config manager
            // For now, this is a placeholder
        }
    }
    
    return true;
}

// RegistryConfigSource implementation
RegistryConfigSource::RegistryConfigSource(const std::string& registryPath, int priority)
    : registryPath(registryPath), priority(priority) {}

bool RegistryConfigSource::load(Config& config) const {
    // Registry loading implementation
    // This would connect to external storage like etcd, consul, etc.
    return false;
}

bool RegistryConfigSource::save(const Config& config) const {
    // Registry saving implementation
    return false;
}

} // namespace nlm