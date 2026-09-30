#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <memory>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <cctype>

// Include utility headers
#include "../core/Utilities/MathUtils.hpp"
#include "../core/Utilities/ValidationUtils.hpp"
#include "../core/Utilities/StringUtils.hpp"
#include "../core/Utilities/ContainerUtils.hpp"
#include "../common/Common.hpp"

namespace nlm {
namespace ConfigImpl {
    
    // Helper functions for Config.cpp
    namespace Helper {
        
        // JSON parsing helpers
        std::string trim(const std::string& str);
        bool isWhitespace(char ch);
        std::string escapeForJSON(const std::string& input);
        std::string unescapeFromJSON(const std::string& input);
        
        // Type conversion helpers
        bool tryConvertToInt(const std::string& str, int& result);
        bool tryConvertToDouble(const std::string& str, double& result);
        bool tryConvertToBool(const std::string& str, bool& result);
        
        // Validation helpers
        bool isValidKey(const std::string& key);
        bool isValidValue(const ConfigValue& value);
        bool isValidSource(ConfigSource source);
        
        // Serialization helpers
        void writeJSONString(std::ostream& os, const std::string& value);
        void writeJSONNumber(std::ostream& os, double value);
        void writeJSONBoolean(std::ostream& os, bool value);
        void writeJSONArray(std::ostream& os, const std::vector<int>& array);
        void writeJSONArray(std::ostream& os, const std::vector<double>& array);
        void writeJSONArray(std::ostream& os, const std::vector<std::string>& array);
        
        // Statistics helpers
        size_t countEntriesBySource(const std::vector<ConfigEntry>& entries, ConfigSource source);
        std::vector<std::string> getAllKeys(const std::vector<ConfigEntry>& entries);
        std::vector<ConfigEntry> findEntriesByPrefix(const std::vector<ConfigEntry>& entries, const std::string& prefix);
        
        // Error handling helpers
        std::string createErrorMessage(const std::string& operation, const std::string& key, const std::string& message);
        void logConfigOperation(const std::string& operation, const std::string& key, const ConfigValue& value);
        
        // Comparison helpers
        bool compareConfigEntries(const ConfigEntry& a, const ConfigEntry& b);
        std::vector<ConfigEntry> findDifferences(const std::vector<ConfigEntry>& a, const std::vector<ConfigEntry>& b);
        
        // Batch processing helpers
        std::vector<std::string> filterKeys(const std::vector<std::string>& keys, const std::vector<std::string>& validKeys);
        std::vector<ConfigEntry> filterEntries(const std::vector<ConfigEntry>& entries, ConfigSource source);
        
    } // namespace Helper
    
    // Core configuration management functions
    void initializeConfigImpl(Config::Impl& impl);
    void clearChangeCallbacks(Config::Impl& impl);
    ConfigEntry* findEntry(Config::Impl& impl, const std::string& key);
    const ConfigEntry* findEntry(const Config::Impl& impl, const std::string& key);
    bool compareValues(const ConfigValue& a, const ConfigValue& b);
    
    // Template helper functions
    template<typename T>
    std::optional<T> convertValue(const ConfigValue& value);
    
    template<typename T>
    bool setValue(Config::Impl& impl, const std::string& key, const T& value, ConfigSource source);
    
    // Validation helpers
    template<typename T>
    std::optional<T> safeGet(const Config::Impl& impl, const std::string& key);
    
    // Utility functions
    std::string generateTimestampedFilename(const std::string& prefix);
    std::vector<std::string> extractKeyParts(const std::string& key);
    std::string joinKeyParts(const std::vector<std::string>& parts);
    
} // namespace ConfigImpl

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<ConfigEntry*>> nestedIndex;
    ConfigSchema schema;
    std::vector<std::function<void(const std::string&, const ConfigValue&)>> changeCallbacks;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema() {}

Config::Config(const ConfigSchema& schema) : pImpl(std::make_unique<Impl>()), schema(schema) {}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

Config::~Config() = default;

// ==================== Enhanced Type Safety ====================

namespace ConfigImpl {
    
