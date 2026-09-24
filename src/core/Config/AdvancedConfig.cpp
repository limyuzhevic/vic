// Advanced Configuration System Implementation
// This file implements the AdvancedConfig class and related functionality

#include "core/Config/AdvancedConfig.hpp"
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <nlohmann/json.hpp>

namespace nlm {

// ValidationRule implementation
ConfigValidator::ValidationRule::ValidationRule(Type type, const std::string& key, const std::string& errorMsg)
    : type_(type), key_(key), errorMsg_(errorMsg) {}

ConfigValidator::ValidationResult ConfigValidator::ValidationRule::validate(const Config& config) const {
    return ValidationResult{false, {errorMsg_}, {}, {}};
}

// RangeRule implementation
ConfigValidator::RangeRule::RangeRule(const std::string& key, double min, double max, const std::string& errorMsg)
    : ValidationRule(Type::Range, key, errorMsg), min_(min), max_(max) {}

ConfigValidator::ValidationResult ConfigValidator::RangeRule::validate(const Config& config) const {
    ValidationResult result;
    
    if (!config.has(key_)) {
        result.warnings.push_back("Key '" + key_ + "' not found in config");
        return result;
    }
    
    double value = config.getOr<double>(key_, 0.0);
    if (value < min_ || value > max_) {
        result.isValid = false;
        result.errors.push_back(errorMsg_);
    }
    
    return result;
}

// StringLengthRule implementation
ConfigValidator::StringLengthRule::StringLengthRule(const std::string& key, size_t min, size_t max, const std::string& errorMsg)
    : ValidationRule(Type::StringLength, key, errorMsg), min_(min), max_(max) {}

ConfigValidator::ValidationResult ConfigValidator::StringLengthRule::validate(const Config& config) const {
    ValidationResult result;
    
    if (!config.has(key_)) {
        result.warnings.push_back("Key '" + key_ + "' not found in config");
        return result;
    }
    
    std::string value = config.getOr<std::string>(key_, "");
    if (value.length() < min_ || value.length() > max_) {
        result.isValid = false;
        result.errors.push_back(errorMsg_);
    }
    
    return result;
}

// PatternRule implementation
ConfigValidator::PatternRule::PatternRule(const std::string& key, const std::string& pattern, const std::string& errorMsg)
    : ValidationRule(Type::Pattern, key, errorMsg), pattern_(pattern) {}

ConfigValidator::ValidationResult ConfigValidator::PatternRule::validate(const Config& config) const {
    ValidationResult result;
    
    if (!config.has(key_)) {
        result.warnings.push_back("Key '" + key_ + "' not found in config");
        return result;
    }
    
    std::string value = config.getOr<std::string>(key_, "");
    if (!std::regex_match(value, pattern_)) {
        result.isValid = false;
        result.errors.push_back(errorMsg_);
    }
    
    return result;
}

// ValueRule implementation
ConfigValidator::ValueRule::ValueRule(const std::string& key, const std::string& allowedValue, const std::string& errorMsg)
    : ValidationRule(Type::Value, key, errorMsg), allowedValue_(allowedValue) {}

ConfigValidator::ValidationResult ConfigValidator::ValueRule::validate(const Config& config) const {
    ValidationResult result;
    
    if (!config.has(key_)) {
        result.warnings.push_back("Key '" + key_ + "' not found in config");
        return result;
    }
    
    // For numeric values, we need to check string representation
    std::string valueStr = config.getOr<std::string>(key_, "");
    if (valueStr != allowedValue_) {
        result.isValid = false;
        result.errors.push_back(errorMsg_);
    }
    
    return result;
}

// ConfigValidator validation
ConfigValidator::ValidationResult ConfigValidator::validate(const Config& config) const {
    ValidationResult result;
    result.isValid = true;
    
    for (const auto& rule : rules_) {
        ValidationResult ruleResult = rule->validate(config);
        result.errors.insert(result.errors.end(), ruleResult.errors.begin(), ruleResult.errors.end());
        result.warnings.insert(result.warnings.end(), ruleResult.warnings.begin(), ruleResult.warnings.end());
        for (const auto& [key, suggestion] : ruleResult.suggestions) {
            result.suggestions[key] = suggestion;
        }
    }
    
    result.isValid = result.errors.empty();
    return result;
}

// AdvancedConfig implementation
AdvancedConfig::AdvancedConfig() {
    config_ = std::make_shared<Config>();
    updateStats();
}

bool AdvancedConfig::setValue(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        config_->set(key, value, source);
        updateStats();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool AdvancedConfig::setValue(const std::string& key, int64_t value, ConfigSource source) {
    return setValue(key, std::to_string(value), source);
}

bool AdvancedConfig::setValue(const std::string& key, double value, ConfigSource source) {
    return setValue(key, std::to_string(value), source);
}

bool AdvancedConfig::setValue(const std::string& key, float value, ConfigSource source) {
    return setValue(key, std::to_string(value), source);
}

bool AdvancedConfig::setValue(const std::string& key, bool value, ConfigSource source) {
    return setValue(key, value ? "true" : "false", source);
}

std::string AdvancedConfig::getString(const std::string& key, const std::string& defaultValue) const {
    return config_->getOr<std::string>(key, defaultValue);
}

int64_t AdvancedConfig::getInt(const std::string& key, int64_t defaultValue) const {
    return config_->getOr<int64_t>(key, defaultValue);
}

double AdvancedConfig::getDouble(const std::string& key, double defaultValue) const {
    return config_->getOr<double>(key, defaultValue);
}

float AdvancedConfig::getFloat(const std::string& key, float defaultValue) const {
    return config_->getOr<float>(key, defaultValue);
}

bool AdvancedConfig::getBool(const std::string& key, bool defaultValue) const {
    std::string value = getString(key, defaultValue ? "true" : "false");
    return (value == "true" || value == "1" || value == "yes");
}

void AdvancedConfig::addPreset(const Preset& preset) {
    presets_.push_back(preset);
    updatePresetScores();
}

bool AdvancedConfig::removePreset(const std::string& name) {
    auto it = std::find_if(presets_.begin(), presets_.end(),
                          [&name](const Preset& preset) { return preset.name == name; });
    if (it != presets_.end()) {
        presets_.erase(it);
        return true;
    }
    return false;
}

AdvancedConfig::Preset* AdvancedConfig::getPreset(const std::string& name) {
    for (auto& preset : presets_) {
        if (preset.name == name) {
            return &preset;
        }
    }
    return nullptr;
}

std::vector<std::string> AdvancedConfig::getCategoryKeys(const std::string& categoryName) const {
    std::vector<std::string> keys;
    for (const auto& category : categories_) {
        if (category.name == categoryName) {
            keys = category.keys;
            break;
        }
    }
    return keys;
}

AdvancedConfig::ValidationResult AdvancedConfig::validate() const {
    return validator_.validate(*config_);
}

std::map<std::string, std::string> AdvancedConfig::analyzePerformance() const {
    std::map<std::string, std::string> analysis;
    
    // Analyze performance based on configuration
    if (config_->has("neuron_count")) {
        int64_t neuronCount = config_->getOr<int64_t>("neuron_count", 0);
        if (neuronCount < 100) {
            analysis["performance"] = "Low - Good for testing";
        } else if (neuronCount < 1000) {
            analysis["performance"] = "Medium - Balanced for research";
        } else if (neuronCount < 10000) {
            analysis["performance"] = "High - Good for production";
        } else {
            analysis["performance"] = "Very High - Requires significant resources";
        }
    }
    
    if (config_->has("connection_probability")) {
        float connProb = config_->getOr<float>"connection_probability", 0.0f);
        if (connProb < 0.05) {
            analysis["sparsity"] = "Very sparse - Low computational load";
        } else if (connProb < 0.15) {
            analysis["sparsity"] = "Sparse - Moderate computational load";
        } else if (connProb < 0.3) {
            analysis["sparsity"] = "Moderate - High computational load";
        } else {
            analysis["sparsity"] = "Dense - Very high computational load";
        }
    }
    
    return analysis;
}

std::vector<std::string> AdvancedConfig::getSuggestions() const {
    std::vector<std::string> suggestions;
    
    ValidationResult validation = validator_.validate(*config_);
    for (const auto& suggestion : validation.suggestions) {
        suggestions.push_back(suggestion.first + ": " + suggestion.second);
    }
    
    return suggestions;
}

bool AdvancedConfig::exportToFile(const std::string& filepath, const ExportOptions& options) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (options.prettyPrint) {
            // Pretty print JSON
            nlohmann::json jsonData;
            for (const auto& key : config_->getKeys()) {
                if (options.keyFilter.empty() || 
                    std::find(options.keyFilter.begin(), options.keyFilter.end(), key) != options.keyFilter.end()) {
                    jsonData[key] = config_->getOr<std::string>(key, "");
                }
            }
            
            if (options.includeComments) {
                file << "// NLM Configuration\n";
                file << "// Generated on " << std::chrono::system_clock::now() << "\n\n";
            }
            
            file << jsonData.dump(2);
        } else {
            // Compact format
            file << exportToJson(options);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool AdvancedConfig::importFromFile(const std::string& filepath, const ImportOptions& options) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        return importFromJson(content, options);
    } catch (const std::exception&) {
        return false;
    }
}

