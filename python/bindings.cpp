#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <functional>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <tuple>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Forward declarations for enhanced Python bindings
class EnhancedConfig;
class ConfigValidator;
class ConfigBuilder;
class ConfigTemplate;
class ConfigRegistry;

// Enhanced Config wrapper class
class EnhancedConfig {
public:
    EnhancedConfig(std::shared_ptr<Config> config = nullptr) : config_(config ? config : std::make_shared<Config>()) {
        // Initialize history tracking
        history_enabled_ = true;
        history_max_size_ = 1000;
        
        // Initialize validation system
        validation_enabled_ = true;
        schema_ = std::make_shared<ValidationSchema>();
    }
    
    ~EnhancedConfig() = default;
    
    // Core configuration interface (maintaining backward compatibility)
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return config_->get<T>(key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return config_->getOr<T>(key, defaultValue);
    }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        // Record in history before setting
        if (history_enabled_) {
            HistoryEntry entry;
            entry.timestamp = std::chrono::system_clock::now();
            entry.key = key;
            entry.old_value = getRawValue(key);
            entry.new_value = value;
            entry.source = source;
            entry.category = getCategory(key);
            history_.push_back(entry);
            
            // Trim history if needed
            if (history_.size() > history_max_size_) {
                history_.erase(history_.begin(), history_.begin() + (history_.size() - history_max_size_));
            }
        }
        
        // Validate if enabled
        if (validation_enabled_ && schema_) {
            validateKey(key, value);
        }
        
