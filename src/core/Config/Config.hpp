#pragma once

#include <string>
#include <unordered_map>
#include <any>
#include <memory>
#include <vector>
#include <functional>
#include <stdexcept>
#include <variant>

namespace nlm {

class ConfigValue;

// Configuration schema validation
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::any& value) const = 0;
    virtual std::string getErrorMessage(const std::any& value) const = 0;
};

// Common validator implementations
class IntegerValidator : public ConfigValidator {
public:
    IntegerValidator(int min = INT_MIN, int max = INT_MAX) : min_(min), max_(max) {}
    bool validate(const std::any& value) const override {
        if (!value.type().is_same<int>()) return false;
        int val = std::any_cast<int>(value);
        return val >= min_ && val <= max_;
    }
    std::string getErrorMessage(const std::any& value) const override {
        if (!value.type().is_same<int>()) return "Value must be an integer";
        int val = std::any_cast<int>(value);
        if (val < min_) return "Value must be >= " + std::to_string(min_);
        if (val > max_) return "Value must be <= " + std::to_string(max_);
        return "Invalid integer value";
    }

private:
    int min_;
    int max_;
};

class FloatValidator : public ConfigValidator {
public:
    FloatValidator(float min = -FLT_MAX, float max = FLT_MAX) : min_(min), max_(max) {}
    bool validate(const std::any& value) const override {
        if (!value.type().is_same<float>()) return false;
        float val = std::any_cast<float>(value);
        return val >= min_ && val <= max_;
    }
    std::string getErrorMessage(const std::any& value) const override {
        if (!value.type().is_same<float>()) return "Value must be a float";
        float val = std::any_cast<float>(value);
        if (val < min_) return "Value must be >= " + std::to_string(min_);
        if (val > max_) return "Value must be <= " + std::to_string(max_);
        return "Invalid float value";
    }

private:
    float min_;
    float max_;
};

class StringValidator : public ConfigValidator {
public:
    StringValidator(size_t minLen = 0, size_t maxLen = SIZE_MAX, 
                    const std::string& pattern = "") : minLen_(minLen), maxLen_(maxLen), pattern_(pattern) {}
    bool validate(const std::any& value) const override {
        if (!value.type().is_same<std::string>()) return false;
        std::string val = std::any_cast<std::string>(value);
        if (val.length() < minLen_ || val.length() > maxLen_) return false;
        if (!pattern_.empty()) {
            // Simple pattern check - for now just check if it's a valid string
            // Could be enhanced with regex
        }
        return true;
    }
    std::string getErrorMessage(const std::any& value) const override {
        if (!value.type().is_same<std::string>()) return "Value must be a string";
        std::string val = std::any_cast<std::string>(value);
        if (val.length() < minLen_) return "String too short (min " + std::to_string(minLen_) + ")";
        if (val.length() > maxLen_) return "String too long (max " + std::to_string(maxLen_) + ")";
        return "Invalid string value";
    }

private:
    size_t minLen_;
    size_t maxLen_;
    std::string pattern_;
};

class BoolValidator : public ConfigValidator {
public:
    bool validate(const std::any& value) const override {
        return value.type().is_same<bool>();
    }
    std::string getErrorMessage(const std::any& value) const override {
        return "Value must be a boolean";
    }
};

// Configuration entry with schema
struct ConfigEntry {
    std::string key;
    std::any value;
    std::string description;
    std::shared_ptr<ConfigValidator> validator;
    bool required;
    std::string category;
    std::string subcategory;
    
    ConfigEntry() : description(""), required(false), category(""), subcategory("") {}
    
    ConfigEntry(const std::string& k, const std::any& v, const std::string& desc = "")
        : key(k), value(v), description(desc), required(false), category(""), subcategory("") {}
    
    bool isValid() const {
        if (validator && value.has_value()) {
            return validator->validate(value);
        }
        return true;
    }
    
    std::string getValidationError() const {
        if (validator && value.has_value()) {
            return validator->getErrorMessage(value);
        }
        return "";
    }
};

// Nested configuration section
class ConfigSection {
public:
    ConfigSection(const std::string& name, const std::string& description = "")
        : name_(name), description_(description) {}
    
    void set(const std::string& key, const std::any& value, const std::string& desc = "", 
             std::shared_ptr<ConfigValidator> validator = nullptr, bool required = false) {
        entries_[key] = ConfigEntry(key, value, desc);
        entries_[key].validator = validator;
        entries_[key].required = required;
        entries_[key].category = name_;
    }
    
    template<typename T>
    void set(const std::string& key, const T& value, const std::string& desc = "",
             std::shared_ptr<ConfigValidator> validator = nullptr, bool required = false) {
        set(key, std::any(value), desc, validator, required);
    }
    