std::string AdvancedConfig::exportToJson(const ExportOptions& options) const {
    nlohmann::json jsonData;
    
    for (const auto& key : config_->getKeys()) {
        if (options.keyFilter.empty() || 
            std::find(options.keyFilter.begin(), options.keyFilter.end(), key) != options.keyFilter.end()) {
            jsonData[key] = config_->getOr<std::string>(key, "");
        }
    }
    
    return jsonData.dump();
}

bool AdvancedConfig::importFromJson(const std::string& jsonStr, const ImportOptions& options) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
        
        if (options.validateOnImport) {
            ValidationResult validation = validator_.validate(*config_);
            if (!validation.isValid) {
                return false;
            }
        }
        
        bool success = true;
        for (const auto& [key, value] : jsonData.items()) {
            std::string valueStr;
            
            if (value.is_string()) {
                valueStr = value.get<std::string>();
            } else if (value.is_number_integer()) {
                valueStr = std::to_string(value.get<int64_t>());
            } else if (value.is_number_float()) {
                valueStr = std::to_string(value.get<double>());
            } else if (value.is_boolean()) {
                valueStr = value.get<bool>() ? "true" : "false";
            }
            
            if (options.mergeWithExisting) {
                if (config_->has(key)) {
                    // Merge with existing value
                    continue;
                }
            }
            
            success &= setValue(key, valueStr, options.configPath.empty() ? ConfigSource::User : ConfigSource::File);
        }
        
        return success;
    } catch (const std::exception&) {
        return false;
    }
}

