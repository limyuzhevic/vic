#include "ConfigValidator.hpp"
#include "Config.hpp"
#include <spdlog/spdlog.h>

namespace nlm {

struct ConfigValidator::Impl {
    json schema;
    std::vector<ConfigValidationError> errors;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

bool ConfigValidator::validate(const Config& config, const json& schema) {
    pImpl->schema = schema;
    pImpl->errors.clear();
    
    bool valid = true;
    
    // Validate required fields
    if (pImpl->schema.contains("required")) {
        for (const auto& field : pImpl->schema["required"]) {
            std::string fieldName = field.get<std::string>();
            if (!config.has(fieldName)) {
                pImpl->errors.emplace_back(fieldName, "Required field is missing", "MISSING_REQUIRED_FIELD", true);
                valid = false;
            }
        }
    }
    
    // Validate field values
    if (pImpl->schema.contains("properties")) {
        for (const auto& [fieldName, fieldSchema] : pImpl->schema["properties"].items()) {
            if (config.has(fieldName)) {
                ConfigValue value = config.get<ConfigValue>(fieldName);
                if (!validateField(fieldName, value, fieldSchema)) {
                    valid = false;
                }
            }
        }
    }
    
    return valid;
}

bool ConfigValidator::validateField(const std::string& key, const ConfigValue& value, const json& fieldSchema) {
    bool valid = true;
    
    if (fieldSchema.contains("type")) {
        std::string expectedType = fieldSchema["type"].get<std::string>();
        bool typeMatches = false;
        
        if (expectedType == "string" && std::holds_alternative<std::string>(value)) {
            typeMatches = true;
        } else if (expectedType == "boolean" && std::holds_alternative<bool>(value)) {
            typeMatches = true;
        } else if (expectedType == "integer" && (std::holds_alternative<int>(value) || 
                                                 std::holds_alternative<int64_t>(value))) {
            typeMatches = true;
        } else if (expectedType == "number" && (std::holds_alternative<int>(value) || 
                                                std::holds_alternative<int64_t>(value) || 
                                                std::holds_alternative<double>(value))) {
            typeMatches = true;
        } else if (expectedType == "array" && std::holds_alternative<std::vector<int>>(value)) {
            typeMatches = true;
        } else if (expectedType == "object") {
            typeMatches = true; // Object type not directly supported
        }
        
        if (!typeMatches) {
            pImpl->errors.emplace_back(key, "Value does not match expected type", 
                                      "TYPE_MISMATCH", false);
            valid = false;
        }
    }
    
    // Check enum values
    if (fieldSchema.contains("enum")) {
        bool valueInEnum = false;
        for (const auto& enumVal : fieldSchema["enum"]) {
            ConfigValue enumConfig = jsonToConfigValue(enumVal);
            if (value == enumConfig) {
                valueInEnum = true;
                break;
            }
        }
        
        if (!valueInEnum) {
            pImpl->errors.emplace_back(key, "Value is not in enum list", 
                                      "ENUM_MISMATCH", false);
            valid = false;
        }
    }
    
    // Check range for numbers
    if (fieldSchema.contains("minimum")) {
        double minVal = fieldSchema["minimum"].get<double>();
        if (!valueInRange(value, minVal, std::numeric_limits<double>::max())) {
            pImpl->errors.emplace_back(key, "Value is below minimum", 
                                      "MINIMUM_VIOLATION", false);
            valid = false;
        }
    }
    
    if (fieldSchema.contains("maximum")) {
        double maxVal = fieldSchema["maximum"].get<double>();
        if (!valueInRange(value, std::numeric_limits<double>::lowest(), maxVal)) {
            pImpl->errors.emplace_back(key, "Value is above maximum", 
                                      "MAXIMUM_VIOLATION", false);
            valid = false;
        }
    }
    
    // Check pattern for strings
    if (fieldSchema.contains("pattern")) {
        std::string pattern = fieldSchema["pattern"].get<std::string>();
        if (!std::holds_alternative<std::string>(value)) {
            pImpl->errors.emplace_back(key, "Pattern validation requires string type", 
                                      "PATTERN_TYPE_MISMATCH", false);
            valid = false;
        } else {
            std::regex regex(pattern, std::regex::ECMAScript);
            if (!std::regex_match(std::get<std::string>(value), regex)) {
                pImpl->errors.emplace_back(key, "Value does not match pattern", 
                                          "PATTERN_MISMATCH", false);
                valid = false;
            }
        }
    }
    
    return valid;
}

const std::vector<ConfigValidationError>& ConfigValidator::getErrors() const {
    return pImpl->errors;
}

void ConfigValidator::setSchema(const json& schema) {
    pImpl->schema = schema;
    pImpl->errors.clear();
}

const json& ConfigValidator::getSchema() const {
    return pImpl->schema;
}

void ConfigValidator::clearErrors() {
    pImpl->errors.clear();
}

bool ConfigValidator::validateSystemConfig(const Config& config, const std::string& systemName) {
    bool valid = true;
    
    if (systemName == "memory") {
        // Memory system validation
        auto capacity = config.get<double>("memory.workingMemoryCapacity");
        if (capacity && *capacity <= 0) {
            pImpl->errors.emplace_back("memory.workingMemoryCapacity", 
                                     "Working memory capacity must be positive", 
                                     "INVALID_CAPACITY", true);
            valid = false;
        }
        
        auto consolidation = config.get<double>("memory.episodicConsolidationRate");
        if (consolidation && (*consolidation < 0.0 || *consolidation > 1.0)) {
            pImpl->errors.emplace_back("memory.episodicConsolidationRate",
                                     "Consolidation rate must be between 0 and 1",
                                     "INVALID_CONSOLIDATION_RATE", false);
            valid = false;
        }
        
        auto decay = config.get<double>("memory.associativeStrengthDecay");
        if (decay && (*decay < 0.0 || *decay > 1.0)) {
            pImpl->errors.emplace_back("memory.associativeStrengthDecay",
                                     "Decay rate must be between 0 and 1",
                                     "INVALID_DECAY_RATE", false);
            valid = false;
        }
        
    } else if (systemName == "neuromodulation") {
        // Neuromodulation system validation
        auto baseline = config.get<double>("neuromodulation.dopamineBaseline");
        if (baseline && (*baseline < 0.0 || *baseline > 1.0)) {
            pImpl->errors.emplace_back("neuromodulation.dopamineBaseline",
                                     "Dopamine baseline must be between 0 and 1",
                                     "INVALID_BASELINE", false);
            valid = false;
        }
        
        auto threshold = config.get<double>("neuromodulation.noveltyThreshold");
        if (threshold && (*threshold < 0.0 || *threshold > 1.0)) {
            pImpl->errors.emplace_back("neuromodulation.noveltyThreshold",
                                     "Novelty threshold must be between 0 and 1",
                                     "INVALID_THRESHOLD", false);
            valid = false;
        }
        
        auto curiosity = config.get<double>("neuromodulation.curiosityLevel");
        if (curiosity && (*curiosity < 0.0 || *curiosity > 1.0)) {
            pImpl->errors.emplace_back("neuromodulation.curiosityLevel",
                                     "Curiosity level must be between 0 and 1",
                                     "INVALID_CURIOUSITY", false);
            valid = false;
        }
        
    } else if (systemName == "cognitive") {
        // Cognitive system validation
        auto planningDepth = config.get<int>("cognitive.planningDepth");
        if (planningDepth && *planningDepth < 1) {
            pImpl->errors.emplace_back("cognitive.planningDepth",
                                     "Planning depth must be at least 1",
                                     "INVALID_PLANNING_DEPTH", false);
            valid = false;
        }
        
        auto attentionDecay = config.get<double>("cognitive.attentionDecayRate");
        if (attentionDecay && (*attentionDecay < 0.0 || *attentionDecay > 1.0)) {
            pImpl->errors.emplace_back("cognitive.attentionDecayRate",
                                     "Attention decay rate must be between 0 and 1",
                                     "INVALID_ATTENTION_DECAY", false);
            valid = false;
        }
    }
    
    return valid;
}

bool ConfigValidator::checkCompatibility(const Config& config1, const Config& config2) {
    std::vector<ConfigDiff> diffs = config1.getDiff(config2);
    
    for (const auto& diff : diffs) {
        if ((std::holds_alternative<std::monostate>(diff.oldValue) && 
             !std::holds_alternative<std::monostate>(diff.newValue)) ||
            (!std::holds_alternative<std::monostate>(diff.oldValue) && 
             std::holds_alternative<std::monostate>(diff.newValue))) {
            return false;
        }
    }
    
    return true;
}

} // namespace nlm