    template<typename T>
    T get(const std::string& key) const {
        auto it = entries_.find(key);
        if (it == entries_.end()) {
            throw std::runtime_error("Configuration key not found: " + key);
        }
        try {
            return std::any_cast<T>(it->second.value);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Configuration key has wrong type: " + key);
        }
    }
    
    bool has(const std::string& key) const {
        return entries_.find(key) != entries_.end();
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : entries_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const std::vector<ConfigEntry>& getEntries() const { return entries_; }
    
    void validateAll() const {
        for (const auto& pair : entries_) {
            const ConfigEntry& entry = pair.second;
            if (entry.required && !hasValue(pair.first)) {
                throw std::runtime_error("Required configuration key missing: " + entry.key);
            }
            if (!entry.isValid()) {
                throw std::runtime_error("Invalid configuration value for key '" + entry.key + 
                    "': " + entry.getValidationError());
            }
        }
    }
    
    bool hasValue(const std::string& key) const {
        auto it = entries_.find(key);
        if (it == entries_.end()) return false;
        return it->second.value.has_value();
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<ConfigEntry> entries_;
};

// Main configuration class with nested structure
class Config {
public:
    Config() = default;
    
    // Add configuration section
    ConfigSection& addSection(const std::string& name, const std::string& description = "") {
        sections_[name] = std::make_unique<ConfigSection>(name, description);
        return *sections_[name];
    }
    
    ConfigSection* getSection(const std::string& name) {
        auto it = sections_.find(name);
        if (it == sections_.end()) return nullptr;
        return it->second.get();
    }
    
    const ConfigSection* getSection(const std::string& name) const {
        auto it = sections_.find(name);
        if (it == sections_.end()) return nullptr;
        return it->second.get();
    }
    
    // Get all section names
    std::vector<std::string> getSectionNames() const {
        std::vector<std::string> names;
        for (const auto& pair : sections_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Validate all sections
    void validateAll() const {
        for (const auto& pair : sections_) {
            pair.second->validateAll();
        }
    }
    
    // Get configuration summary
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration with " << sections_.size() << " sections:\n";
        
        for (const auto& pair : sections_) {
            oss << pair.first << ":\n";
            for (const auto& entry : pair.second->getEntries()) {
                oss << "  " << entry.key;
                if (entry.value.has_value()) {
                    oss << " = [";
                    if (entry.value.type() == typeid(int)) {
                        oss << std::any_cast<int>(entry.value);
                    } else if (entry.value.type() == typeid(float)) {
                        oss << std::any_cast<float>(entry.value);
                    } else if (entry.value.type() == typeid(std::string)) {
                        oss << "\"" << std::any_cast<std::string>(entry.value) << "\"";
                    } else if (entry.value.type() == typeid(bool)) {
                        oss << (std::any_cast<bool>(entry.value) ? "true" : "false");
                    }
                    oss << "]";
                } else {
                    oss << " = [NOT SET]";
                }
                oss << (entry.required ? " (REQUIRED)" : "") << "\n";
            }
        }
        
        return oss.str();
    }
    
    // Template-based getters/setters for backward compatibility
    template<typename T>
    bool has(const std::string& key) const {
        for (const auto& pair : sections_) {
            if (pair.second->has(key)) return true;
        }
        return false;
    }
    
    template<typename T>
    T get(const std::string& key) const {
        for (const auto& pair : sections_) {
            if (pair.second->has(key)) {
                return pair.second->get<T>(key);
            }
        }
        throw std::runtime_error("Configuration key not found: " + key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        try {
            return get<T>(key);
        } catch (const std::runtime_error&) {
            return defaultValue;
        }
    }
    
    template<typename T>
    void set(const std::string& key, const T& value) {
        for (auto& pair : sections_) {
            if (pair.second->has(key)) {
                // Update existing entry
                for (auto& entry : pair.second->getEntries()) {
                    if (entry.key == key) {
                        entry.value = value;
                        break;
                    }
                }
                return;
            }
        }
        // Create new entry in first section that doesn't have it
        for (auto& pair : sections_) {
            if (!pair.second->has(key)) {
                pair.second->set<T>(key, value);
                return;
            }
        }
        // If all sections have the key, add to first section
        if (!sections_.empty()) {
            sections_.begin()->second->set<T>(key, value);
        }
    }
    
    // Delete configuration key
    void remove(const std::string& key) {
        for (auto& pair : sections_) {
            auto& entries = pair.second->getEntries();
            entries.erase(std::remove_if(entries.begin(), entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
                entries.end());
        }
    }
    
    // Clear all configuration
    void clear() {
        sections_.clear();
    }
    
    // Create default configuration with all systems
    static Config createDefaultConfig() {
        Config config;
        
        // Memory system configuration
        auto& memorySection = config.addSection("memory", "Memory system configuration");
        memorySection.set<size_t>("capacity", 1000, "Maximum number of memory traces");
        memorySection.set<float>("decay_rate", 0.01f, "Decay rate for memory traces");
        memorySection.set<size_t>("working_memory_size", 100, "Size of working memory");
        memorySection.set<size_t>("episodic_memory_capacity", 10000, "Maximum episodic episodes");
        memorySection.set<float>("consolidation_threshold", 0.7f, "Threshold for memory consolidation");
        
        // Neuromodulation system configuration
        auto& neuromodulationSection = config.addSection("neuromodulation", "Neuromodulation system configuration");
        neuromodulationSection.set<float>("dopamine_base", 0.1f, "Base dopamine level");
        neuromodulationSection.set<float>("curiosity_base", 0.1f, "Base curiosity level");
        neuromodulationSection.set<float>("novelty_sensitivity", 0.5f, "Sensitivity to novel stimuli");
        neuromodulationSection.set<float>("reward_prediction_error_weight", 1.0f, "Weight of reward prediction error");
        neuromodulationSection.set<bool>("reward_modulation_enabled", true, "Enable reward-based neuromodulation");
        neuromodulationSection.set<bool>("structural_plasticity_enabled", true, "Enable structural plasticity");
        
        // Prediction system configuration
        auto& predictionSection = config.addSection("prediction", "Prediction system configuration");
        predictionSection.set<size_t>("sequence_memory_size", 1000, "Size of temporal sequence memory");
        predictionSection.set<size_t>("prediction_horizon", 10, "Number of steps to predict ahead");
        predictionSection.set<float>("prediction_confidence_threshold", 0.8f, "Threshold for prediction confidence");
        predictionSection.set<bool>("temporal_prediction_enabled", true, "Enable temporal prediction");
        predictionSection.set<bool>("action_consequence_enabled", true, "Enable action consequence prediction");
        
        // Plasticity configuration
        auto& plasticitySection = config.addSection("plasticity", "Plasticity system configuration");
        plasticitySection.set<float>("stdp_ltp_weight", 0.01f, "STDP LTP weight");
        plasticitySection.set<float>("stdp_ltd_weight", 0.012f, "STDP LTD weight");
        plasticitySection.set<float>("stdp_tau", 20.0f, "STDP time constant");
        plasticitySection.set<float>("hebbian_learning_rate", 0.001f, "Hebbian learning rate");
        plasticitySection.set<float>("synaptogenesis_rate", 0.0001f, "Rate of new synapse formation");
        plasticitySection.set<float>("pruning_rate", 0.00001f, "Rate of synapse pruning");
        
        // Development configuration
        auto& developmentSection = config.addSection("development", "Development system configuration");
        developmentSection.set<float>("initial_plasticity", 1.0f, "Initial plasticity level");
        developmentSection.set<float>("critical_period_plasticity", 0.8f, "Plasticity during critical period");
        developmentSection.set<float>("maturation_plasticity", 0.5f, "Plasticity during maturation");
        developmentSection.set<bool>("development_enabled", true, "Enable developmental processes");
        
        // Performance configuration
        auto& performanceSection = config.addSection("performance", "Performance and optimization");
        performanceSection.set<size_t>("checkpoint_interval", 1000, "Steps between checkpoints");
        performanceSection.set<size_t>("max_memory_usage", 1000000000, "Maximum memory usage in bytes");
        performanceSection.set<float>("simulation_speed", 1.0f, "Simulation speed multiplier");
        performanceSection.set<size_t>("max_neurons", 10000, "Maximum number of neurons");
        performanceSection.set<size_t>("max_synapses", 100000, "Maximum number of synapses");
        
        // Cognitive system configuration
        auto& cognitionSection = config.addSection("cognition", "Cognitive system configuration");
        cognitionSection.set<size_t>("planning_depth", 5, "Planning horizon depth");
        cognitionSection.set<float>("attention_inhibition", 0.5f, "Attention inhibition strength");
        cognitionSection.set<float>("attention_excitation", 1.5f, "Attention excitation strength");
        cognitionSection.set<float>("concept_formulation_threshold", 0.8f, "Threshold for concept formation");
        
        // Random seed
        auto& systemSection = config.addSection("system", "System-level configuration");
        systemSection.set<uint64_t>("random_seed", 42, "Random seed for reproducibility");
        systemSection.set<double>("simulation_timestep", 0.001, "Simulation time step duration");
        systemSection.set<size_t>("max_steps", 0, "Maximum simulation steps (0 = unlimited)");
        systemSection.set<std::string>("checkpoint_dir", "./checkpoints", "Directory for checkpoints");
        
        return config;
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<ConfigSection>> sections_;
};

} // namespace nlm