void AdvancedConfig::updatePresetScores() {
    for (auto& preset : presets_) {
        double score = 0.0;
        
        // Calculate score based on neuron count
        if (preset.config->has("neuron_count")) {
            int64_t neuronCount = preset.config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount >= 1000 && neuronCount <= 5000) {
                score += 2.0;
            } else if (neuronCount >= 100 && neuronCount <= 1000) {
                score += 1.5;
            } else if (neuronCount < 100) {
                score += 0.5;
            }
        }
        
        // Score for connection probability
        if (preset.config->has("connection_probability")) {
            float connProb = preset.config->getOr<float>("connection_probability", 0.0f);
            if (connProb >= 0.1 && connProb <= 0.2) {
                score += 2.0;
            } else if (connProb >= 0.05 && connProb <= 0.3) {
                score += 1.0;
            }
        }
        
        preset.performanceScore = score;
    }
}

std::vector<AdvancedConfig::Preset> AdvancedConfig::findPresetsByTag(const std::string& tag) const {
    std::vector<Preset> result;
    for (const auto& preset : presets_) {
        if (std::find(preset.tags.begin(), preset.tags.end(), tag) != preset.tags.end()) {
            result.push_back(preset);
        }
    }
    return result;
}

std::vector<AdvancedConfig::Preset> AdvancedConfig::findPresetsByScore(double minScore) const {
    std::vector<Preset> result;
    for (const auto& preset : presets_) {
        if (preset.performanceScore >= minScore) {
            result.push_back(preset);
        }
    }
    return result;
}