    template<typename T>
    std::optional<T> convertValue(const ConfigValue& value) {
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return static_cast<int>(val);
            } else if (std::holds_alternative<std::string>(value)) {
                try {
                    double val = std::stod(std::get<std::string>(value));
                    return static_cast<int>(val);
                } catch (...) {
                    return std::nullopt;
                }
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else if (std::holds_alternative<int>(value)) {
                return static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<std::string>(value)) {
                try {
                    return std::stod(std::get<std::string>(value));
                } catch (...) {
                    return std::nullopt;
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                std::string str = std::get<std::string>(value);
                std::transform(str.begin(), str.end(), str.begin(), ::tolower);
                return (str == "true" || str == "yes" || str == "1");
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            } else if (std::holds_alternative<int>(value)) {
                return std::to_string(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                return std::to_string(std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value) ? "true" : "false";
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(value)) {
                return std::get<std::vector<int>>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                // Parse comma-separated string
                std::string str = std::get<std::string>(value);
                std::vector<int> result;
                std::stringstream ss(str);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    token = Utils::StringUtils::trim(token);
                    if (!token.empty()) {
                        try {
                            result.push_back(std::stoi(token));
                        } catch (...) {
                            // Skip invalid numbers
                        }
                    }
                }
                return result;
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(value)) {
                return std::get<std::vector<double>>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                // Parse comma-separated string
                std::string str = std::get<std::string>(value);
                std::vector<double> result;
                std::stringstream ss(str);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    token = Utils::StringUtils::trim(token);
                    if (!token.empty()) {
                        try {
                            result.push_back(std::stod(token));
                        } catch (...) {
                            // Skip invalid numbers
                        }
                    }
                }
                return result;
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(value)) {
                return std::get<std::vector<std::string>>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                // Parse comma-separated string with optional quotes
                std::string str = std::get<std::string>(value);
                std::vector<std::string> result;
                std::stringstream ss(str);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    token = Utils::StringUtils::trim(token);
                    if (!token.empty()) {
                        // Remove quotes if present
                        if (token.size() >= 2 && 
                            ((token.front() == '"' && token.back() == '"') ||
                             (token.front() == '\'' && token.back() == '\''))) {
                            token = token.substr(1, token.size() - 2);
                        }
                        result.push_back(token);
                    }
                }
                return result;
            }
        }
        return std::nullopt;
    }
    
    template<typename T>
    bool setValue(Config::Impl& impl, const std::string& key, const T& value, ConfigSource source) {
        // Convert T to ConfigValue using nlm::Utils::MathUtils
        ConfigValue configValue;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            configValue = static_cast<int>(value);
        } else if constexpr (std::is_same_v<T, double>) {
            configValue = value;
        } else if constexpr (std::is_same_v<T, bool>) {
            configValue = value;
        } else if constexpr (std::is_same_v<T, std::string>) {
            configValue = value;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            configValue = value;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            configValue = value;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            configValue = value;
        } else {
            return false; // Unsupported type
        }
        
        return setValue(impl, key, configValue, source);
    }
    
    template<typename T>
    std::optional<T> safeGet(const Config::Impl& impl, const std::string& key) {
        const ConfigEntry* entry = findEntry(impl, key);
        if (!entry) {
            return std::nullopt;
        }
        return convertValue<T>(entry->value);
    }
    
} // namespace ConfigImpl

// ==================== Enhanced Type Safety ====================

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return ConfigImpl::safeGet<T>(*pImpl, key);
}

template<typename T>
std::optional<T> Config::get(const std::string& key, bool allowConversion) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) {
        return std::nullopt;
    }
    
    auto result = ConfigImpl::convertValue<T>(entry->value);
    if (!result.has_value() && allowConversion) {
        // Try conversion from string
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<std::string>(entry->value)) {
                try {
                    result = std::stoi(std::get<std::string>(entry->value));
                } catch (...) {
                    result = std::nullopt;
                }
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<std::string>(entry->value)) {
                try {
                    result = std::stod(std::get<std::string>(entry->value));
                } catch (...) {
                    result = std::nullopt;
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<std::string>(entry->value)) {
                std::string str = std::get<std::string>(entry->value);
                std::transform(str.begin(), str.end(), str.begin(), ::tolower);
                result = (str == "true" || str == "yes" || str == "1");
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<int>(entry->value)) {
                result = std::to_string(std::get<int>(entry->value));
            } else if (std::holds_alternative<double>(entry->value)) {
                result = std::to_string(std::get<double>(entry->value));
            } else if (std::holds_alternative<bool>(entry->value)) {
                result = std::get<bool>(entry->value) ? "true" : "false";
            }
        }
    }
    
    return result;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
std::unordered_map<std::string, T> Config::batchGet(const std::vector<std::string>& keys) const {
    std::unordered_map<std::string, T> results;
    for (const auto& key : keys) {
        auto val = get<T>(key);
        if (val.has_value()) {
            results[key] = val.value();
        }
    }
    return results;
}

// ==================== Convenience Accessors ====================

std::optional<int> Config::getInt(const std::string& key) const {
    return get<int>(key);
}

std::optional<double> Config::getDouble(const std::string& key) const {
    return get<double>(key);
}

std::optional<std::string> Config::getString(const std::string& key) const {
    return get<std::string>(key);
}

std::optional<bool> Config::getBool(const std::string& key) const {
    return get<bool>(key);
}

std::optional<std::vector<int>> Config::getIntVector(const std::string& key) const {
    return get<std::vector<int>>(key);
}

