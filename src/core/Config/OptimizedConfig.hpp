#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <typeindex>
#include <any>
#include <shared_mutex>

namespace nlm {

// Performance-optimized configuration value
class ConfigValue {
public:
    enum class Type {
        INT,
        FLOAT,
        STRING,
        BOOL,
        VECTOR_INT,
        VECTOR_FLOAT,
        VECTOR_STRING,
        UNKNOWN
    };
    
    ConfigValue() : type(Type::UNKNOWN) {}
    
    template<typename T>
    ConfigValue(T value) : type(getType<T>()) {
        setValue<T>(value);
    }
    
    Type getType() const { return type; }
    
    template<typename T>
    bool get(T& value) const {
        if (type == getType<T>()) {
            value = std::any_cast<T>(storage);
            return true;
        }
        return false;
    }
    
    template<typename T>
    bool set(const T& value) {
        if (type == getType<T>() || type == Type::UNKNOWN) {
            setValue<T>(value);
            type = getType<T>();
            return true;
        }
        return false;
    }
    
    // Validation callback
    void setValidator(std::function<bool(const ConfigValue&)> validator) {
        validatorFunc = validator;
    }
    
    bool validate() const {
        return validatorFunc ? validatorFunc(*this) : true;
    }
    
    // Serialization
    std::string serialize() const {
        std::stringstream ss;
        switch (type) {
            case Type::INT:
                ss << std::any_cast<int>(storage);
                break;
            case Type::FLOAT:
                ss << std::any_cast<float>(storage);
                break;
            case Type::STRING:
                ss << std::any_cast<std::string>(storage);
                break;
            case Type::BOOL:
                ss << std::any_cast<bool>(storage);
                break;
            case Type::VECTOR_INT:
                {
                    auto& vec = std::any_cast<std::vector<int>>(storage);
                    ss << "[";
                    for (size_t i = 0; i < vec.size(); ++i) {
                        ss << vec[i];
                        if (i < vec.size() - 1) ss << ",";
                    }
                    ss << "]";
                }
                break;
            case Type::VECTOR_FLOAT:
                {
                    auto& vec = std::any_cast<std::vector<float>>(storage);
                    ss << "[";
                    for (size_t i = 0; i < vec.size(); ++i) {
                        ss << vec[i];
                        if (i < vec.size() - 1) ss << ",";
                    }
                    ss << "]";
                }
                break;
            case Type::VECTOR_STRING:
                {
                    auto& vec = std::any_cast<std::vector<std::string>>(storage);
                    ss << "[";
                    for (size_t i = 0; i < vec.size(); ++i) {
                        ss << '"' << vec[i] << '"';
                        if (i < vec.size() - 1) ss << ",";
                    }
                    ss << "]";
                }
                break;
            default:
                ss << "";
        }
        return ss.str();
    }
    
    // Deserialization
    bool deserialize(const std::string& data) {
        std::stringstream ss(data);
        char bracket;
        switch (type) {
            case Type::INT: {
                int val;
                if (ss >> val) {
                    setValue<int>(val);
                    return true;
                }
            } break;
            case Type::FLOAT: {
                float val;
                if (ss >> val) {
                    setValue<float>(val);
                    return true;
                }
            } break;
            case Type::STRING: {
                std::string val;
                if (ss >> val) {
                    setValue<std::string>(val);
                    return true;
                }
            } break;
            case Type::BOOL: {
                bool val;
                if (ss >> std::boolalpha >> val) {
                    setValue<bool>(val);
                    return true;
                }
            } break;
            case Type::VECTOR_INT: {
                std::vector<int> vec;
                char c;
                if (ss >> c && c == '[') {
                    int val;
                    while (ss >> val) {
                        vec.push_back(val);
                        ss >> c;
                        if (c != ',') break;
                    }
                    if (ss >> c && c == ']') {
                        setValue<std::vector<int>>(vec);
                        return true;
                    }
                }
            } break;
            case Type::VECTOR_FLOAT: {
                std::vector<float> vec;
                char c;
                if (ss >> c && c == '[') {
                    float val;
                    while (ss >> val) {
                        vec.push_back(val);
                        ss >> c;
                        if (c != ',') break;
                    }
                    if (ss >> c && c == ']') {
                        setValue<std::vector<float>>(vec);
                        return true;
                    }
                }
            } break;
            case Type::VECTOR_STRING: {
                std::vector<std::string> vec;
                char c;
                if (ss >> c && c == '[') {
                    std::string val;
                    while (ss >> c && c == '"') {
                        ss >> val;
                        vec.push_back(val);
                        ss >> c;
                        if (c != ',') break;
                    }
                    if (ss >> c && c == ']') {
                        setValue<std::vector<std::string>>(vec);
                        return true;
                    }
                }
            } break;
        }
        return false;
    }
    
private:
    std::any storage;
    Type type;
    std::function<bool(const ConfigValue&)> validatorFunc;
    