std::shared_ptr<Config> AdvancedConfig::createFromTemplate(const std::string& templateName,
                                                           const std::map<std::string, std::string>& variables) {
    for (const auto& template_ : templates_) {
        if (template_.name == templateName) {
            auto result = std::make_shared<Config>(*template_.templateConfig);
            
            // Apply variables
            for (const auto& [varName, varValue] : variables) {
                if (template_.variables.find(varName) != template_.variables.end()) {
                    result->set(template_.variables.at(varName), varValue, ConfigSource::Template);
                }
            }
            
            return result;
        }
    }
    return nullptr;
}

AdvancedConfig::ConfigStats AdvancedConfig::getStats() const {
    updateStats();
    return stats_;
}

std::vector<std::string> AdvancedConfig::getDifferences(const AdvancedConfig& other) const {
    std::vector<std::string> differences;
    
    // Check keys in this config but not in other
    for (const auto& key : config_->getKeys()) {
        if (!other.config_->has(key)) {
            differences.push_back("Missing key in other: " + key);
        } else {
            std::string thisValue = config_->getOr<std::string>(key, "");
            std::string otherValue = other.config_->getOr<std::string>(key, "");
            if (thisValue != otherValue) {
                differences.push_back("Value mismatch for key " + key + ": this='" + thisValue + "', other='" + otherValue + "'");
            }
        }
    }
    
    // Check keys in other but not in this
    for (const auto& key : other.config_->getKeys()) {
        if (!config_->has(key)) {
            differences.push_back("Extra key in other: " + key);
        }
    }
    
    return differences;
}

bool AdvancedConfig::isCompatible(const AdvancedConfig& other) const {
    // Check for basic compatibility
    for (const auto& key : config_->getKeys()) {
        if (other.config_->has(key)) {
            // Both have the key, check if types are compatible
            std::string thisType = getKeyType(key);
            std::string otherType = other.getKeyType(key);
            if (thisType != otherType && !typesAreCompatible(thisType, otherType)) {
                return false;
            }
        }
    }
    
    return true;
}

void AdvancedConfig::updateStats() const {
    stats_.totalKeys = config_->getKeys().size();
    stats_.userKeys = 0;
    stats_.defaultKeys = 0;
    stats_.systemKeys = 0;
    
    for (const auto& key : config_->getKeys()) {
        // This would need to track source information
        // For now, we'll use a simplified approach
        if (key.find("user_") != std::string::npos) {
            stats_.userKeys++;
        } else if (key.find("default_") != std::string::npos) {
            stats_.defaultKeys++;
        } else {
            stats_.systemKeys++;
        }
    }
    
    ValidationResult validation = validator_.validate(*config_);
    stats_.validationScore = validation.isValid ? 1.0 : 0.0;
    stats_.lastModified = std::chrono::system_clock::now();
}

std::string AdvancedConfig::toString(const std::any& value) const {
    if (std::any_cast<std::string>(&value) != nullptr) {
        return std::any_cast<std::string>(value);
    } else if (std::any_cast<int64_t>(&value) != nullptr) {
        return std::to_string(std::any_cast<int64_t>(value));
    } else if (std::any_cast<double>(&value) != nullptr) {
        return std::to_string(std::any_cast<double>(value));
    } else if (std::any_cast<float>(&value) != nullptr) {
        return std::to_string(std::any_cast<float>(value));
    } else if (std::any_cast<bool>(&value) != nullptr) {
        return std::any_cast<bool>(value) ? "true" : "false";
    }
    return "";
}