// ==================== Enhanced Setters ====================

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    ConfigImpl::setValue(*pImpl, key, value, source);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// ==================== Enhanced Validation ====================

std::vector<ValidationError> Config::validate() const {
    std::vector<ValidationError> allErrors;
    
    for (const auto& entry : pImpl->entries) {
        ValidationError error = schema.validate(entry);
        if (!error.key.empty()) {
            allErrors.push_back(error);
        }
    }
    
    return allErrors;
}

bool Config::isValid(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return true; // Key doesn't exist, so no validation to fail
    
    ValidationError error = schema.validate(*entry);
    return error.key.empty();
}

std::vector<ValidationError> Config::getValidationErrors(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return {ValidationError(key, "Key not found")};
    
    ValidationError error = schema.validate(*entry);
    if (error.key.empty()) return {};
    
    return {error};
}

// ==================== Advanced Configuration Features ====================

// Custom validator implementations
namespace ConfigImpl {
    
    class CustomValidator {
    public:
        virtual ~CustomValidator() = default;
        virtual bool validate(const std::string& key, const ConfigValue& value, std::string& error) const = 0;
        virtual std::string getType() const = 0;
    };
    
    class RangeValidator : public CustomValidator {
    public:
        RangeValidator(const std::string& key, double min, double max) 
            : key_(key), min_(min), max_(max) {}
        
        bool validate(const std::string& key, const ConfigValue& value, std::string& error) const override {
            if (this->key_ != key) return true; // Not our key
            
            if (std::holds_alternative<int>(value)) {
                double val = static_cast<double>(std::get<int>(value));
                if (val < min_ || val > max_) {
                    error = "Value " + std::to_string(val) + " is outside range [" + std::to_string(min_) + ", " + std::to_string(max_) + "]";
                    return false;
                }
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                if (val < min_ || val > max_) {
                    error = "Value " + std::to_string(val) + " is outside range [" + std::to_string(min_) + ", " + std::to_string(max_) + "]";
                    return false;
                }
            }
            return true;
        }
        
        std::string getType() const override { return "RangeValidator"; }
        
    private:
        std::string key_;
        double min_;
        double max_;
    };
    
    class ExpressionValidator : public CustomValidator {
    public:
        ExpressionValidator(const std::string& expression) : expression_(expression) {}
        
        bool validate(const std::string& key, const ConfigValue& value, std::string& error) const override {
            if (expression_ == "positive") {
                if (std::holds_alternative<int>(value) && std::get<int>(value) <= 0) {
                    error = "Value must be positive";
                    return false;
                } else if (std::holds_alternative<double>(value) && std::get<double>(value) <= 0) {
                    error = "Value must be positive";
                    return false;
                }
            } else if (expression_ == "non_negative") {
                if (std::holds_alternative<int>(value) && std::get<int>(value) < 0) {
                    error = "Value must be non-negative";
                    return false;
                } else if (std::holds_alternative<double>(value) && std::get<double>(value) < 0) {
                    error = "Value must be non-negative";
                    return false;
                }
            }
            return true;
        }
        
        std::string getType() const override { return "ExpressionValidator"; }
        
    private:
        std::string expression_;
    };
    
    class LuaValidator : public CustomValidator {
    public:
        LuaValidator(const std::string& luaExpression) : luaExpression_(luaExpression) {}
        
        bool validate(const std::string& key, const ConfigValue& value, std::string& error) const override {
            if (luaExpression_.find("value > 0") != std::string::npos) {
                if (std::holds_alternative<int>(value) && std::get<int>(value) <= 0) {
                    error = "Lua validation failed: value must be positive";
                    return false;
                } else if (std::holds_alternative<double>(value) && std::get<double>(value) <= 0) {
                    error = "Lua validation failed: value must be positive";
                    return false;
                }
            }
            return true;
        }
        
        std::string getType() const override { return "LuaValidator"; }
        
    private:
        std::string luaExpression_;
    };
    
    class PythonValidator : public CustomValidator {
    public:
        PythonValidator(const std::string& pythonExpression) : pythonExpression_(pythonExpression) {}
        
        bool validate(const std::string& key, const ConfigValue& value, std::string& error) const override {
            if (pythonExpression_.find("value > 0") != std::string::npos) {
                if (std::holds_alternative<int>(value) && std::get<int>(value) <= 0) {
                    error = "Python validation failed: value must be positive";
                    return false;
                } else if (std::holds_alternative<double>(value) && std::get<double>(value) <= 0) {
                    error = "Python validation failed: value must be positive";
                    return false;
                }
            }
            return true;
        }
        
        std::string getType() const override { return "PythonValidator"; }
        
    private:
        std::string pythonExpression_;
    };
    
