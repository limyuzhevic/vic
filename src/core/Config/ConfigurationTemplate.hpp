// Enhanced Configuration Template
#pragma once

#include "IConfiguration.hpp"
#include <any>
#include <stdexcept>
#include <sstream>

namespace nlm {

class ConfigurationTemplate : public IConfiguration {
public:
    ConfigurationTemplate() = default;
    ConfigurationTemplate(const std::string& data);
    ~ConfigurationTemplate() override = default;
    
    // Disable copying, enable moving
    ConfigurationTemplate(const ConfigurationTemplate&) = delete;
    ConfigurationTemplate& operator=(const ConfigurationTemplate&) = delete;
    ConfigurationTemplate(ConfigurationTemplate&&) noexcept = default;
    ConfigurationTemplate& operator=(ConfigurationTemplate&&) noexcept = default;
    
    // IConfiguration interface implementation
    void set(const std::string& key, const std::string& value) override;
    std::optional<std::string> getString(const std::string& key) const override;
    std::optional<int> getInt(const std::string& key) const override;
    std::optional<double> getDouble(const std::string& key) const override;
    std::optional<bool> getBool(const std::string& key) const override;
    
    std::string getString(const std::string& key, const std::string& defaultValue) const override;
    int getInt(const std::string& key, int defaultValue) const override;
    double getDouble(const std::string& key, double defaultValue) const override;
    bool getBool(const std::string& key, bool defaultValue) const override;
    
    bool has(const std::string& key) const override;
    void remove(const std::string& key) override;
    std::vector<std::string> getKeys() const override;
    void clear() override;
    
    Source getSource(const std::string& key) const override;
    std::string getDescription(const std::string& key) const override;
    
    std::shared_ptr<IConfiguration> getSection(const std::string& section) const override;
    void setSection(const std::string& section, const std::shared_ptr<IConfiguration>& config) override;
    
    void addChangeCallback(const ConfigurationChangeCallback& callback) override;
    void removeChangeCallback(const ConfigurationChangeCallback& callback) override;
    
    std::string toString() const override;
    bool fromString(const std::string& data) override;
    
    bool equals(const IConfiguration& other) const override;
    
    size_t getSize() const override;
    double getMemoryUsage() const override;
    
    // Extended functionality
    template<typename T>
    void setValue(const std::string& key, const T& value, Source source = Source::Runtime, 
                  const std::string& description = "") {
        std::string stringValue;
        if constexpr (std::is_same_v<T, std::string>) {
            stringValue = value;
        } else if constexpr (std::is_same_v<T, int>) {
            stringValue = std::to_string(value);
        } else if constexpr (std::is_same_v<T, double>) {
            stringValue = std::to_string(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            stringValue = value ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < value.size(); ++i) {
                if (i > 0) ss << ",";
                ss << value[i];
            }
            ss << "]";
            stringValue = ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < value.size(); ++i) {
                if (i > 0) ss << ",";
                ss << value[i];
            }
            ss << "]";
            stringValue = ss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < value.size(); ++i) {
                if (i > 0) ss << ",";
                ss << "\"" << value[i] << "\"";
            }
            ss << "]";
            stringValue = ss.str();
        } else {
            throw std::runtime_error("Unsupported type for configuration value");
        }
        
        set(key, stringValue);
        setSource(key, source);
        setDescription(key, description);
    }
    
    template<typename T>
    std::optional<T> getOptional(const std::string& key) const {
        auto value = getString(key);
        if (!value) return std::nullopt;
        
        if constexpr (std::is_same_v<T, std::string>) {
            return *value;
        } else if constexpr (std::is_same_v<T, int>) {
            try {
                return std::stoi(*value);
            } catch (...) {
                return std::nullopt;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            try {
                return std::stod(*value);
            } catch (...) {
                return std::nullopt;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            std::string lower = *value;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return lower == "true" || lower == "1" || lower == "yes";
        } else {
            throw std::runtime_error("Unsupported type for configuration retrieval");
        }
    }
    
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const {
        auto value = getOptional<T>(key);
        return value ? *value : defaultValue;
    }
    
    // Validation support
    using ValidationRule = std::function<bool(const std::string&, const std::any&)>;
    void addValidationRule(const std::string& key, const ValidationRule& rule);
    bool validate() const;
    
    // Batch operations
    void merge(const IConfiguration& other);
    std::shared_ptr<IConfiguration> clone() const;
    
    // Performance optimization
    void optimizeMemory() { /* Implementation */ }
    void optimizeSpeed() { /* Implementation */ }
    
    // Statistics
    struct Statistics {
        size_t totalKeys = 0;
        size_t stringKeys = 0;
        size_t intKeys = 0;
        size_t doubleKeys = 0;
        size_t boolKeys = 0;
        size_t vectorKeys = 0;
        double memoryUsage = 0.0;
        double averageKeySize = 0.0;
    };
    
    Statistics getStatistics() const;
    
protected:
    // Internal helper methods
    static std::any convertToAny(const std::string& value, const std::type_info& type);
    static std::string convertFromAny(const std::any& value);
    
    void setSource(const std::string& key, Source source);
    void setDescription(const std::string& key, const std::string& description);
    
    std::unordered_map<std::string, std::any> values_;
    std::unordered_map<std::string, Source> sources_;
    std::unordered_map<std::string, std::string> descriptions_;
    std::vector<ConfigurationChangeCallback> changeCallbacks_;
    std::unordered_map<std::string, ValidationRule> validationRules_;
    std::unordered_map<std::string, std::shared_ptr<IConfiguration>> sections_;
    mutable bool isDirty_ = false;
};

} // namespace nlm