std::any AdvancedConfig::parseValue(const std::string& value, const std::string& key) const {
    // Try to determine the type based on the key or value format
    if (value == "true" || value == "false") {
        return value == "true";
    }
    
    try {
        // Check if it's an integer
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        // Check if it's a float
        size_t pos;
        double floatVal = std::stod(value, &pos);
        if (pos == value.size()) {
            return floatVal;
        }
    } catch (...) {}
    
    // Default to string
    return value;
}

// Preset implementations
AdvancedConfig::Preset AdvancedPresets::createResearchBrainPreset() {
    Preset preset;
    preset.name = "research_brain";
    preset.description = "Optimized for research and experimentation";
    preset.config = std::make_shared<Config>();
    
    // Research-optimized settings
    preset.config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
    preset.config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
    preset.config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    preset.config->set("connection_probability", 0.08f, ConfigSource::Default);
    preset.config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Enhanced plasticity for learning
    preset.config->set("plasticity_stdp_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_hebbian_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_structural_enable", true, ConfigSource::Default);
    
    // Neuromodulation for curiosity and exploration
    preset.config->set("neuromod_dopamine_scale", 1.5f, ConfigSource::Default);
    preset.config->set("neuromod_curiosity_enable", true, ConfigSource::Default);
    preset.config->set("neuromod_novelty_enable", true, ConfigSource::Default);
    
    // Learning parameters
    preset.config->set("learning_rate", 0.001f, ConfigSource::Default);
    preset.config->set("stdp_ltp_weight", 0.025f, ConfigSource::Default);
    preset.config->set("stdp_ltd_weight", 0.02f, ConfigSource::Default);
    
    preset.tags = {"research", "learning", "exploration"};
    preset.performanceScore = 0.0;
    
    return preset;
}

AdvancedConfig::Preset AdvancedPresets::createProductionBrainPreset() {
    Preset preset;
    preset.name = "production_brain";
    preset.description = "Optimized for production and deployment";
    preset.config = std::make_shared<Config>();
    
    // Production-optimized settings
    preset.config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    preset.config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    preset.config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    preset.config->set("connection_probability", 0.12f, ConfigSource::Default);
    preset.config->set("simulation_timestep", 0.002, ConfigSource::Default);
    
    // Balanced plasticity
    preset.config->set("plasticity_stdp_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_hebbian_enable", false, ConfigSource::Default);
    preset.config->set("plasticity_structural_enable", false, ConfigSource::Default);
    
    // Stable neuromodulation
    preset.config->set("neuromod_dopamine_scale", 1.0f, ConfigSource::Default);
    preset.config->set("neuromod_curiosity_enable", false, ConfigSource::Default);
    preset.config->set("neuromod_novelty_enable", false, ConfigSource::Default);
    
    // Stable learning
    preset.config->set("learning_rate", 0.0005f, ConfigSource::Default);
    preset.config->set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
    preset.config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    
    preset.tags = {"production", "stable", "efficient"};
    preset.performanceScore = 0.0;
    
    return preset;
}

AdvancedConfig::Preset AdvancedPresets::createDevelopmentBrainPreset() {
    Preset preset;
    preset.name = "development_brain";
    preset.description = "Optimized for development and learning";
    preset.config = std::make_shared<Config>();
    
    // Development-optimized settings
    preset.config->set("random_seed", static_cast<int64_t>(0), ConfigSource::Default);  // Non-deterministic
    preset.config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    preset.config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    preset.config->set("connection_probability", 0.06f, ConfigSource::Default);
    preset.config->set("simulation_timestep", 0.0005, ConfigSource::Default);
    
    // Enhanced development plasticity
    preset.config->set("plasticity_stdp_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_hebbian_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_structural_enable", true, ConfigSource::Default);
    
    // Development neuromodulation
    preset.config->set("neuromod_dopamine_scale", 2.0f, ConfigSource::Default);
    preset.config->set("neuromod_curiosity_enable", true, ConfigSource::Default);
    preset.config->set("neuromod_novelty_enable", true, ConfigSource::Default);
    
    // Higher learning rate for development
    preset.config->set("learning_rate", 0.002f, ConfigSource::Default);
    preset.config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
    preset.config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
    
    preset.tags = {"development", "learning", "exploration"};
    preset.performanceScore = 0.0;
    
    return preset;
}

