#pragma once

#include <string>
#include <memory>
#include <optional>
#include <unordered_map>
#include <typeindex>
#include <any>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include <functional>
#include <variant>
#include <fstream>

// Optional JSON support - define NLM_ENABLE_JSON to enable
#ifdef NLM_ENABLE_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#define NLM_HAS_JSON 1
#else
#define NLM_HAS_JSON 0
#endif

namespace nlm {

// Type-safe configuration system with nested structure support
class TypedConfiguration {
public:
    // Basic configuration value types
    using IntConfig = int64_t;
    using FloatConfig = double;
    using StringConfig = std::string;
    using BoolConfig = bool;
    using VectorIntConfig = std::vector<int64_t>;
    using VectorFloatConfig = std::vector<double>;
    using VectorStringConfig = std::vector<std::string>;
    
    // Type alias for all supported types
    using ConfigValue = std::variant<
        IntConfig,
        FloatConfig,
        StringConfig,
        BoolConfig,
        VectorIntConfig,
        VectorFloatConfig,
        VectorStringConfig
    >;
    
    // Configuration metadata for validation and documentation
    struct ConfigEntry {
        std::string key;
        ConfigValue value;
        std::string description;
        bool required = false;
        std::function<bool(const ConfigValue&)> validator;
        
        ConfigEntry() = default;
        ConfigEntry(const std::string& k, const ConfigValue& v, const std::string& desc = "", 
                   bool req = false, std::function<bool(const ConfigValue&)> val = nullptr)
            : key(k), value(v), description(desc), required(req), validator(val) {}
    };
    
    // Nested configuration structure for organized settings
    class ConfigSection {
    public:
        std::string name;
        std::vector<ConfigEntry> entries;
        
        ConfigSection() = default;
        ConfigSection(const std::string& n) : name(n) {}
        
        void addEntry(const std::string& key, const ConfigValue& value, 
                     const std::string& desc = "", bool required = false,
                     std::function<bool(const ConfigValue&)> validator = nullptr) {
            entries.emplace_back(key, value, desc, required, validator);
        }
        
        template<typename T>
        std::optional<T> get(const std::string& key) const {
            auto it = std::find_if(entries.begin(), entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });
            if (it == entries.end()) return std::nullopt;
            
            return convertValue<T>(it->value);
        }
        
        template<typename T>
        T getOr(const std::string& key, const T& defaultValue) const {
            auto val = get<T>(key);
            return val.has_value() ? val.value() : defaultValue;
        }
        
        bool has(const std::string& key) const {
            return std::any_of(entries.begin(), entries.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        }
        
        std::string getDescription(const std::string& key) const {
            auto it = std::find_if(entries.begin(), entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });
            return it != entries.end() ? it->description : "";
        }
        