    // Validation manager
    class ValidationManager {
    public:
        void addValidator(const std::string& key, std::unique_ptr<CustomValidator> validator) {
            validators_[key] = std::move(validator);
        }
        
        bool validate(const std::string& key, const ConfigValue& value, std::string& error) const {
            auto it = validators_.find(key);
            if (it == validators_.end()) return true; // No validator for this key
            
            return it->second->validate(key, value, error);
        }
        
        std::vector<std::string> getValidationErrors(const std::vector<ConfigEntry>& entries) const {
            std::vector<std::string> errors;
            
            for (const auto& entry : entries) {
                std::string error;
                if (!validate(entry.key, entry.value, error)) {
                    errors.push_back("Validation error for key '" + entry.key + "': " + error);
                }
            }
            
            return errors;
        }
        
    private:
        std::unordered_map<std::string, std::unique_ptr<CustomValidator>> validators_;
    };
    
} // namespace ConfigImpl

Config Config::fromTemplate(const std::string& templateName) {
    Config config;
    
    if (templateName == "brain_default") {
        // Brain simulation default template
        config.set("brain.regionCount", 3);
        config.set("brain.neuronCountPerRegion", 1000);
        config.set("brain.synapticDensity", 0.1);
        config.set("brain.plasticity.enableSTDP", true);
        config.set("brain.plasticity.stdpLearningRate", 0.01);
        config.set("brain.plasticity.maxWeight", 1.0);
        config.set("brain.memory.capacity", 1000);
        config.set("brain.memory.decayRate", 0.95);
        config.set("brain.neuromodulation.baseline", 0.5);
        config.set("brain.neuromodulation.noiseLevel", 0.1);
        config.set("simulation.timeStep", 0.01);
        config.set("simulation.maxSteps", 10000);
        config.set("simulation.dt", 0.001);
    } else if (templateName == "simulation_basic") {
        // Basic simulation template
        config.set("simulation.timeStep", 0.01);
        config.set("simulation.maxSteps", 1000);
        config.set("simulation.dt", 0.001);
        config.set("simulation.randomSeed", 42);
        config.set("output.enableLogging", true);
        config.set("output.logLevel", "INFO");
    } else if (templateName == "high_performance") {
        // High performance template
        config.set("simulation.parallel", true);
        config.set("simulation.threadCount", 4);
        config.set("brain.optimization.level", 3);
        config.set("brain.optimization.enablePruning", true);
    }
    
    return config;
}

Config Config::clone() const {
    Config cloneConfig(schema);
    
    for (const auto& entry : pImpl->entries) {
        cloneConfig.pImpl->entries.push_back(entry);
    }
    
    return cloneConfig;
}

void Config::merge(const Config& other, const std::string& strategy) {
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = findEntry(otherEntry.key);
        
        if (it != nullptr) {
            if (strategy == "override") {
                // Override with other's value
                set(otherEntry.key, otherEntry.value, otherEntry.source);
            }
            // Keep existing if strategy is "keepExisting"
        } else {
            // Add new entry
            pImpl->entries.push_back(otherEntry);
        }
    }
}

std::vector<ConfigEntry> Config::diff(const Config& other) const {
    std::vector<ConfigEntry> differences;
    
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = findEntry(otherEntry.key);
        if (it == nullptr || !compareValues(it->value, otherEntry.value)) {
            differences.push_back(otherEntry);
        }
    }
    
    // Check for entries only in this config
    for (const auto& thisEntry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&thisEntry](const ConfigEntry& e) { return e.key == thisEntry.key; });
        if (it == other.pImpl->entries.end()) {
            differences.push_back(thisEntry);
        }
    }
    
    return differences;
}

// ==================== Enhanced Serialization ====================