AdvancedConfig::Preset AdvancedPresets::createRealTimeBrainPreset() {
    Preset preset;
    preset.name = "realtime_brain";
    preset.description = "Optimized for real-time applications";
    preset.config = std::make_shared<Config>();
    
    // Real-time optimized settings
    preset.config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
    preset.config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    preset.config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    preset.config->set("connection_probability", 0.04f, ConfigSource::Default);
    preset.config->set("simulation_timestep", 0.01, ConfigSource::Default);  // Larger timestep
    
    // Minimal plasticity for speed
    preset.config->set("plasticity_stdp_enable", false, ConfigSource::Default);
    preset.config->set("plasticity_hebbian_enable", false, ConfigSource::Default);
    preset.config->set("plasticity_structural_enable", false, ConfigSource::Default);
    
    // Minimal neuromodulation
    preset.config->set("neuromod_dopamine_scale", 0.5f, ConfigSource::Default);
    preset.config->set("neuromod_curiosity_enable", false, ConfigSource::Default);
    preset.config->set("neuromod_novelty_enable", false, ConfigSource::Default);
    
    // Fast learning
    preset.config->set("learning_rate", 0.005f, ConfigSource::Default);
    preset.config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    preset.config->set("stdp_ltd_weight", 0.01f, ConfigSource::Default);
    
    preset.tags = {"realtime", "fast", "low_latency"};
    preset.performanceScore = 0.0;
    
    return preset;
}