    template<typename T>
    Type getType() const {
        if (std::is_same<T, int>::value) return Type::INT;
        if (std::is_same<T, float>::value) return Type::FLOAT;
        if (std::is_same<T, double>::value) return Type::FLOAT;
        if (std::is_same<T, std::string>::value) return Type::STRING;
        if (std::is_same<T, bool>::value) return Type::BOOL;
        if (std::is_same<T, std::vector<int>>::value) return Type::VECTOR_INT;
        if (std::is_same<T, std::vector<float>>::value) return Type::VECTOR_FLOAT;
        if (std::is_same<T, std::vector<std::string>>::value) return Type::VECTOR_STRING;
        return Type::UNKNOWN;
    }
    
    template<typename T>
    void setValue(const T& value) {
        storage = value;
    }
};

// Performance-optimized configuration system with caching
class Config {
public:
    Config() {
        initDefaultValues();
        cacheEnabled = true;
        cacheExpiryMs = 5000; // 5 seconds
    }
    
    ~Config() = default;
    
    // Get configuration value
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const {
        std::shared_lock<std::shared_mutex> lock(configMutex);
        
        // Check cache first
        if (cacheEnabled) {
            auto cacheIt = cache.find(key);
            if (cacheIt != cache.end() && !isCacheExpired(cacheIt->second.timestamp)) {
                // Try to get value from cache
                auto valueIt = cachedValues.find(key);
                if (valueIt != cachedValues.end()) {
                    T result;
                    if (valueIt->second.get(result)) {
                        return result;
                    }
                }
            }
        }
        
        // Get value from main config
        auto it = values.find(key);
        if (it != values.end()) {
            T result;
            if (it->second.get(result)) {
                // Cache the result
                if (cacheEnabled) {
                    cacheValue(key, result);
                }
                return result;
            }
        }
        
        return defaultValue;
    }
    
    // Set configuration value
    template<typename T>
    bool set(const std::string& key, const T& value) {
        std::unique_lock<std::shared_mutex> lock(configMutex);
        
        // Remove from cache
        cache.erase(key);
        cachedValues.erase(key);
        
        // Set new value
        ConfigValue configValue(value);
        auto it = values.find(key);
        if (it != values.end()) {
            it->second = configValue;
        } else {
            values[key] = configValue;
        }
        
        return true;
    }
    
    // Set default value for a key
    template<typename T>
    void setDefault(const std::string& key, const T& value) {
        defaults[key] = ConfigValue(value);
        // Set initial value if not already set
        if (values.find(key) == values.end()) {
            values[key] = defaults[key];
        }
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::shared_lock<std::shared_mutex> lock(configMutex);
        std::vector<std::string> keys;
        keys.reserve(values.size());
        for (const auto& pair : values) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string currentSection;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Section header
            if (line[0] == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Parse key-value pair
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Remove whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Set value
            setConfigValue(key, value, currentSection);
        }
        
        file.close();
        return true;
    }
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Save sections
        for (const auto& sectionPair : sections) {
            file << "[" << sectionPair.first << "]" << std::endl;
            
            // Save values in this section
            for (const auto& pair : values) {
                if (getSection(pair.first) == sectionPair.first) {
                    std::string value = configValueToString(pair.second);
                    file << pair.first << " = " << value << std::endl;
                }
            }
            file << std::endl;
        }
        
        file.close();n        return true;
    }
    
    // Enable/disable caching
    void setCacheEnabled(bool enabled) {
        std::unique_lock<std::shared_mutex> lock(configMutex);
        cacheEnabled = enabled;
        if (!enabled) {
            clearCache();
        }
    }
    
