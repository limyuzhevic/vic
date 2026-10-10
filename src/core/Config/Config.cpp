// Configuration system implementation for NLM
#include "Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace nlm {

Config::Config() {
    // Set default configuration values
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    set("log_level", "INFO", ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Parse value based on type
        if (value.find('.') != std::string::npos) {
            try {
                float floatValue = std::stof(value);
                set(key, floatValue, ConfigSource::File);
            } catch (...) {
                // Try as int
                try {
                    int64_t intValue = std::stoll(value);
                    set(key, intValue, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            }
        } else {
            try {
                int64_t intValue = std::stoll(value);
                set(key, intValue, ConfigSource::File);
            } catch (...) {
                try {
                    float floatValue = std::stof(value);
                    set(key, floatValue, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            }
        }
    }
    
    return true;
}

void Config::loadFromArgs(int argc, char** argv) {
    parseArgs(argc, argv);
}

template<typename T>
void Config::set(const std::string& key, T value, ConfigSource source) {
    setRawValue(key, value, source);
}

template<typename T>
T Config::getOr(const std::string& key, T defaultValue) const {
    void* rawValue = getRawValue(key);
    if (rawValue == nullptr) {
        return defaultValue;
    }
    
    // Try to cast to type T
    if constexpr (std::is_same_v<T, std::string>) {
        return *static_cast<std::string*>(rawValue);
    } else if constexpr (std::is_same_v<T, double>) {
        return static_cast<double>(*static_cast<float*>(rawValue));
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return static_cast<int64_t>(*static_cast<float*>(rawValue));
    } else if constexpr (std::is_same_v<T, float>) {
        return *static_cast<float*>(rawValue);
    } else if constexpr (std::is_same_v<T, bool>) {
        return *static_cast<bool*>(rawValue);
    } else {
        return defaultValue;
    }
}

template<typename T>
T Config::get(const std::string& key) const {
    void* rawValue = getRawValue(key);
    if (rawValue == nullptr) {
        throw std::runtime_error("Configuration key not found: " + key);
    }
    
    // Try to cast to type T
    if constexpr (std::is_same_v<T, std::string>) {
        return *static_cast<std::string*>(rawValue);
    } else if constexpr (std::is_same_v<T, double>) {
        return static_cast<double>(*static_cast<float*>(rawValue));
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return static_cast<int64_t>(*static_cast<float*>(rawValue));
    } else if constexpr (std::is_same_v<T, float>) {
        return *static_cast<float*>(rawValue);
    } else if constexpr (std::is_same_v<T, bool>) {
        return *static_cast<bool*>(rawValue);
    } else {
        return T();
    }
}

bool Config::hasKey(const std::string& key) const {
    return getRawValue(key) != nullptr;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    
    // Collect keys from all maps
    for (const auto& pair : stringValues_) {
        keys.push_back(pair.first);
    }
    for (const auto& pair : doubleValues_) {
        keys.push_back(pair.first);
    }
    for (const auto& pair : intValues_) {
        keys.push_back(pair.first);
    }
    for (const auto& pair : floatValues_) {
        keys.push_back(pair.first);
    }
    for (const auto& pair : boolValues_) {
        keys.push_back(pair.first);
    }
    
    // Remove duplicates and sort
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    
    return keys;
}

void Config::printConfig() const {
    NLM_LOG_INFO("Configuration:");
    for (const auto& key : getKeys()) {
        if (hasKey(key)) {
            if constexpr (std::is_same_v<decltype(stringValues_), std::map<std::string, std::string>>) {
                NLM_LOG_INFO("  " + key + " = " + getOr<std::string>(key, ""));
            } else if constexpr (std::is_same_v<decltype(doubleValues_), std::map<std::string, double>>) {
                NLM_LOG_INFO("  " + key + " = " + std::to_string(getOr<double>(key, 0.0)));
            } else if constexpr (std::is_same_v<decltype(intValues_), std::map<std::string, int64_t>>) {
                NLM_LOG_INFO("  " + key + " = " + std::to_string(getOr<int64_t>(key, 0)));
            } else if constexpr (std::is_same_v<decltype(floatValues_), std::map<std::string, float>>) {
                NLM_LOG_INFO("  " + key + " = " + std::to_string(getOr<float>(key, 0.0f)));
            } else if constexpr (std::is_same_v<decltype(boolValues_), std::map<std::string, bool>>) {
                NLM_LOG_INFO("  " + key + " = " + (getOr<bool>(key, false) ? "true" : "false"));
            }
        }
    }
}

bool Config::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& key : getKeys()) {
        if (hasKey(key)) {
            if constexpr (std::is_same_v<decltype(stringValues_), std::map<std::string, std::string>>) {
                file << key << " = " << getOr<std::string>(key, "") << std::endl;
            } else if constexpr (std::is_same_v<decltype(doubleValues_), std::map<std::string, double>>) {
                file << key << " = " << getOr<double>(key, 0.0) << std::endl;
            } else if constexpr (std::is_same_v<decltype(intValues_), std::map<std::string, int64_t>>) {
                file << key << " = " << getOr<int64_t>(key, 0) << std::endl;
            } else if constexpr (std::is_same_v<decltype(floatValues_), std::map<std::string, float>>) {
                file << key << " = " << getOr<float>(key, 0.0f) << std::endl;
            } else if constexpr (std::is_same_v<decltype(boolValues_), std::map<std::string, bool>>) {
                file << key << " = " << (getOr<bool>(key, false) ? "true" : "false") << std::endl;
            }
        }
    }
    
    return true;
}

void* Config::getRawValue(const std::string& key) const {
    // Check all possible value stores
    auto it = stringValues_.find(key);
    if (it != stringValues_.end()) return const_cast<void*>(static_cast<const void*>(&it->second));
    
    it = doubleValues_.find(key);
    if (it != doubleValues_.end()) {
        static float temp = it->second;
        return const_cast<void*>(static_cast<const void*>(&temp));
    }
    
    it = intValues_.find(key);
    if (it != intValues_.end()) {
        static float temp = static_cast<float>(it->second);
        return const_cast<void*>(static_cast<const void*>(&temp));
    }
    
    it = floatValues_.find(key);
    if (it != floatValues_.end()) return const_cast<void*>(static_cast<const void*>(&it->second));
    
    it = boolValues_.find(key);
    if (it != boolValues_.end()) return const_cast<void*>(static_cast<const void*>(&it->second));
    
    return nullptr;
}

template<typename T>
void Config::setRawValue(const std::string& key, T value, ConfigSource source) {
    if constexpr (std::is_same_v<T, std::string>) {
        stringValues_[key] = value;
    } else if constexpr (std::is_same_v<T, double>) {
        doubleValues_[key] = static_cast<double>(value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        intValues_[key] = static_cast<int64_t>(value);
    } else if constexpr (std::is_same_v<T, float>) {
        floatValues_[key] = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        boolValues_[key] = value;
    }
}

void Config::parseArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 7) == "--config") {
            std::string configFile = parseConfigArg(arg);
            if (!configFile.empty()) {
                loadFromFile(configFile);
            }
        } else if (arg.substr(0, 10) == "--random_seed") {
            std::string value = parseConfigArg(arg);
            if (!value.empty()) {
                try {
                    int64_t seed = std::stoll(value);
                    set("random_seed", seed, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid random_seed value: " + value);
                }
            }
        } else if (arg.substr(0, 15) == "--neuron_count") {
            std::string value = parseConfigArg(arg);
            if (!value.empty()) {
                try {
                    int64_t count = std::stoll(value);
                    set("neuron_count", count, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid neuron_count value: " + value);
                }
            }
        } else if (arg.substr(0, 16) == "--simulation_timestep") {
            std::string value = parseConfigArg(arg);
            if (!value.empty()) {
                try {
                    float timestep = std::stof(value);
                    set("simulation_timestep", timestep, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid simulation_timestep value: " + value);
                }
            }
        } else if (arg.substr(0, 20) == "--connection_probability") {
            std::string value = parseConfigArg(arg);
            if (!value.empty()) {
                try {
                    float prob = std::stof(value);
                    set("connection_probability", prob, ConfigSource::CommandLine);
                } catch (...) {
                    NLM_LOG_WARNING("Invalid connection_probability value: " + value);
                }
            }
        }
    }
}

std::string Config::parseConfigArg(const std::string& arg) const {
    size_t equalPos = arg.find('=');
    if (equalPos != std::string::npos) {
        return arg.substr(equalPos + 1);
    } else if (&arg != nullptr) {
        return arg.substr(7); // Skip "--" prefix
    }
    return "";
}

} // namespace nlm