AdvancedConfig::Preset AdvancedPresets::createMemoryFocusedPreset() {
    Preset preset;
    preset.name = "memory_focused";
    preset.description = "Optimized for memory-intensive tasks";
    preset.config = std::make_shared<Config>();
    
    // Memory-optimized settings
    preset.config->set("random_seed", static_cast<int64_t>(777), ConfigSource::Default);
    preset.config->set("neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
    preset.config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    preset.config->set("connection_probability", 0.15f, ConfigSource::Default);
    preset.config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Enhanced memory systems
    preset.config->set("plasticity_stdp_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_hebbian_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_structural_enable", true, ConfigSource::Default);
    
    // Strong neuromodulation for memory
    preset.config->set("neuromod_dopamine_scale", 1.8f, ConfigSource::Default);
    preset.config->set("neuromod_curiosity_enable", true, ConfigSource::Default);
    preset.config->set("neuromod_novelty_enable", true, ConfigSource::Default);
    
    // Balanced learning
    preset.config->set("learning_rate", 0.001f, ConfigSource::Default);
    preset.config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    preset.config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
    
    preset.tags = {"memory", "storage", "learning"};
    preset.performanceScore = 0.0;
    
    return preset;
}

AdvancedConfig::Preset AdvancedPresets::createLearningFocusedPreset() {
    Preset preset;
    preset.name = "learning_focused";
    preset.description = "Optimized for rapid learning and adaptation";
    preset.config = std::make_shared<Config>();
    
    // Learning-optimized settings
    preset.config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
    preset.config->set("neuron_count", static_cast<int64_t>(2500), ConfigSource::Default);
    preset.config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    preset.config->set("connection_probability", 0.1f, ConfigSource::Default);
    preset.config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Strong plasticity for learning
    preset.config->set("plasticity_stdp_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_hebbian_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_structural_enable", true, ConfigSource::Default);
    
    // Strong neuromodulation for learning
    preset.config->set("neuromod_dopamine_scale", 2.5f, ConfigSource::Default);
    preset.config->set("neuromod_curiosity_enable", true, ConfigSource::Default);
    preset.config->set("neuromod_novelty_enable", true, ConfigSource::Default);
    
    // High learning rate for rapid adaptation
    preset.config->set("learning_rate", 0.003f, ConfigSource::Default);
    preset.config->set("stdp_ltp_weight", 0.035f, ConfigSource::Default);
    preset.config->set("stdp_ltd_weight", 0.03f, ConfigSource::Default);
    
    preset.tags = {"learning", "adaptation", "fast_adaptation"};
    preset.performanceScore = 0.0;
    
    return preset;
}

AdvancedConfig::Preset AdvancedPresets::createVisualizationPreset() {
    Preset preset;
    preset.name = "visualization";
    preset.description = "Optimized for visualization and debugging";
    preset.config = std::make_shared<Config>();
    
    // Visualization-optimized settings
    preset.config->set("random_seed", static_cast<int64_t>(555), ConfigSource::Default);
    preset.config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    preset.config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    preset.config->set("connection_probability", 0.03f, ConfigSource::Default);
    preset.config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Balanced plasticity
    preset.config->set("plasticity_stdp_enable", true, ConfigSource::Default);
    preset.config->set("plasticity_hebbian_enable", false, ConfigSource::Default);
    preset.config->set("plasticity_structural_enable", false, ConfigSource::Default);
    
    // Minimal neuromodulation
    preset.config->set("neuromod_dopamine_scale", 0.8f, ConfigSource::Default);
    preset.config->set("neuromod_curiosity_enable", false, ConfigSource::Default);
    preset.config->set("neuromod_novelty_enable", false, ConfigSource::Default);
    
    // Stable learning
    preset.config->set("learning_rate", 0.0008f, ConfigSource::Default);
    preset.config->set("stdp_ltp_weight", 0.012f, ConfigSource::Default);
    preset.config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    preset.tags = {"visualization", "debugging", "small_scale"};
    preset.performanceScore = 0.0;
    
    return preset;
}

std::vector<AdvancedConfig::Preset> AdvancedPresets::getAllPresets() {
    return {
        createResearchBrainPreset(),
        createProductionBrainPreset(),
        createDevelopmentBrainPreset(),
        createRealTimeBrainPreset(),
        createMemoryFocusedPreset(),
        createLearningFocusedPreset(),
        createVisualizationPreset()
    };
}

AdvancedConfig::ConfigTemplate AdvancedPresets::createCustomTemplate(const std::string& name,
                                                                     const std::string& description,
                                                                     const std::map<std::string, std::string>& variables) {
    ConfigTemplate template_;
    template_.name = name;
    template_.description = description;
    template_.templateConfig = std::make_shared<Config>();
    template_.variables = variables;
    
    // Set default values
    template_.templateConfig->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Template);
    template_.templateConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Template);
    template_.templateConfig->set("connection_probability", 0.1f, ConfigSource::Template);
    template_.templateConfig->set("simulation_timestep", 0.001, ConfigSource::Template);
    
    return template_;
}

// ConfigValidation helper functions
namespace ConfigValidation {
    bool validateNeuronCount(int64_t count) {
        return count > 0 && count <= 100000;
    }
    
    bool validateConnectionProbability(float prob) {
        return prob >= 0.0f && prob <= 1.0f;
    }
    
    bool validateTimestep(double timestep) {
        return timestep > 0.0 && timestep <= 1.0;
    }
    
    bool validateLearningRate(float rate) {
        return rate >= 0.0f && rate <= 1.0f;
    }
    
    bool validatePlasticityParameters(const Config& config) {
        bool stdpEnabled = config.getOr<bool>("plasticity_stdp_enable", false);
        bool hebbianEnabled = config.getOr<bool>("plasticity_hebbian_enable", false);
        bool structuralEnabled = config.getOr<bool>("plasticity_structural_enable", false);
        
        return !(stdpEnabled && hebbianEnabled && structuralEnabled);
    }
    
    bool validateNeuromodulationParameters(const Config& config) {
        float dopamineScale = config.getOr<float>("neuromod_dopamine_scale", 1.0f);
        bool curiosityEnabled = config.getOr<bool>("neuromod_curiosity_enable", false);
        bool noveltyEnabled = config.getOr<bool>("neuromod_novelty_enable", false);
        
        return dopamineScale > 0.0f;
    }
    