        config_->set(key, value, source);
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
        ConfigValue val = value;
        set(key, val, source);
    }
    
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
        ConfigValue val = value;
        set(key, val, source);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
        ConfigValue val = value;
        set(key, val, source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
        ConfigValue val = value;
        set(key, val, source);
    }
    
    // Advanced Python interface
    void setNested(const std::string& path, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        config_->setNested(path, value, source);
        
        // Extract key from path for history
        std::string key = path;
        size_t last_dot = path.find_last_of('.');
        if (last_dot != std::string::npos) {
            key = path.substr(last_dot + 1);
        }
        
        if (history_enabled_) {
            HistoryEntry entry;
            entry.timestamp = std::chrono::system_clock::now();
            entry.key = key;
            entry.old_value = getNestedRawValue(path);
            entry.new_value = value;
            entry.source = source;
            entry.category = getCategoryFromPath(path);
            history_.push_back(entry);
        }
    }
    
    std::optional<ConfigValue> getNested(const std::string& path) const {
        return config_->getNested(path);
    }
    
    bool hasNested(const std::string& path) const {
        return config_->hasNested(path);
    }
    
    // Batch operations
    void batchSet(const std::vector<std::string>& keys, const std::vector<ConfigValue>& values, 
                 ConfigSource source = ConfigSource::Runtime) {
        if (keys.size() != values.size()) {
            throw py::value_error("Keys and values must have the same size for batch set");
        }
        
        for (size_t i = 0; i < keys.size(); ++i) {
            set(keys[i], values[i], source);
        }
    }
    
    std::vector<ConfigValue> batchGet(const std::vector<std::string>& keys) const {
        std::vector<ConfigValue> results;
        results.reserve(keys.size());
        
        for (const auto& key : keys) {
            auto value = config_->get<ConfigValue>(key);
            if (value) {
                results.push_back(*value);
            } else {
                results.push_back(ConfigValue{}); // Empty variant
            }
        }
        
        return results;
    }
    
    // Category support
    void setCategory(const std::string& key, const std::string& category) {
        config_->setEntryCategory(key, category);
    }
    
    std::string getCategory(const std::string& key) const {
        return config_->getEntry(key).category;
    }
    
    std::string getCategoryFromPath(const std::string& path) const {
        size_t last_dot = path.find_last_of('.');
        if (last_dot != std::string::npos) {
            std::string prefix = path.substr(0, last_dot);
            // Map prefix to category
            static const std::unordered_map<std::string, std::string> prefix_to_category = {
                {"brain", "Brain"},
                {"sensory", "Sensory"},
                {"motor", "Motor"},
                {"plasticity", "Plasticity"},
                {"development", "Development"},
                {"neuromodulation", "Neuromodulation"},
                {"memory", "Memory"},
                {"cognition", "Cognition"},
                {"environment", "Environment"},
                {"general", "General"}
            };
            
            auto it = prefix_to_category.find(prefix);
            if (it != prefix_to_category.end()) {
                return it->second;
            }
        }
        return "Custom";
    }
    
    std::vector<std::string> getCategories() const {
        std::vector<std::string> categories;
        std::unordered_map<std::string, bool> seen;
        
        for (const auto& key : config_->getKeys()) {
            std::string category = getCategory(key);
            if (seen.find(category) == seen.end()) {
                categories.push_back(category);
                seen[category] = true;
            }
        }
        
        return categories;
    }
    
    std::map<std::string, std::vector<std::string>> getConfigByCategory() const {
        std::map<std::string, std::vector<std::string>> config_by_category;
        
        for (const auto& key : config_->getKeys()) {
            std::string category = getCategory(key);
            config_by_category[category].push_back(key);
        }
        
        return config_by_category;
    }
    
    // Validation system
    void enableValidation(bool enabled) {
        validation_enabled_ = enabled;
    }
    
    void addValidator(const std::string& key, const std::function<bool(const ConfigValue&)>& validator, 
                     const std::string& errorMessage = "Validation failed") {
        validators_[key] = {validator, errorMessage};
    }
    
    void clearValidators() {
        validators_.clear();
    }
    
    bool validateKey(const std::string& key) const {
        auto it = validators_.find(key);
        if (it == validators_.end()) return true;
        
        auto value = config_->get<ConfigValue>(key);
        if (!value) return false;
        
        return it->second.validator(*value);
    }
    
    bool validateAll() const {
        for (const auto& key : config_->getKeys()) {
            if (!validateKey(key)) {
                return false;
            }
        }
        return true;
    }
    
    void validateKey(const std::string& key, const ConfigValue& value) {
        auto it = validators_.find(key);
        if (it != validators_.end() && !it->second.validator(value)) {
            throw py::value_error(it->second.errorMessage);
        }
    }
    
    // Presets
    void loadPreset(const std::string& presetName) {
        auto it = presets_.find(presetName);
        if (it == presets_.end()) {
            throw py::key_error("Preset not found: " + presetName);
        }
        
        const auto& preset = it->second;
        clear();
        
        for (const auto& [key, value] : preset) {
            set(key, value);
        }
        
        current_preset_ = presetName;
    }
    
    void savePreset(const std::string& presetName, const std::string& description = "") {
        PresetData data;
        data.description = description;
        data.timestamp = std::chrono::system_clock::now();
        
        for (const auto& key : config_->getKeys()) {
            auto value = config_->get<ConfigValue>(key);
            if (value) {
                data.config[key] = *value;
            }
        }
        
        presets_[presetName] = data;
        current_preset_ = presetName;
    }
    
    std::vector<std::string> listPresets() const {
        std::vector<std::string> presetNames;
        for (const auto& pair : presets_) {
            presetNames.push_back(pair.first);
        }
        return presetNames;
    }
    
    // Builder pattern
    ConfigBuilder createBuilder() {
        return ConfigBuilder(this);
    }
    
    // Template system
    ConfigTemplate createTemplate(const std::string& name, const std::map<std::string, ConfigValue>& templateConfig) {
        return ConfigTemplate(name, templateConfig);
    }
    
    ConfigTemplate renderTemplate(const std::string& name, 
                                  const std::map<std::string, std::string>& variables) {
        auto it = template_cache_.find(name);
        if (it == template_cache_.end()) {
            throw py::key_error("Template not found: " + name);
        }
        
        auto& template_obj = it->second;
        return template_obj.render(variables);
    }
    
    // History tracking
    void enableHistory(bool enabled) {
        history_enabled_ = enabled;
    }
    
    void setHistoryMaxSize(size_t maxSize) {
        history_max_size_ = maxSize;
        if (history_.size() > maxSize) {
            history_.erase(history_.begin(), history_.begin() + (history_.size() - maxSize));
        }
    }
    
    std::vector<HistoryEntry> getHistory(size_t limit = 100) const {
        if (limit == 0 || limit > history_.size()) {
            return history_;
        }
        
        auto start = history_.rbegin() + (history_.size() - limit);
        return std::vector<HistoryEntry>(start, history_.rend());
    }
    
    void clearHistory() {
        history_.clear();
    }
    
    // Export/Import utilities
    bool exportToFile(const std::string& filepath, const std::string& format = "json") const {
        if (format == "json") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            py::module json = py::module::import("json");
            auto dict = toDict();
            file << json.dumps(dict);
            return true;
        } else if (format == "yaml") {
            // YAML export would require pybind11's YAML support
            return false;
        } else if (format == "ini") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            for (const auto& key : config_->getKeys()) {
                auto value = config_->get<ConfigValue>(key);
                if (value && std::holds_alternative<std::string>(*value)) {
                    file << key << "=" << std::get<std::string>(*value) << "\n";
                }
            }
            return true;
        }
        
        return false;
    }
    
    bool importFromFile(const std::string& filepath, const std::string& format = "json") {
        clear();
        
        if (format == "json") {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            py::module json = py::module::import("json");
            auto dict = json.load(file);
            
            // Convert JSON dict to config
            for (auto it = dict.begin(); it != dict.end(); ++it) {
                std::string key = it.key();
                
                // Determine type from value
                auto value = it.value();
                if (value.is_string()) {
                    set(key, value.get<std::string>());
                } else if (value.is_number_integer()) {
                    set(key, value.get<int>());
                } else if (value.is_number_float()) {
                    set(key, value.get<double>());
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>());
                } else if (value.is_array()) {
                    // For arrays, try to infer type
                    py::list list = value.get_py();
                    if (list.size() > 0) {
                        // Check if all elements are integers
                        bool all_ints = true;
                        for (size_t i = 0; i < list.size(); ++i) {
                            if (!py::cast<std::any>(list[i]).has_value()) {
                                all_ints = false;
                                break;
                            }
                        }
                        
                        if (all_ints) {
                            std::vector<int> int_list;
                            for (size_t i = 0; i < list.size(); ++i) {
                                int_list.push_back(py::cast<int>(list[i]));
                            }
                            set(key, int_list);
                        } else {
                            // Use string representation as fallback
                            std::stringstream ss;
                            ss << "[";
                            for (size_t i = 0; i < list.size(); ++i) {
                                if (i > 0) ss << ", ";
                                ss << py::cast<std::string>(list[i]);
                            }
                            ss << "]";
                            set(key, ss.str());
                        }
                    }
                }
            }
            
            return true;
        }
        
        return false;
    }
    
    // Comparison and merging
    std::map<std::string, std::variant<bool, std::string>> compare(const EnhancedConfig& other) const {
        std::map<std::string, std::variant<bool, std::string>> result;
        
        // Check keys in this config
        for (const auto& key : config_->getKeys()) {
            auto value1 = config_->get<ConfigValue>(key);
            auto value2 = other.config_->get<ConfigValue>(key);
            
            if (value2) {
                // Key exists in both, compare values
                if (*value1 != *value2) {
                    result[key] = "modified";
                } else {
                    result[key] = true;
                }
            } else {
                // Key only in this config
                result[key] = "added";
            }
        }
        
        // Check keys only in other config
        for (const auto& key : other.config_->getKeys()) {
            if (!config_->has(key)) {
                result[key] = "removed";
            }
        }
        
        return result;
    }
    
    EnhancedConfig merge(const EnhancedConfig& other, const std::string& strategy = "replace") const {
        EnhancedConfig result;
        
        if (strategy == "replace") {
            // Copy all keys from this config
            for (const auto& key : config_->getKeys()) {
                auto value = config_->get<ConfigValue>(key);
                if (value) {
                    result.set(key, *value);
                }
            }
            
            // Overwrite with keys from other config
            for (const auto& key : other.config_->getKeys()) {
                auto value = other.config_->get<ConfigValue>(key);
                if (value) {
                    result.set(key, *value);
                }
            }
        } else if (strategy == "merge") {
            // Merge nested dictionaries
            for (const auto& key : config_->getKeys()) {
                auto value1 = config_->get<ConfigValue>(key);
                auto value2 = other.config_->get<ConfigValue>(key);
                
                if (value1 && value2) {
                    // Try to merge if both are dictionaries (nested configs)
                    if (isConfigValueDict(*value1) && isConfigValueDict(*value2)) {
                        // For now, replace with other config's value
                        result.set(key, *value2);
                    } else {
                        // Use this config's value
                        result.set(key, *value1);
                    }
                } else if (value1) {
                    result.set(key, *value1);
                } else if (value2) {
                    result.set(key, *value2);
                }
            }
        }
        
        return result;
    }
    
    // Registry support
    static void registerConfig(const std::string& name, std::shared_ptr<EnhancedConfig> config) {
        getRegistry().configs_[name] = config;
    }
    
    static std::shared_ptr<EnhancedConfig> getConfig(const std::string& name) {
        auto it = getRegistry().configs_.find(name);
        if (it != getRegistry().configs_.end()) {
            return it->second;
        }
        throw py::key_error("Configuration not found: " + name);
    }
    
    static std::vector<std::string> listConfigs() {
        std::vector<std::string> names;
        for (const auto& pair : getRegistry().configs_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Conversion methods
    std::map<std::string, std::string> toStringMap() const {
        std::map<std::string, std::string> result;
        
        for (const auto& key : config_->getKeys()) {
            auto value = config_->get<ConfigValue>(key);
            if (value) {
                result[key] = configValueToString(*value);
            }
        }
        
        return result;
    }
    
    std::map<std::string, py::object> toPythonMap() const {
        std::map<std::string, py::object> result;
        
        for (const auto& key : config_->getKeys()) {
            auto value = config_->get<ConfigValue>(key);
            if (value) {
                result[key] = configValueToPython(*value);
            }
        }
        
        return result;
    }
    
    // Summary and debugging
    std::string summary() const {
        std::stringstream ss;
        ss << "EnhancedConfig with " << config_->getKeys().size() << " entries\n";
        
        if (!config_->getKeys().empty()) {
            ss << "Keys: ";
            for (size_t i = 0; i < config_->getKeys().size(); ++i) {
                if (i > 0) ss << ", ";
                ss << config_->getKeys()[i];
            }
            ss << "\n";
        }
        
        ss << "Categories: ";
        auto categories = getCategories();
        for (size_t i = 0; i < categories.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << categories[i];
        }
        
        if (history_enabled_) {
            ss << "\nHistory entries: " << history_.size();
        }
        
        ss << "\nValidation: " << (validation_enabled_ ? "enabled" : "disabled");
        ss << "\nCurrent preset: " << (current_preset_.empty() ? "none" : current_preset_);
        
        return ss.str();
    }
    
    // Core Config access
    std::shared_ptr<Config> getCoreConfig() const {
        return config_;
    }
    
    void clear() {
        config_->clear();
        clearHistory();
    }
    
    // Type inference
    std::string inferType(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64";
        if (std::holds_alternative<double>(value)) return "float";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "int_list";
        if (std::holds_alternative<std::vector<double>>(value)) return "float_list";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "string_list";
        return "unknown";
    }
    
    // Default values for common configuration patterns
    void setupMinimalBrain() {
        set("brain.neuronCount", 100);
        set("brain.simulationSpeed", 1.0);
        set("brain.enablePlasticity", true);
        set("brain.enableDevelopment", false);
    }
    
    void setupStandard() {
        set("brain.neuronCount", 1000);
        set("brain.simulationSpeed", 1.0);
        set("brain.enablePlasticity", true);
        set("brain.enableDevelopment", true);
        set("sensory.vision.enabled", true);
        set("sensory.audio.enabled", true);
        set("plasticity.enabled", true);
    }
    
    void setupHighPerformance() {
        set("brain.neuronCount", 5000);
        set("brain.simulationSpeed", 2.0);
        set("sensory.vision.enabled", true);
        set("sensory.audio.enabled", true);
        set("plasticity.enabled", true);
        set("development.enabled", true);
        set("neuromodulation.enabled", true);
    }
    
    void setupResearch() {
        set("brain.neuronCount", 10000);
        set("brain.simulationSpeed", 0.5);
        set("brain.enablePlasticity", true);
        set("brain.enableDevelopment", true);
        set("sensory.vision.enabled", true);
        set("sensory.audio.enabled", true);
        set("plasticity.enabled", true);
        set("development.enabled", true);
        set("neuromodulation.enabled", true);
        set("memory.enabled", true);
    }
    
private:
    // Helper methods
    ConfigValue getRawValue(const std::string& key) const {
        auto value = config_->get<ConfigValue>(key);
        if (value) {
            return *value;
        }
        return ConfigValue{};
    }
    
    ConfigValue getNestedRawValue(const std::string& path) const {
        auto value = config_->getNested(path);
        if (value) {
            return *value;
        }
        return ConfigValue{};
    }
    
    bool isConfigValueDict(const ConfigValue& value) const {
        // Check if value can be interpreted as a dictionary
        return false; // Placeholder
    }
    
    std::string configValueToString(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            std::stringstream ss;
            const auto& vec = std::get<std::vector<int>>(value);
            ss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
            return ss.str();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            std::stringstream ss;
            const auto& vec = std::get<std::vector<double>>(value);
            ss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
            return ss.str();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::stringstream ss;
            const auto& vec = std::get<std::vector<std::string>>(value);
            ss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        return "";
    }
    
    py::object configValueToPython(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return py::cast(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return py::cast(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return py::cast(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return py::cast(std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            return py::cast(std::get<std::string>(value));
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return py::cast(std::get<std::vector<int>>(value));
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return py::cast(std::get<std::vector<double>>(value));
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return py::cast(std::get<std::vector<std::string>>(value));
        }
        return py::none();
    }
    
    std::map<std::string, ConfigValue> toDict() const {
        std::map<std::string, ConfigValue> result;
        
        for (const auto& key : config_->getKeys()) {
            auto value = config_->get<ConfigValue>(key);
            if (value) {
                result[key] = *value;
            }
        }
        
        return result;
    }
    
    // Static registry for configuration management
    struct Registry {
        std::map<std::string, std::shared_ptr<EnhancedConfig>> configs_;
    };
    
    static Registry& getRegistry() {
        static Registry registry;
        return registry;
    }
    
    // Member variables
    std::shared_ptr<Config> config_;
    bool history_enabled_ = true;
    size_t history_max_size_ = 1000;
    std::vector<HistoryEntry> history_;
    
    bool validation_enabled_ = true;
    struct ValidatorData {
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    std::map<std::string, ValidatorData> validators_;
    
    std::shared_ptr<ValidationSchema> schema_;
    
    std::map<std::string, PresetData> presets_;
    std::string current_preset_;
    
    // Template cache
    std::map<std::string, ConfigTemplate> template_cache_;
};

// Validation schema class
class ValidationSchema {
public:
    void addRule(const std::string& key, const std::function<bool(const ConfigValue&)>& validator,
                const std::string& errorMessage = "Validation failed") {
        rules_[key] = {validator, errorMessage};
    }
    
    bool validate(const std::string& key, const ConfigValue& value) const {
        auto it = rules_.find(key);
        if (it == rules_.end()) return true;
        return it->second.validator(value);
    }
    
    std::string getErrorMessage(const std::string& key) const {
        auto it = rules_.find(key);
        if (it != rules_.end()) {
            return it->second.errorMessage;
        }
        return "Validation failed";
    }
    
private:
    struct RuleData {
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    std::map<std::string, RuleData> rules_;
};

// Configuration history entry
struct HistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string key;
    ConfigValue old_value;
    ConfigValue new_value;
    ConfigSource source;
    std::string category;
};

// Preset data structure
struct PresetData {
    std::map<std::string, ConfigValue> config;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
};

// Configuration builder class (fluent API)
class ConfigBuilder {
public:
    ConfigBuilder(std::shared_ptr<EnhancedConfig> config) : config_(config) {}
    
    ConfigBuilder& set(const std::string& key, const ConfigValue& value, 
                      ConfigSource source = ConfigSource::Runtime,
                      const std::string& category = "") {
        operations_.push_back({"set", key, value, source, category});
        config_->set(key, value, source);
        if (!category.empty()) {
            config_->setCategory(key, category);
        }
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, int value,
                      ConfigSource source = ConfigSource::Runtime,
                      const std::string& category = "") {
        ConfigValue val = value;
        return set(key, val, source, category);
    }
    
    ConfigBuilder& set(const std::string& key, double value,
                      ConfigSource source = ConfigSource::Runtime,
                      const std::string& category = "") {
        ConfigValue val = value;
        return set(key, val, source, category);
    }
    
    ConfigBuilder& set(const std::string& key, const std::string& value,
                      ConfigSource source = ConfigSource::Runtime,
                      const std::string& category = "") {
        ConfigValue val = value;
        return set(key, val, source, category);
    }
    
    ConfigBuilder& set(const std::string& key, bool value,
                      ConfigSource source = ConfigSource::Runtime,
                      const std::string& category = "") {
        ConfigValue val = value;
        return set(key, val, source, category);
    }
    
    ConfigBuilder& setNested(const std::string& path, const ConfigValue& value,
                           ConfigSource source = ConfigSource::Runtime) {
        operations_.push_back({"set_nested", path, value, source, ""});
        config_->setNested(path, value, source);
        return *this;
    }
    
    ConfigBuilder& setIfNotExists(const std::string& key, const ConfigValue& value,
                                  ConfigSource source = ConfigSource::Default) {
        if (!config_->has(key)) {
            operations_.push_back({"set_if_not_exists", key, value, source, ""});
            config_->set(key, value, source);
        }
        return *this;
    }
    
    ConfigBuilder& addToList(const std::string& key, const ConfigValue& value,
                           ConfigSource source = ConfigSource::Runtime) {
        operations_.push_back({"add_to_list", key, value, source, ""});
        
        auto current = config_->get<ConfigValue>(key);
        if (current && std::holds_alternative<std::vector<int>>(*current)) {
            auto& vec = std::get<std::vector<int>>(*current);
            if (std::holds_alternative<int>(value)) {
                vec.push_back(std::get<int>(value));
                config_->set(key, vec, source);
            }
        } else if (current && std::holds_alternative<std::vector<double>>(*current)) {
            auto& vec = std::get<std::vector<double>>(*current);
            if (std::holds_alternative<double>(value)) {
                vec.push_back(std::get<double>(value));
                config_->set(key, vec, source);
            }
        } else if (current && std::holds_alternative<std::vector<std::string>>(*current)) {
            auto& vec = std::get<std::vector<std::string>>(*current);
            if (std::holds_alternative<std::string>(value)) {
                vec.push_back(std::get<std::string>(value));
                config_->set(key, vec, source);
            }
        } else {
            // Create new list
            if (std::holds_alternative<int>(value)) {
                std::vector<int> vec = {std::get<int>(value)};
                config_->set(key, vec, source);
            } else if (std::holds_alternative<double>(value)) {
                std::vector<double> vec = {std::get<double>(value)};
                config_->set(key, vec, source);
            } else if (std::holds_alternative<std::string>(value)) {
                std::vector<std::string> vec = {std::get<std::string>(value)};
                config_->set(key, vec, source);
            }
        }
        
        return *this;
    }
    
    ConfigBuilder& validate() {
        config_->enableValidation(true);
        return *this;
    }
    
    ConfigBuilder& withValidator(const std::string& key, 
                                const std::function<bool(const ConfigValue&)>& validator,
                                const std::string& errorMessage = "Validation failed") {
        config_->addValidator(key, validator, errorMessage);
        return *this;
    }
    
    std::shared_ptr<EnhancedConfig> build() {
        return config_;
    }
    
    std::vector<std::map<std::string, std::variant<std::string, ConfigValue, ConfigSource>>>
    getOperations() const {
        return operations_;
    }
    
private:
    std::shared_ptr<EnhancedConfig> config_;
    std::vector<std::map<std::string, std::variant<std::string, ConfigValue, ConfigSource>>> operations_;
};

// Configuration template class
class ConfigTemplate {
public:
    ConfigTemplate(const std::string& name, const std::map<std::string, ConfigValue>& templateConfig)
        : name_(name), templateConfig_(templateConfig) {}
    
    std::shared_ptr<EnhancedConfig> render(const std::map<std::string, std::string>& variables) const {
        auto config = std::make_shared<EnhancedConfig>();
        
        // Substitute variables in template
        for (const auto& pair : templateConfig_) {
            std::string value = configValueToString(pair.second);
            
            // Simple variable substitution
            size_t pos = 0;
            while ((pos = value.find("${", pos)) != std::string::npos) {
                size_t endPos = value.find('}', pos);
                if (endPos != std::string::npos) {
                    std::string varName = value.substr(pos + 2, endPos - pos - 2);
                    auto varIt = variables.find(varName);
                    if (varIt != variables.end()) {
                        value.replace(pos, endPos - pos + 1, varIt->second);
                    }
                    pos = endPos + 1;
                } else {
                    break;
                }
            }
            
            // Parse value based on type
            if (value.empty() || value[0] == '[') {
                // Try to parse as JSON array
                try {
                    py::module json = py::module::import("json");
                    auto jsonValue = json.loads(value);
                    if (jsonValue.is_string()) {
                        config->set(pair.first, jsonValue.get<std::string>());
                    } else if (jsonValue.is_number_integer()) {
                        config->set(pair.first, jsonValue.get<int>());
                    } else if (jsonValue.is_number_float()) {
                        config->set(pair.first, jsonValue.get<double>());
                    } else if (jsonValue.is_boolean()) {
                        config->set(pair.first, jsonValue.get<bool>());
                    } else if (jsonValue.is_array()) {
                        py::list list = jsonValue.get_py();
                        if (list.size() > 0) {
                            std::vector<int> intList;
                            bool allInts = true;
                            for (size_t i = 0; i < list.size(); ++i) {
                                try {
                                    intList.push_back(py::cast<int>(list[i]));
                                } catch (...) {
                                    allInts = false;
                                    break;
                                }
                            }
                            if (allInts) {
                                config->set(pair.first, intList);
                            } else {
                                // Keep as string
                                config->set(pair.first, value);
                            }
                        }
                    }
                } catch (...) {
                    // Fallback to string
                    config->set(pair.first, value);
                }
            } else if (value == "true" || value == "false") {
                config->set(pair.first, value == "true");
            } else if (value.find('.') != std::string::npos) {
                try {
                    double doubleVal = std::stod(value);
                    config->set(pair.first, doubleVal);
                } catch (...) {
                    config->set(pair.first, value);
                }
            } else {
                try {
                    int intVal = std::stoi(value);
                    config->set(pair.first, intVal);
                } catch (...) {
                    config->set(pair.first, value);
                }
            }
        }
        
        return config;
    }
    
    const std::string& getName() const {
        return name_;
    }
    
private:
    std::string name_;
    std::map<std::string, ConfigValue> templateConfig_;
    
    std::string configValueToString(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return "";
    }
};

// Python module initialization
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM Configuration System
        -------------------------------
        Advanced Python bindings for NLM with enhanced configuration management.
        
        Features:
        - Configuration builder pattern (fluent API)
        - Template system with variable substitution
        - Validation and schema support
        - Batch operations for performance
        - Configuration history tracking
        - Export/import utilities
        - Category-based organization
        - Configuration presets for common use cases
        - Comparison and merging utilities
        - Type inference for automatic configuration
        - Comprehensive error handling
    )pbdoc";
    
    // Exception classes
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    
    // Enhanced Config class
    py::class_<EnhancedConfig>(m, "EnhancedConfig", R"pbdoc(
        Enhanced configuration class with advanced features.
        
        Provides comprehensive configuration management including:
        - Validation and schema support
        - Category-based organization  
        - Batch operations
        - Builder pattern
        - Template system
        - History tracking
        - Export/import utilities
        - Presets for common use cases
        - Comparison and merging
    )pbdoc")
        .def(py::init<>())
        .def(py::init<std::shared_ptr<Config>>())
        
        // Core interface (maintaining backward compatibility)
        .def("has", &EnhancedConfig::has, "Check if a configuration key exists")
        .def("get", [](const EnhancedConfig& self, const std::string& key) {
            auto value = self.get<ConfigValue>(key);
            return value ? value.value() : ConfigValue{};
        }, "Get configuration value")
        .def("set", [](EnhancedConfig& self, const std::string& key, const ConfigValue& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set configuration value")
        .def("set", [](EnhancedConfig& self, const std::string& key, int value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
        .def("set", [](EnhancedConfig& self, const std::string& key, double value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
        .def("set", [](EnhancedConfig& self, const std::string& key, const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
        .def("set", [](EnhancedConfig& self, const std::string& key, bool value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime)
        .def("getOr", &EnhancedConfig::getOr, "Get configuration value with default")
        .def("setNested", &EnhancedConfig::setNested, "Set nested configuration value")
        .def("getNested", &EnhancedConfig::getNested, "Get nested configuration value")
        .def("hasNested", &EnhancedConfig::hasNested, "Check if nested configuration exists")
        
        // Batch operations
        .def("batchSet", &EnhancedConfig::batchSet, "Set multiple configuration values in batch")
        .def("batchGet", &EnhancedConfig::batchGet, "Get multiple configuration values in batch")
        
        // Category support
        .def("setCategory", &EnhancedConfig::setCategory, "Set configuration category")
        .def("getCategory", &EnhancedConfig::getCategory, "Get configuration category")
        .def("getCategories", &EnhancedConfig::getCategories, "Get all categories")
        .def("getConfigByCategory", &EnhancedConfig::getConfigByCategory, "Get configuration by category")
        
        // Validation
        .def("enableValidation", &EnhancedConfig::enableValidation, "Enable/disable validation")
        .def("addValidator", &EnhancedConfig::addValidator, "Add validator for a configuration key")
        .def("validateKey", &EnhancedConfig::validateKey, "Validate a specific configuration key")
        .def("validateAll", &EnhancedConfig::validateAll, "Validate all configuration values")
        
        // Presets
        .def("loadPreset", &EnhancedConfig::loadPreset, "Load a configuration preset")
        .def("savePreset", &EnhancedConfig::savePreset, "Save current configuration as a preset")
        .def("listPresets", &EnhancedConfig::listPresets, "List all presets")
        
        // Builder pattern
        .def("createBuilder", &EnhancedConfig::createBuilder, "Create configuration builder")
        
        // Template system
        .def("createTemplate", &EnhancedConfig::createTemplate, "Create configuration template")
        .def("renderTemplate", &EnhancedConfig::renderTemplate, "Render template with variables")
        
        // History tracking
        .def("enableHistory", &EnhancedConfig::enableHistory, "Enable/disable history tracking")
        .def("setHistoryMaxSize", &EnhancedConfig::setHistoryMaxSize, "Set maximum history size")
        .def("getHistory", &EnhancedConfig::getHistory, "Get configuration change history")
        .def("clearHistory", &EnhancedConfig::clearHistory, "Clear configuration history")
        
        // Export/Import
        .def("exportToFile", &EnhancedConfig::exportToFile, "Export configuration to file")
        .def("importFromFile", &EnhancedConfig::importFromFile, "Import configuration from file")
        
        // Comparison and merging
        .def("compare", &EnhancedConfig::compare, "Compare with another configuration")
        .def("merge", &EnhancedConfig::merge, "Merge with another configuration")
        
        // Conversion
        .def("toStringMap", &EnhancedConfig::toStringMap, "Convert configuration to string map")
        .def("toPythonMap", &EnhancedConfig::toPythonMap, "Convert configuration to Python map")
        
        // Convenience presets
        .def("setupMinimalBrain", &EnhancedConfig::setupMinimalBrain, "Setup minimal brain configuration")
        .def("setupStandard", &EnhancedConfig::setupStandard, "Setup standard configuration")
        .def("setupHighPerformance", &EnhancedConfig::setupHighPerformance, "Setup high-performance configuration")
        .def("setupResearch", &EnhancedConfig::setupResearch, "Setup research configuration")
        
        // Summary and debugging
        .def("summary", &EnhancedConfig::summary, "Get configuration summary")
        .def("clear", &EnhancedConfig::clear, "Clear all configuration entries")
        
        // Registry operations (static)
        .def_static("registerConfig", &EnhancedConfig::registerConfig, "Register configuration in global registry")
        .def_static("getConfig", &EnhancedConfig::getConfig, "Get configuration from global registry")
        .def_static("listConfigs", &EnhancedConfig::listConfigs, "List all registered configurations");
    
    // ConfigBuilder class
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(
        Fluent builder pattern for configuration creation.
        
        Provides a chainable API for building complex configurations.
    )pbdoc")
        .def(py::init<std::shared_ptr<EnhancedConfig>>())
        .def("set", [](ConfigBuilder& self, const std::string& key, const ConfigValue& value,
                     ConfigSource source, const std::string& category) {
            return self.set(key, value, source, category);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           py::arg("category") = "", "Set configuration value")
        .def("set", [](ConfigBuilder& self, const std::string& key, int value,
                     ConfigSource source, const std::string& category) {
            return self.set(key, value, source, category);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           py::arg("category") = "")
        .def("set", [](ConfigBuilder& self, const std::string& key, double value,
                     ConfigSource source, const std::string& category) {
            return self.set(key, value, source, category);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           py::arg("category") = "")
        .def("set", [](ConfigBuilder& self, const std::string& key, const std::string& value,
                     ConfigSource source, const std::string& category) {
            return self.set(key, value, source, category);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           py::arg("category") = "")
        .def("set", [](ConfigBuilder& self, const std::string& key, bool value,
                     ConfigSource source, const std::string& category) {
            return self.set(key, value, source, category);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           py::arg("category") = "")
        .def("setNested", &ConfigBuilder::setNested, "Set nested configuration value")
        .def("setIfNotExists", &ConfigBuilder::setIfNotExists, "Set value only if key doesn't exist")
        .def("addToList", &ConfigBuilder::addToList, "Add value to a list configuration")
        .def("validate", &ConfigBuilder::validate, "Enable validation for built configuration")
        .def("withValidator", &ConfigBuilder::withValidator, "Add validator to configuration")
        .def("build", &ConfigBuilder::build, "Build the configuration")
        .def("getOperations", &ConfigBuilder::getOperations, "Get list of operations performed");
    
    // ConfigTemplate class
    py::class_<ConfigTemplate>(m, "ConfigTemplate", R"pbdoc(
        Configuration template system.
        
        Provides template-based configuration management with variable substitution.
    )pbdoc")
        .def(py::init<std::string, std::map<std::string, ConfigValue>>())
        .def("render", &ConfigTemplate::render, "Render template with variables")
        .def("getName", &ConfigTemplate::getName, "Get template name");
    
    // Validation helpers
    m.def("validateIntRange", [](int value, int min, int max) {
        return min <= value && value <= max;
    }, "Validate integer is within range")
    .def("validatePositive", [](double value) {
        return value > 0;
    }, "Validate value is positive")
    .def("validateStringLength", [](const std::string& value, size_t minLen, size_t maxLen) {
        return minLen <= value.length() && value.length() <= maxLen;
    }, "Validate string length is within range")
    .def("createValidationFunction", [](std::vector<std::function<bool(const ConfigValue&)>> validators) {
        return [validators](const ConfigValue& value) {
            for (const auto& validator : validators) {
                if (!validator(value)) {
                    return false;
                }
            }
            return true;
        };
    }, "Create composite validation function");
    
    // Registry helpers
    m.def("registerConfig", [](const std::string& name, std::shared_ptr<EnhancedConfig> config) {
        EnhancedConfig::registerConfig(name, config);
    }, "Register configuration in global registry")
    .def("getConfig", &EnhancedConfig::getConfig, "Get configuration from global registry")
    .def("listConfigs", &EnhancedConfig::listConfigs, "List all registered configurations");
    
    // Template helpers
    m.def("registerTemplate", [](const std::string& name, const std::map<std::string, ConfigValue>& templateConfig) {
        auto templateObj = std::make_shared<ConfigTemplate>(name, templateConfig);
        EnhancedConfig::getRegistry().template_cache_[name] = *templateObj;
    }, "Register configuration template")
    .def("getTemplate", &EnhancedConfig::renderTemplate, "Render template with variables")
    .def("listTemplates", []() {
        std::vector<std::string> names;
        for (const auto& pair : EnhancedConfig::getRegistry().template_cache_) {
            names.push_back(pair.first);
        }
        return names;
    }, "List all templates");
    
    // Comparison helpers
    m.def("diffConfigs", &EnhancedConfig::compare, "Diff two configurations")
    .def("mergeConfigs", [](const EnhancedConfig& config1, const EnhancedConfig& config2, 
                           const std::string& strategy) {
        return config1.merge(config2, strategy);
    }, "Merge two configurations", py::arg("config1"), py::arg("config2"), 
       py::arg("strategy") = "replace")
    .def("exportConfig", &EnhancedConfig::exportToFile, "Export configuration to file")
    .def("importConfig", &EnhancedConfig::importFromFile, "Import configuration from file");
    
    // Type inference
    m.def("inferType", &EnhancedConfig::inferType, "Infer type from configuration value");
    
    // Factory functions
    m.def("createEnhancedConfig", []() {
        return std::make_shared<EnhancedConfig>();
    }, "Create enhanced configuration")
    .def("createEnhancedConfigFromCore", [](std::shared_ptr<Config> coreConfig) {
        return std::make_shared<EnhancedConfig>(coreConfig);
    }, "Create enhanced configuration from core config")
    .def("createConfigBuilder", [](std::shared_ptr<EnhancedConfig> config) {
        return ConfigBuilder(config);
    }, "Create configuration builder")
    .def("createConfigTemplate", &EnhancedConfig::createTemplate, "Create configuration template")
    .def("createConfigFromTemplate", &EnhancedConfig::renderTemplate, "Create configuration from template");
    
    // Error types
    py::register_exception<std::runtime_error>(m, "EnhancedRuntimeError");
    py::register_exception<std::invalid_argument>(m, "EnhancedInvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "EnhancedOutOfRangeError");
    
    // Configuration category enum
    py::enum_<ConfigCategory>(m, "ConfigCategory", R"pbdoc(
        Configuration category enumeration.
        
        Categories for organizing configuration parameters:
        - General: General system settings
        - Brain: Brain simulation parameters
        - Dynamics: Dynamic system parameters
        - Plasticity: Synaptic plasticity parameters
        - Development: Developmental stage parameters
        - Neuromodulation: Neuromodulation parameters
        - Memory: Memory-related parameters
        - Cognition: Cognitive parameters
        - Sensory: Sensory system parameters
        - Motor: Motor system parameters
        - Environment: Environment parameters
        - Custom: User-defined parameters
    )pbdoc")
        .value("General", ConfigCategory::General)
        .value("Brain", ConfigCategory::Brain)
        .value("Dynamics", ConfigCategory::Dynamics)
        .value("Plasticity", ConfigCategory::Plasticity)
        .value("Development", ConfigCategory::Development)
        .value("Neuromodulation", ConfigCategory::Neuromodulation)
        .value("Memory", ConfigCategory::Memory)
        .value("Cognition", ConfigCategory::Cognition)
        .value("Sensory", ConfigCategory::Sensory)
        .value("Motor", ConfigCategory::Motor)
        .value("Environment", ConfigCategory::Environment)
        .value("Custom", ConfigCategory::Custom)
        .export_values();
    
    // Configuration source enum
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(
        Configuration source enumeration.
        
        Sources for configuration values:
        - Default: Default configuration
        - File: Loaded from file
        - CommandLine: Command line argument
        - Runtime: Set at runtime
    )pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();
    
    // Error messages and documentation
    m.def("getValidationErrorMessage", [](const std::string& key) {
        return "Validation failed for key: " + key;
    }, "Get standard validation error message");
    
    // Example configurations
    m.def("getExampleMinimalConfig", []() {
        auto config = std::make_shared<EnhancedConfig>();
        config->setupMinimalBrain();
        return config;
    }, "Get example minimal configuration")
    .def("getExampleStandardConfig", []() {
        auto config = std::make_shared<EnhancedConfig>();
        config->setupStandard();
        return config;
    }, "Get example standard configuration")
    .def("getExampleHighPerformanceConfig", []() {
        auto config = std::make_shared<EnhancedConfig>();
        config->setupHighPerformance();
        return config;
    }, "Get example high-performance configuration")
    .def("getExampleResearchConfig", []() {
        auto config = std::make_shared<EnhancedConfig>();
        config->setupResearch();
        return config;
    }, "Get example research configuration");
    
    // Migration guide helper
    m.def("getMigrationGuide", []() {
        return "";
    }, "Get migration guide for upgrading from simple config system");
}

} // namespace nlm