        bool validate() const {
            for (const auto& entry : entries) {
                if (entry.required) {
                    if (!has(entry.key) || !isValid(entry.key)) {
                        return false;
                    }
                }
                if (entry.validator && has(entry.key)) {
                    if (!entry.validator(getValue(entry.key))) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        bool isValid(const std::string& key) const {
            auto it = std::find_if(entries.begin(), entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });
            if (it == entries.end()) return false;
            return true;
        }
        
        const ConfigValue& getValue(const std::string& key) const {
            auto it = std::find_if(entries.begin(), entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != entries.end()) {
                return it->value;
            }
            throw std::runtime_error("Key not found: " + key);
        }
        
        void set(const std::string& key, const ConfigValue& value) {
            auto it = std::find_if(entries.begin(), entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != entries.end()) {
                it->value = value;
            }
        }
        
        std::vector<std::string> getKeys() const {
            std::vector<std::string> keys;
            keys.reserve(entries.size());
            for (const auto& entry : entries) {
                keys.push_back(entry.key);
            }
            return keys;
        }
    };
    
    TypedConfiguration() = default;
    
    // Add a section to the configuration
    void addSection(const std::string& sectionName) {
        sections[sectionName] = ConfigSection(sectionName);
    }
    
    // Get a section
    ConfigSection& getSection(const std::string& sectionName) {
        if (sections.find(sectionName) == sections.end()) {
            addSection(sectionName);
        }
        return sections[sectionName];
    }
    
    const ConfigSection& getSection(const std::string& sectionName) const {
        auto it = sections.find(sectionName);
        if (it != sections.end()) {
            return it->second;
        }
        throw std::runtime_error("Section not found: " + sectionName);
    }
    
    // Type-safe getter methods
    template<typename T>
    std::optional<T> get(const std::string& section, const std::string& key) const {
        try {
            const auto& sec = getSection(section);
            return sec.get<T>(key);
        } catch (const std::runtime_error&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    T getOr(const std::string& section, const std::string& key, const T& defaultValue) const {
        auto val = get<T>(section, key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    bool has(const std::string& section, const std::string& key) const {
        return sections.find(section) != sections.end() && getSection(section).has(key);
    }
    
    // Set values with type safety
    void set(const std::string& section, const std::string& key, IntConfig value, 
             const std::string& desc = "", bool required = false) {
        getSection(section).addEntry(key, value, desc, required);
    }
    
    void set(const std::string& section, const std::string& key, FloatConfig value,
             const std::string& desc = "", bool required = false) {
        getSection(section).addEntry(key, value, desc, required);
    }
    
    void set(const std::string& section, const std::string& key, const StringConfig& value,
             const std::string& desc = "", bool required = false) {
        getSection(section).addEntry(key, value, desc, required);
    }
    
    void set(const std::string& section, const std::string& key, BoolConfig value,
             const std::string& desc = "", bool required = false) {
        getSection(section).addEntry(key, value, desc, required);
    }
    
    void set(const std::string& section, const std::string& key, const VectorIntConfig& value,
             const std::string& desc = "", bool required = false) {
        getSection(section).addEntry(key, value, desc, required);
    }
    
    // Load from JSON file (JSON support only if enabled)
#if NLM_HAS_JSON
    bool loadFromJSON(const std::string& filepath);
#endif
    
    // Save to JSON file (JSON support only if enabled)
#if NLM_HAS_JSON
    bool saveToJSON(const std::string& filepath) const;
#endif
    
    // Load from simple key=value file (backward compatibility)
    bool loadFromSimpleFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string section = "general";
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            if (line.find("[") != std::string::npos && line.find("]") != std::string::npos) {
                size_t start = line.find("[") + 1;
                size_t end = line.find("]");
                section = line.substr(start, end - start);
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                setSimpleValue(section, key, value);
            }
        }
        
        return true;
    }
    
    // Validate entire configuration
    bool validate() const {
        bool valid = true;
        for (const auto& [sectionName, section] : sections) {
            if (!section.validate()) {
                valid = false;
            }
        }
        return valid;
    }
    
    // Get configuration summary
    std::string summary() const {
        std::ostringstream oss;
        oss << "TypedConfiguration Summary:" << std::endl;
        
        for (const auto& [sectionName, section] : sections) {
            oss << "[" << sectionName << "]" << std::endl;
            for (const auto& entry : section.entries) {
                oss << "  " << entry.key << " = [";
                printValue(oss, entry.value);
                oss << "]" << std::endl;
                if (!entry.description.empty()) {
                    oss << "    Description: " << entry.description << std::endl;
                }
                if (entry.required) {
                    oss << "    Required: true" << std::endl;
                }
            }
            oss << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear all configuration
    void clear() {
        sections.clear();
    }
    
    // Get all section names
    std::vector<std::string> getSections() const {
        std::vector<std::string> sectionNames;
        sectionNames.reserve(sections.size());
        for (const auto& [name, section] : sections) {
            sectionNames.push_back(name);
        }
        return sectionNames;
    }
    
private:
    std::unordered_map<std::string, ConfigSection> sections;
    
    // Convert ConfigValue to specific type
    template<typename T>
    std::optional<T> convertValue(const ConfigValue& value) const {
        try {
            return std::visit([](auto&& v) -> std::optional<T> {
                using V = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<V, T>) {
                    return v;
                } else if constexpr (std::is_same_v<V, IntConfig> && std::is_same_v<T, FloatConfig>) {
                    return static_cast<T>(v);
                } else if constexpr (std::is_same_v<V, FloatConfig> && std::is_same_v<T, IntConfig>) {
                    return static_cast<T>(v);
                } else {
                    return std::nullopt;
                }
            }, value);
        } catch (...) {
            return std::nullopt;
        }
    }
    
    // Parse JSON into sections
    void parseJSON(const json& data, const std::string& sectionPrefix);
    
    // Set value from simple string (backward compatibility)
    void setSimpleValue(const std::string& section, const std::string& key, const std::string& valueStr);
    
    // Print value to stringstream
    void printValue(std::ostringstream& oss, const ConfigValue& value) const;
    
    static std::string trim(const std::string& str);
};

// JSON implementation functions
inline void TypedConfiguration::parseJSON(const json& data, const std::string& sectionPrefix) {
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string key = sectionPrefix.empty() ? it.key() : sectionPrefix + "." + it.key();
        
        if (it->is_object()) {
            parseJSON(*it, key);
        } else {
            setSimpleValue(sectionPrefix.empty() ? "general" : sectionPrefix, it.key(), *it);
        }
    }
}

inline void TypedConfiguration::setSimpleValue(const std::string& section, const std::string& key, const std::string& valueStr) {
    auto& sectionObj = getSection(section);
    
    // Try to infer type from string
    if (valueStr == "true" || valueStr == "false") {
        sectionObj.addEntry(key, valueStr == "true");
    } else if (valueStr.find('.') != std::string::npos) {
        try {
            double val = std::stod(valueStr);
            sectionObj.addEntry(key, val);
        } catch (...) {
            sectionObj.addEntry(key, valueStr);
        }
    } else {
        try {
            int64_t val = std::stoll(valueStr);
            sectionObj.addEntry(key, val);
        } catch (...) {
            sectionObj.addEntry(key, valueStr);
        }
    }
}

inline void TypedConfiguration::printValue(std::ostringstream& oss, const ConfigValue& value) const {
    std::visit([&oss](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, IntConfig>) {
            oss << v;
        } else if constexpr (std::is_same_v<T, FloatConfig>) {
            oss << v;
        } else if constexpr (std::is_same_v<T, StringConfig>) {
            oss << "\"" << v << "\"";
        } else if constexpr (std::is_same_v<T, BoolConfig>) {
            oss << (v ? "true" : "false");
        } else if constexpr (std::is_same_v<T, VectorIntConfig>) {
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << v[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, VectorFloatConfig>) {
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << v[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, VectorStringConfig>) {
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << v[i] << "\"";
            }
            oss << "]";
        }
    }, value);
}

inline std::string TypedConfiguration::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// JSON implementation functions when JSON is enabled
#if NLM_HAS_JSON

inline bool TypedConfiguration::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json data = json::parse(file);
        clear();
        parseJSON(data, "");
        return true;
    } catch (const json::parse_error& e) {
        return false;
    }
}

inline bool TypedConfiguration::saveToJSON(const std::string& filepath) const {
    json data;
    for (const auto& [sectionName, section] : sections) {
        json sectionData;
        for (const auto& entry : section.entries) {
            sectionData[entry.key] = entry.value;
        }
        data[sectionName] = sectionData;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << data.dump(2); // Pretty print with 2-space indentation
    return true;
}

#endif // NLM_HAS_JSON

} // namespace nlm