    std::string getValidationErrorMessage(const std::string& key, const std::string& error) {
        return "Configuration error for key '" + key + "': " + error;
    }
    
    std::map<std::string, std::string> getRecommendations(const Config& config) {
        std::map<std::string, std::string> recommendations;
        
        // Check for common issues and provide recommendations
        if (config.has("neuron_count")) {
            int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
            if (neuronCount > 10000) {
                recommendations["neuron_count"] = "Consider reducing neuron count for better performance, or increase available memory";
            }
        }
        
        if (config.has("connection_probability")) {
            float connProb = config.getOr<float>("connection_probability", 0.0f);
            if (connProb > 0.2) {
                recommendations["connection_probability"] = "High connection density may impact performance. Consider reducing for better scalability";
            }
        }
        
        if (config.has("simulation_timestep")) {
            double timestep = config.getOr<double>("simulation_timestep", 0.0);
            if (timestep > 0.01) {
                recommendations["simulation_timestep"] = "Large timestep may reduce accuracy. Consider smaller timestep for better precision";
            }
        }
        
        return recommendations;
    }
    
    std::string getPerformanceRecommendation(const Config& config) {
        // Analyze configuration and provide performance recommendations
        std::string recommendation = "Configuration looks good! ";
        
        int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
        if (neuronCount > 10000) {
            recommendation = "Large neuron count detected. Consider using more efficient architecture or reducing neuron count for better performance.";
        } else if (neuronCount > 1000) {
            recommendation = "Medium-sized brain. Performance should be good for most use cases.";
        } else {
            recommendation = "Small brain configuration. Excellent for testing and prototyping.";
        }
        
        return recommendation;
    }
}

// ConfigMerge implementation
namespace ConfigMerge {
    MergeResult mergeConfigs(const std::shared_ptr<Config>& base,
                            const std::shared_ptr<Config>& override,
                            bool overwriteConflicting) {
        MergeResult result;
        result.successful = true;
        
        if (!base) {
            result.successful = false;
            result.conflicts.push_back("Base configuration is null");
            return result;
        }
        
        // Create merged config
        result.mergedConfig = std::make_shared<Config>(*base);
        
        if (!override) {
            return result;
        }
        
        // Merge override values
        for (const auto& key : override->getKeys()) {
            if (result.mergedConfig->has(key)) {
                // Check if values are different
                std::string baseValue = result.mergedConfig->getOr<std::string>(key, "");
                std::string overrideValue = override->getOr<std::string>(key, "");
                
                if (baseValue != overrideValue) {
                    if (overwriteConflicting) {
                        result.mergedConfig->set(key, overrideValue, ConfigSource::User);
                        result.changes.push_back("Updated key: " + key);
                    } else {
                        result.conflicts.push_back("Conflicting value for key: " + key);
                    }
                }
            } else {
                // New key
                result.mergedConfig->set(key, override->getOr<std::string>(key, ""), ConfigSource::User);
                result.changes.push_back("Added key: " + key);
            }
        }
        
        return result;
    }
    
    std::vector<MergeResult> mergeMultipleConfigs(const std::vector<std::shared_ptr<Config>>& configs) {
        std::vector<MergeResult> results;
        
        if (configs.empty()) {
            MergeResult result;
            result.successful = false;
            result.conflicts.push_back("No configurations to merge");
            results.push_back(result);
            return results;
        }
        
        // Merge all configurations
        auto merged = std::make_shared<Config>(*configs[0]);
        std::vector<std::string> allConflicts;
        
        for (size_t i = 1; i < configs.size(); ++i) {
            MergeResult mergeResult = mergeConfigs(merged, configs[i], true);
            results.push_back(mergeResult);
            
            // Update merged config
            merged = mergeResult.mergedConfig;
            allConflicts.insert(allConflicts.end(), mergeResult.conflicts.begin(), mergeResult.conflicts.end());
            allConflicts.insert(allConflicts.end(), mergeResult.changes.begin(), mergeResult.changes.end());
        }
        
        return results;
    }
}

} // namespace nlm