bool Config::saveToJSON(const std::string& filepath, bool pretty) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "{" << std::endl;
        
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            
            file << "  \"" << entry.key << "\": {" << std::endl;
            file << "    \"value\": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        file << arg[j];
                        if (j < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        file << arg[j];
                        if (j < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        file << "\"" << arg[j] << "\"";
                        if (j < arg.size() - 1) file << ", ";
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "," << std::endl;
            file << "    \"source\": " << static_cast<int>(entry.source) << ";" << std::endl;
            file << "    \"description\": \"" << entry.description << "\"";
            
            if (i < pImpl->entries.size() - 1) {
                file << "," << std::endl;
            }
        }
        
        file << std::endl << "}";
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string Config::toJSON(bool pretty) const {
    std::ostringstream oss;
    
    oss << "{" << std::endl;
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        oss << "  \"" << entry.key << "\": {" << std::endl;
        oss << "    \"value\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "," << std::endl;
        oss << "    \"source\": " << static_cast<int>(entry.source) << ";" << std::endl;
        oss << "    \"description\": \"" << entry.description << "\"";
        
        if (i < pImpl->entries.size() - 1) {
            oss << "," << std::endl;
        }
    }
    
    oss << std::endl << "}";
    
    return oss.str();
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Simple JSON parsing - improved with utility functions
        std::string line;
        std::string currentKey;
        bool inObject = false;
        
        while (std::getline(file, line)) {
            // Trim whitespace using utility function
            line = Utils::StringUtils::trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || Utils::StringUtils::startsWith(line, "//")) {
                continue;
            }
            
            // Check for object start/end
            if (line == "{") {
                inObject = true;
            } else if (line == "}") {
                inObject = false;
                currentKey = "";
            } else if (inObject) {
                // Parse key: "key": {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    currentKey = line.substr(0, colonPos);
                    // Remove quotes using utility function
                    if (currentKey.size() >= 2 && currentKey.front() == '"' && currentKey.back() == '"') {
                        currentKey = Utils::StringUtils::substring(currentKey, 1, currentKey.size() - 2);
                    }
                }
                // Parse value: "value": <value>;
                else if (Utils::StringUtils::contains(line, "value")) {
                    size_t semicolonPos = line.find(';');
                    if (semicolonPos != std::string::npos) {
                        std::string valueStr = Utils::StringUtils::substring(line, line.find(':') + 1, semicolonPos - line.find(':') - 1);
                        valueStr = Utils::StringUtils::trim(valueStr);
                        
                        // Remove quotes using utility function
                        if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
                            valueStr = Utils::StringUtils::substring(valueStr, 1, valueStr.size() - 2);
                        }
                        
                        // Try to convert to appropriate type using utility function
                        ConfigValue value;
                        
                        // Try boolean
                        if (valueStr == "true" || valueStr == "false") {
                            value = valueStr == "true";
                        }
                        // Try integer
                        else {
                            try {
                                size_t pos;
                                int intVal = std::stoi(valueStr, &pos);
                                if (pos == valueStr.size()) {
                                    value = intVal;
                                } else {
                                    // Try double
                                    double doubleVal = std::stod(valueStr, &pos);
                                    if (pos == valueStr.size()) {
                                        value = doubleVal;
                                    } else {
                                        // String
                                        value = valueStr;
                                    }
                                }
                            } catch (...) {
                                value = valueStr;
                            }
                        }
                        
                        set(currentKey, value, ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// ==================== Backup and Restore ====================

std::string Config::createBackup(const std::string& directory) {
    std::string timestamp = generateTimestampedFilename("config_backup");
    std::string filepath = directory + "/" + timestamp;
    
    if (!saveToJSON(filepath, true)) {
        throw std::runtime_error("Failed to create backup");
    }
    
    return filepath;
}

bool Config::restoreFromBackup(const std::string& backupPath) {
    clear();
    return loadFromJSON(backupPath);
}

// ==================== Configuration Comparison ====================

std::string Config::getDiffSummary(const Config& other) const {
    std::vector<ConfigEntry> differences = diff(other);
    
    if (differences.empty()) {
        return "Configuration is identical";
    }
    
    std::ostringstream oss;
    oss << "Configuration differences (" << differences.size() << " entries):\n";
    
    for (const auto& entry : differences) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

bool Config::hasChanged(const Config& other) const {
    return !diff(other).empty();
}

// ==================== Information and Analysis ====================

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Show first few entries using utility function
    oss << "Sample entries:\n";
    for (size_t i = 0; i < std::min(size_t(3), pImpl->entries.size()); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    
    size_t totalEntries = pImpl->entries.size();
    oss << "  Total entries: " << totalEntries << "\n";
    
    // Source distribution using utility function
    oss << "  Source distribution:\n";
    for (int i = 0; i < 4; ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        size_t count = ConfigImpl::countEntriesBySource(pImpl->entries, source);
        oss << "    " << static_cast<int>(source) << ": " << count << "\n";
    }
    
    return oss.str();
}

std::vector<std::string> Config::getKeys() const {
    return ConfigImpl::getAllKeys(pImpl->entries);
}

bool Config::has(const std::string& key) const {
    return findEntry(key) != nullptr;
}

std::optional<ConfigEntry> Config::getEntry(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return std::nullopt;
    
    return *entry;
}

// ==================== Modification Operations ====================

void Config::remove(const std::string& key) {
    // Remove from main entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    clearChangeCallbacks();
}

void Config::clear() {
    pImpl->entries.clear();
    clearChangeCallbacks();
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& description) {
    set(key, value, source);
    
    // Update or create entry with description
    auto it = findEntry(key);
    if (it != nullptr) {
        it->description = description;
    }
}

// ==================== Change Notifications ====================

void Config::setChangeCallback(std::function<void(const std::string&, const ConfigValue&)> callback) {
    changeCallbacks.push_back(callback);
}

void Config::clearChangeCallbacks() {
    changeCallbacks.clear();
}

void Config::notifyChange(const std::string& key, const ConfigValue& value) const {
    for (const auto& callback : changeCallbacks) {
        callback(key, value);
    }
}

// ==================== Debug and Inspection ====================

std::string Config::debugDump() const {
    std::ostringstream oss;
    oss << "=== Config Debug Dump ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Schema errors: " << schema.getErrors().size() << "\n";
    oss << "Change callbacks: " << changeCallbacks.size() << "\n\n";
    
    oss << "Entries:\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  Entry " << i << ":\n";
        oss << "    Key: \"" << entry.key << "\"\n";
        oss << "    Value: ";
        std::visit([&oss](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "\n";
        oss << "    Source: " << static_cast<int>(entry.source) << "\n";
        oss << "    Description: \"" << entry.description << "\"\n";
        oss << "\n";
    }
    
    return oss.str();
}

// ==================== Legacy Methods ====================

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromJSON(filepath);
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (Utils::StringUtils::startsWith(arg, "--")) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = Utils::StringUtils::substring(arg, 2, pos - 2);
                std::string value = Utils::StringUtils::substring(arg, pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = Utils::StringUtils::substring(arg, 1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToJSON(filepath, true);
}

// ==================== Private Helper Methods ====================

ConfigEntry* Config::findEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? &*it : nullptr;
}

const ConfigEntry* Config::findEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? &*it : nullptr;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ConfigEntry* entry = findEntry(key);
    
    if (entry) {
        // Update existing entry
        entry->value = value;
        entry->source = source;
        entry->timestamp = std::chrono::duration<double>::zero();
    } else {
        // Create new entry
        pImpl->entries.push_back(ConfigEntry(key, value, source));
        
        // Add to nested index
        auto dotPos = key.find('.');
        if (dotPos != std::string::npos) {
            std::string prefix = key.substr(0, dotPos);
            pImpl->nestedIndex[prefix].push_back(&pImpl->entries.back());
        }
    }
    
    // Validate if schema exists
    if (!schema.getErrors().empty()) {
        ConfigEntry tempEntry(key, value, source);
        ValidationError error = schema.validate(tempEntry);
        if (!error.key.empty()) {
            throw std::runtime_error("Validation failed for key \"" + key + "\": " + error.message);
        }
    }
    
    // Notify change
    notifyChange(key, value);
}

// ==================== Helper Function Implementations ====================

namespace ConfigImpl {
    namespace Helper {
        
        std::string trim(const std::string& str) {
            size_t start = 0;
            size_t end = str.size();
            
            while (start < end && std::isspace(str[start])) start++;
            while (end > start && std::isspace(str[end - 1])) end--;
            
            return str.substr(start, end - start);
        }
        
        bool isWhitespace(char ch) {
            return std::isspace(static_cast<unsigned char>(ch));
        }
        
        std::string escapeForJSON(const std::string& input) {
            std::string result;
            result.reserve(input.size() * 2);
            
            for (char ch : input) {
                switch (ch) {
                    case '\"': result += "\\\""; break;
                    case '\\': result += "\\\\"; break;
                    case '\\': result += "\\\\"; break;
                    case '/': result += "\\/"; break;
                    case '\\': result += "\\b"; break;
                    case '\\': result += "\\f"; break;
                    case '\\': result += "\\n"; break;
                    case '\\': result += "\\r"; break;
                    case '\\': result += "\\t"; break;
                    default:
                        if (static_cast<unsigned char>(ch) < 32) {
                            // Control character - use escape sequence
                            result += "\\\\u";
                            char hex[5];
                            std::snprintf(hex, sizeof(hex), "%04x", ch);
                            result += std::string(hex);
                        } else {
                            result += ch;
                        }
                        break;
                }
            }
            
            return result;
        }
        
        std::string unescapeFromJSON(const std::string& input) {
            std::string result;
            result.reserve(input.size());
            
            for (size_t i = 0; i < input.size(); ++i) {
                if (input[i] == '\\' && i + 1 < input.size()) {
                    switch (input[i + 1]) {
                        case '\"': result += '\"'; i++; break;
                        case '\\\\': result += '\\'; i++; break;
                        case '/': result += '/'; i++; break;
                        case 'b': result += '\b'; i++; break;
                        case 'f': result += '\f'; i++; break;
                        case 'n': result += '\n'; i++; break;
                        case 'r': result += '\r'; i++; break;
                        case 't': result += '\t'; i++; break;
                        case 'u':
                            if (i + 5 < input.size()) {
                                std::string hex = input.substr(i + 2, 4);
                                char* end;
                                long code = std::strtol(hex.c_str(), &end, 16);
                                result += static_cast<char>(code);
                                i += 5;
                            }
                            break;
                        default: result += input[i]; break;
                    }
                } else {
                    result += input[i];
                }
            }
            
            return result;
        }
        
        bool isValidKey(const std::string& key) {
            if (key.empty()) return false;
            if (key.find(' ') != std::string::npos) return false;
            if (key.find('\\') != std::string::npos) return false;
            if (key.find('/') != std::string::npos) return false;
            if (key.find('\\') != std::string::npos) return false;
            return true;
        }
        
        bool isValidValue(const ConfigValue& value) {
            return std::visit([](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return !val.empty();
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    return true; // Empty vectors allowed
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    return true; // Empty vectors allowed
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    return true; // Empty vectors allowed
                } else {
                    return true;
                }
            }, value);
        }
        
        bool isValidSource(ConfigSource source) {
            return source >= ConfigSource::Default && source <= ConfigSource::Unknown;
        }
        
        void writeJSONString(std::ostream& os, const std::string& value) {
            os << "\"" << escapeForJSON(value) << "\"";
        }
        
        void writeJSONNumber(std::ostream& os, double value) {
            // Use std::setprecision for consistent formatting
            os.precision(10);
            os << std::fixed << value;
            // Remove trailing zeros
            std::string str = Utils::StringUtils::formatEfficient("{}", value);
            // Simple trailing zero removal (placeholder)
            os << str;
        }
        
        void writeJSONBoolean(std::ostream& os, bool value) {
            os << (value ? "true" : "false");
        }
        
        void writeJSONArray(std::ostream& os, const std::vector<int>& array) {
            os << "[";
            for (size_t i = 0; i < array.size(); ++i) {
                if (i > 0) os << ", ";
                os << array[i];
            }
            os << "]";
        }
        
        void writeJSONArray(std::ostream& os, const std::vector<double>& array) {
            os << "[";
            for (size_t i = 0; i < array.size(); ++i) {
                if (i > 0) os << ", ";
                os.precision(10);
                os << std::fixed << array[i];
            }
            os << "]";
        }
        
        void writeJSONArray(std::ostream& os, const std::vector<std::string>& array) {
            os << "[";
            for (size_t i = 0; i < array.size(); ++i) {
                if (i > 0) os << ", ";
                writeJSONString(os, array[i]);
            }
            os << "]";
        }
        
        size_t countEntriesBySource(const std::vector<ConfigEntry>& entries, ConfigSource source) {
            return std::count_if(entries.begin(), entries.end(),
                [source](const ConfigEntry& e) { return e.source == source; });
        }
        
        std::vector<std::string> getAllKeys(const std::vector<ConfigEntry>& entries) {
            std::vector<std::string> keys;
            keys.reserve(entries.size());
            for (const auto& entry : entries) {
                keys.push_back(entry.key);
            }
            return keys;
        }
        
        std::vector<ConfigEntry> findEntriesByPrefix(const std::vector<ConfigEntry>& entries, const std::string& prefix) {
            std::vector<ConfigEntry> result;
            for (const auto& entry : entries) {
                if (Utils::StringUtils::startsWith(entry.key, prefix)) {
                    result.push_back(entry);
                }
            }
            return result;
        }
        
        std::string createErrorMessage(const std::string& operation, const std::string& key, const std::string& message) {
            return Utils::StringUtils::formatEfficient(
                "Config {} failed for key '{}': {}", operation, key, message);
        }
        
        void logConfigOperation(const std::string& operation, const std::string& key, const ConfigValue& value) {
            NLM_LOG_INFO(Utils::StringUtils::formatEfficient(
                "Config {}: {} = {}", operation, key, value));
        }
        
        bool compareConfigEntries(const ConfigEntry& a, const ConfigEntry& b) {
            return a.key < b.key;
        }
        
        std::vector<ConfigEntry> findDifferences(const std::vector<ConfigEntry>& a, const std::vector<ConfigEntry>& b) {
            std::vector<ConfigEntry> differences;
            
            // Check entries in a but not in b
            for (const auto& entryA : a) {
                bool found = false;
                for (const auto& entryB : b) {
                    if (entryA.key == entryB.key && compareValues(entryA.value, entryB.value)) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    differences.push_back(entryA);
                }
            }
            
            // Check entries in b but not in a
            for (const auto& entryB : b) {
                bool found = false;
                for (const auto& entryA : a) {
                    if (entryB.key == entryA.key && compareValues(entryB.value, entryA.value)) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    differences.push_back(entryB);
                }
            }
            
            return differences;
        }
        
        std::vector<std::string> filterKeys(const std::vector<std::string>& keys, const std::vector<std::string>& validKeys) {
            std::vector<std::string> result;
            std::unordered_set<std::string> validSet(validKeys.begin(), validKeys.end());
            
            for (const auto& key : keys) {
                if (validSet.find(key) != validSet.end()) {
                    result.push_back(key);
                }
            }
            
            return result;
        }
        
        std::vector<ConfigEntry> filterEntries(const std::vector<ConfigEntry>& entries, ConfigSource source) {
            std::vector<ConfigEntry> result;
            for (const auto& entry : entries) {
                if (entry.source == source) {
                    result.push_back(entry);
                }
            }
            return result;
        }
        
    } // namespace Helper
    
    void initializeConfigImpl(Config::Impl& impl) {
        // Initialize the config implementation
    }
    
    void clearChangeCallbacks(Config::Impl& impl) {
        impl.changeCallbacks.clear();
    }
    
    ConfigEntry* findEntry(Config::Impl& impl, const std::string& key) {
        auto it = std::find_if(impl.entries.begin(), impl.entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return (it != impl.entries.end()) ? &*it : nullptr;
    }
    
    const ConfigEntry* findEntry(const Config::Impl& impl, const std::string& key) {
        auto it = std::find_if(impl.entries.begin(), impl.entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        return (it != impl.entries.end()) ? &*it : nullptr;
    }
    
    bool compareValues(const ConfigValue& a, const ConfigValue& b) {
        if (a.index() != b.index()) return false;
        
        return std::visit([](const auto& aVal, const auto& bVal) {
            return aVal == bVal;
        }, a, b);
    }
    
    std::string generateTimestampedFilename(const std::string& prefix) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << prefix << "_" 
           << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S")
           << "_" << std::setfill('0') << std::setw(3) << ms.count() << ".json";
        
        return ss.str();
    }
    
    std::vector<std::string> extractKeyParts(const std::string& key) {
        std::vector<std::string> parts;
        std::string part;
        
        for (char ch : key) {
            if (ch == '.') {
                if (!part.empty()) {
                    parts.push_back(part);
                    part.clear();
                }
            } else {
                part += ch;
            }
        }
        
        if (!part.empty()) {
            parts.push_back(part);
        }
        
        return parts;
    }
    
    std::string joinKeyParts(const std::vector<std::string>& parts) {
        if (parts.empty()) return "";
        
        std::string result = parts[0];
        for (size_t i = 1; i < parts.size(); ++i) {
            result += "." + parts[i];
        }
        
        return result;
    }
    
} // namespace ConfigImpl

// Explicit template instantiations

template std::optional<int> Config::get<int>(const std::string&) const;

template std::optional<int> Config::get<int>(const std::string&, bool) const;

template std::optional<double> Config::get<double>(const std::string&) const;

template std::optional<double> Config::get<double>(const std::string&, bool) const;

template std::optional<bool> Config::get<bool>(const std::string&) const;

template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;

template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;

template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;

template double Config::getOr<double>(const std::string&, const double&) const;

template bool Config::getOr<bool>(const std::string&, const bool&) const;

template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;

template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;

template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

template void Config::set(const std::string&, const int&, ConfigSource);

template void Config::set(const std::string&, const double&, ConfigSource);

template void Config::set(const std::string&, const bool&, ConfigSource);

template void Config::set(const std::string&, const std::string&, ConfigSource);

template void Config::set(const std::string&, const std::vector<int>&, ConfigSource);

template void Config::set(const std::string&, const std::vector<double>&, ConfigSource);

template void Config::set(const std::string&, const std::vector<std::string>&, ConfigSource);

template void Config::set(const std::string&, const ConfigValue&, ConfigSource);

template std::optional<int> Config::get(const std::string&, bool) const;

template std::optional<double> Config::get(const std::string&, bool) const;

template std::optional<bool> Config::get(const std::string&, bool) const;

template std::optional<std::string> Config::get(const std::string&, bool) const;

template std::optional<std::vector<int>> Config::get(const std::string&, bool) const;

template std::optional<std::vector<double>> Config::get(const std::string&, bool) const;

template std::optional<std::vector<std::string>> Config::get(const std::string&, bool) const;

template std::unordered_map<std::string, int> Config::batchGet<int>(const std::vector<std::string>&) const;

template std::unordered_map<std::string, double> Config::batchGet<double>(const std::vector<std::string>&) const;

template std::unordered_map<std::string, bool> Config::batchGet<bool>(const std::vector<std::string>&) const;

template std::unordered_map<std::string, std::string> Config::batchGet<std::string>(const std::vector<std::string>&) const;

template std::unordered_map<std::string, std::vector<int>> Config::batchGet<std::vector<int>>(const std::vector<std::string>&) const;

template std::unordered_map<std::string, std::vector<double>> Config::batchGet<std::vector<double>>(const std::vector<std::string>&) const;

template std::unordered_map<std::string, std::vector<std::string>> Config::batchGet<std::vector<std::string>>(const std::vector<std::string>&) const;

} // namespace nlm