    // Set cache expiry time in milliseconds
    void setCacheExpiryMs(long long expiry) {
        std::unique_lock<std::shared_mutex> lock(configMutex);
        cacheExpiryMs = expiry;
    }
    
    // Clear configuration cache
    void clearCache() {
        std::unique_lock<std::shared_mutex> lock(configMutex);
        cache.clear();
        cachedValues.clear();
    }
    
    // Validate configuration
    bool validate() const {
        std::shared_lock<std::shared_mutex> lock(configMutex);
        
        for (const auto& pair : values) {
            if (!pair.second.validate()) {
                return false;
            }
        }
        
        return true;
    }
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalValues;
        size_t cachedValues;
        double cacheHitRate;
        double validationTime;
        size_t memoryUsage;
    };
    
    ConfigStats getStats() const {
        std::shared_lock<std::shared_mutex> lock(configMutex);
        
        ConfigStats stats;
        stats.totalValues = values.size();
        stats.cachedValues = cachedValues.size();
        stats.cacheHitRate = 0.0; // Would need to track hits/misses
        stats.validationTime = 0.0; // Would need to measure
        stats.memoryUsage = 0; // Would need to measure
        
        return stats;
    }
    
private:
    // Configuration value structure
    struct CachedConfigValue {
        ConfigValue value;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::map<std::string, ConfigValue> values;
    std::map<std::string, ConfigValue> defaults;
    std::map<std::string, std::string> sections;
    
    // Cache for performance optimization
    bool cacheEnabled;
    long long cacheExpiryMs;
    std::unordered_map<std::string, CachedConfigValue> cache;
    std::unordered_map<std::string, ConfigValue> cachedValues;
    
    mutable std::shared_mutex configMutex;
    
    // Get section for a key
    std::string getSection(const std::string& key) const {
        auto it = sections.find(key);
        return it != sections.end() ? it->second : "";
    }
    
    // Convert config value to string
    std::string configValueToString(const ConfigValue& value) const {
        return value.serialize();
    }
    
    // Convert string to config value
    void setConfigValue(const std::string& key, const std::string& value, const std::string& section) {
        ConfigValue configValue;
        
        // Determine type based on content
        if (value.find('.') != std::string::npos) {
            // Could be float or double
            try {
                float floatVal = std::stof(value);
                configValue.set<float>(floatVal);
            } catch (...) {
                configValue.set<std::string>(value);
            }
        } else if (value == "true" || value == "false") {
            bool boolVal = (value == "true");
            configValue.set<bool>(boolVal);
        } else {
            // Try integer
            try {
                int intVal = std::stoi(value);
                configValue.set<int>(intVal);
            } catch (...) {
                configValue.set<std::string>(value);
            }
        }
        
        if (!section.empty()) {
            sections[key] = section;
        }
        
        values[key] = configValue;
    }
    
    // Check if cache entry is expired
    bool isCacheExpired(const CachedConfigValue& cacheEntry) const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - cacheEntry.timestamp);
        return duration.count() > cacheExpiryMs;
    }
    
    // Cache a value
    void cacheValue(const std::string& key, const ConfigValue& value) {
        CachedConfigValue cacheEntry;
        cacheEntry.value = value;
        cacheEntry.timestamp = std::chrono::system_clock::now();
        
        cache[key] = cacheEntry;
        cachedValues[key] = value;
    }
    
    // Initialize default configuration values
    void initDefaultValues() {
        setDefault<int>("simulation_steps", 1000);
        setDefault<float>("timestep", 0.001f);
        setDefault<float>("neuron_threshold", -55.0f);
        setDefault<float>("neuron_leak_conductance", 25.0f);
        setDefault<float>("neuron_time_constant", 20.0f);
        setDefault<int>("random_seed", 42);
        setDefault<bool>("enable_plasticity", true);
        setDefault<bool>("enable_neuromodulation", true);
        setDefault<std::string>("log_level", "info");
        setDefault<std::vector<int>("neuron_population_sizes", std::vector<int>{100, 100, 50});
        setDefault<std::vector<float>("synapse_weights", std::vector<float>{0.1f, 0.5f, 1.0f});
        setDefault<std::vector<std::string>("neuron_types", std::vector<std::string>{"excitatory", "inhibitory", "modulatory"});
    }
};

} // namespace nlm
